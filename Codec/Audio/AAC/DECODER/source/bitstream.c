/************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "bitstream.h"
#include "struct.h"

#define  ALIGNBIT 32
#define  CHECK_OVERFLOW (ALIGNBIT/2+4)
#define  OVERFLOW_FLAG 0xfd

void *voAACDecAlignedMalloc(VO_MEM_OPERATOR *voMemop, int size)
{
	int advance, ret;
	char *realPt, *alignedPt;
	VO_MEM_INFO minfo;
	size += ALIGNBIT+1;//the last byte is used to store 0xfd to check the overflow

	minfo.Flag = 0;
	minfo.Size = size;
	
	ret = voMemop->Alloc(VO_INDEX_DEC_AAC, &minfo);
	if(ret != 0)
		return 0;

	realPt = minfo.VBuffer;
	voMemop->Set(VO_INDEX_DEC_AAC, realPt, 0, size);

	realPt[size-1]=0xfd;
	advance = (ALIGNBIT- ((int)realPt & (ALIGNBIT-1)));
	if(advance<=ALIGNBIT/2)//We just guarantee the ALIGNBIT/2 aligned
		advance +=ALIGNBIT/2;
	alignedPt = realPt + advance; // to aligned location;
	*(alignedPt - 1) = advance; // save real malloc pt at alignedPt[-1] location for free;

	*((int*)(alignedPt-CHECK_OVERFLOW))=size-advance-1;
	return alignedPt;
}

void  voAACDecAlignedFree(VO_MEM_OPERATOR *voMemop, void *alignedPt)
{
	char *realPt;
	int advance;
	if (!alignedPt)
		return;
	advance = *((char *)alignedPt - 1);
	{
		unsigned char* test   = alignedPt;
		int	  offset = *((int*)(test-CHECK_OVERFLOW));
		if(test[offset]!=0xfd)
		{
			//printf("voAACDecAlignedFree overflow error!");
		}
	}
	realPt = (char *)alignedPt - advance ;
	
	voMemop->Free(VO_INDEX_DEC_AAC, realPt);

	return;
}