#include "netget.h"
#include "httpget.h"
#include "ftpget.h"
#include "netstrength.h"
//#include <tchar.h>
CNetStrength::CNetStrength()
	: m_pNetGet(NULL)
{
}

CNetStrength::~CNetStrength()
{
}

VONETDOWNRETURNCODE CNetStrength::Open(LPCTSTR _url, LPCTSTR _proxy, VONETDOWNTYPE type)
{
	if(!_tcsnicmp((TCHAR*)(_url), _T("http://"), 7))
		m_pNetGet = new(MEM_CHECK) CHttpGet;
	else if(!_tcsnicmp(_url, _T("ftp://"), 6))
		m_pNetGet = new(MEM_CHECK) CFtpGet;
	if(!m_pNetGet)
		return VORC_NETDOWN_NOT_IMPLEMENT;
	return m_pNetGet->Open(_url, _proxy, type);
}

VONETDOWNRETURNCODE CNetStrength::Start(bool* pbCancel)
{
	return m_pNetGet->Start(pbCancel);
}

VONETDOWNRETURNCODE CNetStrength::Stop()
{
	return m_pNetGet->Stop();
}

VONETDOWNRETURNCODE CNetStrength::Close()
{
	if(m_pNetGet != NULL)
	{
		m_pNetGet->Close();
		SAFE_DELETE(m_pNetGet);
	}
	return VORC_NETDOWN_OK;
}

VONETDOWNRETURNCODE CNetStrength::SetParam(long id, long param)
{
	return m_pNetGet->SetParam(id, param);
}

VONETDOWNRETURNCODE CNetStrength::GetParam(long id, long * _param)
{
	return m_pNetGet->GetParam(id, _param);
}
