

#include "global.h"
//#include "mbuffer.h"
#include "mb_access.h"


Boolean inline mb_is_available(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int mbAddr)
{
  return ((mbAddr >= 0) /*&& (mbAddr <= ((int)pDecGlobal->PicSizeInMbs - 1))*/&&(pDecLocal->p_Slice->start_mb_nr <= mbAddr));
}

Boolean inline mb_is_available_mbaff(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int mbAddr)
{
  return ((mbAddr >= 0) /*&& (mbAddr <= ((int)pDecGlobal->PicSizeInMbs - 1))*/&&(pDecLocal->p_Slice->start_mb_nr_mbaff <= mbAddr));
}






