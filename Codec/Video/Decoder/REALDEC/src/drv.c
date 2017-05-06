/* ***************************************** BEGIN LICENSE BLOCK ******************************************* 
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
 * ************************************************************ END LICENSE BLOCK **********************************/ 

/**************************************************************************/
/*    RealNetworks, Inc. Confidential and Proprietary Information.        */
/*    Copyright (c) 1995-2002 RealNetworks, Inc.						  */
/*    All Rights Reserved.												  */
/*    Do not redistribute.												  */
/**************************************************************************/
/**************************************************************************
*																		   *
*	VisualOn, Inc. Confidential and Proprietary, 2008		               *
*								 										   *
***************************************************************************/
#include <stdlib.h>

#include "voRealVideo.h"
#include "decoder.h"
#include "rv30dec.h"
#include "rv30backend.h"
#include "basic.h"
#include "realvdo.h"
/****************************************************************************************/
/* Define the version information that describes our generated bitstream.       */
/* See the description of the RV_MSG_ID_Get_Bitstream_Version message */
/* for the meaning of these values.                                                              */
/*****************************************************************************************/

#define RV_BITSTREAM_MAJOR_VERSION          4
#define RV_BITSTREAM_MAJOR_VERSION_RV8      3
#define RV_BITSTREAM_MINOR_VERSION          0
#define RV_BITSTREAM_MINOR_VERSION_RV8      2
#define RV_BITSTREAM_RELEASE_VERSION        0
#define RAW_BITSTREAM_MINOR_VERSION       128

// Define messages specific to a decoder. 
// Define a table that maps RV_Status values to HX_RESULTs 

static HX_RESULT hxresult_map[RV_NUMBER_OF_STATUS_CODES + 1] = {

    #define DEFRVSTATUS(pia, vfw, am, rv) rv,
    #include "rvdefs.h"
    0  //prevents a compiler warning about an uncovered trailing comma 
};


HX_RESULT RV_Status_to_HX_RESULT(RV_Status ps)
{
    if (ps >= 0 && ps < RV_NUMBER_OF_STATUS_CODES)
        return hxresult_map[ps];

    return HXR_FAIL;
}


/*************************************************************************************************
*                rv89toYUV420Init FUN        
*				YU: only use 	UINT16 pels = width	UINT16 lines = height VOULONG32 ulStreamVersion
	
*************************************************************************************************/
VO_U32 _RV40toYUV420Init(void *prv10Init, void **decoderState)
{
    VO_U32				    returnCode = VO_ERR_NONE;
    RV10_INIT				*pRV10_Init = (RV10_INIT *)prv10Init;
    RV_FID					fid;
	RV_Boolean				bIsRV8 = FALSE;
	struct Decoder          *t;
	U32						major, minor;
	U32                     i;

	if (!pRV10_Init || !decoderState){
		return VO_ERR_INVALID_ARG;
	}

	t = (struct Decoder *)(*decoderState);

    //Do the bitstream version check outside of the for-loop below, because we have no RV_Status value that maps to HXR_INVALID_VERSION
    // As of this writing, we support one Tromso bitstream version.

    major = HX_GET_MAJOR_VERSION(pRV10_Init->ulStreamVersion);
    minor = HX_GET_MINOR_VERSION(pRV10_Init->ulStreamVersion);

    if ((major == RV_BITSTREAM_MAJOR_VERSION)
        && (minor == RV_BITSTREAM_MINOR_VERSION)){
        fid = RV_FID_REALVIDEO30;
    }else if ( (major == RV_BITSTREAM_MAJOR_VERSION_RV8)
           && (minor == RV_BITSTREAM_MINOR_VERSION_RV8) ){
        fid = RV_FID_REALVIDEO30;
		/* tell the decoder to decode RV8 */
		bIsRV8 = TRUE;
	}else if ( minor == RAW_BITSTREAM_MINOR_VERSION)  {
		t->m_raw_data_flage = 1;
        fid = RV_FID_RV89COMBO;
        if (major == RV_BITSTREAM_MAJOR_VERSION_RV8)
            bIsRV8 = TRUE;
    }else{
        return HXR_INVALID_VERSION;
	}

	Decoder_Init(t, bIsRV8, fid);

	//YRJ_TBD, merge to CalMemSize() and IniDecBuf()

	t->m_slice_di  = (SLICE_DEC *)MallocMem(sizeof(SLICE_DEC)*t->m_decoding_thread_count, CACHE_LINE, t->m_pMemOP);	

	if (!t->m_slice_di){//YRJ_TBD
		VO_MEM_OPERATOR MemOperator;

		MemOperator.Free = t->m_pMemOP->Free;
		if (t->m_pMemOP){
			FreeMem(t->m_pMemOP, &MemOperator);
		}
		FreeMem(t, &MemOperator);

		return VO_ERR_OUTOF_MEMORY;
	}


	t->m_pTables   = (tDecoderTables *)MallocMem(sizeof(tDecoderTables),CACHE_LINE,t->m_pMemOP);
	if(!t->m_pTables){
		VO_MEM_OPERATOR MemOperator;

		MemOperator.Free = t->m_pMemOP->Free;

		if (t->m_pMemOP){
			FreeMem(t->m_pMemOP, &MemOperator);
		}

		FreeMem(t, &MemOperator);

		return VO_ERR_OUTOF_MEMORY;
	}

#ifdef FBM_INTERNAL_CACHE_SUPPORT
  for(i = 0; i < t->m_priv.buf_num; i++){
    t->m_pDecReferenceFrames[i] = (IMAGE *)MallocMem(sizeof(IMAGE) + SAFETY,CACHE_LINE, t->m_pMemOP);
    if(!t->m_pDecReferenceFrames[i]){
      return VO_ERR_OUTOF_MEMORY;
    }
    SetMem((VO_U8 *)t->m_pDecReferenceFrames[i],0,sizeof(IMAGE),t->m_pMemOP);
  }
#else
	for(i = 0; i < MAXFRAMES; i++){
		t->m_decReferenceFrames[i] = (IMAGE *)MallocMem(sizeof(IMAGE) + SAFETY,CACHE_LINE, t->m_pMemOP);
		if(!t->m_decReferenceFrames[i]){
			return VO_ERR_OUTOF_MEMORY;
		}
		SetMem((VO_U8 *)t->m_decReferenceFrames[i],0,sizeof(IMAGE),t->m_pMemOP);
	}
#endif

	t->m_pTables->y_to_cbp4x4 = (U32 *)MallocMem(sizeof(unsigned int)*(MAX_8x8_DSC),CACHE_LINE,t->m_pMemOP);
	if(!t->m_pTables->y_to_cbp4x4){
		return VO_ERR_OUTOF_MEMORY;
	}

	t->m_pTables->cbp_to_cx = (U32 *)MallocMem(sizeof(unsigned int)*(16),CACHE_LINE,t->m_pMemOP);
	if(!t->m_pTables->cbp_to_cx){
		return VO_ERR_OUTOF_MEMORY;
	}

	t->m_pTables->intra_decode_tables = (INTRA_DECODE_TABLES *)MallocMem(sizeof(INTRA_DECODE_TABLES)*(MAX_INTRA_QP_REGIONS),CACHE_LINE,t->m_pMemOP);
	if(!t->m_pTables->intra_decode_tables){
		return VO_ERR_OUTOF_MEMORY;
	}

	t->m_pTables->inter_decode_tables = (INTER_DECODE_TABLES *)MallocMem(sizeof(INTER_DECODE_TABLES)*(MAX_INTER_QP_REGIONS),CACHE_LINE,t->m_pMemOP);
	if(!t->m_pTables->inter_decode_tables){
		return VO_ERR_OUTOF_MEMORY;
	}

	if(bIsRV8){
		U32 b_per_row = ((pRV10_Init->pels + 15) >> 4) << 2;
		U32 b_per_col = ((pRV10_Init->lines + 15) >> 4) << 2;
		U32 b_per_row_chrom = ((b_per_row + 7) >> 3) << 2;
		U32 b_per_col_chrom = ((b_per_col + 7) >> 3) << 2;

		if (!t->m_pYsh || !t->m_pYsv){
			t->m_pYsh = (U8*)MallocMem(sizeof(U8)*b_per_row*b_per_col,CACHE_LINE,t->m_pMemOP);
			if(!t->m_pYsh){
				return VO_ERR_OUTOF_MEMORY;
			}

			t->m_pYsv = (U8*)MallocMem(sizeof(U8)*b_per_row*b_per_col,CACHE_LINE,t->m_pMemOP);
			if(!t->m_pYsv){
				return VO_ERR_OUTOF_MEMORY;
			}

			t->m_pUsh = (U8*)MallocMem(sizeof(U8)*b_per_row_chrom*b_per_col_chrom,CACHE_LINE,t->m_pMemOP);
			if(!t->m_pUsh){
				return VO_ERR_OUTOF_MEMORY;
			}

			t->m_pUsv = (U8*)MallocMem(sizeof(U8)*b_per_row_chrom*b_per_col_chrom,CACHE_LINE,t->m_pMemOP);
			if(!t->m_pUsv){
				return VO_ERR_OUTOF_MEMORY;
			}

			t->m_pVsh = (U8*)MallocMem(sizeof(U8)*b_per_row_chrom*b_per_col_chrom,CACHE_LINE,t->m_pMemOP);
			if(!t->m_pVsh){
				return VO_ERR_OUTOF_MEMORY;
			}

			t->m_pVsv = (U8*)MallocMem(sizeof(U8)*b_per_row_chrom*b_per_col_chrom,CACHE_LINE,t->m_pMemOP);
			if(!t->m_pVsv){
				return VO_ERR_OUTOF_MEMORY;
			}
		}

		SetMem(t->m_pYsh,0,sizeof(U8)*b_per_row,t->m_pMemOP);
		SetMem(t->m_pUsh,0,sizeof(U8)*b_per_row_chrom,t->m_pMemOP);
		SetMem(t->m_pVsh,0,sizeof(U8)*b_per_row_chrom,t->m_pMemOP);
	}

	returnCode = Decoder_Malloc_buf(t, fid, pRV10_Init->pels, pRV10_Init->lines);

    if (returnCode != VO_ERR_NONE){
	//	VO_MEM_OPERATOR MemOperator;

		Decoder_Delete(t);

		//huwei 20110413 fix a bug about memory free
		/*MemOperator.Free = t->m_pMemOP->Free;
		if (t->m_pMemOP){
			FreeMem(t->m_pMemOP, &MemOperator);
		}

		FreeMem(t, &MemOperator);*/
       
		return VO_ERR_OUTOF_MEMORY;
    }

	decode_4x4_init_vlc(t,t->m_pTables);

    return returnCode;
}



HX_RESULT _RV40toYUV420Free(void *global)
{
    struct Decoder *t = (struct Decoder *)global;

    if (t){
		VO_MEM_OPERATOR MemOperator;

		Decoder_Delete(t);
		MemOperator.Free = t->m_pMemOP->Free;

		if (t->m_pMemOP){
			FreeMem(t->m_pMemOP, &MemOperator);
		}

		FreeMem(t,&MemOperator);    
	}

    return HXR_OK;
}

HX_RESULT _RV40toYUV420Transform(UCHAR *pRV20Packets, UCHAR *pDecodedFrameBuffer, void *pInputParams, void *pOutputParams, void *global)
{
    RV_Status				ps = RV_S_OK;
    struct Decoder			*decoder;
    RV_Decoder_Notes		notes = 0;
    H263DecoderInParams  * pRV20In  = (H263DecoderInParams *)pInputParams;
    H263DecoderOutParams * pRV20Out = (H263DecoderOutParams *)pOutputParams;
	I32 useDataLen;

    if (!pRV20In || !pRV20Out)
        return HXR_POINTER;

    pRV20Out->numFrames = 0;  /* Until we learn otherwise */
    pRV20Out->notes = 0;

    decoder = (struct Decoder*)global;

    /* Init segment info if RealVideo format */
    if (decoder->m_fid != RV_FID_RV89COMBO) {
		decoder->m_nSlices = pRV20In->numDataSegments + 1;
		decoder->m_sliceInfo = (Bitstream_Segment_Info *) pRV20In->pDataSegments;
        // continue if last frame
    }

    /* We could calculate 'size' based on the format id and dimensions, */
    /* but let's let the decoder fill it in for us. */
    ps = Decoder_DecAFrame(decoder,&notes, pRV20Packets, pRV20In->dataLength);

	if ((1 == decoder->m_decoding_thread_count) || (1 == decoder->m_nSlices)){
		useDataLen = ((I32)(decoder->m_slice_di[0].m_pBitptr - pRV20Packets - 4 + ((decoder->m_slice_di[0].m_bitpos + 7) >> 3)));
	}else{
		useDataLen = ((I32)(decoder->m_slice_di[1].m_pBitptr - pRV20Packets - 4 + ((decoder->m_slice_di[1].m_bitpos + 7) >> 3)));
	}
	
	pRV20Out->InputUsed = useDataLen;

	if(0 == decoder->m_output_mode){
		if(ISTRUEBPIC(decoder->m_ptype)){
			pRV20Out->width = decoder->m_out_img_width;
			pRV20Out->height = decoder->m_out_img_height;
		}else{
			pRV20Out->width = decoder->m_pre_out_img_width;
			pRV20Out->height = decoder->m_pre_out_img_height;

			decoder->m_pre_out_img_width = decoder->m_out_img_width;
			decoder->m_pre_out_img_height = decoder->m_out_img_height;
		}

		if(VO_VIDEO_FRAME_NULL == decoder->m_ptype){
			pRV20Out->frame_type = decoder->m_ptype;
			pRV20Out->outData[0] = NULL;
			pRV20Out->outData[1] = NULL;
			pRV20Out->outData[2] = NULL;
			pRV20Out->outStride[0] = decoder->m_img_stride;
			pRV20Out->outStride[1] = decoder->m_img_UVstride;
			pRV20Out->outStride[2] = decoder->m_img_UVstride;
		}else{
			if(NULL == decoder->m_pDisplayFrame){
				pRV20Out->frame_type = VO_VIDEO_FRAME_NULL;
				ps = HXR_FAIL;
			}else{
				pRV20Out->frame_type = decoder->m_ptype - 1;
				pRV20Out->outData[0] = decoder->m_pDisplayFrame->m_pYPlane;
				pRV20Out->outData[1] = decoder->m_pDisplayFrame->m_pUPlane;
				pRV20Out->outData[2] = decoder->m_pDisplayFrame->m_pVPlane;
				pRV20Out->outStride[0] = decoder->m_img_stride;
				pRV20Out->outStride[1] = decoder->m_img_UVstride;
				pRV20Out->outStride[2] = decoder->m_img_UVstride;
				pRV20Out->timestamp    = decoder->m_pDisplayFrame->Time;
				pRV20Out->UserData    = decoder->m_pDisplayFrame->UserData;
			}

		}
	}else{
		if(ISTRUEBPIC(decoder->m_ptype)){
			pRV20Out->width = decoder->m_out_img_width;
			pRV20Out->height = decoder->m_out_img_height;
		}else{
			pRV20Out->width = decoder->m_out_img_width;
			pRV20Out->height = decoder->m_out_img_height;

			decoder->m_pre_out_img_width = decoder->m_out_img_width;
			decoder->m_pre_out_img_height = decoder->m_out_img_height;
		}

		pRV20Out->outStride[0] = decoder->m_img_stride;
		pRV20Out->outStride[1] = decoder->m_img_UVstride;
		pRV20Out->outStride[2] = decoder->m_img_UVstride;

		if((1 == decoder->m_first_Iframe_flage) && (VO_VIDEO_FRAME_NULL == decoder->m_pre_ptype)){
			pRV20Out->frame_type = decoder->m_ptype;
			pRV20Out->outData[0] = NULL;
			pRV20Out->outData[1] = NULL;
			pRV20Out->outData[2] = NULL;
		}else if(ISTRUEBPIC(decoder->m_ptype)){
			pRV20Out->frame_type = decoder->m_ptype - 1;
			pRV20Out->outData[0] = decoder->m_pDisplayFrame->m_pYPlane;
			pRV20Out->outData[1] = decoder->m_pDisplayFrame->m_pUPlane;
			pRV20Out->outData[2] = decoder->m_pDisplayFrame->m_pVPlane;
			pRV20Out->timestamp    = decoder->m_pDisplayFrame->Time;
			pRV20Out->UserData     = decoder->m_pDisplayFrame->UserData;
		}else{
			pRV20Out->frame_type = decoder->m_pre_ptype - 1;
			pRV20Out->outData[0] = decoder->m_pRefFrame->m_pYPlane;
			pRV20Out->outData[1] = decoder->m_pRefFrame->m_pUPlane;
			pRV20Out->outData[2] = decoder->m_pRefFrame->m_pVPlane;
			pRV20Out->timestamp    = decoder->m_pRefFrame->Time;
			pRV20Out->UserData     = decoder->m_pRefFrame->UserData;
		}
	}

	if(VO_MEMRC_NO_YUV_BUFFER == ps){
		return VO_MEMRC_NO_YUV_BUFFER;
	}

	return RV_Status_to_HX_RESULT(ps);
}

VO_U32 _RV40toYUV420CustomMessage(VOUINT32 *msg_id, void *global)
{
	VO_U32          returnCode = VO_ERR_NONE;
	struct Decoder  *t;

	t = (struct Decoder *)global;
	returnCode = Decoder_Custom_Message(t, (U32 *)msg_id);

	return returnCode;
}

HX_RESULT _RV40toYUV420HiveMessage(VOUINT32 *msg_id, void *global)
{
	//TDB add program
	return 0;

}
