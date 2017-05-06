#include "CAnalyseAAC.h"
#include "adtshead.h"
#include "LatmParser.h"
#include "fMacros.h"
#include "CDumper.h"

CAnalyseAAC::CAnalyseAAC(void)
{
}

CAnalyseAAC::~CAnalyseAAC(void)
{
}

VO_BOOL	CAnalyseAAC::AnalyseData(VO_BYTE *pData , VO_U32 nDataLen)
{
	ADTSHeader adts;
	bool ret = adts.Parse(pData);
	if (ret)
	{
		// is adts AAC
		CDumper::WriteLog((char*)"Analyse ADTS AAC head data...");

		if (mpSeqHeader)
		{
			delete mpSeqHeader;
			mpSeqHeader = VO_NULL;
		}
		
		mnSeqLen = 2;
		mpSeqHeader = new VO_BYTE[mnSeqLen];
		adts.ToDSI(mpSeqHeader);
	}
	else// check LATM
	{
		PatternFinder finder(0x56e0, 0xffe0); //high 11 bits == 0x2b7
		uint8* pEnd = pData + nDataLen;
		uint8* p1 = finder.Find(pData, pEnd);
		if (p1 == VO_NULL)
			return VO_FALSE;

		p1 -= 2;

		CLATMParser latm;
		int r = latm.Parse(p1, nDataLen-(p1-pData));

		if (r <= 0)
		{
			ret = true;
			CDumper::WriteLog((char *)"Analyse LATM AAC head data...");
			int n = 0;
			n = latm.GetChannelCount();
			n = latm.GetSampleRate();
			n = 0;
		}
	}

	if (!ret)
	{
	}

	return ret?VO_TRUE:VO_FALSE;
}
