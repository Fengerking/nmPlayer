#include "CCMMBDemux.h"
#include "voLog.h"

#ifdef _WIN32_WCE
#define _FUNC	_T
#else
#define _FUNC
#endif // _WIN32_WCE

#define DEMUX_DLL_NAME	_T("voCmmbParser.dll")


CCMMBDemux::CCMMBDemux(void)
	: m_hDemux (NULL)
{
	if (LoadLib(NULL) == 0)
		printf("Failed to load paser. <--------------\n");
}

CCMMBDemux::~CCMMBDemux(void)
{
	Close();
}

int CCMMBDemux::Open(VO_PARSER_INIT_INFO * pInitInfo)
{
	return m_ParserFunc.Open(&m_hDemux, pInitInfo);
}

int CCMMBDemux::Close()
{
	if (m_hDemux == NULL)
		return -1;

	m_ParserFunc.Close(m_hDemux);
	m_hDemux = NULL;

	return 0;
}

int CCMMBDemux::Process(VO_PARSER_INPUT_BUFFER * pBuffer)
{
	if (m_hDemux == NULL)
		return -1;

	return m_ParserFunc.Process(m_hDemux, pBuffer);
}

int CCMMBDemux::SetCASType(int nType)
{
	if (m_hDemux == NULL)
		return -1;

	return m_ParserFunc.SetParam(m_hDemux, VO_PID_PARSER_BASE/*change*/, (VO_PTR)nType);
}

VO_U32 CCMMBDemux::LoadLib(VO_HANDLE hInst)
{
#ifdef _WIN32
#ifdef _DVBH_
	vostrcpy(m_szDllFile, _T("voRTPPsr.dll"));	
#elif defined _FILESIMULATE
	vostrcpy(m_szDllFile, _T("voCMMBParser.dll"));
#endif
	vostrcpy(m_szAPIName, _T("voGetParserAPI"));
#elif defined _LINUX
#ifdef _DVBH_
	vostrcpy(m_szDllFile, _T("voRTPPsr.so"));	
#elif defined _FILESIMULATE
	vostrcpy(m_szDllFile, _T("voCMMBParser.so"))
#endif
	vostrcpy(m_szAPIName, _T("voGetParserAPI"));
#endif
	
	VOLOGI ("[ENG] Load parser %s\n", m_szDllFile);

	if (CDllLoad::LoadLib (hInst) == 0)
		return 0;

	VOGETPARSERAPI pAPI = (VOGETPARSERAPI) m_pAPIEntry;
	pAPI (&m_ParserFunc);

	VOLOGI ("[ENG] Load parser ok \n");

	return 1;
}
