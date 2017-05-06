#pragma once
#include <voPDPort.h>
enum
{
	VOPD_SINK_SUCCESS				= 0, 
	VOPD_SINK_ERR_CREATE_FAIL		= 1, 
	VOPD_SINK_ERR_NO_ENOUGH_SPACE	= 2, 
	VOPD_SINK_ERR_WRITE_FAIL		= 3, 
	VOPD_SINK_ERR_SEEK_FAIL			= 4, 
};

class CBaseSink:MEM_MANAGER
{
public:
	CBaseSink();
	virtual ~CBaseSink();

public:
	virtual bool	Open(LPVOID lParam);
	virtual void	Close();

	virtual bool	Sink(PBYTE pData, int nLen);
	virtual bool	SetSinkPos(DWORD dwSinkPos);
	virtual DWORD	GetSinkPos();
	virtual	DWORD	GetLastErr();

	virtual bool	SetFileHeaderSize(DWORD dwFileHeaderSize);
	virtual DWORD	GetFileHeaderSize();

	virtual bool	SetFileSize(DWORD dwFileSize);
	virtual DWORD	GetFileSize();

	virtual bool	SetOffet(DWORD dwOffset);

protected:
	//file actual position
	DWORD		m_dwSinkPos;
	//PD header size + actual file size
	DWORD		m_dwFileSize;
	DWORD		m_dwErr;
	DWORD		m_dwFileHeaderSize;
	//PD header size
	DWORD		m_dwOffset;
};
