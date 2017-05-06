#include "CAnalyseH263.h"

CAnalyseH263::CAnalyseH263(void)
{
}

CAnalyseH263::~CAnalyseH263(void)
{
}

VO_BOOL	CAnalyseH263::AnalyseData(VO_BYTE *pData , VO_U32 nDataLen)
{
	return VO_TRUE;
}

//the first 22 sync bits is 0000 0000 0000 0000 1000 00 then 8Bits TR 
//then PType (the 9th bits show if it is I frame : 0 is I frame , 1 is P frame
VO_BOOL	CAnalyseH263::IsKeyFrame(VO_BYTE *pData , VO_U32 nDataLen)
{
	if(pData[0] == 0 && pData[1] == 0)
	{
		if(((pData[2] | 0xFF) >> 2) == 0x20)
		{
			if((pData[4] & 0x20) == 0)
				return VO_TRUE;
		}
	}

	return VO_FALSE;
}
