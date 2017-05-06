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
* @file CSMIParser.cpp
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#define LOG_TAG "CSMIParser"

#include "CSMIParser.h"
#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//#pragma warning (disable : 4996)


CSMIParser::CSMIParser(void) :
		m_pLanguageTable(NULL) {

}

CSMIParser::~CSMIParser(void) {
	//if (m_pLanguageTable)
	//{
	//	VO_BYTE* pTemp = NULL;
	//	VO_BYTE* pNext = m_pLanguageTable;
	//	while(pNext)
	//	{
	//		pTemp = pNext;
	//		pNext = ((LANGUAGETABLE*)pNext)->pNext;
	//		delete pTemp;
	//	}
	//}	
}

bool CSMIParser::Parse(void) {
	m_pCurrTrack = CreateTextTrack();
	if (m_pCurrTrack == NULL
		)
		return false;

	char * pNewLine = NULL;
	int nSize = 0;
	char * pStart = NULL;
	char * pEnd = NULL;
	int nTextLen = 0;
	int nColor = 0XFFFFFF;
	//int count = 1;
	char szText[1024];
	int nNewTextSize = 2048;
	char * pNewText = new char[nNewTextSize];
	if(!pNewText)
		return false;

	// Check the head ID
	if (!ReadNextLine(&pNewLine, nSize))
	{
		delete pNewText;
		pNewText = NULL;
		return false;
	}
	// if (memcmp(pNewLine, "SAMI", 4))
		// return false;
		// VOLOGI("SAMI was Found");
		
	// Remove the head data
	while (ReadNextLine(&pNewLine, nSize)) {
		/*if (!memcmp(pNewLine, ".", 1))
		 {
		 AddToCharsetTable(pNewLine+1, GetLangID(pNewLine));
		 }*/

		if (!memcmp(pNewLine, "<SYNC", 5) || !memcmp(pNewLine, "<Sync", 5))
			break;
	}

	// FixWrongCharsetTable();

	long mPrevpos = 0, mCurrpos = 0; // Init

	// File text item info, begin from a fisrt line.

	do {
		if (!memcmp(pNewLine, "</BODY>", 7))
			break;

		if (nSize <= 0)
			continue;

		pNewLine[nSize] = 0;

		//toUpperString(pNewLine);
		//VOLOGI("---> Subtitle Current line strings : %s ",pNewLine);
        FilterText(pNewLine);

		bool bIsTag = !memcmp(pNewLine, "<SYNC", 5)
				|| !memcmp(pNewLine, "<Sync", 5);

		if (bIsTag) {
			m_pCurrTrack->GetNewTextItem();

			pStart = strstr(pNewLine, "=") + 1; //Start tag , it's a subtitle timestamp
			if (pStart == NULL)
				continue;

			pEnd = strstr(pNewLine, ">"); 
			//VOLOGI("pStart : %s || pEnd : %s",pStart, pEnd);

			if (pEnd != NULL) {  // some subtitle files no find this tag ">".
			   pStart = strtok(pStart, pEnd);
			   mCurrpos = strtol(pStart, NULL, 0);
			}
			else
			   mCurrpos = strtol(pStart, NULL, 0);
			//VOLOGI("Current Position: %d", mCurrpos);

			// adjust wrong timestamp tag 
			if (mCurrpos < mPrevpos) // current position time small than previous is npt true.
				mCurrpos = mPrevpos; // offset 1000ms
			else if ((mCurrpos - mPrevpos) > 5 * 60000) // Don't allow next time geater than lat time 5 mins.
				mCurrpos = mPrevpos;
			//VOLOGI("Prev: %d",mPrevpos);
			mPrevpos = mCurrpos; // save current position into mPrevpos.

			if (pEnd == NULL)
			{
				m_pCurrTrack->SetCurrTextItemStartTime(mCurrpos);
			} else {
				*pEnd = 0;
				m_pCurrTrack->SetCurrTextItemStartTime(mCurrpos);
				*pEnd = '>';
			}

			pStart = strstr(pNewLine, "CLASS=");
			if (pStart == NULL)
				strstr(pNewLine, "Class=");
			if (pStart != NULL)
			{
				SetTag(pStart + 6);

				pStart = pStart + 6;
				char* pNextStart = strstr(pStart, ">");

				if (pNextStart != NULL)
				{
					pNextStart++;
					if (*pNextStart != 0) {
						int nDiff = nSize - (pNextStart - pNewLine);
						if (nDiff > 0) {
							bIsTag = false;
							nSize = nDiff;
							pNewLine = pNextStart;
						}
					}
				}

			}
		}

		if (!bIsTag) {
			strcpy(szText, "");

			if (nSize > nNewTextSize) {
				nNewTextSize = nSize + 1;
				if (pNewText != NULL)
				{
					delete[] pNewText;
					pNewText = NULL;
				}
				pNewText = new char[nNewTextSize];
				if(!pNewText)
					return false;
			}
			strcpy(pNewText, pNewLine);

			char * pTextLine = pNewLine;
			int nTextSize = nSize;

			//toUpperString(pTextLine);

			while (nTextSize > 0) {
				if (pTextLine[0] == '<') {
					if (!memcmp(pTextLine, "<P CLASS=", 9) || !memcmp(pTextLine, "<P Class=", 9) ) {
						pStart = pTextLine+3;//strstr(pTextLine, "CLASS=");
						if (pStart != NULL)
						{
							SetTag(pStart + 6);
						}
					}

					if (!memcmp(pTextLine, "<FONT", 5) || !memcmp(pTextLine, "<Font", 5)) {
						pStart = strstr(pTextLine, "COLOR=\"");
						if (pStart == NULL)
							pStart = strstr(pTextLine, "Color=\"");
						if (pStart != NULL)
						 {
							pStart = pStart + 7;
							if (pStart[0] == '#')
								pStart++;
							sscanf (pStart, "%X", &nColor);
							if (nColor != 0)
								m_pCurrTrack->SetCurrTextItemColor( nColor);
						 }

					} else if (!memcmp(pTextLine, "<BR", 3) ||!memcmp(pTextLine, "<Br", 3) ||!memcmp(pTextLine, "<br", 3) || !memcmp(pTextLine, "<B", 2)|| !memcmp(pTextLine, "<b", 2)) {
						if (ContainChar(szText)) {
							VO_CHAR* pText = m_pCurrTrack->AddCurrTextItemText(szText);
							FilterText(pText);
							//VOLOGI("Subtitle Curr: %d , Text: %s", mCurrpos,szText);
						}
						strcpy(szText, "");
					}

					pEnd = strstr(pTextLine, ">");
					if (pEnd == NULL) break;

					nTextSize = nTextSize - (pEnd - pTextLine) - 1;
					pTextLine = pEnd + 1;
					continue;
				} else {
					nTextLen = nTextSize;
					pStart = strstr(pTextLine, "<");
					if (pStart != NULL)
					{
						nTextLen = pStart - pTextLine;
						strncat(szText, pNewText + (pTextLine - pNewLine),
								nTextLen);
						nTextSize = nTextSize - (pStart - pTextLine);
						pTextLine = pStart;
						continue;
					} else {
						strncat(szText, pNewText + (pTextLine - pNewLine),
								nTextLen);
						break;
					}
				}
			}

			if (ContainChar(szText)) {
				m_pCurrTrack->SetCurrTextItemStartTime(mCurrpos);
				VO_CHAR* pText = m_pCurrTrack->AddCurrTextItemText(szText);
				FilterText(pText);
				// VOLOGI(" Curr: %d , Text: %s", mCurrpos, szText);
			}
		}
		pNewLine[nSize] = '\r';
	} while (ReadNextLine(&pNewLine, nSize));

	if (pNewText)
	{
		delete[] pNewText;
		pNewText = NULL;
	}

	//VOLOGI("Track count %d", m_nTrackCount);

	//bool bUTF8 = false;
	if (m_pFileData[0] == 0xEF && m_pFileData[1] == 0xBB) {
		for (int i = 0; i < m_nTrackCount; i++) {
			GetTrack(i)->SetFileFormat(SUBTITLE_FILEFORMAT_UTF8);
		}
	}

	for (int i = 0; i < m_nTrackCount; i++) {
		GetTrack(i)->FillEndTime();
		if(GetTrack(i)->GetLanguage() == SUBTITLE_LANGUAGE_UNKNOWN)
			GetTrack(i)->SmartLangCheck();
	}

	return true;
}

bool CSMIParser::ContainChar(char * pText) {
#ifndef _WIN32
	if (!vostricmp(pText, "&NBSP;") || !vostricmp(pText, "&NBSP") || !vostricmp(pText, "&nbsp;") || !vostricmp(pText, "&nbsp"))
		return false;

	for (int i = 0; i < (int) vostrlen(pText); i++) {

		if (pText[i] != ' ')
			return true;
	}
#else
	if (!strcmp(pText, "&NBSP;") || !strcmp(pText, "&NBSP") || !strcmp(pText, "&nbsp;") || !strcmp(pText, "&nbsp"))
		return false;
	for (int i = 0; i < (int) strlen(pText); i++) {

		if (pText[i] != ' ')
			return true;
	}
#endif

	return false;
}

void CSMIParser::AddToCharsetTable(char* pText, Subtitle_Language nLanguage) {
	VO_BYTE* pCurrent = NULL;
	if (m_pLanguageTable == NULL)
	{
		m_pLanguageTable = new VO_BYTE[sizeof(LANGUAGETABLE)];
		pCurrent = m_pLanguageTable;
	} else {
		VO_BYTE* pPrev = NULL;
		pCurrent = m_pLanguageTable;
		while (pCurrent) {
			pPrev = pCurrent;
			pCurrent = ((LANGUAGETABLE*) pCurrent)->pNext;
		}

		((LANGUAGETABLE*) pPrev)->pNext = new VO_BYTE[sizeof(LANGUAGETABLE)];
		pCurrent = ((LANGUAGETABLE*) pPrev)->pNext;

	}
	if(!pCurrent)
		return;
	memset(pCurrent, 0, sizeof(LANGUAGETABLE));

	LANGUAGETABLE* pTable = (LANGUAGETABLE*) pCurrent;
	pTable->nLang = nLanguage;
	memcpy(pTable->szLang, pText, 4);
	memset(pTable->szLang + 4, 0, 1);

}

Subtitle_Language CSMIParser::GetLangID(char* pText) {
	Subtitle_Language nReturn = SUBTITLE_LANGUAGE_UNKNOWN;

	char* pStart = strstr(pText, "lang:");

	if (pStart == NULL
		)
		pStart = strstr(pText, "Lang:");
	if (pStart) {
		pStart += 5;
		if (!memcmp(pStart, "en", 2))
			nReturn = SUBTITLE_LANGUAGE_ENGLISH;
		else if (!memcmp(pStart, "EN", 2))
			nReturn = SUBTITLE_LANGUAGE_ENGLISH;
		else if (!memcmp(pStart, "ko", 2))
			nReturn = SUBTITLE_LANGUAGE_KOREAN;
		else if (!memcmp(pStart, "kr", 2))
			nReturn = SUBTITLE_LANGUAGE_KOREAN;
	}
	return nReturn;
}

Subtitle_Language CSMIParser::FindCharsetInCharsetTable(char* pText) {
	Subtitle_Language nReturn = SUBTITLE_LANGUAGE_UNKNOWN;

	VO_BYTE* pCurrent = m_pLanguageTable;

	while (pCurrent) {
		LANGUAGETABLE* pTable = (LANGUAGETABLE*) pCurrent;
		if (!memcmp(pTable->szLang, pText, 4)) {
			nReturn = pTable->nLang;
			break;
		}
		pCurrent = ((LANGUAGETABLE*) pCurrent)->pNext;
	}

	return nReturn;
}

void CSMIParser::FixWrongCharsetTable() {
	VO_BYTE* pCurrent = m_pLanguageTable;

	int nEngLangCount = 0;
	int nKoreanLangCount = 0;

	while (pCurrent) {
		LANGUAGETABLE* pTable = (LANGUAGETABLE*) pCurrent;
		if (pTable->nLang == SUBTITLE_LANGUAGE_ENGLISH) {
			nEngLangCount++;
		} else if (pTable->nLang == SUBTITLE_LANGUAGE_KOREAN) {
			nKoreanLangCount++;
		}
		pCurrent = ((LANGUAGETABLE*) pCurrent)->pNext;
	}

	if (nEngLangCount > 1) {
		pCurrent = m_pLanguageTable;
		while (pCurrent) {
			LANGUAGETABLE* pTable = (LANGUAGETABLE*) pCurrent;
			if (!memcmp(pTable->szLang, "KRCC", 4)
					|| !memcmp(pTable->szLang, "EGCC", 4)) {
				pTable->nLang = SUBTITLE_LANGUAGE_KOREAN;
				break;
			}
			pCurrent = ((LANGUAGETABLE*) pCurrent)->pNext;
		}
	}

	if (nKoreanLangCount > 1) {
		pCurrent = m_pLanguageTable;
		while (pCurrent) {
			LANGUAGETABLE* pTable = (LANGUAGETABLE*) pCurrent;
			if (!memcmp(pTable->szLang, "ENCC", 4)) {
				pTable->nLang = SUBTITLE_LANGUAGE_ENGLISH;
				break;
			}
			pCurrent = ((LANGUAGETABLE*) pCurrent)->pNext;
		}
	}
}
