#ifndef __BITVECTOR_H__
#define __BITVECTOR_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef struct
{
	unsigned char		*streamBuffer;      //!< actual codebuffer for read bytes
	unsigned char		*currReadBuf;			// read position;
	unsigned long		bufferA;			// 32 bits buffer for quick ShowBits and GetBits;
	unsigned long		bufferB;			// 32 bits buffer for quick ShowBits and GetBits;
	int	bBitToGo;
	int code_len;
} VORTSPBitstream;
void VORTSPInitBitStream(VORTSPBitstream *str, unsigned char *buf, int len);
void VORTSPFlushBits(VORTSPBitstream *bStr, int numBits);
unsigned long VORTSPShowBits(VORTSPBitstream *bStr, int numBits);
unsigned long VORTSPGetBits(VORTSPBitstream *bStr, int numBits);

#ifdef _VONAMESPACE
}
#endif

#endif //__BITVECTOR_H__