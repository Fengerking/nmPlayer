#include "Frame.h"

IMAADPCMInfo *voADPCMDecInitIMAInfo(VO_MEM_OPERATOR *pMemOP, int blockalign, int framesperblock)
{
	IMAADPCMInfo *IMAInfo; 
	IMAInfo = (IMAADPCMInfo *)voADPCMDecmem_malloc(pMemOP, sizeof(IMAADPCMInfo), 32);
	if(IMAInfo == NULL)
		return NULL;
	
	return IMAInfo;
}

static int voIMA_Adjust [16] =
{	
	-1, -1, -1, -1,		
	2,  4,  6,  8,     
	-1, -1, -1, -1,		
	2,  4,  6,  8,		
};

int voADPCMDecIMAADPCMDec(ADPCMDecInfo *decoder, short* pSamp)
{
	int		nChs, k, nTemp;
	int		blockindx, indx, indxstart, diff ;
	short	step, nBC, nStepIndx[2];
	int		channels, blocksize;
	unsigned char *buf;

	buf = decoder->stream->this_frame;
	channels = decoder->channel;
	blocksize = decoder->blockalign;

	for (nChs = 0 ; nChs < channels ; nChs++)
	{	
		nTemp = buf[nChs*4] | (buf[nChs*4+1] << 8) ;
		if (nTemp & 0x8000)
			nTemp -= 0x10000 ;

		nStepIndx [nChs] = buf[nChs*4+2] ;
		if (nStepIndx [nChs] < 0)
			nStepIndx [nChs] = 0 ;
		else if (nStepIndx [nChs] > 88)
			nStepIndx [nChs] = 88;
		
		pSamp[nChs] = nTemp;
	}

	blockindx = 4 * channels ;

	indxstart = channels ;
	while(blockindx < blocksize)
	{	
		for (nChs = 0 ; nChs < channels ; nChs++)
		{	
			indx = indxstart + nChs ;
			for (k = 0 ; k < 4 ; k++)
			{	
				nBC = buf[blockindx++] ;
				pSamp [indx] = nBC & 0x0F ;
				indx += channels ;
				pSamp [indx] = (nBC >> 4) & 0x0F ;
				indx += channels ;
			}
		} 
		indxstart += 8 * channels ;
	} 

	for (k = channels ; k < (decoder->framesperblock * channels) ; k ++)
	{	
		nChs = (channels > 1) ? (k % 2) : 0 ;
		nBC = pSamp [k] & 0xF ;
		step = step_table [nStepIndx [nChs]] ;
		nTemp = pSamp [k - channels] ;

		diff = step >> 3 ;

		if (nBC & 1)
			diff += step >> 2 ;
		if (nBC & 2)
			diff += step >> 1 ;
		if (nBC & 4)
			diff += step ;
		if (nBC & 8)
			diff = -diff ;

		nTemp += diff ;

		if (nTemp > 32767)
			nTemp = 32767 ;
		else if (nTemp < -32768)
			nTemp = -32768 ;

		nStepIndx [nChs] += voIMA_Adjust [nBC] ;

		if (nStepIndx [nChs] < 0)
			nStepIndx [nChs] = 0 ;
		else if (nStepIndx [nChs] > 88)
			nStepIndx [nChs] = 88 ;

		pSamp [k] = nTemp ;
	} 

	return 0;
}