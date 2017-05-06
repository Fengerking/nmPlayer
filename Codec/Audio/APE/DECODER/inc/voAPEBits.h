/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2010 	    *
*									                                    *
************************************************************************/
/***********************************************************************
File:		voAPEBits.h

Contains:	APE Decoder bits operator header

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

************************************************************************/
#ifndef __VOAPEBITS_H__
#define __VOAPEBITS_H__

#include "voTypedef.h"

typedef struct{
	unsigned int low;       /* low end of interval */
	unsigned int range;     /* length of interval */
	unsigned int buffer;    /* buffer for input/output */
}Range_Dec;


typedef enum
{
	DECODE_VALUE_METHOD_UNSIGNED_INT,
	DECODE_VALUE_METHOD_UNSIGNED_RICE,
	DECODE_VALUE_METHOD_X_BITS
}voDECODE_VALUE_METHOD;


typedef struct{
	/* file info */
	int     m_nBlockAlign;
	int     m_nCurrentFrame;

	/* start, finish information, decompression seek */
	int     m_bIsRanged;                                         /* adjust seek range */
	int     m_nStartBlock;                                       /* Start block index */
	int     m_nFinishBlock;                                      /* Finish block index */
	int     m_nCurrentBlock;                                     /* current block index */

	int     nBlocksUnitFinish;                                   /* leave blocks of the file */
	int     m_nCurrentFrameBufferBlock;                          /* currentFrameBuffer Blocks number */	
	int     m_bErrorDecodingCurrentFrame;
	int     m_nErrorDecodingCurrentFrameOutputSilenceBlocks;
}voAPEBlocks;

typedef struct
{
	unsigned int  m_nCRC;
	unsigned int  m_nStoredCRC;
	int           m_nSpecialCodes;
	int           m_nLastX;
}voAPE_Framehdr;

typedef struct
{
	Range_Dec         m_RangeCoderInfo;
	voAPEBlocks       Blk_State;
	voAPE_Framehdr    m_Framehdr;
	unsigned int      m_nCurrentBitIndex;                             /* useed Bits this process */
	unsigned int      *m_pBitArray;                                   /* bit array pointer */
	short             *m_poutbuf;                    	              /* output buffer pointer */
}voDecodeBytes;

uint32 voDecodeValueXBits(voDecodeBytes *DecByte, uint32 nBits);
int voDecodeValue(voDECODE_VALUE_METHOD DecodeMethod, voDecodeBytes *DecByte);

#endif   //__VOAPEBITS_H__


