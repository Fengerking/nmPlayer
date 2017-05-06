#include "voSourceBufferManager.h"
#include "voLog.h"

#ifndef LOG_TAG
#define LOG_TAG "voSourceBufferManager"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

typedef VO_S32 (VO_API* pvoGetCaptionParserAPI)(VO_CAPTION_PARSER_API * pParser);

voSourceBufferManager::voSourceBufferManager( VO_S32 nBufferingTime , VO_S32 nMaxBufferTime , VO_S32 nStartBufferTime )
:m_nMaxBufferTime( nMaxBufferTime )
,m_BufferingTime( nBufferingTime )
,m_StartBufferTime( nStartBufferTime )
,m_audio( m_alloc , nMaxBufferTime )
,m_video( m_alloc , nMaxBufferTime )
,m_subtitle( m_alloc )
,m_isccloaded( VO_FALSE )
,m_hcc(0)
,m_isbuffering( VO_FALSE )
,m_isstartbuffering( VO_TRUE )
,m_has_eos(VO_FALSE)
,m_ispureaudio(VO_FALSE)
,m_videorecovertimestamp(0)
,m_lastaudiotimestamp(0xffffffffffffffffLL)
,m_isseek(VO_FALSE)
,m_seekpos(0)
{
	memset( &m_ccapi , 0 , sizeof(VO_CAPTION_PARSER_API) );
}

voSourceBufferManager::~voSourceBufferManager()
{
	if( m_hcc != 0 )
	{
		if( m_ccapi.Close )
			m_ccapi.Close( m_hcc );
		m_hcc = 0;
	}
}

VO_U32 voSourceBufferManager::AddBuffer( VO_U32 buffertype , VO_PTR ptr_obj )
{
	voCAutoLock lock( &m_lock );

	if (VO_SOURCE2_FLAG_SAMPLE_EOS & ( (VO_SOURCE2_SAMPLE*)ptr_obj )->uFlag)
	{
		if (VO_SOURCE2_TT_VIDEO == buffertype)
		{
			VOLOGR("video track meet VO_SOURCE2_FLAG_SAMPLE_EOS");
		}
		else if (VO_SOURCE2_TT_AUDIO == buffertype)
		{
			VOLOGR("audio track meet VO_SOURCE2_FLAG_SAMPLE_EOS");
		}
	} 
	else if (VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & ( (VO_SOURCE2_SAMPLE*)ptr_obj )->uFlag)
	{
		if (VO_SOURCE2_TT_VIDEO == buffertype)
		{
			VOLOGR("video track meet VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE");
		}
		else if (VO_SOURCE2_TT_AUDIO == buffertype)
		{
			VOLOGR("audio track meet VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE");
		}
	}
	else if (VO_SOURCE2_FLAG_SAMPLE_FORCE_FLUSH & ( (VO_SOURCE2_SAMPLE*)ptr_obj )->uFlag)
	{
		if (VO_SOURCE2_TT_VIDEO == buffertype)
			m_video.Flush();
		else if (VO_SOURCE2_TT_AUDIO == buffertype)
			m_audio.Flush();
		else if (VO_SOURCE2_TT_SUBTITLE == buffertype)
			m_subtitle.Flush();
	}

	if( m_isseek )
		return addbuffer_duringseek( buffertype , ptr_obj );

	VO_U32 ret = VO_RET_SOURCE2_OUTPUTNOTFOUND;

	switch( buffertype )
	{
	case VO_SOURCE2_TT_VIDEO:
		ret = add_video( ( VO_SOURCE2_SAMPLE* )ptr_obj );
		break;
	case VO_SOURCE2_TT_AUDIO:
		ret = add_audio( ( VO_SOURCE2_SAMPLE* )ptr_obj );
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		ret = add_subtitle( ( voSubtitleInfo* )ptr_obj );
		break;
	}

	VOLOGR("TRACK_TYPE %d uRet %x, uSize %x, uFlag %d, uTime \t%lld", buffertype, ret, ( (VO_SOURCE2_SAMPLE*)ptr_obj )->uSize, ( (VO_SOURCE2_SAMPLE*)ptr_obj )->uFlag, ( (VO_SOURCE2_SAMPLE*)ptr_obj )->uTime);

	return ret;
}

VO_U32 voSourceBufferManager::GetBuffer( VO_U32 buffertype , VO_PTR ptr_obj )
{
	voCAutoLock lock( &m_lock );

	VO_U32 ret = VO_RET_SOURCE2_OUTPUTNOTFOUND;

	if( m_isseek )
		return VO_RET_SOURCE2_NEEDRETRY;

	if( m_isstartbuffering && !m_has_eos )
	{
		if ( 0 == m_audio.GetBuffTime() && 0 == m_video.GetBuffTime() && 0 == m_subtitle.GetBuffTime() )
		{
			VOLOGI("VO_RET_SOURCE2_NEEDRETRY");
			return VO_RET_SOURCE2_NEEDRETRY;
		}
		else if ( (0 == m_audio.GetBuffTime() ? true : m_audio.GetBuffTime() >= m_BufferingTime) &&
			(0 == m_video.GetBuffTime() ? true : m_video.GetBuffTime() >= m_BufferingTime) &&
			(0 == m_subtitle.GetBuffTime() ? true : m_subtitle.GetBuffTime() >= m_BufferingTime) )
		{
			m_isstartbuffering = VO_FALSE;
		}
	}

	if( m_isbuffering && !m_has_eos )
	{
		if ( 0 == m_audio.GetBuffTime() && 0 == m_video.GetBuffTime() && 0 == m_subtitle.GetBuffTime() )
		{
			VOLOGI("VO_RET_SOURCE2_NEEDRETRY");
			return VO_RET_SOURCE2_NEEDRETRY;
		}
		else if ( (0 == m_audio.GetBuffTime() ? true : m_audio.GetBuffTime() >= m_BufferingTime) &&
			(0 == m_video.GetBuffTime() ? true : m_video.GetBuffTime() >= m_BufferingTime) &&
			(0 == m_subtitle.GetBuffTime() ? true : m_subtitle.GetBuffTime() >= m_BufferingTime) )
		{
			m_isbuffering = VO_FALSE;
		}
	}

	switch( buffertype )
	{
	case VO_SOURCE2_TT_VIDEO:
		ret = get_video( ( VO_SOURCE2_SAMPLE* )ptr_obj );
		break;
	case VO_SOURCE2_TT_AUDIO:
		ret = get_audio( ( VO_SOURCE2_SAMPLE* )ptr_obj );
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		ret = get_subtitle( ( voSubtitleInfo* )ptr_obj );
		break;
	}

	VOLOGR("TRACK_TYPE %d uRet %x, uSize %x, uFlag %d, uTime \t%lld", buffertype, ret, ( (VO_SOURCE2_SAMPLE*)ptr_obj )->uSize, ( (VO_SOURCE2_SAMPLE*)ptr_obj )->uFlag, ( (VO_SOURCE2_SAMPLE*)ptr_obj )->uTime);

	return ret;
}

void voSourceBufferManager::SetLibOp( VO_SOURCE2_LIB_FUNC * pLibOp )
{
	m_loader.SetLibOperator( ( VO_LIB_OPERATOR* )pLibOp );
}

VO_U32 voSourceBufferManager::add_audio( VO_SOURCE2_SAMPLE * ptr_sample )
{
	if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_EOS )
		m_has_eos = VO_TRUE;

	return m_audio.AddBuffer( ptr_sample );
}

VO_U32 voSourceBufferManager::add_video( VO_SOURCE2_SAMPLE * ptr_sample )
{
	if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_EOS )
		m_has_eos = VO_TRUE;

	VO_U32 r1 = m_video.AddBuffer( ptr_sample );

#ifdef _ENABLECC
	if( r1 == VO_RET_SOURCE2_OK )
	{
		if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT )
		{
			VO_SOURCE2_TRACK_INFO * ptr_info = ( VO_SOURCE2_TRACK_INFO * )ptr_sample->pFlagData;

			if( ptr_info->uCodec == VO_VIDEO_CodingH264 )
			{
				if( !m_isccloaded )
				{
					load_cc();

					if( m_ccapi.Open )
					{
						VO_CAPTION_PARSER_INIT_INFO info;
						info.nType = VO_CAPTION_TYPE_EIA608;
						info.stDataInfo.nSize = ptr_info->uHeadSize;
						info.stDataInfo.pHeadData = ptr_info->pHeadData;
						info.pReserved = 0;
						info.stDataInfo.nType = 0;

						m_ccapi.Open( &m_hcc , &info );
					}
				}
			}
		}
		else if( m_hcc && m_ccapi.Open )
		{
			VO_CAPTION_PARSER_INPUT_BUFFER inbuf;
			inbuf.nSize = ptr_sample->uSize;
			inbuf.nTimeStamp = ptr_sample->uTime;
			inbuf.pData = ptr_sample->pBuffer;

			m_ccapi.Process( m_hcc , &inbuf );

			VO_CAPTION_PARSER_OUTPUT_BUFFER outbuf;

			VO_U32 ret = m_ccapi.GetData( m_hcc , &outbuf );

			if( VO_ERR_PARSER_OK == ret )
			{
				//VOLOGR( "We got cc %lld %d %d" , outbuf.pSubtitleInfo->nTimeStamp , outbuf.pSubtitleInfo->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString[0] , outbuf.pSubtitleInfo->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->nSize );
				add_subtitle( outbuf.pSubtitleInfo );
			}

			if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_EOS )
			{
				m_ccapi.SetParam( m_hcc , VO_PARAMETER_ID_FLUSH , 0 );

				VO_U32 ret = m_ccapi.GetData( m_hcc , &outbuf );

				while( VO_ERR_PARSER_OK == ret )
				{
					//VOLOGR( "We got cc %lld %d %d" , outbuf.pSubtitleInfo->nTimeStamp , outbuf.pSubtitleInfo->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString[0] , outbuf.pSubtitleInfo->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->nSize );
					add_subtitle( outbuf.pSubtitleInfo );

					ret = m_ccapi.GetData( m_hcc , &outbuf );
				}
			}
		}
	}
#endif //#ifdef _ENABLECC

	return r1;
}

VO_U32 voSourceBufferManager::add_subtitle( voSubtitleInfo * ptr_sample )
{
	return m_subtitle.AddBuffer( ptr_sample );
}

VO_U32 voSourceBufferManager::get_audio( VO_SOURCE2_SAMPLE * ptr_sample )
{
	VO_U32 ret = m_audio.GetBuffer( ptr_sample );

	if( ret == VO_RET_SOURCE2_NEEDRETRY && !m_has_eos )
		m_isbuffering = VO_TRUE;

	if (VO_RET_SOURCE2_OK == ret)
		m_lastaudiotimestamp = ( (VO_SOURCE2_SAMPLE*)ptr_sample )->uTime;

	return ret;
}

VO_U32 voSourceBufferManager::get_video( VO_SOURCE2_SAMPLE * ptr_sample )
{
	if (m_ispureaudio)
	{
		VO_U64 ullTimeVideo = 0;
		if (m_video.GetFirstBufferTimeStamp(&ullTimeVideo) != VO_RET_SOURCE2_OK)
			return VO_RET_SOURCE2_NEEDRETRY;

		if (ullTimeVideo == 0 || ullTimeVideo < m_lastaudiotimestamp)
		{
			;
		}
		else if (ullTimeVideo - m_lastaudiotimestamp > 500)
		{
			return VO_RET_SOURCE2_NEEDRETRY;
		} 
		else
		{
			m_ispureaudio = VO_FALSE;
		}
	}

	VO_S32 iRet = m_video.GetBuffer( ptr_sample );

	if ( (ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE) && iRet == VO_RET_SOURCE2_OK)
		m_ispureaudio = VO_TRUE;

	return iRet;
}

VO_U32 voSourceBufferManager::get_subtitle( voSubtitleInfo * ptr_sample )
{
	VO_U32 ret = m_subtitle.GetBuffer( ptr_sample );

	/*if( ret == VO_RET_SOURCE2_OK )
	{
		VOLOGR( "We got cc %lld %d %d" , ptr_sample->nTimeStamp , ptr_sample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString[0] , ptr_sample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->nSize );
	}*/

	return ret;
}

VO_VOID voSourceBufferManager::load_cc()
{
#ifndef _IOS
	vostrcpy(m_loader.m_szDllFile, _T("voCaptionParser"));
	vostrcpy(m_loader.m_szAPIName, _T("voGetCaptionParserAPI"));

#if defined _WIN32
	vostrcat(m_loader.m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_loader.m_szDllFile, ".so");
#endif

	if(m_loader.LoadLib(NULL) == 0)
	{
		VOLOGR( "load fail!" );
		return;
	}

	pvoGetCaptionParserAPI pAPI = (pvoGetCaptionParserAPI) m_loader.m_pAPIEntry;
	if (pAPI == NULL)
	{
		VOLOGR( "get api fail!" );
		return;
	}

	pAPI( &m_ccapi );
#else //_IOS
	voGetCaptionParserAPI(&m_ccapi);
#endif //_IOS

	m_isccloaded = VO_TRUE;
}

void voSourceBufferManager::Flush()
{
	m_isbuffering = VO_TRUE;
	m_has_eos = VO_FALSE;
	m_ispureaudio = VO_FALSE;

	m_audio.Flush();
	m_video.Flush();
	m_subtitle.Flush();

	if( m_hcc && m_ccapi.Open )
	{
		m_ccapi.SetParam( m_hcc , VO_PARAMETER_ID_RESTART , 0 );

		VO_CAPTION_PARSER_OUTPUT_BUFFER outbuf;
		
		VO_U32 ret = m_ccapi.GetData( m_hcc , &outbuf );

		if( VO_ERR_PARSER_OK == ret )
		{
			outbuf.pSubtitleInfo->nTimeStamp = 0;
			add_subtitle( outbuf.pSubtitleInfo );
		}
	}
}

void voSourceBufferManager::set_pos( VO_U64 pos )
{
	m_isseek = VO_TRUE;
	m_seekpos = pos;
}

VO_U32 voSourceBufferManager::addbuffer_duringseek( VO_U32 buffertype , VO_PTR ptr_obj )
{
	VO_U32 ret = VO_RET_SOURCE2_OUTPUTNOTFOUND;

	switch( buffertype )
	{
	case VO_SOURCE2_TT_VIDEO:
		{
			VO_SOURCE2_SAMPLE * ptr_sample = ( VO_SOURCE2_SAMPLE* )ptr_obj;

			if( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_KEYFRAME )
			{
				m_video.RemoveTo( ptr_sample->uTime );
				m_audio.RemoveTo( ptr_sample->uTime );
				m_subtitle.RemoveTo( ptr_sample->uTime );
			}

			ret = add_video( ptr_sample );

			if( ( ret == VO_RET_SOURCE2_NEEDRETRY ) || 
				( ptr_sample->uTime >= m_seekpos ) ||
				( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_EOS ) )
				m_isseek = VO_FALSE;

			if( m_audio.GetBuffTime() >= m_nMaxBufferTime )
				m_isseek = VO_FALSE;

			if( !m_isseek )
			{
				VO_U64 timestamp = 0;
				if( VO_RET_SOURCE2_OK == m_video.GetFirstBufferTimeStamp( &timestamp ) )
					m_audio.RemoveTo( timestamp );
			}
		}
		break;
	case VO_SOURCE2_TT_AUDIO:
		{
			VO_SOURCE2_SAMPLE * ptr_sample = ( VO_SOURCE2_SAMPLE* )ptr_obj;

			ret = add_audio( ptr_sample );

			if( ( ret == VO_RET_SOURCE2_NEEDRETRY ) || 
				( ptr_sample->uFlag & VO_SOURCE2_FLAG_SAMPLE_EOS ) )
				m_isseek = VO_FALSE;

			if( !m_isseek )
			{
				if( m_video.GetBuffCount() == 0 )
					m_audio.RemoveTo( m_seekpos );
				else
				{
					VO_U64 timestamp = 0;
					if( VO_RET_SOURCE2_OK == m_video.GetFirstBufferTimeStamp( &timestamp ) )
						m_audio.RemoveTo( timestamp );
				}
			}
		}
		break;
	case VO_SOURCE2_TT_SUBTITLE:
		{
			ret = add_subtitle( ( voSubtitleInfo* )ptr_obj );
		}
		break;
	}

	return ret;
}
