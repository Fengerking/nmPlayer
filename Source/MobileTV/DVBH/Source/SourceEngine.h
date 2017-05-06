#ifndef __SOURCEENGINE_H__
#define __SOURCEENGINE_H__

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
#     include "netbase.h"
#elif defined(LINUX)
#     include "Lin2Win.h"
#     include <sys/types.h>
#     include <sys/socket.h>
#     include <sys/select.h>
#endif

#include "vortpsrc.h"
#include "list_T.h"

class CStreamChannel;
class CRTPSource;

class CSourceEngine
{
public:
	CSourceEngine(CRTPSource * pRTPSource);
	~CSourceEngine();

public:
	VORC_RTPSRC Start();
	VORC_RTPSRC Stop();

	VORC_RTPSRC SendData(VO_PBYTE pData, VO_U32 nSize);

public:
	void AddStreamChannel(CStreamChannel * pStreamChannel);
	int GetStreamChannelCount();
	CStreamChannel * SearchStreamChannel(SOCKET channelSock);
	CStreamChannel * SearchStreamChannel(unsigned short port);

protected:
	static DWORD ReadStreamChannelsProc(void * pParam);
	DWORD ReadStreamChannels();
protected:
	VORC_RTPSRC DoReadingStreamChannels();

protected:
	static DWORD ReadLocalFileProc(void * pParam);
	DWORD ReadLocalFile();
protected:
	VORC_RTPSRC DoReadingLocalFile();
private:
	FILE    * m_pFile;
	VO_BYTE * m_pData;

private:
	CRTPSource * m_pRTPSource;

	list_T<CStreamChannel *> m_listStreamChannel;

private:
	bool    m_stopReading;
	HANDLE	m_tHandle;
	DWORD	m_tID; 

private:
	fd_set  m_channelSockSet;
};


#endif //__SOURCEENGINE_H__