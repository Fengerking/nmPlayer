/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/************************************************************************
* @file voxPlayerStrech.c
*
* xPlayer trans functions file
*
* @author  Huaping Liu
* @date    2013-04-26
************************************************************************/

#include "voAudioSpeedghdr.h"
#include <math.h>
#include <limits.h>
#include <float.h>
#include "cmnMemory.h"
#include "mem_align.h"

static VO_S32 GetLog2Value(double value)
{
	return (VO_S32)(log(value) / log(2.0) + 0.5);
}

//overlap
static void tsmOverLap(voxPlayerStrech *pTsm,
							      VO_S16 *pOutput, 
								  const VO_S16 *pInput, 
								  VO_S32  ovlPos)
{
	VO_S32  i;
	VO_S16  nTemp;
	VO_S32  nIdx;          //Interlace LR
	VO_S32  nOverLens = pTsm->overlapLength;
    VO_S16 *pTemp = pTsm->pMidBuffer;

	if (pTsm->nChs == 2) 
	{
		const VO_S16 *pIn = pInput + 2 * ovlPos;

		for (i = 0; i < nOverLens; i++)
		{
			nTemp = (VO_S16)(nOverLens - i);
			nIdx = 2*i;
			pOutput[nIdx] = (VO_S16)((pIn[nIdx] * i + pTemp[nIdx] * nTemp) / nOverLens);
			pOutput[nIdx + 1] = (VO_S16)((pIn[nIdx + 1] * i + pTemp[nIdx + 1] * nTemp) / nOverLens);
		}
	} else {
		const VO_S16 *pIn = pInput + ovlPos;
		for (i = 0; i < nOverLens; i++)
		{
			nTemp = (VO_S16)(nOverLens - i);
			pOutput[i] = (VO_S16)((pIn[i] * i + pTemp[i] * nTemp) / nOverLens);
		}
	}
}

void tsmCalcParameters(voxPlayerStrech *pTsm)
{
	VO_S32 nSampleRate;
	VO_S32 nSeqMs;
	VO_S32 nSeekWinMs;
	VO_S32 nOverlapMs;
	double   seq, seek;

	nSampleRate = pTsm->sampleRate;
	nSeqMs = pTsm->sequenceMs;
	nSeekWinMs = pTsm->seekWindowMs;
	nOverlapMs = pTsm->overlapMs;

	if (pTsm->bAutoSeqFlag)
	{
		seq = C_SEQ + K_SEQ * pTsm->tempo;
		seq = voClip(seq, SEQ_MAX, SEQ_MIN);
		pTsm->sequenceMs = (VO_S32)(seq + 0.5);
	}

	if (pTsm->bAutoSeekFlag)
	{
		seek = C_SEEK + K_SEEK * pTsm->tempo;
		seek = voClip(seek, SEEK_MAX, SEEK_MIN);
		pTsm->seekWindowMs = (VO_S32)(seek + 0.5);
	}

	// Update seek window lengths
	pTsm->nSeekWinLens = (pTsm->sampleRate * pTsm->sequenceMs) / 1000;
	if (pTsm->nSeekWinLens < 2 * pTsm->overlapLength) 
	{
		pTsm->nSeekWinLens = 2 * pTsm->overlapLength;
	}

	pTsm->seekLength = (pTsm->sampleRate * pTsm->seekWindowMs) / 1000;
}

static void tsmPreCalcRefStereo(voxPlayerStrech *pTsm)
{
	VO_S32     i, cnt2;
	VO_S32    temp, temp2;
	VO_S32    nOverlapLens;

	nOverlapLens = pTsm->overlapLength;
	for (i=0 ; i < nOverlapLens ;i++) 
	{
		temp = i * (nOverlapLens - i);
		cnt2 = i * 2;
		temp2 = (pTsm->pMidBuffer[cnt2] * temp) / pTsm->slopingDivider;
		pTsm->pRefMidBuffer[cnt2] = (VO_S16)(temp2);
		temp2 = (pTsm->pMidBuffer[cnt2 + 1] * temp) / pTsm->slopingDivider;
		pTsm->pRefMidBuffer[cnt2 + 1] = (VO_S16)(temp2);
	}
}

static void tsmPreCalcRefMono(voxPlayerStrech *pTsm)
{
	VO_S32  i, nOverlapLens;
	VO_S32  temp, temp2;

	nOverlapLens = pTsm->overlapLength;
	for (i=0 ; i < nOverlapLens;i ++) 
	{
		temp = i * (nOverlapLens - i);
		temp2 = (pTsm->pMidBuffer[i] * temp) / pTsm->slopingDivider;
		pTsm->pRefMidBuffer[i] = (VO_S16)temp2;
	}
}

static void tsmUpdateOverLens(voxPlayerStrech *pTsm, VO_S32 newOverLens)
{
	VO_S32 prevOvl;
    VO_MEM_OPERATOR      *pvoMemop;
	prevOvl = pTsm->overlapLength;
	pTsm->overlapLength = newOverLens;

	pvoMemop = pTsm->pvoMemop;
	if (newOverLens > prevOvl)
	{
		if (pTsm->pMidBuffer)
		{
			as_mem_free(pvoMemop, pTsm->pMidBuffer);
		}
		if (pTsm->pRefMidBufTemp)
		{
			as_mem_free(pvoMemop, pTsm->pRefMidBufTemp);
		}

        pTsm->pMidBuffer =(VO_S16 *)as_mem_malloc(pvoMemop, newOverLens * 2 * sizeof(VO_S16), 32);
		cmnMemSet(VO_AUDIO_AudioSpeed, pTsm->pMidBuffer, 0, 2 * sizeof(VO_S16) * newOverLens);
		pTsm->pRefMidBufTemp = (VO_S16 *)as_mem_malloc(pvoMemop, 2 * newOverLens * sizeof(VO_S16) + 16 / sizeof(VO_S16), 32);
		pTsm->pRefMidBuffer = (VO_S16 *)((((VO_U32)pTsm->pRefMidBufTemp) + 15) & (VO_U32)-16);
	}
}

static void tsmCalcOverLens(voxPlayerStrech *pTsm)
{
	VO_S32 newOvl;
    VO_S32 nOverlapMs;

    nOverlapMs = pTsm->overlapMs;

	pTsm->overlapDividerBits = GetLog2Value((pTsm->sampleRate * nOverlapMs) / 1000.0) - 1;
	if (pTsm->overlapDividerBits > 9) 
		pTsm->overlapDividerBits = 9;
	if (pTsm->overlapDividerBits < 3) 
		pTsm->overlapDividerBits = 3;

	newOvl = (VO_S32)pow(2.0, (VO_S32)pTsm->overlapDividerBits + 1);    // +1 => account for -1 above

	tsmUpdateOverLens(pTsm, newOvl);

	pTsm->slopingDivider = (newOvl * newOvl - 1) / 3;
}

//setTempo
void tsmSetTempo(voxPlayerStrech *pTsm, float newTempo)
{
	VO_S32 intskip;
	VO_S32  nOverLens = pTsm->overlapLength;

	pTsm->tempo = 1.0f + 0.01f * newTempo;



	tsmCalcParameters(pTsm);

	pTsm->nominalSkip = pTsm->tempo * (pTsm->nSeekWinLens - nOverLens);
	intskip = (VO_S32)(pTsm->nominalSkip + 0.5f);
	pTsm->sampleReq = max(intskip + nOverLens, pTsm->nSeekWinLens) + pTsm->seekLength;
}


void tsmSetParameters(voxPlayerStrech *pTsm,
								   VO_S32  nSR, 
					               VO_S32  nSeqms, 
					               VO_S32  nSeekWinMs, 
					               VO_S32   aOverlapMS)
{
	if (nSR > 0)   
		pTsm->sampleRate = nSR;
	if (aOverlapMS > 0)    
		pTsm->overlapMs = aOverlapMS;

	if (nSeqms > 0)
	{
		pTsm->sequenceMs = nSeqms;
		pTsm->bAutoSeqFlag = VO_FALSE;
	} 
	else if (nSeqms == 0)
	{
		pTsm->bAutoSeqFlag = VO_TRUE;
	}

	if (nSeekWinMs > 0) 
	{
		pTsm->seekWindowMs = nSeekWinMs;
		pTsm->bAutoSeekFlag = VO_FALSE;
	} 
	else if (nSeekWinMs == 0) 
	{
		pTsm->bAutoSeekFlag = VO_TRUE;
	}

	tsmCalcOverLens(pTsm);
  //can not do it here, otherwise, 
  //tempo will be modified when setting channel and sample rate 
  //tsmSetTempo(pTsm, pTsm->tempo);
  
}

/* Calculate Cross Corr Stereo or Mono */
long TsmCalcCCMono(voxPlayerStrech *pTsm,
				                const VO_S16 *pSrc1, 
								const VO_S16 *pSrc2)
{
	VO_S32 i;
	VO_S32 nOverlapLens;

	long corr = 0, norm = 0;
    nOverlapLens = pTsm->overlapLength;

	for (i = 1; i < nOverlapLens; i ++) 
	{
		corr += (pSrc1[i] * pSrc2[i]) >> pTsm->overlapDividerBits;
		norm += (pSrc1[i] * pSrc1[i]) >> pTsm->overlapDividerBits;
	}

	// Normalize result 
	if (norm == 0) 
		norm = 1;    // to avoid div by zero
	return (long)((double)corr * SHRT_MAX / sqrt((double)norm));
}

long tsmCalcCCStereo(voxPlayerStrech *pTsm,
					             const VO_S16 *pSrc1,
								 const VO_S16 *pSrc2)
{
	VO_S32 i;
	VO_S32 nOverlapLens;

	long corr = 0, norm = 0;
	nOverlapLens = pTsm->overlapLength;

	for (i = 2; i < 2 * nOverlapLens; i += 2) 
	{
		corr += (pSrc1[i] * pSrc2[i] +
			pSrc1[i + 1] * pSrc2[i + 1]) >> pTsm->overlapDividerBits;
		norm += (pSrc1[i] * pSrc1[i] + pSrc1[i + 1] * pSrc1[i + 1]) >> pTsm->overlapDividerBits;
	}

	if (norm == 0) norm = 1;    // to avoid div by zero
	return (long)((double)corr * SHRT_MAX / sqrt((double)norm));
}

static VO_S32 tsmSeekBOPMono(voxPlayerStrech *pTsm, const VO_S16 *pRefSrc)
{
	VO_S32  nBestIdx = 0;
	double bestCorr = FLT_MIN, corr;
	double tmp;
	VO_S32  nIdx;
	VO_S32  nSeekLens = pTsm->seekLength;
	const VO_S16 *pOffset;

	tsmPreCalcRefMono(pTsm);

	//Get Best Corr position
	for (nIdx = 0; nIdx < nSeekLens; nIdx++) 
	{
		pOffset = pRefSrc + nIdx;
		corr = (double)TsmCalcCCMono(pTsm, pTsm->pRefMidBuffer, pOffset);
		tmp = (double)(2 * nIdx - nSeekLens) / nSeekLens;
		corr = ((corr + 0.1) * (1.0 - 0.25 * tmp * tmp));

		// Checks for the highest correlation value
		if (corr > bestCorr) 
		{
			bestCorr = corr;
			nBestIdx = nIdx;
		}
	}
	return nBestIdx;
}

static VO_S32 tsmSeekBOPMonoFast(voxPlayerStrech *pTsm,  const VO_S16 *pRefSrc) 
{
	int j;
	VO_S16  bestOffs;
	double    bestCorr = FLT_MIN, corr;
	double    tmp;
	VO_S32  nIdx, corrOffset = 0, tempOffset = 0;
	VO_S32  nSeekLens = pTsm->seekLength;

	tsmPreCalcRefMono(pTsm);

	bestOffs = mFastOffset[0][0];

	for (nIdx = 0; nIdx < 4; nIdx++) 
	{
		j = 0;
		while (mFastOffset[nIdx][j]) 
		{
			tempOffset = corrOffset + mFastOffset[nIdx][j];
			if (tempOffset >= pTsm->seekLength) break;

			// Calculates correlation value for the mixing position corresponding to 'tempOffset'
			corr = (double)TsmCalcCCMono(pTsm, pRefSrc + tempOffset, pTsm->pRefMidBuffer);

			 tmp = (double)(2 * tempOffset - nSeekLens) / nSeekLens;
			corr = ((corr + 0.1) * (1.0 - 0.25 * tmp * tmp));

			// Checks for the highest correlation value
			if (corr > bestCorr) 
			{
				bestCorr = corr;
				bestOffs = (VO_S16)tempOffset;
			}
			j ++;
		}
		corrOffset = bestOffs;
	}

	return bestOffs;
}

//seekBestOverlapPositionStereo
static VO_S32 tsmSeekBOPStereo(voxPlayerStrech *pTsm,  const VO_S16 *pRefSrc) 
{
	VO_S16 bestOffs = 0;
	double   bestCorr = FLT_MIN, corr, tmp;
	VO_S32  nIdx;
	VO_S32  nSeekLens = pTsm->seekLength;

	tsmPreCalcRefStereo(pTsm);

	// Scans for the best correlation value by testing each possible position over the permitted range.
	for (nIdx = 0; nIdx < pTsm->seekLength; nIdx ++) 
	{
		corr = (double)tsmCalcCCStereo(pTsm, pRefSrc + 2 * nIdx, pTsm->pRefMidBuffer);

	    tmp = (double)(2 * nIdx - nSeekLens) / (double)nSeekLens;
		corr = ((corr + 0.1) * (1.0 - 0.25 * tmp * tmp));

		if (corr > bestCorr) 
		{
			bestCorr = corr;
			bestOffs = (VO_S16)nIdx;
		}
	}

	return bestOffs;
}

static VO_S32 tsmSeekBOPStereoFast(voxPlayerStrech *pTsm, const VO_S16 *pRefSrc) 
{
	VO_S32  j;
	VO_S16  bestOffs;
	double    bestCorr = FLT_MIN, corr, tmp;
	VO_S32  nIdx, corrOffset = 0, tempOffset = 0;
	VO_S32  nSeekLens = pTsm->seekLength;

	tsmPreCalcRefStereo(pTsm);

	bestOffs = mFastOffset[0][0];

	for (nIdx = 0; nIdx < 4; nIdx ++) 
	{
		j = 0;
		while (mFastOffset[nIdx][j]) 
		{
			tempOffset = corrOffset + mFastOffset[nIdx][j];
			if (tempOffset >= pTsm->seekLength) 
				break;

			corr = (double)tsmCalcCCStereo(pTsm, pRefSrc + 2 * tempOffset, pTsm->pRefMidBuffer);

			tmp = (double)(2 * tempOffset - nSeekLens) / (double)nSeekLens;
			corr = ((corr + 0.1) * (1.0 - 0.25 * tmp * tmp));

			// Checks for the highest correlation value
			if (corr > bestCorr) 
			{
				bestCorr = corr;
				bestOffs = (VO_S16)tempOffset;
			}
			j ++;
		}
		corrOffset = bestOffs;
	}
	return bestOffs;
}

static VO_S32 tsmSeekBOP(voxPlayerStrech *pTsm, const VO_S16 *pRefSrc)
{
	if (pTsm->nChs == 2) 
	{
		// 2 Chs
		if (pTsm->bFastMode) 
		{
			return tsmSeekBOPStereoFast(pTsm, pRefSrc);
		} 
		else 
		{
			return tsmSeekBOPStereo(pTsm, pRefSrc);
		}
	} 
	else 
	{
		// 1 Chs
		if (pTsm->bFastMode) 
		{
			return tsmSeekBOPMonoFast(pTsm, pRefSrc);
		} 
		else 
		{
			return tsmSeekBOPMono(pTsm, pRefSrc);
		}
	}
}

/* Tsm process */
void tsmProcess(voxPlayerStrech *pTsm)
{
	VO_S32  ovlSkip, offset;
	VO_S32  temp;
	VO_S32  nOverLens;
	VO_S16  *pSrc;
    VO_MEM_OPERATOR      *pvoMemop;

	pvoMemop = pTsm->pvoMemop;

	nOverLens = pTsm->overlapLength;

	if (pTsm->pInBuf->nSamplesInBuffer >= pTsm->sampleReq)
	{
		offset = tsmSeekBOP(pTsm, voGetPtrBegin(pTsm->pInBuf));
		tsmOverLap(pTsm, CheckBufferInsert(pTsm->pvoMemop, pTsm->pOutBuf, nOverLens), voGetPtrBegin(pTsm->pInBuf), offset);

		CheckBufferCapc(pvoMemop, pTsm->pOutBuf, pTsm->pOutBuf->nSamplesInBuffer + nOverLens);
		pTsm->pOutBuf->nSamplesInBuffer += nOverLens;

		temp = (pTsm->nSeekWinLens - 2 * nOverLens);

		pSrc = voGetPtrBegin(pTsm->pInBuf) + pTsm->nChs * (offset + nOverLens);
		cmnMemCopy(VO_AUDIO_AudioSpeed, CheckBufferInsert(pTsm->pvoMemop, pTsm->pOutBuf, temp), pSrc, sizeof(VO_S16) * temp * pTsm->nChs);
		pTsm->pOutBuf->nSamplesInBuffer += temp;

		cmnMemCopy(VO_AUDIO_AudioSpeed, pTsm->pMidBuffer, voGetPtrBegin(pTsm->pInBuf) + pTsm->nChs * (offset + temp + nOverLens), 
			          pTsm->nChs * sizeof(VO_S16) * nOverLens);

		pTsm->skipFract += pTsm->nominalSkip;   
		ovlSkip = (int)pTsm->skipFract;   
		pTsm->skipFract -= ovlSkip;      

		/* Remove pTsm input buffer */
		if(ovlSkip >= pTsm->pInBuf->nSamplesInBuffer)
		{
			pTsm->pInBuf->nSamplesInBuffer = 0;
		}
		else
		{
			pTsm->pInBuf->nSamplesInBuffer -= ovlSkip;
			pTsm->pInBuf->nBufferPos += ovlSkip;
		}
	}
}

void tsmClear(voxPlayerStrech *pTsm)
{
	pTsm->pInBuf->nSamplesInBuffer = 0;
	pTsm->pInBuf->nBufferPos = 0;
	pTsm->pOutBuf->nSamplesInBuffer = 0;
	pTsm->pOutBuf->nBufferPos = 0;
}
