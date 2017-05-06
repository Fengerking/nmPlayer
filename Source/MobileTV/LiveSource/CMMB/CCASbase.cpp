#include "CCASbase.h"

CCASBase::CCASBase()
{
}

CCASBase::~CCASBase()
{
}

int CCASBase::InitializeCAS(void * inParam)
{
	return 0;
}

int CCASBase::TerminateCAS(void)
{
	return 0;
}

int CCASBase::GetCASInfo(void * pInfo)
{
	return 0;
}

int CCASBase::SetCASParam(void * inCAID, int nSize, int nServiceID)
{
	return 0;
}

int CCASBase::SendExMData(VO_BYTE * pExMData, int nSize, int nType, int nServiceID)
{
	return 0;
}

int CCASBase::DescrambleData(VO_BYTE * inData, int inSize, VO_BYTE * outData, unsigned int * outSize, int nType)
{
	return 0;
}

int CCASBase::SetParam(int nID, int nParam)
{
	return 0;
}

int CCASBase::GetParam(int nID, int * nParam)
{
	return 0;
}
