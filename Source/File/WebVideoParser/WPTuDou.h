#ifndef __CWPTuDou_H__
#define __CWPTuDou_H__

#include "IWebPaser.h"
#include "ICallBack_DownloadData.h"

class CWPTuDou :
	public IWebPaser
{
public:
	CWPTuDou(void);
	~CWPTuDou(void);

	VO_U32 GetVideoURL(const VO_PCHAR szLink, VO_PCHAR *ppDLs, VO_S32 *plSize, VO_PCHAR *ppCookie);

	VO_U32 SetLoginInfo(const VO_PCHAR szWebLink, const VO_PCHAR szUserName, const VO_PCHAR szPassWord) { return 0; };

	VO_VOID SetCallBack(ICallBack_DownloadData *pDLer) { m_pCallBack_DLer = pDLer; };

protected:
	VO_U32 GetVideoID(const VO_PCHAR szLink, VO_PCHAR szVideoID);

	VO_U32 GetPlayLink(const VO_PCHAR szVideoID, VO_PCHAR pDLURL);
	VO_U32 CmpLinkInfo(VO_PCHAR pF, VO_PCHAR *ppLink, VO_S32 *plBRT); 

private:
	ICallBack_DownloadData *m_pCallBack_DLer;
};

#endif //__CWPTuDou_H__