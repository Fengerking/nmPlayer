	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoSubtitle.cpp

	Contains:	CVideoSubtitle class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "CVideoSubtitle.h"

#define LOG_TAG "CVideoSubtitle"
#include "voLog.h"
#ifdef _IOS
#include "voLoadLibControl.h"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

typedef VO_S32 (VO_API* pvoGetCaptionParserAPI)(VO_CAPTION_PARSER_API * pParser);

CVideoSubtitle::CVideoSubtitle ()
	: m_nType(-1)
	, m_pSubInfo(NULL)
	, m_Imagetitle(m_alloc)
	, m_subtitle(m_alloc)
	, m_hCC(NULL)
	, m_bCCParser(VO_FALSE)
	, m_hSubhandle(NULL)
	, m_nSubTitleNum(0)
	, m_nSubTitleIndex(0)
	, m_nLanuageGot(0)

{
	memset (&m_ccApi, 0, sizeof(VO_CAPTION_PARSER_API));
	memset(&m_sSubTiltleAPI, 0, sizeof(VO_SUBTITLE_3_API));
	memset(&m_sLanguageInfo, 0, sizeof(VO_CAPTION_LANGUAGE_INFO));

}

CVideoSubtitle::~CVideoSubtitle ()
{
	Uninit ();
}

VO_U32 CVideoSubtitle::Init (unsigned char* pBuffer, int nLength, int nType)
{
	VO_U32 nRC = VO_ERR_NONE;

	voCAutoLock lockReadSrc (&m_Lock);

	if(nType == voSubtitleType_ClosedCaption && m_hSubhandle == NULL)
	{
		Uninit ();

		m_nType = nType;

		if(LoadLib(0))
		{
			if( m_ccApi.Open )
			{
				VO_CAPTION_PARSER_INIT_INFO info;
				info.nType = VO_CAPTION_TYPE_DEFAULT_708;
				info.stDataInfo.nSize = nLength;
				info.stDataInfo.pHeadData = pBuffer;
				info.pReserved = 0;
				info.stDataInfo.nType = 0;

				m_ccApi.Open( (VO_PTR *)&m_hCC , &info );
			}
		}
	}
	else if(nType == voSubtitleType_ExternFile)
	{
		Uninit ();

		m_nType = nType;

		if(LoadLib(0))
		{
			if(m_sSubTiltleAPI.vostInit != NULL)
			{
				nRC = m_sSubTiltleAPI.vostInit(&m_hSubhandle);

				if(m_hSubhandle && m_sSubTiltleAPI.vostSetMediaFile)
				{
					int nRC_S = m_sSubTiltleAPI.vostSetMediaFile(m_hSubhandle, (VO_TCHAR *)pBuffer);

					if(nRC_S == 0) 
					{
						m_nSubTitleNum = m_sSubTiltleAPI.vostGetTrackCount(m_hSubhandle);
					}
					else
					{
						if(m_hSubhandle && m_sSubTiltleAPI.vostUninit)
						{
							m_sSubTiltleAPI.vostUninit(m_hSubhandle);
							m_hSubhandle = NULL;
						}

						memset(&m_sSubTiltleAPI, 0, sizeof(VO_SUBTITLE_3_API));

						FreeLib ();	
					}

				}
			}
		}
	}

	return nRC;
}

VO_U32 CVideoSubtitle::Uninit (void)
{
	if(m_nType == voSubtitleType_ClosedCaption)
	{
		if( m_hCC != 0 )
		{
			if( m_ccApi.Close )
				m_ccApi.Close( m_hCC );
			m_hCC = 0;
		}

		FreeLib ();	

		memset (&m_ccApi, 0, sizeof(VO_CAPTION_PARSER_API));
	}
	else if(m_nType == voSubtitleType_ExternFile)
	{
		if( m_hSubhandle && m_sSubTiltleAPI.vostUninit)
		{
			m_sSubTiltleAPI.vostUninit(m_hSubhandle);
			m_hSubhandle = NULL;
		}

		FreeLib ();	

		memset (&m_sSubTiltleAPI, 0, sizeof(VO_SUBTITLE_3_API));
	}

	if(m_pSubInfo)
	{
		destroy_subtitleinfo(m_pSubInfo, m_alloc);
		m_pSubInfo = NULL;
	}

	UnintLanguage();

	return 0;
}

VO_U32 CVideoSubtitle::SetInputData (unsigned char* pBuffer, int nLength, int nTime, int nFlag)
{
	VO_U32 nRC = 0;
	voCAutoLock lockReadSrc (&m_Lock);

	if(nFlag&VOSUB_FLAG_EXTERNIMAGE)
	{
		//VOLOGI("Add Image Caption, pBuffer %x, nLength, %d, Time %d", pBuffer, nLength, nTime);
		nRC = HandleImage(pBuffer, nLength, nTime, nFlag);
		return nRC;
	}

	if(m_bCCParser == VO_FALSE)
		return 0;
	
	if(nFlag&VOSUB_FLAG_CLOSECAPTION)
	{
		//VOLOGI("Add Close Caption, pBuffer %x, nLength, %d, Time %d", pBuffer, nLength, nTime);
		nRC = HandleCC(pBuffer, nLength, nTime, nFlag);
	}


	if(nFlag&VOSUB_FLAG_EXTERNTEXT)
	{
		nRC = HandleText(pBuffer, nLength, nTime, nFlag);
	}

	return nRC;
}

VO_U32 CVideoSubtitle::HandleCC(unsigned char* pBuffer, int nLength, int nTime, int nFlag)
{
	int nRC = 0;

	if(m_hCC == NULL || m_ccApi.Open == NULL)
	{
		if( nFlag & VOSUB_FLAG_NEW_STREAM)
		{
			Init(pBuffer, nLength, voSubtitleType_ClosedCaption);
			return 0;
		}
		else
		{
			return -1;
		}
	}

	VO_CAPTION_DATA_INFO header;
	VO_CAPTION_PARSER_INPUT_BUFFER inbuf;
	VO_CAPTION_PARSER_OUTPUT_BUFFER outbuf;

	if( nFlag & VOSUB_FLAG_NEW_STREAM)
	{
		m_ccApi.SetParam( m_hCC , VO_PARAMETER_ID_FLUSH , 0 );

		nRC = m_ccApi.GetData( m_hCC , &outbuf );

		int cNum = 0;
		while( VO_ERR_PARSER_OK == nRC )
		{
			AddSubtitle( outbuf.pSubtitleInfo );

			nRC = m_ccApi.GetData( m_hCC , &outbuf );

			cNum++;

			if(cNum > 100)
				break;
		}

		header.nSize = nLength;
		header.nType = 0;
		header.pHeadData = pBuffer;

		m_ccApi.SetParam( m_hCC , VO_PARAMETER_ID_DATAINFO, &header);

		return 0;
	}

	inbuf.nSize = nLength;
	inbuf.nTimeStamp = nTime;
	inbuf.pData = pBuffer;

	m_ccApi.Process( m_hCC , &inbuf );

	nRC = m_ccApi.GetData( m_hCC , &outbuf );

	if( VO_ERR_PARSER_OK == nRC )
	{
		AddSubtitle( outbuf.pSubtitleInfo );
		
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

		if(m_nLanuageGot == 0)
		{
			if(UpdateLanguage() == 0)
				m_nLanuageGot = 1;

			//VOLOGI("UpdateLanguage m_nLanuageGot %d", m_nLanuageGot);
		}
	}

	if( nFlag & VOSUB_FLAG_STREAM_EOS )
	{
		m_ccApi.SetParam( m_hCC , VO_PARAMETER_ID_FLUSH , 0 );

		nRC = m_ccApi.GetData( m_hCC , &outbuf );

		int cNum = 0;
		while( VO_ERR_PARSER_OK == nRC )
		{
			AddSubtitle( outbuf.pSubtitleInfo );

			nRC = m_ccApi.GetData( m_hCC , &outbuf );

			cNum++;

			if(cNum > 100)
				break;
		}
	}

	return 0;
}


VO_U32 CVideoSubtitle::HandleImage(unsigned char* pBuffer, int nLength, int nTime, int nFlag)
{
	voSubtitleInfo      varSubtitleInfo;
	SubtitleInfoEntry   varSubtitleInfoEntry;
	voSubtitleDisplayInfo    varSubtitleDisplayInfo;
	voSubtitleImageInfo      varSubtitleImageInfo;
	voSubtitleImageInfoData  varSubtitleImageInfoData;
	voSubtitleImageInfoDescriptor   varSubtitleImagInfoDesc;

	memset(&varSubtitleInfo, 0, sizeof(voSubtitleInfo));	
	memset(&varSubtitleInfoEntry, 0, sizeof(SubtitleInfoEntry));
	memset(&varSubtitleDisplayInfo, 0, sizeof(voSubtitleDisplayInfo));
	memset(&varSubtitleImageInfo, 0, sizeof(voSubtitleImageInfo));
	memset(&varSubtitleImageInfoData, 0, sizeof(voSubtitleImageInfoData));
	memset(&varSubtitleImagInfoDesc, 0, sizeof(varSubtitleImagInfoDesc));

	varSubtitleImageInfoData.nSize = nLength;
	varSubtitleImageInfoData.pPicData = pBuffer;
	varSubtitleImageInfoData.nType = VO_IMAGE_JPEG;
	varSubtitleImageInfoData.nWidth = 0;
	varSubtitleImageInfoData.nHeight = 0;

	varSubtitleImageInfo.stImageData = varSubtitleImageInfoData;

	varSubtitleDisplayInfo.pImageInfo = &varSubtitleImageInfo;

	varSubtitleInfoEntry.stSubtitleDispInfo = varSubtitleDisplayInfo;
	varSubtitleInfoEntry.nDuration = 0xffffffff;

	varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nBottom = 0xffffffff;
	varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nLeft = 0xffffffff;
	varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nRight = 0xffffffff;
	varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nTop = 0xffffffff;

	varSubtitleInfo.nTimeStamp = nTime;
	varSubtitleInfo.pSubtitleEntry = &varSubtitleInfoEntry;

	AddImage(&varSubtitleInfo);

	return 0;
}
	
VO_U32 CVideoSubtitle::HandleText(unsigned char* pBuffer, int nLength, int nTime, int nFlag)
{
	voSubtitleInfo      varSubtitleInfo;
	SubtitleInfoEntry   varSubtitleInfoEntry;
	voSubtitleDisplayInfo    varSubtitleDisplayInfo;
	voSubtitleTextRowInfo	  varSubtitleTextRowInfo;
	voSubtitleTextInfoEntry	  varSubtitleTextInfoEntry;

	memset(&varSubtitleInfo, 0, sizeof(voSubtitleInfo));	
	memset(&varSubtitleInfoEntry, 0, sizeof(SubtitleInfoEntry));
	memset(&varSubtitleDisplayInfo, 0, sizeof(voSubtitleDisplayInfo));
	memset(&varSubtitleTextRowInfo, 0, sizeof(voSubtitleTextRowInfo));
	memset(&varSubtitleTextInfoEntry, 0, sizeof(voSubtitleTextInfoEntry));

	varSubtitleTextInfoEntry.nSize = nLength;
	varSubtitleTextInfoEntry.pString = pBuffer;

	varSubtitleTextRowInfo.pTextInfoEntry = &varSubtitleTextInfoEntry;

	varSubtitleDisplayInfo.pTextRowInfo = &varSubtitleTextRowInfo;

	varSubtitleInfoEntry.stSubtitleDispInfo = varSubtitleDisplayInfo;
	varSubtitleInfoEntry.nDuration = 0xffffffff;

	varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nBottom = 0xffffffff;
	varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nLeft = 0xffffffff;
	varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nRight = 0xffffffff;
	varSubtitleInfoEntry.stSubtitleRectInfo.stRect.nTop = 0xffffffff;

	varSubtitleInfo.nTimeStamp = nTime+1;
	varSubtitleInfo.pSubtitleEntry = &varSubtitleInfoEntry;

	AddSubtitle(&varSubtitleInfo);

	return 0;
}

VO_U32 CVideoSubtitle::GetOutputImage ( voSubtitleInfo *pSample)
{
	voCAutoLock lockReadSrc (&m_Lock);

	VO_U32 nRC =  VO_RET_SOURCE2_NEEDRETRY;

	nRC = m_Imagetitle.GetBuffer( pSample );

	return nRC;
}

VO_U32 CVideoSubtitle::GetOutputData (voSubtitleInfo *pSample)
{
	voCAutoLock lockReadSrc (&m_Lock); 
	VO_U32 nRC =  VO_RET_SOURCE2_NEEDRETRY;

	 if(m_nType == voSubtitleType_ExternFile)
	 {
		 voSubtitleInfo * pSubSample = (voSubtitleInfo *)pSample;
		 nRC = m_sSubTiltleAPI.vostGetSubtitleObject(m_hSubhandle, (long)pSubSample->nTimeStamp, (voSubtitleInfo **)&pSubSample, m_nSubTitleIndex);

		 if (nRC == 0)
		 {
			 memcpy(pSample,pSubSample,sizeof(voSubtitleInfo));
			 return nRC;
		 }
	 }

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
			if(((voSubtitleInfo*)pSample)->nTimeStamp - m_pSubInfo->nTimeStamp < 100)
				return VO_RET_SOURCE2_NEEDRETRY;
		}
		else 
		{
			destroy_subtitleinfo(m_pSubInfo, m_alloc);
			m_pSubInfo = create_subtitleinfo( (voSubtitleInfo*)pSample, m_alloc);
		}
	}

	return nRC;
}

VO_U32 CVideoSubtitle::Flush (void)
{
	int nRC = 0;

	voCAutoLock lockReadSrc (&m_Lock);
	if(m_nType == voSubtitleType_ClosedCaption)
	{
		if( m_hCC && m_ccApi.SetParam )
		{
			VO_CAPTION_PARSER_OUTPUT_BUFFER outbuf;

			m_ccApi.SetParam( m_hCC , VO_PARAMETER_ID_FLUSH , 0 );

			nRC = m_ccApi.GetData( m_hCC , &outbuf );

			int cNum = 0;
			while( VO_ERR_PARSER_OK == nRC )
			{
				nRC = m_ccApi.GetData( m_hCC , &outbuf );

				cNum++;

				if(cNum > 100)
					break;
			}
			
			m_ccApi.SetParam( m_hCC , VO_PARAMETER_ID_RESTART , 0 );
		}
	}

	m_subtitle.Flush();
	m_Imagetitle.Flush();
	HandleText(NULL, 0, 0, 0);


	return 0;
}

VO_U32 CVideoSubtitle::AddImage( voSubtitleInfo * pSample )
{
	int nRC = m_Imagetitle.AddBuffer( pSample );

	return nRC;
}

VO_U32 CVideoSubtitle::AddSubtitle( voSubtitleInfo * pSample )
{
	//VOLOGI("Add Close Caption %x, Count %d", pSample, m_subtitle.GetBuffCount());

	int nRC = m_subtitle.AddBuffer( pSample );

	VOLOGR("Add Close Caption %x, Count %d", pSample, m_subtitle.GetBuffCount());

	return nRC;
}

VO_U32 CVideoSubtitle::Enable (VO_BOOL bEnable)
{
	if(m_bCCParser == VO_TRUE && bEnable == VO_FALSE)
		Flush();

	m_bCCParser = bEnable;	
	return 0;
}

VO_U32 CVideoSubtitle::ReSet()
{
	if(m_nType == voSubtitleType_ClosedCaption)
	{
		if(m_ccApi.Open && m_hCC)
		{
			m_ccApi.SetParam( m_hCC , VO_PARAMETER_ID_FLUSH , 0 );

			VO_CAPTION_PARSER_OUTPUT_BUFFER outbuf;
			VO_U32 ret = m_ccApi.GetData( m_hCC , &outbuf );

			int cNum = 0;
			while( VO_ERR_PARSER_OK == ret )
			{
				AddSubtitle( outbuf.pSubtitleInfo );

				ret = m_ccApi.GetData( m_hCC , &outbuf );

				cNum++;

				if(cNum > 100)
					break;
			}

			m_ccApi.Close(m_hCC);
			m_hCC = NULL;
		}
	}

	return 0;
}

VO_U32 CVideoSubtitle::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	int Ret = 0;
	voCAutoLock lockReadSrc (&m_Lock);

	if(uParamID == ID_SET_JAVA_ENV)
	{
		if(m_sSubTiltleAPI.vostSetParameter != NULL && m_hSubhandle)
			return m_sSubTiltleAPI.vostSetParameter(m_hSubhandle, uParamID, (long)pData);
	}

	return 	Ret;
}

VO_U32 CVideoSubtitle::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	int Ret = 0;
	voCAutoLock lockReadSrc (&m_Lock);

	switch(uParamID)
	{
	case VOSUB_GETIMAGE_NUMBER:
		if(pData)
			*((int *)pData) = m_Imagetitle.GetBuffCount();
		break;
	}	

	return Ret;	
}

VO_U32 CVideoSubtitle::UpdateLanguage()
{
	m_nLanuageGot = 0;
		
	UnintLanguage();

	int nRC = VO_ERR_FAILED;
	if(m_ccApi.GetLanguage && m_hCC)
	{
		VO_U32 i;
		VO_CAPTION_LANGUAGE_INFO*	pLanguageInfo = NULL;

		nRC = m_ccApi.GetLanguage(m_hCC, &pLanguageInfo);

		VOLOGR("GetLanguage nRC %x", nRC);

		if(pLanguageInfo)
		{
			VOLOGR("GetLanguage pLanguageInfo->nLangCnt %d", pLanguageInfo->nLangCnt);
		}

		if(nRC == 0)
		{
			m_sLanguageInfo.nLangCnt = pLanguageInfo->nLangCnt;
			m_sLanguageInfo.pReserved = pLanguageInfo->pReserved;

			m_sLanguageInfo.pLanguage = new VO_CAPTION_LANGUAGE*[m_sLanguageInfo.nLangCnt];

			memset(m_sLanguageInfo.pLanguage, 0, sizeof(VO_CAPTION_LANGUAGE*) * m_sLanguageInfo.nLangCnt);

			for(i = 0; i < m_sLanguageInfo.nLangCnt; i++)
			{
				m_sLanguageInfo.pLanguage[i] = new VO_CAPTION_LANGUAGE();

				memcpy(m_sLanguageInfo.pLanguage[i], pLanguageInfo->pLanguage[i], sizeof(VO_CAPTION_LANGUAGE));
			}

			m_nLanuageGot = 1;

			return 0;
		}

		return nRC;
	}
	
	return nRC;
}

VO_U32 CVideoSubtitle::UnintLanguage()
{
	if(m_sLanguageInfo.pLanguage)
	{
		VO_U32 i;
		for(i = 0; i < m_sLanguageInfo.nLangCnt; i++)
		{
			if(m_sLanguageInfo.pLanguage[i])
			{
				delete m_sLanguageInfo.pLanguage[i];
				m_sLanguageInfo.pLanguage[i] = NULL;
			}
		}

		delete []m_sLanguageInfo.pLanguage;
		m_sLanguageInfo.pLanguage = NULL;
		m_sLanguageInfo.nLangCnt = 0;
		m_nLanuageGot = 0;
	}

	return 0;
}

VO_U32 CVideoSubtitle::GetLanguageNum(VO_U32 *pNum)
{
	if(pNum)
	{
		int nRC = UpdateLanguage();
		if(nRC) return nRC;
		*pNum = m_sLanguageInfo.nLangCnt;
		return 0;
	}

	return VO_ERR_FAILED;
}

VO_U32 CVideoSubtitle::GetLanguageItem(VO_U32 nNum, VO_CAPTION_LANGUAGE** pLangage)
{
	if(pLangage == NULL)
		return VO_ERR_FAILED;

	if(m_nLanuageGot == 0)
	{
		int nRC = UpdateLanguage();
		if(nRC) return nRC;
	}

	if(nNum >= m_sLanguageInfo.nLangCnt || m_sLanguageInfo.pLanguage == NULL)
		return VO_ERR_FAILED;

	*pLangage = m_sLanguageInfo.pLanguage[nNum];

	return 0;	
}


VO_U32 CVideoSubtitle::SelectLanguage(VO_U32 nLangNum)
{
	VOLOGI("SelectLanguage nLangNum %d, m_nLanuageGot %d", nLangNum, m_nLanuageGot);
	
	if(m_nLanuageGot == 0)
		return VO_ERR_FAILED;

	if(m_ccApi.SelectLanguage && m_hCC)
	{
		m_subtitle.Flush();
		HandleText(NULL, 0, 0, 0);

		int nRC = m_ccApi.SelectLanguage(m_hCC, nLangNum);

		VOLOGI("SelectLanguage return %d, m_nLanuageGot %d", nRC, m_nLanuageGot);

		return nRC;
	}

	return VO_ERR_FAILED;
}


VO_U32 CVideoSubtitle::LoadLib (VO_HANDLE hInst)
{
	if(m_nType == voSubtitleType_ClosedCaption)
	{
		vostrcpy(m_szDllFile, _T("voCaptionParser"));
		vostrcpy(m_szAPIName, _T("voGetCaptionParserAPI"));
	}
	else if(m_nType == voSubtitleType_ExternFile)
	{
		vostrcpy(m_szDllFile, _T("voSubtitleParser"));
		vostrcpy(m_szAPIName, _T("voGetSubtitle3API"));
	}

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, ".so");
#endif

	if(CDllLoad::LoadLib(NULL) == 0)
	{
		VOLOGE( "load %s fail!", m_szDllFile);
		return 0;
	}

	VOLOGR( "Load library successfully %s, SubTitle Type %d",  m_szDllFile, m_nType);

	if(m_nType == voSubtitleType_ClosedCaption)
	{
		pvoGetCaptionParserAPI pAPI = (pvoGetCaptionParserAPI)m_pAPIEntry;
		if (pAPI == NULL)
		{
			VOLOGE( "get api %d fail!",  m_szAPIName);
			return 0;
		}

		pAPI( &m_ccApi );

		VOLOGI( "get api OK");
	}
	else if(m_nType == voSubtitleType_ExternFile)
	{
		VOGETSUBTITLE3API pGetAPI = (VOGETSUBTITLE3API)m_pAPIEntry;
		if (pGetAPI == NULL)
		{
			VOLOGE( "get api %d fail!",  m_szAPIName);
			return 0;
		}

		pGetAPI( &m_sSubTiltleAPI, 0);

		VOLOGI( "get api OK");
	}

	return 1;
}
