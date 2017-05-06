#ifndef __CWV_YouKu_H__
#define __CWV_YouKu_H__

#include "IWebPaser.h"
#include "ICallBack_DownloadData.h"

class CWPYouKu :
	public IWebPaser
{
public:
	CWPYouKu(void);
	~CWPYouKu(void);

	VO_U32 GetVideoURL(const VO_PCHAR szLink, VO_PCHAR *ppDLs, VO_S32 *plSize, VO_PCHAR *ppCookie);

	VO_U32 SetLoginInfo(const VO_PCHAR szWebLink, const VO_PCHAR szUserName, const VO_PCHAR szPassWord) { return 0; };

	VO_VOID SetCallBack(ICallBack_DownloadData *pDLer) { m_pCallBack_DLer = pDLer; };

protected:
	VO_U32 GetVideoID(const VO_PCHAR szLink, VO_PCHAR szVideoID);
	VO_U32 GetFirstVideoURL(const VO_PCHAR szVideoID, VO_S32 *plSize);
	VO_U32 GetPlayList(VO_PCHAR pDLs, VO_S32 lCount);

	VO_U32 CheckErr(VO_PCHAR pContent, VO_PCHAR *ppErr);
	VO_U32 FindSixInfos(VO_PCHAR pContent, VO_PCHAR *ppfileids, VO_PCHAR *ppseed, VO_PCHAR *ppkey1, VO_PCHAR *ppkey2, VO_PCHAR *ppseconds, VO_PCHAR *ppCount);

	VO_U32 getFileId(VO_PCHAR pfileids, VO_PCHAR pseed, VO_PCHAR pRealId);
	VO_U32 getKey(VO_PCHAR pkey1, VO_PCHAR pkey2, VO_PCHAR pvkey, VO_S32 lBufSize);

private:
	ICallBack_DownloadData *m_pCallBack_DLer;

	VO_CHAR m_szPreURL[1024];
};

#endif //__CWV_YouKu_H__
