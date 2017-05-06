#pragma once
#include "voPDPort.h"
class CBaseReadIn:MEM_MANAGER
{
public:
	CBaseReadIn();
	virtual ~CBaseReadIn();

public:
	virtual bool	Open(LPVOID lParam);
	virtual void	Close();

	virtual int		SetReadPos(DWORD dwReadPos, DWORD dwMoveMethod);
	virtual bool	ReadIn(PBYTE pData, int nLen, int& nReaded);
};
