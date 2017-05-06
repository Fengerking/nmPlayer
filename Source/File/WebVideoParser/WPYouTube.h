#ifndef __CWPYouTube_H__
#define __CWPYouTube_H__

#include "IWebPaser.h"
#include "ICallBack_DownloadData.h"

class CWPYouTube :
	public IWebPaser
{
public:
	CWPYouTube(void);
	~CWPYouTube(void);

	VO_U32 GetVideoURL(const VO_PCHAR szLink, VO_PCHAR *ppDLs, VO_S32 *plSize, VO_PCHAR *ppCookie);

	VO_U32 SetLoginInfo(const VO_PCHAR szWebLink, const VO_PCHAR szUserName, const VO_PCHAR szPassWord) { return 0; };

	VO_VOID SetCallBack(ICallBack_DownloadData *pDLer) { m_pCallBack_DLer = pDLer; };

protected:
	VO_U32 GetPlayLink(const VO_PCHAR szWebLink, VO_PCHAR pDLURL, VO_PCHAR *ppCookie);

	VO_PCHAR ChoiceLinkInfo(VO_PCHAR sz_fmt_url_map);
	VO_U32 GeneratePlayLink(VO_PCHAR pScript, VO_PCHAR pDLRUL);

private:
	ICallBack_DownloadData *m_pCallBack_DLer;

	VO_CHAR m_szCookie[1024];
};

#endif //__CWPYouTube_H__
