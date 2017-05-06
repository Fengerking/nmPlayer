#include "CAnalyseMPEG4.h"
#include <stdio.h>
#include <string.h>

CAnalyseMPEG4::CAnalyseMPEG4(void)
{
}

CAnalyseMPEG4::~CAnalyseMPEG4(void)
{
}

VO_BOOL	CAnalyseMPEG4::AnalyseData(VO_BYTE *pData , VO_U32 nDataLen)
{
	VO_U32 nSeekLen = 200;
	if(nDataLen < nSeekLen)
	{
		nSeekLen = nDataLen;
	}

	VO_BYTE *pSeqHeader = FindSyncPos(pData , nSeekLen , 0xB0);
	if(pSeqHeader == NULL)
		return VO_FALSE;

	VO_BYTE *pSeqHeaderEnd = FindSyncPos(pData , nDataLen - (pSeqHeader - pData) , 0xB6);
	if(pSeqHeaderEnd == NULL)
		return VO_FALSE;

	mnSeqLen = pSeqHeaderEnd - pSeqHeader;

	if(mpSeqHeader != NULL)
	{
		delete [] mpSeqHeader;
		mpSeqHeader = NULL;
	}

	mpSeqHeader = new VO_BYTE[mnSeqLen];
	if(mpSeqHeader == NULL)
		return VO_FALSE;

	memcpy(mpSeqHeader , pSeqHeader , mnSeqLen);

	return VO_TRUE;
}

VO_BOOL	CAnalyseMPEG4::IsKeyFrame(VO_BYTE *pData , VO_U32 nDataLen)
{
	if(nDataLen < 6)
		return VO_FALSE;

	VO_BYTE *pVS = FindSyncPos(pData , nDataLen , 0xB6);
	if(pVS != NULL)
	{
		if((pVS[4] & 0xC0) == 0)
			return VO_TRUE;
	}

	return VO_FALSE;
}

