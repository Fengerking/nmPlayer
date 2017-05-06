//#include "StdAfx.h"
#include "PreHTTP.h"
#include "vo_socket.h"
#include "voLog.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define BYTE_1_REP          0x80   
#define BYTE_2_REP          0x800 
#define BYTE_3_REP          0x10000 
#define BYTE_4_REP          0x200000 

CPreHTTP::CPreHTTP()
: m_Status(PreHTTP_STOP)
, m_pURLsHead(NULL)
, m_pURLsTail(NULL)
, m_PreHTTPType(VO_LIVESRC_FT_MAX)
, m_lLenSource(1024 *64)
, m_bContentReady(VO_FALSE)
{
	m_HTTPDLer.set_download_callback(this);

	m_BufSource = new VO_CHAR[1024 *64 + 1];
}

CPreHTTP::~CPreHTTP(void)
{
	delete []m_BufSource;
}

VO_VOID CPreHTTP::Close()
{
	m_PreHTTPType = VO_LIVESRC_FT_MAX;
	m_Status = PreHTTP_STOP;

	if (m_pURLsHead) {
		delete m_pURLsHead;
		m_pURLsHead = m_pURLsTail =NULL;
	}

	m_HTTPDLer.stop_download();
}

VO_U32 CPreHTTP::SetSourceURL(VO_CHAR * pSourceURL)
{
	VO_CHAR szURL[2048] = {0};
	if (sizeof(VO_TCHAR) != 1)
	{
		if( strlen( pSourceURL ) >= sizeof(szURL) )
		{
			VOLOGE(" setin url too long");
			return VO_ERR_FAILED;
		}
		else
		{
			wcstombs(szURL, (wchar_t*)pSourceURL, sizeof(szURL) );
		}
	}
	else
	{
		if( strlen( pSourceURL ) >= sizeof(szURL) )
		{
			VOLOGE(" setin url too long");
			return VO_ERR_FAILED;
		}
		else
		{
			strcpy(szURL, (char*)pSourceURL);
		}
	}

	memset(m_BufSource, 0, 1024 *64 + 1);
	m_lLenSource = 1024 *64;

	if( GetFormatByURL(szURL) )
	{
		return VO_ERR_NONE;
	}

	if( !AsyncDownload( szURL ) )
	{
		return VO_ERR_FAILED;
	}

	return VO_ERR_NONE;
}

VO_BOOL CPreHTTP::AsyncDownload(VO_CHAR *pURL)
{
	VOPDInitParam tmp;
	memset(&tmp, 0, sizeof(VOPDInitParam));

	//m_HTTPDLer.set_workpath(_T("/data/data/com.visualon.osmpDemoPlayer/lib") );
	if ( !m_HTTPDLer.set_url(pURL, &tmp) ) 
	{
		return VO_FALSE;
	}

	m_Status = PreHTTP_ING;
	m_HTTPDLer.stop_download();
	if ( !m_HTTPDLer.start_download_inthread(0) )
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_BOOL CPreHTTP::GetFormatByURL(VO_CHAR *pURL)
{
	VO_S32 iLen = strlen(pURL);
	VO_S32 iBegin = 0, iEnd = iLen;
	while (iBegin < iLen && pURL[iBegin] <= ' ')
		iBegin++;
	while(iEnd > 0 && pURL[iEnd - 1] <= ' ')
		iEnd--;

	VO_CHAR sz[2048] = {0};
	if( iBegin >= iEnd )
	{
		m_Status = PreHTTP_DONE;
		return VO_TRUE;
	}
	else
	{
		strncpy(sz, pURL + iBegin, iEnd - iBegin);
		iLen = strlen(sz);
		if (0 == iLen)
		{
			m_Status = PreHTTP_DONE;
			return VO_TRUE;
		}
	}

	if( strstr(sz , "play.last.fm") || strstr(sz , "0.0.0.0:8081") )
	{
		m_Status = PreHTTP_DONE;
		m_PreHTTPType = VO_LIVESRC_FT_PD;
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_U32 CPreHTTP::GetDesInfo(VO_LIVESRC_FORMATTYPE *pType,VO_PCHAR * ppDesURL)
{
	if( m_bContentReady )
	{
		m_bContentReady = VO_FALSE;
		VO_U32 uRet = GetFormatByContent();

		if (0 == uRet)
		{
			m_Status = PreHTTP_DONE;
		}
		else if (uRet & 0x80000000)
		{
			m_Status = PreHTTP_STOP;
		}
	}

	switch (m_Status)
	{
	case PreHTTP_STOP:
		{
			return VO_ERR_FAILED;
		}
		break;
	case PreHTTP_DONE:
		{
			*pType = m_PreHTTPType;
			VOLOGI("m_PreHTTPType %x", m_PreHTTPType);

			if (VO_LIVESRC_FT_FILE == m_PreHTTPType)
			{
				memset(m_BufSource, 0, 1024 * 64 + 1);

				URLs *pURL = m_pURLsHead;
				while (pURL) 
				{
					VO_CHAR* ptr = strstr(pURL->szURL, "streampreroll.mp3");
					if ( !ptr ) //www.977music.com
					{
						strcat(m_BufSource, pURL->szURL);
						strcat(m_BufSource, "\r\n");
					}

					pURL = pURL->pNextURL;
				}
				*ppDesURL = m_BufSource;
			}
			else
			{
				*ppDesURL = 0;
			}

			return VO_ERR_NONE;
		}
		break;
	case PreHTTP_ING:
	default:
		{
			voOS_Sleep(20);
			return -2;
		}
	}
}

VO_VOID CPreHTTP::ConvertContent()
{
	VO_CHAR *p = NULL;

#if defined _WIN32
	VO_S32 iSize = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_BufSource, -1, NULL, NULL, NULL, NULL );

	p = new VO_CHAR[iSize + 1];
	memset(p , 0 , iSize + 1);

	WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)m_BufSource, -1, p, iSize, NULL, NULL );
#elif defined LINUX || defined _IOS
	VO_S32 iSize = 1024 *64;
	p = new VO_CHAR[iSize + 1];
	memset(p , 0 , iSize + 1);

	int SrcLen = vowcslen((vowchar_t *)m_BufSource);

	VO_UnicodeToUTF8((vowchar_t *)m_BufSource, SrcLen, p, iSize);
#endif

	memset(m_BufSource, 0, 1024 *64 +1);
	memcpy(m_BufSource, p, iSize);

	delete [] p;
}

VO_U32 CPreHTTP::GetFormatByContent()
{
	if ( PreHTTP_DONE == m_Status )
	{
		return 0;
	}

	if (m_BufSource[0] == -1 && m_BufSource[1] == -2)
	{
		ConvertContent();
	}

	VO_CHAR *p = NULL;
	p = m_BufSource;
	while (*p)
	{
		if ((*p)>='A' && (*p)<='Z')
			*p += 'a' - 'A';

		p++;
	}

	p = strstr(m_BufSource, "<asx");
	if (p && p - m_BufSource < 10)
	{
		return ParseASX();
	}

	p = strstr(m_BufSource, "#extm3u");
	if ( p && p - m_BufSource < 10)
	{
		m_PreHTTPType = VO_LIVESRC_FT_HTTP_LS;
	}
	else if ( strstr(m_BufSource, "<smoothstreamingmedia") )
	{
		m_PreHTTPType = VO_LIVESRC_FT_IIS_SS;
	}
	else if ( strstr(m_BufSource, "mswmext=.asf") )
	{
		m_PreHTTPType = VO_LIVESRC_FT_WMS;
	}
	else
	{
		m_PreHTTPType = VO_LIVESRC_FT_PD;
	}

	return 0;
}

int CPreHTTP::vowcslen(const vowchar_t* str) {   
	int len = 0;
	while (*str != '\0') {
		str++;
		len++;
	}
	return len;
} 

int CPreHTTP::VO_UnicodeToUTF8(const vowchar_t *szSrc, const int nSrcLen, char *strDst, const int nDstLen) 
{
	VOLOGR("Enter to VO_UnicodeToUTF8 file size: %d ", nDstLen);

	int is = 0, id = 0;
	const vowchar_t *ps = szSrc;
	unsigned char *pd = (unsigned char*)strDst;

	if (nDstLen <= 0)
		return 0;

	for (is = 0; is < nSrcLen; is++) {
		if (BYTE_1_REP > ps[is]) { /* 1 byte utf8 representation */
			if (id + 1 < nDstLen) {
				pd[id++] = (unsigned char)ps[is];
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_2_REP > ps[is]) {
			if (id + 2 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 6 | 0xc0);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_3_REP > ps[is]) { /* 3 byte utf8 representation */
			if (id + 3 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 12 | 0xe0);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} else if (BYTE_4_REP > ps[is]) { /* 4 byte utf8 representation */
			if (id + 4 < nDstLen) {
				pd[id++] = (unsigned char)(ps[is] >> 18 | 0xf0);
				pd[id++] = (unsigned char)(((ps[is] >> 12)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)(((ps[is] >> 6)  & 0x3f) | 0x80);
				pd[id++] = (unsigned char)((ps[is] & 0x3f) | 0x80);
			} else {
				pd[id] = '\0'; /* Terminate string */
				return 0; /* ERROR_INSUFFICIENT_BUFFER */
			}
		} /* @todo Handle surrogate pairs */
	}

	pd[id] = '\0'; /* Terminate string */

	return id; /* This value is in bytes */
}

VO_U32 CPreHTTP::ParseASX()
{
	REFList * refs = m_ASXParser.Parse(m_BufSource);
	if (NULL == refs)
		return VO_ERR_BASE;

	REFList *pREF = refs;
	VO_S32 iCount = m_ASXParser.getCount();

	pREF = refs;
	for (VO_S32 i = 0; i < iCount; i++)
	{
		if (pREF->nFlag == 0)
		{
			URLs *pURL = new URLs(pREF->szURL);

			if (m_pURLsTail)
			{
				m_pURLsTail->pNextURL = pURL;
				m_pURLsTail = m_pURLsTail->pNextURL;
			}
			else
			{
				m_pURLsHead = m_pURLsTail = pURL;
			}

			pREF = pREF->pNextREF;

			m_PreHTTPType = VO_LIVESRC_FT_FILE;
		}
	}

	pREF = refs;
	for (VO_S32 i = 0; i < iCount; i++)
	{
		if (pREF->nFlag == 1)
		{
			if( !AsyncDownload( pREF->szURL ) )
			{
				break;
			}

			return 1;
		}
	}

	return VO_ERR_NONE;
}

CALLBACK_RET CPreHTTP::received_data(VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size)
{
	if (physical_pos + size <= m_lLenSource)
	{
		memcpy(m_BufSource + physical_pos, ptr_buffer, size);

		return CALLBACK_OK;
	}
	else
	{
		memcpy( m_BufSource + physical_pos, ptr_buffer, m_lLenSource -  static_cast<VO_S32>(physical_pos) );

		return CALLBACK_BUFFER_FULL;
	}
}

CALLBACK_RET CPreHTTP::download_notify(DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data)
{
	if (DOWNLOAD_LIVESRC_TYPE == id)
	{
		m_PreHTTPType = *(VO_LIVESRC_FORMATTYPE *)ptr_data;
		m_Status = PreHTTP_DONE;
		return CALLBACK_OK;
	}

	if (PreHTTP_ING != m_Status)
	{
		return CALLBACK_OK;
	}

	switch(id)
	{
	case DOWNLOAD_LIVESRC_TYPE:
		{
			;
		}
		break;

	case DOWNLOAD_END:
		{
			VOLOGI("DOWNLOAD_END");
			m_bContentReady = VO_TRUE;
		}
		break;

	case DOWNLOAD_START:
		{
			VOLOGR("DOWNLOAD_START");
			m_bContentReady = VO_FALSE;
		}
		break;

	case DOWNLOAD_FILESIZE:
		{
			;
		}
		break;

	case DOWNLOAD_TO_FILEEND:
		{
			VOLOGI("DOWNLOAD_TO_FILEEND");

			if (ptr_data)
			{
				memcpy(&m_lLenSource, ptr_data, sizeof(m_lLenSource));
			}
		}
		break;

	case DOWNLOAD_ERROR:
	case DOWNLOAD_SERVER_RESPONSEERROR:
		{
			VOLOGE("DOWNLOAD_ERROR");
			m_Status = PreHTTP_STOP;
		}
		break;
	}

	return CALLBACK_OK;
}
