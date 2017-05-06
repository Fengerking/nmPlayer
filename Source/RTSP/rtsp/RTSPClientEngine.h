#ifndef __RTSPCLIENTENGINE_H__
#define __RTSPCLIENTENGINE_H__
#include "network.h"
#include "RTSPSession.h"
#include "vostream_digest_auth.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef enum{
	RR_ID_UNKNOWN	    = -1,
	RR_ID_PLAY			=0,
	RR_ID_PAUSE			,
	RR_ID_KEEPALIVE		,
	RR_ID_NUMBER
}RR_ID;

typedef struct  
{
	int		cseq;
	long	sendTime;
}RequestInfo;
class CRTSPClientEngine;
class CASFHeadParser;
class CRTSPResponseHandler
{
	RR_ID		 m_id;
	int			 m_cseq;
	char		 *m_response;
	int			 m_sendTime;
	CRTSPClientEngine* m_engine;
	int			 m_nextTask;
public:
	CRTSPResponseHandler():m_id(RR_ID_UNKNOWN),m_cseq(0),m_response(0),m_sendTime(0),m_nextTask(0){}
	void SerHandlerParam(CRTSPClientEngine* engine,RR_ID id);
	void HandleResponse(char* response);
	void SetCSEQ(int cseq){m_cseq=cseq;}
	int	 GetCSEQ(){return m_cseq;}
	void SetSendTime(int time){m_sendTime=time;}
	int	 GetSendTime(){ return m_sendTime;}
	void Reset(){m_cseq=0;m_sendTime=0;m_nextTask=0;};
	void SetNextTask(int nextTask){m_nextTask=nextTask;}
	int	 GetNextTask(){return m_nextTask;}
};

class CMediaStream;
#define GET_RTSP_RESPONSE_VIATCP 1
#define RECVBUFSIZE	(1024*64)
#define KEEPALIVE_VIA_SET_PARAM 0
#ifdef HTC_PLAYLIST
#include "PlayListInterface_Vo.h"

#endif //HTC_PLAYLIST
class CRTSPSourceEX;
/**
\brief the engine core of the rtsp client

It handles the rtsp request/response,and manage the life time of sesstion and media stream
*/
class CRTSPClientEngine
{
public:
	CRTSPClientEngine(void);
	virtual ~CRTSPClientEngine(void);

public: // RTSP Command
	/**handle the request/response of options*/
	virtual bool OptionsCmd(char * rtspURL);
	/**handle the request/response of Describe*/
	virtual bool DescribeCmd(char * rtspURL);
	/**handle the request/response of Setup*/
	virtual bool SetupCmd(bool streamingOverTCP = false);
	virtual bool ReSetupCmd(bool streamingOverTCP=false);
	/**handle the request/response of Play*/
	virtual bool PlayCmd(float startTime,float endTime=0);
	/**handle the request/response of Pause*/
	virtual bool PauseCmd(int waitResponseBySec=0);
	/**handle the request/response of Teardown*/
	virtual bool TeardownCmd();
	/**handle the request/response of Get_Parameter*/
	virtual bool Get_Parameter_Cmd();
	/**handle the request/response of Set_Parameter*/
	virtual bool Set_Parameter_Cmd(int paramType,void* value);
	virtual bool SwitchChannel(char* newUrl);
	/**handle the request/response of GET for http tunnel*/
	virtual bool GetCmd();
	/**handle the request/response of POST for http tunnel*/
	virtual bool PostCmd();
	
#ifdef HTC_PLAYLIST
	bool Playlist_PlayCmd(char* strPlayList);
#endif //HTC_PLAYLIST
protected:
#define MAX_HANDLER 4
	CRTSPResponseHandler m_responsHandler[MAX_HANDLER];
public:
	bool    RTSPStreamsAvailable(unsigned int millisecond = 0);
	int     ReadRTSPStreams(int flag=0xffffffff);
	void    FlushRTSPStreams(int flag=0xffffffff);
	unsigned long HaveBufferedRTSPStreamDuration(int plusAntijitterTime=0);
	bool ParseSDP2(const char * sessionDescription);
	bool ParseSDP(const char * sessionDescription);
	void	HandlePlayResponse(char* response);
	void    HandlePauseResponse(char* response);  
	void    HandleKeepAliveResponse(char* response); 
	CRTSPResponseHandler* FindTheResponseHandler(char* response);
	void SetNextTaskOfResponse(int id,int task);
public:
	bool Init();
	void Reset();
	bool IsInited(){return m_rtspSession!=NULL;}
public:
	float          StreamingDuration();
	/**Get one mediastream by index*/
	CMediaStream * MediaStream(int streamIndex);
	/**Get the number of mediastream*/
	int	           MediaStreamCount();
	bool PauseLiveStream();
	bool ResumeLiveStream();
	void ResetPauseTime();
protected:
	virtual bool    SetupCmd_MediaStream(int streamIndex); 
	static	DWORD	ConnectThreadProc(void* pParam);
	int CleanURL(char* str2);
	bool	ConnectToServer();
	bool ConnectToServerOverHttpTunnel();
	char*   m_oriURL;
	char*	m_URLofRequest;
	bool	m_bTimeOut;
	int		m_rtspServerPort;
	//RequestInfo m_playRequest;
	RR_ID	m_lastResponse;
	bool	m_bSyncPlayResponse;//work around for Server: CarbonStreamingServer
public:
	//bool	IsPlayResponseTimeOut();
	//void	ResetPlayRequestInfo(){m_playRequest.cseq=m_playRequest.sendTime=0;}
	/**check if the rtsp response is timeout*/
	bool	IsRTSPResponseTimeOut();
	int		IsWaitingResponse();
	RR_ID	GetLastResponse(){return m_lastResponse;}
	void	SetLastResponse(RR_ID id){m_lastResponse=id;}
protected:
	static int HandleRTSPServerRequest(void * param);
	int        DoHandleRTSPServerRequest();
	int        HandleNotSupportRequest(char * CSeqValue);
	void       ResponseToServer(int respCode, const char * _respText);
protected:
	static int ReadStreamsOverTCP(void * param);
	virtual int        DoReadStreamsOverTCP(int flag=0);
	unsigned int IsReadyToParse(unsigned char * buf, unsigned int length);
	void		 ParseOneRTSPResp(unsigned char * buf, unsigned int length);
	int 		 ParseOneRTPInterleave(unsigned char * buf, unsigned int length);

protected:
	static int NotifyStreamingServerClientBeLiving(void * param);
	int        DoNotifyStreamingServerClientBeLiving(int waitResponseTimeOut=0,int cseq=0);
	long       m_taskToken;
	
protected:
	bool	ConnectToStreamingServer(char * rtspURL); 
	bool ConnectToStreamingServerOverHttpTunnel(char * rtspURL);
	bool	SendRTSPCommandToStreamingServer(const char * cmdString,int timeOutBySec=3);
	bool	SendRTSPCommandToStreamingServerOverHttpTunnel(const char * cmdString,int timeOutBySec=3);
	bool	RecvResponseFromStreamingServer(unsigned int & responseBytes,int cmd,int timeoutBySec=0);
	bool	RecvResponseFromStreamingServerOverHttpTunnel(unsigned int & responseBytes,int cmd,int timeoutBySec=0);
	bool    ParseRTCPInterval(char * responseText);
	bool	ParseResponseCode(char * responseText, unsigned int & responseCode);
	bool    ParsePublicAttribute(char * responseText);
	bool    ParseContentBase(char * responseText);
	bool    ParseSetupCMDResponse_Session(char * sdpParam);
	virtual bool    ParseSetupCMDResponse_Transport(char * sdpParam, int streamIndex);
	bool    ParseRTPInfoHeader(char * responseText);
	bool    ParsePlayRange(char * responseText,float* begin, float* end);
	void    parseExtraFiled();
	int		m_failConnectNum;
	bool	m_bReconnect;
public:
	bool	IsRTSPStreamingClosed(){return m_failConnectNum>=10;};
	bool    IsNeedReconnect(){return m_bReconnect;};
	void	SetReconnect(bool flag){m_bReconnect=flag;};
protected:
	char              * m_rtspURL;
	bool                m_streamingOverTCP;
	unsigned int	    m_rtspCSeq;
	char              * m_publicAttr;
	char              * m_rtspSessionID;
	char              * m_contentBase;
	int                 m_serverTimeoutValue;
	IVOSocket*			m_rtspSock;	
	IVOSocket*			m_rtspSock2;	

	struct sockaddr_storage * m_rtspServerAddr;
	struct sockaddr_storage * m_rtspServerAddrOverHttpTunnel;


	char              * m_recvBuf;
	char              * m_rtspCMDString;
	char              * m_userAgentAttr;
	char			  * m_UAProfile;
protected:
	CRTSPSession      * m_rtspSession;
	HANDLE			  m_hConnectThread;
	DWORD			  m_dwConnectThreadId; 
 
	unsigned int        m_streamLength;
	unsigned int        m_streamPos;
	int					m_errorCount;
public:
	int GetDropFrames(){return m_rtspSession->GetDropFrames();};
	bool	IsRTSPSocketExist(){return m_rtspSock>0;}
protected:
	float m_range;
	int m_pauseTime;
	int m_playTime;
	int SendExtraRTPPacket();
	int SendExtraRTCPPacket();
	bool IsKeepAcitveResponse();
	char* m_sdpFileString;
	//char* m_tmpString;
	int HandleResponseViaTCP();
	int GetTheSeqNum(char* response,int len);
public:
	void SetSDPFileString(char* string);
	float GetPlayRangeBegin(){return m_range;}
	void AddRTSPFields(unsigned int nRTSPMethod);
	void SetRTSPSource(CRTSPSourceEX * pRTSPSource);
	void CalculateNPTAfterPlayResponse();
	int	 GetActualBitrate();
	virtual CMediaStream* CreateOneMediaStream(CRTSPSession* session);
	
private:
	CRTSPSourceEX * m_pRTSPSource;
	void	WaitAndProcessResponse(int command);
	void UpdateKeepAlive(int timeBySec=60);
	int HandleServerRequest_OPTION();
	int HandleServerRequest_ANNOUNCE();
	void GenerateSessionCookie();
#if SUPPORT_NPT_FEEDBACK
	RangeInfo m_rangeInfo;
#endif//SUPPORT_NPT_FEEDBACK
#ifdef HTC_PLAYLIST
public:
	int HandleServerRequest_SET_PARAMETER();
	int HandleServerRequest_END_OF_STREAM();
public:
	
	void SetCustomNotifyInterface(ICustomNotifyInterface * pICustomNotify);

private:
	ICustomNotifyInterface * m_pICustomNotify;

#endif //HTC_PLAYLIST
public:
		CASFHeadParser* GetASFHeadParser(){return m_rtspSession->GetASFHeadParser();}
protected:
#define MAX_TMPLEN 4096
	char cmdFormatString[MAX_TMPLEN];
private:
	int m_bIfUAgent;
	bool m_bIfBandWidth;
	bool m_BIfWapProfile;

	voStreamDigestAuth *m_digestAuth;

	char				m_digestRealm[128];
	char				m_digestNonce[128];
	char 				m_digestResp[33];

	char				m_hostPath[MAX_PATH];
	char				m_fSessionCookie[33];
};

#ifdef _VONAMESPACE
}
#endif

#endif //__RTSPCLIENTENGINE_H__

