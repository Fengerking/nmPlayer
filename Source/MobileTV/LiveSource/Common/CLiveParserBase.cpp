#include "CLiveParserBase.h"
#include "voLog.h"


CLiveParserBase::CLiveParserBase(VO_LIVESRC_FORMATTYPE type)
: m_hDemux (NULL)
, m_nParserType(type)
{	
	if(type == VO_LIVESRC_FT_CMMB)
	{
#ifdef _WIN32
		vostrcpy(m_szDllFile, _T("voCMMBParser.dll"));	
#elif defined _LINUX
		vostrcpy(m_szDllFile, _T("libvoCMMBParser.so"));
#endif
	}
	else if(type == VO_LIVESRC_FT_HTTP_LS)
	{
#ifdef _WIN32
		vostrcpy(m_szDllFile, _T("voTSParser.dll"));	
#elif defined _LINUX
		vostrcpy(m_szDllFile, _T("libvoTSParser.so"));
#endif
	}
	else if(type == VO_LIVESRC_FT_TS)
	{
#ifdef _WIN32
		vostrcpy(m_szDllFile, _T("voTSParser.dll"));	
#elif defined _LINUX
		vostrcpy(m_szDllFile, _T("libvoTSParser.so"));
#endif
	}	
	else if(type == VO_LIVESRC_FT_ATSCMH)
	{
#ifdef _WIN32
		vostrcpy(m_szDllFile, _T("voATSCMHParser.dll"));	
#elif defined _LINUX
		vostrcpy(m_szDllFile, _T("libvoATSCMHParser.so"));
#endif
	}
}

CLiveParserBase::~CLiveParserBase(void)
{
	Close();
}

VO_U32 CLiveParserBase::Open(VO_PARSER_INIT_INFO * pInitInfo)
{
	LoadLib(NULL);
	
	return m_ParserFunc.Open(&m_hDemux, pInitInfo);
}

VO_U32 CLiveParserBase::Close()
{
	if (m_hDemux == NULL)
		return VO_ERR_PARSER_INVLAID_HANDLE;

	m_ParserFunc.Close(m_hDemux);
	m_hDemux = NULL;

	return 0;
}

VO_U32 CLiveParserBase::Process(VO_PARSER_INPUT_BUFFER * pBuffer)
{
	if (m_hDemux == NULL)
		return VO_ERR_PARSER_INVLAID_HANDLE;

	return m_ParserFunc.Process(m_hDemux, pBuffer);
}


VO_U32 CLiveParserBase::SetParam(VO_U32 uID, VO_PTR pParam)
{
	if (m_hDemux == NULL)
		return VO_ERR_PARSER_INVLAID_HANDLE;

	return m_ParserFunc.SetParam(m_hDemux, uID, pParam);
}


VO_U32 CLiveParserBase::GetParam(VO_U32 uID, VO_PTR pParam)
{
	if (m_hDemux == NULL)
		return VO_ERR_PARSER_INVLAID_HANDLE;

	return m_ParserFunc.GetParam(m_hDemux, uID, pParam);
}


VO_U32 CLiveParserBase::LoadLib(VO_HANDLE hInst/*, TCHAR* pszParserFileName*/)
{
	//vostrcpy(m_szDllFile, pszParserFileName);
	vostrcpy(m_szAPIName, _T("voGetParserAPI"));

	VOLOGI ("[LIVESRC] Load parser %s\n", m_szDllFile);

#ifdef _IOS
	// Jim: here need to refator to support multiple parser, (change parser's API name)
	if(m_nParserType == VO_LIVESRC_FT_TS)
		m_pAPIEntry = (VO_PTR)voGetParserAPI;
	else if(m_nParserType == VO_LIVESRC_FT_HTTP_LS)
		m_pAPIEntry = (VO_PTR)voGetParserAPI;
#else
	if (CDllLoad::LoadLib (hInst) == 0)
		return VO_ERR_PARSER_OPEN_FAIL;
#endif // _IOS

	VOGETPARSERAPI pAPI = (VOGETPARSERAPI) m_pAPIEntry;
	pAPI (&m_ParserFunc);

	VOLOGI ("[LIVESRC] Load parser ok \n");

	return VO_ERR_PARSER_OK;
}
