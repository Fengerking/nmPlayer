//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    bitwise.c

Abstract:

    packing variable sized words into an octet stream.

Author:

    Witten Wen 14-September-2009

Revision History:

*************************************************************************/

#include "ogg.h"
#include "vovorbisdec.h"

static unsigned long mask[]=
{0x00000000,0x00000001,0x00000003,0x00000007,0x0000000f,
 0x0000001f,0x0000003f,0x0000007f,0x000000ff,0x000001ff,
 0x000003ff,0x000007ff,0x00000fff,0x00001fff,0x00003fff,
 0x00007fff,0x0000ffff,0x0001ffff,0x0003ffff,0x0007ffff,
 0x000fffff,0x001fffff,0x003fffff,0x007fffff,0x00ffffff,
 0x01ffffff,0x03ffffff,0x07ffffff,0x0fffffff,0x1fffffff,
 0x3fffffff,0x7fffffff,0xffffffff };

void voOGGDecPackReadinit(VOOGGInputBitStream *obs, OGGPacket *op)
{
	if(op->m_PacketStage->m_pSpanPacketBuf != 0)
		obs->m_ptr = op->m_PacketStage->m_pSpanPacketBuf;//span lace
	else
		obs->m_ptr = op->m_PacketStage->m_pPageBgn + op->m_PacketStage->m_PacketBegin;
	obs->m_CBitUsed = 0;
	obs->m_PacketLength = op->m_PacketStage->m_PacketLength;
}

/* bits <= 32 */
long voOGGDecPackReadBits(VOOGGInputBitStream *obs, int bits)//oggpack_read
{
	unsigned long m = mask[bits];
	OGG_U32 ret=-1;

	bits += obs->m_CBitUsed;//b->headbit;

	ret = obs->m_ptr[0] >> obs->m_CBitUsed;
	if (bits > 8)
	{
		ret |= obs->m_ptr[1] << (8-obs->m_CBitUsed); //headptr[1] << (8-b->headbit);
		if (bits>16)
		{
			ret |= obs->m_ptr[2] << (16-obs->m_CBitUsed);  
			if (bits>24)
			{
				ret |= obs->m_ptr[3] << (24-obs->m_CBitUsed);  
				if (bits>32 && obs->m_CBitUsed)
				{
					ret |= obs->m_ptr[4] << (32-obs->m_CBitUsed);
				}
			}
		}
	}

	obs->m_ptr += bits/8;
	obs->m_PacketLength -= bits/8;


	ret &= m;
	obs->m_CBitUsed = bits & 7;   
	return ret;
}

/* Read in bits without advancing the bitptr; bits <= 32 */
long voOGGDecPackLookBits(VOOGGInputBitStream *obs, int bits)	//oggpack_look
{
	unsigned long m = mask[bits];
	unsigned long ret = -1;

	bits += obs->m_CBitUsed;

	/* make this a switch jump-table */
	ret = obs->m_ptr[0] >> obs->m_CBitUsed;
	if (bits>8)
	{
		ret |= obs->m_ptr[1] << (8-obs->m_CBitUsed);
		if (bits>16)
		{
			ret |= obs->m_ptr[2] << (16-obs->m_CBitUsed);
			if (bits>24)
			{
				ret |= obs->m_ptr[3] << (24-obs->m_CBitUsed);
				if (bits>32 && obs->m_CBitUsed)
					ret |= obs->m_ptr[4] << (32-obs->m_CBitUsed);
			}
		}
	}
	ret &= m;
	return ret;
}

/* limited to 32 at a time */
void voOGGDecPackAdvBits(VOOGGInputBitStream *obs, int bits)	//oggpack_adv
{
	bits += obs->m_CBitUsed;
	obs->m_CBitUsed = bits & 7;
	obs->m_ptr += bits/8;
	obs->m_PacketLength -= bits/8;
}

void voOGGDecClearBitStream(CVOVorbisObjectDecoder *pvorbisdec)
{
	voOGGMemset(pvorbisdec->m_oggbs, 0, sizeof(VOOGGInputBitStream));
}


void voOGGDecSetNextBlockSpan(CVOVorbisObjectDecoder *pvorbisdec)
{
	pvorbisdec->m_oggbuffer->m_DataLength -= pvorbisdec->m_oggbuffer->m_CurBlockLen;
	pvorbisdec->m_oggbs->m_Flag = VO_FALSE;
	pvorbisdec->m_packetstage->m_BodyBytes = 0;
	pvorbisdec->m_packetstage->m_HeaderBytes = 0;
}

void voOGGDecSetNextBlock(CVOVorbisObjectDecoder *pvorbisdec)
{
	pvorbisdec->m_oggbuffer->m_DataLength -= pvorbisdec->m_oggbuffer->m_CurBlockLen;
	pvorbisdec->m_oggbs->m_Flag = VO_FALSE;
	voOGGMemset(pvorbisdec->m_packetstage, 0, sizeof(PacketStage));
}

/* reset obs to next packet */
void voOGGDecResetOBS2Packet(VOOGGInputBitStream *obs)
{
	if(obs->m_CBitUsed)
	{
		obs->m_CBitUsed = 0;
		obs->m_ptr++;
		obs->m_PacketLength--;
	}
}
void voOGGDecSaveHeadRestData(VOOGGInputBuffer *pInput)
{
	if (pInput->m_DataLength > 0)
	{
		//the last head block have frame data, save it.
		voOGGMemcpy(pInput->m_pBuffer, pInput->m_pNextPcktBgn-pInput->m_DataLength, pInput->m_DataLength);
		pInput->m_pNextPcktBgn = pInput->m_pDataEnd = pInput->m_pBuffer + pInput->m_DataLength;
	}
	else
	{
		pInput->m_pNextPcktBgn	= pInput->m_pDataEnd = pInput->m_pBuffer;
		pInput->m_DataLength	= 0;
	}
}
void BufferReset(VOOGGInputBuffer *pInput)
{
//	VOOGGInputBuffer *pInput = pvorbisdec->m_oggbuffer;
	if (pInput->m_DataLength > 0)
	{
		voOGGMemcpy(pInput->m_pBuffer, pInput->m_pNextPcktBgn, pInput->m_DataLength);
		pInput->m_pDataEnd = pInput->m_pBuffer + pInput->m_DataLength;
		pInput->m_pNextPcktBgn = pInput->m_pBuffer;
	}else{
		pInput->m_pNextPcktBgn	= pInput->m_pDataEnd = pInput->m_pBuffer;
		pInput->m_DataLength	= 0;
	}
}

void BufferResetSpan(CVOVorbisObjectDecoder *pvorbisdec)
{
	VOOGGInputBuffer *pInput = pvorbisdec->m_oggbuffer;
	OGGStreamState	*os = pvorbisdec->os;
	if(pInput->m_DataLength > 0)
	{
		voOGGMemcpy(pInput->m_pBuffer, pInput->m_pNextPcktBgn - os->m_SpanLaceStore, pInput->m_DataLength+os->m_SpanLaceStore);
		pInput->m_pDataEnd = pInput->m_pBuffer + pInput->m_DataLength+os->m_SpanLaceStore;		
	}
	else
	{
		voOGGMemcpy(pInput->m_pBuffer, pInput->m_pNextPcktBgn - os->m_SpanLaceStore, os->m_SpanLaceStore);
		pInput->m_pDataEnd = pInput->m_pBuffer + os->m_SpanLaceStore;
		pInput->m_DataLength	= 0;
	}
	pInput->m_pNextPcktBgn = pInput->m_pBuffer + os->m_SpanLaceStore;
	pvorbisdec->op->m_PacketStage->m_pSpanPacketBuf = pInput->m_pBuffer;
	pvorbisdec->op->m_PacketStage->m_pSpanPacketEnd = pInput->m_pBuffer + os->m_SpanLaceStore;
}

void PacketStageReset(PacketStage *packetstage)
{
	voOGGMemset(packetstage, 0, sizeof(PacketStage));
}