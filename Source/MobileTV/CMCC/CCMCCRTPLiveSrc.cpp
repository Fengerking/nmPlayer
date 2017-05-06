/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CCMMCRTPLiveSrc.cpp

	Contains:	CCMMCRTPLiveSrc source file

	Written by: 	Thomas Liang

	Change History (most recent first):
	2011-03-10		Thomas Liang			Create file

*******************************************************************************/
#include "CCMCCRTPLiveSrc.h"
#include "Fec.h"

#ifdef LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#elif defined WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

#define BUFFER 					2048
#define DEFAULTPORT				27015

#define CMCC_PACKET_SIZE		1316

#define LOG_TAG 		"CCMCCRTPLiveSrc"

typedef enum _transfer_mode
{
	TSMODE 	= 0x1,
	FECMODE	= 0x2
};

#define INVALID_PACK_INDEX(n,m) (((n)>(m-1))||((n)<0))
#define INVALID_FACTOR(n) (((n)>254)||((n)<1))

CCMCCRTPLiveSrc::CCMCCRTPLiveSrc()
{
	m_sockFD = -1;

	m_hCodec = NULL;
	m_nMode = TSMODE;

	m_sessionID = 0;
	m_packetID = 0;

	m_factorK = 0;
	m_factorN = 0;

	m_pktArray = NULL;
	m_bFirstPkt = 0;
}

CCMCCRTPLiveSrc::~CCMCCRTPLiveSrc()
{
	Reset();
}

void CCMCCRTPLiveSrc::Reset()
{
	m_sockFD = -1;

	if(m_hCodec)
	{
		fec_free(m_hCodec);
		m_hCodec = NULL;
	}
	
	m_nMode = TSMODE;

	m_sessionID = 0;
	m_packetID = 0;

	m_factorK = 0;
	m_factorN = 0;

	if(m_pktArray)
	{
		m_pktArray = NULL;
		delete m_pktArray;
	}
	
	m_bFirstPkt = 0;
}

int	CCMCCRTPLiveSrc::CreateSocket()
{
#ifdef WIN32	
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
	m_sockFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(m_sockFD == INVALID_SOCKET)
	{
		return -1;
	}
	
#elif defined LINUX
	m_sockFD = socket(AF_INET, SOCK_DGRAM, 0); /* create a socket */

	if(m_sockFD == -1)
	{
		return -1;
	}

	/* init servaddr */
	bzero(&m_sockAddr, sizeof(m_sockAddr));
#endif //WIN32
	return 0;
}

void CCMCCRTPLiveSrc::CloseSocket()
{
#ifdef LINUX
	close(m_sockFD);
#elif defined WIN32
	closesocket(m_sockFD);
#endif
	m_sockFD= -1;
}

int	CCMCCRTPLiveSrc::Select(unsigned short interval)
{
	int nfds = 0;
	fd_set readfds;
	int ret = 0;
	struct timeval time;

	time.tv_sec = interval;
	time.tv_usec = 0;
	
	FD_ZERO(&readfds);
	FD_SET(m_sockFD, &readfds);
	nfds = m_sockFD+1;
	
	return select(nfds, &readfds, NULL, NULL, &time);
}

int	CCMCCRTPLiveSrc::Start()
{
	int port = 0;

	ENTER();
	CBaseConfig *config = new CBaseConfig();

	if(config->Open((VO_PTCHAR)CONFIGFILE)==VO_FALSE)
	{
		VOLOGE("config file could not be opened\n");
		delete config;
		return -1;
	}

	port = config->GetItemValue ("IP", "PORT", DEFAULTPORT);
	m_nMode = config->GetItemValue ("CONFIG", "MODE", TSMODE);
	
	delete config;

	if(CreateSocket())
	{
		VOLOGE("%s:Create Socket failed", __FUNCTION__);
		return -1;
	}

	m_sockAddr.sin_family = AF_INET;	
	m_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_sockAddr.sin_port = htons(port);

	/* bind address and port to socket */
	if(bind(m_sockFD, (struct sockaddr *)&m_sockAddr, sizeof(m_sockAddr)) == -1)
	{
		VOLOGE("%s:Bind Socket failed", __FUNCTION__);
		CloseSocket();
		return -1;
	}

	CCMCCRTPSrc::Start();
	LEAVE();
	return 0;
}


int	CCMCCRTPLiveSrc::Stop()
{
	ENTER();

	if(m_sockFD != -1)
		CloseSocket();
	
	CCMCCRTPSrc::Stop();
	
	Reset();

	LEAVE();
	return 0;
}

void CCMCCRTPLiveSrc::SavePackByIndex(PFEC_EXT_HEADER ext_header, MEM_NODE *node)
{
	unsigned short *extenFiled = (unsigned short *)(node->buf+14);
	int offset = 0x10+ntohs(*extenFiled)*4;

	if(ext_header->pack_index == m_packetID)
	{
		memcpy((m_pktArray),(node->buf+offset), node->readSize-offset);
		m_packetID++;
	}
	else if(ext_header->pack_index < m_packetID)
	{
		MISS_NODE *node = NULL;
		
		//save this packet if this node marked as missed before and delete that packet
		node = m_hMemSink->FindAndGetMissNode(ext_header->pack_index);
		if(node)
		{
//			memcpy((m_pktArray+ext_header->pack_index*CMCC_PACKET_SIZE),(node->buf+offset), node->readSize-offset);
		}

		//dismiss if not marked as miss
	}
	else
	{
		//if bigger than current, then probable we lost packet, mark them as missed
		MISS_NODE *node = new MISS_NODE;
		memset(node, 0, sizeof(MISS_NODE));

		node->index = m_packetID;
	}
}

void CCMCCRTPLiveSrc::HandleRTPInFEC(MEM_NODE *node)
{
	unsigned short *extenFiled = (unsigned short *)(node->buf+14);
	PFEC_EXT_HEADER ext_header = (PFEC_EXT_HEADER)(node->buf+16);
	
	if(ntohs(*extenFiled) != 3)
		return;

	if(INVALID_FACTOR(ext_header->factor_k)|| INVALID_FACTOR(ext_header->factor_n)||
		(ext_header->factor_k > ext_header->factor_n))
		return;

	if(INVALID_PACK_INDEX(ext_header->pack_index, ext_header->factor_n))
		return;

	//receive first packet
/*	if(!m_bFirstPkt)
	{
		//do some init 
		m_factorK = ext_header->factor_k;
		m_factorN = ext_header->factor_n;
		m_hCodec = fec_new(m_factorK, m_factorN);
		m_pktArray = (unsigned char *)new char[m_factorK][CMCC_PACKET_SIZE];
		memset(m_pktArray, 0, (m_factorK*CMCC_PACKET_SIZE));
		
		m_bFirstPkt = 1;
	}*/

	if(ext_header->pack_base == m_sessionID)
	{
		SavePackByIndex(ext_header,node);
	}
	else if(ext_header->pack_base > m_sessionID)
	{
		//send all packet in this session to TS parser
		
		//clear packet array

		//save packet in current session

		m_sessionID++;
	}
}

void CCMCCRTPLiveSrc::MainLoop()
{
	ENTER();

	while(1)
	{
		MEM_NODE *node = NULL;
		do
		{				
			node = m_hMemSink->GetDataNode();
			if(!node)
			{
				Select(3);
				break;
			}

			if(m_nMode == TSMODE)
			{
				unsigned short *extenFiled = (unsigned short *)(node->buf+14);
				int offset = 0x10+ntohs(*extenFiled)*4;

				VO_BYTE data[1344];
				int size = node->readSize-offset;

				memcpy(data,node->buf+offset,node->readSize-offset);

				m_hMemSink->InsertFreeNode(node);
				OnReceiveRTPData(data, size);
			}
			else if(m_nMode == FECMODE)
			{
				HandleRTPInFEC(node);
			}
		}while(1);
		
		if(m_bExit)
			break;
	}

	CCMCCRTPSrc::MainLoop();

	LEAVE();
	return;
}

void CCMCCRTPLiveSrc::ReceiveLoop()
{
	ENTER();
	
	int	recvlen= 1;

	while((recvlen>0)&&(!m_bExit))
	{
		if(m_sockFD == -1)
		{
			VOLOGE("%s: Invalid Socket handle");
			break;			
		}

		MEM_NODE * node = NULL;
		node = m_hMemSink->GetFreeNode();

		if(!node)
		{
			VOLOGE("%s: No more free space!",__FUNCTION__);
			break;
		}

		do
		{
			recvlen = Select(3);

			if(m_bExit)
				break;			
			
			VOLOGI("%s: Select returned %d", __FUNCTION__, recvlen);

		}while(!recvlen);
		
#ifdef WIN32
		if(recvlen == SOCKET_ERROR)
		{
			m_hMemSink->InsertFreeNode(node);
			VOLOGI("%s: socket error, return", __FUNCTION__);
			break;
		}
#elif defined LINUX
		if(recvlen == -1)
		{
			m_hMemSink->InsertFreeNode(node);
			VOLOGI("%s: socket error, return", __FUNCTION__);
			break;
		}
#endif //WIN32
		
		if((recvlen > 0))
		{
			recvlen = recvfrom(m_sockFD, (char *)node->buf, node->size, 0, NULL, NULL);

			VOLOGW("Time:%lu, %d bytes received", voTimeGetTime(),recvlen);
			
			if(recvlen>0)
			{
				node->readSize = recvlen;
				m_hMemSink->InsertDataNode(node);
			}
		}
		else
		{
			m_hMemSink->InsertFreeNode(node);
		}
	}

	CCMCCRTPSrc::ReceiveLoop();

	LEAVE();
	return;
}
