#ifndef __ICallBack_DownloadData_H__
#define __ICallBack_DownloadData_H__

#include "voType.h"

#define Size_WebContent 1024 * 1024

class ICallBack_DownloadData
{
public:
	virtual VO_S64 DownloadWebData(const VO_PCHAR szLink, VO_PCHAR *ppWebSource, VO_PCHAR pvCookie = NULL) = 0;
};

#endif //__ICallBack_DownloadData_H__