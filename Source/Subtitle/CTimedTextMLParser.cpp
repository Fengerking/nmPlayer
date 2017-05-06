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
* @file CTimedTextMLParser.cpp
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#define LOG_TAG "CTimedTextMLParser"

#include "CTimedTextMLParser.h"
#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
#include "CSubtitleMgr.h"

//#pragma warning (disable : 4996)

CTimedTextMLParser::CTimedTextMLParser(void)
:m_llBaseTime(0)
,m_ColumnCnt(-1)
,m_RowCnt(-1)
,m_uTimeContainer(-1)
{	
	
}

CTimedTextMLParser::~CTimedTextMLParser(void)
{

}
;
bool	CTimedTextMLParser::StrToTime (char* pVal, int& nHour1, int& nMin1, int& nSec1, int& nMS1)
{
	nHour1=0, nMin1=0, nSec1=0, nMS1=0;
	int nLen = strlen(pVal);
	if(nLen>0 && pVal[nLen-1] == 's')
	{
		if(nLen>1 && pVal[nLen-2] == 'm')
		{
			pVal[nLen-1] = 0;
			pVal[nLen-2] = 0;
			float f = atof(pVal);
			nSec1 = (int)f/1000;
			nMS1 = ((int)(f/10))%100;
		}
		else
		{
			pVal[nLen-1] = 0;
			float f = atof(pVal);
			nSec1 = (int)f;
			nMS1 = ((int)(f*100))%100;
		}
	}
	else
	{
		if(strlen(pVal) == 11)
		{
			::sscanf (pVal, "%2d:%02d:%02d.%02d", &nHour1, &nMin1, &nSec1, &nMS1);
		}
		else if(strlen(pVal) == 7)
		{	
			nHour1 = 0;
			::sscanf(pVal,"%2d:%2d.%d",&nMin1,&nSec1,&nMS1);
			nMS1 = nMS1 * 10;
		}
		else
		{
			if (pVal[8] == ':')
			{
				::sscanf (pVal, "%2d:%02d:%02d:%03d", &nHour1, &nMin1, &nSec1, &nMS1);
			}
			else
			{
				::sscanf (pVal, "%2d:%02d:%02d.%03d", &nHour1, &nMin1, &nSec1, &nMS1);
			}	
			nMS1/=10;
		}

	}
	return true;
}
void 	CTimedTextMLParser::FillAttribute(TT_Style* stl, char* subNodeBegin)
{
	char pVal[64];// = NULL;
	memset(pVal,0,64);
	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:fontSize", pVal, 60))
		stl->fontSize = stl->fontSizetoInt(pVal);

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:fontFamily", pVal, 60))
		stl->nFontFamily = stl->fontFamilyToInt(pVal);//strcpy(stl->fontFamily, pVal);

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:textDecoration", pVal, 60))
		strcpy(stl->textDecoration, pVal);

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:fontStyle", pVal, 60))
		strcpy(stl->fontStyle, pVal);

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "id", pVal, 60))
		strcpy(stl->id, pVal);

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:backgroundColor", pVal, 60))
	{
		stl->bBackGroundColorForce = VO_TRUE;
		stl->backgroundColor = stl->colorStringToInt(pVal);
	}

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:color", pVal, 60))
		stl->color = stl->colorStringToInt(pVal);

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:textAlign", pVal, 60))
		stl->textAlign = stl->textAlignToInt(pVal);

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:fontWeight", pVal, 60))
		stl->fontWeight = stl->fontWeightToInt(pVal);

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:wrapOption", pVal, 60))
		stl->wrapEnable = stl->textWrapToBool(pVal);

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:opacity", pVal, 60))
	{
		stl->backgroundColor.nTransparency = stl->opacityToFloat(pVal)*255;
		stl->color.nTransparency = stl->backgroundColor.nTransparency;
	}
	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:extent", pVal, 60))
	{
		VO_CHAR* pSpace = strstr(pVal,"c");
		if (NULL != pSpace)
		{
			*pSpace = 0;
			stl->region.width = atoi(pVal);
			VO_CHAR* pSpace1 = strstr(pSpace+1,"c");
			if (NULL != pSpace1)
			{
				*pSpace1 = 0;
				stl->region.height = atoi(pSpace+1);
			}

		}
	}
	if(CXmlParserWraper::GetAttribute(subNodeBegin, "tts:origin", pVal, 60))
	{
		VO_CHAR* pSpace = strstr(pVal,"c");
		if (NULL != pSpace)
		{
			*pSpace = 0;
			stl->region.xPos = atoi(pVal);
			VO_CHAR* pSpace1 = strstr(pSpace+1,"c");
			if (NULL != pSpace1)
			{
				*pSpace1 = 0;
				stl->region.yPos = atoi(pSpace+1);
			}

		}
	}

	if(CXmlParserWraper::GetAttribute(subNodeBegin, "region", pVal, 60))
	{
		strcpy(stl->region.strID,pVal);
	}
}
bool	CTimedTextMLParser::ParseLine (char* pPage, char* pDivEnd)
{
	bool bReturn = true;
	if (m_pCurrTrack == NULL)
		m_pCurrTrack = CreateTextTrack();
	if (m_pCurrTrack == NULL)
		return false;
	char pVal[64];// = NULL;
	memset(pVal,0,64);
	VO_S32 LastDuration = -1;
	while(pPage)
	{
		pPage = CXmlParserWraper::CheckNodeHead(pPage, "<p");//strstr(pPage, "<p ");
		if(pPage == NULL)
			break;

		char* pPageEnd = strstr(pPage, "</p>");
		if(pPageEnd>pDivEnd || pPageEnd == NULL)
			break;
		pPageEnd+=4;
		char chOld = pPageEnd[0];
		pPageEnd[0] = 0;

		m_pCurrTrack->GetNewTextItem();

		int nHour1=0, nMin1=0, nSec1=0, nMS1=0;
		if(CXmlParserWraper::GetAttribute(pPage, "begin", pVal, 60))
		{
			StrToTime (pVal,  nHour1, nMin1, nSec1, nMS1);
			m_pCurrTrack->SetCurrTextItemStartTime((nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10 + m_llBaseTime);
		}
		nHour1=0; nMin1=0; nSec1=0; nMS1=0;
		if(CXmlParserWraper::GetAttribute(pPage, "end", pVal, 60))
		{
			StrToTime (pVal,  nHour1, nMin1, nSec1, nMS1);
			m_pCurrTrack->SetCurrTextItemEndTime((nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10 + m_llBaseTime);
		}
		else
		{
			CTextItem* pTextItem = m_pCurrTrack->GetCurrTextItem();
			if (NULL != pTextItem && m_uTimeContainer >= 0)
			{
				if (-1 == LastDuration)
				{
					m_pCurrTrack->SetCurrTextItemStartTime(0);
					LastDuration = 0;
				}
				else
					m_pCurrTrack->SetCurrTextItemStartTime(LastDuration);
			}
			if(CXmlParserWraper::GetAttribute(pPage, "dur", pVal, 60))
			{
				int nHour2=0; 
				int nMin2=0; 
				int nSec2=0; 
				int nMS2=0;
				StrToTime (pVal,  nHour2, nMin2, nSec2, nMS2);
				VO_U64 uTime = ((nHour1+nHour2) * 3600 + (nMin1+nMin2) * 60 + (nSec1+nSec2)) * 1000 + (nMS1+nMS2)*10 + m_llBaseTime;
				if (m_uTimeContainer >= 0)
				{
					m_pCurrTrack->SetCurrTextItemEndTime(uTime + LastDuration);
				}
				else
					m_pCurrTrack->SetCurrTextItemEndTime(uTime);

				LastDuration += uTime;
			}
		}
		CTextItem* pTextItem = m_pCurrTrack->GetCurrTextItem();
		if (NULL != pTextItem && pTextItem->m_nEndTime <= 0)
		{
			char* subNodeBegin = NULL;
			char* subNodeEnd = NULL;
			char *	pCurr2 = pPage;
			VO_S64 llBeginTime = -1;
			VO_S64 llEndTime = -1;
			while(1)
			{
				if(!CXmlParserWraper::GetSubNode(pCurr2,"span", subNodeBegin, subNodeEnd))
					break;
				if (subNodeEnd > pPageEnd)
				{
					break;
				}
				if(NULL != subNodeBegin && NULL != subNodeEnd)
				{
					memset(pVal,0,64);
					if(CXmlParserWraper::GetAttribute(subNodeBegin, "begin", pVal, 60))
					{
						StrToTime (pVal,  nHour1, nMin1, nSec1, nMS1);
						if (-1 == llBeginTime || llBeginTime > (nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10)
						{
							llBeginTime = (nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10;
						}
						if(llEndTime == -1 || llEndTime < (nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10)
						{
							llEndTime = (nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10 + 500;///<add a temp 500ms when no end time
						}
						
					}
					if(CXmlParserWraper::GetAttribute(subNodeBegin, "end", pVal, 60))
					{
						StrToTime (pVal,  nHour1, nMin1, nSec1, nMS1);
						if (llEndTime < (nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10 + m_llBaseTime)
						{
							llEndTime = (nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10 + m_llBaseTime;
						}
					}
				}
				pCurr2 = subNodeEnd;
			}
			m_pCurrTrack->SetCurrTextItemStartTime(llBeginTime);
			m_pCurrTrack->SetCurrTextItemEndTime(llEndTime);
		}

		m_pCurrTrack->AddCurrTextItemText(pPage);

		pPageEnd[0] = chOld;
		pPage = pPageEnd;
	}
	if(m_pCurrTrack->GetLanguage() == SUBTITLE_LANGUAGE_UNKNOWN)
		m_pCurrTrack->SmartLangCheck();

	return bReturn;
}

bool	CTimedTextMLParser::Parse (void)
{
	bool bReturn = true;
	char *	pNewLine = NULL;
	int		nSize = 0;
	char *	pEndTime = NULL;
	int		nLines = 0;

	if( m_pFileData == NULL)
		return false;

	if(this->GetParseFragmentStreaming())
	{
		return ParseLine (  (char*)m_pFileData,  ((char*)m_pFileData)+m_nFileSize);
	}

	char *	pCurr = (char*)m_pFileData;
	if(NULL == (pCurr = (CXmlParserWraper::CheckNodeHead(pCurr,  "<tt"))))
		pCurr = strstr((char*)m_pFileData, "<tt>");
	if( pCurr == NULL)
		return false;
	
	char pVal[64];
	memset(pVal,0,64);

	if(CXmlParserWraper::GetAttribute(pCurr, "ttp:cellResolution", pVal, 60))
	{
		VO_CHAR* pSpace = strstr(pVal," ");
		if (NULL != pSpace)
		{
			*pSpace = 0;
			m_ColumnCnt = atoi(pVal);
			m_RowCnt = atoi(pSpace+1);
		}
	}
	char* pTTBegin = pCurr;
	//find head node
	char *	pHead = CXmlParserWraper::CheckNodeHead(pCurr,  "<styling");//strstr(pCurr, "<styling");
	char *	pHeadEnd = strstr(pCurr, "</styling>");
	memset(pVal,0,64);
	if( pHeadEnd != NULL && pHead!=NULL)///<for this style,we use the NULL as style name
	{
		if(m_styleList != NULL)
		{
			char* subNodeBegin = NULL;
			char* subNodeEnd = NULL;
			char *	pCurr2 = pCurr;
			while(true)
			{//to parse style information
				if(!CXmlParserWraper::GetSubNode(pCurr2,"style", subNodeBegin, subNodeEnd))
					break;
				if(subNodeEnd>pHeadEnd)
					break;
				TT_Style* stl = new TT_Style();
				if(!stl)
					return false;
				if(CXmlParserWraper::GetAttribute(subNodeBegin, "style", pVal, 60))
				{
					TT_Style* pOld = TT_Style::GetTTStyle(m_styleList, pVal,true);
					if(pOld)
						*stl = *pOld;
				}
				FillAttribute(stl, subNodeBegin);
				m_styleList->AddTail(stl);

				pCurr2 = subNodeEnd;
				subNodeBegin = 0, subNodeEnd = 0;
			}// end parse style information
		}
	}
	//TO find layout node
	pHead = CXmlParserWraper::CheckNodeHead(pCurr,  "<layout");//strstr(pCurr, "<styling");
	pHeadEnd = strstr(pCurr, "</layout>");
	if( pHeadEnd != NULL && pHead!=NULL)
	{
		if(m_styleList != NULL)
		{
			char* subNodeBegin = NULL;
			char* subNodeEnd = NULL;
			char *	pCurr2 = pCurr;
			while(true)
			{//to parse style information
				memset(pVal,0,64);
				if(!CXmlParserWraper::GetSubNode(pCurr2,"region", subNodeBegin, subNodeEnd))
					break;
				if(subNodeEnd>pHeadEnd)
					break;
				TT_Style* stl = new TT_Style();
				if(!stl)
					return false;
				if(CXmlParserWraper::GetAttribute(subNodeBegin, "style", pVal, 60))
				{
					TT_Style* pOld = TT_Style::GetTTStyle(m_styleList, pVal,VO_TRUE);
					if(pOld)
						stl->StyleReplace(*pOld);
				}

				if(CXmlParserWraper::GetAttribute(subNodeBegin, "id", pVal, 60))///<for region style,we use region id as style name
					strcpy(stl->id, pVal);

				stl->ColumnCnt = m_ColumnCnt;
				stl->RowCnt = m_RowCnt;

				FillAttribute(stl, subNodeBegin);
				while(subNodeBegin < subNodeEnd)
				{
					char* pStyleElementBegin = NULL;
					char* pStyleElementEnd = NULL;
					if (CXmlParserWraper::GetSubNode(subNodeBegin,"style", pStyleElementBegin, pStyleElementEnd))
					{
						FillAttribute(stl, pStyleElementBegin);
						subNodeBegin = pStyleElementEnd;
						pStyleElementBegin = pStyleElementEnd = NULL;
					}
					else
						break;
				}
				

				m_styleList->AddTail(stl);

				pCurr2 = subNodeEnd;
				subNodeBegin = 0, subNodeEnd = 0;
			}// end parse style information
		}
	}//end layout node

	pCurr = CXmlParserWraper::CheckNodeHead(pCurr,  "<body");//strstr(pCurr, "<body ");
	if( pCurr == NULL)
		return false;
	char* pBodyEnd = strstr(pCurr,">");
	if( pBodyEnd == NULL)
		return false;

	TT_Style* stl = new TT_Style();///<for Body style,we use region id as style name
	if(!stl)
		return false;

	if(CXmlParserWraper::GetAttribute(pCurr, "style", pVal, 60) && (strlen(pVal) < 64))
	{
		strcpy(m_strBodyStyle,pVal);
		strcpy(stl->id,pVal);
	}
	else
	{
		strcpy(m_strBodyStyle, "body");
		strcpy(stl->id,"body");
	}

	FillAttribute(stl, pCurr);

	m_styleList->AddTail(stl);

	//to fill track data
	if (m_pCurrTrack == NULL)
		m_pCurrTrack = CreateTextTrack();
	if (m_pCurrTrack == NULL)
		return false;
	while(true)
	{
		pCurr = CXmlParserWraper::CheckNodeHead(pCurr,  "<div");//strstr(pCurr, "<div");
		if( pCurr == NULL)
		{
			goto END_SET_PARAM;
		}
		char* pDivEnd = strstr(pCurr,"</div>");
		if( pDivEnd == NULL)
			goto END_SET_PARAM;

		memset(pVal,0,64);
		if(CXmlParserWraper::GetAttribute(pCurr, "timeContainer", pVal, 60))
		{
			if (0 == strcmp(pVal,"seq"))
			{
				m_uTimeContainer = 0;
			}
			else if (0 == strcmp(pVal,"par"))
			{
				m_uTimeContainer = 1;
			}
		}

		TT_Style* stl = new TT_Style();
		if(!stl)
			return false;

		if(CXmlParserWraper::GetAttribute(pCurr, "style", pVal, 60) && (strlen(pVal) < 64))
		{
			strcpy(stl->StyleId,pVal);
		}

		strcpy(m_strDivStyle, "Div");
		strcpy(stl->id,"Div");

		FillAttribute(stl, pCurr);

		m_styleList->AddTail(stl);

		if(CXmlParserWraper::GetAttribute(pCurr, "xml:lang", pVal, 60))
		{
			this->SetTag(pVal);
		}
		else
		{
			if(CXmlParserWraper::GetAttribute(pTTBegin, "xml:lang", pVal, 60))
			{
				this->SetTag(pVal);
			}
			else
				this->SetTag("en");
		}

		pCurr+=4;
		//
		char* pPage = pCurr;
		ParseLine ( pPage,  pDivEnd);

		pCurr=pDivEnd+6;
	}

END_SET_PARAM:
	//if (m_pFileData[0] == 0xEF && m_pFileData[1] == 0xBB) {
	//for xml, we regard it as utf8
	m_nFileFormat = (SUBTITLE_FILEFORMAT_UTF8);

	return bReturn;
}

VO_BOOL CTimedTextMLParser::SetBeginEndTime(char* xmlBuffer, char* outAttributeVal, VO_S64 llBaseTime)
{
	int nHour1=0, nMin1=0, nSec1=0, nMS1=0;
	if(CXmlParserWraper::GetAttribute(xmlBuffer, "begin", outAttributeVal, 60))
	{
		StrToTime (outAttributeVal,  nHour1, nMin1, nSec1, nMS1);
		m_pCurrTrack->SetCurrTextItemStartTime((nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10 + llBaseTime);
	}
	nHour1=0; nMin1=0; nSec1=0; nMS1=0;
	if(CXmlParserWraper::GetAttribute(xmlBuffer, "end", outAttributeVal, 60))
	{
		StrToTime (outAttributeVal,  nHour1, nMin1, nSec1, nMS1);
		m_pCurrTrack->SetCurrTextItemEndTime((nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1*10 + llBaseTime);
	}
	else
	{
		if(CXmlParserWraper::GetAttribute(xmlBuffer, "dur", outAttributeVal, 60))
		{
			int nHour2=0; 
			int nMin2=0; 
			int nSec2=0; 
			int nMS2=0;
			StrToTime (outAttributeVal,  nHour2, nMin2, nSec2, nMS2);
			m_pCurrTrack->SetCurrTextItemEndTime(((nHour1+nHour2) * 3600 + (nMin1+nMin2) * 60 + (nSec1+nSec2)) * 1000 + (nMS1+nMS2)*10 + llBaseTime);
		}
	}
	return VO_TRUE;
}