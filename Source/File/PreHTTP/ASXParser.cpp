
#include "ASXParser.h"
#include <stdio.h>


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CASXParser::CASXParser(void)
: m_pREFsHead(NULL)
, m_pREFsTail(NULL)
{
}

CASXParser::~CASXParser(void)
{
	if (m_pREFsHead)
		delete m_pREFsHead;
}

VO_VOID CASXParser::Close()
{
	if (m_pREFsHead)
		delete m_pREFsHead;

	m_pREFsHead = m_pREFsTail = NULL;
}

REFList * CASXParser::Parse(VO_CHAR *pSrc)
{
	Close();

	VO_CHAR *pOffset = pSrc;
	while (*pOffset)
	{
		if ((*pOffset)>='A' && (*pOffset)<='Z')
			*pOffset += 'a' - 'A';

		pOffset++;
	}

	if (0 != strncmp(pSrc, "<asx", 4) )
		return NULL;

	VO_CHAR szBase[1024] = {0};
	pOffset = strstr(pSrc, "<entry");
	if (pOffset)
	{
		pOffset--;

		*pOffset = '\0';
		VO_PCHAR pTMP = strstr(pSrc, "<base href = \"");
		if (pTMP)
		{
			pTMP += strlen("<base href = \"");

			memset(szBase, 0, 1024);
			sscanf(pTMP, "%[^\"]", szBase);
		}

		pOffset++;
	}

	while (pOffset)
	{
		VO_PCHAR pEND = NULL;
		REFList *pURL = new REFList();

		if ( 0 == strncmp(pOffset, "<entryref", strlen("<entryref") ) )
		{
			pEND = strstr(pOffset, "/>");
			pURL->nFlag = 1;
		}
		else
		{
			pEND = strstr(pOffset, "</entry>");
			pURL->nFlag = 0;
		}

		if (pEND)
		{
			*pEND = '\0';
			VO_PCHAR szURL = strstr(pOffset, "href");

			szURL +=  strlen("href");

			while(*szURL != '\"')
				szURL++;

			strcpy(pURL->szURL, szBase);
			sscanf( szURL + 1, "%[^\"]", pURL->szURL + strlen(szBase) );

			if (m_pREFsTail)
			{
				m_pREFsTail->pNextREF = pURL;
				m_pREFsTail = m_pREFsTail->pNextREF;
			}
			else
				m_pREFsHead = m_pREFsTail = pURL;
		}

		pOffset = strstr(pEND + 1, "<entry");
	};

	return m_pREFsHead;
}



VO_S32 CASXParser::getCount()
{
	REFList *pREF = m_pREFsHead;
	VO_S32 iCount = 0;

	while (pREF) 
	{
		iCount++;
		pREF = pREF->pNextREF;
	};

	return iCount;
}
