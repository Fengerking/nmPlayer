
#include "BitVector.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define VOI_BYTESWAP(a) ((a<<24) | ((a<<8)&0x00ff0000) | ((a>>8)&0x0000ff00) | (a>>24));
static const unsigned long LsbOnes[32] = {0x0, 0x1, 0x3, 0x7, 
0xf, 0x1f, 0x3f, 0x7f,
0xff, 0x1ff, 0x3ff, 0x7ff,
0xfff, 0x1fff, 0x3fff, 0x7fff,
0xffff, 0x1ffff, 0x3ffff, 0x7ffff,
0xfffff, 0x1fffff, 0x3fffff, 0x7fffff,
0xffffff, 0x1ffffff, 0x3ffffff, 0x7ffffff,
0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff};


void VORTSPInitBitStream(VORTSPBitstream *str, unsigned char *buf, int len)
{
	unsigned long tmp;
	int align4, i;
	str->streamBuffer = buf;
	str->code_len = len;
	align4 = (int)(4 - ((unsigned long)buf & 3));

	// make sure the memory is 4 aligned after first read;
	str->bufferA = 0;
	for (i = 0; i < align4; i++){
		str->bufferA <<= 8;
		str->bufferA |= buf[i];
	}

	buf += align4;
	tmp = *(unsigned long*)buf;
	str->bufferB = VOI_BYTESWAP(tmp);
	str->currReadBuf = buf + 4;

	str->bBitToGo = (align4<<3);


}
void VORTSPFlushBits(VORTSPBitstream *bStr,int numBits)
{
	bStr->bBitToGo -= numBits;
	if (bStr->bBitToGo < 0){
		unsigned long tmp = *(int*)bStr->currReadBuf;
		bStr->bufferA = bStr->bufferB;
		bStr->bufferB = VOI_BYTESWAP(tmp);
		bStr->currReadBuf += 4;
		bStr->bBitToGo += 32;
	}
}

unsigned long VORTSPShowBits(VORTSPBitstream *bStr, int numBits)
{
	int extraBits;
	const unsigned long lsbOnes = LsbOnes[numBits];
	extraBits = bStr->bBitToGo - numBits;
	if (extraBits >= 0){
		return (bStr->bufferA>>(extraBits))&lsbOnes;
	}

	return ((bStr->bufferA<<(-extraBits)) | (bStr->bufferB>>(32+extraBits)))
		&lsbOnes;
}

unsigned long VORTSPGetBits(VORTSPBitstream *bStr, int numBits)
{
	// Show bits plus Flush bits;
	unsigned long tmp, value;
	const unsigned long lsbOnes = LsbOnes[numBits];
	bStr->bBitToGo -= numBits;
	if (bStr->bBitToGo >= 0){
		return (bStr->bufferA>>bStr->bBitToGo)&lsbOnes;
	}

	bStr->bBitToGo += 32;
	value = ((bStr->bufferA<<(32 - bStr->bBitToGo)) | (bStr->bufferB>>bStr->bBitToGo))
		&lsbOnes;
	tmp = *(int*)bStr->currReadBuf;
	bStr->bufferA = bStr->bufferB;
	bStr->bufferB = VOI_BYTESWAP(tmp);
	bStr->currReadBuf += 4;
	return value;
}

#ifdef _VONAMESPACE
}
#endif