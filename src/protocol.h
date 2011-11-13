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

#ifndef PROTOCOL_H
#define PROTOCOL_H

enum p_parser_state {
  PPS_IDLE,
  PPS_PRE1,
  PPS_PRE2,
  PPS_LED,
  PPS_USART_ID,
  PPS_USART_SIZE,
  PPS_USART_DATA
};

typedef enum p_parser_state (*p_hook_f) (char ch);

void p_init(void);
void p_parse_byte(char ch);
void p_register_hook(char id, p_hook_f hook);

#endif /* PROTOCOL_H */
