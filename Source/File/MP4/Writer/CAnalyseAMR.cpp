#include "CAnalyseAMR.h"

CAnalyseAMR::CAnalyseAMR(bool bNB)
: mbNB(bNB)
{
}

CAnalyseAMR::~CAnalyseAMR(void)
{
}

VO_BOOL	CAnalyseAMR::AnalyseData(VO_BYTE *pData , VO_U32 nDataLen)
{
	if(mbNB)
	{
		mWaveFormat.nChannels = 1;
		mWaveFormat.wBitsPerSample = 16;
		mWaveFormat.nSamplesPerSec = 8000;
		mWaveFormat.nBlockAlign = 2;
		mWaveFormat.nAvgBytesPerSec = 16000;
	}
	else
	{
		mWaveFormat.nChannels = 1;
		mWaveFormat.wBitsPerSample = 16;
		mWaveFormat.nSamplesPerSec = 16000;
		mWaveFormat.nBlockAlign = 2;
		mWaveFormat.nAvgBytesPerSec = 32000;

	}

	return VO_TRUE;
}
