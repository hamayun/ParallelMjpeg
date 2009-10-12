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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <Private/buffer.h>

#include <DnaTools/DnaTools.h>

buffer_t buffer_create (int16_t fd, int32_t size)
{
  buffer_t buffer = malloc (sizeof (struct _buffer));

  buffer -> fd = fd;
  buffer -> size = size;
  buffer -> level = 0;
  buffer -> position = 0;
  buffer -> data = malloc (size + 1);

  return buffer;
}

void buffer_destroy (buffer_t buffer)
{
  free (buffer -> data);
  free (buffer);
}

void buffer_read (buffer_t buffer, void * data, int32_t size)
{
  int32_t counter = size, res = 0;

  while (counter != 0)
  {
    if (counter <= buffer -> level)
    {
      memcpy (data, (void *)(buffer -> data + buffer -> position), counter);
      buffer -> level -= counter;
      buffer -> position += counter;
      counter = 0;
    }
    else
    {
      if (buffer -> level != 0)
      {
        memcpy (data, (void *)(buffer -> data +
              buffer -> position), buffer -> level);
        counter -= buffer -> level;
      }

      buffer -> data[buffer -> size] = 0xDC;
      res = read (buffer -> fd, buffer -> data, buffer -> size);

      buffer -> level = buffer -> size;
      buffer -> position = 0;
    }
  }
}

void buffer_write (buffer_t buffer, void * data, int32_t size)
{

}
