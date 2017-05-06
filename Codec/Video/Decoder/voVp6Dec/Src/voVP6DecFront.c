/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "voVP6back.h"
#include "voVP6.h"
#include "on2_mem.h"

INT32 VP6_StartDecoder( PB_INSTANCE **pbi, char *p, UINT32 ImageWidth, UINT32 ImageHeight );
INT32 CCONV VP6_DecodeFrameToYUV( PB_INSTANCE *pbi,	DEC_FRAME_INFO* dec_frame_info,	UINT8* VideoBufferPtr,UINT32 ByteCount, const voint64_t time, VO_PTR hCheck);
INT32 VP6_StopDecoder ( PB_INSTANCE **pbi );
void  reset_buf_seq(PB_INSTANCE *dec);


//YU_TBD
#define CHECK_API_ERR(result) (result & (VO_PID_DEC_VP6_BASE))

typedef struct{	
	DEC_FRAME_INFO dec_frame_info;
	VO_CODECBUFFER  pInput;
	VO_U32			UsedLen;
	void *pBackDec;
	VO_PTR phCheck;
	VO_U32 errorflag;
}VP6DEC_FRONT;

void* voVP6_hModule = NULL;

/**
* Initialize a VP6 decoder instance use default settings.
* \param phCodec [out] Return the VP6 Decoder handle.
* \param pQueryMem [in] Input memory allocated outside.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_OUT_OF_MEMORY Out of memory.
*/

VO_U32 VO_API voVP6DecInit(VO_HANDLE * phDec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VP6DEC_FRONT* pDecFront;
//	VO_S32 OutSideMem = 0, MemSize;
//	VO_U8 *OutMem;
#ifdef 	LICENSEFILE
    VO_S32 iRet;    
    VO_PTR  gHandle = NULL;
    if(pUserData)
    {
      if (((pUserData->memflag) & 0xF0 ) == 0x10)
        iRet = voCheckLibInit(&(gHandle), VO_INDEX_DEC_VP6, pUserData->memflag, voVP6_hModule, pUserData->libOperator);
      else
        iRet = voCheckLibInit(&(gHandle), VO_INDEX_DEC_VP6, pUserData->memflag, voVP6_hModule, NULL);
    }
    else {
      iRet = voCheckLibInit(&(gHandle), VO_INDEX_DEC_VP6, 0, voVP6_hModule, NULL);
    }

   if(iRet != VO_ERR_NONE)
   {
    //*phDec = NULL;
    if(gHandle)
    {
     voCheckLibUninit(gHandle);
     gHandle = NULL;
    }
    return iRet;
   }
#else

#endif

	if(VO_VIDEO_CodingVP6 != vType){
		return VO_ERR_DEC_VP6_CODEC_ID;
	}
	pDecFront  = (VP6DEC_FRONT*)duck_memalign (32, sizeof(VP6DEC_FRONT));
	if (!pDecFront )
		return -1;

	//pDecFront = (VP6DEC_FRONT*)calloc(1,sizeof(VP6DEC_FRONT));
	pDecFront->dec_frame_info.image_width = 0;
	pDecFront->dec_frame_info.image_height = 0;

	pDecFront->errorflag = 0;

	if(!pDecFront)
		return VO_ERR_OUTOF_MEMORY;

	*phDec = pDecFront;

#ifdef 	LICENSEFILE
	(pDecFront->phCheck) = gHandle;
#endif

	VP6_StartDecoder((PB_INSTANCE **)&pDecFront->pBackDec, NULL,0,0); //zou

	return VO_ERR_NONE;
}

/**
* Decode one frame data.
* \param hCodec [in] VP6 decoder instance handle, returned by voVP6DecInit().
* \param pInData [in/out] Input buffer pointer and length. 
*  When the function return, pInData->length indicates the actual size consumed by decoder.
* \param pOutData [in/out] Output buffer pointer and length. The buffer alloc and release by decoder
* \param pOutFormat [out] Output format info. The pointer can be NULL if you don't care the format info. 
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_STATUS The decoder is not ready for decode. Usually this indicates more parameters needed. See voVP6SetParameter().
*/
VO_U32 VO_API voVP6DecProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutPutInfo)
{
	//VO_S32 length;
	VO_S32 returncode = 0;
	VP6DEC_FRONT* pDecFront = (VP6DEC_FRONT*)hCodec;
	DEC_FRAME_INFO* dec_frame_info;//YU_TBD

	if(pDecFront==NULL)
		return -1;//VORC_COM_WRONG_STATUS;//YU_TBD
	dec_frame_info = &(pDecFront->dec_frame_info);

	//length = pInData->Buffer[3]<<24|pInData->Buffer[2]<<16|pInData->Buffer[1]<<8|pInData->Buffer[0];

	//if(CHECK_API_ERR(pInData->Length)){
	//	printf("SDK level error, return code 0x%x !\n", pInData->Length);
	//	memset(pOutData, 0, sizeof(VO_VIDEO_BUFFER));
	//	return (VO_S32)pInData->Length;
	//}  //zou

	//if(pDecFront->errorflag)
	//	return -1;


	returncode = VP6_DecodeFrameToYUV(pDecFront->pBackDec, dec_frame_info, pInData->Buffer, pInData->Length, (voint64_t)pInData->Time, pDecFront->phCheck);//pInData->Length

	if(returncode)
	{
		//pDecFront->errorflag = 1;
		return returncode;
	}
	//length +=4;
	//pInData->Length -= length;
	//pInData->Buffer += length;

	pDecFront->UsedLen += pInData->Length;

	pOutData->Buffer[0] = dec_frame_info->image_y;
	pOutData->Buffer[1] = dec_frame_info->image_u;
	pOutData->Buffer[2] = dec_frame_info->image_v;
	pOutData->Stride[0] = dec_frame_info->image_stride_y;
	pOutData->Stride[1] = 
	pOutData->Stride[2] = dec_frame_info->image_stride_uv;
	pOutData->ColorType = VO_COLOR_YUV_PLANAR420;
	pOutData->Time      = pInData->Time;
	pOutData->UserData      = pInData->UserData;

	pOutPutInfo->Format.Width   = dec_frame_info->image_width;
	pOutPutInfo->Format.Height  = dec_frame_info->image_height;
	pOutPutInfo->Format.Type    = dec_frame_info->image_type;
	pOutPutInfo->InputUsed		= pDecFront->UsedLen;

#ifdef		 LICENSEFILE
	voCheckLibCheckVideo(pDecFront->phCheck, pOutData, &pOutPutInfo->Format);
#else

#endif
	//if(pInData->Length < 5){
	pOutPutInfo->Flag = 0;/*no more data*/
	//}else{
	//pOutPutInfo->Flag = 1;/*more video data*/
	return returncode;
}


/**
* Close the VP6 decoder instance, release any resource it used.
* \param hCodec [in] VP6 decoder instance handle, returned by voVP6DecInit().
* \return The function should always return VORC_OK.
*/
VO_U32 VO_API voVP6DecUninit(VO_HANDLE hCodec)
{
	VP6DEC_FRONT* pDecFront = (VP6DEC_FRONT*)hCodec;
	

#ifdef		 LICENSEFILE
	voCheckLibUninit(pDecFront->phCheck);
#else

#endif

	if(pDecFront){
		VP6_StopDecoder((PB_INSTANCE **)&pDecFront->pBackDec);
		duck_free(pDecFront);
	}
	return VO_ERR_NONE;
}


/**
* Set parameter of the decoder instance.
* \param hCodec [in] VP6 decoder instance handle, returned by voVP6DecInit().
* \param nID [in] Parameter ID, Supports: VOID_COM_HEAD_DATA,VOID_VP6_GETFIRSTFRAME
* \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
*/
VO_U32 VO_API voVP6DecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR lValue)
{
	VP6DEC_FRONT* pFront = (VP6DEC_FRONT*)hCodec;
//	VO_S32 length;
	DEC_FRAME_INFO* dec_frame_info;


	if(nID == VO_PID_DEC_VP6_SET_QUERYMEM){
		return VO_ERR_NONE;
	}
	if(pFront == NULL)
		return VO_ERR_INVALID_ARG;

	dec_frame_info = &(pFront->dec_frame_info);

	switch(nID) {
	case VO_PID_COMMON_FLUSH:
		if(!pFront)
			return VO_ERR_INVALID_ARG;
		reset_buf_seq(pFront->pBackDec);
		break;

	case VO_PID_DEC_VP6_SET_CURRENTFRAME_GREY:
		break;

	case VO_PID_DEC_VP6_SET_VIDEO_WIDTH:
		/*dec_frame_info->image_width = (VO_U32)lValue;
		printf("VO_PID_DEC_VP6_SET_VIDEO_WIDTH \n");
		if(dec_frame_info->image_width && dec_frame_info->image_height)
		{
			if(VP6_StartDecoder(&pFront->pBackDec, NULL, dec_frame_info->image_width, dec_frame_info->image_height))
				return VO_ERR_OUTOF_MEMORY;
		}*/
		break;

	case VO_PID_DEC_VP6_SET_VIDEO_HEIGHT:
		/*dec_frame_info->image_height = (VO_U32)lValue;
		printf("VO_PID_DEC_VP6_SET_VIDEO_HEIGHT \n");
		if(dec_frame_info->image_width && dec_frame_info->image_height)
		{
			if(VP6_StartDecoder(&pFront->pBackDec, NULL, dec_frame_info->image_width, dec_frame_info->image_height))
				return VO_ERR_OUTOF_MEMORY;
		}*/
		break;

	case VO_PID_VIDEO_FORMAT:
		{
			VO_VIDEO_FORMAT *pVideoFormat = (VO_VIDEO_FORMAT *)lValue;
			dec_frame_info->image_width = pVideoFormat->Width;
			dec_frame_info->image_height = pVideoFormat->Height;
			if(dec_frame_info->image_width && dec_frame_info->image_height)
			{
				if(VP6_StartDecoder(( PB_INSTANCE **)&pFront->pBackDec, NULL, dec_frame_info->image_width, dec_frame_info->image_height))
					return VO_ERR_OUTOF_MEMORY;
			}
		}
	case VO_PID_COMMON_HEADDATA:
		break;


	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}


/**
* Get parameter of the decoder instance.
* \param hCodec [in] VP6 decoder instance handle, returned by voVP6DecInit().
* \param nID [in] Parameter ID, supports VOID_COM_VIDEO_FORMAT, VOID_COM_VIDEO_WIDTH, VOID_COM_VIDEO_HEIGHT
* \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
*/
VO_U32 VO_API voVP6DecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	VP6DEC_FRONT* pFront = (VP6DEC_FRONT*)hCodec;

	DEC_FRAME_INFO* dec_frame_info;

	if(pFront == NULL)
		return VO_ERR_INVALID_ARG;
	dec_frame_info = &(pFront->dec_frame_info);

	switch(nID) {
	case VO_PID_DEC_VP6_GET_VIDEOFORMAT:
		{
			VO_VIDEO_FORMAT* fmt = (VO_VIDEO_FORMAT*)plValue;
			//YU_TBD
			if(dec_frame_info->image_width != 0)
			{
				fmt->Width  = dec_frame_info->image_width;
				fmt->Height = dec_frame_info->image_height;
				fmt->Type   = dec_frame_info->image_type;//Invalid
			}
			else
			{
				return -1;//VORC_COM_WRONG_STATUS;//YU_TBD
			}
		}
		break;
	case VO_PID_VIDEO_FRAMETYPE:
		{
			PB_INSTANCE *pbi = pFront->pBackDec;
			VO_CODECBUFFER* pInData = (VO_CODECBUFFER *)plValue;

			VP6_LoadFrameHeader(pbi, pInData->Buffer,pInData->Length , 0);
			pInData->Time = pbi->FrameType;
		}
		break;
	case VO_PID_DEC_VP6_GET_VIDEO_WIDTH:
		*((VO_S32*)plValue) = dec_frame_info->image_width;
		break;
	case VO_PID_DEC_VP6_GET_VIDEO_HEIGHT:
		*((VO_S32*)plValue) = dec_frame_info->image_height;
		break;
	case VO_PID_DEC_VP6_MB_SKIP:
		break;
	case VO_PID_DEC_VP6_GET_LASTVIDEOBUFFER:
		break;
	case VO_PID_DEC_VP6_GET_LASTVIDEOINFO:
		break;
	case VO_PID_DEC_VP6_GET_ERRNUM:
		break;
	case VO_PID_VIDEO_ASPECTRATIO:
		*((VO_S32*)plValue) = 0;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voVP6DecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	VP6DEC_FRONT* pFront = (VP6DEC_FRONT*)hDec;
	if(!pFront)
		return VO_ERR_INVALID_ARG;
	pFront->pInput = *pInput;
	pFront->UsedLen = 0;/*clean up used length*/

	//pFront->pInput.Length += 32;

	if(0){
		FILE* fp =fopen("E:/MyResource/Video/clips/VP6/error/T1_483_AVI_VP6_new.vp6","ab");
		fwrite(&pFront->pInput.Length,4,1,fp);
		fwrite(pFront->pInput.Buffer,1,pFront->pInput.Length,fp);
		fclose(fp);
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voVP6DecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutPutInfo)
{
	VP6DEC_FRONT* pFront = (VP6DEC_FRONT*)hDec;

	if(!pFront)
		return VO_ERR_INVALID_ARG;

	if(pOutPutInfo->InputUsed >= pFront->pInput.Length){
		;//return VO_ERR_INPUT_BUFFER_SMALL; //zou
	}

	return voVP6DecProcess(hDec, &(pFront->pInput), pOutput, pOutPutInfo);

}

VO_S32 VO_API voGetVP6DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VO_VIDEO_DECAPI *pVP6Dec = pDecHandle;

	if(!pVP6Dec)
		return VO_ERR_INVALID_ARG;

	pVP6Dec->Init   = voVP6DecInit;
	pVP6Dec->Uninit = voVP6DecUninit;
	pVP6Dec->SetParam = voVP6DecSetParameter;
	pVP6Dec->GetParam = voVP6DecGetParameter;
	pVP6Dec->SetInputData = voVP6DecSetInputData;
	pVP6Dec->GetOutputData = voVP6DecGetOutputData;

	return VO_ERR_NONE;

}