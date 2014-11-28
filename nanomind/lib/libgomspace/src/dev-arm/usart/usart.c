/**
 * @file usart.c
 * Driver for UART on AT91SAM7A1
 *
 * @author Johan De Claville Christiansen
 * Copyright 2012 GomSpace Aps. All rights reserved.
 */

#include <stdint.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <dev/arm/at91sam7.h>
#include <dev/usart.h>

#define USART_COUNT 			3
#define USART_RX_QUEUE_LENGTH 	1024
#define USART_RX_BUF_LENGTH		0x1000
#define USART_TX_BUF_LENGTH 	0x1000

static void __attribute__((naked)) usart0_ISR(void);
static void __attribute__((naked)) usart1_ISR(void);
static void __attribute__((naked)) usart2_ISR(void);

static xSemaphoreHandle usart_pdc_irq = NULL;

static struct usart_struct {
	AT91PS_USART base;
	xQueueHandle usart_rxqueue;
	usart_callback_t rx_callback;
	void (*isr)(void);
	int irq;
	volatile unsigned long * rx_pdc_mpr;
	volatile unsigned long * rx_pdc_tcr;
	volatile unsigned long * tx_pdc_tcr;
	volatile unsigned long * tx_pdc_mpr;
	volatile int rx_pdc_pending;

	char tx_buf1[USART_TX_BUF_LENGTH];
	char tx_buf2[USART_TX_BUF_LENGTH];
	char * tx_curbuf;
	int tx_nextbuf_len;

	xSemaphoreHandle tx_sem;

	char rx_buf1[USART_RX_BUF_LENGTH];
	char rx_buf2[USART_RX_BUF_LENGTH];
	char * rx_curbuf;
	char * rx_prevbuf;
	int rx_prevbuf_len;

} usart[USART_COUNT] = {
	{
		.base = AT91C_BASE_USART0,
		.isr = usart0_ISR,
		.irq = AT91C_ID_USART0,
		.rx_pdc_mpr = &PDC_MPR0,
		.rx_pdc_tcr = &PDC_TCR0,
		.tx_pdc_mpr = &PDC_MPR1,
		.tx_pdc_tcr = &PDC_TCR1,
		.tx_nextbuf_len = 0,
	},
	{
		.base = AT91C_BASE_USART1,
		.isr = usart1_ISR,
		.irq = AT91C_ID_USART1,
		.rx_pdc_mpr = &PDC_MPR2,
		.rx_pdc_tcr = &PDC_TCR2,
		.tx_pdc_mpr = &PDC_MPR3,
		.tx_pdc_tcr = &PDC_TCR3,
		.tx_nextbuf_len = 0,
	},
	{
		.base = AT91C_BASE_USART2,
		.isr = usart2_ISR,
		.irq = AT91C_ID_USART2,
		.rx_pdc_mpr = &PDC_MPR4,
		.rx_pdc_tcr = &PDC_TCR4,
		.tx_pdc_mpr = &PDC_MPR5,
		.tx_pdc_tcr = &PDC_TCR5,
		.tx_nextbuf_len = 0,
	}
};

static void usart_try_tx_from_isr(int handle, portBASE_TYPE * pxTaskWoken) {

	if (usart[handle].tx_nextbuf_len == 0)
		return;

	*usart[handle].tx_pdc_mpr = (uintptr_t) usart[handle].tx_curbuf;
	*usart[handle].tx_pdc_tcr = usart[handle].tx_nextbuf_len;

	usart[handle].tx_curbuf = (usart[handle].tx_curbuf == usart[handle].tx_buf1) ? usart[handle].tx_buf2 : usart[handle].tx_buf1;
	usart[handle].tx_nextbuf_len = 0;

	xSemaphoreGiveFromISR(usart[handle].tx_sem, pxTaskWoken);

}

static void __attribute__((noinline)) usart_DSR(int handle) {

	signed portBASE_TYPE xTaskWoken = pdFALSE;
	unsigned portLONG status = usart[handle].base->SR;

	/* TX */
	if (status & ENDTX) {
		usart_try_tx_from_isr(handle, &xTaskWoken);
		return;
	}

	/* RX PDC */
	if (usart_pdc_irq != NULL) {

		if (*usart[handle].rx_pdc_tcr != USART_RX_BUF_LENGTH && usart[handle].rx_pdc_pending == 0) {

			/* Stop PDC */
			*usart[handle].rx_pdc_tcr = 0;
			*usart[handle].rx_pdc_tcr = 0;

			/* Fetch length from buffer */
			usart[handle].rx_prevbuf_len = *usart[handle].rx_pdc_mpr - (unsigned int) usart[handle].rx_curbuf;

			/* Rotate buffers */
			usart[handle].rx_prevbuf = usart[handle].rx_curbuf;
			usart[handle].rx_curbuf = (usart[handle].rx_curbuf == usart[handle].rx_buf1) ? usart[handle].rx_buf2 : usart[handle].rx_buf1;

			/* Re-Setup PDC */
			*usart[handle].rx_pdc_mpr = (unsigned int) usart[handle].rx_curbuf;
			*usart[handle].rx_pdc_tcr = USART_RX_BUF_LENGTH;

			/* Trigger on ENDRX ISR, not on RXRDY */
			usart[handle].base->IDR = 0xFFFFFFFF;
			usart[handle].base->IER = ENDRX | ENDTX;

			/* Wake task */
			usart[handle].rx_pdc_pending = 1;
			xSemaphoreGiveFromISR(usart_pdc_irq, &xTaskWoken);

		}

	/* RX NO PDC */
	} else if (status & (RXRDY | ENDRX)) {

		static uint8_t byte;
		byte = usart[handle].base->RHR;
		if(usart[handle].rx_callback != NULL) {
			usart[handle].rx_callback(&byte, 1, &xTaskWoken);
		} else {
			xQueueSendToBackFromISR(usart[handle].usart_rxqueue, &byte, &xTaskWoken);
		}

	}

	if (xTaskWoken == pdTRUE) {
		portYIELD_FROM_ISR();
	}

}

static void __attribute__((naked)) usart0_ISR(void) {
	portSAVE_CONTEXT();
	usart_DSR(0);
	AT91C_BASE_AIC->AIC_EOICR = 1;
	portRESTORE_CONTEXT();
}

static void __attribute__((naked)) usart1_ISR(void) {
	portSAVE_CONTEXT();
	usart_DSR(1);
	AT91C_BASE_AIC->AIC_EOICR = 1;
	portRESTORE_CONTEXT();
}

static void __attribute__((naked)) usart2_ISR(void) {
	portSAVE_CONTEXT();
	usart_DSR(2);
	AT91C_BASE_AIC->AIC_EOICR = 1;
	portRESTORE_CONTEXT();
}

void vTaskUsartRx(void * pvParameters) {

	int i;
	int handle;

	/* Allocate the RX task PDC ISR queue */
	if (usart_pdc_irq == NULL)
		vSemaphoreCreateBinary(usart_pdc_irq);

	/* Setup DMA */
	for (handle = 0; handle < USART_COUNT; handle++) {
		*usart[handle].rx_pdc_mpr = (unsigned int) usart[handle].rx_curbuf;
		*usart[handle].rx_pdc_tcr = USART_RX_BUF_LENGTH;
		usart[handle].base->IDR = 0xFFFFFFFF;
		usart[handle].base->IER = ENDRX | RXRDY | ENDTX;
	}

	while(1) {

		/* Wait for curbuf to be complete */
		xSemaphoreTake(usart_pdc_irq, portMAX_DELAY);

		for (handle = 0; handle < USART_COUNT; handle++) {

			if (usart[handle].rx_pdc_pending == 0)
				continue;
			if (usart[handle].rx_prevbuf_len == 0)
				continue;

			/* Process buffer deferred */
			if (usart[handle].rx_callback != NULL) {
				usart[handle].rx_callback((unsigned char *) usart[handle].rx_prevbuf, usart[handle].rx_prevbuf_len, NULL);
			} else if (usart[handle].usart_rxqueue != NULL) {
				for (i = 0; i < usart[handle].rx_prevbuf_len; i++)
					xQueueSendToBack(usart[handle].usart_rxqueue, usart[handle].rx_prevbuf + i, 0);
			}

			portENTER_CRITICAL();

			/* Stop PDC if something happened */
			if (USART_RX_BUF_LENGTH - *usart[handle].rx_pdc_tcr != 0) {

				/* Stop PDC */
				*usart[handle].rx_pdc_tcr = 0;
				*usart[handle].rx_pdc_tcr = 0;

				/* Fetch length from buffer */
				usart[handle].rx_prevbuf_len = *usart[handle].rx_pdc_mpr - (unsigned int) usart[handle].rx_curbuf;

				/* Rotate buffers */
				usart[handle].rx_prevbuf = usart[handle].rx_curbuf;
				usart[handle].rx_curbuf = (usart[handle].rx_curbuf == usart[handle].rx_buf1) ? usart[handle].rx_buf2 : usart[handle].rx_buf1;

				/* Re-Setup PDC */
				*usart[handle].rx_pdc_mpr = (unsigned int) usart[handle].rx_curbuf;
				*usart[handle].rx_pdc_tcr = USART_RX_BUF_LENGTH;

				/* Wake task */
				usart[handle].rx_pdc_pending = 1;
				xSemaphoreGive(usart_pdc_irq);

			} else {

				usart[handle].rx_pdc_pending = 0;
				usart[handle].base->IDR = 0xFFFFFFFF;
				usart[handle].base->IER = ENDRX | RXRDY | ENDTX;

			}

			portEXIT_CRITICAL();

		}

	}

}

/**
 * Initialise UART
 *
 * @param fcpu CPU speed in Hz
 * @param usart_baud Baud rate in bps
 */
void usart_init(int handle, uint32_t fcpu, uint32_t usart_baud) {

	/* Allocate the RX queue for the device */
	if (usart[handle].usart_rxqueue == NULL)
		usart[handle].usart_rxqueue = xQueueCreate(USART_RX_QUEUE_LENGTH, 1);

	/* TX */
	if (usart[handle].tx_sem == NULL)
		vSemaphoreCreateBinary(usart[handle].tx_sem);
	usart[handle].tx_curbuf = usart[handle].tx_buf1;

	/* RX */
	usart[handle].rx_curbuf = usart[handle].rx_buf1;

	/* Switch On USART */
	usart[handle].base->ECR = (USART | PIO);
	AT91C_BASE_PMC->ECR = 2; /* Enable PMC for PDC */

	/* Set SCK pin as output and low */
	usart[handle].base->PER = SCK; /* Enable PIO on SCK */
	usart[handle].base->OER = SCK; /* Enable PIO on SCK as output */
	usart[handle].base->CODR = SCK; /* Set pin SCK low */

	/* Reset Error Flags Rx & Tx State Machine */
	usart[handle].base->CR = (RSTRX | RSTTX);
	usart[handle].base->CR = RSTSTA;

	/* Disable port interrupts while changing hardware */
	usart[handle].base->IDR = 0xFFFFFFFF;

	/* Initialize the channel */
	/* Tx and Rx as peripheral */
	usart[handle].base->PDR = (TXD | RXD);

	/* Set Mode */
	usart[handle].base->MR = CHRL_8 | PAR_NO | NBSTOP_1 | USCLKS_MCKI;

	/* Set baud rate. */
	usart[handle].base->BRGR = (fcpu / 16 / usart_baud);

	// Register intterupt service routine
	AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, usart[handle].irq, AT91C_AIC_PRIOR_HIGHEST, AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE, usart[handle].isr);
	AT91F_AIC_EnableIt(AT91C_BASE_AIC, usart[handle].irq);

	/* Map PDC channels */
	switch (handle) {
	case 0:
		PDC_PRA0 = (uint32_t) & (usart[handle].base->RHR);
		PDC_PRA1 = (uint32_t) & (usart[handle].base->THR);
		PDC_CR0 = 0;
		PDC_CR1 = 1;
		break;
	case 1:
		PDC_PRA2 = (uint32_t) & (usart[handle].base->RHR);
		PDC_PRA3 = (uint32_t) & (usart[handle].base->THR);
		PDC_CR2 = 0;
		PDC_CR3 = 1;
		break;
	case 2:
		PDC_PRA4 = (uint32_t) & (usart[handle].base->RHR);
		PDC_PRA5 = (uint32_t) & (usart[handle].base->THR);
		PDC_CR4 = 0;
		PDC_CR5 = 1;
		break;
	}

	/* Configure PDC Rx Line */
	*usart[handle].rx_pdc_tcr = 0;

	/* Configure PDC Tx Line */
	*usart[handle].tx_pdc_mpr = (uintptr_t) usart[handle].tx_buf1;
	*usart[handle].tx_pdc_tcr = 0;

	usart[handle].base->IER = RXRDY | ENDRX | ENDTX;

	/* Enable Tx and Rx */
	usart[handle].base->CR = TXEN | RXEN;

}

void usart_set_callback(int handle, usart_callback_t callback) {
	usart[handle].rx_callback = callback;
}

void usart_insert(int handle, char c, void * pxTaskWoken) {
	if (pxTaskWoken == NULL)
		xQueueSendToBack(usart[handle].usart_rxqueue, &c, 0);
	else
		xQueueSendToBackFromISR(usart[handle].usart_rxqueue, &c, pxTaskWoken);
}

/**
 * Polling putchar
 *
 * @param c Character to transmit
 */
void usart_putc(int handle, char c) {

	portBASE_TYPE xTaskWoken = pdFALSE;

	/* Assume max 10 concurrent calls to usart_putc */
	if (usart[handle].tx_nextbuf_len + 10 > USART_TX_BUF_LENGTH) {
		xSemaphoreTake(usart[handle].tx_sem, 0);
		xSemaphoreTake(usart[handle].tx_sem, portMAX_DELAY);
	}

	portENTER_CRITICAL();
	usart[handle].tx_curbuf[usart[handle].tx_nextbuf_len++] = c;

	if (*usart[handle].tx_pdc_tcr == 0)
		usart_try_tx_from_isr(handle, &xTaskWoken);
	portEXIT_CRITICAL();

}

void usart_putstr(int handle, char *buf, int len) {

	portBASE_TYPE xTaskWoken = pdFALSE;

	/* Assume max 10 concurrent calls to usart_putc */
	if (usart[handle].tx_nextbuf_len + len + 10 > USART_TX_BUF_LENGTH) {
		xSemaphoreTake(usart[handle].tx_sem, 0);
		xSemaphoreTake(usart[handle].tx_sem, portMAX_DELAY);
	}

	portENTER_CRITICAL();
	memcpy(&usart[handle].tx_curbuf[usart[handle].tx_nextbuf_len], buf, len);
	usart[handle].tx_nextbuf_len += len;

	if (*usart[handle].tx_pdc_tcr == 0)
		usart_try_tx_from_isr(handle, &xTaskWoken);
	portEXIT_CRITICAL();
}

char usart_getc(int handle) {
	char buf;
	xQueueReceive(usart[handle].usart_rxqueue, &buf, portMAX_DELAY);
	return buf;
}

int usart_messages_waiting(int handle) {
	return uxQueueMessagesWaiting(usart[handle].usart_rxqueue);
}
