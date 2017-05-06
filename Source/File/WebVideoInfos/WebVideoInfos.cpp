#include "WebVideoInfos.h"
#include "MakeXML_YouKu.h"
#include "MakeXML_YouTube.h"
#include "MakeXML_TuDou.h"
#include "MakeXML_Sina.h"
#include "voOSFunc.h"

CWebVideoInfos::CWebVideoInfos(VO_PCHAR szDir) :
m_bEixt(VO_FALSE),
m_lFileCount(0),
m_lLenSource(0),
m_pBufSource(NULL),
m_pXMLMaker(NULL),
m_funcCallBack(NULL)
{
	memset(m_szDir, 0, 260);

	strcpy(m_szDir, szDir);

	VO_PCHAR pEnd = &m_szDir[strlen(m_szDir) - 1];
	while ( *pEnd == ' ')
		pEnd--;

	if (*pEnd == '\\')
		*pEnd = '\0';

	m_HTTPDLer.set_download_callback(this);
}

CWebVideoInfos::~CWebVideoInfos(void)
{
	if (m_pXMLMaker)
		delete m_pXMLMaker;

	if (m_pBufSource)
		delete []m_pBufSource;
}

CALLBACK_RET CWebVideoInfos::received_data(VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size)
{
	if (physical_pos + size <= m_lLenSource)
	{
		memcpy(m_pBufSource + physical_pos, ptr_buffer, size);

		return CALLBACK_OK;
	}
	else
	{
		memcpy(m_pBufSource + physical_pos, ptr_buffer, m_lLenSource -  physical_pos);

		return CALLBACK_BUFFER_FULL;
	}
}

CALLBACK_RET CWebVideoInfos::download_notify(DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data)
{
	VOLOGF();

	switch(id)
	{
	case DOWNLOAD_END:
		{
			if (m_bEixt)
				return CALLBACK_OK;

			MakeXML();

			break;
		}
	case DOWNLOAD_START:
			break;
	case DOWNLOAD_FILESIZE:
		{
			//printf("\nDOWNLOAD_FILESIZE: ");
			if (ptr_data)
			{
				memcpy(&m_lLenSource, ptr_data, sizeof(m_lLenSource));

				if (m_lLenSource == -1)
					m_lLenSource = 1024 * 100;

				if (m_pBufSource)
					delete []m_pBufSource;
				
				m_pBufSource =  new VO_CHAR[m_lLenSource];
			}

			break;
		}
	case DOWNLOAD_TO_FILEEND:
		{
			//printf("\nDOWNLOAD_TO_FILEEND: ");
			if (ptr_data)
				memcpy(&m_lLenSource, ptr_data, sizeof(m_lLenSource));

			break;
		}
	case DOWNLOAD_ERROR:
		{
			if (NULL != ptr_data)
				if ( 0 == strcmp( (char *)ptr_data, "!connect" ) )
					break;

			if (m_funcCallBack)
				m_funcCallBack(CALLBACK_ERR, m_szInputURL, NULL);

			break;
		}
	}
	return CALLBACK_OK;
}

VO_U32 CWebVideoInfos::OpenPage(const VO_PCHAR szWebLink)
{
	VOLOGF();

	if ( m_HTTPDLer.is_downloading() )
	{
		if ( 0 == strncmp(m_szInputURL, szWebLink, sizeof(szWebLink)) )
			return VO_ERR_NONE;
		else
		{
			m_bEixt = VO_TRUE;

			m_HTTPDLer.stop_download();
		}
	}

	m_bEixt = VO_FALSE;

	memset(m_szInputURL, 0, 1024);
	strcpy(m_szInputURL, szWebLink);

	if (RedirectURL(szWebLink))
		return VO_ERR_INVALID_ARG;

	VOPDInitParam tmp;
	memset(&tmp, 0, sizeof(VOPDInitParam));
	tmp.nHttpProtocol = 1;

	m_HTTPDLer.set_url(m_szRealURL, &tmp);

	if (!m_HTTPDLer.start_download(0))
		return VO_ERR_FAILED;

	return VO_ERR_NONE;
}

VO_U32 CWebVideoInfos::BecomeHex(VO_PCHAR p)
{
	if (0 <= *p && *p <= 9) *p += 48;    //0,48inascii
	if (10 <= *p && *p <= 15) *p += 97 - 10; //a,97inascii

	return 0;
}

VO_U32 CWebVideoInfos::Char2Hex(VO_PCHAR szDes, VO_CHAR dec)
{
	VO_CHAR c1 = (dec&0xF0)>>4;
	VO_CHAR c2 = (dec&0x0F);

	BecomeHex(&c1);
	BecomeHex(&c2);

	strcat(szDes, "%");
	strncat(szDes, &c1, 1);
	strncat(szDes, &c2, 1);

	return 0;
}

VO_U32 CWebVideoInfos::URLEncode(VO_PCHAR szDes, VO_PCHAR pSource)
{
	while (*pSource != '\0') {
		if ( (48 <= *pSource && *pSource <= 57) ||	//0-9
			(65 <= *pSource && *pSource <= 90) ||	//a-z
			(97 <= *pSource && *pSource <= 122) ||	//A-Z
			(*pSource =='~' || *pSource=='!' || *pSource=='*' || *pSource=='(' || *pSource==')' || *pSource=='\'')
			)
		{
			strncat(szDes, pSource, 1);
		}
		else
		{
			VO_CHAR szHex[4] = {0};

			Char2Hex(szHex, *pSource); //converts char 255 to string "ff"

			strcat(szDes, szHex);
		}

		pSource++;
	}

	VOLOGI("KeyWord %s", szDes);

	return 0;
}


VO_U32 CWebVideoInfos::Search(const VO_PCHAR szKeyWord, VO_WVI_SEARCH id)
{
	VO_CHAR szKeyHex[255] = {0};

	URLEncode(szKeyHex, szKeyWord);

	memset(m_szRealURL, 0, 1024);

	switch (id) 
	{
	case SEARCH_YOUTUBE:
		{
			m_pXMLMaker =  new CMakeXML_YouTube;

			sprintf(m_szRealURL, "http://gdata.youtube.com/feeds/base/videos?q=%s", szKeyHex);
			break;
		}
	case SEARCH_YOUKU:
		{
			m_pXMLMaker =  new CMakeXML_YouKu;

			sprintf(m_szRealURL, "http://www.soku.com/search_video/q_%s", szKeyHex);

			break;
		}
	case SEARCH_TUDOU:
		{
			m_pXMLMaker = new CMakeXML_TuDou;

			sprintf(m_szRealURL, "http://rss.tudou.com/tag/%s", szKeyHex);
			break;
		}
	case SEARCH_SINA:
		{
			m_pXMLMaker = new CMakeXML_Sina;

			sprintf(m_szRealURL, "http://video.sina.com.cn/search/index.php?k=%s", szKeyHex);
			break;
		}
	}

	printf("m_szRealURL %s", m_szRealURL);

	VOPDInitParam tmp;
	memset(&tmp, 0, sizeof(VOPDInitParam));
	tmp.nHttpProtocol = 1;

	m_HTTPDLer.set_url(m_szRealURL, &tmp);

	if (!m_HTTPDLer.start_download(0))
		return VO_ERR_FAILED;

	return VO_ERR_NONE;
}


VO_U32 CWebVideoInfos::Close()
{
	m_bEixt = VO_TRUE;

	if (m_HTTPDLer.is_downloading())
		m_HTTPDLer.stop_download();

	return VO_ERR_NONE;
}

VO_U32 CWebVideoInfos::RedirectURL(const VO_PCHAR szLink)//capitalization? no http? no www?
{
	VOLOGF();

	VO_PCHAR pPath = NULL;
	VO_CHAR szDomain[1024] = {0};

	ParseURL(szLink, szDomain, &pPath);

	if (m_pXMLMaker) {
		delete m_pXMLMaker;
		m_pXMLMaker = NULL;
	}

	memset(m_szRealURL, 0, 1024);

	if (strstr(szDomain,"youku"))
	{
		m_pXMLMaker =  new CMakeXML_YouKu;
		if (NULL == pPath)
			strcpy(m_szRealURL, "http://www.youku.com/index/rss_cool_v/");
		//else if (0 == strcmp(pPath, "1"))
		//	strcpy(m_szRealURL, "http://www.youku.com/index/rss_category_videos/cateid/98");
	}
	else if (strstr(szDomain, "youtube"))
	{
		m_pXMLMaker = new CMakeXML_YouTube();

		if (NULL == pPath)
			strcpy(m_szRealURL, "http://gdata.youtube.com/feeds/base/standardfeeds/most_viewed?client=ytapi-youtube-browse&alt=rss&time=today");
	}
	else if (strstr(szDomain, "tudou"))
	{
		m_pXMLMaker = new CMakeXML_TuDou();

		if (NULL == pPath)
			strcpy(m_szRealURL, "http://rss.tudou.com/feed?type=recommend");
	}
	else if (strstr(szDomain, "sina"))
	{
		m_pXMLMaker = new CMakeXML_Sina();

		if (NULL == pPath)
			strcpy(m_szRealURL, "http://video.sina.com.cn/list/videolist.php");
	}

	if (0 == strlen(m_szRealURL))
		return VO_ERR_INVALID_ARG;

	return VO_ERR_NONE;
}

VO_U32 CWebVideoInfos::ParseURL(const VO_PCHAR szLink, VO_PCHAR svDomain, VO_PCHAR *ppPath)
{
	VOLOGF();

	VO_PCHAR pDomain = strstr(szLink, "http://");

	if (pDomain)
		pDomain += 7;
	else
	{
		pDomain = szLink;

		while(*pDomain == ' ')
			pDomain++;
	}

	*ppPath = strstr(pDomain, "/");

	if (*ppPath)
	{
		strncpy(svDomain, pDomain, *ppPath - pDomain);

		(*ppPath)++;
	}
	else
	{
		strcpy(svDomain, pDomain);

		*ppPath = NULL;
	}
	//-----
	pDomain = svDomain;

	while (*pDomain)
	{
		if ((*pDomain)>='A' && (*pDomain)<='Z')
			*pDomain += 'a' - 'A';

		pDomain++;
	}

	return VO_ERR_NONE;
}

VO_U32 CWebVideoInfos::MakeXML()
{
	VOLOGF();

	VO_CHAR szPath[260] = {0};

#ifdef _WIN32
	if (-1 == sprintf(szPath, "%s\\%d.xml", m_szDir, m_lFileCount))
		return 2;
#elif defined LINUX
	if (-1 == sprintf(szPath, "%s%d.xml", m_szDir, m_lFileCount))
		return 2;
#endif

	m_lFileCount++;
	//-----
	VO_U32 ulRet = m_pXMLMaker->MakeXML(m_pBufSource, szPath);

	if (m_bEixt)
		return 0;

	if (0 != ulRet)
	{
		if (m_funcCallBack)
			m_funcCallBack(CALLBACK_ERR, m_szInputURL, NULL);
	}
	else
	{
		if (m_funcCallBack)
			m_funcCallBack(CALLBACK_FILE, m_szInputURL, szPath);
	}

	return 0;
}