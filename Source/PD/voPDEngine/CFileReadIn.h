#pragma once
#include "CBaseReadIn.h"

class CFileReadIn :
	public CBaseReadIn
{
public:
	CFileReadIn();
	virtual ~CFileReadIn();

public:
	virtual bool	Open(LPVOID lParam);
	virtual void	Close();

	virtual int		SetReadPos(DWORD dwReadPos, DWORD dwMoveMethod);
	virtual bool	ReadIn(PBYTE pData, int nLen, int& nReaded);

protected:
	FILE*	m_hFile;
	char			m_fileName[MAX_FNAME_LEN];
};
