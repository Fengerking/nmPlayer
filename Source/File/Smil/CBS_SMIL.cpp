	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		Smil.h

	Contains:	Smil class file

	Written by:	Aiven

	Change History (most recent first):
	2013-07-08		Aiven			Create file

*******************************************************************************/
#include "CBS_SMIL.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
#ifdef WIN32 
#define _ATO64(a) _atoi64(a) 
#elif defined LINUX || defined _IOS
#define _ATO64(a) atoll(a) 
#endif 

static VO_U64 String2Time(const char *str, int size)
{
	_VODS_CHECK_NULL_POINTER_TWO_(str, size, 0)

char interstr[255] = {0};

memcpy(&interstr, str, size);

 int c[3];
 memset(c, -1, sizeof(int)*3);
 int t[3];
 memset(t, 0, sizeof(int)*3);
 sscanf(interstr, "%d:%d:%d", &c[0], &c[1], &c[2]);
 int i = 2;
 int j = 2;
 while( i >=0)
 {
  if( c[i] != -1 )
  {
   t[j] = c[i];
   j--;
  }
  i--;
 }

 VO_U64 xt =( t[0] * 3600 + t[1] * 60 + t[2] ) * 1000;
 return xt;
}

CBS_SMIL::CBS_SMIL()
:m_pCBS_SMIL(NULL)
{
}

CBS_SMIL::~CBS_SMIL()
{
	ReleaseCBS_SMIL(&m_pCBS_SMIL);
}


VO_U32 CBS_SMIL::Parse(VO_PBYTE pBuffer, VO_U32 nSize, VO_PTR* ppSmilStruct)
{
	_VODS_CHECK_NULL_POINTER_THREE_(pBuffer, nSize, ppSmilStruct, VO_RET_SOURCE2_FAIL)

	VO_U32 nResult = VO_RET_SOURCE2_FAIL;
	VO_SMIL_STRUCT* pSmil = NULL;

	if(VO_RET_SOURCE2_OK == CSmil::Parse(pBuffer, nSize, (VO_PTR*)&pSmil)){
		if(VO_RET_SOURCE2_OK == ParseCBS_SMIL(pSmil, &m_pCBS_SMIL)){
			*ppSmilStruct = m_pCBS_SMIL;
			nResult = VO_RET_SOURCE2_OK;
		}
	}

	return nResult;
}

VO_U32 CBS_SMIL::ParseCBS_SMIL(VO_SMIL_STRUCT* pSmil, VO_CBS_SMIL_VIDEO** ppCBSSMIL)
{
	_VODS_CHECK_NULL_POINTER_TWO_(pSmil, ppCBSSMIL, VO_RET_SOURCE2_FAIL)
	
	VO_U32 nResult = VO_RET_SOURCE2_FAIL;

	ReleaseCBS_SMIL(ppCBSSMIL);
	
	VO_CBS_SMIL_VIDEO* phead = NULL;

	nResult = ParseCBS_Video(pSmil->body, &phead);

	return nResult;
}

VO_U32 CBS_SMIL::ParseCBS_Video(VO_ELEMENT_STRUCT* pElement, VO_CBS_SMIL_VIDEO** ppCBSSMIL)
{
	_VODS_CHECK_NULL_POINTER_ONE_(pElement, VO_RET_SOURCE2_FAIL)
	
	VO_U32 nResult = VO_RET_SOURCE2_OK;
	VO_ATTRIBUTE_STRUCT* pAttribute = NULL;
	VO_CBS_SMIL_VIDEO* pTmp = NULL;
	if(TYPE_ELEMENT_VIDEO == pElement->Type){
		
		if((*ppCBSSMIL)){
			(*ppCBSSMIL)->pNext = new VO_CBS_SMIL_VIDEO;
			(*ppCBSSMIL) = (*ppCBSSMIL)->pNext;
		}else{
			(*ppCBSSMIL) = new VO_CBS_SMIL_VIDEO;
			(*ppCBSSMIL) = *ppCBSSMIL;
			m_pCBS_SMIL = *ppCBSSMIL;
		}
		pTmp = *ppCBSSMIL;
		memset(pTmp, 0x0, sizeof(VO_CBS_SMIL_VIDEO));

 		pAttribute = NULL;
		if (VO_RET_SOURCE2_OK == FindAttribute(pElement, (VO_CHAR*)ATTRIBUTE_SRC, &pAttribute)){
			memcpy(pTmp->pSrc, pAttribute->pValue, pAttribute->nValuesize);
		}

 		pAttribute = NULL;
		if (VO_RET_SOURCE2_OK == FindAttribute(pElement, (VO_CHAR*)ATTRIBUTE_TITLE, &pAttribute)){
			memcpy(pTmp->pTitle, pAttribute->pValue, pAttribute->nValuesize);
		}
		
		pAttribute = NULL;
		if (VO_RET_SOURCE2_OK == FindAttribute(pElement, (VO_CHAR*)ATTRIBUTE_CLIP_BEGIN, &pAttribute)){
			pTmp->clipBegin = String2Time(pAttribute->pValue, pAttribute->nValuesize);

		}
 
		 pAttribute = NULL;
		 if (VO_RET_SOURCE2_OK == FindAttribute(pElement, (VO_CHAR*)ATTRIBUTE_GUID, &pAttribute)){
			 memcpy(pTmp->pGuid, pAttribute->pValue, pAttribute->nValuesize);
		 }
		 
		pAttribute = NULL;
		if (VO_RET_SOURCE2_OK == FindAttribute(pElement, (VO_CHAR*)ATTRIBUTE_CLIP_END, &pAttribute)){
			pTmp->clipEnd= String2Time(pAttribute->pValue, pAttribute->nValuesize);

		}else if(VO_RET_SOURCE2_OK == FindAttribute(pElement, (VO_CHAR*)ATTRIBUTE_DUR, &pAttribute)){
			pAttribute->pValue[pAttribute->nValuesize - 1] = 0;
			pAttribute->pValue[pAttribute->nValuesize - 2] = 0;
			pTmp->clipEnd= _ATO64(pAttribute->pValue);
		}

		list_T<VO_ELEMENT_STRUCT *>::iterator elementiter;
		for(elementiter = pElement->ChildList.begin(); elementiter != pElement->ChildList.end(); ++elementiter)
		{
			VO_ELEMENT_STRUCT* tmpelement = (VO_ELEMENT_STRUCT*)(*elementiter);
			if(TYPE_ELEMENT_PARAM == tmpelement->Type){

				pAttribute = NULL;
				if (VO_RET_SOURCE2_OK == FindAttribute(tmpelement, (VO_CHAR*)PARAM_ATTRIBUTE_NAME, &pAttribute))
				{
					if (0 == strncmp(pAttribute->pValue, "ClosedCaptionURL", pAttribute->nValuesize))
					{
						if (VO_RET_SOURCE2_OK == FindAttribute(tmpelement, (VO_CHAR*)PARAM_ATTRIBUTE_VALUE, &pAttribute))
						{
							memcpy(pTmp->pClosedCaptionURL, pAttribute->pValue, pAttribute->nValuesize);
							continue;
						}
					}
					else if (0 == strncmp(pAttribute->pValue, "IsLive", pAttribute->nValuesize))
					{
						if (VO_RET_SOURCE2_OK == FindAttribute(tmpelement, (VO_CHAR*)PARAM_ATTRIBUTE_VALUE, &pAttribute))
						{
						
							if (0 == strncmp(pAttribute->pValue, "true", pAttribute->nValuesize)){
								pTmp->pIsLive = VO_TRUE;
							}
							continue;
						}
					}
					else if (0 == strncmp(pAttribute->pValue, "EpisodeFlag", pAttribute->nValuesize))
					{
						if (VO_RET_SOURCE2_OK == FindAttribute(tmpelement, (VO_CHAR*)PARAM_ATTRIBUTE_VALUE, &pAttribute))
						{
						
							if (0 == strncmp(pAttribute->pValue, "true", pAttribute->nValuesize)){
								pTmp->bEpisodeFlag = VO_TRUE;
							}
							continue;
						}
					}
					else if (0 == strncmp(pAttribute->pValue, "trackingData", pAttribute->nValuesize))
					{
						if (VO_RET_SOURCE2_OK == FindAttribute(tmpelement, (VO_CHAR*)PARAM_ATTRIBUTE_VALUE, &pAttribute))
						{
							VO_CHAR* p1 = NULL;							
							VO_CHAR* p2 = NULL;
							p1 = strstr (pAttribute->pValue, "cid=");
							p1+=strlen("cid=");
							p2 = strstr (p1, "|");
							memcpy(pTmp->pContantDAta, p1, p2-p1);
							continue;
						}
					}
				}

			}
		}
		
	}

	list_T<VO_ELEMENT_STRUCT *>::iterator elementiter;
	for(elementiter = pElement->ChildList.begin(); elementiter != pElement->ChildList.end(); ++elementiter)
	{
		VO_ELEMENT_STRUCT* tmpelement = (VO_ELEMENT_STRUCT*)(*elementiter);
		if(TYPE_ELEMENT_A == tmpelement->Type ||
			TYPE_ELEMENT_PAR == tmpelement->Type ||
			TYPE_ELEMENT_SEQ == tmpelement->Type ||
			TYPE_ELEMENT_SWITCH == tmpelement->Type ||
			TYPE_ELEMENT_VIDEO == tmpelement->Type){
			ParseCBS_Video(tmpelement, ppCBSSMIL);
		}
	}

	return nResult;
}

VO_U32 CBS_SMIL::ReleaseCBS_SMIL(VO_CBS_SMIL_VIDEO** ppCBSSMIL)
{
	_VODS_CHECK_NULL_POINTER_ONE_(ppCBSSMIL, VO_RET_SOURCE2_FAIL)

	VO_U32 nResult = VO_RET_SOURCE2_OK;

	VO_CBS_SMIL_VIDEO* pVideo = *ppCBSSMIL;
	while(pVideo)
	{
		VO_CBS_SMIL_VIDEO* pNext = pVideo->pNext;
		delete pVideo;
		pVideo = pNext;
	}
	
	pVideo = NULL;

	return nResult;
}

