#include "voSourceDataBuffer.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voSourceDataBuffer"
#endif

#define SAMPLEMAXDURATION 2000


voSourceDataBuffer::voSourceDataBuffer( vo_allocator< VO_BYTE >& allocator , VO_S32 nMaxBufferTime )
: m_ptr_cache(0)
, m_nMaxBufferTime(nMaxBufferTime)
, m_allocator(allocator)
, m_bWillUsable(VO_TRUE)
{
}

voSourceDataBuffer::~voSourceDataBuffer(void)
{
	Flush ();

	if( m_ptr_cache )
	{
		destroy_source2_sample( m_ptr_cache , m_allocator );
		m_ptr_cache = 0;
	}
}

VO_S32 voSourceDataBuffer::AddBuffer (_SAMPLE * pBuffer)
{
	if (NULL == pBuffer) {
		VOLOGE("empty pointor");
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	voCAutoLock lock( &m_lock );

	if( GetBuffTime() > m_nMaxBufferTime )
		return VO_RET_SOURCE2_NEEDRETRY;

	if (pBuffer->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
	{
		m_bWillUsable = VO_TRUE;
	}
	else if ( (pBuffer->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE) ||
		(pBuffer->uFlag & VO_SOURCE2_FLAG_SAMPLE_EOS) )
	{
		m_bWillUsable = VO_FALSE;
	}

	_SAMPLE * ptr_obj = create_source2_sample( pBuffer , m_allocator );
	if (NULL == ptr_obj) {
		VOLOGE("Out of Memory");
		return VO_ERR_OUTOF_MEMORY;
	}
	
	m_list.push_back( ptr_obj );

	return VO_RET_SOURCE2_OK;
}

VO_S32 voSourceDataBuffer::GetBuffer (_SAMPLE * pBuffer)
{
	if (NULL == pBuffer) {
		VOLOGE("empty pointor");
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	}

	voCAutoLock lock( &m_lock );

	if( m_list.empty() )
		return VO_RET_SOURCE2_NEEDRETRY;

	if( m_ptr_cache )
	{
		destroy_source2_sample( m_ptr_cache , m_allocator );
		m_ptr_cache = NULL;
	}

	m_ptr_cache = *m_list.begin();

	memcpy( pBuffer , m_ptr_cache , sizeof( _SAMPLE ) );

	m_list.pop_front();

	return VO_RET_SOURCE2_OK;
}

VO_U64 voSourceDataBuffer::GetBuffTime (void)
{
	voCAutoLock lock(&m_lock);

	VOLOGR("count %d %s", m_list.count(), m_list.empty()?"true":"false");

	if ( m_list.empty() )
		return 0;

	vo_singlelink_list<_SAMPLE *>::iterator iter	= m_list.begin();
	vo_singlelink_list<_SAMPLE *>::iterator itere	= m_list.end();

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
	return ullDuration;
}

VO_S32 voSourceDataBuffer::GetBuffCount (void)
{
	voCAutoLock lock( &m_lock );

	return m_list.count();
}

VO_S32 voSourceDataBuffer::Flush (void)
{
	voCAutoLock lock( &m_lock );

	vo_singlelink_list<_SAMPLE*>::iterator iter = m_list.begin();
	vo_singlelink_list<_SAMPLE*>::iterator itere = m_list.end();

	while( iter != itere )
	{
		destroy_source2_sample( *iter , m_allocator );
		iter++;
	}

	m_list.reset();

	m_bWillUsable = VO_TRUE;

	return VO_RET_SOURCE2_OK;
}


VO_S32 voSourceDataBuffer::RemoveTo(VO_U64 ullTime)
{
	voCAutoLock lock(&m_lock);

	struct SAMPLE_WITH_SEQ{
		VO_U32				uSequence;
		_SAMPLE*	pSample;
	};

	SAMPLE_WITH_SEQ s_Samples[6] = {0};
	//0 VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM
	//1 VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT
	//2 VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE
	//3 VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
	//4 VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE
	//5 VO_SOURCE2_FLAG_SAMPLE_EOS

	VO_U32 uSequence = 0;
	vo_singlelink_list<_SAMPLE*>::iterator iter	= m_list.begin();
	vo_singlelink_list<_SAMPLE*>::iterator itere	= m_list.end();

	while (iter != itere)
	{
		if (ullTime <= (*iter)->uTime)
			break;

		if (VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM、 VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			if (s_Samples[0].pSample)
				destroy_source2_sample(s_Samples[0].pSample, m_allocator);
			s_Samples[0].pSample	= *iter;
			s_Samples[0].uSequence	= uSequence;

			if (s_Samples[1].pSample)
			{
				destroy_source2_sample(s_Samples[1].pSample, m_allocator);
				s_Samples[1].pSample	= NULL;
				s_Samples[1].uSequence	= 0;
			}

			if (s_Samples[4].pSample)
			{
				destroy_source2_sample(s_Samples[4].pSample, m_allocator);
				s_Samples[4].pSample	= NULL;
				s_Samples[4].uSequence	= 0;
			}
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM、VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			if (s_Samples[1].pSample)
				destroy_source2_sample(s_Samples[1].pSample, m_allocator);
			s_Samples[1].pSample	= *iter;
			s_Samples[1].uSequence	= uSequence;

			if (s_Samples[4].pSample)
			{
				destroy_source2_sample(s_Samples[4].pSample, m_allocator);
				s_Samples[4].pSample	= NULL;
				s_Samples[4].uSequence	= 0;
			}
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			if (s_Samples[1].pSample)
			{
				destroy_source2_sample(s_Samples[1].pSample, m_allocator);
				s_Samples[1].pSample	= NULL;
				s_Samples[1].uSequence	= 0;
			}

			if (s_Samples[4].pSample)
				destroy_source2_sample(s_Samples[4].pSample, m_allocator);
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
					destroy_source2_sample(s_Samples[i].pSample, m_allocator);
					s_Samples[i].pSample	= NULL;
					s_Samples[i].uSequence	= 0;
				}
			}
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE & (*iter)->uFlag)
		{
			if (s_Samples[2].pSample)
				destroy_source2_sample(s_Samples[2].pSample, m_allocator);
			s_Samples[2].pSample	= *iter;
			s_Samples[2].uSequence	= uSequence;
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET & (*iter)->uFlag)
		{
			if (s_Samples[3].pSample)
				destroy_source2_sample(s_Samples[3].pSample, m_allocator);
			s_Samples[3].pSample	= *iter;
			s_Samples[3].uSequence	= uSequence;
		}
		else
		{
			destroy_source2_sample(*iter , m_allocator);//throw
		}

		iter++;
		uSequence++;
	}

	m_list.removeto(iter);

	if (s_Samples[1].pSample)
	{
		if( !m_list.empty() )
		{
			vo_singlelink_list<_SAMPLE*>::iterator iter = m_list.begin();

			s_Samples[1].pSample->uTime = (*iter)->uTime - 1 > 0 ? (*iter)->uTime - 1 : 0;
		}
		else
		{
			s_Samples[1].pSample->uTime = ullTime - 1;
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

	if (m_list.count() && s_Samples[5].pSample)
	{
		iter = m_list.begin();

		VO_U32 flag = (*iter)->uFlag & s_Samples[5].pSample->uFlag;
		if ( (flag & VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM) ||
			(flag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT) )
		{
			if (s_Samples[5].pSample)
			{
				destroy_source2_sample(s_Samples[5].pSample, m_allocator);
				s_Samples[5].pSample	= NULL;
				s_Samples[5].uSequence	= 0;
			}
		}
	}

	for (int i = 5; i >= 0; i--)
	{
		if (s_Samples[i].pSample)
			m_list.push_front(s_Samples[i].pSample);
	}

	return VO_RET_SOURCE2_OK;
}


VO_S32 voSourceDataBuffer::CutFrom(VO_U64 ullTime)
{
	voCAutoLock lock(&m_lock);

	vo_singlelink_list<_SAMPLE*>::iterator iter = m_list.begin();
	vo_singlelink_list<_SAMPLE*>::iterator itere = m_list.end();

	while (iter != itere)
	{
		if (VO_SOURCE2_FLAG_SAMPLE_BASTART & (*iter)->uFlag)
		{
			(*iter)->uFlag &= ~VO_SOURCE2_FLAG_SAMPLE_BASTART;
			break;
		}

		iter++;
	}

	while (iter != itere)
	{
		if( (*iter)->uTime > ullTime )
			break;

		iter++;
	}

	vo_singlelink_list<_SAMPLE*>::iterator iterPoint = iter;
	while( iter != itere)
	{
		destroy_source2_sample( *iter , m_allocator );

		iter++;
	}

	if (iterPoint != NULL)
		m_list.removefrom(iterPoint);

	return VO_RET_SOURCE2_OK;
}

_SAMPLE * voSourceDataBuffer::PeekTheFirst()
{
	voCAutoLock lock( &m_lock );

	if( m_list.empty() )
		return NULL;

	return *m_list.begin();
}

_SAMPLE * voSourceDataBuffer::PeekTheLast()
{
	voCAutoLock lock( &m_lock );

	if( m_list.empty() )
		return NULL;

	return *m_list.last();
}


