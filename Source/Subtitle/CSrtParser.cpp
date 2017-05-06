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
* @file CSrtParser.cpp
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#define LOG_TAG "CSrtParser"

#include "CSrtParser.h"
#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//#pragma warning (disable : 4996)

CSrtParser::CSrtParser(void)
{	
	
}

CSrtParser::~CSrtParser(void)
{

}

bool	CSrtParser::Parse (void)
{
	m_pCurrTrack = CreateTextTrack();
	if (m_pCurrTrack == NULL)
		return false;


	char *	pNewLine = NULL;
	int		nSize = 0;

	char *	pEndTime = NULL;
	int		nLines = 0;

	// Check the head ID
	if (!ReadNextLine (&pNewLine, nSize))
		return false;

	int nHour1, nMin1, nSec1, nMS1;
	int nHour2, nMin2, nSec2, nMS2;

	do
	{
		pNewLine[nSize] = 0;

		if (strlen (pNewLine) > 12 && (pNewLine[2] == ':' && pNewLine[5] == ':' &&  pNewLine[8] == ','))
		{
			m_pCurrTrack->GetNewTextItem();
		
			pEndTime = strstr (pNewLine, "-->");
			if (pEndTime != NULL)
			{
				*pEndTime = 0;
				::sscanf (pNewLine, "%2d:%02d:%02d,%03d", &nHour1, &nMin1, &nSec1, &nMS1);			
				m_pCurrTrack->SetCurrTextItemStartTime((nHour1 * 3600 + nMin1 * 60 + nSec1) * 1000 + nMS1);

				pEndTime += 3;
				while (pEndTime[0] == ' ' && pEndTime - pNewLine < nSize - 3)
					pEndTime++;
				::sscanf (pEndTime, "%2d:%02d:%02d,%03d", &nHour2, &nMin2, &nSec2, &nMS2);	
				m_pCurrTrack->SetCurrTextItemEndTime((nHour2 * 3600 + nMin2 * 60 + nSec2) * 1000 + nMS2);
			}
		}
		else
		{
			nLines = atol (pNewLine);
			if (nLines == 0 && strlen (pNewLine) > 0)
			{
				VO_CHAR* pText = m_pCurrTrack->AddCurrTextItemText(pNewLine);
				FilterText(pText);
				//VOLOGI("Subtitle Text: %s",pText);
			}
		}

		pNewLine[nSize] = '\r';
	}	while (ReadNextLine (&pNewLine, nSize));
	if(m_pCurrTrack->GetLanguage() == SUBTITLE_LANGUAGE_UNKNOWN)
		m_pCurrTrack->SmartLangCheck();
	//VOLOGI("return true");
	return true;
}

