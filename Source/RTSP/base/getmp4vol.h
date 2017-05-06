#ifndef __GETMP4VOL_H
#define __GETMP4VOL_H

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif

typedef struct bitstream
{
	int bits;
	int bitpos;
	const unsigned char *bitptr;
	const unsigned char *bitend;
	
} bitstream;

static __inline int _log2(unsigned int a)
{
	int i;
	if (!a) ++a;
	for (i=0;a;++i)
		a >>= 1;
    return i;
}

void initbits(bitstream* bs,const unsigned char *ptr, int len);

void loadbits( bitstream * bs );


#define showbits(pbs,n) ((unsigned int)(pbs->bits << pbs->bitpos) >> (32-(n)))

#define flushbits(pbs,n) pbs->bitpos += n;


static __inline int getbits(bitstream* pbs,int n)
{
	int i = showbits(pbs,n);
	flushbits(pbs,n);
	return i;
}



int GetMpeg4SequenceHr(const unsigned char *ptr, int len, int * width, int *height);


#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif


#endif