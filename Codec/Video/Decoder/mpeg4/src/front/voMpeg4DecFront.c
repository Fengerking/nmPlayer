/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "voMpeg4Decoder.h"
#ifdef VODIVX3
#include "voDIVX3.h"
#define VO_PID_DEC_MPEG4_GET_LASTVIDEOBUFFER	VO_PID_DEC_DIVX3_GET_LASTVIDEOBUFFER
#define VO_PID_DEC_MPEG4_GET_LASTVIDEOINFO		VO_PID_DEC_DIVX3_GET_LASTVIDEOINFO
#define VO_PID_DEC_MPEG4_SET_THREAD_NUM			VO_PID_DEC_DIVX3_SET_THREAD_NUM
#define yyGetMPEG4DecFunc						voGetDIVX3DecAPI			 
#else
#include "voMPEG4.h"
#endif

#include "voMpeg4Dec.h"
#include "voMpegBuf.h"
#include "voMpeg4MB.h"
#include "voMpeg4Frame.h"

#if ENABLE_LICENSE_CHECK
#include "voCheck.h"
#ifdef VODIVX3
#define voMPEG4DechInst voDIVX3DechInst	
#endif
VO_HANDLE voMPEG4DechInst = NULL;
#endif	

#if ENABLE_SIMPLE_LC
VO_S32 gMPEG4DecLicNum = 30*60*60;
#endif

//YU_TBD
#define CHECK_API_ERR(result) (result & (VO_ERR_BASE))

static VO_U32 vo_gcd(int m, int n)
{
	if (m == 0)
		return n;
	if (n == 0)
		return m;
	if (m < n)
	{
		int tmp = m;
		m = n;
		n = tmp;
	}
	while (n != 0)
	{
		int tmp = m % n;
		m = n;
		n = tmp;
	}

	return m;
}

//FILE *outdebug = NULL;
/**
* Initialize a MPEG4 decoder instance use default settings.
* \param phCodec [out] Return the MPEG4 Decoder handle.
* \param pQueryMem [in] Input memory allocated outside.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_OUT_OF_MEMORY Out of memory.
*/

VO_U32 VO_API voMPEG4DecInit(VO_HANDLE * phDec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VO_MPEG4DEC_FRONT* pDecFront = NULL;
	VO_S32 nCodecIdx; 
	VO_U32 voRC = VO_ERR_NONE;
	VO_LIB_OPERATOR *pLibOP = NULL;
#if ENABLE_LICENSE_CHECK	
	VO_PTR phLicenseCheck = NULL;
#endif
	//outdebug = fopen("e:/debug.txt", "wb");

	switch(vType){
	case VO_VIDEO_CodingMPEG4:
		nCodecIdx = VO_INDEX_DEC_MPEG4;
		break;
	case VO_VIDEO_CodingH263:
		nCodecIdx = VO_INDEX_DEC_H263;
		break;
	case VO_VIDEO_CodingS263:
		nCodecIdx = VO_INDEX_DEC_S263;
		break;
	case VO_VIDEO_CodingDIVX:
		nCodecIdx = VO_INDEX_DEC_DIVX3;
		break;
	default:
		return VO_ERR_NOT_IMPLEMENT;
	}

	if(pUserData){
		if(pUserData->memflag & 0XF0){//valid libOperator flag
			pLibOP = pUserData->libOperator;
		}
	}

#if ENABLE_LICENSE_CHECK
	voRC = voCheckLibInit (&phLicenseCheck, nCodecIdx, pUserData ? pUserData->memflag : 0, voMPEG4DechInst, pLibOP);
	if (voRC != VO_ERR_NONE)
	{
		if (phLicenseCheck)
		{
			voCheckLibUninit(phLicenseCheck);
		}
		return voRC;
	}
#endif

	if(pUserData){
		if((!(pUserData->memflag&0xF)) || (pUserData->memData == NULL)){
			pUserData = NULL;
		}
	}
	pDecFront = (VO_MPEG4DEC_FRONT*)MallocMem(pUserData, nCodecIdx, sizeof(VO_MPEG4DEC_FRONT), CACHE_LINE);
	if(!pDecFront)
		return VO_ERR_OUTOF_MEMORY;

	SetMem(pUserData, nCodecIdx, (VO_U8 *)pDecFront, 0, sizeof(VO_MPEG4DEC_FRONT));

	pDecFront->pUserData = pUserData;
	pDecFront->nCodecIdx = nCodecIdx;

	voRC = voMPEG4DecCreate(&pDecFront->pBackDec, pUserData, nCodecIdx);
	if(voRC != VO_ERR_NONE){
		FreeMem(pUserData, nCodecIdx, pDecFront);
		return voRC;
	}

#if ENABLE_LICENSE_CHECK	
	pDecFront->phLicenseCheck = phLicenseCheck; 
#endif

	*phDec = pDecFront;
	return voRC;
}


/**
* Close the MPEG4 decoder instance, release any resource it used.
* \param hCodec [in] MPEG4 decoder instance handle, returned by voMPEG4DecInit().
* \return The function should always return VORC_OK.
*/
VO_U32 VO_API voMPEG4DecUninit(VO_HANDLE hCodec)
{
	VO_MPEG4DEC_FRONT* pDecFront = (VO_MPEG4DEC_FRONT*)hCodec;

	if(pDecFront == NULL)
		return VO_ERR_INVALID_ARG;

#if ENABLE_LICENSE_CHECK
	voCheckLibUninit (pDecFront->phLicenseCheck);
#endif	

	voMPEG4DecFinish(pDecFront->pBackDec);
	FreeMem(pDecFront->pUserData, pDecFront->nCodecIdx, pDecFront);

	//fclose(outdebug);

	return VO_ERR_NONE;
}


/**
* Set parameter of the decoder instance.
* \param hCodec [in] MPEG4 decoder instance handle, returned by voMPEG4DecInit().
* \param nID [in] Parameter ID, Supports: VOID_COM_HEAD_DATA,VOID_MPEG4_GETFIRSTFRAME
* \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
*/
VO_U32 VO_API voMPEG4DecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR lValue)
{
	VO_MPEG4DEC_FRONT* pDecFront = (VO_MPEG4DEC_FRONT*)hCodec;
	VO_MPEG4_DEC *pDec;
	VO_U32 voRC = VO_ERR_NONE;

	//if(nID == VO_PID_DEC_MPEG4_SET_QUERYMEM){//TBD
		//QueryMem((VOVIDEODECQUERYMEM*)lValue); 
	//	return VO_ERR_NONE;
	//}

	if(pDecFront == NULL)
		return VO_ERR_INVALID_ARG;

	pDec = (VO_MPEG4_DEC *)pDecFront->pBackDec;

	switch(nID) {
	case VO_PID_COMMON_HEADDATA: 
		voRC = voMPEGDecFrame(pDec, (VO_CODECBUFFER *)lValue, NULL, NULL, DEC_VOLHR);
		if(CHECK_API_ERR(voRC))
			return voRC;
		break;

	case VO_PID_COMMON_FLUSH:
		pDec->nFrame = 0;
		break;
	case VO_PID_VIDEO_VIDEOMEMOP:
		pDec->vMemSever.pFrameMemOp = (VO_MEM_VIDEO_OPERATOR *)lValue;
		break;
	case VO_PID_COMMON_HeadInfo:
		{
			VO_CODECBUFFER vCodecBuffer;
		
			pDec->pHeadInfo = (VO_HEAD_INFO*)lValue;
			
			vCodecBuffer.Time = 0;
			vCodecBuffer.Buffer = pDec->pHeadInfo->Buffer;
			vCodecBuffer.Length = pDec->pHeadInfo->Length;

			voRC = voMPEGDecFrame(pDec, &vCodecBuffer, NULL, NULL, DEC_HEADINFO);
#if ENABLE_HEADINFO
			pDec->pHeadInfo->Size = strlen(pDec->pHeadInfo->Description);
#endif
			if(CHECK_API_ERR(voRC))
				return voRC;
			break;
		}
	case VO_PID_VIDEO_OUTPUTMODE:
		pDec->nOutMode = *((VO_S32 *)lValue);
		break;
	case VO_PID_COMMON_CPUNUM:
	case VO_PID_DEC_MPEG4_SET_THREAD_NUM:
		{
			VO_U32 newValue = *((VO_U32 *)lValue);
			if (newValue > 2)
				newValue = 2;	//limit thd num, only support dual-core

			if (newValue != pDec->nMpeg4Thd)
			{
				if (newValue > 1)
				{
					pDec->nMpeg4ThdMaxMB = 80;
					if (pDec->nCodecId == MPEG4_ID)
					{
						pDec->fpDecIVOP = MPEG4DecVOP;
						pDec->fpDecPVOP = MPEG4DecVOP;
						pDec->fpDecBVOP = MPEG4DecVOP;
					}
				}
				else
				{
					pDec->nMpeg4ThdMaxMB = 4;
					if (pDec->nCodecId == MPEG4_ID)
					{
						pDec->fpDecIVOP = MPEG4DecVOP_s;
						pDec->fpDecPVOP = MPEG4DecVOP_s;
						pDec->fpDecBVOP = MPEG4DecVOP_s;
					}
				}
				if (pDec->nMpeg4Thd > 1)
				{
					ReleaseMBThread(pDec);
				}
				FreeMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->vopCtxt.bFieldDct);
				pDec->nMpeg4Thd = newValue;
			
				//vop context size
				pDec->nVopCtxtSize = 0;

				//bFieldDct
				pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB;

				//gmcmb
				pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB;

				pDec->nVopCtxtSize = (pDec->nVopCtxtSize + 3)&(~3);

				//MVBack
				pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*6*sizeof(VO_S32);

				//MVFwd
				pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*6*sizeof(VO_S32);

				//fpReconMB
				pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*sizeof(FuncReconMB);

				//scanType
				pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*6;

				//blockLen
				pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*6;

				//blockptr
				pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*6*64*sizeof(VO_S16);

				//pMBPos
				pDec->nVopCtxtSize += pDec->nMpeg4ThdMaxMB*sizeof(VO_S32);

				//mc_data
				pDec->nVopCtxtSize += BLOCK_SIZE*BLOCK_SIZE;

				pDec->vopCtxt.bFieldDct = (VO_S8*) MallocMem(NULL, 0, pDec->nVopCtxtSize, CACHE_LINE);
				if(!pDec->vopCtxt.bFieldDct)
					return VO_ERR_OUTOF_MEMORY;

				SetMem(NULL, 0, (VO_U8*)pDec->vopCtxt.bFieldDct, 0, pDec->nVopCtxtSize);
				pDec->vopCtxt.gmcmb = pDec->vopCtxt.bFieldDct + pDec->nMpeg4ThdMaxMB;
				pDec->vopCtxt.MVBack = (VO_S32 *)(((VO_U32)(pDec->vopCtxt.gmcmb + pDec->nMpeg4ThdMaxMB + 3))&(~3));
				pDec->vopCtxt.MVFwd = pDec->vopCtxt.MVBack + pDec->nMpeg4ThdMaxMB*6;
				pDec->vopCtxt.fpReconMB = (FuncReconMB *)(pDec->vopCtxt.MVFwd + pDec->nMpeg4ThdMaxMB*6);
				pDec->vopCtxt.scanType = (VO_S8 *)(pDec->vopCtxt.fpReconMB + pDec->nMpeg4ThdMaxMB);
				pDec->vopCtxt.blockLen = pDec->vopCtxt.scanType + pDec->nMpeg4ThdMaxMB*6;
				pDec->vopCtxt.blockptr = (VO_S16 *)(pDec->vopCtxt.blockLen + pDec->nMpeg4ThdMaxMB*6);
				pDec->vopCtxt.pMBPos = (VO_S32 *)(pDec->vopCtxt.blockptr + pDec->nMpeg4ThdMaxMB*6*64);
				pDec->vopCtxt.mc_data = (VO_U8 *)(pDec->vopCtxt.pMBPos + pDec->nMpeg4ThdMaxMB);

				if (pDec->nMpeg4Thd > 1)
				{
					if (InitialMBThread(pDec))
					{
						return VO_ERR_INVALID_ARG;
					}
				}
			}
		}
		break;
	case VO_PID_COMMON_FRAME_BUF_EX:
		{
			VO_U32 nFrameNum = FRAMEDEFAULT + *((VO_U32*)lValue);
			if (nFrameNum > MAXFRAMES)
			{
				nFrameNum = MAXFRAMES;
				*((VO_U32*)lValue) = MAXFRAMES - FRAMEDEFAULT;	//loop back the true value
			}
			if (pDec->nFrameBufNum != nFrameNum)
			{
				pDec->nFrameBufNum = nFrameNum;
				if (pDec->validvol)
				{
					pDec->validvol = 0;
					voRC = ConfigDecoder(pDec, pDec->nTrueWidth, pDec->nTrueHeight);
					if(CHECK_API_ERR(voRC))
						return voRC;
					pDec->validvol = 1;
				}
			}
		}
		break;
#ifdef VODIVX3
	case VO_PID_DEC_DIVX3_VIDEO_WIDTH:
		pDecFront->nWidth = *((VO_U32*)lValue);
		break;
	case VO_PID_DEC_DIVX3_VIDEO_HEIGHT:
		pDecFront->nHeight = *((VO_U32*)lValue);
		break;
	case VO_PID_VIDEO_FORMAT:
		{
			VO_VIDEO_FORMAT *pVideoFormat = (VO_VIDEO_FORMAT *)lValue;
			pDecFront->nWidth = pVideoFormat->Width;
			pDecFront->nHeight = pVideoFormat->Height;

		}

		break;
#endif
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}


/**
* Get parameter of the decoder instance.
* \param hCodec [in] MPEG4 decoder instance handle, returned by voMPEG4DecInit().
* \param nID [in] Parameter ID, supports VOID_COM_VIDEO_FORMAT, VOID_COM_VIDEO_WIDTH, VOID_COM_VIDEO_HEIGHT
* \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
*/
VO_U32 VO_API voMPEG4DecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	VO_MPEG4DEC_FRONT* pDecFront = (VO_MPEG4DEC_FRONT*)hCodec;
	VO_MPEG4_DEC *pDec;
	VO_U32 voRC = VO_ERR_NONE;

	if(pDecFront == NULL)
		return VO_ERR_INVALID_ARG;

	pDec = (VO_MPEG4_DEC *)pDecFront->pBackDec;

	switch(nID) {
	case VO_PID_VIDEO_FRAMETYPE: //TBD
		{
			VO_VIDEO_OUTPUTINFO vOutPutInfo;
			VO_CODECBUFFER* pInData = (VO_CODECBUFFER *)plValue;

			voRC = voMPEGDecFrame(pDecFront->pBackDec, pInData, NULL, &vOutPutInfo, DEC_FRAMETYPE);
			if(CHECK_API_ERR(voRC))
				return voRC;

			pInData->Time = (VO_S64)vOutPutInfo.Format.Type;
		}
		break;
	case VO_PID_DEC_MPEG4_GET_LASTVIDEOBUFFER:
		{
			VO_VIDEO_BUFFER	*pOutBuf = (VO_VIDEO_BUFFER *)plValue;
			if(pDec->pRefFrame){
				pOutBuf->Buffer[0] = pDec->pRefFrame->y;
				pOutBuf->Buffer[1] = pDec->pRefFrame->u;
				pOutBuf->Buffer[2] = pDec->pRefFrame->v;
			}else{
				pOutBuf->Buffer[0] = NULL;
				pOutBuf->Buffer[1] = NULL;
				pOutBuf->Buffer[2] = NULL;
			}
			pOutBuf->Stride[0]	= pDec->nLumEXWidth;
			pOutBuf->Stride[1]	= pDec->nChrEXWidth;
			pOutBuf->Stride[2]	= pDec->nChrEXWidth;
			pOutBuf->ColorType = VO_COLOR_YUV_PLANAR420;
		}
		break;
	case VO_PID_DEC_MPEG4_GET_LASTVIDEOINFO:
		{
			VO_VIDEO_OUTPUTINFO	*pOutBufInfo = (VO_VIDEO_OUTPUTINFO *)plValue;
			pOutBufInfo->Format.Width = pDec->nTrueWidth;
			pOutBufInfo->Format.Height = pDec->nTrueHeight;
			if(pDec->pRefFrame){
				pOutBufInfo->Format.Type = pDec->pRefFrame->nVOPType;				/* 0-Iframe, 1-Pframe, 2-pDstFrame*/
			}else{
				pOutBufInfo->Format.Type = VO_VIDEO_FRAME_NULL;
			}
		}
		break;
	case VO_PID_VIDEO_ASPECTRATIO:
		{
			VO_S32 width = pDec->nTrueWidth, height = pDec->nTrueHeight;
			VO_U32 gcd;
			switch (pDec->nAspectRadio)
			{
			case 0:
			case VO_RATIO_11:
				*((VO_S32 *)plValue) = 0;
				return VO_ERR_NONE;
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
			default:
				width *= ((VO_U32)(pDec->nAspectRadio))>>16;
				height *= pDec->nAspectRadio & 0xFFFF;
			}
			gcd = vo_gcd(width, height);
			width /= gcd;
			height /= gcd;
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
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voMPEG4DecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	VO_MPEG4DEC_FRONT* pDecFront = (VO_MPEG4DEC_FRONT*)hDec;

	if(!pDecFront)
		return VO_ERR_INVALID_ARG;

	//fprintf(outdebug, "\n in time = %d, len = %d", (int)pInput->Time, (int)pInput->Length);
	pDecFront->vInBuf = *pInput;
	pDecFront->nUsedLen = 0;/*clean up used length*/

	return VO_ERR_NONE;
}

VO_U32 VO_API voMPEG4DecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutPutInfo)
{
	VO_MPEG4DEC_FRONT* pDecFront = (VO_MPEG4DEC_FRONT*)hDec;
	VO_CODECBUFFER  *pInData;
	VO_U32 voRC = VO_ERR_NONE;

	if(!pDecFront)
		return VO_ERR_INVALID_ARG;

	pInData = &pDecFront->vInBuf;

#if ENABLE_LICENSE_CHECK
	voCheckLibResetVideo(pDecFront->phLicenseCheck, pDecFront->pOutVideoBuf);
#endif
#if ENABLE_SIMPLE_LC
	if(--gMPEG4DecLicNum<0)
		return VO_ERR_LICENSE_ERROR;
#endif

#ifdef VODIVX3
	pOutPutInfo->Format.Width	= pDecFront->nWidth;
	pOutPutInfo->Format.Height	= pDecFront->nHeight;
#endif
	voRC = voMPEGDecFrame(pDecFront->pBackDec, pInData, pOutput, pOutPutInfo, DEC_FRAME);
	if(CHECK_API_ERR(voRC)){
		return voRC;
	}

	pInData->Length -= pOutPutInfo->InputUsed;
	pInData->Buffer += pOutPutInfo->InputUsed;
	pDecFront->nUsedLen += pOutPutInfo->InputUsed;
	pOutPutInfo->InputUsed	= pDecFront->nUsedLen;

	if(pInData->Length < 5){
		pOutPutInfo->Flag = 0;/*no more data*/
	}else{
		pOutPutInfo->Flag = 1;/*more video data*/
	}


#if ENABLE_LICENSE_CHECK
	pDecFront->pOutVideoBuf = pOutput;/* backup for next voCheckLibResetVideo*/
	if(pOutPutInfo->Format.Type != VO_VIDEO_FRAME_NULL)
		voCheckLibCheckVideo (pDecFront->phLicenseCheck, pOutput, &pOutPutInfo->Format);
#endif


	//fprintf(outdebug, "\n out time = %d, used len = %d, type = %d, moredata = %d", (int)pOutput->Time, (int)pOutPutInfo->InputUsed, (int)pOutPutInfo->Format.Type, (int)pOutPutInfo->Flag);


	return voRC;
}

VO_S32 VO_API yyGetMPEG4DecFunc (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VO_VIDEO_DECAPI *pMpeg4Dec = pDecHandle;

	if(!pMpeg4Dec)
		return VO_ERR_INVALID_ARG;

	pMpeg4Dec->Init				= voMPEG4DecInit;
	pMpeg4Dec->Uninit			= voMPEG4DecUninit;
	pMpeg4Dec->SetParam			= voMPEG4DecSetParameter;
	pMpeg4Dec->GetParam			= voMPEG4DecGetParameter;
	pMpeg4Dec->SetInputData		= voMPEG4DecSetInputData;
	pMpeg4Dec->GetOutputData	= voMPEG4DecGetOutputData;

	return VO_ERR_NONE;

}

