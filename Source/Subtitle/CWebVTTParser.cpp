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
* @file CWebVTTParser.cpp
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#define LOG_TAG "CWebVTTParser"

#include "CWebVTTParser.h"
#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//#pragma warning (disable : 4996)

CWebVTTParser::CWebVTTParser(void)
{	
	
}

CWebVTTParser::~CWebVTTParser(void)
{

}

bool	CWebVTTParser::Parse (void)
{
	m_pCurrTrack = CreateTextTrack();
	if (m_pCurrTrack == NULL)
		return false;

	char *	pNewLine = NULL;
	int		nSize = 0;
	char *	pEndTime = NULL;
	char		text[1024];
	int		nCopyLength = 0;
	char*	pTagData = NULL;
	
	// Check the head ID
	if (!ReadNextLine (&pNewLine, nSize))
		return false;
	pNewLine = strstr(pNewLine,"WEBVTT");		//why do this,cause there is some ugly signal in the first line,only a work around way
	//VOLOGI("Ferry debug print pNewLine is %s",pNewLine);
	if (memcmp(pNewLine, "WEBVTT", 6))
	{		
		VOLOGE("WebVTT signature is not found,abort it");
		return false;
	}
	VOLOGI("WEBVTT was Found");
	int nHour1, nMin1, nSec1, nMS1;
	int nHour2, nMin2, nSec2, nMS2;
	int nHourLen1,nHourLen2;

	while(ReadNextLine (&pNewLine, nSize))
	{
		pNewLine[nSize] = 0;
		if(strstr (pNewLine, "-->"))	//find first timestamp
			break;
	}
	
	do
	{
		pEndTime = NULL;
		pNewLine[nSize] = 0;
		pEndTime = strstr (pNewLine, "-->");
		if(pEndTime)
		{
			m_pCurrTrack->GetNewTextItem();
			memset(text,0,1024);
			nCopyLength = 0;
			nHourLen1 = strlen(pNewLine) - strlen(strstr(pNewLine,":"));
			switch(nHourLen1)
			{
				case 1:
					::sscanf (pNewLine, "%01d:%02d:%02d.%03d", &nHour1, &nMin1, &nSec1, &nMS1);
					break;
				case 2:
					::sscanf (pNewLine, "%02d:%02d:%02d.%03d", &nHour1, &nMin1, &nSec1, &nMS1);
					break;
				case 3:
					::sscanf (pNewLine, "%03d:%02d:%02d.%03d", &nHour1, &nMin1, &nSec1, &nMS1);
					break;
				default:
					::sscanf (pNewLine, "%02d:%02d:%02d.%03d", &nHour1, &nMin1, &nSec1, &nMS1);
					break;
			}
		//	VOLOGI("%d:%d:%d:%d",nHour1,nMin1,nSec1,nMS1);
			m_pCurrTrack->SetCurrTextItemStartTime((nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1);

			pEndTime += 3;
			while (pEndTime[0] == ' ' && pEndTime - pNewLine < nSize - 3)
				pEndTime++;
			nHourLen2 = strlen(pEndTime) - strlen(strstr(pEndTime,":"));
			switch(nHourLen2)
			{
				case 1:
					::sscanf (pEndTime, "%01d:%02d:%02d.%03d", &nHour2, &nMin2, &nSec2, &nMS2);
					break;
				case 2:
					::sscanf (pEndTime, "%02d:%02d:%02d.%03d", &nHour2, &nMin2, &nSec2, &nMS2);
					break;
				case 3:
					::sscanf (pEndTime, "%03d:%02d:%02d.%03d", &nHour2, &nMin2, &nSec2, &nMS2);
					break;
				default:
					::sscanf (pEndTime, "%02d:%02d:%02d.%03d", &nHour2, &nMin2, &nSec2, &nMS2);
					break;
			}
		//	VOLOGI("%d:%d:%d:%d",nHour2,nMin2,nSec2,nMS2);
			m_pCurrTrack->SetCurrTextItemEndTime((nHour2 * 3600 + nMin2 * 60 + nSec2) * 1000 + nMS2);
			//Parse webvtt tag here
			//Parse vertical
			pTagData = strstr(pNewLine,"vertical");
			if(pTagData)
			{
				pTagData += 9;	// vertical:
				if(pTagData[0] == 'r')
				{
					m_pCurrTrack->SetCurrTextItemVertical(0);
				}
				else if(pTagData[0] == 'l')
				{
					m_pCurrTrack->SetCurrTextItemVertical(1);
				}
				pTagData = NULL;
			}
			//Parse Line
			pTagData = strstr(pNewLine,"line");
			if(pTagData)
			{
				pTagData += 5;
				if(strstr(pTagData,"%"))
				{
					m_pCurrTrack->SetCurrTextItemLine(atol(pTagData));
				}
				else if(strstr(pTagData,"-"))
				{
					//TODO cause our limitation(subtitle parser don`t know the resolution),this feature cannot be supported
				}
				pTagData = NULL;
			}
			//Parse position
			pTagData = strstr(pNewLine,"position");
			if(pTagData)
			{
				pTagData += 9;
				if(strstr(pTagData,"%"))
				{
					m_pCurrTrack->SetCurrTextItemPosition(atol(pTagData));
				}
				pTagData = NULL;
			}
			//Parse size
			pTagData = strstr(pNewLine,"size");
			if(pTagData)
			{
				pTagData += 5;
				if(strstr(pTagData,"%"))
				{
					m_pCurrTrack->SetCurrTextItemSize(atol(pTagData));
				}
				pTagData = NULL;
			}
			//Parse align
			pTagData = strstr(pNewLine,"align");
			if(pTagData)
			{
				pTagData += 6;
				if(pTagData[0] == 's')
				{
					m_pCurrTrack->SetCurrTextItemAlign(0);
				}
				else if(pTagData[0] == 'm')
				{
					m_pCurrTrack->SetCurrTextItemAlign(2);
				}
				else if(pTagData[0] == 'e')
				{
					m_pCurrTrack->SetCurrTextItemAlign(1);
				}
				else if(pTagData[0] == 'l')
				{
					m_pCurrTrack->SetCurrTextItemAlign(3);
				}
				else if(pTagData[0] == 'r')
				{
					m_pCurrTrack->SetCurrTextItemAlign(4);
				}
				pTagData = NULL;
			}
		}
		else
		{
			//nLines = atol (pNewLine);
			//if(nLines != 0 || strlen(pNewLine) == 0)
			if(strlen(pNewLine) == 0)	//do not check atol to avoid if text first char is number
			{
				//text[nCopyLength - 1] = '\0'; 		//remove the last '\n'
				VO_CHAR* pText = m_pCurrTrack->AddCurrTextItemText(text);
				memset(text,0,1024);
				FilterText(pText);
				continue;
			}
			
			pNewLine[nSize] = '\n';
			if(nCopyLength + (nSize + 1) > 1024)
			{
				VOLOGE("Missing some subtitle characters,continue to parse next one");
				nCopyLength += nSize + 1;
				continue;
			}
			memcpy(text + nCopyLength,pNewLine,nSize + 1);
			nCopyLength += nSize + 1;
		}
	}while (ReadNextLine (&pNewLine, nSize));
	//for webvtt, we regard it as utf8,please take webvtt standard for reference
	m_nFileFormat = (SUBTITLE_FILEFORMAT_UTF8);
	return true;
}
