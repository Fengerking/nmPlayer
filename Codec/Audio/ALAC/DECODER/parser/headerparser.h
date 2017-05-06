#include "voALAC.h"
#include "voalacdec.h"

typedef struct HeadParse{
	VO_WAVEFORMATEX	*ALACFormat;

	struct {
		VO_U32	m_SampleCount;
		VO_U32	m_SampleDuration;
	}*m_pTime2Sample;
	VO_U32		m_NumTime2Sample;

	VO_U32		*m_pSampleByteSize;
	VO_U32		m_NumSampleByteSize;
}HeadParse;

VO_U32 ParseHeaderMoov(CALACObjectDecoder *palacdec, HeadParse *headparse, VO_U8 *pMoov, VO_U32 moovlen);