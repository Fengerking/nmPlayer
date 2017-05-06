#ifndef __FTPGET_H__
#define __FTPGET_H__

#include "netget.h"

class CFtpGet : public CNetGet
{
public:
	CFtpGet();
	virtual ~CFtpGet();

public:
	virtual VONETDOWNRETURNCODE Open(LPCTSTR _url, LPCTSTR _proxy, VONETDOWNTYPE type);
	virtual VONETDOWNRETURNCODE Close();
	virtual VONETDOWNRETURNCODE SetParam(long id, long param);
	virtual VONETDOWNRETURNCODE GetParam(long id, long * _param);
protected:
	virtual DWORD Getting();
};

#endif //__FTPGET_H__