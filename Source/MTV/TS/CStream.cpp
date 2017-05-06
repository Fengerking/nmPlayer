#include "CStream.h"
#include "cmnMemory.h"
#include "CDumper.h"
#include "voLog.h"
#include "CBaseTrack.h"



#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CStream::CStream(VOSTREAMPARSESTREAMINFO* pInfo)
: CStreamFormat(pInfo), CvoBaseMemOpr(VO_NULL)
, m_pFrameBuf(VO_NULL)
, m_nFrameBufLen(0)
, m_nCurrBufPos(0)
, m_nStreamID(pInfo->id)
, m_nTimeStamp(-1)
#ifdef USE_CACHE
, m_nCurrCachePos(0)
, m_nCacheSize(MAX_CACHE_SIZE)
#endif
{
	memset(m_chLanguage, 0, 16);
	if(pInfo != NULL)
    {
		if(pInfo->is_video == false && pInfo->codec != VO_AUDIO_Coding_MAX && pInfo->eStreamMediaType == VO_TS_STREAM_MEDIA_TYPE_AUDIO)
        {
            memcpy(m_chLanguage, pInfo->audio.audio_language, strlen(pInfo->audio.audio_language));
        }
		if(pInfo->eStreamMediaType == VO_TS_STREAM_MEDIA_TYPE_SUBTITLE)
		{
            memcpy(m_chLanguage, pInfo->subtitle.subtitle_language, strlen(pInfo->subtitle.subtitle_language));
		}
    }
#ifdef USE_CACHE
	m_pFrameBuf = (VO_BYTE*)MemAlloc(m_nCacheSize);
#else
	m_pFrameBuf = (VO_BYTE*)MemAlloc(GetMaxFrameSize());
#endif
	//end
}

CStream::~CStream(void)
{
	if (m_pFrameBuf)
	{
		MemFree(m_pFrameBuf);
		m_pFrameBuf = VO_NULL;
	}
}

VO_VOID CStream::Reset()
{
	// add by Lin Jun
#ifdef USE_CACHE
	m_nCurrCachePos += m_nCurrBufPos;
	if(m_nCurrBufPos > m_nCacheSize)
		m_nCurrCachePos = 0;
#endif
	//end

	m_nFrameBufLen	= 0;
	m_nCurrBufPos	= 0;
	m_nTimeStamp	= -1;

	// 20100617 test code
	//MemSet(m_pFrameBuf, 0, GetMaxFrameSize());
}


VO_VOID CStream::OnNewFrame(VOSTREAMPARSEFRAMEINFO* pFrameInfo)
{
	if (pFrameInfo->timestamp.mul != 0xFFFFFFFFFFFFFFFFLL)
	{
		m_nFrameBufLen	= pFrameInfo->length;
		m_nTimeStamp	= (pFrameInfo->timestamp.mul*1000) / pFrameInfo->timestamp.div;

#ifdef USE_CACHE
		if(m_nCurrBufPos+pFrameInfo->length > m_nCacheSize)
			m_nCurrCachePos = 0;
#endif

#if 1 // test code
		if (IsVideo())
		{
#ifdef _PRINT_ORIGINAL_VIDEO_TIME_STAMP_
			CDumper::WriteLog("Video(%d) Frame, TS    = %llu", m_nStreamID, m_nTimeStamp);
#endif
		}
		else
		{
#ifdef _PRINT_ORIGINAL_AUDIO_TIME_STAMP_
			CDumper::WriteLog("Audio(%d) Frame, TS    = %llu", m_nStreamID, m_nTimeStamp);
#endif
		}
#endif
	}
	else
	{
		m_nTimeStamp = 0xFFFFFFFFFFFFFFFELL;
	}
	
}

VO_VOID CStream::OnFrameData(void* pData, int nSize)
{
	if (-1 == m_nTimeStamp)
		return;

	if ((m_nCurrBufPos + (uint32)nSize) > (uint32)m_nMaxFrameSize)
	{
		// discard this frame
		CDumper::WriteLog((char *)"discard frame by out of max frame size...");
		Reset();

		return;
	}

#ifdef USE_CACHE
	MemCopy(m_pFrameBuf+m_nCurrCachePos+m_nCurrBufPos, pData, nSize);
#else
	MemCopy(m_pFrameBuf+m_nCurrBufPos, pData, nSize);
#endif
	m_nCurrBufPos	+= nSize;
}

VO_BOOL CStream::OnFrameEnd(int* sizes, int* count, VO_BYTE** pOutBuf, VO_U64* pTimeStamp)
{

	if (m_nTimeStamp == -1)
		return VO_FALSE;

	bool bValid = true;

	//check frame size
	if (m_nFrameBufLen)
		bValid = m_nCurrBufPos == m_nFrameBufLen;

	VO_BOOL bRet = VO_TRUE;
	int split = 0;

#ifdef USE_CACHE
	uint8* pStart = m_pFrameBuf+m_nCurrCachePos;
#else
	uint8* pStart = m_pFrameBuf;
#endif
	if (!m_bVideo && VO_AUDIO_CodingPCM == GetCodec())
	{
		switch(m_audio.sample_bits)
		{
		case 16:
			{
				uint16* temp = (uint16*)pStart;
				uint32 size = m_nCurrBufPos/2;
				for (VO_U32 nCnt = 0;nCnt < size;nCnt++)
				{
					temp[nCnt] = (temp[nCnt] >> 8)|(temp[nCnt]<<8);
				}
			}
			break;
		case 20:
			break;
		case 24:
			break;
		default:
			break;
		}
	}

	if (bValid && m_pFrameChecker)
	{
		split = m_pFrameChecker->Split(pStart, m_nCurrBufPos, sizes, pStart);
		if (split <= 0)
			bValid = false;
	}
	else
	{
		if(bValid && (NULL == m_pFrameChecker))
		{
			//the whole pes is a frame, need no m_pFrameChecker
			split = 1;
		}
	}

	if (bValid)
	{
#ifdef USE_CACHE
		if ( (split == 1) && (pStart == m_pFrameBuf+m_nCurrCachePos) )
#else
		if ( (split == 1) && (pStart == m_pFrameBuf) )
#endif
		{
			//DeliverSample();
			sizes[0]	= m_nCurrBufPos;
			*count		= 1;
			*pTimeStamp	= m_nTimeStamp;
#ifdef USE_CACHE
			*pOutBuf	= m_pFrameBuf+m_nCurrCachePos;
#else
			*pOutBuf	= m_pFrameBuf;
#endif

		}
		else
		{
			*count		= split;
			*pTimeStamp	= m_nTimeStamp;
			*pOutBuf	= pStart;
			//DeliverMultiSamples(sizes, split, pStart);
		}
		
		bRet		= VO_TRUE;
	}
	else
	{
		bRet = VO_FALSE;
		//DiscardSample();
	}

	// 20100617
	Reset();

	return bRet;
}

VO_BOOL	CStream::IsSync(VO_BYTE* pFrame, VO_U32 nFrameLen)
{
	VO_BOOL bSync = VO_FALSE;

	if (m_pFrameChecker)
	{
		bSync = m_pFrameChecker->IsSync(pFrame, nFrameLen)?VO_TRUE:VO_FALSE;
	}

	return bSync;
}

VO_U32	CStream::GetTrackType()
{
    switch(m_eStreamMediaType)
    {
	    case VO_TS_STREAM_MEDIA_TYPE_VIDEO:
		{
			return (VO_U32)VOTT_VIDEO;
		}
	    case VO_TS_STREAM_MEDIA_TYPE_AUDIO:
		{
			return (VO_U32)VOTT_AUDIO;
		}
	    case VO_TS_STREAM_MEDIA_TYPE_SUBTITLE:
        {
			return (VO_U32)VOTT_TEXT;
		}
	    case VO_TS_STREAM_MEDIA_TYPE_PRIVATE:
		{
			return (VO_U32)VOTT_METADATA;
		}
	}

	return VOTT_MAX;
}
