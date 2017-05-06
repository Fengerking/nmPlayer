
#include "memory.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#ifdef LINUX
#include <stdlib.h>
#endif

#include "cConstructCodecInfo.h"

void WriterBufSize(unsigned char *pBuf , int nSiz)
{
	pBuf[0] = (unsigned char) (((nSiz & 0x0fe00000) >> 21) | 0x80);
	pBuf[1] = (unsigned char) (((nSiz & 0x0001fc00) >> 14) | 0x80);
	pBuf[2] = (unsigned char) (((nSiz & 0x00003f80) >>  7) | 0x80);
	pBuf[3] = (unsigned char) (nSiz & 0x7f);
}

bool ConstructESDS(unsigned char *pInput , int nInputSize , unsigned char *pOutput , int &nOutputSize)
{
	if(pOutput == NULL)
		return false;

	memset(pOutput , 0 , nOutputSize);

	unsigned char *pBuf = pOutput;
	int  nOutSize = 0;
	//ESDescriptor head
	
	pBuf[0] = 0x03;
	pBuf[5] = 0x0;
	pBuf[6] = 0x1;
	pBuf[7] = 0x10;

	pBuf += 8;
	nOutSize += 8;

	//DecoderConfigDescr
	pBuf[0] = 0x4;
	WriterBufSize(pBuf + 1 , nInputSize + 18);
	pBuf[5] = 0x20; //objectTypeIndication
	pBuf[6] = 0x11;

	pBuf += 18;
	nOutSize += 18;

	//DecoderSpecificInfo
	pBuf[0] = 0x5;
	WriterBufSize(pBuf + 1 , nInputSize);
	pBuf +=5;
	nOutSize += 5;

	if(nInputSize > 0)
	{
		memcpy(pBuf , pInput , nInputSize);
		pBuf += nInputSize;
		nOutSize += nInputSize;
	}

	//SLConfigDescriptor
	pBuf[0] = 0x6;
	WriterBufSize(pBuf + 1 , 1);
	pBuf[5] = 0x2;

	nOutSize += 6;

	WriterBufSize(pOutput + 1 , nOutSize - 5);

	nOutputSize = nOutSize;
	return true;
}

