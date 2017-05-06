#include "voAdaptiveStreamingTimeStampSync.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "voToolUtility.h"

#define MAXDELTA 2000
#define TIMESTAMPNOTAVALIABLE 0xffffffffffffffffll

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voAdaptiveStreamingTimeStampSync::voAdaptiveStreamingTimeStampSync()
:m_last_audiotime(0)
,m_last_videotime(0)
,m_sync_timestamp(0)
,m_disable( VO_FALSE )
,m_isaudioavalible( VO_TRUE )
,m_isvideoavalible( VO_TRUE )
,m_max_deltavalue(MAXDELTA)
,m_nResetType(RESET_IMMEDIATELY)
,m_nResetTime(0)
,m_bReset(VO_TRUE)
{
}

voAdaptiveStreamingTimeStampSync::~voAdaptiveStreamingTimeStampSync()
{
	;
}

VO_U32 voAdaptiveStreamingTimeStampSync::Reset( VO_U64 ullTime, VO_U32 nResetType )
{
	VOLOGI("Reset. Time:%lld, Type:%d", ullTime, nResetType);
	m_nResetType = nResetType;
	m_nResetTime = ullTime;
	m_bReset = VO_TRUE;
	return VO_RET_SOURCE2_OK;
}
VO_U32 voAdaptiveStreamingTimeStampSync::Reset2( VO_U64 ullTime )
{
	if(	m_bReset != VO_TRUE)
		return VO_RET_SOURCE2_OK;

	VO_U64 temp = ullTime;
	switch(m_nResetType)
	{
	case RESET_IMMEDIATELY:
		{
			temp = m_nResetTime;

			m_sync_timestamp = temp;
			m_last_videotime = temp;
			m_last_audiotime = temp;
			m_isaudioavalible = VO_TRUE;
			m_isvideoavalible = VO_TRUE;
		}
		break;
	case RESET_FIRSTVIDEOFRAME:
		{
			temp = ullTime;

			m_sync_timestamp = temp;
			if( m_isvideoavalible )
			{
				m_last_videotime = temp;
				m_isvideoavalible = VO_TRUE;
			}
		}
		break;
	case RESET_FIRSTAUDIOFRAME:
		{
			temp = ullTime;

			m_sync_timestamp = temp;
			if( m_isaudioavalible )
			{
				m_last_audiotime = temp;
				m_isaudioavalible = VO_TRUE;
			}
		}
		break;
	case RESET_FIRSTAVFRAME:
		{
			temp = ullTime;

			m_sync_timestamp = temp;
			if( m_isvideoavalible )
			{
				m_last_videotime = temp;
				m_isvideoavalible = VO_TRUE;
			}
			if( m_isaudioavalible )
			{
				m_last_audiotime = temp;
				m_isaudioavalible = VO_TRUE;
			}
		}
		break;
	default:
		return VO_RET_SOURCE2_OK;
	}

	//m_max_deltavalue = MAXDELTA;
	
	m_bReset = VO_FALSE;

	return VO_RET_SOURCE2_OK;
}
VO_VOID voAdaptiveStreamingTimeStampSync::SetChunkDuration( VO_U64 duration )
{
	VO_U64 new_duration = duration + 1000 > 5000 ? duration + 1000 : 5000;//* 2 / 3;
	if( new_duration == 0)
		return;

	VOLOGI( "Use Delta Value: %lld %lld" , duration , new_duration );
	m_max_deltavalue = new_duration;
}

VO_U32 voAdaptiveStreamingTimeStampSync::CheckAndWait( VO_U32 type , _SAMPLE * pSample , VO_BOOL * pIsStop )
{
	if( m_disable )
		return VO_RET_SOURCE2_OK;

	if( type != VO_SOURCE2_TT_AUDIO && type != VO_SOURCE2_TT_VIDEO )
		return VO_RET_SOURCE2_OK;
	Reset2(pSample->uTime);

	if( pSample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE )
	{
		switch ( type )
		{
		case VO_SOURCE2_TT_VIDEO:
			{
				m_last_videotime = TIMESTAMPNOTAVALIABLE;
				m_isvideoavalible = VO_FALSE;
			}
			break;
		case VO_SOURCE2_TT_AUDIO:
			{
				m_last_audiotime = TIMESTAMPNOTAVALIABLE;
				m_isaudioavalible = VO_FALSE;
			}
			break;
		}

		VOLOGI( "See UnAvailable For %s Track!" , TrackType2String((VO_SOURCE2_TRACK_TYPE)type) );

		return VO_RET_SOURCE2_OK;
	}

 //	if( m_last_videotime == TIMESTAMPNOTAVALIABLE && m_last_audiotime == TIMESTAMPNOTAVALIABLE )
// 	{
// 		VOLOGR(" last audio & last video reset:%lld", pSample->uTime);
// 		m_last_audiotime = pSample->uTime;
// 		m_last_videotime = pSample->uTime;
// 	}
	VO_U64 uGap = 0;
	if( pSample->uTime > m_max_deltavalue )
		uGap = (VO_U64) ( pSample->uTime - m_max_deltavalue );
	else
		uGap = 0;
	VO_U64 uGap2 = 0;
	if( pSample->uTime > m_max_deltavalue / 2 )
		uGap2 = (VO_U64) ( pSample->uTime - m_max_deltavalue / 2 );
	else
		uGap2 = 0;

	VO_BOOL bMust2Wait = VO_FALSE;
	VO_BOOL *pIsAvalibleOther = &m_isvideoavalible;
	VO_BOOL *pIsAvalibleSelf = &m_isaudioavalible;
	voLockedObject< VO_U64 > *pLastTimeOther = &m_last_videotime;
	voLockedObject< VO_U64 > *pLastTimeSelf = &m_last_audiotime;	

	switch( type )
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			pIsAvalibleOther = &m_isvideoavalible;
			pIsAvalibleSelf = &m_isaudioavalible;
			pLastTimeOther = &m_last_videotime;
			pLastTimeSelf = &m_last_audiotime;
			m_nAudioThreadID = pSample->uThreadID;
		}
		break;
	case VO_SOURCE2_TT_VIDEO:
		{
			pIsAvalibleOther = &m_isaudioavalible;
			pIsAvalibleSelf = &m_isvideoavalible;
			pLastTimeOther = &m_last_audiotime;
			pLastTimeSelf = &m_last_videotime;
			m_nVideoThreadID = pSample->uThreadID;
		}
		break;
	}
#ifdef _USE_BUFFERCOMMANDER
	if(!(pSample->uDataSource_Flag & VO_DATASOURCE_FLAG_FORCEOUTPUT) && *pIsAvalibleOther && uGap > *pLastTimeOther && m_nVideoThreadID!= m_nAudioThreadID)
#else
	if(!(pSample->uFlag & VO_DATASOURCE_FLAG_FORCEOUTPUT) && *pIsAvalibleOther && uGap > *pLastTimeOther)
#endif
		bMust2Wait = VO_TRUE;

	if( uGap > *pLastTimeSelf )
		*pLastTimeSelf = pSample->uTime;
	
	if( bMust2Wait )
	{
		VOLOGI( "Start %s Wait. CurTimeStamp:%lld, LastVideoTime:%lld, LastAudioTime:%lld" 
				, TrackType2String((VO_SOURCE2_TRACK_TYPE)type), pSample->uTime, (VO_U64)m_last_videotime, (VO_U64)m_last_audiotime );
		while( !m_disable && uGap2 > *pLastTimeOther && !( *pIsStop ) && *pIsAvalibleOther )
			voOS_SleepExitable(100, pIsStop);
	}

	*pLastTimeSelf = pSample->uTime;
	if( !(*pIsAvalibleSelf) )
		*pIsAvalibleSelf = VO_TRUE;

	if( bMust2Wait )
	{
		VOLOGI("End %s Wait. CurTimeStamp:%lld, LastVideoTime:%lld, LastAudioTime:%lld" 
			, TrackType2String((VO_SOURCE2_TRACK_TYPE)type), pSample->uTime, (VO_U64)m_last_videotime, (VO_U64)m_last_audiotime);
	}

	return VO_RET_SOURCE2_OK;
}

VO_VOID voAdaptiveStreamingTimeStampSync::Disable( VO_BOOL b )
{
	Reset(0, 1);
	m_disable = b;
}
