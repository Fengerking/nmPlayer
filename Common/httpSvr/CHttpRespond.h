	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2011		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CHttpRespond.h

	Contains:	CHttpRespond header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-12-21		JBF			Create file

*******************************************************************************/
#ifndef __CHttpRespond_H__
#define __CHttpRespond_H__

#include "CHttpStream.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


#define	HRP_BUFF_SIZE	4096

class CHttpRequest;


static const char hrp_szCopyright[]		="Copyright (C) VisualOn 2011";
static const char hrp_szAppName[]		="HTTP Server/1.0";

static const char hrp_szHttp_endl[]		= "\r\n";
static const char hrp_szHttp_1_0[]		= "HTTP/1.0";
static const char hrp_szHttp_1_1[]		= "HTTP/1.1";

static const char hrp_sz200[]			= "OK";
static const char hrp_sz206[]			= "Partial content";

static const char hrp_sz302[]			= "Object Moved";
static const char hrp_sz304[]			= "Not Modified";

static const char hrp_sz403[]			= "Acces Forbidden";
static const char hrp_sz404[]			= "Not Found";
static const char hrp_sz405[]			= "Method Not Allowed";
static const char hrp_sz408[]			= "Request Timeout";
static const char hrp_sz414[]			= "Request-URI Too Long";
static const char hrp_sz416[]			= "Requested Range Not Satisfiable";

static const char hrp_sz501[]			= "Not Implemented";
static const char hrp_szHTML_BR[]		= "<br>";
static const char hrp_szContenTypeMP4[]	= "video/mp4";
static const char hrp_szContenTypeTEXT_HTML[]   = "text/html";
static const char hrp_szContenTypeTEXT_PLAIN[]  = "text/plain";
static const char hrp_szContenTypeM3U8[]        = "application/x-mpegURL";
static const char hrp_szContenTypeTS[]          = "video/MP2T";


static const char* hrp_DeysOfWeek[]={
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};

static const char* hrp_Months[]={
	"",
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Okt",
	"Nov",
	"Dec"
};


class CHttpRespond
{
public:
	CHttpRespond (CHttpBaseStream * pStream);
	virtual ~CHttpRespond (void);

	virtual bool	Create (CHttpRequest * pRequest, unsigned char * pBuff, int * nSize);
	virtual int		ReadStream (unsigned char * pBuff, int nSize);

	virtual int		Close (void);
    virtual int     UpdateStream(CHttpBaseStream * pStream);
    virtual int     UpdateErrCode(int nErrCode);

protected:
	bool	GetSysTimeString (char * pSysTime, int nSize);
    void    Respond406Error(CHttpRequest * pRequest, unsigned char * pBuff, int * nSize);

protected:
	char				m_szRespond[1024];

	unsigned char *		m_pDataBuff;
	int					m_nDataSize;

	unsigned int		m_uErrorCode;
	char				m_szHttpErr[32];
	char				m_szAddHeader[128];
	bool				m_bHaveBody;

	CHttpBaseStream *	m_pStream;
};


#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE

#endif // __CHttpRespond_H__
