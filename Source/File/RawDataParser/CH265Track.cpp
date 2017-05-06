#include "H265RawData.h"
#include "CH265Track.h"
#include "fCC.h"
#include "fCodec.h"

//CH264Track(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CH264RawData *pReader, VO_MEM_OPERATOR* pMemOp)
CH265Track::CH265Track(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CH265RawData *pReader, VO_MEM_OPERATOR* pMemOp)
 : CBaseTrack(nType, btStreamNum, dwDuration,pMemOp)//CBaseStreamFileTrack(nType, btStreamNum, dwDuration, pReader, pMemOp)
 , m_pH265RawData(pReader)
{
	
}
CH265Track::~CH265Track()
{

}

VO_U32 CH265Track::GetCodecCC(VO_U32* pCC)
{
//	*pCC = FOURCC_h264;
	return m_pH265RawData->GetCodecCC(pCC);
}

VO_U32	CH265Track::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	return m_pH265RawData->GetSample(pSample);
}

VO_U32	CH265Track::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	return m_pH265RawData->GetHeadData(pHeadData);
}
VO_U32	CH265Track::SetPosN(VO_S64* pPos)
{
	return m_pH265RawData->SetPos(pPos);
}