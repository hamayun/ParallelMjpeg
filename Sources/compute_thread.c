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

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/times.h>

#include <Private/ComputeThread.h>
#include <Core/Core.h>

int32_t compute_thread (kpn_channel_t c[2])
{
	uint8_t * Idct_YCbCr;
	int32_t * block_YCbCr;
	uint32_t flit_size = 0;

#ifdef COMPUTE_TIME
  int32_t id;
  thread_info_t info;
  clock_t c_start, c_end;
#endif

	kpn_channel_read (c[0], (uint8_t *) & flit_size, sizeof (uint32_t));
	VPRINTF("Flit size = %lu\r\n", flit_size);

	Idct_YCbCr = (uint8_t *) malloc (flit_size * 64 * sizeof (uint8_t));
	if (Idct_YCbCr == NULL)
  {
    printf ("%s,%d: malloc failed\n", __FILE__, __LINE__);
  }

	block_YCbCr = (int32_t *) malloc (flit_size * 64 * sizeof (int32_t));
	if (block_YCbCr == NULL)
  {
    printf ("%s,%d: malloc failed\n", __FILE__, __LINE__);
  }

#ifdef COMPUTE_TIME
  thread_find (NULL, & id);
#endif

	while (1)
  {
#ifdef COMPUTE_TIME
    c_start = times (NULL);
#endif

		kpn_channel_read (c[0], (unsigned char *) block_YCbCr,
        flit_size * 64 * sizeof (int32_t));

#ifdef COMPUTE_TIME
    c_end = times (NULL);
    thread_get_info (id, & info);
    printf ("(%ld, %ld) READ = %ld\r\n", id, info . cpu_id, c_end - c_start);
#endif

		for (uint32_t i = 0; i < flit_size; i++)
    {
      IDCT(& block_YCbCr[i * 64], & Idct_YCbCr[i * 64]);
    }

#ifdef COMPUTE_TIME
    c_start = times (NULL);
#endif

		kpn_channel_write (c[1], (unsigned char *) Idct_YCbCr,
        flit_size * 64 * sizeof (uint8_t));

#ifdef COMPUTE_TIME
    c_end = times (NULL);
    thread_get_info (id, & info);
    printf ("(%ld, %ld) WRITE = %ld\r\n", id, info . cpu_id, c_end - c_start);
#endif
	}

	return 0;
}

