#include "BVUtility.h"

CBVUtility::CBVUtility(unsigned char * pBits, unsigned int offset, unsigned int numBits) 
{
	Setup(pBits, offset, numBits);
}

CBVUtility::~CBVUtility()
{
}

void CBVUtility::Setup(unsigned char * pBits, unsigned int offset, unsigned int numBits) 
{
	m_baseBytePtr   = pBits;
	m_baseBitOffset = offset;
	m_totNumBits    = numBits;
	m_curBitIndex   = 0;
}

static unsigned char singleBitMask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

#define MAX_LENGTH 32

void CBVUtility::PutBits(unsigned int from, unsigned int numBits) 
{
	unsigned overflowingBits = 0;
	
	if(numBits > MAX_LENGTH) 
	{
		numBits = MAX_LENGTH;
	}

	if(numBits > m_totNumBits - m_curBitIndex) 
	{
		overflowingBits = numBits - (m_totNumBits - m_curBitIndex);
	}

	unsigned char tmpBuf[4];
	tmpBuf[0] = (unsigned char)(from >> 24);
	tmpBuf[1] = (unsigned char)(from >> 16);
	tmpBuf[2] = (unsigned char)(from >> 8);
	tmpBuf[3] = (unsigned char)from;

	ShiftBits(m_baseBytePtr, m_baseBitOffset + m_curBitIndex, tmpBuf, MAX_LENGTH - numBits, numBits - overflowingBits);
	m_curBitIndex += numBits - overflowingBits;
}

void CBVUtility::PutBit(unsigned int bit) 
{
	if(m_curBitIndex >= m_totNumBits)
		return;
	
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

unsigned CBVUtility::GetBits(unsigned int numBits) 
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

unsigned CBVUtility::GetBit() 
{
	if (m_curBitIndex >= m_totNumBits) 
		return 0;

	unsigned totBitOffset = m_baseBitOffset + m_curBitIndex++;
	unsigned char curFromByte = m_baseBytePtr[totBitOffset/8];
	unsigned result = (curFromByte >> (7-(totBitOffset%8))) & 0x01;
	return result;
}

void CBVUtility::SkipBits(unsigned int numBits) 
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

void ShiftBits(unsigned char* toBasePtr, unsigned int toBitOffset, unsigned char * fromBasePtr, unsigned int fromBitOffset, unsigned int numBits) 
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
