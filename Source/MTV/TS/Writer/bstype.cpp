#include "bstype.h"
#include "stdlib.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

BitsStream::BitsStream(uint8* p)
		: pbyte(p)
		, bitpos(0)
		, bitsbuf(0)
		, test_byte(p)
{
	if (p)
	{
		bitsbuf = *p;
	}
}

void BitsStream::Init(uint8* p)
{
	pbyte	= p;
	bitpos	= 0;
	if (p)
	{
		bitsbuf = *p;
	}

	test_byte = p;
}

uint8* BitsStream::Position()
{
	if (bitpos != 0)
	{
		VOLOGI("BitsStream::Position bitpos != 0");
	}
	return pbyte;
}

void BitsStream::SetPosition(uint8* pos)
{
	pbyte = pos;
	bitpos = 0;
	bitsbuf = *pos;
}

void BitsStream::SkipBits(int width)
{
	bitpos += width % 8;
	if (bitpos >= 8)
	{
		*pbyte = bitsbuf;
		bitpos -= 8;

		pbyte++;
		bitsbuf = *pbyte;
	}

	uint8 BYTE_count = width / 8;

	if(BYTE_count > 0)
	{
		pbyte	+= BYTE_count;
		bitsbuf = *pbyte;
	}
}

// size in byte
int BitsStream::WriteByte(int size, uint8* pByte)
{
	if (bitpos != 0)
	{
		VOLOGI("BitsStream::WriteByte bitpos != 0");
	}
	memcpy (pbyte, pByte, size);
	//cmnMemCopy(0, pbyte, pByte, size);
	pbyte += size;

	int n = pbyte - test_byte;
	if (n > 188)
	{
		VOLOGI("BitsStream::WriteByte n > 188");
	}
	return size*8;
}

// 	void SkipBytes(int size)
// 	{
// 		pbyte += size;
// 	}


// Write OP
int BitsStream::WriteBits(int width, uint64 val)
{
	uint64 mask = 1LL << (width - 1);
	while (mask)
	{
		bitsbuf <<= 1;
		if (val & mask) 
			bitsbuf |= 1;
		else
			bitsbuf |= 0;

		++bitpos;
		if (bitpos == 8)
		{	
			*pbyte = bitsbuf;

			pbyte++;
			bitsbuf = *pbyte;
			bitpos = 0;
		}
		mask >>= 1;
	}

	int n = pbyte - test_byte;
	if (n > 188)
	{
		VOLOGI("BitsStream::WriteBits n > 188");
	}
	return width;
}



void BitsStream::Flush()
{
// 		if(bitpos == 0)
// 			return;
	*pbyte = bitsbuf;
}
