/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2010	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		voResample.c

Contains:	resample API function

Written by:	Jacky Lin

Change History (most recent first):
2010-05-18		Jacky Lin		Create file

*******************************************************************************/
#include <string.h>
#include <malloc.h>
#include "voResample.h"
#include "resample.h"
#include "cmnMemory.h"
#include "ReSampleBy2.h"
//#include "mem_align.h"

#ifdef G1_LOG
#include <android/log.h>
#define  LOG_TAG    "RESAMPLE"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif


VO_U32 VO_API voResampleInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VO_U32 hr = VO_ERR_NONE;
	VO_MEM_OPERATOR *pMemOP;
	ResInfo *pRes = NULL;
	FIRFilter *filter = NULL;
	int	*coef = NULL;
	short	*BufRemain = NULL;

	if(pUserData == NULL || (pUserData->memflag & 0x0F) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
		hr = cmnMemFillPointer(VO_INDEX_RESAMPLE);
		pMemOP = &g_memOP;
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	} 

	pRes = (ResInfo *)voauCalloc(pMemOP, sizeof(ResInfo));
	if(NULL == pRes)
		goto INIT_FAIL;

	pRes->pvoMemop = pMemOP;

	filter = (FIRFilter *)voRESCalloc(sizeof(FIRFilter));
	if(NULL == filter)
		goto INIT_FAIL;

	coef = (int *)voRESCalloc(sizeof(int)*MAXFILLEN);
	if(NULL == coef)
		goto INIT_FAIL;

	BufRemain = (short *)voRESCalloc(sizeof(short)*MAXFILLEN*MAX_CHANNEL);
	if(NULL == BufRemain)
		goto INIT_FAIL;

	pRes->nLstate1 = (int *)voRESCalloc(sizeof(int)*8);
    memset(pRes->nLstate1, 0, 8*sizeof(int));

	pRes->nLstate2 = (int *)voRESCalloc(sizeof(int)*8);
	memset(pRes->nLstate2, 0, 8*sizeof(int));

	pRes->nLstate3 = (int *)voRESCalloc(sizeof(int)*8);
	memset(pRes->nLstate3, 0, 8*sizeof(int));

	pRes->nRstate1 = (int *)voRESCalloc(sizeof(int)*8);
	memset(pRes->nRstate1, 0, 8*sizeof(int));

	pRes->nRstate2 = (int *)voRESCalloc(sizeof(int)*8);
	memset(pRes->nRstate2, 0, 8*sizeof(int));

	pRes->nRstate3 = (int *)voRESCalloc(sizeof(int)*8);
	memset(pRes->nRstate3, 0, 8*sizeof(int));

	pRes->pTemp = (short*)malloc(40960*sizeof(short));
	pRes->pTemp1 = (short*)malloc(40960*sizeof(short));
	pRes->pLtemp = (short*)malloc(40960*sizeof(short));
	pRes->pLtemp1 = (short*)malloc(40960*sizeof(short));

	filter->Coef = coef;
	filter->RemainBuf = BufRemain;
	filter->nRemain = 0;	
	pRes->filter = filter;	
	pRes->InRate = 48000;
	pRes->OutRate = 8000;
	pRes->ChanIn = 1;
	pRes->ChanOut = 2;
	pRes->Scale = (pRes->OutRate/100)*(1 << 20)/(pRes->InRate/100);
	pRes->inv_Scale = (pRes->InRate/100)*(1 << 20)/(pRes->OutRate/100);

	pRes->level = 1;
	pRes->ChanSel = 0xFFFF;
	pRes->tmplen = 0;

	*phCodec = (void *)pRes;

	return hr;

INIT_FAIL:
	if(coef)		voRESFree(coef);	
	if(BufRemain)	voRESFree(BufRemain);	
	if(filter)		voRESFree(filter);	
	if(pRes)	voauFree(pMemOP, pRes);
	*phCodec = NULL;

	return VO_ERR_OUTOF_MEMORY;	
}


VO_U32 VO_API voResampleSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	ResInfo *pRes = NULL;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	pRes = (ResInfo *)hCodec;

	pRes->inlen = pInput->Length/sizeof(short);
	pRes->input = (short *)pInput->Buffer;
#ifdef G1_LOG
    LOGI("SetInput Length = %d\n", pRes->inlen);
#endif
	return VO_ERR_NONE;
}

VO_U32 VO_API voResampleGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioFormat)
{
	ResInfo *pRes = NULL;
	short *pTemp;
	short *pTemp1;
	short *pLtemp;
	short *pLtemp1;
	int outlen;
	int nInRate;
	int nTempLen;
	int nTempChs = 0;
	int i;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	pRes = (ResInfo *)hCodec;

	if(pRes->inlen == 0)
		return VO_ERR_INPUT_BUFFER_SMALL;

	//Target and Original have these info, only copy. 
	if ((pRes->ChanIn == pRes->ChanOut) && (pRes->InRate == pRes->OutRate))
	{
		memcpy(pOutput->Buffer, (char*)pRes->input, pRes->inlen*2);
		pOutput->Length = pRes->inlen * 2;

		if(pAudioFormat)
		{
			pAudioFormat->Format.Channels   = pRes->ChanOut;
			pAudioFormat->Format.SampleBits = 16;
			pAudioFormat->Format.SampleRate = pRes->OutRate;
			pAudioFormat->InputUsed         = pRes->inlen*sizeof(short);
		}
		pRes->inlen = 0;
		return VO_ERR_NONE;
	}

	pTemp = pRes->pTemp;
	pTemp1 = pRes->pTemp1;
	pLtemp = pRes->pLtemp;
	pLtemp1 = pRes->pLtemp1;

	nInRate = pRes->InRate;

	if(pRes->ChanIn ==1)
	{
		for (i = 0; i < pRes->inlen; i++)
		{
			pTemp[i] = pRes->input[i];
			pTemp1[i] = pRes->input[i];
		}
        pRes->inlen *= 2;
		nTempChs = 1;
	}else if(pRes->ChanIn == 2)
	{
		for (i = 0; i < pRes->inlen/2; i++)
		{
			pTemp[i] = pRes->input[2*i];          //Left
			pTemp1[i] = pRes->input[2*i + 1];     //Right
		}
	}

	nTempLen = pRes->inlen/2;
#ifdef G1_LOG
    LOGI("Convertion Length = %d\n", pRes->inlen);
	LOGI("BaseInfo InRate = %d, OutRate = %d, InChs = %d, OutChs = %d\n", pRes->InRate, pRes->OutRate,pRes->ChanIn, pRes->ChanOut);
#endif

#ifdef G1_LOG
    LOGI("OutBuffer Lens = %d\n", pOutput->Length);
#endif
	switch (pRes->InRate)
	{
		case 8000:
			//1:2 16K
			voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate1);
			voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate1);
			nTempLen *= 2;

#ifdef G1_LOG
			LOGI("Label1 = %d\n", nTempLen);
#endif
			//1:4 32K
			voUpsampleBy2(pLtemp, nTempLen, pTemp, pRes->nLstate2);
			voUpsampleBy2(pLtemp1,nTempLen, pTemp1, pRes->nRstate2);
			nTempLen *=2;

#ifdef G1_LOG
			LOGI("Label2 = %d\n", nTempLen);
#endif
			//1:8 64K
			voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate3);
			voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate3);
			nTempLen *=2;

#ifdef G1_LOG
			LOGI("Label3 = %d\n", nTempLen);
#endif
			for (i = 0; i < nTempLen; i++)
			{
				pTemp[2 * i] = pLtemp[i];
				pTemp[2 * i + 1] = pLtemp1[i];
			}

			pRes->ChanIn = 2;
			// 64K ---> 44.1K
			pRes->InRate = 64000;
			if(pRes->init == 0)
			{
				InitResample(pRes);
				pRes->init = 1;
			}
			pRes->input = pTemp;
			pRes->inlen = pRes->inlen * 8;
			outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
			pRes->InRate = nInRate;

			break;
		case 12000:
			//1:2 24K
			voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate1);
			voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate1);
			nTempLen *= 2;

			//1:4 48K
			voUpsampleBy2(pLtemp, nTempLen, pTemp, pRes->nLstate2);
			voUpsampleBy2(pLtemp1,nTempLen, pTemp1, pRes->nRstate2);
			nTempLen *= 2;

			for (i = 0; i < nTempLen; i++)
			{
				pLtemp[2 * i] = pTemp[i];
				pLtemp[2 * i + 1] = pTemp1[i];
			}

			pRes->ChanIn = 2;
			// 48K ---> 44.1K
			pRes->InRate = 48000;
			if(pRes->init == 0)
			{
				InitResample(pRes);
				pRes->init = 1;
			}
			pRes->input = pLtemp;
			pRes->inlen = pRes->inlen * 4;
			outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
			pRes->InRate = nInRate;
			break;
		case 16000:
			//1:2 32K
			voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate1);
			voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate1);
			nTempLen *= 2;

			//1:4 64K
			voUpsampleBy2(pLtemp, nTempLen, pTemp, pRes->nLstate2);
			voUpsampleBy2(pLtemp1,nTempLen, pTemp1, pRes->nRstate2);
			nTempLen *= 2;

			for (i = 0; i < nTempLen; i++)
			{
				pLtemp[2 * i] = pTemp[i];
				pLtemp[2 * i + 1] = pTemp1[i];
			}

			pRes->ChanIn = 2;
			// 64K ---> 44.1K
			pRes->InRate = 64000;
			if(pRes->init == 0)
			{
				InitResample(pRes);
				pRes->init = 1;
			}
			pRes->input = pLtemp;
			pRes->inlen = pRes->inlen * 4;
			outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
			pRes->InRate = nInRate;

			break;

		case 24000:
			//1:2 48K
			voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate1);
			voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate1);
			nTempLen *= 2;


			for (i = 0; i < nTempLen; i++)
			{
				pTemp[2 * i] = pLtemp[i];
				pTemp[2 * i + 1] = pLtemp1[i];
			}

			pRes->ChanIn = 2;
			// 48K ---> 44.1K
			pRes->InRate = 48000;
			if(pRes->init == 0)
			{
				InitResample(pRes);
				pRes->init = 1;
			}
			pRes->input = pTemp;
			pRes->inlen = pRes->inlen * 2;
			outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
			pRes->InRate = nInRate;
			break;

		case 32000:
			//1:2 64K
			voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate1);
			voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate1);
			nTempLen *= 2;


			for (i = 0; i < nTempLen; i++)
			{
				pTemp[2 * i] = pLtemp[i];
				pTemp[2 * i + 1] = pLtemp1[i];
			}

			pRes->ChanIn = 2;
			// 64K ---> 44.1K
			pRes->InRate = 64000;
			if(pRes->init == 0)
			{
				InitResample(pRes);
				pRes->init = 1;
			}
			pRes->input = pTemp;
			pRes->inlen = pRes->inlen * 2;
			outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
			pRes->InRate = nInRate;

			break;

		default:
			for (i = 0; i < pRes->inlen/2; i++)
			{
				pLtemp[2*i] = pTemp[i];
				pLtemp[2*i + 1] = pTemp1[i];
			}

			pRes->ChanIn = 2;
			if(pRes->init == 0)
			{
				InitResample(pRes);
				pRes->init = 1;
			}

			if(pRes->inlen == 0)
				return VO_ERR_INPUT_BUFFER_SMALL;

			pRes->input = pLtemp;

			outlen = (int)MUL_20(pRes->inlen/pRes->ChanIn, pRes->Scale);

			if(pOutput->Length < outlen*sizeof(short)*pRes->ChanOut + 8)
			{
				//DEBUG_printf("Out put buufer is too small!");
				return VO_ERR_OUTPUT_BUFFER_SMALL;
			}

			outlen = ResampleCore(pRes, (short *)pOutput->Buffer);

			if(outlen < 0)
				return VO_ERR_OUTOF_MEMORY;
			break;
	}

#if 0

	if (pRes->ChanIn == 1) //Mono
	{
		nInRate = pRes->InRate;
		switch (pRes->InRate)
		{
			case 8000:

				//1:2 16K
				voUpsampleBy2(pRes->input, pRes->inlen, pTemp, pRes->nLstate1);
				outlen = pRes->inlen * 2;

				//1:4 32K
				voUpsampleBy2(pTemp, outlen, pTemp1, pRes->nLstate2);
				outlen = pRes->inlen * 4;

				//1:8 64K
				voUpsampleBy2(pTemp1, outlen, pTemp, pRes->nLstate3);

				// 64K ---> 44.1K
				pRes->InRate = 64000;
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}
				pRes->input = pTemp;
				pRes->inlen = pRes->inlen * 8;
				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
				pRes->InRate = nInRate;
				break;
			case 12000:
				//1:2 24K
				voUpsampleBy2(pRes->input, pRes->inlen, pTemp, pRes->nLstate1);
				outlen = pRes->inlen * 2;

				//1:4 48K
				voUpsampleBy2(pTemp, outlen, pTemp1, pRes->nLstate2);

				// 48K ---> 44.1K
				pRes->InRate = 48000;
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}
				pRes->input = pTemp;
				pRes->inlen = pRes->inlen * 4;
				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
				pRes->InRate = nInRate;

				break;
			case 16000:
				//1:2 32K
				voUpsampleBy2(pRes->input, pRes->inlen, pTemp, pRes->nLstate1);
				outlen = pRes->inlen * 2;

				//1:4 64K
				voUpsampleBy2(pTemp, outlen, pTemp1, pRes->nLstate2);

				// 64K ---> 44.1K
				pRes->InRate = 64000;
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}
				pRes->input = pTemp;
				pRes->inlen = pRes->inlen * 4;
				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
				pRes->InRate = nInRate;

				break;

			case 24000:
				//1:2 48K
				voUpsampleBy2(pRes->input, pRes->inlen, pTemp, pRes->nLstate1);

				// 48K ---> 44.1K
				pRes->InRate = 48000;
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}
				pRes->input = pTemp;
				pRes->inlen = pRes->inlen * 2;
				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
				pRes->InRate = nInRate;

				break;

			case 32000:
				//1:2 64K
				voUpsampleBy2(pRes->input, pRes->inlen, pTemp, pRes->nLstate1);

				// 64K ---> 44.1K
				pRes->InRate = 64000;
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}
				pRes->input = pTemp;
				pRes->inlen = pRes->inlen * 2;
				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
				pRes->InRate = nInRate;
				break;

			default:
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}

				if(pRes->inlen == 0)
					return VO_ERR_INPUT_BUFFER_SMALL;

				outlen = (int)MUL_20(pRes->inlen/pRes->ChanIn, pRes->Scale);

				if(pOutput->Length < outlen*sizeof(short)*pRes->ChanOut + 8)
				{
					DEBUG_printf("Out put buufer is too small!");
					return VO_ERR_OUTPUT_BUFFER_SMALL;
				}

				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);

				if(outlen < 0)
					return VO_ERR_OUTOF_MEMORY;
				break;

		}
	}else if (pRes->ChanIn == 2) //Stereo
	{
		nInRate = pRes->InRate;
		nTempLen = pRes->inlen/2;
		for (i = 0; i < nTempLen; i++)
		{
			pTemp[i] = pRes->input[2*i];          //Left
			pTemp1[i] = pRes->input[2*i + 1];     //Right
		}

		switch (pRes->InRate)
		{
			case 8000:
				//1:2 16K
				voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate1);
				voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate1);
				nTempLen *= 2;

				//1:4 32K
				voUpsampleBy2(pLtemp, nTempLen, pTemp, pRes->nLstate2);
				voUpsampleBy2(pLtemp1,nTempLen, pTemp1, pRes->nRstate2);
				nTempLen *=2;

				//1:8 64K
				voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate3);
				voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate3);
				nTempLen *=2;

				for (i = 0; i < nTempLen; i++)
				{
					pTemp[2 * i] = pLtemp[i];
					pTemp[2 * i + 1] = pLtemp1[i];
				}

				// 64K ---> 44.1K
				pRes->InRate = 64000;
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}
				pRes->input = pTemp;
				pRes->inlen = pRes->inlen * 8;
				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
				pRes->InRate = nInRate;

				break;
			case 12000:
				//1:2 24K
				voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate1);
				voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate1);
				nTempLen *= 2;

				//1:4 48K
				voUpsampleBy2(pLtemp, nTempLen, pTemp, pRes->nLstate2);
				voUpsampleBy2(pLtemp1,nTempLen, pTemp1, pRes->nRstate2);
				nTempLen *= 2;

				for (i = 0; i < nTempLen; i++)
				{
					pLtemp[2 * i] = pTemp[i];
					pLtemp[2 * i + 1] = pTemp1[i];
				}

				// 48K ---> 44.1K
				pRes->InRate = 48000;
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}
				pRes->input = pLtemp;
				pRes->inlen = pRes->inlen * 4;
				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
				pRes->InRate = nInRate;
				break;
			case 16000:
				//1:2 32K
				voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate1);
				voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate1);
				nTempLen *= 2;

				//1:4 64K
				voUpsampleBy2(pLtemp, nTempLen, pTemp, pRes->nLstate2);
				voUpsampleBy2(pLtemp1,nTempLen, pTemp1, pRes->nRstate2);
				nTempLen *= 2;

				for (i = 0; i < nTempLen; i++)
				{
					pLtemp[2 * i] = pTemp[i];
					pLtemp[2 * i + 1] = pTemp1[i];
				}

				// 64K ---> 44.1K
				pRes->InRate = 64000;
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}
				pRes->input = pLtemp;
				pRes->inlen = pRes->inlen * 4;
				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
				pRes->InRate = nInRate;

				break;

			case 24000:
				//1:2 48K
				voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate1);
				voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate1);
				nTempLen *= 2;


				for (i = 0; i < nTempLen; i++)
				{
					pTemp[2 * i] = pLtemp[i];
					pTemp[2 * i + 1] = pLtemp1[i];
				}

				// 48K ---> 44.1K
				pRes->InRate = 48000;
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}
				pRes->input = pTemp;
				pRes->inlen = pRes->inlen * 2;
				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
				pRes->InRate = nInRate;
				break;

			case 32000:
				//1:2 64K
				voUpsampleBy2(pTemp, nTempLen, pLtemp, pRes->nLstate1);
				voUpsampleBy2(pTemp1, nTempLen, pLtemp1, pRes->nRstate1);
				nTempLen *= 2;


				for (i = 0; i < nTempLen; i++)
				{
					pTemp[2 * i] = pLtemp[i];
					pTemp[2 * i + 1] = pLtemp1[i];
				}

				// 64K ---> 44.1K
				pRes->InRate = 64000;
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}
				pRes->input = pTemp;
				pRes->inlen = pRes->inlen * 2;
				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);
				pRes->InRate = nInRate;

				break;

			default:
				if(pRes->init == 0)
				{
					InitResample(pRes);
					pRes->init = 1;
				}

				if(pRes->inlen == 0)
					return VO_ERR_INPUT_BUFFER_SMALL;

				outlen = (int)MUL_20(pRes->inlen/pRes->ChanIn, pRes->Scale);

				if(pOutput->Length < outlen*sizeof(short)*pRes->ChanOut + 8)
				{
					DEBUG_printf("Out put buufer is too small!");
					return VO_ERR_OUTPUT_BUFFER_SMALL;
				}

				outlen = ResampleCore(pRes, (short *)pOutput->Buffer);

				if(outlen < 0)
					return VO_ERR_OUTOF_MEMORY;
				break;
		}
	}
#endif 

	pOutput->Length = outlen*pRes->ChanOut*sizeof(short);

#ifdef G1_LOG
    LOGI("pOutput->Length = %d\n", pOutput->Length);
#endif

	if (nTempChs == 1)
	{
		pRes->ChanIn = 1;
	}
	if(pAudioFormat)
	{
		pAudioFormat->Format.Channels   = pRes->ChanOut;
		pAudioFormat->Format.SampleBits = 16;
		pAudioFormat->Format.SampleRate = pRes->OutRate;
		pAudioFormat->InputUsed         = pRes->inlen*sizeof(short);
	}

	pRes->inlen = 0;

	return VO_ERR_NONE;
}

VO_U32 VO_API voResampleSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	ResInfo *pRes = NULL;
	FIRFilter *filter = NULL;
	int n, k, tmp;
	if(NULL == hCodec)
	{
		return VO_ERR_WRONG_STATUS;
	}

	pRes = (ResInfo *)hCodec;
	filter = pRes->filter;

	if(pData == NULL)
		return VO_ERR_INVALID_ARG;

	switch(uParamID)
	{
	case VO_PID_COMMON_FLUSH:
		if(*((int *)pData))
		{
			//			pRes->pvoMemop->Set(VO_INDEX_RESAMPLE, filter->RemainBuf, 0, filter->nRemain*sizeof(short));
			voRESMemset(filter->RemainBuf, 0, filter->nRemain*sizeof(short));
			filter->nRemain =  filter->length>>1;
			pRes->lastpos = 0;
			pRes->UplastScale = 1024;

			for(n =0; n < MAX_CHANNEL; n++)
			{
				for(k=0; k<3; k++)
					pRes->UplastSimple[n][k] = 0;
			}
		}
		break;

	case VO_PID_RESAMPLE_INRATE:
		tmp = *((int *)pData);
		for(n = 0; n < 12; n++)
		{
			if(tmp == sampRateTab[n])
			{
				pRes->InRate = tmp;
				pRes->init = 0;
				break;
			}
		}
		break;
	case VO_PID_RESAMPLE_OUTRATE:
		tmp = *((int *)pData);
		for(n = 0; n < 12; n++)
		{
			if(tmp == sampRateTab[n])
			{
				pRes->OutRate = tmp;
				pRes->init = 0;
				break;
			}
		}
		break;
	case VO_PID_AUDIO_CHANNELS:
		tmp = *((int *)pData);
		if(tmp > 0 && tmp < MAX_CHANNEL) {
			pRes->ChanIn = pRes->ChanOut = *((int *)pData);
		}
		break;
	case VO_PID_RESAMPLE_LEVEL:
		tmp = *((int *)pData);
		if(tmp >= 0 && tmp < 3)
		{
			pRes->level = *((int *)pData);
			pRes->init = 0;
		}
		break;
	case VO_PID_RESAMPLE_SELECTCHS:
		pRes->ChanSel = *((int *)pData);
		break;
	case VO_PID_RESAMPLE_INCHAN:
		tmp = *((int *)pData);
		if(tmp > 0 && tmp <= MAX_CHANNEL) {
			pRes->ChanIn = tmp;
		}
#ifdef G1_LOG
        LOGI("SetParam1 pRes->ChanIn = %d \n", pRes->ChanIn);
#endif
		break;
	case VO_PID_RESAMPLE_OUTCHAN:
		tmp = *((int *)pData);
		if(tmp > 0 && tmp < MAX_CHANNEL) {
			pRes->ChanOut = tmp;
		}
#ifdef G1_LOG
		LOGI("SetParam1 pRes->ChanOut = %d \n", pRes->ChanOut);
#endif
		break;
		//	case VO_PID_RESAMPLE_CHMAPPING:
		//		pRes->ChanMapSet = 1;
		//		{
		//			int ch;
		//			int *mapping = (int *)pData;
		//
		//			for(ch = 0; ch < pRes->ChanIn; ch++)
		//			{
		//				pRes->ChanMap[ch] = mapping[ch];
		//			}
		//		}
		//		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;

}

VO_U32 VO_API voResampleGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	if(NULL == hCodec)
	{
		return VO_ERR_WRONG_STATUS;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voResampleUninit(VO_HANDLE hCodec)
{
	ResInfo *pRes = NULL;
	FIRFilter *filter = NULL;
	VO_MEM_OPERATOR *pMemOP;
	if(NULL == hCodec)
	{
		return VO_ERR_WRONG_STATUS;
	}

	pRes = (ResInfo *)hCodec;
	filter = pRes->filter;
	pMemOP = pRes->pvoMemop;

	if(pRes->nLstate1)   voRESFree(pRes->nLstate1);
    if(pRes->nLstate2)   voRESFree(pRes->nLstate2);
    if(pRes->nLstate3)   voRESFree(pRes->nLstate3);
    if(pRes->nRstate1)   voRESFree(pRes->nRstate1);
    if(pRes->nRstate2)   voRESFree(pRes->nRstate2);
    if(pRes->nRstate3)   voRESFree(pRes->nRstate3);
	if(pRes->pTemp)      voRESFree(pRes->pTemp);
	if(pRes->pTemp1)     voRESFree(pRes->pTemp1);
	if(pRes->pLtemp)     voRESFree(pRes->pLtemp);
	if(pRes->pLtemp1)    voRESFree(pRes->pLtemp1);
	if(filter->Coef)		voRESFree(filter->Coef);	
	if(filter->RemainBuf)	voRESFree(filter->RemainBuf);	
	if(pRes->tmpbuf)	voRESFree(pRes->tmpbuf);
	if(pRes->tmpbuf1)	voRESFree(pRes->tmpbuf1);
	if(filter)			voRESFree(filter);	
	if(pRes)		voauFree(pMemOP, pRes);

	return VO_ERR_NONE;	
}

VO_S32 VO_API voGetResampleAPI(VO_AUDIO_CODECAPI * pResHandle)
{
	if(NULL == pResHandle)
		return VO_ERR_INVALID_ARG;

	pResHandle->Init = voResampleInit;
	pResHandle->SetInputData = voResampleSetInputData;
	pResHandle->GetOutputData = voResampleGetOutputData;
	pResHandle->SetParam = voResampleSetParam;
	pResHandle->GetParam = voResampleGetParam;
	pResHandle->Uninit = voResampleUninit;

	return VO_ERR_NONE;
}
