#ifndef __voWebVideoInfos_H__
#define __voWebVideoInfos_H__
#include "voType.h"

#if defined __cplusplus
extern "C" {
#endif

	enum VO_WVI_CALLBACK {
		CALLBACK_FILE,
		CALLBACK_ERR
	};

	enum VO_WVI_SEARCH {
		SEARCH_YOUKU,
		SEARCH_YOUTUBE,
		SEARCH_TUDOU,
		SEARCH_SINA
		//...
	};

typedef VO_U32 (VO_API * voWVI_CallBackPROC) (VO_S32 callbackID, const VO_PCHAR pWebURL, const VO_PCHAR pData);

typedef struct  
{
	VO_U32 (VO_API *voWVIInit) (VO_HANDLE *phWVI, VO_PCHAR szDir);

	VO_U32 (VO_API *voWVIUninit) (VO_HANDLE hWVI);

	VO_U32 (VO_API *voWVISetCallBack) (VO_HANDLE hWVI, voWVI_CallBackPROC pCallBack);

	VO_U32 (VO_API *voWVIOpenPage) (VO_HANDLE hWVI, VO_PCHAR szWebLink);

	VO_U32 (VO_API *voWVISearch)(VO_HANDLE hWVI, const VO_PCHAR szKeyWork, VO_WVI_SEARCH id);

} VO_WEBVIDEOINFOS_API;


VO_S32 VO_API voGetWebVideoInfosAPI(VO_WEBVIDEOINFOS_API * pHandle, VO_U32 uFlag);


#ifdef __cplusplus
}
#endif

#endif //__voWebVideoInfos_H__