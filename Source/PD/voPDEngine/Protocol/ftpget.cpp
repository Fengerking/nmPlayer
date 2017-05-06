#include "ftpget.h"

CFtpGet::CFtpGet()
: CNetGet()
{
}

CFtpGet::~CFtpGet()
{
}

VONETDOWNRETURNCODE CFtpGet::Open(LPCTSTR _url, LPCTSTR _proxy, VONETDOWNTYPE type)
{
	return VORC_NETDOWN_OK;
}

VONETDOWNRETURNCODE CFtpGet::Close()
{
	return VORC_NETDOWN_OK;
}

VONETDOWNRETURNCODE CFtpGet::SetParam(long id, long param)
{
	return CNetGet::SetParam(id, param);
}

VONETDOWNRETURNCODE CFtpGet::GetParam(long id, long * _param)
{
	return CNetGet::GetParam(id, _param);
}

DWORD CFtpGet::Getting()
{
	return 0;
}
