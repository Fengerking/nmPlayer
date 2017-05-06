#include "H264RawData.h"
#include "CH264Track.h"
#include "fCC.h"
#include "fCodec.h"

//CH264Track(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CH264RawData *pReader, VO_MEM_OPERATOR* pMemOp)
CH264Track::CH264Track(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CH264RawData *pReader, VO_MEM_OPERATOR* pMemOp)
 : CBaseTrack(nType, btStreamNum, dwDuration,pMemOp)//CBaseStreamFileTrack(nType, btStreamNum, dwDuration, pReader, pMemOp)
 , m_pH264RawData(pReader)
{
	
}
CH264Track::~CH264Track()
{

}

VO_U32 CH264Track::GetCodecCC(VO_U32* pCC)
{
	//*pCC = FOURCC_h264;
	return m_pH264RawData->GetCodecCC(pCC);
}

VO_U32	CH264Track::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 nRC = m_pH264RawData->GetSample(pSample);
	return nRC;
}

VO_U32	CH264Track::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	return m_pH264RawData->GetHeadData(pHeadData);
}
VO_U32	CH264Track::SetPosN(VO_S64* pPos)
{
	return m_pH264RawData->SetPos(pPos);
	
}