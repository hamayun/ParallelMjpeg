/*
 * Copyright (C) 2007 TIMA Laboratory
 *
 * Author(s):
 *   Patrice GERIN, patrice.gerin@imag.fr
 *   Xavier GUERIN, xavier.guerin@imag.fr
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
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <string.h>
#include <stdbool.h>
#include <malloc.h>

#include <Private/Dispatcher.h>

/*
 * The dispatcher function
 */

int32_t dispatcher (kpn_channel_t c[2])
{
	uint8_t marker0, marker1, data;
  bool keep_looking = true, soi_started = false;
  uint8_t * buffer = NULL;
  uint32_t section_size = 0;

  /*
   * Allocate an arbitrary sized buffer
   */

  buffer = malloc (0x8000);

  /*
   * Parse the stream
   */

  while (true)
  {
    kpn_channel_read (c[0], & marker0, 1);

    if (marker0 == M_SMS)
    {
      kpn_channel_read (c[0], & marker1, 1);

      switch (marker1)
      {
        case M_SOI :
          buffer[0] = marker0;
          buffer[1] = marker1;

          section_size = 2;
          break;

        case M_EOI :
          buffer[section_size++] = marker0;
          buffer[section_size++] = marker1;

          section_size += 2;
          printf ("Section size is %d", section_size);
          kpn_channel_write (c[1], buffer, 0x8000);

          section_size = 0;
          soi_started = false;
          break;

        default:
          buffer[section_size++] = marker1;
          break;
      }
    }
    else if (soi_started)
    {
      buffer[section_size++] = marker0;
    }
  }
}

