
#include "oggreader.h"
#include "oggparser.h"
#include "ogg_vorbis_track.h"
#include "voMetaData.h"

#define LOG_TAG "oggreader"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

static VO_U8 GOnData(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	oggreader* ptr_reader = (oggreader*)pUser;
	ogg_packet* ptr_ogg_packet = (ogg_packet*)pParam;

	return ptr_reader->on_receive_packet( ptr_ogg_packet );
}

/*
static VO_BOOL GOnBlock(VO_PTR pUser, VO_PTR pUserData, VO_PTR pParam)
{
	//CMKVTrack* pTrack = (CMKVTrack*)pUser;
	//return pTrack->OnBlockWithFilePos(*(VO_U64*)pParam);
	return VO_TRUE; 
}
*/

//return vo_false means we do not expect this stream
static VO_BOOL GOnNewPageArrived( VO_PTR pUser, VO_U32 stream_id , VO_U64 granule_pos )
{
	oggreader* ptr_reader = (oggreader*)pUser;
	return ptr_reader->notify_new_page( stream_id , granule_pos );
}

oggreader::oggreader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB):CBaseStreamFileReader( pFileOp , pMemOp, pLibOP, pDrmCB )
, m_header_parser( &m_chunk , pMemOp )
, m_data_parser( &m_chunk , pMemOp )
, m_ptr_audio_track( VO_NULL )
, m_ptr_video_track( VO_NULL )
{
	m_pFileDataParser = &m_data_parser;
}

oggreader::~oggreader()
{
	;
}

VO_U32 oggreader::Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource)
{
	VO_U32 rc = CBaseReader::Load(nSourceOpenFlags, pFileSource);
	if(VO_ERR_SOURCE_OK != rc)
    {
		return rc; 
    }

    VO_BOOL is_scan = VO_FALSE;

    if( nSourceOpenFlags & VO_SOURCE_OPENPARAM_FLAG_INFOONLY )
        is_scan = VO_TRUE;

    m_header_parser.set_filesize( m_ullFileSize );
	m_header_parser.ReadFromFile( is_scan );

	const track_info * ptr_vorbis_track = m_header_parser.get_vorbis_track_info();

	if( !ptr_vorbis_track )
    {
		return VO_ERR_SOURCE_FORMATUNSUPPORT;
    }

	VO_U64 duration = (ptr_vorbis_track->ptr_index_tail->granule_pos * 1000);
	if (duration % ptr_vorbis_track->info.audio_sample_rate) {
		 duration /= ptr_vorbis_track->info.audio_sample_rate;
		 duration++;
	} else {
		 duration /= ptr_vorbis_track->info.audio_sample_rate;
	}

	m_ptr_audio_track = new ogg_vorbis_track( VOTT_AUDIO, 1, (VO_U32)(duration), this, m_pMemOp );

	m_ptr_audio_track->set_trackinfo( ptr_vorbis_track );

	InitTracks( nSourceOpenFlags);
	SelectDefaultTracks();
	PrepareTracks();

	m_data_parser.Init(VO_NULL);
	m_data_parser.SetStartFilePos( 0 );
	m_data_parser.SetOnDataCallback( this , VO_NULL , GOnData );
	m_data_parser.set_new_page_arrive_callback( this , GOnNewPageArrived );
	FileGenerateIndex();

	return VO_ERR_SOURCE_OK;
}

VO_U32 oggreader::InitTracks(VO_U32 nSourceOpenFlags)
{
	TracksCreate( 2 );
	m_nTracks = 2;

	int count = 0;
	if( m_ptr_audio_track )
	{
		m_ppTracks[count] = m_ptr_audio_track;
		count++;
	}

	if( m_ptr_video_track )
	{
		m_ppTracks[count] = m_ptr_video_track;
		count++;
	}

	m_nTracks = count;

	return CBaseReader::InitTracks(nSourceOpenFlags);
}

VO_BOOL oggreader::notify_new_page( VO_U32 stream_id , VO_U64 granule_pos )
{
	if( m_ptr_audio_track )
	{
		if( m_ptr_audio_track->get_stream_id() == stream_id )
		{
			m_ptr_audio_track->set_new_page( granule_pos );
			return VO_TRUE;
		}
	}

	if( m_ptr_video_track )
	{
		if( m_ptr_video_track->get_stream_id() == stream_id )
		{
			m_ptr_video_track->set_new_page( granule_pos );
			return VO_TRUE;
		}
	}

	return VO_FALSE;
}

VO_U8 oggreader::on_receive_packet( const ogg_packet* ptr_ogg_packet )
{
	if( m_ptr_audio_track && ptr_ogg_packet->serial_number == m_ptr_audio_track->get_stream_id() )
	{
		if( !m_ptr_audio_track->on_receive_packet( ptr_ogg_packet ) )
			return CBRT_FALSE;
	}
	else if( m_ptr_video_track && ptr_ogg_packet->serial_number == m_ptr_video_track->get_stream_id() )
	{
		if( !m_ptr_video_track->on_receive_packet( ptr_ogg_packet ) )
			return CBRT_FALSE;
	}
	else
	{
		return CBRT_FALSE;
	}

	return CBRT_CONTINUABLENOTNEEDSKIP;
}

VO_U32 oggreader::Close()
{
    UnprepareTracks();
	UninitTracks();

	delete m_ptr_audio_track;
	m_ptr_audio_track = NULL;

	delete m_ptr_video_track;
	m_ptr_video_track = NULL;

	return VO_ERR_SOURCE_OK;
}

VO_U32 oggreader::MoveTo(VO_S64 llTimeStamp)
{
	VO_U64 file_pos = 0;

	Flush();

	if( m_ptr_audio_track )
	{
		file_pos = m_ptr_audio_track->get_seek_pos( llTimeStamp );
	}

	if( m_ptr_video_track )
	{
		VO_U64 temp_pos = m_ptr_video_track->get_seek_pos( llTimeStamp );

		if(  temp_pos < file_pos )
			file_pos = temp_pos;
	}

	m_data_parser.seek_to( file_pos );

	FileGenerateIndex();

	return CBaseReader::MoveTo(llTimeStamp);
}

VO_VOID	oggreader::Flush()
{
	m_data_parser.Flush();
	return CBaseStreamFileReader::Flush();
}

VO_U32 oggreader::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch ( uID )
	{
	case VO_PID_METADATA_TRACK:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_header_parser.get_track_number();
			ptr_str->dwBufferSize = strlen( (VO_PCHAR)ptr_str->pBuffer );
		}
		break;
	case VO_PID_METADATA_TITLE:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_header_parser.get_title();
			ptr_str->dwBufferSize = strlen( (VO_PCHAR)ptr_str->pBuffer );
		}
		break;
	case VO_PID_METADATA_ARTIST:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_header_parser.get_artist();
			ptr_str->dwBufferSize = strlen( (VO_PCHAR)ptr_str->pBuffer );
		}
		break;
	case VO_PID_METADATA_ALBUM:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_header_parser.get_album();
			ptr_str->dwBufferSize = strlen( (VO_PCHAR)ptr_str->pBuffer );
		}
		break;
	case VO_PID_METADATA_GENRE:
		{
			PMetaDataString ptr_str = (PMetaDataString)pParam;
			ptr_str->dwFlags = VO_METADATA_TE_UTF8;
			ptr_str->pBuffer = (VO_PBYTE)m_header_parser.get_genre();
			ptr_str->dwBufferSize = strlen( (VO_PCHAR)ptr_str->pBuffer );
		}
		break;
	case VO_PID_SOURCE_NEEDLOOPPLAY:
		{
			VO_BOOL * ptr_loop = ( VO_BOOL * )pParam;
			*ptr_loop = m_header_parser.get_needloop();
		}
		break;
	case VO_PID_METADATA_FRONTCOVER:
		{
			PMetaDataImage ptr_image = (PMetaDataImage)pParam;
			PMetaDataImage ptr = m_header_parser.get_frontcover();

			if( ptr )
			{
				ptr_image->nImageType = ptr->nImageType;
				ptr_image->dwBufferSize = ptr->dwBufferSize;
				ptr_image->pBuffer = ptr->pBuffer;
			}
		}
		break;
	default:
		return CBaseStreamFileReader::GetParameter( uID , pParam );
	}

	return 0;
}




