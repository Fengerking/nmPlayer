#pragma once
#include "httpcmdhdl.h"
typedef struct  
{
	char* begin;
	int	  currentSize;
	int	  dataSize;
	int	  paddingSize;
	int	  packNum;
}TASFPacket;
class CWMSHttpCmdHandle :
	public CHttpCmdHandle
{
	VONETDOWNTYPE m_netDownType;
	int			  m_counts;
	long		  m_clientID;
	long		  m_playListID;
	int			  m_remainedDataLen;
	int			  m_foundHead;
	CHttpGet*	  m_pGet;
	bool FindHead(char* str,char* end);
	bool FindMeta(char* str,char* end);
	void Play();
	int CheckSocketActivity();
	int m_headSize;
	char* m_curPackPos;
	int   m_prevRemainedSize;
	int	  m_prevLastCopySize;
	int	  m_contentLength;
	int		m_packSize;
	int	  m_loopNum;
	char* m_headBuf;
	TASFPacket m_pack;
	typedef enum
	{
		STATUS_STOP			=0,
		STATUS_SETUP		=1,
		STATUS_PLAY_REQUEST	=2,
		STATUS_PLAY			=3,
		STATUS_PLAY_FAIL = 4,
	}WMS_STATUS;
	WMS_STATUS m_status;
public:
	CWMSHttpCmdHandle();
	virtual ~CWMSHttpCmdHandle();

public:
	virtual HTTPGETRETURNVALUE	CMD_GET(CHttpGet* pGet, const char* szHost, const char* szObject, const char* szUserAgent, const char* szRange, bool bUseProxy,int flag=0);
	virtual bool	ParseContentLength(unsigned int* _contentLength);

	virtual HTTPGETRETURNVALUE	RecvResponse(CHttpGet* pGet);
	virtual VONETDOWNTYPE	GetNetDownType(){return m_netDownType;};
	virtual int		GetRemainedDataInCMDBuf(char* p);
	virtual bool	ParseStatusCode(unsigned int* _statusCode);
	virtual HTTPGETRETURNVALUE		Receive(CHttpGet* pGet,void* pBuffer, int nToRecv, int* pnRecved);
	virtual HTTPGETRETURNVALUE		ReceiveTest(CHttpGet* pGet);
};
