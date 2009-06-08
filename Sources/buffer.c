#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <Private/buffer.h>

#include <VirtualFileSystem/VirtualFileSystem.h>

buffer_t buffer_create (int16_t fd, int32_t size)
{
  buffer_t buffer = malloc (sizeof (struct _buffer));

  buffer -> fd = fd;
  buffer -> size = size;
  buffer -> level = 0;
  buffer -> position = 0;
  buffer -> data = malloc (size);

  return buffer;
}

void buffer_destroy (buffer_t buffer)
{
  free (buffer -> data);
  free (buffer);
}

void buffer_read (buffer_t buffer, void * data, int32_t size)
{
  int32_t counter = size, chunk = 0;

  while (counter != 0)
  {
    if (counter <= buffer -> level)
    {
      memcpy (data, (void *)(buffer -> data + buffer -> position), size);
      buffer -> level -= size;
      buffer -> position += size;
      counter -= size;
    }
    else
    {
      if (buffer -> level != 0)
      {
        chunk = counter - buffer -> level;
        memcpy (data, (void *)(buffer -> data + buffer -> position), chunk);
      }

      read (buffer -> fd, buffer -> data, buffer -> size);
      buffer -> level = buffer -> size;
      buffer -> position = 0;

      counter -= chunk;
    }
  }
}

void buffer_write (buffer_t buffer, void * data, int32_t size)
{

}
