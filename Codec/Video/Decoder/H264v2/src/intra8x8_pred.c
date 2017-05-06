#include "global.h"
#include "intra8x8_pred.h"
#include "mb_access.h"
#include "image.h"


extern int intrapred8x8_normal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int ioff, int joff);

int intrapred8x8(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int ioff, int joff)              

{  
  return intrapred8x8_normal(pDecGlobal,pDecLocal, ioff, joff);
}


