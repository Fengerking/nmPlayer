#include <string.h>

#include "utility.h"
#include "network.h"
#include "TaskSchedulerEngine.h"
#include "MediaStream.h"
#include "RDTMediaStream.h"
#include "RTSPSession.h"
#include "RTSPMediaStreamSyncEngine.h"
#include "MediaStreamSocket.h"
#include "RDTParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CRTSPMediaStreamSyncEngine * CRTSPMediaStreamSyncEngine::m_pRTSPMediaStreamSyncEngine = NULL;

CRTSPMediaStreamSyncEngine * CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine()
{
	if(m_pRTSPMediaStreamSyncEngine == NULL)
	{
		m_pRTSPMediaStreamSyncEngine = new CRTSPMediaStreamSyncEngine();TRACE_NEW("m_pRTSPMediaStreamSyncEngine",m_pRTSPMediaStreamSyncEngine);
	}

	return m_pRTSPMediaStreamSyncEngine;
}
void   CRTSPMediaStreamSyncEngine:: RecvMediaFrame(int streamNum,FrameData* frame)
{

	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		if(m_mediaStreams[i]->GetStreamNum() == streamNum)
		{
			FrameData* frame2=m_mediaStreams[i]->GetFrameData();
			if(m_commonASFTimeStamp==0&&(m_mediaStreamCount==1||m_mediaStreams[i]->IsAudioStream()))//&&!frame->isFragment&&frame2->len==0)//first frame
			{
				m_commonASFTimeStamp=frame->timestamp;
			}
			if(!frame->isFragment&&frame2->len>0)//if there is a frame in buffer,output it first
			{
				m_mediaStreams[i]->RecvMediaFrame(frame2->data,frame2->len,frame2->timestamp,frame2->seqNum,frame2->isKeyFrame);
			}

			return m_mediaStreams[i]->SetFrameData(frame);
			
		}
	}
}

void CRTSPMediaStreamSyncEngine::DestroyRTSPMediaStreamSyncEngine()
{
	SAFE_DELETE(m_pRTSPMediaStreamSyncEngine);
}
void CRTSPMediaStreamSyncEngine::EnableWaitIFrame(bool isEnable)
{
	if(m_mediaStreamCount<2)
		isEnable = false;

	m_isWatingIFrame = isEnable;
	if(m_isWatingIFrame)
	{
		m_theAudioBeforeIFrameIsCleared=false;
		m_firstIFrameTime=-1;//m_firstIFrameTime also act as a flag if the waiting mechanisam is working -1:enable
		m_avsyncAfterWait=0;

	}
	else
	{
		m_theAudioBeforeIFrameIsCleared=true;
		m_firstIFrameTime=-2;//m_firstIFrameTime also act as a flag if the waiting mechanisam is working,-2:disable
		m_avsyncAfterWait=0;

	}
	m_tryWaitCount = 0;
	m_waitIFrameTime = 0;

}
void CRTSPMediaStreamSyncEngine::EnableRejectAfterScan(bool isRejectAll)
{
	m_isRejectAll = isRejectAll;
	m_isWatingIFrame = true;
	m_theAudioBeforeIFrameIsCleared=false;
	m_firstIFrameTime=-1;//m_firstIFrameTime also act as a flag if the waiting mechanisam is working -1:enable
	m_avsyncAfterWait=0;
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{

		for(int i=0; i<m_mediaStreamCount; ++i)
		{
			sprintf(CLog::formatString,"!!!!After flush,the left frame=%d\n",m_mediaStreams[i]->HaveBufferedMediaFrameCount());
			CLog::Log.MakeLog(LL_BUF_STATUS,"buffering.txt",CLog::formatString);
		}
		
	}
}
#define INVALID_SYNC_AUDIO_TIME 0x7fffffff
CRTSPMediaStreamSyncEngine::CRTSPMediaStreamSyncEngine()
{
	m_baseSyncWallClockTime.tv_sec = m_baseSyncWallClockTime.tv_usec = 0;
	memset(m_mediaStreams, 0, sizeof(m_mediaStreams));
	m_mediaStreamCount = 0;
	m_maxWaitFrames = 50;
	m_isRejectAll = false;
	EnableWaitIFrame(false);
	m_dataSyncbyRTCP = false;
	m_rdtClientPort = 0;
	m_rdtStreamSock = NULL;
	m_rdtParser		= NULL;
	m_dataSyncbyRTPInfo = false;
	m_playResponseDone	= true;
	ResetASFCOMSEQ();
	m_firstIframeInfo.valid=false;
	m_firstIframeInfo.syncAudioTime=INVALID_SYNC_AUDIO_TIME;
	m_commonASFTimeStamp=0;
	m_randomPort= 0;
	m_playBegin = 0;
}
int	CRTSPMediaStreamSyncEngine::CreateRDTParser(CRDTMediaStream* mediaStream)
{
#if ENABLE_REAL
	if(m_rdtClientPort==0)
	{
		CMediaStreamSocket * rdtStreamSock = NULL;
		int minPort = CUserOptions::UserOptions.m_nUDPPortFrom;
		int maxPort = CUserOptions::UserOptions.m_nUDPPortTo;
		minPort += (maxPort-minPort)/2;
		if(minPort&1)
			minPort+=1;

		int max_getport = maxPort-minPort;

		m_rdtClientPort = minPort;
		while(max_getport-->0)//it is safe to limit the count of fetching port
		{
			rdtStreamSock = new CMediaStreamSocket(mediaStream->m_mediaAddr->ss_family, 0, m_rdtClientPort);TRACE_NEW("m_rdtStreamSock ",rdtStreamSock );

			m_rdtClientPort = rdtStreamSock->GetMediaStreamSockLocalPort();
			if(m_rdtClientPort >=minPort&&m_rdtClientPort<=maxPort)
			{
				
				break;
			}
			else//error!!
			{
				char* format=CLog::formatString;
				sprintf(format,"%d:socket warning:invalid RDT port=%d valid(%d,%d)\n",max_getport,m_rdtClientPort,minPort,maxPort);
				CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt", format);

				//error resilience if it still fails at last time 
				if(m_rdtClientPort>maxPort)
					m_rdtClientPort-=1;
				else
					m_rdtClientPort+=1;
				SAFE_DELETE(rdtStreamSock);
				rdtStreamSock = NULL;
			}


		}
		rdtStreamSock->SetSockRecvBufSize(1024 * 200);
		m_rdtStreamSock = rdtStreamSock;
		m_rdtParser		= new CRDTParser(m_rdtStreamSock);TRACE_NEW("m_m_rdtParser ",m_rdtParser );
	}
	if(m_rdtParser)
		m_rdtParser->AddMediaStream(mediaStream);
#endif
	return m_rdtClientPort;
}
CRTSPMediaStreamSyncEngine::~CRTSPMediaStreamSyncEngine()
{
	SAFE_DELETE(m_rdtParser);
}

void CRTSPMediaStreamSyncEngine::AddMediaStream(CMediaStream * mediaStream)
{
	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		if(m_mediaStreams[i] == mediaStream)
		{
			return;
		}
	}

	m_mediaStreams[m_mediaStreamCount++] = mediaStream;

	if(m_mediaStreamCount>1&&CUserOptions::UserOptions.m_URLType!=UT_SDP2_LOCAL)
		m_dataSyncbyRTPInfo = true;//default,use rtpinfo to sync

}

bool CRTSPMediaStreamSyncEngine::AllMediaStreamsHaveBeenSynchronized()
{
#if SYNC_ONDEMAND_WITH_RTPINFO
	if(m_dataSyncbyRTPInfo)
		return false;
#endif
	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		if(!m_mediaStreams[i]->m_hasBeenSynchronized)
		{
			return false;
		}
	}
	return true;
}
void	CRTSPMediaStreamSyncEngine::SetSyncAudioTime(unsigned long syncAudioTime)
{
	if(m_firstIframeInfo.syncAudioTime==INVALID_SYNC_AUDIO_TIME)
	{
		m_firstIframeInfo.syncAudioTime=syncAudioTime;
		sprintf(CLog::formatString,"LIVE-SYNC:SetSyncAudioTime=%lu\n",syncAudioTime);
		CLog::Log.MakeLog(LL_BUF_STATUS,"flow.txt",CLog::formatString);
	}
}

void	CRTSPMediaStreamSyncEngine::SetFirstIFrameInfo(bool isIntra,unsigned long startTime,long seq,int flag)
{
	if(m_firstIframeInfo.valid)
		return;
	if(isIntra)
	{
		m_firstIframeInfo.valid=true;	
		m_firstIframeInfo.startTime = startTime;
		m_firstIframeInfo.seq		= seq;
		m_firstIframeInfo.flag		= flag;
		sprintf(CLog::formatString,"LIVE-SYNC:Get Intra Frame time=%lu,seq=%ld,flag=%d\n",startTime,seq,flag);
		CLog::Log.MakeLog(LL_BUF_STATUS,"flow.txt",CLog::formatString);
	}
}
bool CRTSPMediaStreamSyncEngine::RejectFrameBeforeIFrame(CMediaStream * mediaStream,unsigned long startTime,bool isIntra,bool isVideo)
{
	//test whether one stream can work
#if TEST_ONE_TRACK
	m_firstIFrameTime=-2;return isVideo;
#endif		
	if(m_isRejectAll)
		return true;
	if(m_firstIFrameTime==-2)//-2 means the mechanisam is disable
	{
		m_tryWaitCount = 0;
		m_waitIFrameTime = 0;
		m_isWatingIFrame = false;
		//mediaStream->GetRTSPSession()->SetDropFrames(0);
		return false;
	}

	do
	{
		if(isVideo)
		{
			
			if(isIntra)
			{

				m_firstIFrameTime=-2;
				if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
				{
					sprintf(CLog::formatString,"seek IntraFrame:The frame is Intra Frame time=%lu\n",startTime);
					CLog::Log.MakeLog(LL_BUF_STATUS,"buffering.txt",CLog::formatString);
				}
				break;
			}
			else
			{
				m_tryWaitCount++;
				if(m_tryWaitCount>m_maxWaitFrames)
				{
					sprintf(CLog::formatString,"$$$$wait for I frame long time(%lu),so give up!!\n",startTime);
					CLog::Log.MakeLog(LL_BUF_STATUS,"buffering.txt",CLog::formatString);
					m_firstIFrameTime=-2;

					break;
				}
			}
		
		}
		else
		{
			//break;//receive audio no doubt
			//always reject the audio before the I frame got
		}
		return true;

	}while(0);
#if ENABLE_DELAY
	if(m_firstIFrameTime==-2)//stop waiting
	{
		for(int i=0; i<m_mediaStreamCount; ++i)
		{
			m_mediaStreams[i]->SetDelayTime(startTime);
		}
	}
#endif//ENABLE_DELAY
	return false;
}


void CRTSPMediaStreamSyncEngine::RestartMediaStream(CMediaStream * mediaStream)
{
	mediaStream->m_hasBeenSynchronized = false;
	mediaStream->m_streamSyncTimestamp = -1;
	mediaStream->m_streamSyncWallClockTime->tv_sec = 0;
	mediaStream->m_streamSyncWallClockTime->tv_usec = 0;
	//if(!mediaStream->IsNeedMinusStartTime())//
	{
		mediaStream->m_streamTimestamp = -1;
		mediaStream->m_streamWallClockTime->tv_sec = 0;
		mediaStream->m_streamWallClockTime->tv_usec = 0;
	}
	mediaStream->m_dataSyncbySeqNum = true;
	mediaStream->m_streamRestartTime = 0;
	mediaStream->m_streamCurTime = 0;
	mediaStream->m_baseSyncWallClock->tv_sec = mediaStream->m_baseSyncWallClock->tv_usec = 0;
	mediaStream->m_streamTimeBeforeSync = 0;
	m_baseSyncWallClockTime.tv_sec = m_baseSyncWallClockTime.tv_usec = 0;
	m_dataSyncbyRTCP = false;
	m_commonASFTimeStamp=0;
}

void CRTSPMediaStreamSyncEngine::SetBaseSyncTimeIfAllStreamsHaveBeenSynchronized()
{
	if(m_baseSyncWallClockTime.tv_sec != 0 || m_baseSyncWallClockTime.tv_usec != 0)
		return;

	if(AllMediaStreamsHaveBeenSynchronized())
	{
		long rtspSessionStartTime = 0;
		int streamIndex = 0;
		for(streamIndex=0; streamIndex<m_mediaStreamCount; ++streamIndex)
		{
			rtspSessionStartTime = max(rtspSessionStartTime, m_mediaStreams[streamIndex]->m_streamCurTime);
			m_mediaStreams[streamIndex]->m_streamCurTime = 0;
		}

		for(streamIndex=0; streamIndex<m_mediaStreamCount; ++streamIndex)
		{
			m_mediaStreams[streamIndex]->m_streamRestartTime = rtspSessionStartTime;
		}
	}
}

void CRTSPMediaStreamSyncEngine::SyncMediaStream(CMediaStream * mediaStream, unsigned int ntpTimestampMSW, unsigned int ntpTimestampLSW, unsigned int rtpTimestamp)
{

	//if(mediaStream->MediaStreamEndTime() != 0||m_mediaStreamCount<2)
	if(m_mediaStreamCount<2||m_dataSyncbyRTCP)//if synced,do not update the RTCP any longer 20070930
	{
		if(m_dataSyncbyRTCP&&CUserOptions::UserOptions.m_bMakeLog)
		{
			char* format = CLog::formatString;
			sprintf(format,"RTCP_Packet:rtpTimestamp=%u,NTP_SEC=%u,NTP_USEC=%u\n",
				rtpTimestamp,ntpTimestampMSW - 0x83AA7E80,(unsigned int)((ntpTimestampLSW * 15625.0) / 0x04000000));
			if(mediaStream->IsVideoStream())
			{
				CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus.txt",format);
			}
			else
			{
				CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus.txt",format);
			}
		}
		return;
	}
	
	mediaStream->m_hasBeenSynchronized = true;
	mediaStream->m_streamSyncTimestamp = rtpTimestamp;
	mediaStream->m_streamSyncWallClockTime->tv_sec = ntpTimestampMSW - 0x83AA7E80;
	double microseconds = (ntpTimestampLSW * 15625.0) / 0x04000000;
	mediaStream->m_streamSyncWallClockTime->tv_usec = (unsigned int)(microseconds + 0.5); 
	mediaStream->AdjustSyncClockWhenRTCP();
	SetBaseSyncTimeIfAllStreamsHaveBeenSynchronized();
	if(CUserOptions::UserOptions.m_bMakeLog)
	{
		char* format = CLog::formatString;
		sprintf(format,"RTCP_Data:rtpTimestamp=%u,NTP_SEC=%ld,NTP_USEC=%ld\n",
		rtpTimestamp,mediaStream->m_streamSyncWallClockTime->tv_sec,mediaStream->m_streamSyncWallClockTime->tv_usec);
		if(mediaStream->IsVideoStream())
		{
			CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus.txt",format);
		}
		else
		{
			CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus.txt",format);
		}
	}
}

static int prevTimestampDiff = 0;

int CRTSPMediaStreamSyncEngine::CalculateMediaFramePresentationTime(CMediaStream * mediaStream, unsigned int rtpTimestamp, long * frameStartTime, int seqNum)
{
	int Allsynced = 0;
	int timestampDiff;
	long  clockDiff=0;
	
	if(AllMediaStreamsHaveBeenSynchronized())
	{
		timestampDiff =(int)(rtpTimestamp - mediaStream->m_streamSyncTimestamp);

		if(mediaStream->IsVideoStream())
		{
			if(prevTimestampDiff)
			{
				timestampDiff+=prevTimestampDiff*(double)mediaStream->RTPTimestampFrequency()/1000;
				prevTimestampDiff = 0;
			}
		}
		
		double timeDiff = timestampDiff / (double)mediaStream->RTPTimestampFrequency();
		long seconds, uSeconds;
		
		if(timeDiff >= 0.0)
		{
			seconds  = mediaStream->m_streamSyncWallClockTime->tv_sec + (unsigned int)(timeDiff);
			uSeconds = mediaStream->m_streamSyncWallClockTime->tv_usec + (unsigned int)((timeDiff - (unsigned int)timeDiff) * million);
			if(uSeconds >= million)
			{
				uSeconds -= million;
				++seconds;
			}
		}
		else
		{
			timeDiff = -timeDiff;
			seconds  = mediaStream->m_streamSyncWallClockTime->tv_sec - (unsigned int)(timeDiff);
			uSeconds = mediaStream->m_streamSyncWallClockTime->tv_usec - (unsigned int)((timeDiff - (unsigned int)timeDiff) * million);
			if((int)uSeconds < 0)
			{
				uSeconds += million;
				--seconds;
			}
		}
		
		if(mediaStream->m_streamSyncNTPAfterSync==-1&&CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
		{
			char* format = CLog::formatString;
			sprintf(format,"timediff=%f,m_streamTimestamp=%ld,m_streamSyncTimestamp=%u,NTP_SEC=%ld,NTP_USEC=%ld\n",
				timeDiff,mediaStream->m_streamTimestamp,mediaStream->m_streamSyncTimestamp,
				mediaStream->m_streamSyncWallClockTime->tv_sec,mediaStream->m_streamSyncWallClockTime->tv_usec);
			if(mediaStream->IsVideoStream())
			{
				CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus.txt",format);
			}
			else
			{
				CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus.txt",format);
			}
		}

		mediaStream->m_streamSyncTimestamp = rtpTimestamp;
		mediaStream->m_streamSyncWallClockTime->tv_sec = seconds;
		mediaStream->m_streamSyncWallClockTime->tv_usec = uSeconds;


		if(mediaStream->m_streamSyncNTPAfterSync==-1)//firstTime sync
		{
			mediaStream->m_streamSyncNTPAfterSync = seconds+100;

		}

		//After A/V are both synced,check whether the sync by RTCP is reasonable
		if(m_mediaStreams[0]->m_streamSyncNTPAfterSync!=-1&&m_mediaStreams[1]->m_streamSyncNTPAfterSync!=-1)
		{
#define AV_CLOCK_DIFF_LIMIT 20
#define MOVE_FOW_SEC 5 //5 million second
			int diff = m_mediaStreams[0]->m_streamSyncNTPAfterSync-m_mediaStreams[1]->m_streamSyncNTPAfterSync;
			if(diff>AV_CLOCK_DIFF_LIMIT||diff<-AV_CLOCK_DIFF_LIMIT)//not reasonable,reset the sync,
			{
				CLog::Log.MakeLog(LL_RTCP_ERR,"rtcpStatus.txt","the diff of A/V NTP is too large,unnormal\n");

				for (int streamIndex=0; streamIndex<m_mediaStreamCount; ++streamIndex)
				{
					CMediaStream* mediaStream=m_mediaStreams[streamIndex];
					mediaStream->m_hasBeenSynchronized = false;
					mediaStream->m_streamSyncTimestamp = -1;
					mediaStream->m_streamSyncWallClockTime->tv_sec = 0;
					mediaStream->m_streamSyncWallClockTime->tv_usec = 0;

					mediaStream->m_streamRestartTime = 0;
					m_baseSyncWallClockTime.tv_sec = m_baseSyncWallClockTime.tv_usec = 0;
					mediaStream->m_baseSyncWallClock->tv_sec = mediaStream->m_baseSyncWallClock->tv_usec = 0;
					mediaStream->m_streamSyncNTPAfterSync = -1;
				}
				*frameStartTime = mediaStream->m_streamCurTime = mediaStream->m_streamTimeBeforeSync+ m_playBegin;
				
				return 0;
			}
			Allsynced = 1;
			//mediaStream->m_streamRestartTime = mediaStream->m_streamTimeBeforeSync;
		}
		else//the check whether the sync is reasonable has not finished,so still use the seq_number to sync it
		{
			//m_baseSyncWallClockTime= *(mediaStream->m_streamSyncWallClockTime);
			goto SYNC_BY_SEQUENCE;
		}

		if(mediaStream->m_isNewStart)//mediaStream->m_baseSyncWallClock->tv_sec==0&&mediaStream->m_baseSyncWallClock->tv_usec==0||mediaStream->m_isNewStart)
		{
			mediaStream->m_baseSyncWallClock->tv_sec  = seconds;//mediaStream->m_streamSyncWallClockTime->tv_sec;
			mediaStream->m_baseSyncWallClock->tv_usec = uSeconds;//mediaStream->m_streamSyncWallClockTime->tv_usec;
			if(1)//mediaStream->m_isNewStart)
			{

				for (int streamIndex=0; streamIndex<m_mediaStreamCount; ++streamIndex)
				{
					CMediaStream* mediaStream2=m_mediaStreams[streamIndex];
					if(mediaStream2!=mediaStream&&mediaStream2->m_isNewStart==false)//mediastream2 has been synced by RTCP
					{
						timeDiff = Timeval2Int32MSDiff(mediaStream2->m_baseSyncWallClock,mediaStream->m_baseSyncWallClock);//(Timeval2Int64((mediaStream2->m_baseSyncWallClock))-Timeval2Int64((mediaStream->m_baseSyncWallClock)))/10000;

						if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
						{
							sprintf(CLog::formatString,"after_new_start:NTP(%ld,%ld) NTP2(%ld,%ld)\n",mediaStream2->m_baseSyncWallClock->tv_sec,
								mediaStream2->m_baseSyncWallClock->tv_usec,
								mediaStream->m_baseSyncWallClock->tv_sec,
								mediaStream->m_baseSyncWallClock->tv_usec);

							CLog::Log.MakeLog(LL_VIDEO_TIME,"flow.txt",CLog::formatString);
						}
						if(mediaStream->IsAudioStream())//based on audio
						{
							mediaStream2->m_streamTimeBeforeSync = mediaStream->m_streamTimeBeforeSync;
							mediaStream2->m_baseSyncWallClock->tv_sec  = mediaStream->m_baseSyncWallClock->tv_sec;
							mediaStream2->m_baseSyncWallClock->tv_usec = mediaStream->m_baseSyncWallClock->tv_usec;

							if(CUserOptions::UserOptions.m_nHKIcableAVOutOfSync)
							{
								if(mediaStream->m_streamTimeBeforeSync>200)
									prevTimestampDiff = mediaStream->m_streamTimeBeforeSync;
							}
								
						}
						else
						{
							mediaStream->m_streamTimeBeforeSync = mediaStream2->m_streamTimeBeforeSync;
							mediaStream->m_baseSyncWallClock->tv_sec  = mediaStream2->m_baseSyncWallClock->tv_sec;
							mediaStream->m_baseSyncWallClock->tv_usec = mediaStream2->m_baseSyncWallClock->tv_usec; 
						}


						mediaStream2->m_dataSyncbySeqNum = false;
						mediaStream->m_dataSyncbySeqNum = false;
						mediaStream2->AdjustSyncClockWithRTPInfo();
						mediaStream->AdjustSyncClockWithRTPInfo();
						m_dataSyncbyRTCP = true;

					}
				}

			}
			mediaStream->m_isNewStart = false;

		}
		clockDiff = Timeval2Int32MSDiff(mediaStream->m_streamSyncWallClockTime,mediaStream->m_baseSyncWallClock);
		if(m_playBegin>mediaStream->m_streamTimeBeforeSync)
			mediaStream->m_streamTimeBeforeSync = m_playBegin;
		mediaStream->m_streamCurTime = clockDiff+mediaStream->m_streamTimeBeforeSync;
	}
	else 
	{
SYNC_BY_SEQUENCE:
		if(mediaStream->m_streamTimestamp==-1)
		{
#if SYNC_ONDEMAND_WITH_RTPINFO
			if (m_dataSyncbyRTPInfo)
			{
				mediaStream->m_streamTimestamp=mediaStream->m_rtpInfo.rtpTimestamp;
				//Allsynced = 1;
			}
			else
#endif//SYNC_ONDEMAND_WITH_RTPINFO
			{		
				mediaStream->m_streamTimestamp = rtpTimestamp;

			if(!mediaStream->m_rtpInfo.rtpTimestamp)
				mediaStream->m_rtpInfo.rtpTimestamp = rtpTimestamp;
			}

			if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
			{
				sprintf(CLog::formatString,"%d,m_streamTimestamp==%ld(rtpinfo=%u),and the rtpTimestamp=%u,rtpInfo.SeqNum=%d\n",m_dataSyncbyRTPInfo,mediaStream->m_streamTimestamp,mediaStream->m_rtpInfo.rtpTimestamp,rtpTimestamp,mediaStream->m_rtpInfo.seqNum);
				if(mediaStream->IsVideoStream())
				{
					CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus.txt",CLog::formatString);
				}
				else
				{
					CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus.txt",CLog::formatString);
				}
			}

		}

		timestampDiff = (int)(rtpTimestamp - mediaStream->m_rtpInfo.rtpTimestamp);

		double timeDiff = timestampDiff / (double)mediaStream->RTPTimestampFrequency();
		long TimeDiff = (long)(timeDiff*1000);
		long tmpTS = m_playBegin + TimeDiff;

		if(tmpTS<m_playBegin)
		{
			sprintf(CLog::formatString,"INVALID_TIMESTAMP:ts=%d,prev=%ld,time=%ld,prevTime=%ld\n",rtpTimestamp,mediaStream->m_streamTimestamp,tmpTS,mediaStream->m_streamCurTime);
			if(mediaStream->IsVideoStream())
			{
				CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus.txt",CLog::formatString);
			}
			else if (mediaStream->IsAudioStream())
			{
				CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus.txt",CLog::formatString);
			}
			return INVALID_TIMESTAMP;
		}
		
		mediaStream->m_streamTimestamp   = rtpTimestamp;
		mediaStream->m_streamCurTime	 = tmpTS;
		mediaStream->m_streamTimeBeforeSync=mediaStream->m_streamCurTime;
	}
	*frameStartTime = mediaStream->m_streamCurTime;


	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		char* format = CLog::formatString;
		long start = *frameStartTime;
		long restart = mediaStream->m_streamTimeBeforeSync;
		long currTime = clockDiff;
		unsigned long ts = Allsynced?mediaStream->m_streamSyncTimestamp:mediaStream->m_streamTimestamp;
		if(Allsynced)
		{

			sprintf(format,"SYNC:NTP=%ld(%ld+%ld):diff=%d,TS=%lu,NTP(%ld,%ld),base(%ld,%ld),seq=%d\n",
				start,currTime,restart,timestampDiff,ts,mediaStream->m_streamSyncWallClockTime->tv_sec,mediaStream->m_streamSyncWallClockTime->tv_usec,
				mediaStream->m_baseSyncWallClock->tv_sec,mediaStream->m_baseSyncWallClock->tv_usec,seqNum);
		}
		else
		{
			sprintf(format,"NTP=%ld:rtpTSDiff=%d,TS=%lu,NTP(%ld,%ld),begin=%ld,seq=%d\n",
				start,timestampDiff,ts,mediaStream->m_streamWallClockTime->tv_sec,mediaStream->m_streamWallClockTime->tv_usec,m_playBegin,seqNum);
		}

		if(mediaStream->IsVideoStream())
		{
			CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus.txt",format);
		}
		else if (mediaStream->IsAudioStream())
		{
			CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus.txt",format);
		}
#ifdef DIMS_DATA
		else if (mediaStream->IsDataStream())
		{
			CLog::Log.MakeLog(LL_AUDIO_TIME,"dimsDataStatus.txt",format);
		}
#endif//DIMS_DATA
		

	}
	return 0;

}
void CRTSPMediaStreamSyncEngine::AssignRTPInfo(CMediaStream* stream)
{
	if(stream->m_rtpInfo.rtpTimestampNotExist==false)
	{
		if(m_commonASFSeqNum == ASF_SEQ_RESET)
			m_commonASFSeqNum = stream->m_rtpInfo.seqNum;
		else if(m_commonASFSeqNum!=stream->m_rtpInfo.seqNum)//there is another stream
			m_commonASFSeqNum = ASF_SEQ_USE_INDIVIDUAL;

	}
	/*
	for(int i=0; i<m_mediaStreamCount; ++i)
	{
		if(m_mediaStreams[i]!=stream&&m_mediaStreams[i]->m_rtpInfo.seqNum!=stream->m_rtpInfo.seqNum)
		{
			m_mediaStreams[i]->m_rtpInfo.seqNum=stream->m_rtpInfo.seqNum;
			m_mediaStreams[i]->m_rtpInfo.rtpTimestamp=stream->m_rtpInfo.rtpTimestamp;
			m_mediaStreams[i]->m_rtpInfo.seqNumNotExist=false;
			m_mediaStreams[i]->m_rtpInfo.rtpTimestampNotExist=false;
		}
	}*/
}
void CRTSPMediaStreamSyncEngine::SetSyncByRTPInfo(bool sync)
{
#if SYNC_ONDEMAND_WITH_RTPINFO
	if(m_mediaStreamCount>1&&!m_dataSyncbyRTCP&&m_mediaStreams[0]->m_mediaStreamEndTime>0)//if live or  synced by RTCP,do not use RTPInfo again
	{
		m_dataSyncbyRTPInfo=sync;
		int start=0;
		UINT32 start_usec=0;
		for(int i=0; i<m_mediaStreamCount; ++i)
		{
			if(sync)
			{
				if(m_mediaStreams[i]->m_streamTimestamp!=-1)
				{
					//m_mediaStreams[i]->retrieveAllPacketsFromRTPBuffer();
					start_usec = 0;
				}

				start = m_playBegin-m_mediaStreams[i]->m_streamStartTime/1000;
				if(start<0)
					start = 0;
				m_mediaStreams[i]->m_streamWallClockTime->tv_sec  = start;
				m_mediaStreams[i]->m_streamWallClockTime->tv_usec = start_usec;
			}
			else
			{
				if(m_mediaStreams[i]->m_streamTimestamp!=-1)//if not restart(paused),set the start time
				{
					start = m_mediaStreams[i]->GetPauseTime()/1000;
					m_mediaStreams[i]->m_streamWallClockTime->tv_sec  = 0;
					m_mediaStreams[i]->m_streamWallClockTime->tv_usec = million/2;
				}
				
			}
			m_mediaStreams[i]->m_streamTimestamp=-1;
		}
		if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
		{
			sprintf(CLog::formatString,"RTPInfoSync=%d,start=%d\n",sync,start);
			CLog::Log.MakeLog(LL_AUDIO_TIME,"rtsp.txt",CLog::formatString);
		}
	}
	else
	{
		m_dataSyncbyRTPInfo= false;
	}
	
#endif//SYNC_ONDEMAND_WITH_RTPINFO
}
void CRTSPMediaStreamSyncEngine::CalcualteMediaFramePresentationTimeASF(CMediaStream * mediaStream,int rtpTimeStamp,int seq,long * frameStartTime)
{
	*frameStartTime=0;
	if(m_commonASFTimeStamp)
	{
		float ts=rtpTimeStamp-m_commonASFTimeStamp;
		if(ts>0)
		{
			ts=ts*1000.0/mediaStream->RTPTimestampFrequency();
			*frameStartTime=int(ts);
		}
	}

	*frameStartTime += m_playBegin;
	
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		char* format = CLog::formatString;
		sprintf(format,"ASF:NTP=%ld:diff=%d,(%d-%d) seq=%d\n",
			*frameStartTime,rtpTimeStamp-m_commonASFTimeStamp,rtpTimeStamp,m_commonASFTimeStamp,seq);

		if(mediaStream->IsVideoStream())
		{
			CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus.txt",format);
		}
		else
		{
			CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus.txt",format);
		}
	}
}
void CRTSPMediaStreamSyncEngine::CalculateMediaFramePresentationTimeBasedOnFirstFrame(CMediaStream * mediaStream, unsigned int rtpTimestamp,long * frameStartTime)
{
	int timestampDiff = (int)(rtpTimestamp - mediaStream->m_firstTimeStamp);
	double timeDiff = timestampDiff / (double)mediaStream->RTPTimestampFrequency();
	int seconds  = (int)(timeDiff);
	int uSeconds = (int)((timeDiff - (int)timeDiff) * million);
	mediaStream->m_streamTimestamp = rtpTimestamp;
	mediaStream->m_streamWallClockTime->tv_sec = seconds;
	mediaStream->m_streamWallClockTime->tv_usec = uSeconds;
	mediaStream->m_streamCurTime = Timeval2Int32MS(mediaStream->m_streamWallClockTime);
	mediaStream->m_streamTimeBeforeSync=mediaStream->m_streamCurTime;
	int actualTime = mediaStream->m_streamCurTime;
	
	if(abs(actualTime-mediaStream->m_previousPos)<1500)
		*frameStartTime = 0;
	else
		*frameStartTime = actualTime-mediaStream->m_streamStartTime;

	*frameStartTime+= m_playBegin;

	if(*frameStartTime==0&&abs(actualTime-mediaStream->m_streamStartTime)>3000)
	{
		sprintf(CLog::formatString,"rebase the streamStartTime(%d->%d)\n",mediaStream->m_streamStartTime,actualTime);
		CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt",CLog::formatString);
		mediaStream->m_streamStartTime=actualTime;
	}
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		char* format = CLog::formatString;
		sprintf(format,"SYNC_FIRST:NTP=%ld(%d-%d):rtpTSDiff=%d,previousTime=%d\n",*frameStartTime,actualTime,mediaStream->m_streamStartTime,timestampDiff,mediaStream->m_previousPos);

		if(mediaStream->IsVideoStream())
		{
			CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus.txt",format);
		}
		else
		{
			CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus.txt",format);
		}
	}
}

