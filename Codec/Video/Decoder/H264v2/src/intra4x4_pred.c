#include "global.h"
#include "intra4x4_pred.h"
#include "mb_access.h"
#include "image.h"


extern int intra4x4_pred_normal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int ioff, int joff);


int intrapred(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,    //!< current macroblock
              int ioff,              
              int joff)       
{
  return intra4x4_pred_normal(pDecGlobal,pDecLocal, ioff, joff);      
}
