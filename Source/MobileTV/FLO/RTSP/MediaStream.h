#ifndef __MEDIASTREAM_H__
#define __MEDIASTREAM_H__

#include "utility.h"
#include "mswitch.h"
#include "network.h"
#include "RTPParser.h"

#define INVALID_TIMESTAMP 0x12345678


class CRTSPClientEngine;
class CRTSPSession;
class CRTSPMediaStreamSyncEngine;
class CDVBHMediaStreamSyncEngine;
//class CRTPParser;
class CRTCPParser;

class CMediaStreamSocket;

#ifndef DIMS_DATA
#define SYNC_ONDEMAND_WITH_RTPINFO 0
#endif//DIMS_DATA

#if 1//def _HTC
#define H264_COMBINE_ALL_NALU 1
#endif//#if H264_ANNEXB_FORMAT

#ifdef WIN_X86
const int MAX_TMP_H264BUF = 64*1024;
#else//WIN_X86
const int MAX_TMP_H264BUF = 64*1024;
#endif//WIN_X86

typedef struct  
{
	int size;
	int isIntra;
	unsigned long time;
	unsigned long seqNum;
	int	IsNotFirstNalu;
	int flag;
	int fragment;
	unsigned char buffer[MAX_TMP_H264BUF];
}GTmpBuf;

enum{
	FLUSH_MEDIABUFFER		= 1,
	FLUSH_RTPBUFFER			= 2,
	FLUSH_H264TEMPBUFFER	= 4,
};



const int sampRateTab[12] = {
	96000, 88200, 64000, 48000, 44100, 32000, 
	24000, 22050, 16000, 12000, 11025,  8000
};





#define MAX_OPAQUE_SIZE	 1024
/**
\brief the alive rtp media stream

1,maintain the media attributes of a session,such as name,duration,audio sample rate,video frame rate and so on.
2,parse the codec info
3,forward the RTP data to MediaSinker 
*/
class CMediaStream
{
public:
	CMediaStream(CRTSPSession * rtspSession);
	virtual ~CMediaStream(void);
	
protected:
	bool m_isVideo;
	int m_frameNum;
	GTmpBuf *m_gTmpBuf;
	void ResetTmpBuf();
	int    m_errPackNum;

public:
	/**parse the media attributes of the SDP*/
	virtual bool Init(char *  &sdpParam);

	unsigned short ClientRTPPort()  { return m_clientRTPPort; }
	unsigned short ClientRTCPPort() { return m_clientRTCPPort; }
	unsigned short ServerRTPPort()  { return m_serverRTPPort; }
	unsigned short ServerRTCPPort() { return m_serverRTCPPort; }
	unsigned char  RTPChannelID()   { return m_rtpChannelID; }
	
	void SetRTPChannelID(int id) {m_rtpChannelID=id; }
	unsigned char RTCPChannelID() { return m_rtcpChannelID; }
	const char * ControlAttr() { return m_controlAttr; }
	
#if 0//CP_SOCKET
	VOSocketAddress * MediaAddr() { return m_mediaAddr; }
#else
	struct voSocketAddress * MediaAddr() { return m_mediaAddr; }
#endif
	
	void SetServerRTPPort(unsigned short serverRTPPort)   { m_serverRTPPort = serverRTPPort; }
	void SetServerRTCPPort(unsigned short serverRTCPPort) { m_serverRTCPPort = serverRTCPPort; }
	unsigned int RTPPayloadType() { return m_rtpPayloadType; }
	unsigned int RTPTimestampFrequency() { return m_rtpTimestampFrequency; }
	
	/**Get the media attributes of the media name*/
	const char * MediaName() { return m_mediaName; }
	/**Get the media attributes of the codec name*/
	const char * CodecName() { return m_codecName; }
	const char * ProtocolName() { return m_protocolName; }
	/**Get the media attributes of the media start time*/
	float MediaStreamStartTime() { return m_mediaStreamStartTime; }
	/**Get the media attributes of the media end time*/
	float MediaStreamEndTime() { return m_mediaStreamEndTime; }
	void SetMediaStreamEndTime(float sessionDuration) { m_mediaStreamEndTime = sessionDuration; };
	bool IsVideoStream();
	float VideoFPS() { return m_videoFPS; }
	/**Get the media attributes of the video width*/
	unsigned int VideoWidth() { return m_videoWidth; }
	/**Get the media attributes of the video height*/
	unsigned int VideoHeight() { return m_videoHeight; }
	void SetVideoHW(unsigned int width,unsigned int height) { m_videoWidth=width; m_videoHeight=height; }
	
	
	bool IsAudioStream();
	/**Get the media attributes of the audio channels*/
	unsigned int AudioChannels() { return m_audioChannels; }
	void SetAudioFormat(unsigned int channels,unsigned int SampleRate,unsigned int audioBits)
	{
		m_audioChannels=channels;
		m_audioSampleRate=SampleRate;
		m_audioBits=audioBits;
	}
	
	/**Get the media attributes of the audio SampleRate*/
	unsigned int AudioSampleRate() { return m_audioSampleRate; }
	unsigned int AudioBits() { return m_audioBits; }

	bool IsApplicationStream();
	const char * FmtpConfig() { return m_fmtpConfig; }
	const char * FmtpSpropParameterSets() { return m_fmtpSpropParameterSets; }
	const char * FmtpMode() { return m_fmtpMode; }
	unsigned int FmtpPacketizationMode() { return m_fmtpPacketizationMode; }
	unsigned int FmtpProfileLevelID() { return m_fmtpProfileLevelID; }
	unsigned int FmtpSizeLength() { return m_fmtpSizeLength; }
	unsigned int FmtpIndexLength() { return m_fmtpIndexLength; }
	unsigned int FmtpIndexDeltaLength() { return m_fmtpIndexDeltaLength; }
	unsigned int FmtpOctetAlign() { return m_fmtpOctetAlign; }
	unsigned int FmtpInterleaving() { return m_fmtpInterleaving; }
	unsigned int FmtpRobustSorting() { return m_fmtpRobustSorting; }
	unsigned int FmtpCRC() { return m_fmtpCRC; }

	/**Get the media attributes of the meta info of codec*/
	virtual int GetCodecHeadData(unsigned char * codecHead);
	unsigned int MediaStreamBandwidthAS() { return m_mediaStreamBandwidthAS; }
	void retrieveAllPacketsFromRTPBuffer();
	
protected:
//	unsigned int m_fcc;
public:
	CRTPParser  * RTPParser() { return m_rtpParser; }
	CRTCPParser * RTCPParser() { return m_rtcpParser; }
	//unsigned int  GetCodecFCC(){return m_fcc;}
	//void		  SetCodecFCC(unsigned int fcc1){m_fcc=fcc1;}
	
public:
	/**receive the rtp media frame from RTP parser*/
	virtual void    RecvMediaFrame(unsigned char * frameData, int frameSize, unsigned int rtpTimestamp,unsigned int seqNum=0,bool IsKeyFrame=false);
	virtual void    FlushMediaStream(int flushFlag=0xffffffff);
#ifdef DIMS_DATA
	virtual void    RecvMediaFrameDIMSData(unsigned char * frameData, int frameSize, unsigned int rtpTimestamp,unsigned int seqNum=0);
#endif//DIMS_DATA
	
	//VOFILEREADRETURNCODE GetMediaFrameByIndex(VOSAMPLEINFO * pSampleInfo);
	//int                  GetNextKeyFrame(int nIndex, int nDirectionFlag);

	
	/**schedule the task of sending the rtcp receiver report*/
	void ScheduleReceiverReportTask();
	void UnscheduleReceiverReportTask();

//public: // multicast
//	bool AddMembership();
//	void DropMembership();
	
public:
	void SetFrameData(FrameData* frame){m_rtpParser->SetFrameData(frame);};
	FrameData* GetFrameData(){return m_rtpParser->GetFrameData();};
	
protected:
	virtual bool ParseSDPParam_m(char * sdpParam);
	bool		 ParseSDPParam_c(char * sdpParam);

	virtual bool ParseSDPAttribute_rtpmap(char * sdpParam);
	bool         ParseSDPAttribute_range(char * sdpParam);
	virtual bool ParseSDPAttribute_control(char * sdpParam);
	bool         ParseSDPAttribute_cliprect(char * sdpParam);
	bool         ParseSDPAttribute_framesize(char * sdpParam);
	bool         ParseSDPAttribute_framerate(char * sdpParam);

	bool		 ParseSDPAttribute_stream(char * sdpParam);
	virtual bool ParseSDPAttribute_fmtp(char * sdpParam);

	bool         CreateRTPPayloadParser(CMediaStreamSocket * rtpStreamSock, CMediaStreamSocket * rtcpStreamSock);
	
public:
	bool IsValidFrameBySeqNum(int seqNum){
		if(m_rtpParser)
			return m_rtpParser->IsValidPackBySeqNum(seqNum);
		else
			return false;
	}
	
	/**Parse the rtpInfo of the play response*/
	bool         ParseRTPInfoHeader(char * rtpInfoText);
	struct
	{
		unsigned int   streamID;
		unsigned short seqNum;
		unsigned int   rtpTimestamp;
		bool		   seqNumNotExist;
		bool		   rtpTimestampNotExist;
	}m_rtpInfo;
	
	bool		 IsRTPInfoTimeStampNotExist(){return m_rtpInfo.rtpTimestampNotExist;}
	int			 GetSeqNumInRTPInfo(){return m_rtpInfo.seqNumNotExist?0:m_rtpInfo.seqNum;}
	void		 ResetRTPInfo(){
		m_rtpInfo.streamID = m_rtpInfo.seqNum = m_rtpInfo.rtpTimestamp = 0;
		m_rtpInfo.seqNumNotExist = m_rtpInfo.rtpTimestampNotExist = true;}
	
	int          GetLastTimeInBuffer(){return 0; } //m_mediaFrameSink->GetLastTimeInBuffer();}
	
protected:
	int					  m_h264ConfigNotBase64;
	char			      m_mediaName[16];
	char			      m_codecName[32];
	char			      m_protocolName[16];
	char				  *m_controlAttr;
	float                 m_mediaStreamStartTime;
	float                 m_mediaStreamEndTime;
	unsigned int          m_rtpPayloadType;
	unsigned int          m_rtpTimestampFrequency;

	unsigned int		  m_fmtpPacketizationMode;
	unsigned int          m_fmtpProfileLevelID;
	unsigned int          m_fmtpSizeLength;
	unsigned int          m_fmtpIndexLength;
	unsigned int          m_fmtpIndexDeltaLength;
	unsigned int          m_fmtpOctetAlign;
	unsigned int          m_fmtpInterleaving;
	unsigned int          m_fmtpRobustSorting;
	unsigned int          m_fmtpCRC;
	char				  m_fmtpConfig[256];
    char                  m_fmtpSpropParameterSets[1024];
	char                  m_fmtpMode[32];

	unsigned char       * m_codecHeadData;
	int                   m_codecHeadDataLen;
	float       		  m_videoFPS;
	unsigned int		  m_videoWidth;
	unsigned int		  m_videoHeight;

	unsigned int		  m_audioChannels;
	unsigned int          m_audioSampleRate;
	unsigned int          m_audioBits;
	int					  m_framerate;//video or audio
#if 0//CP_SOCKET
	VOSocketAddress		 * m_mediaAddr;
#else
	struct voSocketAddress		 * m_mediaAddr;
#endif

	unsigned short		  m_clientRTPPort;
	unsigned short	 	  m_clientRTCPPort;
	unsigned short        m_serverRTPPort;
	unsigned short        m_serverRTCPPort;

	
	unsigned char         m_rtpChannelID;
	unsigned char         m_rtcpChannelID;

	unsigned int          m_mediaStreamBandwidthAS;


	CRTSPSession        * m_rtspSession;
	CRTPParser          * m_rtpParser;
	CRTCPParser         * m_rtcpParser;
//	CMediaFrameSink     * m_mediaFrameSink;
	
public:
	unsigned int  GetB_AS() {return m_mediaStreamBandwidthAS;}
	int		      GetFrameRate() {return m_framerate;}
	bool	      IsNeedRTCPSync();
	CRTSPSession* GetRTSPSession() {return m_rtspSession;};
	static unsigned char  m_streamChannelIDGenerator;
	
protected:
	bool			      m_hasBeenSynchronized;
	struct timeval        *m_baseSyncWallClock;

	struct timeval       *m_streamSyncWallClockTime;
	
	unsigned int          m_streamSyncTimestamp;
	int					  m_streamSyncNTPAfterSync;
	unsigned long		  m_nPausedTime;
	unsigned long		  m_nCurPlayTime;
	typedef struct{
		unsigned long timeStamp;
		long		  sec;
		long		  uSec;
	}RTCP_Data;
	
	RTCP_Data		  m_previousRTCP;
	void ResetPreviousRTCPData(){m_previousRTCP.timeStamp=m_previousRTCP.sec=m_previousRTCP.uSec=0;};
	
public:
	void AdjustSyncClockWhenRTCP();
	unsigned char *GetCodecHeadBuffer(int size);                 

public:
	int		GetPauseTime(){return m_nCurPlayTime;};
	void	SetPauseTime(int time){m_nPausedTime=time;}
	
protected:
	struct timeval      * m_streamWallClockTime;
	unsigned int          m_streamTimestamp;
	int					  m_firstTimeStamp;
	int					  m_streamStartTime;
	int					  m_previousPos;
	bool				  m_dataSyncbySeqNum;
	bool				  IsNeedSyncByFirstFrame();
	int					  m_recalFrameNum;
	
public:
	void				  SetStreamStartTime(int start){start>=0?m_streamStartTime=start:m_streamStartTime=0;};

	
protected:
	long               m_streamRestartTime;
	long               m_streamCurTime;
	long               m_streamTimeBeforeSync;
public:
	int  removeFrameBefore(long upTime);
	void AdjustSyncClock(long adjustTime,float begin);
	void AdjustSyncClockWithRTPInfo();
	
protected:
	int m_adjustTimeBetweenPauseAndPlay;
	CMediaStreamSocket* m_rtpStreamSocket;
	CMediaStreamSocket* m_rtcpStreamSocket;
public:
	CMediaStreamSocket* GetRTPStreamSocket(){return m_rtpStreamSocket;}
	CMediaStreamSocket* GetRTCPStreamSocket(){return m_rtcpStreamSocket;}
protected:
	unsigned long m_previousTimeStamp;
	long m_frameCount;
	long m_totalTime ;
	bool m_done ;

	long m_delayTime;
public:
//	void SetRTSPTrack(CRTSPTrack* rtspTrack){m_rtspTrack=rtspTrack;}
	void SetDelayTime(int time);
	bool HasGotData(){return m_firstTimeStamp!=-1;}
	void CalculateNPTAfterPlayResponse();
	unsigned long CalculateNPT(unsigned long timestamp,int seqNum);
	
protected:
	int	m_previousFrameTime;
	int m_previousFrameIsIntra;
	int m_lengthSizeOfNALU;
	int ParseAVCConfig(char* pData,char* dst);
	bool m_isNewStart;

	
	
#if SUPPORT_ADAPTATION
	int		m_reportFreq;
	bool	ParseSDPAttribute_3gppAdapation(char* sdpParam);
public:
	int		GetReportFreqOf3gppAdaptation(){return m_reportFreq;}
	unsigned int GetFirstFrameSeqNumInBuffer();
#endif//SUPPORT_ADAPTATION
	
friend class CRTSPMediaStreamSyncEngine;
	
public:
	bool IsNewStart(){return m_isNewStart;}
	bool IsVideoH264();
	bool IsVideoH264AVC();
	bool IsVideoMpeg4();
	bool IsVideoH263();
	bool IsVideoRMVideo();
	bool IsVideoWMV();

	bool IsAudioAMRNB();
	bool IsAudioAMRWB();
	bool IsAudioAMRWBP();
	bool IsAudioAAC();
	bool IsAudioQCELP();
	bool IsAudioRMAudio();
	bool IsAudioMP3();
	bool IsAudioWMA();
	bool IsPacketLoss(){return m_packetLoss;}
	void SetPacketLoss(bool loss){m_packetLoss=loss;}
protected:
	int m_streamNum;
	long m_timePerFrame;
	bool m_packetLoss;
	int  m_scaleAACPlus;
public:
	long GetTimePerFrame(){return m_timePerFrame;}
	
public:
	void ResetAllFramesAfterPos(int oldstart,int newstart);//{m_mediaFrameSink->ResetAllFramesAfterPos(oldstart, newstart);}

	int  GetStreamNum(){return m_streamNum;}
	void SetStreamNum(int streamNum){m_streamNum=streamNum;}
	bool SetTransportProtocol(bool streamingOverTCP);
	void UpdateSocket();
	
#ifdef MEDIA_FLO
private:
	bool m_newFrameAvailable;
public:
	void NotifyNewFrame(bool bAvailable) { m_newFrameAvailable = bAvailable; }
	void NotifyErrPack();
#endif//MEDIA_FLO
	
public:
	void HandleH264BitError();
	void FastChannelChange();
public:
	int    m_nRTCPSyncNum;






public:





int Config2ADTSHeader(/*in*/	unsigned char *config, 
					  /*in*/	unsigned int  conlen,
					  /*in*/	unsigned int  framelen,
					  /*in&out*/unsigned char *adtsbuf,
					  /*in&out*/unsigned int  *adtslen)
{
	int object, sampIdx, sampFreq, chanNum;
//	unsigned char tbuf;

	if(config == NULL || conlen < 2 || adtsbuf == NULL || *adtslen < 7)
		return -1;

	object = config[0] >> 3;
	if(object > 5 && object != 29)
	{
		return -2;
	}
	else if(object == 5 || object == 29 )
	{
		object = 2;
	}
	if(object == 0) object += 1;

	object--;

	sampIdx = ((config[0] & 7) << 1) | (config[1] >> 7);
	if(sampIdx == 0x0f)
	{
		int idx;

		if(conlen < 5)
			return -3;

		sampFreq = ((config[1]&0x7f) << 17) | (config[2] << 9) | ((config[3] << 1)) | (config[4] >> 7);

		for (idx = 0; idx < 12; idx++) {
			if (sampFreq == sampRateTab[idx]) {
				sampIdx = idx;
				break;
			}
		}

		if (idx == 12)
			return -4;

		chanNum = (config[4]&0x78) >> 3;
	}
	else
	{
		chanNum = (config[1]&0x78) >> 3;
	}

	if(chanNum > 7)
		return -5;

	adtsbuf[0] = 0xFF; adtsbuf[1] = 0xF9;

	adtsbuf[2] = (unsigned char)((object << 6) | (sampIdx << 2) | ((chanNum&4)>>2));

	framelen += 7;

	if(framelen > 0x1FFF)
		return -6;

	adtsbuf[3] = (chanNum << 6) | (framelen >> 11);
	adtsbuf[4] = (framelen & 0x7FF) >> 3;
	adtsbuf[5] = ((framelen & 7) << 5);// | 0x1F;
	adtsbuf[6] = 0;//0xFC;	

	*adtslen = 7;

	return 0;
}


#if _DUMP_AVTIMESTAMP
	FILE * m_hAudioTS;
	FILE * m_hVideoTS;
#endif //_DUMP_AVTIMESTAMP

#if _DUMP_H264FRAME
	FILE * m_hDumpStreams;
#endif //_DUMP_H264FRAME

};

#define ENABLE_DELAY 1




#endif //__MEDIASTREAM_H__

