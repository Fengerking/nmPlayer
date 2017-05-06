// voWebVideoInfos.cpp : Defines the exported functions for the DLL application.
//

#include "voWebVideoInfos.h"
#include "WebVideoInfos.h"

VO_U32 Init(VO_HANDLE *phWVI, VO_PCHAR szDir)
{
	if (NULL == szDir)
		return VO_ERR_INVALID_ARG;

	CWebVideoInfos *pWVI = new CWebVideoInfos(szDir);
	if (NULL == pWVI)
		return VO_ERR_OUTOF_MEMORY;

	*phWVI = pWVI;

	return VO_ERR_NONE;
}

VO_U32 Uninit(VO_HANDLE hWVI)
{
	if (NULL == hWVI)
		return VO_ERR_INVALID_ARG;

	CWebVideoInfos *pWVI = (CWebVideoInfos *)(hWVI);
	pWVI->Close();

	delete hWVI;

	return VO_ERR_NONE;
}

VO_U32 SetCallBack(VO_HANDLE hWVI, voWVI_CallBackPROC pCallBack)
{
	if (NULL == hWVI || NULL == pCallBack)
		return VO_ERR_INVALID_ARG;

	CWebVideoInfos *pWVI = (CWebVideoInfos *)(hWVI);

	pWVI->SetCallBack(pCallBack);

	return VO_ERR_NONE;
}

VO_U32 OpenPage(VO_HANDLE hWVI, VO_PCHAR szWebLink)
{
	if (NULL == hWVI)
		return VO_ERR_INVALID_ARG;

	CWebVideoInfos *pWVI = (CWebVideoInfos *)(hWVI);

	return pWVI->OpenPage(szWebLink);
}

VO_U32 Search(VO_HANDLE hWVI, const VO_PCHAR szKeyWork, VO_WVI_SEARCH id)
{
	if (NULL == hWVI)
		return VO_ERR_INVALID_ARG;

	CWebVideoInfos *pWVI = (CWebVideoInfos *)(hWVI);

	return pWVI->Search(szKeyWork, id);
}

VO_S32 VO_API voGetWebVideoInfosAPI(VO_WEBVIDEOINFOS_API * pHandle, VO_U32 uFlag)
{
	if (NULL == pHandle)
		return VO_ERR_INVALID_ARG;

	pHandle->voWVIInit			= Init;
	pHandle->voWVIUninit		= Uninit;
	pHandle->voWVISetCallBack	= SetCallBack;
	pHandle->voWVIOpenPage		= OpenPage;
	pHandle->voWVISearch		= Search;

	return VO_ERR_NONE;
}


