#include "RTPSource.h"
#include "StreamChannel.h"
#include "SourceEngine.h"

CSourceEngine::CSourceEngine(CRTPSource * pRTPSource)
: m_pRTPSource(pRTPSource)
, m_stopReading(true)
, m_tHandle(NULL)
, m_tID(0)
{
	FD_ZERO(&m_channelSockSet);
}

CSourceEngine::~CSourceEngine()
{
	list_T<CStreamChannel *>::iterator iter;
	for(iter=m_listStreamChannel.begin(); iter!=m_listStreamChannel.end(); ++iter)
	{
		delete *iter;
	}
	m_listStreamChannel.clear();

	if(m_tHandle != NULL)
	{
		CloseHandle(m_tHandle);
		m_tHandle = NULL;
		m_tID = 0;
	}

	FD_ZERO(&m_channelSockSet);
}

VORC_RTPSRC CSourceEngine::Start()
{
	VORC_RTPSRC rc = VORC_RTPSRC_OK;

	list_T<CStreamChannel *>::iterator iter;
	for(iter=m_listStreamChannel.begin(); iter!=m_listStreamChannel.end(); ++iter)
	{
		rc = ((CStreamChannel *)(*iter))->Start();
		if(rc != VORC_RTPSRC_OK)
			return rc;
	}

#if 0
	m_stopReading = false;
	m_tHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadStreamChannelsProc, this, 0, &m_tID);
#endif

#if 1
	m_stopReading = false;
	m_tHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadLocalFileProc, this, 0, &m_tID);
#endif

	return VORC_RTPSRC_OK;
}

VORC_RTPSRC CSourceEngine::Stop()
{
	m_stopReading = true;

	DWORD dwWaitResult = WaitForSingleObject(m_tHandle, 10000); 

	if(m_tHandle != NULL)
	{
		CloseHandle(m_tHandle);
		m_tHandle = NULL;
		m_tID = 0;
	}

	VORC_RTPSRC rc = VORC_RTPSRC_OK;

	list_T<CStreamChannel *>::iterator iter;
	for(iter=m_listStreamChannel.begin(); iter!=m_listStreamChannel.end(); ++iter)
	{
		rc = ((CStreamChannel *)(*iter))->Stop();
		if(rc != VORC_RTPSRC_OK)
			return rc;
	}

	return VORC_RTPSRC_OK;
}

VORC_RTPSRC CSourceEngine::SendData(VO_PBYTE pData, VO_U32 nSize)
{
	const VO_PBYTE pIPHeader = pData;
	const VO_PBYTE pUDPHeader = pIPHeader + 20;

	unsigned short SrcPort = (ntohl(*(u_long *)(pUDPHeader)) & 0xFFFF0000) >> 16;
	unsigned short DestPort = ntohl(*(u_long *)(pUDPHeader)) & 0x0000FFFF;
	unsigned short Length = (ntohl(*(u_long *)(pUDPHeader+4)) & 0xFFFF0000) >> 16;
//	unsigned short Checksum = 0;

	CStreamChannel * pStreamChannel = SearchStreamChannel(DestPort);
	if(pStreamChannel != NULL)
	{
		VO_PBYTE pRTPData = pUDPHeader + 8;
		VO_U32 nRTPLength = Length - 8;
		pStreamChannel->OnPacketArrived(pRTPData, nRTPLength);
	}

	return VORC_RTPSRC_OK;
}

void CSourceEngine::AddStreamChannel(CStreamChannel * pStreamChannel)
{
	m_listStreamChannel.push_back(pStreamChannel);

	SOCKET channelSock = pStreamChannel->GetChannelSocket();
	FD_SET(channelSock, &m_channelSockSet);
}

int CSourceEngine::GetStreamChannelCount()
{
	return m_listStreamChannel.size();
}

CStreamChannel * CSourceEngine::SearchStreamChannel(SOCKET channelSock)
{
	list_T<CStreamChannel *>::iterator iter;
	for(iter=m_listStreamChannel.begin(); iter!=m_listStreamChannel.end(); ++iter)
	{
		CStreamChannel * pStreamChannel = (CStreamChannel *)(*iter);
		if(channelSock == pStreamChannel->GetChannelSocket())
			return pStreamChannel;
	}

	return NULL;
}

CStreamChannel * CSourceEngine::SearchStreamChannel(unsigned short port)
{
	list_T<CStreamChannel *>::iterator iter;
	for(iter=m_listStreamChannel.begin(); iter!=m_listStreamChannel.end(); ++iter)
	{
		CStreamChannel * pStreamChannel = (CStreamChannel *)(*iter);
		if(port == pStreamChannel->GetTransportPort())
			return pStreamChannel;
	}

	return NULL;
}

DWORD CSourceEngine::ReadStreamChannelsProc(void * pParam)
{
	CSourceEngine * pSourceEngine = (CSourceEngine *)pParam;
	return pSourceEngine->ReadStreamChannels();
}

DWORD CSourceEngine::ReadStreamChannels()
{	
	VORC_RTPSRC rc = VORC_RTPSRC_OK;

	while(!m_stopReading)
	{
		rc = DoReadingStreamChannels();
		if(rc == VORC_RTPSRC_ERROR) // socket error
		{
			m_stopReading  = true;
		}
		else if(rc == VORC_RTPSRC_BAD_NETWORK)
		{
			m_stopReading = true;
		}
		else if(rc == VORC_RTPSRC_NETWORK_TIMEOUT)
		{
		}
		else
		{
		}
	}

	return 0;
}

VORC_RTPSRC CSourceEngine::DoReadingStreamChannels()
{
	fd_set channelSockSet = m_channelSockSet;

	struct timeval tv_timeout;
	tv_timeout.tv_sec = 0;
	tv_timeout.tv_usec = 500000;

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
	int selectResult = select(0, &channelSockSet, NULL, NULL, &tv_timeout);
	if(selectResult < 0)
		return VORC_RTPSRC_BAD_NETWORK;
	else if(selectResult == 0)
		return VORC_RTPSRC_NETWORK_TIMEOUT;

	VORC_RTPSRC rc = VORC_RTPSRC_OK;
	for(unsigned int i=0; i<channelSockSet.fd_count; ++i)
	{
		CStreamChannel * pStreamChannel = SearchStreamChannel(channelSockSet.fd_array[i]);
		if(pStreamChannel == NULL)
			continue;

		rc = pStreamChannel->Read();
		if(rc != VORC_RTPSRC_OK)
			return rc;
	}
#elif defined(LINUX)
	// find the max nFd
	int nFdMax = 0;
	list_T<CStreamChannel *>::iterator iter;
	for(iter=m_listStreamChannel.begin(); iter!=m_listStreamChannel.end(); ++iter)
	{
		CStreamChannel * pStreamChannel = (CStreamChannel *)(*iter);
		SOCKET nFd = pStreamChannel->GetChannelSocket();
		if(nFdMax < nFd)
		{
			nFdMax = nFd;
		}
	}
	nFdMax += 1;

	int selectResult = select(nFdMax, &channelSockSet, NULL, NULL, &tv_timeout);
	if(selectResult < 0)
		return VORC_RTPSRC_BAD_NETWORK;
	else if(selectResult == 0)
		return VORC_RTPSRC_NETWORK_TIMEOUT;

	VORC_RTPSRC rc = VORC_RTPSRC_ERROR;

	for(iter=m_listStreamChannel.begin(); iter!=m_listStreamChannel.end(); ++iter)
	{
		CStreamChannel * pStreamChannel = (CStreamChannel *)(*iter);
		SOCKET nFd = pStreamChannel->GetChannelSocket();
		if(FD_ISSET(nFd, &channelSockSet))
		{
			rc = pStreamChannel->Read();
			if(rc != VORC_RTPSRC_OK)
				return rc;
		}
	}
#endif

	return VORC_RTPSRC_OK;
}


DWORD CSourceEngine::ReadLocalFileProc(void * pParam)
{
	CSourceEngine * pSourceEngine = (CSourceEngine *)pParam;
	return pSourceEngine->ReadLocalFile();
}

DWORD CSourceEngine::ReadLocalFile()
{	
	VORC_RTPSRC rc = VORC_RTPSRC_OK;

	m_pData = new unsigned char[2048];
	if(m_pData == NULL)
		return VORC_RTPSRC_OUT_OF_MEMORY;

	m_pFile = fopen("D:\\TMP\\ATSC.udp", "rb");
	if(m_pFile == NULL)
	{
		SAFE_DELETE_ARRAY(m_pData);
		return VORC_RTPSRC_ERROR;
	}

	while(!m_stopReading)
	{
		rc = DoReadingLocalFile();
		if(rc == VORC_RTPSRC_ERROR)
		{
			m_stopReading  = true;
		}
		else
		{
		}
	}

	if(m_pData != NULL)
	{
		SAFE_DELETE_ARRAY(m_pData);
	}

	if(m_pFile != NULL)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	return 0;
}

VORC_RTPSRC CSourceEngine::DoReadingLocalFile()
{
	VO_U32 nSize = fread(m_pData, 1, 4, m_pFile);
	if(feof(m_pFile) || nSize != 4)
		return VORC_RTPSRC_ERROR;

	unsigned short TotalLength = ntohl(*(u_long *)(m_pData)) & 0x0000FFFF;

	nSize = fread(m_pData+4, 1, TotalLength-4, m_pFile);
	if(feof(m_pFile) || nSize != TotalLength-4)
		return VORC_RTPSRC_ERROR;

	SendData(m_pData, TotalLength);

	return VORC_RTPSRC_OK;
}
