#pragma once
#include "CBaseSink.h"

typedef struct tagVOMemSinkOpenParam 
{
	PBYTE	memory_buffer;
	DWORD	memory_buffer_size;
} VOMemSinkOpenParam, *PVOMemSinkOpenParam;

class CMemSink :
	public CBaseSink
{
public:
	CMemSink();
	virtual ~CMemSink();

public:
	virtual bool	Open(LPVOID lParam);
	virtual void	Close();

	virtual bool	Sink(PBYTE pData, int nLen);

protected:
	PBYTE		m_pBuffer;
};
