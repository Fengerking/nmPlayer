#ifndef _LOOPFILTER_H_
#define _LOOPFILTER_H_

#include "global.h"
#include "mbuffer.h"

extern void DeblockPicture(H264DEC_G *pDecGlobal, StorablePicture *p) ;
//void  init_Deblock(H264DEC_G *pDecGlobal, int mb_aff_frame_flag);
extern void DeblockMb(H264DEC_G *pDecGlobal, H264DEC_L *pDecLocal,StorablePicture *p);
extern void DeblockMb_MBAFF(H264DEC_G *pDecGlobal, H264DEC_L *pDecLocal,StorablePicture *p);
extern void GetStrengthVer         (H264DEC_G *pDecGlobal,int b_slice,int is_intra,VO_U8* Strength, int edge, int mvlimit, StorablePicture *p, int MbQAddr);
extern void GetStrengthHor         (H264DEC_G *pDecGlobal,int b_slice,int is_intra,VO_U8* Strength, int edge, int mvlimit, StorablePicture *p, int MbQAddr);

extern VO_S32 update_deblock_cache(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mbaddr,VO_S32 mb_x);

#endif //_LOOPFILTER_H_
