//
//  CHLSHttpRequest.cpp
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


#include "voOSFunc.h"
#include "CHLSHttpRequest.h"

#define LOG_TAG "CHLSHttpRequest"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif



CHLSHttpRequest::CHLSHttpRequest(void)
{

}

CHLSHttpRequest::~CHLSHttpRequest()
{
}



bool CHLSHttpRequest::ParseLine (char* pLine, int nSize)
{
	char	szLine[512];
	//char	szWord[128];
    
	memset (szLine, 0, sizeof (szLine));
	strcpy (szLine, pLine);
    
	char *	pLineText = szLine;
	char *  pWordText = NULL;
	int		nLineLen = strlen (pLineText);
	int		nWordLen = 0;
        
	while (nLineLen > 0)
	{
		GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
		strlwr (pWordText);
		if(m_nType == hrqUnknown)
		{
			if(!strcmp (pWordText, "get"))
			{
				m_nType = hrqGet;
                
				GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
				strcpy (m_szTarget, pWordText);
                
				if(strlen (pWordText) > 512)
					m_bURLTooLong = true;
                
				GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
				strlwr (pWordText);
				if(strncmp (pWordText, "http/", 5) == 0)
				{
					pWordText += 5;
					char * pDot = strstr (pWordText, ".");
					*pDot = 0;
					m_nVerMajor = atoi(pWordText);
					m_nVerMinor = atoi(pDot + 1);
				}
			}
			else if(!strcmp (pWordText, "head"))
			{
				m_nType = hrqHead;
                
				GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
				strcpy (m_szTarget, pWordText);
                
				if(strlen (pWordText) > 512)
					m_bURLTooLong = true;
                
				GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
				strlwr (pWordText);
                
				if(strncmp (pWordText, "http/", 5) == 0)
				{
					pWordText += 5;
					char * pDot = strstr (pWordText, ".");
					*pDot = 0;
					m_nVerMajor = atoi(pWordText);
					m_nVerMinor = atoi(pDot + 1);
				}
			}
		}
		else
		{
			if(!strcmp (pWordText, "range:") || !strcmp (pWordText, "content-range:"))
			{                
				GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
				strlwr (pWordText);
                
				char * pStart = strstr (pWordText, "=");
				char * pEnd = strstr (pWordText, "-");
				if (pEnd != NULL)
				{
                    char szStartPos[128];
                    memset(szStartPos, 0, 128);
                    strncpy(szStartPos, ++pStart, pEnd-pStart);
                    m_lPosBegin = atol(szStartPos);
                    
                    char szEndPos[128];
                    memset(szEndPos, 0, 128);
                    strcpy(szEndPos, ++pEnd);
                    m_lPosEnd = atol(szEndPos)+1;
                    
                    m_lLength = m_lPosEnd - m_lPosBegin;
                }
			}
			else if(!strcmp (pWordText, "host:"))
			{
				strcpy (m_szHost, "http://");
				GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
				strcat (m_szHost, pWordText);
			}
			else if(!strcmp (pWordText, "if-modified-since:"))
			{
 				GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
                
                //				remote_file_date = http_date(date);
				if(m_nType == hrqGet)
				{
					m_nType = hrqGetModified;
				}
			}
			else if(!strcmp (pWordText, "connection:"))
			{
				GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
				strlwr (pWordText);
				if(!strcmp (pWordText, "close"))
				{
					m_bKeepAlive = false;
				}
				else if(!strcmp (pWordText, "keep-alive"))
				{
					m_bKeepAlive = true;
				}
			}
            else if(!strcmp (pWordText, "accept-encoding:"))
            {
                memset(m_szAcceptEncoding, 0, 256);
                GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
				strcpy (m_szAcceptEncoding, pWordText);
                
                if(!strcmp(m_szAcceptEncoding, "gzip"))
                    m_bGzip = true;
            }
		}
	}
    
	return true;
}
