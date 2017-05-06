#pragma once
#include "voPreHTTP.h"
#include "interface_download_callback.h"
#include "vo_http_downloader.h"
#include "ASXParser.h"

#if !defined __VO_WCHAR_T__
#define __VO_WCHAR_T__
typedef unsigned short vowchar_t;
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CPreHTTP : public interface_download_callback
{
public:
	CPreHTTP();
	~CPreHTTP(void);

	VO_U32 SetSourceURL(VO_CHAR *);
	VO_U32 GetDesInfo(VO_LIVESRC_FORMATTYPE *pType,VO_PCHAR * ppDesURL);

	VO_VOID Close();

protected:
	VO_BOOL GetFormatByURL(VO_CHAR *pURL);

	VO_BOOL AsyncDownload(VO_CHAR *pURL);

	VO_VOID ConvertContent();

	VO_U32 GetFormatByContent();

	VO_U32 ParseASX();

	CALLBACK_RET received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size );
	CALLBACK_RET download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data );

	int VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen);
	int vowcslen(const vowchar_t* str);
private:
	enum Status {PreHTTP_STOP, PreHTTP_ING, PreHTTP_DONE};

	struct URLs//reference
	{
		URLs(VO_CHAR *sz) : pNextURL(NULL) {
			memset(szURL, 0, 1024);
			memcpy(szURL, sz, strlen(sz) );
		}

		~URLs() {
			if(pNextURL)
				delete pNextURL;
		};

		VO_CHAR szURL[1024];

		URLs *pNextURL;
	};
	
	volatile CPreHTTP::Status m_Status;

	URLs *m_pURLsHead;
	URLs *m_pURLsTail;

	vo_http_downloader m_HTTPDLer;

	VO_LIVESRC_FORMATTYPE m_PreHTTPType;

	VO_S32 m_lLenSource;
	VO_CHAR* m_BufSource;

	CASXParser m_ASXParser;

	VO_BOOL m_bContentReady;
};

#ifdef _VONAMESPACE
}
#endif

