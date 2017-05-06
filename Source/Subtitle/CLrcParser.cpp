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
* @file CLrcParser.cpp
*
* @author  Ferry Zhang
* 
* Change History
* 2013-01-08    Create File
************************************************************************/

#define LOG_TAG "CLrcParser"

#include "CLrcParser.h"
#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define is_digit(d)	((d) >= '0' && (d) <= '9')
//#pragma warning (disable : 4996)

CLrcParser::CLrcParser(void)
{	
	
}

CLrcParser::~CLrcParser(void)
{

}

bool	CLrcParser::Parse (void)
{
	m_pCurrTrack = CreateTextTrack();
	if (m_pCurrTrack == NULL)
		return false;

	char*	pNewLine = NULL;
	int		nSize = 0;
	char*	pTagStart = NULL;
	char* 	pTagEnd = NULL;
	char*	pString = NULL;
	char*  	pTmpString = NULL;
	
	
	
	int nMin1, nSec1, nMS1;
	nMin1 = nSec1 = nMS1 = 0;

	if (!ReadNextLine (&pNewLine, nSize))
		return false;

	do
	{
		pNewLine[nSize] = 0;
		pTagStart = strstr(pNewLine, "[");
		if(!pTagStart)
			continue;
		pTagEnd = strstr(pNewLine,"]");
		if(!pTagEnd)
			continue;
		do
		{
			if(is_digit(pTagStart[1]))
			{
				m_pCurrTrack->GetNewTextItem();
				
				if(pTagEnd - pTagStart == 9)
				{
					if(strstr(pTagStart,"."))
					{
						::sscanf (pNewLine, "[%02d:%02d.%02d]", &nMin1, &nSec1, &nMS1);
					}
					else
					{
						::sscanf (pNewLine, "[%02d:%02d:%02d]", &nMin1, &nSec1, &nMS1);
					}
				}
				else if(pTagEnd - pTagStart == 6)
				{
					::sscanf (pNewLine, "[%02d:%02d]", &nMin1, &nSec1);
				}
				m_pCurrTrack->SetCurrTextItemStartTime((nMin1 * 60 + nSec1) * 1000 + nMS1);
				pString = strstr(pNewLine,"]");
				while(pString)
				{
					pTmpString = pString;
					pString = strstr(pString + 1,"]");
				}
				VO_CHAR* pText = m_pCurrTrack->AddCurrTextItemText(pTmpString + 1);
				FilterText(pText);

				pTagStart = strstr(pTagEnd + 1,"[");
				pTagEnd = strstr(pTagEnd + 1,"]");
				continue;
			}
			else
			{	
				if(strstr(pTagStart,"offset"))
				{
					pTagStart = strstr(pTagEnd + 1,"[");
					pTagEnd = strstr(pTagEnd + 1,"]");
					continue;
				}
				else
				{
					//ignore [ar:artist]/[ti:song]/[al:album]/[by:writer]
					pTagStart = strstr(pTagEnd + 1,"[");
					pTagEnd = strstr(pTagEnd + 1,"]");
					continue;
				}
			}	
		}while(pTagStart && pTagEnd);
	}while (ReadNextLine (&pNewLine, nSize));

	for (int i = 0; i < m_nTrackCount; i++) {
		GetTrack(i)->FillEndTime();
	}
	return true;
}

