#include "CMemoryOpt.h"
#include "windows.h"


CMemoryOpt::CMemoryOpt(VO_MEM_OPERATOR *pMemOpt , VO_S32 uID)
: mpMemOpt(pMemOpt)
, mnID(uID)
{
}

CMemoryOpt::~CMemoryOpt(void)
{
}

void * CMemoryOpt::Alloc(VO_U32 nSize)
{
	if(mpMemOpt != NULL)
	{
		VO_MEM_INFO memInfo;
		memInfo.Size = nSize;

		VO_U32 nRSize = mpMemOpt->Alloc(mnID , &memInfo);
		if(nRSize == 0)
		{
			return memInfo.VBuffer;
		}
	
		return NULL;
	}

	return malloc(nSize);
}

VO_U32 CMemoryOpt::Free(VO_PTR pBuff)
{
	if(mpMemOpt != NULL)
	{
		return mpMemOpt->Free(mnID , pBuff);
	}
	free(pBuff);
	return 0;
}

VO_U32 CMemoryOpt::Set (VO_PTR pBuff, VO_U8 uValue, VO_U32 uSize)
{
	if(mpMemOpt != NULL)
	{
		return mpMemOpt->Set(mnID , pBuff , uValue , uSize);
	}

	memset (pBuff, uValue, uSize);
	return 0;
}

VO_U32 CMemoryOpt::Copy(VO_PTR pDest, VO_PTR pSource, VO_U32 uSize)
{
	if(mpMemOpt != NULL)
	{
		return mpMemOpt->Copy(mnID , pDest , pSource , uSize);
	}

	memcpy (pDest, pSource, uSize);
	return 0;
}

VO_U32 CMemoryOpt::Check(VO_PTR pBuffer, VO_U32 uSize)
{
	if(mpMemOpt != NULL)
	{
		return mpMemOpt->Check(mnID , pBuffer , uSize);
	}

	return 0;
}

VO_S32 CMemoryOpt::Compare(VO_PTR pBuffer1, VO_PTR pBuffer2, VO_U32 uSize)
{
	if(mpMemOpt != NULL)
	{
		return mpMemOpt->Compare(mnID , pBuffer1 , pBuffer2 , uSize);
	}

	return memcmp(pBuffer1, pBuffer2, uSize);
}

VO_U32 CMemoryOpt::Move(VO_PTR pDest, VO_PTR pSource, VO_U32 uSize)
{
	if(mpMemOpt != NULL)
	{
		return mpMemOpt->Move(mnID , pDest , pSource , uSize);
	}

	memmove (pDest, pSource, uSize);
	return 0;
}
