#ifdef _WIN32
#include <Windows.h>
#endif //_WIN32
#include "voLog.h"
#include "filebase.h"
#include "utility.h"
#include "network.h"
#include "macro.h"
#include "voSource2ParserWrapper.h"
#include "RTSPSource.h"
#include "RTSPTrack.h"
#include "voRTSPSDK.h"
#include "voOnStreamType.h"
#include "voStreaming.h"
#include "voCheck.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define VIDEO_FRAME_BUFFER_SIZE (1024*256)
#define AUDIO_FRAME_BUFFER_SIZE (1024*28)

CVORTSPTrack2::CVORTSPTrack2(HVOFILEREAD session,int index)
:m_startTime(0)
,m_headData(NULL)
,m_session(session)
{
	memset(&m_sampleInfo,0,sizeof(m_sampleInfo));
	memset(&m_nextKeyFrame,0,sizeof(m_nextKeyFrame));
	memset(&m_trackInfo,0,sizeof(VO_SOURCE_TRACKINFO));
	voRTSPTrackOpen(&m_track, m_session, index) ;
	
}
CVORTSPTrack2::~CVORTSPTrack2()
{
	SAFE_DELETE_ARRAY(m_headData);
	voRTSPTrackClose(m_session, m_track);
}

void CVORTSPTrack2::Init()
{
	VO_SOURCE_TRACKINFO * pTrackInfo=&m_trackInfo;
	VOTRACKINFO	oldTrackInfo;
	voRTSPTrackInfo(m_session, m_track, &oldTrackInfo);
	pTrackInfo->Type			= (VO_SOURCE_TRACKTYPE)(oldTrackInfo.uType+1);//damn!
	pTrackInfo->Duration		= oldTrackInfo.uDuration;
	pTrackInfo->Start			= 0;//TODO
	//Get head data
	VOCODECDATABUFFER	buffer;
	buffer.buffer = NULL;
	buffer.length = 0;

	int dwRC = voRTSPTrackGetParameter (m_session, m_track,VOID_COM_HEAD_DATA, (LONG *)&buffer);
	if (dwRC == VORC_FILEREAD_OK && buffer.length > 0)
	{
		if (m_headData != NULL)
		{
			SAFE_DELETE_ARRAY(m_headData);
		}
		pTrackInfo->HeadSize=buffer.length;
		m_headData = new VO_BYTE[buffer.length];TRACE_NEW("m_headData2",m_headData);
		buffer.buffer = m_headData;
		dwRC = voRTSPTrackGetParameter (m_session, m_track,VOID_COM_HEAD_DATA, (LONG *)&buffer);
		if(dwRC==VORC_FILEREAD_OK)
			pTrackInfo->HeadData = m_headData;
	}

	switch(oldTrackInfo.uCodec)
	{
	case VC_H264:
	case VC_AVC:
		pTrackInfo->Codec=VO_VIDEO_CodingH264;

		break;
	case VC_H263:
		pTrackInfo->Codec=VO_VIDEO_CodingH263;
		break;
	case VC_MPEG4:
		pTrackInfo->Codec=VO_VIDEO_CodingMPEG4;

		break;
	case VC_WMV:
		pTrackInfo->Codec=VO_VIDEO_CodingWMV;

		break;
	case VC_RV:
		pTrackInfo->Codec=VO_VIDEO_CodingRV;

		break;
	case AC_AAC:
		pTrackInfo->Codec=VO_AUDIO_CodingAAC;

		break;
	case AC_AMR:
		pTrackInfo->Codec=VO_AUDIO_CodingAMRNB;

		break;
	case AC_AWB:
		pTrackInfo->Codec=VO_AUDIO_CodingAMRWB;

		break;
	case AC_AWBP:
		pTrackInfo->Codec=VO_AUDIO_CodingAMRWBP;

		break;
	case AC_QCELP:
		pTrackInfo->Codec=VO_AUDIO_CodingQCELP13;

		break;
	case AC_WMA:
		pTrackInfo->Codec=VO_AUDIO_CodingWMA;

		break;
	case AC_RA:
		pTrackInfo->Codec=VO_AUDIO_CodingRA;

		break;
	case AC_ADPCM:
		pTrackInfo->Codec=VO_AUDIO_CodingADPCM;

		break;
	default:
		break;
	}
}

VO_U32 CVORTSPTrack2::SetTrackParam(VO_U32 uID, VO_PTR pParam)
{
	return voRTSPTrackSetParameter(m_session, m_track, uID, (LONG) pParam);
}

VO_U32 CVORTSPTrack2::GetTrackParam (VO_U32 uID, VO_PTR pParam)
{
	return voRTSPTrackGetParameter(m_session, m_track, uID, (LONG*) pParam);
}

VO_U32 CVORTSPTrack2::GetTrackInfo (VO_SOURCE_TRACKINFO * pTrackInfo)
{
		memcpy(pTrackInfo,&m_trackInfo,sizeof(m_trackInfo));
		return 0;
}

VO_U32 CVORTSPTrack2::GetSample (VO_SOURCE_SAMPLE * pSample)
{
	m_sampleInfo.pBuffer=pSample->Buffer;
	VO_U32 inputTime=pSample->Time;

	if(m_trackInfo.Type==VO_SOURCE_TT_RTSP_VIDEO)
	{
		//update the next key frame
		inputTime-=m_startTime;
		if((long)inputTime<0)
			inputTime=0;
#define MIN_FORWARD_TIME 300
#define MAX_FORWARD_TIME 500
#define MAX_FRAME_DIFF 20
		pSample->Flag = 0;
		if (inputTime>m_sampleInfo.uTime+MIN_FORWARD_TIME)
		{
			//SLOG2(LL_RTSP_ERR,"videoBuffer.txt","inTime=%d,curTime=%d\n",inputTime,m_sampleInfo.uTime);
			sprintf(CLog::formatString,"curTime=%lu,inputTime=%lu,input-cur=%lu\n",
				m_sampleInfo.uTime,inputTime,inputTime-m_sampleInfo.uTime);
			
			CLog::Log.MakeLog(LL_RTSP_ERR,"videoBuffer.txt",CLog::formatString);
			
			if(m_nextKeyFrame.uIndex<m_sampleInfo.uIndex)//m_nextKeyFrame.uTime<inputTime&&
			{
				long nextKey=voRTSPTrackGetNextKeyFrame(m_session, m_track, m_sampleInfo.uIndex, 1);
				if(nextKey>0&&nextKey>(long)m_sampleInfo.uIndex)
				{
					m_nextKeyFrame.uIndex = nextKey;
					voRTSPTrackGetSampleByIndex(m_session, m_track,  &m_nextKeyFrame);
				}
				
				sprintf(CLog::formatString,"start_%ld,input=%lu,curTime=%lu,nextKey=%ld,curIdx=%lu\n",
					m_startTime,inputTime,m_sampleInfo.uTime,nextKey,m_sampleInfo.uIndex);
				CLog::Log.MakeLog(LL_RTSP_ERR,"videoBuffer.txt",CLog::formatString);
			}
			
			{
				long diff=(long)m_nextKeyFrame.uIndex-(long)m_sampleInfo.uIndex;
				long framesGap = CUserOptions::UserOptions.outInfo.clip_AvgFrameRate;
				if(framesGap<MAX_FRAME_DIFF)
					framesGap = MAX_FRAME_DIFF;
				if (framesGap>40)//error
				{
					framesGap = MAX_FRAME_DIFF;
				}
				
				bool needDrop = false;
				
				if(diff>0&&m_nextKeyFrame.uTime < inputTime)
				{
					if(diff<framesGap)
					{
						needDrop = true;
						
					}
					else
					{
						bool forceDrop = inputTime>m_sampleInfo.uTime+MAX_FORWARD_TIME;
					
						if (forceDrop)
						{
							long bufTime=0;
							voRTSPTrackGetParameter(m_session,m_track,VOID_INNER_BUFFERING_TIME,&bufTime);
							
							long leftTimeInBuf = bufTime-(m_nextKeyFrame.uTime-m_sampleInfo.uTime);

							if(leftTimeInBuf>MAX_FORWARD_TIME)//left time in buf > 
								needDrop = true;
							sprintf(CLog::formatString,"framesGap_%ld,bufTime=%ld,(%lu-%lu)\n",
								framesGap,bufTime,m_nextKeyFrame.uTime,m_sampleInfo.uTime);
							CLog::Log.MakeLog(LL_RTSP_ERR,"videoBuffer.txt",CLog::formatString);
						}
					}
				}
			
				if(needDrop)
				{
					sprintf(CLog::formatString,"Drop frame! uIndex = %lu, nextKeyFram.uIndex= %lu,time=%lu\n",
						m_sampleInfo.uIndex,m_nextKeyFrame.uIndex,m_nextKeyFrame.uTime);
			
					CLog::Log.MakeLog(LL_RTSP_ERR,"videoBuffer.txt",CLog::formatString);
				
					m_sampleInfo.uIndex = m_nextKeyFrame.uIndex;
					pSample->Flag = VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED;//1;//indicate drop frame
				}
			}
		}

	}
	unsigned int ret=voRTSPTrackGetSampleByIndex(m_session, m_track,  &m_sampleInfo);
	if(ret==0)
	{
		m_sampleInfo.uIndex++;
		pSample->Size			= m_sampleInfo.uSize;
		pSample->Time			= m_sampleInfo.uTime;
		pSample->Duration		= m_sampleInfo.uDuration;
	}
	if (ret==VORC_FILEREAD_NEED_RETRY)
	{
		if(CUserOptions::UserOptions.m_bMakeLog)
		{
			sprintf(CLog::formatString,"VORC_FILEREAD_NEED_RETRY:start_%ld,input=%lu,curTime=%lu,curIdx=%lu\n",
				m_startTime,inputTime,m_sampleInfo.uTime,m_sampleInfo.uIndex);
			CLog::Log.MakeLog(LL_RTSP_ERR,"buffer.txt",CLog::formatString);
		}
		
		ret=VO_ERR_SOURCE_NEEDRETRY;
	}
	else if (ret==VORC_FILEREAD_INDEX_END)
	{
		ret=VO_ERR_SOURCE_END;
	}
	
	return ret;
}

VO_U32 CVORTSPTrack2::SetPos (VO_S32 * pPos)
{
	m_sampleInfo.uIndex=0;
	m_sampleInfo.uTime=0;
	m_nextKeyFrame.uTime=0;
	m_nextKeyFrame.uIndex=0;
	m_startTime = *pPos;
	return 0;
}

CVORTSPSource2::CVORTSPSource2(VO_SOURCE_OPENPARAM * pParam):
m_sourceLock(0),
m_newPos(0),
m_session(0),
m_startTime(0),
m_isLoading(false)
{
	memset(&m_tracks,0,sizeof(m_tracks));
	memset(&m_sourceInfo,0,sizeof(VO_SOURCE_INFO));
	//m_sourceInfo.Tracks=0;
	if(voRTSPSessionCreate(&m_session))
		m_session=NULL;//error
	else
	{
		//set param first
		memcpy(&m_param,pParam,sizeof(VO_SOURCE_OPENPARAM ));
		long version=1;
		voRTSPSessionSetParameter(m_session,VOID_FILEREAD_CALLBACK_NOTIFY,(LONG)m_param.pSourceOP);
		voRTSPSessionSetParameter(m_session, VOID_STREAMING_SDK_VERSION,(LONG)&version);
		voRTSPSessionSetParameter(m_session, VOID_STREAMING_INIT_PARAM, (LONG)m_param.nReserve);
	}
	m_sourceLock = VOCPFactory::CreateOneMutex();TRACE_NEW("m_criticalSec3",m_sourceLock);
}

CVORTSPSource2::~CVORTSPSource2()
{
	if(m_session)
	{
		voRTSPSessionSetParameter(m_session,VOID_STREAMING_FORCE_STOP,1);
		//wait for the close is done
		while (m_isLoading)
		{
			IVOThread::Sleep(10);
			CLog::Log.MakeLog(LL_RTSP_ERR,"flow.txt","close: wait for loading url done");
		} 
		voRTSPSessionClose(m_session);
	}
	for (unsigned int i=0;i<m_sourceInfo.Tracks;i++)
	{
		SAFE_DELETE(m_tracks[i]);
	}
	if(m_sourceLock)
	{
		VOCPFactory::DeleteOneMutex(m_sourceLock);
		TRACE_DELETE(m_sourceLock);
	}
	
}

VO_U32 CVORTSPSource2::OpenURL()
{
	int ret=1;
	m_isLoading=true;
	if(m_param.pSource)
	{
			ret=voRTSPSessionOpenURL(m_session, (const char *) m_param.pSource,NULL);
			if(ret==0)
			{
				voRTSPSessionGetParameter (m_session,VOID_FILEREAD_TRACKS, (LONG *)&(m_sourceInfo.Tracks));
				voRTSPSessionGetParameter(m_session,VOID_COM_DURATION,(LONG *)&(m_sourceInfo.Duration));
				if(m_sourceInfo.Tracks<0||m_sourceInfo.Tracks>=4)
				{
					CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt","m_sourceInfo.Tracks<0||m_sourceInfo.Tracks>=MAX_TRACK_COUNT\n");
					//break;
				}
				for (unsigned int i=0;i<m_sourceInfo.Tracks;i++)
				{
					m_tracks[i] = new CVORTSPTrack2(m_session,i);TRACE_NEW("CVORTSPTrack2",m_tracks[i]);
					m_tracks[i]->Init();
				}
			}
	}
	m_isLoading=false;
	return ret;
}

VO_U32	CVORTSPSource2::GetSourceInfo(VO_SOURCE_INFO * pSourceInfo)
{
	memcpy(pSourceInfo,&m_sourceInfo,sizeof(VO_SOURCE_INFO));
	return 0;
}

VO_U32 CVORTSPSource2::GetTrackInfo (VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
{
	int ret=-1;
	if(m_tracks[nTrack])
	{
		ret = m_tracks[nTrack]->GetTrackInfo(pTrackInfo);
	}
	return ret;
}

VO_U32 CVORTSPSource2::GetSample (VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample)
{
	CAutoCriticalSec cAutoLock(m_sourceLock);

	int ret=-1;
	if(m_tracks[nTrack])
	{
		ret = m_tracks[nTrack]->GetSample(pSample);
	}
	return ret;
}

VO_U32 CVORTSPSource2::SetPos (VO_U32 nTrack, VO_S32 * pPos)
{
	CAutoCriticalSec cAutoLock(m_sourceLock);
	if(1)//nTrack==0)//only seek once
	{
		VO_S32 newPos=*pPos;
		if(0==voRTSPSessionSetParameter(m_session,  VOID_FILEREAD_ACTION_SEEK, (LONG)&newPos))
		{
			for (unsigned int i=0;i<m_sourceInfo.Tracks;i++)
			{
				m_tracks[i]->SetPos(pPos);
			}
			m_newPos=*pPos;
			return 0;
		}
	}
	return VO_ERR_SOURCE_SEEKFAIL;
}

VO_U32 CVORTSPSource2::SetSourceParam ( VO_U32 uID, VO_PTR pParam)
{
	if (uID==VOID_STREAMING_OPENURL)
	{
		int ret = OpenURL();
		/*if(CUserOptions::UserOptions.initPlayPos>3000)*/
		{
			SLOG1(LL_SOCKET_ERR,"rtsp.txt","the init pos=%ld\n",CUserOptions::UserOptions.initPlayPos);
			for (unsigned int i=0;i<m_sourceInfo.Tracks;i++)
			{
				VO_S32 pPos = CUserOptions::UserOptions.initPlayPos;
				m_tracks[i]->SetPos(&pPos);
			}
		}
		//CUserOptions::UserOptions.initPlayPos = 0;
		return ret;
	}
	return voRTSPSessionSetParameter(m_session, uID, (LONG) pParam );
}

VO_U32 CVORTSPSource2::GetSourceParam (VO_U32 uID, VO_PTR pParam)
{
	return voRTSPSessionGetParameter(m_session, uID, (LONG* )pParam);
}

VO_U32 CVORTSPSource2::SetTrackParam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	int ret=-1;
	if(m_tracks[nTrack])
	{
		ret = m_tracks[nTrack]->SetTrackParam(uID,pParam);
	}
	return ret;
}

VO_U32 CVORTSPSource2::GetTrackParam (VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
{
	int ret=-1;
	if(m_tracks[nTrack])
	{
		ret = m_tracks[nTrack]->GetTrackParam(uID,pParam);
	}
	return ret;
}

int OnEventSource2(long EventCode, long * EventParam1, long * EventParam2)
{
	voStream2ParserWrapper *source = (voStream2ParserWrapper *)EventParam2;

	if(!source)
		return 0;

	switch(EventCode)
	{		
		case VO_EVENT_CONNECT_FAIL:
			source->SendEvent(VO_EVENTID_SOURCE2_ERR_RTSP_CONNECT_FAIL,(long)(*EventParam1));
			break;
		case VO_EVENT_DESCRIBE_FAIL:
			source->SendEvent(VO_EVENTID_SOURCE2_ERR_RTSP_DESCRIBE_FAIL,(long)(*EventParam1));
			break;
		case VO_EVENT_SETUP_FAIL:
			source->SendEvent(VO_EVENTID_SOURCE2_ERR_RTSP_SETUP_FAIL,(long)(*EventParam1));
			break;
		case VO_EVENT_PLAY_FAIL:
			source->SendEvent(VO_EVENTID_SOURCE2_ERR_RTSP_PLAY_FAIL,(long)(*EventParam1));
			break;
		case VO_EVENT_PAUSE_FAIL:
			source->SendEvent(VO_EVENTID_SOURCE2_ERR_RTSP_PAUSE_FAIL,(long)(*EventParam1));
			break;
		case VO_EVENT_OPTION_FAIL:
			source->SendEvent(VO_EVENTID_SOURCE2_ERR_RTSP_OPTION_FAIL,(long)(*EventParam1));
			break;
		case VO_EVENT_SOCKET_ERR:
			VOLOGI("************************************VO_EVENTID_SOURCE2_ERR_RTP_SOCKET_ERROR************************************************");
			source->SendEvent(VO_EVENTID_SOURCE2_ERR_RTP_SOCKET_ERROR,(long)(*EventParam1));
			break;
		default:
			break;
	}
		
	return 0;
}

voStream2ParserWrapper::voStream2ParserWrapper()
:m_pCallback(NULL)
,m_session(NULL)
,m_streamInfo(NULL)
,m_trackCnt(0)
,m_isSeeking(false)
,m_nFlag(0)
{
	m_sourceLock = VOCPFactory::CreateOneMutex();
	
	memset(m_url, 0, 2048);
	memset(m_workingPath,0,1024);
	memset(&m_eventCallback,0,sizeof(m_eventCallback));
	memset(&m_tracks,0,sizeof(m_tracks));
	memset(&m_programmInfo,0,sizeof(m_programmInfo));
	memset(&m_audioSample,0,sizeof(m_audioSample));
	memset(&m_videoSample,0,sizeof(m_videoSample));
	memset(&m_info,0,sizeof(m_info));
	memset(&m_outInfo,0, sizeof(m_outInfo));
	
	for(unsigned int i=0; i<4;i++)
	{
		m_isFirstFrame[i] = true;
		m_trackInfoSize[i] = 0;
	}
	
	m_audioSample.Buffer = (VO_PBYTE)new char[AUDIO_FRAME_BUFFER_SIZE];
	memset(m_audioSample.Buffer,0,AUDIO_FRAME_BUFFER_SIZE);

	m_videoSample.Buffer = (VO_PBYTE)new char[VIDEO_FRAME_BUFFER_SIZE];
	memset(m_videoSample.Buffer,0,VIDEO_FRAME_BUFFER_SIZE);

	memset(&m_pInitParam,0,sizeof(m_pInitParam));
	m_nInitFlag = 0;

	m_isEOSReached = false;
	m_openFlag = true;
	m_hLicCheck = NULL;
	m_isAddLicCheck = VO_FALSE;
	m_uDuration = 0;
}

voStream2ParserWrapper::~voStream2ParserWrapper()
{
	Reset();
	
	SAFE_DELETE_ARRAY(m_videoSample.Buffer);
	memset(&m_videoSample,0,sizeof(m_videoSample));

	SAFE_DELETE_ARRAY(m_audioSample.Buffer);
	memset(&m_audioSample,0,sizeof(m_audioSample));
	
	if(m_sourceLock)
	{
		VOCPFactory::DeleteOneMutex(m_sourceLock);
	}
}
void voStream2ParserWrapper::Reset()
{
	CAutoCriticalSec cAutoLock(m_sourceLock);

	for (unsigned int i=0;i<m_trackCnt;i++)
	{
		SAFE_DELETE(m_tracks[i]);
	}

	if(m_session)
		voRTSPSessionClose(m_session);	

	if(m_streamInfo)
	{
		for(unsigned int i=0;i<m_trackCnt;i++)
		{
			SAFE_DELETE_ARRAY(m_streamInfo->ppTrackInfo[i]->pHeadData);
			SAFE_DELETE_ARRAY(m_streamInfo->ppTrackInfo[i]);
		}
		SAFE_DELETE_ARRAY(m_streamInfo->ppTrackInfo);
	}
	
	SAFE_DELETE(m_streamInfo);
	
	m_trackCnt = 0;
	memset(&m_programmInfo,0,sizeof(m_programmInfo));
	memset(&m_outInfo,0, sizeof(m_outInfo));

	for(unsigned int i=0; i<4;i++)
	{
		m_isFirstFrame[i] = true;
		m_trackInfoSize[i] = 0;
	}

	memset(&m_info,0,sizeof(m_info));
	m_session = NULL;
}

VO_U32 voStream2ParserWrapper::Init( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam )
{
	CAutoCriticalSec cAutoLock(m_sourceLock);
	m_nFlag = nFlag;
	if (!(VO_SOURCE2_FLAG_OPEN_URL & nFlag))
		return VO_RET_SOURCE2_FAIL;
	strcpy(m_url,(char *)pSource);
	m_nInitFlag = nFlag;	
	memcpy(&m_pInitParam,pInitParam,sizeof(VO_SOURCE2_INITPARAM));
	
	VOLOGINIT((VO_TCHAR *)m_workingPath);

	if(pInitParam->uFlag&VO_SOURCE2_FLAG_INIT_EVENTCALLBACK)
		m_pCallback = (VO_SOURCE2_EVENTCALLBACK *)pInitParam;

	if(voRTSPSessionCreate(&m_session) != VORC_FILEREAD_OK)
		return VO_RET_SOURCE2_FAIL;

	long version=1;
	voRTSPSessionSetParameter(m_session, VOID_STREAMING_SDK_VERSION,(LONG)&version);
	m_eventCallback.funtcion = OnEventSource2;
	m_eventCallback.parent = (void *)this;
	voRTSPSessionSetParameter(m_session,VOID_FILEREAD_CALLBACK_NOTIFY,(LONG)&m_eventCallback);
	
	return VO_RET_SOURCE2_OK;
}

VO_VOID voStream2ParserWrapper::UnInit()
{
	if(m_hLicCheck != NULL)
	{
		voCheckLibUninit(m_hLicCheck);
		m_hLicCheck = NULL;
		m_isAddLicCheck = VO_FALSE;
	}
}

VO_U32 voStream2ParserWrapper::Open()
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	CAutoCriticalSec cAutoLock(m_sourceLock);
	VO_U32   ulRet = VO_RET_SOURCE2_OK;
	ulRet = voCheckLibInit (&m_hLicCheck, VO_INDEX_SRC_RTSP, VO_LCS_WORKPATH_FLAG,0, (VO_TCHAR *)m_workingPath); 
	if(ulRet != VO_ERR_NONE )
	{
		if(m_hLicCheck != NULL)
		{
			voCheckLibUninit(m_hLicCheck);
		}
		m_hLicCheck = NULL;
		m_isAddLicCheck = VO_FALSE;
		return ulRet;
	}
	else
	{
		m_isAddLicCheck = VO_TRUE;
	}

	if(m_session)
	{
		ret = voRTSPSessionOpenURL(m_session, (const char *)m_url,NULL);
		if(ret == VORC_FILEREAD_OK)
		{
			ret = VO_RET_SOURCE2_OK;
			voRTSPSessionGetParameter (m_session,VOID_STREAMING_INFO, (LONG *)&(m_info));
			voRTSPSessionGetParameter (m_session,VOID_FILEREAD_TRACKS, (LONG *)&(m_trackCnt));

			for(unsigned int i=0;i<m_trackCnt;i++)
			{
				m_tracks[i] = new CVORTSPTrack2(m_session,i);
				m_tracks[i]->Init();
			}

			m_programmInfo.sProgramType = (m_info.clipType==ST_ON_DEMAND)?VO_SOURCE2_STREAM_TYPE_VOD:VO_SOURCE2_STREAM_TYPE_LIVE;
			m_programmInfo.uStreamCount	=	1;
			strcpy(m_programmInfo.strProgramName,"RTSP Stream");

			m_streamInfo = new VO_SOURCE2_STREAM_INFO;
			memset(m_streamInfo, 0, sizeof(VO_SOURCE2_STREAM_INFO));
			m_programmInfo.ppStreamInfo = (VO_SOURCE2_STREAM_INFO **)&m_streamInfo;

			m_streamInfo->uTrackCount = m_trackCnt;
			m_streamInfo->ppTrackInfo = (VO_SOURCE2_TRACK_INFO	**)new VO_SOURCE2_TRACK_INFO *[m_streamInfo->uTrackCount] ;
			voRTSPSessionGetParameter (m_session,HSID_STREAMING_INFO, (LONG *)&(m_outInfo));

			for(unsigned int i=0;i<m_trackCnt;i++)
			{
				m_streamInfo->ppTrackInfo[i] = (VO_SOURCE2_TRACK_INFO *)new char[sizeof(VO_SOURCE2_TRACK_INFO)];
				memset(m_streamInfo->ppTrackInfo[i],0, sizeof(VO_SOURCE2_TRACK_INFO));

				m_streamInfo->ppTrackInfo[i]->uTrackID = i;

				if(m_tracks[i]->m_trackInfo.Type == VO_SOURCE_TT_RTSP_VIDEO)
				{
					m_streamInfo->ppTrackInfo[i]->uTrackType = VO_SOURCE2_TT_RTSP_VIDEO; 
					m_streamInfo->ppTrackInfo[i]->sVideoInfo.sFormat.Width = m_outInfo.clip_width;
					m_streamInfo->ppTrackInfo[i]->sVideoInfo.sFormat.Height = m_outInfo.clip_height;
				}
				else if(m_tracks[i]->m_trackInfo.Type == VO_SOURCE_TT_RTSP_AUDIO)
				{
					m_streamInfo->ppTrackInfo[i]->uTrackType = VO_SOURCE2_TT_RTSP_AUDIO;
					m_streamInfo->ppTrackInfo[i]->sAudioInfo.sFormat.Channels = m_outInfo.clip_audioChannels;
					m_streamInfo->ppTrackInfo[i]->sAudioInfo.sFormat.SampleRate = m_outInfo.clip_audioSampleRate;
				}
				else
					m_streamInfo->ppTrackInfo[i]->uTrackType = VO_SOURCE2_TT_MAX;

				m_streamInfo->ppTrackInfo[i]->uTrackType = (VO_SOURCE2_TRACK_TYPE)((VO_U32)m_tracks[i]->m_trackInfo.Type);
				m_streamInfo->ppTrackInfo[i]->uCodec = m_tracks[i]->m_trackInfo.Codec;
				m_streamInfo->ppTrackInfo[i]->uDuration = m_tracks[i]->m_trackInfo.Duration;
				m_streamInfo->ppTrackInfo[i]->uHeadSize = m_tracks[i]->m_trackInfo.HeadSize;

				m_streamInfo->ppTrackInfo[i]->pHeadData = (VO_PBYTE)new char[m_streamInfo->ppTrackInfo[i]->uHeadSize];
				memcpy(m_streamInfo->ppTrackInfo[i]->pHeadData,m_tracks[i]->m_trackInfo.HeadData,m_tracks[i]->m_trackInfo.HeadSize);

			}

			if(m_pCallback && m_pCallback->SendEvent)
				m_pCallback->SendEvent(m_pCallback->pUserData, VO_EVENTID_SOURCE2_PROGRAMRESET, 0, 0);

			if(m_openFlag&&(m_nFlag&VO_SOURCE2_FLAG_OPEN_ASYNC&&m_pCallback&&m_pCallback->SendEvent))
			{
				m_openFlag = false;
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "****************VO_SOURCE2_FLAG_OPEN_ASYNC****************\n");
				m_pCallback->SendEvent(m_pCallback->pUserData, VO_EVENTID_SOURCE2_OPENCOMPLETE, (VO_U32)&ret, 0);
			}
		}
		else
			ret = VO_RET_SOURCE2_FAIL;
	}
		
	return ret;
}

VO_U32 voStream2ParserWrapper::Close()
{
	VOLOGUNINIT();
	UnInit();
	Reset();
	return VO_RET_SOURCE2_OK;
}

VO_U32 voStream2ParserWrapper::Start()
{
//	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	CAutoCriticalSec cAutoLock(m_sourceLock);

	if(m_session)
		voRTSPSessionSetParameter(m_session,VOID_FILEREAD_ACTION_RUN,1);
	
	return VO_RET_SOURCE2_OK;
}

VO_U32 voStream2ParserWrapper::Pause()
{
//	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	CAutoCriticalSec cAutoLock(m_sourceLock);

	if(m_session)
		voRTSPSessionSetParameter(m_session,VOID_FILEREAD_ACTION_PAUSE,1);
	
	return VO_RET_SOURCE2_OK;
}

VO_U32 voStream2ParserWrapper::Stop()
{
//	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	CAutoCriticalSec cAutoLock(m_sourceLock);

	if(m_session)
		voRTSPSessionSetParameter(m_session,VOID_FILEREAD_ACTION_STOP,1);
	
	return VO_RET_SOURCE2_OK;
}

VO_U32 voStream2ParserWrapper::Seek(VO_U64* pTimeStamp)
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	CAutoCriticalSec cAutoLock(m_sourceLock);

	VO_S32 newPos=*pTimeStamp;

	if(m_isEOSReached)
	{
		m_isEOSReached = false;
		return ReOpen(pTimeStamp);
	}

	if(m_session)
	{
		if(0==voRTSPSessionSetParameter(m_session,  VOID_FILEREAD_ACTION_SEEK, (LONG)&newPos))
		{
			ret = VO_RET_SOURCE2_OK;
			m_isSeeking = true;
			for (unsigned int i=0;i<m_trackCnt;i++)
			{
				m_tracks[i]->SetPos(&newPos);
			}

			if(m_pCallback&&m_pCallback->SendEvent)
			{
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", "****************VO_SOURCE2_FLAG_SEEK_ASYNC****************\n");
				m_pCallback->SendEvent(m_pCallback->pUserData, VO_EVENTID_SOURCE2_SEEKCOMPLETE, (VO_U32)pTimeStamp, (VO_U32)&ret);
			}
		}
	}

	for(unsigned int i=0; i<4;i++)
	{
		m_isFirstFrame[i] = true;
		m_trackInfoSize[i] = 0;
	}

	return ret;
}

VO_U32 voStream2ParserWrapper::GetDuration(VO_U64 * pDuration)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	if(m_session)
		voRTSPSessionGetParameter(m_session,VOID_COM_DURATION,(LONG *)&m_uDuration);

	*pDuration = m_uDuration;
	//VOLOGI("(RTSP module)Duration is= %lu, m_uDuration = %lu", *pDuration, m_uDuration);

	return ret;
}	
	
VO_U32 voStream2ParserWrapper::GetSample(VO_SOURCE2_TRACK_TYPE nOutPutType , VO_PTR pSample )
{
	CAutoCriticalSec cAutoLock(m_sourceLock);
	CVORTSPTrack2* track = NULL;
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	VO_SOURCE2_SAMPLE * sample = (VO_SOURCE2_SAMPLE *)pSample;
	VO_U32 trackIndex = 0;

	for(unsigned int i=0;i<m_trackCnt;i++)
	{
		if((VO_U32)m_tracks[i]->m_trackInfo.Type == (VO_U32)nOutPutType)
		{
			track = m_tracks[i];
			trackIndex = i;
		}
	}

	if(!track)
	{
		//return ret;
		sample->uFlag = VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE;
		return VO_RET_SOURCE2_OK;
	}
	
	VO_SOURCE_SAMPLE * Sample = NULL;

	if(track->m_trackInfo.Type == VO_SOURCE_TT_RTSP_VIDEO)
		Sample = &m_videoSample;
	else
		Sample = &m_audioSample;

	if(m_isFirstFrame[trackIndex])
	{
		sample->pFlagData = m_streamInfo->ppTrackInfo[trackIndex];
		sample->uFlag |= VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT;
		m_isFirstFrame[trackIndex] = false;

		return VO_RET_SOURCE2_OK;
	}

	ret = track->GetSample(Sample);

	if(ret==0)
	{
		if(Sample->Size & 0x80000000)
		{
			Sample->Size &= 0x7fffffff;
			sample->uFlag |= VO_SOURCE2_FLAG_SAMPLE_KEYFRAME;
			
		}
		sample->uTime = Sample->Time;
		sample->uDuration = Sample->Duration;
		sample->uSize = Sample->Size;
		sample->pBuffer = Sample->Buffer;

		ret = VO_RET_SOURCE2_OK;
	}
	else if (ret==VO_ERR_SOURCE_NEEDRETRY)
	{
		ret=VO_RET_SOURCE2_NEEDRETRY;
	}
	else if (ret==VO_ERR_SOURCE_END)
	{
		ret=VO_RET_SOURCE2_END;
		sample->uFlag |= VO_SOURCE2_FLAG_SAMPLE_EOS;

		m_isEOSReached = true;
	}

	return ret;
}

VO_U32 voStream2ParserWrapper::GetProgrammCount(VO_U32 *pStreamCount)
{
	*pStreamCount = 1;
	return VO_RET_SOURCE2_OK;
}
VO_U32 voStream2ParserWrapper::GetProgrammInfo(VO_U32 nProgram, VO_SOURCE2_PROGRAM_INFO **pProgramInfo)
{
	*pProgramInfo = &m_programmInfo;
	PrintProgramInfo(&m_programmInfo);
	return VO_RET_SOURCE2_OK;
}

VO_U32 voStream2ParserWrapper::GetCurTrackInfo(VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo )
{
	CAutoCriticalSec cAutoLock(m_sourceLock);
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	
	for(unsigned int i=0;i<m_trackCnt;i++)
	{
		if(m_streamInfo->ppTrackInfo[i]->uTrackType == (VO_U32)nTrackType)
		{
			*ppTrackInfo = m_streamInfo->ppTrackInfo[i];
			ret = VO_RET_SOURCE2_OK;
			break;
		}
	}

	return ret;
}

VO_U32 voStream2ParserWrapper::SelectProgram(VO_U32 nProgram)
{
	return	VO_RET_SOURCE2_OK;
}

VO_U32 voStream2ParserWrapper::SelectStream(VO_U32 nStream)
{
	return	VO_RET_SOURCE2_OK;
}

VO_U32 voStream2ParserWrapper::SelectTrack(VO_U32 nTrack)
{
	return	VO_RET_SOURCE2_OK;
}

VO_U32 voStream2ParserWrapper::GetParam(VO_U32 nParamID, VO_PTR pParam)
{
//	VO_U32 ret = VO_RET_SOURCE2_FAIL;

	if(m_session)
		voRTSPSessionGetParameter(m_session,nParamID,(LONG *)pParam);

	return VO_RET_SOURCE2_OK;
}

VO_U32 voStream2ParserWrapper::SetParam(VO_U32 nParamID, VO_PTR pParam)
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	switch(nParamID)
	{
		case VO_PID_SOURCE2_EVENTCALLBACK:
			m_pCallback = (VO_SOURCE2_EVENTCALLBACK *)pParam;
			ret = VO_RET_SOURCE2_OK;
			break;

		case VO_PID_COMMON_LOGFUNC:
			{
				//VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
				//vologInit(pVologCB->pUserData, pVologCB->fCallBack);
				ret=VO_RET_SOURCE2_OK;
			}
			break;

		case VO_PID_SOURCE2_LOWLATENCYMODE:
			VOLOGI("***************************uplayer has set latency param********************");
			CUserOptions::UserOptions.m_minimumLatency = true;
			ret = VO_RET_SOURCE2_OK;
			break;

		case VO_PID_SOURCE2_WORKPATH:
			strcpy(m_workingPath, (VO_CHAR *)pParam);
			strcpy(CUserOptions::UserOptions.m_apkworkpath,(char *)pParam);
			ret = VO_RET_SOURCE2_OK;
			break;

		default:
			if(m_session)
			{
				ret = voRTSPSessionSetParameter(m_session,nParamID,(LONG)pParam);
				if(ret == VORC_FILEREAD_OK)
					ret = VO_RET_SOURCE2_OK;
			}
	}
	
	return ret;
}

VO_U32 voStream2ParserWrapper::SendEvent(long event,long param)
{
	if(!m_pCallback)
		return 0;

	if((!m_pCallback->SendEvent) || (!m_pCallback->pUserData))
		return 0;

	sprintf(CLog::formatString,"code=%08X,param=%ld\n",(unsigned int)event,param);
	CLog::Log.MakeLog(LL_BUF_STATUS,"eventSource2.txt",CLog::formatString);

	m_pCallback->SendEvent(m_pCallback->pUserData,(VO_U32)event,(VO_U32)param,0);
	
	return VO_RET_SOURCE2_OK;
}

VO_U32 voStream2ParserWrapper::ReOpen(VO_U64* pTimeStamp)
{	
	CAutoCriticalSec cAutoLock(m_sourceLock);
	Reset();
	Init(m_url,m_nInitFlag,&m_pInitParam);
	SetParam(VOID_STREAMING_INITPLAY_POS,pTimeStamp);
	Open();
	Start();
	
	return VO_RET_SOURCE2_OK;
}

VO_BOOL voStream2ParserWrapper::GetLicState()
{
	if(m_hLicCheck != NULL)
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}

VO_BOOL voStream2ParserWrapper::IsAddLicCheck()
{
	return m_isAddLicCheck;
}

VO_VOID voStream2ParserWrapper::PrintTrackInfo(VO_SOURCE2_TRACK_INFO *pTrackInfo)
{
	VOLOGI("****************************PrintTrackInfo*********************************************");

	if(!pTrackInfo){
		return;
	}

	VOLOGI("uOutSideTrackID=%lu, Sel=%lu, TrackType=%lu, Codec=%lu, pTrackInfo->sAudioInfo.sFormat.Channels = %lu, pTrackInfo->sAudioInfo.sFormat.SampleRate = %lu",
		pTrackInfo->uTrackID, pTrackInfo->uSelInfo, pTrackInfo->uTrackType, pTrackInfo->uCodec, pTrackInfo->sAudioInfo.sFormat.Channels, pTrackInfo->sAudioInfo.sFormat.SampleRate);

	VOLOGI("Duration=%llu, ChunkCounts=%lu, Bitrate=%lu, HeadSize=%lu",
		pTrackInfo->uDuration, pTrackInfo->uChunkCounts, pTrackInfo->uBitrate, pTrackInfo->uHeadSize);

	VOLOGI("______PrintTrackInfo_____pTrackInfo->sVideoInfo.sFormat.Width = %d, pTrackInfo->sVideoInfo.sFormat.Height=%d", pTrackInfo->sVideoInfo.sFormat.Width,pTrackInfo->sVideoInfo.sFormat.Height);


	VOLOGI("*****************************************************************************************");

}

VO_VOID voStream2ParserWrapper::PrintStreamInfo(VO_SOURCE2_STREAM_INFO *pStreamInfo)
{
	VOLOGI("****************************PrintStreamInfo*********************************************");

	if(!pStreamInfo){
		return;
	}
	VOLOGI("StreamID=%lu, Sel=%lu, uBitrate=%lu, uTrackCount=%lu",
		pStreamInfo->uStreamID, pStreamInfo->uSelInfo, pStreamInfo->uBitrate, pStreamInfo->uTrackCount);

	for(VO_U32 index = 0; index < pStreamInfo->uTrackCount; index++)
	{
		PrintTrackInfo(pStreamInfo->ppTrackInfo[index]);
	}

	VOLOGI("*******************************************************************************************");

}

VO_VOID voStream2ParserWrapper::PrintProgramInfo(VO_SOURCE2_PROGRAM_INFO *pProgramInfo)
{
	VOLOGI("****************************PrintProgramInfo*********************************************");

	if(!pProgramInfo){
		return;
	}

	VOLOGI("ProgramID=%lu, Sel=%lu, Type=%lu, Name=%s,uStreamCount=%lu",
		pProgramInfo->uProgramID, pProgramInfo->uSelInfo, pProgramInfo->sProgramType, pProgramInfo->strProgramName, pProgramInfo->uStreamCount);

	for(VO_U32 index = 0; index < pProgramInfo->uStreamCount; index++)
	{
		PrintStreamInfo(pProgramInfo->ppStreamInfo[index]);
	}


	VOLOGI("*********************************************************************************************");
}