#if !defined(_IOS)
	#ifdef _PUSHPLAYPARSER


#include "vo_file_parser.h"

#ifdef _AVI_READER
#include "CAviReader.h"
VO_U32 g_dwFRModuleID = VO_INDEX_SRC_AVI;
#endif	//_AVI_READER

#ifdef _ASF_READER
#include "CAsfReader.h"
VO_U32 g_dwFRModuleID = VO_INDEX_SRC_ASF;
#endif	//_ASF_READER

#ifdef _MP4_READER
#ifdef _USE_J3LIB
#include "CMp4Reader2.h"
#else	//_USE_J3LIB
#include "CMP4Reader.h"
#endif	//_USE_J3LIB
VO_U32 g_dwFRModuleID = VO_INDEX_SRC_MP4;
#endif	//_MP4_READER

#ifdef _AUDIO_READER
#include "CFileFormatCheck.h"
#include "CMp3Reader.h"
#include "CAacReader.h"
#include "CWavReader.h"
#include "CFlacReader.h"
#include "CQcpReader.h"
#include "CAmrReader.h"
#include "CAuReader.h"
#include "ape_reader.h"
#include "CAC3Reader.h"
VO_U32 g_dwFRModuleID = VO_INDEX_SRC_AUDIO;
#endif	//_AUDIO_READER

#ifdef _RM_READER
#include "CRealReader.h"
VO_U32 g_dwFRModuleID = VO_INDEX_SRC_REAL;
#endif	//_RM_READER

#ifdef _FLV_READER
#include "CFlvReader.h"
VO_U32 g_dwFRModuleID = VO_INDEX_SRC_FLASH;
#endif	//_FLV_READER

#ifdef _OGG_READER
#include "oggreader.h"
VO_U32 g_dwFRModuleID = VO_INDEX_SRC_OGG;
#endif  //_OGG_READER

#ifdef _MKV_READER
#include "CMKVFileReader.h"
VO_U32 g_dwFRModuleID = VO_INDEX_SRC_MKV;
#endif  //_MKV_READER

#ifdef _MPEG_READER
#include "CMpegReader.h"
VO_U32 g_dwFRModuleID = VO_INDEX_SRC_MPEG;
#endif	//_MPEG_READER


#define LOG_TAG "vo_file_parser"
#include "voLog.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

vo_file_parser::vo_file_parser()
:CvoBaseFileOpr(0)
,m_h_fileparser(0)
,get_sample(&vo_file_parser::nor_get_sample)
,set_pos(&vo_file_parser::nor_set_pos)
,m_mode(NORMALFILEPARSE)
,m_ptr_sample_thread(0)
,m_trackcount(0)
,m_is_stop(VO_FALSE)
,m_is_opening(VO_FALSE)
,m_seekthread_handle(0)
,m_firstseek_track(-1)
,m_is_seeking(VO_FALSE)
,m_videotrack(-1)
,m_audiotrack(-1)
,m_stop_getsample(VO_FALSE)
{
    m_opFile.Close		=	file_close;
    m_opFile.Flush		=	file_flush;
    m_opFile.Open		=	file_open;
    m_opFile.Read		=	file_read;
    m_opFile.Save		=	file_save;
    m_opFile.Seek		=	file_seek;
    m_opFile.Size		=	file_size;
    m_opFile.Write		=	file_write;
}

vo_file_parser::~vo_file_parser()
{
    ;
}

VO_U32 vo_file_parser::open(VO_SOURCE_OPENPARAM* pParam)
{
    if(VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE != (pParam->nFlag & 0xFF) && 
        (VO_SOURCE_OPENPARAM_FLAG_OPENPD != (pParam->nFlag & 0xFF)) &&
		(VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL != (pParam->nFlag & VO_SOURCE_OPENPARAM_FLAG_OPENFORTHUMBNAIL)) )    {
        VOLOGE("source open flags unvalid!!");
        return VO_ERR_INVALID_ARG;
    }

    if(VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR != (pParam->nFlag & 0xFF00))
    {
        VOLOGE("source operator flags unvalid!!");
        return VO_ERR_INVALID_ARG;
    }

    m_is_opening = VO_TRUE;
    VO_SOURCE_OPENPARAM param={0};//TODO

    m_ptr_org_opFile = ( VO_FILE_OPERATOR * )pParam->pSourceOP;
    VOLOGI( "%p" , m_ptr_org_opFile );
    m_pFileOp = m_ptr_org_opFile;
    m_org_filehandle = FileOpen( ( VO_FILE_SOURCE* )pParam->pSource );

    m_io.init( this , m_org_filehandle );
    
    memcpy( &param , pParam , sizeof( VO_SOURCE_OPENPARAM ) );

    VOLOGI( "%p" , (( VO_FILE_SOURCE* )pParam->pSource)->pSource );

    VO_FILE_SOURCE source;
    source.nFlag = param.nFlag;
    source.pSource = (( VO_FILE_SOURCE* )pParam->pSource)->pSource;
    source.nReserve = ( VO_U32 )this;
    param.pSourceOP = &m_opFile;
    param.pSource = &source;

    m_actualfilesize = (( VO_FILE_SOURCE* )pParam->pSource)->nLength;

    VOLOGI( "ActualFileSize: %lld   Now FileSize: %lld" , m_actualfilesize , m_io.get_filesize() );

	m_io.setRealFileSize(m_actualfilesize);

	if (VO_SOURCE_OPENPARAM_FLAG_OPENPD == (pParam->nFlag & 0xFF))
	{
		VOLOGI( "It is PD" );
		m_mode = PDFILEPARSER;
	}
	else if (m_actualfilesize != 0 && m_io.get_filesize() != m_actualfilesize)
    {
		VOLOGI( "It is push play!" );
		source.nFlag = source.nFlag | VO_SOURCE_OPENPARAM_FLAG_PUSHPLAY;
		m_mode = PUSHMODEFILEPARSE;
		m_io.set_mode( IO_CHECK );
    }
    else
    {
		VOLOGI( "It is normal playback!" );
		m_mode = NORMALFILEPARSE;
		m_io.set_mode( IO_NORMAL );
    }

    VO_U32 ret = open_internal( &param );

    judge_parsemode_fillfunction();

    if( m_mode == PUSHMODEFILEPARSE )
        init_asyncmode();

    m_is_opening = VO_FALSE;

    return ret;
}

VO_U32 vo_file_parser::get_fileinfo(VO_SOURCE_INFO* pSourceInfo)
{
    if(!m_h_fileparser)
        return VO_ERR_INVALID_ARG;

    CBaseReader* pReader = (CBaseReader*)m_h_fileparser;
    return pReader->GetInfo(pSourceInfo);
}

VO_U32 vo_file_parser::get_trackinfo(VO_U32 nTrack, VO_SOURCE_TRACKINFO* pTrackInfo)
{
    if(!m_h_fileparser)
        return VO_ERR_INVALID_ARG;

    VO_U32 ret = 0;

#ifndef _AUDIO_READER
    CBaseReader* pReader = (CBaseReader*)m_h_fileparser;
    CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
    if(!pTrack)
        return VO_ERR_INVALID_ARG;

    ret = pTrack->GetInfo(pTrackInfo);
#else	//_AUDIO_READER
    CBaseAudioReader* pReader = (CBaseAudioReader*)m_h_fileparser;
    ret = pReader->GetTrackInfo(pTrackInfo);
#endif	//_AUDIO_READER

    if( ret == 0 )
    {
        if( pTrackInfo->Type == VO_SOURCE_TT_VIDEO )
            m_videotrack = nTrack;
    }

    return ret;
}

VO_U32 vo_file_parser::set_fileparam(VO_U32 uID, VO_PTR pParam)
{
    if(!m_h_fileparser)
        return VO_ERR_INVALID_ARG;

    CBaseReader* pReader = (CBaseReader*)m_h_fileparser;
    return pReader->SetParameter(uID, pParam);
}

VO_U32 vo_file_parser::get_fileparam(VO_U32 uID, VO_PTR pParam)
{
    if(!m_h_fileparser)
        return VO_ERR_INVALID_ARG;

    switch ( uID )
    {
    case VO_PID_SOURCE_SEEKABLETIME:
        {
            VOLOGI( "Get SEEKABLETIME");

            if( m_mode == NORMALFILEPARSE )
                return VO_ERR_NOT_IMPLEMENT;

            VO_FILE_MEDIATIMEFILEPOS timepos;
            memset( &timepos , 0 , sizeof( VO_FILE_MEDIATIMEFILEPOS ) );
            timepos.llFilePos = m_io.get_filesize();
            VOLOGI( "The File pos: %lld" , timepos.llFilePos );
            VO_U32 ret = get_fileparam( VO_PID_SOURCE_GETSEEKTIMEBYPOS , &timepos );

            if( ret == 0 )
            {
                VO_S64 * ptr_time = (VO_S64*) pParam;
                *ptr_time = timepos.llMediaTime;
                VOLOGI( "It can seek to %lld" , *ptr_time );
            }
            else
            {
                ret = VO_ERR_BASE;
                VOLOGE("It can not seek return %x!", VO_ERR_BASE);
            }

            return ret;
        }
        break;
    }

    CBaseReader* pReader = (CBaseReader*)m_h_fileparser;
    return pReader->GetParameter(uID, pParam);
}

VO_U32 vo_file_parser::set_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
    if(!m_h_fileparser)
        return VO_ERR_INVALID_ARG;

#ifndef _AUDIO_READER
    CBaseReader* pReader = (CBaseReader*)m_h_fileparser;
    CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
    if(!pTrack)
        return VO_ERR_INVALID_ARG;

    return pTrack->SetParameter(uID, pParam);
#else	//_AUDIO_READER
    CBaseAudioReader* pReader = (CBaseAudioReader*)m_h_fileparser;
    return pReader->SetTrackParameter(uID, pParam);
#endif	//_AUDIO_READER
}

VO_U32 vo_file_parser::get_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
    if(!m_h_fileparser)
        return VO_ERR_INVALID_ARG;

#ifndef _AUDIO_READER
    CBaseReader* pReader = (CBaseReader*)m_h_fileparser;
    CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
    if(!pTrack)
        return VO_ERR_INVALID_ARG;

    return pTrack->GetParameter(uID, pParam);
#else	//_AUDIO_READER
    CBaseAudioReader* pReader = (CBaseAudioReader*)m_h_fileparser;
    return pReader->GetTrackParameter(uID, pParam);
#endif	//_AUDIO_READER
}


VO_U32 vo_file_parser::close()
{
    m_is_stop = VO_TRUE;

    m_io.set_to_close(VO_TRUE);

    while( m_is_opening )
        voOS_Sleep( 20 );

    stop_getsamplethread();

    if(!m_h_fileparser)
        return VO_ERR_INVALID_ARG;

    CBaseReader* pReader = (CBaseReader*)m_h_fileparser;
    VO_U32 rc = pReader->Close();
    delete pReader;
    return rc;
}

VO_U32 vo_file_parser::nor_get_sample(VO_U32 nTrack, VO_SOURCE_SAMPLE* pSample)
{
    if(!m_h_fileparser)
        return VO_ERR_INVALID_ARG;

#ifndef _AUDIO_READER
    CBaseReader* pReader = (CBaseReader*)m_h_fileparser;
    CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
    if(!pTrack)
    {
        VOLOGI("error track index: %d!!", nTrack);

        return VO_ERR_INVALID_ARG;
    }

    if(pTrack->IsEndOfStream())
    {
        VOLOGI("Type %d track is end!!", pTrack->GetType());

        return VO_ERR_SOURCE_END;
    }

    pSample->Flag = 0;
    VO_U32 rc = pTrack->GetSample(pSample);
    if(VO_ERR_SOURCE_OK != rc)
    {
        if(VO_ERR_SOURCE_END == rc)
            pTrack->SetEndOfStream(VO_TRUE);

        VOLOGI("get sample error: 0x%08X!!", rc);

        return rc;
    }

    VO_TRACKTYPE nTrackType = pTrack->GetType();
    if(VOTT_VIDEO == nTrackType)
        pReader->CheckVideo(pSample);
    else if(VOTT_AUDIO == nTrackType)
        pReader->CheckAudio(pSample);
#else	//_AUDIO_READER
    CBaseAudioReader* pReader = (CBaseAudioReader*)m_h_fileparser;;
    if(pReader->IsEndOfStream())
    {
        VOLOGI("track is end!!");

        return VO_ERR_SOURCE_END;
    }

    VO_U32 rc = pReader->GetSample(pSample);
    if(VO_ERR_SOURCE_OK != rc)
    {
        if(VO_ERR_SOURCE_END == rc)
            pReader->SetEndOfStream(VO_TRUE);

        VOLOGI("get sample error: 0x%08X!!", rc);

        return rc;
    }

    pReader->CheckAudio(pSample);
#endif	//_AUDIO_READER

    VOLOGR("track type %d getsample ok, size: 0x%08X, time: %d!!", (VO_U32)pTrack->GetType(), pSample->Size, pSample->Time);

    return VO_ERR_SOURCE_OK;
}

VO_U32 vo_file_parser::nor_set_pos(VO_U32 nTrack, VO_S64* pPos)
{
    if(!m_h_fileparser)
        return VO_ERR_INVALID_ARG;

    VOLOGI("%d track want to setpos %d!!", nTrack, VO_S32(*pPos));
#ifndef _AUDIO_READER
    CBaseReader* pReader = (CBaseReader*)m_h_fileparser;
    CBaseTrack* pTrack = pReader->GetTrackByIndex(nTrack);
    if(!pTrack)
    {
        VOLOGI("error track index: %d!!", nTrack);

        return VO_ERR_INVALID_ARG;
    }

    if(VO_SOURCE_PM_PLAY == pTrack->GetPlayMode())
    {
        if(-1 == pReader->GetSeekTrack())	//not initialize!!
        {
            VOLOGI("set seek track: %d!!", nTrack);

            pReader->SetSeekTrack(nTrack);
        }

        if(pReader->GetSeekTrack() == nTrack)	//the track will activate file set position
        {
            VOLOGI("%d track activate moveto %d!!", nTrack, VO_S32(*pPos));

            pReader->MoveTo(*pPos);
        }
    }

    pTrack->SetEndOfStream(VO_FALSE);

    VO_U32 rc = pTrack->SetPos(pPos);
    if(VO_ERR_SOURCE_END == rc)
        pTrack->SetEndOfStream(VO_TRUE);
#else	//_AUDIO_READER
    CBaseAudioReader* pReader = (CBaseAudioReader*)m_h_fileparser;
    pReader->SetEndOfStream(VO_FALSE);

    VO_U32 rc = pReader->SetPos(pPos);
    if(VO_ERR_SOURCE_END == rc)
        pReader->SetEndOfStream(VO_TRUE);
#endif	//_AUDIO_READER
    VOLOGI("setpos %d return 0x%08X!!", VO_S32(*pPos), rc);

	for( VO_S32 i = 0 ; i < m_trackcount ; i++ )
	{
		if(m_ptr_sample_thread)
		{
			m_ptr_sample_thread[i].dropframe_timestamp = 0;
		}
	}

    return rc;
}

VO_U32 vo_file_parser::async_get_sample(VO_U32 nTrack, VO_SOURCE_SAMPLE* pSample)
{
	//VOLOGF("m_sem_seek.Count() %u m_is_seeking %d", m_sem_seek.Count(), m_is_seeking);
    GetSampleThreadInfo * ptr_info = m_ptr_sample_thread + nTrack;
    if( ptr_info->thread_handle == 0 )
    {
        unsigned int thread_id;
        SampleThreadCreateInfo * ptr_threadcreate_info = new SampleThreadCreateInfo;
        ptr_threadcreate_info->ptr_obj = this;
        ptr_threadcreate_info->ptr_thread_info = ptr_info;
        create_thread( &( ptr_info->thread_handle ) , &thread_id , getsample_threadfunc , ptr_threadcreate_info , 0 );
        ptr_info->m_sem_sample.Up();
    }

    if( ptr_info->m_sem_sample.Count() > 0 )
        return VO_ERR_SOURCE_NEEDRETRY;

    {
        voCAutoLock lock( &(ptr_info->sample_lock) );
        ptr_info->m_sem_sample.Up();

        if( ptr_info->sample.Size != 0 )
        {
            pSample->Duration = ptr_info->sample.Duration;
            pSample->Flag = ptr_info->sample.Flag;
            pSample->Size = ptr_info->sample.Size;
            pSample->Time = ptr_info->sample.Time;
            pSample->Buffer = ptr_info->ptr_buffer;

            memcpy( ptr_info->ptr_buffer , ptr_info->sample.Buffer , 0x7fffffff & ptr_info->sample.Size );
        }

        return ptr_info->ret_getsample;
    }

    return 0;
}

VO_U32 vo_file_parser::async_set_pos(VO_U32 nTrack, VO_S64* pPos)
{
	stop_getsamplethread();
	return nor_set_pos(nTrack, pPos);
/*
	VOLOGF("set_pos to %lld", *pPos);
    if( !m_seekthread_handle )
    {
        unsigned int thread_id;
        create_thread( &m_seekthread_handle , &thread_id , seek_threadfunc , this , 0 );
    }

    if( m_firstseek_track == -1 )
        m_firstseek_track = nTrack;

    if( m_firstseek_track == nTrack )
    {
        voCAutoLock lock(&m_seekpos_lock);
        m_seek_pos = *pPos;
        m_sem_seek.Up();
    }

    return VO_ERR_SOURCE_OK;
*/
}

VO_U32 vo_file_parser::open_internal(VO_SOURCE_OPENPARAM * pParam)
{
#ifdef _AVI_READER
    CAviReader* pReader = new CAviReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#endif	//_AVI_READER
#ifdef _ASF_READER
    CAsfReader* pReader = new CAsfReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#endif	//_ASF_READER
#ifdef _MP4_READER
#ifdef _USE_J3LIB
    CMp4Reader2* pReader = new CMp4Reader2((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#else	//_USE_J3LIB
    CMp4Reader* pReader = new CMp4Reader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#endif	//_USE_J3LIB
#endif	//_MP4_READER
#ifdef _AUDIO_READER
    //check file format
    CFileFormatCheck checker((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP);
    VO_FILE_FORMAT ff = checker.GetFileFormat((VO_FILE_SOURCE*)pParam->pSource, FLAG_CHECK_AUDIOREADER);
    //new file parser!!
    CBaseAudioReader* pReader = VO_NULL;
    if(VO_FILE_FFAUDIO_MP3 == ff)
        pReader = new CMp3Reader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    else if(VO_FILE_FFAUDIO_AAC == ff)
        pReader = new CAacReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    else if(VO_FILE_FFAUDIO_WAV == ff)
        pReader = new CWavReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    else if(VO_FILE_FFAUDIO_AMR == ff || VO_FILE_FFAUDIO_AWB == ff)
        pReader = new CAmrReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    else if(VO_FILE_FFAUDIO_QCP == ff)
        pReader = new CQcpReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    else if(VO_FILE_FFAUDIO_FLAC == ff)
        pReader = new CFlacReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    else if(VO_FILE_FFAUDIO_AU == ff)
        pReader = new CAuReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    else if( VO_FILE_FFAUDIO_APE == ff )
        pReader = new ape_reader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    else if(VO_FILE_FFAUDIO_AC3 == ff)
        pReader = new CAC3Reader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
    else
    {
        VOLOGE("Unknow file format!!");
        return VO_ERR_SOURCE_FORMATUNSUPPORT;
    }

#endif	//_AUDIO_READER
#ifdef _RM_READER
    CRealReader* pReader = new CRealReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#endif	//_RM_READER
#ifdef _FLV_READER
    CFlvReader* pReader = new CFlvReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#endif	//_FLV_READER
#ifdef _OGG_READER
    oggreader * pReader = new oggreader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#endif  //_OGG_READER
#ifdef _MKV_READER
    CMKVFileReader * pReader = new CMKVFileReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#endif  //_MKV_READER
#ifdef _MPEG_READER
    CMpegReader* pReader = new CMpegReader((VO_FILE_OPERATOR*)pParam->pSourceOP, pParam->pMemOP, pParam->pLibOP, pParam->pDrmCB);
#endif	//_MPEG_READER
    if(!pReader)
    {
        VOLOGE("create reader fail!!");
        return VO_ERR_OUTOF_MEMORY;
    }

    VO_U32 rc = pReader->Load(pParam->nFlag, (VO_FILE_SOURCE*)pParam->pSource);
    //only OK/DRM can be playback
    if(VO_ERR_SOURCE_OK != rc && VO_ERR_SOURCE_CONTENTENCRYPT != rc)
    {
        VOLOGE("file parser load fail: 0x%08X!!", rc);
        delete pReader;
        return rc;
    }

    m_h_fileparser = pReader;
    return rc;
}

VO_VOID vo_file_parser::judge_parsemode_fillfunction()
{
    if(m_mode != PDFILEPARSER && m_actualfilesize == 0 && get_fileparam( VO_PID_SOURCE_ACTUALFILESIZE , &m_actualfilesize ) == 0 )
    {
        if( m_actualfilesize <= m_io.get_filesize() )
        {
            VOLOGI( "It is normal playback!" );
            m_mode = NORMALFILEPARSE;
        }
        else
        {
            VOLOGI( "It is push play!" );
            m_mode = PUSHMODEFILEPARSE;
        }
    }

    if( m_mode == PDFILEPARSER || m_mode == NORMALFILEPARSE )
    {
        get_sample = &vo_file_parser::nor_get_sample;
        set_pos = &vo_file_parser::nor_set_pos;

        m_io.set_mode(IO_NORMAL );
    }
    else if( m_mode == PUSHMODEFILEPARSE )
    {
        get_sample = &vo_file_parser::async_get_sample;
        set_pos = &vo_file_parser::async_set_pos;

        m_io.set_mode( IO_CHECK );
    }
}

VO_VOID vo_file_parser::init_asyncmode()
{
    if( m_mode != PUSHMODEFILEPARSE )
        return;

    VO_SOURCE_INFO info;
    get_fileinfo( &info );

    m_trackcount = info.Tracks;
    m_ptr_sample_thread = new GetSampleThreadInfo[ m_trackcount ];

    for( VO_S32 i = 0 ; i < m_trackcount ; i++ )
    {
        VO_U32 max_size = 0;
        get_trackparam( i , VO_PID_SOURCE_MAXSAMPLESIZE , (VO_PTR)&max_size );
        
        m_ptr_sample_thread[i].thread_handle = 0;
        m_ptr_sample_thread[i].dropframe_timestamp = 0;

        m_ptr_sample_thread[i].ptr_buffer = new VO_BYTE[max_size+1];
        m_ptr_sample_thread[i].track_number = i;

        VO_SOURCE_SELTRACK sel_track;
        sel_track.nIndex = i;
        sel_track.bInUsed = VO_FALSE;
        get_fileparam( VO_PID_SOURCE_SELTRACK , &sel_track );

        if( sel_track.bInUsed )
        {
            VO_SOURCE_TRACKINFO track_info;
            get_trackinfo( i , &track_info );
            if( track_info.Type == VO_SOURCE_TT_AUDIO )
                m_audiotrack = i;
            else
                m_videotrack = i;
        }
    }
}

unsigned int vo_file_parser::getsample_threadfunc( void * pParam )
{
    SampleThreadCreateInfo * ptr_threadcreate_info = (SampleThreadCreateInfo *)pParam;

    vo_file_parser * ptr_obj = ptr_threadcreate_info->ptr_obj;
    GetSampleThreadInfo * ptr_info = ptr_threadcreate_info->ptr_thread_info;

    while( !ptr_obj->m_is_stop || !ptr_obj->m_stop_getsample )
    {
        ptr_info->m_sem_sample.Down();

        if( ptr_obj->m_is_stop || ptr_obj->m_stop_getsample || ptr_info->ret_getsample)
            break;

        {
            voCAutoLock lock( &(ptr_info->sample_lock) );

            ptr_info->sample.Time = ptr_info->dropframe_timestamp;

            {
                voCAutoLock lock( &ptr_obj->m_getsamplelock );
                ptr_info->ret_getsample = ptr_obj->nor_get_sample( ptr_info->track_number , &(ptr_info->sample) );
            }

            if( ptr_info->ret_getsample == 0 )
            {
                ptr_info->dropframe_timestamp = ptr_info->sample.Time;
            }
        }
    }

    exit_thread();

    return 0;
}

unsigned int vo_file_parser::seek_threadfunc( void * pParam )
{
    vo_file_parser * ptr_obj = (vo_file_parser *)pParam;

    while( !ptr_obj->m_is_stop )
    {
        ptr_obj->m_sem_seek.Down();

        if( ptr_obj->m_is_stop )
            break;

        ptr_obj->m_is_seeking = VO_TRUE;
        VO_S64 seekpos;

        {
            voCAutoLock lock(&ptr_obj->m_seekpos_lock);
            ptr_obj->m_sem_seek.Reset();
            seekpos = ptr_obj->m_seek_pos;
        }

        ptr_obj->stop_getsamplethread();

        if( ptr_obj->m_videotrack >= 0 )
            ptr_obj->nor_set_pos( ptr_obj->m_videotrack , &seekpos );

        ptr_obj->nor_set_pos( ptr_obj->m_audiotrack , &seekpos );

        ptr_obj->m_is_seeking = VO_FALSE;
    }

    exit_thread();
    return 0;
}

VO_VOID vo_file_parser::stop_getsamplethread()
{
    m_stop_getsample = VO_TRUE;

    for( VO_S32 i = 0 ; i < m_trackcount ; i++ )
    {
        if( m_ptr_sample_thread )
        {
            if( m_ptr_sample_thread[i].thread_handle )
            {
                m_ptr_sample_thread[i].m_sem_sample.Up();
                wait_thread_exit( m_ptr_sample_thread[i].thread_handle );
                m_ptr_sample_thread[i].thread_handle = 0;
            }
            m_ptr_sample_thread[i].sample.Size = 0;
			m_ptr_sample_thread[i].sample.Time = 0;
            m_ptr_sample_thread[i].ret_getsample = 0;
            m_ptr_sample_thread[i].m_sem_sample.Reset();
        }
    }

    m_stop_getsample = VO_FALSE;
}


	#endif
#endif
