/*
   Bitstream encoder
 */
#ifndef _BITENC_H
#define _BITENC_H

#include "qc_data.h"
#include "tns.h"
#include "channel_map.h"
#include "interface.h"  

struct BITSTREAMENCODER_INIT
{
  Word16 nChannels;
  Word32 bitrate;
  Word32 sampleRate;
  Word16 profile;
};



Word16 WriteBitstream (HANDLE_BIT_BUF hBitstream,
                       ELEMENT_INFO elInfo,
                       QC_OUT *qcOut,
                       PSY_OUT *psyOut,
                       Word16 *globUsedBits,
                       const UWord8 *ancBytes,
					   Word16 samplerate
                       );

#endif /* _BITENC_H */
