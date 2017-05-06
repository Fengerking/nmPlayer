//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    oggdec.c

Abstract:

    VisualOn Ogg decoder API file.

Author:

    Witten Wen 04-September-2009

Revision History:

*************************************************************************/

#include "vovorbisdec.h"
#include "voOGG.h"
#include "vorbismemory.h"
#include "block.h"
#include "voCheck.h"
//#include "cmnMemory.h"

//#include <android/log.h>
//#define  LOG_TAG    "OGGDec"
//#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)





//#define TEST
#ifdef TEST
int PageNumber = 0;
#endif
static int GetDataBuffer(CVOVorbisObjectDecoder *pvorbisdec)
{
	if(pvorbisdec->m_pDataBuffer)
	{
		VOOGGInputBuffer *pInputBuffer = pvorbisdec->m_oggbuffer;
		voOGGMemcpy(pInputBuffer->m_pDataEnd, pvorbisdec->m_pDataBuffer, pvorbisdec->m_DataLength);
		pInputBuffer->m_DataLength	+= pvorbisdec->m_DataLength;
		pInputBuffer->m_pDataEnd	+= pvorbisdec->m_DataLength;
//		pInputBuffer->m_leftdatalength = pInputBuffer->m_DataLength;

		return pInputBuffer->m_DataLength;
	}
	else
		return 0;
}

static VO_U32 OGGDecInput(CVOVorbisObjectDecoder *pvorbisdec)
{
	VO_S32 Result = 0;
	if((Result = voOGGDecCheckHeader(pvorbisdec)) < 0)
	{
		return VO_ERR_FAILED;
	}
	else if(Result == 0)
	{
		if(pvorbisdec->m_oggbuffer->m_pNextPcktBgn != pvorbisdec->m_oggbuffer->m_pBuffer)
		{
			if(pvorbisdec->os->m_SpanLaceStore != 0) 
				BufferResetSpan(pvorbisdec);
			else 
				BufferReset(pvorbisdec->m_oggbuffer);
			voOGGDecClearBitStream(pvorbisdec);
		}

		return VO_ERR_INPUT_BUFFER_SMALL;

	}else{
		/* got a page.  Return the offset at the page beginning,
		advance the internal offset past the page end */
//		OGG_S64 Result	=	pvorbisdec->offset;
//		pvorbisdec->offset	+=	Result;

		pvorbisdec->op->m_PacketStage = pvorbisdec->m_packetstage;
	}

	if(pvorbisdec->m_ReadyState == INITSET)
	{
		if(pvorbisdec->m_CurSerialNo != voOGGDecGetPacketSerialno(pvorbisdec->m_oggbs))
			return VO_ERR_WRONG_STATUS;
	}else if(pvorbisdec->m_ReadyState == STREAMSET)
	{
		pvorbisdec->m_ReadyState = INITSET;
	}

//	if (pvorbisdec->m_HeaderRead == 0)
//		voOGGDecStreamResetSerialno(pvorbisdec->os, voOGGDecGetPacketSerialno(pvorbisdec->m_oggbs));//ogg_page_serialno(og_ptr));
//	pvorbisdec->m_CurSerialNo	= pvorbisdec->os->serialno;
//	pvorbisdec->ready_state			= STREAMSET;

	if(voOGGDecStreamPageIn(pvorbisdec->os, pvorbisdec->m_oggbs) < 0)
		return VO_ERR_INVALID_ARG;

	pvorbisdec->m_decState = OGGDecStateDecode;
	return VO_ERR_NONE;
}

VO_U32 OGGDecProcess(CVOVorbisObjectDecoder *pvorbisdec, VO_CODECBUFFER * pOutBuffer )
{
	int i, j, ret = OGG_SUCCESS;

	OGG_S32 **pcm;
	long samples;

	if (pvorbisdec->m_ReadyState<OPENED)
	{
		pvorbisdec->m_decState = OGGDecStateInput;
		return(OGG_EINVAL);
	}

	while(1)
	{
		if (pvorbisdec->m_ReadyState==INITSET)
		{
			samples = voOGGDecSynthesisPcmout(&pvorbisdec->vd, &pcm);
			if(samples)break;
		}

		/* suck in another packet */
		{
			ret = ProcessPacketFile(pvorbisdec, 1, 1);
			if (ret == OV_PACKEND)
			{
				//OV_HOLE or OV_PACKEND, either to next block.
				pvorbisdec->m_decState = OGGDecStateInput;
				voOGGDecSetNextBlock(pvorbisdec);
				voOGGDecResetOGGStreamState(pvorbisdec->os);
				return OGG_NEXT;
			}
			else if( ret == OV_SPANLACE)
			{
				pvorbisdec->m_decState = OGGDecStateInput;
				voOGGDecSetNextBlockSpan(pvorbisdec);
				RestOGGStreamStateSpan(pvorbisdec->os);
				return OGG_SPAN;
			}
		}
	}

	if (samples>0)
	{
		/* yay! proceed to pack data into the byte buffer */
//		long channels = ov_info(vf,-1)->channels;
		long channels = pvorbisdec->vi->m_Channels;

		if (channels==1)
		{
			if (samples>(long)(pOutBuffer->Length/2))
				samples = pOutBuffer->Length/2;
		}else{
			if(samples>(long)(pOutBuffer->Length/4))
				samples = pOutBuffer->Length/4;
		}

		for(i=0; i<channels; i++) 
		{ 
			/* It's faster in this order */
			OGG_S32 *src = pcm[i];
			short *dest = ((short *)pOutBuffer->Buffer) + i;
			for(j=0; j<samples; j++) 
			{
				*dest = CLIP_TO_15(src[j]>>9);//ͬ(src>0x7fff)?0x7fff:((src<0x8000)?0x8000:src); 16bits
				dest += channels;
			}
		}

		voOGGDecSynthesisRead(&pvorbisdec->vd, samples);
//		pvorbisdec->pcm_offset += samples;
		pOutBuffer->Length = samples * 2 * channels;
	}else{
		pOutBuffer->Length = samples;
	}

	return OGG_SUCCESS;
}

void OGGFree(CVOVorbisObjectDecoder *pvorbisdec)
{
	VO_MEM_OPERATOR *voMemOp = pvorbisdec->vopMemOP;
	if (pvorbisdec)
	{
		voOGGDecBlockClear(pvorbisdec, &pvorbisdec->vb);
		voOGGDecDSPClear(pvorbisdec, &pvorbisdec->vd);

		if (pvorbisdec->vi && pvorbisdec->m_Links)
		{
			int i;
			for(i=0; i<pvorbisdec->m_Links; i++)
			{
				VorbisInfoClear(pvorbisdec, pvorbisdec->vi+i);
				VorbisCommentClear(pvorbisdec, pvorbisdec->vc+i);
			}
			voOGGFree(pvorbisdec->vi);
			voOGGFree(pvorbisdec->vc);
		}

		if(pvorbisdec->m_oggbs)
		{
			voOGGFree(pvorbisdec->m_oggbs);
			pvorbisdec->m_oggbs = NULL;
		}

		if(pvorbisdec->m_packetstage)
		{
			voOGGFree(pvorbisdec->m_packetstage);
			pvorbisdec->m_packetstage = NULL;
		}

		if(pvorbisdec->op)
		{
			voOGGFree(pvorbisdec->op);
			pvorbisdec->op = NULL;
		}
		
		if(pvorbisdec->os)
		{
			voOGGFree(pvorbisdec->os);
			pvorbisdec->os = NULL;
		}

		if(pvorbisdec->m_oggbuffer)
		{
			OGGBufferUnint(pvorbisdec);
			voOGGFree(pvorbisdec->m_oggbuffer);
			pvorbisdec->m_oggbuffer = NULL;
		}
	}
	voFree(voMemOp, pvorbisdec);
}

VO_PTR	g_OGGPlayInst = NULL;

VO_U32 VO_API voOGGDecInit(VO_HANDLE * phCodec, VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VO_U32 nRet = 0;
#ifdef LCHECK
	VO_PTR pTemp;
#endif

	VO_MEM_OPERATOR *vopMemOP;

	CVOVorbisObjectDecoder *pvorbisdec;

	if(vType != VO_AUDIO_CodingOGG)
		return VO_ERR_INVALID_ARG;

	if(pUserData ==NULL || (pUserData->memflag & 0x0F) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
		return VO_ERR_INVALID_ARG;
		//cmnMemFillPointer(VO_AUDIO_CodingWMA);
		//vopMemOP = &g_memOP;
	}
	else
	{
		vopMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	}

#ifdef LCHECK
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_OGG, pUserData->memflag|1, g_OGGPlayInst, pUserData->libOperator);
	else
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_OGG, pUserData->memflag|1, g_OGGPlayInst, NULL);
	if (nRet != VO_ERR_NONE)
	{
		*phCodec = NULL;
		if(pTemp)
		{
			voCheckLibUninit(pTemp);
			pTemp = NULL;
		}
		return nRet;
	}
#endif

	pvorbisdec = (CVOVorbisObjectDecoder *)voMalloc(vopMemOP, sizeof(CVOVorbisObjectDecoder));
	if(!pvorbisdec)
	{
		return VO_ERR_OUTOF_MEMORY;
	}
	// Wipe clean the object just allocated
	vopMemOP->Set(DECODER_TYPE_ID, pvorbisdec, 0, sizeof (CVOVorbisObjectDecoder));

	pvorbisdec->vopMemOP = vopMemOP;	
	pvorbisdec->m_decState = OGGDecStateInput;

	/* init the framing state */
//	pvorbisdec->oy = ogg_sync_create(pvorbisdec);////////////////	
//	if(!pvorbisdec->oy)
//		return VO_ERR_OUTOF_MEMORY;
	pvorbisdec->m_oggbuffer = (VOOGGInputBuffer *)voOGGCalloc(1, sizeof(VOOGGInputBuffer));
	if(!pvorbisdec->m_oggbuffer)
		return VO_ERR_OUTOF_MEMORY;

	pvorbisdec->m_oggbs = (VOOGGInputBitStream *)voOGGCalloc(1, sizeof(VOOGGInputBitStream));
	if(!pvorbisdec->m_oggbs)
		return VO_ERR_OUTOF_MEMORY;

	pvorbisdec->m_packetstage = (PacketStage *)voOGGCalloc(1, sizeof(PacketStage));
	if(!pvorbisdec->m_packetstage)
		return VO_ERR_OUTOF_MEMORY;

	pvorbisdec->op = (OGGPacket *)voOGGCalloc(1, sizeof(OGGPacket));
	if(!pvorbisdec->op)
		return VO_ERR_OUTOF_MEMORY;

	/* No seeking yet; Set up a 'single' (current) logical bitstream
	entry for partial open */
	pvorbisdec->m_Links = 1;
//	pvorbisdec->read_mode = 1;
	pvorbisdec->m_fileformat = 0;

	pvorbisdec->vi= (VorbisInfo *)voOGGCalloc(pvorbisdec->m_Links, sizeof(VorbisInfo));//_ogg_calloc(vf->m_Links,sizeof(*vf->vi));
	if(!pvorbisdec->vi)
		return VO_ERR_OUTOF_MEMORY;

	pvorbisdec->vc= (VorbisComment *)voOGGCalloc(pvorbisdec->m_Links, sizeof(VorbisComment));//_ogg_calloc(vf->m_Links,sizeof(*vf->vc));
	if(!pvorbisdec->vc)
		return VO_ERR_OUTOF_MEMORY;

	pvorbisdec->os = OGGStreamCreate(pvorbisdec, -1); /* fill in the serialno later */
	if(!pvorbisdec->os)
		return VO_ERR_OUTOF_MEMORY;

	if(OGGBufferInit(pvorbisdec) != VO_ERR_NONE)
		return VO_ERR_OUTOF_MEMORY;
	if(voOGGDecInfoInit(pvorbisdec, pvorbisdec->vi) == VO_ERR_OUTOF_MEMORY)
		return VO_ERR_OUTOF_MEMORY;

	VorbisCommentInit(pvorbisdec->vc);

#ifdef LCHECK
	pvorbisdec->hCheck = pTemp;	
#endif

	*phCodec = (VO_HANDLE)pvorbisdec;
	return VO_ERR_NONE;
}

VO_U32 VO_API voOGGDecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	CVOVorbisObjectDecoder *pvorbisdec = (CVOVorbisObjectDecoder *)hCodec;

	if(pvorbisdec == NULL)
		return VO_ERR_INVALID_ARG;

	if(!pvorbisdec->m_HeaderOK)
		return VO_ERR_INVALID_ARG;

	if(!(pInput && pInput->Buffer && pInput->Length > 0))
		return VO_ERR_INVALID_ARG;

	if(pvorbisdec->m_fileformat == 0)
	{
		pvorbisdec->m_oggbs->m_ptr = pInput->Buffer;
		pvorbisdec->m_oggbs->m_PacketLength = pInput->Length;
		pvorbisdec->m_oggbs->m_CBitUsed		= 0;
	}
	else
	{
		pvorbisdec->m_pDataBuffer = pInput->Buffer;
		pvorbisdec->m_DataLength = pInput->Length;
		if(pvorbisdec->m_DataLength > 0)
		{
			if(GetDataBuffer(pvorbisdec) == 0)
				return VO_ERR_INPUT_BUFFER_SMALL;		
		}
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voOGGDecGetOutData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	CVOVorbisObjectDecoder *pvorbisdec = (CVOVorbisObjectDecoder *)hCodec;
	VO_U32	hr = 0;
	VO_S32	samples, i, j;
	OGG_S32 **pcm;

	if(pvorbisdec->m_fileformat == 0)
	{
		long channels;
		
		if(!(pvorbisdec && pOutBuffer && pOutBuffer->Buffer && pvorbisdec->m_oggbs->m_ptr))
			return VO_ERR_INVALID_ARG;

		if(pvorbisdec->m_oggbs->m_PacketLength == 0)
			goto gCleanup;

		pvorbisdec->m_inputused += pvorbisdec->m_oggbs->m_PacketLength;

		hr = ProcessPacket(pvorbisdec, 1, 1);
		if(hr == OV_ENOTAUDIO)
			goto gCleanup;
		
		samples = voOGGDecSynthesisPcmout(&pvorbisdec->vd, &pcm);
		if(!samples) goto gCleanup; 

		/* ok! proceed to pack data into the byte buffer */
//		long channels = ov_info(vf,-1)->channels;
		channels = pvorbisdec->vi->m_Channels;

		if (channels==1)
		{
			if (samples>(long)(pOutBuffer->Length/2))
				samples = pOutBuffer->Length/2;
		}else{
			if(samples>(long)(pOutBuffer->Length/4))
				samples = pOutBuffer->Length/4;
		}

		for(i=0; i<channels; i++) 
		{ 
			/* It's faster in this order */
			OGG_S32 *src = pcm[i];
			short *dest = ((short *)pOutBuffer->Buffer) + i;
			for(j=0; j<samples; j++) 
			{
				*dest = CLIP_TO_15(src[j]>>9);//ͬ(src>0x7fff)?0x7fff:((src<0x8000)?0x8000:src); 16bits
				dest += channels;
			}
		}

		voOGGDecSynthesisRead(&pvorbisdec->vd, samples);
//		pvorbisdec->pcm_offset += samples;
		pOutBuffer->Length = samples * 2 * channels;
		pvorbisdec->m_oggbs->m_PacketLength = 0;
	}
	else
	{
		while(1)
		{
			switch(pvorbisdec->m_decState)
			{
			case OGGDecStateInput:
				hr = OGGDecInput(pvorbisdec);
				if(hr != VO_ERR_NONE)
					goto gCleanup;
				break;

			case OGGDecStateDecode:
				hr = OGGDecProcess(pvorbisdec, pOutBuffer);
				if(hr == OGG_SUCCESS)
					goto gEnd;
				break;

			case OGGDecStateDone:
				goto gEnd;
			}
		}
	}

gEnd:
	if(pOutInfo)
	{
		pOutInfo->Format.Channels	= pvorbisdec->vi->m_Channels;
		pOutInfo->Format.SampleBits	= pvorbisdec->vi->m_SampleBits;
		pOutInfo->Format.SampleRate	= pvorbisdec->vi->m_Rate;
		pOutInfo->InputUsed			= pvorbisdec->m_inputused;
		pvorbisdec->m_inputused		= 0;
	}
#ifdef LCHECK
	//License Check
	if(voCheckLibCheckAudio(pvorbisdec->hCheck, pOutBuffer, &(pOutInfo->Format)) != VO_ERR_NONE)
		return VO_ERR_LICENSE_ERROR;
#endif
	return VO_ERR_NONE;

gCleanup:
//	pOutInfo->InputUsed = pvorbisdec->m_oggbuffer->m_CurBlockLen;
	return VO_ERR_INPUT_BUFFER_SMALL;
}

VO_U32 VO_API voOGGDecUninit(VO_HANDLE hCodec)
{
	CVOVorbisObjectDecoder *pvorbisdec = (CVOVorbisObjectDecoder *)hCodec;

#ifdef LCHECK
	if(pvorbisdec->hCheck != NULL)
		voCheckLibUninit(pvorbisdec->hCheck);
#endif	

	OGGFree(pvorbisdec);
	
	return VO_ERR_NONE;
}

VO_U32 VO_API voOGGDecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	CVOVorbisObjectDecoder *pvorbisdec = NULL;
	int Result = 0;

	if(hCodec == NULL)
		return VO_ERR_INVALID_ARG;
//	if(pData == NULL)
//		return VO_ERR_INVALID_ARG;

	pvorbisdec = (CVOVorbisObjectDecoder *)hCodec;

	switch(uParamID)
	{
	case VO_PID_OGG_FORMATFILE:
		pvorbisdec->m_fileformat = 1;
		break;

	case VO_PID_AUDIO_FORMAT:
		break;

	case VO_PID_COMMON_HEADDATA:
		{
			VO_CODECBUFFER *pBuffer = (VO_CODECBUFFER *)pData;

			//LOGI("In Set HeadData\n");
			if(pvorbisdec->m_fileformat == 0)
			{
				struct VORBISFORMAT2 *format2 = (struct VORBISFORMAT2 *)pBuffer->Buffer;
				VO_U8 *ptemp = NULL;
				int headlength = 0;
				if(!format2)
					return VO_ERR_INVALID_ARG;

				
				headlength = format2->HeadPacketSize[0] + format2->HeadPacketSize[1] + 
								format2->HeadPacketSize[2] + sizeof(struct VORBISFORMAT2);

				//LOGI("Headlength = %d, Inbuffer = %d\n",headlength, pBuffer->Length);

				ptemp = (VO_U8 *)format2 + sizeof(struct VORBISFORMAT2);
				if(pBuffer->Length != headlength)
					return VO_ERR_INVALID_ARG;

				for (pvorbisdec->m_HeaderRead = 0; pvorbisdec->m_HeaderRead < 3; pvorbisdec->m_HeaderRead++)
				{
					pvorbisdec->m_oggbs->m_ptr = ptemp;
					pvorbisdec->m_oggbs->m_PacketLength = format2->HeadPacketSize[pvorbisdec->m_HeaderRead];
					if ((Result = voOGGDecHeaderParser(pvorbisdec)))
					{
						return VO_ERR_FAILED;
					}
					ptemp += format2->HeadPacketSize[pvorbisdec->m_HeaderRead];
				}
				if(pvorbisdec->m_HeaderRead == 3)
				{
					pvorbisdec->m_HeaderOK = 1;
					pvorbisdec->m_PacketNo	= 0;
					voOGGDecSynthesisInit(pvorbisdec, &pvorbisdec->vd, pvorbisdec->vi);
					voOGGDecBlockInit(&pvorbisdec->vd, &pvorbisdec->vb);
					pvorbisdec->samptrack = 0;
				}
			}
			else
			{
				/* Input data length must be more than or equal to the head data length, 
				or the decoder will return VO_ERR_INPUT_BUFFER_SMALL */			

				pvorbisdec->m_pDataBuffer = pBuffer->Buffer;
				pvorbisdec->m_DataLength = pBuffer->Length;

				if(pvorbisdec->m_DataLength > 0)
				{
					Result = GetDataBuffer(pvorbisdec);
					if(Result == 0)
						return VO_ERR_INPUT_BUFFER_SMALL;
				}

				while (pvorbisdec->m_HeaderRead < 3)
				{
					if(pvorbisdec->m_HeaderRead < 2 || pvorbisdec->os->m_SpanLaceStore)//maybe the header will span
					{
						Result = voOGGDecCheckHeader(pvorbisdec);//, pvorbisdec->oy, &og);
						//				og_ptr = &og;
						if(Result < 0)
						{
							return VO_ERR_FAILED;
						}
						else if(Result == 0)
						{
							return VO_ERR_INPUT_BUFFER_SMALL;
						}else{
							/* got a page.  Return the offset at the page beginning,
							advance the internal offset past the page end */
							//						OGG_S64 Result	=	pvorbisdec->offset;
							//						pvorbisdec->offset	+=	Result;

							pvorbisdec->op->m_PacketStage = pvorbisdec->m_packetstage;
						}

						if (pvorbisdec->m_HeaderRead == 0)
							voOGGDecStreamResetSerialno(pvorbisdec->os, voOGGDecGetPacketSerialno(pvorbisdec->m_oggbs));//ogg_page_serialno(og_ptr));
						pvorbisdec->m_CurSerialNo	= pvorbisdec->os->m_SerialNo;
						pvorbisdec->m_ReadyState			= STREAMSET;

						voOGGDecStreamPageIn(pvorbisdec->os, pvorbisdec->m_oggbs);
					}
					//				Result = StreamPacketOut(pvorbisdec->os, pvorbisdec, pvorbisdec->op);//have problem that only step in once.
					Result = voOGGDecGetPacket(pvorbisdec, pvorbisdec->os, pvorbisdec->op);
					if (Result == OV_SPANLACE) 
					{
						voOGGDecSetNextBlockSpan(pvorbisdec);
						RestOGGStreamStateSpan(pvorbisdec->os);
						continue;
					}

					if ((Result = voOGGDecHeaderParser(pvorbisdec)))
					{
						//					ogg_packet_release(&op);
						//					ogg_page_release(&og);
						return VO_ERR_FAILED;
					}
					pvorbisdec->m_HeaderRead++;
				}
				if(pvorbisdec->m_HeaderRead == 3)
				{
					pvorbisdec->m_HeaderOK = 1;
					voOGGDecSaveHeadRestData(pvorbisdec->m_oggbuffer);
					if(pvorbisdec->os->m_Laceptr != pvorbisdec->os->m_LaceSize)
					{
						//there is frame data in head data, and set decode mode.
						pvorbisdec->m_decState = OGGDecStateDecode;
						pvorbisdec->m_ReadyState = INITSET;
						pvorbisdec->m_packetstage->m_pPageBgn = pvorbisdec->m_oggbuffer->m_pBuffer;
					}
					//				if(pvorbisdec->m_Seekable)
					//					voOGGDecSynthesisInit(pvorbisdec, &pvorbisdec->vd, pvorbisdec->vi+pvorbisdec->current_link);
					//				else
					voOGGDecSynthesisInit(pvorbisdec, &pvorbisdec->vd, pvorbisdec->vi);

					voOGGDecBlockInit(&pvorbisdec->vd, &pvorbisdec->vb);
					//				vf->m_ReadyState = INITSET;
					//				pvorbisdec->bittrack = 0;
					pvorbisdec->samptrack = 0;
				}
				else
					return VO_ERR_FAILED;
			}
		}
		break;

	case VO_PID_COMMON_FLUSH:
		if(pvorbisdec->m_fileformat == 0)
		{
			pvorbisdec->m_PacketNo = 0;
		}
		else
		{
			pvorbisdec->m_decState = OGGDecStateInput;
			pvorbisdec->m_oggbuffer->m_DataLength = 0;
			pvorbisdec->m_oggbs->m_Flag = VO_FALSE;
			BufferReset(pvorbisdec->m_oggbuffer);
			voOGGDecStreamReset(pvorbisdec->os);
			PacketStageReset(pvorbisdec->m_packetstage);
			pvorbisdec->os->m_ResetFlag = VO_TRUE;
		}
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}


VO_U32 VO_API voOGGDecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	CVOVorbisObjectDecoder *pvorbisdec = (CVOVorbisObjectDecoder *)hCodec;
	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT *format = (VO_AUDIO_FORMAT *)pData;
			format->SampleRate = pvorbisdec->vi->m_Rate;
			format->Channels   = pvorbisdec->vi->m_Channels;
			format->SampleBits = pvorbisdec->vi->m_SampleBits;
		}
		break;

	case VO_PID_AUDIO_SAMPLEREATE:
		{
			long *pRate = (long *)pData;
			*pRate = pvorbisdec->vi->m_Rate;
		}
		break;

	case VO_PID_AUDIO_CHANNELS:
		{
			int *pChannel = (int *)pData;
			*pChannel = pvorbisdec->vi->m_Channels;
		}
		break;

	case VO_PID_AUDIO_BITRATE:
		{
			long *pBitrate = (long *)pData;
			*pBitrate = pvorbisdec->vi->m_BitrateNom;
		}
		break;

	case VO_PID_OGG_OUTBUFFERSIZE:
		{
			CodecSetupInfo *csi = (CodecSetupInfo *)pvorbisdec->vi->m_pCodecSetup;
			long *pOutSize = (long *)pData;
			*pOutSize = ((pvorbisdec->vi->m_Channels * pvorbisdec->vi->m_SampleBits)>>3) * csi->m_BlockSize[1];
		}
		break;

	case VO_PID_OGG_FRAMELENGTH:
		{
			CodecSetupInfo *csi = (CodecSetupInfo *)pvorbisdec->vi->m_pCodecSetup;
			long *pFRLength = (long *)pData;
			*pFRLength = csi->m_BlockSize[1];
		}
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}


VO_S32 VO_API voGetOGGDecAPI (VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return	VO_ERR_INVALID_ARG;

	pDecHandle->Init			= voOGGDecInit;
	pDecHandle->SetParam		= voOGGDecSetParameter;
	pDecHandle->GetParam		= voOGGDecGetParameter;
	pDecHandle->SetInputData	= voOGGDecSetInputData;
	pDecHandle->GetOutputData	= voOGGDecGetOutData;
	pDecHandle->Uninit			= voOGGDecUninit;

	return VO_ERR_NONE;
}

