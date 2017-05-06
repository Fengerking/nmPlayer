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
#include "defs.h"
#include "basop32.h"
#include "mem_align.h"
#include "cmnMemory.h"
#include "voG722.h"
#include "voCheck.h"

#define MAX_SAMPLE_RATE 32000
#define MAX_FRAMESIZE   (MAX_SAMPLE_RATE/50)


void write_ITU_format(G722EncState* st, Word16 number_of_16bit_words_per_frame);
void *g_hG722EncInst = NULL;

VO_U32 VO_API voG722_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	Word16 i;
	G722EncState *st;
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

	/*-------------------------------------------------------------------------*
	* Memory allocation for coder state.                                      *
	*-------------------------------------------------------------------------*/
	if ((st = (G722EncState *)mem_malloc(pMemOP, sizeof(G722EncState), 32)) == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}        
	st->input      = NULL;
	st->output     = NULL;
	st->SampleRate = 16000;                 //default sample rate: 16KHz
	st->bit_rate   = 0;						//default bit_rate: 24kbit/s
	st->syntax     = 0;						//default pack bit_stream
	st->bandwidth  = 7000;					//default bandwidth is 7KHz
	st->frame_size = 0;
	st->number_of_bits_per_frame  = 0;
	st->number_of_regions = 0;

	/* initialize the mlt history buffer */
	for (i=0; i< 640; i++)
		st->history[i] = 0;

	if(interMem)
	{
		st->voMemoprator.Alloc = cmnMemAlloc;
		st->voMemoprator.Copy = cmnMemCopy;
		st->voMemoprator.Free = cmnMemFree;
		st->voMemoprator.Set = cmnMemSet;
		st->voMemoprator.Check = cmnMemCheck;
		pMemOP = &st->voMemoprator;
	}
	st->pvoMemop = pMemOP;

	*phCodec = (VO_HANDLE)st;

#ifdef LCHECK
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		voCheckLibInit(&(st->hCheck), VO_INDEX_ENC_G722, 1, g_hG722EncInst, pUserData->libOperator);
	else
		voCheckLibInit(&(st->hCheck), VO_INDEX_ENC_G722, 1, g_hG722EncInst, NULL);
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voG722_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	G722EncState *gData;
	if (NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (G722EncState *)hCodec;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData->input  = (Word16 *)pInput->Buffer;
	gData->InLens = pInput->Length >> 1;

	return VO_ERR_NONE;
}

VO_U32 VO_API voG722_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	Word16  mlt_coefs[MAX_FRAMESIZE];
	Word16  mag_shift;
	Word16  number_of_16bit_words_per_frame;

	G722EncState* st = (G722EncState*)hCodec;

	if(st == NULL)
		return VO_ERR_WRONG_PARAM_ID;

	st->number_of_bits_per_frame = (Word16)((st->bit_rate)/50);

	if(st->bandwidth == 7000)
	{
		st->number_of_regions = NUMBER_OF_REGIONS;
		st->frame_size = MAX_FRAMESIZE >> 1;
		st->SampleRate = 16000;
	}
	else if(st->bandwidth == 14000)
	{
		st->number_of_regions = MAX_NUMBER_OF_REGIONS;
		st->frame_size = MAX_FRAMESIZE;
		st->SampleRate = 32000;
	}

	if (st->InLens != st->frame_size)
	{
		return VO_ERR_INVALID_ARG;
	}
	number_of_16bit_words_per_frame = (Word16)(st->number_of_bits_per_frame/16);
	st->output = (Word16*)pOutput->Buffer;

	//memcpy(input,st->input, st->frame_size<<1);

	mag_shift = samples_to_rmlt_coefs(st, mlt_coefs);

	encoder(st, mlt_coefs,mag_shift);

	if(st->syntax == 0)
		pOutput->Length = (number_of_16bit_words_per_frame<<1);
	else
	{
		pOutput->Length = (st->number_of_bits_per_frame + 2)<<1;
		write_ITU_format(st, number_of_16bit_words_per_frame);
	}

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleRate = st->SampleRate;
		pAudioFormat->Format.SampleBits = 16;	
		pAudioFormat->InputUsed = st->frame_size;	
	}

#ifdef LCHECK
	voCheckLibCheckAudio(st->hCheck, pOutput, &(pAudioFormat->Format));
#endif

	return VO_ERR_NONE;

}

VO_U32 VO_API voG722_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	G722EncState *st = (G722EncState *)hCodec;
	int  *lValue = (int*)pData;

	if(st == NULL)
		return VO_ERR_INVALID_ARG;

	switch(uParamID)
	{
	case VO_PID_G722_FRAMETYPE:
		if(*lValue < VOG722_DEFAULT|| *lValue > VOG722_ITU)
			return VO_ERR_WRONG_PARAM_ID; 
		st->syntax = (Word16)*lValue;
		break;

	case VO_PID_G722_MODE:
		{
			if(*lValue < VOG722_MD24||*lValue > VOG722_MD48)
				return VO_ERR_WRONG_PARAM_ID; 
			if((Word16)*lValue == 0)
				st->bit_rate  = 24000;
			else if((Word16)*lValue == 1)
				st->bit_rate  = 32000;
			else
				st->bit_rate  = 48000;
		}
		break;
	case VO_PID_G722_MRATE:
		{
			if(*lValue < VOG722_7K ||*lValue > VOG722_14K)
				return VO_ERR_WRONG_PARAM_ID; 
			if((Word16)*lValue == 0)
				st->bandwidth = 7000;
			else
				st->bandwidth = 14000;
		}
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;

}

VO_U32 VO_API voG722_GetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	G722EncState *st = (G722EncState*)hCodec;
	if (st == NULL) 
		return VO_ERR_INVALID_ARG;
	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;
			fmt->Channels = 1;
			fmt->SampleRate = st->SampleRate;
			fmt->SampleBits = 16;
			break;
		}

	case VO_PID_AUDIO_CHANNELS:
		*((int *)pData) = 1;
		break;
	case VO_PID_AUDIO_SAMPLEREATE:
		*((int *)pData) = st->SampleRate;
		break;
	case VO_PID_G722_FRAMETYPE:
		*((int *)pData) = st->syntax;
		break;
	case VO_PID_G722_MODE:
		*((int *)pData) = st->bit_rate;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;

}

VO_U32 VO_API voG722_Uninit(VO_HANDLE hCodec)
{
	G722EncState *st;
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	st = (G722EncState *)hCodec;
	pMemOP = st->pvoMemop;

#ifdef LCHECK
	voCheckLibUninit(st->hCheck);
#endif

	if(hCodec)
	{
		mem_free(pMemOP, hCodec);
		hCodec = NULL;
	}
	return VO_ERR_NONE;

}

/************************************************************************************
 Procedure/Function:     Write ITU format function 

 Syntax:                void write_ITU_format(Word16 *out_words,
                                              Word16 number_of_bits_per_frame,
                                              Word16 number_of_16bit_words_per_frame,
                                              FILE   *fp_bitstream)

 Description:           Writes file output in PACKED ITU format


************************************************************************************/

void write_ITU_format(G722EncState* st,Word16 number_of_16bit_words_per_frame)

{
	Word16 *out_words = (Word16 *)st->output;
	Word16 number_of_bits_per_frame = st->number_of_bits_per_frame;
    Word16 frame_start = 0x6b21;
    Word16 one = 0x0081;
    Word16 zero = 0x007f;

    Word16  i,j;
    Word16  packed_word;
    Word16  bit_count;
    Word16  bit;
    Word16  out_array[MAX_BITS_PER_FRAME+2];

    j = 0;
    out_array[j++] = frame_start;
    out_array[j++] = number_of_bits_per_frame;

    for (i=0; i<number_of_16bit_words_per_frame; i++)
    {
        packed_word = out_words[i];
        bit_count = 15;
        while (bit_count >= 0)
        {
            bit = (Word16)((packed_word >> bit_count) & 1);
            bit_count--;
            if (bit == 0)
                out_array[j++] = zero;
            else
                out_array[j++] = one;
        }
    }
    memcpy(st->output, out_array, (number_of_bits_per_frame+2)<<1);
    //fwrite(out_array, 2, number_of_bits_per_frame+2, fp_bitstream);
}


VO_S32 VO_API voGetG722EncAPI(VO_AUDIO_CODECAPI * pEncHandle)
{
	if(NULL == pEncHandle)
		return VO_ERR_INVALID_ARG;
	pEncHandle->Init = voG722_Init;
	pEncHandle->SetInputData = voG722_SetInputData;
	pEncHandle->GetOutputData = voG722_GetOutputData;
	pEncHandle->SetParam = voG722_SetParam;
	pEncHandle->GetParam = voG722_GetParam;
	pEncHandle->Uninit = voG722_Uninit;

	return VO_ERR_NONE;
}







