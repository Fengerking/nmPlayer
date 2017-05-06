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
* @file voRingBuffer.h
*
* FIFO RingBuffer basic operation header files
*
* @author  Huaping Liu
* @date    2013-05-02
************************************************************************/

#ifndef __VO_RINGBUFFER_H__
#define __VO_RINGBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voType.h"
#include "voMem.h"

typedef struct  
{
	/* Buffer first address */
	VO_S16    *pBuf;        
    /* Raw unaligned buffer memory. 'pBuf' is made aligned by pointing it to first 16-byte aligned location of this buffer */
	VO_S16    *pBufUnaligned;                    

	/* Sample Buffer size in bytes */
    VO_S32    nSizeInBytes;

    /* Really samples in Buffer */
	VO_S32    nSamplesInBuffer;

    /* Channels number */
	VO_S32    nChannles;
    
	/* Current position pointer to the buffer */
	VO_S32   nBufferPos;

}voRingBuffer;

/* Ring buffer operation function */
// ensureCapacity
void CheckBufferCapc(VO_MEM_OPERATOR   *pvoMemop, voRingBuffer *pCurBuff, VO_S32 nRequireNum);

// Check Inset buffer, and expand the buffer
VO_S16 *CheckBufferInsert(VO_MEM_OPERATOR  *pvoMemop, voRingBuffer *pCurBuf, VO_S32 nInsetNum);

// Check the buffer Empty or not
VO_S32 CheckIsEmpty(voRingBuffer *pCurBuf);

// Clear the Buffer
void ClearBuf(voRingBuffer *pCurBuf);

// Remove the used Buffer data
VO_S32 RemoveBuf(voRingBuffer *pCurBuf, VO_S32 nSample);

// Get the Start Address
VO_S16 *voGetPtrBegin(voRingBuffer *pBuf);

// Store data into Buffer
void AddSamples(voRingBuffer *pCurBuf, VO_S16 *pSrc, VO_S32 nSamples);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__VO_RINGBUFFER_H__


