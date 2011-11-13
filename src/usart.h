/*
 * This file is part of usb io tester firmware.
 *
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

#ifndef USART_H
#define USART_H

typedef void (*usart_receive_callback)(char *data, int size);

void usart_init(void);
void usart2_register_receive_callback(usart_receive_callback callback);
int usart2_send(char *data, int size);

#endif /* USART_H */
