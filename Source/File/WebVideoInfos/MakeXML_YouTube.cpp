#include "MakeXML_YouTube.h"
#include <string.h>
#include <stdio.h>


CMakeXML_YouTube::CMakeXML_YouTube(void) :
m_pBufXML(NULL)
{
}

CMakeXML_YouTube::~CMakeXML_YouTube(void)
{
	if (m_pBufXML)
		delete []m_pBufXML;
}

VO_U32 CMakeXML_YouTube::parseVideoItem(VO_PCHAR pInfo)
{
	VOLOGF();

	VO_PCHAR pLink, pImg, pTitle, pDetail;
	VO_CHAR szTitle[256] = {0};
	VO_PCHAR pEnd = NULL;

	pTitle = strstr(pInfo, "<title>");
	pImg = strstr(pTitle, "&lt;img ");
	pLink = strstr(pInfo, "<link>");

	pTitle += 7;

	pEnd = strstr(pTitle, "</title>");
	if (pEnd)
		*pEnd = '\0';

	memcpy(szTitle, pTitle, strlen(pTitle));

	SpecialCharacters(szTitle);
	//
	pImg = strstr(pImg, "src=\"");
	pImg += 5;

	pEnd = strstr(pImg, "\"");
	if (pEnd)
		*pEnd = '\0';
	//
	pLink += 6;

	pEnd = strstr(pLink, "&amp");
	if (pEnd)
		*pEnd = '\0';

	//----------------------
	//printf("Title: %s\nLink: %s\nThumb: %s\n------------------------------\n", pTitle, pLink, pImg);

	sprintf(m_pBufXML + strlen(m_pBufXML), "<item><title>%s</title><link>%s</link><description></description><thumbnail>%s</thumbnail></item>", szTitle, pLink, pImg);

	return 0;
}

VO_U32 CMakeXML_YouTube::parseVideoEntry(VO_PCHAR pInfo)
{
	VOLOGF();

	VO_PCHAR pLink, pImg, pTitle, pDetail;
	VO_CHAR szTitle[256] = {0};
	VO_PCHAR pEnd = NULL;

	pTitle = strstr(pInfo, "<title");
	pImg = strstr(pInfo, "&lt;img ");
	pLink = strstr(pInfo, "&lt;a href");

	pTitle = strstr(pTitle, ">");
	pTitle++;

	pEnd = strstr(pTitle, "</title>");
	if (pEnd)
		*pEnd = '\0';

	memcpy(szTitle, pTitle, strlen(pTitle));

	SpecialCharacters(szTitle);
	//
	pImg = strstr(pImg, "src=\"");
	pImg += 5;

	pEnd = strstr(pImg, "\"");
	if (pEnd)
		*pEnd = '\0';
	//
	pLink += 12;
	while(*pLink == ' ' || *pLink == '=' || *pLink == '\"')
		pLink++;

	pEnd = strstr(pLink, "&amp");
	if (pEnd)
		*pEnd = '\0';

	//----------------------
	//printf("Title: %s\nLink: %s\nThumb: %s\n------------------------------\n", pTitle, pLink, pImg);

	sprintf(m_pBufXML + strlen(m_pBufXML), "<item><title>%s</title><link>%s</link><description></description><thumbnail>%s</thumbnail></item>", szTitle, pLink, pImg);
	//m_ulBufLen = m_ulBufLen + strlen(pTitle) + strlen(pLink) + strlen(pImg) + strlen("<item><title>%s</title><link>%s</link><description></description><thumbnail>%s</thumbnail></item>");

	return 0;
}

VO_U32 CMakeXML_YouTube::SplitVideoInfo(VO_PCHAR pSource)
{
	VOLOGF();

	VO_CHAR *ptr0, *ptr1 = pSource;

	while ( (ptr0 = strstr(ptr1, "<item>")) )
	{

		ptr1 = strstr(ptr0, "</item>");
		if (ptr1)
			*ptr1++ = '\0';
		else
			break;

		if (0 != parseVideoItem(ptr0))
			return 2;
	}

	if (!ptr1)
		return 0;

	while ( (ptr0 = strstr(ptr1, "<entry>")) ) //for search
	{

		ptr1 = strstr(ptr0, "</entry>");
		if (ptr1)
			*ptr1++ = '\0';
		else
			break;

		if (0 != parseVideoEntry(ptr0))
			return 2;
	}

	return 0;
}



VO_U32 CMakeXML_YouTube::MakeXML(VO_PCHAR pSource, const VO_PCHAR szPath)
{
	VOLOGF();

	if (m_pBufXML)
	{
		delete []m_pBufXML;
		m_pBufXML = NULL;
	}

	m_pBufXML = new VO_CHAR[Size_BufXML];

	strcpy(m_pBufXML, "<?xml version=\"1.0\" encoding=\"utf-8\"?><rss version=\"2.0\"><channel>");
	//m_ulBufLen+=strlen("<?xml version=\"1.0\" encoding=\"utf-8\"?><rss version=\"2.0\"><channel>");

	if( SplitVideoInfo(pSource) )
		return 2;

	if (strlen(m_pBufXML) < 100)//
		return 2;

	strcat(m_pBufXML, "</channel></rss>");

	FILE *pxml = fopen(szPath, "w+b");
	if (!pxml)
		return 2;

	fwrite(m_pBufXML, sizeof(VO_CHAR), strlen(m_pBufXML), pxml);

	fclose(pxml);

	delete []m_pBufXML;
	m_pBufXML = NULL;

	return 0;
}
