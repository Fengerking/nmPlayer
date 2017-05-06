/**************************************************************************
*																		  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		              *
*								 										  *
***************************************************************************/
#include <stdlib.h>

#include "decoder.h"
#include "voRealVideo.h"
#include "rv_backend.h"
#include "rv30dec.h"
#include "rv_decode.h"
#include "basic.h"
#include "tables.h"
#include "rv30backend.h"


/***************************************************************************
* Initialize a RealVideo decoder instance use default settings.	           *
* \param phCodec [out] Return the RealVideo Decoder handle.                *
* \retval VORC_OK Succeeded.                                               *
* \retval VORC_COM_OUT_OF_MEMORY Out of memory.                            *
****************************************************************************/
#ifdef LICENSEFILE 
#include "voCheck.h"
#endif

#if defined(VODEBUG)||defined(_DEBUG)
extern int g_inputFrameNum;
#endif

#if 0
//YUR
#include <android/log.h>

#define LOG_TAG "voRVDecSDK"

//#if !defined LOGW
#define LOGW(...) ((int)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
//#endif

//#if !defined LOGI
#define LOGI(...) ((int)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
//#endif

//#if !defined LOGE
#define LOGE(...) ((int)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
//#endif
#endif

VO_HANDLE g_hRVDecInst = NULL;

#ifdef MEM_LEAKAGE
I32 malloc_count = 0;
I32 free_count = 0;
#endif // MEM_LEAKAGE

typedef struct{	
	VO_VOID* pDecCore;
	VO_VIDEO_BUFFER OutData;
	VO_CODECBUFFER  InputData;
	VO_VIDEO_OUTPUTINFO OutPutInfo;
}RVDEC_FRONT;

VO_U32 VO_API voRealVideoDecInit(VO_HANDLE *phCodec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	rv_backend* pDec;
	struct Decoder *t;
	RVDEC_FRONT* pDecFront;
	VO_MEM_OPERATOR *pMemOperator;
	VO_MEM_OPERATOR MemOperator;
	VO_U32 i=0,j=0;
#if defined(LICENSEFILE)
	VO_PTR pLicHandle;
	VO_U32 err;
	VO_LIB_OPERATOR *pLibOp = NULL;
#endif

	if(VO_VIDEO_CodingRV != vType){
		return VO_ERR_NOT_IMPLEMENT;
	}
#if defined(LICENSEFILE)
	if(pUserData) {
		if(pUserData->memflag & 0xF0)
			pLibOp = pUserData->libOperator;
	}
	err = voCheckLibInit(&pLicHandle, VO_INDEX_DEC_RV, pUserData?pUserData->memflag:0, g_hRVDecInst, pLibOp);
	if(err != VO_ERR_NONE)
	{
		*phCodec = NULL;
		if(pLicHandle)
		{
			voCheckLibUninit(pLicHandle);
			pLicHandle = NULL;
		}
		return err;
	}
#endif

    voVLog("voRealVideoDecInit start!\n");

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

	pDecFront = (RVDEC_FRONT *)(MallocMem(sizeof(RVDEC_FRONT),CACHE_LINE, pMemOperator));
	if(!pDecFront){
		return VO_ERR_OUTOF_MEMORY;
	}
	SetMem((VO_U8 *)pDecFront,0,sizeof(RVDEC_FRONT),pMemOperator);

	pDecFront->pDecCore = (VO_VOID *)(MallocMem(sizeof(rv_backend), CACHE_LINE, pMemOperator));
	if(!pDecFront->pDecCore){
		return VO_ERR_OUTOF_MEMORY;
	}

	pDec = pDecFront->pDecCore;
	SetMem((VO_U8 *)pDec, 0, sizeof(rv_backend), pMemOperator);

	t = (struct Decoder *)MallocMem(sizeof(struct Decoder),CACHE_LINE, pMemOperator);
	if(!t){
		return VO_ERR_OUTOF_MEMORY;
	}
	SetMem((VO_U8 *)t,0,sizeof(struct Decoder),pMemOperator);

	t->m_pMemOP = (VO_MEM_OPERATOR *)MallocMem(sizeof(VO_MEM_OPERATOR),CACHE_LINE, pMemOperator);
	if(!t->m_pMemOP){
		return VO_ERR_OUTOF_MEMORY;
	}

	SetMem((VO_U8 *)t->m_pMemOP,0,sizeof(VO_MEM_OPERATOR),pMemOperator);
	t->m_enable_deblocking_flage = 1;
	t->m_decoding_thread_count   = 1;
#ifdef FBM_INTERNAL_CACHE_SUPPORT
  t->m_priv.buf_num = MAXFRAMES;
#endif

	if(pUserData){
		if(VO_IMF_USERMEMOPERATOR == (pUserData->memflag & 0x0f)){
			t->m_pMemOP->Alloc     = pMemOperator->Alloc;
			t->m_pMemOP->Free      = pMemOperator->Free;
			t->m_pMemOP->Set       = pMemOperator->Set;
			t->m_pMemOP->Move      = pMemOperator->Move;
			t->m_pMemOP->Compare   = pMemOperator->Compare;
			t->m_pMemOP->Check     = pMemOperator->Check;
			t->m_pMemOP->Copy      = pMemOperator->Copy;
		}
	}

	pDec->pDecodeState = (VO_VOID *)t;
	pDec->fpCustomMessage = _RV40toYUV420CustomMessage;
	*phCodec = pDecFront;
#ifdef LICENSEFILE
	t->m_phCheck = pLicHandle;
#endif

	//zou 
	// To avoid the conflicts with GPL license
	// we generate the dsc_to_l0123  in voRealVideoDecInit instead of using a const table.
	for(i=0;i< 300;i++)
	{
		if( (i & 0x0f)==0x03 || (i &0x0f) == 0x07 || (i &0x0f) > 0xa || (i>>4) == 0x3 || (i>>4) == 0x7 || (i>>4) == 0xb)
		{
		}
		else
		{
			dsc_to_l0123[j++] = i;
			if(j>108)
				break;
		}
	}
	

    voVLog("voRealVideoDecInit done!\n");
	
	return VO_ERR_NONE;
}

/*************************************************************************************************************
* Close the RealVideo decoder instance, release any resource it used.                                   *
* \param hCodec [in] RealVideo decoder instance handle, returned by voRealVideoDecInit().     *
* \return The function should always return VORC_OK.                                                          *
**************************************************************************************************************/
VO_U32 VO_API voRealVideoDecUninit(VO_HANDLE hCodec)
{
	RVDEC_FRONT* pDecFront = (RVDEC_FRONT *)hCodec;
	rv_backend* pDec;
	struct Decoder *t;
	VO_MEM_OPERATOR MemOperator;

	MemOperator.Free = NULL;

    voVLog("voRealVideoDecUninit start!\n");

	if(NULL == pDecFront){
		return VO_ERR_FAILED;//TBD
	}

	pDec = (rv_backend *)pDecFront->pDecCore;

	if (NULL == pDec){
		return VO_ERR_FAILED;
	}

	t = (struct Decoder*)pDec->pDecodeState;

	if (NULL == t){
		return VO_ERR_FAILED;
	}

	MemOperator.Free = t->m_pMemOP->Free;

#ifdef LICENSEFILE
	voCheckLibUninit(t->m_phCheck);
#endif

	rv_decode_destroy(pDec);
	FreeMem(pDecFront, &MemOperator);	
	pDecFront = NULL;

    voVLog("voRealVideoDecUninit done!\n");

	return VO_ERR_NONE;
}


/*****************************************************************************************************************************
* Decode one frame data.
* param hCodec [in] RealVideo decoder instance handle, returned by voRealVideoDecInit().
* param pInData [in/out] Input buffer pointer and length. 
* When the function return, pInData->length indicates the actual size consumed by decoder.
* param pOutData [in/out] Output buffer pointer and length. The buffer alloc and release by decoder
* retval VORC_OK Succeeded.
* retval VORC_COM_WRONG_STATUS The decoder is not ready for decode. 
*Usually this indicates more parameters needed. See voRealVideoSetParameter().
********************************************************************************************************************************/
VO_U32 VO_API voRealVideoDecProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_VIDEO_BUFFER *pOutData, VO_VIDEO_OUTPUTINFO *pOutPutInfo)
{
	RVDEC_FRONT* pDecFront = (RVDEC_FRONT*)hCodec;
	rv_backend* pDec;
	VO_U32      returnCode = VO_ERR_NONE;
	HX_RESULT hr;
	rv_frame* pFrame;
	H263DecoderInParams  pRV20In;
	H263DecoderOutParams pRV20Out;

	struct Decoder *t;

    voVLog("voRealVideoDecProcess start!\n");

	if ((NULL == pDecFront) || (NULL == pInData) || (NULL == pOutData) ||(NULL == pOutPutInfo)){
		return VO_ERR_FAILED;
	}

	pDec = (rv_backend *)pDecFront->pDecCore;
	t = (struct Decoder *)pDec->pDecodeState;
	pFrame = (rv_frame*)pInData->Buffer;

	t->m_timeStamp = pInData->Time;
	t->UserData = pInData->UserData;

	if(NULL == pFrame->pData){//YRJ_TBD??

		pOutPutInfo->Format.Type   = VO_VIDEO_FRAME_NULL;
		pOutData->ColorType        = VO_COLOR_YUV_PLANAR420;
		pOutPutInfo->Format.Width  = 0;
		pOutPutInfo->Format.Height = 0;
		pOutPutInfo->InputUsed     = 0;

		pOutData->Buffer[0] = NULL;
		pOutData->Buffer[1] = NULL;
		pOutData->Buffer[2] = NULL;
		pOutData->Stride[0] = 0;
		pOutData->Stride[1] = 0;
		pOutData->Stride[2] = 0;

		return VO_ERR_INVALID_ARG;
	}

	pRV20In.pInputFrame = pFrame->pData;
	pRV20In.dataLength = pFrame->ulDataLen;
	pRV20In.numDataSegments = pFrame->ulNumSegments - 1;
	pRV20In.pDataSegments = (HXCODEC_SEGMENTINFO*)pFrame->pSegment;
	pRV20In.flags = 0;
	//TBD?? /* Use ECC to recover lost packets */

	//printf("rv start process frame !\n" );
	hr = pDec->fpDecode(pRV20In.pInputFrame,
		NULL,
		&pRV20In,
		&pRV20Out,
		pDec->pDecodeState);

	switch (hr)//YRJ_TBD remove old error code
	{
	case HXR_OK:
		returnCode = VO_ERR_NONE;
		break;
	case HXR_FAIL:
		returnCode = VO_ERR_FAILED; 
		break;
	case HXR_OUTOFMEMORY:
		returnCode = VO_ERR_OUTOF_MEMORY;
		break;
	case HXR_NOT_SUPPORTED:
	case HXR_NOTIMPL:
		returnCode = VO_ERR_NOT_IMPLEMENT;
		break;
	default:
		returnCode = VO_ERR_FAILED;
	}

	pOutPutInfo->Format.Width  = pRV20Out.width;
	pOutPutInfo->Format.Height = pRV20Out.height;
	pOutPutInfo->Format.Type   = pRV20Out.frame_type;
	pOutPutInfo->InputUsed     = pRV20Out.InputUsed;
	pOutPutInfo->Flag          = 0;

	//if(VO_ERR_NONE != returnCode){
	//	return returnCode;
	//}

	if(VO_VIDEO_FRAME_NULL == pRV20Out.frame_type){//YRJ_TBD 
		pOutData->Buffer[0] = NULL;
		pOutData->Buffer[1] = NULL;
		pOutData->Buffer[2] = NULL;
		pOutData->Stride[0] = 0;
		pOutData->Stride[1] = 0;
		pOutData->Stride[2] = 0;

		pOutData->Time = 0;
		pOutData->UserData = 0;
	}else{
		pOutData->Buffer[0] = pRV20Out.outData[0];
		pOutData->Buffer[1] = pRV20Out.outData[1];
		pOutData->Buffer[2] = pRV20Out.outData[2];
		pOutData->Stride[0] = pRV20Out.outStride[0];
		pOutData->Stride[1] = pRV20Out.outStride[1];
		pOutData->Stride[2] = pRV20Out.outStride[2];

		pOutData->Time = pRV20Out.timestamp;
		pOutData->UserData = pRV20Out.UserData;
	}

	pOutData->ColorType = VO_COLOR_YUV_PLANAR420;

#if defined(VODEBUG)
	sprintf(aa, "\n real video returnCode = %x", returnCode);
	NU_SIO_Puts(aa);

	sprintf(aa, "\n real video frame number = %d end", g_inputFrameNum - 1);	
	NU_SIO_Puts(aa);
#endif
    voVLog("voRealVideoDecProcess return %x \n", returnCode);

	return returnCode;
}

/*********************************************************************************************************************************************
* Set parameter of the decoder instance.                                                                                                     *
* \param hCodec [in] RealVideo decoder instance handle, returned by voRealVideoDecInit().                                                    *
* \param nID [in] Parameter ID, Supports VOID_COM_HEAD_DATA, VOID_RealVideo_STREAMFORMAT,                                                    *
*VOID_RealVideo_SEQUENCE_PARAMS, VOID_RealVideo_PICTURE_PARAMS,VOID_RealVideo_FLUSH                                                          *
* \VOID_RealVideo_GETFIRSTFRAME,VOID_RealVideo_DISABLEDEBLOCK                                                                                *
* \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.                                  *
* \retval VORC_OK Succeeded.                                                                                                                 *
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.                                                                            *
**********************************************************************************************************************************************/
VO_U32 VO_API voRealVideoDecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	RVDEC_FRONT* pDecFront = (RVDEC_FRONT*)hCodec;
	rv_backend* pDec;
	struct Decoder *t;
	VO_U32 returnCode = VO_ERR_NONE;

	if(pDecFront == NULL)
		return -3;//TBD

	pDec = (rv_backend*)pDecFront->pDecCore;
	t = (struct Decoder *)pDec->pDecodeState;

	if(pDec == NULL || pDec->fpCustomMessage == NULL)
		return -3;

	switch(nID)
	{
	case VO_PID_COMMON_HEADDATA:
	case VO_PID_DEC_RealVideo_INIT_PARAM:
		{
			VO_CODECBUFFER*		pInData;

			pInData = (VO_CODECBUFFER*)(plValue);
			returnCode = rv_decode_init(pDec, (rv_format_info*)(pInData->Buffer));

			voVLog("set VO_PID_COMMON_HEADDATA return %x\n ", returnCode);
		}
		break;
	case VO_PID_COMMON_FLUSH:
	case VO_PID_DEC_RealVideo_FLUSH:
		{
			RV_MSG_Simple msg;

#if defined(VODEBUG)
			sprintf(aa, "\n set ID VO_PID_DEC_RealVideo_FLUSH");
			NU_SIO_Puts(aa);
#endif
			msg.message_id = nID;
			msg.value1 = *((I32 *)plValue);
      		returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);
		}
		break;
	case VO_PID_VIDEO_VIDEOMEMOP:
		{
			RV_MSG_Simple msg;
			msg.message_id = nID;
			msg.pValue = (U32 *)plValue;
      		returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);
		}
		break;
	case VO_PID_DEC_RealVideo_DISABLEDEBLOCKING:
		{
			RV_MSG_Simple msg;

			msg.message_id = nID;
			msg.value1 = *((I32 *)plValue);
      		returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);
		}
		break;
	case VO_PID_VIDEO_OUTPUTMODE:
		{
			RV_MSG_Simple msg;

			msg.message_id = nID;
			msg.value1 = *((I32 *)plValue);
      		returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);
		}
		break;
	case VO_PID_DEC_RealVideo_VO_COLOR_YUV_420_PACK:
		{
			RV_MSG_Simple msg;

			msg.message_id = nID;
			msg.value1 = *((I32 *)plValue);
      		returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);
		}
		break;
	case VO_PID_COMMON_CPUNUM:
	case VO_PID_DEC_RealVideo_THREADS:
		{
			 VO_S32 thread_number = 1;

			thread_number = *((I32 *)plValue);

			if (thread_number < 2){
				t->m_decoding_thread_count = 1;
			}else{
				t->m_decoding_thread_count = 2;
			}
            
            voVLog("set VO_PID_COMMON_CPUNUM %x!\n", thread_number );
		}
		break;
#ifdef FBM_INTERNAL_CACHE_SUPPORT
  case VO_PID_COMMON_FRAME_BUF_EX:
    {
      // Extend internal fifo capacity.
      VO_U32 fbm_count = *((U32 *)plValue) + MAXFRAMES;
      t->m_priv.buf_num = fbm_count;

      voVLog("set VO_PID_COMMON_FRAME_BUF_EX %x!\n", fbm_count);
    }
    break;
#endif
	default:
        return VO_ERR_WRONG_PARAM_ID;
		break;
	}

#if defined(VODEBUG)
	sprintf(aa, "\n set ID  end");
	NU_SIO_Puts(aa);
#endif

	return returnCode;
}


/**********************************************************************************************************************************
* Get parameter of the decoder instance.
* \param hCodec [in] RealVideo decoder instance handle, returned by voRealVideoDecInit().
* \param nID [in] Parameter ID, supports VOID_COM_VIDEO_FORMAT, VOID_COM_VIDEO_WIDTH, 
* VOID_COM_VIDEO_HEIGHT,VOID_RealVideo_GETFIRSTFRAME,VOID_RealVideo_DISABLEDEBLOCK
* \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
************************************************************************************************************************************/
VO_U32 VO_API voRealVideoDecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	RVDEC_FRONT* pDecFront = (RVDEC_FRONT*)hCodec;
	VO_U32 returnCode = VO_ERR_NONE;
	rv_backend *pDec;

	if(pDecFront == NULL)
		return -1;//TBD

	pDec = (rv_backend*)pDecFront->pDecCore;
	if(pDec == NULL || pDec->fpCustomMessage == NULL)
		return -3;
	
	//TODO:
	switch(nID)
	{
	case VO_PID_DEC_RealVideo_GET_VIDEOFORMAT:
		{
			//must after first decode process
			RV_MSG_Simple msg;

#if defined(VODEBUG)
			sprintf(aa, "\n get ID VO_PID_DEC_RealVideo_GET_VIDEOFORMAT");
			NU_SIO_Puts(aa);
#endif

			msg.message_id = nID;
			msg.pValue = (U32 *)plValue;
			returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);
		}
		break;

	case VO_PID_DEC_RealVideo_GET_VIDEO_WIDTH:
		{
			//TBD
			RV_MSG_Simple msg;

#if defined(VODEBUG)
			sprintf(aa, "\n get ID VO_PID_DEC_RealVideo_GET_VIDEO_WIDTH");
			NU_SIO_Puts(aa);
#endif

			msg.message_id = nID;
			returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);

			*((I32 *)plValue) = msg.value1;

		}
		break;
	case VO_PID_DEC_RealVideo_SHAREDMEMWAITOUTPUT:
		{
			RV_MSG_Simple msg;

#if defined(VODEBUG)
			sprintf(aa, "\n get ID VO_PID_DEC_RealVideo_SHAREDMEMWAITOUTPUT");
			NU_SIO_Puts(aa);
#endif

			msg.message_id = nID;
			msg.pValue = (U32 *)plValue;
			returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);
		}
		break;
	case VO_PID_DEC_RealVideo_GET_VIDEO_HEIGHT:
		{
			//TBD
			RV_MSG_Simple msg;

#if defined(VODEBUG)
			sprintf(aa, "\n get ID VO_PID_DEC_RealVideo_GET_VIDEO_HEIGHT");
			NU_SIO_Puts(aa);
#endif

			msg.message_id = nID;
			returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);

			*((I32 *)plValue) = msg.value1;

		}
		break;
	case VO_PID_DEC_RealVideo_FORMAT:
		{
			RV_MSG_Simple msg;
			VORV_FrameType* frameType= (VORV_FrameType*)plValue;

#if defined(VODEBUG)
			sprintf(aa, "\n get ID VO_PID_DEC_RealVideo_FORMAT");
			NU_SIO_Puts(aa);
#endif

			msg.message_id = nID;
			msg.value1 = (long)frameType->bits;
			returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);

			frameType->type = msg.value2;
		}
		break;
	case VO_PID_DEC_RealVideo_GETLASTOUTVIDEOBUFFER:
		{
			RV_MSG_Simple msg;

#if defined(VODEBUG)
			sprintf(aa, "\n get ID VO_PID_DEC_RealVideo_GETLASTOUTVIDEOBUFFER");
			NU_SIO_Puts(aa);
#endif

			msg.message_id = nID;
			msg.pValue = (U32 *)plValue;
			returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);

		}
		break;
	case VO_PID_DEC_RealVideo_GETLASTOUTVIDEOFORMAT:
		{
			RV_MSG_Simple msg;

#if defined(VODEBUG)
			sprintf(aa, "\n get ID VO_PID_DEC_RealVideo_GETLASTOUTVIDEOFORMAT");
			NU_SIO_Puts(aa);
#endif

			msg.message_id = nID;
			msg.pValue = (U32 *)plValue;
			returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);
		}
		break;
	case VO_PID_VIDEO_FRAMETYPE:
		{
			rv_frame *pInDataBuffer;
			RV_MSG_Simple msg;
			VO_CODECBUFFER* pInData = (VO_CODECBUFFER*)plValue;

#if defined(VODEBUG)
			sprintf(aa, "\n get ID VO_PID_VIDEO_FRAMETYPE");
			NU_SIO_Puts(aa);
#endif

			pInDataBuffer = (rv_frame *)pInData->Buffer;

			msg.message_id = nID;
			msg.value1 = (long)pInDataBuffer->pData;
			returnCode = pDec->fpCustomMessage((U32 *)&msg,pDec->pDecodeState);
			pInData->Time = msg.value2;
		}
		break;
	case VO_PID_VIDEO_ASPECTRATIO:
		*((I32 *)plValue) = 0;// no other aspec for real

		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

#if defined(VODEBUG)
	sprintf(aa, "\n get ID end");
	NU_SIO_Puts(aa);
#endif

	return returnCode;
}

VO_U32 VO_API voRVDecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	RVDEC_FRONT* pFront = (RVDEC_FRONT*)hDec;

	if(!pFront)
		return VO_ERR_INVALID_ARG;
        voVLog("voRVDecSetInputData start !\n" );

	pFront->InputData.Buffer     = pInput->Buffer;
	pFront->InputData.Length     = pInput->Length;
	pFront->InputData.Time       = pInput->Time;
	pFront->InputData.UserData   = pInput->UserData;
	pFront->OutPutInfo.InputUsed = 0;

	voVLog("voRVDecSetInputData done !\n" );
	return VO_ERR_NONE;
}

VO_U32 VO_API voRVDecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutPutInfo)
{
	RVDEC_FRONT* pFront = (RVDEC_FRONT*)hDec;
	rv_backend* pDec;
	struct Decoder *t;

	if(!pFront)
		return VO_ERR_INVALID_ARG;

	if(pFront->OutPutInfo.InputUsed >= pFront->InputData.Length){
		return VO_ERR_INPUT_BUFFER_SMALL;
	}
    voVLog("voRVDecGetOutputData start !\n" );

  if(voRealVideoDecProcess(hDec, &pFront->InputData, &(pFront->OutData), &(pFront->OutPutInfo)) != VO_ERR_NONE){
    return VO_ERR_FAILED;
  }

	pDec = (rv_backend*)pFront->pDecCore;
	t = (struct Decoder*)pDec->pDecodeState;
#ifdef LICENSEFILE
	if(VO_VIDEO_FRAME_NULL != pFront->OutPutInfo.Format.Type){
		t->m_display_ptype = pFront->OutPutInfo.Format.Type + 1;	
		voCheckLibCheckVideo(t->m_phCheck, &(pFront->OutData), &(pFront->OutPutInfo.Format));
	}else{
		t->m_display_ptype = pFront->OutPutInfo.Format.Type;
	}	
#endif

	*pOutput = pFront->OutData;
	*pOutPutInfo = pFront->OutPutInfo;
	if (t->m_yuv420_pack_flag){
		pOutput->ColorType = VO_COLOR_YUV_420_PACK;

		if (VO_VIDEO_FRAME_NULL != pFront->OutPutInfo.Format.Type){
			U32 i = 0;//huwei 20110307 yuv420pack
			U32 frame_number = t->m_frame_number;

			i = frame_number % MAXYUV420PACKFRAMES;
			YUV420T0YUV420PACK(pFront->OutData.Buffer[0], pFront->OutData.Buffer[1],pFront->OutData.Buffer[2], t->m_YUV420Frame[i]->m_pYPlane,
				t->m_YUV420Frame[i]->m_pUVPack, pFront->OutData.Stride[0], pOutput->Stride[1], pFront->OutPutInfo.Format.Width, 
				pFront->OutPutInfo.Format.Height);


			pOutput->Buffer[0] = t->m_YUV420Frame[i]->m_pYPlane;
			pOutput->Buffer[1] = t->m_YUV420Frame[i]->m_pUVPack;
			pOutput->Buffer[2] = NULL;
			pOutput->Stride[0] = (pFront->OutPutInfo.Format.Width + 15) & (~0xf);
			pOutput->Stride[1] = (pFront->OutPutInfo.Format.Width + 15) & (~0xf);
			pOutput->Stride[2] = 0;
		}

	}

	t->m_frame_number++;

    voVLog("voRVDecGetOutputData done !\n" );
	return VO_ERR_NONE;
}

#if defined(VOSYMBIAN)
EXPORT_C VO_S32 VO_API voGetRVDecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
#else
VO_S32 VO_API voGetRVDecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
#endif
{
	VO_VIDEO_DECAPI *pRVDec = pDecHandle;

	if(!pRVDec)
		return VO_ERR_INVALID_ARG;

	pRVDec->Init   = voRealVideoDecInit;
	pRVDec->Uninit = voRealVideoDecUninit;
	pRVDec->SetParam = voRealVideoDecSetParameter;
	pRVDec->GetParam = voRealVideoDecGetParameter;
	pRVDec->SetInputData = voRVDecSetInputData;
	pRVDec->GetOutputData = voRVDecGetOutputData;

	//IOSD
	//printf("rv get api!\n");

	return VO_ERR_NONE;

}
