/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#include <math.h>

#include "voH264EncGlobal.h"
#include "voH264EncBitStream.h"

//#define BitUE BitUEBig


void IniSPS(H264ENC *pEncGlobal, OUT_PARAM *InternalParam )
{
  pEncGlobal->log2_max_frame_num = 4;  /* at least 4 */
  while( (1 << pEncGlobal->log2_max_frame_num) <= InternalParam->max_iframe_interval && pEncGlobal->log2_max_frame_num < 10 )
        pEncGlobal->log2_max_frame_num++;
  pEncGlobal->log2_max_frame_num++;
  pEncGlobal->log2_max_poc_lsb = pEncGlobal->log2_max_frame_num + 1;    /* max poc = 2*nGFrameNum */
  //YU_TBD
  IniLevel( pEncGlobal, &pEncGlobal->level);
}


void WriteSPS(H264ENC *pEncGlobal, BS_TYPE *s)
{
  RealignBits( s );
  PutBits( s, 8, 66 );//sps->i_profile_idc // PROFILE_BASELINE
  PutBits( s, 8, 192 );//b_constraint_set0:1,b_constraint_set1:1,b_constraint_set2:0,reserved:0(5)
  PutBits( s, 8, pEncGlobal->level->level_idc );//>i_level_idc
  UEBits( s, 0);//i_id 
  LargeUEBits( s, pEncGlobal->log2_max_frame_num - 4  );
  UEBits( s, 0 );//poc_type == 0
  LargeUEBits( s, pEncGlobal->log2_max_poc_lsb - 4  );
  UEBits( s,1);//i_num_ref_frame
  PutBits( s, 1, 0);//b_gaps_in_frame_num_value_allowed
  UEBits( s, pEncGlobal->mb_width - 1 );
  UEBits( s, pEncGlobal->mb_height - 1);
  PutBits( s, 2, 3 );//b_frame_mbs_only:1,b_direct8x8_inference:1,b_crop:0,sps->b_vui:0
  if (pEncGlobal->frame_cropping_flag)
  {
	PutBits(s,1,1);
	UEBits( s, pEncGlobal->frame_crop_left_offset);
	UEBits( s, pEncGlobal->frame_crop_right_offset);
	UEBits( s, pEncGlobal->frame_crop_top_offset);
	UEBits( s, pEncGlobal->frame_crop_bottom_offset);
  }
  else
	PutBits(s,1,0);
  PutBits(s,1,0);
  RBSPBits( s );
  FlushBits( s );
}


void WritePPS(H264ENC *pEncGlobal, BS_TYPE *s)
{
  RealignBits( s );
  //pps->i_id:0,sps_id:0,cabac:0,b_pic_order:0,i_num_slice_groups - 1:1,i_num_ref_idx_l1_active:0,b_weighted_pred:0
  PutBits( s, 8, 206);
  //b_weighted_bipred:0(2),pps->i_pic_init_qp - 26:0,pps->i_pic_init_qs - 26 :0,pps->i_chroma_qp_index_offset:0
  //pEncGlobal->OutParam.b_deblocking_filter:1,b_constrained_intra_pred:0,b_redundant_pic_cnt:0
  //PutBits( s, 8, 60);
  PutBits( s, 2, 0);//b_weighted_bipred

  SEBits( s, 0);//pps->i_pic_init_qp - 26
  SEBits( s, 0);//pps->i_pic_init_qs - 26 
  SEBits( s, 0);//pps->i_chroma_qp_index_offset

  PutBits( s, 1, pEncGlobal->OutParam.b_deblocking_filter);
  PutBits( s, 1, 0);//b_constrained_intra_pred
  PutBits( s, 1, 0);//b_redundant_pic_cnt
  RBSPBits( s );
  FlushBits( s );
}


const H264LEVEL AVCLevel[] =
{
  { 10,   1485,    99,   152064,     64,  64},
  { 11,   3000,   396,   345600,    192, 128},
  { 12,   6000,   396,   912384,    384, 128},
  { 13,  11880,   396,   912384,    768, 128},
  { 20,  11880,   396,   912384,   2000, 128},
  { 21,  19800,   792,  1824768,   4000, 256},
  { 22,  20250,  1620,  3110400,   4000, 256},
  { 30,  40500,  1620,  3110400,  10000, 256},
  { 31, 108000,  3600,  6912000,  14000, 512},
  { 32, 216000,  5120,  7864320,  20000, 512},
  { 40, 245760,  8192, 12582912,  20000, 512},
  { 41, 245760,  8192, 12582912,  50000, 512},
  { 42, 522240,  8704, 13369344,  50000, 512},
  { 50, 589824, 22080, 42393600, 135000, 512},
  { 51, 983040, 36864, 70778880, 240000, 512},
  { 0 }
};


VO_S32 IniLevel( H264ENC *pEncGlobal, H264LEVEL **level)
{
  VO_S32 mbs = pEncGlobal->mb_width * pEncGlobal->mb_height;
  VO_S32 dpb = mbs * 384;
  const H264LEVEL *l = AVCLevel;
	
  //support more limitation YU_TBD
  while(( l->frame_size < mbs|| l->frame_size*8 < pEncGlobal->mb_width * pEncGlobal->mb_width
		  || l->frame_size*8 < pEncGlobal->mb_height * pEncGlobal->mb_height ) ||( dpb > l->dpb ) ||
		  ( pEncGlobal->InternalParam.i_mv_range > l->mv_range ) ||
		  ( ((VO_S64)mbs * pEncGlobal->OutParam.fps_num) > l->mbps ))
    l++;

  *level = (H264LEVEL *)l;
  return 0;
}

