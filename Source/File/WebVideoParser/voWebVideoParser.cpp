// voWebVideo.cpp : Defines the exported functions for the DLL application.
//

#include "voWebVideoParser.h"
#include "WebVideoParser.h"

VO_U32 Init(VO_HANDLE *ppHandle)
{
	CWebVideoParser *pWVP = new CWebVideoParser();
	if (NULL == pWVP)
		return VO_ERR_OUTOF_MEMORY;

	*ppHandle = pWVP;

	return VO_ERR_NONE;
}

VO_U32 Uninit(VO_HANDLE hWV)
{
	if (NULL == hWV)
		return VO_ERR_INVALID_ARG;

	CWebVideoParser *pWV = (CWebVideoParser *)(hWV);

	delete pWV;

	return VO_ERR_NONE;
}

VO_U32 GetVideoURL(VO_HANDLE hWV, const VO_PCHAR szWebLink, VO_PCHAR *ppDLs, VO_PCHAR *ppCookie)
{
	if (NULL == hWV)
		return VO_ERR_INVALID_ARG;

	CWebVideoParser *pWV = (CWebVideoParser *)(hWV);

	return pWV->GetVideoURL(szWebLink, ppDLs, ppCookie);
}

VO_U32 SetLoginInfo(VO_HANDLE hWV, const VO_PCHAR szWebLink, const VO_PCHAR szUserName, const VO_PCHAR szPassWord)
{
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetWVParserAPI (VO_WVPARSER_API * pHandle, VO_U32 uFlag)
{
	if (pHandle == NULL)
		return VO_ERR_INVALID_ARG;

	pHandle->Init			= Init;
	pHandle->Uninit			= Uninit;
	pHandle->GetVideoURL	= GetVideoURL;
	pHandle->SetLoginInfo	= SetLoginInfo;

	return VO_ERR_NONE;
}