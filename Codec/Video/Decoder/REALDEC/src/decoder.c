/*************************************************** BEGIN LICENSE BLOCK ****************************************** 
* Version: RCSL 1.0 and Exhibits. 
* REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
* Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
* All Rights Reserved. 
* 
* The contents of this file, and the files included with this file, are 
* subject to the current version of the RealNetworks Community Source 
* License Version 1.0 (the "RCSL"), including Attachments A though H, 
* all available at http://www.helixcommunity.org/content/rcsl. 
* You may also obtain the license terms directly from RealNetworks. 
* You may not use this file except in compliance with the RCSL and 
* its Attachments. There are no redistribution rights for the source 
* code of this file. Please see the applicable RCSL for the rights, 
* obligations and limitations governing use of the contents of the file. 
* 
* This file is part of the Helix DNA Technology. RealNetworks is the 
* developer of the Original Code and owns the copyrights in the portions 
* it created. 
* 
* This file, and the files included with this file, is distributed and made 
* available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
* INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
* FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
* 
* Technology Compatibility Kit Test Suite(s) Location: 
* https://rarvcode-tck.helixcommunity.org 
* 
* Contributor(s): 
* 
*********************************************************** END LICENSE BLOCK ***********************************************/ 

/***************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc.                        
*    All Rights Reserved.                                                         
*    Do not redistribute.                                                           
**************************************************************************
***************************************************************************
*    INTEL Corporation Proprietary Information                           
*    This listing is supplied under the terms of a license             
*    agreement with INTEL Corporation and may not be copied   
*    nor disclosed except in accordance with the terms of          
*    that agreement.                                                               
*    Copyright (c) 1995 - 2000 Intel Corporation.                       
*    All Rights Reserved.                                                        
***************************************************************************
************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/

//Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/decoder.c,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp
//This module implements most of the Decoder class and DecoderYUVWorkSpace
// classes. 
/********************************************************************************
Emuzed India Private Limited
*********************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2nd May, 2005	Real Networks			Initial Code
16th May, 2005	M. Anjaneya Prasad		Minor modification in
Decoder_applyPostFilters function
*******************************************************************************************************************************/
//Main decoder functionality.
#undef FALSE
#undef TRUE

#include <string.h>

#include "voRealVideo.h"
#include "decoder.h"
#include "rv30dec.h"
#include "basic.h"
#include "deblock.h"
#include "parsebs.h"
#include "bsx.h"
#include "voCheck.h"

#define WEAK_SMOOTHING 3
#define MEDIUM_SMOOTHING 2
#define STRONG_SMOOTHING 1
#define NO_SMOOTHING 0
#define OFFSET_STRONGER_FILTER 4


#define RV_Maximum_Deblocking_Strength     6
#define RV_Default_Deblocking_Strength     0


#if defined(VODEBUG)||defined(_DEBUG)
int g_inputFrameNum;
#endif//_DEBUG

#if defined(VODEBUG)
FILE * out_file;
unsigned char aa[400];
#endif

/****************************************/
/* Decoder constructor                  */
/****************************************/
void Decoder_Init(struct Decoder *t, RV_Boolean	bIsRV8, RV_FID fid)
{
	t->m_fid	= fid;
	t->m_bIsRV8 = bIsRV8;
	t->m_pQPSTab = s_deblock_rv89combo[RV_Default_Deblocking_Strength];
	t->m_first_Iframe_flage = 1;
#ifdef FBM_INTERNAL_CACHE_SUPPORT
  t->m_pDecReferenceFrames = (IMAGE**)MallocMem(sizeof(IMAGE*)*(t->m_priv.buf_num), CACHE_LINE, t->m_pMemOP);
  t->m_priv.img_seq = (IMAGE **)MallocMem(sizeof(IMAGE*)*(t->m_priv.buf_num+1), CACHE_LINE, t->m_pMemOP);
  t->m_priv.r_idx = 0;
  t->m_priv.w_idx = t->m_priv.buf_num;
#else
	t->m_priv.r_idx = 0;
	t->m_priv.w_idx = MAXFRAMES;
#endif
	t->m_uv_interlace_flage = 1;

	if(t->m_mem_share.Init){
		t->m_uv_interlace_flage = 0;
	}

	if(t->m_pMemOP->Alloc){
		t->m_uv_interlace_flage = 0;
	}

	Global_Decoder_Options_Init(&t->m_options);

	if (bIsRV8){
		t->gc_C_InterpolateNew				= InterpolateLuma_RV8;
		t->gc_C_InterpolateChromaNew		= InterpolateChroma_RV8;
		t->gc_C_AddInterpolateNew			= InterpolateAddLuma_RV8;
		t->gc_C_AddInterpolateChromaNew		= InterpolateAddChroma_RV8;
		t->GetIntraMBType					= DecodeIntraTypes_RV8;
		t->GetPictureHeader					= CB_GetPictureHeader_RV8;
		t->GetSliceHeader					= CB_GetSliceHeader_RV8;
		t->InLoopFilter						= InLoopFilterRV8;

	}else{ 
		t->gc_C_InterpolateNew              = InterpolateLuma_RV9;
		t->gc_C_InterpolateChromaNew        = InterpolateChroma_RV9;
		t->gc_C_AddInterpolateNew           = InterpolateAddLuma_RV9;
		t->gc_C_AddInterpolateChromaNew     = InterpolateAddChroma_RV9;
		t->GetIntraMBType                   = DecodeIntraTypes_RV9;
		t->GetPictureHeader					= CB_GetPictureHeader;
		t->GetSliceHeader                   = CB_GetSliceHeader;
		t->InLoopFilter                     = InLoopFilterRV9;
	}
}

/**************************************************************************************************
*         Start_Sequence: This method should be called if there are significant 
*         changes in the input format in the compressed bit stream.
*         This method must be called between construction 
*         of the Decoder object and the first time a decompress is done. 
*		YU: malloc buffer according to display dimension YU
***************************************************************************************************/
VO_U32 Decoder_Malloc_buf(void *decoder, RV_FID	fid,U32 uWidth,U32 uHeight)
{
	VO_U32    returnCode = VO_ERR_NONE;
	U32       i;

	struct Decoder *t = (struct Decoder *)decoder;

	for (;;)// Not really a loop.  Use "break" instead of "goto" on error. 
	{
		//TBD
		/* tell the smoothing filter whether or not to smooth */

		switch (fid)
		{
		case RV_FID_REALVIDEO30:
			t->m_uTRWrap = TR_WRAP_RV;
			break;
		case RV_FID_RV89COMBO:
			t->m_uTRWrap = TR_WRAP;
			break;
		default:
			RVAssert(0);
			returnCode = VO_ERR_WRONG_STATUS;
			break;
		}

		if (returnCode != VO_ERR_NONE)
			break;

		t->m_fid = fid;
		t->m_pDisplayFrame = 0;

		t->m_out_img_width = uWidth;
		t->m_out_img_height= uHeight;

		t->m_pre_out_img_width = uWidth;
		t->m_pre_out_img_height= uHeight;

		t->m_mbX = (uWidth + 15) >> 4;
		t->m_mbY = (uHeight + 15) >> 4;

		t->m_img_width  = t->m_mbX << 4;
		t->m_img_height = t->m_mbY << 4;
		t->m_img_stride = t->m_img_width + (YUV_Y_PADDING<<1);

		if(t->m_uv_interlace_flage){
			t->m_img_UVstride = t->m_img_stride;
		}else{
			t->m_img_UVstride = t->m_img_stride >> 1;
		}

		t->m_max_img_size   = t->m_img_width * t->m_img_height;
		t->m_max_img_width  = t->m_img_width;
		t->m_max_img_height = t->m_img_height;

		if(t->m_mem_share.Init){
			VO_MEM_VIDEO_INFO videoMem;

			videoMem.Stride = t->m_img_stride;
			videoMem.Height = t->m_img_height + (YUV_Y_PADDING<<1);
			videoMem.ColorType = VO_COLOR_YUV_PLANAR420;
			videoMem.FrameCount = MAXFRAMES;

			t->m_mem_share.Init(VO_INDEX_DEC_RV, &videoMem);

			t->m_img_stride = videoMem.VBuffer[0].Stride[0];
			t->m_img_UVstride = videoMem.VBuffer[0].Stride[1];
#ifdef FBM_INTERNAL_CACHE_SUPPORT
      for (i = 0; i < t->m_priv.buf_num; i++){
        t->m_pDecReferenceFrames[i]->m_pYPlane = videoMem.VBuffer[i].Buffer[0];
        t->m_pDecReferenceFrames[i]->m_pUPlane = videoMem.VBuffer[i].Buffer[1];
        t->m_pDecReferenceFrames[i]->m_pVPlane = videoMem.VBuffer[i].Buffer[2];

        t->m_pDecReferenceFrames[i]->m_pYPlane += YUV_Y_PADDING * t->m_img_stride + YUV_Y_PADDING;
        t->m_pDecReferenceFrames[i]->m_pUPlane += YUV_UV_PADDING * t->m_img_UVstride + YUV_UV_PADDING;
        t->m_pDecReferenceFrames[i]->m_pVPlane += YUV_UV_PADDING * t->m_img_UVstride + YUV_UV_PADDING;
      }
#else
			for(i = 0; i < MAXFRAMES; i++){
				t->m_decReferenceFrames[i]->m_pYPlane = videoMem.VBuffer[i].Buffer[0];
				t->m_decReferenceFrames[i]->m_pUPlane = videoMem.VBuffer[i].Buffer[1];
				t->m_decReferenceFrames[i]->m_pVPlane = videoMem.VBuffer[i].Buffer[2];

				t->m_decReferenceFrames[i]->m_pYPlane += YUV_Y_PADDING * t->m_img_stride + YUV_Y_PADDING;
				t->m_decReferenceFrames[i]->m_pUPlane += YUV_UV_PADDING * t->m_img_UVstride + YUV_UV_PADDING;
				t->m_decReferenceFrames[i]->m_pVPlane += YUV_UV_PADDING * t->m_img_UVstride + YUV_UV_PADDING;
			}
#endif
		}else{
#ifdef FBM_INTERNAL_CACHE_SUPPORT
      for (i = 0; i < t->m_priv.buf_num; i++){
        returnCode = image_create(t, t->m_pDecReferenceFrames[i], t->m_img_width + (YUV_Y_PADDING<<1),
          t->m_img_height + (YUV_Y_PADDING<<1),t->m_uv_interlace_flage);

        if (returnCode != VO_ERR_NONE)
          break;
      }
#else
			for(i = 0; i < MAXFRAMES; i++){
				returnCode = image_create(t,t->m_decReferenceFrames[i], t->m_img_width + (YUV_Y_PADDING<<1), 
					t->m_img_height + (YUV_Y_PADDING<<1),t->m_uv_interlace_flage);

				if (returnCode != VO_ERR_NONE)
					break;
			}
#endif
		}

#ifdef FBM_INTERNAL_CACHE_SUPPORT
    for(i = 0; i < t->m_priv.buf_num; i++){
      t->m_priv.img_seq[i] = t->m_pDecReferenceFrames[i];
    }
    t->m_priv.img_seq[t->m_priv.buf_num] = NULL;

    /* Any previously displayed or decoded frame */
    /* will no longer be valid. */

    t->m_pCurrentFrame = t->m_pDecReferenceFrames[0];
    t->m_pRefFrame     = t->m_pDecReferenceFrames[1];
    t->m_pBFrame       = t->m_pDecReferenceFrames[2];
#else
		for(i = 0; i < MAXFRAMES; i++){
			t->m_priv.img_seq[i] = t->m_decReferenceFrames[i];
		}

		/* Any previously displayed or decoded frame */
		/* will no longer be valid. */

		t->m_pCurrentFrame = t->m_decReferenceFrames[0];
		t->m_pRefFrame     = t->m_decReferenceFrames[1];
		t->m_pBFrame       = t->m_decReferenceFrames[2];
#endif

		returnCode = Decoder_allocateParsedData(t, uWidth, uHeight, TRUE);
		break;
	}

	return returnCode;
}
/***************************************************************/
/* Decoder Destructor 										   */			
/***************************************************************/
void Decoder_Delete(void *decoder)
{
	struct Decoder *t;
	U32 i;

	t = (struct Decoder *)decoder;

	if(t->m_mem_share.Uninit){
		t->m_mem_share.Uninit(VO_INDEX_DEC_RV);
#ifdef FBM_INTERNAL_CACHE_SUPPORT
    for(i = 0; i < t->m_priv.buf_num; i++){
      if (t->m_pDecReferenceFrames[i]){
        FreeMem(t->m_pDecReferenceFrames[i],t->m_pMemOP);
      }		
      t->m_pDecReferenceFrames[i] = NULL;
    }
#else
		for(i = 0; i < MAXFRAMES; i++){
			if (t->m_decReferenceFrames[i]){
				FreeMem(t->m_decReferenceFrames[i],t->m_pMemOP);
			}		
			t->m_decReferenceFrames[i] = NULL;
		}
#endif
	}else{
#ifdef FBM_INTERNAL_CACHE_SUPPORT
    for(i = 0; i < t->m_priv.buf_num; i++){
      if (t->m_pDecReferenceFrames[i]){
        image_destroy(t,t->m_pDecReferenceFrames[i], t->m_img_stride);
        FreeMem(t->m_pDecReferenceFrames[i],t->m_pMemOP);
      }			
      t->m_pDecReferenceFrames[i] = NULL;
    }
#else
		for(i = 0; i < MAXFRAMES; i++){
			if (t->m_decReferenceFrames[i]){
				image_destroy(t,t->m_decReferenceFrames[i], t->m_img_stride);
				FreeMem(t->m_decReferenceFrames[i],t->m_pMemOP);
			}			
			t->m_decReferenceFrames[i] = NULL;
		}
#endif
	}

	for(i = 0; i < MAXYUV420PACKFRAMES; i++){
		if (t->m_YUV420Frame[i]){
				FreeMem(t->m_YUV420Frame[i]->m_pYPlane,t->m_pMemOP);
				FreeMem(t->m_YUV420Frame[i]->m_pUVPack,t->m_pMemOP);
				FreeMem(t->m_YUV420Frame[i],t->m_pMemOP);
		}			
		t->m_YUV420Frame[i] = NULL;
	}

	if(t->m_pTables){
		decode_4x4_delete(t,t->m_pTables);
		FreeMem(t->m_pTables,t->m_pMemOP);		
		t->m_pTables = NULL;
	}

	if(t->m_pYsh){
		FreeMem(t->m_pYsh,t->m_pMemOP);
		t->m_pYsh = NULL;	
	}

	if(t->m_pYsv){
		FreeMem(t->m_pYsv,t->m_pMemOP);
		t->m_pYsv = NULL;
	}

	if(t->m_pUsh){
		FreeMem(t->m_pUsh,t->m_pMemOP);
		t->m_pUsh = NULL;
	}

	if(t->m_pUsv){
		FreeMem(t->m_pUsv,t->m_pMemOP);
		t->m_pUsv = NULL;
	}

	if(t->m_pVsh){
		FreeMem(t->m_pVsh,t->m_pMemOP);
		t->m_pVsh = NULL;
	}

	if(t->m_pVsv){
		FreeMem(t->m_pVsv,t->m_pMemOP);
		t->m_pVsv = NULL;
	}

	Decoder_deallocateParsedData(t);

#ifdef FBM_INTERNAL_CACHE_SUPPORT
  if (t->m_pDecReferenceFrames){
    FreeMem(t->m_pDecReferenceFrames, t->m_pMemOP);
    t->m_pDecReferenceFrames = NULL;
  }

  if (t->m_priv.img_seq){
    FreeMem(t->m_priv.img_seq, t->m_pMemOP);
    t->m_priv.img_seq = NULL;
  }
#endif

}

/* YU: do not remalloc buffer, only change pic size, TBD*/
RV_Status Decoder_prepareDecodeBuffers(struct Decoder *t)
{
	I32 i;
	t->m_pDisplayFrame = 0;

	//TBD remalloc buffer or chage resolution
	//rpr
	if((0 == t->m_out_img_width)|| (0 == t->m_out_img_height) || (t->m_max_img_size < (t->m_out_img_height * t->m_out_img_width))){
		return RV_S_ERROR;
	}
	if(ISINTERPIC(t->m_ptype) && (t->m_pre_out_img_width != t->m_out_img_width || t->m_pre_out_img_height != t->m_out_img_height)){
		T_RPR_EdgeParams  edgeParams;
		IMAGE *pNewImageBuffer,*pOldImageBuffer;
		U8 *pNewBuffer[3],*pOldBuffer[3];

		pNewImageBuffer = buf_seq_ctl(&t->m_priv, NULL , FIFO_READ); 

		pNewBuffer[0] = pNewImageBuffer->m_pYPlane;
		pNewBuffer[1] = pNewImageBuffer->m_pUPlane;
		pNewBuffer[2] = pNewImageBuffer->m_pVPlane;

		pOldBuffer[0] = t->m_pRefFrame->m_pYPlane;
		pOldBuffer[1] = t->m_pRefFrame->m_pUPlane;
		pOldBuffer[2] = t->m_pRefFrame->m_pVPlane;

		pOldImageBuffer = t->m_pRefFrame;

		edgeParams.iDeltaBottom = 0;
		edgeParams.iDeltaLeft = 0;
		edgeParams.iDeltaRight = 0;
		edgeParams.iDeltaTop = 0;

		ResampleYUVFrame(t,pOldBuffer, pNewBuffer,&edgeParams);
		t->m_pRefFrame = pNewImageBuffer; 
		image_setedges(t, t->m_pRefFrame,t->m_img_stride,t->m_img_height + (YUV_Y_PADDING<<1),t->m_img_width, t->m_img_height, t->m_uv_interlace_flage);
		buf_seq_ctl(&t->m_priv, pOldImageBuffer, FIFO_WRITE);

		t->m_pre_out_img_width  = t->m_out_img_width;
		t->m_pre_out_img_height = t->m_out_img_height;

	}

	if (!ISTRUEBPIC(t->m_ptype)){
		t->m_refTR = t->m_curTR;
		t->m_curTR = t->m_tr;
	}

	for (i = 0; i < t->m_decoding_thread_count; i++){
		if (ISTRUEBPIC(t->m_ptype)) {
			t->m_slice_di[i].m_pMBInfo = t->m_slice_di[i].m_pBFrameMBInfo;
		}else {
			t->m_slice_di[i].m_pMBInfo = t->m_slice_di[i].m_pReferenceMBInfo;
		}
	}

	return RV_S_OK;
}

static void mtdecode_slice(struct Decoder *t, I32 slice_count, U8* const pb, U32 pb_size)
{
	I32 j;
	I32 uMBA = 0;
	I32 returnCode = 0;
	I32 slice_main;
	I32 slice_minor;
	SLICE_DEC *slice_di = &t->m_slice_di[0];

	t->m_pb      = pb;
	t->m_pb_size = pb_size;
	slice_main   = slice_count >> 1;
	slice_minor  = slice_count - slice_main;

	t->m_slice_minor = slice_minor;
	t->m_slice_main  = slice_main;

#ifdef THREAD_TEST
//    pthread_mutex_lock(&t->child_mutex);
	printf("signal child_cond \n");
	pthread_cond_signal(&t->child_cond);
	printf("main lock main mutex \n");
	pthread_mutex_lock(&t->main_mutex);
//	pthread_mutex_unlock(&t->child_mutex);
#else
	//printf("post slice event \n");
	vo_sem_post(&t->m_event_slice_decoding[0]);
#endif


//	printf("send post!\n");

	for (j = 0; j < slice_main; j++){
		if(0 != j){
			initbits(slice_di, pb + t->m_sliceInfo[j].offset, pb_size);
			slice_di->m_pBitstart = pb;
			slice_di->m_pBitend   = pb + pb_size + 4;
			returnCode = t->GetSliceHeader(t, &uMBA, slice_di);
		}

		slice_di->m_resync_mb_x      = uMBA % t->m_mbX;
		slice_di->m_resync_mb_y      = uMBA / t->m_mbX;

		if(returnCode){
			break;
		}

		SetMem((VO_U8 *)slice_di->m_pAboveSubBlockIntraTypes, -1, t->m_mbX << 2, t->m_pMemOP);

		slice_di->m_pMBIntraTypes[3]  = -1;
		slice_di->m_pMBIntraTypes[7]  = -1;
		slice_di->m_pMBIntraTypes[11] = -1;
		slice_di->m_pMBIntraTypes[15] = -1;

		Decoder_Slice(t, j+1, 0);
	}

	t->m_main_threaded_finish = 1;
#ifdef THREAD_TEST
//    pthread_mutex_lock(&t->main_mutex);
//	printf("waiting main_cond \n");
//	pthread_cond_wait(&t->main_cond, &t->main_mutex);
//	printf("inter main_cond \n");
//	pthread_mutex_unlock(&t->main_mutex);

	pthread_mutex_lock(&t->main_mutex);
	printf("main lock main mutex \n");
	pthread_mutex_unlock(&t->main_mutex);
	printf("main unlock main mutex \n");

//	pthread_mutex_lock(&t->child_mutex);
//	printf("main lock child mutex \n");
//	pthread_mutex_unlock(&t->child_mutex);
//	printf("main unlock child mutex \n");
#else
	//printf("wait main event \n");
	vo_sem_wait(&t->m_event_main);
	//printf("inter main event \n");
#endif

}

RV_Status Decoder_DecAFrame(void *decoder, RV_Decoder_Notes *notes, U8* const pb, U32 pb_size)
{
	U32 slice_count=0;
	U32 uMB_Y;
	//I32 buf_size = pb_size;
	RV_Status ps = RV_S_OK;
	struct Decoder *t = (struct Decoder *)decoder;
#ifdef LICENSEFILE
	VO_VIDEO_BUFFER outBuffer;
#endif

#if defined(VODEBUG)||defined(_DEBUG)
	g_inputFrameNum++;
#endif//_DEBUG

#ifdef LICENSEFILE
	if (((RV_FORCED_INTRAPIC == t->m_display_ptype)||(RV_INTERPIC == t->m_display_ptype)) &&(!t->m_first_Iframe_flage) ){

		if (((NULL != t->m_pDisplayFrame) && ( 0 == t->m_output_mode))||
			((NULL != t->m_pRefFrame->m_pYPlane) && ( 0 != t->m_output_mode))){
				if ( 0 == t->m_output_mode){
					outBuffer.Buffer[0] = t->m_pDisplayFrame->m_pYPlane;
					outBuffer.Buffer[1] = t->m_pDisplayFrame->m_pUPlane;
					outBuffer.Buffer[2] = t->m_pDisplayFrame->m_pVPlane;
					outBuffer.Time      = t->m_pDisplayFrame->Time;
					outBuffer.UserData      = t->m_pDisplayFrame->UserData;
				}else{
					outBuffer.Buffer[0] = t->m_pRefFrame->m_pYPlane;
					outBuffer.Buffer[1] = t->m_pRefFrame->m_pUPlane;
					outBuffer.Buffer[2] = t->m_pRefFrame->m_pVPlane;
					outBuffer.Time      = t->m_pRefFrame->Time;
					outBuffer.UserData      = t->m_pRefFrame->UserData;
				}

				outBuffer.Stride[0] = t->m_img_stride;
				outBuffer.Stride[1] = t->m_img_UVstride;
				outBuffer.Stride[2] = t->m_img_UVstride;
				outBuffer.ColorType = VO_COLOR_YUV_PLANAR420;
				
				voCheckLibResetVideo(t->m_phCheck, &outBuffer);
		}
	}
#endif

	//if(g_inputFrameNum==89)
	//	g_inputFrameNum=g_inputFrameNum;
	/* defualt no frame to display*/
	t->m_pDisplayFrame = 0;
	t->m_main_threaded_finish = 0;
	t->m_slice_di[0].m_first_flage = 0;
	if (t->m_decoding_thread_count > 1){
		t->m_slice_di[1].m_first_flage = 0;
	}
	
	if(!t->m_raw_data_flage){
		slice_count = t->m_nSlices;
	}
	
	/* When a sequence containing B frames is encoded to some file formats, */
	/* there will be an 8-byte, all zeroes, dummy frame in place of the initial B frame. */
	/* There's nothing to decode right now, but turn on latency because */
	/* it's a good bet that B frames are coming. */
	/* TBD, we might want to put this check in prepareDecodeBuffers. */
	/* That would allow 8-byte dummies to be used to flush out the final */
	/* reference frame at the end of a B-frame sequence.  Need to be */
	/* careful though, and make sure InitDecClumpedFrames can handle this. */
	/* Need to access src->data as bytes, not as a couple of U32's, */
	/* because theoretically it might not be 4-byte aligned. */
	/* Check the later bytes first, since they're more likely to */
	/* be non-zero in the general case. */
	if (pb_size == 8 && pb[7] == 0 && pb[6] == 0 && pb[5] == 0 && pb[4] == 0
		&& pb[3] == 0 && pb[2] == 0 && pb[1] == 0 && pb[0] == 0 ){
			RVDebug((RV_TL_INIT,"::Decode -- ignoring 8-byte dummy"
				" frame, enabling latency"));
			return RV_S_OK;
	}

	/* This prevents the previously decoded frame, if any, from having */
	/* its bitstream info exposed to the application. */
	initbits(&t->m_slice_di[0], pb, pb_size);

	if(t->GetPictureHeader(t, &t->m_slice_di[0]))
		return RV_S_ERROR;

	if(t->m_first_Iframe_flage){
		if(ISINTRAPIC(t->m_ptype)){
			t->m_first_Iframe_flage = 0;
			t->m_flush_err = 1;
		}else{
			t->m_ptype = VO_VIDEO_FRAME_NULL;
			return ps;
		}
	}else if(t->m_flush_err){
		if(ISTRUEBPIC(t->m_ptype)){
			t->m_ptype = VO_VIDEO_FRAME_NULL;
			return ps;
		}else{
			t->m_flush_err = 0;
		}
	}

	if(Decoder_prepareDecodeBuffers(t)){
		return RV_S_ERROR;
	}

	//if(1840 == g_inputFrameNum){
	//	int huwei;
	//	huwei = 1;
	//}

	if((!ISINTRAPIC(t->m_ptype)) && (t->m_ptype != RV_INTERPIC) && (t->m_ptype != RV_TRUEBPIC)){
		return RV_S_UNSUPPORTED;
	}

	if(t->m_decoding_thread_count > 1){

		if (RV_INTRAPIC == t->m_ptype){
			t->m_ptype = RV_FORCED_INTRAPIC;
		}

		if(RV_TRUEBPIC == t->m_ptype){
			t->m_pBFrame =  buf_seq_ctl(&t->m_priv, NULL, FIFO_READ);
			if(NULL == t->m_pBFrame){
				return VO_MEMRC_NO_YUV_BUFFER;
			}
			t->m_pBFrame->Time = t->m_timeStamp;
			t->m_pBFrame->UserData = t->UserData;
		}else{
			t->m_pCurrentFrame = buf_seq_ctl(&t->m_priv,NULL,FIFO_READ);
			if(NULL == t->m_pCurrentFrame){
				return VO_MEMRC_NO_YUV_BUFFER;
			}
			t->m_pCurrentFrame->Time = t->m_timeStamp;
			t->m_pCurrentFrame->UserData = t->UserData;
		}

		if (t->m_multithreaded_rd && (slice_count > 1)){
			mtdecode_slice(t, slice_count,pb,pb_size);
		}else{
			SLICE_DEC *slice_di = &t->m_slice_di[0];

			slice_di->m_resync_mb_x      = 0;
			slice_di->m_resync_mb_y      = 0;

			SetMem((VO_U8 *)slice_di->m_pAboveSubBlockIntraTypes, -1, t->m_mbX << 2, t->m_pMemOP);

			slice_di->m_pMBIntraTypes[3]  = -1;
			slice_di->m_pMBIntraTypes[7]  = -1;
			slice_di->m_pMBIntraTypes[11] = -1;
			slice_di->m_pMBIntraTypes[15] = -1;

			ps = Decoder_Slice(t, 1, 0);
		}

		if ((!t->m_deblocking_filter_passthrough) && (t->m_enable_deblocking_flage) && (RV_TRUEBPIC != t->m_ptype)){
			for(uMB_Y = 0; uMB_Y < t->m_mbY; uMB_Y++){
				t->InLoopFilter(t, uMB_Y);
			}
		}

	}else{
		SLICE_DEC *slice_di = &t->m_slice_di[0];

		if (ISTRUEBPIC(t->m_ptype)) {
			slice_di->m_pMBInfo = slice_di->m_pBFrameMBInfo;
			slice_di->m_pCurMotionVectors = slice_di->m_pBMotionVectors;
		}else {
			slice_di->m_pMBInfo = slice_di->m_pReferenceMBInfo;
			slice_di->m_pCurMotionVectors = slice_di->m_pReferenceMotionVectors;
		}

		slice_di->m_pTopMotionVectors = slice_di->m_pCurMotionVectors + (t->m_img_width >> 2);

		slice_di->m_resync_mb_x      = 0;
		slice_di->m_resync_mb_y      = 0;

		SetMem((VO_U8 *)slice_di->m_pAboveSubBlockIntraTypes, -1, t->m_mbX << 2, t->m_pMemOP);

		slice_di->m_pMBIntraTypes[3]  = \
			slice_di->m_pMBIntraTypes[7]  = \
			slice_di->m_pMBIntraTypes[11] = \
			slice_di->m_pMBIntraTypes[15] = -1;
		//---------------------------------------------

		switch (t->m_ptype){ 
		case RV_INTRAPIC:
		case RV_FORCED_INTRAPIC:
			ps = Decoder_IPicture(t);
			t->m_ptype = RV_FORCED_INTRAPIC;
			break;
		case RV_INTERPIC:
			ps = Decoder_PPicture(t);
			break;
		case RV_TRUEBPIC: 
			ps = Decoder_BPicture(t);
			break;
		default:

		RVDebug((RV_TL_ALWAYS,"ERROR: parseBitStream -- "
			"Unknown picture coding type"));
		return RV_S_UNSUPPORTED;

		}

		if ((!t->m_deblocking_filter_passthrough) && (t->m_enable_deblocking_flage) && (RV_TRUEBPIC != t->m_ptype)){
			for(uMB_Y = 0; uMB_Y < t->m_mbY; uMB_Y++){
				t->InLoopFilter(t, uMB_Y);
			}
		}
	}

	t->m_enable_deblocking_flage = 1;

	if(!ISTRUEBPIC(t->m_ptype)){
		IMAGE *tmp;
		EnumRVPicCodType tmp_ptype;
		//VO_S64 tmp_time = 0;


		image_setedges(t, t->m_pCurrentFrame,t->m_img_stride,t->m_img_height + (YUV_Y_PADDING<<1),t->m_img_width, t->m_img_height,t->m_uv_interlace_flage);

		if(t->m_flush_err){
			t->m_pre_ptype = t->m_ptype;
			t->m_ptype = VO_VIDEO_FRAME_NULL;

			t->m_pRefFrame = t->m_pCurrentFrame;
			t->m_pDisplayFramePre = NULL;
			t->m_pDisplayFrame    = NULL;
		}else{
			tmp = t->m_pRefFrame;
			t->m_pRefFrame = t->m_pCurrentFrame;
			t->m_pCurrentFrame = tmp;

			tmp_ptype = t->m_ptype;
			t->m_ptype = t->m_pre_ptype;
			t->m_pre_ptype = tmp_ptype;

			buf_seq_ctl(&t->m_priv, t->m_pDisplayFramePre, FIFO_WRITE);
			t->m_pDisplayFrame = t->m_pCurrentFrame;
			t->m_pDisplayFramePre = t->m_pDisplayFrame;
		}
	}else{
		t->m_pDisplayFrame = t->m_pBFrame;
		buf_seq_ctl(&t->m_priv, t->m_pDisplayFrame, FIFO_WRITE);

		t->m_pre_out_img_width  = t->m_out_img_width;
		t->m_pre_out_img_height = t->m_out_img_height;
	}

	return ps;
}

void Decoder_deallocateParsedData(struct Decoder *t)
{
	if (t->m_pParsedData){
		/* Free the old buffer. */
		FreeMem(t->m_pParsedData,t->m_pMemOP);		
		t->m_pParsedData = NULL;
	}
	t->m_parsedDataLength = 0;
}


//TBD clean up exactSizeRequested - only calculate the size or including memory allocation
U32 Decoder_allocateParsedData(struct Decoder *t, U32 uWidth, U32 uHeight, RV_Boolean exactSizeRequested)
{
	U32     MB_Frame_Width;
	U32     MB_Frame_Height;
	U32     Sub_Block_Width;
	U32     Sub_Block_Height;
	U32     quantBufferSize;
	U32     subblockRowIntraTypeSize;
	U32     macroblockIntraTypeSize;
	U32     motionVectorSize;
	U32     motionVectorSizeBFrame;
	U32     motionVectorSizeBackward;
	U32     MBInfoSize;
	U32     uPredBufSize;	// 16x16 buffer required for one luma MB
	U32     totalSize;
	U8*		pAlignedParsedData;
	U32     offset;
	I32     thread_count;
	I32     i;

	uWidth	= (uWidth  + 15) & ~15;
	uHeight = (uHeight + 15) & ~15;

	/* If our buffer and internal pointers are already set up for this */
	/* image size, then there's nothing more to do. */
	/* But if exactSizeRequested, we need to see if our existing */
	/* buffer is oversized, and perhaps reallocate it. */

	if (t->m_img_width	== uWidth &&t->m_img_height == uHeight && !exactSizeRequested){
		return VO_ERR_NONE;
	}

	thread_count    = t->m_decoding_thread_count;
	t->m_img_width	= uWidth;
	t->m_img_height = uHeight;
	/* Determine how much space we need */

	MB_Frame_Width   = uWidth >> 4;
	MB_Frame_Height  = uHeight >> 4;

	Sub_Block_Width  = MB_Frame_Width << 2;
	Sub_Block_Height = MB_Frame_Height << 2;

	quantBufferSize = NUM_QUANT_BUFFER_ELEMENTS * sizeof(t->m_slice_di[0].m_pQuantBuf[0]);
	uPredBufSize = NUM_QUANT_BUFFER_ELEMENTS*sizeof(U8);

	subblockRowIntraTypeSize = Sub_Block_Width * sizeof(DecoderIntraType);

	macroblockIntraTypeSize = NUM_INTRA_TYPE_ELEMENTS * sizeof(t->m_slice_di[0].m_pMBIntraTypes[0]);

	motionVectorSize = (Sub_Block_Width >> 1) * (Sub_Block_Height >> 1) * sizeof(struct DecoderPackedMotionVector);

	motionVectorSizeBFrame = MB_Frame_Width * 4 * 2 * sizeof(struct DecoderPackedMotionVector);

	motionVectorSizeBackward = MB_Frame_Width * 2 * 4 * sizeof(struct DecoderPackedMotionVector);

	MBInfoSize = MB_Frame_Width * MB_Frame_Height * sizeof(struct DecoderMBInfo);

	totalSize =
		((uPredBufSize + 15) * thread_count)		/* needs 16-byte alignment */
		+ ((quantBufferSize + 7) * thread_count)	/* needs 8-byte alignment */ 
		+ (motionVectorSize + 7)
		+ ((motionVectorSizeBFrame + 7) * thread_count)
		+ ((motionVectorSizeBackward + 7) * thread_count) /* Backward motion vectors for bidird */
		+ ((macroblockIntraTypeSize + 7) * thread_count)
		+ ((subblockRowIntraTypeSize + 7) * thread_count)
		/* m_pAboveSubBlockIntraTypes will be 8-byte aligned */
		/* However, subblockRowIntraTypeSize is only */
		/* guaranteed to be a multiple of 4.  So we need */
		/* to align back up to 8, thus the "+7". */
		+ (MBInfoSize + 7)  /* reference frame MB Info */
		+ 7 + MBInfoSize  /* B frame MB Info */
		+ YUV_ALIGNMENT;

	/* Reallocate our buffer if its size is not appropriate. */
	if (t->m_parsedDataLength){
		Decoder_deallocateParsedData(t);
	}

	t->m_pParsedData = (U8 *)MallocMem(totalSize, CACHE_LINE, t->m_pMemOP);
	if (!t->m_pParsedData) {
		return VO_ERR_OUTOF_MEMORY;
	}

	SetMem(t->m_pParsedData,0,totalSize,t->m_pMemOP);	

	t->m_parsedDataLength = totalSize;

	/* Reassign our internal pointers if need be */
	offset = 0;
	pAlignedParsedData = t->m_pParsedData;

	/* t->m_pQuantBuf must be 8-byte aligned, is 32-byte aligned */
	RVAssert((offset & 0x7) == 0);
	for(i = 0; i < thread_count; i++){
		t->m_slice_di[i].m_pQuantBuf = (I32 *)(pAlignedParsedData + offset);
		offset += quantBufferSize;
	}

	/* m_pPredBuf must be 16-byte aligned, is 32-byte aligned */
	RVAssert((offset & 0xf) == 0);
	for (i = 0; i < thread_count; i++){
		t->m_slice_di[i].m_pPredBuf = (U8 *)(pAlignedParsedData + offset);
		offset += uPredBufSize;
	}

	/* m_pReferenceMotionVectors must be 8-byte aligned, is 32-byte aligned */
	RVAssert((offset & 0x7) == 0);
	{
		struct DecoderPackedMotionVector* pTemp;
		pTemp = (struct DecoderPackedMotionVector *)(pAlignedParsedData + offset);
		offset += motionVectorSize;
		for(i = 0; i < thread_count; i++){
			t->m_slice_di[i].m_pReferenceMotionVectors = pTemp;
		}
	}
	
	/* t->m_pBMotionVectors must be 8-byte aligned, is 32-byte aligned */
	RVAssert((offset & 0x7) == 0);
	for(i = 0; i < thread_count; i++){
		t->m_slice_di[i].m_pBMotionVectors =
			(struct DecoderPackedMotionVector *)(pAlignedParsedData + offset);
		offset += motionVectorSizeBFrame;
	}

	/* t->m_pBbidirMotionVectors must be 8-byte aligned, is 32-byte aligned */
	/* These are the backward (pointing to future) motion vectors */
	RVAssert((offset & 0x7) == 0);
	for(i = 0; i < thread_count; i++){
		t->m_slice_di[i].m_pBidirMotionVectors =
			(struct DecoderPackedMotionVector *)(pAlignedParsedData + offset);
		offset += motionVectorSizeBackward;  /* only one vector per MB */
		t->m_slice_di[i].m_pTopBidirMotionVectors = t->m_slice_di[i].m_pBidirMotionVectors + 4*MB_Frame_Width;
		t->m_slice_di[i].m_pCurBidirMotionVectors = t->m_slice_di[i].m_pBidirMotionVectors;
	}

	/* t->m_pMBIntraTypes must be 4-byte aligned, is 16-byte aligned */
	RVAssert((offset & 0x7) == 0);
	for(i = 0; i < thread_count; i++){
		t->m_slice_di[i].m_pMBIntraTypes = (DecoderIntraType*)(pAlignedParsedData + offset);
		offset += macroblockIntraTypeSize;
	}

	/* t->m_pAboveSubBlockIntraTypes must be 4-byte aligned, is 8-byte aligned */
	RVAssert((offset & 0x7) == 0);
	for(i = 0; i < thread_count; i++){
		t->m_slice_di[i].m_pAboveSubBlockIntraTypes =
			(DecoderIntraType*)(pAlignedParsedData + offset);
		offset += subblockRowIntraTypeSize;
	}


	/* align to 8-byte boundary */
	if (offset & 0x7)
		offset = (offset + 7) & ~7;

	RVAssert((offset & 0x7) == 0);
	{
		struct DecoderMBInfo* pTmp;

		pTmp = (struct DecoderMBInfo*)(pAlignedParsedData + offset);
		offset += MBInfoSize;
		for(i = 0; i < thread_count; i++){
			t->m_slice_di[i].m_pReferenceMBInfo = pTmp;
		}
	}

	/* align to 8-byte boundary */
	if (offset & 0x7)
		offset = (offset + 7) & ~7;
	RVAssert((offset & 0x7) == 0);
	{
		struct DecoderMBInfo* pTem;
		pTem = (struct DecoderMBInfo*)(pAlignedParsedData + offset);
		offset += MBInfoSize;
		for(i = 0; i < thread_count; i++){
			t->m_slice_di[i].m_pBFrameMBInfo = pTem;
		}
	}
	

	RVAssert(offset <= totalSize);

	/*memset(pAlignedParsedData,0,offset); */

	return VO_ERR_NONE;
}
/* After flush*/
static void  reset_buf_seq(struct Decoder *dec)
{
	FIFOTYPE *priv = &dec->m_priv; 
	U32 i, w_idx, r_idx;

	if (dec == NULL || dec->m_priv.img_seq == NULL || dec->m_pDecReferenceFrames == NULL)
		return;

#ifdef FBM_INTERNAL_CACHE_SUPPORT
  for(i = 0; i < priv->buf_num; i++) {
#else
	for(i = 0; i < MAXFRAMES; i++){
#endif
		w_idx = priv->w_idx;
		r_idx = priv->r_idx;

		do{
#ifdef FBM_INTERNAL_CACHE_SUPPORT
      if(priv->img_seq[r_idx] == dec->m_pDecReferenceFrames[i])
        break;
      r_idx = (r_idx + 1) % (priv->buf_num + 1);
#else
			if(priv->img_seq[r_idx] == dec->m_decReferenceFrames[i])
        break;
      r_idx = (r_idx + 1) % FIFO_NUM;
#endif
		}while(r_idx != w_idx);

		if(r_idx == w_idx)
#ifdef FBM_INTERNAL_CACHE_SUPPORT
      buf_seq_ctl(priv, dec->m_pDecReferenceFrames[i], FIFO_WRITE);
#else
			buf_seq_ctl(priv, dec->m_decReferenceFrames[i], FIFO_WRITE);
#endif
	}
}

void YUV420T0YUV420PACKMB_C(VO_U8* srcu, VO_U8* srcv, VO_U8* dst, VO_U32 srcStride, VO_U32 dstStride)
{
	VO_U32 i;
	VO_U8 temp0, temp1;

	for (i = 0; i < 8; i++){
		temp0 = srcu[0];
		temp1 = srcv[0];
		dst[0] = temp0;
		dst[1] = temp1;

		temp0 = srcu[1];
		temp1 = srcv[1];
		dst[2] = temp0;
		dst[3] = temp1;

		temp0 = srcu[2];
		temp1 = srcv[2];
		dst[4] = temp0;
		dst[5] = temp1;

		temp0 = srcu[3];
		temp1 = srcv[3];
		dst[6] = temp0;
		dst[7] = temp1;

		temp0 = srcu[4];
		temp1 = srcv[4];
		dst[8] = temp0;
		dst[9] = temp1;

		temp0 = srcu[5];
		temp1 = srcv[5];
		dst[10] = temp0;
		dst[11] = temp1;

		temp0 = srcu[6];
		temp1 = srcv[6];
		dst[12] = temp0;
		dst[13] = temp1;

		temp0 = srcu[7];
		temp1 = srcv[7];
		dst[14] = temp0;
		dst[15] = temp1;

		srcu += srcStride;
		srcv += srcStride;
		dst  += dstStride;	
	}

}

void YUV420T0YUV420PACK(VO_U8* srcy, VO_U8* srcu, VO_U8* srcv,  VO_U8* dstY, VO_U8 *dstUV, VO_U32 srcStrideY, VO_U32 srcStrideUV, VO_U32 width, VO_U32 height)
{
	VO_U32 i,j;
	VO_U32 blockX,blockY;
	VO_U32 dstStride;

	blockX = (width  + 15) >> 4;
	blockY = (height + 15) >> 4;

	dstStride = blockX << 4;

	for (j = 0; j < blockY; j++){
		for (i = 0; i < blockX; i++){
			CopyBlock16x16(srcy + (i << 4), dstY + (i << 4), srcStrideY, dstStride);
		}
		srcy  += (srcStrideY << 4);
		dstY  += (dstStride << 4);
	}

	blockX = ((width >> 1) + 7)  >> 3;
	blockY = ((height >> 1) + 7) >> 3;

	dstStride = blockX << 4;

	for (j = 0; j < blockY; j++){
		for (i = 0; i < blockX; i++){
			YUV420T0YUV420PACKMB(srcu + (i << 3), srcv + (i << 3), dstUV + (i << 4), srcStrideUV, dstStride);
		}
		srcu += (srcStrideUV << 3);
		srcv += (srcStrideUV << 3);
		dstUV  += (dstStride << 3);
	}

}
/******************************************************/
/*      Custom_Message                                */
/*      Handle our custom decoder messages            */
/******************************************************/
U32 Decoder_Custom_Message(struct Decoder *t,U32 *p_msgid)
{
	U32           returnCode = VO_ERR_NONE;
	RV_MSG_Simple *simple = (RV_MSG_Simple*)p_msgid;  /* It just might be! */

	switch (simple->message_id)
	{
	case VO_PID_COMMON_FLUSH:
	case VO_PID_DEC_RealVideo_FLUSH:
		{
			if(simple->value1){
				t->m_first_Iframe_flage = 1;
				reset_buf_seq(t);
			}
		}
		break;
	case VO_PID_DEC_RealVideo_DISABLEDEBLOCKING:
		{
			if(simple->value1){
				t->m_enable_deblocking_flage = 0;
			}else{
				t->m_enable_deblocking_flage = 1;
			}
		}
		break;
	case VO_PID_VIDEO_OUTPUTMODE:
		{
			if(simple->value1){
				t->m_output_mode = 1;
			}else{
				t->m_output_mode = 0;
			}
		}
		break;
	case VO_PID_DEC_RealVideo_VO_COLOR_YUV_420_PACK:
		{
			if(simple->value1){
				U32 i;

				t->m_yuv420_pack_flag   = 1;

				for(i = 0; i < MAXYUV420PACKFRAMES; i++){
					t->m_YUV420Frame[i] = MallocMem(sizeof(YUV420PACK) + SAFETY, CACHE_LINE, t->m_pMemOP);
					if(!t->m_YUV420Frame[i]){
						returnCode = VO_ERR_OUTOF_MEMORY;
						break;
					}
					SetMem((VO_U8 *)t->m_YUV420Frame[i], 0, sizeof(YUV420PACK), t->m_pMemOP);
				}

				for (i = 0; i < MAXYUV420PACKFRAMES; i++){

					t->m_YUV420Frame[i]->m_pYPlane = MallocMem((((t->m_img_width + 15) & (~0xf)) + SAFETY ) *t->m_img_height, CACHE_LINE, t->m_pMemOP);
					if(!t->m_YUV420Frame[i]->m_pYPlane){
						returnCode = VO_ERR_OUTOF_MEMORY;
						break;
					}

					t->m_YUV420Frame[i]->m_pUVPack = MallocMem((((t->m_img_width + 15) & (~0xf)) + SAFETY) *t->m_img_height/2, CACHE_LINE, t->m_pMemOP);
					if(!t->m_YUV420Frame[i]->m_pUVPack){
						returnCode = VO_ERR_OUTOF_MEMORY;
						break;

					}
				}
			}else{
				t->m_yuv420_pack_flag = 0;
			}
		}
		break;
	case VO_PID_VIDEO_FRAMETYPE:
	case VO_PID_DEC_RealVideo_FORMAT:
		{
			simple->value2 = realVideoFrametype(t,(U8*)simple->value1);
		}
		break;
	case VO_PID_DEC_RealVideo_GETLASTOUTVIDEOBUFFER:
		{
			VO_VIDEO_BUFFER* last_buf = (VO_VIDEO_BUFFER*)simple->pValue;
#ifdef LICENSEFILE
			VO_VIDEO_BUFFER outBuffer;
			VO_VIDEO_FORMAT outFormat; 
#endif

			last_buf->Buffer[0] = t->m_pRefFrame->m_pYPlane;
			last_buf->Buffer[1] = t->m_pRefFrame->m_pUPlane;
			last_buf->Buffer[2] = t->m_pRefFrame->m_pVPlane;

			last_buf->Stride[0] = t->m_img_stride;
			last_buf->Stride[1] = t->m_img_UVstride;
			last_buf->Stride[2] = t->m_img_UVstride;

			last_buf->Time = t->m_pRefFrame->Time;
			last_buf->UserData = t->UserData;

			last_buf->ColorType = VO_COLOR_YUV_PLANAR420;

#ifdef LICENSEFILE
			outBuffer.Buffer[0] = last_buf->Buffer[0];
			outBuffer.Buffer[1] = last_buf->Buffer[1];
			outBuffer.Buffer[2] = last_buf->Buffer[2];
			outBuffer.Stride[0] = last_buf->Stride[0]; 
			outBuffer.Stride[1] = last_buf->Stride[1]; 
			outBuffer.Stride[2] = last_buf->Stride[2];
			outBuffer.ColorType = last_buf->ColorType;
			outBuffer.Time      = last_buf->Time;
			outBuffer.UserData      = last_buf->UserData;

			outFormat.Width  = t->m_pre_out_img_width;
			outFormat.Height = t->m_pre_out_img_height;
			outFormat.Type   = t->m_pre_ptype - 1;
			voCheckLibCheckVideo(t->m_phCheck, &outBuffer, &outFormat);
#endif
			if (t->m_yuv420_pack_flag){
				U32 i = 0;//huwei 20110307 yuv420pack
				U32 frame_number = t->m_frame_number;

				i = frame_number % MAXYUV420PACKFRAMES;

				YUV420T0YUV420PACK(last_buf->Buffer[0], last_buf->Buffer[1],last_buf->Buffer[2], t->m_YUV420Frame[i]->m_pYPlane,
					t->m_YUV420Frame[i]->m_pUVPack,last_buf->Stride[0], last_buf->Stride[1], t->m_pre_out_img_width, t->m_pre_out_img_height);

				last_buf->ColorType = VO_COLOR_YUV_420_PACK;
				last_buf->Buffer[0] = t->m_YUV420Frame[i]->m_pYPlane;
				last_buf->Buffer[1] = t->m_YUV420Frame[i]->m_pUVPack;
				last_buf->Buffer[2] = NULL;
				last_buf->Stride[0] = (t->m_pre_out_img_width +15) & (~0xf);
				last_buf->Stride[1] = (t->m_pre_out_img_width +15) & (~0xf);
				last_buf->Stride[2] = 0;
			}
		}
		break;
	case VO_PID_DEC_RealVideo_GETLASTOUTVIDEOFORMAT:
		{
			VO_VIDEO_FORMAT* last_format = (VO_VIDEO_FORMAT*)simple->pValue;

			last_format->Width  = t->m_pre_out_img_width;
			last_format->Height = t->m_pre_out_img_height;
			last_format->Type   = t->m_pre_ptype - 1;

		}
		break;
	case VO_PID_DEC_RealVideo_GET_VIDEOFORMAT:
		{
			VO_VIDEO_FORMAT* last_format = (VO_VIDEO_FORMAT*)simple->pValue;

			last_format->Width  = t->m_out_img_width;
			last_format->Height = t->m_out_img_height;
			last_format->Type   = VO_VIDEO_FRAME_NULL;
		}
		break;
	case VO_PID_DEC_RealVideo_GET_VIDEO_WIDTH:
		{
			simple->value1 = t->m_out_img_width;
		}
		break;
	case VO_PID_DEC_RealVideo_GET_VIDEO_HEIGHT:
		{
			simple->value1 = t->m_out_img_height;
		}
		break;
	case VO_PID_VIDEO_VIDEOMEMOP:
		{
			VO_MEM_VIDEO_OPERATOR *pMemShare;

			pMemShare = (VO_MEM_VIDEO_OPERATOR *)simple->pValue;

			t->m_mem_share.GetBufByIndex = pMemShare->GetBufByIndex;
			t->m_mem_share.Init          = pMemShare->Init;
			t->m_mem_share.Uninit        = pMemShare->Uninit;

		}
		break;
	default:
		returnCode = VO_ERR_WRONG_PARAM_ID;
		break;
	}

	return returnCode;
}


/* This constructor simply gives each member a default value. */
void  Global_Decoder_Options_Init(struct Global_Decoder_Options *t)
{
	t->debockingFlage               = 1;
}
