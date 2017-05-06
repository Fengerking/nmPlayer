#include "CMp4TrackPushPlay.h"
#include "fCodec.h"
#include "voLog.h"
#include "fCC.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const double TIME_UNIT = 1.0;	//all time multiply this value is MS

CMp4TrackPushPlay::CMp4TrackPushPlay(Track* pTrack, Reader* pReader, VO_MEM_OPERATOR* pMemOp, CMp4Reader2* pMp4Reader)
: CMp4Track2(pTrack, pReader, pMemOp)
, m_LastIndexNo(-1)
, m_pMp4Reader(pMp4Reader)
, m_bEndOfFile(VO_FALSE)
{

}

CMp4TrackPushPlay::~CMp4TrackPushPlay()
{

}

VO_U32 CMp4TrackPushPlay::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	
	VOLOGR("+GetSampleN:time=%lld",pSample->Time);
	VO_U32 ret = VO_ERR_SOURCE_OK;

	if(VOTT_VIDEO == m_nType)	//video track need to process drop frame
	{
		VO_S32 nCurrIndex = -1;
		while(true)
		{
			VOLOGR("m_dwCurrIndex+1=%d:m_LastIndexNo=%d",m_dwCurrIndex+1,m_LastIndexNo);
			if (VO_ERR_SOURCE_OK != (ret = GetInfoByIndex(m_dwCurrIndex+1)))
			{
				return ret;
			}
			
			nCurrIndex = m_pTrack->GetNextSyncPoint(m_dwCurrIndex+1);
			VOLOGR("nCurrIndex=%d",nCurrIndex);

			if (VO_ERR_SOURCE_OK != (ret = GetInfoByIndex(nCurrIndex)))
			{
				if (ret == VO_ERR_SOURCE_END)
				{
					break;
				}
				else
				{
					return ret;
				}
			}
			if(-1 == nCurrIndex || pSample->Time < m_pTrack->GetSampleTime(nCurrIndex) * TIME_UNIT)
				break;
			VOLOGR("TrackID %d,Sample %d, time (%lld %lld)", m_pTrack->GetTrackID(),nCurrIndex, pSample->Time, m_pTrack->GetSampleTime(nCurrIndex) * TIME_UNIT);

			m_dwCurrIndex = nCurrIndex;

			pSample->Flag |= VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED;
		}
	}

	VOLOGR("m_dwCurrIndex=%d:m_dwSampleCount=%d",m_dwCurrIndex,m_dwSampleCount);
	if (VO_ERR_SOURCE_OK != (ret = GetInfoByIndex(m_dwCurrIndex)))
	{
		return ret;
	}
	m_dwSampleCount = m_pTrack->GetSampleCount();
	VOLOGR("m_dwCurrIndex:%d,m_dwSampleCount:%d",m_dwCurrIndex,m_dwSampleCount);
	if(m_dwCurrIndex >= VO_S32(m_dwSampleCount))
		return VO_ERR_SOURCE_END;
	uint32 size = m_pTrack->GetSampleData(m_dwCurrIndex, m_pSampleData, m_dwMaxSampleSize);
	VOLOGR("size=%x",size);
#ifdef _CHECK_READ
	if (size == ERR_CHECK_READ)
#else //_CHECK_READ
	if (size == 0)
#endif //_CHECK_READ
	{
		uint32 err = m_pReader->GetErrorCode();
		m_pReader->ClearErrorCode();
		switch((VO_S32)err)
		{
		case 0:
			VOLOGR("Source End!");
			return VO_ERR_SOURCE_END;

		case MPXERR_DATA_DOWNLOADING:
			return VO_ERR_SOURCE_NEEDRETRY;
		}
	}

	pSample->Buffer = m_pSampleData;

	if(m_nType == VOTT_VIDEO && m_nNALLengthSize >0)
	{

		if(!(m_pSampleData[0] ==0x00 && m_pSampleData[1] == 0x00 &&m_pSampleData[2] ==0x00 && m_pSampleData[3] == 0x01 ))
		{
			VO_U32 codec_cc = 0;
			GetCodecCC(&codec_cc);
			if(!(FOURCC_HVC1 == codec_cc))
			{

				uint32 len =0;
				VO_PBYTE buf = m_pSampleData;

				while(len != size )
				{
					uint32 sSize = 0;
					if( len >size || len + m_nNALLengthSize > size) 
					{
						VOLOGE("ERROR - CORRUPT NAL ");
						size = 0;
						break;
					}
					for(VO_U32 i =0 ;i<m_nNALLengthSize;i++)
						sSize |= buf [i] << (m_nNALLengthSize - 1 - i)* 8;
					len += sSize + m_nNALLengthSize;
					buf += sSize + m_nNALLengthSize;
				}
			}

		}

	}

	pSample->Size = size;
	pSample->Time = m_pTrack->GetSampleTime(m_dwCurrIndex) * TIME_UNIT;
	pSample->Duration = 1;

	if(m_pTrack->GetSampleSync(m_dwCurrIndex))
		pSample->Size |= 0x80000000;


	VOLOGR("Type: %d, [%c] Index: %u, Time: %lld, Size: %d", m_nType, ((pSample->Size & 0x80000000) ? 'I' : ' '), m_dwCurrIndex, pSample->Time, size);
	m_dwCurrIndex++;
	m_qwCurrTime = pSample->Time;
	VOLOGR("-GetSampleN");
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMp4TrackPushPlay::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	voCAutoLock lock(&m_lock);
	VO_U32 ret = VO_ERR_SOURCE_OK;
	if (uID == VO_PID_SOURCE_NEXT_FRAME_INFO)
	{
		if (m_pMp4Reader == NULL || m_pMp4Reader->GetPushReader() == NULL)
		{
			return VO_ERR_SOURCE_END;
		}
		VO_S32 Index = GetCurIndex() + 1;
		ret = m_pTrack->TrackGenerateIndex(Index,m_pMp4Reader->GetPushReader());
		if(ret == VO_ERR_SOURCE_END)
		{
			SetFileEnd(VO_TRUE);
		}
		else if (ret == VO_ERR_SOURCE_OK)
		{
			VO_SOURCE_SAMPLE *pSample = (VO_SOURCE_SAMPLE*)pParam;
			if (pSample != NULL)
			{
				ret = m_pTrack->GetMaxTimeByIndex(Index,pSample->Time);
				pSample->Time *= TIME_UNIT;
			}
			SetCurIndex(Index);
		}	
		else
		{
			VOLOGE("TrackGenerateIndex fail:m_nType=%d,Index=%d",m_nType,Index);
		}
		return ret;
	}
	else if (uID == VO_PID_SOURCE_CURRENT_FILE_INFO)
	{
		VO_U32 ret = VO_ERR_SOURCE_OK;
		if (pParam)
		{
			VO_PUSH_TRACK_INFO *pInfo = (VO_PUSH_TRACK_INFO*)pParam;
			pInfo->EndOfFile = IsFileEnd();
			pInfo->llCurTs = m_pTrack->GetSampleTime(m_dwCurrIndex) * TIME_UNIT;
			ret = m_pTrack->GetMaxTimeByIndex(GetCurIndex(),pInfo->llCurDuration);
			if (ret != VO_ERR_SOURCE_OK)
			{
				ret = VO_ERR_SOURCE_END;
			}
			pInfo->llCurDuration *=  TIME_UNIT;
		}
		return ret;
	}
	return CMp4Track2::GetParameter(uID,pParam);
}

VO_U32 CMp4TrackPushPlay::GetInfoByIndex(VO_U32 uIndex)
{
	voCAutoLock lock(&m_lock);
	m_dwSampleCount = m_pTrack->GetSampleCount();

	if(IsFileEnd() && uIndex >= m_dwSampleCount)
		return VO_ERR_SOURCE_END;

	VO_S32 Index = GetCurIndex();
	if (Index < 0)
	{
		return VO_ERR_SOURCE_NEEDRETRY;
	}
	VO_U32 uLastIndex = Index;
	if (!m_pTrack->IsIndexReady(uIndex,&uLastIndex))
	{
		return VO_ERR_SOURCE_NEEDRETRY;
	}
	return VO_ERR_SOURCE_OK;
}

VO_BOOL CMp4TrackPushPlay::IsFileEnd()
{
	voCAutoLock lock(&m_lock);
	return m_bEndOfFile;
}

VO_VOID CMp4TrackPushPlay::SetFileEnd(VO_BOOL bEnd)
{
	voCAutoLock lock(&m_lock);
	m_bEndOfFile = bEnd;
}

VO_S32 CMp4TrackPushPlay::GetCurIndex()
{
	voCAutoLock lock(&m_lock);
	return m_LastIndexNo;
}

VO_VOID CMp4TrackPushPlay::SetCurIndex(VO_S32 Index)
{
	voCAutoLock lock(&m_lock);
	m_LastIndexNo = Index;
}
VO_U32 CMp4TrackPushPlay::GetCurDuration(VO_S64& llDuration)
{
	voCAutoLock lock(&m_lock);
	if (GetCurIndex() < 0)
	{
		return VO_ERR_SOURCE_NEEDRETRY;
	}
	return m_pTrack->GetMaxTimeByIndex(GetCurIndex(),llDuration);
}