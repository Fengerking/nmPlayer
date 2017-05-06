#include "WPTuDou.h"
#include <string.h>
#include <stdio.h>
#include <voIndex.h>
#include <stdlib.h>

CWPTuDou::CWPTuDou(void)
{
}

CWPTuDou::~CWPTuDou(void)
{
}

VO_U32 CWPTuDou::GetVideoURL(const VO_PCHAR szLink, VO_PCHAR *ppDLs, VO_S32 *plSize, VO_PCHAR *ppCookie)
{
	VO_U32 ulRet = 0;

	if (NULL == ppDLs)
	{
		*plSize = 1024;

		return VO_ERR_NONE;
	}
	else
	{
		VO_CHAR svVideoID[16] = {0};

		ulRet = GetVideoID(szLink, svVideoID);

		if (VO_ERR_NONE == ulRet)
			ulRet = GetPlayLink(svVideoID, *ppDLs);
	}

	return ulRet;
}

VO_U32 CWPTuDou::GetVideoID(const VO_PCHAR szLink, VO_PCHAR svVideoID)
{
	VO_PCHAR pBegin = NULL;
	VO_PCHAR pBuf_WebSource = NULL;
	VO_CHAR szInfoLink[2048] = {0};

	const VO_CHAR * pAppKey = "b754db9385b2f3a4";

	sprintf(szInfoLink, "http://api.tudou.com/v3/gw?method=repaste.info.get&appKey=%s&format=xml&url=%s", pAppKey, szLink);
	if (m_pCallBack_DLer->DownloadWebData(szInfoLink, &pBuf_WebSource) <= 0)
		return E_NETWORK_ERROR;

	pBegin = strstr(pBuf_WebSource, "<itemId>");
	if (!pBegin)
		return E_INFO_NOT_AVAILABLE;

	if (1 != sscanf(pBegin, "<itemId>%[^<]", svVideoID) )
		return E_INFO_NOT_AVAILABLE;

	return VO_ERR_NONE;
}

/*	因为服务器有时会返回gzip压缩数据，我们换用tudou API
	该API绑定AppKey，每分钟研制使用100次
VO_U32 CWPTuDou::GetVideoID(const VO_PCHAR szLink, VO_PCHAR svVideoID)
{
	VO_PCHAR pBegin = NULL;
	VO_PCHAR pBuf_WebSource = NULL;

	if (m_pCallBack_DLer->DownloadWebData(szLink, &pBuf_WebSource) <= 0)
		return E_NETWORK_ERROR;

	if ( (pBegin = strstr(pBuf_WebSource, ",iid =")) )
		pBegin +=  strlen(",iid =");
	else if ( (pBegin = strstr(pBuf_WebSource, ",defaultIid = ")) )
		pBegin += strlen(",defaultIid = ");
	else
		return E_INFO_NOT_AVAILABLE;

	while(*pBegin == ' ' || *pBegin == '=')
		pBegin++;

	if (1 != sscanf(pBegin, "%[0-9]", svVideoID) )
		return E_INFO_NOT_AVAILABLE;

	return VO_ERR_NONE;
}
*/

VO_U32 CWPTuDou::CmpLinkInfo(VO_PCHAR pF, VO_PCHAR *ppLink, VO_S32 *plBRT)
{
	VO_PCHAR pTmp = strstr(pF, ">");
	if (!pTmp)
		return E_INFO_NOT_AVAILABLE;

	*pTmp = '\0';
	//---
	VO_PCHAR pURL = pTmp + 1;
	while (*pURL == ' ')
		pURL++;

	pTmp = pURL + 7;
	pTmp = strchr(pTmp, '/');
	if (!pTmp)
		return E_INFO_NOT_AVAILABLE;

	//if (0 == strncmp(pTmp, "/f4v", 4))
	//{
		VO_PCHAR psBRT = strstr(pF, "brt");
		if (psBRT)
		{
			psBRT += 4;
			while (*psBRT == '\"' || *psBRT == ' ')
				psBRT++;

			pTmp = psBRT;
			while (*pTmp >= '0' && *pTmp <= '9')
				pTmp ++;

			if (pTmp)
				*pTmp = '\0';

			if (*plBRT < atol(psBRT))
			{
				*ppLink = pURL;

				*plBRT = atol(psBRT);
			}
		}
	//}

	return 0;
}

VO_VOID URL_Decode(VO_CHAR *pURL)
{
	VO_CHAR *pSrc = pURL;
	VO_CHAR temp[1024] = {0};
	VO_CHAR *pDes = temp;

	while (*pSrc)
	{
		if ('&'== *pSrc)
		{
			if (memcmp(pSrc, "&amp;", strlen("&amp;")) == 0)
			{
				*pDes++ = '&';
				pSrc += strlen("&amp;");
			} 
			else if (memcmp(pSrc, "&quot;", strlen("&quot;")) == 0)
			{
				*pDes++ = '"';
				pSrc += strlen("&quot;");
			}
			else if (memcmp(pSrc, "&#039;", strlen("&#039;")) == 0)
			{
				*pDes++ = '\'';
				pSrc += strlen("&#039;");
			}
			else if (memcmp(pSrc, "&lt;", strlen("&lt;")) == 0)
			{
				*pDes++ = '<';
				pSrc += strlen("&lt;");
			}
			else if (memcmp(pSrc, "&gt;", strlen("&gt;")) == 0)
			{
				*pDes++ = '>';
				pSrc += strlen("&gt;");
			}
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

VO_U32 CWPTuDou::GetPlayLink(const VO_PCHAR szVideoID, VO_PCHAR pDLURL)
{
	VO_PCHAR pBegin, pLink, pEnd;
	VO_S32 lBRT = 0;
	VO_PCHAR pBuf_WebSource = NULL;
	VO_CHAR szRequestURL[1024] = {0};
	pBegin = pLink = NULL;

	sprintf(szRequestURL, "http://v0.tudou.com/v2/itudou?vn=02&st=-1&id=%s", szVideoID);
	if (m_pCallBack_DLer->DownloadWebData(szRequestURL, &pBuf_WebSource) <= 0)
		return E_NETWORK_ERROR;

	pEnd = pBuf_WebSource;

	while ( (pBegin = strstr(pEnd, "<f")) )
	{
		pEnd = strstr(pBegin, "</f>");
		if (!pEnd)
			return E_INFO_NOT_AVAILABLE;

		*pEnd++ = '\0';

		if (CmpLinkInfo(pBegin, &pLink, &lBRT))
			return E_INFO_NOT_AVAILABLE;
	}

	URL_Decode(pLink);

	if (!pLink || !strcpy(pDLURL, pLink))
		return E_INFO_NOT_AVAILABLE;

	return VO_ERR_NONE;
}
