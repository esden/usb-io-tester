/*
 * This file is part of usb io tesing firmware.
 *
 * Copyright (C) 2011 Piotr Esden-Tempski <piotr@esden.net>
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

#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>

#include "cdcacm.h"
#include "protocol.h"

#include "led.h"

int led1_toggle_flag = 0;
int led2_toggle_flag = 0;
int led3_toggle_flag = 0;
int led4_toggle_flag = 0;
int led5_toggle_flag = 0;

enum p_parser_state p_led_hook(char ch);

void led_init()
{
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_AFIOEN);

	/* led 1 */
	gpio_set(GPIOA, GPIO8);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);

	/* led 2 formely trst */
	gpio_set(GPIOB, GPIO4);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);

	AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_FULL_SWJ_NO_JNTRST;

	/* led 3 formely adc 6 */
	gpio_set(GPIOC, GPIO2);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);

	/* led 4 formely adc 4 */
	gpio_set(GPIOC, GPIO5);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);

	/* led 5 */
	gpio_set(GPIOC, GPIO15);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, GPIO15);


	/* Initialize internal state */
	led1_toggle_flag = 0;
	led2_toggle_flag = 0;
	led3_toggle_flag = 0;
	led4_toggle_flag = 0;
	led5_toggle_flag = 0;

	/* Register our hook to the protocol */
	p_register_hook('l', p_led_hook);
}

void led1_on()
{
	gpio_clear(GPIOA, GPIO8);
}

void led1_off()
{
	gpio_set(GPIOA, GPIO8);
}

void led1_toggle()
{
	gpio_toggle(GPIOA, GPIO8);
}

void led2_on()
{
	gpio_clear(GPIOB, GPIO4);
}

void led2_off()
{
	gpio_set(GPIOB, GPIO4);
}

void led2_toggle()
{
	gpio_toggle(GPIOB, GPIO4);
}

void led3_on()
{
	gpio_clear(GPIOC, GPIO2);
}

void led3_off()
{
	gpio_set(GPIOC, GPIO2);
}

void led3_toggle()
{
	gpio_toggle(GPIOC, GPIO2);
}

void led4_on()
{
	gpio_clear(GPIOC, GPIO5);
}

void led4_off()
{
	gpio_set(GPIOC, GPIO5);
}

void led4_toggle()
{
	gpio_toggle(GPIOC, GPIO5);
}

void led5_on()
{
	gpio_clear(GPIOC, GPIO15);
}

void led5_off()
{
	gpio_set(GPIOC, GPIO15);
}

void led5_toggle()
{
	gpio_toggle(GPIOC, GPIO15);
}

enum p_parser_state p_led_hook(char ch)
{
		switch (ch) {
		case '1':
			led1_toggle_flag = 1;
			break;
		case '2':
			led2_toggle_flag = 1;
			break;
		case '3':
			led3_toggle_flag = 1;
			break;
		case '4':
			led4_toggle_flag = 1;
			break;
		case '5':
			led5_toggle_flag = 1;
			break;
		}

		return PPS_IDLE;
}

void led_process(void)
{

	if (led1_toggle_flag == 1) {
		led1_toggle_flag = 0;
		led1_toggle();
		cdcacm_send("led1 toggle\r\n", 13);
	} else

	if (led2_toggle_flag == 1) {
		led2_toggle_flag = 0;
		led2_toggle();
		cdcacm_send("led2 toggle\r\n", 13);
	} else

	if (led3_toggle_flag == 1) {
		led3_toggle_flag = 0;
		led3_toggle();
		cdcacm_send("led3 toggle\r\n", 13);
	} else

	if (led4_toggle_flag == 1) {
		led4_toggle_flag = 0;
		led4_toggle();
		cdcacm_send("led4 toggle\r\n", 13);
	} else

	if (led5_toggle_flag == 1) {
		led5_toggle_flag = 0;
		led5_toggle();
		cdcacm_send("led5 toggle\r\n", 13);
	}
}
