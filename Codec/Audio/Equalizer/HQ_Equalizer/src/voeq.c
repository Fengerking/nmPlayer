/*
*  Copyright 2003 - 2010 by Visualon software Incorporated.
*  All rights reserved. Property of Visualon software Incorporated.
*  Restricted rights to use, duplicate or disclose this code are
*  granted through contract.
*/

/***************************** Change History**************************
;* 
;*    DD/MMM/YYYY     Code Ver         Description             Author
;*    -----------     --------     ------------------        -----------
;*    06-21-2010        1.0        File imported from        Huaping Liu
;*                                             
;**********************************************************************/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "voHdEq.h"
#include "cmnMemory.h"
#include "voeq_tab.h"
#include "coefs.h"


//#define DUMP
//#define UPDATE

#ifdef DUMP
FILE *dump = NULL;
#endif

static void Cal_Shelving_Filter(voEq_State *st, int index)
{
	int    fs, fc, G;                           
	float  Q = 2.0;                    /* Q = (2^(1/2))/2 */
	float  K, V0, root2;

	fs = st->sample_rate;                /* bit-stream sample rate */
	G  = Eq_Type[st->Eq_mode][index];    /* Boost or cut gain, form the const table */
	fc = EQ_Band[index];                 /* Cut off freqeuncy */

	K  = tan((pi * fc) / fs);
	V0 = pow(10.0, (G/20.0));
	root2 = 1/Q;             

	/* Invert gain if a cut */
	if(V0 < 1)
		V0 = 1/V0;

	/********* Cal Shelving filter coefficients ********/
	st->a[0][0] = 1;

	/* a[0][], b[0][] Base Shelving filter */
	if(G > 0)
	{
		st->b[index][0] = (1 + sqrt(V0)*root2*K + V0*K*K) / (1 + root2*K + K*K);
		st->b[index][1] = (2 * (V0*K*K - 1)) / (1 + root2*K + K*K);
		st->b[index][2] = (1 - sqrt(V0)*root2*K + V0*K*K) / (1 + root2*K + K*K);
		st->a[index][1] = (2 * (K*K - 1)) / (1 + root2*K + K*K);
		st->a[index][2] = (1 - root2*K + K*K) / (1 + root2*K + K*K);
	}
	else
	{
		st->b[index][0] = (1 + root2*K + K*K) / (1 + root2*sqrt(V0)*K + V0*K*K);
		st->b[index][1] = (2 * (K*K - 1)) / (1 + root2*sqrt(V0)*K + V0*K*K);
		st->b[index][2] = (1 - root2*K + K*K) / (1 + root2*sqrt(V0)*K + V0*K*K);
		st->a[index][1] = (2 * (V0 * K*K -1)) / (1 + root2*sqrt(V0)*K + V0*K*K);
		st->a[index][2] = (1 - root2*sqrt(V0)*K + V0*K*K) / (1 + root2*sqrt(V0)*K + V0*K*K);
	}

	index += 9;

	G  = Eq_Type[st->Eq_mode][index];    /* Boost or cut gain, form the const table */
	fc = EQ_Band[index];                 /* Cut off freqeuncy */

	K  = tan((pi * fc) / fs);
	V0 = pow(10.0, (G/20.0));           

	/* Invert gain if a cut */
	if(V0 < 1)
		V0 = 1/V0;

	/* a[9][], b[9][] Treble shelving filter */
	st->a[9][0] = 1;

	if(G > 0)
	{
		st->b[index][0] = (V0 + root2*sqrt(V0)*K + K*K) / (1 + root2*K + K*K);
		st->b[index][1] = (2 * (K*K - V0)) / (1 + root2*K + K*K);
		st->b[index][2] = (V0 - root2*sqrt(V0)*K + K*K) / (1 + root2*K + K*K);
		st->a[index][1] = (2 * (K*K - 1)) / (1 + root2*K + K*K);
		st->a[index][2] = (1 - root2*K + K*K) / (1 + root2*K + K*K);	
	}
	else
	{
		st->b[index][0] = (1 + root2*K + K*K) / (V0 + root2*sqrt(V0)*K + K*K);
		st->b[index][1] = (2 * (K*K - 1)) / (V0 + root2*sqrt(V0)*K + K*K);
		st->b[index][2] = (1 - root2*K + K*K) / (V0 + root2*sqrt(V0)*K + K*K);
		st->a[index][1] = (2 * ((K*K)/V0) - 1) / (1 + root2/sqrt(V0)*K + (K*K)/V0);
		st->a[index][2] = (1- root2/sqrt(V0)*K + (K*K)/V0) / (1 + root2/sqrt(V0)*K + (K*K)/V0);
	}

}

static void Cal_Peak_Filters(voEq_State *st, int index)
{
	int    fs, fc, G;                         
	float  K, V0, root2;                       
	float  Q = 1.25 * (10 - index)/4;                     /* Octave */

	fs = st->sample_rate;               /* bit-stream sample rate */
	fc = EQ_Band[index];                /* filter cut off frequency */
	G  = Eq_Type[st->Eq_mode][index];   /* boost or cut gain */

	K  = tan((pi * fc)/fs);
	V0 = pow(10.0, (G/20.0));
	root2 = 1/Q;         

	/* Invert gain if a cut */
	if(V0 < 1)
		V0 = 1/V0;

	st->a[index][0] = 1;
	/* Peak Boost */

	if(G > 0)
	{
		st->b[index][0] = (1 + V0*root2*K + K*K) / (1 + root2*K + K*K);
		st->b[index][1] = (2 * (K*K - 1)) / (1 + root2*K + K*K);
		st->b[index][2] = (1 - V0*root2*K + K*K) /  (1 + root2*K + K*K);
		st->a[index][1] = (2 * (K*K - 1)) /  (1 + root2*K + K*K);
		st->a[index][2] = (1 - root2*K + K*K) /  (1 + root2*K + K*K);
	}
	else
	{
		st->b[index][0] = (1 + root2*K + K*K) / (1 + V0*root2*K + K*K);
		st->b[index][1] = (2 * (K*K -1)) / (1 + V0*root2*K + K*K);
		st->b[index][2] = (1 - root2*K + K*K) / (1 + V0*root2*K + K*K);
		st->a[index][1] = (2 * (K*K - 1)) /  (1 + V0*root2*K + K*K);
		st->a[index][2] = (1 - root2*K*V0 + K*K) /  (1 + V0*root2*K + K*K);
	}
}

static void Cal_Filter_Coefs(voEq_State *st)
{
	int i;

	/* calculate filters coefficients */
	Cal_Shelving_Filter(st, 0);

	for(i = 1; i < 9; i++)
	{
		Cal_Peak_Filters(st, i);
	}
}

VO_U32 VO_API voEQInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA *pUserData )
{
	int i, j, k;
	voEq_State *st;
	int interMem = 0;
	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	

	if(pUserData == NULL || (pUserData->memflag & 0x0F) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
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

	/* Memory allocation for coder state */

	st = (voEq_State *)mem_malloc(pMemOP, sizeof(voEq_State), 32);
	if(st == NULL)
		return VO_ERR_OUTOF_MEMORY;

	st->sample_rate = 44100;
	st->channel = 1;
	st->Eq_mode = 1;
	st->Cal_flag = 0;

	for(i = 0; i < 10; i++)
	{
		for(j = 0; j < 3; j++)
		{
			st->a[i][j] = 0.0;
			st->b[i][j] = 0.0;
		}

#ifdef FIX_CODE
		for(k = 0; k < 2; k++)
		{
			st->xl_mem[i][k] = 0;
			st->xr_mem[i][k] = 0;
			st->ylh_mem[i][k] = 0;
			st->ylo_mem[i][k] = 0;
			st->yrh_mem[i][k] = 0;
			st->yro_mem[i][k] = 0;
		}

#else
		for(k = 0; k < 2; k++)
		{
			st->xl_mem[i][k] = 0.0;
			st->xr_mem[i][k] = 0.0;
			st->yl_mem[i][k] = 0.0;
			st->yr_mem[i][k] = 0.0;
		}
#endif
	}

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

#ifdef DUMP
	dump = fopen("coef.txt", "a+");
#endif

	return VO_ERR_NONE;

}

VO_U32 VO_API voEQSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	voEq_State *st;
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	st = (voEq_State *)hCodec;
	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	st->inBuf = (short *)pInput->Buffer;
	st->nLens = (int)pInput->Length;

	return VO_ERR_NONE;

}

#ifdef FIX_CODE

#define MAX_32 0x7fffffffL
#define MIN_32 0x80000000L
#define extract_h(x)     (((x))>>16)

static __inline int L_add(int L_var1, int L_var2)
{
	int L_var_out = L_var1 + L_var2;
	if (((L_var1 ^ L_var2) & MIN_32) == 0)
	{
		if ((L_var_out ^ L_var1) & MIN_32)
		{
			L_var_out = (L_var1 < 0) ? MIN_32 : MAX_32;
		}
	}
	return (L_var_out);
}

static __inline short vo_round (int L_var1)
{
	int L_temp;
	L_temp = L_add (L_var1, (int)0x00008000L);
	return (short)(L_temp >> 16);
}

static __inline int L_shl2 (int L_var1, short var2)
{
	int L_var_out =L_var1;
	for (; var2 > 0; var2--)
	{
		if (L_var1 > (int) 0x3fffffffL)
		{
			L_var_out = MAX_32;
			break;
		}
		else
		{
			if (L_var1 < (int) 0xc0000000L)
			{
				L_var_out = MIN_32;
				break;
			}
		}
		L_var1 *= 2;
		L_var_out = L_var1;
	}
	return (L_var_out);
}

#endif

#ifdef FIX_CODE
VO_U32 VO_API voEQGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	voEq_State *st;
	int    i, j;
	short  x2, y2;
	short  a1, a2, b0, b1, b2;
	int    suml, sumr;
	int    templ, tempr;
	int    FrameLen, chn, mode;
	short  *signal, *signal2; 
	short  *output;

	st = (voEq_State *)hCodec;

	signal = (short *)st->inBuf;
	signal2 = signal+1; 
	output = (short *)pOutput->Buffer;

	FrameLen = (int)(st->nLens >> 1);
	chn = st->channel;
	mode = st->Eq_mode;

	for(i = 0; i < 10; i++)
	{
		if(Eq_Type[st->Eq_mode][i] == 0)
			continue;

		b0 = EQ_Coefs[mode][i][0];
		b1 = EQ_Coefs[mode][i][1];
		b2 = EQ_Coefs[mode][i][2];

		a1 = EQ_Coefs[mode][i][3];
		a2 = EQ_Coefs[mode][i][4];

		for(j = 0; j < FrameLen; j += chn)
		{
			x2 = st->xl_mem[i][1];
			st->xl_mem[i][1] = st->xl_mem[i][0];
			st->xl_mem[i][0] = signal[j];

			/* y(n) = b[0] * x[n] + b[1] * x[n-1] + b[2] * x[n-2]
			*        - a[1] * y[n-1] - a[2] * y[n-2]         */

			suml  = (st->xl_mem[i][0] * b0);
			suml += (st->xl_mem[i][1] * b1);
			suml += (x2 * b2);

			suml -= (st->ylh_mem[i][0] * a1);
			suml -= ((st->ylo_mem[i][0] * a1) >> 15);
			suml -= (st->ylh_mem[i][1] * a2);
			suml -= ((st->ylo_mem[i][1] * a2) >> 15);

			suml = L_shl2(suml, 3);
			templ = vo_round(L_shl2(suml, 1));
			st->ylh_mem[i][1] = st->ylh_mem[i][0];
			st->ylo_mem[i][1] = st->ylo_mem[i][0];

			st->ylh_mem[i][0] = suml >> 16;
			st->ylo_mem[i][0] = (suml - (st->ylh_mem[i][0]<< 16)) >> 1;

			signal[j] = templ;

			if(chn == 2)
			{
				y2 = st->xr_mem[i][1];
				st->xr_mem[i][1] = st->xr_mem[i][0];
				st->xr_mem[i][0] = signal2[j];

				/* y(n) = b[0] * x[n] + b[1] * x[n-1] + b[2] * x[n-2]
				*        - a[1] * y[n-1] - a[2] * y[n-2]         */
				sumr  = (st->xr_mem[i][0] * b0);
				sumr += (st->xr_mem[i][1] * b1);
				sumr += (y2 * b2);

				sumr -= (st->yrh_mem[i][0] * a1);
				sumr -= ((st->yro_mem[i][0] * a1) >> 15);
				sumr -= (st->yrh_mem[i][1] * a2);
				sumr -= ((st->yro_mem[i][1] * a2) >> 15);

				sumr = L_shl2(sumr, 3);
				tempr = vo_round(L_shl2(sumr, 1));

				st->yrh_mem[i][1] = st->yrh_mem[i][0];
				st->yro_mem[i][1] = st->yro_mem[i][0];

				st->yrh_mem[i][0] = sumr >> 16;
				st->yro_mem[i][0] = (sumr - (st->yrh_mem[i][0] << 16)) >> 1;
				signal2[j] = tempr;

			}

			output[j] = signal[j];

			if(st->channel == 2)
			{
				output[j+1] = signal2[j];
			}
		}
		j = 0;
	}

	pOutput->Length = st->nLens;

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = st->channel;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->Format.SampleRate = st->sample_rate;
		pAudioFormat->InputUsed = st->nLens;
	}

	return VO_ERR_NONE;
}

#else

VO_U32 VO_API voEQGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	voEq_State *st;
	int    i, j;
	float  x2, y2;
	double suml, sumr;
	short  *signal, *signal2; 
	short  *output;
	short  b0, b1, b2, a1, a2;

	st = (voEq_State *)hCodec;

	/* Calculate these filters coefficient */
	if(!st->Cal_flag)
	{
		Cal_Filter_Coefs(st);    
		st->Cal_flag = 1;

#ifdef  UPDATE /* update fixed EQ coefs */
		for(i = 0; i < 10; i++)
		{
			b0 = (short)(st->b[i][0] * 4096);
			b1 = (short)(st->b[i][1] * 4096);
			b2 = (short)(st->b[i][2] * 4096);

			a1 = (short)(st->a[i][1] * 8192);
			a2 = (short)(st->a[i][2] * 8192);

#ifdef DUMP
			fprintf(dump, "Type = %d \n", st->Eq_mode);
			fprintf(dump, "i = %d %8d,\t%8d,\t%8d,\t%8d,\t%8d \n ", i, b0, b1, b2, a1, a2);
			fflush(dump);
#endif

		}
#endif
	}                            

	signal = (short *)st->inBuf;
	signal2 = signal+1; 
	output = (short *)pOutput->Buffer;

	for(i = 0; i < nBand; i++)
	{
		if(Eq_Type[st->Eq_mode][i] == 0)
			continue;

		for(j = 0; j < (st->nLens/2); j += st->channel)
		{
			x2 = st->xl_mem[i][1];
			st->xl_mem[i][1] = st->xl_mem[i][0];
			st->xl_mem[i][0] = signal[j];

			/* y(n) = b[0] * x[n] + b[1] * x[n-1] + b[2] * x[n-2]
			*        - a[1] * y[n-1] - a[2] * y[n-2]         */

			suml = st->b[i][0] * st->xl_mem[i][0] + st->b[i][1] * st->xl_mem[i][1] + st->b[i][2] * x2
				- st->a[i][1] * st->yl_mem[i][0] - st->a[i][2] * st->yl_mem[i][1];



			st->yl_mem[i][1] = st->yl_mem[i][0];
			st->yl_mem[i][0] = suml;

			if(suml >= 32768.0)
				suml = 32767.0;
			else if(suml <= -32768.0)
				suml = -32768.0;
			signal[j] = suml;

			if(st->channel == 2)
			{
				y2 = st->xr_mem[i][1];
				st->xr_mem[i][1] = st->xr_mem[i][0];
				st->xr_mem[i][0] = signal2[j];

				/* y(n) = b[0] * x[n] + b[1] * x[n-1] + b[2] * x[n-2]
				*        - a[1] * y[n-1] - a[2] * y[n-2]         */

				sumr = st->b[i][0] * st->xr_mem[i][0] + st->b[i][1] * st->xr_mem[i][1] + st->b[i][2] * y2
					- st->a[i][1] * st->yr_mem[i][0] - st->a[i][2] * st->yr_mem[i][1];

				st->yr_mem[i][1] = st->yr_mem[i][0];
				st->yr_mem[i][0] = sumr;

				if(sumr >= 32768.0)
					sumr = 32767.0;
				else if(sumr <= -32768.0)
					sumr = -32768.0;

				signal2[j] = sumr;
			}

			output[j] = suml;

			if(st->channel == 2)
			{
				output[j+1] = sumr;
			}
		}
	}

	pOutput->Length = st->nLens;

	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels = st->channel;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->Format.SampleRate = st->sample_rate;
		pAudioFormat->InputUsed = st->nLens;
	}

	return VO_ERR_NONE;
}
#endif

int voEQSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	voEq_State* st;
	if(NULL == hCodec)
	{
		return 1;
	}

	st = (voEq_State *)hCodec;

	switch(uParamID)
	{
	case VO_PID_HDEQ_SAMPLERATE:
		st->sample_rate = sr[*(int *)pData];
		break;
	case VO_PID_HDEQ_CHANNELNUM:
		st->channel = *(int *)pData;
		break;
	case VO_PID_HDEQ_MODE:
		st->Eq_mode = *(int *)pData;
		break;
	default:
		return 1;
	}

	return VO_ERR_NONE;
}

int voEQGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{

	return VO_ERR_NONE;
}

int voEQUnInit(VO_HANDLE hCodec)
{
    voEq_State* st;
	VO_MEM_OPERATOR *pMemOP;
	if(hCodec){
		st = (voEq_State *)hCodec;
		pMemOP = st->pvoMemop;

		mem_free(pMemOP, hCodec);
		hCodec = NULL;
	}

#ifdef DUMP
	fclose(dump);
#endif

	return VO_ERR_NONE;
}

VO_S32 VO_API voGetHDEQAPI (VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return VO_ERR_INVALID_ARG;

	pDecHandle->Init = voEQInit;
	pDecHandle->SetInputData = voEQSetInputData;
	pDecHandle->GetOutputData = voEQGetOutputData;
	pDecHandle->SetParam = voEQSetParam;
	pDecHandle->GetParam = voEQGetParam;
	pDecHandle->Uninit = voEQUnInit;

	return VO_ERR_NONE;
}

