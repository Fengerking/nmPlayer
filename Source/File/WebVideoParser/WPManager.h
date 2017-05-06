#ifndef __CWPManager_H__
#define __CWPManager_H__

#include "ICallBack_DownloadData.h"
#include "IWebPaser.h"
#include "WPYouKu.h"
#include "WPYouTube.h"
#include "WPTuDou.h"
#include "WPSina.h"


class CWPManager : public IWebPaser
{
public:
	CWPManager(void);
	~CWPManager(void);

	VO_U32 GetVideoURL(const VO_PCHAR szLink, VO_PCHAR *ppDLs, VO_S32 *plSize, VO_PCHAR *ppCookie);

	VO_U32 SetLoginInfo(const VO_PCHAR szWebLink, const VO_PCHAR szUserName, const VO_PCHAR szPassWord);

	VO_VOID SetCallBack(ICallBack_DownloadData *pDLer);

private:
	IWebPaser *SelectPaser(const VO_PCHAR szLink);

private:
	VO_S32 m_lCount;
	VO_PCHAR m_pDLURLs;

	CWPYouKu m_YouKuPaser;
	CWPYouTube m_YouTubePaser;
	CWPTuDou m_TuDouPaser;
	CWPSina m_SinaPaser;
};

#endif //__CWVManager_H__
