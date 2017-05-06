/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
ITU-T G.729A Speech Coder with Annex B    ANSI-C Source Code
Version 1.5    Last modified: October 2006

Copyright (c) 1996,
AT&T, France Telecom, NTT, Universite de Sherbrooke, Lucent Technologies,
Rockwell International
All rights reserved.
*/

/*-----------------------------------------------------------------*
* Main program of the G.729A 8.0 kbit/s decoder.                  *
*                                                                 *
*    Usage : decoder  bitstream_file  synth_file                  *
*                                                                 *
*-----------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"
#include "dtx.h"
#include "octet.h"
#include "voAudio.h"
#include "mem_align.h"
#include "cmnMemory.h"
#include "voG729.h"
#include "voIndex.h"
#include "voCheck.h"
#include "g729dec_api.h"     
/*
This variable should be always set to zero unless transmission errors
in LSP indices are detected.
This variable is useful if the channel coding designer decides to
perform error checking on these important parameters. If an error is
detected on the  LSP indices, the corresponding flag is
set to 1 signalling to the decoder to perform parameter substitution.
(The flags should be set back to 0 for correct transmission).
*/
/*-----------------------------------------------------------------*
*            global data structure                                *
*-----------------------------------------------------------------*/
Word16  parm[PRM_SIZE+2];             /* Synthesis parameters        */
Word16  Az_dec[MP1*2];                /* Decoded Az for post-filter  */
Word16  T2[2];                        /* Pitch lag for 2 subframes   */
Word16  serial[SERIAL_SIZE];          /* Serial stream    SERIAL_SIZE = 82*/

void *g_hG729DecInst = NULL;

/*-----------------------------------------------------------------*
*            Main decoder routine                                 *
*-----------------------------------------------------------------*/
#ifdef DUMP
FILE *dump_dat = NULL;
#endif

VO_U32 VO_API voG729D_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData )
{
	G729DGlobalData *gData;
	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	int interMem = 0;
	if(pUserData == NULL || pUserData->memflag != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
		voMemoprator.Alloc = cmnMemAlloc;
		voMemoprator.Copy = cmnMemCopy;
		voMemoprator.Free = cmnMemFree;
		voMemoprator.Set = cmnMemSet;
		voMemoprator.Check = cmnMemCheck;
		interMem = 1;
		pMemOP = &voMemoprator;
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	} 

	gData = (G729DGlobalData *)mem_malloc(pMemOP, sizeof(G729DGlobalData), 32);

	if(gData == NULL)
		return VO_ERR_OUTOF_MEMORY;

	gData->inbuf_ptr = NULL;
	gData->outbuf_ptr = NULL;
	gData->PackType  = 0;             /* 0 --- Raw data, 1 --- Add Sync word*/
	gData->bad_lsf = 0;               /* Initialize bad LSF indicator */

	Init_Decod_ld8a();
	Init_Post_Filter();
	Init_Post_Process();

	if(interMem)
	{
		gData->voMemoprator.Alloc = cmnMemAlloc;
		gData->voMemoprator.Copy = cmnMemCopy;
		gData->voMemoprator.Free = cmnMemFree;
		gData->voMemoprator.Set = cmnMemSet;
		gData->voMemoprator.Check = cmnMemCheck;
		pMemOP = &gData->voMemoprator;
	}
	gData->pvoMemop = pMemOP;

	*phCodec = (VO_HANDLE)gData;

#ifdef DUMP
	dump_dat = fopen("E:\g729dump.dat", "wb");
#endif

#ifdef LCHECK
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
	    voCheckLibInit(&(gData->hCheck), VO_INDEX_DEC_G729, 1, g_hG729DecInst, pUserData->libOperator);
	else
		voCheckLibInit(&(gData->hCheck), VO_INDEX_DEC_G729, 1, g_hG729DecInst, NULL);
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voG729D_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	G729DGlobalData  *gData;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (G729DGlobalData *)hCodec;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData->inbuf_ptr = pInput->Buffer;
	gData->inbuf_len = pInput->Length;	

	return VO_ERR_NONE;
}


VO_U32 VO_API voG729D_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	int   i;
	short Vad=0;
	int   Packnum = 0;
	G729DGlobalData *gData;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData = (G729DGlobalData *)hCodec;
	gData->outbuf_ptr = pOutput->Buffer;

	if(gData->PackType == 0)
	{
		if(gData->inbuf_len == 10)
		{
		    serial[0] = SYNC_WORD;
		    serial[1] = RATE_8000;
			Packnum = 5;
		    memcpy((unsigned char *)&serial[2], gData->inbuf_ptr, gData->inbuf_len);
		}
		else if(gData->inbuf_len == 2)
		{
		    serial[0] = SYNC_WORD;
		    serial[1] = RATE_SID_OCTET;
			Packnum = 1;
		    memcpy((unsigned char *)&serial[2], gData->inbuf_ptr, gData->inbuf_len);
		}
		else
		{
			serial[0] = SYNC_WORD;
			serial[1] = 0;
		}
	}
	else
	{
		memcpy((unsigned char *)serial, gData->inbuf_ptr, gData->inbuf_len);
	}

	bits2prm_ld8k(&serial[1], parm);
	parm[0] = 0;

	if(serial[1] != 0){
		for (i=0; i < Packnum; i++)
			if(serial[i+2] == 0) 
				parm[0] = 1;  //frame erased
	}
	else if(serial[0] != SYNC_WORD) 
		parm[0] = 1;

	if(parm[1] == 1){
		parm[5] = Check_Parity_Pitch(parm[4], parm[5]);
	}

	Decod_ld8a(parm, gData, Az_dec, T2, &Vad);
	Post_Filter((Word16 *)gData->outbuf_ptr, Az_dec, T2, Vad);        /* Post-filter */
	Post_Process((Word16 *)gData->outbuf_ptr, L_FRAME);

	gData->outbuf_len = 160;

#ifdef DUMP
	fwrite(gData->outbuf_ptr, 1, gData->outbuf_len, dump_dat);
#endif
	pOutput->Length = gData->outbuf_len;

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleRate = 8000;
		pAudioFormat->Format.SampleBits = 16;	
		pAudioFormat->InputUsed = 10;
	}

#ifdef LCHECK
	voCheckLibCheckAudio(gData->hCheck, pOutput, &(pAudioFormat->Format));
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voG729D_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	G729DGlobalData* gData = (G729DGlobalData*)hCodec;
	int  *lValue = (int*)pData;

	switch(uParamID)
	{
	case VO_PID_G729_FRAMETYPE:
		if(*lValue < VOG729RAW_DATA||*lValue > VOG729PACK_DATA)
			return VO_ERR_WRONG_PARAM_ID; 
		gData->PackType = *lValue;
		break;

	case VO_PID_COMMON_HEADDATA:
		break;

	case VO_PID_COMMON_FLUSH:
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}


VO_U32 VO_API voG729D_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	int temp;
	G729DGlobalData* gData = (G729DGlobalData*)hCodec;
	if (gData==NULL) 
		return VO_ERR_OUTOF_MEMORY;

	switch(uParamID)
	{
	case VO_PID_G729_FORMAT:
		{
			VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;

			fmt->Channels    = 1;
			fmt->SampleRate  = 8000;
			fmt->SampleBits  = 16;
			break;
		}

	case VO_PID_G729_CHANNELS:
		temp = 1;
		pData = (void *)(&temp);
		break;

	case VO_PID_G729_SAMPLERATE:
		temp = 8000;
		pData = (void *)(&temp);
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API voG729D_Uninit(VO_HANDLE hCodec)
{
	G729DGlobalData *gData;	
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (G729DGlobalData *)hCodec;

	pMemOP = gData->pvoMemop;

#if defined (LCHECK)
	voCheckLibUninit(gData->hCheck);
#endif

	mem_free(pMemOP, hCodec);

#ifdef DUMP
	fclose(dump_dat);
#endif
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetG729DecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voG729D_Init;
	pDecHandle->SetInputData = voG729D_SetInputData;
	pDecHandle->GetOutputData = voG729D_GetOutputData;
	pDecHandle->SetParam = voG729D_SetParam;
	pDecHandle->GetParam = voG729D_GetParam;
	pDecHandle->Uninit = voG729D_Uninit;
	return VO_ERR_NONE;
}



