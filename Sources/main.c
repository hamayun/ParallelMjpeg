#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include <Private/Decoder.h>

#include <PosixThreads/PosixThreads.h>
#include <KahnProcessNetwork/KahnProcessNetwork.h>
#include <SoclibHostAccess/SoclibHostAccess.h>
#include <SoclibFramebuffer/SoclibFramebuffer.h>

int main (void)
{
	kpn_channel_t decoder[1][2];
	kpn_channel_t dispatcher[2];

  /*
   * Create a channel connected to the input movie
   */

	kpn_channel_create ("/devices/fdaccess", 0x10000, & dispatcher[0]);
  ioctl (dispatcher[0] -> fd, FD_OPEN, "movie.mjpeg");

  /*
   * Create a channel connected to the framebuffer 
   */

	kpn_channel_create ("/devices/framebuffer.0", 0, & output);
  ioctl (output -> fd, FB_SET_AUTOREWIND, (void *) true);

  /*
   * The actual computation
   */

  decoder (input, output);
	return 0;
}
