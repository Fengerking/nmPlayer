// voPreHTTP.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"

#include "voPreHTTP.h"
#include "PreHTTP.h"
#include "voIndex.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif

VO_U32 voPreHTTPInit(VO_PTR * ppHandle)
{
	CPreHTTP *pPreHTTP = new CPreHTTP();

	if (NULL == pPreHTTP)
		return VO_ERR_OUTOF_MEMORY;

	*ppHandle = pPreHTTP;

	return VO_ERR_NONE;
}

VO_U32 voPreHTTPUninit(VO_PTR pHandle)
{
	if (NULL == pHandle)
		return VO_ERR_INVALID_ARG;

	CPreHTTP *pPreHTTP = (CPreHTTP *)(pHandle);
	pPreHTTP->Close();

	delete pPreHTTP;

	return VO_ERR_NONE;
}

VO_U32 voPreHTTPSetSourceURL(VO_PTR pHandle, VO_CHAR * pSourceURL)
{
	if (NULL == pHandle)
		return VO_ERR_INVALID_ARG;

	CPreHTTP *pPreHTTP = (CPreHTTP *)(pHandle);

	return pPreHTTP->SetSourceURL(pSourceURL);
}

VO_U32 voPreHTTPGetDesInfo(VO_PTR pHandle, VO_LIVESRC_FORMATTYPE *pType,VO_PCHAR * ppDesURL)
{
	if (NULL == pHandle)
		return VO_ERR_INVALID_ARG;

	CPreHTTP *pPreHTTP = (CPreHTTP *)(pHandle);

	return pPreHTTP->GetDesInfo(pType, ppDesURL);
}

VO_S32 VO_API voGetPreHTTPAPI (VO_PREHTTP_API * pPreHTTP, VO_U32 uFlag)
{
	if (NULL == pPreHTTP)
		return VO_ERR_INVALID_ARG;

	pPreHTTP->voPreHTTPInit			= voPreHTTPInit;
	pPreHTTP->voPreHTTPUninit		= voPreHTTPUninit;
	pPreHTTP->voPreHTTPSetSrcURL	= voPreHTTPSetSourceURL;
	pPreHTTP->voPreHTTPGetDesInfo	= voPreHTTPGetDesInfo;

	return VO_ERR_NONE;
}

#ifndef _VONAMESPACE
#if defined __cplusplus
}
#endif
#endif