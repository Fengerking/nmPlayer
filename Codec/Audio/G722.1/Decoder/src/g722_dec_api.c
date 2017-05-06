/*
ITU-T G.722 Speech Coder   ANSI-C Source Code     Version 5.00
copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
Universite de Sherbrooke.  All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "basop32.h"
#include "voAudio.h"
#include "mem_align.h"
#include "cmnMemory.h"
#include "voG722.h"
#include "voIndex.h"
#include "voCheck.h"

#define MAX_SAMPLE_RATE 32000
#define MAX_FRAMESIZE   (MAX_SAMPLE_RATE/50)

Word16 read_ITU_format(G722DecState *st, Word16 number_of_16bit_words_per_frame);
void *g_hG722DecInst = NULL;

VO_U32 VO_API voG722_Init(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	Word16 i;
	G722DecState *st;
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
	if ((st = (G722DecState *)mem_malloc(pMemOP, sizeof(G722DecState), 32)) == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}  

	if((st->randobj = (Rand_Obj *)mem_malloc(pMemOP, sizeof(Rand_Obj), 32)) == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}

	if((st->bitobj = (Bit_Obj *)mem_malloc(pMemOP, sizeof(Bit_Obj), 32)) == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}

	/* initialize the random number generator */
	st->randobj->seed0 = 1;
	st->randobj->seed1 = 1;
	st->randobj->seed2 = 1;
	st->randobj->seed3 = 1;

	st->bitobj->code_bit_count = 0;
	st->bitobj->code_word_ptr = NULL;
	st->bitobj->current_word = 0;
	st->bitobj->next_bit = 0;
	st->bitobj->number_of_bits_left = 0;

	st->input      = NULL;
	st->output     = NULL;
	st->bit_rate   = 0;				//default bit_rate: 24kbit/s
	st->syntax     = 0;				//default pack bit_stream
	st->bandwidth  = 0;				//default bandwidth is 7KHz
	st->frame_size = 0;
	st->number_of_bits_per_frame  = 0;
	st->number_of_regions = 0;
	st->frame_error_flag = 0;
	st->old_mag_shift = 0;

	for (i=0; i<MAX_DCT_LENGTH; i++)
		st->old_decoder_mlt_coefs[i] = 0;    

	for (i = 0; i < (MAX_DCT_LENGTH >> 1); i++)
		st->old_samples[i] = 0;

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
		voCheckLibInit(&(st->hCheck), VO_INDEX_DEC_G722, 1, g_hG722DecInst, pUserData->libOperator);
	else
		voCheckLibInit(&(st->hCheck), VO_INDEX_DEC_G722, 1, g_hG722DecInst, NULL);
#endif

	return VO_ERR_NONE;

}

VO_U32 VO_API voG722_SetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	G722DecState *gData;
	if (NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	gData = (G722DecState *)hCodec;

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	gData->input  = (Word16 *)pInput->Buffer;
	gData->InLens = pInput->Length;

	return VO_ERR_NONE;

}

VO_U32 VO_API voG722_GetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	Word16  i;
	Word16  number_of_16bit_words_per_frame;
	Word16  words;
	Word16  mag_shift;
	G722DecState* st = (G722DecState*)hCodec;
	Word16 decoder_mlt_coefs[MAX_DCT_LENGTH];


	if(st == NULL)
		return VO_ERR_WRONG_PARAM_ID;

	st->number_of_bits_per_frame = (Word16 )((st->bit_rate)/50);
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

	number_of_16bit_words_per_frame = (Word16)(st->number_of_bits_per_frame/16);

	if(st->syntax == 0)
	{
		words = number_of_16bit_words_per_frame;
	}
	else
	{
		words = read_ITU_format(st,number_of_16bit_words_per_frame);
	}

	if (words != (st->InLens >> 1))
	{
		return VO_ERR_INVALID_ARG;
	}

	st->output = (Word16*)pOutput->Buffer;

	/* reinit the current word to point to the start of the buffer */
	st->bitobj->code_word_ptr = st->input;
	st->bitobj->current_word = *(st->input);
	st->bitobj->code_bit_count = 0;
	st->bitobj->number_of_bits_left = st->number_of_bits_per_frame;

	//decoder(&bitobj, &randobj, control.number_of_regions, decoder_mlt_coefs, &mag_shift,
	//	&old_mag_shift,old_decoder_mlt_coefs, frame_error_flag);
	decoder(st, decoder_mlt_coefs, &mag_shift);

	/* convert the decoder_mlt_coefs to samples */
	rmlt_coefs_to_samples(decoder_mlt_coefs, st, mag_shift);

	/* For ITU testing, off the 2 lsbs. */
	for (i=0; i< st->frame_size; i++)
		st->output[i]  &= 0xfffc;


	if(st->bandwidth == 7000)
	{
		pOutput->Length = st->frame_size << 1;   //320 samples
	}
	else
	{
		pOutput->Length = st->frame_size << 1;   //320 sampls
	}

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = 1;
		pAudioFormat->Format.SampleRate = st->SampleRate;
		pAudioFormat->Format.SampleBits = 16;	
		pAudioFormat->InputUsed = st->InLens;	
	}
#ifdef LCHECK
	voCheckLibCheckAudio(st->hCheck, pOutput, &(pAudioFormat->Format));
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voG722_SetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	G722DecState *st = (G722DecState *)hCodec;
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
	G722DecState *st = (G722DecState*)hCodec;

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
	G722DecState *st;
	VO_MEM_OPERATOR *pMemOP;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}
	st = (G722DecState *)hCodec;
	pMemOP = st->pvoMemop;

#ifdef LCHECK
	voCheckLibUninit(st->hCheck);
#endif

	if(hCodec)
	{
		mem_free(pMemOP, st->randobj);
		mem_free(pMemOP, st->bitobj);
		mem_free(pMemOP, hCodec);
		hCodec = NULL;
	}
	return VO_ERR_NONE;

}

/***************************************************************************
Procedure/Function:     Read ITU format function 

Syntax:

Description:  reads file input in ITU format

Design Notes:


***************************************************************************/

Word16 read_ITU_format(G722DecState *st, Word16 number_of_16bit_words_per_frame)
{
	Word16 i,j;
	Word16 packed_word;
	Word16 bit_count;
	Word16 bit;
	Word16 in_array[MAX_BITS_PER_FRAME+2] = {0};
	Word16 one = 0x0081;
	Word16 zero = 0x007f;
	Word16 frame_start = 0x6b21;
	//Word16 *in_array = st->input;
	Word16 *out_words = st->input;

	j = 0;
	bit = in_array[j++];
	if (bit != frame_start) 
	{
		st->frame_error_flag = 1;
	}
	else 
	{
		st->frame_error_flag = 0;

		/* increment j to skip over the number of bits in frame */
		j++;

		for (i=0; i<number_of_16bit_words_per_frame; i++) 
		{
			packed_word = 0;
			bit_count = 15;
			while (bit_count >= 0)
			{
				bit = in_array[j++];
				if (bit == zero) 
					bit = 0;
				else if (bit == one) 
					bit = 1;
				else 
				{
					st->frame_error_flag = 1;
					/*	  printf("read_ITU_format: bit not zero or one: %4x\n",bit); */
				}
				packed_word <<= 1;
				packed_word = (Word16 )(packed_word + bit);
				bit_count--;
			}
			out_words[i] = packed_word;
		}
	}
	return((Word16)((1 + 16*number_of_16bit_words_per_frame)/16));
}

VO_S32 VO_API voGetG722DecAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(NULL == pDecHandle)
		return VO_ERR_INVALID_ARG;
	pDecHandle->Init = voG722_Init;
	pDecHandle->SetInputData = voG722_SetInputData;
	pDecHandle->GetOutputData = voG722_GetOutputData;
	pDecHandle->SetParam = voG722_SetParam;
	pDecHandle->GetParam = voG722_GetParam;
	pDecHandle->Uninit = voG722_Uninit;

	return VO_ERR_NONE;
}











