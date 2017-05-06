
#include "vo_timestamp_recalculator.h"
#include "voCMutex.h"
#include "voLog.h"
#include "voDSType.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_mediatype_checker::vo_mediatype_checker()
:m_mediatype( VO_SOURCE2_MT_AUDIOVIDEO )
{
	;
}

vo_mediatype_checker::~vo_mediatype_checker()
{
	;
}

void vo_mediatype_checker::add_sample( VO_U32 type , const _SAMPLE * const ptr_sample )
{
	if( ( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT ) ||
		( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM ) )
	{
		if( type == VO_SOURCE2_TT_AUDIO )
		{
			switch( m_mediatype )
			{ 
			case VO_SOURCE2_MT_PUREVIDEO:
				m_mediatype = VO_SOURCE2_MT_AUDIOVIDEO;
				break;
			}
		}
		else if( type == VO_SOURCE2_TT_VIDEO )
		{
			switch( m_mediatype )
			{
			case VO_SOURCE2_MT_PUREAUDIO:
				m_mediatype = VO_SOURCE2_MT_AUDIOVIDEO;
				break;
			}
		}
	}
	else if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE )
	{
		if( type == VO_SOURCE2_TT_AUDIO )
		{
			switch( m_mediatype )
			{
			case VO_SOURCE2_MT_AUDIOVIDEO:
				m_mediatype = VO_SOURCE2_MT_PUREVIDEO;
				break;
			}
		}
		else if( type == VO_SOURCE2_TT_VIDEO )
		{
			switch( m_mediatype )
			{
			case VO_SOURCE2_MT_AUDIOVIDEO:
				m_mediatype = VO_SOURCE2_MT_PUREAUDIO;
				break;
			}
		}
	}
}

VO_SOURCE2_MEDIATYPE vo_mediatype_checker::get_mediatype()
{
	return m_mediatype;
}

vo_timestamp_recalculator::vo_timestamp_recalculator()
:m_last_video(0)
,m_last_audio(0)
,m_audio_offset(0)
,m_video_offset(0)
,m_subtitle_offset(0)
,m_customtag_offset(0)
,m_uStartOffset(0)
,m_audio_jumpcount(0)
,m_video_jumpcount(0)
,m_offset_notsync_count(0)
,m_b_firstaudio(VO_TRUE)
,m_b_firstvideo(VO_TRUE)
,m_b_firstsubtitle(VO_TRUE)
,m_b_firstcustomtag(VO_TRUE)
,m_last_calaudio(0)
,m_last_calvideo(0)
{
	;
}

vo_timestamp_recalculator::~vo_timestamp_recalculator()
{
	;
}
VO_VOID vo_timestamp_recalculator::seek(VO_U64 uPos)
{
	m_last_video = 0;
	m_last_audio = 0;
	m_audio_offset = 0;
	m_subtitle_offset = 0;
	m_customtag_offset = 0;
	m_video_offset = 0;
	m_uStartOffset = uPos;
	m_b_firstvideo = VO_TRUE;
	m_b_firstaudio = VO_TRUE;
	m_b_firstsubtitle = VO_TRUE;
	m_b_firstcustomtag = VO_TRUE;
}
VO_VOID vo_timestamp_recalculator::recalculate( VO_U32 type , _SAMPLE* ptr_sample )
{
	voCAutoLock lock( &m_Lock );
	
	if(type != VO_SOURCE2_TT_CUSTOMTIMEDTAG)
	{
		VOLOGI( "+ReCalc Type: %d Flag: 0x%08x TimeStamp: %lld,uSize is %d" , type , ptr_sample->uFlag , ptr_sample->uTime,ptr_sample->uSize);
		VO_SOURCE2_MEDIATYPE old_mt = m_mediatype.get_mediatype();

		m_mediatype.add_sample( type , ptr_sample );

		VO_SOURCE2_MEDIATYPE new_mt = m_mediatype.get_mediatype();

		if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE )
			return;
/*
		if( old_mt != new_mt && !m_b_firstaudio && !m_b_firstvideo)
		{
			VOLOGI( "MediaType Change: %d --> %d" , old_mt , new_mt );
			if( old_mt == VO_SOURCE2_MT_PUREVIDEO )
			{
				m_uStartOffset = m_last_calvideo;
			}
			else
			{
				m_uStartOffset = m_last_calaudio;
			}

			m_b_firstvideo = m_b_firstaudio = m_b_firstsubtitle = m_b_firstcustomtag = VO_TRUE;
			m_last_video = 0;
			m_last_audio = 0;
			m_audio_offset = 0;
			m_video_offset = 0;
			m_subtitle_offset = 0;
			m_customtag_offset = 0;
		}
*/
		switch( type )
		{
		case VO_SOURCE2_TT_AUDIO:
			{
				VO_BOOL forcerecalculate = VO_FALSE;

	/*			if(ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET)
				{
					m_last_audio = 0;
					m_audio_offset = 0;
					forcerecalculate = VO_TRUE;
				}*/

				if( ( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_CHUNKDROPPED ) && ptr_sample->uTime > m_last_audio && !m_b_firstaudio )
				{
					m_last_audio = ptr_sample->uTime;
				}

				VO_BOOL b = need_recalculate( m_last_audio , ptr_sample , m_b_firstaudio || forcerecalculate ? VO_TRUE : VO_FALSE );
				if(b)
				{
					VO_U64 expect_audio_ts = m_last_audio + 1;

					if( !m_b_firstvideo && m_b_firstaudio )
					{
						m_audio_offset = m_video_offset;
					}
					else
					{
						if( ptr_sample->uTime >= expect_audio_ts )
						{
							m_audio_offset = m_audio_offset - ( ptr_sample->uTime - expect_audio_ts );
						}
						else
						{
							m_audio_offset = m_audio_offset + ( expect_audio_ts - ptr_sample->uTime );
						}
					}

					/*m_audio_jumpcount++;

					if( m_audio_jumpcount == m_video_jumpcount )
						m_video_offset = m_audio_offset = m_video_offset > m_audio_offset ? m_video_offset : m_audio_offset;*/
				}

				m_b_firstaudio = VO_FALSE;

				m_last_audio = ptr_sample->uTime;
				ptr_sample->uTime = ptr_sample->uTime + m_audio_offset;

				

				if( m_mediatype.get_mediatype() == VO_SOURCE2_MT_PUREAUDIO )
				{
					m_video_offset = m_audio_offset;
					m_last_video = m_last_audio;
				}
			}
#ifdef _USE_BUFFERCOMMANDER
			recalculate_offset((ptr_sample->uDataSource_Flag & VO_DATASOURCE_FLAG_CHUNK_BEGIN) ? VO_TRUE : VO_FALSE);
#else
			recalculate_offset((ptr_sample->uFlag & VO_DATASOURCE_FLAG_CHUNK_BEGIN) ? VO_TRUE : VO_FALSE);
#endif
			break;
		case VO_SOURCE2_TT_VIDEO:
			{
				VO_BOOL forcerecalculate = VO_FALSE;

	/*			if(ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET)
				{
					m_last_video = 0;
					m_video_offset = 0;
					forcerecalculate = VO_TRUE;
				}*/

				if( ( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_CHUNKDROPPED ) && ptr_sample->uTime > m_last_video && !m_b_firstvideo )
				{
					m_last_video = ptr_sample->uTime;
				}

				VO_BOOL b = need_recalculate( m_last_video , ptr_sample , m_b_firstvideo || forcerecalculate ? VO_TRUE : VO_FALSE );
				if(b)
				{
					VO_U64 expect_video_ts = m_last_video + 1;

					VO_U64 delta = expect_video_ts > m_last_audio ? expect_video_ts - m_last_audio : m_last_audio - expect_video_ts;

					if( !m_b_firstaudio && m_b_firstvideo )
					{
						m_video_offset = m_audio_offset;
					}
					else
					{
						if( ptr_sample->uTime > expect_video_ts )
						{
							m_video_offset = m_video_offset - ( ptr_sample->uTime - expect_video_ts );
						}
						else
						{
							m_video_offset = m_video_offset + ( expect_video_ts - ptr_sample->uTime );
						}
					}

					/*m_video_jumpcount++;

					if( m_audio_jumpcount == m_video_jumpcount )
						m_video_offset = m_audio_offset = m_video_offset > m_audio_offset ? m_video_offset : m_audio_offset;*/
				}
				
				m_b_firstvideo = VO_FALSE;

				m_last_video = ptr_sample->uTime;
				ptr_sample->uTime = ptr_sample->uTime + m_video_offset;

				if( m_mediatype.get_mediatype() == VO_SOURCE2_MT_PUREVIDEO )
				{
					m_audio_offset = m_video_offset;
					m_last_audio = m_last_video;
				}
			}
#ifdef _USE_BUFFERCOMMANDER
			recalculate_offset((ptr_sample->uDataSource_Flag & VO_DATASOURCE_FLAG_CHUNK_BEGIN) ? VO_TRUE : VO_FALSE);
#else
			recalculate_offset((ptr_sample->uFlag & VO_DATASOURCE_FLAG_CHUNK_BEGIN) ? VO_TRUE : VO_FALSE);
#endif
			break;
		case VO_SOURCE2_TT_SUBTITLE:
			{
				if( VO_U32(ptr_sample->pReserve2) == __INTERNALSUBTITLE )
				{
					if( !m_b_firstvideo )
					{
						ptr_sample->uTime = ptr_sample->uTime + m_video_offset;
					}
					else if( !m_b_firstaudio )
					{
						ptr_sample->uTime = ptr_sample->uTime + m_audio_offset;
					}
					else
					{
						// in case of +m_uStartOffset, -m_uStartOffset first
						if(m_b_firstsubtitle)
						{
							m_b_firstsubtitle = VO_FALSE;
							m_subtitle_offset = m_subtitle_offset - ptr_sample->uTime;
							ptr_sample->uTime = 0 - m_uStartOffset;
						}
						else
						{
							ptr_sample->uTime = ptr_sample->uTime + m_subtitle_offset - m_uStartOffset;
						}
					}
				}
				else
					return;
			}
			break;
		default:
			return;
		}

		ptr_sample->uTime += m_uStartOffset;

		if( ((VO_S64)ptr_sample->uTime) < 0 )
		 	ptr_sample->uTime = 0;

		if( type == VO_SOURCE2_TT_AUDIO )
			m_last_calaudio = ptr_sample->uTime;
		else if( type == VO_SOURCE2_TT_VIDEO )
			m_last_calvideo = ptr_sample->uTime;

		VOLOGI( "-ReCalc Type: %d Flag: 0x%08x TimeStamp: %lld Video offset %lld Audio Offset %lld StartOffset %lld" , type , ptr_sample->uFlag , ptr_sample->uTime , m_video_offset , m_audio_offset , m_uStartOffset );
	}
	else
	{
		VOLOGI( "+ReCalc Type: %d TimeStamp: %lld" , type , ptr_sample->uTime);
		if( !m_b_firstaudio )
		{
			ptr_sample->uTime = m_last_calaudio;
		}
		else if( !m_b_firstvideo )
		{
			ptr_sample->uTime = m_last_calvideo;
		}
		else
		{
			ptr_sample->uTime = m_uStartOffset;
		}
		VOLOGI( "-ReCalc Type: %d TimeStamp: %lld" , type , ptr_sample->uTime);
	}
}

VO_BOOL vo_timestamp_recalculator::need_recalculate( VO_U64 last_timestamp , _SAMPLE* ptr_sample , VO_BOOL isforce )
{
	VO_U64 delta = ptr_sample->uTime > last_timestamp ? ptr_sample->uTime - last_timestamp : last_timestamp - ptr_sample->uTime;

	if( isforce )
		return VO_TRUE;

/*	if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_BAEND )
		return VO_FALSE;
*/
	if( delta > 10000 || ( delta > 2000 && ptr_sample->uTime < last_timestamp ) )
	{
#ifdef _USE_BUFFERCOMMANDER
		if(ptr_sample->uDataSource_Flag & VO_DATASOURCE_FLAG_CHUNK_SWITCH)
#else
		if(ptr_sample->uFlag & VO_DATASOURCE_FLAG_CHUNK_SWITCH)
#endif
		{
			if(delta > 100000)
			{
				VOLOGW("timestamp jump!");
				return VO_TRUE;
			}
			if(ptr_sample->uDataSource_Flag & VO_DATASOURCE_FLAG_DISCONTINUE)
			{
				VOLOGI("discontinue chunk, need force recalculate");
				return VO_TRUE;
			}
			VOLOGW("do not need recalculate since it is ba chunk");
			return VO_FALSE;
		}
		VOLOGI( "Need ReCalc TimeStamp: %lld , LastTimeStamp: %lld" , ptr_sample->uTime , last_timestamp );
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_VOID vo_timestamp_recalculator::recalculate_offset(VO_BOOL bChunkBegin)
{
	// if first audio/video, the offset is meaningless, so we won't compare them, East 20130409
	if( m_video_offset != m_audio_offset && !m_b_firstaudio && !m_b_firstvideo)
	{
		m_offset_notsync_count++;

		VO_S64 delta = m_video_offset > m_audio_offset ? m_video_offset - m_audio_offset : m_audio_offset - m_video_offset;

		if(delta <= 2000 || (m_offset_notsync_count > 600 && bChunkBegin && m_mediatype.get_mediatype() == VO_SOURCE2_MT_AUDIOVIDEO))
		{
			VOLOGW("TS Recalc force A/V use same offset: delta %lld, notsync_count %d, bChunkBegin %d, video_offset %lld, audio_offset %lld", 
				delta, m_offset_notsync_count, bChunkBegin, m_video_offset, m_audio_offset);

			m_video_offset = m_audio_offset = m_video_offset > m_audio_offset ? m_video_offset : m_audio_offset;
		}

		return;
	}

	m_offset_notsync_count = 0;
}