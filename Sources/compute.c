/*************************************************************************************
 * File   : idct.c, file for JPEG-JFIF Multi-thread decoder    
 *
 * Copyright (C) 2007 TIMA Laboratory
 * Author(s) :      Alexandre Chagoya-Garzon 
 * Bug Fixer(s) :   
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 *************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/times.h>

#include <Private/compute.h>
#include <Private/mjpeg.h>
#include <Private/utils.h>

#include <Core/Core.h>
#include <KahnProcessNetwork/KahnProcessNetwork.h>

int idct_process (Channel * c[2]) {
	uint8_t * Idct_YCbCr;
	int32_t * block_YCbCr;
	uint32_t flit_size = 0;

#ifdef TIME
  int32_t thread_id;
  clock_t c_start, c_end;
  struct tms time_start, time_end;
#endif

	channelRead (c[0], (uint8_t *) & flit_size, sizeof (uint32_t));
	VPRINTF("Flit size = %lu\r\n", flit_size);

	Idct_YCbCr = (uint8_t *) malloc (flit_size * 64 * sizeof (uint8_t));
	if (Idct_YCbCr == NULL) printf ("%s,%d: malloc failed\n", __FILE__, __LINE__);

	block_YCbCr = (int32_t *) malloc (flit_size * 64 * sizeof (int32_t));
	if (block_YCbCr == NULL) printf ("%s,%d: malloc failed\n", __FILE__, __LINE__);

#ifdef TIME
  thread_find (NULL, & thread_id);
#endif

	while (1)
  {
#ifdef TIME
    c_start = times (& time_start);
#endif

		channelRead (c[0], (unsigned char *) block_YCbCr, flit_size * 64 * sizeof (int32_t));

#ifdef TIME
    c_end = times (& time_end);
    printf ("[Compute(%ld)] READ = %ld ms(abs), %ld ns(rel)\r\n", thread_id,
        (uint32_t)(c_end - c_start),
        (uint32_t)(time_end . tms_stime - time_start . tms_stime));

    c_start = times (& time_start);
#endif

		for (uint32_t i = 0; i < flit_size; i++)
    {
      IDCT(& block_YCbCr[i * 64], & Idct_YCbCr[i * 64]);
    }

#ifdef TIME
    c_end = times (& time_end);
    printf ("[Compute(%ld)] IDCT = %ld ms(abs), %ld ns(rel)\r\n", thread_id,
        (uint32_t)(c_end - c_start),
        (uint32_t)(time_end . tms_stime - time_start . tms_stime));

    c_start = times (& time_start);
#endif

		channelWrite (c[1], (unsigned char *) Idct_YCbCr, flit_size * 64 * sizeof (uint8_t));

#ifdef TIME
    c_end = times (& time_end);
    printf ("[Compute(%ld)] WRITE = %ld ms(abs), %ld ns(rel)\r\n", thread_id,
        (uint32_t)(c_end - c_start),
        (uint32_t)(time_end . tms_stime - time_start . tms_stime));
#endif
	}

	return 0;
}

