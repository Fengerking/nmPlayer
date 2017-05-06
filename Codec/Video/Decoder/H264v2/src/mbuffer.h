#ifndef _MBUFFER_H_
#define _MBUFFER_H_

#include "global.h"


//! Decoded Picture Buffer
typedef struct decoded_picture_buffer
{
  //VideoParameters *p_Vid;
  FrameStore  **fs;
  FrameStore  **fs_ref;
  FrameStore  **fs_ltref;
  struct storable_picture **p_remove;
  unsigned      size;
  unsigned      used_size;
  unsigned      ref_frames_in_buffer;
  unsigned      ltref_frames_in_buffer;
  VO_U32      remove_size;
  int           last_output_poc;
  VO_U32        used_ref_index;

  int           max_long_term_pic_idx;  


  int           init_done;
  int           num_ref_frames;
  struct storable_picture *storable_pic_buffer[3];
  struct storable_picture **ref_pic_buffer;
  struct storable_picture *direct_out;
   VO_S8* ref_idx_buffer;
   VO_S32* mv_buffer;
  VO_U8* imgY_buffer;
  VO_U8* imgUV_buffer[2];
  VO_S32* mb_type_buffer;
  FrameStore   *last_picture;
  
} DecodedPictureBuffer;
VO_U32 FrameBufCtl(FIFOTYPE *priv, VO_U32 img_index , const VO_U32 flag);
extern void              init_dpb(H264DEC_G *pDecGlobal, DecodedPictureBuffer *p_Dpb);
//extern void              re_init_dpb(H264DEC_G *pDecGlobal, DecodedPictureBuffer *p_Dpb);
extern void              free_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb);
extern FrameStore*       alloc_frame_store(H264DEC_G *pDecGlobal);
extern void              free_frame_store (H264DEC_G *pDecGlobal,FrameStore* f);
extern StorablePicture*  alloc_storable_picture(H264DEC_G *pDecGlobal, PictureStructure type);
extern void              free_storable_picture (H264DEC_G *pDecGlobal,StorablePicture* p);
extern VO_S32 store_picture_in_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb, StorablePicture* p);
extern StorablePicture*  get_short_term_pic (H264DEC_G *pDecGlobal,Slice *p_Dpb, int picNum);
extern StorablePicture*  get_long_term_pic  (H264DEC_G *pDecGlobal,Slice *p_Dpb, int LongtermPicNum);

#if (MVC_EXTENSION_ENABLE)
extern VO_S32             flush_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb, int curr_view_id);
extern int              GetMaxDecFrameBuffering(H264DEC_G *pDecGlobal);
extern void             append_interview_list(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb, 
                                              PictureStructure currPicStructure, 
                                              int list_idx, 
                                              FrameStore **list,
                                              int *listXsize, 
                                              int currPOC, 
                                              int curr_view_id, 
                                              int anchor_pic_flag);
extern void             update_ref_list(DecodedPictureBuffer *p_Dpb, int curr_view_id);
extern void             update_ltref_list(DecodedPictureBuffer *p_Dpb, int curr_view_id);
#else
extern void             update_ref_list(DecodedPictureBuffer *p_Dpb);
extern void             update_ltref_list(DecodedPictureBuffer *p_Dpb);
extern VO_S32             flush_dpb(H264DEC_G *pDecGlobal,DecodedPictureBuffer *p_Dpb);
#endif
extern void             init_lists(H264DEC_G *pDecGlobal,Slice *currSlice);
extern void             init_lists_p_slice(H264DEC_G *pDecGlobal,Slice *currSlice);
extern void             init_lists_b_slice(H264DEC_G *pDecGlobal,Slice *currSlice);
extern void             init_lists_i_slice(H264DEC_G *pDecGlobal,Slice *currSlice);
extern void             update_pic_num    (H264DEC_G *pDecGlobal,Slice *currSlice);

extern void             dpb_split_field  (H264DEC_G *pDecGlobal, FrameStore *fs);
//extern void             dpb_combine_field(H264DEC_G *pDecGlobal, FrameStore *fs);
extern void             dpb_combine_field_yuv(H264DEC_G *pDecGlobal, FrameStore *fs);

extern VO_S32             reorder_ref_pic_list(H264DEC_G *pDecGlobal,Slice *currSlice, int cur_list);

extern VO_S32   init_mbaff_lists(H264DEC_G *pDecGlobal, Slice *currSlice);
extern void             alloc_ref_pic_list_reordering_buffer(H264DEC_G *pDecGlobal,Slice *currSlice);
extern void             free_ref_pic_list_reordering_buffer(H264DEC_G *pDecGlobal,Slice *currSlice);

//extern void             fill_frame_num_gap(H264DEC_G *pDecGlobal, Slice *pSlice);

extern VO_S32 compute_colocated (H264DEC_G *pDecGlobal,Slice *currSlice, StorablePicture **listX[6]);


extern void gen_pic_list_from_frame_list(PictureStructure currStructure, FrameStore **fs_list, int list_idx, StorablePicture **list, char *list_size, int long_term);

#if 1 //USE_FRAME_THREAD
extern void inline split_4x4(struct storable_picture *field, struct storable_picture *frame, VO_S32 idx0, VO_S32 idx1, VO_S32 list_idx);
extern void inline combine_4x4(struct storable_picture *frame, struct storable_picture *field0, struct storable_picture *field1, VO_S32 idx0, VO_S32 idx1);
extern void combine_mb(struct storable_picture *frame, VO_S32 mb_x, VO_S32 mb_y, VO_S32 mbWidth);

#endif

#endif

