#include "TrackBuffer.h"
#include "voSampleOP.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define SAMPLEMAXDURATION 2000



TrackBuffer::TrackBuffer(VO_U32 uMaxDuration)
: m_uMaxDuration(uMaxDuration)
, m_uCurDuration(0)
//, m_pCacheSample()
, m_bWillUsable(VO_TRUE)
{
}

TrackBuffer::~TrackBuffer(void)
{
	Flush();
}

const shared_ptr<_SAMPLE> TrackBuffer::Peek() const
{
	if ( m_listSamples.empty() )
	{
		shared_ptr<_SAMPLE> p;
		return p;
	}

	return m_listSamples.front();
}

VO_U32 TrackBuffer::PutSample(const shared_ptr<_SAMPLE> pSample)
{
	voCAutoLock lock(&m_lock);

	if (Duration() >= m_uMaxDuration)
		return VO_RET_SOURCE2_NEEDRETRY;

	if (m_listSamples.empty() == false)
	{
		VO_U32 uInterval = pSample->uTime > m_listSamples.back()->uTime ? pSample->uTime - m_listSamples.back()->uTime : m_listSamples.back()->uTime - pSample->uTime;
		if (uInterval < SAMPLEMAXDURATION)
			m_uCurDuration += uInterval;
	}

	if (pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
	{
		m_bWillUsable = VO_TRUE;
	}
	else if ( (pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE) ||
		(pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_EOS) )
	{
		m_bWillUsable = VO_FALSE;
	}

	m_listSamples.push_back(pSample);

	return VO_RET_SOURCE2_OK;
}

VO_U32 TrackBuffer::PutSamples(std::list< shared_ptr<_SAMPLE> >* plist, VO_BOOL bForceInput)
{
	voCAutoLock lock(&m_lock);

	if ( plist->empty() )
	{
		return 0;
	}

	VO_S32 iDuration = plist->back()->uTime - plist->front()->uTime;
	VOLOGI("uSize 0x%x, uFlag 0x%x, uTime \t%lld", plist->front()->uSize, plist->front()->uFlag, plist->front()->uTime);
	VOLOGI("uSize 0x%x, uFlag 0x%x, uTime \t%lld", plist->back()->uSize, plist->back()->uFlag, plist->back()->uTime);
	VOLOGI("Duration %d", iDuration);
	if (iDuration < 0)
		iDuration = -iDuration;

	if (bForceInput ||
		(iDuration >= m_uMaxDuration && Duration() < m_uMaxDuration / 2) )
	{
		;
	}
	else if (Duration() + iDuration > m_uMaxDuration)
		return VO_RET_SOURCE2_NEEDRETRY;

	VOLOGI("Duration+ %d", iDuration);

	m_listSamples.splice(m_listSamples.end(), *plist);

	m_uCurDuration += iDuration;

	if ( (m_listSamples.back()->uFlag& VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE) ||
		(m_listSamples.back()->uFlag & VO_SOURCE2_FLAG_SAMPLE_EOS) )
	{
		m_bWillUsable = VO_FALSE;
	}
	else
	{
		m_bWillUsable = VO_TRUE;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 TrackBuffer::GetSample(_SAMPLE* pSample)
{
	voCAutoLock lock(&m_lock);

	if ( m_listSamples.empty() )
		return VO_RET_SOURCE2_NEEDRETRY;

	m_pCacheSample = m_listSamples.front();
	m_listSamples.pop_front();

	memcpy( pSample, m_pCacheSample.get(), sizeof(_SAMPLE) );

	if ( m_listSamples.empty() )
	{
		m_uCurDuration = 0;
	}
	else
	{
		VO_U32 uInterval = pSample->uTime > m_listSamples.front()->uTime ? pSample->uTime - m_listSamples.front()->uTime : m_listSamples.front()->uTime - pSample->uTime;
		if (uInterval < SAMPLEMAXDURATION && uInterval < m_uCurDuration)
			m_uCurDuration -= uInterval;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 TrackBuffer::SetPos(VO_U64* pullTimePoint)
{
	voCAutoLock lock(&m_lock);

	VO_U32 uRet = VO_RET_SOURCE2_NEEDRETRY;

	if ( m_listSamples.empty() )
		return uRet;

	std::list< shared_ptr<_SAMPLE> >::iterator iter	= m_listSamples.begin();
	std::list< shared_ptr<_SAMPLE> >::iterator itere	= m_listSamples.end();

	std::list< shared_ptr<_SAMPLE> >::iterator iterP	= m_listSamples.end();
	while (iter != itere)
	{
		if (*pullTimePoint >= (*iter)->uTime)
		{
			if (m_listSamples.end() == iterP || (*iter)->uTime > (*iterP)->uTime)
				iterP = iter;
		}
		else
		{
			uRet = VO_RET_SOURCE2_OK;
		}

		iter++;
	}

	if (m_listSamples.end() == iterP)
	{
		*pullTimePoint = m_listSamples.front()->uTime;

		return uRet;
	}
	else
	{
		_SetPos(iterP);
		m_uCurDuration = CalculateDuration();
	}

	return uRet;
}

VO_U32 TrackBuffer::_SetPos(std::list< shared_ptr<_SAMPLE> >::iterator iterP)
{
	struct SAMPLE_WITH_SEQ{
		VO_U32						uSequence;
		shared_ptr<_SAMPLE>	pSample;
	};

	SAMPLE_WITH_SEQ s_Samples[6] = {0};
	//0 VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM
	//1 VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT
	//2 VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE
	//3 VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
	//4 VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE
	//5 VO_SOURCE2_FLAG_SAMPLE_EOS

	VO_U32 uSequence = 0;
	std::list< shared_ptr<_SAMPLE> >::iterator iter = m_listSamples.begin();

	while (iter != iterP)
	{
		if (VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM、 VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			s_Samples[0].pSample	= *iter;
			s_Samples[0].uSequence	= uSequence;

			if (s_Samples[1].pSample)
			{
				s_Samples[1].pSample	= NULL;
				s_Samples[1].uSequence	= 0;
			}

			if (s_Samples[4].pSample)
			{
				s_Samples[4].pSample	= NULL;
				s_Samples[4].uSequence	= 0;
			}
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM、VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			s_Samples[1].pSample	= *iter;
			s_Samples[1].uSequence	= uSequence;

			if (s_Samples[4].pSample)
			{
				s_Samples[4].pSample	= NULL;
				s_Samples[4].uSequence	= 0;
			}
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			if (s_Samples[1].pSample)
			{
				s_Samples[1].pSample	= NULL;
				s_Samples[1].uSequence	= 0;
			}

			s_Samples[4].pSample	= *iter;
			s_Samples[4].uSequence	= uSequence;
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_EOS & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM、 VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			for (VO_S32 i = 0; i < 4; i++)
			{
				if (s_Samples[i].pSample)
				{
					s_Samples[i].pSample	= NULL;
					s_Samples[i].uSequence	= 0;
				}
			}
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE & (*iter)->uFlag)
		{
			s_Samples[2].pSample	= *iter;
			s_Samples[2].uSequence	= uSequence;
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET & (*iter)->uFlag)
		{
			s_Samples[3].pSample	= *iter;
			s_Samples[3].uSequence	= uSequence;
		}

		iter++;
		uSequence++;
	}

	m_listSamples.erase(m_listSamples.begin(), iter);

	if (s_Samples[1].pSample)
	{
		if (m_listSamples.end() == iterP)
		{
			s_Samples[1].pSample->uTime = 0;
		}
		else if (1 <= (*iterP)->uTime)
		{
			s_Samples[1].pSample->uTime = (*iterP)->uTime - 1;
		}
		else
		{
			s_Samples[1].pSample->uTime = 0;
		}
	}

	SAMPLE_WITH_SEQ temp;
	for (VO_S32 i = 6; i > 0; i--)
	{
		for (VO_S32 j = 0; j < i - 1; j++)
		{
			if (s_Samples[j].uSequence > s_Samples[j + 1].uSequence)
			{
				temp = s_Samples[j];
				s_Samples[j] = s_Samples[j + 1];
				s_Samples[j + 1] = temp;
			}
		}
	}

	if (m_listSamples.size() && s_Samples[5].pSample)
	{
		iter = m_listSamples.begin();

		VO_U32 flag = (*iter)->uFlag & s_Samples[5].pSample->uFlag;
		if ( (flag & VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM) ||
			(flag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT) )
		{
			if (s_Samples[5].pSample)
			{
				s_Samples[5].pSample	= NULL;
				s_Samples[5].uSequence	= 0;
			}
		}
	}

	for (int i = 5; i >= 0; i--)
	{
		if (s_Samples[i].pSample)
			m_listSamples.push_front(s_Samples[i].pSample);
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 TrackBuffer::CalculateDuration()
{
	voCAutoLock lock(&m_lock);

	if ( m_listSamples.empty() )
		return 0;

	std::list< shared_ptr<_SAMPLE> >::iterator iter		= m_listSamples.begin();
	std::list< shared_ptr<_SAMPLE> >::iterator itere	= m_listSamples.end();

	VO_U64 ullDuration = 0;
	VO_U64 ullStartTimeofContinuous		= (*iter)->uTime;
	VO_U64 ullEndTimeofContinuous		= (*iter)->uTime;

	while (iter != itere)
	{
		if( ( (*iter)->uTime > ullEndTimeofContinuous ? (*iter)->uTime - ullEndTimeofContinuous : ullEndTimeofContinuous - (*iter)->uTime ) > SAMPLEMAXDURATION )
		{
			ullDuration += ullEndTimeofContinuous > ullStartTimeofContinuous ? ullEndTimeofContinuous - ullStartTimeofContinuous : ullStartTimeofContinuous - ullEndTimeofContinuous;

			ullStartTimeofContinuous = ullEndTimeofContinuous = (*iter)->uTime;
		}
		else
		{
			ullEndTimeofContinuous = (*iter)->uTime;

			iter++;
		}
	}

	ullDuration += ullEndTimeofContinuous > ullStartTimeofContinuous ? ullEndTimeofContinuous - ullStartTimeofContinuous : ullStartTimeofContinuous - ullEndTimeofContinuous;

	VOLOGR(" ullDuration:%lld", ullDuration);
	return static_cast<VO_U32>(ullDuration);
}

VO_U32 TrackBuffer::Flush()
{
	voCAutoLock lock(&m_lock);

	m_listSamples.clear();
	
	m_pCacheSample = NULL;

	m_bWillUsable = VO_TRUE;
	m_uCurDuration = 0;

	return VO_RET_SOURCE2_OK;
}