#include "Stream.h"

void voADPCMDecStreamInit(FrameStream *stream)
{
  stream->buffer     = 0;
  stream->inlen     = 0;
  stream->this_frame = 0;
  stream->next_frame = 0;
  stream->storelength = 0;
  stream->length = 0;
  stream->inlen = 0;
}