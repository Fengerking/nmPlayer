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
* @file CSuntitleMgr.cpp
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#define LOG_TAG "CSubtitleMgr"


#include "CSubtitleMgr.h"
#include "CSrtParser.h"
#include "CSMIParser.h"
#include "CTimedTextMLParser.h"
#include "CWebVTTParser.h"
#include "voOMXFile.h"
#include "cmnFile.h"
#ifdef _WIN32
#include "CLrcParser.h"
#endif
#include "CSMPTETTMLParser.h"

#include "voLog.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


//#pragma warning (disable : 4996)

CSubtitleMgr::CSubtitleMgr(void)
: m_nTrackCount(0)
, m_jniEnv(NULL)
, n_nTrackIndex(0)
, m_bIsFounded(false)
, m_llBaseTime(0)
{	
	memset(m_TrackList, 0, sizeof(m_TrackList));
	memset(m_strBodyStyle,0,64);
	memset(m_strDivStyle,0,64);
	m_Parser_Type = VO_SUBTITLE_CodingUnused;
}

CSubtitleMgr::~CSubtitleMgr(void)
{
	Clear();
}

bool CSubtitleMgr::IsFileExist(VO_TCHAR* pFile, VO_TCHAR* pExt, bool* pbLangPreFile)
{
	//VOLOGI("IsFileExist %s", pFile);
	*pbLangPreFile = false;
	VO_TCHAR myFilePath[1024];


	vostrcpy(myFilePath, pFile);


	VO_TCHAR* pFileName = vostrrchr(myFilePath, '.');
	if (pFileName == NULL)
		return false;

	vostrcpy(pFileName + 1, pExt);

	OMX_PTR hFile = voOMXFileOpen ((OMX_STRING) myFilePath, VOOMX_FILE_READ_ONLY);
	if (hFile != NULL)
	{
		vostrcpy(pFile, myFilePath);

		voOMXFileClose(hFile);
		return true;
	}

#ifdef _WIN32
	vostrcpy(pFileName +1, L"en.");
#else
	vostrcpy(pFileName +1, "en.");
#endif
	vostrcpy(pFileName +4, pExt);

	//VOLOGI("EN SRT %s", myFilePath);

	
	hFile = voOMXFileOpen ((OMX_STRING) myFilePath, VOOMX_FILE_READ_ONLY);
	if (hFile != NULL)
	{
		//VOLOGI(".EN SET is founded");
		vostrcpy(pFile, myFilePath);

		voOMXFileClose(hFile);
		*pbLangPreFile = true;
		return true;
	}

#ifdef _WIN32
	vostrcpy(pFileName , L"_en.");
#else
	vostrcpy(pFileName , "_en.");
#endif

	vostrcpy(pFileName +4, pExt);

	//VOLOGI("_EN SRT %s", myFilePath);


	hFile = voOMXFileOpen ((OMX_STRING) myFilePath, VOOMX_FILE_READ_ONLY);
	if (hFile != NULL)
	{
		//VOLOGI("_EN SET is founded");
		vostrcpy(pFile, myFilePath);

		voOMXFileClose(hFile);
		*pbLangPreFile = true;
		return true;
	}

	return false;
}
int CSubtitleMgr::SetInputData ( VO_CHAR * pData, int len,int nType, VO_S64 llBaseTime)
{
	m_Parser_Type = (voSubtitleCodingType)nType;
	bool bClear = true;
	VOLOGI("SetInputData,pData size is %d,nType=%d,llBaseTime==%lld",len,nType,llBaseTime);
	VOLOGI("SetInputData,pData is %s",pData);
	if(m_Parser_Type == VO_SUBTITLE_CodingSSTTML)
	{
		VO_CHAR ch[1024];
		memset(ch,0,1024);
		memcpy(ch, pData, len>1012?1012:len);
		VO_CHAR * pCurr = CXmlParserWraper::CheckNodeHead(ch,  "<tt");//strstr(pCurr, "<tt ");
		if(pCurr == NULL)
		{
			pCurr = CXmlParserWraper::CheckNodeHead(ch,  "<p");
			if(pCurr == NULL)
				return Subtitle_ErrorFileNotFound;
			else
				bClear = false;
		}
	}
	if(bClear)
		Clear();

	bool	bFounded = false;
	bool	bIsSRT   = false;
	bool	bLangPreFile = false;

	while(1)
	{
		if (nType == VO_SUBTITLE_CodingSMI)
		{
			bFounded = true;
			break;
		}

		if (nType == VO_SUBTITLE_CodingSRT)
		{
			bFounded = true;
			bIsSRT   = true;
			break;
		}
		if (nType == VO_SUBTITLE_CodingTTML)
		{
			bFounded = true;
			break;
		}
		if (nType == VO_SUBTITLE_CodingWEBVTT)
		{
			bFounded = true;
			break;
		}
		if (nType == VO_SUBTITLE_CodingSSTTML)
		{
			bFounded = true;
			break;
		}
		if (nType == VO_SUBTITLE_CodingSMPTETT)
		{
			bFounded = true;
			break;
		}
		break;
	}	
	m_bIsFounded = bFounded;
	if (bFounded)
	{
		CBaseSubtitleParser* pSubtitleParser = NULL;
		switch(nType)
		{
			case VO_SUBTITLE_CodingSRT:
				pSubtitleParser = (CBaseSubtitleParser*)new CSrtParser();
				break;

			case VO_SUBTITLE_CodingSMI:
				pSubtitleParser = (CBaseSubtitleParser*)new CSMIParser();
				break;
			case VO_SUBTITLE_CodingTTML:
				pSubtitleParser = (CBaseSubtitleParser*)new CTimedTextMLParser();
				if(!bClear)
					pSubtitleParser->SetParseFragmentStreaming(true);
				break;
			case VO_SUBTITLE_CodingSSTTML:
				m_llBaseTime = llBaseTime;
				pSubtitleParser = (CBaseSubtitleParser*)new CTimedTextMLParser();
				pSubtitleParser->SetBaseTime(m_llBaseTime);
				if(!bClear)
					pSubtitleParser->SetParseFragmentStreaming(true);
				break;
			case VO_SUBTITLE_CodingWEBVTT:
				pSubtitleParser = (CBaseSubtitleParser*)new CWebVTTParser();
				break;
			case VO_SUBTITLE_CodingSMPTETT:
				{
					m_llBaseTime = llBaseTime;
					pSubtitleParser = (CBaseSubtitleParser*)new CSMPTETTMLParser();
					pSubtitleParser->SetBaseTime(m_llBaseTime);
				}
				
				break;
			case VO_SUBTITLE_CodingUnused:
				break;

		}
		
		if (pSubtitleParser == NULL)
		{
			return Subtitle_ErrorFileNotFound;
		}
		pSubtitleParser->SetStyleList(&m_styleList);
		pSubtitleParser->SetJniEnv(m_jniEnv);

		if(false == pSubtitleParser->SetSubtitleData(pData, len))
			return Subtitle_ErrorParserFailed;
		GetTracksFromParser(pSubtitleParser);
		if (pSubtitleParser)
		{
			memcpy(m_strBodyStyle,pSubtitleParser->GetBodyStyle(),sizeof(m_strBodyStyle));
			memcpy(m_strDivStyle,pSubtitleParser->GetDivStyle(),sizeof(m_strDivStyle));
			delete pSubtitleParser;
			pSubtitleParser = NULL;
		}
		if (bIsSRT)
		{	
			GetTrack(0)->SetLanguage(SUBTITLE_LANGUAGE_ENGLISH);
		}

		return Subtitle_ErrorNone;
	}
	return Subtitle_ErrorFileNotFound;	
}

int CSubtitleMgr::SetMediaFile(VO_TCHAR* pFilePath)
{
	if (pFilePath == NULL)
	{
		return Subtitle_ErrorFileNotFound;
	}
	Clear();

	VOLOGI("Subtitle SetMediaFile %s", pFilePath);
	VO_TCHAR szPath[2048];
	vostrcpy(szPath, pFilePath);
	bool	bFounded = false;
	bool	bIsSRT   = false;
	bool	bLangPreFile = false;

	while(1)
	{
		if (IsFileExist(szPath, VOUNICODE("smi"), &bLangPreFile))
		{
			VOLOGI("Subtitle smi is founded");
			bFounded = true;
			break;
		}

		if (IsFileExist(szPath, VOUNICODE("srt"), &bLangPreFile))
		{
			VOLOGI("Subtitle srt is founded");
			bFounded = true;
			bIsSRT   = true;
			break;
		}
		if (IsFileExist(szPath, VOUNICODE("xml"), &bLangPreFile))
		{
			bFounded = true;
			break;
		}
		
		if(IsFileExist(szPath,VOUNICODE("vtt"),&bLangPreFile))
		{
			bFounded = true;
			break;
		}
#ifdef _WIN32
		if(IsFileExist(szPath,VOUNICODE("lrc"),&bLangPreFile))
		{
			bFounded = true;
			break;
		}
#endif
		else
		{
			void* hFile = voOMXFileOpen ((OMX_STRING) szPath, VOOMX_FILE_READ_ONLY);
			if (hFile != NULL)
			{
				char* fileBuffer = new char[1024];
				if(!fileBuffer)
					return Subtitle_ErrorInsufficientResources;
				voOMXFileRead(hFile,(OMX_U8*)fileBuffer,1024);
				VOLOGI("format check buffer is %s",fileBuffer);
				if(strstr(fileBuffer,"SAMI"))
				{
					bFounded = true;
				}
				else if(strstr(fileBuffer,"WEBVTT"))
				{
					bFounded = true;
				}
				else if(strstr(fileBuffer,"xml"))
				{
					bFounded = true;
				}
				else
				{
					VOLOGI("subtitle is not founded");
				}
				delete []fileBuffer;
				fileBuffer = NULL;
			}		
		}
		break;
	}	
	m_bIsFounded = bFounded;		//walk around for ios subtitle&CC priority
	VOLOGI("Subtitle founded %d", (bFounded)?1:0);
	if (bFounded)
	{
		AddSubtitleFile(szPath);
		if (bIsSRT && bLangPreFile)
		{	
			GetTrack(0)->SetLanguage(SUBTITLE_LANGUAGE_ENGLISH);
		}
		return Subtitle_ErrorNone;
	}
	VOLOGI("Subtitle finished");
	return Subtitle_ErrorFileNotFound;	
}


bool CSubtitleMgr::AddSubtitleFile(VO_TCHAR* pFilePath)
{
	voSubtitleCodingType nType = CBaseSubtitleParser::GetParserType(pFilePath);
	m_Parser_Type = nType;

	CBaseSubtitleParser* pSubtitleParser = NULL;
	switch(nType)
	{
		case VO_SUBTITLE_CodingSRT:
			pSubtitleParser = (CBaseSubtitleParser*)new CSrtParser();
			{
				VOLOGI("Subtitle VO_SUBTITLE_CodingSRT");
			}
			break;

		case VO_SUBTITLE_CodingSMI:
			pSubtitleParser = (CBaseSubtitleParser*)new CSMIParser();
			{
				VOLOGI("Subtitle VO_SUBTITLE_CodingSMI");
			}
			break;
		case VO_SUBTITLE_CodingTTML:
			pSubtitleParser = (CBaseSubtitleParser*)new CTimedTextMLParser();
			break;
		case VO_SUBTITLE_CodingWEBVTT:
			pSubtitleParser = (CBaseSubtitleParser*)new CWebVTTParser();
			break;
#ifdef _WIN32
		case VO_SUBTITLE_CodingLRC:
			pSubtitleParser = (CBaseSubtitleParser*)new CLrcParser();
			break;
#endif
		case VO_SUBTITLE_CodingUnused:
			break;

	}
	
	if (pSubtitleParser == NULL)
	{
		return false;
	}
	pSubtitleParser->SetStyleList(&m_styleList);
	VOLOGI("Subtitle SetSubtitleFile m_jniEnv =%d", (int)m_jniEnv );
	pSubtitleParser->SetJniEnv(m_jniEnv);

	VOLOGI("Subtitle SetSubtitleFile bbb");
	if(false == pSubtitleParser->SetSubtitleFile(pFilePath))
		return false;
	VOLOGI("Subtitle SetSubtitleFile eee");
	GetTracksFromParser(pSubtitleParser);

	if (pSubtitleParser)
	{
		memcpy(m_strBodyStyle,pSubtitleParser->GetBodyStyle(),sizeof(m_strBodyStyle));
		memcpy(m_strDivStyle,pSubtitleParser->GetDivStyle(),sizeof(m_strDivStyle));
		delete pSubtitleParser;
		pSubtitleParser = NULL;
	}

	return true;
}

CBaseSubtitleTrack* CSubtitleMgr::GetTrack(int nTrackIndex)
{
	if (nTrackIndex > m_nTrackCount)
		return NULL;

	CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[nTrackIndex];
	return pTrack;
}


void	CSubtitleMgr::GetTracksFromParser(CBaseSubtitleParser* pParser)
{
	pParser->Parse();
	int nTrackCount = pParser->GetTrackCount();
	if(pParser->GetParseFragmentStreaming())
	{
		if( m_TrackList[0]!= (unsigned int)NULL && m_nTrackCount>0)
		{
			if(nTrackCount>0 && m_TrackList[0]!= (unsigned int)NULL)
			{
				CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*) pParser->GetTrack(0);
				CBaseSubtitleTrack* pTrackValid = (CBaseSubtitleTrack*)m_TrackList[0];
				pTrackValid->AppendTrackData(pTrack);
			}
			for( int i = 0; i < nTrackCount; i++)
			{
				delete pParser->GetTrack(i);
			}
		}
		else
		{
			for( int i = 0; i < nTrackCount; i++)
			{
				m_TrackList[m_nTrackCount] = (unsigned int) pParser->GetTrack(i);
				m_nTrackCount++;
				pParser->GetTrack(i)->SetFileFormat(pParser->GetFileFormat());
			}
		}
	}
	else
	{
		for( int i = 0; i < nTrackCount; i++)
		{
			m_TrackList[m_nTrackCount] = (unsigned int) pParser->GetTrack(i);
			m_nTrackCount++;
			pParser->GetTrack(i)->SetFileFormat(pParser->GetFileFormat());
		}
	}
}

void	CSubtitleMgr::Clear()
{
	for (int i = 0; i < m_nTrackCount; i++)
	{
		CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[i];
		if (pTrack)
		{
			delete pTrack;
			pTrack = NULL;
		}
	}

	m_nTrackCount = 0;
	TT_Style::DeleteStyles(&m_styleList);
}

VO_CHAR*	CSubtitleMgr::GetCurrSubtitleItem(int nTrackIndex, VO_CHAR* o_szTextLine1, VO_CHAR* o_szTextLine2, VO_CHAR* o_szTextLine3,VO_CHAR*  o_szTextLine4,VO_CHAR* o_szTextLine5,VO_CHAR*  o_szTextLine6, VO_CHAR*  o_szTextLine7,VO_CHAR* o_szTextLine8,VO_CHAR*  o_szTextLine9,  int* o_StartTime, int* o_EndTime, int* o_Count, bool* o_IsEnd)
{
	if (nTrackIndex > m_nTrackCount || m_nTrackCount == 0)
	{
		*o_IsEnd = true;
		return NULL;
	}

	CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[nTrackIndex];
	return pTrack->GetCurrSubtitleItem(o_szTextLine1, o_szTextLine2, o_szTextLine3, o_szTextLine4,o_szTextLine5, o_szTextLine6, o_szTextLine7,o_szTextLine8, o_szTextLine9,o_StartTime, o_EndTime, o_Count, o_IsEnd);
}



VO_CHAR*	CSubtitleMgr::GetSubtitleItem(int nTrackIndex, VO_CHAR* o_szTextLine1, VO_CHAR* o_szTextLine2, VO_CHAR* o_szTextLine3, VO_CHAR* o_szTextLine4,VO_CHAR* o_szTextLine5, VO_CHAR* o_szTextLine6, VO_CHAR* o_szTextLine7,VO_CHAR* o_szTextLine8, VO_CHAR* o_szTextLine9, int* o_StartTime, int* o_EndTime, int* o_Count, bool* o_IsEnd)
{
	if (nTrackIndex > m_nTrackCount || m_nTrackCount == 0)
	{
		*o_IsEnd = true;
		return NULL;
	}
	
	CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[nTrackIndex];
	return pTrack->GetSubtitleItem(o_szTextLine1, o_szTextLine2, o_szTextLine3, o_szTextLine4,o_szTextLine5, o_szTextLine6,o_szTextLine7,o_szTextLine8, o_szTextLine9,o_StartTime, o_EndTime, o_Count, o_IsEnd);
}

Subtitle_Track_Type	CSubtitleMgr::GetTrackType(int nTrackIndex)
{
	if (nTrackIndex > m_nTrackCount || m_nTrackCount == 0)
	{
		return SUBTITLE_TRACK_TYPE_UNKNOWN;
	}
	
	CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[nTrackIndex];
	return pTrack->GetTrackType();
}

bool	CSubtitleMgr::SetPos(int nPos, int nTrackIndex)
{
	if (nTrackIndex == -1)
	{
		for (int i = 0; i < m_nTrackCount; i++)
		{
			CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[i];
			if (pTrack)
				pTrack->SetPos(nPos);
		}
	}
	else
	{
		CBaseSubtitleTrack* pTrack = (CBaseSubtitleTrack*)m_TrackList[nTrackIndex];
		if (pTrack)
			pTrack->SetPos(nPos);
	}
	
	return true;
}
