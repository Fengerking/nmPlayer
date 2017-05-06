   /************************************************************************
	*									*								   *
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*								   *
	************************************************************************/

#ifndef _HUFF_H_
#define _HUFF_H_

#include "global.h"
#include "bitstream.h"

#define GET_OFFBIT(v)			((int)(v) >> 15)		        /* bits 15    */
#define GET_OFFSET(v)			((int)(v) & 0xFFF)		        /* bits 11-0  */
#define GET_VALUE(v)			(((int)(v) << 20) >> 20)	    /* bits 11-0  */
#define GET_BITS(v)				(((int)(v) >> 12) & 0x7)	    /* bits 14-12 */

#define GET_QUAD_W(v)			(((int)(v) << 20) >> 29)		/* bits 11-9  */
#define GET_QUAD_X(v)			(((int)(v) << 23) >> 29)		/* bits  8-6  */
#define GET_QUAD_Y(v)			(((int)(v) << 26) >> 29)		/* bits  5-3  */
#define GET_QUAD_Z(v)			(((int)(v) << 29) >> 29)		/* bits  2-0  */

#define GET_PAIR_Y(v)			(((int)(v) << 22) >> 27)		/* bits  9-5  */
#define GET_PAIR_Z(v)			(((int)(v) << 27) >> 27)		/* bits  4-0  */

#define GET_ESC_Y(v)			(((int)(v) << 20) >> 26)		/* bits 11-6  */
#define GET_ESC_Z(v)			(((int)(v) << 26) >> 26)		/* bits  5-0  */

#define SET_SIGN(v, s)		    {(v) ^= ((int)(s) >> 31); (v) -= ((int)(s) >> 31);}

typedef struct _HuffInfoData {
    const short* HufTab;
	int  maxBits;                                                
    int  startBits;                                                 
} HuffInfoData;

static INLINE
int DecodeHuffmanData(const HuffInfoData* huffInfo, BitStream *bs)
{
	int maxBits, nCodeBits, nSkipBits, HufVal;
	unsigned int bitCache;
	const short *HufTab, *sHufTab;

	maxBits = huffInfo->maxBits;
	sHufTab = huffInfo->HufTab;

	bitCache = BitStreamShowBits(bs, maxBits) << (32 - maxBits);
	
	HufTab = sHufTab;
	nSkipBits = huffInfo->startBits;
	nCodeBits = 0;	
	HufVal = HufTab[bitCache >> (32 - nSkipBits)];

	while(GET_OFFBIT(HufVal))
	{
		bitCache <<= nSkipBits;
		nCodeBits += nSkipBits;
		nSkipBits =	GET_BITS(HufVal);
		HufTab = sHufTab + GET_OFFSET(HufVal);		
		HufVal = HufTab[bitCache >> (32 - nSkipBits)];
	}		

	nCodeBits += GET_BITS(HufVal);
	BitStreamSkip(bs, nCodeBits);
	
	return GET_VALUE(HufVal);
}

/* hufftab.c */
extern const HuffInfoData huffTabSpecInfo[11];
extern const HuffInfoData huffTabScaleInfo; 

#endif// _HUFF_H_

