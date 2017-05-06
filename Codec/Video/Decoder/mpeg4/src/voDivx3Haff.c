/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
 
//#include "stdafx.h"
#include  "voDivx3Haff.h"
#include "voDivx3Tab.h"
#include "voMpegReadbits.h"
#ifdef VODIVX3

#define VLC_BITS 9

VO_S32 GetDivx3VLC( VO_MPEG4_DEC* pDec, const VO_U16* table )
{
	VO_S32 nUsedBits;
	VO_S32 VLCode;
	VO_S32 nNextBits = VLC_BITS;
	VLCode = table[SHOW_BITS(pDec,nNextBits)];
	nUsedBits = VLCode >> 12;
	if (nUsedBits >= 0xC)
	{
		FLUSH_BITS(pDec,nNextBits);
		nNextBits = VLCode & 0xF;
		table += (VLCode & (0x3FF<<4)) >> 2;

		VLCode = table[SHOW_BITS(pDec,nNextBits)];
		nUsedBits = VLCode >> 12;
		if (nUsedBits >= 0xC)
		{
			FLUSH_BITS(pDec,nNextBits);
			nNextBits = VLCode & 0xF;
			table += (VLCode & (0x3FF<<4)) >> 2;

			UPDATE_CACHE(pDec);
			VLCode = table[SHOW_BITS(pDec,nNextBits)];
			nUsedBits = VLCode >> 12;
			//			assert(n < 12);
		}
	}

	FLUSH_BITS(pDec,nUsedBits);

	return VLCode & 0xFFF;
}
#endif
