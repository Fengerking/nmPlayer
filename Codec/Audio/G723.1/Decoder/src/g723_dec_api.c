/*
ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
Universite de Sherbrooke.  All rights reserved.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedef.h"
#include "basop.h"
#include "cst_lbc.h"
#include "decod.h"
#include "dec_cng.h"

#include "g723dec.h"

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#include "voChkLcsLib.h"
#endif // only for win

int VOI_limit= 0x7fffffff;
int FrameNum = 1;


VOG723DECRETURNCODE VOCODECAPI voG723DecInit(HVOCODEC *phCodec)
{
	G723DncState *st;
	Word32  ret;
	//ret = CheckLicenseFile (NULL);
	//if(ret < 0)
	//{
	//	*phCodec = NULL;
	//	return VORC_COM_LICENSE_LIMIT;
	//}
	//else if(ret > 0)
	//	VOI_limit = ret * 50;

	/*-------------------------------------------------------------------------*
	* Memory allocation for coder state.                                      *
	*-------------------------------------------------------------------------*/
	if ((st = (G723DncState *)AlignedMalloc(sizeof(G723DncState))) == NULL)
	{
		return VORC_COM_OUT_OF_MEMORY;
	}

	st->Dec_Cng  = NULL;          
	st->Dec_Stat = NULL;

	if ((st->Dec_Cng = (DECCNGDEF *)AlignedMalloc(sizeof(DECCNGDEF))) == NULL)
	{
		return VORC_COM_OUT_OF_MEMORY;
	}

	if ((st->Dec_Stat = (DECSTATDEF *)AlignedMalloc(sizeof(DECSTATDEF))) == NULL)
	{
		return VORC_COM_OUT_OF_MEMORY;
	}

	Init_Dec_Cng(st->Dec_Cng);
	Init_Decod(st->Dec_Stat);

	st->input_buffer  = NULL;
	st->output_buffer = NULL;
	st->input_len  = 0;
	st->output_len = 0;	
	st->bitRate = 0;
	st->g_Crc  = 0;

	*phCodec = (void *) st;

	return VORC_OK;
}

VOG723DECRETURNCODE VOCODECAPI voG723DecProcess(HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECDATABUFFER *pOutData, VOCODECAUDIOFORMAT *pOutFormat)
{
 	G723DncState* st = (G723DncState*)hCodec;

	if(st == NULL)
		return VORC_COM_WRONG_STATUS;

	st->input_buffer = pInData->buffer;
	st->output_buffer = (Word16*)pOutData->buffer;

	if(FrameNum>VOI_limit)
		return VORC_COM_LICENSE_LIMIT;

	st->input_len = G723_decoder(st);

	FrameNum++;

	pOutData->length = 480;
	pInData->length  = st->input_len;//st->input_len;
	if(pOutFormat)
	{
		pOutFormat->channels = 1;
		pOutFormat->sample_rate = 8000;
		pOutFormat->sample_bits = 16;	
	}
	return VORC_OK;
}

VOG723DECRETURNCODE VOCODECAPI voG723DecUninit(HVOCODEC hCodec)
{
	G723DncState *st = (G723DncState*)hCodec;
	if(hCodec)
	{
		AlignedFree(st->Dec_Cng);
		AlignedFree(st->Dec_Stat);
		AlignedFree(hCodec);
		hCodec = NULL;
	}
	return VORC_OK;
}

VOG723DECRETURNCODE VOCODECAPI voG723DecSetParameter(HVOCODEC hCodec, LONG nID, LONG lValue)
{
	return VORC_OK;
}

VOG723DECRETURNCODE VOCODECAPI voG723DecGetParameter(HVOCODEC hCodec, LONG nID, LONG *plValue)
{
	G723DncState *st = (G723DncState*)hCodec;
	if (st == NULL) 
		return VORC_COM_OUT_OF_MEMORY;
	switch(nID)
	{
	case VOID_COM_AUDIO_FORMAT:
		{
			VOCODECAUDIOFORMAT* fmt = (VOCODECAUDIOFORMAT*)plValue;
			fmt->channels = 1;
			fmt->sample_rate = 8000;
			fmt->sample_bits = 16;
			break;
		}

	case VOID_COM_AUDIO_CHANNELS:
		*plValue = 1;
		break;
	case VOID_COM_AUDIO_SAMPLERATE:
		*plValue = 16000;
		break;
	case VOID_G723_MODE:
		*plValue = st->bitRate;
		break;
	default:
		return VORC_COM_WRONG_PARAM_ID;
	}
	return VORC_OK;
}










