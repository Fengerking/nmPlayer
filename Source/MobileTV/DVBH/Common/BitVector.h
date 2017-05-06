#ifndef __BITVECTOR_H__
#define __BITVECTOR_H__

class CBitVector 
{
public:
  CBitVector(unsigned char * baseBytePtr, unsigned baseBitOffset, unsigned totNumBits);

  void Setup(unsigned char * baseBytePtr, unsigned baseBitOffset, unsigned totNumBits);

  void PutBits(unsigned from, unsigned numBits);
  void Put1Bit(unsigned bit);

  unsigned GetBits(unsigned numBits);
  unsigned Get1Bit();

  void SkipBits(unsigned numBits);

  unsigned CurBitIndex() const { return m_curBitIndex; }
  unsigned TotNumBits() const { return m_totNumBits; }
  unsigned NumBitsRemaining() const { return m_totNumBits - m_curBitIndex; } 

private:
  unsigned char * m_baseBytePtr;
  unsigned        m_baseBitOffset;
  unsigned        m_totNumBits;
  unsigned        m_curBitIndex;
};

// A general bit copy operation:
void ShiftBits(unsigned char * toBasePtr, unsigned toBitOffset,
	           unsigned char * fromBasePtr, unsigned fromBitOffset,
	           unsigned numBits);

#endif //__BITVECTOR_H__