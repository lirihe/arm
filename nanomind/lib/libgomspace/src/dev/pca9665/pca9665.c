/**
 * Device driver primitives enabling communication
 * with the NXP PCA9665 parallel I2C controller.
 *
 * @author Johan De Claville Christiansen
 * Copyright 2009-2012 GomSpace ApS. All rights reserved.
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <util/error.h>
#include <util/hexdump.h>
#include <util/csp_buffer.h>
#include <util/log.h>
#include <util/driver_debug.h>
#include <util/delay.h>
#include <dev/i2c.h>
#include <dev/cpu.h>
#include <command/command.h>

#include "../../dev/pca9665/pca9665.h"

/** I2C master mode lock */
xSemaphoreHandle i2c_lock = NULL;

/** Registers */
#define I2CSTA							0
#define INDPTR							0
#define I2CDAT							1
#define INDIRECT						2
#define I2CCON							3
#define I2CCOUNT						(4+0)
#define I2CADR							(4+1)
#define I2CSCLL							(4+2)
#define I2CSCLH							(4+3)
#define I2CTO							(4+4)
#define I2CPRESET						(4+5)
#define I2CMODE							(4+6)

/** Control register bits */
#define CON_AA 							0x80
#define CON_ENSIO 						0x40
#define CON_STA 						0x20
#define CON_STO 						0x10
#define CON_SI							0x08
#define CON_MODE 						0x01

/** Status register bits */
#define STA_IDLE						0xF8
#define STA_M_START_SENDT				0x08
#define STA_M_REPEATED_START_SENDT		0x10
#define STA_M_SLAW_SENDT_ACKED			0x18
#define STA_M_SLAW_SENDT_NACKED			0x20
#define STA_M_DATA_SENDT_ACKED			0x28
#define STA_M_DATA_SENDT_LAST_NACKED	0x30
#define STA_M_ARBITRATION_LOST			0x38

#define STA_M_SLAR_SENT_ACKED 			0x40
#define STA_M_SLAR_SENT_NACKED 			0x48
#define STA_M_DATA_RECEIVED_ACKED		0x50
#define STA_M_DATA_RECEIVED_NACKED		0x58

#define STA_S_SLAW_RECEIVED_ACKED		0x60
#define STA_S_ARB_LOST_SLAW_RECEIVED	0x68
#define STA_S_GC_RECEIVED				0xD0
#define STA_S_ARB_LOST_GC_RECEIVED		0xD8
#define STA_S_DATA_RECEIVED_SLA_ACKED	0x80
#define STA_S_DATA_RECEIVED_SLA_NACKED	0x88
#define STA_S_DATA_RECEIVED_GC_ACKED	0xE0
#define STA_S_DATA_RECEIVED_GC_NACKED	0xE8
#define STA_S_STOP_REP_RECEIVED			0xA0

#define PCA9665_MAX_BUF					68

/**
 * Write a value to a register
 *
 * @param handler Device handle
 * @param reg Register address (indirect registers are addressed by their indirect address plus 4)
 * @param val 8 bit value
 * @return Error code as per util/error.h
 */
static inline void pca9665_write_reg(int handler, uint8_t reg, uint8_t val) {

	//driver_debug(DEBUG_I2C, "Write reg %x, %x\r\n", reg, val);

	if (reg < 0x04) {
		*(uint8_t *) (device[handler].base + reg) = val;
	} else {
		*(uint8_t *) (device[handler].base + INDPTR) = reg - 4;
		*(uint8_t *) (device[handler].base + INDIRECT) = val;
	}

}

/**
 * Send a soft reset sepcial command
 * @param handler Device handle
 */
static inline void pca9665_soft_reset(int handler) {

	driver_debug(DEBUG_I2C, "RESET\r\n");

	*(uint8_t *) (device[handler].base + INDPTR) = I2CPRESET - 4;
	*(uint8_t *) (device[handler].base + INDIRECT) = 0xA5;
	*(uint8_t *) (device[handler].base + INDIRECT) = 0x5A;

}

/**
 * Read a value from a register
 *
 * @param handler Device handle
 * @param reg Register address (indirect registers are addressed by their indirect address plus 4
 * @return 8 bit value
 */
static inline uint8_t pca9665_read_reg(int handler, uint8_t reg) {

	//driver_debug(DEBUG_I2C, "Read reg %x at %p\r\n", reg, device[handler].base + reg);

	if (reg < 0x04)
		return *(uint8_t *) (device[handler].base + reg);
	else {
		*(uint8_t *) (device[handler].base + INDPTR) = reg - 4;
		return *(uint8_t *) (device[handler].base + INDIRECT);
	}

}

/**
 * Write a block of data directly to data register
 *
 * @param handler Device handle
 * @param src Source data pointer
 * @param len number of bytes to write
 * @param offset number of bytes to offset the write
 */
static inline void pca9665_write_data(int handler, uint8_t* src, int len) {

	while (len--)
		*(uint8_t *) (device[handler].base + I2CDAT) = *src++;

}

/**
 * Read a block of data directly from data register
 *
 * @param handler Device handle
 * @param src Source data pointer
 * @param len number of bytes to read
 */
static inline void pca9665_read_data(int handler, uint8_t* dst, int len) {

	while (len--)
		*dst++ = *(uint8_t *) (device[handler].base + I2CDAT);

}

static inline void pca9665_read_data_to_buffer(int handle) {

	if (device[handle].rx.frame == NULL)
		return;

	/* Check number of bytes */
	int count = pca9665_read_reg(handle, I2CCOUNT) & 0x7f;
	if (count == 0)
		return;

	/* Check buffer allocation */
	if (count + device[handle].rx.next_byte > I2C_MTU)
		return;

	/* Copy data and increment next_byte counter */
	pca9665_read_data(handle, &device[handle].rx.frame->data[device[handle].rx.next_byte], count);
	device[handle].rx.next_byte += count;
	driver_debug(DEBUG_I2C, "RX: count %u, next_byte %u\r\n", count, device[handle].rx.next_byte);

}

/**
 * Initialises the remote registers in the PCA9665
 * This function is called both from first init, and
 * when an error occurs inside the ISR.
 * @param handle integer handle (starting from 0)
 */
static int pca9665_init_registers(int handle) {

	/* Soft reset the device */
	pca9665_soft_reset(handle);

	int retry = 10;
	while (retry--) {
		if ((pca9665_read_reg(handle, I2CCON) & 0x40) == 0)
			break;
		pca9665_soft_reset(handle);
		delay_us(1000);
	}

	/* Setup: I2C Addr */
	pca9665_write_reg(handle, I2CADR, device[handle].slave_addr << 1);
	if (pca9665_read_reg(handle, I2CADR) != (device[handle].slave_addr << 1))
		return -1;

	/* Setup: I2C mode */
	pca9665_write_reg(handle, I2CMODE, 0x01);

	/* Setup: I2C Timeout */
	pca9665_write_reg(handle, I2CTO, 0xFF);

	/* Setup: Speed
	 *                               1
	 * f SCL = -----------------------------------------
	 *          Tosc (I2CSCLL + I2CSCLH) + Tr + Tf + Td
	 * */
	double Td = 300e-9;		// Tdelay in s
	double Tr = 300e-9;		// Estimated Trise in s
	double Tf = 300e-9;		// Estimated Tfall in s
	double Tosc = 28e-9;	// Tosc in s
	double period = 1 / (Tosc * device[handle].speed * 1000) - (Tr + Tf + Td) / Tosc;

	driver_debug(DEBUG_I2C, "Period is %f\r\n", period);

	pca9665_write_reg(handle, I2CSCLL, (uint8_t) (period * 0.66));
	pca9665_write_reg(handle, I2CSCLH, (uint8_t) (period * 0.33));

	return 0;

}

/**
 * Initialise PCA9665 device
 * @param handle integer handle (starting from 0)
 * @param base_addr hardware base address in memory
 * @param addr I2C address of device (not bit-shifted)
 * @param speed Speed in decimal kbit/sec.
 * @return error code
 */
static int pca9665_init(int handle, uint8_t * base_addr, uint8_t addr, uint16_t speed) {

	/* Remember chip memory address, and I2C node address */
	device[handle].base = base_addr;
	device[handle].slave_addr = addr;
	device[handle].speed = speed;

	/* Call HW init, sets up registers */
	if (pca9665_init_registers(handle) < 0)
		return E_GARBLED_BUFFER;

	/* Enable */
	pca9665_write_reg(handle, I2CCON, CON_ENSIO | CON_MODE | CON_AA);

	return E_NO_ERR;
}

/**
 * Pick up a transmission from scratch or continue an on-going transmission
 * Context: ISR ONLY
 * @param handle Handle to device
 */
static void pca9665_try_tx_from_isr(int handle, portBASE_TYPE * pxTaskWoken) {

	uint8_t flags = 0;

	if (device[handle].is_initialised == 0)
		return;

	if (uxQueueMessagesWaitingFromISR(device[handle].tx.queue) > 0 || device[handle].tx.frame != NULL) {
		device[handle].is_busy = 1;
		flags |= CON_STA;
	} else {
		device[handle].is_busy = 0;
		flags |= CON_STO;
	}

	/* Send the start/stop/restart condition */
	device[handle].mode = DEVICE_MODE_M_T;
	pca9665_write_reg(handle, I2CCON, CON_ENSIO | CON_MODE | CON_AA | flags);

}

/**
 * Interrupt service routine
 * Basically handles the entire protocol
 */
void __attribute__((noinline)) pca9665_dsr(portBASE_TYPE * task_woken) {

	static int handle, len;
	static uint8_t state;
	static uint8_t dest;

	/* Loop through number of devices */
	for (handle = 0; handle < pca9665_device_count; handle++) {

		/* Check for interrupt flag in device status register */
		if (!(pca9665_read_reg(handle, I2CCON) & CON_SI))
			continue;

		/* We have an interrupt, read the status register */
		state = pca9665_read_reg(handle, I2CSTA);

		/* The I2C driver is one _big_ state-machine */
		driver_debug(DEBUG_I2C, "I2C ISR %u %x\n\r", handle, state);
		switch (state) {

		/**
		 * MASTER IRQ's
		 */

		/* START: is the first ISR that appears for outgoing frames */
		case STA_M_REPEATED_START_SENDT:
		case STA_M_START_SENDT:

			/* Mark as busy, so start flag is not sent from task context while transmission is active */
			device[handle].is_busy = 1;

			/* If this is the beginning of a new frame, dequeue */
			if (device[handle].tx.frame == NULL && device[handle].rx.frame == NULL) {

				/* Try do dequeue element, if it fails, stop transmission */
				xQueueReceiveFromISR(device[handle].tx.queue, &device[handle].tx.frame, task_woken);
				if (device[handle].tx.frame == NULL) {
					pca9665_try_tx_from_isr(handle, task_woken);
					break;
				}

				/* If TX len > 0, go for master transmit */
				if (device[handle].tx.frame->len) {
					device[handle].mode = DEVICE_MODE_M_T;
					device[handle].tx.next_byte = 0;

				/* If TX len == 0 and RX len > 0, go for master receive */
				} else if (device[handle].tx.frame->len_rx) {
					device[handle].mode = DEVICE_MODE_M_R;
					device[handle].rx.frame = device[handle].tx.frame;
					device[handle].tx.frame = NULL;
					device[handle].rx.frame->len = device[handle].rx.frame->len_rx;
					device[handle].rx.next_byte = 0;

				/* Well, this should not happen */
				} else {
					csp_buffer_free_isr(device[handle].tx.frame);
					device[handle].tx.frame = NULL;
					pca9665_try_tx_from_isr(handle, task_woken);
					break;
				}

			}

			/* If mode is master receiver then set the read-bit in the address field */
			if (device[handle].mode == DEVICE_MODE_M_R) {

				dest = (device[handle].rx.frame->dest << 1) | 0x01;
				device[handle].rx.next_byte = 0;

				/* Do first part of frame here */
				if (device[handle].rx.frame->len > PCA9665_MAX_BUF) {
					pca9665_write_reg(handle, I2CCOUNT, PCA9665_MAX_BUF);
				} else {
					pca9665_write_reg(handle, I2CCOUNT, device[handle].rx.frame->len | 0x80);
				}

				pca9665_write_data(handle, &dest, 1);

			} else {

				dest = device[handle].tx.frame->dest << 1;
				device[handle].tx.next_byte = 0;

				/* Do first part of frame here */
				if (device[handle].tx.frame->len + 1 > PCA9665_MAX_BUF) {
					pca9665_write_reg(handle, I2CCOUNT, PCA9665_MAX_BUF);
					pca9665_write_data(handle, &dest, 1);
					pca9665_write_data(handle, &device[handle].tx.frame->data[device[handle].tx.next_byte],	PCA9665_MAX_BUF - 1);
					device[handle].tx.next_byte += PCA9665_MAX_BUF - 1;
				} else {
					pca9665_write_reg(handle, I2CCOUNT, device[handle].tx.frame->len + 1);
					pca9665_write_data(handle, &dest, 1);
					pca9665_write_data(handle, &device[handle].tx.frame->data[device[handle].tx.next_byte],	device[handle].tx.frame->len);
					device[handle].tx.next_byte += device[handle].tx.frame->len;
				}
			}

			/* Let the hardware continue */
			pca9665_write_reg(handle, I2CCON, CON_ENSIO | CON_MODE | CON_AA);
			break;

		/* WRITE ACK: A node is ready to be written to */
		case STA_M_SLAW_SENDT_ACKED:
		case STA_M_DATA_SENDT_ACKED:

			/* Safety first */
			if (device[handle].tx.frame == NULL)
				goto isr_error;

			/* Calculate remaining length */
			len = device[handle].tx.frame->len - device[handle].tx.next_byte;

			/* Transmit next chunk */
			if (len > 0) {

				if (len > PCA9665_MAX_BUF) {
					pca9665_write_reg(handle, I2CCOUNT, PCA9665_MAX_BUF);
					pca9665_write_data(handle, &device[handle].tx.frame->data[device[handle].tx.next_byte],	PCA9665_MAX_BUF);
					device[handle].tx.next_byte += PCA9665_MAX_BUF;
				} else {
					pca9665_write_reg(handle, I2CCOUNT, len);
					pca9665_write_data(handle, &device[handle].tx.frame->data[device[handle].tx.next_byte],	len);
					device[handle].tx.next_byte += len;
				}

				pca9665_write_reg(handle, I2CCON, CON_ENSIO | CON_MODE | CON_AA);
				break;

			/* Or, Change from master transmit, to master read if wanted */
			} else if (device[handle].tx.frame->len_rx) {

				device[handle].mode = DEVICE_MODE_M_R;
				device[handle].rx.frame = device[handle].tx.frame;
				device[handle].tx.frame = NULL;
				device[handle].rx.frame->len = device[handle].rx.frame->len_rx;

				/* We need to send a repeated start now! */
				pca9665_write_reg(handle, I2CCON, CON_ENSIO | CON_MODE | CON_AA | CON_STA);
				break;

			/* Or, We are done */
			} else {
				csp_buffer_free_isr(device[handle].tx.frame);
				device[handle].tx.frame = NULL;
				pca9665_try_tx_from_isr(handle, task_woken);
			}

			break;

		/* WRITE ERROR: A write has failed */
		case STA_M_SLAW_SENDT_NACKED:
		case STA_M_DATA_SENDT_LAST_NACKED:

			if (device[handle].tx.frame != NULL) {
				driver_debug(DEBUG_I2C, "I2C SLA+W NACK: 0x%02"PRIx8"\n\r", device[handle].tx.frame->dest);
				csp_buffer_free_isr(device[handle].tx.frame);
				device[handle].tx.frame = NULL;
			}

			pca9665_try_tx_from_isr(handle, task_woken);
			break;

		/* ARBITRATION LOST: Start condition failed */
		case STA_M_ARBITRATION_LOST:

			/* Restart transmission by resetting next_byte and preserving tx_frame pointer */
			device[handle].tx.next_byte = 0;
			pca9665_try_tx_from_isr(handle, task_woken);
			break;

		/* READ ACK: A node is ready to be read from */
		case STA_M_SLAR_SENT_ACKED:
		case STA_M_DATA_RECEIVED_ACKED:
		case STA_M_DATA_RECEIVED_NACKED:

			/* Safety first */
			if (device[handle].rx.frame == NULL)
				goto isr_error;

			if (device[handle].rx.queue == NULL)
				goto isr_error;

			pca9665_read_data_to_buffer(handle);
			int remaining = device[handle].rx.frame->len - device[handle].rx.next_byte;
			driver_debug(DEBUG_I2C, "RX: Remaining %u\r\n", remaining);

			/* If no more to receive */
			if (remaining == 0) {
				if (xQueueSendToBackFromISR(device[handle].rx.queue, &device[handle].rx.frame, task_woken)	== pdFALSE) {
					driver_debug(DEBUG_I2C, "I2C rx queue full - freeing\n\r");
					csp_buffer_free_isr(device[handle].rx.frame);
				}
				device[handle].rx.frame = NULL;
				pca9665_try_tx_from_isr(handle, task_woken);
				break;
			}

			/* If more than a full PCA9665 buffer remains */
			if (remaining > PCA9665_MAX_BUF) {
				pca9665_write_reg(handle, I2CCOUNT, PCA9665_MAX_BUF);

			/* Otherwise, this is the last bit, set NACK on final slave read */
			} else {
				pca9665_write_reg(handle, I2CCOUNT, remaining | 0x80);
			}

			pca9665_write_reg(handle, I2CCON, CON_ENSIO | CON_MODE | CON_AA);
			break;

		/* READ ERROR: A read has failed */
		case STA_M_SLAR_SENT_NACKED:

			/* Safety first */
			if (device[handle].rx.frame == NULL)
				goto isr_error;

			driver_debug(DEBUG_I2C, "I2C SLA+R nacked\n\r");
			csp_buffer_free_isr(device[handle].rx.frame);
			device[handle].rx.frame = NULL;

			/* Start up again */
			pca9665_try_tx_from_isr(handle, task_woken);
			break;

		/**
		 * SLAVE RECEIVER BUFFERED MODE
		 */

		/* START: Lost the arbitration and is addressed as a slave receiver */
		case STA_S_ARB_LOST_SLAW_RECEIVED:
		case STA_S_ARB_LOST_GC_RECEIVED:

			/* Preserve TX frame active, so the START flag will be re-set when the
			 * reception is completed
			 */

			/* Deliberate Fallthrough */

		/* START: Addressed as a slave receiver */
		case STA_S_SLAW_RECEIVED_ACKED:
		case STA_S_GC_RECEIVED:

			/* Check if RX frame was started */
			if (device[handle].rx.frame != NULL)
				goto isr_error;

			/* Allocate new frame */
			device[handle].rx.frame = csp_buffer_get_isr(I2C_MTU);
			if (device[handle].rx.frame == NULL)
				goto isr_error;

			device[handle].is_busy = 1;
			device[handle].rx.next_byte = 0;
			device[handle].rx.frame->len = 0;
			device[handle].rx.frame->dest = device[handle].slave_addr;
			pca9665_write_reg(handle, I2CCOUNT, PCA9665_MAX_BUF);
			pca9665_write_reg(handle, I2CCON, CON_ENSIO | CON_MODE | CON_AA);
			break;

		/* READ: Data received. */
		case STA_S_DATA_RECEIVED_SLA_ACKED:
		case STA_S_DATA_RECEIVED_GC_ACKED:

			/* Safety first */
			if (device[handle].rx.frame == NULL)
				goto isr_error;

			/* Receive data, if any */
			pca9665_read_data_to_buffer(handle);

			/* Limit incoming bytes */
			pca9665_write_reg(handle, I2CCOUNT, (device[handle].rx.next_byte + PCA9665_MAX_BUF > I2C_MTU) ? (I2C_MTU - device[handle].rx.next_byte) | 0x80 : PCA9665_MAX_BUF);
			pca9665_write_reg(handle, I2CCON, CON_ENSIO | CON_MODE | CON_AA);

			break;

		/* STOP or NACK: No more data to receive */
		case STA_S_STOP_REP_RECEIVED:
		case STA_S_DATA_RECEIVED_SLA_NACKED:
		case STA_S_DATA_RECEIVED_GC_NACKED:

			/* Safety first */
			if (device[handle].rx.frame == NULL)
				goto isr_error;

			/* Receive data, if any */
			pca9665_read_data_to_buffer(handle);

			/* Queue up frame */
			device[handle].rx.frame->len = device[handle].rx.next_byte;
			if (device[handle].rx.queue != NULL) {
				if (xQueueSendToBackFromISR(device[handle].rx.queue, &device[handle].rx.frame, task_woken)	== pdFALSE) {
					driver_debug(DEBUG_I2C, "I2C RX queue full\n\r");
					csp_buffer_free_isr(device[handle].rx.frame);
				}
			} else if (device[handle].callback != NULL) {
				device[handle].callback(device[handle].rx.frame, task_woken);
			} else {
				csp_buffer_free_isr(device[handle].rx.frame);
			}

			/* The frame has been freed now */
			device[handle].rx.frame = NULL;

			/* Set back to master mode */
			pca9665_try_tx_from_isr(handle, task_woken);
			break;

		/**
		 * Other IRQ's, typically indicates a hardware or protcol error
		 * The IDLE status is considered an error if asserted at the same time as the Serial Interrupt flag
		 */
		case STA_IDLE:
		default:

isr_error:
			/* Soft reset the device */
			driver_debug(DEBUG_I2C, "I2C ERR 0x%02X\n\r", state);
			pca9665_init_registers(handle);

			/* Clean up RX */
			if (device[handle].rx.frame != NULL) {
				csp_buffer_free_isr(device[handle].rx.frame);
				device[handle].rx.frame = NULL;
			}

			/* Clean up TX */
			if (device[handle].tx.frame != NULL) {
				csp_buffer_free_isr(device[handle].tx.frame);
				device[handle].tx.frame = NULL;
			}

			/* Start up again */
			pca9665_try_tx_from_isr(handle, task_woken);

			break;
		}

	} /**< END Switch/Case */

}

/**
 * Dump all the registers to a string
 *
 * @param handler Device handle
 * @param str Pointer to string (the caller allocates)
 */
void pca9665_dump_regs(int handler) {
	printf("I2CSTA\t\t: 0x%02X\n\r", pca9665_read_reg(handler, I2CSTA));
	printf("I2CDAT\t\t: 0x%02X\n\r", pca9665_read_reg(handler, I2CDAT));
	printf("I2CCON\t\t: 0x%02X\n\r", pca9665_read_reg(handler, I2CCON));
	printf("I2CCOUNT\t: 0x%02X\n\r", pca9665_read_reg(handler, I2CCOUNT));
	printf("I2CADR\t\t: 0x%02X\n\r", pca9665_read_reg(handler, I2CADR));
	printf("I2CSCLL\t\t: 0x%02X\n\r", pca9665_read_reg(handler, I2CSCLL));
	printf("I2CSCLH\t\t: 0x%02X\n\r", pca9665_read_reg(handler, I2CSCLH));
	printf("I2CTO\t\t: 0x%02X\n\r", pca9665_read_reg(handler, I2CTO));
	printf("I2CMODE\t\t: 0x%02X\n\r", pca9665_read_reg(handler, I2CMODE));
}

int pca9665_dump_regs_cmd(struct command_context * ctx) {
	int handler;
	if (ctx->argc != 2)
		return CMD_ERROR_SYNTAX;
	handler = atoi(ctx->argv[1]);
	pca9665_dump_regs(handler);
	return CMD_ERROR_NONE;
}

command_t __root_command pca9554_commands[] = {
	{
		.name = "i2cdump",
		.handler = pca9665_dump_regs_cmd,
		.help = "Dump PCA9665 registers",
		.usage = "<handle-id>"
	}
};

/**
 * Initialise the I2C driver
 *
 * @param handle Which I2C bus (if more than one exists)
 * @param mode I2C device mode. Must be either I2C_MASTER or I2C_SLAVE
 * @param addr Own slave address
 * @param speed Bus speed in kbps
 *
 */
int i2c_init(int handle, int mode, uint8_t addr, uint16_t speed, int queue_len_tx, int queue_len_rx, i2c_callback_t callback) {

	/* Error checking */
	if (handle >= pca9665_device_count)
		return E_NO_DEVICE;

	if (device[handle].is_initialised)
		return E_NO_ERR;

	if (queue_len_tx <= 0)
		return E_INVALID_PARAM;

	if (mode != I2C_MASTER) {
		driver_debug(DEBUG_I2C, "PCA9665 driver does not support slave mode operation\r\n");
		return E_INVALID_PARAM;
	}

	if (i2c_lock == NULL)
		i2c_lock = xSemaphoreCreateMutex();
	if (i2c_lock == NULL)
		return E_NO_BUFFER;

	/* TX Queue */
	if (device[handle].tx.queue == NULL) {
		device[handle].tx.queue = xQueueCreate(queue_len_tx, sizeof(i2c_frame_t *));
	}

	/* RX Queue */
	if ((device[handle].rx.queue == NULL) && (queue_len_rx > 0)) {
		device[handle].rx.queue = xQueueCreate(queue_len_rx, sizeof(i2c_frame_t *));
	}

	/* Callback */
	if (callback != NULL) {
		device[handle].callback = callback;
	}

	/* Call board specific ISR initialization */
	pca9665_isr_init();

	/* Initialise device */
	int result = pca9665_init(handle, device[handle].base, addr, speed);
	if (result == E_NO_ERR) {
		device[handle].is_initialised = 1;
	} else {
		driver_debug(DEBUG_I2C, "Failed to initialise PCA9665 driver\r\n");
	}

	command_register(pca9554_commands);

	return result;
}

/**
 * Send I2C frame via the selected device
 *
 * @param handle Handle to the device
 * @param frame Pointer to I2C frame
 * @param timeout Ticks to wait
 * @return Error code as per error.h
 */
int i2c_send(int handle, i2c_frame_t * frame, uint16_t timeout) {

	if (handle >= pca9665_device_count)
		return E_NO_DEVICE;

	if (!device[handle].is_initialised)
		return E_NO_DEVICE;

	if (xQueueSendToBack(device[handle].tx.queue, &frame, timeout) == pdFALSE)
		return E_TIMEOUT;

	/* Check state in critical region */
	vPortEnterCritical();
	{
		/* If not currently busy, send the start condition... */
		if (device[handle].is_busy == 0)
			pca9665_write_reg(handle, I2CCON, CON_ENSIO | CON_MODE | CON_AA | CON_STA);
	}
	vPortExitCritical();

	return E_NO_ERR;

}

/**
 * receive I2C frame from selected device
 * Context: Task only
 *
 * @param handle Handle to the device
 * @param frame Pointer to I2C frame (free this when done!!!)
 * @param timeout Number of ticks to wait for a frame
 * @return Returns error code: E_NO_ERR if a frame is received, or E_TIMEOUT if timed out, E_NO_DEVICE if handle is not a valid device
 */
int i2c_receive(int handle, i2c_frame_t ** frame, uint16_t timeout) {

	if (handle >= pca9665_device_count)
		return E_NO_DEVICE;

	if (!device[handle].is_initialised)
		return E_NO_DEVICE;

	if (device[handle].rx.queue == NULL)
		return E_NO_DEVICE;

	if (xQueueReceive(device[handle].rx.queue, frame, timeout) == pdFALSE)
		return E_TIMEOUT;

	return E_NO_ERR;

}

/**
 * Context: Task only
 */
int i2c_master_transaction(int handle, uint8_t addr, void * txbuf, size_t txlen, void * rxbuf, size_t rxlen, uint16_t timeout) {

	if (handle >= pca9665_device_count)
		return E_NO_DEVICE;

	if (!device[handle].is_initialised)
		return E_NO_DEVICE;

	if ((txlen > I2C_MTU - 10) || (rxlen > I2C_MTU - 10))
		return E_INVALID_BUF_SIZE;

	i2c_frame_t * frame = csp_buffer_get(I2C_MTU);
	if (frame == NULL)
		return E_NO_BUFFER;

	xSemaphoreTake(i2c_lock, 10 * configTICK_RATE_HZ);

	frame->dest = addr;
	memcpy(&frame->data[0], txbuf, txlen);
	frame->len = txlen;
	frame->len_rx = rxlen;

	if (i2c_send(handle, frame, 0) != E_NO_ERR) {
		csp_buffer_free(frame);
		xSemaphoreGive(i2c_lock);
		return E_TIMEOUT;
	}

	if (rxlen == 0) {
		xSemaphoreGive(i2c_lock);
		return E_NO_ERR;
	}

	if (i2c_receive(handle, &frame, timeout) != E_NO_ERR) {
		xSemaphoreGive(i2c_lock);
		return E_TIMEOUT;
	}

	memcpy(rxbuf, &frame->data[0], rxlen);

	csp_buffer_free(frame);
	xSemaphoreGive(i2c_lock);
	return E_NO_ERR;

}
