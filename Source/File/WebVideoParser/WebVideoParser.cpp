#include "WebVideoParser.h"
#include "voLog.h"


CWebVideoParser::CWebVideoParser(void) :
m_DLerStatus(DOWNLOAD_START),
m_pBufSource(NULL),
m_llLenSource(0),
m_pCookie(NULL)
{
	m_HTTPDLer.set_download_callback(this);
	m_WPM.SetCallBack(this);
}

CWebVideoParser::~CWebVideoParser(void)
{
	if (m_pBufSource)
		delete []m_pBufSource;
}

VO_U32 CWebVideoParser::GetVideoURL(const VO_PCHAR szWebLink, VO_PCHAR *ppDLs, VO_PCHAR *ppCookie)
{
	if (!szWebLink || !ppDLs)
		return VO_ERR_INVALID_ARG;

	VO_U32 ulRet = m_WPM.GetVideoURL(szWebLink, ppDLs, NULL, ppCookie);

	if (m_pBufSource) {
		delete []m_pBufSource;
		m_pBufSource = NULL;
	}

	return ulRet;
}

VO_U32 CWebVideoParser::SetLoginInfo(const VO_PCHAR szWebLink, const VO_PCHAR szUserName, const VO_PCHAR szPassWord)
{
	return VO_ERR_NONE;
}

CALLBACK_RET CWebVideoParser::received_data(VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size)
{
	if (physical_pos + size <= m_llLenSource)
	{
		memcpy(m_pBufSource + physical_pos, ptr_buffer, size);

		return CALLBACK_OK;
	}
	else
	{
		memcpy( m_pBufSource + physical_pos, ptr_buffer, static_cast<unsigned>(m_llLenSource -  physical_pos) );

		return CALLBACK_BUFFER_FULL;
	}

}

CALLBACK_RET CWebVideoParser::download_notify(DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data)
{
	VOLOGR("download_notify %d", id);

	switch(id)
	{
	case DOWNLOAD_END:
		{
			m_DLerStatus = DOWNLOAD_END;
		}
		break;

	case DOWNLOAD_START:
		break;

	case DOWNLOAD_FILESIZE:
		{
			if (ptr_data)
			{
				memcpy(&m_llLenSource, ptr_data, sizeof(m_llLenSource));

				if (m_llLenSource == -1)
					m_llLenSource = Size_WebContent;

				if (m_pBufSource)
					delete []m_pBufSource;

				m_pBufSource =  new VO_CHAR[static_cast<unsigned>(m_llLenSource)];
			}
		}
		break;

	case DOWNLOAD_TO_FILEEND:
		{
			if (ptr_data)
				memcpy(&m_llLenSource, ptr_data, sizeof(m_llLenSource));
		}
		break;

	case DOWNLOAD_COOKIE:
		{
			if (NULL != m_pCookie)
				strcat(m_pCookie, (VO_PCHAR)ptr_data);
		}
		break;

	case DOWNLOAD_ERROR:
		{
			m_DLerStatus = DOWNLOAD_ERROR;
		}
		break;

	default:
		break;
	}
	return CALLBACK_OK;
}


VO_S64 CWebVideoParser::DownloadWebData(const VO_PCHAR szLink, VO_PCHAR *ppContent, VO_PCHAR pvCookie)
{
	if(m_pBufSource) {
		delete []m_pBufSource;
		m_pBufSource = NULL;
	}
	m_pCookie = pvCookie;
	m_DLerStatus = DOWNLOAD_START;

	VOPDInitParam tmp;
	memset(&tmp, 0, sizeof(VOPDInitParam));
	tmp.nHttpProtocol = 1;

	m_HTTPDLer.set_url(szLink, &tmp);

	if (!m_HTTPDLer.start_download(0))
		return E_NETWORK_ERROR;

	do 
	{
		voOS_Sleep(300);
		if (m_DLerStatus == DOWNLOAD_ERROR)
			return E_NETWORK_ERROR;

	} while (m_DLerStatus != DOWNLOAD_END);

	*ppContent = m_pBufSource;

	return m_llLenSource;
}
