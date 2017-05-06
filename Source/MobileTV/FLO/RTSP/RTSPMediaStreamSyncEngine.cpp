#include <string.h>

#include "utility.h"
#include "network.h"
#include "TaskSchedulerEngine.h"
#include "MediaStream.h"
//#include "RDTMediaStream.h"
#include "RTSPSession.h"
#include "RTSPMediaStreamSyncEngine.h"
#include "MediaStreamSocket.h"
//#include "RDTParser.h"

CRTSPMediaStreamSyncEngine * CRTSPMediaStreamSyncEngine::m_pRTSPMediaStreamSyncEngine = NULL;

CRTSPMediaStreamSyncEngine * CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine()
{
	if(m_pRTSPMediaStreamSyncEngine == NULL)
	{
		m_pRTSPMediaStreamSyncEngine = new CRTSPMediaStreamSyncEngine();TRACE_NEW("m_pRTSPMediaStreamSyncEngine",m_pRTSPMediaStreamSyncEngine);
	}

	return m_pRTSPMediaStreamSyncEngine;
}

void CRTSPMediaStreamSyncEngine::RecvMediaFrame(int streamNum, FrameData* frame)
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
	printf("CRTSPMediaStreamSyncEngine::EnableRejectAfterScan(...) \n");
	m_isRejectAll = isRejectAll;
	m_isWatingIFrame = true;
	m_theAudioBeforeIFrameIsCleared=false;
	m_firstIFrameTime=-1;//m_firstIFrameTime also act as a flag if the waiting mechanisam is working -1:enable
	m_avsyncAfterWait=0;
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{

		for(int i=0; i<m_mediaStreamCount; ++i)
		{
			//sprintf(CLog::formatString,"!!!!After flush,the left frame=%d\n",m_mediaStreams[i]->HaveBufferedMediaFrameCount());
			//CLog::Log.MakeLog(LL_BUF_STATUS,"buffering.txt",CLog::formatString);
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
}

int	CRTSPMediaStreamSyncEngine::CreateRDTParser(CRDTMediaStream* mediaStream)
{
	printf("FloTV: Should not call function  CRTSPMediaStreamSyncEngine::CreateRDTParser(...)\n");
	return 0;
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

void CRTSPMediaStreamSyncEngine::SetSyncAudioTime(unsigned long syncAudioTime)
{
	if(m_firstIframeInfo.syncAudioTime == INVALID_SYNC_AUDIO_TIME)
	{
		m_firstIframeInfo.syncAudioTime=syncAudioTime;
		sprintf(CLog::formatString,"LIVE-SYNC:SetSyncAudioTime=%u\n",syncAudioTime);
		CLog::Log.MakeLog(LL_BUF_STATUS,"flow.txt",CLog::formatString);
	}
}

void CRTSPMediaStreamSyncEngine::SetFirstIFrameInfo(bool isIntra,unsigned long startTime,long seq,int flag)
{
	if(m_firstIframeInfo.valid)
		return;
	
	if(isIntra)
	{
		m_firstIframeInfo.valid=true;	
		m_firstIframeInfo.startTime = startTime;
		m_firstIframeInfo.seq		= seq;
		m_firstIframeInfo.flag		= flag;
		sprintf(CLog::formatString,"LIVE-SYNC:Get Intra Frame time=%u,seq=%d,flag=%d\n",startTime,seq,flag);
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
					sprintf(CLog::formatString,"seek IntraFrame:The frame is Intra Frame time=%d\n",startTime);
					CLog::Log.MakeLog(LL_BUF_STATUS,"buffering.txt",CLog::formatString);
				}
				break;
			}
			else
			{
				m_tryWaitCount++;
				if(m_tryWaitCount>m_maxWaitFrames)
				{
					sprintf(CLog::formatString,"$$$$wait for I frame long time(%d),so give up!!\n",startTime);
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
	mediaStream->m_isNewStart = true;
	mediaStream->m_streamSyncNTPAfterSync=-1;
	mediaStream->m_baseSyncWallClock->tv_sec = mediaStream->m_baseSyncWallClock->tv_usec = 0;
	mediaStream->m_streamTimeBeforeSync = 0;
	m_baseSyncWallClockTime.tv_sec = m_baseSyncWallClockTime.tv_usec = 0;
	m_dataSyncbyRTCP = false;
	m_commonASFTimeStamp=0;
}


#define VO_MAX(a, b)	(((a) > (b)) ? (a) : (b))


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
			rtspSessionStartTime = VO_MAX(rtspSessionStartTime, m_mediaStreams[streamIndex]->m_streamCurTime);
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
	mediaStream->m_nRTCPSyncNum++;
	
#if 1
	if(m_dataSyncbyRTCP)//if synced,do not update the RTCP any longer 20070930
	{
		if(CUserOptions::UserOptions.m_bMakeLog)
		{
			char* format = CLog::formatString;
			sprintf(format,"RTCP_Packet: rtpTimestamp = %u, NTP_SEC = %u, NTP_USEC = %u, NTP_MSW = %u, NTP_LSW = %u\n",
				rtpTimestamp, ntpTimestampMSW - 0x83AA7E80, (unsigned int)((ntpTimestampLSW * 15625.0) / 0x04000000), ntpTimestampMSW, ntpTimestampLSW);
			
			if(mediaStream->IsVideoStream())
			{
				CLog::Log.MakeLog(LL_RTP_ERR, "videoDataStatus.txt", format);
			}
			else
			{
				CLog::Log.MakeLog(LL_RTP_ERR, "audioDataStatus.txt", format);
			}
		}
		
		return;
	}
#endif
	
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
		sprintf(format, "RTCP_Data: rtpTimestamp = %u, NTP_SEC = %u, NTP_USEC = %u, NTP_MSW = %u, NTP_LSW = %u\n",
		rtpTimestamp, mediaStream->m_streamSyncWallClockTime->tv_sec, mediaStream->m_streamSyncWallClockTime->tv_usec, ntpTimestampMSW, ntpTimestampLSW);
		
		if(mediaStream->IsVideoStream())
		{
			CLog::Log.MakeLog(LL_RTP_ERR, "videoDataStatus.txt",format);
		}
		else
		{
			CLog::Log.MakeLog(LL_RTP_ERR, "audioDataStatus.txt",format);
		}
	}
}

int CRTSPMediaStreamSyncEngine::CalculateMediaFramePresentationTime(CMediaStream * mediaStream, unsigned int rtpTimestamp, long * frameStartTime, int seqNum)
{
	int Allsynced = 0;
	int timestampDiff;
	long  clockDiff=0;
	if(AllMediaStreamsHaveBeenSynchronized())
	{
		timestampDiff =(int)(rtpTimestamp - mediaStream->m_streamSyncTimestamp);
		double timeDiff = timestampDiff / (double)mediaStream->RTPTimestampFrequency();

		unsigned int seconds, uSeconds;

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
		if(0)//timeDiff>10)
		{
			SLOG2(LL_SOCKET_ERR,"rtpStatus.txt","timeDiff>10:curTS=%u, baseTS=%u\n",rtpTimestamp,mediaStream->m_streamSyncTimestamp);
			goto SYNC_BY_SEQUENCE;
		}
		if(mediaStream->m_streamSyncNTPAfterSync==-1 && CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
		{
			char* format = CLog::formatString;
			//unsigned long temp;
			sprintf(format,"timediff = %f, m_streamTimestamp = %u, m_streamSyncTimestamp = %u, NTP_SEC = %u, NTP_USEC = %u\n",
				timeDiff, mediaStream->m_streamTimestamp, mediaStream->m_streamSyncTimestamp,
				mediaStream->m_streamSyncWallClockTime->tv_sec, mediaStream->m_streamSyncWallClockTime->tv_usec);
			
			if(mediaStream->IsVideoStream())
			{
				CLog::Log.MakeLog(LL_RTP_ERR, "videoDataStatus.txt",format);
			}
			else
			{
				CLog::Log.MakeLog(LL_RTP_ERR, "audioDataStatus.txt",format);
			}
		}

		mediaStream->m_streamSyncTimestamp = rtpTimestamp;
		mediaStream->m_streamSyncWallClockTime->tv_sec = seconds;
		mediaStream->m_streamSyncWallClockTime->tv_usec = uSeconds;

		if(mediaStream->m_streamSyncNTPAfterSync==-1)//firstTime sync
		{
			mediaStream->m_streamSyncNTPAfterSync = seconds+100;
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
							sprintf(CLog::formatString,"after_new_start:NTP(%u,%u) NTP2(%u,%u)\n",mediaStream2->m_baseSyncWallClock->tv_sec,
								mediaStream2->m_baseSyncWallClock->tv_usec,
								mediaStream->m_baseSyncWallClock->tv_sec,
								mediaStream->m_baseSyncWallClock->tv_usec);

							CLog::Log.MakeLog(LL_RTP_ERR,"flow.txt",CLog::formatString);
						}

						if(mediaStream->IsAudioStream())//based on audio
						{
							mediaStream2->m_streamTimeBeforeSync = mediaStream->m_streamTimeBeforeSync;
							mediaStream2->m_baseSyncWallClock->tv_sec  = mediaStream->m_baseSyncWallClock->tv_sec;
							mediaStream2->m_baseSyncWallClock->tv_usec = mediaStream->m_baseSyncWallClock->tv_usec; 
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
		mediaStream->m_streamCurTime = clockDiff+mediaStream->m_streamTimeBeforeSync;
		Allsynced = 1;
	}
	else 
	{
SYNC_BY_SEQUENCE:

		if(mediaStream->m_streamTimestamp==-1)
		{

			mediaStream->m_streamTimestamp = rtpTimestamp;
		}

		timestampDiff = (int)(rtpTimestamp - mediaStream->m_streamTimestamp);
		

		
		double timeDiff = timestampDiff / (double)mediaStream->RTPTimestampFrequency();
		const unsigned int million = 1000000;
		unsigned int seconds, uSeconds;
		if(timeDiff >= 0.0)
		{
			seconds  = mediaStream->m_streamWallClockTime->tv_sec + (unsigned int)(timeDiff);
			uSeconds = mediaStream->m_streamWallClockTime->tv_usec + (unsigned int)((timeDiff - (unsigned int)timeDiff) * million);
			if(uSeconds >= million)
			{
				uSeconds -= million;
				++seconds;
			}
		}
		else
		{
			timeDiff = -timeDiff;
			seconds  = mediaStream->m_streamWallClockTime->tv_sec - (unsigned int)(timeDiff);
			uSeconds = mediaStream->m_streamWallClockTime->tv_usec - (unsigned int)((timeDiff - (unsigned int)timeDiff) * million);
			if((int)uSeconds < 0)
			{
				uSeconds += million;
				--seconds;
			}
		}

		mediaStream->m_streamTimestamp = rtpTimestamp;
		mediaStream->m_streamWallClockTime->tv_sec = seconds;
		mediaStream->m_streamWallClockTime->tv_usec = uSeconds;

		mediaStream->m_streamCurTime = Timeval2Int32MS(mediaStream->m_streamWallClockTime);
		mediaStream->m_streamTimeBeforeSync=mediaStream->m_streamCurTime;
	}
	*frameStartTime = mediaStream->m_streamCurTime;//mediaStream->m_streamRestartTime + 


	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		char* format = CLog::formatString;
		long start = *frameStartTime;
		//long temp;
		long restart = mediaStream->m_streamTimeBeforeSync;
		long currTime = clockDiff;
		unsigned long ts = Allsynced?mediaStream->m_streamSyncTimestamp:mediaStream->m_streamTimestamp;
		if(Allsynced)
		{

			sprintf(format,"SYNC: NTP = %d(%d+%d): diff = %d, TS = %u, NTP(%u, %u), base(%u, %u), seq = %d\n",
				start,currTime,restart,timestampDiff,ts,mediaStream->m_streamSyncWallClockTime->tv_sec,mediaStream->m_streamSyncWallClockTime->tv_usec,
				mediaStream->m_baseSyncWallClock->tv_sec,mediaStream->m_baseSyncWallClock->tv_usec,seqNum);
		}
		else
		{
			sprintf(format,"NTP = %d: rtpTSDiff = %d, TS = %u, NTP(%u, %u), seq = %d\n",
				start,timestampDiff,ts,mediaStream->m_streamWallClockTime->tv_sec,mediaStream->m_streamWallClockTime->tv_usec,seqNum);
		}

		if(mediaStream->IsVideoStream())
		{
			CLog::Log.MakeLog(LL_RTP_ERR, "videoDataStatus.txt",format);
		}
		else if (mediaStream->IsAudioStream())
		{
			CLog::Log.MakeLog(LL_RTP_ERR, "audioDataStatus.txt",format);
		}

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
		unsigned int start_usec=0;
		for(int i=0; i<m_mediaStreamCount; ++i)
		{
			if(sync)
			{
				if(m_mediaStreams[i]->m_streamTimestamp!=-1)
				{
					//m_mediaStreams[i]->retrieveAllPacketsFromRTPBuffer();
					start_usec = m_playBegin.tv_usec;
				}

				start = m_playBegin.tv_sec-m_mediaStreams[i]->m_streamStartTime/1000;
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
			CLog::Log.MakeLog(LL_RTP_ERR,"rtsp.txt",CLog::formatString);
		}
	}
	else
	{
		m_dataSyncbyRTPInfo=false;
	}
	
#endif//SYNC_ONDEMAND_WITH_RTPINFO
}

