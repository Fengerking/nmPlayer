#include "WPYouTube.h"
#include <string.h>
#include <stdio.h>
#include "voIndex.h"

CWPYouTube::CWPYouTube(void)
{
}

CWPYouTube::~CWPYouTube(void)
{
}

VO_U32 CWPYouTube::GetVideoURL(const VO_PCHAR szLink, VO_PCHAR *ppDLs, VO_S32 *plSize, VO_PCHAR *ppCookie)
{
	if (ppDLs)
	{
		return GetPlayLink(szLink, *ppDLs, ppCookie);
	}
	else
	{
		memset(m_szCookie, 0 , 1024);

		*plSize = 1024;
	}

	return VO_ERR_NONE;
}

VO_CHAR hex2A(const VO_CHAR *p)
{
	unsigned char c[2] = {0};
	for (int i = 0; i < 2; i++)
	{
		if (p[i] >= '0' && p[i] <= '9') c[i] = p[i] - 48;
		else if (p[i] >= 'A' && p[i] <= 'Z')
		{
			c[i] = p[i] - ('A' - 'a');
			c[i] -= 97 - 10;
		}
	}

	return (c[0] << 4) + c[1];
}

VO_VOID URLunEncode(VO_CHAR *pURL)
{
	VO_CHAR *pSrc = pURL;
	VO_CHAR temp[2048] = {0};
	VO_CHAR *pDes = temp;

	while (*pSrc)
	{
		if ('%' == *pSrc)
		{
			*pDes++ = hex2A(pSrc + 1);

			pSrc += 3;
		}
		else
		{
			*pDes++ = *pSrc;
			pSrc++;
		}
	}
	*pDes = 0;

	memset( pURL, 0, sizeof(pURL) );

	strcpy(pURL, temp);
}

VO_U32 CWPYouTube::GetPlayLink(const VO_PCHAR szWebLink, VO_PCHAR pDLURL, VO_PCHAR *ppCookie)
{
	VO_PCHAR pBuf_WebSource = NULL;

	VO_CHAR szVideoID[32] = {0};
	sscanf(szWebLink, "%*[^=]=%[0-9a-zA-Z]", szVideoID);

	const VO_CHAR * el[] = {"&el=embedded", "&el=detailpage", "&el=vevo", ""};
	for (VO_S32 i = 0; i < 4; i++)
	{
		VO_CHAR szVideoInfo[1024] = {0};
		sprintf(szVideoInfo, "http://www.youtube.com/get_video_info?&video_id=%s%s&ps=default&eurl=&gl=US&hl=en", szVideoID, el[i]);

		if (m_pCallBack_DLer->DownloadWebData(szVideoInfo, &pBuf_WebSource, m_szCookie) <= 0)
			return E_NETWORK_ERROR;

		if ( strstr(pBuf_WebSource, "token") )
			break;
	}
	//---TODO
	VO_PCHAR pBegin = strstr(pBuf_WebSource, "url_encoded_fmt_stream_map");
	if (pBegin)
	{
		pBegin = strstr(pBegin, "http%253A");
		if (!pBegin)
			return E_INFO_NOT_AVAILABLE;

		VO_PCHAR pEnd = strstr(pBegin, "%26quality");
		if (pEnd)
			*pEnd = '\0';
		else
			return E_INFO_NOT_AVAILABLE;

		URLunEncode(pBegin);
		URLunEncode(pBegin);

		strcpy(pDLURL, pBegin);
		return 0;
	}
	//---

	return E_INFO_NOT_AVAILABLE;
}

VO_PCHAR CWPYouTube::ChoiceLinkInfo(VO_PCHAR sz_fmt_url_map)
{
	VO_PCHAR pLinkInfo = NULL;
	
	if ( (pLinkInfo = strstr(sz_fmt_url_map, "37|http:\\/\\/")) )
		return pLinkInfo;
	else if ( (pLinkInfo = strstr(sz_fmt_url_map, "22|http:\\/\\/")) )
		return pLinkInfo;
	else if ( (pLinkInfo = strstr(sz_fmt_url_map, "35|http:\\/\\/")) )
		return pLinkInfo;
	else if ( (pLinkInfo = strstr(sz_fmt_url_map, "34|http:\\/\\/")) )
		return pLinkInfo;
	else if ( (pLinkInfo = strstr(sz_fmt_url_map, "18|http:\\/\\/")) )
		return pLinkInfo;
	else if ( (pLinkInfo = strstr(sz_fmt_url_map, "5|http:\\/\\/")) )
		return pLinkInfo;

	return NULL;
}

VO_U32 CWPYouTube::GeneratePlayLink(VO_PCHAR pScript, VO_PCHAR pDLRUL)
{
	//Domain
	VO_PCHAR pDomain = strstr(pScript, "http:\\/\\/");
	if (!pDomain)
		return E_INFO_NOT_AVAILABLE;

	pDomain += 9;

	VO_PCHAR pTmp = strstr(pDomain, "\\/");
	if (!pTmp)
		return E_INFO_NOT_AVAILABLE;

	*pTmp = '\0';
	//Path
	VO_PCHAR pPath = strstr(pTmp + 1, "?");
	if (!pPath)
		return E_INFO_NOT_AVAILABLE;

	pTmp = strchr(pPath, ',');
	if (!pTmp)
		return E_INFO_NOT_AVAILABLE;

	*pTmp = '\0';

	sprintf(pDLRUL, "http://%s/videoplayback%s&", pDomain, pPath);

	return 0;
}
