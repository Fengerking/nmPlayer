#ifndef __BVUTILITY_H__
#define __BVUTILITY_H__

class CBVUtility 
{
public:
	CBVUtility(unsigned char * pBits, unsigned int offset, unsigned int numBits);
	~CBVUtility();
	
public:
	void Setup(unsigned char * pBits, unsigned int offset, unsigned int numBits);
	void PutBits(unsigned int from, unsigned int numBits);
	void PutBit(unsigned int bit);
	unsigned GetBits(unsigned int numBits);
	unsigned GetBit();
	void SkipBits(unsigned int numBits);
	
public:	
	unsigned CurBitIndex() const { return m_curBitIndex; }
	unsigned TotNumBits() const { return m_totNumBits; }
	unsigned NumBitsRemaining() const { return m_totNumBits - m_curBitIndex; } 
	
private:
	unsigned char * m_baseBytePtr;
	unsigned        m_baseBitOffset;
	unsigned        m_totNumBits;
	unsigned        m_curBitIndex;
};

void ShiftBits(unsigned char * toBasePtr, unsigned int toBitOffset, unsigned char * fromBasePtr, unsigned int fromBitOffset, unsigned int numBits);

#endif //__BVUTILITY_H__