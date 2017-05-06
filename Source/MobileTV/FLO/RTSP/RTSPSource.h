#ifndef __RTSPSOURCE_H__
#define __RTSPSOURCE_H__

#include "voFLOEngine.h"

#include "network.h"

#include "voType.h"
#include "voOSFunc.h"
#include "voCMutex.h"
#include "voThread.h"

class CRTSPClientEngine;
class CMediaStream;
class CRTSPSource
{
public:
	CRTSPSource();
	~CRTSPSource();

public:
	VO_U32 Open(const char * pDataSource);
	VO_U32 Close();
	VO_U32 Start();
	VO_U32 Stop();
	VO_U32 GetParam(VO_U32 nParamID, VO_S32 * pParam);
	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pParam);

public:
	VO_U32 StartupRTSPSession();
	VO_U32 ShutdownRTSPSession();

	VO_U32 HandleOCC(const char * pDataSource);

	//VO_U32 HandleOCCIncompatibleTransport(const char * pDataSource);
	//VO_U32 StartupRTSPSessionForOCC(char * media_url2);

	VO_U32 HandleH264BitError();

public:
	static VO_U32 RecvRTSPStreamsProc(void * pParam);
	VO_U32 RecvRTSPStreams(void);
	
private:
	voCMutex         m_SessionMutex;
	voCMutex		 m_OpenMutex;
	voThreadHandle   m_hRecvThread;
	VO_U32		     m_nRecvThreadID;
	bool		     m_bStopRecv;
	int				 m_timeoutNum;
private:
	char 				     m_rtspURL[1024];
	
	CRTSPClientEngine      * m_RTSPClientEngine;

private:
	bool					m_bRTPTimeOut;
	VO_U32					m_nChannelChangeMode;
	enum{
		VIDEO_TRACK = 1,
		AUDIO_TRACK = 2,
	};
	CMediaStream*			GetMediaStream(int id=VIDEO_TRACK);
	void SetSessionStatus(Session_Status status);
	Session_Status  GetSessionStatus(){return CUserOptions::UserOptions.status;}
};



#endif //__RTSPSOURCE_H__
