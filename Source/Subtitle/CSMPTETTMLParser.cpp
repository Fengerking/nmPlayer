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
/************************************************************************
* @file CSMPTETTMLParser.cpp
*
* @author  Dolby Du
* 
* Change History
* 2013-06-18    Create File
************************************************************************/

#include "CSMPTETTMLParser.h"
#include "voLog.h"
#include "CSubtitleMgr.h"
#include "base64.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define VO_MAX(v1, v2) ((v1 > v2) ? v1 : v2)
const VO_U32 SMPTE_MAX_IMAGE_BUFFER_SIZE = 2*1024*1024;
CSMPTETTMLParser::CSMPTETTMLParser(void)
{	
	memset(&m_ImageInfo,0,sizeof(m_ImageInfo));
}

CSMPTETTMLParser::~CSMPTETTMLParser(void)
{

}

bool CSMPTETTMLParser::ParseLine (char* pPage, char* pDivEnd)
{
	bool ret = true;
	memset(&m_ImageInfo,0,sizeof(m_ImageInfo));

	if(NULL != (CXmlParserWraper::CheckNodeHead(pPage,  "<smpte:image")))
	{
		if (m_pCurrTrack == NULL)
			m_pCurrTrack = CreateTextTrack();

		if (m_pCurrTrack == NULL)
			return false;

		m_pCurrTrack->GetNewTextItem();
		VO_CHAR pVal[64];
		memset(pVal,0,64);
		SetBeginEndTime(pPage,pVal,m_llBaseTime);
		
		if(CXmlParserWraper::GetAttribute(pPage, "tts:extent", pVal, 60))
		{
			GetTwoValueFromPercent(pVal,m_ImageInfo.uImaWidth,m_ImageInfo.uImaHeight);
		}

		if(CXmlParserWraper::GetAttribute(pPage, "tts:origin", pVal, 60))
		{
			GetTwoValueFromPercent(pVal,m_ImageInfo.uXco,m_ImageInfo.uYco);
		}

		pPage = CXmlParserWraper::CheckNodeHead(pPage, "smpte:image");
		if (pPage)
		{
			if(CXmlParserWraper::GetAttribute(pPage, "imagetype", pVal, 60))
			{
				VO_U32 uStrLen = strlen(pVal) > 10 ? 10 : strlen(pVal);
				strncpy(m_ImageInfo.uCodeingType,pVal,uStrLen);
			}

			VO_CHAR* pImageBegin = strstr(pPage,">");
			pImageBegin += 1;
			VOLOGI("SMPTE_TTML:pImageBegin:%x:%x:%x",(pImageBegin[0] & 0xFF),(pImageBegin[1] & 0xFF),(pImageBegin[2] & 0xFF));
			
			for (;(pImageBegin[0] == 0xD || pImageBegin[0] == 0xA);++pImageBegin)///<remove the \r\n
			{
			}

			VO_CHAR* pImageEnd = strstr(pImageBegin,"</smpte:image>");
			if (pImageBegin && pImageEnd && pImageEnd > pImageBegin)
			{
				m_ImageInfo.pBuf = pImageBegin;
				m_ImageInfo.uBufSize = pImageEnd - pImageBegin;

				VO_BOOL bEncoding = VO_FALSE;
				if(CXmlParserWraper::GetAttribute(pPage, "encoding", pVal, 60))
				{
					if (strcmp(pVal,"Base64") == 0)
					{
						bEncoding = VO_TRUE;
					}
				}
				m_pCurrTrack->AddCurrTextItemImage(&m_ImageInfo,bEncoding);
				
			}
		}

	}
	else
	{
		ret = CTimedTextMLParser::ParseLine(pPage,pDivEnd);
	}
	return ret;
}
VO_BOOL CSMPTETTMLParser::GetTwoValueFromPercent(char* pString, VO_U32& uValue1, VO_U32& uValue2)
{
	char pValueImage[64] = {0};
	char* pTmp1 = NULL;
	char* pTmp2 = NULL;
	pTmp1 = strstr(pString,"%");
	if (pTmp1)
	{
		strncpy(pValueImage,pString,pTmp1 - pString);
		uValue1 = atoi(pValueImage);

		char* pTmp2 = strstr(pTmp1+1,"%");
		if (pTmp2)
		{
			memset(pValueImage,0,64);
			strncpy(pValueImage,pTmp1+1,pTmp2 - (pTmp1+1));
			uValue2 = atoi(pValueImage);
			return VO_TRUE;
		}
	}
	return VO_FALSE;
}
