#include "WPYouKu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voIndex.h"
#include "voLog.h"


CWPYouKu::CWPYouKu(void) :
m_pCallBack_DLer(NULL)
{
	memset(m_szPreURL, 0, 1024);
}

CWPYouKu::~CWPYouKu(void)
{
	if (m_pCallBack_DLer)
		delete m_pCallBack_DLer;
}

VO_U32 CWPYouKu::GetVideoURL(const VO_PCHAR szLink, VO_PCHAR *ppDLs, VO_S32 *plSize, VO_PCHAR *ppCookie)
{
	VO_U32 ulRet = 0;
	if (NULL == ppDLs)
	{
		VO_CHAR svVideoID[24] = {0};

		ulRet = GetVideoID(szLink, svVideoID);

		if (VO_ERR_NONE == ulRet)
			ulRet = GetFirstVideoURL(svVideoID, plSize);
		else
			VOLOGE("CWPYouKu::GetVideoID: %lu", ulRet);
	}
	else
	{
		ulRet = GetPlayList(*ppDLs, *plSize);
	}

	return ulRet;
}

VO_U32 CWPYouKu::GetVideoID(const VO_PCHAR szLink, VO_PCHAR svVideoID)
{
	VO_PCHAR pBegin = NULL;
	VO_PCHAR pBuf_WebSource = NULL;

	if (m_pCallBack_DLer->DownloadWebData(szLink, &pBuf_WebSource) <= 0)
		return E_NETWORK_ERROR;

	if ( !(pBegin = strstr(pBuf_WebSource, "var videoId")) )
		return E_INFO_NOT_AVAILABLE;

	pBegin += 13;
	while(*pBegin == ' ' || *pBegin == '\''|| *pBegin == '\"')
		pBegin++;

	if (1 != sscanf(pBegin, "%[0-9]", svVideoID))
		return E_INFO_NOT_AVAILABLE;

	//VOLOGI("svVideoID: %s", svVideoID);

	return VO_ERR_NONE;
}

VO_U32 CWPYouKu::GetFirstVideoURL(const VO_PCHAR szVideoID, VO_S32 *plSize)
{
	VO_PCHAR pfileids, pseed, pkey1, pkey2, pseconds, pCount;
	VO_PCHAR pErr = NULL;
	VO_PCHAR pBuf_WebSource = NULL;
	VO_CHAR szRequestURL[1024] = {0};

	pseed = pkey1 = pkey2 = pseconds = pfileids = pCount = NULL;

	sprintf(szRequestURL, "http://v.youku.com/player/getPlayList/VideoIDS/%s/version/5/source/video/password/undefined", szVideoID);
	if (m_pCallBack_DLer->DownloadWebData(szRequestURL, &pBuf_WebSource) <= 0)
		return E_NETWORK_ERROR;

	if (0 == CheckErr(pBuf_WebSource, &pErr))
		return E_INFO_NOT_AVAILABLE;
	
	if (FindSixInfos(pBuf_WebSource, &pfileids, &pseed, &pkey1, &pkey2, &pseconds, &pCount))
		return E_INFO_NOT_AVAILABLE;

	VO_CHAR svkey[200] = {0};
	if (getKey(pkey1, pkey2, svkey, 200))
		return E_INFO_NOT_AVAILABLE;

	VO_CHAR svRealId[72] = {0};
	getFileId(pfileids, pseed, svRealId);

	memset(m_szPreURL, 0, 1024);
	sprintf(m_szPreURL, "http://f.youku.com/player/getFlvPath/fileid/%s?K=%s", svRealId, svkey);
	
	*plSize = atol(pCount) * (strlen(m_szPreURL) + 4);

	return VO_ERR_NONE;
}

VO_U32 CWPYouKu::GetPlayList(VO_PCHAR pDLs, VO_S32 lSize)
{
	memset(pDLs, 0, lSize);
	if (!strcpy(pDLs, m_szPreURL))
		return E_INFO_NOT_AVAILABLE;

	VO_PCHAR pfileid = strstr(m_szPreURL, "fileid");
	if (!pfileid)
		return E_INFO_NOT_AVAILABLE;

	pfileid += 15;

	VO_S32 lCount = lSize/(strlen(m_szPreURL) + 4);

	for (VO_S32 iTmp = 1; iTmp < lCount; iTmp++)
	{
		VO_CHAR szSeg[4] = {0};
		sprintf(szSeg, "%X", (unsigned)iTmp);

		if( strlen(szSeg) <= 1 )
			pfileid[1] = szSeg[0];
		else
		{
			pfileid[0] = szSeg[0];
			pfileid[1] = szSeg[1];
		}

		if (!strcat(pDLs, "@@@"))
			return E_INFO_NOT_AVAILABLE;

		if (!strcat(pDLs, m_szPreURL))
			return E_INFO_NOT_AVAILABLE;
	}

	return VO_ERR_NONE;
}

VO_U32 CWPYouKu::CheckErr(VO_PCHAR pContent, VO_PCHAR *ppErr)
{
	*ppErr = strstr(pContent, "error");
	if (!*ppErr)
		return E_INFO_NOT_AVAILABLE;

	//...

	return VO_ERR_NONE;
}

VO_U32 CWPYouKu::FindSixInfos(VO_PCHAR pContent, VO_PCHAR *ppfileids, VO_PCHAR *ppseed, VO_PCHAR *ppkey1, VO_PCHAR *ppkey2, VO_PCHAR *ppseconds, VO_PCHAR *ppCount)
{
	VO_PCHAR pEnd = NULL;

	*ppfileids = strstr(pContent, "streamfileids");
	*ppseed = strstr(pContent, "seed");
	*ppkey1 = strstr(pContent, "key1");
	*ppkey2 = strstr(pContent, "key2");
	*ppseconds = strstr(pContent, "seconds");
	*ppCount = strstr(pContent, "segs");

	if (!*ppfileids || !*ppseed || !*ppkey1 || !*ppkey2 || !*ppCount)
		return E_INFO_NOT_AVAILABLE;
	//seed
	*ppseed += 6;
	while(**ppseed == ' ' || **ppseed == '\"')
		(*ppseed)++;

	pEnd = *ppseed;
	while ((*pEnd >= '0' && *pEnd <= '9'))
		pEnd++;

	*pEnd = '\0';
	//key1
	*ppkey1 += 6;
	while(**ppkey1 == ' ' || **ppkey1 == '\"')
		(*ppkey1)++;

	pEnd = *ppkey1;
	while ( (*pEnd >= '0' && *pEnd <= '9') || (*pEnd >= 'a' && *pEnd <= 'z') )
		pEnd++;

	*pEnd = '\0';
	//key2
	*ppkey2 += 6;
	while(**ppkey2 == ' ' || **ppkey2 == '\"')
		(*ppkey2)++;

	pEnd = *ppkey2;
	while ( (*pEnd >= '0' && *pEnd <= '9') || (*pEnd >= 'a' && *pEnd <= 'z') )
		pEnd++;

	*pEnd = '\0';
	//seconds
	*ppseconds += 9;
	while (**ppseconds == ' ' || **ppseconds == '\"')
		(*ppseconds)++;

	pEnd = *ppseconds;
	while (*pEnd >= '0' && *pEnd <= '9')
		pEnd++;

	*pEnd = '\0';
	//streamfileids
	if ( !(*ppfileids = strstr(*ppfileids, "flv")) )
		return E_INFO_NOT_AVAILABLE;

	*ppfileids += 5;
	while(**ppfileids == ' ' || **ppfileids == '\"')
		(*ppfileids)++;

	pEnd = *ppfileids;
	pEnd = strstr(pEnd, "\"");
	if (!pEnd)
		return E_INFO_NOT_AVAILABLE;
	
	*pEnd = '\0';
	//segs > flv > no
	if ( !(*ppCount = strstr(*ppCount, "flv")) )
		return E_INFO_NOT_AVAILABLE;

	if ( !(pEnd = strstr(*ppCount, "]")) )
		return E_INFO_NOT_AVAILABLE;

	*pEnd = '\0';

	if (!(*ppCount = strrchr(*ppCount, '{')))
		return E_INFO_NOT_AVAILABLE;
	if (!(*ppCount = strstr(*ppCount, "no")))
		return E_INFO_NOT_AVAILABLE;
	*ppCount += 5;
	while (**ppCount == ' ' || **ppCount == '\"')
		(*ppCount)++;

	pEnd = *ppCount;
	while (*pEnd >= '0' && *pEnd <= '9')
		pEnd++;

	*pEnd = '\0';

	return VO_ERR_NONE;
}

VO_U32 CWPYouKu::getFileId(VO_PCHAR pfileids, VO_PCHAR pseed, VO_PCHAR pRealId)
{
	VO_CHAR source[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ/\\:._-1234567890";
	VO_S32 index = 0;
	VO_S32 lLen = strlen(source);
	VO_S32 seed = atol(pseed);

	VO_CHAR mixed[72] = {0};

	for (VO_S32 iTmp = 0; iTmp < lLen; ++iTmp)  
	{ 
		seed = (seed * 211 + 30031) % 65536;

		index = static_cast<VO_S32>((double)seed / 65536 * strlen(source));

		VO_PCHAR pch = &source[index];

		mixed[strlen(mixed)] = pch[0];

		pch[0] = '\0';
		strcat(source, pch + 1);
	} 
	//----------
	VO_S32 arrayfileids[66], lLen_array = 0;
	VO_PCHAR pBegin = pfileids;
	VO_PCHAR pEnd = pfileids;

	do 
	{
		while (*pEnd != '*')
			pEnd++;

		*pEnd = '\0';

		arrayfileids[lLen_array] = atol(pBegin);
		lLen_array++;

		pEnd++;
		pBegin = pEnd;
	} while (lLen_array != 66);
	//----------
	for (VO_S32 iTmp = 0; iTmp < 66; ++iTmp)
		pRealId[strlen(pRealId)] = mixed[ arrayfileids[iTmp] ];

	pRealId[strlen(pRealId)+1] = '\0';

	return 0;
}

VO_U32 CWPYouKu::getKey(VO_PCHAR pkey1, VO_PCHAR pkey2, VO_PCHAR pvkey, VO_S32 lBufSize)
{
	unsigned appendkey ;

	sscanf(pkey1, "%x", &appendkey);

	appendkey ^= 0xA55AA5A5;

	VO_CHAR szHex[16] = {0};
	sprintf(szHex, "%x", appendkey);

	if( strlen(szHex) + strlen(pkey2) > static_cast<unsigned>(lBufSize - 1) )
		return E_INFO_NOT_AVAILABLE;

	strcpy(pvkey, pkey2);
	strcat(pvkey, szHex);

	return 0;
}



