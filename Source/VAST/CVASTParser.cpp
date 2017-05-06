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
#include "voVASTParser.h"
#include "CVASTParser.h"
#include "CVASTTag.h"
#include "voLog.h"
#include "fMacros.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define STRING_

VO_U32 DecodeCharacterEntities(VO_PCHAR pstrInput)
{
	if (NULL == pstrInput || strlen(pstrInput) < 5)
	{
		return VO_RET_SOURCE2_OK;
	}
	VO_PCHAR pStrBegin = pstrInput;
	VO_PCHAR pStrEnd = pstrInput + strlen(pstrInput);
	while((0 != *pStrBegin) && (pStrBegin < pStrEnd - 4))
	{
		if (0 == strncmp(pStrBegin,"&amp;",5))
		{
			strcpy(pStrBegin+1,pStrBegin+5);
		}
		pStrBegin++;
	}
	return VO_RET_SOURCE2_OK;
}

CVASTParser::CVASTParser()
{
	memset(&m_VastData,0,sizeof(VOAdData));
}

CVASTParser::~CVASTParser()
{
	Close();
}

VO_U32 CVASTParser::Open(VO_TCHAR *pWorkPath)
{
	SetWorkPath(pWorkPath);
	VO_U32 ret = VO_RET_SOURCE2_OK;
	if (VO_ERR_NONE != (ret = CXMLLoad::OpenParser()))
	{
		return ret;
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::Close()
{
	VO_U32 ret = VO_RET_SOURCE2_OK;

	if (VO_ERR_NONE != (ret = CXMLLoad::CloseParser()))
	{
		return ret;
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::Process(VO_PTR pBuffer, VO_U32 uSize)
{
	if(LoadXML((char*)pBuffer, int(uSize),voXML_FLAG_SOURCE_BUFFER) != VO_ERR_NONE)
	{
		VOLOGE("Load VAST XML failed:pBuffer=%x:uSize=%d",pBuffer,uSize);
		return VO_RET_SOURCE2_FAIL;
	}

	VO_VOID* pRootNode = NULL;
	GetFirstChild(NULL,&pRootNode);
	if (NULL == pRootNode)
	{
		VOLOGE("Can not find the root XML node");
		return VO_RET_SOURCE2_FAIL;
	}

	///<first we find the vmap:Extensions to get the content URI for CBS case
	VO_VOID *pTageNode = NULL;
	VO_PCHAR pStr = NULL;
	int Size = 0;
	GetChildByName(pRootNode,(VO_CHAR*)VASTAd,&pTageNode);
	if (NULL != pTageNode)
	{
		VO_VOID *pChildTageNode = NULL;
		GetChildByName(pTageNode,(VO_CHAR*)VASTInLine,&pChildTageNode);
		if (NULL != pChildTageNode)
		{
			GetChildByName(pTageNode,(VO_CHAR*)VASTCreatives,&pChildTageNode);
			if (NULL != pChildTageNode)
			{
				GetChildByName(pTageNode,(VO_CHAR*)VASTCreative,&pChildTageNode);
				if (NULL != pChildTageNode)
				{
					GetChildByName(pTageNode,(VO_CHAR*)VASTLinear,&pChildTageNode);
					if (NULL != pChildTageNode)
					{
						GetChildByName(pTageNode,(VO_CHAR*)VASTDuration,&pChildTageNode);
						if (NULL != pChildTageNode)
						{
							GetTagValue(pTageNode,&pStr,Size);
							if (NULL != pStr)
							{
								VO_U32 uHour = 0, uMin = 0, uSec = 0, uMilliSec = 0;
								if (Size == 7)
								{
									::sscanf (pStr, "%2d:%02d:%02d", &uHour, &uMin, &uSec);
								}
								else if (Size > 7)
								{
									::sscanf (pStr, "%2d:%02d:%02d.%03d", &uHour, &uMin, &uSec, &uMilliSec);
								}
								
								VO_U32 uDuration = (uHour*3600 + uMin * 60 + uSec)*1000 + uMilliSec;
							}
						}
						else
						{
							VOLOGE("Can not find VASTDuration");
						}
					}
					else
					{
						VOLOGE("Can not find VASTLinear");
					}
				}
				else
				{
					VOLOGE("Can not find VASTCreative");
				}
			}
			else
			{
				VOLOGE("Can not find VASTCreatives");
			}
		}
		else
		{
			VOLOGE("Can not find VASTInLine node");
		}
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::GetData(VOAdInfo* pData)
{
	return VO_RET_SOURCE2_OK;
}
VO_U32 CVASTParser::SetParam(VO_U32 uID, VO_PTR pParam)
{
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::GetParam(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
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

VO_U32 CVASTParser::ProcessByTag(VO_VOID *pAdDataNode)
{
	memset(&m_VastData,0,sizeof(VOAdData));
	//<first we find the vmap:Extensions to get the content URI for CBS case
	VO_VOID *pTageNode = NULL;
	VO_PCHAR pStr = NULL;
	int Size = 0;
	GetChildByName(pAdDataNode,(VO_CHAR*)VASTVAST,&pTageNode);
	if (NULL != pTageNode)
	{
		VO_PTR pChildTageNode = NULL;
		GetChildByName(pTageNode,(VO_CHAR*)VASTAd,&pChildTageNode);
		if (NULL != pChildTageNode)
		{
			LoadAdAttri(pChildTageNode,&m_VastData);
			LoadTag(pChildTageNode,(VO_PTR)&m_VastData,&CVASTParser::HandleAdData);	
		}
		else
		{
			VOLOGE("Can not find VASTAd node");
		}

	}
	else
	{
		VOLOGE("Can not find VASTVAST node");
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::ReleaseVASTData(pVOAdData pAdData)
{
	if (NULL != pAdData && pAdData->pInLine)
	{
		SAFE_DELETE(pAdData->pInLine->strAdSystem);
		SAFE_DELETE(pAdData->pInLine->strAdTitle);

		pVOAdImpression pIm = pAdData->pInLine->pImpression;
		for (VO_U32 uCnt = 0;((uCnt < pAdData->pInLine->uImpressionCount) && (NULL != pIm));uCnt++)
		{
			SAFE_DELETE(pIm->strURI);

			pVOAdImpression pTmp = pIm->pNext;
			delete pIm;
			pIm = pTmp;
		}
		SAFE_DELETE(pAdData->pInLine->strDescription);
		SAFE_DELETE(pAdData->pInLine->strAdvertiser);
		SAFE_DELETE(pAdData->pInLine->strSurvey);
		SAFE_DELETE(pAdData->pInLine->strError);

		pVOAdCreative pCreative = pAdData->pInLine->pCreative;
		for (VO_U32 uCnt = 0;((uCnt < pAdData->pInLine->uCreativeCount) && (NULL != pCreative));uCnt++)
		{
			SAFE_DELETE(pCreative->strID);
			SAFE_DELETE(pCreative->strAdId);
			if (pCreative->CreativeType == VOAdBreakType_Linear)
			{
				ReleaseLiner(&pCreative->Linear);
			}
			else if (pCreative->CreativeType == VOAdBreakType_NonLinear)
			{
				ReleaseNonLiner(&pCreative->NonLinear);
			}
			else if (pCreative->CreativeType == VOAdBreakType_Companion)
			{
				ReleaseCompanion(&pCreative->CompanionAds);
			}
			pVOAdCreative pTmp = pCreative->pNext;
			delete pCreative;
			pCreative = pTmp;
		}

		delete pAdData->pInLine;
		pAdData->pInLine = NULL;
	}
	if(NULL != pAdData)
	{	
		if (NULL != pAdData->strAdID)
		{
			delete pAdData->strAdID;
		}
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::LoadTag(VO_PTR pNode, VO_PTR pData ,pHandleData pHandle)
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

VO_U32 CVASTParser::LoadAdAttri(VO_PTR pNode, pVOAdData pData)
{
	if (NULL == pNode || NULL == pData)
	{
		return VO_RET_SOURCE2_OK;
	}
	VO_PCHAR str = NULL;
	int Size = 0;

	GetAttributeValue(pNode,(VO_CHAR*)VASTid,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->strAdID = new VO_CHAR[strlen(str) + 1];
		strcpy(pData->strAdID,str);
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriSequence,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->uSequenceNum = atoi(str);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::LoadCreativeAttri(VO_PTR pNode, pVOAdCreative pData)
{
	if (NULL == pNode || NULL == pData)
	{
		return VO_RET_SOURCE2_OK;
	}
	VO_PCHAR str = NULL;
	int Size = 0;

	GetAttributeValue(pNode,(VO_CHAR*)VASTid,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->strID = new VO_CHAR[strlen(str) + 1];
		strcpy(pData->strID,str);
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriSequence,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->uSequence = atoi(str);
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriCreativeAdId,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->strAdId = new VO_CHAR[strlen(str) + 1];
		strcpy(pData->strAdId,str);
	}

	return VO_RET_SOURCE2_OK;

}
VO_U32 CVASTParser::LoadMediaFileAttri(VO_VOID *pNode, pVOAdMediaFile pData)
{
	if (NULL == pNode || NULL == pData)
	{
		return VO_RET_SOURCE2_OK;
	}
	VO_PCHAR str = NULL;
	int Size = 0;

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileDelivery,&str,Size);
	if (NULL != str && Size > 0)
	{
		if(0 == strcmp(str,"progressive"))
		{
			pData->uDeliveryType = 0;
		}
		else if (0 == strcmp(str,"streaming"))
		{
			pData->uDeliveryType = 1;
		}
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileType,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->strType = new VO_CHAR[strlen(str)];
		strcpy(pData->strType,str);
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileWidth,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->uWidth = atoi(str);
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileHeight,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->uHeight = atoi(str);
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileCodec,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->strCodec = new VO_CHAR[strlen(str)];
		strcpy(pData->strCodec,str);
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileID,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->uID = atoi(str);
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileBitrate,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->uBitrate = atoi(str);
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileMinBitrate,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->uMinBitrate = atoi(str);
	}

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileMaxBitrate,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->uMaxBitrate = atoi(str);
	}
	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileScalable,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->bScalable = (VO_BOOL)atoi(str);
	}
	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileMaintainAspectRatio,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->bMaintainAspectRatio = (VO_BOOL)atoi(str);
	}
	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriMediaFileapiFramework,&str,Size);
	if (NULL != str && Size > 0)
	{
		VOLOGE("Not support MediaFileapiFramework");
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::LoadTrackingAttri(VO_VOID *pNode, pVOVastTrackingEvent pData)
{
	if (NULL == pNode || NULL == pData)
	{
		return VO_RET_SOURCE2_OK;
	}
	VO_PCHAR str = NULL;
	int Size = 0;

	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriTrackingEvent,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->strEvent = new VO_CHAR[strlen(str) + 1];
		strcpy(pData->strEvent,str);
	}
	
	GetAttributeValue(pNode,(VO_CHAR*)VASTAttriTrackingOffset,&str,Size);
	if (NULL != str && Size > 0)
	{
		pData->uOffset = atoi(str);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::HandleAdData(VO_VOID* pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR str = NULL;
	int Size = 0;
	pVOAdData pTmp = (pVOAdData)pData;
	if (0 == strcmp(strTage,VASTInLine))
	{
		if (NULL != pTmp->pInLine)
		{
			VOLOGE("Ad has multiple VOAdInLine");
			return VO_RET_SOURCE2_OK;
		}
		pTmp->pInLine = new VOAdInLine;
		memset(pTmp->pInLine,0,sizeof(VOAdInLine));

		LoadTag(pNode,(VO_PTR)pTmp->pInLine,&CVASTParser::HandleInLine);
	}
	else
	{
		VOLOGE("Unknown Ad data tag:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::HandleInLine(VO_VOID* pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR pStr = NULL;
	int Size = 0;
	pVOAdInLine pTmp = (pVOAdInLine)pData;
	if (0 == strcmp(strTage,VASTAdSystem))
	{
		GetTagValue(pNode,&pStr,Size);
		if (NULL != pStr && Size > 0)
		{
			if (NULL != pTmp->strAdSystem)
			{
				delete pTmp->strAdSystem;
				pTmp->strAdSystem = NULL;
			}
			pTmp->strAdSystem = new VO_CHAR[strlen(pStr)+1];
			strcpy(pTmp->strAdSystem,pStr);
		}
	}
	else if (0 == strcmp(strTage,VASTAdTitle))
	{
		GetTagValue(pNode,&pStr,Size);
		if (NULL != pStr && Size > 0)
		{
			if (NULL != pTmp->strAdTitle)
			{
				delete pTmp->strAdTitle;
				pTmp->strAdTitle = NULL;
			}
			pTmp->strAdTitle = new VO_CHAR[strlen(pStr)+1];
			strcpy(pTmp->strAdTitle,pStr);
		}	
	}
	else if (0 == strcmp(strTage,VASTImpression))
	{
		++pTmp->uImpressionCount;
		GetTagValue(pNode,&pStr,Size);
		if (NULL != pStr && Size > 0)
		{
			pVOAdImpression pIm = new VOAdImpression;
			memset(pIm,0,sizeof(VOAdImpression));
			if (NULL == pTmp->pImpression)
			{
				pTmp->pImpression = pIm;
			}
			else
			{
				pVOAdImpression pTmpIm = pTmp->pImpression;
				while(1)
				{
					if (NULL == pTmpIm->pNext)
					{
						break;
					}
					pTmpIm = pTmpIm->pNext;
				}
				pTmpIm->pNext = pIm;
			}

			pIm->strURI = new VO_CHAR[strlen(pStr)+1];
			strcpy(pIm->strURI,pStr);
			DecodeCharacterEntities(pIm->strURI);
		}	
	}
	else if (0 == strcmp(strTage,VASTDescription))
	{
		GetTagValue(pNode,&pStr,Size);
		if (NULL != pStr && Size > 0)
		{
			if (NULL != pTmp->strDescription)
			{
				delete pTmp->strDescription;
				pTmp->strDescription = NULL;
			}
			pTmp->strDescription = new VO_CHAR[strlen(pStr)+1];
			strcpy(pTmp->strDescription,pStr);
		}	
	}
	else if (0 == strcmp(strTage,VASTAdvertiser))
	{
		GetTagValue(pNode,&pStr,Size);
		if (NULL != pStr && Size > 0)
		{
			if (NULL != pTmp->strAdvertiser)
			{
				delete pTmp->strAdvertiser;
				pTmp->strAdvertiser = NULL;
			}
			pTmp->strAdvertiser = new VO_CHAR[strlen(pStr)+1];
			strcpy(pTmp->strAdvertiser,pStr);
		}	
	}
	else if (0 == strcmp(strTage,VASTSurvey))
	{
		GetTagValue(pNode,&pStr,Size);
		if (NULL != pStr && Size > 0)
		{
			if (NULL != pTmp->strSurvey)
			{
				delete pTmp->strSurvey;
				pTmp->strSurvey = NULL;
			}
			pTmp->strSurvey = new VO_CHAR[strlen(pStr)+1];
			strcpy(pTmp->strSurvey,pStr);
			DecodeCharacterEntities(pTmp->strSurvey);
		}	
	}
	else if (0 == strcmp(strTage,VASTError))
	{
		GetTagValue(pNode,&pStr,Size);
		if (NULL != pStr && Size > 0)
		{
			if (NULL != pTmp->strError)
			{
				delete pTmp->strError;
				pTmp->strError = NULL;
			}
			pTmp->strError = new VO_CHAR[strlen(pStr)+1];
			strcpy(pTmp->strError,pStr);
			DecodeCharacterEntities(pTmp->strError);
		}	
	}
	else if (0 == strcmp(strTage,VASTCreatives))
	{
		LoadTag(pNode,pData,&CVASTParser::HandleCreatives);
	}
	else
	{
		VOLOGE("Unknown InLine data tag:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::HandleCreatives(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR pStr = NULL;
	int Size = 0;
	pVOAdInLine pTmpInLine = (pVOAdInLine)pData;
	if (0 == strcmp(strTage,VASTCreative))
	{
		++pTmpInLine->uCreativeCount;
		pVOAdCreative pCreativeTmp = new VOAdCreative;
		memset(pCreativeTmp,0,sizeof(VOAdCreative));
		if (NULL == pTmpInLine->pCreative)
		{
			pTmpInLine->pCreative = pCreativeTmp;
		}
		else
		{
			pVOAdCreative pTmp = pTmpInLine->pCreative;
			while(1)
			{
				if (NULL == pTmp->pNext)
				{
					break;
				}
				pTmp = pTmp->pNext;
			}
			pTmp->pNext = pCreativeTmp;
		}
		LoadCreativeAttri(pNode,pCreativeTmp);
		LoadTag(pNode,pCreativeTmp,&CVASTParser::HandleCreative);
	}
	else
	{
		VOLOGE("Unknown Creatives data tag:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::HandleCreative(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR pStr = NULL;
	int Size = 0;

	pVOAdCreative pTmp = (pVOAdCreative)pData;
	if (0 == strcmp(strTage,VASTLinear))
	{
		pTmp->CreativeType = VOAdBreakType_Linear;
		LoadLinearAttri(pNode,&pTmp->Linear);
		LoadTag(pNode,&pTmp->Linear,&CVASTParser::HandleLinear);
	}
	else if (0 == strcmp(strTage,VASTNonLinear))
	{
		pTmp->CreativeType = VOAdBreakType_NonLinear;
		LoadNonLinearAttri(pNode,&pTmp->NonLinear);
		LoadTag(pNode,&pTmp->NonLinear,&CVASTParser::HandleNonLinear);
	}
	else if (0 == strcmp(strTage,VASTCompanionAds))
	{
		pTmp->CreativeType = VOAdBreakType_Companion;
		LoadCompanioAttri(pNode,&pTmp->CompanionAds);
		LoadTag(pNode,&pTmp->CompanionAds,&CVASTParser::HandleCompanion);
	}
	else
	{
		VOLOGE("Unknown Creative data tag:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::HandleLinear(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR pStr = NULL;
	int Size = 0;

	pVOVastLinear pTmp = (pVOVastLinear)pData;
	if (0 == strcmp(strTage,VASTDuration))
	{
		GetTagValue(pNode,&pStr,Size);
		if (NULL != pStr)
		{
			VO_U32 uHour = 0, uMin = 0, uSec = 0, uMilliSec = 0;
			if (Size == 7)
			{
				::sscanf (pStr, "%2d:%02d:%02d", &uHour, &uMin, &uSec);
			}
			else if (Size > 7)
			{
				::sscanf (pStr, "%2d:%02d:%02d.%03d", &uHour, &uMin, &uSec, &uMilliSec);
			}

			pTmp->ullDuration = (uHour*3600 + uMin * 60 + uSec)*1000 + uMilliSec;
		}
		else
		{
			VOLOGE("VASTDuration is null");
		}
	}
	else if (0 == strcmp(strTage,VASTMediaFiles))///<support only one MediaFiles tag
	{
		LoadTag(pNode,pData,&CVASTParser::HandleMediaFiles);
	}
	else if (0 == strcmp(strTage,VASTVideoClicks))///<support only one VideoClicks tag
	{
		if (NULL != pTmp->pVideoClicks)
		{
			VOLOGE("Ad has multiple pVideoClicks");
			return VO_RET_SOURCE2_OK;
		}
		pTmp->uVideoClicksCount = 1;
		pTmp->pVideoClicks = new VOAdVideoClicks;
		memset(pTmp->pVideoClicks,0,sizeof(VOAdVideoClicks));

		LoadTag(pNode,(VO_PTR)pTmp->pVideoClicks,&CVASTParser::HandleVideoClicks);
	}
	else if (0 == strcmp(strTage,VASTAdParameters))///<support only one AdParameters tag
	{
		if (NULL != pTmp->pAdParameter)
		{
			VOLOGE("Ad has multiple pVideoClicks");
			return VO_RET_SOURCE2_OK;
		}
		pTmp->uAdParameterCount = 1;
		pTmp->pAdParameter = new VOAdParameter;
		memset(pTmp->pAdParameter,0,sizeof(VOAdParameter));

		LoadTag(pNode,(VO_PTR)pTmp->pAdParameter,&CVASTParser::HandleAdParameter);
	}
	else if (0 == strcmp(strTage,VASTTrackingEvents))
	{
		LoadTag(pNode,pData,&CVASTParser::HandleTrackingEvents);
	}
	else if (0 == strcmp(strTage,VASTIcons))
	{
		LoadTag(pNode,pData,&CVASTParser::HandleIcons);
	}
	else
	{
		VOLOGE("Unknown pVOVastLinear data tag:%s",strTage);
	}

	return VO_RET_SOURCE2_OK;
}



VO_U32 CVASTParser::HandleMediaFiles(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR pStr = NULL;
	int Size = 0;

	pVOVastLinear pTmpLinear = (pVOVastLinear)pData;
	if (0 == strcmp(strTage,VASTMediaFile))
	{
		++pTmpLinear->uMediaFileCount;
		pVOAdMediaFile pMediaFileTmp = new VOAdMediaFile;
		memset(pMediaFileTmp,0,sizeof(VOAdMediaFile));
		if (NULL == pTmpLinear->pMediaFile)
		{
			pTmpLinear->pMediaFile = pMediaFileTmp;
		}
		else
		{
			pVOAdMediaFile pTmp = pTmpLinear->pMediaFile;
			while(1)
			{
				if (NULL == pTmp->pNext)
				{
					break;
				}
				pTmp = pTmp->pNext;
			}
			pTmp->pNext = pMediaFileTmp;
		}
		LoadMediaFileAttri(pNode,pMediaFileTmp);
		GetTagValue(pNode,&pStr,Size);
		if (NULL != pStr && Size > 0)
		{
			pMediaFileTmp->strURI = new VO_CHAR[strlen(pStr)];
			strcpy(pMediaFileTmp->strURI,pStr);
			DecodeCharacterEntities(pMediaFileTmp->strURI);
		}
	}
	else
	{
		VOLOGE("Unknown MediaFiles data tag:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::HandleVideoClicks(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR str = NULL;
	int Size = 0;

	pVOAdVideoClicks pTmpVideoClicks = (pVOAdVideoClicks)pData;
	if (0 == strcmp(strTage,VASTClickThrough))
	{
		++pTmpVideoClicks->uClickThroughCount;
		pTmpVideoClicks->pClickThrough = new VOAdClick;
		memset(pTmpVideoClicks->pClickThrough,0,sizeof(VOAdClick));
		GetAttributeValue(pNode,(VO_CHAR*)VASTAttriClickThroughID,&str,Size);
		if (NULL != str && Size > 0)
		{
			pTmpVideoClicks->pClickThrough->strID = new VO_CHAR[strlen(str) + 1];
			strcpy(pTmpVideoClicks->pClickThrough->strID,str);
		}

		str = NULL;
		GetTagValue(pNode,&str,Size);
		if (NULL != str && Size > 0)
		{
			pTmpVideoClicks->pClickThrough->strURI= new VO_CHAR[strlen(str) + 1];
			strcpy(pTmpVideoClicks->pClickThrough->strURI,str);
			DecodeCharacterEntities(pTmpVideoClicks->pClickThrough->strURI);
		}	
	}
	else if (0 == strcmp(strTage,VASTClickTracking))
	{
		++pTmpVideoClicks->uClickTrackingCount;
		pTmpVideoClicks->pClickTracking = new VOAdClick;
		memset(pTmpVideoClicks->pClickTracking,0,sizeof(VOAdClick));

		GetAttributeValue(pNode,(VO_CHAR*)VASTAttriClickThroughID,&str,Size);
		if (NULL != str && Size > 0)
		{
			pTmpVideoClicks->pClickTracking->strID = new VO_CHAR[strlen(str) + 1];
			strcpy(pTmpVideoClicks->pClickTracking->strID,str);
		}

		str = NULL;
		GetTagValue(pNode,&str,Size);
		if (NULL != str && Size > 0)
		{
			pTmpVideoClicks->pClickTracking->strURI= new VO_CHAR[strlen(str) + 1];
			strcpy(pTmpVideoClicks->pClickTracking->strURI,str);
			DecodeCharacterEntities(pTmpVideoClicks->pClickTracking->strURI);
		}	
	}
	else if (0 == strcmp(strTage,VASTCustomClick))
	{
		++pTmpVideoClicks->uCustomClickCount;
		pTmpVideoClicks->pCustomClick = new VOAdClick;
		memset(pTmpVideoClicks->pCustomClick,0,sizeof(VOAdClick));

		GetAttributeValue(pNode,(VO_CHAR*)VASTAttriClickThroughID,&str,Size);
		if (NULL != str && Size > 0)
		{
			pTmpVideoClicks->pCustomClick->strID = new VO_CHAR[strlen(str) + 1];
			strcpy(pTmpVideoClicks->pCustomClick->strID,str);
		}

		str = NULL;
		GetTagValue(pNode,&str,Size);
		if (NULL != str && Size > 0)
		{
			pTmpVideoClicks->pCustomClick->strURI= new VO_CHAR[strlen(str) + 1];
			strcpy(pTmpVideoClicks->pCustomClick->strURI,str);
			DecodeCharacterEntities(pTmpVideoClicks->pCustomClick->strURI);
		}	
	}
	else
	{
		VOLOGE("Unknown VideoClicksData:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::HandleIcons(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR str = NULL;
	int Size = 0;

	if (0 == strcmp(strTage,VASTIcon))
	{
		VOLOGE("not support icon");
	}
	else
	{
		VOLOGE("Unknown Icons tag:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::HandleTrackingEvents(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize)
{
	if (NULL == pNode || NULL == pData || NULL == strTage)
	{
		return VO_RET_SOURCE2_OK;
	}

	VO_PCHAR pStr = NULL;
	int Size = 0;

	pVOVastLinear pTmpLinear = (pVOVastLinear)pData;
	if (0 == strcmp(strTage,VASTTracking))
	{
		++pTmpLinear->uTrackingEvenCount;
		pVOVastTrackingEvent pTrackingTmp = new VOAdTrackingEvent;
		memset(pTrackingTmp,0,sizeof(VOAdTrackingEvent));
		if (NULL == pTmpLinear->pTrackingEvents)
		{
			pTmpLinear->pTrackingEvents = pTrackingTmp;
		}
		else
		{
			pVOVastTrackingEvent pTmp = pTmpLinear->pTrackingEvents;
			while(1)
			{
				if (NULL == pTmp->pNext)
				{
					break;
				}
				pTmp = pTmp->pNext;
			}
			pTmp->pNext = pTrackingTmp;
		}
		LoadTrackingAttri(pNode,pTrackingTmp);
		GetTagValue(pNode,&pStr,Size);
		if (NULL != pStr && Size > 0)
		{
			pTrackingTmp->strURI = new VO_CHAR[strlen(pStr) + 1];
			strcpy(pTrackingTmp->strURI,pStr);
			DecodeCharacterEntities(pTrackingTmp->strURI);
		}
	}
	else
	{
		VOLOGE("Unknown VASTTrackingEvent tag:%s",strTage);
	}
	return VO_RET_SOURCE2_OK;
}

VO_U32 CVASTParser::ReleaseLiner(pVOVastLinear pData)
{
	pVOAdMediaFile pMediaFile = pData->pMediaFile;
	for (VO_U32 uCnt = 0;((uCnt < pData->uMediaFileCount) && (NULL != pMediaFile));uCnt++)
	{
		SAFE_DELETE(pMediaFile->strType);
		SAFE_DELETE(pMediaFile->strCodec);
		SAFE_DELETE(pMediaFile->strURI);
		
		pVOAdMediaFile pTmp = pMediaFile->pNext;
		delete pMediaFile;
		pMediaFile = pTmp;
	}

	pVOAdVideoClicks pVideoClicks = pData->pVideoClicks;
	for (VO_U32 uCnt = 0;((uCnt < pData->uVideoClicksCount) && (NULL != pVideoClicks));uCnt++)
	{
		pVOAdClick pClick = pVideoClicks->pClickThrough;
		for (VO_U32 uCnt = 0;((uCnt < pVideoClicks->uClickThroughCount) && (NULL != pClick));uCnt++)
		{
			SAFE_DELETE(pClick->strID);
			SAFE_DELETE(pClick->strURI);

			pVOAdClick pTmp = pClick->pNext;
			delete pClick;
			pClick = pTmp;
		}

		pClick = pVideoClicks->pClickTracking;
		for (VO_U32 uCnt = 0;((uCnt < pVideoClicks->uClickTrackingCount) && (NULL != pClick));uCnt++)
		{
			SAFE_DELETE(pClick->strID);
			SAFE_DELETE(pClick->strURI);

			pVOAdClick pTmp = pClick->pNext;
			delete pClick;
			pClick = pTmp;
		}

		pClick = pVideoClicks->pCustomClick;
		for (VO_U32 uCnt = 0;((uCnt < pVideoClicks->uCustomClickCount) && (NULL != pClick));uCnt++)
		{
			SAFE_DELETE(pClick->strID);
			SAFE_DELETE(pClick->strURI);

			pVOAdClick pTmp = pClick->pNext;
			delete pClick;
			pClick = pTmp;
		}
	}

	pVOAdParameter pAdParameter = pData->pAdParameter;
	for (VO_U32 uCnt = 0;((uCnt < pData->uAdParameterCount) && (NULL != pAdParameter));uCnt++)
	{
		pVOAdParameter pTmp = pAdParameter->pNext;
		delete pAdParameter;
		pAdParameter = pTmp;
	}

	pVOVastTrackingEvent pTracking = pData->pTrackingEvents;
	for (VO_U32 uCnt = 0;((uCnt < pData->uTrackingEvenCount) && (NULL != pTracking));uCnt++)
	{
		SAFE_DELETE(pTracking->strEvent);
		SAFE_DELETE(pTracking->strURI);

		pVOVastTrackingEvent pTmp = pTracking->pNext;
		delete pTracking;
		pTracking = pTmp;
	}

	pVOAdIcon pIcon = pData->pIcons;
	for (VO_U32 uCnt = 0;((uCnt < pData->uIconCount) && (NULL != pIcon));uCnt++)
	{
		pVOAdIcon pTmp = pIcon->pNext;
		delete pIcon;
		pIcon = pTmp;
	}
	return VO_RET_SOURCE2_OK;
}