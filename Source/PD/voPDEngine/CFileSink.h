#pragma once
#include "CBaseSink.h"

typedef struct tagVOFileSinkOpenParam 
{
	TCHAR*		file_path;
	DWORD		file_size;
	bool		if_create;
} VOFileSinkOpenParam, *PVOFileSinkOpenParam;

class CFileSink :public CBaseSink
{
public:
	CFileSink();
	virtual ~CFileSink();

public:
	virtual bool	Open(LPVOID lParam);
	virtual void	Close();

	virtual bool	Sink(PBYTE pData, int nLen);
	virtual bool	SetSinkPos(DWORD dwSinkPos);

protected:
	FILE*		m_hFile;
	char			m_fileName[MAX_FNAME_LEN];
};
