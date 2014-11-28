/**
 * @file usart.h
 * Common USART interface
 *
 * @author Johan De Claville Christiansen
 * Copyright 2011 GomSpace ApS. All rights reserved.
 */

#ifndef USART_H_
#define USART_H_

#include <stdint.h>

/**
 * Initialise UART
 *
 * @param handle usart[0,1,2,3]
 * @param fcpu CPU speed in Hz
 * @param usart_baud Baud rate in bps
 */
void usart_init(int handle, uint32_t fcpu, uint32_t usart_baud);

/**
 * In order to catch incoming chars use the callback.
 * Only one callback per interface.
 * @param handle usart[0,1,2,3]
 * @param callback function pointer
 */
typedef void (*usart_callback_t) (uint8_t * buf, int len, void * pxTaskWoken);
void usart_set_callback(int handle, usart_callback_t callback);

/**
 * Insert a character to the RX buffer of a usart
 * @param handle usart[0,1,2,3]
 * @param c Character to insert
 */
void usart_insert(int handle, char c, void * pxTaskWoken);

/**
 * Polling putchar
 *
 * @param handle usart[0,1,2,3]
 * @param c Character to transmit
 */
void usart_putc(int handle, char c);

/**
 * Send char buffer on UART
 *
 * @param handle usart[0,1,2,3]
 * @param buf Pointer to data
 * @param len Length of data
 */
void usart_putstr(int handle, char *buf, int len);

/**
 * Buffered getchar
 *
 * @param handle usart[0,1,2,3]
 * @return Character received
 */
char usart_getc(int handle);

/**
 * Buffered getchar (not blocking)
 *
 * @param handle usart[0,1,2,3]
 * @return Character received
 */
char usart_getc_nblock(int handle);

/**
 * Report number of messages waiting on the RX queue of a handle
 * @param handle usart[0,1,2,3]
 * @return Number of char's in rx buffer
 */
int usart_messages_waiting(int handle);

#endif /* USART_H_ */
