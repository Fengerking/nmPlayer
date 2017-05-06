#ifndef __CWPSina_H__
#define __CWPSina_H__

#include "IWebPaser.h"
#include "ICallBack_DownloadData.h"

class CWPSina :
	public IWebPaser
{
public:
	CWPSina(void);
	~CWPSina(void);

	VO_U32 GetVideoURL(const VO_PCHAR szLink, VO_PCHAR *ppDLs, VO_S32 *plSize, VO_PCHAR *ppCookie);

	VO_U32 SetLoginInfo(const VO_PCHAR szWebLink, const VO_PCHAR szUserName, const VO_PCHAR szPassWord) { return 0; };

	VO_VOID SetCallBack(ICallBack_DownloadData *pDLer) { m_pCallBack_DLer = pDLer; };

protected:
	VO_U32 GetVideoID(const VO_PCHAR szLink, VO_PCHAR szVideoID);

	VO_U32 GetListCount(const VO_PCHAR szVideoID, VO_S32 *plSize);

	VO_U32 GetPlayList(VO_PCHAR pDLs, VO_S32 lCount);

private:
	ICallBack_DownloadData *m_pCallBack_DLer;

	VO_PCHAR m_pBufSource;
};

#endif //__CWPSina_H__
