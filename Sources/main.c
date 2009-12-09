#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/ioctl.h>

#include <Private/mjpeg.h>

#include <PosixThreads/PosixThreads.h>
#include <KahnProcessNetwork/KahnProcessNetwork.h>
#include <SoclibHostAccess/SoclibHostAccess.h>

extern void * fetch_process (void * c);
extern void * idct_process (void * c);
extern void * dispatch_process (void * c);

int main (void)
{
	kpn_channel_t channel[2 * NB_IDCT + 2];
	kpn_channel_t fetch_channel[2 + NB_IDCT];
	kpn_channel_t dispatch_channel[1 + NB_IDCT];
	kpn_channel_t idct_channel[NB_IDCT][2];

	pthread_t fetchThread, dispatchThread, idctThread[NB_IDCT];
	pthread_attr_t fetchAttr, dispatchAttr, idctAttr[NB_IDCT];

  char buffer[128];

	kpn_channel_create ("/devices/fdaccess", 0x10000, & channel[0]);
  ioctl (channel[0] -> fd, FD_OPEN, "movie.mjpeg");

	kpn_channel_create ("/devices/rdv.0", 0, & channel[1]);

	for (uint32_t i = 0; i < 2 * NB_IDCT; i += 1)
  {
	  sprintf(buffer,"/devices/rdv.%lu", i + 1);
		kpn_channel_create (buffer, 0, & channel[i + 2]);
	}
	
	fetch_channel[0] = channel[0];
	fetch_channel[1] = channel[1];
	dispatch_channel[0] = channel[1];

	for (uint32_t i = 0; i < NB_IDCT; i++)
  {
		fetch_channel[i + 2] = channel[2 * i + 2];
		idct_channel[i][0] = channel[2 * i + 2];

		idct_channel[i][1] = channel[2 * i + 3];
		dispatch_channel[i + 1] = channel[2 * i + 3];
	}

	printf ("[MJPEG] %d IDCT\n", NB_IDCT);

	for (uint32_t i = 0; i < NB_IDCT; i++)
  {
		pthread_attr_init (& idctAttr[i]);
	  sprintf(buffer,"idct.%lu", i + 1);
		//  idctAttr[i] . procid = i;
		idctAttr[i] . name = buffer;
  	pthread_create (& idctThread[i], & idctAttr[i], idct_process, idct_channel[i]);
	}

	pthread_attr_init (& fetchAttr);
	//  fetchAttr . procid = 3;
	fetchAttr . name = "fetch";

	pthread_attr_init (& dispatchAttr);
	//	dispatchAttr . procid = 1;
	dispatchAttr . name = "dispatch";

  pthread_create (& fetchThread, & fetchAttr, fetch_process, fetch_channel);
  pthread_create (& dispatchThread, & dispatchAttr, dispatch_process, dispatch_channel);

  pthread_join(dispatchThread, NULL);
	return 0;
}
