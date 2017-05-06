#ifndef __CWebVideo_H__
#define __CWebVideo_H__

#include "ICallBack_DownloadData.h"
#include "WPManager.h"
#include "vo_http_downloader.h"
#include "voOSFunc.h"


class CWebVideoParser : public ICallBack_DownloadData, public interface_download_callback
{
public:
	CWebVideoParser(void);
	~CWebVideoParser(void);

	VO_U32 GetVideoURL(VO_IN const VO_PCHAR szWebLink, VO_OUT VO_PCHAR *ppDLs, VO_OUT VO_PCHAR *ppCookie);

	VO_U32 SetLoginInfo(const VO_PCHAR szWebLink, const VO_PCHAR szUserName, const VO_PCHAR szPassWord);

protected:
	VO_S64 DownloadWebData(const VO_PCHAR szLink, VO_PCHAR *ppContent, VO_PCHAR pvCookie);

	CALLBACK_RET received_data(VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size);
	CALLBACK_RET download_notify(DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data);

private:
	CWPManager m_WPM;

	vo_http_downloader m_HTTPDLer;
	DOWNLOAD_CALLBACK_NOTIFY_ID m_DLerStatus;

	VO_PCHAR m_pBufSource;
	VO_S64 m_llLenSource;

	VO_PCHAR m_pCookie;
};

#endif
