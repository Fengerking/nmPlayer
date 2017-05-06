#include <stdlib.h>
#include "config.h"
#include "global.h"
#include "stream.h"

void StreamInit(FrameStream *stream)
{
  stream->buffer     = 0;
  stream->inlen     = 0;
  stream->this_frame = 0;
  stream->next_frame = 0;
  stream->storelength = 0;
  stream->length = 0;
  stream->inlen = 0;
 
  InitBits(&stream->bitptr, 0, 0);
}

