#ifndef _LIBADPCM_BIT_H__
#define _LIBADPCM_BIT_H__


#if defined(_MSC_VER)
typedef __int64 int64;
#	elif defined(__GNUC__)
typedef long long int64;
#endif

typedef struct {
  unsigned char   *byte;
  unsigned int   cache;
  int   bitsleft;
  int   nbytes;		
}Bitstream;

void voADPCMDecInitBits(Bitstream* bitptr, unsigned char *ptr, int length);
int  voADPCMDecGetBits(Bitstream *bitptr, int length);
void voADPCMDecAlignBits(Bitstream *bitptr);
int  voADPCMDecSkipBits(Bitstream *bitptr, int nBits);
int	 voADPCMDecCalcBytesUsed(Bitstream *bitptr, unsigned char *buf);

#endif
