#ifndef __NETSTRENGTH_H__
#define __NETSTRENGTH_H__

#include "commonheader.h"
#include "netdownbase.h"
#include "voPDPort.h"
class CNetGet;

class CNetStrength:MEM_MANAGER
{
public:
	CNetStrength();
	~CNetStrength();

public:
	VONETDOWNRETURNCODE Open(LPCTSTR _url, LPCTSTR _proxy, VONETDOWNTYPE type);
	VONETDOWNRETURNCODE Start(bool* pbCancel);
	VONETDOWNRETURNCODE Stop();
	VONETDOWNRETURNCODE Close();
	VONETDOWNRETURNCODE SetParam(long id, long param);
	VONETDOWNRETURNCODE GetParam(long id, long * _param);

private:
	CNetGet * m_pNetGet;
};


#endif //__NETSTRENGTH_H__