/*
 * This file was part of the libopencm3 project.
 * Was adapted to be used as part of Transition Robotics Inc. hardware testing platform.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 * Copyright (C) 2011 Piotr Esden-Tempski <piotr@transition-robotics.com>
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

#include "system.h"
#include "led.h"
#include "cdcacm.h"
#include "usart.h"

int led1_toggle_flag = 0;
int led2_toggle_flag = 0;
int led3_toggle_flag = 0;
int led4_toggle_flag = 0;
int led5_toggle_flag = 0;
int ret_flag = 0;

enum p_state {
  PS_IDLE,
  PS_PRE1,
  PS_PRE2,
  PS_LED,
  PS_USART_ID,
  PS_USART_SIZE,
  PS_USART_DATA
};

enum p_state p_state;

char p_u_data[100];
int p_u_size;
int p_u_tmp_size;

void p_init(void)
{
	p_state = PS_IDLE;
	p_u_size = 0;
}

void p_parse_byte(char ch)
{
	switch (p_state) {
	case PS_IDLE:
		if (ch == 'u') {
			p_state = PS_PRE1;
		}
		break;
	case PS_PRE1:
		if (ch == 'i') {
			p_state = PS_PRE2;
		} else {
			p_state = PS_IDLE;
		}
		break;
	case PS_PRE2:
		switch (ch) {
		case 'l':
			p_state = PS_LED;
			break;
		case 'u':
			p_state = PS_USART_ID;
			break;
		}
		break;
	case PS_LED:
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

		p_state = PS_IDLE;

		break;
	case PS_USART_ID:
		switch (ch) {
		case '2':
			p_state = PS_USART_SIZE;
			break;
		default:
			p_state = PS_IDLE;
			break;
		}
		break;
	case PS_USART_SIZE:
		p_u_tmp_size = p_u_size = ch - '0';
		p_state = PS_USART_DATA;
		break;
	case PS_USART_DATA:
		p_u_data[--p_u_tmp_size] = ch;
		if (p_u_tmp_size == 0) {
			usart2_send(p_u_data, p_u_size);
			p_state = PS_IDLE;
		}
		break;
	}
}

void cdcacm_input_callback(char *data, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		p_parse_byte(data[i]);
	}
}

void led_toggle_process(void)
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
	} else

	if (ret_flag == 1) {
		ret_flag = 0;
		cdcacm_send("\r\n", 2);
	}
}

void usart2_input_callback(char *data, int size)
{
	char buff[140];
	char *prefix = "usart2 -> ";
	int i;

	for (i = 0; i<10; i++) {
		buff[i] = prefix[i];
	}
	for (i = 10; i<size+10; i++) {
		buff[i] = data[i-10];
	}
	buff[i++] = '\r';
	buff[i++] = '\n';
	cdcacm_send(buff, i);
}

int main(void)
{
	int i;

	led1_toggle_flag = 0;

	system_init();
	led_init();
	p_init();
	cdcacm_init();
	cdcacm_register_receive_callback(cdcacm_input_callback);
	usart_init();
	usart2_register_receive_callback(usart2_input_callback);

	led1_on();
	led2_on();
	led3_on();
	led4_on();
	led5_on();

	for (i = 0; i < 0x800000; i++)
		__asm__("nop");

	led1_off();
	led2_off();
	led3_off();
	led4_off();
	led5_off();

	while (1) {
		cdcacm_run();
		led_toggle_process();
	}
}
