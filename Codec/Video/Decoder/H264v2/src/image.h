#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "mbuffer.h"
//extern void update_cache(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Slice *currSlice);

extern int  picture_order     ( Slice *pSlice );

extern VO_S32 decode_one_slice  (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,Slice *currSlice);
extern int  read_new_slice    (H264DEC_G *pDecGlobal,Slice *currSlice);
extern int  process_one_nalu    (H264DEC_G *pDecGlobal,Slice *currSlice);
extern int process_first_sps(H264DEC_G *pDecGlobal);

extern VO_S32 exit_picture      (H264DEC_G *pDecGlobal, StorablePicture **dec_picture);
extern VO_S32  decode_one_frame  (DecoderParams *pDecoder);

//extern int  is_new_picture(H264DEC_G *pDecGlobal,StorablePicture *dec_picture, Slice *currSlice, OldSliceParams *p_old_slice);
//extern void init_old_slice(OldSliceParams *p_old_slice);
// For 4:4:4 independent mode
//extern void copy_dec_picture_JV ( StorablePicture *dst, StorablePicture *src );


#if (MVC_EXTENSION_ENABLE)
extern int GetVOIdx( H264DEC_G *pDecGlobal,int iViewId);
extern int get_maxViewIdx( H264DEC_G *pDecGlobal,int view_id, int anchor_pic_flag, int listidx);
#endif

extern VO_S32 init_slice(H264DEC_G *pDecGlobal, Slice *currSlice);
extern VO_S32 decode_slice(H264DEC_G *pDecGlobal,Slice *currSlice);



#endif

