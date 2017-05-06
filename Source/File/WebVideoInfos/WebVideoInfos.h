#ifndef __CWebVideoInfos_H__
#define __CWebVideoInfos_H__
#include "voWebVideoInfos.h"
#include "MakeXML_YouKu.h"
#include "MakeXML_TuDou.h"
#include "MakeXML_YouTube.h"
#include "interface_download_callback.h"
#include "vo_http_downloader.h"


class CWebVideoInfos : public interface_download_callback
{
public:
	CWebVideoInfos(VO_PCHAR);
	~CWebVideoInfos(void);

	VO_U32 OpenPage(const VO_PCHAR szWebLink);

	VO_U32 Search(const VO_PCHAR szKeyWork, VO_WVI_SEARCH id);

	VO_U32 Close();

	VO_VOID SetCallBack(voWVI_CallBackPROC pCallBack) { m_funcCallBack = pCallBack; }

protected:
	CALLBACK_RET received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size );
	CALLBACK_RET download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data );

	VO_U32 RedirectURL(const VO_PCHAR szLink);
	VO_U32 ParseURL(const VO_PCHAR szLink, VO_PCHAR svDomain, VO_PCHAR *ppPath);

	VO_U32 MakeXML();

	VO_U32 URLEncode(VO_PCHAR szDes, VO_PCHAR pSource);
	VO_U32 Char2Hex(VO_PCHAR szDes, VO_CHAR dec);
	VO_U32 BecomeHex(VO_PCHAR p);

private:
	VO_BOOL m_bEixt;

	VO_CHAR m_szDir[260];

	VO_CHAR m_szInputURL[1024];
	VO_CHAR m_szRealURL[1024];

	VO_S32 m_lFileCount;//name
	
	VO_S32 m_lLenSource;
	VO_PCHAR m_pBufSource;
	
	voWVI_CallBackPROC m_funcCallBack;

	CMakeXML *m_pXMLMaker;

	vo_http_downloader m_HTTPDLer;
};


#endif //__CWebVideoInfos_H__