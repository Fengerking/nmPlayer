#include "sdp_error.h"
#include "sdp_session.h"
#include "sdp_utility.h"

#if defined(LINUX)
#   include "vocrsapis.h"
#endif

CSDPSession::CSDPSession()
{
}

CSDPSession::~CSDPSession()
{
	list_T<char *>::iterator iter;
	for(iter=m_listSessionDesLine.begin(); iter!=m_listSessionDesLine.end(); ++iter)
	{
		delete[] *iter;
	}
	m_listSessionDesLine.clear();
}

void CSDPSession::AddSessionDescriptionLine(const char * _sessionDesLine)
{
	size_t len = SDP_GetLineLength(_sessionDesLine);
	if(len == 0)
		return;

	char * _line = SDP_StringDup_N(_sessionDesLine, len);
	if(_line != NULL)
		m_listSessionDesLine.push_back(_line);
}

char * CSDPSession::SearchSessionDescriptionLine(const char * _searchLineName)
{
	char * _sessionDesLine = NULL;
	list_T<char *>::iterator iter;
	for(iter=m_listSessionDesLine.begin(); iter!=m_listSessionDesLine.end(); ++iter)
	{
		_sessionDesLine = *iter;
		if(_strnicmp(_sessionDesLine, _searchLineName, strlen(_searchLineName)) == 0)
			return _sessionDesLine;
	}
	return NULL;
}



bool CSDPSession::ParseSessionDescription_v(char * _v, size_t len)
{
	do
	{
		char * _sessionDes_v = SearchSessionDescriptionLine("v=");
		if(_sessionDes_v == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_sessionDes_v, "v=%[^\r\n]", _v) != 1)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPSession::ParseSessionDescription_o(char * _o, size_t len)
{
	return false;
}

bool CSDPSession::ParseSessionDescription_s(char * _s, size_t len)
{
	return false;
}

bool CSDPSession::ParseSessionDescription_i(char * _i, size_t len)
{
	return false;
}

bool CSDPSession::ParseSessionDescription_u(char * _u, size_t len)
{
	return false;
}

bool CSDPSession::ParseSessionDescription_e(char * _e, size_t len)
{
	return false;
}

bool CSDPSession::ParseSessionDescription_c(char * _c, size_t len)
{
	do
	{
		char * _sessionDes_c = SearchSessionDescriptionLine("c=");
		if(_sessionDes_c == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_sessionDes_c, "c=%[^\r\n]", _c) != 1)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPSession::ParseSessionDescription_t(char * _t, size_t len)
{
	return false;
}


bool CSDPSession::ParseAddressType(char * _addressType, size_t len)
{
	do
	{
		char * _sessionDes_c = SearchSessionDescriptionLine("c=");
		if(_sessionDes_c == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_sessionDes_c, "c=IN %s", _addressType) != 1)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPSession::ParseConnectionAddress(char * _connectionAddress, size_t len)
{
	do
	{
		char * _sessionDes_c = SearchSessionDescriptionLine("c=");
		if(_sessionDes_c == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		char _addressType[16];
		if(sscanf(_sessionDes_c, "c=IN %s %[^/ \r\n]", _addressType, _connectionAddress) != 2)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

bool CSDPSession::ParseTTL(char * _ttl, size_t len)
{
	do
	{
		char * _sessionDes_c = SearchSessionDescriptionLine("c=");
		if(_sessionDes_c == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		char _addressType[16];
		char _connectionAddress[64];
		if(sscanf(_sessionDes_c, "c=IN %s %[^/ \r\n]/%[^/ \r\n]", _addressType, _connectionAddress, _ttl) != 3)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}


bool CSDPSession::ParseSessionAttribute_sdplang(char * _sdplang, size_t len)
{
	do
	{
		char * _sessionAttr_sdplang = SearchSessionDescriptionLine("a=sdplang");
		if(_sessionAttr_sdplang == NULL)
		{
			SDP_SetLastError(SDPERROR_DESCRIPTION_NOT_FOUND);
			break;
		}

		if(sscanf(_sessionAttr_sdplang, "a=sdplang:[^\r\n]", _sdplang) != 1)
		{
			SDP_SetLastError(SDPERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		return true;

	}while(0);

	return false;
}

