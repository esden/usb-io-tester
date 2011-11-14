/*
 * This file was part of the libopencm3 project.
 * Now it is part of usb io tester firmware.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2010 Piotr Esden-Tempski <piotr@transition-robotics.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/nvic.h>

#include "cdcacm.h"
#include "protocol.h"

#include "usart.h"

char usart2_rx_buffer[128];
int usart2_rx_buffer_counter = 0;

volatile char usart2_tx_buffer[128];
volatile int usart2_tx_buffer_counter = 0;
volatile int usart2_tx_buffer_index = 0;

void usart_report_rx(char *prefix, int psize, char *data, int size);
enum p_parser_state p_usart_hook(char ch);

void usart_init(void)
{
	/* local state reset */
	usart2_rx_buffer_counter = 0;
	usart2_tx_buffer_counter = 0;
	usart2_tx_buffer_index = 0;

	/* Enable peripheral clocks */
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_USART2EN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN);

	/* Enable the USART1 interrupt. */
	nvic_enable_irq(NVIC_USART2_IRQ);

	/* Enable TX GPIO */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);

	/* Enable RX GPIO */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
		      GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART2, 115200, rcc_ppre1_frequency);
	usart_set_databits(USART2, 8);
	usart_set_stopbits(USART2, USART_STOPBITS_1);
	usart_set_parity(USART2, USART_PARITY_NONE);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART2, USART_MODE_TX_RX);

	/* Enable USART2 Receive interrupt. */
	USART_CR1(USART2) |= USART_CR1_RXNEIE;

	/* Finally enable the USART. */
	usart_enable(USART2);

	p_register_hook('u', p_usart_hook);

}

void usart2_isr(void)
{
	/* Check if we were called because of RXNE. */
	if (((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(USART2) & USART_SR_RXNE) != 0)) {

		/* Retrieve the data from the peripheral. */
		usart2_rx_buffer[usart2_rx_buffer_counter++] = usart_recv(USART2);

		if ((usart2_rx_buffer[usart2_rx_buffer_counter-1] == '\0') ||
		    (usart2_rx_buffer_counter == 128)) {
			usart_report_rx("usart2 -> ", 10, usart2_rx_buffer, usart2_rx_buffer_counter);
			usart2_rx_buffer_counter = 0;
		}
	}

	/* Check if we were called because of TXE. */
	if (((USART_CR1(USART2) & USART_CR1_TXEIE) != 0) &&
	    ((USART_SR(USART2) & USART_SR_TXE) != 0)) {

		/* Put data into the transmit register. */
		usart_send(USART2, (u16)usart2_tx_buffer[usart2_tx_buffer_index++]);

		if ( usart2_tx_buffer_index == usart2_tx_buffer_counter) {
			usart2_tx_buffer_index = 0;
			/* Disable the TXE interrupt as we don't need it anymore. */
			USART_CR1(USART2) &= ~USART_CR1_TXEIE;
		}
	}
}

int usart2_send(char *data, int size)
{
	int i;

	if (usart2_tx_buffer_index != 0) {
		return 0;
	}
	if ( size > 128 ) {
		size = 128;
	}

	for (i=0; i<size; i++) {
		usart2_tx_buffer[i] = data[i];
	}

	usart2_tx_buffer_counter = size;

	USART_CR1(USART2) |= USART_CR1_TXEIE;

	return size;
}

enum p_parser_state p_usart_hook(char ch)
{
	enum p_usart_state {
		PUS_ID,
		PUS_SIZE,
		PUS_DATA
	};
	static enum p_usart_state state = PUS_ID;
	static int id;
	static int size;
	static char data[128];
	static int data_count;
	static char *id_error_label = "ERR: Unknown USART ID '";
	static char *size_error_label = "ERR: String oversize '";
	enum p_parser_state ret = PPS_IDLE;
	int i;


	switch (state) {
	case PUS_ID:
		switch (ch) {
		case '2':
			id = 2;
			size = 0;
			ret = PPS_HOOK;
			state = PUS_SIZE;
			break;
		default:
			for (i=0; i<23; i++) {
				data[i] = id_error_label[i];
			}
			data[i++] = ch;
			data[i++] = '\'';
			data[i++] = '\r';
			data[i++] = '\n';
			cdcacm_send(data, i);
			ret = PPS_IDLE;
		}
		break;
	case PUS_SIZE:
		if ((ch >= '0') &&
		    (ch <= '9')) {
			size *= 10;
			size += ch - '0';
			if (size < 128) {
				ret = PPS_HOOK;
			} else {
				for (i=0; i<22; i++) {
					data[i] = size_error_label[i];
				}
				data[i++] = '\'';
				data[i++] = '\r';
				data[i++] = '\n';
				cdcacm_send(data, i);
				ret = PPS_IDLE;
				state = PUS_ID;
			}
		} else {
			if (size > 0) {
				data_count = 0;
				ret = PPS_HOOK;
				state = PUS_DATA;
			} else {
				ret = PPS_IDLE;
				state = PUS_ID;
			}
		}
		break;
	case PUS_DATA:
		data[data_count++] = ch;
		if (data_count == size) {
			switch (id) {
			case 2:
				usart2_send(data, size);
				break;
			}
			ret = PPS_IDLE;
			state = PUS_ID;
		} else {
			ret = PPS_HOOK;
		}
		break;
	}

	return ret;
}

void usart_report_rx(char *prefix, int psize, char *data, int size)
{
	char buff[140];
	int i;

	for (i = 0; i<psize; i++) {
		buff[i] = prefix[i];
	}
	for (i = psize; i<size+psize; i++) {
		buff[i] = data[i-psize];
	}
	buff[i++] = '\r';
	buff[i++] = '\n';
	cdcacm_send(buff, i);
}
