/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2007		            *
*								 	                                    *
************************************************************************/
#include "voMPEG2.h"
#include "voMpeg2Decoder.h"
#include "voMpegMem.h"
#include "voMpegBuf.h"
#include "voMpegReadbits.h"
#ifdef LICENSEFILE 
#include "voCheck.h"
#endif

//#define DEBUG_OUTPUT_DATA

#ifdef DEBUG_OUTPUT_DATA
#include <Windows.h>
#endif

//YU_TBD
#define CHECK_API_ERR(result) (result & (VO_ERR_BASE))

VO_HANDLE g_hMPEG2DecInst = NULL;

#ifdef MEM_LEAKAGE
int malloc_count = 0;
int free_count = 0;
#endif // MEM_LEAKAGE

typedef struct{	
	VO_VOID* pDecCore;
	VO_VIDEO_BUFFER OutData;
	VO_CODECBUFFER  InputData;
	VO_VIDEO_OUTPUTINFO OutPutInfo;
	VO_U32 InputUsed;
	VO_U8* pInputData;
	VO_U32 Set_flage;

}MPEG2DEC_FRONT;

extern Image*  buf_seq_ctl(FifoType *priv, Image* img , const VO_U32 flag);
/* After flush*/
static VO_VOID  reset_buf_seq(MpegDecode *dec)
{
	FifoType *priv = &dec->privFIFO; 

	VO_U32 i, w_idx, r_idx;

	for(i = 0; i < MAXFRAMES; i++){
		w_idx = priv->w_idx;
		r_idx = priv->r_idx;

		do{
			if(priv->img_seq[r_idx] == dec->decReferenceFrames[i])
				break;
			r_idx = (r_idx + 1) % FIFO_NUM;
		}while(r_idx != w_idx);

		if(r_idx == w_idx)
			buf_seq_ctl(priv, dec->decReferenceFrames[i], FIFO_WRITE);
	}
}

/**
* Initialize a MPEG2 decoder instance use default settings.
* \param phCodec [out] Return the MPEG2 Decoder handle.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_OUT_OF_MEMORY Out of memory.
*/
VO_U32 VO_API voMPEG2DecInit(VO_HANDLE *phCodec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	MPEG2DEC_FRONT* pDecFront;
	MpegDecode* dec;
	VO_MEM_OPERATOR *pMemOperator;
	VO_MEM_OPERATOR MemOperator;
	VO_PTR phLicenseCheck = NULL;
	VO_U32 voRC = VO_ERR_NONE;
	//VO_S32 OutSideMem = 0;

#if FF_DUMP
	fLog = fopen("c:/mpeg1Dump.txt","w");
	ff_debug = FF_DUMP_ALL;
#endif

	if(VO_VIDEO_CodingMPEG2 != vType){
		return VO_ERR_DEC_MPEG2_HEADER_ERR;
	}

	if(!pUserData){
		pMemOperator = &MemOperator;
		pMemOperator->Set = NULL;
		SetMem((VO_U8 *)pMemOperator, 0,sizeof(VO_MEM_OPERATOR),pMemOperator);
	}else if (VO_IMF_USERMEMOPERATOR != (pUserData->memflag & 0x0f)){
		pMemOperator = &MemOperator;
		pMemOperator->Set = NULL;
		SetMem((VO_U8 *)pMemOperator, 0,sizeof(VO_MEM_OPERATOR),pMemOperator);
	}else if(VO_IMF_USERMEMOPERATOR == (pUserData->memflag & 0x0f)){
		pMemOperator = (VO_MEM_OPERATOR *)pUserData->memData;
	}

#ifdef LICENSEFILE 
	if (NULL == pUserData){
		voRC = voCheckLibInit(&phLicenseCheck, VO_INDEX_DEC_MPEG2, 0, g_hMPEG2DecInst, NULL);
	}else if (pUserData->memflag & 0xf0){
		voRC = voCheckLibInit(&phLicenseCheck, VO_INDEX_DEC_MPEG2, pUserData->memflag, g_hMPEG2DecInst,pUserData->libOperator);
	}else{
		voRC = voCheckLibInit(&phLicenseCheck, VO_INDEX_DEC_MPEG2, pUserData->memflag, g_hMPEG2DecInst, NULL);
	}
	if (voRC != VO_ERR_NONE)
	{
		if (phLicenseCheck)
		{
			voCheckLibUninit(phLicenseCheck);
		}
		return voRC;
	}
#endif

	pDecFront = (MPEG2DEC_FRONT *)(MallocMem(sizeof(MPEG2DEC_FRONT),CACHE_LINE, pMemOperator));
	if(!pDecFront){
		return VO_ERR_OUTOF_MEMORY;
	}
	SetMem((VO_U8 *)pDecFront,0,sizeof(MPEG2DEC_FRONT),pMemOperator);

	pDecFront->pDecCore = (MpegDecode *)(MallocMem(sizeof(MpegDecode),CACHE_LINE, pMemOperator));
	if(NULL == pDecFront->pDecCore)
		return VO_ERR_OUTOF_MEMORY;
	SetMem((VO_U8 *)pDecFront->pDecCore,0,sizeof(MpegDecode),pMemOperator);
	dec = (MpegDecode *)pDecFront->pDecCore;

	dec->aspect_ratio = 2;
	dec->first_Iframe_flage = 1;
	dec->uv_interlace_flage = 1;
	dec->privFIFO.r_idx = 0;
	dec->privFIFO.w_idx = MAXFRAMES;
	//default as mpeg1
	dec->picture_structure = PICT_FRAME;//default
	dec->mv_type = MT_FRAME;//default

#ifdef LICENSEFILE 
	dec->phCheck = phLicenseCheck;
#endif

#ifdef REF_DCT
	Initialize_Decoder();
#endif
	*phCodec = pDecFront;

	if(pUserData){
		if(VO_IMF_USERMEMOPERATOR == (pUserData->memflag & 0x0f)){
			dec->uv_interlace_flage = 0;

			dec->memoryOperator.Alloc     = pMemOperator->Alloc;
			dec->memoryOperator.Free      = pMemOperator->Free;
			dec->memoryOperator.Set       = pMemOperator->Set;
			dec->memoryOperator.Move      = pMemOperator->Move;
			dec->memoryOperator.Compare   = pMemOperator->Compare;
			dec->memoryOperator.Check     = pMemOperator->Check;
			dec->memoryOperator.Copy      = pMemOperator->Copy;
		}
	}

	return VO_ERR_NONE;

}

/**
* Decode one frame data.
* \param hCodec [in] MPEG2 decoder instance handle, returned by voMPEG2DecInit().
* \param pInData [in/out] Input buffer pointer and length. 
*  When the function return, pInData->length indicates the actual size consumed by decoder.
* \param pOutData [in/out] Output buffer pointer and length. The buffer alloc and release by decoder
* \param pOutFormat [out] Output format info. The pointer can be NULL if you don't care the format info. 
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_STATUS The decoder is not ready for decode. Usually this indicates more parameters needed. See voMPEG2SetParameter().
*/

VO_U32 VO_API voMPEG2DecProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutPutInfo)
{
	VO_S32 length;
	VO_S32 Result;
	const VO_U8* Ptr;
	MPEG2DEC_FRONT* pDecFront = (MPEG2DEC_FRONT*)hCodec;
	MpegDecode* dec;

	if(NULL == pDecFront)
		return VO_ERR_WRONG_STATUS;

	dec = (MpegDecode *)pDecFront->pDecCore;

	Ptr = pInData->Buffer;//huwei 20101227 stability
// 	dec->timeStamp = pInData->Time;
	dec->user_data.time_stamp  = pInData->Time;
	dec->user_data.UserData = pInData->UserData;
	pInData->Buffer = pDecFront->pInputData;

	Result = dec_frame(dec, pInData->Buffer, pInData->Length, 0, pOutData, pOutPutInfo);
	length = (VO_S32)(bytepos(dec)-Ptr-4);
	

	if((dec->frame_state == FRAME_DECODING)|| (dec->frame_state == FRAME_FINISH))
		length += 4;

	if((dec->frame_state == FRAME_FINISH)||(dec->frame_state == FRAME_READY)||CHECK_API_ERR(Result)){
		dec->frame_state = FRAME_BEGIN;
	}

	if (length <= 0 || CHECK_API_ERR(length)){
		length = 4;
		Result = VO_ERR_FAILED;
// 		return VO_ERR_FAILED;
	}

	pDecFront->pInputData += length;
	pDecFront->InputUsed += length;
	pOutPutInfo->InputUsed = pDecFront->InputUsed;
	pOutPutInfo->Flag = 0;

	//if(pInData->Length > pOutPutInfo->InputUsed){
	//	pOutPutInfo->Flag = 1;
	//}else{
	//	pOutPutInfo->Flag = 0;
	//}

	return Result;
}


/**
* Close the MPEG2 decoder instance, release any resource it used.
* \param hCodec [in] MPEG2 decoder instance handle, returned by voMPEG2DecInit().
* \return The function should always return VORC_OK.
*/
VO_U32 VO_API voMPEG2DecUninit(VO_HANDLE hCodec)
{
	MPEG2DEC_FRONT* pDecFront = (MPEG2DEC_FRONT*)hCodec;
	MpegDecode* dec;
	VO_S32 i;

	VO_MEM_OPERATOR MemOperator;

	if(pDecFront == NULL)
		return VO_ERR_INVALID_ARG;

	MemOperator.Free = NULL;
	dec = (MpegDecode *)pDecFront->pDecCore;

	MemOperator.Free = dec->memoryOperator.Free;

#ifdef LICENSEFILE
	voCheckLibUninit(dec->phCheck);
#endif

	if(!(dec->amd_malloc_flag)){
		if(dec){
			if(dec->blockptr){
				FreeMem(dec->blockptr, &MemOperator);			
				dec->blockptr = NULL;
				dec->mc_dst[0] = NULL;
				dec->numLeadingZeros = NULL;
			}

			if (dec->block_buffer){
				FreeMem(dec->block_buffer, &MemOperator);
				dec->block_buffer = NULL;
			}
		}

#ifdef IPP_EDIT
		if(dec->pBuff){
			FreeMem(dec->pBuff, &MemOperator);
			dec->pBuff = NULL;
		}
#endif //IPP_EDIT

#ifdef SKIP_IDENTICAL_COPY
		if (dec->frBCopyFlag){
			mpeg_free(dec->frBCopyFlag);
			dec->frBCopyFlag = NULL;
		}
#endif // SKIP_IDENTICAL_COPY

		if(dec->memoryShare.Init){
			dec->memoryShare.Uninit(VO_INDEX_DEC_MPEG2);
		}

		for(i = 0; i < MAXFRAMES; i++){
			if(dec->decReferenceFrames[i]){
				if(!(dec->amd_malloc_flag)){
					if(!dec->memoryShare.Init){
						image_destroy(dec->decReferenceFrames[i], dec->img_stride, dec->img_height, dec);
					}					
					FreeMem(dec->decReferenceFrames[i], &MemOperator);					
					dec->decReferenceFrames[i] = NULL;
				}
			}
		}

		for(i = 0; i < MAXFRAMES; i++){
			dec->privFIFO.img_seq[i] = NULL;
		}

#ifdef LICENSEFILE
		FreeMem(dec->pDisplayFrame, &MemOperator);		
#endif
		if(!(dec->amd_malloc_flag)){
			FreeMem(dec, &MemOperator);	
		}

		FreeMem(pDecFront, &MemOperator);
		
	}
#if FF_DUMP
	if(fLog)
		fclose(fLog);
#endif

	return VO_ERR_NONE;
}

#ifdef AMD_SDK
/*************************************************************************************************************************
* Set parameter of the decoder instance.
* param hCodec [in] MPEG2 decoder instance handle, returned by voMPEG2DecInit().
* param nID [in] Parameter ID, Supports: VOID_COM_HEAD_DATA,VOID_MPEG2_GETFIRSTFRAME
* param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
* retval VORC_OK Succeeded.
* retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
*************************************************************************************************************************/
VO_VOID QueryMem(VOVIDEODECQUERYMEM * pQueryMem)
{
	VO_S32 MemSize;
	VO_S32 width, height, mb_xsize, mb_ysize, pos_end, mb_count;

	VO_S32 block_size = 8;

	pQueryMem->decNumRefFrames = 4;
	width = (pQueryMem->decPictureWidth+15)&0xfffffff0;
	height = (pQueryMem->decPictureHeight+15)&0xfffffff0;

	mb_xsize = (width + 15) / 16;
	mb_ysize = (height + 15) / 16;	

	pQueryMem->decReferenceFrameWidth = mb_xsize*16 + EDGE_SIZE*2; 
	pQueryMem->decReferenceFrameHeight = mb_ysize*16 + EDGE_SIZE*2;
	pQueryMem->decPrivateDataSize = 64*sizeof(VO_S16) + 8*block_size*6*sizeof(VO_U8) 
		+ 256*sizeof(VO_U8) + 3*sizeof(Image) + sizeof(MpegDecode);
}
#endif//AMD_SDK

VO_U32 VO_API voMPEG2DecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	MPEG2DEC_FRONT* pFront = (MPEG2DEC_FRONT*)hCodec;
	VO_S32 length;
	MpegDecode* dec;

	if(pFront == NULL)
		return VO_ERR_INVALID_ARG;

	dec = (MpegDecode *)pFront->pDecCore;

	switch(nID) {
	case VO_PID_COMMON_HEADDATA:
		{
			VO_CODECBUFFER *pInData = (VO_CODECBUFFER *)plValue;
			VO_S32 Result;
			VO_U8* Ptr = pInData->Buffer;
			//default as mpeg1
			dec->picture_structure = PICT_FRAME;//default
			dec->mv_type = MT_FRAME;//default
			Result = dec_frame(dec, pInData->Buffer, pInData->Length, 1, NULL, NULL);
			length = (VO_S32)(bytepos(dec)-Ptr-4);
			if(dec->frame_state == FRAME_DECODING)
				length += 4;
			if(!GET_VALID_SEQ(dec->mpeg2_flag))
				return VO_ERR_DEC_MPEG2_HEADER_ERR;
			else{
				pInData->Length = length;
			}
		}
		break;
	case VO_PID_DEC_MPEG2_GETFIRSTFRAME:
		//		dec_frame_info->IsGetFirstFrame = lValue;
		break;
	case VO_PID_VIDEO_DRM_FUNC:
		//dec->fp_postIDCTCallback = (postIDCTCallbackPtr)plValue;//huwei 20110901 add new ID
		//dec->callback_flage = 1;

		//if(!dec->block_buffer){
		//	dec->block_buffer = (VO_S16 *)(MallocMem(64*sizeof(VO_S16), CACHE_LINE, &dec->memoryOperator));
		//}
		//break;
		{
			DRMclientData *DRMData = (DRMclientData *)plValue; 

			if(IRDETO_DRM == DRMData->vendorID){
				dec->fp_postIDCTCallback = (postIDCTCallbackPtr)DRMData->callback1;
				dec->callback_flage = 1;
				if(!dec->block_buffer){
					dec->block_buffer = (VO_S16 *)(MallocMem(64*sizeof(VO_S16), CACHE_LINE, &dec->memoryOperator));
				}
			}
		}
		break;
		
	case VO_PID_COMMON_FLUSH:
	case VO_PID_DEC_MPEG2_FLUSH:
		{
			if(*((VO_S32 *)plValue)){
				dec->first_Iframe_flage = 1;
				reset_buf_seq(dec);
			}
		}
		break;
	case VO_PID_DEC_MPEG2_DOWMSAMPLE:
		dec->ds_dec =*((VO_S32 *)plValue);
		dec->ds_Bframe  = 0;

		if(2 == *((VO_S32 *)plValue)){
			dec->ds_dec = 0;
			dec->ds_Bframe  = 1;
		}

		dec->img_stride = dec->img_width>>DOWNSAMPLE_FLAG(dec);
		break;
	case VO_PID_VIDEO_VIDEOMEMOP:
		{
			VO_MEM_VIDEO_OPERATOR *pMemShare;

			pMemShare = (VO_MEM_VIDEO_OPERATOR *)plValue;

			dec->memoryShare.GetBufByIndex = pMemShare->GetBufByIndex;
			dec->memoryShare.Init = pMemShare->Init;
			dec->memoryShare.Uninit = pMemShare->Uninit;

			dec->uv_interlace_flage = 1;

		}
		break;
	case VO_PID_VIDEO_OUTPUTMODE:
		{
			if(*((VO_S32 *)plValue)){
				dec->outputMode = 1;
			}else{
				dec->outputMode = 0;
			}

		}
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}


/**
* Get parameter of the decoder instance.
* \param hCodec [in] MPEG2 decoder instance handle, returned by voMPEG2DecInit().
* \param nID [in] Parameter ID, supports VOID_COM_VIDEO_FORMAT, VOID_COM_VIDEO_WIDTH, VOID_COM_VIDEO_HEIGHT
* \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
*/
VO_U32 VO_API voMPEG2DecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	MPEG2DEC_FRONT* pFront = (MPEG2DEC_FRONT*)hCodec;
	MpegDecode* dec ;
	VO_U32 Result = VO_ERR_NONE;

	if(pFront == NULL)
		return VO_ERR_INVALID_ARG;

	dec = (MpegDecode *)pFront->pDecCore;

	switch(nID) {
	case VO_PID_DEC_MPEG2_GET_VIDEOFORMAT:
		{
			VO_VIDEO_FORMAT* fmt = (VO_VIDEO_FORMAT*)plValue;
			if(dec->img_width!=0)
			{
				fmt->Width = dec->out_img_width >> DOWNSAMPLE_FLAG(dec);
				fmt->Type = 5;//Invalid

				if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
					fmt->Height = (dec->out_img_height>>1);
				else
					fmt->Height = dec->out_img_height;				
			}
			else
			{
				return VO_ERR_WRONG_STATUS;
			}
		}
		break;
	case VO_PID_DEC_MPEG2_GET_VIDEO_WIDTH:
		(*(VO_S32*)plValue) = dec->out_img_width>>DOWNSAMPLE_FLAG(dec);
		break;
	case VO_PID_DEC_MPEG2_GET_VIDEO_HEIGHT:
		{
			if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
				(*(VO_S32*)plValue) = (dec->out_img_height>>1);
			else
				(*(VO_S32*)plValue) = dec->out_img_height;

		}
		break;
	case VO_PID_DEC_MPEG2_MB_SKIP:
		//		if(dec->mb_skip_flag){
		//			dec->mb_skip_flag = 0;
		//			*plValue = (LONG)dec->mb_info;
		//		}else{
		//			if(dec->prediction_type == B_VOP){
		//				*plValue = (LONG)dec->mb_info_b;
		//			}else{
		//				*plValue = (LONG)dec->mb_info_bak;
		//			}
		//		}
		break;
	case VO_PID_DEC_MPEG2_GETLASTOUTVIDEOBUFFER:
		{
			VO_VIDEO_BUFFER* last_buf = (VO_VIDEO_BUFFER*)plValue;
#ifdef LICENSEFILE
			VO_VIDEO_BUFFER outBuffer;
			VO_VIDEO_FORMAT outFormat; 
#endif
			last_buf->Buffer[0] = dec->reference_frame->y;
			last_buf->Buffer[1] = dec->reference_frame->u;
			last_buf->Buffer[2] = dec->reference_frame->v;

			last_buf->Stride[0] = dec->img_stride;
			last_buf->Stride[1] = dec->img_stride_uv;
			last_buf->Stride[2] = dec->img_stride_uv;
			last_buf->ColorType = VO_COLOR_YUV_PLANAR420;

// 			last_buf->Time = dec->timeStampPre;
			last_buf->Time = dec->user_data_pre.time_stamp;
			last_buf->UserData  = dec->user_data_pre.UserData;

#ifdef LICENSEFILE
			outBuffer.Buffer[0] = last_buf->Buffer[0];
			outBuffer.Buffer[1] = last_buf->Buffer[1];
			outBuffer.Buffer[2] = last_buf->Buffer[2];
			outBuffer.Stride[0] = last_buf->Stride[0]; 
			outBuffer.Stride[1] = last_buf->Stride[1]; 
			outBuffer.Stride[2] = last_buf->Stride[2];
			outBuffer.ColorType = last_buf->ColorType;
			outBuffer.Time      = last_buf->Time;

			outFormat.Width  = dec->img_width>>DOWNSAMPLE_FLAG(dec);
			outFormat.Height = dec->img_height;
			outFormat.Type   = dec->prediction_type - 1;
			voCheckLibCheckVideo(dec->phCheck, &outBuffer, &outFormat);
#endif
		}
		break;
	case VO_PID_VIDEO_ASPECTRATIO:	//video aspect ration
		{
			VO_S32 width = dec->out_img_width, height = dec->out_img_height;
			switch (dec->aspect_ratio)
			{
			case VO_RATIO_43:
				width *= 4;
				height *= 3;
				break;
			case VO_RATIO_169:
				width *= 16;
				height *= 9;
				break;
			case VO_RATIO_21:
				width *= 2;
				break;
			default:	//0 or 1:1
				;
			}
			if (width == height)
			{
				*((VO_S32 *)plValue) = VO_RATIO_11;
			}
			else if (width*3 == height*4)
			{
				*((VO_S32 *)plValue) = VO_RATIO_43;
			}
			else if (width*9 == height*16)
			{
				*((VO_S32 *)plValue) = VO_RATIO_169;
			}
			else if (width == height*2)
			{
				*((VO_S32 *)plValue) = VO_RATIO_21;
			}
			else
			{
				*((VO_S32 *)plValue) = (width<<16)|height;
			}
		}
		break;
	case VO_PID_DEC_MPEG2_ASPECT_RATIO:		//pixel aspect ration
		if (dec->aspect_ratio <= 4)
		{
			(*(VO_S32*)plValue) = dec->aspect_ratio;
		}
		else
		{
			(*(VO_S32*)plValue) = 0;
		}
		break;
	case VO_PID_DEC_MPEG2_GET_FRAME_RATE:
		{
			VO_S32 frame_rate_code = dec->frame_rate;
			switch(frame_rate_code){
	        case 1:
				(*(float*)plValue) = 23.976f;
				break;
			case 2:
				(*(float*)plValue) = 24;
				break;
			case 3:
				(*(float*)plValue) = 25;
				break;
			case 4:
				(*(float*)plValue) = 29.97f;
				break;
			case 5:
				(*(float*)plValue) = 30;
				break;
			case 6:
				(*(float*)plValue) = 50;
				break;
			case 7:
				(*(float*)plValue) = 59.94f;
				break;
			case 8:
				(*(float*)plValue) = 60;
				break;
			default:
				(*(float*)plValue) = 0;
				return VO_ERR_FAILED;
			}			
		}
		break;
	case VO_PID_DEC_MPEG2_GET_PROGRESSIVE:
		{
			(*(VO_S32*)plValue) = dec->displayFrameProgressive;
		}
		break;
	case VO_PID_DEC_MPEG2_GETLASTOUTVIDEOFORMAT:
		{
			VO_VIDEO_FORMAT* last_format = (VO_VIDEO_FORMAT*)plValue;

			last_format->Width  = dec->out_img_width>>DOWNSAMPLE_FLAG(dec);
			last_format->Height = dec->out_img_height;
			last_format->Type   = dec->prediction_type;
		}
		break;
	case VO_PID_VIDEO_FRAMETYPE:
		{
			MpegDecode* dec;
			MPEG2DEC_FRONT* pFront =  (MPEG2DEC_FRONT*)hCodec;
			VO_CODECBUFFER* pInData = (VO_CODECBUFFER*)plValue;/*huwei 20090709 get frame type*/

			dec = (MpegDecode *)pFront->pDecCore;

// 			dec->bits = 0;
// 			dec->bitpos = 32;
// 			dec->bitptr = pInData->Buffer;
// 			dec->bitend = pInData->Buffer + pInData->Length + 4;
			InitBitStream(dec, pInData->Buffer, pInData->Length);

			Result = get_frame_type(dec);
			pInData->Time = dec->prediction_type - 1;
		}
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return Result;
}

VO_U32 VO_API voMPEG2DecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	MPEG2DEC_FRONT* pFront = (MPEG2DEC_FRONT*)hDec;
//	VO_CODECBUFFER  Input;


	if(!pFront)
		return VO_ERR_INVALID_ARG;

#ifdef DEBUG_OUTPUT_DATA
	{
		char ss[100];

		sprintf(ss, "voMPEG2DecSetInputData\r\n");
		OutputDebugString(ss);
		sprintf(ss, "pInput->Length =%d \r\n", pInput->Length);
		OutputDebugString(ss);
	}
#endif

	pFront->InputData.Buffer = pInput->Buffer;
	pFront->InputData.Length = pInput->Length;
	pFront->InputData.Time   = pInput->Time;

	pFront->InputUsed  = 0;
	pFront->pInputData = pInput->Buffer;
	pFront->OutPutInfo.InputUsed = 0;
	pFront->Set_flage  = 1;

	return VO_ERR_NONE;
}

VO_U32 VO_API voMPEG2DecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutPutInfo)
{
	MPEG2DEC_FRONT* pFront = (MPEG2DEC_FRONT*)hDec;
	VO_U32 Result = VO_ERR_NONE;
	MpegDecode* dec;

	dec = (MpegDecode*)pFront->pDecCore;

	if(!pFront)
		return VO_ERR_INVALID_ARG;

#ifdef DEBUG_OUTPUT_DATA
	{
		char ss[100];
		sprintf(ss, "voMPEG2DecGetOutputData dec->frame_num = %d \r\n",dec->frame_num);
		OutputDebugString(ss);
		sprintf(ss, "pFront->InputData.Length =%d \r\n", pFront->InputData.Length);
		OutputDebugString(ss);
	}
#endif

	if((pFront->OutPutInfo.InputUsed >= pFront->InputData.Length)||(!pFront->Set_flage )){
		pOutPutInfo->Flag = ~VO_VIDEO_OUTPUT_MORE;
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	pFront->InputData.Length = pFront->InputData.Length - pFront->OutPutInfo.InputUsed;

	pFront->Set_flage = 0;

	Result = voMPEG2DecProcess(hDec, &pFront->InputData, &(pFront->OutData), &(pFront->OutPutInfo));

	if (VO_VIDEO_FRAME_NULL == pFront->OutPutInfo.Format.Type){
		pFront->OutData.Buffer[0] = NULL;
		pFront->OutData.Buffer[1] = NULL;
		pFront->OutData.Buffer[2] = NULL;
	}

#ifdef LICENSEFILE
	if(VO_VIDEO_FRAME_NULL != pFront->OutPutInfo.Format.Type){
		dec->pDisplayFrame->y = pFront->OutData.Buffer[0];
		dec->pDisplayFrame->u = pFront->OutData.Buffer[1];
		dec->pDisplayFrame->v = pFront->OutData.Buffer[2];
		dec->display_ptype = pFront->OutPutInfo.Format.Type + 1;
		voCheckLibCheckVideo(dec->phCheck, &(pFront->OutData), &(pFront->OutPutInfo.Format));
	}else{
		dec->display_ptype = pFront->OutPutInfo.Format.Type;
	}	
#endif

	*pOutput = pFront->OutData;
	*pOutPutInfo = pFront->OutPutInfo;

#ifdef DEBUG_OUTPUT_DATA
	{
		char ss[100];
		sprintf(ss, "pOutPutInfo->InputUsed =%d \r\n", pOutPutInfo->InputUsed);
		OutputDebugString(ss);
	}
#endif
	return Result;
}

VO_S32 VO_API voGetMPEG2DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VO_VIDEO_DECAPI *pMpeg2Dec = pDecHandle;

	if(!pMpeg2Dec)
		return VO_ERR_INVALID_ARG;

	pMpeg2Dec->Init   = voMPEG2DecInit;
	pMpeg2Dec->Uninit = voMPEG2DecUninit;
	pMpeg2Dec->SetParam = voMPEG2DecSetParameter;
	pMpeg2Dec->GetParam = voMPEG2DecGetParameter;
	pMpeg2Dec->SetInputData = voMPEG2DecSetInputData;
	pMpeg2Dec->GetOutputData = voMPEG2DecGetOutputData;

	return VO_ERR_NONE;

}