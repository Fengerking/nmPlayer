#include "WPManager.h"
#include <string.h>
#include "voIndex.h"


CWPManager::CWPManager(void) :
m_lCount(0),
m_pDLURLs(NULL)
{
}

CWPManager::~CWPManager(void)
{
	if (m_pDLURLs)
		delete []m_pDLURLs;
}

VO_U32 CWPManager::GetVideoURL(const VO_PCHAR szLink, VO_PCHAR *ppDLs, VO_S32 *plSize, VO_PCHAR *ppCookie)
{
	VO_U32 ulRet = 0;
	VO_S32 lSize = 0;

	if (m_pDLURLs) {
		delete []m_pDLURLs;
		m_pDLURLs = NULL;

		m_lCount = 0;
	}

	IWebPaser *pWP = SelectPaser(szLink);
	if (!pWP)
		return VO_ERR_INVALID_ARG;
	//
	ulRet = pWP->GetVideoURL(szLink, NULL, &lSize, ppCookie);

	if (VO_ERR_NONE == ulRet)
	{
		m_pDLURLs = new VO_CHAR[lSize];
		memset(m_pDLURLs, 0, lSize);

		ulRet = pWP->GetVideoURL(szLink, &m_pDLURLs, &lSize, ppCookie);
		if (ulRet)
			return ulRet;

		*ppDLs = m_pDLURLs;
	}

	return ulRet;
}

VO_U32 CWPManager::SetLoginInfo(const VO_PCHAR szWebLink, const VO_PCHAR szUserName, const VO_PCHAR szPassWord)
{
	IWebPaser *pWP = SelectPaser(szWebLink);
	if (!pWP)
		return VO_ERR_INVALID_ARG;

	return pWP->SetLoginInfo(szWebLink, szUserName, szPassWord);
}

VO_VOID CWPManager::SetCallBack(ICallBack_DownloadData *pDLer)
{
	m_YouKuPaser.SetCallBack(pDLer);
	m_YouTubePaser.SetCallBack(pDLer);
	m_TuDouPaser.SetCallBack(pDLer);
	m_SinaPaser.SetCallBack(pDLer);
}

IWebPaser * CWPManager::SelectPaser(const VO_PCHAR szLink)
{
	VO_CHAR szDomain[1024] = {0};

	VO_PCHAR pDomain = strstr(szLink, "http://");

	if (pDomain)
		pDomain += 7;
	else
	{
		pDomain = szLink;

		while(*pDomain == ' ')
			pDomain++;
	}

	VO_PCHAR pEnd = strstr(pDomain, "/");

	if (pEnd)
		strncpy(szDomain, pDomain, pEnd - pDomain);
	else
		strcpy(szDomain, pDomain);
	//-----
	pDomain = szDomain;

	while (*pDomain)
	{
		if ((*pDomain)>='A' && (*pDomain)<='Z')
			*pDomain += 'a' - 'A';

		pDomain++;
	}

	if (strstr(szDomain,"youku"))
		return &m_YouKuPaser;
	else if (strstr(szDomain, "youtube"))
		return &m_YouTubePaser;
	else if (strstr(szDomain, "tudou"))
		return &m_TuDouPaser;
	else if (strstr(szDomain, "sina"))
		return &m_SinaPaser;

	return NULL;
}
