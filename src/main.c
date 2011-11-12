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

int led1_toggle_flag = 0;
int led2_toggle_flag = 0;
int led3_toggle_flag = 0;
int led4_toggle_flag = 0;
int led5_toggle_flag = 0;
int ret_flag = 0;

void cdcacm_input_callback(char *data, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		if ( data[i] == '1' ) {
			led1_toggle_flag = 1;
		}
		if ( data[i] == '2' ) {
			led2_toggle_flag = 1;
		}
		if ( data[i] == '3' ) {
			led3_toggle_flag = 1;
		}
		if ( data[i] == '4' ) {
			led4_toggle_flag = 1;
		}
		if ( data[i] == '5' ) {
			led5_toggle_flag = 1;
		}
		if ( data[i] == '\n' || data[i] == '\r' ) {
			ret_flag = 1;
		}
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

int main(void)
{
	int i;

	led1_toggle_flag = 0;

	system_init();
	led_init();
	cdcacm_init();
	cdcacm_register_receive_callback(cdcacm_input_callback);

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
