/************************************************************************
VisualOn Proprietary
Copyright (c) 2003, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#include "voVMAPParser.h"
#include "CVMAPParser.h"
#include "CVMAPTag.h"
#include "voLog.h"
#include "CVASTParser.h"
#include "CVASTTag.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define STRING_

CVMAPParser::CVMAPParser()
: m_pContentURI(NULL)
#if 0
, m_hDll(NULL)
, m_hVast(NULL)
, m_pVastData(NULL)
#endif
, m_pVastParser(NULL)
{
	memset(&m_VMAPData,0,sizeof(VOAdInfo));
#if 0
	memset(&m_hVastAPI,0,sizeof(VO_VAST_PARSER_API));
#endif
}

CVMAPParser::~CVMAPParser()
{
	Close();
}

VO_U32 CVMAPParser::Open(VO_TCHAR *pWorkPath)
{
	SetWorkPath(pWorkPath);
	VOLOGINIT(pWorkPath);
	VO_U32 ret = VO_RET_SOURCE2_OK;
	if (VO_ERR_NONE != (ret = CXMLLoad::OpenParser()))
	{
		return ret;
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::Close()
{
	Reset();
	VO_U32 ret = VO_RET_SOURCE2_OK;

	if (VO_ERR_NONE != (ret = CXMLLoad::CloseParser()))
	{
		VOLOGUNINIT();
		return ret;
	}
	VOLOGUNINIT();
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::Reset()
{
	if (NULL != m_pContentURI)
	{
		delete m_pContentURI;
		m_pContentURI = NULL;
	}
	ReleaseVMAPData();
	CloseVastParser();

	return VO_RET_SOURCE2_OK;
}
VO_U32 CVMAPParser::Process(VO_PTR pBuffer, VO_U32 uSize)
{
	Reset();
	if(LoadXML((char*)pBuffer, int(uSize),voXML_FLAG_SOURCE_BUFFER) != VO_ERR_NONE)
	{
		VOLOGE("Load VAMP XML failed:pBuffer=%x:uSize=%d",pBuffer,uSize);
		return VO_RET_SOURCE2_FAIL;
	}
	
	VO_VOID* pRootNode = NULL;
	GetFirstChild(NULL,&pRootNode);
	if (NULL == pRootNode)
	{
		VOLOGE("Can not find the root XML node");
		return VO_RET_SOURCE2_FAIL;
	}

	LoadTag(pRootNode,&m_VMAPData,&CVMAPParser::HandleVmapRoot);
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::GetData(VOAdInfo** ppData)
{
	if (NULL != ppData)
	{
		*ppData = &m_VMAPData;
	}
	return VO_RET_SOURCE2_OK;
}
VO_U32 CVMAPParser::SetParam(VO_U32 uID, VO_PTR pParam)
{
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::GetParam(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_PID_VMAP_CONTENT_URI:
		{
			VO_PCHAR* pTmp = (VO_PCHAR*)pParam;
			*pTmp = m_pContentURI;
		}
		break;
	case VO_PID_COMMON_LOGFUNC:
		{
		//	VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
		//	vologInit (pVologCB->pUserData, pVologCB->fCallBack);
		}
		break;
	default:
		break;
	}
	return VO_RET_SOURCE2_OK;
}




VO_U32 CVMAPParser::LoadAdbreakAttri(VO_VOID *pAdbreakNode, pVOAdBreak pAdBreak)
{
	if (NULL == pAdbreakNode)
	{
		return VO_RET_SOURCE2_OK;
	}
	VO_PCHAR str = NULL;
	int Size = 0;

	GetAttributeValue(pAdbreakNode,(VO_CHAR*)VMAPAttriBreakType,&str,Size);
	if (NULL != str && Size > 0)
	{
		if (NULL != strstr(str,VMAPAttriBreakTypeLin))
		{
			pAdBreak->BreakType |= VOAdBreakType_Linear; 
		}
		if (NULL != strstr(str,VMAPAttriBreakTypeNonLin))
		{
			pAdBreak->BreakType |= VOAdBreakType_NonLinear; 
		}
		if (NULL != strstr(str,VMAPAttriBreakTypeComp))
		{
			pAdBreak->BreakType |= VOAdBreakType_Companion; 
		}
		if (NULL != strstr(str,VMAPAttriBreakTypeSkippable))
		{
			pAdBreak->BreakType |= VOAdBreakType_Skippable_Linear; 
		}
		if (NULL != strstr(str,VMAPAttriBreakTypeAdPods))
		{
			pAdBreak->BreakType |= VOAdBreakType_Ad_Pods; 
		}
	}

	GetAttributeValue(pAdbreakNode,(VO_CHAR*)VMAPAttriBreakId,&str,Size);
	if (NULL != str && Size > 0)
	{
		pAdBreak->strBreakID = new VO_CHAR[Size + 1];
		strncpy(pAdBreak->strBreakID,str,Size);
		pAdBreak->strBreakID[Size] = 0;
	}

	GetAttributeValue(pAdbreakNode,(VO_CHAR*)VMAPAttriTimeOffset,&str,Size);
	if (NULL != str && Size > 0)
	{
		VO_CHAR* pTmp = NULL;
		VO_U32 uHour = 0, uMin = 0, uSec = 0, uMilliSec = 0;
		if (NULL != (pTmp = strstr(str,":")))
		{
			if(12 == Size)
			{
				pAdBreak->Time.TimeIndex = VOAdTimeType_Time;
				::sscanf (str, "%2d:%02d:%02d.%03d", &uHour, &uMin, &uSec, &uMilliSec);
				pAdBreak->Time.ullTime = (uHour*3600 + uMin * 60 + uSec)*1000 + uMilliSec;
			}
			else if(9 == Size)
			{	
				pAdBreak->Time.TimeIndex = VOAdTimeType_Time;
				::sscanf (str, "%2d:%02d:%02d", &uHour, &uMin, &uSec);
				pAdBreak->Time.ullTime = (uHour*3600 + uMin * 60 + uSec)*1000 + uMilliSec;
			}
			else
			{
				pAdBreak->Time.TimeIndex = VOAdTimeType_Unknow;
				pAdBreak->Time.ullTime = 0;
			}
		}
		else if (NULL != (pTmp = strstr(str,"%")))
		{
			pAdBreak->Time.TimeIndex = VOAdTimeType_Percentage;
			pTmp[0] = 0;
			pAdBreak->Time.ullTime = atoi(str);
		}
		else if ((NULL != (pTmp = strstr(str,"start"))) || (NULL != (pTmp = strstr(str,"end"))))
		{
			pAdBreak->Time.TimeIndex = VOAdTimeType_StartEnd;
			pAdBreak->Time.ullTime = (NULL == strstr(str,"start")) ? 1 : 0;
		}
		else
		{
			VO_U32 uCnt = 0;
			while (uCnt < (VO_U32)Size)
			{
				if (str[uCnt] < '0' || str[uCnt] > '9')
				{
					break;
				}
				uCnt++;
			}
			if (uCnt == Size)
			{
				pAdBreak->Time.TimeIndex = VOAdTimeType_Positon;
				pAdBreak->Time.ullTime = atoi(str);
			}
			else
			{
				pAdBreak->Time.TimeIndex = VOAdTimeType_Unknow;
				pAdBreak->Time.ullTime = 0;
			}
			
		}

	}

	GetAttributeValue(pAdbreakNode,(VO_CHAR*)VMAPAttriRepeatAfter,&str,Size);
	if (NULL != str && Size > 0)
	{
		VO_U32 uHour = 0, uMin = 0, uSec = 0, uMilliSec = 0;
		if(12 == Size)
		{
			::sscanf (str, "%2d:%02d:%02d.%03d", &uHour, &uMin, &uSec, &uMilliSec);
		}
		else if(9 == Size)
		{	
			::sscanf (str, "%2d:%02d:%02d", &uHour, &uMin, &uSec);
		}
		pAdBreak->ullRepeatAfter = (uHour*3600 + uMin * 60 + uSec)*1000 + uMilliSec;
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::LoadAdSourceAttri(VO_VOID *pAdSourceNode, pVOAdSource pAdSource)
{
	if (NULL == pAdSourceNode)
	{
		return VO_RET_SOURCE2_OK;
	}
	VO_PCHAR str = NULL;
	int Size = 0;

	GetAttributeValue(pAdSourceNode,(VO_CHAR*)VMAPAttriSourceID,&str,Size);
	if (NULL != str && Size > 0)
	{
		pAdSource->uID = atoi(str);
	}
	GetAttributeValue(pAdSourceNode,(VO_CHAR*)VMAPAttriSourceAllowMultipleAds,&str,Size);
	if (NULL != str && Size > 0)
	{
		pAdSource->bAllowMultipleAds = ((0 == strcmp(str,"true")) ? VO_TRUE : VO_FALSE);
	}
	GetAttributeValue(pAdSourceNode,(VO_CHAR*)VMAPAttriSourceFollowRedirects,&str,Size);
	if (NULL != str && Size > 0)
	{
		pAdSource->bFollowRedirects = ((0 == strcmp(str,"true")) ? VO_TRUE : VO_FALSE);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::LoadAdData(VO_VOID *pAdDataNode, pVOAdData pAdData)
{
	if (NULL == m_pVastParser && VO_RET_SOURCE2_OK != OpenVastParser())
	{
		return VO_RET_SOURCE2_OK;
	}

	m_pVastParser->ProcessByTag(pAdDataNode);
	VOAdData* pVast = m_pVastParser->GetVastData();
	if (pVast)
	{
		memcpy(pAdData,pVast,sizeof(VOAdData));
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::OpenVastParser()
{
#if 0
	if (NULL != m_hDll)
	{
		return VO_RET_SOURCE2_OK;
	}
	
	m_hDll = new CDllLoad;

	m_hDll->SetWorkPath(m_pWorkPath);
	vostrcpy(m_hDll->m_szDllFile, _T("voVASTParser"));
	vostrcpy(m_hDll->m_szAPIName, _T("voGetVASTParserAPI"));

#if defined _WIN32
	vostrcat(m_hDll->m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_hDll->m_szDllFile, _T(".so"));
#elif defined _MAC_OS
	vostrcat(m_hDll->m_szDllFile, _T(".dylib"));
#elif defined _IOS
	{
		voGetVASTParserAPI (&m_hVastAPI);
	}
#endif

	if(m_hDll->LoadLib(NULL) == 0)
	{
		VOLOGE ("Load voVASTParser fail");
		CloseVastParser();
		return VO_RET_SOURCE2_NOLIBOP;
	}

	VOGETVASTPARSERAPI pVastAPI = (VOGETVASTPARSERAPI) m_hDll->m_pAPIEntry;
	if (pVastAPI == NULL)
	{
		CloseVastParser();
		return VO_RET_SOURCE2_NOLIBOP;
	}
	pVastAPI (&m_hVastAPI);

	m_hVastAPI.Open(&m_hVast,m_pWorkPath);
#else
	if (NULL != m_pVastParser)
	{
		CloseVastParser();
	}
	m_pVastParser = new CVASTParser;
	m_pVastParser->Open(m_pWorkPath);
#endif
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::CloseVastParser()
{
#if 0
	if (NULL != m_hVastAPI.Close)
	{
		m_hVastAPI.Close(m_hVast);
	}
	if (NULL == m_hDll)
	{
		return VO_RET_SOURCE2_OK;
	}
	m_hDll->FreeLib();
	delete m_hDll;
	m_hDll = NULL;
#else
	if (NULL != m_pVastParser)
	{
		m_pVastParser->Close();
		delete m_pVastParser;
		m_pVastParser = NULL;
	}
#endif
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::ReleaseVMAPData()
{
	pVOAdBreak pBreak = m_VMAPData.pAdBreak;
	for (VO_U32 uCnt = 0;((uCnt < m_VMAPData.uAdBreakCount) && (NULL != pBreak));uCnt++)
	{
		pVOAdSource pSource = pBreak->pAdSource;

		pVOAdData pData = pSource->pAdData;
		for (VO_U32 uCnt = 0;((uCnt < pSource->uAdDataCount) && (NULL != pData));uCnt++)
		{
			m_pVastParser->ReleaseVASTData(pData);

			pVOAdData pTmp = pData->pNext;
			delete pData;
			pData = pTmp;
		}

		if (NULL != pSource->strAdTagURI)
		{
			delete pSource->strAdTagURI;
		}
		if (NULL != pSource->strCustomAdData)
		{
			delete pSource->strCustomAdData;
		}
		delete pSource;

		if (pBreak->strBreakID)
		{
			delete pBreak->strBreakID;
		}
		
		pVOAdBreak pTmp = pBreak->pNext;
		delete pBreak;
		pBreak = pTmp;
	}
	m_VMAPData.uAdBreakCount = 0;
	m_VMAPData.pAdBreak = NULL;

	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::LoadTag(VO_PTR pNode, VO_PTR pData ,pHandleData pHandle)
{
	if (NULL == pNode || NULL == pData)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_VOID* pTageNode = NULL;
	VO_PCHAR str = NULL;
	int Size = 0;

	VO_VOID* pSibleTageNode = NULL;
	while(1)
	{
		if (NULL == pTageNode)
		{
			GetFirstChild (pNode,&pTageNode);
		}
		else
		{
			GetNextSibling(pTageNode,&pSibleTageNode);
			pTageNode = pSibleTageNode;
		}
		if (NULL == pTageNode)
		{
			break;
		}
		str = NULL;
		Size = 0;
		GetTagName(pTageNode,&str,Size);
		if (str > 0 && Size > 0)
		{
			(this->*pHandle)(pTageNode,pData,str,Size);
		}
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::HandleVmapRoot(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR pStr = NULL;
	int Size = 0;

	VOAdInfo* pVmapData = (VOAdInfo*)pData;
	if (0 == strcmp(strTage,VMAPTagExtensions))
	{
		VO_VOID *pChildTageNode = NULL;
		GetChildByName(pNode,(VO_CHAR*)VMAPTagUnicornOnce,&pChildTageNode);
		if (NULL != pChildTageNode)
		{
			GetAttributeValue(pChildTageNode,(VO_CHAR*)VMAPAttriContenturi,&pStr,Size);
			if (Size > 0 && NULL != pStr)
			{
				m_pContentURI = new VO_CHAR[strlen(pStr) + 1];
				strcpy(m_pContentURI,pStr);
				VOLOGE("m_pContentURI before=%s",m_pContentURI);
				DecodeCharacterEntities(m_pContentURI);
				VOLOGE("m_pContentURI after =%s",m_pContentURI);
			}
			else
			{
				VOLOGE("Can not find VMAPAttriContenturi");
			}
		}
		else
		{
			VOLOGE("Can not find VMAPTagUnicornOnce node");
		}
	}
	else if (0 == strcmp(strTage,VMAPTagAdBreak))
	{
		++pVmapData->uAdBreakCount;
		pVOAdBreak pAdBreak = new VOAdBreak;
		memset(pAdBreak,0,sizeof(VOAdBreak));
		if (NULL == m_VMAPData.pAdBreak)
		{
			m_VMAPData.pAdBreak = pAdBreak;
		}
		else
		{
			pVOAdBreak pTmp = m_VMAPData.pAdBreak;
			while(1)
			{
				if (NULL == pTmp->pNext)
				{
					break;
				}
				pTmp = pTmp->pNext;
			}
			pTmp->pNext = pAdBreak;
		}

		LoadAdbreakAttri(pNode,pAdBreak);
		LoadTag(pNode,pAdBreak,&CVMAPParser::HandleAdBreak);
	}
	else
	{
		VOLOGE("Unknow Root tag:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::HandleAdBreak(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR pStr = NULL;
	int Size = 0;

	pVOAdBreak pAdBreak = (pVOAdBreak)pData;
	if (0 == strcmp(strTage,VMAPTagAdSource))
	{
		if(NULL != pAdBreak->pAdSource)
		{
			VOLOGE("pAdSource already exist");
			return VO_RET_SOURCE2_OK;
		}
		pVOAdSource pAdSourceTmp = new VOAdSource;
		memset(pAdSourceTmp,0,sizeof(VOAdSource));

		pAdBreak->pAdSource = pAdSourceTmp;

		LoadAdSourceAttri(pNode,pAdSourceTmp);
		LoadTag(pNode,pAdSourceTmp,&CVMAPParser::HandleAdSource);

		pVOAdData pData = pAdSourceTmp->pAdData;
		for (VO_U32 uCnt = 0;(uCnt < pAdSourceTmp->uAdDataCount) && (NULL != pData);uCnt++)
		{
			if (pData->pInLine)
			{
				pVOAdCreative pCrea = pData->pInLine->pCreative;
				for (VO_U32 uCnt = 0;(uCnt < pData->pInLine->uCreativeCount) && (NULL != pCrea);uCnt++)
				{
					if(pCrea->CreativeType == VOAdBreakType_Linear)
					{
						pAdBreak->ullDuration += pCrea->Linear.ullDuration;
					}
					pCrea = pCrea->pNext;
				}
			}
			pData = pData->pNext;
		}
	}
	else if (0 == strcmp(strTage,VMAPTagTrackingEvents))
	{
		VOLOGE("Not support  VMAPTagTrackingEvents");
	}
	else if (0 == strcmp(strTage,VMAPTagExtensions))
	{
		VOLOGE("Not support  VMAPTagExtensions");
	}
	else
	{
		VOLOGE("Unknow pVOAdBreak tag:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVMAPParser::HandleAdSource(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR pStr = NULL;
	int Size = 0;

	pVOAdSource pAdSource = (pVOAdSource)pData;
	if (0 == strcmp(strTage,VMAPTagAdData))
	{
		++pAdSource->uAdDataCount;

		pVOAdData pAdDataTmp = new VOAdData;
		memset(pAdDataTmp,0,sizeof(VOAdData));
		if (NULL == pAdSource->pAdData)
		{
			pAdSource->pAdData = pAdDataTmp;
		}
		else
		{
			pVOAdData pTmp = pAdSource->pAdData;
			while(1)
			{
				if (NULL == pTmp->pNext)
				{
					break;
				}
				pTmp = pTmp->pNext;
			}
			pTmp->pNext = pAdDataTmp;
		}

		LoadAdData(pNode,pAdDataTmp);
	}
	else if (0 == strcmp(strTage,VMAPTagAdTagURI))
	{
		VOLOGE("Not support  VMAPTagAdTagURI");
	}
	else if (0 == strcmp(strTage,VMAPTagCustomAdData))
	{
		VOLOGE("Not support  VMAPTagCustomAdData");
	}
	else
	{
		VOLOGE("Unknow pVOAdSource tag:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}