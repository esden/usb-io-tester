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

#include "usart.h"

volatile char usart2_rx_buffer[128];
volatile int usart2_rx_buffer_counter = 0;

usart_receive_callback _usart2_receive_callback;

volatile char usart2_tx_buffer[128];
volatile int usart2_tx_buffer_counter = 0;

void usart_init(void)
{
	/* local state reset */
	usart2_rx_buffer_counter = 0;
	usart2_tx_buffer_counter = 0;

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
			if (_usart2_receive_callback != NULL) {
				_usart2_receive_callback(usart2_rx_buffer, usart2_rx_buffer_counter);
			}
			usart2_rx_buffer_counter = 0;
		}
	}

	/* Check if we were called because of TXE. */
	if (((USART_CR1(USART2) & USART_CR1_TXEIE) != 0) &&
	    ((USART_SR(USART2) & USART_SR_TXE) != 0)) {

		/* Put data into the transmit register. */
		usart_send(USART2, (u16)usart2_tx_buffer[--usart2_tx_buffer_counter]);

		if ( usart2_tx_buffer_counter == 0) {
			/* Disable the TXE interrupt as we don't need it anymore. */
			USART_CR1(USART2) &= ~USART_CR1_TXEIE;
		}
	}
}

void usart2_register_receive_callback(usart_receive_callback callback)
{
	_usart2_receive_callback = callback;
}

int usart2_send(char *data, int size)
{
	int i;

	if (usart2_tx_buffer_counter != 0) {
		return 0;
	}
	if ( size > 128 ) {
		size = 128;
	}

	for (i=0; i<128; i++) {
		usart2_tx_buffer[i] = data[i];
	}

	usart2_tx_buffer_counter = size;

	USART_CR1(USART2) |= USART_CR1_TXEIE;

	return size;
}
