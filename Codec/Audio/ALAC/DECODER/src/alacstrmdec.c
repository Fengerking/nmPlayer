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
*		alacstrmdec.c
* 
* Abstact:	
*
*		Apple Lossless Audio Codec for VisualOn Decoder read stream file.
*
* Author:
*
*		Witten Wen 1-April-2010
*
* Revision History:
*
******************************************************/

#include "vomemory.h"
#include "voalacdec.h"
#include "alacstrmdec.h"

VO_VOID	ReadStream(CALACObjectDecoder *palacdec, VO_U32 iSize, VO_VOID *pBuffer)
{
	voALACMemcpy(pBuffer, palacdec->m_pInBuffer, iSize);
	palacdec->m_pInBuffer += iSize;
}

VO_U32 ReadU32(CALACObjectDecoder *palacdec)
{
	VO_U32 v;
	voALACMemcpy(&v, palacdec->m_pInBuffer, 4);
	palacdec->m_pInBuffer += 4;
	return SWAPBYTE32(v);
}

VO_U16 ReadU16(CALACObjectDecoder *palacdec)
{
	VO_U16 v;
	voALACMemcpy(&v, palacdec->m_pInBuffer, 2);
	palacdec->m_pInBuffer += 2;
	return SWAPBYTE16(v);
}

/* supports reading 1 to 16 bits, in big endian format */
VO_U32	ReadBits16(CALACBitStream *pstrmbits, int bits)
{
	VO_U32	result = 0;

//	bits += pstrmbits->m_UsedBits;
	result = (pstrmbits->m_ptr[0] << 16) | (pstrmbits->m_ptr[1] << 8) | pstrmbits->m_ptr[2];

	result <<= pstrmbits->m_UsedBits;

	result &= 0x00ffffff;

	result >>= 24 - bits;

	bits += pstrmbits->m_UsedBits;

	pstrmbits->m_ptr += (bits >> 3);

	pstrmbits->m_Length -= (bits >> 3);

	pstrmbits->m_UsedBits = bits & 7;

	return result;
}

VO_U32	ReadBits32(CALACBitStream *pstrmbits, int bits)
{
	VO_U32	result = 0;
	
	if(bits>16)
	{
		bits -= 16;
		result = ReadBits16(pstrmbits, 16) << bits;
	}
	result |= ReadBits16(pstrmbits, bits);

	return result;
}

/* read a single bit */
VO_U32	ReadBit(CALACBitStream *pstrmbits)
{
	VO_U32 result = 0;
	
	result = pstrmbits->m_ptr[0];
	result <<= pstrmbits->m_UsedBits; 
	result = (result>>7) & 1;

	pstrmbits->m_UsedBits ++;
	pstrmbits->m_ptr += (pstrmbits->m_UsedBits>>3);	
	pstrmbits->m_Length -= (pstrmbits->m_UsedBits>>3);
	pstrmbits->m_UsedBits &= 7;

	return result;
}

VO_VOID SkipBits(CALACBitStream *pstrmbits, int bits)
{
	bits += pstrmbits->m_UsedBits;

	pstrmbits->m_ptr += (bits >> 3);

	pstrmbits->m_Length -= (bits >> 3);

	pstrmbits->m_UsedBits = bits & 7;
}

VO_VOID	UnReadBits(CALACBitStream *pstrmbits, int bits)
{
	int temp = pstrmbits->m_UsedBits - bits;

	pstrmbits->m_ptr += (temp>>3);

	pstrmbits->m_Length -= (temp>>3);

	pstrmbits->m_UsedBits = temp & 7;
}
