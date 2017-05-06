#pragma once
#include "filebase.h"
#include "CNetGet.h"
#include "CBaseReadIn.h"
#include "voPDPort.h"
class CHttpDownloadFile:MEM_MANAGER
{
public:
	static	CNetGet2*	g_pNetGet;
	static DWORD		g_dwOffset;

public:
	CHttpDownloadFile();
	virtual ~CHttpDownloadFile();

public:
	//szFile is url
	virtual	bool	Open(LPCTSTR szFile, VOFILEOPMODE opMode);
	virtual void	Close();
	virtual	int		Read(PBYTE pBuffer, DWORD dwToRead, DWORD* pdwReaded);
	virtual int		Seek(DWORD dwOffset, VOFILESEEKPOS fsPos);
	virtual int		Size(DWORD* pHighSize);

protected:
	CBaseReadIn*	m_pReadIn;
	//g_dwOffset + actual file position
	DWORD			m_dwFilePos;
};
