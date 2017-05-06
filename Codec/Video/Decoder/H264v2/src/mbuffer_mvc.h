

#ifndef _MBUFFER_MVC_H_
#define _MBUFFER_MVC_H_

#include "global.h"

#if (MVC_EXTENSION_ENABLE)
extern void reorder_lists_mvc     (H264DEC_G *pDecGlobal,Slice * currSlice, int currPOC);
extern void init_lists_mvc        (H264DEC_G *pDecGlobal,Slice *currSlice);
extern void init_lists_p_slice_mvc(H264DEC_G *pDecGlobal,Slice *currSlice);
extern void init_lists_b_slice_mvc(H264DEC_G *pDecGlobal,Slice *currSlice);
extern void init_lists_i_slice_mvc(H264DEC_G *pDecGlobal,Slice *currSlice);

extern void reorder_ref_pic_list_mvc(H264DEC_G *pDecGlobal,Slice *currSlice, int cur_list, int **anchor_ref, int **non_anchor_ref,
                                                 int view_id, int anchor_pic_flag, int currPOC, int listidx);

extern VO_S32 reorder_short_term(H264DEC_G *pDecGlobal,Slice *currSlice, int cur_list, int num_ref_idx_lX_active_minus1, int picNumLX, int *refIdxLX, int currViewID);
extern VO_S32 reorder_long_term(H264DEC_G *pDecGlobal,Slice *currSlice, StorablePicture **RefPicListX, int num_ref_idx_lX_active_minus1, int LongTermPicNum, int *refIdxLX, int currViewID);
#endif

#endif

