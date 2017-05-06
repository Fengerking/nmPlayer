/**********************************************************************
*
* VisualOn voplayer
*
* CopyRight (C) VisualOn SoftWare Co., Ltd. All rights are reserved.
*
***********************************************************************/

/*********************************************************************
*
* Filename:	
*		alacdec.c
* 
* Abstact:	
*
*		Apple Lossless Audio Codec for VisualOn Decoder AP file.
*
* Author:
*
*		Witten Wen 24-March-2010
*
* Revision History:
*
******************************************************/

#include "cmnMemory.h"
#include "vomemory.h"
#include "voALAC.h"
#include "voCheck.h"
#include "voalacdec.h"

#define ENABLELICENCECHECK	0
VO_PTR	g_hvommPlayInst = NULL;
VO_U32 VO_API voALACDecInit(VO_HANDLE * phCodec, VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)//HVOCODEC *phCodec)
{
	CALACObjectDecoder *palacdec = NULL;
	VO_MEM_OPERATOR *vopMemOP = NULL;

	if(vType != VO_AUDIO_CodingALAC)
		return VO_ERR_INVALID_ARG;

	if(pUserData ==NULL || (pUserData->memflag & 0x0F) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
		cmnMemFillPointer(VO_AUDIO_CodingALAC);
		vopMemOP = &g_memOP;
	}
	else
	{
		vopMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	}

	palacdec = (CALACObjectDecoder *)voMalloc(vopMemOP, sizeof(CALACObjectDecoder));
	if(palacdec == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}

	// Wipe clean the object just allocated
	voALACMemset(palacdec, 0, sizeof (CALACObjectDecoder));
	
	palacdec->vopMemOP = vopMemOP;	
	
	if((palacdec->m_pBitStream = (CALACBitStream *)voALACCalloc(1, sizeof(CALACBitStream))) == NULL)
		return VO_ERR_OUTOF_MEMORY;

	if((palacdec->m_pALACInfo = (CALACSetInfo *)voALACCalloc(1, sizeof(CALACSetInfo))) ==NULL)
		return VO_ERR_OUTOF_MEMORY;

#if ENABLELICENCECHECK
	if((pUserData->memflag & 0xF0) == 0x10)
	{
		voCheckLibInit(&(palacdec->hCheck), VO_AUDIO_CodingALAC, 1, g_hvommPlayInst, pUserData->libOperator);
	}
	else
	{
		voCheckLibInit(&(palacdec->hCheck), VO_AUDIO_CodingALAC, 1, g_hvommPlayInst, NULL);
	}
#endif

	*phCodec = (VO_HANDLE)palacdec;
	return VO_ERR_NONE;
}

VO_U32 VO_API voALACDecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER *pInput)
{
	CALACObjectDecoder *palacdec = (CALACObjectDecoder *)hCodec;
	if(palacdec == NULL)
		return VO_ERR_INVALID_ARG;

	if(!palacdec->m_Header_OK)
		return VO_ERR_INVALID_ARG;

	if(!(pInput && pInput->Buffer && pInput->Length > 0))
		return VO_ERR_INVALID_ARG;

	palacdec->m_pBitStream->m_ptr		= pInput->Buffer;
	palacdec->m_pBitStream->m_Length	= pInput->Length;
	palacdec->m_pBitStream->m_UsedBits	= 0;

	return VO_ERR_NONE;
}

VO_U32 VO_API voALACDecGetOutData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)//HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECDATABUFFER *pOutData, VOCODECAUDIOFORMAT *pOutFormat)
{
	VO_U32	hr = 0;
	CALACObjectDecoder *palacdec = (CALACObjectDecoder *)hCodec;

	if(palacdec == NULL)
		return VO_ERR_INVALID_ARG;

	if(palacdec->m_pBitStream->m_Length <= 0)
		return VO_ERR_INPUT_BUFFER_SMALL;

	hr = voALACDecodeFrame(palacdec, pOutBuffer->Buffer, &pOutBuffer->Length);

	if(hr < 0)
		return VO_ERR_INVALID_ARG;

	pOutInfo->Format.Channels = palacdec->m_Channels;
	pOutInfo->Format.SampleBits = palacdec->m_SampleBits;
	pOutInfo->Format.SampleRate = palacdec->m_SampleRate;
	pOutInfo->InputUsed = 0;

#if ENABLELICENCECHECK
	if(voCheckLibCheckAudio(palacdec->hCheck, pOutBuffer, &(pOutInfo->Format)) != VO_ERR_NONE)
		return VO_ERR_LICENSE_ERROR;
#endif

	return VO_ERR_NONE;	
}

VO_U32 VO_API voALACDecUninit(VO_HANDLE hCodec)
{
	CALACObjectDecoder *palacdec = (CALACObjectDecoder *)hCodec;

	if(palacdec)
	{
		VO_MEM_OPERATOR *vopMemOP = palacdec->vopMemOP;
		if(palacdec->m_pBitStream)
		{
			voALACFree(palacdec->m_pBitStream);
			palacdec->m_pBitStream = NULL;
		}		
//		if(palacdec->m_pCodecData)
//		{
//			voALACFree(palacdec->m_pCodecData);
//			palacdec->m_pCodecData = NULL;
//		}
//		if(palacdec->m_pTime2Sample)
//		{
//			voALACFree(palacdec->m_pTime2Sample);
//			palacdec->m_pTime2Sample = NULL;
//		} 
//		if(palacdec->m_pSampleByteSize)
//		{
//			voALACFree(palacdec->m_pSampleByteSize);
//			palacdec->m_pSampleByteSize = NULL;
//		}
		if(palacdec->m_pALACInfo)
		{
			CALACSetInfo *psetinfo = palacdec->m_pALACInfo;
			if(psetinfo->m_pErrorBuffA)
			{
				voALACFree(psetinfo->m_pErrorBuffA);
				psetinfo->m_pErrorBuffA = NULL;
			}
			if(psetinfo->m_pErrorBuffB)
			{
				voALACFree(psetinfo->m_pErrorBuffB);
				psetinfo->m_pErrorBuffA = NULL;
			}
			if(psetinfo->m_pOutBuffA)
			{
				voALACFree(psetinfo->m_pOutBuffA);
				psetinfo->m_pOutBuffA = NULL;
			}
			if(psetinfo->m_pOutBuffB)
			{
				voALACFree(psetinfo->m_pOutBuffB);
				psetinfo->m_pOutBuffB = NULL;
			}
			if(psetinfo->m_pUncprsseBytsBufA)
			{
				voALACFree(psetinfo->m_pUncprsseBytsBufA);
				psetinfo->m_pUncprsseBytsBufA = NULL;
			}
			if(psetinfo->m_pUncprsseBytsBufB)
			{
				voALACFree(psetinfo->m_pUncprsseBytsBufB);
				psetinfo->m_pUncprsseBytsBufB = NULL;
			}
			voALACFree(palacdec->m_pALACInfo);
			palacdec->m_pALACInfo = NULL;
		}		
#if ENABLELICENCECHECK
		if(palacdec->hCheck != NULL)
			voCheckLibUninit(palacdec->hCheck);
#endif
		voFree(vopMemOP, palacdec);
		palacdec = NULL;
	}	

	return VO_ERR_NONE;
}

VO_U32 VO_API voALACDecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)//HVOCODEC hCodec, LONG nID, LONG lValue)
{
	CALACObjectDecoder *palacdec = (CALACObjectDecoder *)hCodec;
	VO_WAVEFORMATEX *ALACHeadata = NULL;
	if(palacdec == NULL)
		return VO_ERR_INVALID_ARG;

	switch(uParamID)
	{
	case VO_PID_COMMON_HEADDATA:
		{
			VO_CODECBUFFER *pBuffer = (VO_CODECBUFFER *)pData;
			if(pBuffer->Buffer == NULL)
				return VO_ERR_INVALID_ARG;
			
			ALACHeadata = (VO_WAVEFORMATEX *)pBuffer->Buffer;
//			VO_U32 Headerlen = pBuffer->Length;

//			if(ParseHeaderMoov(palacdec, pHeader, Headerlen) == 0)
//				return VO_ERR_FAILED;

			if(pBuffer->Length != ALACHeadata->cbSize + 18)
				return VO_ERR_INVALID_ARG;

			//Get WAV format
			palacdec->m_Channels	= ALACHeadata->nChannels;
			palacdec->m_SampleBits	= ALACHeadata->wBitsPerSample;
			palacdec->m_SampleRate	= ALACHeadata->nSamplesPerSec;

			//Get Codec Data
			palacdec->m_CodecDataLen= ALACHeadata->cbSize;
			palacdec->m_pCodecData	= (VO_U8 *)ALACHeadata + 18;

			/* Set infomation */
			ALACSetInfo(palacdec);
			
			palacdec->m_Header_OK = 1;
		}
		break;

	case VO_PID_COMMON_FLUSH:
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voALACDecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)//HVOCODEC hCodec, LONG nID, LONG *plValue)
{
	CALACObjectDecoder *palacdec = (CALACObjectDecoder *)hCodec;
	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT *format = (VO_AUDIO_FORMAT *)pData;
			format->SampleRate = palacdec->m_SampleRate;
			format->Channels   = palacdec->m_Channels;
			format->SampleBits = palacdec->m_SampleBits;
		}
		break;

	case VO_PID_AUDIO_SAMPLEREATE:
		{
			VO_U32 *pRate = (VO_U32 *)pData;
			*pRate = palacdec->m_SampleRate;
		}
		break;

	case VO_PID_AUDIO_CHANNELS:
		{
			VO_U32 *pChannels = (VO_U32 *)pData;
			*pChannels = palacdec->m_Channels;
		}
		break;

	case VO_PID_AUDIO_BITRATE:
		{
//			VO_U32 *pBitrate = (VO_U32 *)pData;
//			*pBitrate = palacdec->;
		}
		break;

	case VO_PID_ALAC_OUTBUFFERSIZE:
		{
			VO_U32 *pOutSize = (VO_U32 *)pData;
			*pOutSize = 3<<13;	//1024*24;
		}
		break;

	case VO_PID_ALAC_FRAMELENGTH:
		{
			VO_U32 *pFRLength = (VO_U32 *)pData;
			*pFRLength = 3<<13;		/* 24kB buffer = 4096 samples = 1 alac frames */
		}
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_S32 VO_API voGetALACDecAPI (VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return	VO_ERR_INVALID_ARG;

	pDecHandle->Init			= voALACDecInit;
	pDecHandle->SetParam		= voALACDecSetParameter;
	pDecHandle->GetParam		= voALACDecGetParameter;
	pDecHandle->SetInputData	= voALACDecSetInputData;
	pDecHandle->GetOutputData	= voALACDecGetOutData;
	pDecHandle->Uninit			= voALACDecUninit;

	return VO_ERR_NONE;
}
