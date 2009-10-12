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

#include <Private/mjpeg.h>
#include <Private/dispatch.h>
#include <Private/utils.h>

#include <PosixThreads/PosixThreads.h>
#include <KahnProcessNetwork/KahnProcessNetwork.h>

int dispatch_process (Channel * c[NB_IDCT + 1])
{
	uint8_t * MCU_YCbCr = NULL, * picture = NULL;
	uint8_t * CELLS = NULL, * Y_SRC = NULL, * Y_DST = NULL;
	uint8_t * U_SRC = NULL, * U_DST = NULL;
	uint8_t * V_SRC = NULL, * V_DST = NULL;
	uint8_t * uv_line_src = NULL, * uv_line_dst = NULL;
	uint8_t idct_index = 0;

	uint16_t NB_MCU_X = 0, NB_MCU_Y = 0;
	uint16_t NB_CELLS = 0, NB_MCU = 0;

	uint32_t flit_bytes = 0,  flit_size = 0;
	uint32_t YV = 0, YH = 0, LB_X = 0, LB_Y = 0;
	uint32_t * y_line_dst = NULL, * y_line_src = NULL;

#ifdef PROGRESS
	char progress_tab[4] = {'/', '-', '\\', '|'};
	uint32_t imageCount = 1;
	uint32_t block_index = 0;
#endif

#ifdef DISPATCH_TIME
  clock_t c_start, c_end;
  struct tms time_start, time_end;
#endif

	SOF_section_t SOF_section;
  FILE * fb;

  fb = fopen ("/devices/framebuffer.0", "r+");
  if (fb == NULL)
  {
    printf ("error: cannot open framebuffer.\r\n");
    pthread_exit (NULL);
  }

	channelRead (c[0], (unsigned char*) & SOF_section, sizeof (SOF_section));
	channelRead (c[0], (unsigned char*) & YV, sizeof (uint32_t));
	channelRead (c[0], (unsigned char*) & YH, sizeof (uint32_t));
	channelRead (c[0], (unsigned char*) & flit_size, sizeof (uint32_t));

	flit_bytes = flit_size * MCU_sx * MCU_sy * sizeof (uint8_t);

	NB_MCU_X = intceil(SOF_section.width, MCU_sx);
	NB_MCU_Y = intceil(SOF_section.height, MCU_sy);
	NB_CELLS = YV * YH + 2;

	picture = (uint8_t *) malloc (SOF_section . width * SOF_section . height * 2);
	if (picture == NULL)
  {
    printf ("%s,%d: malloc failed\n", __FILE__, __LINE__);
  }

	MCU_YCbCr = (uint8_t *) malloc(flit_bytes);
	if (MCU_YCbCr == NULL)
  {
    printf ("%s,%d: malloc failed\n", __FILE__, __LINE__);
  }

	NB_MCU = NB_MCU_Y * NB_MCU_X;

#ifdef PROGRESS
	printf ("Image %lu : |", imageCount++);
	fflush (stdout);
#endif

#ifdef DISPATCH_TIME
  c_start = times (& time_start);
#endif

	while (1)
  {
		channelRead (c[idct_index + 1], MCU_YCbCr, flit_bytes);
		idct_index = (idct_index + 1) % NB_IDCT;

		for (int flit_index = 0; flit_index < flit_size; flit_index += NB_CELLS)
    {
			CELLS = MCU_INDEX(MCU_YCbCr, flit_index);

			for (int cell_y_index = 0; cell_y_index < YV; cell_y_index += 1)
      {
				for (int cell_x_index = 0; cell_x_index < YH; cell_x_index += 1)
        {
					Y_SRC = MCU_INDEX(CELLS, (YH * cell_y_index + cell_x_index));
					Y_DST = FB_Y_INDEX(picture, LB_X + cell_x_index, LB_Y + cell_y_index);

					for (int line_index = 0; line_index < MCU_sy; line_index += 1)
          {
						y_line_src = (uint32_t *) MCU_LINE(Y_SRC, line_index);
						y_line_dst = (uint32_t *) FB_Y_LINE(Y_DST, line_index);
						*y_line_dst++ = *y_line_src++; *y_line_dst++ = *y_line_src++;
					}
				}

				U_SRC = MCU_INDEX(CELLS, (YH * YV));
				U_DST = FB_U_INDEX(picture, LB_X, LB_Y + cell_y_index);
				
				for (int line_index = 0; line_index < MCU_sy; line_index += 1)
        {
					uv_line_src = MCU_LINE(U_SRC, line_index);
					uv_line_dst = FB_UV_LINE(U_DST, line_index);

					for (int i = 0; i < (MCU_sx / (3 - YH)); i += 1)
          {
            uv_line_dst[i] = uv_line_src[i * (3 - YH)];
          }
				}

				V_SRC = MCU_INDEX(CELLS, (YH * YV + 1));
				V_DST = FB_V_INDEX(picture, LB_X, LB_Y + cell_y_index);
				
				for (int line_index = 0; line_index < MCU_sy; line_index += 1)
        {
					uv_line_src = MCU_LINE(V_SRC, line_index);
					uv_line_dst = FB_UV_LINE(V_DST, line_index);

					for (int i = 0; i < (MCU_sx / (3 - YH)); i += 1)
          {
            uv_line_dst[i] = uv_line_src[i * (3 - YH)];
          }
				}
			}

			LB_X = (LB_X + YH) % NB_MCU_X;

#ifdef PROGRESS
			fputs ("\033[1D", stdout);
			putchar (progress_tab[block_index++ % 4]);
			fflush (stdout);
#endif

			if (LB_X == 0)
      {
				LB_Y = (LB_Y + YV) % NB_MCU_Y;

				if (LB_Y == 0) 
        {
          rewind (fb);
          fwrite (picture, SOF_section . width *
              SOF_section . height * 2, 1, fb);
          fflush (fb);

#ifdef PROGRESS
          puts ("\033[1Ddone");
#endif

#ifdef DISPATCH_TIME
          c_end = times (& time_end);
          printf ("[Dispatch time] %ld ns\r\n",
              (uint32_t)(time_end . tms_stime - time_start . tms_stime));
          printf ("[Total time] %ld us\r\n", (uint32_t)(c_end - c_start));
          c_start = c_end;
#endif

#ifdef PROGRESS
          printf ("Image %lu : |", imageCount++);
          fflush (stdout);
#endif
        }
			}
		}
	}

	return 0;
}
