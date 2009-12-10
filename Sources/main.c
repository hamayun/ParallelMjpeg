#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/ioctl.h>

#include <Private/Mjpeg.h>
#include <Private/FetchThread.h>
#include <Private/ComputeThread.h>
#include <Private/DispatchThread.h>

#include <PosixThreads/PosixThreads.h>
#include <KahnProcessNetwork/KahnProcessNetwork.h>
#include <SoclibHostAccess/SoclibHostAccess.h>
#include <SoclibFramebuffer/SoclibFramebuffer.h>

int main (void)
{
  char buffer[128];

	kpn_channel_t channel[2 * NB_IDCT + 3];
	kpn_channel_t fetch_channel[2 + NB_IDCT];
	kpn_channel_t compute_channel[NB_IDCT][2];
	kpn_channel_t dispatch_channel[2 + NB_IDCT];

	pthread_t fetchThread, dispatchThread, computeThread[NB_IDCT];
	pthread_attr_t fetchAttr, dispatchAttr, computeAttr[NB_IDCT];

  /*
   * Create a channel connected to the input movie
   */

	kpn_channel_create ("/devices/fdaccess", 0x10000, & channel[0]);
  ioctl (channel[0] -> fd, FD_OPEN, "movie.mjpeg");

  /*
   * Create a channel connected to the framebuffer 
   */

	kpn_channel_create ("/devices/framebuffer.0", 0, & channel[1]);
  ioctl (channel[1] -> fd, FB_SET_AUTOREWIND, (void *) true);

  /*
   * Create the first FETCH -> DISPATCH channel
   */

	kpn_channel_create ("/devices/rdv.0", 0, & channel[2]);

  /*
   * Create the FETCH -> IDCT and IDCT -> DISPATCH channels
   */

	for (uint32_t i = 0; i < 2 * NB_IDCT; i += 1)
  {
	  sprintf(buffer,"/devices/rdv.%lu", i + 1);
		kpn_channel_create (buffer, 0, & channel[i + 3]);
	}

  /*
   * Connect the channels altogether
   */
	
	fetch_channel[0] = channel[0];
	fetch_channel[1] = channel[2];
	dispatch_channel[0] = channel[2];
	dispatch_channel[1] = channel[1];

	for (uint32_t i = 0; i < NB_IDCT; i++)
  {
		fetch_channel[i + 2] = channel[2 * i + 3];
		compute_channel[i][0] = channel[2 * i + 3];

		compute_channel[i][1] = channel[2 * i + 4];
		dispatch_channel[i + 2] = channel[2 * i + 4];
	}

  /*
   * Starting the MJPEG application
   */

	printf ("[MJPEG] %d IDCT\n", NB_IDCT);

  /*
   * Create the IDCT threads
   */

	for (uint32_t i = 0; i < NB_IDCT; i++)
  {
		pthread_attr_init (& computeAttr[i]);

	  sprintf(buffer,"compute.%lu", i + 1);
		computeAttr[i] . name = buffer;

#if 0 /* Enable to set a particular processor affinity */
		computeAttr[i] . procid = i;
#endif

  	pthread_create (& computeThread[i], & computeAttr[i],
        (pthread_func_t) compute_thread, compute_channel[i]);
	}

  /*
   *  Create the FETCH thread
   */

	pthread_attr_init (& fetchAttr);
	fetchAttr . name = "fetch";

#if 0 /* Enable to set a particular processor affinity */
	fetchAttr . procid = 3;
#endif

  pthread_create (& fetchThread, & fetchAttr, 
      (pthread_func_t) fetch_thread, fetch_channel);

  /*
   * Create the DISPATCH thread
   */

	pthread_attr_init (& dispatchAttr);
	dispatchAttr . name = "dispatch";

#if 0 /* Enable to set a particular processor affinity */
	dispatchAttr . procid = 1;
#endif

  pthread_create (& dispatchThread, & dispatchAttr,
      (pthread_func_t) dispatch_thread, dispatch_channel);

  /*
   * Join with the created threads
   */

	for (uint32_t i = 0; i < NB_IDCT; i++)
  {
  	pthread_join (computeThread[i], NULL);
	}

  pthread_join (fetchThread, NULL);
  pthread_join (dispatchThread, NULL);

	return 0;
}
