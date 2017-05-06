#ifndef _H264DECODER_H_
#define _H264DECODER_H_

#include "global.h"

typedef enum
{
  DEC_GEN_NOERR = 0,
  DEC_OPEN_NOERR = 0,
  DEC_CLOSE_NOERR = 0,  
  DEC_SUCCEED = 0,
  DEC_EOS =1,
  DEC_NEED_DATA = 2,
  DEC_INVALID_PARAM = 3,
  DEC_FRAME = 4,
  DEC_ERRMASK = 0x8000
//  DEC_ERRMASK = 0x80000000
}DecErrCode;

typedef struct dec_set_t
{
  int iPostprocLevel; // valid interval are [0..100]
  int bDBEnable;
  int bAllLayers;
  int time_incr;
	int bDecCompAdapt;
} DecSet_t;

#ifdef __cplusplus
extern "C" {
#endif

int OpenDecoder(DecoderParams ** phDec, VO_CODEC_INIT_USERDATA * pUserData);
int DecodeOneFrame(VO_HANDLE phDec);
VO_S32 voH264DecProcessNALU(VO_HANDLE phDec);
VO_S32 voH264DecProcessAnnexB(VO_HANDLE phDec);
int FinitDecoder(VO_HANDLE phDec);
int CloseDecoder(VO_HANDLE phDec);
int SetOptsDecoder(DecSet_t *pDecOpts);

#ifdef __cplusplus
}
#endif
#endif
