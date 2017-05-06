#include "CAnalysePCM.h"
#include "fMacros.h"
#include "CDumper.h"
#include "voSink.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CAnalysePCM::CAnalysePCM()
{
}

CAnalysePCM::~CAnalysePCM()
{
}

VO_BOOL	CAnalysePCM::AnalyseData(VO_BYTE *pData , VO_U32 nDataLen)
{
	if (!pData || nDataLen < sizeof(VO_SINK_MEDIA_INFO))
	{
		VOLOGE("TS Muxer Error:Wrong pExtData Data");
		return VO_FALSE;
	}
	VO_SINK_MEDIA_INFO pPCMInfo;
	memcpy(&pPCMInfo,pData,nDataLen);
	memset(pData,0,nDataLen);
	
	VO_U16 nSizeInByte = pPCMInfo.AudioFormat.nSample_rate/100 * pPCMInfo.AudioFormat.nSample_bits /8;
	pData[0] = nSizeInByte >> 8;
	pData[1] = (VO_BYTE)nSizeInByte;

	VO_U8 Index = 0;
	switch (pPCMInfo.AudioFormat.nChannels)
	{
	case 0:
		{
			Index = 0;
		}
		break;
	case 1:
		{
			Index = 1;
		}
		break;
	case 2:
		{
			Index = 3;
		}
		break;
	case 3:
		{
			Index = 4;
		}
		break;
	case 4:
		{
			Index = 6;
		}
		break;
	case 5:
		{
			Index = 8;
		}
		break;
	case 6:
		{
			Index = 9;
		}
		break;
	case 7:
		{
			Index = 10;
		}
		break;
	case 8:
		{
			Index = 11;
		}
		break;
	}
	pData[2] = (Index << 4)&0xF0;

	Index = 0;
	switch (pPCMInfo.AudioFormat.nSample_rate)
	{
	case 48000:
		{
			Index = 1;
		}
		break;
	case 96000:
		{
			Index = 4;
		}
		break;
	case 192000:
		{
			Index = 5;
		}
		break;
	default:
		{

		}
		break;
	}
	pData[2] |= (Index & 0x0F);

	Index = 0;
	switch (pPCMInfo.AudioFormat.nSample_bits)
	{
	case 16:
		{
			Index = 1;
		}
		break;
	case 20:
		{
			Index = 2;
		}
		break;
	case 24:
		{
			Index = 3;
		}
		break;
	default:
		break;
	}
	pData[3] = ((Index << 6) & 0xC0);

	pData[3] |= ((1 << 5) & 0x20);

	if (mpSeqHeader)
	{
		delete mpSeqHeader;
		mpSeqHeader = VO_NULL;
	}

	mnSeqLen = 4;
	mpSeqHeader = new VO_BYTE[mnSeqLen];
	memcpy(mpSeqHeader,pData,4);

	return VO_TRUE;
}
