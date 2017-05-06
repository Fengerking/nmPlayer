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
* @file voRingBuffer.c
*
* FIFO RingBuffer basic operation files
*
* @author  Huaping Liu
* @date    2013-05-02
************************************************************************/
#include "voRingBuffer.h"
#include "voAudioSpeed.h"
#include "cmnMemory.h"
#include "mem_align.h"

static VO_S32 voGetSampleNum(voRingBuffer *pBuf)
{
	return pBuf->nSizeInBytes / (pBuf->nChannles * sizeof(VO_S16));
}

VO_S16 *voGetPtrBegin(voRingBuffer *pBuf)
{
    return pBuf->pBuf + pBuf->nBufferPos * pBuf->nChannles;
}

static void voRewind(voRingBuffer *pBuf)
{
	if (pBuf->pBuf && pBuf->nBufferPos)
	{
		cmnMemMove( VO_AUDIO_AudioSpeed, (void *)pBuf->pBuf, (void*)voGetPtrBegin(pBuf), sizeof(VO_S16) * pBuf->nChannles * pBuf->nSamplesInBuffer);
		pBuf->nBufferPos = 0;
	}
}

void CheckBufferCapc(VO_MEM_OPERATOR      *pvoMemop, voRingBuffer *pCurBuff, VO_S32 nRequireNum)
{
	VO_S16 *tempUnaligned, *temp;

	if (nRequireNum > voGetSampleNum(pCurBuff)) 
	{
		// enlarge the buffer in 4kbyte steps (round up to next 4k boundary)
		pCurBuff->nSizeInBytes = (nRequireNum * pCurBuff->nChannles * sizeof(VO_S16) + 4095) & (VO_U32)-4096;

		tempUnaligned = (VO_S16 *)as_mem_malloc(pvoMemop, pCurBuff->nSizeInBytes + 16 / sizeof(VO_S16), 32);

		// Align the buffer to begin at 16byte cache line boundary for optimal performance
		temp = (VO_S16 *)(((VO_U32)tempUnaligned + 15) & (VO_U32)-16);
		if (pCurBuff->nSamplesInBuffer)
		{
			cmnMemCopy(VO_AUDIO_AudioSpeed, (void*)temp, (void*)voGetPtrBegin(pCurBuff), pCurBuff->nSamplesInBuffer * pCurBuff->nChannles * sizeof(VO_S16));
		}

		if (pCurBuff->pBufUnaligned)
		{
			as_mem_free(pvoMemop, pCurBuff->pBufUnaligned);
		}

		pCurBuff->pBuf = temp;
		pCurBuff->pBufUnaligned = tempUnaligned;

		pCurBuff->nBufferPos = 0;
	} 
	else 
	{
		// simply rewind the buffer (if necessary)
		voRewind(pCurBuff);
	}
}

/*
*  Get the insert buffer position, and meanwhile grows the buffer size to comply with this requirement
* //ptrEnd
*/
VO_S16 *CheckBufferInsert(VO_MEM_OPERATOR *pvoMemop, voRingBuffer *pCurBuf, VO_S32 nInsetNum)
{
	CheckBufferCapc(pvoMemop, pCurBuf, nInsetNum + pCurBuf->nSamplesInBuffer);
	return pCurBuf->pBuf + pCurBuf->nSamplesInBuffer * pCurBuf->nChannles;
}

/*
* Returns nonzero if the sample buffer is empty
*/
VO_S32 CheckIsEmpty(voRingBuffer *pCurBuf)
{
	return (pCurBuf->nSamplesInBuffer == 0) ? 1 : 0;
}

/*
* Clear Sample buffer
*/
void ClearBuf(voRingBuffer *pCurBuf)
{
	pCurBuf->nSamplesInBuffer = 0;
	pCurBuf->nBufferPos = 0;
}

/*
* Remove the used buffer data
*/
VO_S32 RemoveBuf(voRingBuffer *pCurBuf, VO_S32 nSample)
{
	if (nSample >= pCurBuf->nSamplesInBuffer)
	{
		VO_S32 temp;

		temp = pCurBuf->nSamplesInBuffer;
		pCurBuf->nSamplesInBuffer = 0;
		return temp;
	}

	pCurBuf->nSamplesInBuffer -= nSample;
	pCurBuf->nBufferPos += nSample;
	return nSample;
}

/*
* Add sample data into Buffer
*/
//void AddSamples(voRingBuffer *pCurBuf, VO_S16 *pSrc, VO_S32 nSamples)
//{
//	VO_S16  *pDst;
//
//	pDst = CheckBufferInsert(pCurBuf, nSamples + pCurBuf->nSamplesInBuffer);
//
//	cmnMemCopy(VO_AUDIO_AudioSpeed, pDst, pSrc, sizeof(VO_S16) * nSamples * pCurBuf->nChannles);
//	pCurBuf->nSamplesInBuffer += nSamples;
//}