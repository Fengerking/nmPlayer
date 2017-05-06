#include "CSourceWrapper.h"

#ifdef _WIN32_WCE
#define _FUNC	_T
#else
#define _FUNC
#endif // _WIN32_WCE

#define DEMUX_DLL_NAME	_T("voRTPSrc.dll")


CMTVSourceWrapper::CMTVSourceWrapper(void)
	: m_hMTVSrc (NULL)
{
	LoadLib(NULL);
}

CMTVSourceWrapper::~CMTVSourceWrapper(void)
{
	Close();
}

int CMTVSourceWrapper::Open(char * szSDPData, VO_RTPSRC_INIT_INFO * pInitInfo)
{
	return m_MTVSrcFunc.Open(&m_hMTVSrc, szSDPData, pInitInfo);
}

int CMTVSourceWrapper::Close()
{
	if (m_hMTVSrc == NULL)
		return -1;

	m_MTVSrcFunc.Close(m_hMTVSrc);
	m_hMTVSrc = NULL;

	return 0;
}

int CMTVSourceWrapper::Start()
{
	if (m_hMTVSrc == NULL)
		return -1;

	return m_MTVSrcFunc.Start(m_hMTVSrc);
}

int CMTVSourceWrapper::Stop()
{
	if (m_hMTVSrc == NULL)
		return -1;

	return m_MTVSrcFunc.Stop(m_hMTVSrc);
}

int CMTVSourceWrapper::GetParam(VO_U32 uID, VO_PTR pParam)
{
	if (m_hMTVSrc == NULL)
		return -1;

	return m_MTVSrcFunc.GetParam(m_hMTVSrc, uID, pParam);
}

int CMTVSourceWrapper::SetParam(VO_U32 uID, VO_PTR pParam)
{
	if (m_hMTVSrc == NULL)
		return -1;

	return m_MTVSrcFunc.SetParam(m_hMTVSrc, uID, pParam);
}

VO_U32 CMTVSourceWrapper::LoadLib(VO_HANDLE hInst)
{
#ifdef _WIN32
	vostrcpy(m_szDllFile, _T("voRTPSrc.dll"));
	vostrcpy(m_szAPIName, _T("voGetRTPSourceAPI"));
#elif defined _LINUX
	vostrcpy(m_szDllFile, _T("voRTPSrc.so"));
	vostrcpy(m_szAPIName, _T("voGetRTPSourceAPI"));
#endif

	if (CDllLoad::LoadLib (hInst) == 0)
		return 0;

	VOGETSOURCEAPI pAPI = (VOGETSOURCEAPI) m_pAPIEntry;
	pAPI (&m_MTVSrcFunc);

	return 1;
}
