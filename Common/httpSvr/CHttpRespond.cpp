	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2011				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CHttpRespond.cpp

	Contains:	CHttpRespond class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-12-21		JBF			Create file

*******************************************************************************/
#include <stdio.h>

#ifdef _WIN32
#include <io.h>
#include <string>
#else
#include <ctype.h>
#endif // _WIN32

#if defined (_IOS) || defined (_MAC_OS)
#include <sys/time.h>
#endif

#include "voOSFunc.h"
#include "CHttpRespond.h"

#include "CHttpRequest.h"

#define LOG_TAG "CHttpRespond"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif



CHttpRespond::CHttpRespond(CHttpBaseStream * pStream)
	: m_pStream (pStream)
{
	strcpy (m_szRespond, "");
	m_pDataBuff = new unsigned char[HRP_BUFF_SIZE];
	m_nDataSize = 0;

	m_uErrorCode = 200;
	strcpy (m_szHttpErr, " OK");

	strcpy (m_szAddHeader, "");

	m_bHaveBody = true;
}

CHttpRespond::~CHttpRespond ()
{
    if(m_pDataBuff)
    {
        delete []m_pDataBuff;
        m_pDataBuff = NULL;
    }
}

bool CHttpRespond::Create (CHttpRequest * pRequest, unsigned char * pBuff, int * nSize)
{
	if (m_pStream == NULL)
		return false;

	char * 	pStartText = m_szRespond;
	char 	szTemp[64];

	if(pRequest->m_nVerMajor == 1 && pRequest->m_nVerMinor == 1)
		strcpy (m_szRespond, hrp_szHttp_1_1);
	else
		strcpy (m_szRespond, hrp_szHttp_1_0);

	sprintf(szTemp, " %u", m_uErrorCode);
	strcat (m_szRespond, szTemp);
	strcat (m_szRespond, m_szHttpErr);
	strcat (m_szRespond, hrp_szHttp_endl);
	
	VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
	strcat (m_szRespond, m_szAddHeader);  

	strcat (m_szRespond, "Server: ");
	strcat (m_szRespond, hrp_szAppName);
	strcat (m_szRespond, " ");
	strcat (m_szRespond, hrp_szCopyright);
	strcat (m_szRespond, hrp_szHttp_endl);

	VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
	strcat (m_szRespond, "Date: ");
	GetSysTimeString (szTemp, sizeof (szTemp));
	strcat (m_szRespond, szTemp);
	strcat (m_szRespond, hrp_szHttp_endl);

	VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
 	strcat (m_szRespond, "Connection: ");
	if (pRequest->m_bKeepAlive)
 		strcat (m_szRespond, "Keep-Alive");
	else
 		strcat (m_szRespond, "close");
	strcat (m_szRespond, hrp_szHttp_endl);

	VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
	if (m_bHaveBody)
	{
		strcat (m_szRespond, "Content-Type: ");
		strcat (m_szRespond, hrp_szContenTypeMP4);
		strcat (m_szRespond, hrp_szHttp_endl);

		VOLOGI ("Respond %s", pStartText);
		pStartText = m_szRespond + strlen (m_szRespond);
	
 		if(pRequest->m_nVerMajor == 1 && pRequest->m_nVerMinor == 1 &&
			(m_uErrorCode == 200 || m_uErrorCode == 206))
		{
			strcat (m_szRespond, "Accept-Ranges: bytes");
			strcat (m_szRespond, hrp_szHttp_endl);
			
			VOLOGI ("Respond %s", pStartText);
			pStartText = m_szRespond + strlen (m_szRespond);		
		}

		strcat (m_szRespond, "Last-Modified: ");
		GetSysTimeString (szTemp, sizeof (szTemp));
		strcat (m_szRespond, szTemp);
		strcat (m_szRespond, hrp_szHttp_endl);
		
		VOLOGI ("Respond %s", pStartText);
		pStartText = m_szRespond + strlen (m_szRespond);

		strcat (m_szRespond, "Content-Length: ");
		sprintf (szTemp, "%lld", m_pStream->GetEndPos () - m_pStream->GetStartPos ());
		strcat (m_szRespond, szTemp);
		strcat (m_szRespond, hrp_szHttp_endl);

		VOLOGI ("Respond %s", pStartText);
		pStartText = m_szRespond + strlen (m_szRespond);

  		if(pRequest->m_nVerMajor == 1 && pRequest->m_nVerMinor == 1 && m_uErrorCode == 206)
		{
			strcat (m_szRespond, "Content-Range: bytes: ");
			sprintf (szTemp, "%lld-%lld/%lld", m_pStream->GetStartPos (), 
								m_pStream->GetEndPos () - 1, 
								m_pStream->GetEndPos () - m_pStream->GetStartPos ());
			strcat (m_szRespond, szTemp);
			strcat (m_szRespond, hrp_szHttp_endl);
			
			VOLOGI ("Respond %s", pStartText);
			//pStartText = m_szRespond + strlen (m_szRespond);		
		}
	}
 	
	strcat (m_szRespond, hrp_szHttp_endl);

	memcpy (pBuff, m_szRespond, strlen (m_szRespond));
	*nSize = strlen (m_szRespond);

	return true;
}

int CHttpRespond::ReadStream (unsigned char * pBuff, int nSize)
{
	if (m_pStream == NULL)
    {
        return 0;
    }
		
	return m_pStream->Read (pBuff, nSize);
}

int CHttpRespond::Close (void)
{
	return 0;
}

bool CHttpRespond::GetSysTimeString (char * pSysTime, int nSize)
{
#ifdef _WIN32
	SYSTEMTIME tmNow;
	GetSystemTime (&tmNow);
	sprintf(pSysTime, "%s, %u %s %u %02u:%02u:%02u GMT",
			hrp_DeysOfWeek[tmNow.wDayOfWeek],
			tmNow.wDay,
			hrp_Months[tmNow.wMonth],
			tmNow.wYear,
			tmNow.wHour,
			tmNow.wMinute,
			tmNow.wSecond);
#elif defined (_IOS) || defined (_MAC_OS)
	time_t	tmTmp;
	struct	tm *pTime;
	time (&tmTmp);
	pTime = gmtime (&tmTmp);
	sprintf(pSysTime, "%s, %u %s %u %02u:%02u:%02u GMT",
			hrp_DeysOfWeek[pTime->tm_wday],
			pTime->tm_mday,
			hrp_Months[pTime->tm_mon+1],
			pTime->tm_year + 1900,
			pTime->tm_hour,
			pTime->tm_min,
			pTime->tm_sec);
#else
	time_t	tmTmp;
	struct	tm *pTime;
	time (&tmTmp);
	pTime = gmtime (&tmTmp);
	sprintf(pSysTime, "%s, %u %s %u %02u:%02u:%02u GMT",
			hrp_DeysOfWeek[pTime->tm_wday],
			pTime->tm_mday,
			hrp_Months[pTime->tm_mon],
			pTime->tm_year + 1900,
			pTime->tm_hour,
			pTime->tm_min,
			pTime->tm_sec);
	
#endif // _WIN32

	return true;
}

void CHttpRespond::Respond406Error(CHttpRequest * pRequest, unsigned char * pBuff, int * nSize)
{
    strcpy(m_szRespond, "");
    
    char * 	pStartText = m_szRespond;
	char 	szTemp[64];
    
	if(pRequest->m_nVerMajor == 1 && pRequest->m_nVerMinor == 1)
		strcpy (m_szRespond, hrp_szHttp_1_1);
	else
		strcpy (m_szRespond, hrp_szHttp_1_0);
    
	sprintf(szTemp, " %u", m_uErrorCode);
	strcat (m_szRespond, szTemp);
	strcat (m_szRespond, m_szHttpErr);
	strcat (m_szRespond, hrp_szHttp_endl);
	
	VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
	strcat (m_szRespond, m_szAddHeader);
    
	strcat (m_szRespond, "Server: ");
	strcat (m_szRespond, hrp_szAppName);
	strcat (m_szRespond, " ");
	strcat (m_szRespond, hrp_szCopyright);
	strcat (m_szRespond, hrp_szHttp_endl);
    
	VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
	strcat (m_szRespond, "Date: ");
	GetSysTimeString (szTemp, sizeof (szTemp));
	strcat (m_szRespond, szTemp);
	strcat (m_szRespond, hrp_szHttp_endl);
    
	VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
// 	strcat (m_szRespond, "Connection: ");
//	if (pRequest->m_bKeepAlive)
// 		strcat (m_szRespond, "Keep-Alive");
//	else
// 		strcat (m_szRespond, "close");
//	strcat (m_szRespond, hrp_szHttp_endl);
    
//	VOLOGI ("Respond %s", pStartText);
//	pStartText = m_szRespond + strlen (m_szRespond);
    
    strcat (m_szRespond, "Content-Type: ");
    strcat (m_szRespond, hrp_szContenTypeTEXT_PLAIN);
    strcat (m_szRespond, hrp_szHttp_endl);
    
    VOLOGI ("Respond %s", pStartText);
    pStartText = m_szRespond + strlen (m_szRespond);
 	
	strcat (m_szRespond, hrp_szHttp_endl);
    
	memcpy (pBuff, m_szRespond, strlen (m_szRespond));
	*nSize = strlen (m_szRespond);
}

int CHttpRespond::UpdateStream(CHttpBaseStream * pStream)
{
    m_pStream = pStream;
    return 0;
}

int CHttpRespond::UpdateErrCode(int nErrCode)
{
    m_uErrorCode = nErrCode;
    return 0;
}
