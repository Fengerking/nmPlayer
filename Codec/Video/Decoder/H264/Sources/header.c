
/*!
 *************************************************************************************
 * \file header.c
 *
 * \brief
 *    H.264 Slice headers
 *
 *************************************************************************************
 */
#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#endif

#include "global.h"
#include "elements.h"
#include "defines.h"
#include "fmo.h"
#include "vlc.h"
#include "mbuffer.h"
#include "header.h"

//#include "ctx_tables.h"

#if TRACE
#define SYMTRACESTRING(s) strncpy(sym.tracestring,s,TRACESTRING_SIZE)
#else
#define SYMTRACESTRING(s) // to nothing
#endif



/*!
 ************************************************************************
 * \brief
 *    calculate Ceil(Log2(uiVal))
 ************************************************************************
 */
#ifndef VOI_H264D_BLOCK_FMO
avdNativeUInt CeilLog2(avdNativeUInt uiVal)
{
  avdNativeUInt uiTmp = uiVal;
  avdNativeUInt uiRet = 0;

  while( uiTmp != 0 )
  {
    uiTmp >>= 1;
    uiRet++;
  }
  return uiRet;
}
#endif //VOI_H264D_BLOCK_FMO


/*!
 ************************************************************************
 * \brief
 *    read the first part of the header (only the pic_parameter_set_id)
 * \return
 *    Length of the first part of the slice header (in bits)
 ************************************************************************
 */
avdNativeInt FirstPartOfSliceHeader(ImageParameters *img,Bitstream *currStream)
{
  Slice *currSlice = img->currentSlice;
  avdNativeUInt tmp;

  // Get first_mb_in_slice
  currSlice->start_mb_nr = avd_ue_v (currStream);

#ifdef USE_JOBPOOL
    if (img->vdLibPar->multiCoreNum > 1 && currSlice->start_mb_nr == 0 && img->dec_picture)
    {
	    restart_cores(img);
        // this may only happen on slice loss
        //pocInfo->currPoc++;
        AVD_ERROR_CHECK2(img, "the slice of the previous frame(PF) is lost", WARN_PREV_FRAME_SLICE_LOST);

#if 1//TRACE_ERROR_DETAIL
        AvdLog2(LL_INFO, "lost slice:frame type:%s, frameNum:%d,poc:%d, because: slice data losses\n", GetStrFrameType(img->dec_picture->typeIPB), img->dec_picture->frame_num, img->dec_picture->poc);
#endif
        StoreErrorFrame(img, 0);
    }
#endif    
  tmp = (avdNativeUInt)avd_ue_v (currStream);
  
  if (tmp>4) tmp -=5;

	#if FEATURE_BFRAME
   		if (tmp>4)
		{
			tmp = 4;
   			AVD_ERROR_CHECK(img,"invalid slice type",ERROR_SliceType);
		}
   	#else
   		if(tmp!=P_SLICE && tmp != I_SLICE)
		{
			tmp=P_SLICE;
   			AVD_ERROR_CHECK(img,"invalid slice type",ERROR_SliceType);
		}
   	#endif//VOI_H264D_NON_BASELINE
#if !FEATURE_EXPROFILE
		if (tmp>2)
		{
			AVD_ERROR_CHECK(img,"invalid slice type,the version doest support extention profile",ERROR_SliceType);
		}
#endif
  //img->prevType = img->type;
  img->type = currSlice->picture_type = tmp;

  currSlice->pic_parameter_set_id = (avdNativeUInt)avd_ue_v (currStream);
  
  return 0;//(avdNativeInt) partition->bitstream->frame_bitoffset;
}

/*!
 ************************************************************************
 * \brief
 *    read the scond part of the header (without the pic_parameter_set_id 
 * \return
 *    Length of the second part of the Slice header in bits
 ************************************************************************
 */
avdNativeInt RestOfSliceHeader(ImageParameters *img,Bitstream *currStream)
{
	Slice *currSlice = img->currentSlice;
	pic_parameter_set_rbsp_t *active_pps = img->active_pps;
	seq_parameter_set_rbsp_t *active_sps = img->active_sps;
	TPOCInfo* pocInfo = img->pocInfo;
	avdNativeInt val, len;
	// log2_max_frame_num_minus4 range 0 to 12;
	TSpecitialInfo	*specialInfo = img->specialInfo;
	//TMBBitStream	*mbBits = GetMBBits(currMB);
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TCABACInfo	*cabacInfo = img->cabacInfo;
	specialInfo->frame_num = (avdNativeUInt)avd_u_v (active_sps->log2_max_frame_num_minus4 + 4, currStream);

	/* Tian Dong: frame_num gap processing, if found */
	if (specialInfo->idr_flag)
	{
		specialInfo->pre_frame_num = specialInfo->frame_num;
	}

#if FEATURE_INTERLACE//VOI_H264D_NON_BASELINE
	if (active_sps->frame_mbs_only_flag)
	{
		specialInfo->structure = FRAME;
		img->field_pic_flag=0;
		img->MbaffFrameFlag=0;
	}
	else
	{
		// field_pic_flag   u(1)
		img->field_pic_flag = avd_u_1(currStream);
		if (img->field_pic_flag)
		{
			// bottom_field_flag  u(1)
			specialInfo->bottom_field_flag = avd_u_1(currStream);
			specialInfo->structure = specialInfo->bottom_field_flag ? BOTTOM_FIELD : TOP_FIELD;
		}
		else
		{
			specialInfo->structure = FRAME;
			specialInfo->bottom_field_flag=0;
		}
		img->MbaffFrameFlag=(active_sps->mb_adaptive_frame_field_flag && (img->field_pic_flag==0));
		
	}
#else//baseline
	specialInfo->structure = FRAME;
	
#endif
	currSlice->structure = specialInfo->structure;


	if (specialInfo->idr_flag)
		specialInfo->idr_pic_id = avd_ue_v(currStream);

	// POC200301
	if (active_sps->pic_order_cnt_type == 0)
	{
		// range 0 to 12;
		
		pocInfo->PicOrderCntLsb = avd_u_v(active_sps->log2_max_pic_order_cnt_lsb_minus4 + 4,currStream);
		if( active_pps->pic_order_present_flag  ==  1 
 #if FEATURE_INTERLACE
			&&  !img->field_pic_flag 
#endif
			)
			pocInfo->delta_pic_order_cnt_bottom = avd_se_v(currStream);
		else
			pocInfo->delta_pic_order_cnt_bottom = 0;  
	}

	if( active_sps->pic_order_cnt_type == 1 && !active_sps->delta_pic_order_always_zero_flag ) 
	{
		pocInfo->delta_pic_order_cnt[ 0 ] = avd_se_v(currStream);
		if( active_pps->pic_order_present_flag  ==  1 
#if FEATURE_INTERLACE
			&&  !img->field_pic_flag  
#endif// #if FEATURE_INTERLACE
			)
			pocInfo->delta_pic_order_cnt[ 1 ] = avd_se_v(currStream);
	}
	else
	{
		if (active_sps->pic_order_cnt_type == 1)
		{
			pocInfo->delta_pic_order_cnt[ 0 ] = 0;
			pocInfo->delta_pic_order_cnt[ 1 ] = 0;
		}
	}

	//! redundant_pic_cnt is missing here
	if (active_pps->redundant_pic_cnt_present_flag)
		specialInfo->redundant_pic_cnt = avd_ue_v ( currStream);

#if FEATURE_BFRAME
	if(img->type==B_SLICE)
	{
		int oriType = avd_u_1 (currStream)==0?B_DIRECT_TEMPORAL:B_DIRECT_SPACE;
#if OPT_ASSUME_DIRECTTYPE_IS_CONSISTENT
		if(oriType!=currSlice->direct_type&&currSlice->direct_type!=0 )
			AVD_ERROR_CHECK(img,"error: the direct type is not consistent",ERROR_InvalidBitstream);
#endif//
		currSlice->direct_type = oriType;
	}
	
	img->num_ref_idx_l1_active = active_pps->num_ref_idx_l1_active_minus1 + 1;
	if(img->num_ref_idx_l1_active<1||img->num_ref_idx_l1_active>MAX_REFERENCE_FRAMES)
	{
		AVD_ERROR_CHECK(img,"the ref frame count is not valid!!",ERROR_InvalidNumRefFrame);
	}
#endif
	img->num_ref_idx_l0_active = active_pps->num_ref_idx_l0_active_minus1 + 1;
	
	if(img->type==P_SLICE || img->type == SP_SLICE || img->type==B_SLICE)
	{
		//num_ref_idx_override_flag
		val = avd_u_1 (currStream);
		if (val)
		{
			img->num_ref_idx_l0_active = 1 + avd_ue_v ( currStream);
#if FEATURE_BFRAME
			if(img->type==B_SLICE)
				img->num_ref_idx_l1_active = 1 + avd_ue_v (currStream);
#endif //VOI_H264D_NON_BASELINE
			//Be careful the override_flag may be caused by bit-stream error,
			//So if the img->num_ref_idx_l0_active>active_pps->num_ref_idx_l0_active_minus1 + 1,take care!!
		
		}
	}
	

	//20110402: Make sure that the num_ref_idx_l0_active is not great than maxListSize, otherwise, overflow
	if(img->num_ref_idx_l0_active>img->maxListSize)
	{
		AVD_ERROR_CHECK(img,"img->num_ref_idx_l0_active>img->maxListSize!!",ERROR_InvalidNumRefFrame);
	}
#if FEATURE_BFRAME
	if (img->type!=B_SLICE)
		img->num_ref_idx_l1_active = 0;
	if(img->num_ref_idx_l1_active>img->maxListSize)
	{
		AVD_ERROR_CHECK(img,"img->num_ref_idx_l1_active>img->maxListSize!!",ERROR_InvalidNumRefFrame );
	}
#endif //VOI_H264D_NON_BASELINE


	if (img->type!=I_SLICE && img->type!=SI_SLICE)
	{
		val = currSlice->ref_pic_list_reordering_flag_l0 = avd_u_1 (currStream);
		if (val)
		{
			alloc_ref_pic_list0_reordering_buffer(img,currSlice);
			len = 0;
			do {
				val = currSlice->remapping_of_pic_nums_idc_l0[len] = avd_ue_v(currStream);
				switch(val) {
				case 0:
				case 1:
					currSlice->abs_diff_pic_num_minus1_l0[len] = avd_ue_v(currStream);
					break;
				case 2:
					currSlice->long_term_pic_idx_l0[len] = avd_ue_v(currStream);
					break;
				case 3: 
					break;
				default:
					AVD_ERROR_CHECK(img,"RestOfSliceHeader,invalid reorder number",ERROR_RemappingPicIdc);//shall we turn it to warning?
					break;
				}
				len++;
				if(len>MAX_REFERENCE_FRAMES)
				{
					AVD_ERROR_CHECK(img,"RestOfSliceHeader,too much ref IDC",ERROR_TOOMuchReorderIDC);
				}
				// assert (i>img->num_ref_idx_l0_active);
			} while (val != 3);
		}
	}
#if FEATURE_BFRAME
	if (img->type==B_SLICE)
	{
		val = currSlice->ref_pic_list_reordering_flag_l1 = avd_u_1 (currStream);
		if (val)
		{
			len = 0;
			alloc_ref_pic_list1_reordering_buffer(img,currSlice);
			do {
				val = currSlice->remapping_of_pic_nums_idc_l1[len] = avd_ue_v(currStream);
				if (val==0 || val==1)
					currSlice->abs_diff_pic_num_minus1_l1[len] = avd_ue_v(currStream);
				else if (val==2)
					currSlice->long_term_pic_idx_l1[len] = avd_ue_v(currStream);
				len++;
				if(len>MAX_REFERENCE_FRAMES)
				{
					AVD_ERROR_CHECK(img,"RestOfSliceHeader,too much ref IDC2",ERROR_TOOMuchReorderIDC);
				}
			} while (val != 3);
		}
	}
#endif//FEATURE_BFRAME
#if FEATURE_WEIGHTEDPRED
	{
		int applyW2P = active_pps->weighted_pred_flag && currSlice->picture_type == P_SLICE;
		int applyW2B  = active_pps->weighted_bipred_idc > 0  && currSlice->picture_type == B_SLICE;
		TWeightInfo* weight;
		img->apply_weights = applyW2P ||applyW2B;
		if(img->apply_weights)
		{
			if(img->weightInfo==NULL)
				img->weightInfo= voH264AlignedMalloc(img,1002,sizeof(TWeightInfo));
			//weight = img->weightInfo;
			//weight->wp_weight = weight->weightBuf;
			//weight->wp_offset = (avdInt8*)weight->wp_weight+sizeof(TWeightArray);
		}
		

		if (applyW2P
			||(active_pps->weighted_bipred_idc==1 && img->type==B_SLICE))
		{
			pred_weight_table(img,currStream);
		}
		
	}
#endif

	if (specialInfo->nal_reference_idc)
		dec_ref_pic_marking(img,currStream);
	CHECK_ERR_RET_INT
#if FEATURE_CABAC
	if (active_pps->entropy_coding_mode_flag && img->type!=I_SLICE && img->type!=SI_SLICE)
	{
		//cabac_init_idc;
		cabacInfo->model_number = avd_ue_v(currStream);

		if(cabacInfo->model_number>=3)
		{
			AVD_ERROR_CHECK(img,"invalid model_number",ERROR_InvalidBitstream);
		}
	}
	else 
	{
		cabacInfo->model_number = 0;
	}
#endif

	//slice_qp_delta;
	val = (avdNativeInt)avd_se_v(currStream);

	currSlice->qp  = 26 + active_pps->pic_init_qp_minus26 + val;
	if(currSlice->qp>51 ||currSlice->qp<0 )
	{
		int error = img->qp?WARN_GENERAL:ERROR_QP;
		currSlice->qp = img->qp;
		AVD_ERROR_CHECK(img,"img->qp>51 ||img->qp<0,reuse the original QP",error);//ERROR_QP) ;
	}
	img->qp = currSlice->qp;
#if FEATURE_EXPROFILE
	if(img->type==SP_SLICE || img->type == SI_SLICE) 
	{
		if(img->type==SP_SLICE)
			cabacInfo->sp_switch = avd_u_1 (currStream);
		//slice_qp_delta;
		val = (avdNativeInt)avd_se_v(currStream);
		cabacInfo->qpsp = 26 + active_pps->pic_init_qs_minus26 + val;
	}
#endif

	if (active_pps->deblocking_filter_control_present_flag)
	{
		//disable_deblocking_filter_idc
		currSlice->LFDisableIdc = avd_ue_v (currStream);
		if(currSlice->LFDisableIdc>2||currSlice->LFDisableIdc<0)
		{
			currSlice->LFDisableIdc = 1;
			
		}
		if (currSlice->LFDisableIdc!=1)
		{
			currSlice->LFAlphaC0Offset = (avdNativeInt)avd_se_v(currStream)<<1;
			currSlice->LFBetaOffset = (avdNativeInt)avd_se_v(currStream)<<1;
			if(currSlice->LFAlphaC0Offset<-12||currSlice->LFAlphaC0Offset>12||currSlice->LFBetaOffset<-12||currSlice->LFBetaOffset>12)
			{
				//currSlice->LFAlphaC0Offset=currSlice->LFBetaOffset=0;
				AVD_ERROR_CHECK(img,"LFAlphaC0Offset>12",100);//ERROR_LFDisableIdc) ;
			}
		}
		else
			currSlice->LFAlphaC0Offset = currSlice->LFBetaOffset = 0;
	}
	else 
	{
		currSlice->LFDisableIdc = currSlice->LFAlphaC0Offset = currSlice->LFBetaOffset = 0;
	}

#ifndef VOI_H264D_BLOCK_FMO
	if (active_pps->num_slice_groups_minus1>0 && active_pps->slice_group_map_type>=3 &&
		active_pps->slice_group_map_type<=5){
		len = (active_sps->pic_height_in_map_units_minus1+1)*(active_sps->pic_width_in_mbs_minus1+1)/ 
		(active_pps->slice_group_change_rate_minus1+1);
		/*BUG: len += 1;*/ // see 7.4.3 (no trancation on division);
		if (((active_sps->pic_height_in_map_units_minus1+1)*(active_sps->pic_width_in_mbs_minus1+1))% 
		(active_pps->slice_group_change_rate_minus1+1))
			len +=1;
		len = CeilLog2(len);
		img->slice_group_change_cycle = (avdNativeUInt)avd_u_v (len, currStream);
	}
#endif

#if FEATURE_INTERLACE
	sizeInfo->PicHeightInMbs = sizeInfo->FrameHeightInMbs / ( 1 + img->field_pic_flag );
#else
	sizeInfo->PicHeightInMbs = sizeInfo->FrameHeightInMbs;
#endif
	sizeInfo->PicSizeInMbs = sizeInfo->PicWidthInMbs * sizeInfo->PicHeightInMbs;
	sizeInfo->FrameSizeInMbs = sizeInfo->PicWidthInMbs * sizeInfo->FrameHeightInMbs;
	return 0;
}


/*!
 ************************************************************************
 * \brief
 *    read the weighted prediction tables
 ************************************************************************
 */

/*!
 ************************************************************************
 * \brief
 *    read the memory control operations
 ************************************************************************
 */
void dec_ref_pic_marking(ImageParameters *img,Bitstream *currStream)
{
  avdNativeInt val;
  avdNativeInt currBufferSize = 0;
  avdNativeInt newBuf = 0;
  DecRefPicMarking_t *tmp_drpm,*tmp_drpm2;
  TSpecitialInfo	*specialInfo = img->specialInfo;
  // free old buffer content
  tmp_drpm=specialInfo->dec_ref_pic_marking_buffer;
  memset(tmp_drpm,0,sizeof(DecRefPicMarking_t));
  if (specialInfo->idr_flag)
  {
    specialInfo->no_output_of_prior_pics_flag = avd_u_1(currStream);
    specialInfo->long_term_reference_flag = avd_u_1(currStream);
  }
  else
  {
    specialInfo->adaptive_ref_pic_buffering_flag = avd_u_1(currStream);
#ifndef VOI_H264D_USE_SLIDINGWINDOW
    if (specialInfo->adaptive_ref_pic_buffering_flag)
    {
      // read Memory Management Control Operation 
		
      do
      {
		val = avd_ue_v(currStream);
		if(val==0)
			break;
		if(val<0||val>6)
		{
			AVD_ERROR_CHECK2(img,"dec_ref_pic_marking,memory_management_control_operation out of [0..6]",ERROR_InvalidMCO);
		}
		
		tmp_drpm->memory_management_control_operation = val;
		if ((val==1)||(val==3)) 
		{
			tmp_drpm->difference_of_pic_nums_minus1 = avd_ue_v(currStream);
		}
		if (val==2)
		{
			tmp_drpm->long_term_pic_num = avd_ue_v(currStream);
		}
	 
		if ((val==3)||(val==6))
		{
			tmp_drpm->long_term_frame_idx = avd_ue_v(currStream);
		}
		if (val==4)
		{
			tmp_drpm->max_long_term_frame_idx_plus1 = avd_ue_v(currStream);
		}
		tmp_drpm->Next = tmp_drpm+1;
   		tmp_drpm++;
		if(newBuf++>=MAX_REFERENCE_FRAMES)
		{
			AVD_ERROR_CHECK2(img,"newBuf>=MAX_REFERENCE_FRAMES\n",ERROR_InvalidBitstream);
		}
		memset(tmp_drpm,0,sizeof(DecRefPicMarking_t));   
      }while (1);
      
    }
#endif
  }


}



/*!
 ************************************************************************
 * \brief
 *    To calculate the poc values
 *        based upon JVT-F100d2
 *  POC200301: Until Jan 2003, this function will calculate the correct POC
 *    values, but the management of POCs in buffered pictures may need more work.
 * \return
 *    none
 ************************************************************************
 */
int voH264IntDiv2(int numerator, int denominator)
{
	int i=1;
	while(i*denominator<=numerator)
	{
		i++;
	}
	return i-1;
}
void decode_poc(ImageParameters *img)
{
	// Check: dose this support top_field is idr, but bottom field is not?
	// we used to use post_poc(above) to support this;
	avdNativeInt i;
	// for POC mode 0:
	avdInt32 MaxPicOrderCntLsb = (1<<(img->active_sps->log2_max_pic_order_cnt_lsb_minus4+4));
	seq_parameter_set_rbsp_t *active_sps = img->active_sps;
	TPOCInfo* pocInfo = img->pocInfo;
	TSpecitialInfo	*specialInfo = img->specialInfo;

	switch ( active_sps->pic_order_cnt_type )
	{
	case 0: // POC MODE 0
		// 1st
		if(specialInfo->idr_flag)
			pocInfo->PrevPicOrderCntMsb = pocInfo->PrevPicOrderCntLsb = 0;
		else if (specialInfo->last_has_mmco_5) {
#if FEATURE_INTERLACE
			if (specialInfo->last_pic_bottom_field)
			  pocInfo->PrevPicOrderCntMsb = pocInfo->PrevPicOrderCntLsb = 0;
			else
#endif // VOI_H264D_NON_BASELINE
			{
			  pocInfo->PrevPicOrderCntMsb = 0;
			  pocInfo->PrevPicOrderCntLsb = pocInfo->currPoc;
			}
		}
		// Calculate the MSBs of current picture
		if( pocInfo->PicOrderCntLsb  <  pocInfo->PrevPicOrderCntLsb  &&  
			( pocInfo->PrevPicOrderCntLsb - pocInfo->PicOrderCntLsb )  >=  ( MaxPicOrderCntLsb>>1 ) )
			pocInfo->PicOrderCntMsb = pocInfo->PrevPicOrderCntMsb + MaxPicOrderCntLsb;
		else if ( pocInfo->PicOrderCntLsb  >  pocInfo->PrevPicOrderCntLsb  &&
			( pocInfo->PicOrderCntLsb - pocInfo->PrevPicOrderCntLsb )  >  ( MaxPicOrderCntLsb>>1 ) )
			pocInfo->PicOrderCntMsb = pocInfo->PrevPicOrderCntMsb - MaxPicOrderCntLsb;
		else
			pocInfo->PicOrderCntMsb = pocInfo->PrevPicOrderCntMsb;
    
		// 2nd
#if FEATURE_INTERLACE

		if (!img->field_pic_flag)//||(!specialInfo->bottom_field_flag ))
		{
			pocInfo->toppoc = pocInfo->PicOrderCntMsb + pocInfo->PicOrderCntLsb;
			pocInfo->bottompoc = pocInfo->toppoc + pocInfo->delta_pic_order_cnt_bottom;
			pocInfo->currPoc = pocInfo->framepoc = (pocInfo->toppoc < pocInfo->bottompoc)? pocInfo->toppoc : pocInfo->bottompoc; // POC200301
		}

		else if (!specialInfo->bottom_field_flag)
		{  //top field
			pocInfo->currPoc= pocInfo->toppoc = pocInfo->PicOrderCntMsb + pocInfo->PicOrderCntLsb;
		}
		else
		{  //bottom field
			pocInfo->currPoc= pocInfo->bottompoc = pocInfo->PicOrderCntMsb + pocInfo->PicOrderCntLsb;
		}
#else//#if FEATURE_INTERLACE
		pocInfo->toppoc = pocInfo->PicOrderCntMsb + pocInfo->PicOrderCntLsb;
		pocInfo->bottompoc = pocInfo->toppoc + pocInfo->delta_pic_order_cnt_bottom;
		pocInfo->currPoc = pocInfo->framepoc = (pocInfo->toppoc < pocInfo->bottompoc)? pocInfo->toppoc : pocInfo->bottompoc; // POC200301

#endif//#if FEATURE_INTERLACE
		pocInfo->framepoc=pocInfo->currPoc;

		if ( specialInfo->frame_num!=specialInfo->PreviousFrameNum)
		  specialInfo->PreviousFrameNum=specialInfo->frame_num;

		if(!specialInfo->disposable_flag)
		{
		  pocInfo->PrevPicOrderCntLsb = pocInfo->PicOrderCntLsb;
		  pocInfo->PrevPicOrderCntMsb = pocInfo->PicOrderCntMsb;
		}

		break;

	case 1: // POC MODE 1
		// 1st
		if(specialInfo->idr_flag)
		{
			specialInfo->FrameNumOffset=0;     //  first pix of IDRGOP, 
			pocInfo->delta_pic_order_cnt[0]=0;                        //ignore first delta
			if(specialInfo->frame_num) 
			{
				specialInfo->frame_num = 0;
				AVD_ERROR_CHECK2(img,"frame_num != 0 in idr pix", ERROR_IDRHasRemainedFrame);
			}
		}
		else 
		{
			if (specialInfo->last_has_mmco_5)
			{
				specialInfo->PreviousFrameNumOffset = 0;
				specialInfo->PreviousFrameNum = 0;
			}//fixed_MP	
			if (specialInfo->frame_num<specialInfo->PreviousFrameNum) //not first pix of IDRGOP
				specialInfo->FrameNumOffset = specialInfo->PreviousFrameNumOffset + specialInfo->MaxFrameNum;
			else 
				specialInfo->FrameNumOffset = specialInfo->PreviousFrameNumOffset;
		}

		// 2nd
		if(active_sps->num_ref_frames_in_pic_order_cnt_cycle) 
			specialInfo->AbsFrameNum = specialInfo->FrameNumOffset+specialInfo->frame_num;
		else 
			specialInfo->AbsFrameNum=0;
		if(specialInfo->disposable_flag && specialInfo->AbsFrameNum>0)
			specialInfo->AbsFrameNum--;

		// 3rd
		pocInfo->ExpectedDeltaPerPicOrderCntCycle=0;
		if(active_sps->num_ref_frames_in_pic_order_cnt_cycle)
			for(i=0;i<(int) active_sps->num_ref_frames_in_pic_order_cnt_cycle;i++)
				pocInfo->ExpectedDeltaPerPicOrderCntCycle += active_sps->offset_for_ref_frame[i];

		if(specialInfo->AbsFrameNum)
		{
#if 1
			int i=1;
			while(i*active_sps->num_ref_frames_in_pic_order_cnt_cycle<=specialInfo->AbsFrameNum-1)
			{
				i++;
			}
			pocInfo->PicOrderCntCycleCnt = i-1;
			pocInfo->FrameNumInPicOrderCntCycle =specialInfo->AbsFrameNum-1-pocInfo->PicOrderCntCycleCnt*active_sps->num_ref_frames_in_pic_order_cnt_cycle;//voH264IntMod( (specialInfo->AbsFrameNum-1),active_sps->num_ref_frames_in_pic_order_cnt_cycle);
			if (pocInfo->PicOrderCntCycleCnt<0)
			{
				pocInfo->PicOrderCntCycleCnt = 0;
			}
			if (pocInfo->FrameNumInPicOrderCntCycle<0)
			{
				pocInfo->FrameNumInPicOrderCntCycle = 0;
			}
#else
			pocInfo->PicOrderCntCycleCnt = voH264IntDiv((specialInfo->AbsFrameNum-1),active_sps->num_ref_frames_in_pic_order_cnt_cycle);
			pocInfo->FrameNumInPicOrderCntCycle =voH264IntMod( (specialInfo->AbsFrameNum-1),active_sps->num_ref_frames_in_pic_order_cnt_cycle);
#endif
			pocInfo->ExpectedPicOrderCnt = pocInfo->PicOrderCntCycleCnt*pocInfo->ExpectedDeltaPerPicOrderCntCycle;
			for(i=0;i<=(int)pocInfo->FrameNumInPicOrderCntCycle;i++)
				pocInfo->ExpectedPicOrderCnt += active_sps->offset_for_ref_frame[i];
		}
		else 
			pocInfo->ExpectedPicOrderCnt=0;

		if(specialInfo->disposable_flag)
			pocInfo->ExpectedPicOrderCnt += active_sps->offset_for_non_ref_pic;
#if FEATURE_INTERLACE
		if(img->field_pic_flag==0)
		{           //frame pix
		  int toppoc = pocInfo->ExpectedPicOrderCnt + pocInfo->delta_pic_order_cnt[0];
		  int bottompoc = toppoc + active_sps->offset_for_top_to_bottom_field + pocInfo->delta_pic_order_cnt[1];
		  pocInfo->currPoc = (toppoc <bottompoc)? toppoc : bottompoc; // POC200301
		}

		else if (specialInfo->bottom_field_flag==0)
		{  //top field 
		  pocInfo->currPoc = pocInfo->toppoc = pocInfo->ExpectedPicOrderCnt + pocInfo->delta_pic_order_cnt[0];

		} 
		else
		{  //bottom field
		  pocInfo->currPoc = pocInfo->bottompoc = pocInfo->ExpectedPicOrderCnt + active_sps->offset_for_top_to_bottom_field + pocInfo->delta_pic_order_cnt[0];

		}
#else//#if FEATURE_INTERLACE
		{           //frame pix
			int toppoc = pocInfo->ExpectedPicOrderCnt + pocInfo->delta_pic_order_cnt[0];
			int bottompoc = toppoc + active_sps->offset_for_top_to_bottom_field + pocInfo->delta_pic_order_cnt[1];
			pocInfo->currPoc = (toppoc <bottompoc)? toppoc : bottompoc; // POC200301
		}
#endif //VOI_H264D_NON_BASELINE
		pocInfo->framepoc = pocInfo->currPoc;
		specialInfo->PreviousFrameNum = specialInfo->frame_num;
		specialInfo->PreviousFrameNumOffset = specialInfo->FrameNumOffset;
		break;

	case 2: // POC MODE 2
		if(specialInfo->idr_flag) // IDR picture
		{
			pocInfo->bottompoc = pocInfo->toppoc = pocInfo->framepoc = specialInfo->FrameNumOffset=0;     //  first pix of IDRGOP, 
			pocInfo->currPoc  = 0;
			if(specialInfo->frame_num)  
			{
				specialInfo->frame_num = 0;
				AVD_ERROR_CHECK2(img,"frame_num != 0 in idr pix", ERROR_IDRHasRemainedFrame);
			}
		}
		else
		{
			if (specialInfo->last_has_mmco_5)
			{
				specialInfo->PreviousFrameNumOffset = 0;
				specialInfo->PreviousFrameNum = 0;
			}

			if (specialInfo->frame_num<specialInfo->PreviousFrameNum)
				specialInfo->FrameNumOffset = specialInfo->PreviousFrameNumOffset + specialInfo->MaxFrameNum;
			else 
				specialInfo->FrameNumOffset = specialInfo->PreviousFrameNumOffset;

			specialInfo->AbsFrameNum = specialInfo->FrameNumOffset + specialInfo->frame_num;
			if(specialInfo->disposable_flag)
				pocInfo->currPoc = ((specialInfo->AbsFrameNum<<1) - 1);
			else
				pocInfo->currPoc = (specialInfo->AbsFrameNum<<1);

#if FEATURE_INTERLACE
			if (specialInfo->bottom_field_flag==0)
				pocInfo->toppoc = pocInfo->framepoc = pocInfo->currPoc;
			else 
				pocInfo->bottompoc = pocInfo->framepoc = pocInfo->currPoc;
#endif //VOI_H264D_NON_BASELINE
		}

		if (!specialInfo->disposable_flag)
			specialInfo->PreviousFrameNum = specialInfo->frame_num;
		specialInfo->PreviousFrameNumOffset = specialInfo->FrameNumOffset;
		break;
	default:
		//error must occurs
		assert( 1==0 );
		break;
	}
}

/*!
 ************************************************************************
 * \brief
 *    return the poc of img as per (8-1) JVT-F100d2
 *  POC200301
 ************************************************************************
 */
avdInt32 picture_order(ImageParameters *img)
{
  TPOCInfo* pocInfo = img->pocInfo;
  TSpecitialInfo* specialInfo = img->specialInfo;
#if FEATURE_INTERLACE
  if (img->field_pic_flag==0) // is a frame
    return pocInfo->framepoc;

  else if (specialInfo->bottom_field_flag==0) // top field
    return pocInfo->toppoc;
  else // bottom field
    return pocInfo->bottompoc;
#else// #if FEATURE_INTERLACE
  return pocInfo->framepoc;
#endif// #if FEATURE_INTERLACE
  return 0;
}


