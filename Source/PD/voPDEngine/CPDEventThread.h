#pragma once
#include "voPDPort.h"
class CNetGet2;
class CPDEventThread:public IVOCommand,MEM_MANAGER
{
public:
	CPDEventThread();
	virtual ~CPDEventThread();

public:
	void Execute();

public:
	bool			Create(CNetGet2* pNetGet);
	void			Destroy();

	DWORD			ThreadProcB();

protected:
	IVOThread*				m_hThread;
	bool		m_bStop;

	DWORD		m_dwWatchTime;	//’Ï≤‚ ±º‰<MS>
	CNetGet2*	m_pNetGet;
};
