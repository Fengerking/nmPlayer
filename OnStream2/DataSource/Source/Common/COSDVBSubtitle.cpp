#include "voString.h"
#include "COSDVBSubtitle.h"

#define  LOG_TAG    "COSDVBSubtitle"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

COSDVBSubtitle::COSDVBSubtitle(VO_SOURCE2_LIB_FUNC *pLibop, VO_TCHAR* pWorkPath /*= NULL*/)
	: COSBaseSubtitle(pLibop, pWorkPath)
	, m_subtitle(m_alloc)
{
	memset(&m_szDVBSubtitleAPI, 0, sizeof(VO_DVB_PARSER_API));
	memset(&m_sDVBSubInfo, 0, sizeof(voSubtitleInfo));
	LoadDll();

	m_nSubtitlteType = VOOS_SUBTITLE_DVBT;
}

COSDVBSubtitle::~COSDVBSubtitle ()
{
	Uninit();
	unLoadDll();
}

int	COSDVBSubtitle::Init(unsigned char* pBuffer, int nLength, int nType)
{
	int nRC = VOOSMP_ERR_Implement;
	
	if(m_hSubtitle) Uninit();
	
	if(m_szDVBSubtitleAPI.Open == NULL) 
		LoadDll();

	if(m_szDVBSubtitleAPI.Open == NULL)
		return nRC;

	voCAutoLock lockReadSrc (&m_Lock);
	VO_DVB_PARSER_INIT_INFO DVBInitInfo;
	memset(&DVBInitInfo, 0, sizeof(VO_DVB_PARSER_INIT_INFO));
	DVBInitInfo.strWorkPath = m_szWorkPath;
	if(nType == VO_SUBTITLE_CodingDVBTeleText)
		DVBInitInfo.nType = VO_DVB_SUBTITLE_TYPE_TELETEXT;
	else if(nType == VO_SUBTITLE_CodingDVBSystem)
		DVBInitInfo.nType = VO_DVB_SUBTITLE_TYPE_SYSTEM;
	else
		return nRC;

	nRC = m_szDVBSubtitleAPI.Open(&m_hSubtitle, &DVBInitInfo);

	m_subtitle.Flush();

	return nRC;
}

int	COSDVBSubtitle::Uninit()
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_szDVBSubtitleAPI.Close == NULL)
		return nRC;

	voCAutoLock lockReadSrc (&m_Lock);
	if(m_hSubtitle)
		nRC = m_szDVBSubtitleAPI.Close(m_hSubtitle);
	m_hSubtitle = NULL;
	
	return nRC;
}

int	COSDVBSubtitle::SetInputData(VO_CHAR * pData, int len, int nTime,int nType)
{
	int nRC = VOOSMP_ERR_Implement;

	voCAutoLock lockReadSrc (&m_Lock);
	if(m_szDVBSubtitleAPI.Process == NULL || m_szDVBSubtitleAPI.GetData == NULL || m_hSubtitle == NULL)
		return nRC;
	
	VO_DVB_PARSER_INPUT_BUFFER DVBInputBuf;
	memset(&DVBInputBuf, 0, sizeof(VO_DVB_PARSER_INPUT_BUFFER));

	DVBInputBuf.nSize = len;
	DVBInputBuf.pData = (VO_PBYTE)pData;
	DVBInputBuf.nTimeStamp = nTime;

	nRC = m_szDVBSubtitleAPI.Process(m_hSubtitle, &DVBInputBuf);
	if(nRC) return nRC;

	VO_DVB_PARSER_OUTPUT_BUFFER DVBOutBuf;
	memset(&DVBOutBuf, 0, sizeof(VO_DVB_PARSER_OUTPUT_BUFFER));
	nRC = m_szDVBSubtitleAPI.GetData(m_hSubtitle, &DVBOutBuf);

	if(nRC == 0)
	{
		nRC = m_subtitle.AddBuffer( DVBOutBuf.pSubtitleInfo );
	}

	return nRC;
}

int	COSDVBSubtitle::GetSubtitleInfo(long nTimeCurrent, voSubtitleInfo ** subtitleInfo)
{
	int nRC = VOOSMP_ERR_Implement;
	
	voCAutoLock lockReadSrc (&m_Lock);
	memset(&m_sDVBSubInfo, 0, sizeof(voSubtitleInfo));
	*subtitleInfo = &m_sDVBSubInfo;

	m_sDVBSubInfo.nTimeStamp = nTimeCurrent;

	nRC = m_subtitle.GetBuffer( &m_sDVBSubInfo );

	return nRC;
}

int	COSDVBSubtitle::SetCurPos (long long *pCurPos)
{
	m_subtitle.Flush();
	return 0;
}

int	COSDVBSubtitle::GetParam (int nID, void * pValue)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_szDVBSubtitleAPI.GetParam == NULL || m_hSubtitle == NULL)
		return nRC;

	nRC = m_szDVBSubtitleAPI.GetParam(m_hSubtitle, nID, pValue);

	return nRC;
}

int	COSDVBSubtitle::SetParam (int nID, void * pValue)
{
	int nRC = VOOSMP_ERR_Implement;

	if(m_szDVBSubtitleAPI.SetParam == NULL || m_hSubtitle == NULL)
		return nRC;

	nRC = m_szDVBSubtitleAPI.SetParam(m_hSubtitle, nID, pValue);

	return nRC;
}

int	COSDVBSubtitle::LoadDll()
{
	if(m_pLibop == NULL)
		return -1;
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = m_pLibop->LoadLib(m_pLibop->pUserData, "voDVBSubtitleParser", 0);

	if (m_hDll == NULL)
	{
		VOLOGI("Load library voSubtitleParser failed");
		return -1;
	}

	pvoGetDVBSubtitleAPI pGetAPI = (pvoGetDVBSubtitleAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, "voGetDVBParserAPI", 0);

	if (pGetAPI == NULL)
	{
		VOLOGI("get API pvoGetDVBSubtitleAPI failed");
		return -1;
	}
	pGetAPI(&m_szDVBSubtitleAPI);

	if(m_szDVBSubtitleAPI.Open == NULL)
	{
		VOLOGI("m_szDVBSubtitleAPI.Open  == NULL");
		return -1;
	}

	return 0;
}

int	COSDVBSubtitle::unLoadDll()
{
	if (m_hDll != NULL && m_pLibop != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	memset(&m_szDVBSubtitleAPI, 0, sizeof(m_szDVBSubtitleAPI));

	return 0; 
}
