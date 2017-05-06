#include "CMp4Track.h"
#include "fMacros.h"
#include "fCC.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
CMp4Track::CMp4Track(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CMp4Reader* pReader, VO_MEM_OPERATOR* pMemOp)
	: CBaseTrack(nType, btStreamNum, dwDuration, pMemOp)
	, m_pReader(pReader)
	, m_pHeadData(VO_NULL)
	, m_dwHeadData(0)
	, m_dwCurrIndex(0)
	, m_dwMaxSampleSize(0)
{
}

CMp4Track::~CMp4Track()
{
}

VO_U32 CMp4Track::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CMp4Track::SetPos(VO_S64* pPos)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CMp4Track::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = m_dwHeadData;
	pHeadData->Buffer = m_pHeadData;

	return VO_ERR_SOURCE_OK;
}

VO_BOOL CMp4Track::Init(VO_PTR pUserData)
{
	return VO_FALSE;
}

VO_U32 CMp4Track::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	if(pdwMaxSampleSize)
		*pdwMaxSampleSize = m_dwMaxSampleSize;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4Track::GetCodecCC(VO_U32* pCC)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CMp4Track::GetNextKeyFrame(VO_SOURCE_SAMPLE* pSample)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CMp4Track::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CMp4Track::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CMp4Track::GetBitrate(VO_U32* pdwBitrate)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CMp4Track::GetMediaTimeByFilePos(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CMp4Track::GetFilePosByMediaTime(VO_FILE_MEDIATIMEFILEPOS* pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}