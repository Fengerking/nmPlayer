#include "TrackCache.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

TrackCache::TrackCache(VO_U32 uMaxDuration)
	: m_uMaxDuration(uMaxDuration)
{
}


TrackCache::~TrackCache(void)
{
}

VO_U32 TrackCache::PutSample(const shared_ptr<_SAMPLE> pSample)
{
	if ( m_listSamples.empty() == false)
	{
		VO_S32 iDuration = m_listSamples.back()->uTime - m_listSamples.front()->uTime;
		if (iDuration < 0) iDuration = -iDuration;

		if (iDuration > m_uMaxDuration)
		{
			VOLOGI("Chunk full last %lld first %lld", m_listSamples.back()->uTime, m_listSamples.front()->uTime);

			return VO_RET_SOURCE2_NEEDRETRY;
		}
	}

	m_listSamples.push_back(pSample);

	return VO_RET_SOURCE2_OK;
}

VO_U32 TrackCache::GetSample(shared_ptr<_SAMPLE>* pSample)
{
	if ( m_listSamples.empty() )
		return VO_RET_SOURCE2_NEEDRETRY;

	*pSample = m_listSamples.front();
	m_listSamples.pop_front();

	return VO_RET_SOURCE2_OK;
}

VO_U32 TrackCache::GetSamples(std::list< shared_ptr<_SAMPLE> > **plist)
{
	*plist = &m_listSamples;

	return VO_RET_SOURCE2_OK;
}

VO_U32 TrackCache::SetPos(std::list< shared_ptr<_SAMPLE> >::iterator iterP)
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