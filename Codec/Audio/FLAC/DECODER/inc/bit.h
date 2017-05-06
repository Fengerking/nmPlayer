#ifndef _LIBFLAC_BIT_H__
#define _LIBFLAC_BIT_H__


#ifdef LINUX 
typedef	long long int64;
typedef	unsigned long long uint64;
#elif _IOS
typedef	long long int64;
typedef	unsigned long long uint64;
#else
typedef __int64	int64;
typedef unsigned __int64 uint64;
#endif

typedef struct {
  unsigned char   *byte;
  unsigned int   cache;
  int   bitsleft;
  int   nbytes;		
}Bitstream;

void voFLACDecInitBits(Bitstream* bitptr, unsigned char *ptr, int length);
int  voFLACDecGetBits(Bitstream *bitptr, int length);
int  voFLACDecGetsBits(Bitstream *bitptr, int nBits);
void  voFLACDecAlignBits(Bitstream *bitptr);
int64 voFLACDecGetUtf8(Bitstream *bitptr);
int  voFLACDecGetUnaryBits(Bitstream *bitptr);
int  voFLACDecGetRiceBits(Bitstream *bitptr, int bits);
int  voFLACDecSkipBits(Bitstream *bitptr, int nBits);
int  voFLACDecCalcBytesUsed(Bitstream *bitptr, unsigned char *buf);

#endif
