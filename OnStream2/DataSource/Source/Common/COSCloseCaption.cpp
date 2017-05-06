#include "voString.h"
#include "COSCloseCaption.h"

#define  LOG_TAG    "COSCloseCaption"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

COSCloseCaption::COSCloseCaption(VO_SOURCE2_LIB_FUNC *pLibop, VO_TCHAR* pWorkPath /*= NULL*/)
	: COSBaseSubtitle(pLibop, pWorkPath)
	, m_subtitle(m_alloc)
	, m_pSubInfo(0)
{
	memset(&m_szCloseCaptionAPI, 0, sizeof(VO_CAPTION_PARSER_API));
	memset(&m_sCCInfo, 0, sizeof(voSubtitleInfo));
	LoadDll();

	m_nSubtitlteType = VOOS_SUBTITLE_CLOSECAPTION;
}

COSCloseCaption::~COSCloseCaption ()
{
	Uninit();
	unLoadDll();
}

int	COSCloseCaption::Init(unsigned char* pBuffer, int nLength, int nType)
{
	voCAutoLock lockReadSrc (&m_Lock);

	int nRC = VOOSMP_ERR_Implement;

	if(m_hSubtitle)
		Uninit();

	if(m_szCloseCaptionAPI.Open == NULL)
	{
		LoadDll();
	}

	if( m_szCloseCaptionAPI.Open )
	{
		VO_CAPTION_PARSER_INIT_INFO info;
		info.nType = VO_CAPTION_TYPE_DEFAULT_708;
		info.stDataInfo.nSize = nLength;
		info.stDataInfo.pHeadData = pBuffer;
		info.pReserved = 0;
		info.stDataInfo.nType = 0;
		info.strWorkPath = m_szWorkPath;

		nRC = m_szCloseCaptionAPI.Open( (VO_PTR *)&m_hSubtitle , &info );
	}

	m_subtitle.Flush();

	return nRC;
}

int	COSCloseCaption::Uninit()
{
	int nRC = VOOSMP_ERR_Status;
	
	voCAutoLock lockReadSrc (&m_Lock);
	if( m_hSubtitle != 0 )
	{
		if( m_szCloseCaptionAPI.Close )
			nRC = m_szCloseCaptionAPI.Close( m_hSubtitle );
		m_hSubtitle = 0;
	}

	if(m_pSubInfo)
	{
		destroy_subtitleinfo(m_pSubInfo, m_alloc);
		m_pSubInfo = NULL;
	}

	ClearLanguage();

	return nRC;
}


int	COSCloseCaption::SetInputData(VO_CHAR * pData, int len, int nTime, int nType)
{
	int nRC = VOOSMP_ERR_Implement;
	voCAutoLock lockReadSrc (&m_Lock);

	if(m_hSubtitle == NULL || m_szCloseCaptionAPI.Open == NULL)
	{
		if( nType & VOCC_FLAG_NEW_STREAM)
		{
			nRC = Init((unsigned char *)pData, len, voSubtitleType_ClosedCaption);
			return nRC;
		}
		else
		{
			return nRC;
		}
	}

	VO_CAPTION_DATA_INFO header;
	VO_CAPTION_PARSER_INPUT_BUFFER inbuf;
	VO_CAPTION_PARSER_OUTPUT_BUFFER outbuf;

	if( nType & VOCC_FLAG_NEW_STREAM)
	{
		voCAutoLock lock(&m_mtLanguageInfo);

		m_szCloseCaptionAPI.SetParam( m_hSubtitle , VO_PARAMETER_ID_FLUSH , 0 );

		nRC = m_szCloseCaptionAPI.GetData( m_hSubtitle , &outbuf );
		int cNum = 0;
		while( VO_ERR_PARSER_OK == nRC )
		{
			m_subtitle.AddBuffer( outbuf.pSubtitleInfo );

			nRC = m_szCloseCaptionAPI.GetData( m_hSubtitle , &outbuf );

			cNum++;

			if(cNum > 100)
				break;
		}

		header.nSize = len;
		header.nType = 0;
		header.pHeadData = (VO_PBYTE)pData;

		nRC = m_szCloseCaptionAPI.SetParam( m_hSubtitle , VO_PARAMETER_ID_DATAINFO, &header);

		return nRC;
	}

	inbuf.nSize = len;
	inbuf.nTimeStamp = nTime;
	inbuf.pData = (VO_PBYTE)pData;

	m_mtLanguageInfo.Lock();
	m_szCloseCaptionAPI.Process( m_hSubtitle , &inbuf );
	m_mtLanguageInfo.Unlock();

	nRC = m_szCloseCaptionAPI.GetData( m_hSubtitle , &outbuf );

	if( VO_ERR_PARSER_OK == nRC )
	{
		m_subtitle.AddBuffer( outbuf.pSubtitleInfo );
		
		//voSubtitleInfo *pSample = (voSubtitleInfo *)outbuf.pSubtitleInfo;

		//VOLOGR("m_ccApi.GetData %x, Subtitle Time %d", nRC, (int)pSample->nTimeStamp);
		//if (pSample->pSubtitleEntry)
		//{
		//	if (pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo)
		//	{
		//		if (pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry)
		//		{
		//			if(pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString)
		//			{
		//				VOLOGR("CCCCCC==%s",pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString);
		//			}
		//			else
		//			{
		//				VOLOGR("pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString is null");
		//			}
		//		}
		//		else
		//		{
		//			VOLOGR("pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry is null");
		//		}

		//	}
		//	else
		//	{
		//		VOLOGR("pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo is null");
		//	}
		//}
		//else
		//{
		//	VOLOGR("pSample->pSubtitleEntry is null");
		//}

		if(m_nSubLangGot == 0)
		{
			if(UpdateLanguage() == 0)
				m_nSubLangGot = 1;
		}
	}

	if( nType & VOCC_FLAG_STREAM_EOS )
	{
		voCAutoLock lock(&m_mtLanguageInfo);

		m_szCloseCaptionAPI.SetParam( m_hSubtitle , VO_PARAMETER_ID_FLUSH , 0 );

		nRC = m_szCloseCaptionAPI.GetData( m_hSubtitle , &outbuf );

		int cNum = 0;
		while( VO_ERR_PARSER_OK == nRC )
		{
			m_subtitle.AddBuffer( outbuf.pSubtitleInfo );

			nRC = m_szCloseCaptionAPI.GetData( m_hSubtitle , &outbuf );

			cNum++;

			if(cNum > 100)
				break;
		}
	}

	return 0;
}

int	COSCloseCaption::GetSubtitleInfo(long nTimeCurrent, voSubtitleInfo ** subtitleInfo)
{
	voCAutoLock lockReadSrc (&m_Lock); 
	VO_U32 nRC =  VOOSMP_ERR_Retry;

	memset(&m_sCCInfo, 0, sizeof(voSubtitleInfo));
	voSubtitleInfo *pSample = &m_sCCInfo;

	*subtitleInfo = pSample;

	pSample->nTimeStamp = nTimeCurrent;

	VOLOGR("Get Close Caption %x, time %d, Count %d", pSample, (int)(pSample->nTimeStamp), m_subtitle.GetBuffCount());

	nRC = m_subtitle.GetBuffer( pSample );

	VOLOGR("Get Close Caption return %d, Time %d", nRC, pSample->nTimeStamp);

	if( nRC == VO_RET_SOURCE2_OK )
	{
		//if (pSample->pSubtitleEntry)
		//{
		//	if (pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo)
		//	{
		//		if (pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry)
		//		{
		//			if(pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString)
		//			{
		//				VOLOGR("CCCCCC==%x,%x,%x",pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString[0]
		//				,pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString[1]
		//				,pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString[2]);
		//			}
		//			else
		//			{
		//				VOLOGR("pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry->pString is null");
		//			}
		//		}
		//		else
		//		{
		//			VOLOGR("pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo->pTextInfoEntry is null");
		//		}

		//	}
		//	else
		//	{
		//		VOLOGR("pSample->pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo is null");
		//	}
		//}
		//else
		//{
		//	VOLOGR("pSample->pSubtitleEntry is null");
		//}

		VOLOGR("Get Close Caption return %d, Time %d", nRC, pSample->nTimeStamp);

		
		if(m_pSubInfo == NULL)
		{
			m_pSubInfo = create_subtitleinfo( (voSubtitleInfo*)pSample, m_alloc);
			return nRC;
		}

		if(isequal_subtitleinfo((voSubtitleInfo*)pSample, m_pSubInfo))
		{
			if(pSample->nTimeStamp - m_pSubInfo->nTimeStamp < 100)
				return VOOSMP_ERR_Retry;
		}
		else 
		{
			destroy_subtitleinfo(m_pSubInfo, m_alloc);
			m_pSubInfo = create_subtitleinfo( (voSubtitleInfo*)pSample, m_alloc);
		}
	}

	return nRC;
}

int	COSCloseCaption::GetTrackCount ()
{
	int nRC = UpdateLanguage();
	if(nRC) return 0;

	return m_nTrackNum;
}

int	COSCloseCaption::GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** ppSubLangInfo)
{
	int nRC = VOOSMP_ERR_Retry;

	m_nTrackNum = GetTrackCount();
	if(m_nTrackNum <= 0) return nRC;
	
	*ppSubLangInfo = &m_sSubLangInfo;

	return VOOSMP_ERR_None;
}

int	COSCloseCaption::GetTrackInfo(int nIndex, VOOSMP_SUBTITLE_LANGUAGE ** ppTrackInfo)
{
	if(m_nSubLangGot == 0)
	{
		m_nTrackNum = GetTrackCount();
		if(m_nTrackNum <= 0) return VOOSMP_ERR_Implement;
	}

	if(nIndex < m_nTrackNum && m_sSubLangInfo.ppLanguage)
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

int	COSCloseCaption::SelectTrackByIndex(int nIndex)
{
	int nRC = VOOSMP_ERR_Implement;
	if(m_nSubLangGot == 0)
		return VOOSMP_ERR_Implement;

	if(m_szCloseCaptionAPI.SelectLanguage && m_hSubtitle)
	{
		m_subtitle.Flush();

		nRC = m_szCloseCaptionAPI.SelectLanguage(m_hSubtitle, nIndex);

		VOLOGI("SelectLanguage return %d, m_nLanuageGot %d", nRC, m_nSubLangGot);

		return nRC;
	}

	return nRC;
}


int	COSCloseCaption::SetCurPos (long long *pCurPos)
{
	int nRC = VOOSMP_ERR_Implement;
	if( m_hSubtitle && m_szCloseCaptionAPI.SetParam )
	{
		voCAutoLock lock(&m_mtLanguageInfo);

		m_szCloseCaptionAPI.SetParam( m_hSubtitle , VO_PARAMETER_ID_FLUSH , 0 );

		VO_CAPTION_PARSER_OUTPUT_BUFFER outbuf;
		nRC = m_szCloseCaptionAPI.GetData( m_hSubtitle , &outbuf );

		int cNum = 0;
		while( VO_ERR_PARSER_OK == nRC )
		{
			nRC = m_szCloseCaptionAPI.GetData( m_hSubtitle , &outbuf );

			cNum++;

			if(cNum > 100)
				break;
		}

		m_szCloseCaptionAPI.SetParam( m_hSubtitle, VO_PARAMETER_ID_RESTART , 0 );
	}

	m_subtitle.Flush();

	return 0;
}

int	COSCloseCaption::GetParam (int nID, void * pValue)
{
	int nRC = VOOSMP_ERR_Implement;

	if( m_hSubtitle && m_szCloseCaptionAPI.GetParam )
	{
		nRC = m_szCloseCaptionAPI.GetParam(m_hSubtitle, nID, pValue);
	}

	return nRC;
}

int	COSCloseCaption::SetParam (int nID, void * pValue)
{
	int nRC = VOOSMP_ERR_Implement;

	if( m_hSubtitle && m_szCloseCaptionAPI.SetParam )
	{
		nRC = m_szCloseCaptionAPI.SetParam(m_hSubtitle, nID, pValue);
	}

	return nRC;
}

int	COSCloseCaption::UpdateLanguage()
{
	m_nSubLangGot = 0;
		
	ClearLanguage();

	int nRC = VOOSMP_ERR_Implement;
	if(m_szCloseCaptionAPI.GetLanguage && m_hSubtitle)
	{
		voCAutoLock lock(&m_mtLanguageInfo);

		VO_CAPTION_LANGUAGE_INFO * pLanguageInfo = NULL;
		nRC = m_szCloseCaptionAPI.GetLanguage(m_hSubtitle, &pLanguageInfo);

		VOLOGR("GetLanguage nRC %x", nRC);

		if(nRC == 0)
		{
			VOLOGR("GetLanguage pLanguageInfo->nLangCnt %d", pLanguageInfo->nLangCnt);

			m_nTrackNum = pLanguageInfo->nLangCnt;
			if(m_nTrackNum > 0)
			{
				m_sSubLangInfo.nLanguageCount = m_nTrackNum;
				m_sSubLangInfo.ppLanguage = new VOOSMP_SUBTITLE_LANGUAGE*[m_nTrackNum];
				if(m_sSubLangInfo.ppLanguage == NULL)
					return VOOSMP_ERR_OutMemory;
				memset(m_sSubLangInfo.ppLanguage, 0, sizeof(VOOSMP_SUBTITLE_LANGUAGE*) * m_nTrackNum);

				for(int i = 0; i < m_nTrackNum; i++)
				{
					m_sSubLangInfo.ppLanguage[i] = new VOOSMP_SUBTITLE_LANGUAGE();
					if(m_sSubLangInfo.ppLanguage[i] == NULL)
						return VOOSMP_ERR_OutMemory;
					memset(m_sSubLangInfo.ppLanguage[i], 0, sizeof(VOOSMP_SUBTITLE_LANGUAGE));

					strcpy((char *)m_sSubLangInfo.ppLanguage[i]->szLangName, (char *)pLanguageInfo->pLanguage[i]->chLangName);
				}

				m_nSubLangGot = 1;
			}
		}

		return nRC;
	}
	
	return nRC;
}

int	COSCloseCaption::LoadDll()
{
	if(m_pLibop == NULL)
		return -1;
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = m_pLibop->LoadLib(m_pLibop->pUserData, "voCaptionParser", 0);

	if (m_hDll == NULL)
	{
		VOLOGI("Load library voCaptionParser failed");
		return -1;
	}

	pvoGetCaptionParserAPI pGetAPI = (pvoGetCaptionParserAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, "voGetCaptionParserAPI", 0);

	if (pGetAPI == NULL)
	{
		VOLOGI("get API voGetCaptionParserAPI failed");
		return -1;
	}
	pGetAPI(&m_szCloseCaptionAPI);

	if(m_szCloseCaptionAPI.Open == NULL)
	{
		VOLOGI("m_szSubParserAPI.voInit  == NULL");
		return -1;
	}

	return 0;
}

int	COSCloseCaption::unLoadDll()
{
	if (m_hDll != NULL && m_pLibop != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	memset(&m_szCloseCaptionAPI, 0, sizeof(m_szCloseCaptionAPI));

	return 0; 
}
