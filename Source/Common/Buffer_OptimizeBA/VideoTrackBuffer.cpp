#include "VideoTrackBuffer.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

VideoTrackBuffer::VideoTrackBuffer(VO_U32 uMaxSize)
: TrackBuffer(uMaxSize)
, m_uflag(0)
, m_uFrameInterval(35)//33.367
, m_ullLatestPrunePoint(0)
{
}

VideoTrackBuffer::~VideoTrackBuffer(void)
{
}

VO_U32 VideoTrackBuffer::GetSample(_SAMPLE *pSample)
{
	voCAutoLock lock(&m_lock);

	if ( m_listSamples.empty() )
		return VO_RET_SOURCE2_NEEDRETRY;

	std::list< shared_ptr<_SAMPLE> >::iterator iter_drop = m_listSamples.end();

	if ( pSample->uTime > 0 && m_listSamples.front()->uTime + 500 < pSample->uTime )
		iter_drop = GetNextKeyFrame(pSample->uTime);

	VO_BOOL bDropFrame = VO_FALSE;

	if ( iter_drop != m_listSamples.end() )
	{
		std::list< shared_ptr<_SAMPLE> >::iterator iter = m_listSamples.begin();

		while( iter != iter_drop )
		{
			if (VO_SOURCE2_FLAG_SAMPLE_FRAMEDECODEONLY & (*iter)->uFlag)
			{
				;
			}
			else
			{
				(*iter)->uFlag |= VO_SOURCE2_FLAG_SAMPLE_FRAMESHOULDDROP;
			}
			
			iter++;
		}

		bDropFrame = VO_TRUE;
	}

	VO_U32 uRet = TrackBuffer::GetSample(pSample);

	if(bDropFrame)
		pSample->pReserve1 = &(m_uflag = VO_SOURCE2_FLAG_SAMPLE_FRAMEDROPPED);

	return uRet;
}

VO_U32 VideoTrackBuffer::Duration() /*const*/
{
	voCAutoLock lock(&m_lock);

	return m_listSamples.size() * m_uFrameInterval;
}

VO_U32 VideoTrackBuffer::SetPos(VO_U64* pullTimePoint)
{
	VO_U32 uRet = VO_RET_SOURCE2_NEEDRETRY;

	std::list< shared_ptr<_SAMPLE> >::iterator iter		= m_listSamples.begin();
	std::list< shared_ptr<_SAMPLE> >::iterator itere	= m_listSamples.end();

	std::list< shared_ptr<_SAMPLE> >::iterator iterK1	= m_listSamples.end();
	std::list< shared_ptr<_SAMPLE> >::iterator iterP		= m_listSamples.end();

	while (iter != itere)
	{
		if (VO_SOURCE2_FLAG_SAMPLE_KEYFRAME & (*iter)->uFlag)
		{
			if (m_listSamples.end() == iterK1)
				iterK1 = iter;

			if (*pullTimePoint >= (*iter)->uTime)
			{
				if (m_listSamples.end() == iterP || (*iter)->uTime > (*iterP)->uTime )
					iterP = iter;
			}
		}

		if (*pullTimePoint <= (*iter)->uTime)
		{
			uRet = VO_RET_SOURCE2_OK;
		}

		iter++;
	}
	

	if (m_listSamples.end() == iterP)
		iterP = iterK1;

	if (m_listSamples.end() == iterP)
	{
		if (VO_RET_SOURCE2_OK == uRet)
			uRet = VO_RET_SOURCE2_FAIL;

		VOLOGI("iterP NULL");
	}
	else
	{
		VOLOGI("iterP %lld", (*iterP)->uTime);
		_SetPos(iterP);
	}

	return uRet;
}

std::list< shared_ptr<_SAMPLE> >::iterator VideoTrackBuffer::GetNextKeyFrame(VO_U64 timestamp)
{
	std::list< shared_ptr<_SAMPLE> >::iterator iter = m_listSamples.begin();
	std::list< shared_ptr<_SAMPLE> >::iterator itere = m_listSamples.end();
	std::list< shared_ptr<_SAMPLE> >::iterator iter_pos = m_listSamples.end();

	while( iter != itere )
	{
		if( (*iter)->uTime > timestamp )
			break;

		if( (*iter)->uFlag & VO_SOURCE2_FLAG_SAMPLE_KEYFRAME )
			iter_pos = iter;

		if( (*iter)->uFlag & 0xFFFFF3FE )
		{
			iter_pos = iter;
			break;
		}

		iter++;
	}

	return iter_pos;
}