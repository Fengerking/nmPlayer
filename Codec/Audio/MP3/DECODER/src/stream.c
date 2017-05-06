# include "config.h"
# include "global.h"
# include "stream.h"

void voMP3DecStreamInit(FrameStream *stream)
{
  stream->buffer     = 0;
  stream->inlen     = 0;
  stream->this_frame = 0;
  stream->next_frame = 0;
  stream->main_data  = 0;
  stream->md_len     = 0;
  stream->free_bitrates = 0;
  stream->storelength = 0;
  stream->length = 0;
  stream->inlen = 0;

  voMP3DecInitBits(&stream->bitptr, 0, 0);

}


