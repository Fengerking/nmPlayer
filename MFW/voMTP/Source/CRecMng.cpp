#include "CRecMng.h"
#include "vomtpAPI.h"

CRecMng::CRecMng(void)
:m_bRecording(VO_FALSE)
{
}

CRecMng::~CRecMng(void)
{
}

VO_U32 CRecMng::RecStart(TCHAR* pszFileName, VO_U32 nFileFormat/*=REC_MP4*/)
{
	return VO_ERR_MTP_NOT_IMPLEMENT;
}

VO_U32 CRecMng::RecStop()
{
	return VO_ERR_MTP_NOT_IMPLEMENT;
}

VO_BOOL	CRecMng::IsRecording()
{
	return m_bRecording;
}

