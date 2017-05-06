#include <voPDPort.h>
#include "CBaseReadIn.h"

CBaseReadIn::CBaseReadIn()
{
}

CBaseReadIn::~CBaseReadIn()
{
	Close();
}

bool CBaseReadIn::Open(LPVOID lParam)
{
	return true;
}

void CBaseReadIn::Close()
{
}

int CBaseReadIn::SetReadPos(DWORD dwReadPos, DWORD dwMoveMethod)
{
	return -1;
}

bool CBaseReadIn::ReadIn(PBYTE pData, int nLen, int& nReaded)
{
	return true;
}
