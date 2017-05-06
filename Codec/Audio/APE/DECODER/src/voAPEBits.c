/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2010         *
*									                                    *
************************************************************************/
/************************************************************************
File:		voAPEBits.c

Contains:	APE Decoder bits operator function

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

*************************************************************************/
#include  "stdio.h"
#include  "voAPE_Global.h"
#include  "voAPEBits.h"
#include  "voAPE_Header.h"
#include  "voTypedef.h"
#include  "voAPE_Predictor.h"
#include  "voAPETable.h"


#if 0   //disable old branch first.
/* Gets a rice value from the array*/
voDecodeValueRiceUnsigned(uint32 k)   //this is old version local function
{
	//variabl declares
	uint32 v;

	//plug through the string of 0's (the overflow)
	uint32 BitInitial = m_nCurrentBitIndex;
	while (!(m_pBitArray[m_nCurrentBitIndex >> 5] & Powers_of_Two_Reversed[m_nCurrentBitIndex++ & 31])) {}
	//if k = 0; your done
	if(k == 0)
		return (m_nCurrentBitIndex - BitInitial - 1);
	//put the overflow value into v
	v = (m_nCurrentBitIndex - BitInitial - 1) << k;

	return v|voDecodeValueXBits(k);
}
#endif

/* Read Byte from input buffer, and update m_nCurrentBitIndex */
static uint32 voDecodeByte(voDecodeBytes *DecByte)
{
	uint32 nByte;
	unsigned int *m_pBitArray = DecByte->m_pBitArray;
	unsigned int m_nCurrentBitIndex = DecByte->m_nCurrentBitIndex;
	/* read byte */
	nByte = ((m_pBitArray[m_nCurrentBitIndex >> 5] >> (24 - (m_nCurrentBitIndex & 31))) & 0xFF);
	DecByte->m_nCurrentBitIndex += 8;
	//printf("m_nCurrentBitIndex = %d, %d\n",DecByte->m_nCurrentBitIndex, nByte);
	return nByte;
}

uint32 voDecodeValueXBits(voDecodeBytes *DecByte, uint32 nBits)
{
	uint32  nLeftBits;
	uint32  nBitArrayIndex;
	int     nRightBits;
	uint32  nLeftValue;
	uint32  nRightValue;
	uint32  m_nCurrentBitIndex = DecByte->m_nCurrentBitIndex;
	uint32  *m_pBitArray = DecByte->m_pBitArray;

	/*variable declare */
	nLeftBits = 32 - (m_nCurrentBitIndex & 31);
	nBitArrayIndex = m_nCurrentBitIndex >> 5;
	DecByte->m_nCurrentBitIndex += nBits;

	// if theirs isn't an overflow to the right value, get the value and exit(m_nCNNFilter1 *)malloc(sizeof(m_nCNNFilter1));
	if(nLeftBits >= nBits)
		return (m_pBitArray[nBitArrayIndex] & POWERS_OF_TWO_MINUS_ONE[nLeftBits]) >> (nLeftBits - nBits);

	// must get the "split" value from left and right
	nRightBits = nBits - nLeftBits;

	nLeftValue = (m_pBitArray[nBitArrayIndex] & (POWERS_OF_TWO_MINUS_ONE[nLeftBits])) << nRightBits;
	nRightValue = (m_pBitArray[nBitArrayIndex + 1]) >> (32 - nRightBits);

	return (nLeftValue | nRightValue);
}


int voDecodeValue(voDECODE_VALUE_METHOD DecodeMethod, voDecodeBytes *DecByte)
{
	switch(DecodeMethod)
	{
	case DECODE_VALUE_METHOD_UNSIGNED_INT:
		return voDecodeValueXBits(DecByte, 32);
	}

	return 0;
}


int voAPESeek(int nBlockOffset)
{
	return 0;

}

static int  voRangeDecodeFast(voDecodeBytes *DecState, int nShift)
{
	while(DecState->m_RangeCoderInfo.range <= BOTTOM_VALUE)
	{
		DecState->m_RangeCoderInfo.buffer = (DecState->m_RangeCoderInfo.buffer << 8) | voDecodeByte(DecState);
		DecState->m_RangeCoderInfo.low = (DecState->m_RangeCoderInfo.low << 8) | ((DecState->m_RangeCoderInfo.buffer >> 1) & 0xFF);
		DecState->m_RangeCoderInfo.range <<= 8;
	}
	//Decode
	DecState->m_RangeCoderInfo.range = DecState->m_RangeCoderInfo.range >> nShift;

	return DecState->m_RangeCoderInfo.low / DecState->m_RangeCoderInfo.range;
}

static int voRangeDecodeFastWithUpdate(voDecodeBytes *DecState, int nShift)
{
	int nRetVal;

	while(DecState->m_RangeCoderInfo.range <= BOTTOM_VALUE)
	{
		DecState->m_RangeCoderInfo.buffer = (DecState->m_RangeCoderInfo.buffer << 8) | voDecodeByte(DecState);
		DecState->m_RangeCoderInfo.low = (DecState->m_RangeCoderInfo.low << 8) | ((DecState->m_RangeCoderInfo.buffer >> 1) & 0xFF);
		DecState->m_RangeCoderInfo.range <<= 8;
	}

	//decode
	DecState->m_RangeCoderInfo.range = DecState->m_RangeCoderInfo.range >> nShift;
	nRetVal = DecState->m_RangeCoderInfo.low / DecState->m_RangeCoderInfo.range;
	DecState->m_RangeCoderInfo.low -= DecState->m_RangeCoderInfo.range * nRetVal;
	return nRetVal;
}


int  voDecodeValueRange(voAPE_GData *st, voUnbit_Array_State *BitArrayState)
{
	int  m_nVersion;
	int  nValue = 0;
	int  nPivotValue;
	int  nOverflow = 0;
	int  nRangeTotal;
	int  nBase = 0;
	int  nShift = 0;
	int  nPivotValueA;
	int  nPivotValueB;
	int  nBaseA;
	int  nBaseB;
	int  nPivotValueBits;
	int  nSplitFactor;
	int  nBaseLower;
	int  nTempK;
	int  nX1;
	int  nX2;
	voDecodeBytes *DecodeState = st->DecProcess;
	m_nVersion = st->ape_info->nVersion;

	if(m_nVersion >= 3990)
	{
		//figure the pivot value
		nPivotValue = (BitArrayState->nKSum/32 > 1) ? BitArrayState->nKSum/32 : 1;//max(BitArrayState->nKSum/32, 1);
		//get the overflow
		nRangeTotal = voRangeDecodeFast(DecodeState, RANGE_OVERFLOW_SHIFT);
		while(nRangeTotal >= RANGE_TOTAL_2[nOverflow + 1])
		{
			nOverflow++;
		}

		//update
		DecodeState->m_RangeCoderInfo.low -= DecodeState->m_RangeCoderInfo.range * RANGE_TOTAL_2[nOverflow];
		DecodeState->m_RangeCoderInfo.range = DecodeState->m_RangeCoderInfo.range * RANGE_WIDTH_2[nOverflow];

		//get the working k
		if(nOverflow == (MODEL_ELEMENTS - 1))
		{
			nOverflow = voRangeDecodeFastWithUpdate(DecodeState, 16);
			nOverflow <<= 16;
			nOverflow |= voRangeDecodeFastWithUpdate(DecodeState, 16);
		}
		//get the value
		if(nPivotValue >= (1 << 16))
		{
			nPivotValueBits = 0;
			while((nPivotValue >> nPivotValueBits) > 0)
			{
				nPivotValueBits++;
			}
			nSplitFactor = 1 << (nPivotValueBits - 16);
			nPivotValueA = (nPivotValue / nSplitFactor) + 1;
			nPivotValueB = nSplitFactor;

			while(DecodeState->m_RangeCoderInfo.range <= BOTTOM_VALUE)
			{
				DecodeState->m_RangeCoderInfo.buffer = (DecodeState->m_RangeCoderInfo.buffer << 8) | voDecodeByte(DecodeState);
				DecodeState->m_RangeCoderInfo.low = (DecodeState->m_RangeCoderInfo.low << 8) | ((DecodeState->m_RangeCoderInfo.buffer >> 1) & 0xFF);
				DecodeState->m_RangeCoderInfo.range <<= 8;
			}
			DecodeState->m_RangeCoderInfo.range = DecodeState->m_RangeCoderInfo.range/nPivotValueA;
			nBaseA = DecodeState->m_RangeCoderInfo.low/DecodeState->m_RangeCoderInfo.range;
			DecodeState->m_RangeCoderInfo.low -= DecodeState->m_RangeCoderInfo.range * nBaseA;

			while(DecodeState->m_RangeCoderInfo.range <= BOTTOM_VALUE)
			{
				DecodeState->m_RangeCoderInfo.buffer = (DecodeState->m_RangeCoderInfo.buffer << 8) | voDecodeByte(DecodeState);
				DecodeState->m_RangeCoderInfo.low = (DecodeState->m_RangeCoderInfo.low << 8) | ((DecodeState->m_RangeCoderInfo.buffer >> 1) & 0xFF);
				DecodeState->m_RangeCoderInfo.range <<= 8;
			}
			DecodeState->m_RangeCoderInfo.range = DecodeState->m_RangeCoderInfo.range / nPivotValueB;
			nBaseB = DecodeState->m_RangeCoderInfo.low/DecodeState->m_RangeCoderInfo.range;
			DecodeState->m_RangeCoderInfo.low -= DecodeState->m_RangeCoderInfo.range * nBaseB;
			nBase = nBaseA * nSplitFactor + nBaseB;
		}	
		else
		{
			while(DecodeState->m_RangeCoderInfo.range <= BOTTOM_VALUE)
			{
				DecodeState->m_RangeCoderInfo.buffer = (DecodeState->m_RangeCoderInfo.buffer << 8) | voDecodeByte(DecodeState);
				DecodeState->m_RangeCoderInfo.low = (DecodeState->m_RangeCoderInfo.low << 8) | ((DecodeState->m_RangeCoderInfo.buffer >> 1) & 0xFF);
				DecodeState->m_RangeCoderInfo.range <<= 8;
			}

			//Decode
			DecodeState->m_RangeCoderInfo.range = DecodeState->m_RangeCoderInfo.range / nPivotValue;
			nBaseLower = DecodeState->m_RangeCoderInfo.low/DecodeState->m_RangeCoderInfo.range;
			DecodeState->m_RangeCoderInfo.low -= DecodeState->m_RangeCoderInfo.range * nBaseLower;

			nBase = nBaseLower;
		}

		//build the value
		nValue = nBase + (nOverflow * nPivotValue);
	}
	else
	{
		//decode
		nRangeTotal = voRangeDecodeFast(DecodeState, RANGE_OVERFLOW_SHIFT);

		//lookup the symbol 
		nOverflow = 0;
		while(nRangeTotal >= RANGE_TOTAL_1[nOverflow + 1])
		{
			nOverflow++;
		}

		//update
		DecodeState->m_RangeCoderInfo.low -= DecodeState->m_RangeCoderInfo.range * RANGE_TOTAL_1[nOverflow];
		DecodeState->m_RangeCoderInfo.range = DecodeState->m_RangeCoderInfo.range * RANGE_WIDTH_1[nOverflow];

		//get the working k
		if(nOverflow == (MODEL_ELEMENTS - 1))
		{
			nTempK = voRangeDecodeFastWithUpdate(DecodeState, 5);
			nOverflow = 0;
		}
		else
		{
			nTempK = (BitArrayState->k < 1) ? 0 : BitArrayState->k - 1; 
		}
		//figure the extra bits on the left and the left value
		if(nTempK <= 16 || m_nVersion < 3910)
		{
			nValue = voRangeDecodeFastWithUpdate(DecodeState, nTempK);
		}
		else
		{
			nX1 = voRangeDecodeFastWithUpdate(DecodeState, 16);
			nX2 = voRangeDecodeFastWithUpdate(DecodeState, nTempK - 16);
			nValue = nX1 | (nX2 << 16);
		}

		//build the value and output it
		nValue += (nOverflow << nTempK);
	}

	//update nKsum
	BitArrayState->nKSum += ((nValue + 1)/2) - ((BitArrayState->nKSum + 16) >> 5);

	//update k
	if(BitArrayState->nKSum < K_SUM_MIN_BOUNDARY[BitArrayState->k])
		BitArrayState->k--;
	else if(BitArrayState->nKSum >= K_SUM_MIN_BOUNDARY[BitArrayState->k + 1])
		BitArrayState->k++;

	//output the value(converted to signed)
	return (nValue & 1) ? (nValue >> 1) + 1 : -(nValue >> 1);
}


