#ifndef __SDP_SESSION_H__
#define __SDP_SESSION_H__

#include "list_T.h"

class CSDPSession
{
public:
	CSDPSession();
	~CSDPSession();

public:
	void   AddSessionDescriptionLine(const char * _sessionDesLine);
	char * SearchSessionDescriptionLine(const char * _searchLineName);


public:
	bool ParseSessionDescription_v(char * _v, size_t len);
	bool ParseSessionDescription_o(char * _o, size_t len);
	bool ParseSessionDescription_s(char * _s, size_t len);
	bool ParseSessionDescription_i(char * _i, size_t len);
	bool ParseSessionDescription_u(char * _u, size_t len);
	bool ParseSessionDescription_e(char * _e, size_t len);
	bool ParseSessionDescription_c(char * _c, size_t len);
	bool ParseSessionDescription_t(char * _t, size_t len);

public:
	bool ParseAddressType(char * _addressType, size_t len);
	bool ParseConnectionAddress(char * _connectionAddress, size_t len);
	bool ParseTTL(char * _ttl, size_t len);

public:
	bool ParseSessionAttribute_sdplang(char * _sdplang, size_t len);


private:
	list_T<char *>   m_listSessionDesLine;
};


#endif //__SDP_SESSION_H__