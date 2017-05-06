#include "CISSControl.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voThread.h"

#include "voVideo.h"


//#define	_TEST
//#define DUMP_FILE
FILE *out_video = NULL;
FILE *out_audio = NULL;
unsigned char CHANGE_CODE[]={ 0x01,0x02,0x03,0x04};

static const VO_CHAR SYNC_HEADER[] = {0x00,0x00,0x00,0x01};
#define be32toh(x) 	((x &0xff000000)>> 24 | (x & 0xff0000) >>16 << 8 | (x &0xff00) >>8 <<16 | (x & 0xff)<<24)

typedef VO_U32 (VO_API * voGetISSLiveSrcPlusAPI) (VO_LIVESRC_PLUS_API* pHandle);

CISSControl::CISSControl(void)
:m_VideoBuffer(1024*64, 0)
,m_AudioBuffer(1024*32, 0)
{

	m_bMediaStop = VO_FALSE;

	m_recoverfrompause = VO_FALSE;
	m_is_flush = VO_FALSE;
	m_is_pause = VO_FALSE;
	m_is_seek = VO_FALSE;
	m_is_H264 = VO_FALSE;
	m_is_first = VO_TRUE;
	download_bitrate = 0;
	m_lastFregmentDuration = 0;

	start_time = 0;
	seek_time = 0;
	m_seeked = VO_FALSE;
	m_bNeedFlushBuffer = VO_FALSE;

	m_nLastTimeStamp_Video = 0;
	m_nLastTimeStamp_Audio = 0;
	m_nCurTimeStamp_Audio = 0;
	m_nCurTimeStamp_Video = 0;
	m_audioCodecType = 0;
	m_videoCodecType = 0;
	m_isPushMode = VO_FALSE;

	m_pStreamInfo = NULL;
	m_livePlusHandle = NULL;
	m_pDrmValue = NULL;
	m_pLibOp = NULL;
	m_uStreamCounts = 0;
	m_nCulStreamID_audio = -1;
	m_nCulStreamID_video = -1;
	memset (&m_liveSrcPlus, 0, sizeof (VO_LIVESRC_PLUS_API));
	strcpy (m_szWorkPath, "");

#ifdef __DUMP
	video_ff = NULL;
	audio_ff = NULL;
#endif
}

CISSControl::~CISSControl(void)
{
	freeLiveSrcPlus();
	memset (&m_liveSrcPlus, 0, sizeof (VO_LIVESRC_PLUS_API));
}
VO_VOID CISSControl::NotifyData()
{
}
VO_U32 CISSControl::GetSample( VO_U32 nOutPutType , VO_PTR pSample )
{
	VO_U32 nRc = 0;
	if(VO_SOURCE2_TT_VIDEO == nOutPutType)
	{
		voCAutoLock lock(&m_voMemMutexVideo);
		nRc = m_VideoBuffer.GetBuffer((VO_SOURCE2_SAMPLE*)pSample);
	}
	if(VO_SOURCE2_TT_AUDIO == nOutPutType)
	{
		voCAutoLock lock(&m_voMemMutexAudio);
		nRc = m_AudioBuffer.GetBuffer((VO_SOURCE2_SAMPLE*)pSample);
	}

//	VOLOGI("GetSample(%d):size %08x, time %lld.  %08x", nOutPutType, ((VO_SOURCE2_SAMPLE*)pSample)->Size, ((VO_SOURCE2_SAMPLE*)pSample)->Time, nRc);
	return nRc;
}

void CISSControl::thread_function()
{
	VOLOGI("Start thread!");
	start_smoothstream();
}

VO_VOID CISSControl::start_smoothstream()
{
	VOLOGI("start_smoothstream!");
	VO_S32 ret = 0;
	VO_CHAR url_fragment[1024];

	VO_S32 fragment_number = 1;
	VO_U64 update_time = 0;
	VO_S64 sleep_time = 0;
	VO_S64 seek_fragment_time = 0;
	VO_S64 last_fragment_time = 0;
	VO_BOOL live_after_pause = VO_FALSE;

//	GetProtectionLicense(&m_manager.m_manifest.piff);

	seek_fragment_time = seek_time; 
	FragmentItem * ptr_item = m_manager.GetFirstChunk();
/*	if(m_is_seek)
	{
		VOLOGI("is seek pos: %lld", seek_time);

		m_manager.m_reader.get_fragmentItem( &ptr_item, seek_time );
		need_flush();
		m_is_seek = VO_FALSE;
	}*/

	if( !ptr_item)
		return;

	if(m_audioCodecType > 0 )
	{	
		VO_U64 nStartTime =0;
		m_nLastTimeStamp_Audio = nStartTime;

		m_manager.bitrate_changed_Audio = VO_TRUE;
	}

	if(1 ) //on-demand
	{	
		while( !m_bMediaStop && ptr_item)
		{
		//	voCAutoLock lock (&m_voMemMutex_forSeek);
			if(m_is_seek)
			{
				VOLOGI("is seek pos: %lld", seek_time);
				m_manager.GetFragmentItem( &ptr_item, seek_time );
				need_flush();
				m_is_seek = VO_FALSE;

				m_manager.bitrate_changed_Video = VO_TRUE;
				m_manager.bitrate_changed_Audio = VO_TRUE;
			}
			//VOLOGI(" chunk starttime: %lld,streamindex %d", ptr_item->starttime, ptr_item->streamindex);
			
			start_time = ptr_item->starttime;
			memset(url_fragment, 0, 1024);

			ret = m_manager.pop_fragment(ptr_item, url_fragment, download_bitrate,m_lastFregmentDuration);

			if( ret == -1 || m_bMediaStop )
			{
				break;
			}
			if( m_manager.m_video_index != -1 && ptr_item->streamindex == m_manager.m_video_index )
			{
				m_nLastTimeStamp_Video = ptr_item->starttime/ m_manager.GetTimeScaleMs() -1;
				download_bitrate =0;
				download_bitrate = GetMediaItem( url_fragment, ptr_item->streamindex );
			}
			else if(m_manager.m_audio_index != -1 && ptr_item->streamindex == m_manager.m_audio_index)
			{
				m_nLastTimeStamp_Audio = ptr_item->starttime /m_manager.GetTimeScaleMs() -1;
				download_bitrate += GetMediaItem( url_fragment, ptr_item->streamindex );
			}

			//VOLOGI("Fragment download finish: F_Number:%d,D_Cost:%llds", fragment_number,download_bitrate);

			ptr_item = m_manager.GetNextChunk(ptr_item);

			fragment_number++;

		}
	}
	else //live mode
	{
		;
	}

	VOLOGI("Ended!");
	send_eos();
	VOLOGI( "Finished!" );

	VOLOGI( "End of start_livestream" );
}

VO_VOID CISSControl::stop_smoothstream( VO_BOOL isflush )
{
	if( isflush )
		VOLOGI("it is flush");

	VOLOGI("+stop_livestream");
	m_is_flush = isflush;
	
	m_bMediaStop = VO_TRUE;
	m_is_first = VO_TRUE;
	m_is_pause =VO_FALSE;

	if( m_is_flush )
	{
		need_flush();
	}	
	vo_thread::stop();

	m_is_flush = VO_FALSE;
	m_bMediaStop = VO_FALSE;

	m_lastFregmentDuration = 0;
	download_bitrate = 0;

	m_manager.last_QL_bitrate = -1;
	VOLOGI("-stop_livestream");
}


VO_U32 CISSControl::LoadISSPlus()
{
	VOLOGI("+LoadISSPlus");

	if (strlen (m_szWorkPath) > 0)
		m_dlEngine.SetWorkPath ((VO_TCHAR*)m_szWorkPath);

	VOLOGI ("Work path %s", m_szWorkPath);

	vostrcpy(m_dlEngine.m_szDllFile, _T("voLiveSrcISS"));
	vostrcpy(m_dlEngine.m_szAPIName, _T("voGetISSLiveSrcPlusAPI"));

#if defined _WIN32
	vostrcat(m_dlEngine.m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_dlEngine.m_szDllFile, _T(".so"));
#elif defined _MAC_OS
	vostrcat(m_dlEngine.m_szDllFile, _T(".dylib"));
#endif



	if(m_dlEngine.LoadLib(NULL) == 0)
	{
		VOLOGE ("LoadLib fail");
		return VO_RET_SOURCE2_FAIL;
	}
	
	voGetISSLiveSrcPlusAPI pAPI = (voGetISSLiveSrcPlusAPI) m_dlEngine.m_pAPIEntry;
	if (pAPI == NULL)
	{
		return VO_RET_SOURCE2_FAIL;
	}

	pAPI (&m_liveSrcPlus);
	m_liveSrcPlusCallback.SendData = DataArrived;
	m_liveSrcPlusCallback.SendEvent = SendEvent;
	m_liveSrcPlusCallback.pUserData = this;
	m_liveSrcPlus.Init(&m_livePlusHandle, &m_liveSrcPlusCallback);
	if(m_pLibOp) 
		m_liveSrcPlus.SetParam(m_livePlusHandle, VO_PID_LIVESRC_LIBOP, m_pLibOp);
	if(m_pDrmValue) 
		m_liveSrcPlus.SetParam(m_livePlusHandle, VO_PID_LIVESRC_DRMCALLBACK, m_pDrmValue);

	VOLOGI("- LoadISSPlus");

	return VO_RET_SOURCE2_OK;
}

VO_U32 CISSControl::GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo)
{
	return m_liveSrcPlus.GetDRMInfo(m_livePlusHandle, ppDRMInfo);
}

VO_S32 CISSControl::SendEvent (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	return 0;
}

VO_U32 CISSControl::audio_data_arrive( VO_SOURCE2_SAMPLE * pSample )
{
	VO_PBYTE pAudioData = NULL;
	pSample->Size &=0x7fffffff;
	if( m_manager.GetCurSubStreamInfo(VO_SOURCE2_TT_AUDIO)->pTrackInfo->nCodec  == VO_AUDIO_CodingAAC )
	{
		//Wrap ADTS head for AAC raw data
		VO_U32 framelen = 0;
		int object, sampIdx, chanNum;

		framelen = pSample->Size + 7; 
		pAudioData = new VO_BYTE[ framelen + 1 ];
		memset(pAudioData, 0, framelen + 1);

		object = 1;
		sampIdx = 4;//m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].samplerate_index;
		chanNum = 2;//int(m_manager.m_manifest.streamIndex[m_manager.m_audio_index].audio_QL[0].audio_info.Channels);

		pAudioData[0] = 0xFF; 
		pAudioData[1] = 0xF9;

		pAudioData[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));

		if(framelen > 0x1FFF)
			return 0;

		pAudioData[3] = char(((chanNum << 6) | (framelen >> 11)));
		pAudioData[4] = char(((framelen & 0x7FF) >> 3));
		pAudioData[5] = char((((framelen & 7) << 5) | 0x1F));
		pAudioData[6] = 0xFC;	

		memcpy( pAudioData + 7, pSample->pBuffer, pSample->Size);

		pSample->pBuffer = pAudioData;
		pSample->Size = framelen;
	}
#ifdef __DUMP
	if(!issc->audio_ff)issc->audio_ff = fopen("audio.aac","wb");
	fwrite(pAudioData, 1, pSample->nSize, iss->audio_ff);
#endif
	if(pSample->Time == 0)  pSample->Time = 1 ;
	m_nCurTimeStamp_Audio = (pSample->Time += m_nCurTimeStamp_Audio);
//	VOLOGI("Audio TimeStamp: %lld", pSample->Time);
	if(m_manager.bitrate_changed_Audio) SendTrackInfo(0);
	send_media_data(pSample  , 0);

	if(pAudioData) delete []pAudioData;
	return 0;
}
VO_U32 CISSControl::video_data_arrive( VO_SOURCE2_SAMPLE *pSample )
{
	if( m_manager.GetCurSubStreamInfo(VO_SOURCE2_TT_VIDEO)->pTrackInfo->nCodec == VO_VIDEO_CodingH264 )
	{
		VO_BYTE *outputbuf = pSample->pBuffer;
		VO_U32 slicesize = 0;
		VO_U32 headerlen = sizeof(SYNC_HEADER);
		VO_U32 size =  pSample->Size &0x7fffffff;
		for( VO_U32 i = 0 ;i < size;  )
		{
			memcpy(&slicesize, outputbuf, headerlen);
			slicesize = (VO_U32)be32toh(slicesize);
			if((i +slicesize) >= size) break;
			memcpy(outputbuf, SYNC_HEADER, headerlen);
			i += slicesize + headerlen;
			if((outputbuf +slicesize + headerlen +headerlen ) >= (pSample->pBuffer + size)) break;
			outputbuf +=  slicesize + headerlen;
		}
		//if( GetFrameType(ptr_buffer->pData, ptr_buffer->nSize) == 0 )
		//	ptr_buffer->nFrameType = 0;

	}

	if(pSample->Time == 0)	pSample->Time = 1 ;
	m_nCurTimeStamp_Video = (pSample->Time += m_nCurTimeStamp_Video);
//	VOLOGI("Video TimeStamp: %lld, %08x", pSample->Time,pSample->Size);
	if(m_manager.bitrate_changed_Video ) SendTrackInfo(1);

	send_media_data(pSample  , 1 );

	return 0;
}


VO_S32 CISSControl::DataArrived (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	CISSControl *issc = (CISSControl*)pUserData;
	VO_SOURCE2_SAMPLE *pSample = (VO_SOURCE2_SAMPLE*)pData;
	if(	VO_SOURCE2_TT_VIDEO == nOutputType)
	{
		issc->video_data_arrive(pSample);
	}
	else if(VO_SOURCE2_TT_AUDIO== nOutputType)
	{
		issc->audio_data_arrive(pSample);
	}

	return 0;
}


VO_VOID CISSControl::freeLiveSrcPlus()
{
	if( m_livePlusHandle )
	{
		m_liveSrcPlus.Uninit( m_livePlusHandle );
		m_dlEngine.FreeLib ();
		m_livePlusHandle = 0;
	}
} 


VO_U32 CISSControl::ToDownLoad(VO_CHAR * pUrl, VO_ISSCONTROL_DATA_TYPE type, unsigned int *uTime )
{
	VO_VOID *pHeader = NULL;

	if(!m_pDownloadFunc) 
		return VO_RET_SOURCE2_NODOWNLOADOP;

	VO_S32 nRc = m_pDownloadFunc->Open( &pHeader , pUrl ,VO_DOWNLOAD_OPENFLAG_URL) ;
	if(nRc != VO_RET_SOURCE2_OK)
		return nRc;

	VO_S64 nContentSize = 0;
//	while( !m_bMediaStop)
	{
		nContentSize = m_pDownloadFunc->GetContentSize(pHeader);
	//	if(nContentSize == -2) continue;
	//	if(nContentSize >0 )break;
	}
	//VOLOGI("GetContentSize: %lld", nContentSize);

	if(nContentSize <= 0 )return VO_RET_SOURCE2_BASE;
	VO_PBYTE pContentBuffer = new VO_BYTE[VO_U32(nContentSize +1)];
	if(!pContentBuffer ) return VO_RET_SOURCE2_BASE;
	//while( !m_bMediaStop)
	{
		nContentSize = m_pDownloadFunc->Read(pHeader,pContentBuffer, nContentSize);
	//	if(nContentSize == -2) continue;
	//	if(nContentSize >0 )break;
	}
	nRc = m_pDownloadFunc->GetDownloadTime(pHeader, uTime);
	//VOLOGI("GetDownloadTime: %d", *uTime);

	nRc = m_pDownloadFunc->Close(pHeader);
	if(m_is_seek || m_bMediaStop) return VO_RET_SOURCE2_BASE;

	if(VO_ISSCONTROL_TT_VIDEO == type)
		nRc |= m_liveSrcPlus.ChunkParser(m_livePlusHandle, VO_SOURCE_TT_VIDEO,pContentBuffer, VO_U32(nContentSize));
	if(VO_ISSCONTROL_TT_AUDIO == type)
		nRc |= m_liveSrcPlus.ChunkParser(m_livePlusHandle, VO_SOURCE_TT_AUDIO,pContentBuffer, (VO_U32)nContentSize);
	if(VO_ISSCONTROL_TT_HEADER == type)
		nRc |= m_liveSrcPlus.HeaderParser(m_livePlusHandle, pContentBuffer, (VO_U32)nContentSize);

	delete []pContentBuffer;
	pContentBuffer = NULL;
	
	return nRc;
}

VO_S64 CISSControl::GetMediaItem( VO_CHAR * pUrl, VO_S32 indexStream )
{
	VO_S32 nRc = VO_RET_SOURCE2_OK;
	unsigned int uTime = 0;
	if(indexStream == m_manager.m_video_index)
		nRc = ToDownLoad(pUrl, VO_ISSCONTROL_TT_VIDEO, &uTime);
	if(indexStream == m_manager.m_audio_index)
		nRc = ToDownLoad(pUrl, VO_ISSCONTROL_TT_AUDIO, &uTime);
	
	if(nRc != VO_RET_SOURCE2_OK)
		return nRc;

	return (VO_S64)uTime;
}

VO_VOID CISSControl::send_media_data( VO_SOURCE2_SAMPLE * sample , VO_U32 index , VO_BOOL newfile )
{

	//VOLOGE( "====================================send_media_data" );

	if( newfile )
	{
		sample->Flag = VO_LIVESRC_FLAG_BUFFER_HEADDATA;
		sample->Flag |= VO_LIVESRC_FLAG_BUFFER_NEW_FORMAT;
	}
	else
		sample->Flag = 0;


	if( sample->Size & 0x80000000  )
		sample->Flag |= VO_LIVESRC_FLAG_BUFFER_KEYFRAME;


	if( index == 0 )
	{	
		sample->nCodecType = m_audioCodecType;
#ifdef __DUMP
		if(audio_ff)	fwrite(sample->pBuffer,1,sample->Size &0x7FFFFFFF,audio_ff);
#endif
		AddSampleToList(sample, VO_SOURCE2_TT_AUDIO);	
	}
	else if( index == 1 )
	{
		sample->nCodecType = m_videoCodecType;
#ifdef __DUMP
		if(video_ff)	fwrite(sample->pBuffer,1,sample->Size &0x7FFFFFFF,video_ff);
#endif
		AddSampleToList(sample, VO_SOURCE2_TT_VIDEO);	
	}



}

VO_VOID CISSControl::send_eos()
{

	VO_SOURCE2_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_SOURCE2_SAMPLE ) );
	unsigned char c[20];
	sample.pBuffer = c;
	sample.Size = 20;

	sample.Flag = VO_SOURCE2_FLAG_SAMPLE_EOS;
	sample.Time = m_nCurTimeStamp_Audio + 1;
	AddSampleToList(&sample, VO_SOURCE2_TT_AUDIO);	
	sample.Time = m_nCurTimeStamp_Video+1;
	AddSampleToList(&sample, VO_SOURCE2_TT_VIDEO);
}




VO_VOID CISSControl::need_flush()
{
	VOLOGI("+CISSControl::need_flush");

	m_AudioBuffer.Flush();
	m_VideoBuffer.Flush();
	VOLOGI("-CISSControl::need_flush");

}

int CISSControl::GetTrackNumber()
{
	VOLOGI("+ GetTrackNumber");
	int tracks = 0;
	if(m_videoCodecType >0) tracks ++;
	if(m_audioCodecType > 0) tracks ++;
	VOLOGI("- GetTrackNumber. %d",tracks);
	return tracks;
}
VO_VOID CISSControl::AddSampleToList(VO_SOURCE2_SAMPLE * pSample , VO_SOURCE2_OUTPUT_TYPE nType )
{
	if(VO_SOURCE2_TT_AUDIO == nType)
	{
		voCAutoLock lock(&m_voMemMutexAudio);
		m_AudioBuffer.AddBuffer(pSample);
	}
	if(VO_SOURCE2_TT_VIDEO== nType)
	{
		voCAutoLock lock(&m_voMemMutexVideo);
		m_VideoBuffer.AddBuffer(pSample);
	}
}

VO_U32 CISSControl::SendTrackInfo(int isVideo)
{
	VO_SOURCE2_SAMPLE sample;
	memset(&sample, 0, sizeof(VO_SOURCE2_SAMPLE));

	if(isVideo){
		VO_SOURCE2_SUBSTREAM_INFO *sss = m_manager.GetCurSubStreamInfo(VO_SOURCE2_TT_VIDEO );
		// added by Lin Jun 20110519,sometimes it's NULL
		if(!sss)
		{
			VOLOGE("no headData");
			return -1;
		}

		sample.pBuffer = sss->pTrackInfo->pHeadData;
		sample.Size  = sss->pTrackInfo->nHeadSize;
		sample.Time = m_nCurTimeStamp_Video - 1;
		VOLOGI("headData size =%d",sample.Size);
		send_media_data(&sample, 1, VO_TRUE);

	}
	else {
		VO_SOURCE2_SUBSTREAM_INFO *sss = m_manager.GetCurSubStreamInfo(VO_SOURCE2_TT_VIDEO );
		if( m_audioCodecType== VO_AUDIO_CodingWMA )
		{
			//send WmaPro headdata
			sample.pBuffer =  sss->pTrackInfo->pHeadData;
			sample.Size  = sss->pTrackInfo->nHeadSize;
		}
		else if( m_audioCodecType == VO_AUDIO_CodingAAC)
		{
			//aac
			unsigned char c;
			sample.pBuffer = &c;
			sample.Size = 1;
		}
		else
		{
			sample.pBuffer =  sss->pTrackInfo->pHeadData;
			sample.Size  = sss->pTrackInfo->nHeadSize;
		}
		sample.Time = m_nCurTimeStamp_Audio - 1;
		send_media_data(&sample, 0, VO_TRUE);
	}

	if(isVideo)	
		m_manager.bitrate_changed_Video = VO_FALSE;
	else
		m_manager.bitrate_changed_Audio = VO_FALSE;
	return VO_RET_SOURCE2_OK;
}

VO_U32 CISSControl::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	switch(nParamID)
	{
	case VO_PID_SOURCE2_LIBOP:
		VOLOGI("Set lib OP,%p", pParam);

		m_dlEngine.SetLibOperator((VO_LIB_OPERATOR*)pParam);
		m_pLibOp = (VO_LIB_OPERATOR*)pParam;
		break;
	case VO_PID_SOURCE2_IO:
			VOLOGI("VO_PID_SOURCE2_IO");
		m_pDownloadFunc = (VODOWNLOAD_FUNC *)pParam;
		break;
	default:
		return VO_RET_SOURCE2_FAIL;
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CISSControl::GetCurSelTrackInfo( VO_U32 nOutPutType , VO_SOURCE2_TRACK_INFO * pTrackInfo )
{
	return VO_RET_SOURCE2_OK;
}


VO_U32 CISSControl::GetParam( VO_U32 nParamID, VO_PTR pParam )
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}


VO_U32 CISSControl::GetStreamCount(VO_U32 *pCount)
{
	return m_liveSrcPlus.GetStreamCounts(m_livePlusHandle, pCount);
}

VO_S32 CISSControl::Seek( VO_U64 *pTimeStamp )
{
	VOLOGI( "in set_pos " );
// 	if(  *pTimeStamp <= 0)
// 		return 0;
// 	m_is_seek = VO_TRUE;
// 	if( *pTimeStamp >= m_manager.get_duration())
// 	{
// 		need_flush();
// 		stop_smoothstream(VO_FALSE);
// 	}
// 	else
// 	{
// 		VO_S64 ppp = VO_S64(*pTimeStamp) * (m_manager.GetTimeScaleMs());
// 		VOLOGI( "set_pos %lld" , ppp );
// 		*pTimeStamp = seek_time = m_manager.set_pos(ppp );
// 		VOLOGI( "seek_time %lld" , seek_time);
// 		voOS_Sleep(1);
// 		need_flush();
// 	}		
	return VO_RET_SOURCE2_NOIMPLEMENT;
}


VO_U32 CISSControl::Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_CALLBACK * pCallback )
{
	m_pSource2Callback = pCallback;
	memset(m_strUrl, 0x00, sizeof(m_strUrl));
	memcpy(m_strUrl, (char*)pSource, sizeof(m_strUrl));
	return VO_RET_SOURCE2_OK;
}
VO_U32 CISSControl::Uninit()
{
	freeLiveSrcPlus();
	stop_smoothstream( VO_FALSE );
	return VO_RET_SOURCE2_OK;
}
VO_U32 CISSControl::Close()
{
	Uninit();
	return VO_RET_SOURCE2_OK;
}
VO_U32 CISSControl::Open()
{
	if(!m_pDownloadFunc)
	{
		VOLOGI("no download func");
		return VO_RET_SOURCE2_NODOWNLOADOP;
	}

	VO_U32 rt = SetDataSource(m_strUrl);
	return rt;
}
VO_U32 CISSControl::SelectTrack( VO_U32 nStreamID , VO_U32 nSubStreamID , VO_U32 nTrackID )
{
	return VO_RET_SOURCE2_NOIMPLEMENT;
}


VO_U32 CISSControl::SetDataSource( VO_CHAR * pUrl )
{
	VOLOGI("+SetDataSource");
	Close();
	VO_U32 nRc = VO_RET_SOURCE2_OK;

	nRc = LoadISSPlus();
	if(nRc != VO_RET_SOURCE2_OK)	return nRc;

	unsigned int uTime = 0;
	nRc = ToDownLoad(pUrl, VO_ISSCONTROL_TT_HEADER, &uTime );
	if(nRc != VO_RET_SOURCE2_OK) return nRc;

	nRc = m_liveSrcPlus.GetStreamCounts(m_livePlusHandle, &m_uStreamCounts);
	if(nRc !=VO_RET_SOURCE2_OK || m_uStreamCounts ==0) return VO_RET_SOURCE2_FAIL;

	VO_SOURCE2_STREAM_INFO *pStreamInfo = NULL;
	m_manager.SetManifest(pUrl);
	for(VO_U32 i = 0; i< m_uStreamCounts; i++)
	{
		nRc = m_liveSrcPlus.GetStreamInfo(m_livePlusHandle,i, &pStreamInfo);
		if(nRc !=VO_RET_SOURCE2_OK || pStreamInfo == NULL) return nRc;
		
		int kid = 0;
		if( strcmp(pStreamInfo->strStreamName, "video") >=0 && !(kid &0x01))
		{
			m_nCulStreamID_video = i;
			kid |= 0x01;

			m_videoCodecType= pStreamInfo->pSubStreamInfo->pTrackInfo->nCodec;
			m_manager.SetStreamIndex(VO_SOURCE2_TT_VIDEO, i);
			m_manager.SetStreamInfo(VO_SOURCE2_TT_VIDEO, pStreamInfo);

		}
		else if( strcmp(pStreamInfo->strStreamName, "audio") ==0 && !(kid & 0x02))
		{
			m_nCulStreamID_audio = i;
			kid |= 0x02;
			m_audioCodecType= pStreamInfo->pSubStreamInfo->pTrackInfo->nCodec;
			m_manager.SetStreamIndex(VO_SOURCE2_TT_AUDIO, i);
			m_manager.SetStreamInfo(VO_SOURCE2_TT_AUDIO, pStreamInfo);
		}
	}

	FragmentItem *pPlaylist;
	m_liveSrcPlus.GetPlayList(m_livePlusHandle, &pPlaylist);
	m_manager.SetPlaylist(pPlaylist);

	VOLOGI("AudioCodec:%d, VideoCodec:%d", m_audioCodecType, m_videoCodecType);
	
	m_bMediaStop = VO_FALSE;
	VOLOGI("-SetDataSource");
	return VO_RET_SOURCE2_OK;
}

VO_U32 CISSControl::Start()
{
	if(!m_thread_handle)vo_thread::begin();
	m_is_pause = VO_FALSE; 

	return VO_RET_SOURCE2_OK;
}

VO_U32 CISSControl::Pause()
{
	return VO_RET_SOURCE2_NOIMPLEMENT;;
}
VO_U32 CISSControl::Stop()
{
	stop_smoothstream(VO_TRUE);
	return VO_RET_SOURCE2_OK;
}

VO_U32 CISSControl::GetStreamInfo( VO_U32 nStream, VO_SOURCE2_STREAM_INFO **ppStreamInfo )
{
	return	m_liveSrcPlus.GetStreamInfo(m_livePlusHandle,nStream, ppStreamInfo);
}
