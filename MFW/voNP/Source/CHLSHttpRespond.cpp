//
//  CHLSHttpRespond.cpp
//  
//
//  Created by Jim Lin on 9/11/12.
//
//

#ifdef _WIN32
#include <io.h>
#include <string>
#else
#include <ctype.h>
#endif // _WIN32

#include "CHLSHttpRespond.h"
#include "CHLSHttpRequest.h"
#include "voOSFunc.h"
#include "CHLSM3u8Stream.h"

#define LOG_TAG "CHLSHttpRespond"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CHLSHttpRespond::CHLSHttpRespond(CHttpBaseStream * pStream)
:CHttpRespond(pStream)
{
}

CHLSHttpRespond::~CHLSHttpRespond()
{
}

bool CHLSHttpRespond::Create (CHttpRequest * pRequest, unsigned char * pBuff, int * nSize)
{
	if (m_pStream == NULL)
		return false;
    
    if(!CheckRequestSupport((CHLSHttpRequest*)pRequest, pBuff, nSize))
    {
        VOLOGW("[NPW]Request not support");
        return true;
    }
    
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
	
	//VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
	strcat (m_szRespond, m_szAddHeader);
    
	strcat (m_szRespond, "Server: ");
	strcat (m_szRespond, hrp_szAppName);
	strcat (m_szRespond, " ");
	strcat (m_szRespond, hrp_szCopyright);
	strcat (m_szRespond, hrp_szHttp_endl);
    
	//VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
	strcat (m_szRespond, "Date: ");
	GetSysTimeString (szTemp, sizeof (szTemp));
	strcat (m_szRespond, szTemp);
	strcat (m_szRespond, hrp_szHttp_endl);
    
	//VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
 	strcat (m_szRespond, "Connection: ");
	if (pRequest->m_bKeepAlive)
 		strcat (m_szRespond, "Keep-Alive");
	else
 		strcat (m_szRespond, "close");
	strcat (m_szRespond, hrp_szHttp_endl);
    
	//VOLOGI ("Respond %s", pStartText);
	pStartText = m_szRespond + strlen (m_szRespond);
	
	if (m_bHaveBody)
	{
		strcat (m_szRespond, "Content-Type: ");
        
        bool bM3U8 = ((CHLSHttpStream*)m_pStream)->IsM3u8Stream();
        if(bM3U8)
            strcat (m_szRespond, hrp_szContenTypeM3U8);
        else
            strcat (m_szRespond, hrp_szContenTypeTS);
		strcat (m_szRespond, hrp_szHttp_endl);
        
		//VOLOGI ("Respond %s", pStartText);
		pStartText = m_szRespond + strlen (m_szRespond);
        
 		if(pRequest->m_nVerMajor == 1 && pRequest->m_nVerMinor == 1 &&
           (m_uErrorCode == 200 || m_uErrorCode == 206))
		{
			strcat (m_szRespond, "Accept-Ranges: bytes");
			strcat (m_szRespond, hrp_szHttp_endl);
			
			//VOLOGI ("Respond %s", pStartText);
			pStartText = m_szRespond + strlen (m_szRespond);
		}
        
		strcat (m_szRespond, "Last-Modified: ");
		GetSysTimeString (szTemp, sizeof (szTemp));
		strcat (m_szRespond, szTemp);
		strcat (m_szRespond, hrp_szHttp_endl);
		
		//VOLOGI ("Respond %s", pStartText);
		pStartText = m_szRespond + strlen (m_szRespond);
        
		strcat (m_szRespond, "Content-Length: ");
		sprintf (szTemp, "%lld", m_pStream->GetEndPos () - m_pStream->GetStartPos ());
		strcat (m_szRespond, szTemp);
		strcat (m_szRespond, hrp_szHttp_endl);
        
		//VOLOGI ("Respond %s", pStartText);
		pStartText = m_szRespond + strlen (m_szRespond);
        
  		if(pRequest->m_nVerMajor == 1 && pRequest->m_nVerMinor == 1 && m_uErrorCode == 206)
		{
			strcat (m_szRespond, "Content-Range: bytes: ");
            sprintf (szTemp, "%lld-%lld/%lld", m_pStream->GetStartPos (),
                     m_pStream->GetEndPos () - 1,
                     m_pStream->Size());

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

bool CHLSHttpRespond::CheckRequestSupport(CHLSHttpRequest * pRequest, unsigned char * pBuff, int * nSize)
{
    return true;
    
    //check accept-encoding
    char szEncoding[256];
    memset(szEncoding, 0, 256);
    strcpy(szEncoding, pRequest->m_szAcceptEncoding);
    //strlwr(szEncoding);
    
    if(!strcmp (szEncoding, "gzip"))
    {
        VOLOGW("[NPW]406 error");
        m_uErrorCode = 406;
        m_bHaveBody = false;
        strcpy (m_szHttpErr, " Not Acceptable");
        Respond406Error(pRequest, pBuff, nSize);
        return false;
    }
    return true;
}