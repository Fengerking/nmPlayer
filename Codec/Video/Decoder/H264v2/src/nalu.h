


#ifndef _NALU_H_
#define _NALU_H_

#include "nalucommon.h"

typedef struct sBitsFile
{
  void (*OpenBitsFile)    (H264DEC_G *pDecGlobal, char *filename);
  void (*CloseBitsFile)   (H264DEC_G *pDecGlobal);
  int  (*GetNALU)         (H264DEC_G *pDecGlobal, NALU_t *nalu);
} BitsFile;
extern void CheckZeroByteNonVCL(H264DEC_G *pDecGlobal, NALU_t *nalu);
extern void CheckZeroByteVCL   (H264DEC_G *pDecGlobal, NALU_t *nalu);

//extern int read_next_nalu(H264DEC_G *pDecGlobal, NALU_t *nalu);

#endif
