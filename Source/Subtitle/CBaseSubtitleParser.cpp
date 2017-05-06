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
* @file CBaseSubtitleParser.cpp
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#define LOG_TAG "CBaseSubtitleParser"

#include "CBaseSubtitleParser.h"
#include "CTextTrack.h"

#include "cmnFile.h"
#include "voFile.h"
#include "voLog.h"
#ifndef _WIN32
#include "wchar.h"
#endif
#include "voOMXFile.h"
#include "CFileFormatConverter.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//#pragma warning (disable : 4996)
void toUpperString(char *src) {
	int i;
	i = 0;
	while (src[i] != 0) {
		if ((src[i] <= 'z') && (src[i] >= 'a'))
			src[i] = src[i] - 'a' + 'A';
		i++;
	}
}

#ifdef _WIN32
bool WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_ACP,NULL,lpcwszStr,-1,NULL,0,NULL,FALSE);
	if(dwSize < dwMinSize)
	{
		return false;
	}
	WideCharToMultiByte(CP_ACP,NULL,lpcwszStr,-1,lpszStr,dwSize,NULL,FALSE);
	return true;
}

bool MByteToWChar(LPCSTR lpcszStr, LPWSTR lpwszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar (CP_UTF8, 0, lpcszStr, -1, NULL, 0);

	//CP_ACP
	if(dwSize < dwMinSize)
	{
		return false;
	}

	// Convert headers from ASCII to Unicode.
	MultiByteToWideChar (CP_UTF8, 0, lpcszStr, -1, lpwszStr, dwMinSize);  
	return true;
}
#else
bool WCharToMByte(VO_TCHAR* lpcwszStr, VO_CHAR* lpszStr, int dwSize)
{
	vostrcpy(lpszStr, lpcwszStr);
	return 1;
}

bool MByteToWChar(VO_CHAR* lpszStr, VO_TCHAR* lpwszStr, int dwSize)
{
	vostrcpy(lpwszStr, lpszStr);
	return 1;
}
#endif

CBaseSubtitleParser::CBaseSubtitleParser(void)
: m_nTrackCount(0)
, m_nCurrTrackIndex(0)
, m_pCurrTrack(NULL)
, m_nFileSize()
, m_pFileData(NULL)
, m_pCurPos(NULL)
, m_jniEnv(NULL)
, m_styleList(NULL)
, m_nFileFormat(SUBTITLE_FILEFORMAT_NORMAL)
, m_bParseFragmentStreaming(false)
{	
	memset(m_strBodyStyle,0,64);
	memset(m_strDivStyle,0,64);
}

CBaseSubtitleParser::~CBaseSubtitleParser(void)
{
	if (m_pFileData != NULL)
	{
		delete []m_pFileData;
		m_pFileData = NULL;
	}	
}


voSubtitleCodingType CBaseSubtitleParser::GetParserType(VO_TCHAR * pFilePath)
{
	voSubtitleCodingType nReturnType = VO_SUBTITLE_CodingUnused;

	VO_TCHAR* pExt = vostrrchr(pFilePath, '.');
	//VOLOGI("Ext is %s", pExt);

	if (pExt == NULL) {     	
		return nReturnType;
	}
#ifndef _WIN32
	if (CheckStr(pExt, ".srt") || CheckStr(pExt, ".SRT"))
		nReturnType = VO_SUBTITLE_CodingSRT;

	//Check SAMI File Tag for parser
	if (CheckStr(pExt, ".smi") || CheckStr(pExt, ".SMI")) {
		FILE * pFile;
		char buf[20];

		pFile = fopen(pFilePath, "rb");
		if (!pFile == 0) {
			fscanf(pFile, "%[^\n]", buf);
			//VOLOGI("Subtitle file source: %s",pFilePath);
			//VOLOGI("Subtitle Tag string : %s ",buf);
			//VOLOGI("Subtitle Tag buf length : %d",strlen(buf));

			if (strstr(buf, "SAMI") != NULL) {
				nReturnType = VO_SUBTITLE_CodingSMI;
				//VOLOGI("Subtitle type is SAMI format %s",pFilePath);
			} else if (strstr(buf, "\0xFF\0xFE<") != NULL && strlen(buf) == 3) {
				nReturnType = VO_SUBTITLE_CodingSMI;
				//VOLOGI("Subtitle type is SAMI format %s",pFilePath);
			} else {
				nReturnType = VO_SUBTITLE_CodingSRT;
				//VOLOGI("Subtitle type is SRT format %s",pFilePath);
			}
			fclose(pFile);
		}
	}
	if (CheckStr(pExt, ".xml") || CheckStr(pExt, ".XML"))
		nReturnType = VO_SUBTITLE_CodingTTML;
	if (CheckStr(pExt, ".vtt") || CheckStr(pExt, ".VTT"))
		nReturnType = VO_SUBTITLE_CodingWEBVTT; 
#else
	nReturnType = VO_SUBTITLE_CodingSMI;
	if (CheckStr(pExt, ".srt") || CheckStr(pExt, ".SRT"))
		nReturnType = VO_SUBTITLE_CodingSRT;
	if (CheckStr(pExt, ".xml") || CheckStr(pExt, ".XML"))
		nReturnType = VO_SUBTITLE_CodingTTML;
	if (CheckStr(pExt, ".vtt") || CheckStr(pExt, ".VTT"))
		nReturnType = VO_SUBTITLE_CodingWEBVTT;
	if (CheckStr(pExt, ".lrc") || CheckStr(pExt, ".LRC"))
		nReturnType = VO_SUBTITLE_CodingLRC; 
#endif

	if(nReturnType == VO_SUBTITLE_CodingUnused)
	{
		void* hFile = voOMXFileOpen ((OMX_STRING) pFilePath, VOOMX_FILE_READ_ONLY);
		if (hFile != NULL)
		{
			char* fileBuffer = new char[32];
			voOMXFileRead(hFile,(OMX_U8*)fileBuffer,32);
			if(strstr(fileBuffer,"SAMI"))
			{
				nReturnType = VO_SUBTITLE_CodingSMI;
			}
			else if(strstr(fileBuffer,"WEBVTT"))
			{
				nReturnType = VO_SUBTITLE_CodingWEBVTT;
			}
			else if(strstr(fileBuffer,"xml"))
			{
				nReturnType = VO_SUBTITLE_CodingTTML;
			}
			else
			{
				//VOLOGI("subtitle is not founded when GetParserType");
			}
			delete []fileBuffer;
			fileBuffer = NULL;
		}
	}
	//VOLOGI("Subtitle type is : %d", nReturnType);
	return nReturnType;
}
bool	CBaseSubtitleParser::SetSubtitleData(VO_CHAR * pData, int len)
{
	m_nFileSize = (int)len;
	if(m_nFileSize>2)
	{
		if (pData[0] == 0xFF &&  pData[1] == 0xFe)
		{
			m_pFileData = new VO_BYTE[m_nFileSize*3/2];//if it is unicode, we enlarge the buffer
			if(!m_pFileData)
				return false;
			memset(m_pFileData, 0 ,m_nFileSize*3/2);
		}
		else
		{
			m_pFileData = new VO_BYTE[m_nFileSize+32];
			if(!m_pFileData)
				return false;
			memset(m_pFileData, 0 ,m_nFileSize+32);
		}
		memcpy(m_pFileData,pData,len);

		m_pCurPos = m_pFileData;

		if(m_pFileData)
		{
			CFileFormatConverter	converter;
			if(converter.Convert(this->m_jniEnv, m_pFileData, &m_nFileSize))
				this->m_nFileFormat = SUBTITLE_FILEFORMAT_UTF8;
		}
	}
	else
		m_nFileSize = 0;
	return true;
}
bool	CBaseSubtitleParser::SetSubtitleFile(VO_TCHAR * pFilePath)
{
	VO_FILE_SOURCE filSource;
	memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
	filSource.pSource = (VO_PTR) pFilePath;
	filSource.nFlag = VO_FILE_TYPE_NAME;
	filSource.nMode = VO_FILE_READ_ONLY;
	filSource.nOffset = 0;
	filSource.nLength = 0;
	filSource.nReserve = 0;

	OMX_PTR hFile = voOMXFileOpen ((OMX_STRING) pFilePath, VOOMX_FILE_READ_ONLY);
	if (hFile != NULL)
	{
		m_nFileSize = (int) voOMXFileSize (hFile);//cmnFileSize (hFile);
		if(m_nFileSize>0)
		{
			VO_BYTE bt[2];
			voOMXFileRead (hFile, bt, (OMX_U32) 2);
			if (bt[0] == 0xFF &&  bt[1] == 0xFe)
			{
				m_pFileData = new VO_BYTE[m_nFileSize*3/2];//if it is unicode, we enlarge the buffer
				if(!m_pFileData)
					return false;
				memset(m_pFileData, 0 ,m_nFileSize*3/2);
			}
			else
			{
				m_pFileData = new VO_BYTE[m_nFileSize+32];
				if(!m_pFileData)
					return false;
				memset(m_pFileData, 0 ,m_nFileSize+32);
			}
			memcpy(m_pFileData,bt,2);

			voOMXFileRead (hFile, m_pFileData+2, (OMX_U32) m_nFileSize-2);

			m_pCurPos = m_pFileData;

			if(m_pFileData)
			{
				CFileFormatConverter	converter;
				if(m_jniEnv == NULL)
				{
				//	VOLOGI("Subtitle SetSubtitleFile m_jniEnv == NULL");
				}
				//converter.Convert(this->m_jniEnv, m_pFileData, &m_nFileSize);
				if(converter.Convert(this->m_jniEnv, m_pFileData, &m_nFileSize))
					this->m_nFileFormat = SUBTITLE_FILEFORMAT_UTF8;
			}
		}
		voOMXFileClose(hFile);
		return true;
	}
	return true;
}

CBaseSubtitleTrack*		CBaseSubtitleParser::GetTrack(int nTrackIndex)
{
	if (nTrackIndex > m_nTrackCount)
		return NULL;

	return (CBaseSubtitleTrack*)m_TrackList[nTrackIndex];
}


CBaseSubtitleTrack*		CBaseSubtitleParser::CreateSubtitleTrack()
{
	CBaseSubtitleTrack* pReturn = new CBaseSubtitleTrack();
	m_TrackList[m_nTrackCount] = (unsigned int)pReturn;

	m_nTrackCount++;
	return pReturn;
}


CBaseSubtitleTrack*		CBaseSubtitleParser::CreateTextTrack()
{
	CBaseSubtitleTrack* pReturn = new CTextTrack();
	m_TrackList[m_nTrackCount] = (unsigned int)pReturn;

	m_nTrackCount++;
	return pReturn;
}


bool CBaseSubtitleParser::ReadNextLine (char ** ppNextLine, int & nSize)
{
	bool	bFind = false;
	VO_PBYTE	pFind = m_pCurPos;
	while (pFind - m_pFileData < m_nFileSize)
	{
		if ((*pFind == '\r' && *(pFind + 1) == '\n') || *pFind == '\n')
		{
			bFind = true;
			if(m_pCurPos)
				break;
		}
		pFind++;
	}

	if (!bFind)
		return false;

	char * pNewLine = (char *)m_pCurPos;
	nSize = pFind - m_pCurPos;
	if(*pFind == '\r' && *(pFind + 1) == '\n')
	{
		m_pCurPos += (nSize + 2);
	}
	else if(*pFind == '\n')
	{
		m_pCurPos += (nSize + 1);
	}

	while (pNewLine[0] == ' ')
	{
		pNewLine++;
		nSize--;
	}
	while (pNewLine[nSize-1] == ' ')
		nSize--;

	*ppNextLine = (char *)pNewLine;

	return true;
}

bool CBaseSubtitleParser::CheckStr(VO_TCHAR* szText1, VO_CHAR* szText2)
{
	VO_TCHAR szText[1024];
	MByteToWChar(szText2, szText, 1024);
	int nPos = vostrncmp(szText1, szText, 1024);

	return nPos == 0;
}

bool CBaseSubtitleParser::FilterText (VO_CHAR* pText)
{
	if (pText == NULL)
		return true;

	int nTextLen = strlen (pText);
	char * pNBSP = strstr (pText, "&nbsp");
	if(pNBSP==NULL)
		pNBSP = strstr (pText, "&NBSP");

	while (pNBSP != NULL)
	{
		memcpy (pNBSP, pNBSP + 5, nTextLen - (pNBSP - pText) - 4);
		*pNBSP = ' ';
		*(pText + nTextLen - 5) = 0;

		char * pNBSPOld = pNBSP;
		pNBSP = strstr (pNBSP, "&nbsp");
		if(pNBSP==NULL)
			pNBSP = strstr (pNBSPOld, "&NBSP");
		nTextLen = strlen (pText);
	}

	return true;
}

bool CBaseSubtitleParser::SetTag(VO_CHAR* pText)
{
	CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[m_nCurrTrackIndex];
	if (pTrack == NULL)
		return false;

	int nSameTag = pTrack->IsSameTag(pText);
	if (nSameTag > 0)
	{
		if (nSameTag == 2)
		{
			pTrack->SetLanguageString(pText);
			pTrack->SetLanguage(GetLangFromTag(pText));
		}

		return true;
	}
	

	for (int i = 0; i < m_nTrackCount; i++)
	{
		CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[i];
		if (pTrack->IsSameTag(pText) == 1)
		{
			SwitchTrack(i);
			return true;
		}
	}


	CreateTextTrack();
	CBaseSubtitleTrack* pNewTrack = (CBaseSubtitleTrack*)(m_TrackList[m_nTrackCount - 1]);
	pNewTrack->SetTag(pText);
	pNewTrack->SetLanguage(GetLangFromTag(pText));	

	SwitchTrack(m_nTrackCount - 1);	

	return true;
}

Subtitle_Language CBaseSubtitleParser::GetLangFromTag(VO_CHAR* pText)
{
	Subtitle_Language lang = SUBTITLE_LANGUAGE_UNKNOWN;

	lang = FindCharsetInCharsetTable(pText);
	if (lang == SUBTITLE_LANGUAGE_UNKNOWN)
	{
		//	VOLOGI("NOT IN TALBE LIST");
		VO_CHAR ch[16];
		memset(ch,0,16);
		memcpy(ch,pText,2);
		toUpperString(ch);

		if (!memcmp(ch, "EN", 2))
		{
			lang = SUBTITLE_LANGUAGE_ENGLISH;
		}
		else if (!memcmp(ch, "EG", 2))
		{
			lang = SUBTITLE_LANGUAGE_ENGLISH;
		}
		else if (!memcmp(ch, "JP", 2))
		{
			lang = SUBTITLE_LANGUAGE_JAPANESE;
		}
		else if (!memcmp(ch, "KR", 2))
		{
			lang = SUBTITLE_LANGUAGE_KOREAN;
		}	
		else if (!memcmp(ch, "ES" ,2))
		{
			lang = SUBTITLE_LANGUAGE_SPANISH;
		}
		else if (!memcmp(ch, "DE" ,2))
		{
			lang = SUBTITLE_LANGUAGE_GERMAN;
		}
		else if(!memcmp(ch,"FR", 2))
		{
			lang = SUBTITLE_LANGUAGE_FRENCH;
		}
	}

	return lang;
}


void CBaseSubtitleParser::SwitchTrack(int nTrackIndex)
{
	if (nTrackIndex == m_nCurrTrackIndex)
		return;

	CBaseSubtitleTrack* pCurrTrack = (CBaseSubtitleTrack*)m_TrackList[m_nCurrTrackIndex];
	CBaseSubtitleTrack* pDestTrack = (CBaseSubtitleTrack*)m_TrackList[nTrackIndex];	

	CTextItem* pNewItem = pDestTrack->GetNewTextItem();

	pCurrTrack->CloneAndCommitCurrTextItem(pNewItem);

	m_nCurrTrackIndex = nTrackIndex;
	m_pCurrTrack = pDestTrack;
}
