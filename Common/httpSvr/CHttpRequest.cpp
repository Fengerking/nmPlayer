	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2011				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CHttpRequest.cpp

	Contains:	CHttpRequest class file

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


#include "voOSFunc.h"
#include "CHttpRequest.h"

#define LOG_TAG "CHttpRequest"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CHttpRequest::CHttpRequest(void)
{
	Reset ();
}

CHttpRequest::~CHttpRequest ()
{
}

bool CHttpRequest::Parse (const char* pBuffer, int nLen)
{
	char*	pBuff = const_cast<char*>(pBuffer);
	int		nLength = nLen;
	int		nLineSize = 0;
	char*	pLine = NULL;

	if(nLen > 0x1000-1)
	{
		m_bTooLarge = true;
		return true;
	}

    int nLine = 0;
    
	while(GetLine (&pBuff, &nLength, &pLine, &nLineSize) == true)
	{
        if(nLine <= 0)
        {
            //VOLOGI ("Line:  %s", pLine);
        }
        
		nLine++;
		ParseLine (pLine, nLineSize);
	}

	m_lProcessed++;
	
	return true;
}

//request::interpret_line(char* line, int line_size)
bool CHttpRequest::ParseLine (char* pLine, int nSize)
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
				m_lPosBegin = 0;
				m_lPosEnd = 0;
				m_lLength = 0;

				GetWord (&pLineText, &nLineLen, &pWordText, &nWordLen);
				strlwr (pWordText);				

				char * pStart = strstr (pWordText, "=");
				char * pEnd = strstr (pWordText, "-");
				if (pEnd != NULL)
				{
					*pEnd = 0;
					m_lPosBegin = atol(pStart);
					pStart = pEnd++;
					pEnd = strstr (pWordText, "//");
					if (pEnd != NULL)
					{
						*pEnd = 0;
						m_lPosEnd = atol(pStart);
						m_lPosEnd++;
					}
				}

				pStart = pEnd++;
				if(pStart != NULL)
				{
					m_lLength = atol(pStart);
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
		}

	}
 
	return true;
}

bool CHttpRequest::GetLine (char ** pBuffer, int* nLen, char** pLine, int* nLineSize)
{
	char *	pBegin = *pBuffer;
	char *	pEnd = *pBuffer;
	int		nChars = 0;

	*pLine = NULL;
	*nLineSize = 0;

	if(*nLen<=0)
		return false;

	while(*nLen > 0 && *pBegin != 0 && (*pBegin ==' \r' || 
			*pBegin=='\n' || *pBegin==' ' || *pBegin == '\t'))
	{
		pBegin++;
		(*nLen)--;
	}

	pEnd=pBegin;
	
	while((*nLen) > 0 && *pEnd!=0 && *pEnd!='\r' && *pEnd!='\n')
	{
		pEnd++;
		(*nLen)--;
		nChars++;
	}

	*pBuffer=pEnd;
	if(*nLen>0)
	{
		(*pBuffer)++;
		(*nLen)--;
	}

	if(nChars>0)
	{
		*pEnd=0;
		*pLine=pBegin;
		*nLineSize=nChars;
		return true;
	}
	else
	{
		return false;
	}
}

bool CHttpRequest::GetWord (char ** ppLine, int* nLineSize, char** pWord, int* nWordSize)
{
	char *	pBegin = *ppLine;
	int		nChars = 0;

	*pWord = NULL;
	*nWordSize = 0;

	if(*nLineSize<=0)
		return false;

  	while(*nLineSize > 0)
	{
		if (*pBegin == ' ')
			break;

		pBegin++;
		(*nLineSize)--;

		nChars++;
	}

	*pWord = *ppLine;
	*nWordSize = nChars;

	if (*nLineSize > 0)
	{
		*pBegin = 0;
		*ppLine = pBegin + 1;
	}

	return true;
}

void CHttpRequest::strlwr (char* pText) 
{
	char * pChar = pText;
	for (int i = 0; i < strlen (pText); i++) 
	{
		 *pChar = tolower(*pChar);
		 pChar++;
	}
} 

void CHttpRequest::Reset (void)
{
	m_nType = hrqUnknown;
	strcpy (m_szTarget, "");
	strcpy (m_szHost, "");
	m_nVerMajor = 0;
	m_nVerMinor = 0;
	m_lPosBegin = 0;
	m_lPosEnd = 0;
	m_lLength = 0;
	m_bTooLarge = false;
	m_bURLTooLong = false;
	m_bKeepAlive = false;
	m_lProcessed = 0;
    m_bGzip = false;
}

bool CHttpRequest::TimeOut (void)
{
    return false;
}
