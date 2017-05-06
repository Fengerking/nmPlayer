#include "CAnalyseAAC.h"

CAnalyseAAC::CAnalyseAAC(void)
{
}

CAnalyseAAC::~CAnalyseAAC(void)
{
}

VO_BOOL	CAnalyseAAC::AnalyseData(VO_BYTE *pData , VO_U32 nDataLen)
{
	if(pData[0] == 0xFF && (pData[1] & 0xF0)  == 0xF0)
	{
		const int sample_rates[] =
		{
			96000, 88200, 64000, 48000, 
			44100, 32000,24000, 22050, 
			16000, 12000, 11025, 8000,
			0, 0, 0, 0
		};

		unsigned char sampleIndex = pData[2];
		sampleIndex >>= 2;
		sampleIndex &= 0x0F;

		mWaveFormat.nSamplesPerSec = sample_rates[sampleIndex];
		mWaveFormat.wBitsPerSample = 16;

		unsigned char nChannels = pData[3];
		nChannels >>= 6;
		mWaveFormat.nChannels = nChannels;
		nChannels = pData[2];
		if((nChannels >> 7) > 0)
			mWaveFormat.nChannels += 4;

	}

	return VO_TRUE;
}

VO_U32 CAnalyseAAC::TrimSample(VO_BYTE **ppData , VO_U32 &nDataLen)
{
	VO_PBYTE pSrc = *ppData;

	if(pSrc[0] == 0xFF && (pSrc[1] & 0xF0) == 0xF0)
	{
		if((pSrc[1] & 0x1) > 0)
		{
			*ppData = pSrc + 7;
			nDataLen -= 7;
		}
		else
		{
			*ppData = pSrc + 9;
			nDataLen -= 9;
		}
		
	}

	return 0;
}
