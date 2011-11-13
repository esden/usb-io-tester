/*
 * This is part of the usb io tester.
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

#include <stdlib.h>

#include "protocol.h"

struct p_state {
  enum p_parser_state parser_state;
  char u_data[100];
  int u_size;
  int u_tmp_size;
};

struct p_state p_state;

p_hook_f p_hooks[128];

void p_init(void)
{
	int i;

	p_state.parser_state = PPS_IDLE;
	p_state.u_size = 0;
	p_state.u_tmp_size = 0;

	for (i=0; i< 128; i++) {
		p_hooks[i] = NULL;
	}
}

void p_parse_byte(char ch)
{
	switch (p_state.parser_state) {
	case PPS_IDLE:
		if (ch == 'u') {
			p_state.parser_state = PPS_PRE1;
		}
		break;
	case PPS_PRE1:
		if (ch == 'i') {
			p_state.parser_state = PPS_PRE2;
		} else {
			p_state.parser_state = PPS_IDLE;
		}
		break;
	case PPS_PRE2:
		switch (ch) {
		case 'l':
			p_state.parser_state = PPS_LED;
			break;
		case 'u':
			p_state.parser_state = PPS_USART_ID;
			break;
		}
		break;
	case PPS_LED:

		p_state.parser_state = p_hooks['l'](ch);

		p_state.parser_state = PPS_IDLE;

		break;
	case PPS_USART_ID:
		switch (ch) {
		case '2':
			p_state.parser_state = PPS_USART_SIZE;
			break;
		default:
			p_state.parser_state = PPS_IDLE;
			break;
		}
		break;
	case PPS_USART_SIZE:
		p_state.u_tmp_size = p_state.u_size = ch - '0';
		p_state.parser_state = PPS_USART_DATA;
		break;
	case PPS_USART_DATA:
		p_state.u_data[--p_state.u_tmp_size] = ch;
		if (p_state.u_tmp_size == 0) {
			usart2_send(p_state.u_data, p_state.u_size);
			p_state.parser_state = PPS_IDLE;
		}
		break;
	}
}

void p_register_hook(char id, p_hook_f hook)
{
	p_hooks[(int)id] = hook;
}
