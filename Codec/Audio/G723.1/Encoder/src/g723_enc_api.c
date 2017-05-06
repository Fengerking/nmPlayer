/*
**
** File:        "lbccodec.c"
**
** Description:     Top-level source code for G.723 dual-rate codec
**
** Functions:       main
**                  Process_files()
**
**
*/


/*
ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
Universite de Sherbrooke.  All rights reserved.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "typedef.h"
#include "basop.h"
#include "cst_lbc.h"
#include "tab_lbc.h"
#include "coder.h"
#include "decod.h"
#include "exc_lbc.h"
#include "util_lbc.h"
#include "cod_cng.h"
#include "dec_cng.h"
#include "vad.h"
#include "g723enc.h"

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE)
#include "voChkLcsLib.h"
#endif // only for win

int VOI_limit= 0x7fffffff;
int FrameNum = 1;


VOG723ENCRETURNCODE VOCODECAPI voG723EncInit(HVOCODEC *phCodec)
{
	G723EncState *st;
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
	if ((st = (G723EncState *)AlignedMalloc(sizeof(G723EncState))) == NULL)
	{
		return VORC_COM_OUT_OF_MEMORY;
	}

	st->cod_cng = NULL;          
	st->cod_state = NULL;
	st->vad_state = NULL;

	if ((st->cod_cng = (CODCNGDEF *)AlignedMalloc(sizeof(CODCNGDEF))) == NULL)
	{
		return VORC_COM_OUT_OF_MEMORY;
	}

	if ((st->cod_state = (CODSTATDEF *)AlignedMalloc(sizeof(CODSTATDEF))) == NULL)
	{
		return VORC_COM_OUT_OF_MEMORY;
	}

	if ((st->vad_state = (VADSTATDEF *)AlignedMalloc(sizeof(VADSTATDEF))) == NULL)
	{
		return VORC_COM_OUT_OF_MEMORY;
	}

	Init_Cod_Cng(st->cod_cng);
	Init_Coder(st->cod_state);
	Init_Vad(st->vad_state);

	st->input_buffer  = NULL;
	st->output_buffer = NULL;
	st->input_len  = 0;
	st->output_len = 0;
	st->frametype = 0;
	st->Ratemode = 0;
	st->DTX_CNG  = 0;

	*phCodec = (void *) st;

	return VORC_OK;
}

VOG723ENCRETURNCODE VOCODECAPI voG723EncProcess(HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECDATABUFFER *pOutData, VOCODECAUDIOFORMAT *pOutFormat)
{
 	G723EncState* st = (G723EncState*)hCodec;

	if(st == NULL)
		return VORC_COM_WRONG_STATUS;

	st->input_buffer = (Word16*)pInData->buffer;
	st->output_buffer = (Word16*)pOutData->buffer;

	if(FrameNum>VOI_limit)
		return VORC_COM_LICENSE_LIMIT;

	if(st->Ratemode == Rate53)
		reset_max_time();
	st->output_len = G723_Encoder(st);

	FrameNum++;

	pOutData->length = st->output_len;
	pInData->length  = 240;//st->input_len;
	if(pOutFormat)
	{
		pOutFormat->channels = 1;
		pOutFormat->sample_rate = 8000;
		pOutFormat->sample_bits = 16;	
	}
	return VORC_OK;
}

VOG723ENCRETURNCODE VOCODECAPI voG723EncUninit(HVOCODEC hCodec)
{
	G723EncState *st = (G723EncState*)hCodec;
	if(hCodec)
	{
		AlignedFree(st->cod_cng);
		AlignedFree(st->cod_state);
		AlignedFree(st->vad_state);
		AlignedFree(hCodec);
		hCodec = NULL;
	}
	return VORC_OK;
}

VOG723ENCRETURNCODE VOCODECAPI voG723EncSetParameter(HVOCODEC hCodec, LONG nID, LONG lValue)
{
	G723EncState *st = (G723EncState *)hCodec;
	if(st == NULL)
		return VORC_COM_WRONG_STATUS;
	switch(nID)
	{
	case VOID_G723_FRAMETYPE:
			if(lValue < VOG723_DEFAULT|| lValue > VOG723_ITU)
				return VORC_COM_WRONG_PARAM_ID; 
			st->frametype = (Word16)lValue;
			break;

	case VOID_G723_MODE:
		{

			if(lValue < VOG723_MD53||lValue > VOG723_MD63)
				return VORC_COM_WRONG_PARAM_ID; 
			st->Ratemode = (Word16)lValue;
		}
		break;
	case VOID_G723_DTX:
		st->DTX_CNG = (Word16)lValue;
		break;

	default:
		return VORC_COM_WRONG_PARAM_ID;
	}
	return VORC_OK;
}

VOG723ENCRETURNCODE VOCODECAPI voG723EncGetParameter(HVOCODEC hCodec, LONG nID, LONG *plValue)
{
	G723EncState *st = (G723EncState*)hCodec;
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
	case VOID_G723_FRAMETYPE:
		*plValue = st->frametype;
		break;
	case VOID_G723_MODE:
		*plValue = st->Ratemode;
		break;
	default:
		return VORC_COM_WRONG_PARAM_ID;
	}
	return VORC_OK;
}










