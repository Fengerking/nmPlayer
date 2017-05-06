#ifndef __RTSPCLIENTENGINE_H__
#define __RTSPCLIENTENGINE_H__

#include "network.h"

#include "RTSPSession.h"

#define RECVBUFSIZE	(1024*10)

class CMediaStream;
class CRTSPSource;

class CRTSPClientEngine
{
public:
	CRTSPClientEngine(void);
	~CRTSPClientEngine(void);

public:
	bool ConnectServer(char * rtspURL);
	bool OptionsCmd(char * rtspURL);
	bool DescribeCmd(char * rtspURL);
	bool SetupCmd();
	bool PlayCmd();
	bool TeardownCmd();

public:
	bool SetupCmd_MediaStream(int streamIndex); 
	void HandlePlayResponse(char* response);

	bool HandleOCC(const char * media_url2); //Optimized Channel Change with Compatible Transport
	bool HandleOCCIncompatibleTrans(char * media_url2); //Optimized Channel Change with Incompatible Transport

public:
	int ReadRTSPStreams(int flag = 0xffffffff);

	void FlushRTSPStreams(int flag = 0xffffffff);
	void HandleKeepAliveResponse(char* response);
	
public:
	bool Init();
	void Reset();
	void ResetForOCC();
	
	bool IsInited() { return m_bInit; }
	bool m_bInit;
	
public:
	CMediaStream * MediaStream(int streamIndex);
	int	MediaStreamCount();

protected:
	bool	ConnectToServer();
	
	const char*   m_oriURL;
	const char*	m_URLofRequest;
	bool	m_bTimeOut;
	int		m_rtspServerPort;

protected:
	static int HandleRTSPServerRequest(void * param);
	int        DoHandleRTSPServerRequest();
	int        HandleNotSupportRequest(char * CSeqValue);
	void       ResponseToServer(int respCode, const char * _respText);
	int SendExtraRTPPacket();
	int SendExtraRTCPPacket();
protected:

//	long       m_taskToken;
	
protected:
	bool	ConnectToStreamingServer(char * rtspURL); 
	bool	SendRTSPCommandToStreamingServer(const char * cmdString, int timeOutBySec=3);
	bool	RecvResponseFromStreamingServer(unsigned int & responseBytes, int cmd, int timeoutBySec = 0);
	int     CheckRTSPTimeout(int timeoutBySec);

	bool	ParseResponseCode(char * responseText, unsigned int & responseCode);
	bool    ParseContentBase(char * responseText);
	bool    ParseSetupCMDResponse_Session(char * sdpParam);
	virtual bool    ParseSetupCMDResponse_Transport(char * sdpParam, int streamIndex);

	
protected:
	char              * m_rtspURL;

	unsigned int	    m_rtspCSeq;
	char              * m_rtspSessionID;
	char              * m_contentBase;
	int                 m_serverTimeoutValue;
	

	IVOSocket*			m_rtspSock;	

	struct voSocketAddress * m_rtspServerAddr;

	char              * m_recvBuf;
	char              * m_rtspCMDString;
	char              * m_userAgentAttr;
	
protected:
	CRTSPSession      * m_rtspSession;

protected:
	int m_pauseTime;
	int m_playTime;
	

	int GetTheSeqNum(char* response,int len);
	
public:
	void SetRTSPSource(CRTSPSource * pRTSPSource);
	
private:
	CRTSPSource * m_pRTSPSource;
	
	int HandleServerRequest_OPTION();
	int HandleServerRequest_ANNOUNCE();



private:
#if _DUMP_RTSP
	FILE    * m_hLogRTSP;
#endif //_DUMP_RTSP

};

#endif //__RTSPCLIENTENGINE_H__
