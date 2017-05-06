
#include "BitVector.h"

CBitVector::CBitVector(unsigned char * baseBytePtr, unsigned baseBitOffset, unsigned totNumBits) 
{
	Setup(baseBytePtr, baseBitOffset, totNumBits);
}

void CBitVector::Setup(unsigned char * baseBytePtr, unsigned baseBitOffset, unsigned totNumBits) 
{
	m_baseBytePtr   = baseBytePtr;
	m_baseBitOffset = baseBitOffset;
	m_totNumBits    = totNumBits;
	m_curBitIndex   = 0;
}

static unsigned char singleBitMask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

#define MAX_LENGTH 32

void CBitVector::PutBits(unsigned from, unsigned numBits) 
{
	unsigned char tmpBuf[4];
	unsigned overflowingBits = 0;

	if(numBits > MAX_LENGTH) 
	{
		numBits = MAX_LENGTH;
	}

	if(numBits > m_totNumBits - m_curBitIndex) 
	{
		overflowingBits = numBits - (m_totNumBits - m_curBitIndex);
	}

	tmpBuf[0] = (unsigned char)(from>>24);
	tmpBuf[1] = (unsigned char)(from>>16);
	tmpBuf[2] = (unsigned char)(from>>8);
	tmpBuf[3] = (unsigned char)from;

	ShiftBits(m_baseBytePtr, m_baseBitOffset + m_curBitIndex, tmpBuf, MAX_LENGTH - numBits, numBits - overflowingBits);
	m_curBitIndex += numBits - overflowingBits;
}

void CBitVector::Put1Bit(unsigned bit) 
{
	if(m_curBitIndex >= m_totNumBits)
	{
		return;
	}
	else 
	{
		unsigned totBitOffset = m_baseBitOffset + m_curBitIndex++;
		unsigned char mask = singleBitMask[totBitOffset%8];
		if (bit) 
		{
			m_baseBytePtr[totBitOffset/8] |= mask;
		} 
		else 
		{
			m_baseBytePtr[totBitOffset/8] &=~ mask;
		}
	}
}


unsigned CBitVector::GetBits(unsigned numBits) 
{
	unsigned char tmpBuf[4];
	unsigned overflowingBits = 0;

	if (numBits > MAX_LENGTH)
	{
		numBits = MAX_LENGTH;
	}

	if (numBits > m_totNumBits - m_curBitIndex)
	{
		overflowingBits = numBits - (m_totNumBits - m_curBitIndex);
	}

	ShiftBits(tmpBuf, 0, m_baseBytePtr, m_baseBitOffset + m_curBitIndex, numBits - overflowingBits );
	m_curBitIndex += numBits - overflowingBits;

	unsigned result = (tmpBuf[0]<<24) | (tmpBuf[1]<<16) | (tmpBuf[2]<<8) | tmpBuf[3];
	result >>= (MAX_LENGTH - numBits);
	result &= (0xFFFFFFFF << overflowingBits);
	return result;
}

unsigned CBitVector::Get1Bit() 
{
	if (m_curBitIndex >= m_totNumBits) 
	{
		return 0;
	} 
	else 
	{
		unsigned totBitOffset = m_baseBitOffset + m_curBitIndex++;
		unsigned char curFromByte = m_baseBytePtr[totBitOffset/8];
		unsigned result = (curFromByte >> (7-(totBitOffset%8))) & 0x01;
		return result;
	}
}

void CBitVector::SkipBits(unsigned numBits) 
{
	if (numBits > m_totNumBits - m_curBitIndex) 
	{ 
		m_curBitIndex = m_totNumBits;
	}
	else 
	{
		m_curBitIndex += numBits;
	}
}

void ShiftBits(unsigned char* toBasePtr, unsigned toBitOffset,
			   unsigned char * fromBasePtr, unsigned fromBitOffset,
			   unsigned numBits) 
{
	unsigned char * fromBytePtr = fromBasePtr + fromBitOffset/8;
	unsigned fromBitRem = fromBitOffset%8;
	unsigned char * toBytePtr = toBasePtr + toBitOffset/8;
	unsigned toBitRem = toBitOffset%8;

	while (numBits-- > 0)
	{
		unsigned char fromBitMask = singleBitMask[fromBitRem];
		unsigned char fromBit = (*fromBytePtr)&fromBitMask;
		unsigned char toBitMask = singleBitMask[toBitRem];

		if (fromBit != 0)
		{
			*toBytePtr |= toBitMask;
		} 
		else 
		{
			*toBytePtr &=~ toBitMask;
		}

		if (++fromBitRem == 8) 
		{
			++fromBytePtr;
			fromBitRem = 0;
		}
		if (++toBitRem == 8) 
		{
			++toBytePtr;
			toBitRem = 0;
		}
	}
}

