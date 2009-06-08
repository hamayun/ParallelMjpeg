#ifndef MJPEG_BUFFER
#define MJPEG_BUFFER

#include <stdint.h>

typedef struct _buffer
{
  int16_t fd;
  int32_t size;
  int32_t level;
  int32_t position;
  int8_t * data;
}
* buffer_t;

extern buffer_t buffer_create (int16_t fd, int32_t size);
extern void buffer_destroy (buffer_t buffer);

extern void buffer_read (buffer_t buffer, void * data, int32_t size);
extern void buffer_write (buffer_t buffer, void * data, int32_t size);

#endif
