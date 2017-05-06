/**********************************************************************
*
* VisualOn voplayer
*
* CopyRight (C) VisualOn SoftWare Co., Ltd. All rights are reserved.
*
***********************************************************************/

/*********************************************************************
*
* Filename:	
*		alacdec.c
* 
* Abstact:	
*
*		Apple Lossless Audio Codec for VisualOn Decoder AP file.
*
* Author:
*
*		Witten Wen 24-March-2010
*
* Revision History:
*
******************************************************/
#include "headerparser.h"
#include "alacstrmdec.h"
#include "voalacdec.h"
#include <malloc.h>

VO_S32 Parsestsd(CALACObjectDecoder *palacdec, HeadParse *headparse, VO_U32 chunklen)
{
	VO_U32	num_entries = 0, i;
	VO_U32	remainsize = chunklen - 8;
	fourc_t m_Format;
	//skip version 1 byte, flag 3 byte
	palacdec->m_pInBuffer += 4;
	remainsize -= 4;

	num_entries = ReadU32(palacdec);
	remainsize -= 4;
	if(num_entries != 1)return -1;
	for(i=0; i<num_entries; i++)
	{
		VO_U32	entrysize, remainentry, CodecDataLen;
		VO_U16	version;
		fourc_t	CodecDataID;

		remainentry = entrysize = ReadU32(palacdec);
		m_Format = ReadU32(palacdec);
		remainentry -= 8;

		/* sound info: */
		palacdec->m_pInBuffer += 6;/* reserved, skip it! */
		remainentry -= 6;

		version = ReadU16(palacdec);
		if(version != 1)
		{
			/* so far, the version must be 1.  It may be others in the future.*/
			return -1;
		}
		remainentry -= 2;

		/* Revision level 2 bytes, vendor 4 bytes, and Reserved 2 bytes. skip them! */
		palacdec->m_pInBuffer += 8;
		remainentry -= 8;

		/* channel and sample bits */
//		palacdec->m_Channels = ReadU16(palacdec);
//		palacdec->m_SampleBits = ReadU16(palacdec);
		headparse->ALACFormat->nChannels = ReadU16(palacdec);
		headparse->ALACFormat->wBitsPerSample = ReadU16(palacdec);
		remainentry -= 4;

		/* ID 2 bytes and packet size 2 bytes */
		palacdec->m_pInBuffer += 4;
		remainentry -= 4;

		/* sample rate 2 bytes */
		headparse->ALACFormat->nSamplesPerSec = (VO_U32)ReadU16(palacdec);
		remainentry -= 2;		  

		/* stream skip 2 bytes */
		ReadU16(palacdec);
		remainentry -= 2;

		/* Codec data */
		/* 12 = audio format atom, 8 = padding */
//		palacdec->m_CodecDataLen = remainentry + 20;	//12 + 8;
//		palacdec->m_pCodecData = voALACCalloc(1, palacdec->m_CodecDataLen);

		/* audio format atom */
//		((VO_U32 *)palacdec->m_pCodecData)[0] = 0x0c000000;
//		((VO_U32 *)palacdec->m_pCodecData)[1] = MAKEFOURC('a', 'm', 'r', 'f');
//		((VO_U32 *)palacdec->m_pCodecData)[2] = MAKEFOURC('c', 'a', 'l', 'a');

//		ReadStream(palacdec, remainentry, ((VO_U8 *)palacdec->m_pCodecData) + 12);
		CodecDataLen = ReadU32(palacdec);		
		CodecDataID = ReadU32(palacdec);		
		if(CodecDataID != MAKEFOURC('a', 'l', 'a', 'c'))return -1;
		if(CodecDataLen != remainentry)return -1;
		remainentry -= 8;
		headparse->ALACFormat->cbSize = (VO_U16)remainentry;
		ReadStream(palacdec, remainentry, ((VO_U8 *)headparse->ALACFormat) + 18);

		remainentry = 0;

//		palacdec->m_ReadFormat = 1;

		if(m_Format != MAKEFOURC('a', 'l', 'a', 'c'))return -1;		
	}

	return 0;
}

VO_VOID Parsestts(CALACObjectDecoder *palacdec, HeadParse *headparse, VO_U32 chunklen)
{
	VO_U32	num_entries = 0, i;
	VO_U32	remainsize = chunklen - 8;

	//skip version 1 byte, flag 3 byte
	palacdec->m_pInBuffer += 4;
	remainsize -= 4;

	num_entries = ReadU32(palacdec);
	remainsize -= 4;
//	palacdec->m_NumTime2Sample = num_entries;
//	palacdec->m_pTime2Sample = voALACMalloc(num_entries * sizeof(*palacdec->m_pTime2Sample));
	headparse->m_NumTime2Sample = num_entries;
	headparse->m_pTime2Sample = malloc(num_entries * sizeof(*headparse->m_pTime2Sample));

	for(i=0; i<num_entries; i++)
	{
//		palacdec->m_pTime2Sample[i].m_SampleCount =		ReadU32(palacdec);
//		palacdec->m_pTime2Sample[i].m_SampleDuration =	ReadU32(palacdec);
		headparse->m_pTime2Sample[i].m_SampleCount =	ReadU32(palacdec);
		headparse->m_pTime2Sample[i].m_SampleDuration =	ReadU32(palacdec);
		remainsize -= 8;
	}
	if(remainsize)
		palacdec->m_pInBuffer += remainsize;
}

VO_VOID Parsestsz(CALACObjectDecoder *palacdec, HeadParse *headparse, VO_U32 chunklen)
{
	VO_U32	num_entries = 0, i;
	VO_U32	remainsize = chunklen - 8;

	//skip version 1 byte, flag 3 byte
	palacdec->m_pInBuffer += 4;
	remainsize -= 4;

	/* default sample size */
    if (ReadU32(palacdec) != 0)
    {
        ReadU32(palacdec);
        remainsize -= 8;
        return;
    }
	remainsize -= 4;

	num_entries = ReadU32(palacdec);
	remainsize -= 4;
//	palacdec->m_NumSampleByteSize = num_entries;
//	palacdec->m_pSampleByteSize = voALACMalloc(num_entries * sizeof(*palacdec->m_pSampleByteSize));
	headparse->m_NumSampleByteSize = num_entries;
	headparse->m_pSampleByteSize = malloc(num_entries * sizeof(*headparse->m_pSampleByteSize));
	for(i=0; i<num_entries; i++)
	{
//		palacdec->m_pSampleByteSize[i] = ReadU32(palacdec);
		headparse->m_pSampleByteSize[i] = ReadU32(palacdec);
		remainsize -= 4;
	}
	if(remainsize)
		palacdec->m_pInBuffer += remainsize;
}

VO_S32 Parsestbl(CALACObjectDecoder *palacdec, HeadParse *headparse, VO_U32 chunklen)
{
	VO_U32	remainsize = chunklen - 8;

	while(remainsize)
	{
		VO_U32	SubChunkLen = ReadU32(palacdec);
		fourc_t	SubChunkID = ReadU32(palacdec);

		switch(SubChunkID)
		{
		case MAKEFOURC('s', 't', 's', 'd'):
			if(Parsestsd(palacdec, headparse, SubChunkLen) < 0)return -1;
			break;

		case MAKEFOURC('s', 't', 't', 's'):
			Parsestts(palacdec, headparse, SubChunkLen);
			break;

		case MAKEFOURC('s', 't', 's', 'z'):
			Parsestsz(palacdec, headparse, SubChunkLen);
			break;

		case MAKEFOURC('s', 't', 's', 'c'):
		case MAKEFOURC('s', 't', 'c', 'o'):
			palacdec->m_pInBuffer += SubChunkLen - 8;
			break;

		default:
			return -1;
		}
		remainsize -= SubChunkLen;
	}

	return 1;

}

VO_S32 Parseminf(CALACObjectDecoder *palacdec, HeadParse *headparse, VO_U32 chunklen)
{
	VO_U32	remainsize = chunklen - 8;
	VO_U32	SubChunkLen = 0;

	/**** SOUND HEADER CHUNK ****/
	if(ReadU32(palacdec) != 16)return -1;
	if(ReadU32(palacdec) != MAKEFOURC('s', 'm', 'h', 'd'))return -1;
	palacdec->m_pInBuffer += 8;
	remainsize -= 16;

	/**** DINF CHUNK ****/
	SubChunkLen = ReadU32(palacdec);
	if(ReadU32(palacdec) != MAKEFOURC('d', 'i', 'n', 'f'))return -1;
	palacdec->m_pInBuffer += SubChunkLen - 8;
	remainsize -= SubChunkLen;

	/**** SAMPLE TABLE ****/
	SubChunkLen = ReadU32(palacdec);
	if(ReadU32(palacdec) != MAKEFOURC('s', 't', 'b', 'l'))return -1;
	if(Parsestbl(palacdec, headparse, SubChunkLen)<0)return -1;
	remainsize -= SubChunkLen;

	if(remainsize > 0)
	{
		palacdec->m_pInBuffer += remainsize;
		remainsize = 0;
	}
	return 0;
}

VO_S32 Parsemdia(CALACObjectDecoder *palacdec, HeadParse *headparse, VO_U32 chunklen)
{
	VO_U32 remainsize = chunklen - 8;
	while(remainsize)
	{
		VO_U32	SubChunkLen = ReadU32(palacdec);
		fourc_t	SubChunkID = ReadU32(palacdec);

		if (SubChunkLen <= 1 || SubChunkLen > remainsize)return -1;

		switch(SubChunkID)
		{
		case MAKEFOURC('m', 'd', 'h', 'd'):
		case MAKEFOURC('h', 'd', 'l', 'r'):
			palacdec->m_pInBuffer += SubChunkLen - 8;
			break;

		case MAKEFOURC('m', 'i', 'n', 'f'):
			if(Parseminf(palacdec, headparse, SubChunkLen)<0)return -1;
			break;

		default:
			return -1;

		}
		remainsize -= SubChunkLen;
	}
	return 0;
}

static VO_U32 Parsetrak(CALACObjectDecoder *palacdec, HeadParse *headparse, VO_U32 chunklen)
{
	VO_U32 remainsize = chunklen - 8;

	while(remainsize)
	{
		VO_U32	SubChunkLen = ReadU32(palacdec);
		fourc_t	SubChunkID = ReadU32(palacdec);

		if (SubChunkLen <= 1 || SubChunkLen > remainsize)return 0;

		switch(SubChunkID)
		{
		case MAKEFOURC('m', 'd', 'i', 'a'):
			if(Parsemdia(palacdec, headparse, SubChunkLen)<0)return 0;
			break;

		case MAKEFOURC('t', 'k', 'h', 'd'):
		case MAKEFOURC('e', 'd', 't', 's'):
			palacdec->m_pInBuffer += SubChunkLen - 8;
			break;

		default:
			return 0;

		}
		remainsize -= SubChunkLen;
	}

	return 1;

}

VO_U32 ParseHeaderMoov(CALACObjectDecoder *palacdec, HeadParse *headparse, VO_U8 *pMoov, VO_U32 moovlen)
{
//	VO_U32 remain_size = moovlen;

	if (moovlen == 0) return 0; 
	palacdec->m_pInBuffer = pMoov;
	palacdec->m_InSize = moovlen;

	while(moovlen)
	{
		VO_U32	SubChunkLen = ReadU32(palacdec);
		fourc_t	SubChunkID = ReadU32(palacdec);

		if (SubChunkLen <= 1 || SubChunkLen > moovlen)return 0;

		switch(SubChunkID)
		{
		case MAKEFOURC('t', 'r', 'a', 'k'):
			if(Parsetrak(palacdec, headparse, SubChunkLen)==0)
				return 0;
			break;

		case MAKEFOURC('m', 'v', 'h', 'd'):
		case MAKEFOURC('u', 'd', 't', 'a'):
		case MAKEFOURC('e', 'l', 's', 't'):
		case MAKEFOURC('i', 'o', 'd', 's'):
			palacdec->m_pInBuffer += SubChunkLen - 8;
			break;

		default:
			return 0;
		}
		moovlen -= SubChunkLen;
	}

	return 1;
}

VO_VOID	FreeALACParser(HeadParse *headparse)
{
	if(headparse->m_pTime2Sample)
	{
		free(headparse->m_pTime2Sample);
		headparse->m_pTime2Sample = NULL;
	}
	if(headparse->m_pSampleByteSize)
	{
		free(headparse->m_pSampleByteSize);
		headparse->m_pSampleByteSize = NULL;
	}
}

