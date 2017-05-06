#include "COSBaseSubtitle.h"

#define  LOG_TAG    "COSBaseSubtitle"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

COSBaseSubtitle::COSBaseSubtitle(VO_SOURCE2_LIB_FUNC *pLibop, VO_TCHAR* pWorkPath  /*= NULL*/)
	: m_pLibop(pLibop)
	, m_pbVOLOG(NULL)
	, m_hDll(NULL)	
	, m_hSubtitle(NULL)
	, m_nSubTitleNum(0)
	, m_nTrackNum(0)
	, m_nTrackIndex(0)
	, m_nSubtitlteType(0)
{
	memset(&m_szWorkPath, 0, 512*sizeof (VO_TCHAR));
	memset(&m_sSubLangInfo, 0, sizeof(VOOSMP_SUBTITLE_LANGUAGE_INFO));
	if (pWorkPath) vostrcpy(m_szWorkPath, pWorkPath);
}

COSBaseSubtitle::~COSBaseSubtitle ()
{
	ClearLanguage();
}

int	COSBaseSubtitle::Init(unsigned char* pBuffer, int nLength, int nType)
{
	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::Uninit()
{
	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::SetTimedTextFilePath(VO_TCHAR * pPath, int nType)
{
	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::SetInputData(VO_CHAR * pData, int len, int nTime, int nType)
{
	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::GetSubtitleInfo(long nTimeCurrent, voSubtitleInfo ** subtitleInfo)
{
	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::GetTrackCount ()
{
	return m_nTrackNum;
}

int	COSBaseSubtitle::GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** ppSubLangInfo)
{
	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::GetTrackInfo(int nIndex, VOOSMP_SUBTITLE_LANGUAGE ** ppTrackInfo)
{
	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::SelectTrackByIndex(int nIndex)
{
	return VOOSMP_ERR_Implement;
}


int	COSBaseSubtitle::SetCurPos (long long *pCurPos)
{
	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::GetParam (int nID, void * pValue)
{
	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::SetParam (int nID, void * pValue)
{
	if(nID == VOOSMP_PID_COMMON_LOGFUNC)
	{
		m_pbVOLOG = (VO_LOG_PRINT_CB*)pValue;

		return VOOSMP_ERR_None;
	}

	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::GetSubTitleNum()
{
	return m_nSubTitleNum;
}

int	COSBaseSubtitle::UpdateLanguage()
{
	return VOOSMP_ERR_Implement;
}

int	COSBaseSubtitle::ClearLanguage()
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

int	COSBaseSubtitle::LoadDll()
{
	if(m_pLibop == NULL)
		return -1;

	return -1;
}

int	COSBaseSubtitle::unLoadDll()
{
	return 0; 
}
