#include "WPSina.h"
#include "voIndex.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "voLog.h"

CWPSina::CWPSina(void) :
m_pBufSource(NULL)
{
}

CWPSina::~CWPSina(void)
{
}

VO_U32 CWPSina::GetVideoURL(const VO_PCHAR szLink, VO_PCHAR *ppDLs, VO_S32 *plSize, VO_PCHAR *ppCookie)
{
	VO_U32 ulRet = 0;
	if (NULL == ppDLs)
	{
		VO_CHAR svVideoID[24] = {0};

		ulRet = GetVideoID(szLink, svVideoID);

		if (VO_ERR_NONE == ulRet)
			ulRet = GetListCount(svVideoID, plSize);
		else
			VOLOGE("CWPSina::GetVideoID: %lu", ulRet);
	}
	else
	{
		ulRet = GetPlayList(*ppDLs, *plSize);
	}

	return ulRet;
}

VO_U32 CWPSina::GetVideoID(const VO_PCHAR szLink, VO_PCHAR svVideoID)
{
	VO_PCHAR pBegin = NULL;
	m_pBufSource = NULL;

	if (m_pCallBack_DLer->DownloadWebData(szLink, &m_pBufSource) <= 0)
		return E_NETWORK_ERROR;

	if ( !(pBegin = strstr(m_pBufSource, "vid :")) )
		if ( !(pBegin = strstr(m_pBufSource, "vid:")) )
			return E_INFO_NOT_AVAILABLE;

	pBegin += 5;
	while(*pBegin == ' ' || *pBegin == '\'')
		pBegin++;

	VO_PCHAR pEnd = strchr(pBegin, '\'');
	if (pEnd)
		*pEnd = '\0';

	if (strchr(pBegin, '|'))
	{
		if (1 != sscanf(pBegin, "%*[^|]|%s", svVideoID) )
			return E_INFO_NOT_AVAILABLE;
	}
	else
	{
		if (1 != sscanf(pBegin, "%[0-9]", svVideoID) )
			return E_INFO_NOT_AVAILABLE;
	}

	return VO_ERR_NONE;
}

VO_U32 CWPSina::GetListCount(const VO_PCHAR szVideoID, VO_S32 *plSize)
{
	VO_PCHAR p1 = NULL;
	VO_CHAR szRequestURL[1024] = {0};

	m_pBufSource = NULL;

	sprintf(szRequestURL, "http://v.iask.com/v_play.php?vid=%s", szVideoID);
	if (m_pCallBack_DLer->DownloadWebData(szRequestURL, &m_pBufSource) <= 0)
		return E_NETWORK_ERROR;

	VO_PCHAR p2 = m_pBufSource;

	do 
	{
		p1 = p2;
		p2 = strstr(p1 + 1, "<order>");
	} while (p2);

	p1 += strlen("<order>");

	VO_CHAR szCount[6] = {0};

	sscanf(p1, "%[0-9]", szCount);

	*plSize = atoi(szCount) * 1024;
	
	return VO_ERR_NONE;
}


VO_U32 CWPSina::GetPlayList(VO_PCHAR pDLs, VO_S32 lCount)
{
	VO_PCHAR pURL, pVSTR, pEnd;

	pVSTR = strstr(m_pBufSource, "<vstr>");
	if (pVSTR)
	{
		pVSTR += strlen("<vstr>");

		pEnd = strstr(pVSTR, "</vstr>");
		if (!pEnd)
		{
			return 0;
		}
		else
		{
			*pEnd = '\0';

			if (0 == strncmp(pVSTR, "<![CDATA[", 9))//<![CDATA[	]]>
			{
				pVSTR += 9;

				pEnd --;
				while(*pEnd == ']' || *pEnd == '>')
					*pEnd-- = '\0';
			}
		}
	}
	//---
	pURL = strstr(m_pBufSource, "<url>");
	if(!pURL)
		return E_INFO_NOT_AVAILABLE;

	do
	{
		pURL += strlen("<url>");

		pEnd = strstr(pURL, "</url>");
		if (!pEnd)
		{
			return E_INFO_NOT_AVAILABLE;
		}
		else
		{
			*pEnd = '\0';

			if (0 == strncmp(pURL, "<![CDATA[", 9))//<![CDATA[	]]>
			{
				pURL += 9;

				pEnd -= 3;
				*pEnd = '\0';

				pEnd += 4;
			}

			strcat(pDLs, pURL);
		}

		if (pVSTR)
		{
			strcat(pDLs, "?vstr=");
			strcat(pDLs, pVSTR);
		}

		pURL = strstr(pEnd + 1, "<url>");

		if (pURL)
			if (!strcat(pDLs, "@@@"))
				return E_INFO_NOT_AVAILABLE;
	} while(pURL);

	return VO_ERR_NONE;
}