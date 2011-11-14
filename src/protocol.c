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

#include "usart.h"

#include "protocol.h"

struct p_state {
	enum p_parser_state parser_state;
	char hook_id;
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
			p_state.parser_state = PPS_PRE;
		}
		break;
	case PPS_PRE:
		if (ch == 'i') {
			p_state.parser_state = PPS_HOOK_ID;
		} else {
			p_state.parser_state = PPS_IDLE;
		}
		break;
	case PPS_HOOK_ID:
		if (p_hooks[(int)ch] != NULL) {
			p_state.hook_id = ch;
			p_state.parser_state = PPS_HOOK;
		} else {
			p_state.parser_state = PPS_IDLE;
		}
		break;
	case PPS_HOOK:
		p_state.parser_state = p_hooks[(int)p_state.hook_id](ch);
		break;
	}
}

void p_register_hook(char id, p_hook_f hook)
{
	p_hooks[(int)id] = hook;
}
