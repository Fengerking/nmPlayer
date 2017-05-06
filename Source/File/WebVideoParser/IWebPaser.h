#ifndef __IWebPaser_H__
#define __IWebPaser_H__
#include "ICallBack_DownloadData.h"
#include "voStreaming.h"


class IWebPaser
{
public:
	virtual ~IWebPaser() {};

	virtual VO_U32 GetVideoURL(VO_IN const VO_PCHAR szLink, VO_OUT VO_PCHAR *ppDLs, VO_OUT VO_S32 *plSize, VO_OUT VO_PCHAR *ppCookie) = 0;
	virtual VO_U32 SetLoginInfo(VO_IN const VO_PCHAR szWebLink, VO_IN const VO_PCHAR szUserName, VO_IN const VO_PCHAR szPassWord) = 0;

	virtual VO_VOID SetCallBack(ICallBack_DownloadData *pDLer) = 0;
};

#endif //__IWebPaser_H__