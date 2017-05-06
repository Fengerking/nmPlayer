	
#include "stdlib.h"
#include "string.h"
#include "CRTSPSource.h"
#include "voStreaming.h"
CRTSPSource2::CRTSPSource2()
:m_hSource (NULL)
	, m_nTracks (0)
	, m_ppTrack (NULL)
	
{
	m_fRTSPOpenURL = NULL;
	m_fRTSPCreate = NULL;
	m_fRTSPOpen = NULL;
	InitDll ();
	if(m_fRTSPCreate)
		m_fRTSPCreate(&m_hSource);
	
}

CRTSPSource2::~CRTSPSource2(void)
{
	Close ();
}
static char szFile[1024];
bool CRTSPSource2::OpenSource (const char * pFile)
{
	if (m_fRTSPCreate==NULL||m_fRTSPOpen == NULL)
		return false;
	strcpy (szFile, pFile);
	VOStreamInitParam m_initParam;
	memset(&m_initParam,0,sizeof(m_initParam));
	m_initParam.nMaxUdpPort		= DEFAULT_PORT_MAX;
	m_initParam.nMinUdpPort		= DEFAULT_PORT_MIN;
	m_initParam.nRTPTimeOut		= DEFAULT_RTPTIMEOUT;
	m_initParam.nBufferTime		= 3;//DEFAULT_BUFFERTIME;
	m_initParam.nRTCPInterval	= 5000;
	m_initParam.nFirstIFrame	= 1;
	m_initParam.nExtraRTPPacket	= 1;
	m_initParam.nConnectTimeout	= DEFAULT_RTPTIMEOUT;
	m_initParam.nRTSPTimeout	= DEFAULT_RTPTIMEOUT;
	m_initParam.nSDKlogflag = 2;
	m_initParam.nUseTCP = 0;
	RTSPSetParam(VOID_STREAMING_INIT_PARAM,(LONG)&m_initParam);
	long rc;
	if(m_fRTSPOpenURL)
	{
		//DumpLog(_T("filter2.txt"),"before new m_fRTSPOpenURL\n");
		rc = m_fRTSPOpenURL(m_hSource, szFile, &m_opFile);
	}
	else
	{
		rc = m_fRTSPOpen (&m_hSource, szFile, &m_opFile);
	}
	if (rc != VORC_FILEREAD_OK || m_hSource == NULL)
		return false;
	rc = RTSPGetParam (VOID_FILEREAD_TRACKS, (LONG *)&m_nTracks);
	if (rc != VORC_FILEREAD_OK || m_nTracks <= 0)
		return false;
	
	strcpy(m_initParam.szUserAgent,"VisualOn Streaming Player 2.2");
	m_ppTrack = new HVORTSPTRACK [m_nTracks];
	memset(m_TrackInfo,0,sizeof(m_TrackInfo));
	memset(m_samples,0,sizeof(m_samples));
	HVOFILETRACK	pTrack = NULL;
	VOTRACKINFO		infoTrack;

	//char* tmp = new char[12];
	int nTrackIndex=0;
#define MAX_BUFSIZE 128*1024
	for (;nTrackIndex < m_nTracks; nTrackIndex++)
	{
		m_ppTrack[nTrackIndex] = NULL;

		rc = m_fTrackOpen (&pTrack, m_hSource, nTrackIndex);
		if (pTrack == NULL)
			continue;
		m_ppTrack[nTrackIndex] = pTrack;
		m_samples[nTrackIndex].pBuffer = new unsigned char[MAX_BUFSIZE];
		rc = TrackGetInfo (pTrack, &infoTrack);
		if(VORC_FILEREAD_OK != rc)
			continue;

		if (infoTrack.uType == TT_RTSP_AUDIO)
		{
			m_TrackInfo[nTrackIndex].Type=kMediaType_Audio;
			VOCODECAUDIOFORMAT	infoAudio;
			int dwRC = TrackGetParam (pTrack, VOID_COM_AUDIO_FORMAT, (LONG *)&infoAudio);
			if (dwRC != VORC_FILEREAD_OK)
				return false;

			sampleRate = infoAudio.sample_rate;
			channels = infoAudio.channels;
		}
		else if (infoTrack.uType == TT_RTSP_VIDEO)
		{
			m_TrackInfo[nTrackIndex].Type=kMediaType_Video;
			VOCODECVIDEOFORMAT	infoVideo;
			int dwRC = TrackGetParam (pTrack, VOID_COM_VIDEO_FORMAT, (LONG *)&infoVideo);
			if (dwRC != VORC_FILEREAD_OK)
				return false;
			width = infoVideo.width;
			height = infoVideo.height;
			VOCODECDATABUFFER	buffer;
			buffer.buffer = h264Head;
			buffer.length = 512;

			dwRC = TrackGetParam (pTrack, VOID_COM_HEAD_DATA, (LONG *)&buffer);
			sps.buffer = buffer.buffer+2;
			sps.length = (buffer.buffer[0]<<8)|buffer.buffer[1];
			pps.buffer = sps.buffer+sps.length+2;
			pps.length = (pps.buffer[-2]<<8)|pps.buffer[-1];
		}	
		LONG value;
		
		TrackGetParam(pTrack,VOID_FILEREAD_CODEC,&value);
		switch (value)
		{
		case VC_MPEG4:
			m_TrackInfo[nTrackIndex].Format = kMediaFormat_MP4V;
			break;
		case VC_H264:
			m_TrackInfo[nTrackIndex].Format = kMediaFormat_H264;
			break;
		case AC_AAC:
			m_TrackInfo[nTrackIndex].Format = kMediaFormat_MP4A;
			break;
		case AC_AMR:
			m_TrackInfo[nTrackIndex].Format = kMediaFormat_AMR;
			break;
		}
		MK_Time duration;
		TrackGetParam(pTrack,VOID_COM_DURATION,(LONG*)&duration);
		m_TrackInfo[nTrackIndex].BeginTime	=	0;
		m_TrackInfo[nTrackIndex].EndTime		=	duration;
	}
	return true;
}

bool CRTSPSource2::Close (void)
{
	if (m_hSource == NULL || m_fRTSPClose == NULL)
		return true;

	
	int dwRC = m_fRTSPClose (m_hSource);
	for (int nTrackIndex=0;nTrackIndex < m_nTracks; nTrackIndex++)
	{
		delete[] m_samples[nTrackIndex].pBuffer;
	}
	delete[] m_ppTrack;
	m_hSource = NULL;
	return true;
}

bool CRTSPSource2::Stop (void)
{
	long dwRC = RTSPSetParam (VOID_FILEREAD_ACTION_STOP, 0);
	return dwRC == VORC_OK;;
}

bool CRTSPSource2::Pause (void)
{
	long dwRC = RTSPSetParam (VOID_FILEREAD_ACTION_PAUSE, 0);
	return dwRC == VORC_OK;;
}

bool CRTSPSource2::Run (void)
{
	long dwRC = RTSPSetParam (VOID_FILEREAD_ACTION_RUN, 0);
	return dwRC == VORC_OK;;
}

bool CRTSPSource2::Start (void)
{
	long dwRC = RTSPSetParam (VOID_FILEREAD_ACTION_START, 0);
	return dwRC == VORC_OK;
}

bool CRTSPSource2::SetPos (long llStart, long llEnd)
{

	unsigned long lSeekPos = (unsigned long)(llStart);
	long rc = RTSPSetParam(VOID_FILEREAD_ACTION_SEEK, lSeekPos);
	if(rc != VORC_FILEREAD_OK)
		return false;

	return true;
}




long CRTSPSource2::RTSPSetParam (LONG nID, LONG lValue)
{
	if (m_fRTSPSetParam == NULL || m_hSource == NULL)
		return (long)VORC_FAIL;
	long dwRC = m_fRTSPSetParam (m_hSource, nID, lValue);

	return dwRC;
}

long CRTSPSource2::RTSPGetParam (LONG nID, LONG * plValue)
{
	if (m_fRTSPGetParam == NULL || m_hSource == NULL)
		return (long)VORC_FAIL;

	long dwRC = m_fRTSPGetParam (m_hSource, nID, plValue);

	return dwRC;
}

long CRTSPSource2::TrackSetParam (HVORTSPTRACK pRTSPTrack, LONG nID, LONG lValue)
{
	if (m_fTrackSetParam == NULL || m_hSource == NULL)
		return (long)VORC_FAIL;

	long dwRC = m_fTrackSetParam (m_hSource, pRTSPTrack, nID, lValue);
	
	return dwRC;
}

long CRTSPSource2::TrackGetParam (HVORTSPTRACK pRTSPTrack, LONG nID, LONG * plValue)
{
	if (m_fTrackGetParam == NULL || m_hSource == NULL)
		return (long)VORC_FAIL;

	long dwRC = m_fTrackGetParam (m_hSource, pRTSPTrack, nID, plValue);

	return dwRC;
}

long CRTSPSource2::TrackGetInfo (HVORTSPTRACK pRTSPTrack, VOTRACKINFO * pTrackInfo)
{
	if (m_fTrackGetInfo == NULL || m_hSource == NULL)
		return (long)VORC_FAIL;

	long dwRC = m_fTrackGetInfo (m_hSource, pRTSPTrack, pTrackInfo);

	return dwRC;
}

long CRTSPSource2::TrackGetSampleByIndex (HVORTSPTRACK pRTSPTrack, VOSAMPLEINFO * pSampleInfo)
{ 
	if (m_fGetSampleByIndex == NULL || m_hSource == NULL)
		return (long)VORC_FAIL;
	//if(m_filter->IsBuffering())
	//	return VORC_FILEREAD_NEED_RETRY;
	long dwRC = m_fGetSampleByIndex (m_hSource, pRTSPTrack, pSampleInfo);

	return dwRC;
}

int CRTSPSource2::TrackGetNextKeySample (HVORTSPTRACK pRTSPTrack, int nIndex, int nDirectionFlag)
{
	if (m_fGetNextKeyFrame == NULL || m_hSource == NULL)
		return (long)VORC_FAIL;
	//if(m_filter->IsBuffering())
	//	return VORC_FILEREAD_NEED_RETRY;
	long dwRC = m_fGetNextKeyFrame (m_hSource, pRTSPTrack, nIndex, nDirectionFlag);

	return dwRC;
}

bool CRTSPSource2::InitDll (void)
{

	m_fRTSPCreate = (VORTSPCREATE)voRTSPSessionCreate;
	m_fRTSPOpenURL = (VORTSPOPENURL)voRTSPSessionOpenURL;
	m_fRTSPOpen = (VORTSPOPEN)voRTSPSessionOpen;
	m_fRTSPClose = (VORTSPCLOSE)voRTSPSessionClose;
	m_fRTSPSetParam = (VORTSPSETPARAMETER)voRTSPSessionSetParameter;
	m_fRTSPGetParam = (VORTSPGETPARAMETER)voRTSPSessionGetParameter;

	m_fTrackOpen = (VOTRACKOPEN)voRTSPTrackOpen;
	m_fTrackClose = (VOTRACKCLOSE)voRTSPTrackClose;
	m_fTrackSetParam = (VOTRACKSETPARAMETER)voRTSPTrackSetParameter;
	m_fTrackGetParam = (VOTRACKGETPARAMETER)voRTSPTrackGetParameter;
	m_fTrackGetInfo = (VOTRACKGETINFO)voRTSPTrackInfo;
	m_fGetSampleByIndex = (VOTRACKGETSAMPLEBYINDEX) voRTSPTrackGetSampleByIndex;
	m_fGetSampleByTime = NULL;//(VOTRACKGETSAMPLEBYTIME) voRTSPTrackGetSampleByTime;
	m_fGetNextKeyFrame = (VOTRACKGETNEXTKEYFRAME) voRTSPTrackGetNextKeyFrame;
	

	return true;
}






	

	/*
	* Allocate and initialize a MK_Player instance using aCtrl as the driving event
	* controller (must be provided externally).
	*/
   static MK_Player* mkPlayer;
	MK_Player* MK_Player_Create(void)
	{
		return mkPlayer=new CRTSPSource2();
	}

	/*
	* Destroy the provided MK_Player instance and free all related resources.
	*
	* NOTE: Does not destroy the MK_EvtCtrl passed in Create().
	*/
	void MK_Player_Destroy(MK_Player* aPlay)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		delete handle;
		mkPlayer=NULL;
		handle=0;
	}

	/******************************************************************************/

	/*
	* Set the interface to use when communicating over the network. The default is
	* to use a wildcard interface (which basically leaves the choise to the network
	* stack). This function currently only accepts IP-numbers as interface names.
	*
	* NOTE: To return to the default beahivour of using a wildcard interface just
	*       use an empty string (or NULL) as argument.
	*/
	MK_Result MK_Player_Net_SetLocalIF(MK_Player* aPlay, const MK_Char* aName)
	{
		return 0;
	}

	/*
	* Set the range of UDP port numbers from which ports are selected when creating
	* listening UDP sockets (like in RTP over UDP transport).
	*
	* NOTE: The default range is the IANA open range: 49152-65535
	*/
	MK_Result MK_Player_Net_SetUDPPortRange(MK_Player* aPlay, MK_U16 aFrom, MK_U16 aTo)
	{
		return 0;
	}
	/*
	* Set the timeout value for network wait-operations (like connection
	* establishment and waiting for incoming data).
	*/
	MK_Result MK_Player_Net_SetTimeout(MK_Player* aPlay, MK_U32 aMSec)
	{
		return 0;
	}

	/******************************************************************************/

	/*
	* Set the MobiDRM private key. To disable key pass a NULL key and/or a 0 size.
	*/
	MK_Result MK_Player_MobiDRM_SetKey(MK_Player* aPlay, const void* aKey, MK_U32 aSz)
	{
		return 0;
	}

	/*
	* Set the active MobiDRM license. To disable license pass a NULL license and/or
	* a 0 size. The time in aNow should be a securely acquired UNIX timestamp
	* representing the current time (for expiration validation).
	*/
	MK_Result MK_Player_MobiDRM_SetLicense(MK_Player* aPlay, const void* aLic, MK_U32 aSz, MK_U32 aNow)
	{
		return 0;
	}

	/******************************************************************************/

	/*
	* Set HTTP proxy to use by providing a hostname/IP and a port. To disable proxy
	* pass a NULL host and/or a 0 port.
	*/
	MK_Result MK_Player_HTTP_SetProxy(MK_Player* aPlay, const MK_Char* aHost, MK_U16 aPort)
	{
		return 0;
	}

	/*
	* Set some values to modify the User-Agent string provided by the HTTP stack
	* using this format:
	*
	* User-Agent: <aName>/<aVer> (MediaKit/<build-id>/<platform>; Device/<aDev>; <aExtra>)
	*
	* If NULL is passed in any argument the currently set value will be used.
	*/
	MK_Result MK_Player_HTTP_SetUserAgentVals(MK_Player* aPlay, const MK_Char* aName, const MK_Char* aVer, const MK_Char* aDev, const MK_Char* aExtra)
	{
		return 0;
	}

	/******************************************************************************/

	/*
	* Toggle initial hole-punching on or off (default is on) for RTP over UDP.
	*/
	MK_Result MK_Player_RTP_ToggleInitialHolePunch(MK_Player* aPlay, MK_Bool aOn)
	{
		return 0;
	}

	/*
	* Set the initial RTCP RR interval (the default is 5000). The value actually
	* used by the RTP stack may wary based on the number of participants in some
	* cases (as per RFC3550). A value of 0 disables periodic RTCP RR.
	*/
	MK_Result MK_Player_RTCP_SetInitialRRInterval(MK_Player* aPlay, MK_U32 aMSec)
	{
		return 0;
	}

	/*
	* Toggle periodic hole-punching on RTP channel when sending RTCP on or off
	* (default is on).
	*/
	MK_Result MK_Player_RTCP_ToggleHolePunchWithRR(MK_Player* aPlay, MK_Bool aOn)
	{
		return 0;
	}

	/******************************************************************************/

	/*
	* Set how often the RTSP stack will ping the server to keep the connection
	* alive (default is 30000). A value of 0 disables keep-alive.
	*/
	MK_Result MK_Player_RTSP_SetPingInterval(MK_Player* aPlay, MK_U32 aMSec)
	{
		return 0;
	}

	/*
	* Set RTSP proxy to use by providing a hostname/IP and a port. To disable proxy
	* pass a NULL host and/or a 0 port.
	*/
	MK_Result MK_Player_RTSP_SetProxy(MK_Player* aPlay, const MK_Char* aHost, MK_U16 aPort)
	{
		return 0;
	}

	/*
	* Set the RTP transport prioritization preferences (i.e. provide a list of
	* enabled RTP transports ordered by preference). To revert to the default
	* behaivour pass NULL and/or 0 as argument values (default is: UDP, IRTP).
	*/
	MK_Result MK_Player_RTSP_SetRTPTransports(MK_Player* aPlay, const MK_RTPTransport aTpt[], MK_U32 aN)
	{
		return 0;
	}

	/*
	* Set some values to modify the User-Agent string provided by the RTSP stack
	* using this format:
	*
	* User-Agent: <aName>/<aVer> (MediaKit/<build-id>/<platform>; Device/<aDev>; <aExtra>)
	*
	* If NULL is passed in any argument the currently set value will be used.
	*/
	MK_Result MK_Player_RTSP_SetUserAgentVals(MK_Player* aPlay, const MK_Char* aName, const MK_Char* aVer, const MK_Char* aDev, const MK_Char* aExtra)
	{
		return 0;
	}

	/******************************************************************************/

	/*
	* Set the buffer limits for the internal media buffers. The Min value guides
	* how much buffer should be accumulated before starting playback, the Low value
	* guides when a playing buffer should be considered starved and the Max value
	* guides when a buffer shold be considered full/overflowing.
	*/
	//MK_Result MK_Player_Media_SetBufferLimits(MK_Player* aPlay, MK_U32 aMinMSec, MK_U32 aLowMSec, MK_U32 aMaxMSec);
	

	/******************************************************************************/

	/*
	* Open the specified URL for playback. After the call MediaInfo and TrackInfo
	* will be available and track selection should be done (by default all tracks
	* are disabled) before issuing Play().
	*
	* NOTE: May be called even though no Close() call has been made since the last
	*       Open() call. This allows MediaKit to perform some clip-switching
	*       optimizations in certain cases.
	*/
	MK_Result MK_Player_Open(MK_Player* aPlay, const MK_Char* aURL)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		if (handle)
		{
			if(handle->OpenSource(aURL))
				return 0;
		}
		return -1;
	}

	/*
	* Close the current media session. If aFull is MK_TRUE a more complete close
	* operation is performed and the currently active media manager is released
	* instead of (possibly) being reused.
	*
	* NOTE: Does not reset the configuraion information. For a complete reset the
	*       MK_Player instance should be destroyed and re-created.
	*/
	MK_Result MK_Player_Close(MK_Player* aPlay, MK_Bool aFull)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		if (handle)
		{
			handle->Close();
		}
		return 0;
	}

	/******************************************************************************/

	/*
	* Set the error callback function. The aObj pointer is a user provided value
	* which will be available in the callback to aFunc.
	*
	* NOTE: This callback is preserved for the lifetime of the MK_Player instance
	*       or a new error callback is set.
	*/
	MK_Result MK_Player_SetErrorFunc(MK_Player* aPlay, MK_ErrorFunc aFunc, void* aObj)
	{
		return 0;
	}

	/*
	* Get the current media information. This information will be available if an
	* URL has been opened successfully.
	*
	* NOTE: The pointer returned should not be kept around after the desired
	*       information has been acquired since other calls to MK_Player may
	*       invalidate it.
	*/
	const MK_MediaInfo* MK_Player_GetMediaInfo(const MK_Player* aPlay)
	{
		return 0;
	}

	

	/******************************************************************************/

	/*
	* Get the number of track in the currently opened URL resource.
	*/
	MK_U32 MK_Player_GetTrackCount(const MK_Player* aPlay)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		if (handle)
		{
			return handle->GetTrackCount();
		}
	}

	/*
	* Get track information about the track indicated by aIdx.
	*
	* NOTE: The pointer returned should not be kept around after the desired
	*       information has been acquired since other calls to MK_Player may
	*       invalidate it.
	*/
	const MK_TrackInfo* MK_Player_GetTrackInfo(const MK_Player* aPlay, MK_U32 aIdx)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		if (handle)
		{
			return handle->GetTrackInfo(aIdx);
		}
		return 0;
	}

	/*
	* Get the current TrackState. See the documentation for MK_TrackState for more
	* information. If aRsn is non-NULL the reason for being in the current state
	* is returned.
	*/
	//MK_TrackState MK_Player_GetTrackState(const MK_Player* aPlay, MK_U32 aIdx, MK_StateReason* aRsn);

	/*
	* Set the TrackState change callback function. The aObj pointer is a user
	* provided value which will be available in the callback to aFunc.
	*
	* NOTE: This callback is preserved for the lifetime of the track (i.e. to the
	*       next Open() call).
	*/
	//MK_Result MK_Player_SetTrackStateFunc(MK_Player* aPlay, MK_U32 aIdx, MK_TrackStateFunc aFunc, void* aObj);

	/*
	* Enable playback of the track indicated by aIdx. This is currently only
	* allowed between an Open() and a Play() call (to select which tracks to play).
	*/
	MK_Result MK_Player_EnableTrack(MK_Player* aPlay, MK_U32 aIdx)
	{
		return 0;
	}

	/*
	* Get the next sample for a track. May return NULL if the track is in a
	* non-playing state or is buffer-starved.
	*/
	MK_Sample* MK_Player_GetTrackSample(MK_Player* aPlay, MK_U32 aIdx)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		if (handle)
		{
			return handle->GetTrackSample(aIdx);
		}
		return NULL;
	}

	/******************************************************************************/

	/*
	* Start playback of the enabled media tracks.
	*/
	MK_Result MK_Player_Play(MK_Player* aPlay)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		if (handle)
		{
			return handle->Run();
		}
	}
//#ifdef VORTSPIMP
#define MK_TrackInfo_GetAudioChannels MK_TrackInfo_GetAudioChannels2
#define MK_TrackInfo_GetAudioSampleRate MK_TrackInfo_GetAudioSampleRate2
#define MK_TrackInfo_GetH264ConfPPSCount MK_TrackInfo_GetH264ConfPPSCount2
#define MK_TrackInfo_GetH264ConfSPSCount MK_TrackInfo_GetH264ConfSPSCount2
#define MK_TrackInfo_GetVideoHeight MK_TrackInfo_GetVideoHeight2
#define MK_TrackInfo_GetVideoWidth MK_TrackInfo_GetVideoWidth2

	/*
	* Returns the width of the video. A negative value is return if the width is
	* unknown.
	*/
	MK_S32 MK_TrackInfo_GetVideoWidth(const MK_TrackInfo* aInfo)
	{
		CRTSPSource2* handle=(CRTSPSource2*)mkPlayer;
		if (handle)
		{
			return handle->GetWidth();
		}
	}

	/*
	* Returns the height of the video. A negative value is return if the height is
	* unknown.
	*/
	MK_S32 MK_TrackInfo_GetVideoHeight(const MK_TrackInfo* aInfo)
	{
		CRTSPSource2* handle=(CRTSPSource2*)mkPlayer;
		if (handle)
		{
			return handle->GetHeight();
		}
	}

	/******************************************************************************/

	/*
	* Return the number of SequenceParameterSets available for the H264 track.
	*/
	MK_U32 MK_TrackInfo_GetH264ConfSPSCount(const MK_TrackInfo* aInfo)
	{
		return 1;
	}
	MK_S32 MK_Player_Wait(MK_Player* aPlay, MK_U32 aMSec)
	{
		return 0;
	}
	MK_Result MK_Player_SetPlayStateFunc(MK_Player* aPlay, MK_PlayStateFunc aFunc, void* aObj)
	{
		return 0;
	}
	MK_Result MK_Player_SetTrackStateFunc(MK_Player* aPlay, MK_U32 aIdx, MK_TrackStateFunc aFunc, void* aObj)
	{
		return 0;
	}

	/*
	* Get a SequenceParameterSet for the H264 track by index. Returns the length
	* required for a full copy of the SPS to be returned (even if it is larger than
	* aSz). Returns 0 if there were no PPS at the given index.
	*/
	MK_U32 MK_TrackInfo_GetH264ConfSPS(const MK_TrackInfo* aInfo, MK_U32 aIdx, MK_U8* aBuf, MK_U32 aSz)
	{
		CRTSPSource2* handle=(CRTSPSource2*)mkPlayer;
		if (handle)
		{
			return handle->GetSPS(aBuf,aSz);
		}
	}
	/*
	* Return the number of PictureParameterSets available for the H264 track.
	*/
	MK_U32 MK_TrackInfo_GetH264ConfPPSCount(const MK_TrackInfo* aInfo)
	{
		return 1;
	}

	/*
	* Get a PictureParameterSet for the H264 track by index. Returns the length
	* required for a full copy of the PPS to be returned (even if it is larger than
	* aSz). Returns 0 if there were no PPS at the given index.
	*/
	MK_U32 MK_TrackInfo_GetH264ConfPPS(const MK_TrackInfo* aInfo, MK_U32 aIdx, MK_U8* aBuf, MK_U32 aSz)
	{
		CRTSPSource2* handle=(CRTSPSource2*)mkPlayer;
		if (handle)
		{
			return handle->GetPPS(aBuf,aSz);
		}
	}
	/*
	* Perform a seek operation inside the current media resource. Seeks to the time
	* specified by aTime. If the call succeeds aTime will contain the time which
	* was actually the result of the seek operation (which may differ from the
	* requested time).
	*/
	
	MK_S32 MK_TrackInfo_GetAudioSampleRate(const MK_TrackInfo* aInfo)
	{
		{
			CRTSPSource2* handle=(CRTSPSource2*)mkPlayer;
			if (handle)
			{
				return handle->GetSampleRate();
			}
		}
	}

	MK_S32 MK_TrackInfo_GetAudioChannels(const MK_TrackInfo* aInfo)
	{
		CRTSPSource2* handle=(CRTSPSource2*)mkPlayer;
		if (handle)
		{
			return handle->GetChannelNum();
		}
	}
	MK_Result MK_Player_Seek(MK_Player* aPlay, MK_Time* aTime)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		if (handle)
		{
			return handle->SetPos((long)aTime,0);
		}
	}

	/*
	* Pause playback.
	*/
	MK_Result MK_Player_Pause(MK_Player* aPlay)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		if (handle)
		{
			return handle->Pause();
		}
	}

	/*
	* Resume playback.
	*/
	MK_Result MK_Player_Resume(MK_Player* aPlay)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		if (handle)
		{
			return handle->Run();
		}
	}

	/*
	* Stop playback. Much like Pause() in functionality but assumes playback will
	* not resume at the currently playing time.
	*/
	MK_Result MK_Player_Stop(MK_Player* aPlay)
	{
		CRTSPSource2* handle=(CRTSPSource2*)aPlay;
		if (handle)
		{
			return handle->Stop();
		}
	}

	/******************************************************************************/

	/*
	* NOTE: The API below is what is used to drive the Player by waiting for and
	*       handling different types of events. This can be used in a
	*       single-threaded poll-loop approach or a thread can be spun off to loop
	*       around MK_Player_Wait().
	*/

	/******************************************************************************/

	/*
	* Wait a maximum of aMSec (0 for a poll) millisecond for one or more events to
	* occur. Returns a value > 0 if events were processed, 0 if the timeout expired
	* and < 0 if an error occured:
	*
	*   MK_E_INTR    - The wait was interrupted
	*   MK_E_FAIL    - Unspecified error (triggers LOGERR)
	*/
	//MK_S32 MK_Player_Wait(MK_Player* aPlay, MK_U32 aMSec);

	/*
	* Interrupt any MK_Player_Wait() call in progress. If a wait call was
	* interrrupted it will return with the error MK_E_INTR (though this error may
	* also be returned for other interruptions).
	*/
	//void MK_Player_Interrupt(MK_Player* aPlay);

	/******************************************************************************/


