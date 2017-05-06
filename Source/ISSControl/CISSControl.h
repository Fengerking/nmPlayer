
#ifndef __CISSControl_H__

#define __CISSControl_H__

#include "CManifestManager.h"

#include "CDllLoad.h"
#include "voLog.h"
#include "vo_thread.h"
#include "CPtrList.h"
#include "voType.h"
#include "voLiveSource.h"
#include "voLiveSourcePlus.h"
#include "voDownloader.h"
#include "voSource2DataBuffer.h"
#include "voSource2.h"


struct Event
{
	VO_U32 nID; 
	VO_U32 nParam1;
	VO_U32 nParam2;
};
enum VO_ISSCONTROL_DATA_TYPE
{
	VO_ISSCONTROL_TT_VIDEO				= 0X00000001,	/*!< video chunk*/
	VO_ISSCONTROL_TT_AUDIO				= 0X00000002,	/*!< audio chunk*/
	VO_ISSCONTROL_TT_HEADER				= 0X00000003,	/*!< header data*/
};

#define ISS_PID_CALLBACK_STATUS 1
#define ISS_PID_CALLBACK_DATA 2

typedef struct
{
	VO_VOID* buffer;
	VO_U32 type;
}CALLBACK_BUFFER_ST;

class CISSControl:
	public vo_thread
{
public:
	CISSControl(void);
	~CISSControl(void);


	void	setWorkPath (const char * pWorkPath) {strcpy (m_szWorkPath, pWorkPath); strcat (m_szWorkPath, "/lib/");}

	VO_S32 GetDuration(){ return m_manager.get_duration(); }
	

	void SetLibOp(VO_LIB_OPERATOR* pValue);
	VO_VOID start_after_seek();

	VO_VOID pause(){ m_is_pause = VO_TRUE; VOLOGI("++++++++++++++++++++++++++++++++++++++++++++++m_is_pause: %d",m_is_pause);}

	VO_U32 SetDrmCallBack(VO_PTR pValue);
	virtual void thread_function();

protected:

	static VO_U32 threadfunc( VO_PTR pParam );
	VO_VOID start_smoothstream();
	VO_VOID stop_smoothstream( VO_BOOL isflush = VO_FALSE );




	VO_S64 GetMediaItem( VO_CHAR * ptr_item, VO_S32 index_stream );

// 	static void VO_API ParserProc(VO_PARSER_OUTPUT_BUFFER* pData);
// 
// 	VO_VOID audio_data_arrive( VO_SMTH_FRAME_BUFFER * ptr_buffer );
// 	VO_VOID video_data_arrive( VO_SMTH_FRAME_BUFFER * ptr_buffer );

	VO_S32 GetFrameFormat (VO_BYTE* buffer, VO_U32 nBufSize);
	VO_S32 GetFrameType (VO_BYTE* pInData, VO_U32 nInSize);

	VO_VOID NotifyData();
	VO_VOID UpdateTrackInfoData(VO_U32 nID);

	VO_VOID send_media_data( VO_SOURCE2_SAMPLE * ptr_buffer , VO_U32 index , VO_BOOL newfile = VO_FALSE );


	VO_S64 AddSendDataList(VO_SOURCE2_SAMPLE * ptr_buffer , VO_U32 index , VO_BOOL newfile= VO_FALSE);



	VO_LIVESRC_CALLBACK	*m_pCallbackPlus;


protected:
	/*use to flush BufferLish*/
	VO_BOOL m_bNeedFlushBuffer;

	CManifestManager m_manager;

	VO_S64 download_bitrate;
	VO_S64 m_lastFregmentDuration;
	VO_S64 start_time;
	VO_S64 seek_time;

	VO_BOOL m_bMediaStop;
	VO_BOOL m_recoverfrompause;
	VO_BOOL m_is_pause;
	VO_BOOL m_is_seek;
	VO_BOOL m_seeked;
	VO_BOOL m_isPushMode;





	VO_BOOL m_is_H264;
	VO_BOOL m_is_first;

	VO_BOOL m_is_flush;

	CDllLoad			m_dlEngine;
	char				m_szWorkPath[256];



	VO_LIVESRC_PLUS_API		m_liveSrcPlus;
	VO_LIVESRC_CALLBACK		m_liveSrcPlusCallback;
	VO_PTR								m_livePlusHandle;


private:
	VO_S64  m_nLastTimeStamp_Video;
	VO_S64 m_nLastTimeStamp_Audio;

	VO_U64 m_nCurTimeStamp_Audio;
	VO_U64 m_nCurTimeStamp_Video;

public:
	int GetTrackNumber();


public:
	VO_U32 Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_CALLBACK * pCallback );
	VO_U32 Uninit();
	VO_U32 Open();
	VO_U32 Close();
	VO_U32 GetCurSelTrackInfo( VO_U32 nOutPutType , VO_SOURCE2_TRACK_INFO * pTrackInfo );
	VO_U32 SelectTrack( VO_U32 nStreamID , VO_U32 nSubStreamID , VO_U32 nTrackID );
	VO_U32 GetDRMInfo( VO_SOURCE2_DRM_INFO **ppDRMInfo );
	VO_U32 GetParam( VO_U32 nParamID, VO_PTR pParam );
	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);
	VO_U32 GetStreamCount(VO_U32 *pCount);
	VO_S32 Seek( VO_U64 *pTimeStamp );
	VO_U32 Start();
	VO_U32 Pause();
	VO_U32 Stop();
	VO_U32 GetStreamInfo( VO_U32 nStream, VO_SOURCE2_STREAM_INFO **ppStreamInfo );
	VO_U32 GetDuration( VO_U64 * pDuration ) { *pDuration = m_nDuration; return VO_RET_SOURCE2_OK;}
	VO_U32 GetSample( VO_U32 nOutPutType , VO_PTR pSample );
	VO_VOID freeLiveSrcPlus();
private:
#ifdef __DUMP
	FILE  *video_ff ;
	FILE  *audio_ff;
#endif
	VO_U64 m_nDuration;
	VO_CHAR m_strUrl[1024];
	VO_U32  m_uStreamCounts;
	VO_PTR		m_pDrmValue;
	VODOWNLOAD_FUNC *m_pDownloadFunc;
	VO_PBYTE m_pContentBuffer;
	VO_LIB_OPERATOR *m_pLibOp;
	VO_SOURCE2_CALLBACK *m_pSource2Callback;
private:
	VO_U32 ToDownLoad(VO_CHAR * pUrl, VO_ISSCONTROL_DATA_TYPE type, unsigned int *uTime );
	VO_U32 SetDataSource( VO_CHAR * pUrl);
	VO_VOID send_eos();
	VO_VOID need_flush();
	VO_U32	SendTrackInfo(int isVideo);
	VO_U32  LoadISSPlus();
	static VO_S32 SendEvent (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	static VO_S32 DataArrived (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);
	VO_VOID AddSampleToList(VO_SOURCE2_SAMPLE * ptr_buffer , VO_SOURCE2_OUTPUT_TYPE nType );
	VO_U32 video_data_arrive( VO_SOURCE2_SAMPLE *pSample );
	VO_U32 audio_data_arrive( VO_SOURCE2_SAMPLE * pSample );

	VO_SOURCE2_STREAM_INFO *m_pStreamInfo;
	VO_SOURCE2_SUBSTREAM_INFO *m_pCurSubStreamInfo_video;
	VO_SOURCE2_SUBSTREAM_INFO *m_pCurSubStreamInfo_audio;


	VO_U32 m_nCulStreamID_video;
	VO_U32 m_nCulStreamID_audio;
	VO_U32    m_audioCodecType;
	VO_U32    m_videoCodecType;
	VO_LIB_OPERATOR *m_libOP ;

	voSource2DataBufferItemList m_AudioBuffer;
	voSource2DataBufferItemList m_VideoBuffer;


	voCMutex m_voMemMutexAudio;
	voCMutex m_voMemMutexVideo;
};


#endif
