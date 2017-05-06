#include "vortpsrc.h"
#include "sdp_headers.h"
#include "ipnetwork.h"
#include "StreamControlChannel.h"

CStreamControlChannel::CStreamControlChannel(VO_RTPSRC_INIT_INFO * pInitInfo)
: CStreamDataChannel(pInitInfo)
{
}

CStreamControlChannel::~CStreamControlChannel()
{
}

VORC_RTPSRC CStreamControlChannel::Init(CSDPMedia * pSDPMedia)
{
	VORC_RTPSRC rc = CStreamChannel::Init(pSDPMedia);
	if(rc != VORC_RTPSRC_OK)
		return rc;

	do
	{
		m_channelID = (ParseStreamId() | PACKET_CTRL);

		char _addressType[8];
		if(!m_pSDPMedia->ParseAddressType(_addressType, 16))
		{
			CSDPSession * pSDPSession = m_pSDPMedia->GetSDPSession();
			if(!pSDPSession->ParseAddressType(_addressType, 16))
				break;
		}

#if defined(LINUX)
		char _connectionAddress[64];
		if(!m_pSDPMedia->ParseConnectionAddress(_connectionAddress, 64))
		{
			CSDPSession * pSDPSession = m_pSDPMedia->GetSDPSession();
			if(!pSDPSession->ParseConnectionAddress(_connectionAddress, 64))
				return VORC_RTPSRC_ERROR;
		}
#endif

		if(!m_pSDPMedia->ParseTransportPort(&m_transPort))
			break;
		++m_transPort;

		if(_strnicmp(_addressType, "IP4", 3) == 0)
		{
			m_pIPSocket = new CIPv4Socket();
		}
		else if(_strnicmp(_addressType, "IP6", 3) == 0)
		{
			m_pIPSocket = new CIPv6Socket();
		}
		else
		{
			m_pIPSocket = NULL;
		}

		if(m_pIPSocket == NULL)
			break;

#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
		const char * _interface = NULL;
		if(!m_pIPSocket->CreateDGramSocket(_interface, m_transPort))
			break;
#elif defined(LINUX)
		if(!m_pIPSocket->CreateDGramSocket(_connectionAddress, m_transPort))
			break;
#endif

		m_buffer = new unsigned char[MTUSize];
		if(m_buffer == NULL)
			return VORC_RTPSRC_OUT_OF_MEMORY;

		return VORC_RTPSRC_OK;

	}while(0);

	SAFE_DELETE(m_pIPSocket);
	SAFE_DELETE_ARRAY(m_buffer);

	return VORC_RTPSRC_ERROR;
}
