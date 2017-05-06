//#include "stdafx.h"

	
#include "vo_mpd_streaming.h"
#include "voThread.h"
#include "voLog.h"
#include "voOSFunc.h"

#include "fortest.h"
#include "vompType.h"
#include "fAudioHeadDataInfo.h"
#include <stdio.h>
#ifdef _IOS
#include <sys/time.h>
#else
#include "time.h"
#endif
//#define LEON_ADD

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define DANNY_ADD

VO_SOURCE_SAMPLE oneChunkBuffer;


static FILE *out_video = NULL;
static FILE *out_audio = NULL;
#define be32toh(x) 	((x &0xff000000)>> 24 | (x & 0xff0000) >>16 << 8 | (x &0xff00) >>8 <<16 | (x & 0xff)<<24)
static const VO_CHAR SYNC_HEADER[] = {0x00,0x00,0x00,0x01};
static unsigned char CHANGE_CODE[]={ 0x01,0x02,0x03,0x04};
typedef VO_S32 ( VO_API *pvoGetDASHParserAPI)(VO_PARSER_API * pParser);

vo_mpd_streaming::vo_mpd_streaming(void)
:m_bNeedFlushBuffer(VO_FALSE)
,m_dashparser_handle(NULL)
,m_audioCodecType(0)
,m_videoCodecType(0)
,repre_index(0)
,cur_pre(NULL)
,m_eventcallback_func(0)
,m_datacallback_func(0)
,m_judgementor(5)
,m_thread(0)
,seek_time(0)
,start_time(0)
,cur_duration(0)
,m_is_flush(VO_FALSE)
,m_is_seek(VO_FALSE)
//,m_is_seek(VO_TRUE)
,m_is_pause(VO_FALSE)
,m_bMediaStop(VO_FALSE)
,download_bitrate(0)
,m_lastFregmentDuration(0)
,m_bRecoverFromPause(VO_FALSE)
,m_nCurTimeStamp_Audio(0)
,m_nCurTimeStamp_Video(0)
,m_nLastTimeStamp_Audio(0)
,m_nLastTimeStamp_Video(0)
,m_pTrackInfoEx_Audio(NULL)
,m_pTrackInfoEx_Video(NULL)
,m_is_first(VO_TRUE)
,cur_video_ql(NULL)
,cur_audio_ql(NULL)
,m_nStartUTCMark(0)
,m_nCurUTCTime(-1)
,m_bGetUTC2NTP(VO_FALSE)
{
//	memset( cur_pre , 0 , sizeof(Representation)); 
	 sample_number = 0;
	 m_nSyncTimeStamp  = -1;
	memset(&oneChunkBuffer,0x00, sizeof(VO_SOURCE_SAMPLE));
	strcpy (m_szWorkPath, "");
	memset (&m_dashhparser_api, 0, sizeof (VO_PARSER_API));
	try_counts = 0;
    m_pPlusProgramInfo = NULL;
	pVologCB = NULL;
	

#ifdef DUMP_FILE
	
#ifdef WIN32
		out_video = fopen("d:/video_dump","wb");
		out_audio = fopen("d:/audio_dump","wb");
#elif defined LINUX
		//out_video = fopen("/mnt/sdcard/video_dump","wb");
	    //if(!out_video) VOLOGR("dump file create failed");
	    //out_audio = fopen("/mnt/sdcard/video_dump","wb");
#else
		char szFolder[1024];
		memset(szFolder, 0, 1024);
		voOS_GetAppFolder(szFolder, 1024);
		strcat(szFolder, "video_dump");
		out_video = fopen(szFolder, "wb");
	
		memset(szFolder, 0, 1024);
		voOS_GetAppFolder(szFolder, 1024);
		strcat(szFolder, "audio_dump");
		out_audio = fopen(szFolder, "wb");
#endif
	
#endif // DUMP_FILE

}

void vo_mpd_streaming::SetLibOp(VO_LIB_OPERATOR* pValue) 
{
//	VOLOGR("+ Set Lib Op!");
		m_dlEngine.SetLibOperator(pValue);

   //	VOLOGR("- Set Lib Op!");
}
VO_VOID vo_mpd_streaming::NotifyData()
{

}

VO_U32 vo_mpd_streaming::SetDrmCallBack(VO_PTR pValue)
{
	VOLOGR("in SetDrmCallBack");

	VO_U32 nRC = VO_ERR_FAILED;
	if(m_dashparser_handle)
		nRC = m_dashhparser_api.SetParam(m_dashparser_handle,VODASH_PID_DRM_CALLBACK, pValue);
	VOLOGR("out SetDrmCallBack");
	return nRC;
}
VO_VOID vo_mpd_streaming::set_VideoDelayTime(VO_PTR pValue,VO_U64 timeStamp)
{
	// now we need not think the device performance.
	//so return now.
#ifdef _USE_NEW_BITRATEADAPTATION
	int delay = *((int*)pValue);
	VOLOGR("=================================Delay Time %d",delay);
	m_manager.CheckDelayTimeStamp(delay, timeStamp);
#else
	int delay = *((int*)pValue);
	VOLOGR("=================================Delay Time %d",delay);
	if(delay > 130)
		m_manager.TooHightQualityLevel();
#endif
}
int vo_mpd_streaming::GetTrackNumber()
{
	int tracks = 0;
//	tracks =1;
	if(m_videoCodecType >0) tracks ++;
	if(m_audioCodecType > 0) tracks ++;
	return tracks;
}

VO_VOID vo_mpd_streaming::run()
{
	if(!m_thread_handle)
		vo_thread::begin();
	if(m_is_pause)
		m_bRecoverFromPause = VO_TRUE;
	m_is_pause = VO_FALSE;

	m_bMediaStop = VO_FALSE;
}
vo_mpd_streaming::~vo_mpd_streaming(void)
{

	if(oneChunkBuffer.Buffer) delete []oneChunkBuffer.Buffer;
	memset(&oneChunkBuffer, 0x00, sizeof(VO_SOURCE_SAMPLE));
    ReleaseProgramInfo();
	free_DASH_parser();
	memset (&m_dashhparser_api, 0, sizeof (VO_PARSER_API));
	if(out_video)fclose(out_video); 
	if(cur_video_ql)
		delete cur_audio_ql;
	if(cur_video_ql)
		delete cur_video_ql;
	cur_video_ql = NULL;
	cur_audio_ql = NULL;
   

}
VO_VOID  vo_mpd_streaming::pause()
{ 
	VOLOGI("Set Pause.");
	m_is_pause = VO_TRUE; 
	//m_nPauseStartTime = voOS_GetSysTime();
}

#ifdef WIN32
time_t vo_mpd_streaming::SystemTimeToTime_t( const SYSTEMTIME& st )
{
 tm temptm = {st.wSecond, 
  st.wMinute, 
  st.wHour, 
  st.wDay, 
  st.wMonth - 1, 
  st.wYear - 1900, 
  st.wDayOfWeek, 
  0, 
  0};

 return mktime(&temptm);
}
#endif
VO_S64 vo_mpd_streaming::set_pos( VO_S32 pos )
{ 
	VOLOGR( "get_duration %d ",m_manager.get_duration() );
	VOLOGR( "set_pos %d ",pos );
	if(pos < 0)
		return 0;
		
	if( pos >=m_manager.get_duration())
	{
		//2012/02/15, modify by Leon
    	need_flush();
		seek_time =m_manager.get_duration();
        m_is_seek = VO_TRUE;
		//stop_dashstream(VO_FALSE);
	}
	else
	{
		
		VO_S64 ppp = VO_S64(pos)*(m_manager.GetTimeScaleMs());
	    VOLOGR( "seek_time ppp before %lld" , ppp);
		if(ppp==0){
			try_counts = 0;
		    VOLOGR( "seek_time ppp try_counts  %d" , try_counts);
		}
		start_time_tamp = m_manager.m_manifest.m_start_time;
		ppp+= start_time_tamp;
		seek_time = m_manager.set_pos(ppp );
		VOLOGR( "seek_time after %lld" , seek_time);
		m_is_seek = VO_TRUE;
		//2012/02/15, modify by Leon
		voOS_Sleep(1);
        need_flush();
	}
	oneChunkBuffer.Size = 0;
#ifdef _USE_NEW_BITRATEADAPTATION
	m_manager.FlushMap();
#endif
	VOLOGR("SeekedPos:%lld",seek_time/m_manager.GetTimeScaleMs())
	return seek_time/m_manager.GetTimeScaleMs() ;
}


VO_VOID vo_mpd_streaming::CreateAudioTrackInfoBuffer(VO_DASH_FRAME_BUFFER *pBuffer)
{
	VOLOGR("+ CreateAudioTrackInfoBuffer");
	VO_DASH_FRAME_BUFFER *buffer = pBuffer;
	memset( buffer , 0 , sizeof(VO_DASH_FRAME_BUFFER) );
	//VOLOGR("1");
	buffer->nSize = m_pTrackInfoEx_Audio->HeadSize > 12 ? sizeof( VO_LIVESRC_TRACK_INFOEX ) + m_pTrackInfoEx_Audio->HeadSize - 12 : sizeof( VO_LIVESRC_TRACK_INFOEX );
	//VOLOGR("2");
	buffer->pData = (VO_PBYTE)m_pTrackInfoEx_Audio;
	//VOLOGR("3");
	buffer->nStartTime = m_nCurTimeStamp_Audio -1;
	VOLOGR("- CreateAudioTrackInfoBuffer");
//	send_media_data( &buffer , 0 , VO_TRUE );
}

VO_VOID vo_mpd_streaming::CreateVideoTrackInfoBuffer(VO_DASH_FRAME_BUFFER *pBuffer)
{
	VOLOGR("+ CreateVideoTrackInfoBuffer");
	VO_DASH_FRAME_BUFFER *buffer = pBuffer ;
	memset( buffer , 0 , sizeof(VO_DASH_FRAME_BUFFER) );
	buffer->nSize = m_pTrackInfoEx_Video->HeadSize > 12 ? sizeof( VO_LIVESRC_TRACK_INFOEX ) + m_pTrackInfoEx_Video->HeadSize - 12 : sizeof( VO_LIVESRC_TRACK_INFOEX );
	buffer->pData = (VO_PBYTE)m_pTrackInfoEx_Video;
	buffer->nStartTime = m_nCurTimeStamp_Video - 1;
	buffer->nFrameType = 0;
	VOLOGR("- CreateVideoTrackInfoBuffer");
}

VO_VOID vo_mpd_streaming::audio_data_arrive( VO_SOURCE_SAMPLE * ptr_buffer, int nFlag ){

#ifdef _VOLOG_INFO
	static VO_U64 t = 0;

	//VOLOGR("[DASH] Recv audio frame from paraser:%lld, %lld, %d", ptr_buffer->Time, ptr_buffer->Time-t, ptr_buffer->Size & 0x7FFFFFFF);
	t = ptr_buffer->Time;
#endif //_VOLOG_INFO
	VOLOGR("Audio data arrive");
	VO_PBYTE pAudioData = NULL;
	int frame_type = 1;
	VO_S32 nSize = (ptr_buffer->Size &= 0x7FFFFFFF);
	if( m_audioCodecType == VO_AUDIO_CodingAAC )
	{
		int len = nSize + 7;
		pAudioData = new VO_BYTE[len + 1 ];
		VO_CODECBUFFER codebufIn, codebufOut;
		codebufIn.Buffer = ptr_buffer->Buffer;
		codebufIn.Length = nSize;
		codebufOut.Buffer = pAudioData;
		codebufOut.Length = len;
		VO_AUDIO_HEADDATAINFO info;
		info.nChannels = cur_audio_ql->audio_info.Channels;
		info.nSampleBits = cur_audio_ql->audio_info.SampleBits;
		info.nSampleRate = cur_audio_ql->audio_info.SampleRate;
		if( voAACADTSHeadPack(&codebufIn,&codebufOut,&info ) == 0)
		{
			ptr_buffer->Buffer = codebufOut.Buffer;
			ptr_buffer->Size = codebufOut.Length;
		}
		else
		{
			VOLOGR("ADTSHeader pack error");
		}
	}

	  if(ptr_buffer->Time == 0)  ptr_buffer->Time = 1 ;
	  m_nCurTimeStamp_Audio = ptr_buffer->Time;
	  VOLOGR("start_time dgdg audio %lld" , ptr_buffer->Time);

	 if(m_manager.bitrate_changed_Audio) SendTrackInfo(0,frame_type);
	//AddSendDataList( ptr_buffer ,0, VO_FALSE);
//  	 if(!out_audio) 
// 		 out_audio = fopen("d:/audio_dump.aac","wb");	
//  	 if(out_audio)
//  		fwrite(ptr_buffer->Buffer, 1, ptr_buffer->Size &0x7fffffff, out_audio);
// 	 fclose(out_audio);

	/* if(nFlag == 1)//
	 {
		 if(!oneChunkBuffer.Buffer)
			 oneChunkBuffer.Buffer = new unsigned char[1024*1024];
		 memcpy(oneChunkBuffer.Buffer + oneChunkBuffer.Size, ptr_buffer->Buffer, ptr_buffer->Size);
		 oneChunkBuffer.Size += ptr_buffer->Size;
		 oneChunkBuffer.Time = ptr_buffer->Time;
	 }
	 else
	 {
		 if(oneChunkBuffer.Size > 0)
			 ptr_buffer = &oneChunkBuffer;
		 AddSendDataList( ptr_buffer ,0,frame_type,VO_FALSE);
		 oneChunkBuffer.Time = 0;
		 oneChunkBuffer.Size = 0;
	 }*/
	 AddSendDataList( ptr_buffer ,0,frame_type,VO_FALSE);
	if(pAudioData) delete []pAudioData;
}

/*
VO_VOID vo_mpd_streaming::audio_data_arrive( VO_SOURCE_SAMPLE * ptr_buffer ){

#ifdef _VOLOG_INFO
	static VO_U64 t = 0;

	//VOLOGR("[DASH] Recv audio frame from paraser:%lld, %lld, %d", ptr_buffer->Time, ptr_buffer->Time-t, ptr_buffer->Size & 0x7FFFFFFF);
	t = ptr_buffer->Time;
#endif //_VOLOG_INFO
	VOLOGR("Audio data arrive");
	VO_PBYTE pAudioData = NULL;
	int frame_type = 1;
	if( m_audioCodecType == VO_AUDIO_CodingAAC )
	{

		//Wrap ADTS head for AAC raw data
		VO_U32 framelen = 0;
		int object, sampIdx, chanNum;
		VO_S32 nSize = ptr_buffer->Size & 0x7FFFFFFF;
		framelen = nSize + 7; 
		pAudioData = new VO_BYTE[ framelen + 1 ];
		memset(pAudioData, 0, framelen + 1);
		frame_type = ptr_buffer->Size & 0x80000000;
		frame_type = frame_type>>31 == 1? 0:1;

		object = 1;
		 sampIdx = cur_audio_ql->samplerate_index;
		chanNum = cur_audio_ql->audio_info.Channels;
	//sampIdx = m_manager.m_manifest.adaptionSet[m_manager.m_audio_index].audio_QL.samplerate_index;
	//	m_manager.m_manifest.adaptionSet[m_manager.m_audio_index].audio_QL.samplerate_index;
	//	chanNum = m_manager.m_manifest.adaptionSet[m_manager.m_audio_index].audio_QL.audio_info.Channels;

		pAudioData[0] = 0xFF; 
		pAudioData[1] = 0xF9;

		pAudioData[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));

		if(framelen > 0x1FFF)
			return;

		pAudioData[3] = (chanNum << 6) | (framelen >> 11);
		pAudioData[4] = (framelen & 0x7FF) >> 3;
		pAudioData[5] = ((framelen & 7) << 5) | 0x1F;
		pAudioData[6] = 0xFC;
		memcpy( pAudioData + 7, ptr_buffer->Buffer, nSize);
		ptr_buffer->Buffer = pAudioData;
		ptr_buffer->Size = framelen;
		
	}
     VOLOGR("[DASH] Recv audio frame from paraser:TimeStamp:%llu, Size%d,FrameType:%d", ptr_buffer->Time, ptr_buffer->Size,frame_type);
	  if(ptr_buffer->Time == 0)  ptr_buffer->Time = 1 ;
	  m_nCurTimeStamp_Audio = ptr_buffer->Time;
	 if(m_manager.bitrate_changed_Audio) SendTrackInfo(0,frame_type);
	//AddSendDataList( ptr_buffer ,0, VO_FALSE);
	  AddSendDataList( ptr_buffer ,0,frame_type,VO_FALSE);
	if(pAudioData) delete []pAudioData;
}
*/

VO_VOID  vo_mpd_streaming::video_data_arrive( VO_SOURCE_SAMPLE * ptr_buffer ){
	    VOLOGR("Video data arrive");
		VO_BYTE *outputbuf = ptr_buffer->Buffer;
		VO_U32 slicesize = 0;
		VO_S32 headerlen = sizeof(SYNC_HEADER);
		VO_S64 nSize = ptr_buffer->Size & 0x7FFFFFFF;
		VO_S64 frame_type = ptr_buffer->Size & 0x80000000;
		//VO_S64 duration =  ptr_buffer->Duration/m_manager.GetTimeScaleMs();
		frame_type = frame_type>>31 == 1? 0:1;
#if 0
		for( VO_S32 i = 0 ;i < nSize; )
		{
			memcpy(&slicesize, outputbuf, headerlen);
			slicesize = (VO_U32)be32toh(slicesize);
			memcpy(outputbuf, SYNC_HEADER, headerlen);
			i += slicesize + headerlen;
			outputbuf +=  slicesize + headerlen;
		}
#endif
	
		VOLOGR("[DASH] Recv video frame from paraser:TimeStamp:%llu, Size%d,FrameType:%d", ptr_buffer->Time, nSize,frame_type);
	  if(ptr_buffer->Time == 0)	ptr_buffer->Time = 1 ;
	  m_nCurTimeStamp_Video = ptr_buffer->Time ;
	  if(ptr_buffer->Time == 3870){
	  if(m_manager.bitrate_changed_Video ) SendTrackInfo(1,frame_type); //send video
	  AddSendDataList( ptr_buffer , 1,frame_type,VO_FALSE);
    }
	  else
	  {
		    if(m_manager.bitrate_changed_Video ) SendTrackInfo(1,frame_type); //send video
	        AddSendDataList( ptr_buffer , 1,frame_type,VO_FALSE);
	  }
#ifdef _USE_NEW_BITRATEADAPTATION
	m_nCountsInOneChunk_video ++;
#endif
}



VO_VOID vo_mpd_streaming::need_flush()
{
	#ifdef _DASH_SOURCE_
	//m_bNeedFlushBuffer = VO_TRUE;
//	VOLOGR("before Flush BufferFromList size: %d",m_List_buffer.GetCount());
//	while( m_bNeedFlushBuffer) voOS_Sleep(1);

	VO_LIVESRC_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );
	sample.Sample.Flag = VOMP_FLAG_BUFFER_FORCE_FLUSH;
	sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;

	VOLOGR("+Send Flush Audio Buffer");
	if( m_datacallback_func )

		m_datacallback_func( m_ptr_callbackobj , &sample );
	VOLOGR("-Send Flush Audio Buffer");

	sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;

	VOLOGR("+Send Flush Video Buffer");
	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );
	VOLOGR("-Send Flush Video Buffer");
	#else
	Event event;
	event.id = EVENT_NEEDFLUSH;
	m_eventcallback_func( m_ptr_eventcallbackobj , &event );

#endif

}



VO_VOID  vo_mpd_streaming::free_DASH_parser(){
	if( m_dashhparser_api.Close )
	{
		m_dashhparser_api.Close( m_dashparser_handle );
		m_dlEngine.FreeLib ();
		m_dashparser_handle = 0;
	}
	  
}
void vo_mpd_streaming::SetTrackInfo(VO_LIVESRC_TRACK_INFOEX * trackInfo,int index){
	
	if(index==1)
	{   cur_pre = m_manager.get_qualityLevel( m_manager.m_video_index,m_manager.cur_bitrate_index);
		ApaptionSet * pStreamIndex = NULL;
		pStreamIndex = &m_manager.m_manifest.adaptionSet[m_manager.m_video_index];
		cur_pre->length_CPD = trackInfo->HeadSize;
		VO_U32 length =    cur_pre->length_CPD;
		pStreamIndex->nTrackType = trackInfo->Type;
		pStreamIndex->nCodecType = trackInfo->Codec;
		m_videoCodecType =	pStreamIndex->nCodecType ;
		memcpy(cur_pre->codecPrivateData,trackInfo->HeadData,length);
		/*add for live*/
		if(cur_video_ql)
			delete cur_video_ql;
		cur_video_ql = NULL;
		cur_video_ql= new Representation;
		memset( cur_video_ql , 0 , sizeof(Representation_Audio));
		cur_video_ql->length_CPD = trackInfo->HeadSize;
		cur_video_ql->video_QL.Height = cur_pre->video_QL.Height;
		cur_video_ql->video_QL.Width = cur_pre->video_QL.Width;
	    memcpy(cur_video_ql->codecPrivateData,trackInfo->HeadData,cur_video_ql->length_CPD );
		/*add for live*/
	}
	else if(index ==0)
	{
		cur_pre = m_manager.get_qualityLevel( m_manager.m_audio_index,m_manager.cur_bitrate_index);
		ApaptionSet * pStreamIndex = NULL;
		pStreamIndex = &m_manager.m_manifest.adaptionSet[m_manager.m_audio_index];
		pStreamIndex->audio_QL.length_CPD = trackInfo->HeadSize;
		VO_U32 length =    trackInfo->HeadSize;
		pStreamIndex->nTrackType = trackInfo->Type;
		pStreamIndex->nCodecType = trackInfo->Codec;
		m_audioCodecType = trackInfo->Codec;
		// start save the cur_audio
		if(cur_audio_ql)
			delete cur_audio_ql;
		cur_audio_ql = NULL;
	    cur_audio_ql= new Representation_Audio;
		memset( cur_audio_ql , 0 , sizeof(Representation_Audio));
		memcpy(&cur_audio_ql->audio_info, &trackInfo->audio_info, sizeof(VO_AUDIO_FORMAT));
		cur_audio_ql->samplerate_index = m_manager.m_reader.get_samplerate_index(trackInfo->audio_info.SampleRate);
		memcpy(cur_audio_ql->codecPrivateData,trackInfo->HeadData,length);
		cur_audio_ql->length_CPD = trackInfo->HeadSize;
	
		// end save the cur_audio
 
		memcpy(&pStreamIndex->audio_QL.audio_info, &trackInfo->audio_info, sizeof(VO_AUDIO_FORMAT));
		pStreamIndex->audio_QL.samplerate_index = m_manager.m_reader.get_samplerate_index(trackInfo->audio_info.SampleRate);
	    memcpy(pStreamIndex->audio_QL.codecPrivateData,trackInfo->HeadData,length);
	}

	
	

	//m_manager.hexstr2byte(cur_pre->codecPrivateData,(VO_CHAR *)trackInfo->pInfo->HeadData,length);

};
VO_S64 vo_mpd_streaming::send_status_data(STATUS_BUFFER_ST_DASH *pBuffer)
{
	if(m_statuscallback_func) m_statuscallback_func(m_ptr_statuscallbackobj,pBuffer->id,pBuffer->ptr_buffer);
	return 0;
}

void  vo_mpd_streaming::ParserProc(VO_PARSER_OUTPUT_BUFFER* pData){

    vo_mpd_streaming * ptr_player = (vo_mpd_streaming *)pData->pUserData;
	if(ptr_player->m_is_seek) return ;
	if(ptr_player->m_is_pause) return;
	switch ( pData->nType )
	{
	case VO_PARSER_OT_AUDIO:
	{
			VO_SOURCE_SAMPLE * ptr_buffer= (VO_SOURCE_SAMPLE *)pData->pOutputData;
			VO_U64  flag = 0;
			flag =  *(VO_U64 *)(pData->pReserved);
			if(flag!= 0)
			{
          ptr_buffer->Time = ptr_buffer->Time* 1000/flag;

			}
			else
			{
				if( ptr_player->m_manager.GetTimeScaleMs()==1)
				{
					ptr_buffer->Time = ptr_buffer->Time* 1000/ptr_player->cur_audio_ql->audio_info.SampleRate;
				}
				else
				{
					ptr_buffer->Time = ptr_buffer->Time/ptr_player->m_manager.GetTimeScaleMsAudio();
				}
			}
/*
#if 1
			
			
			if( ptr_player->m_manager.GetTimeScaleMs()==1){

				ptr_buffer->Time = ptr_buffer->Time* 1000/ptr_player->cur_audio_ql->audio_info.SampleRate;
			}
			
#else
			if( ptr_player->m_manager.GetTimeScaleMs()==1){
				if(ptr_player->m_audioCodecType == VO_AUDIO_CodingAAC)
				{
					  ptr_buffer->Time = (ptr_buffer->Time+1024*ptr_player->sample_number)* 1000/ptr_player->cur_audio_ql->audio_info.SampleRate ;
					  ptr_player->sample_number++;
					  


				}
				else
					ptr_buffer->Time = ptr_buffer->Time;

			}
#endif
			*/
			if(ptr_player->m_nSyncTimeStamp== -1)
			{
				ptr_player->m_nSyncTimeStamp = ptr_buffer->Time;
			}
			if(ptr_buffer->Time <  ptr_player->m_nSyncTimeStamp)
			{
				ptr_player->m_nSyncTimeStamp = ptr_buffer->Time;
			}
			ptr_buffer->Time -=  ptr_player->m_nSyncTimeStamp;
			VOLOGR("start_time dg audio  %lld,%lld" , ptr_buffer->Time,ptr_player->m_nSyncTimeStamp);
			ptr_player->audio_data_arrive( ptr_buffer ,*((VO_U32*)pData->pReserved));
			break;
		}
	case VO_PARSER_OT_VIDEO:
		{
			VO_SOURCE_SAMPLE * ptr_buffer= (VO_SOURCE_SAMPLE *)pData->pOutputData;
		   // VOLOGR("start_time dg video %lld" , ptr_buffer->Time);
			VO_U64  flag = 0;
			flag =  *(VO_U64 *)(pData->pReserved);
			if(flag!= 0)
			{
								ptr_buffer->Time = ptr_buffer->Time* 1000/flag;

			}
			else
			{
				if( ptr_player->m_manager.GetTimeScaleMs()==1)
				{
					ptr_buffer->Time = ptr_buffer->Time;
				}
				else
				{
				ptr_buffer->Time = 	ptr_buffer->Time/ptr_player->m_manager.GetTimeScaleMs();
				}
			}
			if(ptr_player->m_nSyncTimeStamp== -1)
			{
				ptr_player->m_nSyncTimeStamp = ptr_buffer->Time;
			}
			if(ptr_buffer->Time <  ptr_player->m_nSyncTimeStamp)
			{
				ptr_player->m_nSyncTimeStamp = ptr_buffer->Time;
			}
			ptr_buffer->Time -=  ptr_player->m_nSyncTimeStamp;
			//if(out_video)	fwrite(ptr_buffer->Buffer,1,ptr_buffer->Size&0x7FFFFFFF,out_video);
			VOLOGR("start_time dg video %lld" , ptr_buffer->Time);
			ptr_player->video_data_arrive( ptr_buffer );	
			break;
		}
	case VO_PARSER_OT_TRACKINFO:
		{
			VO_LIVESRC_TRACK_INFOEX * trackInfo =(VO_LIVESRC_TRACK_INFOEX *)pData->pOutputData;
		   if(trackInfo->Type == VO_SOURCE_TT_VIDEO)
		   {
			ptr_player->SetTrackInfo(trackInfo,1);
		   }
		   else if (trackInfo->Type == VO_SOURCE_TT_AUDIO)
		  {
			  ptr_player->SetTrackInfo(trackInfo,0);
		   }
		   break;
		}
	default:
		break;
	}
}

VO_VOID vo_mpd_streaming::UpdateTrackInfoData(VO_U32 nID){
#ifdef VOME
	STATUS_BUFFER_ST_DASH status;
	switch(nID){
			case VO_DASH_PID_AUDIO_TRACKINFO:{
				ApaptionSet *pStreamIndex= NULL;
				if(m_manager.m_audio_index  != -1)
					pStreamIndex = &m_manager.m_manifest.adaptionSet[m_manager.m_audio_index];
				if(!pStreamIndex ) return ;
			

				if(m_pTrackInfoEx_Audio)
				{
					delete []m_pTrackInfoEx_Audio;
					m_pTrackInfoEx_Audio = 0;
				}
			//	m_pTrackInfoEx_Audio = ( VO_LIVESRC_TRACK_INFOEX * )new VO_BYTE[ sizeof( VO_LIVESRC_TRACK_INFOEX ) + pStreamIndex->audio_QL.length_CPD ];
                m_pTrackInfoEx_Audio = ( VO_LIVESRC_TRACK_INFOEX * )new VO_BYTE[ sizeof( VO_LIVESRC_TRACK_INFOEX ) + cur_audio_ql->length_CPD ];
				VO_LIVESRC_TRACK_INFOEX *trackinfoex = m_pTrackInfoEx_Audio;
				memset( trackinfoex , 0 , sizeof( VO_LIVESRC_TRACK_INFOEX ) );

				trackinfoex->Type = VO_SOURCE_TT_AUDIO;
				trackinfoex->Codec = m_audioCodecType;
				
			  //  memcpy(&trackinfoex->audio_info, &pStreamIndex->audio_QL.audio_info, sizeof(VO_AUDIO_FORMAT));
				memcpy(&trackinfoex->audio_info, &cur_audio_ql->audio_info, sizeof(VO_AUDIO_FORMAT));
				trackinfoex->HeadData = trackinfoex->Padding;
				if(cur_audio_ql->length_CPD > 0)
				{  
				//	memcpy(trackinfoex->HeadData,pStreamIndex->audio_QL.codecPrivateData, pStreamIndex->audio_QL.length_CPD);
					memcpy(trackinfoex->HeadData,cur_audio_ql->codecPrivateData, cur_audio_ql->length_CPD);

				}
			   memcpy(trackinfoex->HeadData, pStreamIndex->audio_QL.codecPrivateData, pStreamIndex->audio_QL.length_CPD);
				//trackinfoex->HeadSize = pStreamIndex->audio_QL.length_CPD;
				trackinfoex->HeadSize = cur_audio_ql->length_CPD;

				VOLOGR("Reset Audio TrackInfo. Codec:%d, Channels:%d, SampleBits:%d, SampleRate:%d, HeadSize: %d", trackinfoex->Codec, trackinfoex->audio_info.Channels, trackinfoex->audio_info.SampleBits, trackinfoex->audio_info.SampleRate, trackinfoex->HeadSize);
			  }

				break;
		    case VO_DASH_PID_VIDEO_TRACKINFO:
				{
				ApaptionSet *pAdaptionSet= NULL;
				if(m_manager.m_video_index  != -1)
					pAdaptionSet = &m_manager.m_manifest.adaptionSet[m_manager.m_video_index];
				if(!pAdaptionSet ) return ;

				Representation *ql = m_manager.get_qualityLevel(m_manager.m_video_index, m_manager.cur_bitrate_index);
				if(m_pTrackInfoEx_Video)
				{
					delete []m_pTrackInfoEx_Video;
					m_pTrackInfoEx_Video = 0;
				}
				m_pTrackInfoEx_Video = ( VO_LIVESRC_TRACK_INFOEX * )new VO_BYTE[ sizeof( VO_LIVESRC_TRACK_INFOEX ) + ql->length_CPD ];
				VO_LIVESRC_TRACK_INFOEX *trackinfoex = m_pTrackInfoEx_Video;
				memset( trackinfoex , 0 , sizeof( VO_LIVESRC_TRACK_INFOEX ) );
				trackinfoex->Type = VO_SOURCE_TT_VIDEO;
				//trackinfoex->Codec = pAdaptionSet->nCodecType;
				trackinfoex->Codec  = m_videoCodecType;

				/*trackinfoex->video_info.Height = ql->video_QL.Height;
				trackinfoex->video_info.Width = ql->video_QL.Width;
				trackinfoex->HeadData = trackinfoex->Padding;
				if(ql->length_CPD > 0)
					memcpy(trackinfoex->HeadData,ql->codecPrivateData, ql->length_CPD);
				trackinfoex->HeadSize = ql->length_CPD;
				char c[255];
				for(int i = 0; i< ql->length_CPD;i++)
					sprintf(&c[i*2], "%02x", ql->codecPrivateData[i]);
				VOLOGR("HeadData:%s", c);*/
				trackinfoex->video_info.Height = cur_video_ql->video_QL.Height;
				trackinfoex->video_info.Width = cur_video_ql->video_QL.Width;
				trackinfoex->HeadData = trackinfoex->Padding;
				if(ql->length_CPD > 0)
					memcpy(trackinfoex->HeadData,cur_video_ql->codecPrivateData, cur_video_ql->length_CPD);
				trackinfoex->HeadSize = cur_video_ql->length_CPD;
				char c[255];
				for(int i = 0; i< cur_video_ql->length_CPD;i++)
					sprintf(&c[i*2], "%02x", cur_video_ql->codecPrivateData[i]);
				VOLOGR("HeadData:%s", c);
				VOLOGR("Reset Video TrackInfo. Codec:%d, Width:%d, Height:%d, HeadSize: %d", trackinfoex->Codec, trackinfoex->video_info.Width, trackinfoex->video_info.Height, trackinfoex->HeadSize);

				}

				break;


	}
#endif

}
VO_VOID vo_mpd_streaming::load_DASH_parser(){

	if (strlen (m_szWorkPath) > 0)
	m_dlEngine.SetWorkPath ((VO_TCHAR*)m_szWorkPath);
	VOLOGI ("Work path %s", m_szWorkPath);
	vostrcpy(m_dlEngine.m_szDllFile, _T("voMP4FR"));
	vostrcpy(m_dlEngine.m_szAPIName, _T("voGetMp4StreamReaderAPI"));
	#if defined _WIN32
	vostrcat(m_dlEngine.m_szDllFile, _T(".dll"));
    #elif defined LINUX
	vostrcat(m_dlEngine.m_szDllFile, _T(".so"));
    #elif defined _MAC_OS
	vostrcat(m_dlEngine.m_szDllFile, _T(".dylib"));
    #elif defined _IOS
	{
		;
    }
	#endif

#if defined _WIN32 || defined LINUX || defined _MAC_OS
	VOLOGI("Load %s",m_dlEngine.m_szDllFile);
	if(m_dlEngine.LoadLib(NULL) == 0)
	{
		VOLOGE ("LoadLib fail");
		return;
	}
	VOLOGI("Load %s",m_dlEngine.m_szAPIName);
	pvoGetDASHParserAPI pAPI = (pvoGetDASHParserAPI) m_dlEngine.m_pAPIEntry;
	if (pAPI == NULL)
	{
		return;
	}
	pAPI (&m_dashhparser_api);
#elif defined _IOS
	voGetMp4StreamReaderAPI (&m_dashhparser_api);
#endif

	VO_PARSER_INIT_INFO info;
	info.pProc = ParserProc;
	info.pUserData = this;
	info.pMemOP = NULL;
	m_dashhparser_api.Open( &m_dashparser_handle , &info );
	if(m_dashparser_handle) 
		VOLOGI("dash api open ok");
	if(pVologCB)
		m_dashhparser_api.SetParam(m_dashparser_handle,VO_PID_COMMON_LOGFUNC,pVologCB);

}

VO_VOID vo_mpd_streaming::close(){
		//stop_smoothstream( VO_FALSE );
		stop_dashstream( VO_FALSE );
		
}
void vo_mpd_streaming::ReleaseProgramInfo()
{
 if(!m_pPlusProgramInfo)
  return ;
 VO_SOURCE2_STREAM_INFO **ppSInfo = m_pPlusProgramInfo->ppStreamInfo ;
 for (VO_U32 i= 0;i<m_pPlusProgramInfo->uStreamCount; i++ )
 {
  VO_SOURCE2_TRACK_INFO **ppTInfo = (*ppSInfo)->ppTrackInfo;
  for (VO_U32 n= 0; n < (*ppSInfo)->uTrackCount; n++ )
  {
   if((*ppTInfo)->pHeadData)
    delete [](*ppTInfo)->pHeadData;
   ppTInfo ++;
  }
  if((*ppSInfo)->ppTrackInfo)
  {
   delete [](*(*ppSInfo)->ppTrackInfo);
   delete (*ppSInfo)->ppTrackInfo;
  }
  ppSInfo ++;
 }
 if(m_pPlusProgramInfo->ppStreamInfo)
 {
  delete [](*m_pPlusProgramInfo->ppStreamInfo);
  delete m_pPlusProgramInfo->ppStreamInfo;
 }
 memset(m_pPlusProgramInfo, 0x00, sizeof(VO_SOURCE2_PROGRAM_INFO));
 delete m_pPlusProgramInfo;
 m_pPlusProgramInfo = NULL;
}

VO_BOOL vo_mpd_streaming::CreateProgramInfo()
{
	 VOLOGR("CreateProgramInfo");
	//delete programinfo
	 ReleaseProgramInfo();
	 if(m_manager.m_manifest.group_count <= 0)
	  return VO_FALSE;
	 m_pPlusProgramInfo = new VO_SOURCE2_PROGRAM_INFO;
	 m_pPlusProgramInfo->sProgramType =m_manager.m_manifest.islive? VO_SOURCE2_STREAM_TYPE_LIVE :VO_SOURCE2_STREAM_TYPE_VOD;
	 m_pPlusProgramInfo->uProgramID = 1;
	 sprintf(m_pPlusProgramInfo->strProgramName,"%s", "DASH");
	 int videoCount = 0;
	 int audioCount = 0;
	 m_nStreamCount = 0;
	 for(int i = 0;i < m_manager.m_manifest.group_count; i++ )
	 {
	  if(m_manager.m_manifest.adaptionSet[i].nTrackType == VO_SOURCE_TT_VIDEO)
	  {
		  videoCount += m_manager.m_manifest.adaptionSet[i].rpe_number;
	  }
	  else if(m_manager.m_manifest.adaptionSet[i].nTrackType == VO_SOURCE_TT_AUDIO)
	  {
		  audioCount += 1;//m_manifest.streamIndex[i].audio_QL[0] ? 1 :0);
	  }
	 }
	 m_nStreamCount = videoCount + 1;//add pure audio stream;
	 m_pPlusProgramInfo->uStreamCount = m_nStreamCount;
	 m_pPlusProgramInfo->ppStreamInfo = new VO_SOURCE2_STREAM_INFO* [m_nStreamCount];
	 memset(m_pPlusProgramInfo->ppStreamInfo, 0x00, m_nStreamCount);
	 for(int n = 0; n < m_nStreamCount ;n++)
	 {
	  VO_SOURCE2_STREAM_INFO *pStreamInfo = m_pPlusProgramInfo->ppStreamInfo[n] = new VO_SOURCE2_STREAM_INFO;
	  memset(pStreamInfo, 0x00, sizeof(VO_SOURCE2_STREAM_INFO));
	  pStreamInfo->uTrackCount = audioCount;
	  if(n != m_nStreamCount -1)
		  pStreamInfo->uTrackCount += 1;
	  pStreamInfo->ppTrackInfo = new VO_SOURCE2_TRACK_INFO* [pStreamInfo->uTrackCount];
	  memset(pStreamInfo->ppTrackInfo, 0x00 , pStreamInfo->uTrackCount);
	  for(int l =0; l < pStreamInfo->uTrackCount; l ++)
	  {
	   pStreamInfo->ppTrackInfo[l] = new VO_SOURCE2_TRACK_INFO;
	   memset(pStreamInfo->ppTrackInfo[l], 0x00, sizeof(VO_SOURCE2_TRACK_INFO));
	  }
	  pStreamInfo->uTrackCount = 0;
      pStreamInfo ++;
	 }
	    int count = 0;
	 for(VO_U32 i =0; i< m_manager.m_manifest.group_count; i++)
	  {
	 
	   if(m_manager.m_manifest.adaptionSet[i].nTrackType == VO_SOURCE_TT_VIDEO)
	   {
		   Representation * ql = m_manager.m_manifest.adaptionSet[i].m_ptr_QL_head;
	
	   while(ql)
	   {
		VO_SOURCE2_STREAM_INFO *pStreamInfo = m_pPlusProgramInfo->ppStreamInfo[count++];
		pStreamInfo->uSelInfo = 0;//VO_SOURCE2_SELECT_DEFAULT;
		pStreamInfo->uStreamID = ql->index_QL;
		pStreamInfo->uBitrate += ql->bitrate;
		VO_SOURCE2_TRACK_INFO *pTrackInfo = pStreamInfo->ppTrackInfo[pStreamInfo->uTrackCount] ;
		pTrackInfo->uTrackID = pStreamInfo->uTrackCount;
		pStreamInfo->uTrackCount++;
		pTrackInfo->uSelInfo = 0;//VO_SOURCE2_SELECT_DEFAULT;
	   // memcpy(pTrackInfo->strFourCC , ql->fourCC, sizeof(pTrackInfo->strFourCC));
		pTrackInfo->uTrackType = VO_SOURCE2_TT_VIDEO;
		pTrackInfo->uCodec  = m_manager.m_manifest.adaptionSet[i].nCodecType;
		pTrackInfo->uDuration = m_manager.m_manifest.mediaPresentationDuration *1000/ m_manager.m_manifest.video_scale;
		pTrackInfo->uBitrate = ql->bitrate;
		pTrackInfo->sVideoInfo.sFormat.Height = ql->video_QL.Height;
		pTrackInfo->sVideoInfo.sFormat.Width = ql->video_QL.Width;
		pTrackInfo->uHeadSize = ql->length_CPD;
	   // pTrackInfo->pHeadData = new VO_BYTE[pTrackInfo->uHeadSize+1];
	  //  memcpy(pTrackInfo->pHeadData, ql->codecPrivateData, pTrackInfo->uHeadSize);
		ql = ql->ptr_next;
		
	   
	   }
	   continue;
	  }
	  else if(m_manager.m_manifest.adaptionSet[i].nTrackType == VO_SOURCE_TT_AUDIO)
	  {
		  Representation * ql = m_manager.m_manifest.adaptionSet[i].m_ptr_QL_head;
		 for(int j =0; j < m_pPlusProgramInfo->uStreamCount; j ++)
		 {
			 VO_SOURCE2_STREAM_INFO *pStreamInfo2 = m_pPlusProgramInfo->ppStreamInfo[j];
			 pStreamInfo2->uBitrate += ql->bitrate;
			 VOLOGR("pStreamInfo2->uTrackCount %d",pStreamInfo2->uTrackCount);
			 VO_SOURCE2_TRACK_INFO *pTrackInfo = pStreamInfo2->ppTrackInfo[pStreamInfo2->uTrackCount] ;
		   pTrackInfo->uTrackID = pStreamInfo2->uTrackCount;
		   pStreamInfo2->uTrackCount ++;
		   pTrackInfo->uSelInfo = VO_SOURCE2_SELECT_DEFAULT;
	   //    memcpy(pTrackInfo->strFourCC , ql->fourCC, sizeof(pTrackInfo->strFourCC));
		   pTrackInfo->uTrackType = VO_SOURCE2_TT_AUDIO;
		   pTrackInfo->uCodec  = m_manager.m_manifest.adaptionSet[i].nCodecType;
		   pTrackInfo->uDuration = m_manager.m_manifest.mediaPresentationDuration *1000/m_manager.m_manifest.audio_scale;
		   //pTrackInfo->uChunkCounts = m_manifest.adaptionSet[i].chunksNumber;
		   pTrackInfo->uBitrate = ql->bitrate;
		   memcpy(pTrackInfo->sAudioInfo.chLanguage,m_manager.m_manifest.adaptionSet[i].lang,strlen(m_manager.m_manifest.adaptionSet[i].lang));
		   
		 /*  pTrackInfo->sAudioInfo.sFormat.Channels = ql->audio_QL.Channels;
		   pTrackInfo->sAudioInfo.sFormat.SampleBits = ql->audio_QL.SampleBits;
		   pTrackInfo->sAudioInfo.sFormat.SampleRate = ql->audio_QL.SampleRate;
		   pTrackInfo->uHeadSize = ql->length_CPD;
		   VOLOGR("pTrackInfo->uHeadSize %d", pTrackInfo->uHeadSize);
		   pTrackInfo->pHeadData = new VO_BYTE[pTrackInfo->uHeadSize+1];
		   memcpy(pTrackInfo->pHeadData, ql->codecPrivateData, pTrackInfo->uHeadSize);*/
		 }
	    
	   }
	 }
           return VO_TRUE;
	}

VO_BOOL vo_mpd_streaming::SetDataSource( VO_CHAR * ptr_url , VO_BOOL is_sync ){
	close();
	VOLOGR("+m_manager.set_manifest_url");
	if( m_manager.SetManifest( &m_fragment, ptr_url )== VO_FALSE)
	{
		VOLOGR( "First set manifest url failed!" );
		return VO_FALSE;
	}

	  load_DASH_parser();
	//  VO_PID_COMMON_LOGFUNC
	 // vompSetParam(VO_PID_COMMON_LOGFUNC);
      m_bMediaStop = VO_FALSE;
	  int rpe_index_video_old =m_manager.m_manifest.adaptionSet[ m_manager.m_video_index].m_ptr_QL_head->rpe_item_index;
      int rpe_index_audio_old = 0;
 	  GetTrackInfo(VO_TRUE, m_manager.m_video_index,rpe_index_video_old);
	  GetTrackInfo(VO_FALSE, m_manager.m_audio_index,rpe_index_audio_old);
	  UpdateTrackInfoData(VO_DASH_PID_VIDEO_TRACKINFO);
	  UpdateTrackInfoData(VO_DASH_PID_AUDIO_TRACKINFO);
	  CreateProgramInfo();

#ifdef WIN32
	//  int pos =255000;
	//  seek_time = set_pos( pos );
#endif


	     VO_U32 thread_id;
		 voThreadHandle hthread;
     //    voThreadCreate( &hthread , &thread_id , threadfunc , this , 0 );
		return VO_TRUE;
		
}

VO_U32 vo_mpd_streaming::SetParam(VO_U32 nParamID, VO_PTR pParam)
{

	switch(nParamID)
	{
	case VO_PID_COMMON_LOGFUNC:
		{
			pVologCB = (VO_LOG_PRINT_CB *)pParam;
		}
		break;
	case VO_PID_SOURCE2_BACAP:
		{
			m_manager.SetBACAP((VO_SOURCE2_CAP_DATA*)pParam);
			break;
		}
	default:
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}
	return VO_RET_SOURCE2_OK;

}

/*
VO_U32 vo_mpd_streaming::threadfunc( VO_PTR pParam )
{
	vo_mpd_streaming * ptr_obj = ( vo_mpd_streaming * )pParam;
	ptr_obj->m_thread = (voThreadHandle)1;
//	ptr_obj->SendBufferFromList();//start_smoothstream();
	ptr_obj->m_thread = 0;
	return 1;
}*/
VO_S32 vo_mpd_streaming::setLang(VO_CHAR * lang){
	return m_manager.setLang(lang);
}
VO_VOID vo_mpd_streaming::send_eos()
{
	VO_LIVESRC_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );
	unsigned char c[20];
	sample.Sample.Buffer = c;
	sample.Sample.Size = 20;


#ifdef VOME
	sample.Sample.Flag = OMX_BUFFERFLAG_EOS;
#else
	sample.Sample.Flag = VOMP_FLAG_BUFFER_EOS;
#endif

	// #ifdef VOME
	// 	sample.Sample.Flag = OMX_BUFFERFLAG_EOS;
	// 	sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;
	// 	sample.Sample.Time = 0 ;
	// 	if( m_datacallback_func )
	// 		m_datacallback_func( m_ptr_callbackobj , &sample );
	// 
	// 	sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;
	// 	sample.Sample.Time = 0 ;
	// 	if( m_datacallback_func )
	// 		m_datacallback_func( m_ptr_callbackobj , &sample );
	// #else
	// 	sample.Sample.Flag = VOMP_FLAG_BUFFER_EOS;
	sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;
	// 	sample.Sample.Time = 0 ;
	// 	if( m_datacallback_func )
	// 		m_datacallback_func( m_ptr_callbackobj , &sample );
	// 	sample.Sample.Time = 200000 ;
	sample.Sample.Time = m_nCurTimeStamp_Audio+1;
	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );

	sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;
	// 	sample.Sample.Time = 0;
	// 	if( m_datacallback_func )
	// 		m_datacallback_func( m_ptr_callbackobj , &sample );
	// 	sample.Sample.Time = 200000 ;
	sample.Sample.Time = m_nCurTimeStamp_Video+1;
	if( m_datacallback_func )
		m_datacallback_func( m_ptr_callbackobj , &sample );
	//#endif
	m_nCurTimeStamp_Video = 0;
	m_nCurTimeStamp_Audio = 0;

}

VO_VOID vo_mpd_streaming::start_dashstream(){

	VO_S64 update_time = 0;
	VO_S64 update_time_test = 0;
	VO_S64 sleep_time = 0;
 	VO_S32 fragment_number = 1;
	VO_CHAR url_segment[1024];
	VO_S32  ret = 0;
	VO_S64 seek_fragment_time = 0;
	VO_U64 last_fragment_time = 0;
	VO_BOOL live_after_pause = VO_FALSE;
	seek_fragment_time = seek_time; 
	Seg_item * ptr_item = NULL;
	ptr_item =  m_manager.GetFirstChunk();
	if(m_is_seek){
		 VOLOGR("m_is_seek! %d",seek_time);
        m_manager.get_segmentItem(&ptr_item, (VO_U64)seek_time);
		need_flush();
		m_is_seek = VO_FALSE;
	}

	if( !ptr_item)
		return;
 
	int EOSTime = 100;
	if( !is_live())
	{
		int rpe_index_video_old =m_manager.m_manifest.adaptionSet[m_manager.m_video_index].m_ptr_QL_head->rpe_item_index;
		int rpe_index_audio_old = 0;
		start_time_tamp = m_manager.m_manifest.m_start_time;
	
		if(m_audioCodecType > 0 )
		{	
			VO_DASH_FRAME_BUFFER pbuffer;
			memset(&pbuffer, 0, sizeof(VO_DASH_FRAME_BUFFER));
			pbuffer.nStartTime  = m_manager.set_pos(seek_time)/ m_manager.GetTimeScaleMsAudio();// ptr_item->starttime/10000;//
			m_nLastTimeStamp_Audio = pbuffer.nStartTime;
			m_manager.bitrate_changed_Audio = VO_TRUE;
		}
		while( !m_bMediaStop)
		{//fix the seek before the last chunk  bug
			VOLOGR("m_bMediaStop:%d", m_bMediaStop);
			if(m_is_seek)
			{
			  VOLOGR("m_is_seek2! %d",seek_time);
			  VOLOGR("seek cur_bitrate_index  %d",m_manager.cur_bitrate_index);
			  m_manager.get_segmentItem(&ptr_item,(VO_U64)seek_time);
			  need_flush();
			  m_is_seek = VO_FALSE; 
			  m_manager.bitrate_changed_Video = VO_TRUE;
			  m_manager.bitrate_changed_Audio = VO_TRUE;
			  UpdateTrackInfoData(VO_DASH_PID_VIDEO_TRACKINFO);
			  UpdateTrackInfoData(VO_DASH_PID_AUDIO_TRACKINFO);
			  VOLOGR("m_is_seek2! cur_bitrate_index  %d",m_manager.cur_bitrate_index);
			}
			if(!ptr_item)
			{
				VOLOGR("item is null  so sleep!");
				send_eos();
				voOS_Sleep( EOSTime );
				continue;
			}
			int last_rpe_inex = ptr_item->representation_index;
			int last_stream_index = ptr_item->group_index;
			start_time = ptr_item->starttime;
		//	VOLOGE(" start_time %lld", ptr_item->starttime);

			memset(url_segment, 0, 1024);//get the url of the first segment of the first chunk
			ret = m_manager.pop_fragment(ptr_item, url_segment, download_bitrate,m_lastFregmentDuration);//the segmen_item
			
			if( ret == -1 || m_bMediaStop )
			{
			   break;
			}
			repre_index = ptr_item->representation_index;
			if( m_manager.m_video_index != -1 && ptr_item->group_index == m_manager.m_video_index )
			{
				download_bitrate =0;
				m_nLastTimeStamp_Video = ptr_item->starttime/ m_manager.GetTimeScaleMs();
			//	VOLOGE(" m_nLastTimeStamp_Video %lld", ptr_item->starttime);
				VO_BOOL is_video = VO_TRUE;
				if(repre_index!= rpe_index_video_old)
				{
					VOLOGR("1111");
					m_manager.bitrate_changed_Video = VO_TRUE;
					GetTrackInfo(is_video, m_manager.m_video_index,repre_index);
				}
				rpe_index_video_old = repre_index;
				VOLOGE("dg url_segment video %s",url_segment);
				download_bitrate = GetMediaItem( url_segment, ptr_item->group_index);//取得当前的下载速率
				m_judgementor.add_item( download_bitrate );
			}
			else if(m_manager.m_audio_index != -1 && ptr_item->group_index == m_manager.m_audio_index)
			{
				VO_BOOL is_video = VO_FALSE;
				if(repre_index!= rpe_index_audio_old)
				{
					GetTrackInfo(is_video, m_manager.m_audio_index,repre_index);
					m_manager.bitrate_changed_Audio = VO_TRUE;
				}
				rpe_index_audio_old = repre_index;
			//	VOLOGE(" m_nLastTimeStamp_Audio ");
				VOLOGE("dg url_segment audio %s",url_segment);
			    download_bitrate += GetMediaItem( url_segment, ptr_item->group_index );
			}
			ptr_item = m_manager.GetNextChunk(ptr_item);
			fragment_number++;
		}//end while
	}//endof if( !is_live()) 
	else if(is_live()&&!is_update())
	{
		int rpe_index_video_old = m_manager.m_manifest.adaptionSet[ m_manager.m_video_index].m_ptr_QL_head->rpe_item_index;
		int rpe_index_audio_old = 0;
		start_time_tamp = m_manager.m_manifest.m_start_time;
		if(m_audioCodecType > 0 )
		{	
			VO_DASH_FRAME_BUFFER pbuffer;
			memset(&pbuffer, 0, sizeof(VO_DASH_FRAME_BUFFER));
			is_live()  ? pbuffer.nStartTime  = m_manager.get_audiostarttime()  : pbuffer.nStartTime  = m_manager.set_pos(seek_time)/ m_manager.GetTimeScaleMsAudio();// ptr_item->starttime/10000;//
			m_nLastTimeStamp_Audio = pbuffer.nStartTime;
			m_manager.bitrate_changed_Audio = VO_TRUE;
		}
		int ini_chunk =0;
		int count_video = 0;
		int count_audio = 0;
		Seg_item* new_item = NULL;
		VO_S64 real_count = m_manager.m_reader.real_count;
		while( !m_bMediaStop)
		{
			if(try_counts>4)
			{  
				VOLOGR("try_counts>4");
				send_eos();
				voOS_Sleep( EOSTime );
				try_counts = 0; 
				start_time = 0;
				continue;
			}
			else if(try_counts<2)
			{
				VOLOGR("try_counts<2");
				VOLOGR("start_time %lld",start_time);
				VOLOGR("m_ptr_FI_tail time %lld",m_manager.m_reader.m_ptr_FI_tail->starttime);
				if(start_time >= m_manager.m_reader.m_ptr_FI_tail->starttime&&fragment_number<=real_count)
				{
				   VOLOGR("too large playlist.%p",new_item);

					new_item  = new Seg_item;
					new_item->starttime = m_manager.m_reader.m_ptr_FI_tail->starttime+m_manager.m_reader.m_ptr_FI_tail->duration;
					if(m_manager.m_reader.m_ptr_FI_tail->nIsVideo == 0)
					{
						new_item->nIsVideo = 1;
						new_item->group_index =m_manager.m_audio_index;
					}
					else
					{
						new_item->nIsVideo = 0;
						new_item->group_index = m_manager.m_video_index;
					}
					new_item->ptr_next = NULL;
					m_manager.m_reader.add_fragmentItem(new_item);
				}
			}
			VOLOGR("GetMediaItem try_counts %d",try_counts);
			if(m_is_seek)
			{
				VOLOGR("m_is_seek21111111111! %lld",seek_time);
				m_manager.get_segmentItem(&ptr_item,(VO_U64)seek_time);
				need_flush();
				m_is_seek = VO_FALSE; 
				m_manager.bitrate_changed_Video = VO_TRUE;
				m_manager.bitrate_changed_Audio = VO_TRUE;
				UpdateTrackInfoData(VO_DASH_PID_VIDEO_TRACKINFO);
				UpdateTrackInfoData(VO_DASH_PID_AUDIO_TRACKINFO);
			}
			if(!ptr_item)
			{
				VOLOGR("item is null  so sleep!");
				send_eos();
				voOS_Sleep( EOSTime ); 
				try_counts = 0;
				continue;
			}
			start_time = ptr_item->starttime;
			VOLOGR("start_time  %lld",ptr_item->starttime);
			memset(url_segment, 0, 1024);//get the url of the first segment of the first chunk
			ret = m_manager.pop_fragment(ptr_item, url_segment, download_bitrate,m_lastFregmentDuration);//the segmen_item
			VOLOGR("dg url_segment %s",url_segment);
			if( ret == -1 || m_bMediaStop )
			{
			   break;
			}
			repre_index = ptr_item->representation_index;
			if( m_manager.m_video_index != -1 && ptr_item->group_index == m_manager.m_video_index )
			{
				download_bitrate =0;
				VO_BOOL is_video = VO_TRUE;
				if(repre_index!= rpe_index_video_old)
				{
					VOLOGR("1111");
					m_manager.bitrate_changed_Video = VO_TRUE;
					GetTrackInfo(is_video, m_manager.m_video_index,repre_index);
				}
				rpe_index_video_old = repre_index;
				download_bitrate = GetMediaItem( url_segment, ptr_item->group_index);//取得当前的下载速率
				m_judgementor.add_item( download_bitrate );
			}
			else if(m_manager.m_audio_index != -1 && ptr_item->group_index == m_manager.m_audio_index)
			{
				VO_BOOL is_video = VO_FALSE;
				if(repre_index!= rpe_index_audio_old)
				{
				   VOLOGR("2222");
				   GetTrackInfo(is_video, m_manager.m_audio_index,repre_index);
				   m_manager.bitrate_changed_Audio = VO_TRUE;
				}
				rpe_index_audio_old = repre_index;
				download_bitrate += GetMediaItem( url_segment, ptr_item->group_index );
			}
			ptr_item = m_manager.GetNextChunk(ptr_item);
			fragment_number++;
		}//endof while( !m_bMediaStop)
	}
	else 
	{
		int rpe_index_video_old =m_manager.m_manifest.adaptionSet[ m_manager.m_video_index].m_ptr_QL_head->rpe_item_index;
		int rpe_index_audio_old = 0;
		if(m_audioCodecType > 0 )
		{	
			VO_DASH_FRAME_BUFFER pbuffer;
			memset(&pbuffer, 0, sizeof(VO_DASH_FRAME_BUFFER));
			is_live()  ? pbuffer.nStartTime  = m_manager.get_audiostarttime()  : pbuffer.nStartTime  = m_manager.set_pos(seek_time)/ m_manager.GetTimeScaleMsAudio();// ptr_item->starttime/10000;//
			m_nLastTimeStamp_Audio = pbuffer.nStartTime;
			m_manager.bitrate_changed_Audio = VO_TRUE;
		}

		int ini_chunk  = 0;
		int curSegmentCounts = m_manager.GetSegmentCounts();


		int count_video = 0;
		int count_audio = 0;

		m_manager.get_segmentItem(&ptr_item,0,1);

		sleep_time =500;
		VO_BOOL flag = VO_TRUE;
		start_time_tamp = m_manager.m_reader.GetStartTime();

#ifdef DANNY_ADD
		time_t Avaiable_time = m_manager.m_reader.m_read_time;
		VO_S64 av_duration = m_manager.m_reader.m_manifest->mediaPresentationDuration;
		
		if(av_duration ==0)
		{
		   if(m_manager.m_reader.m_manifest->video_scale==1)
		   {
			   av_duration = curSegmentCounts*ptr_item->duration/(m_manager.m_reader.m_manifest->video_scale*1000);
		   }
		   else
			 av_duration = curSegmentCounts*ptr_item->duration/ m_manager.m_reader.m_manifest->video_scale;
		}
		
		VO_S64 video_duration = ptr_item->duration;
	    VO_S64  left_time = 0;
		int index_video = -1;
		VO_S64 timeline_time  = 0;
		VO_BOOL enReflesh = VO_FALSE;
		VO_U32 enRefleshMarkTime = voOS_GetSysTime();
		time_t nCurUTCTime = 0;
		GetCurUTCTime(nCurUTCTime,VO_TRUE);
		while( !m_bMediaStop)
		{ 

			if(m_is_pause)
			{
				voOS_Sleep(200);
				need_flush();
				enReflesh = VO_TRUE;
				continue;
			}

			if((voOS_GetSysTime() - enRefleshMarkTime) >10 *1000)//m_manager.m_reader.m_update_period)//
				enReflesh = VO_TRUE;
			if( enReflesh) //refresh mpd
			{
				voOS_Sleep(100);
				//fresh UTC
				GetCurUTCTime(nCurUTCTime,VO_TRUE);
				if( !(m_manager.Parse_mpd( &m_fragment)))
					break;
				curSegmentCounts  =  m_manager.GetSegmentCounts();
				VOLOGR("Refresh MPD. NewSegCounts:%d",curSegmentCounts);
				enReflesh = VO_FALSE;

				enRefleshMarkTime = voOS_GetSysTime();
				continue;
			}

			Seg_item * pSegItem = NULL;
			m_manager.get_segmentItem(&pSegItem,0,1);
			if(pSegItem)
				timeline_time = pSegItem->starttime/m_manager.m_reader.m_manifest->video_scale;
			
			Avaiable_time = m_manager.m_reader.m_read_time;

			//get UTC not fresh
			GetCurUTCTime(nCurUTCTime,VO_FALSE);
			left_time =(nCurUTCTime - Avaiable_time-timeline_time);
			if( left_time<0)
			{
				 VOLOGR("Left_time:%lld", left_time);
				 voOS_Sleep( sleep_time );
				 continue;
			}
			else 
			{
				int tmpindex = 0;
				if(m_manager.m_reader.m_manifest->video_scale==1)
				{
					tmpindex = (left_time*m_manager.m_reader.m_manifest->video_scale*1000)/(video_duration);
				}
				else
					tmpindex = (left_time*m_manager.m_reader.m_manifest->video_scale)/(video_duration);

					
				if(index_video == tmpindex)
				{
					voOS_Sleep(500);
					continue;
				}
				index_video = tmpindex;
				VOLOGR("index:%d", index_video);
				//if(index_video > curSegmentCounts-6)
				//	enReflesh = VO_TRUE;
				m_manager.get_segmentItem(&ptr_item,index_video,1);
				if(!ptr_item )
				{
					enReflesh = VO_TRUE;
					continue;
				}
				int count = 0;
				while(count<2 && ptr_item && !m_is_pause)
				{
					start_time = ptr_item->starttime;
					memset(url_segment, 0, 1024);
					ret = m_manager.pop_fragment(ptr_item, url_segment, download_bitrate,m_lastFregmentDuration);
					repre_index = ptr_item->representation_index;
					if( ret == -1 || m_bMediaStop )
						break;

					if( ptr_item->nIsVideo == 1 && ptr_item->group_index == m_manager.m_video_index )
					{ 
						VO_BOOL is_video = VO_TRUE;
						if(repre_index!= rpe_index_video_old)
						{
							GetTrackInfo(is_video, m_manager.m_video_index,repre_index);
						}				       
						rpe_index_video_old = repre_index;
						VOLOGR(" m_nLastTimeStamp_Video %lld", m_nLastTimeStamp_Video);
						download_bitrate = 0;
						download_bitrate = GetMediaItem( url_segment, ptr_item->group_index );
						m_judgementor.add_item( download_bitrate );
					}
					else if(ptr_item->nIsVideo == 0&& m_manager.m_audio_index != -1 && ptr_item->group_index == m_manager.m_audio_index)
					{  
						VO_BOOL is_video = VO_FALSE;
						if(repre_index!= rpe_index_audio_old)
						{
							GetTrackInfo(is_video, m_manager.m_audio_index,repre_index);
							m_manager.bitrate_changed_Audio = VO_TRUE;
						}
							
						rpe_index_audio_old = repre_index;
						VOLOGR(" m_nLastTimeStamp_Audio : %lld", m_nLastTimeStamp_Audio);
						VO_S64 temp = GetMediaItem( url_segment, ptr_item->group_index );
						download_bitrate+= temp;
						if(temp == -1)
						{
							download_bitrate = -1;
						}
					} 
				
					if(download_bitrate == -1)
						enReflesh = VO_TRUE;
					m_manager.get_segmentItem(&ptr_item, index_video, 0);
					count++;
				}
			//	VOLOGR("While loop");
			}
		}

#elif defined LEON_ADD
		VO_BOOL enReflesh = VO_FALSE;
		VO_U32 nRefreshStart = voOS_GetSysTime();
		VO_U64 nTime = 0;
		VO_U64 nLastVideoTime =0;
		VO_U64 nn = 0;
		//added by avtime
		while( !m_bMediaStop )
		{
			if( !ptr_item ||voOS_GetSysTime() - nRefreshStart > 5000|| enReflesh)
			{   

				flag = VO_TRUE;
				enReflesh = VO_FALSE;
				if( !ptr_item )
				{
					voOS_Sleep( sleep_time );
				}
				VO_U32 ss = voOS_GetSysTime();
				int left_chunk_before = m_manager.FindIndexByStartTime(last_fragment_time);
				if( !(m_manager.Parse_mpd( &m_fragment)))
					break;

				nRefreshStart = voOS_GetSysTime();
				curSegmentCounts = m_manager.GetSegmentCounts();
				int left_chunk = m_manager.FindIndexByStartTime(last_fragment_time);
		
 				if(left_chunk_before == left_chunk)
  				{
  					int left_number = curSegmentCounts-left_chunk_before;
  					if(left_chunk_before<curSegmentCounts)
  					{
  						ini_chunk = left_chunk+1;
						VOLOGR("ReFresh MPD. ini_chunk,%d", ini_chunk);
						//fragment_number = curSegmentCounts/4-left_number;
  					}
  					else if(left_chunk_before >= curSegmentCounts )
  					{
  						enReflesh  = VO_TRUE;
						VOLOGR("2");
  	  					continue;
  					}
  					VOLOGR(" left_chunk_before==left_chunk");
  					flag = VO_FALSE;

				}
 				else
				{
					nTime = m_manager.GetLastStartTime(1);
					nn = (nTime - nLastVideoTime)*1000./m_manager.m_reader.video_scale;
					if(nn > (3000 + 16000))
					{
						left_chunk = 0;
						VOLOGR("Refresh MPD.nTime:%lld",nTime);
					}

					if(!ptr_item || left_chunk <= 2)
					{
						ini_chunk = (curSegmentCounts - 12);
					}
					else
					{
						ini_chunk = left_chunk +1;
					}

				}

				m_manager.get_segmentItem(&ptr_item,ini_chunk);
				VOLOGR("Refresh MPD.ptr_item NULL. %d,%p,left_chunk:%d,%d[%d],%lld",curSegmentCounts,ptr_item,left_chunk,ini_chunk,voOS_GetSysTime() - ss,nn );
				if(ptr_item)
					last_fragment_time = ptr_item->starttime;
				nTime = ptr_item->starttime;
				continue;
			}

			start_time = ptr_item->starttime;
			memset(url_segment, 0, 1024);
			ret = m_manager.pop_fragment(ptr_item, url_segment, download_bitrate,m_lastFregmentDuration);
			repre_index = ptr_item->representation_index;

			if( ret == -1 || m_bMediaStop )
				break;

			VOLOGR("repre_index %d",repre_index);
			VO_U32 ss1 = voOS_GetSysTime();
			if( ptr_item->nIsVideo == 1&&ptr_item->group_index == m_manager.m_video_index )
			{ 
				VO_BOOL is_video = VO_TRUE;
			//	VOLOGI("m_video_index %s",url_segment);
				if(repre_index!= rpe_index_video_old)
				{
					GetTrackInfo(is_video, m_manager.m_video_index,repre_index);
				}
									       
				rpe_index_video_old = repre_index;
				VOLOGR(" m_nLastTimeStamp_Video %lld", m_nLastTimeStamp_Video);
				download_bitrate =0;
				download_bitrate = GetMediaItem( url_segment, ptr_item->group_index );
				m_judgementor.add_item( download_bitrate );
				nLastVideoTime = ptr_item->starttime;
			}
			else if(ptr_item->nIsVideo == 0&& m_manager.m_audio_index != -1 && ptr_item->group_index == m_manager.m_audio_index)
			{  

			//	VOLOGI("m_audio_index %d",repre_index);
			//	VOLOGI("m_audio_index_old %d",rpe_index_audio_old);
				VO_BOOL is_video = VO_FALSE;
				if(repre_index!= rpe_index_audio_old)
				{
					GetTrackInfo(is_video, m_manager.m_audio_index,repre_index);
					m_manager.bitrate_changed_Audio = VO_TRUE;
				}
					
				rpe_index_audio_old = repre_index;
				VOLOGR(" m_nLastTimeStamp_Audio : %lld", m_nLastTimeStamp_Audio);
				VO_S64 temp = GetMediaItem( url_segment, ptr_item->group_index );
				download_bitrate+= temp;
				if(temp == -1)
				{
					download_bitrate = -1;
				}
				
			} 
			VOLOGR("Refresh MPD.DownLoad :%d", voOS_GetSysTime() - ss1);
			if( m_bRecoverFromPause )
			{
				m_bRecoverFromPause = VO_FALSE;
				enReflesh = VO_TRUE;
			}
			VOLOGR("download_bitrate: %d", download_bitrate);
     
			if(download_bitrate == -1)
				enReflesh = VO_TRUE;
		
				
			VOLOGR("ptr_item %d,%p",curSegmentCounts,ptr_item);
			last_fragment_time = ptr_item->starttime;
		   	ptr_item = m_manager.GetNextChunk(ptr_item);

		}
#else
		while( !m_bMediaStop )
		{
			if( !ptr_item ||fragment_number > curSegmentCounts/4|| live_after_pause)
			{   
				flag = VO_TRUE;
				live_after_pause = VO_FALSE;
				if( !ptr_item )
				{
					voOS_Sleep( sleep_time );
				}
				int left_chunk_before = m_manager.FindIndexByStartTime(last_fragment_time);

				VOLOGR(" left_chunk_before %d", left_chunk_before);
				if( !(m_manager.Parse_mpd( &m_fragment)))
				{
					break;
				}
				else
					curSegmentCounts = m_manager.GetSegmentCounts();

				int left_chunk = m_manager.FindIndexByStartTime(last_fragment_time);
				int gene_chunk = m_manager.GetSegmentCounts();
				if(left_chunk_before == left_chunk)
				{
					int left_number = gene_chunk-left_chunk_before;
					if(left_chunk_before<gene_chunk)
					{
						m_manager.get_segmentItem(&ptr_item, left_chunk+1);
						fragment_number = curSegmentCounts/4-left_number;
					}
					else if(left_chunk_before >= gene_chunk )
					{
						fragment_number = curSegmentCounts/4+1;
						continue;
					}
					VOLOGR(" left_chunk_before==left_chunk");
					flag = VO_FALSE;
				}
				else
				{
					if(left_chunk<0)
					{
						left_chunk -= left_chunk;
						left_chunk = curSegmentCounts/2;
						VOLOGR(" left_chunk1 <0 last_fragment_time %lld", last_fragment_time);
						m_manager.get_segmentItem(&ptr_item, left_chunk);
						flag = VO_FALSE;
						VOLOGR(" left_chunk2 <0 last_fragment_time %lld", last_fragment_time);
					}
					else if(left_chunk<=curSegmentCounts/5)
					{
						left_chunk+=curSegmentCounts/3;
						VOLOGR(" left_chunk1 <0 last_fragment_time %lld", last_fragment_time);
						m_manager.get_segmentItem(&ptr_item, left_chunk);
						flag = VO_FALSE;
						VOLOGR(" left_chunk2 <0 last_fragment_time %lld",  last_fragment_time);
					}
					fragment_number =1;
				}

				if(flag)
				{
					if( m_manager.m_reader.get_segmentItem( &ptr_item, last_fragment_time ))
					{
						ptr_item = ptr_item->ptr_next;
					}
					else
					{
						ptr_item = m_manager.m_reader.m_ptr_FI_head;
					}
					fragment_number =1;
				}
				count_video++;
			}

			start_time = ptr_item->starttime;
			VOLOGR("start_time  %lld",start_time);
			VOLOGR("start_time_tamp %lld",start_time_tamp);
			VOLOGR("head_time   %lld", m_manager.m_reader.m_ptr_FI_head->starttime);
			VOLOGR("fragment_number: %d", fragment_number);
			memset(url_segment, 0, 1024);
			ret = m_manager.pop_fragment(ptr_item, url_segment, download_bitrate,m_lastFregmentDuration);
			repre_index = ptr_item->representation_index;

			if( ret == -1 || m_bMediaStop )
				break;

			VOLOGR("repre_index %d",repre_index);
			if( ptr_item->nIsVideo == 1&&ptr_item->group_index == m_manager.m_video_index )
			{ 
				VO_BOOL is_video = VO_TRUE;
				VOLOGR("m_video_index %s",url_segment);
				if(count_video)
				{
					count_video++;				 
				}
				else
				{
					if(repre_index!= rpe_index_video_old)
					{
						GetTrackInfo(is_video, m_manager.m_video_index,repre_index);
					}
				}

				rpe_index_video_old = repre_index;
				VOLOGR(" m_nLastTimeStamp_Video %lld", m_nLastTimeStamp_Video);
				download_bitrate =0;
				download_bitrate = GetMediaItem( url_segment, ptr_item->group_index );
				m_judgementor.add_item( download_bitrate );
			}
			else if(ptr_item->nIsVideo == 0&& m_manager.m_audio_index != -1 && ptr_item->group_index == m_manager.m_audio_index)
			{  

				VOLOGR("m_audio_index %d",repre_index);
				VOLOGR("m_audio_index_old %d",rpe_index_audio_old);
				VO_BOOL is_video = VO_FALSE;
				if(count_video)
				{
					count_video++;
				}
				else if(repre_index!= rpe_index_audio_old)
				{
					GetTrackInfo(is_video, m_manager.m_audio_index,repre_index);
					m_manager.bitrate_changed_Audio = VO_TRUE;
				}

				rpe_index_audio_old = repre_index;
				VOLOGR(" m_nLastTimeStamp_Audio : %lld", m_nLastTimeStamp_Audio);
				VO_S64 temp = GetMediaItem( url_segment, ptr_item->group_index );
				download_bitrate+= temp;
				if(temp==-1)
				{
					download_bitrate = -1;
				}
			} 
			if( m_bRrecoverFromPause )
			{
				m_bRrecoverFromPause = VO_FALSE;
				live_after_pause = VO_TRUE;
			}
			VOLOGR("download_bitrate: %d", download_bitrate);

			if(count_video==3)
			{
				count_video = 0;
			}
			if(download_bitrate==-1)
			{
				fragment_number = curSegmentCounts/4+1;
			}

			last_fragment_time = ptr_item->starttime;
			ptr_item = m_manager.GetNextChunk(ptr_item);
			fragment_number++;
		}

#endif
	}

	VOLOGR("Ended!");
	m_manager.close();
	send_eos();
	VOLOGR( "End of start_livestream" );
}


VO_VOID vo_mpd_streaming::stop_dashstream( VO_BOOL isflush )
{
	VOLOGR("+stop_livestream");
	if( isflush )
	{
		VOLOGR("it is flush");
	}
	m_is_flush = isflush;
	m_bMediaStop = VO_TRUE;
	m_is_first = VO_TRUE;
	m_is_pause =VO_FALSE;
	need_flush();
	VOLOGR("stop_livestream1");
	vo_thread::stop();
	VOLOGR("stop_livestream2");

	m_lastFregmentDuration = 0;
	download_bitrate = 0;

	m_manager.last_QL_bitrate = -1;
	m_nSyncTimeStamp = -1;

	VOLOGR("-stop_livestream");

}
void vo_mpd_streaming::thread_function()
{
	VOLOGR("Start thread!");
	start_dashstream();
}
/*
VO_U32 vo_mpd_streaming::threadfunc( VO_PTR pParam )
{
	vo_mpd_streaming * ptr_obj = ( vo_mpd_streaming * )pParam;
    ptr_obj->m_thread = (voThreadHandle)1;
	// ptr_obj->m_thread = 0;
	//ptr_obj->add_media_data();//
    ptr_obj->SendBufferFromList();
	ptr_obj->m_thread = 0;
	return 1;
}*/
VO_S64 vo_mpd_streaming::SendBufferFromList()
{
	VOLOGR("+ SendBufferFromList");
	return 0;
	while(1)
	{

	
         voOS_Sleep(1);
		if(m_List_buffer.GetCount() ==0)
		{
			if(m_bNeedFlushBuffer) m_bNeedFlushBuffer = VO_FALSE;
			if(m_bMediaStop  ) break;
			continue;
		}
	    voCAutoLock lock (&g_voMemMutex);
		if(m_List_buffer.GetCount()>0)
		{
			 VOLOGR("in SendBufferFromList %d",m_List_buffer.GetCount());
			 DATA_BUFFER_ST_DASH  * ptr_buffer = m_List_buffer.GetHead();
			if(!m_bMediaStop && !m_bNeedFlushBuffer && !m_is_seek){
				VOLOGR( "===SendBuffer:ID(%d),Size(%d),TimeStamp(%llu),newfile(%d)", ptr_buffer->index,ptr_buffer->ptr_buffer->nSize,ptr_buffer->ptr_buffer->nStartTime,ptr_buffer->newfile );
				add_media_data(ptr_buffer->ptr_buffer,ptr_buffer->index,ptr_buffer->newfile);
			}
			delete []ptr_buffer->ptr_buffer->pData;
			ptr_buffer->ptr_buffer->pData = NULL;
			delete ptr_buffer->ptr_buffer;
			ptr_buffer->ptr_buffer = NULL;
			delete ptr_buffer;
			ptr_buffer = NULL;
			m_List_buffer.RemoveHead();
		}
	}
	VOLOGR("- SendBufferFromList");
	return 0;
}

VO_LIVESRC_SAMPLE  vo_mpd_streaming:: GetParam(int type){
	VO_LIVESRC_SAMPLE sample;
    memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );
	if(type == 1){
    //    m_databuffer_video.GetBuffer(&sample);

	}
	else if(type ==0){
	//	m_databuffer_audio.GetBuffer(&sample);

	}
	return sample;
	
}



VO_VOID vo_mpd_streaming::add_media_data( VO_DASH_FRAME_BUFFER * ptr_buffer , VO_U32 index , VO_BOOL newfile ){
	
	VOLOGR( "====================================add_media_data" );
	VOLOGR( "_send_media_data m_audioCodecType %d", m_audioCodecType);
	VOLOGR( "_send_media_data m_videoCodecType %d", m_videoCodecType);

	
//	if( m_bRrecoverFromPause )
	//	return;

	VO_LIVESRC_SAMPLE sample;
	memset( &sample , 0 , sizeof( VO_LIVESRC_SAMPLE ) );
	sample.Sample.Buffer = ptr_buffer->pData;
	sample.Sample.Time = ptr_buffer->nStartTime;
	sample.Sample.Size = ptr_buffer->nSize;
	VOLOGR( "sample.Sample.Time  %lld", sample.Sample.Time );
	

//	sample.Sample.Duration =(ptr_buffer->nEndTime-ptr_buffer->nStartTime)/m_manager.GetTimeScaleMs();;



	if( index == 0 )
	{	
		sample.nTrackID = VO_LIVESRC_OUTPUT_AUDIO;
		sample.nCodecType = m_audioCodecType;
		//if(out_audio)	fwrite(ptr_buffer->pData,1,ptr_buffer->nSize,out_audio);
		VOLOGR( "====================================Audio CodecType: %d",m_audioCodecType );
	}
	else if( index == 1 )
	{
		sample.nTrackID = VO_LIVESRC_OUTPUT_VIDEO;
		sample.nCodecType = m_videoCodecType;
		//if(out_video)	fwrite(ptr_buffer->pData,1,ptr_buffer->nSize,out_video);
	 VOLOGR( "====================================Video CodecType: %d",m_videoCodecType );
	}
	else if(index == 4){
		sample.nTrackID = VO_LIVESRC_OUTPUT_TRACKINFO;
	}

#if 1
	if( newfile )
	{
	//	if(index == 1 )
		{
			sample.Sample.Flag  = VO_LIVESRC_FLAG_BUFFER_HEADDATA;
		}
#ifdef VOME
			sample.Sample.Flag |= OMX_BUFFERFLAG_EXTRADATA | OMX_VO_BUFFERFLAG_NEWSTREAM;
#else

			sample.Sample.Flag |= VOMP_FLAG_BUFFER_NEW_FORMAT;
#endif	
	}
	else
		sample.Sample.Flag = 0;
#endif

	if( ptr_buffer->nFrameType == 0 )
	{
#ifdef VOME
		sample.Sample.Flag = sample.Sample.Flag | OMX_BUFFERFLAG_SYNCFRAME;
#else
		sample.Sample.Flag = sample.Sample.Flag | VOMP_FLAG_BUFFER_KEYFRAME;
#endif
	}
	
	VO_BOOL bFlush = VO_FALSE;
	while( m_is_pause )
	{
	
		//VOLOGR( "in pause333" );

		if( is_live()&&is_update())
		{
			need_flush();
			return;
		}
		voOS_Sleep( 10 );
		if( m_bMediaStop )
			return;
		if(m_bNeedFlushBuffer )
			return;
		if(m_is_seek)
			return ;
	}

   
		if( m_datacallback_func && !m_is_seek && !m_bMediaStop)
		{
			VOLOGR("before Data Callback. 0x%0X", sample.Sample.Flag);
		   
			m_datacallback_func( m_ptr_callbackobj , &sample );

#ifdef _USE_NEW_BITRATEADAPTATION
			if(index == 1 && !newfile)//video
				m_manager.AddtimeStamp2Map(sample.Sample.Time, m_nCountsInOneChunk_video == 1 ? 1:0 );
#endif
		}
	 
}



VO_PBYTE vo_mpd_streaming::GetMediacodecItem(VO_CHAR * ptr_url){
	if( !m_fragment.open( ptr_url , DOWNLOAD2MEM ) ){
	   VOLOGR("GetMediaItem fail");
		return NULL;
	}
	VO_S64 fragment_size = 0;
	VO_S64 read_size = 0; 
    m_fragment_buffer_codec = NULL;
	fragment_size = read_size = m_fragment.get_content_length();
	m_fragment_buffer_codec = new VO_BYTE[ fragment_size + 1 ];
	memset( m_fragment_buffer_codec , 0 , fragment_size + 1 );
	VO_PBYTE ptr = m_fragment_buffer_codec;
	while( read_size != 0){
		voOS_Sleep(10);
	VO_S64 size = m_fragment.read( ptr , read_size );
		if( size == -2 )
			continue;
		if( size == -1 )
		{
			break;
		}
		read_size -= size;
		ptr = ptr + size;
	}
	
	  m_fragment.closedownload();
	  VOLOGR("-GetMediaItem");
	  return m_fragment_buffer_codec;
}



VO_VOID vo_mpd_streaming::DownloadTrackInfo(VO_BOOL is_video,VO_CHAR * ptr_url,VO_S32 stream_index,VO_S32 represen_index)
{
	//ptr_url ="http://usa.visualon.com/streaming/VerizonDash/sourceA/Dash/h264baseline-1000/video/QL_749952/Header";
	VOLOGR("DownloadTrackInfo %s ",ptr_url);
         m_manager.cur_bitrate_index = represen_index;
	if( !m_fragment.open( ptr_url , DOWNLOAD2MEM ) ){//get the 
		VOLOGR("DownloadTrackInfo fail");
		return ;
	}
	VO_S64 fragment_size = 0;
	VO_S64 read_size = 0; 
	VO_PBYTE fragment_buffer = NULL;
	fragment_size = read_size = m_fragment.get_content_length();
	fragment_buffer = new VO_BYTE[ fragment_size + 1 ];
	memset( fragment_buffer , 0 , fragment_size + 1 );
	VO_PBYTE ptr = fragment_buffer;
	while( read_size != 0){
		voOS_Sleep(10);
		VO_S64 size = m_fragment.read( ptr , read_size );
		if( size == -2 )
			continue;
		if( size == -1 )
		{
			break;
		}
		read_size -= size;
		ptr = ptr + size;
	}

	VO_PARSER_INPUT_BUFFER input;
	memset(&input, 0, sizeof(VO_PARSER_INPUT_BUFFER));

	if(is_video)
		input.nStreamID = 0; // video nsrreamID 
	else
		input.nStreamID = 1; // audio
		

	input.pBuf = fragment_buffer;
	input.nBufLen = fragment_size;
	input.pReserved = &start_time;
	input.nFlag = VO_PARSER_FLAG_STREAM_DASHHEAD;
	VO_U32 start = voOS_GetSysTime();
	m_dashhparser_api.Process( m_dashparser_handle,&input ); //the dash parser_api
	VO_U32 end = voOS_GetSysTime();
	VOLOGR("DownloadTrackInfo OK! Cost %f Seconds!\n" , ( end - start ) / 1000.);
	delete []fragment_buffer;
	m_fragment.closedownload();
		

}

void  vo_mpd_streaming::GetTrackInfo(VO_BOOL isVedio,VO_S32 stream_index,VO_S32 represen_index){

	//if(isVedio){
	 Representation * rpe = m_manager.get_qualityLevel(stream_index,represen_index);
	 if(isVedio) cur_pre = rpe;
	// VO_CHAR *  base_irl =  "http://usa.visualon.com/streaming/VerizonDash/sourceA/Dash/h264baseline-1000/video/QL_749952/Header";
	 VO_CHAR *  base_irl = rpe->initalUrl;
	 m_manager.replace(base_irl,"vlcManifestVideo.mpd","");
	 m_manager.replace(base_irl,"vlcManifestAudio.mpd","");
	 m_manager.replace(base_irl,"ManifestVideo.mpd","");
	 m_manager.replace(base_irl,"Manifest.mpd","");
	 m_manager.replace(base_irl,"verizon.mpd","");
	 m_manager.replace(base_irl,"generic.mpd","");
	 m_manager.replace(base_irl,"vlc","");
	 m_manager.replace(base_irl,"x264_base_2seg_2buf.mpd","");
	 VO_CHAR c[10];
	 memset(c,0x00,sizeof(c));
#if defined _WIN32
	 strcat(c,"%I64d");
	 //	strcat(base_url, "/QualityLevels(%I64d)/Fragments(");
#elif defined LINUX
	 strcat(c,"%lld");
	 //	strcat(base_url, "/QualityLevels(%lld)/Fragments(");
#elif defined _IOS
	 strcat(c,"%lld");
#elif defined _MAC_OS
	 strcat(c,"%lld");
#endif
	 char str[255];
	 memset(str,0x00,sizeof(str));
#ifdef _WIN32
	 _snprintf(str,sizeof(str),c,rpe->bitrate);
#else
	snprintf(str,sizeof(str),c,rpe->bitrate);
#endif
	 m_manager.replace(base_irl,"$Bandwidth$",str);
	 m_manager.replace(base_irl,"$RepesendationID$",rpe->id);
	 m_manager.replace(base_irl,"$RepresentationID$",rpe->id);
	 m_manager.replace(base_irl,"mp4v","mp4");
	 m_manager.replace(base_irl,"mp4a","mp4");
	 VOLOGI("Header URL:%s",base_irl);
	 DownloadTrackInfo(isVedio,base_irl, stream_index, represen_index);
	// return m_pTrackInfoEx_Video;
//	}

//	return m_pTrackInfoEx_Audio;


}


VO_S64 vo_mpd_streaming::GetMediaItem( VO_CHAR * ptr_url, VO_S32 index_stream ){
	   VOLOGR("GetMediaItem ptr_url %s",ptr_url);
	   if( !m_fragment.startdownload( ptr_url , DOWNLOAD2MEM ) ){//get the 
	//   if( !m_fragment.open( ptr_url , DOWNLOAD2MEM ) ){//get the 
	   VOLOGR("GetMediaItem fail");
	   if(is_live()&&!is_update())
	   {
		    try_counts++;
            VOLOGR("GetMediaItem try_counts %d",try_counts);
	   }
		return -1;
	}
	try_counts = 0;
	
	if(start_time <0)
		start_time = 0;

	VO_S64 fragment_size = 0;
	VO_S64 read_size = 0; 
	VO_PBYTE fragment_buffer = NULL;
	fragment_size = read_size = m_fragment.get_content_length();
	fragment_buffer = new VO_BYTE[ fragment_size + 1 ];
	memset( fragment_buffer , 0 , fragment_size + 1 );
	VO_PBYTE ptr = fragment_buffer;
	VO_BOOL bExit = VO_FALSE;
	while( read_size != 0 && !m_bMediaStop ){
		voOS_Sleep(10);
		VO_S64 size = m_fragment.read( ptr , read_size );
		if( size == -2 )
			continue;
		if( size == -1 )
		{
			break;
		}
		read_size -= size;
		ptr = ptr + size;

		if( is_live()&&is_update() && m_is_pause)
		{
			need_flush();
			bExit = VO_TRUE;
			break;
		}
	}
	
	if(!bExit ||!m_is_seek){
        if(m_bMediaStop)
		{
			VOLOGR("It stopped, return GetMediaItem");
		}
		else
		{
			if( m_manager.bitrate_changed_Video == VO_TRUE)
			{
				VOLOGR("bitrate_changed_Video");

		    	UpdateTrackInfoData(VO_DASH_PID_VIDEO_TRACKINFO);

			}

			VO_PARSER_INPUT_BUFFER input;
			memset(&input, 0, sizeof(VO_PARSER_INPUT_BUFFER));
			if(index_stream == m_manager.m_video_index)
			{
				input.nStreamID = 0; // video nsrreamID 
				m_nLastTimeStamp_Video = start_time/ m_manager.GetTimeScaleMs();
#ifdef _USE_NEW_BITRATEADAPTATION
				m_nCountsInOneChunk_video = 1;
#endif
				start_time -= start_time_tamp;
			}
			else
			{
				input.nStreamID = 1; // audio
				sample_number = 1;
				m_nLastTimeStamp_Audio = start_time/ m_manager.GetTimeScaleMsAudio();
				start_time -= m_manager.m_manifest.m_start_time_audio;
			}

			input.pBuf = fragment_buffer;
			input.nBufLen = fragment_size;
		    VOLOGR("start_time dg dg %lld" , start_time);
			input.pReserved = &start_time;
			input.nFlag = VO_PARSER_FLAG_STREAM_DASHDATA;
			VO_U32 start = voOS_GetSysTime();
         	m_dashhparser_api.Process( m_dashparser_handle,&input ); //the dash parser_api
			VO_U32 end = voOS_GetSysTime();
			VOLOGR("GetMediaItem OK! Cost %f Seconds!\n" , ( end - start ) / 1000.);
		}
	}
	else{
		VOLOGR("Seek skip download chunk");
	}

	
	  delete []fragment_buffer;
	  m_fragment.closedownload();
	  VO_S64  br = m_fragment.get_download_bitrate();

	  VOLOGI( "BAInfo*** Download Speed %d , File Bitrate %d , URL %s" , (VO_U32)br, (VO_U32)m_manager.get_cur_bitrate(), ptr_url );
	  VOLOGR("-GetMediaItem");
	  return br;

	 // return m_fragment.get_download_bitrate();
	
}


VO_S64 vo_mpd_streaming::AddSendDataList(VO_SOURCE_SAMPLE * ptr_buffer , VO_U32 index ,VO_S8 frame_type,VO_BOOL newfile){

	//VOLOGR("in AddSendDataList %d",m_List_buffer.GetCount());
	if(m_bMediaStop || m_is_seek) return VO_ERR_NONE;
  // while(m_List_buffer.GetCount()>200 || m_bNeedFlushBuffer) voOS_Sleep(10);
	// while(m_List_buffer.GetCount()>200 ) voOS_Sleep(10);
	//voCAutoLock lock (&g_voMemMutex);
	
/*	DATA_BUFFER_ST_DASH *buffer = new DATA_BUFFER_ST_DASH;
	buffer->ptr_buffer = new VO_DASH_FRAME_BUFFER;
	memset(buffer->ptr_buffer , 0x00,sizeof(VO_DASH_FRAME_BUFFER));
	
	buffer->ptr_buffer->nEndTime = (ptr_buffer->Time+duration)/10000;
	buffer->ptr_buffer->nFrameType = frame_type;
//	buffer->ptr_buffer->nPos = ptr_buffer->
	buffer->ptr_buffer->nSize = ptr_buffer->Size & 0x7FFFFFFF;
	buffer->ptr_buffer->nStartTime = ptr_buffer->Time;
	buffer->ptr_buffer->pData = new VO_BYTE[buffer->ptr_buffer->nSize];
	VOLOGR( "===AddData:ID(%d),Size(%d),TimeStamp(%llu),newfile(%d)", index,buffer->ptr_buffer->nSize,buffer->ptr_buffer->nStartTime,newfile );
	memcpy(buffer->ptr_buffer->pData,ptr_buffer->Buffer,buffer->ptr_buffer->nSize);
	buffer->index = index;
	buffer->newfile = newfile;
    add_media_data(buffer->ptr_buffer,buffer->index,buffer->newfile);
	//m_List_buffer.AddTail(buffer);
	delete []buffer->ptr_buffer->pData;
	delete buffer->ptr_buffer;
	delete buffer;*/
	VO_DASH_FRAME_BUFFER dashbuffer;
	dashbuffer.nEndTime = (ptr_buffer->Time+duration)/10000;
	dashbuffer.nFrameType = frame_type;
	dashbuffer.nSize = ptr_buffer->Size & 0x7FFFFFFF;
	dashbuffer.nStartTime = ptr_buffer->Time;
	dashbuffer.pData = ptr_buffer->Buffer;

	add_media_data(&dashbuffer,index,newfile);
	return 0;

}

VO_U32 vo_mpd_streaming::SendTrackInfo(int isVideo,int frame_type){
   //  VO_DASH_FRAME_BUFFER pbuffer;
	VOLOGR("+SendTrackInfo");
	 VO_SOURCE_SAMPLE pbuffer;
    memset(&pbuffer, 0, sizeof(VO_SOURCE_SAMPLE));
	//memset(&pbuffer, 0, sizeof(VO_SOURCE_SAMPLE));
	#ifdef VOME
	if(isVideo)	
		CreateVideoTrackInfoBuffer(&pbuffer);
	else
		CreateAudioTrackInfoBuffer(&pbuffer);
	#else
	if(isVideo){
		VO_S32 bitrate_index =  m_manager.cur_bitrate_index;
	    Representation *headData = m_manager.get_qualityLevel(m_manager.m_video_index,bitrate_index);
	//	Representation *headData = cur_pre;
		// added by Lin Jun 20110519,sometimes it's NULL
		if(!headData)
		{
			VOLOGR("no headData");
			return -1;
		}

		if(!cur_video_ql)
		{
			VOLOGR("no headData 1");
			return -1;
		}

		/*pbuffer.Buffer = headData->codecPrivateData;
		pbuffer.Time = m_nCurTimeStamp_Video;//start_time /m_manager.GetTimeScaleMs();
		pbuffer.Size  = headData->length_CPD;*/
        pbuffer.Buffer = cur_video_ql->codecPrivateData;
		pbuffer.Time = m_nCurTimeStamp_Video;//start_time /m_manager.GetTimeScaleMs();
		pbuffer.Size  = cur_video_ql->length_CPD;


	
		//VOLOGR("headData size =%d",pbuffer.Size);
	}
	else {
		if( m_audioCodecType== VO_AUDIO_CodingWMA )
		{
			//send WmaPro headdata
			pbuffer.Buffer = cur_audio_ql->codecPrivateData;
			pbuffer.Size  = cur_audio_ql->length_CPD;

		//	pbuffer.Buffer = m_manager.m_manifest.adaptionSet[m_manager.m_audio_index].audio_QL.codecPrivateData;
		//	pbuffer.Size  = m_manager.m_manifest.adaptionSet[m_manager.m_audio_index].audio_QL.length_CPD;
		}
		else if( m_audioCodecType == VO_AUDIO_CodingAAC)
		{
			//aac
			unsigned char c;
			pbuffer.Buffer = &c;
			pbuffer.Size = 1;
		}
		pbuffer.Time = m_nCurTimeStamp_Audio;
	
   //	m_manifest->streamIndex[stream_index].audio_QL.samplerate_index = get_samplerate_index(nVal);
	}
#endif
	 
	  	AddSendDataList( &pbuffer , isVideo ,frame_type,VO_TRUE );
	 if(isVideo)	
		m_manager.bitrate_changed_Video = VO_FALSE;
	else
		m_manager.bitrate_changed_Audio = VO_FALSE;
	// ptr_obj->m_thread = 0;
	 VOLOGR("-SendTrackInfo");
	return 0;

}

VO_U32 vo_mpd_streaming::GetProgramInfo( VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo )
{
	return 1;
}

VO_VOID vo_mpd_streaming::GetCurUTCTime(time_t &httpUTC, VO_BOOL enFlesh)
{
	if(enFlesh && !m_bGetUTC2NTP)
	{
		m_bGetUTC2NTP = m_httpUTC.GetUTCTime(m_nCurUTCTime);
		m_nStartUTCMark = voOS_GetSysTime();
	}
	VOLOGR("Get NTP Time:%d,CurUTCTime:ll%d",m_bGetUTC2NTP,m_nCurUTCTime);
	if(m_bGetUTC2NTP == VO_FALSE)
	{
#ifdef WIN32
		SYSTEMTIME   sysTime;
		GetSystemTime(&sysTime);
		httpUTC = SystemTimeToTime_t(sysTime);
#else
		timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		httpUTC = tv.tv_sec + tz.tz_minuteswest*60;
		VOLOGR("tv:%d,%d", (tv.tv_sec ),tv.tv_usec);
		VOLOGR("tz:%d,%d", (tz.tz_minuteswest ),tz.tz_dsttime);
#endif
	}
	else
		httpUTC = m_nCurUTCTime + (voOS_GetSysTime() - m_nStartUTCMark)/1000;
}
