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

#ifndef CDCACM_H
#define CDCACM_H

typedef void (*cdcacm_receive_callback) (char *data, int size);

void cdcacm_init();
void cdcacm_run();
void cdcacm_register_receive_callback(cdcacm_receive_callback callback);
void cdcacm_send(char *data, int size);

#endif /* CDCACM_H */
