#include "MakeXML_TuDou.h"
#include <string.h>
#include <stdio.h>



CMakeXML_TuDou::CMakeXML_TuDou(void) :
m_pBufXML(NULL)
{
}

CMakeXML_TuDou::~CMakeXML_TuDou(void)
{
	if (m_pBufXML)
		delete []m_pBufXML;
}

VO_U32 CMakeXML_TuDou::parseVideoItem(VO_PCHAR pInfo)
{
	VOLOGF();

	VO_PCHAR pLink, pImg, pTitle, pDetail;
	VO_CHAR szTitle[256] = {0};
	VO_PCHAR pEnd = NULL;

	pTitle = strstr(pInfo, "<title>");
	pImg = strstr(pTitle, "<description>");
	pLink = strstr(pInfo, "<link>");

	pTitle += 7;

	pEnd = strstr(pTitle, "</title>");
	if (pEnd)
		*pEnd = '\0';

	memcpy(szTitle, pTitle, strlen(pTitle));

	SpecialCharacters(szTitle);
	//
	pImg = strstr(pImg, "img src=");
	pImg += 8;
	while (*pImg == ' ' || *pImg == '\"')
		pImg++;

	pEnd = strstr(pImg, "\"");
	if (pEnd)
		*pEnd = '\0';
	//
	pLink += 6;

	pEnd = strstr(pLink, "</link>");
	if (pEnd)
		*pEnd = '\0';

	//----------------------
	//printf("Title: %s\nLink: %s\nThumb: %s\n------------------------------\n", pTitle, pLink, pImg);

	sprintf(m_pBufXML + strlen(m_pBufXML), "<item class=\"video\"><title>%s</title><link>%s</link><description></description><thumbnail>%s</thumbnail></item>", szTitle, pLink, pImg);

	return 0;
}


VO_U32 CMakeXML_TuDou::MakeXML(VO_PCHAR pSource, const VO_PCHAR szPath)
{
	VOLOGF();

	if (m_pBufXML)
	{
		delete []m_pBufXML;
		m_pBufXML = NULL;
	}

	m_pBufXML = new VO_CHAR[Size_BufXML];

	strcpy(m_pBufXML, "<?xml version=\"1.0\" encoding=\"utf-8\"?><rss version=\"2.0\"><channel>");

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

	if (strlen(m_pBufXML) < 80)//
		return 2;

	strcat(m_pBufXML, "</channel></rss>");

	FILE *pxml = fopen(szPath, "w+b");
	if (!pxml)
		return 2;
	else
	{
		fwrite(m_pBufXML, sizeof(VO_CHAR), strlen(m_pBufXML), pxml);

		fclose(pxml);
	}

	delete []m_pBufXML;
	m_pBufXML = NULL;

	return 0;
}
