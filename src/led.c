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

#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>

void led_init()
{
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN);

	gpio_set(GPIOC, GPIO2);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);
	gpio_set(GPIOC, GPIO5);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
			GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);
}

void led1_on()
{
	gpio_clear(GPIOC, GPIO2);
}

void led1_off()
{
	gpio_set(GPIOC, GPIO2);
}
