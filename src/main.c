/*
 * This file is part of usb io tester firmware.
 *
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
#include "protocol.h"
#include "led.h"
#include "cdcacm.h"
#include "usart.h"

void cdcacm_input_callback(char *data, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		p_parse_byte(data[i]);
	}
}

int main(void)
{
	int i;

	system_init();
	p_init();
	led_init();
	cdcacm_init();
	cdcacm_register_receive_callback(cdcacm_input_callback);
	usart_init();

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
		led_process();
	}
}
