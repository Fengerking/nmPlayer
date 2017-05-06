#include "MakeXML_Sina.h"

CMakeXML_Sina::CMakeXML_Sina(void) :
m_pBufXML(NULL)
{
}

CMakeXML_Sina::~CMakeXML_Sina(void)
{
	if (m_pBufXML)
		delete []m_pBufXML;
}

VO_U32 CMakeXML_Sina::MakeXML(VO_PCHAR pSource, const VO_PCHAR szPath)
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

	while ( (ptr0 = strstr(ptr1, "<li class=\"picbox\">")) )//search
	{
		ptr1 = strstr(ptr0, "</li>");
		if (ptr1)
			*ptr1++ = '\0';
		else
			break;

		if (0 != parseVideoItem(ptr0))
			return 2;
	}

	while ( (ptr0 = strstr(ptr1, "<div class=\"v_Info\">")) )
	{
		ptr0 = strstr(ptr0, "<div class=\"videoPic vp120\"");

		ptr1 = strstr(ptr0, "</div>");
		if (ptr1)
			*ptr1++ = '\0';
		else
			break;

		if (0 != parseClassVideo(ptr0))
			return 2;
	}

	if (strlen(m_pBufXML) < 80)//
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

VO_U32 CMakeXML_Sina::parseVideoItem(VO_PCHAR pInfo)
{
	VOLOGF();

	VO_PCHAR pLink, pImg, pTitle, pDetail;
	VO_CHAR szTitle[256] = {0};
	VO_PCHAR pEnd = NULL;

	pLink = strstr(pInfo, "<div class=\"con\">");
	pImg = strstr(pInfo, "<img src=\"");

	pLink = strstr(pLink, "<a href=\"");
	pTitle = strstr(pLink, "\">");

	pTitle += strlen("\">");

	pEnd = strstr(pTitle, "</a>");
	if (pEnd)
		*pEnd = '\0';

	memcpy(szTitle, pTitle, strlen(pTitle));

	SpecialCharacters(szTitle);
	//
	pImg += strlen("<img src=\"");
	while (*pImg == ' ' || *pImg == '\"')
		pImg++;

	pEnd = strstr(pImg, "\"");
	if (pEnd)
		*pEnd = '\0';
	//
	pLink += strlen("<a href=\"");

	pEnd = strstr(pLink, "\"");
	if (pEnd)
		*pEnd = '\0';
	//----------------------
	//printf("Title: %s\nLink: %s\nThumb: %s\n------------------------------\n", pTitle, pLink, pImg);

	if (strstr(pLink, "v.youku.com/v_show/id_"))
		sprintf(m_pBufXML+strlen(m_pBufXML), "<item class=\"video\">");
	else
		sprintf(m_pBufXML+strlen(m_pBufXML), "<item class=\"page\">");

	sprintf(m_pBufXML + strlen(m_pBufXML), "<title>%s</title><link>%s</link><description></description><thumbnail>%s</thumbnail><duration></duration></item>", szTitle, pLink, pImg);
	return 0;
}


VO_U32 CMakeXML_Sina::parseClassVideo(VO_PCHAR pInfo)
{
	VOLOGF();

	VO_PCHAR pLink, pImg, pTitle, pDetail, pDuration;
	VO_CHAR szTitle[256] = {0};
	VO_CHAR szDuration[64] = {0};
	VO_PCHAR pEnd = NULL;

	pLink = strstr(pInfo, "href=\"");
	pImg = strstr(pInfo, "src=\"");
	pTitle = strstr(pInfo, "title=\"");
	pDuration = strstr(pInfo, "<span class=\"timetxt\">");

	pTitle += strlen("title=\"");

	sscanf(pTitle, "%[^\"]", szTitle);

	SpecialCharacters(szTitle);
	//
	pImg += strlen("src=\"");
	while (*pImg == ' ' || *pImg == '\"')
		pImg++;

	pEnd = strstr(pImg, "\"");
	if (pEnd)
		*pEnd = '\0';
	//
	pLink += strlen("href=\"");

	pEnd = strstr(pLink, "\"");
	if (pEnd)
		*pEnd = '\0';
	//
	if (pDuration)
	{
		pDuration += strlen("<span class=\"timetxt\">");
		sscanf(pDuration, "%[0-9:]",szDuration);
	}
	//----------------------
	//printf("Title: %s\nLink: %s\nThumb: %s\n------------------------------\n", pTitle, pLink, pImg);

	if (strstr(pLink, "v.youku.com/v_show/id_"))
		sprintf(m_pBufXML+strlen(m_pBufXML), "<item class=\"video\">");
	else
		sprintf(m_pBufXML+strlen(m_pBufXML), "<item class=\"page\">");

	sprintf(m_pBufXML + strlen(m_pBufXML), "<title>%s</title><link>%s</link><description></description><thumbnail>%s</thumbnail><duration>%s</duration></item>", szTitle, pLink, pImg, szDuration);
	return 0;
}
