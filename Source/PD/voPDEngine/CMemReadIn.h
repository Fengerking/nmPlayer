#pragma once
#include "CBaseReadIn.h"

typedef struct tagVOMemReadInOpenParam 
{
	PBYTE	memory_buffer;
	DWORD	memory_buffer_size;
} VOMemReadInOpenParam, *PVOMemReadInOpenParam;

class CMemReadIn :
	public CBaseReadIn
{
public:
	CMemReadIn();
	virtual ~CMemReadIn();

public:
	virtual bool	Open(LPVOID lParam);

	virtual int		SetReadPos(DWORD dwReadPos, DWORD dwMoveMethod);
	virtual bool	ReadIn(PBYTE pData, int nLen, int& nReaded);

protected:
	PBYTE		m_pBuffer;
	DWORD		m_dwBufferSize;
	DWORD		m_dwReadPos;
};
