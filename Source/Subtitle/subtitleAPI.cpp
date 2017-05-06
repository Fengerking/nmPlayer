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
* @file subtitleAPI.cpp
*    entrance of external subtitle lib
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/



#ifdef _WIN32
#  include <windows.h>
#  include <tchar.h>
#endif // _WIN32

#include "voSubtitleParser.h"
#include "CSubtitleHtmlMgr.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define SUBTITLE_CHECK_HANDLE if (hSubtitle == NULL)\
	return Subtitle_ErrorBadParameter;\
CSubtitleHtmlMgr* pSubtitle = (CSubtitleHtmlMgr *)hSubtitle;

VO_PTR				g_hInst = NULL;

//#define LOG_TAG "subtitle3API"

#ifdef _WIN32
#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hInst = (VO_PTR) hModule;
    return TRUE;
}
#endif // _WIN32


SUBTITLE_API int SUBTITLE_APIENTRY voInit  (VO_PTR * phSubtitle,VO_SUBTITLE_PARSER_INIT_INFO* pParam)
{
	//VOLOGF ();

	//VOLOGI("vostInit");
	VOLOGINIT(pParam->strWorkPath)
	CSubtitleHtmlMgr * pSubtitleHtmlMgr = new CSubtitleHtmlMgr ();
	if(!pSubtitleHtmlMgr)
		return Subtitle_ErrorInsufficientResources;
	*phSubtitle = pSubtitleHtmlMgr;
	//VOLOGI("after vostInit");

	return Subtitle_ErrorNone;
}


SUBTITLE_API int SUBTITLE_APIENTRY voUninit (VO_PTR hSubtitle)
{
	//VOLOGF ();

	//VOLOGI(" vostUninit");
	SUBTITLE_CHECK_HANDLE

	delete pSubtitle;
	pSubtitle = NULL;
	VOLOGUNINIT()
	//VOLOGI("after vostUninit");
	return Subtitle_ErrorNone;
}


SUBTITLE_API int SUBTITLE_APIENTRY voSetTimedTextFilePath (VO_PTR hSubtitle, VO_TCHAR * pPath, int nType)
{
	//VOLOGF ();
	//VOLOGI(" vostSetMediaFile");

	SUBTITLE_CHECK_HANDLE
	
	int nReturn =  pSubtitle->SetMediaFile(pPath);
	VOLOGI("Subtitle vostSetMediaFile: nReturn=%d", nReturn);
	return nReturn;
}
SUBTITLE_API int SUBTITLE_APIENTRY voSetInputData (VO_PTR hSubtitle,  VO_CHAR * pData, int len,int nType, VO_S64 llBaseTs = 0)
{
	//VOLOGF ();
	VOLOGI("Subtitle voSetInputData");

	SUBTITLE_CHECK_HANDLE
	
	int nReturn =  pSubtitle->SetInputData(pData, len, nType, llBaseTs);
	VOLOGI("Subtitle after voSetInputData: nReturn=%d", nReturn);

	return nReturn;
}


SUBTITLE_API int SUBTITLE_APIENTRY voGetTrackCount(VO_PTR hSubtitle)
{
	//VOLOGF ();

	SUBTITLE_CHECK_HANDLE
	
	return pSubtitle->GetTrackCount();
}

SUBTITLE_API int SUBTITLE_APIENTRY voSetParameter(VO_PTR hSubtitle, long nID,  void* pValue)
{
	//VOLOGF ();
	//VOLOGI("Subtitle before SetJniEnv");

	SUBTITLE_CHECK_HANDLE

	if(nID == ID_SET_JAVA_ENV)
		pSubtitle->SetJniEnv(pValue);
	//VOLOGI("Subtitle after SetJniEnv");

	return Subtitle_ErrorNone;
}

SUBTITLE_API int SUBTITLE_APIENTRY voGetParameter(VO_PTR hSubtitle, long nID,  void* pValue)
{
	//SUBTITLE_CHECK_HANDLE
	return Subtitle_ErrorNone;
}

SUBTITLE_API int SUBTITLE_APIENTRY voGetSubtitleObject (VO_PTR hSubtitle, long nTimeCurrent, voSubtitleInfo ** subtitleInfo)
{
	//VOLOGF ();

	SUBTITLE_CHECK_HANDLE

	//VOLOGI("vostGetSubtitleObject: nTimeCurrent=%d", nTimeCurrent);
	*subtitleInfo = pSubtitle->GetCurrentText(nTimeCurrent);
	if ((int)(*subtitleInfo) != 0) {
	   //VOLOGI("Subtitle voGetSubtitleObject cur=%d; time=%d",nTimeCurrent,(*subtitleInfo)->nTimeStamp);
	   return Subtitle_ErrorNone;//pSubtitle->SetVideoInfo(nWidth, nHeight, nAngle);
	}
	else
	{
       //VOLOGE("vosetGetSubtitleObject not found , nTimeCurrent=%d",nTimeCurrent);
	   return Subtitle_ErrorParserFailed;
	}

}
	/**
	 * get track info
	 * \param hTtml [in] object from voInit.
	 * \param pTrackInfo [out] track information include language name, which will be defined by lin jin.
	 * \param nIndex [in] track index.
	 * \retval VO_ERR_OK Succeeded.
	 */
SUBTITLE_API int voGetTrackInfo(VO_PTR hSubtitle, VOOSMP_SUBTITLE_LANGUAGE * pTrackInfo, int nIndex)
{
	SUBTITLE_CHECK_HANDLE

	if(pSubtitle->GetTrackCount()>nIndex && nIndex>=0)
		pSubtitle->GetTrackInfo(pTrackInfo,nIndex);
	else
		return Subtitle_ErrorBadParameter;

	return Subtitle_ErrorNone;
}

	/**
	 * get select track by track index
	 * \param hTtml [in] object from voInit.
	 * \param nIndex [in] track index.
	 * \retval VO_ERR_OK Succeeded.
	 */
SUBTITLE_API int voSelectTrackByIndex(VO_PTR hSubtitle, int nIndex)
{
	SUBTITLE_CHECK_HANDLE

	if(pSubtitle->GetTrackCount()>nIndex && nIndex>=0)
		pSubtitle->SelectTrackByIndex(nIndex);
	else
		return Subtitle_ErrorBadParameter;

	return Subtitle_ErrorNone;
}


SUBTITLE_ERRORTYPE voGetSubTitleParserAPI (VO_SUBTITLE_PARSER * pSubtitleFunc, int uFlag)
{
	//VOLOGOGS ();

	pSubtitleFunc->voInit = voInit;
	pSubtitleFunc->voUninit = voUninit;

	pSubtitleFunc->voGetSubtitleInfo = voGetSubtitleObject;

	pSubtitleFunc->voSetTimedTextFilePath = voSetTimedTextFilePath;
	pSubtitleFunc->voSetInputData = voSetInputData;
	pSubtitleFunc->voGetTrackInfo = voGetTrackInfo;
	pSubtitleFunc->voSelectTrackByIndex = voSelectTrackByIndex;

	pSubtitleFunc->voGetTrackCount = voGetTrackCount;
	pSubtitleFunc->voSetParam = voSetParameter;
	pSubtitleFunc->voGetParam = voGetParameter;

	return Subtitle_ErrorNone;
}


