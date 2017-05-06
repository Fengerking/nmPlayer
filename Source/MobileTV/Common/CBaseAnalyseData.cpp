#include "CBaseAnalyseData.h"

CBaseAnalyseData::CBaseAnalyseData(void)
: mnSyncLen(0)
, mpSeqHeader(NULL)
, mnSeqLen(0)
{
}

CBaseAnalyseData::~CBaseAnalyseData(void)
{
	if(mpSeqHeader != NULL)
	{
		delete [] mpSeqHeader;
		mpSeqHeader = NULL;
	}
}

VO_WAVEFORMATEX *CBaseAnalyseData::GetAudioInfo()
{
	return &mWaveFormat;
}

VO_VIDEOINFOHEADER *CBaseAnalyseData::GetVideoInfo()
{
	return &mVideoInfoHeader;
}

VO_BYTE *CBaseAnalyseData::GetSequenceHeader(VO_U32 &nDataLen)
{
	nDataLen = mnSeqLen;

	return mpSeqHeader;
}

VO_BYTE *CBaseAnalyseData::FindSyncPos(VO_BYTE *pData , VO_U32 nDataLen)
{
	VO_BYTE *pTemp = pData;
	VO_BYTE *pStopP = pData + nDataLen - 4;

	while (pTemp < pStopP)
	{
		if(pTemp[0] == 0 && pTemp[1] == 0 )
		{	
			if(pTemp[2] == 0 && pTemp[3] == 1)
			{
				mnSyncLen = 4;
				return pTemp;
			}
			else if(pTemp[2] == 1)
			{
				mnSyncLen = 3;
				return pTemp;
			}
		}

		pTemp += 1;
	}

	return NULL;
}

VO_BYTE *CBaseAnalyseData::FindSyncPos(VO_BYTE *pData , VO_U32 nDataLen , VO_BYTE nMarkByte)
{
	VO_BYTE *pTemp = pData;
	VO_BYTE *pStopP = pData + nDataLen - 4;

	while (pTemp < pStopP)
	{
		if(pTemp[0] == 0 && pTemp[1] == 0 )
		{	
			if(pTemp[4] == nMarkByte && pTemp[2] == 0 && pTemp[3] == 1)
			{
				mnSyncLen = 4;
				return pTemp;
			}
			else if(pTemp[3] == nMarkByte && pTemp[2] == 1)
			{
				mnSyncLen = 3;
				return pTemp;
			}
		}

		pTemp += 1;
	}

	return NULL;
}