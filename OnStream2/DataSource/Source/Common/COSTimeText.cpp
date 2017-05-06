#include "voString.h"
#include "COSTimeText.h"

#define  LOG_TAG    "COSTimeText"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

COSTimeText::COSTimeText(VO_SOURCE2_LIB_FUNC *pLibop, VO_TCHAR* pWorkPath/* = NULL*/)
	: COSBaseSubtitle(pLibop, pWorkPath)
{
	memset(&m_szSubParserAPI, 0, sizeof(VO_SUBTITLE_PARSER));
	LoadDll();

	m_nSubtitlteType = VOOS_SUBTITLE_TIMETEXT;
}

COSTimeText::~COSTimeText ()
{
	Uninit();
	unLoadDll();
}

int	COSTimeText::Init(unsigned char* pBuffer, int nLength, int nType)
{
	if(m_hSubtitle)
	{
		Uninit();
	}

	if(m_szSubParserAPI.voInit == NULL)
	{
		LoadDll();
	}
	
	if(m_szSubParserAPI.voInit == NULL)
		return VOOSMP_ERR_Implement;

	voCAutoLock lockReadSrc (&m_Lock);
	VO_SUBTITLE_PARSER_INIT_INFO sInitInfo;
	memset (&sInitInfo, 0, sizeof (VO_SUBTITLE_PARSER_INIT_INFO));
	sInitInfo.strWorkPath = m_szWorkPath;

	int nRC = m_szSubParserAPI.voInit(&m_hSubtitle, &sInitInfo);

	return nRC;
}

int	COSTimeText::Uninit()
{
	int nRC = VOOSMP_ERR_Implement;
	if(m_szSubParserAPI.voUninit == NULL)
		return VOOSMP_ERR_Implement;

	voCAutoLock lockReadSrc (&m_Lock);
	if(m_hSubtitle)
		nRC = m_szSubParserAPI.voUninit(m_hSubtitle);
	m_hSubtitle = NULL;
	m_nSubTitleNum = 0;

	ClearLanguage();

	return nRC;
}


int	COSTimeText::SetTimedTextFilePath(VO_TCHAR * pPath, int nType)
{
	if(m_szSubParserAPI.voSetTimedTextFilePath == NULL && m_hSubtitle == NULL)
		return VOOSMP_ERR_Implement;

	voCAutoLock lockReadSrc (&m_Lock);
	int nRC = m_szSubParserAPI.voSetTimedTextFilePath(m_hSubtitle, pPath, nType);

	m_nSubTitleNum++;

	return nRC;
}

int	COSTimeText::SetInputData(VO_CHAR * pData, int len, int nTime,int nType)
{
	if(m_szSubParserAPI.voSetInputData == NULL && m_hSubtitle == NULL)
		return VOOSMP_ERR_Implement;

	voCAutoLock lockReadSrc (&m_Lock);
	int nRC = m_szSubParserAPI.voSetInputData(m_hSubtitle, pData, len, nType, (VO_S64)nTime);

	return nRC;
}

int	COSTimeText::GetSubtitleInfo(long nTimeCurrent, voSubtitleInfo ** subtitleInfo)
{
	if(m_szSubParserAPI.voGetSubtitleInfo == NULL && m_hSubtitle == NULL)
		return VOOSMP_ERR_Implement;

	voCAutoLock lockReadSrc (&m_Lock);
	int nRC = m_szSubParserAPI.voGetSubtitleInfo(m_hSubtitle, nTimeCurrent, subtitleInfo);

	return nRC;
}

int	COSTimeText::GetTrackCount ()
{
	if(m_szSubParserAPI.voGetTrackCount == NULL && m_hSubtitle == NULL)
		return -1;

	m_nTrackNum = m_szSubParserAPI.voGetTrackCount(m_hSubtitle);

	ClearLanguage();

	if(m_nTrackNum <= 0)
		return -1;

	if(m_nTrackNum > 0)
	{
		m_sSubLangInfo.nLanguageCount = m_nTrackNum;

		m_sSubLangInfo.ppLanguage = new VOOSMP_SUBTITLE_LANGUAGE*[m_nTrackNum];
		if(m_sSubLangInfo.ppLanguage == NULL)
			return -1;

		memset(m_sSubLangInfo.ppLanguage, 0, sizeof(VOOSMP_SUBTITLE_LANGUAGE*) * m_nTrackNum);

		int i = 0;				
		for(i = 0; i < m_nTrackNum; i++)
		{
			if(m_szSubParserAPI.voGetTrackInfo == NULL)
				return -1;

			m_sSubLangInfo.ppLanguage[i] = new VOOSMP_SUBTITLE_LANGUAGE();
			if(m_sSubLangInfo.ppLanguage[i] == NULL)
				return -1;
			memset(m_sSubLangInfo.ppLanguage[i], 0, sizeof(VOOSMP_SUBTITLE_LANGUAGE));

			m_szSubParserAPI.voGetTrackInfo(m_hSubtitle, m_sSubLangInfo.ppLanguage[i], i);
		}

		m_nSubLangGot = 1;
	}

	return m_nTrackNum;
}

int	COSTimeText::GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** ppSubLangInfo)
{
	int nRC = VOOSMP_ERR_Retry;

	m_nTrackNum = GetTrackCount();
	if(nRC < 0) return nRC;
	
	*ppSubLangInfo = &m_sSubLangInfo;

	return VOOSMP_ERR_None;
}

int	COSTimeText::GetTrackInfo(int nIndex, VOOSMP_SUBTITLE_LANGUAGE ** ppTrackInfo)
{
	if(m_szSubParserAPI.voGetTrackInfo == NULL && m_hSubtitle == NULL)
		return VOOSMP_ERR_Implement;

	if(m_nSubLangGot == 0)
	{
		m_nTrackNum = GetTrackCount();
		if(m_nTrackNum <= 0) return VOOSMP_ERR_Implement;
	}

	if(nIndex < m_nTrackNum)
	{
		*ppTrackInfo = m_sSubLangInfo.ppLanguage[nIndex];

		return VOOSMP_ERR_None;
	}
	else
	{
		return VOOSMP_ERR_Implement;
	}

	return VOOSMP_ERR_None;
}

int	COSTimeText::SelectTrackByIndex(int nIndex)
{
	if(m_szSubParserAPI.voSelectTrackByIndex == NULL && m_hSubtitle == NULL && m_nSubTitleNum == 0)
		return VOOSMP_ERR_Implement;

	int nRC = VOOSMP_ERR_Implement;

	voCAutoLock lockReadSrc (&m_Lock);
	if(m_nSubLangGot == 0)
	{
		m_nTrackNum = GetTrackCount();
		if(m_nTrackNum <= 0) return m_nTrackNum;
	}

	if(nIndex < m_nTrackNum)
	{
		nRC = m_szSubParserAPI.voSelectTrackByIndex(m_hSubtitle, nIndex);
		
		return nRC;
	}
	else
	{
		return VOOSMP_ERR_Implement;
	}

	return nRC;
}


int	COSTimeText::SetCurPos (long long *pCurPos)
{
	int nFlush = 1;
	voCAutoLock lockReadSrc (&m_Lock);
	int nRC = SetParam(SUBTITLE_PARAMETER_FLUSH_BUFFER, &nFlush);

	return nRC;
}

int	COSTimeText::GetParam (int nID, void * pValue)
{
	if(m_szSubParserAPI.voGetParam == NULL && m_hSubtitle == NULL)
		return VOOSMP_ERR_Implement;

	int nRC = m_szSubParserAPI.voGetParam(m_hSubtitle, nID, pValue);

	return nRC;
}

int	COSTimeText::SetParam (int nID, void * pValue)
{
	if(nID == VOOSMP_PID_COMMON_LOGFUNC)
	{
		m_pbVOLOG = (VO_LOG_PRINT_CB*)pValue;

		return VOOSMP_ERR_None;
	}
	
	if(m_szSubParserAPI.voSetParam == NULL && m_hSubtitle == NULL)
		return VOOSMP_ERR_Implement;

	int nRC = m_szSubParserAPI.voSetParam(m_hSubtitle, nID, pValue);

	return nRC;
}

int	COSTimeText::GetSubTitleNum()
{
	return m_nSubTitleNum;
}

int	COSTimeText::ClearLanguage()
{
	if(m_sSubLangInfo.nLanguageCount > 0 && m_sSubLangInfo.ppLanguage)
	{
		int n;

		for(n = 0; n < m_sSubLangInfo.nLanguageCount; n++)
		{
			if(m_sSubLangInfo.ppLanguage[n])
			{
				delete m_sSubLangInfo.ppLanguage[n];
				m_sSubLangInfo.ppLanguage[n] = NULL;
			}
		}

		delete []m_sSubLangInfo.ppLanguage;
		m_sSubLangInfo.ppLanguage = NULL;

		m_sSubLangInfo.nLanguageCount = 0;
		m_nSubLangGot = 0;
	}

	return 0;
}

int	COSTimeText::LoadDll()
{
	if(m_pLibop == NULL)
		return -1;
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = m_pLibop->LoadLib(m_pLibop->pUserData, "voSubtitleParser", 0);

	if (m_hDll == NULL)
	{
		VOLOGI("Load library voSubtitleParser failed");
		return -1;
	}

	VOGETSUBTITLEPARSERAPI pGetAPI = (VOGETSUBTITLEPARSERAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, "voGetSubTitleParserAPI", 0);

	if (pGetAPI == NULL)
	{
		VOLOGI("get API VOGETSUBTITLEPARSERAPI failed");
		return -1;
	}
	pGetAPI(&m_szSubParserAPI, 0);

	if(m_szSubParserAPI.voInit == NULL)
	{
		VOLOGI("m_szSubParserAPI.voInit  == NULL");
		return -1;
	}

	return 0;
}

int	COSTimeText::unLoadDll()
{
	if (m_hDll != NULL && m_pLibop != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	memset(&m_szSubParserAPI, 0, sizeof(m_szSubParserAPI));

	return 0; 
}
