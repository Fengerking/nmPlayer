/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		StreamingDownloadStruct.h

	Contains:	StreamingDownloadStruct class file

	Written by:	Aiven

	Change History (most recent first):
	2013-10-30		Aiven			Create file

*******************************************************************************/
#ifndef _SDOWNLOADER_H_
#define _SDOWNLOADER_H_
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define SD_LOG
#ifdef SD_LOG

#ifdef _WIN32

#define SD_LOGI(fmt, ...)	VOLOGI("SDownloader###"fmt, __VA_ARGS__)
#define SD_LOGW(fmt, ...) 	VOLOGW("SDownloader###"fmt, __VA_ARGS__)
#define SD_LOGE(fmt, ...) 	VOLOGE("SDownloader###"fmt, __VA_ARGS__)

#else

#define SD_LOGI(fmt, args...)	VOLOGI("SDownloader###"fmt, ##args)
#define SD_LOGW(fmt, args...) 	VOLOGW("SDownloader###"fmt, ##args)
#define SD_LOGE(fmt, args...) 	VOLOGE("SDownloader###"fmt, ##args)

#endif



#else
#define SD_LOGI 		VOLOGI
#define SD_LOGW 	VOLOGW
#define SD_LOGE 		VOLOGE

#endif

#ifdef _VONAMESPACE
}
#endif

#endif	//_SDOWNLOADER_H_

