#include "voParser.h"
#include "commonheader.h"
#include "netbase.h"
#include "RTPPacket.h"
#include "sdp_headers.h"
#include "DescrambleEngine.h"

#ifdef _S3
#include "EngineContainerWrapper.h"
#include "EngineContainer.h"
#endif //_S3

CDescrambleEngine::CDescrambleEngine()
: m_scrambleType(NON_SCRAMBLE)
#ifdef _S3
, m_pS3Dsc(NULL)
#endif //_S3
{
}

CDescrambleEngine::~CDescrambleEngine()
{
	UnInit();
}

bool CDescrambleEngine::Init(CSDPMedia * pSDPMedia)
{
	char _encodingName[32] = {0};
	pSDPMedia->ParseAttribute_rtpmap_encodingName(_encodingName, 32);
	if(_stricmp(_encodingName, "enc-mpeg4-generic") == 0)
	{
		m_scrambleType = ISMA_SCRAMBLE;
	}

	char _transportProtocol[16] = {0};
	pSDPMedia->ParseTransportProtocol(_transportProtocol, 16);
	if(_stricmp(_transportProtocol, "RTP/SAVP") == 0)
	{
		m_scrambleType = SRTP_SCRAMBLE;
	}

	if(m_scrambleType == ISMA_SCRAMBLE || m_scrambleType == SRTP_SCRAMBLE)
	{
#ifdef _S3
	return Init_S3_Descrambler(pSDPMedia);
#endif //_S3
	}

	return true;
}

void CDescrambleEngine::UnInit()
{
	if(m_scrambleType == ISMA_SCRAMBLE || m_scrambleType == SRTP_SCRAMBLE)
	{
#ifdef _S3
	return UnInit_S3_Descrambler();
#endif //_S3
	}
}

bool CDescrambleEngine::DescrambleRTPPacket(CRTPPacket * pRTPPacket)
{
	if(m_scrambleType == ISMA_SCRAMBLE)
	{
#ifdef _S3
		return S3_Descramble(pRTPPacket);
#endif //_S3
	}
	else if(m_scrambleType == SRTP_SCRAMBLE)
	{
#ifdef _S3
		return S3_Descramble(pRTPPacket);
#endif //_S3
	}
	else if(m_scrambleType == NON_SCRAMBLE)
	{
		return true;
	}

	return true;
}

#ifdef _S3

bool CDescrambleEngine::Init_S3_Descrambler(CSDPMedia * pSDPMedia)
{
	CEngineContainerWrapper engineContainerWrapper;
	if(!engineContainerWrapper.Load(_T("voenginecont.dll")))
		return false;
	CEngineContainer * pEngineContainer = NULL;
	engineContainerWrapper.GetEngineContainer(&pEngineContainer);
	if(pEngineContainer == NULL)
		return false;
	pEngineContainer->Get_S3_Descrambler(&m_pS3Dsc);
	if(m_pS3Dsc == NULL)
		return false;

	return ParseSocketAddress(pSDPMedia);
}

void CDescrambleEngine::UnInit_S3_Descrambler()
{
}

bool CDescrambleEngine::S3_Descramble(CRTPPacket * pRTPPacket)
{
	OHT_DSC_PLAYER_PacketInfo_t packetInfo;
	memset(&packetInfo, 0, sizeof(OHT_DSC_PLAYER_PacketInfo_t));

	memcpy(&(packetInfo.socketAddress), &m_socketAddress, sizeof(OHT_DSC_PLAYER_SocketAddress_t));
	packetInfo.ptrData = pRTPPacket->RTPPacketData();
	packetInfo.sizeInputData = pRTPPacket->RTPPacketSize();
	packetInfo.maxSizeOfData = pRTPPacket->RTPPacketSize();

	OHT_DSC_PLAYER_RetCode_t rc = m_pS3Dsc->ProcessPacket(&packetInfo);
	switch(rc)
	{
	case OHT_RET_DSC_PLAYER_SUCCESS:
		{
			pRTPPacket->SetRTPPacketSize(packetInfo.sizeOutputData);
			return true;
		}
	case OHT_RET_DSC_PLAYER_NOT_DESCRAMBLED:
	case OHT_RET_DSC_PLAYER_FAILURE:
	case OHT_RET_DSC_PLAYER_HEADER_PARSE_ERROR:
	case OHT_RET_DSC_PLAYER_INVALID_PARAM1:
		return false;
	default:
		return false;
	};

	return true;
}

bool CDescrambleEngine::ParseSocketAddress(CSDPMedia * pSDPMedia)
{
	CSDPSession * pSDPSession = pSDPMedia->GetSDPSession();

	char _addressType[16];
	if(!pSDPMedia->ParseAddressType(_addressType, 16))
	{
		if(!pSDPSession->ParseAddressType(_addressType, 16))
			return false;
	}

	char _address[64];
	if(!pSDPMedia->ParseConnectionAddress(_address, 64))
	{
		if(!pSDPSession->ParseConnectionAddress(_address, 64))
			return false;
	}

	unsigned short sockPort = 0;
	if(!pSDPMedia->ParseTransportPort(&sockPort))
		return false;
	m_socketAddress.port = sockPort;

	char _port[16];
	sprintf(_port, "%hu", sockPort);

	if(_strnicmp(_addressType, "IP4", 3) == 0)
	{
		m_socketAddress.IPAddress.eClass = OHT_DSC_PLAYER_IPCLASS_IPV4;

		struct addrinfo hints;
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family   = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		struct addrinfo * pAddrInfo = NULL;
		if(getaddrinfo(_address, NULL, &hints, (struct addrinfo **)&pAddrInfo) != 0)
			return false;
		struct addrinfo * pAI = pAddrInfo;
		while(pAI != NULL)
		{
			if(pAI->ai_socktype == SOCK_DGRAM && pAI->ai_protocol == IPPROTO_UDP)
			{
				struct sockaddr_in    sockAddress;
				memset(&sockAddress, 0, sizeof(sockaddr_in));
				memcpy(&sockAddress, pAI->ai_addr, pAI->ai_addrlen);

				memcpy(m_socketAddress.IPAddress.IP, &(sockAddress.sin_addr.S_un.S_addr), 4);
				break;
			}

			pAI = pAI->ai_next;
		}
		freeaddrinfo(pAddrInfo);
		if(pAI == NULL)
			return false;
	}
	else if(_strnicmp(_addressType, "IP6", 3) == 0)
	{
		m_socketAddress.IPAddress.eClass = OHT_DSC_PLAYER_IPCLASS_IPV6;

		struct addrinfo hints;
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family   = AF_INET6;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		struct addrinfo * pAddrInfo = NULL;
		if(getaddrinfo(_address, NULL, &hints, (struct addrinfo **)&pAddrInfo) != 0)
			return false;
		struct addrinfo * pAI = pAddrInfo;
		while(pAI != NULL)
		{
			if(pAI->ai_socktype == SOCK_DGRAM && pAI->ai_protocol == IPPROTO_UDP)
			{
				struct sockaddr_in6    sockAddress;
				memset(&sockAddress, 0, sizeof(sockaddr_in));
				memcpy(&sockAddress, pAI->ai_addr, pAI->ai_addrlen);

				memcpy(m_socketAddress.IPAddress.IP, &(sockAddress.sin6_addr.u.Byte), 16);
				break;
			}

			pAI = pAI->ai_next;
		}
		freeaddrinfo(pAddrInfo);
		if(pAI == NULL)
			return false;
	}

	return true;
}

#endif //S3
