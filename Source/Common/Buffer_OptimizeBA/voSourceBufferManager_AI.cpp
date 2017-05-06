#include "voSourceBufferManager_AI.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


voSourceBufferManager_AI::voSourceBufferManager_AI(VO_S32 nBufferingTime, VO_S32 nMaxBufferTime, VO_S32 nStartBufferTime)
: m_MediaBuffer(nMaxBufferTime, nBufferingTime, nStartBufferTime)
, m_ullTimeLastAudio(0)
{
}

voSourceBufferManager_AI::~voSourceBufferManager_AI(void)
{
}


VO_U32 voSourceBufferManager_AI::AddBuffer(VO_U32 uType, VO_PTR ptr_obj)
{
	if (VO_BUFFER_SETTINGS == uType)
	{
		VO_BUFFER_SETTING_SAMPLE * ptr_sample = static_cast<VO_BUFFER_SETTING_SAMPLE *>(ptr_obj);

		switch (ptr_sample->nType)
		{
		case VO_BUFFER_SETTING_STARTBUFFERTIME:
			{
				VO_U32* uDuration = static_cast<VO_U32*>(ptr_sample->pObj);
				m_MediaBuffer.SetParameter(VO_BUFFER_PID_Quik_Start_Duration, uDuration);
			}
			break;

		case VO_BUFFER_SETTING_BUFFERTIME:
			{
				VO_U32* uDuration = static_cast<VO_U32*>(ptr_sample->pObj);
				m_MediaBuffer.SetParameter(VO_BUFFER_PID_Buffering_Duration, uDuration);
			}
			break;

		case VO_BUFFER_SETTING_BUFFERING_STYLE:
			{
				VO_BUFFERING_STYLE_TYPE* eBuffering_Style = static_cast<VO_BUFFERING_STYLE_TYPE*>(ptr_sample->pObj);
				m_MediaBuffer.SetParameter(VO_BUFFER_PID_STYLE, eBuffering_Style);
			}
			break;

		default:
			return VO_ERR_NOT_IMPLEMENT;
		}

		return VO_RET_SOURCE2_OK;
	}

	return m_MediaBuffer.PutSample( *( (shared_ptr<_SAMPLE>*)ptr_obj ), static_cast<VO_SOURCE2_TRACK_TYPE>(uType) );
}

VO_U32 voSourceBufferManager_AI::AddBuffers(VO_U32 uType, VO_PTR ptr_obj)
{
	VO_U64 ullTimeEnd = 0xffffffffffffffffll;
	std::list< shared_ptr<_SAMPLE> >* plist = (std::list< shared_ptr<_SAMPLE> >*)(ptr_obj);

	if (VO_SOURCE2_TT_AUDIO == uType && !plist->empty())
	{
		//find the effective start ponit
		VO_U64 ullTimeStart = 0xffffffffffffffffll;
		{
			std::list< shared_ptr<_SAMPLE> >::iterator iter		= plist->begin();
			std::list< shared_ptr<_SAMPLE> >::iterator itere	= plist->end();

			while (iter != itere)
			{
				if (VO_SOURCE2_FLAG_SAMPLE_EOS & (*iter)->uFlag ||
					VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & (*iter)->uFlag)
				{
				}
				else
				{
					VOLOGI("Start uSize 0x%x, uFlag 0x%x, uTime \t%lld", (*iter)->uSize, (*iter)->uFlag, (*iter)->uTime);
					ullTimeStart = (*iter)->uTime;
					break;
				}

				iter++;
			}
		}

		//find the effective end ponit
		std::list< shared_ptr<_SAMPLE> >::reverse_iterator  riter	= plist->rbegin();
		std::list< shared_ptr<_SAMPLE> >::reverse_iterator  ritere	= plist->rend();

		while (riter != ritere)
		{
			if (VO_SOURCE2_FLAG_SAMPLE_EOS & (*riter)->uFlag ||
				VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & (*riter)->uFlag)
			{
			}
			else
			{
				VOLOGI("End uSize 0x%x, uFlag 0x%x, uTime \t%lld", (*riter)->uSize, (*riter)->uFlag, (*riter)->uTime);
				ullTimeEnd = (*riter)->uTime;
				break;
			}

			riter++;
		}

		if (ullTimeStart < m_ullTimeLastAudio)
		{
			//need drop reduplicate data
			std::list< shared_ptr<_SAMPLE> >::iterator iter		= plist->begin();
			std::list< shared_ptr<_SAMPLE> >::iterator itere	= plist->end();

			while (iter != itere)
			{
				if (m_ullTimeLastAudio < (*iter)->uTime)
					break;

				iter++;
			}

			_SetPos(plist, iter);

			//adjust the time-stamp of VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT
			VO_U64 ullTimeDropEnd = ullTimeEnd;
			if (plist->end() == iter)
			{
				if (!plist->empty() &&
					plist->front()->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT)
				{
					VO_S64 llTime = m_ullTimeLastAudio - 1;
					plist->front()->uTime = llTime >= 0 ? llTime : 0;
				}
			}
			else
			{
				ullTimeDropEnd = (*iter)->uTime;
			}

			VOLOGI("Drop from %lld to %lld, last audio is %lld", ullTimeStart, ullTimeDropEnd, m_ullTimeLastAudio);
		}
	}

	VO_U32 uRet = m_MediaBuffer.PutSamples( plist, static_cast<VO_SOURCE2_TRACK_TYPE>(uType) );

	if (VO_SOURCE2_TT_AUDIO == uType &&
		VO_RET_SOURCE2_OK == uRet &&
		0xffffffffffffffffll != ullTimeEnd &&
		m_ullTimeLastAudio < ullTimeEnd)
	{
		//remeber the last time stamp
		m_ullTimeLastAudio = ullTimeEnd;
	}

	return uRet;
}