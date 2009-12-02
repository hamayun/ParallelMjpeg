/*
 * Copyright (C) 2007 TIMA Laboratory
 * Author(s) :      Patrice GERIN patrice.gerin@imag.fr
 * Bug Fixer(s) :   Xavier GUERIN xavier.guerin@imag.fr
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef MJPEG_BUFFER
#define MJPEG_BUFFER

#include <stdint.h>

typedef struct _buffer
{
  int16_t fd;
  int32_t size;
  int32_t level;
  int32_t position;
  uint8_t * data;
}
* buffer_t;

extern buffer_t buffer_create (int16_t fd, int32_t size);
extern void buffer_destroy (buffer_t buffer);

extern void buffer_read (buffer_t buffer, void * data, int32_t size);
extern void buffer_write (buffer_t buffer, void * data, int32_t size);
extern void buffer_skip (buffer_t buffer, int32_t size);

#endif
