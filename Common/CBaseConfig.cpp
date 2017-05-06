	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseConfig.cpp

	Contains:	CBaseConfig class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "voOSFunc.h"

#include "CBaseConfig.h"

#define LOG_TAG "CBaseConfig"
#include "voLog.h"

#ifndef _LINUX
#pragma warning (disable : 4996)
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CCfgSect::CCfgSect (void)
	: m_pName (NULL)
	, m_pData (NULL)
	, m_pNext (NULL)
{
}

CCfgSect::~CCfgSect (void)
{
	if (m_pName != NULL)
		cmnMemFree (VO_INDEX_MFW_VOMMPLAY, m_pName);
	if (m_pData != NULL)
		cmnMemFree (VO_INDEX_MFW_VOMMPLAY, m_pData);
}

CCfgItem::CCfgItem (void)
	: m_pSection (NULL)
	, m_pName (NULL)
	, m_nValue (0)
	, m_pValue (NULL)
	, m_pNext (NULL)
{
}

CCfgItem::~CCfgItem (void)
{
	if (m_pName != NULL)
		cmnMemFree (VO_INDEX_MFW_VOMMPLAY, m_pName);
	if (m_pValue != NULL)
		cmnMemFree (VO_INDEX_MFW_VOMMPLAY, m_pValue);
}

CBaseConfig::CBaseConfig(void)
		: m_pFileName (NULL)
		, m_bUpdated (VO_FALSE)
		, m_pFirstSect (NULL)
		, m_nSectNum (0)
		, m_pFirstItem (NULL)
		, m_nItemNum (0)
{
}

CBaseConfig::~CBaseConfig(void)
{
	Release ();

	if (m_pFileName != NULL)
		cmnMemFree (VO_INDEX_MFW_VOMMPLAY, m_pFileName);
}

VO_S32 CBaseConfig::GetItemValue (const char* pSection, const char* pItemName, VO_S32 nDefault)
{
	CCfgItem * pItem = FindItem (pSection, pItemName);
	if (pItem == NULL)
		return nDefault;

	return pItem->m_nValue;
}

VO_PCHAR CBaseConfig::GetItemText (const char* pSection, const char* pItemName, const char* pDefault)
{
	CCfgItem * pItem = FindItem (pSection, pItemName);
	if (pItem == NULL)
	{
		if (pDefault != NULL)
		{
			strcpy (m_szDefaultValue, pDefault);
			return m_szDefaultValue;
		}
		else
		{
			return NULL;
		}
	}

	return pItem->m_pValue;
}

VO_BOOL CBaseConfig::AddSection (VO_PCHAR pSection)
{
	CCfgSect * pNewSect = new CCfgSect ();
	m_memInfo.Size = strlen (pSection) + 1;
	cmnMemAlloc (VO_INDEX_MFW_VOMMPLAY, &m_memInfo);
	if (m_memInfo.VBuffer == NULL)
		return VO_FALSE;
	pNewSect->m_pName = (VO_PCHAR)m_memInfo.VBuffer;

	strcpy (pNewSect->m_pName, pSection);

	if (m_pFirstSect == NULL)
		m_pFirstSect = pNewSect;
	else
	{
		CCfgSect * pSect = m_pFirstSect;
		while (pSect != NULL)
		{
			if (pSect->m_pNext == NULL)
			{
				pSect->m_pNext = pNewSect;
				break;
			}
			pSect = pSect->m_pNext;
		}
	}

	m_nSectNum++;

	m_bUpdated = VO_TRUE;

	return VO_TRUE;
}

VO_BOOL CBaseConfig::RemoveSection (VO_PCHAR pSection)
{
	if (pSection == NULL)
		return VO_FALSE;

	CCfgSect * pSect = m_pFirstSect;
	CCfgSect * pPrev = m_pFirstSect;
	while (pSect != NULL)
	{
		if (!strcmp (pSect->m_pName, pSection))
		{
			if (pSect == m_pFirstSect)
				m_pFirstSect = m_pFirstSect->m_pNext;
			else
			{
				pPrev->m_pNext = pSect->m_pNext;
			}

			delete pSect;
			return VO_TRUE;
		}

		pPrev = pSect;
		pSect = pSect->m_pNext;
	}

	m_bUpdated = VO_TRUE;

	return VO_FALSE;
}


VO_BOOL CBaseConfig::AddItem (VO_PCHAR pSection, VO_PCHAR pItemName, VO_S32 nValue)
{
	CCfgItem * pNewItem = CreateItem (pSection, pItemName);
	if (pNewItem == NULL)
		return VO_FALSE;

	m_memInfo.Size = strlen (pItemName) + 1;
	cmnMemAlloc (VO_INDEX_MFW_VOMMPLAY, &m_memInfo);
	if (m_memInfo.VBuffer == NULL)
		return VO_FALSE;
	pNewItem->m_pName = (VO_PCHAR)m_memInfo.VBuffer;

	strcpy (pNewItem->m_pName, pItemName);
	pNewItem->m_nValue = nValue;

	CCfgItem * pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		if (pItem->m_pNext == NULL)
		{
			pItem->m_pNext = pNewItem;
			break;
		}

		pItem = pItem->m_pNext;
	}

	m_bUpdated = VO_TRUE;

	return VO_TRUE;
}

VO_BOOL CBaseConfig::AddItem (VO_PCHAR pSection, VO_PCHAR pItemName, VO_PCHAR pValue)
{
	CCfgItem * pNewItem = CreateItem (pSection, pItemName);
	if (pNewItem == NULL)
		return VO_FALSE;

	m_memInfo.Size = strlen (pItemName) + 1;
	cmnMemAlloc (VO_INDEX_MFW_VOMMPLAY, &m_memInfo);
	if (m_memInfo.VBuffer == NULL)
		return VO_FALSE;
	pNewItem->m_pName = (VO_PCHAR)m_memInfo.VBuffer;
	strcpy (pNewItem->m_pName, pItemName);

	m_memInfo.Size = strlen (pValue) + 1;
	cmnMemAlloc (VO_INDEX_MFW_VOMMPLAY, &m_memInfo);
	if (m_memInfo.VBuffer == NULL)
		return VO_FALSE;
	pNewItem->m_pName = (VO_PCHAR)m_memInfo.VBuffer;
	strcpy (pNewItem->m_pValue, pValue);

	CCfgItem * pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		if (pItem->m_pNext == NULL)
		{
			pItem->m_pNext = pNewItem;
			break;
		}

		pItem = pItem->m_pNext;
	}

	m_bUpdated = VO_TRUE;

	return VO_TRUE;
}


VO_BOOL CBaseConfig::RemoveItem (VO_PCHAR pSection, VO_PCHAR pItemName)
{
	CCfgItem * pFound = FindItem (pSection, pItemName);
	if (pFound == NULL)
		return VO_FALSE;

	CCfgItem * pItem = m_pFirstItem;
	CCfgItem * pPrev = m_pFirstItem;
	while (pItem != NULL)
	{
		if (pFound == pItem)
		{
			if (pItem == m_pFirstItem)
				m_pFirstItem = m_pFirstItem->m_pNext;
			else
			{
				pPrev->m_pNext = pItem->m_pNext;
			}

			delete pItem;
			return VO_TRUE;
		}

		pPrev = pItem;
		pItem = pItem->m_pNext;
	}

	m_bUpdated = VO_TRUE;

	return VO_TRUE;
}


VO_BOOL CBaseConfig::UpdateItem (VO_PCHAR pSection, VO_PCHAR pItemName, VO_S32 nValue)
{
	CCfgItem * pFound = FindItem (pSection, pItemName);
	if (pFound == NULL)
		return VO_FALSE;

	if (pFound->m_nValue == nValue)
		return VO_TRUE;

	pFound->m_nValue = nValue;

	m_bUpdated = VO_TRUE;

	return VO_TRUE;
}

VO_BOOL CBaseConfig::UpdateItem (VO_PCHAR pSection, VO_PCHAR pItemName, VO_PCHAR pValue)
{
	CCfgItem * pFound = FindItem (pSection, pItemName);
	if (pFound == NULL)
		return VO_FALSE;

	if (pFound->m_pValue != NULL)
	{
		if (!strcmp (pFound->m_pValue, pValue))
			return VO_FALSE;

		cmnMemFree(VO_INDEX_MFW_VOMMPLAY, pFound->m_pValue);
	}

	m_memInfo.Size = strlen (pValue) + 1;
	cmnMemAlloc (VO_INDEX_MFW_VOMMPLAY, &m_memInfo);
	if (m_memInfo.VBuffer == NULL)
		return VO_FALSE;
	pFound->m_pValue = (VO_PCHAR)m_memInfo.VBuffer;
	strcpy (pFound->m_pValue, pValue);

	m_bUpdated = VO_TRUE;

	return VO_TRUE;
}

VO_BOOL CBaseConfig::Open (VO_PTCHAR pFile, VO_FILE_OPERATOR * pFileOP)
{
	if (m_pFileName != NULL)
		cmnMemFree (VO_INDEX_MFW_VOMMPLAY, m_pFileName);
	m_memInfo.Size = 256 * sizeof(VO_TCHAR);
	cmnMemAlloc (VO_INDEX_MFW_VOMMPLAY, &m_memInfo);
	if (m_memInfo.VBuffer == NULL)
		return VO_FALSE;
	m_pFileName = (VO_PTCHAR)m_memInfo.VBuffer;

//	if (pFileOP == NULL)
	{
		if (g_fileOP.Open == NULL)
			cmnFileFillPointer ();
		pFileOP = &g_fileOP;
	}

#ifdef LINUX
	vostrcpy (m_pFileName, _T("/data/local/voOMXPlayer/"));
	vostrcat (m_pFileName, pFile);
#else
	vostrcpy (m_pFileName, pFile);
#endif // LINUX

	VO_FILE_SOURCE filSource;
	memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
	filSource.pSource = m_pFileName;
	filSource.nFlag = VO_FILE_TYPE_NAME;
	filSource.nMode = VO_FILE_READ_ONLY;

	VO_PTR hFile = pFileOP->Open (&filSource);
	if (hFile == NULL)
	{
#ifdef LINUX
		vostrcpy (m_pFileName, _T("/etc/"));
		vostrcat (m_pFileName, pFile);
#else
		voOS_GetAppFolder (m_pFileName, 256);
		vostrcat (m_pFileName, pFile);
#endif // LINUX

		hFile = pFileOP->Open (&filSource);
	}
	if (hFile == NULL)
	{
		vostrcpy (m_pFileName, pFile);
		hFile = pFileOP->Open (&filSource);
	}
	if (hFile == NULL)
	{
		// VOLOGE ("The config file %s could not be opened!", pFile);
		return VO_FALSE;
	}

	VO_S32		nFileSize = (VO_S32) pFileOP->Size (hFile);
	if (nFileSize <= 0)
	{
		VOLOGE ("nFileSize is %d", (int)nFileSize);
		pFileOP->Close (hFile);
		return VO_FALSE;
	}

	Release ();

	m_memInfo.Size = nFileSize;
	cmnMemAlloc (VO_INDEX_MFW_VOMMPLAY, &m_memInfo);
	if (m_memInfo.VBuffer == NULL)
    {
		VOLOGE ("m_memInfo.VBuffer is NULL");
		return VO_FALSE;
    }
	VO_S8 *	pFileBuffer = (VO_S8 *)m_memInfo.VBuffer;
	VO_U32		dwRead = 0;

	memset (pFileBuffer, 0, nFileSize);
	dwRead = pFileOP->Read (hFile, (VO_U8 *)pFileBuffer, nFileSize);
	pFileOP->Close (hFile);

	if(dwRead == 0)
		return VO_FALSE;

	CCfgSect *	pCurSect = NULL;
	CCfgSect *	pNewSect = NULL;

	CCfgItem *	pCurItem = NULL;
	CCfgItem *	pNewItem = NULL;

	VO_PCHAR	pName = NULL;
	VO_PCHAR	pPos = 0;
	VO_PCHAR	pValue = NULL;

	VO_S32		nLineSize = 256;
	VO_S8 *		pBuffer = pFileBuffer;
	VO_S32		nBufSize = nFileSize;
	VO_S8 *		pNextLine = GetNextLine (pBuffer, nBufSize, (VO_S8*)m_szLineText, nLineSize);

	while (pNextLine != NULL)
	{
		if (m_szLineText[0] == ('['))
		{
			pNewSect = new CCfgSect ();
			m_memInfo.Size = nLineSize;
			cmnMemAlloc (VO_INDEX_MFW_VOMMPLAY, &m_memInfo);
			if (m_memInfo.VBuffer == NULL)
            {
                VOLOGE ("m_memInfo.VBuffer is NULL");
				return VO_FALSE;
            }
			pNewSect->m_pName = (VO_PCHAR)m_memInfo.VBuffer;
			cmnMemSet (VO_INDEX_MFW_VOMMPLAY, pNewSect->m_pName, 0, nLineSize);

			VO_S32 nEnd = 0;
			for (nEnd = 2; nEnd < nLineSize; nEnd++)
			{
				if (m_szLineText[nEnd] == ']')
					break;
			}
			strncpy (pNewSect->m_pName, (const char *)m_szLineText + 1, nEnd - 1);

			if (m_pFirstSect == NULL)
				m_pFirstSect = pNewSect;
			if (pCurSect != NULL)
				pCurSect->m_pNext = pNewSect;
			pCurSect = pNewSect;
			m_nSectNum++;
		}
		else
		{
			pPos = (VO_PCHAR) strstr ((const char *)m_szLineText, "=");
			if (pPos != NULL)
			{
				pNewItem = new CCfgItem ();
				pValue = pPos + 1;
				*pPos = 0;
				pName = (VO_PCHAR)m_szLineText;

				m_memInfo.Size = strlen (pName) + 1;
				cmnMemAlloc (VO_INDEX_MFW_VOMMPLAY, &m_memInfo);
				if (m_memInfo.VBuffer == NULL)
                {
                    VOLOGE ("m_memInfo.VBuffer is NULL");
					return VO_FALSE;
                }
				pNewItem->m_pName = (VO_PCHAR)m_memInfo.VBuffer;
				if (pNewItem->m_pName == NULL)
					break;
				strcpy (pNewItem->m_pName, pName);

				if (pValue[0] == '\"')
				{
					pValue = pValue + 1;
					VO_PCHAR pEnd = strstr (pValue, "\"");
					if (pEnd != NULL)
						*pEnd = 0;

					m_memInfo.Size = strlen (pValue) + 1;
					cmnMemAlloc (VO_INDEX_MFW_VOMMPLAY, &m_memInfo);
					if (m_memInfo.VBuffer == NULL)
                    {
                        VOLOGE ("m_memInfo.VBuffer is NULL");
						return VO_FALSE;
                    }
					pNewItem->m_pValue = (VO_PCHAR)m_memInfo.VBuffer;
					if (pNewItem->m_pValue == NULL)
						break;
					strcpy (pNewItem->m_pValue, pValue);
				}
				else
				{
					if ((* (pValue + 1)) == 'X' || (* (pValue + 1)) == 'x')
						sscanf (pValue, "%xd", (unsigned int*)&pNewItem->m_nValue);
					else
						pNewItem->m_nValue = atoi (pValue);
				}

				pNewItem->m_pSection = pCurSect;

				if (m_pFirstItem == NULL)
					m_pFirstItem = pNewItem;
				if (pCurItem != NULL)
					pCurItem->m_pNext = pNewItem;
				pCurItem = pNewItem;
				m_nItemNum++;
			}
		}

		nLineSize = 256;
		nBufSize = nFileSize - (pNextLine - pFileBuffer);
		pBuffer = pNextLine;
		pNextLine = GetNextLine (pBuffer, nBufSize, (VO_S8*)m_szLineText, nLineSize);
	}

	cmnMemFree (VO_INDEX_MFW_VOMMPLAY, pFileBuffer);

	return VO_TRUE;
}


VO_BOOL CBaseConfig::Write (VO_PTCHAR pFile)
{
	if (!m_bUpdated)
		return VO_TRUE;

	VO_FILE_SOURCE filSource;
	memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
	filSource.pSource = (VO_PTR) pFile;
	filSource.nFlag = VO_FILE_TYPE_NAME;
	filSource.nMode = VO_FILE_READ_WRITE;

	VO_PTR hFile = cmnFileOpen (&filSource);
	if (hFile == NULL)
		return VO_FALSE;

	VO_U32 dwWrite = 0;

	CCfgSect * pSect = m_pFirstSect;
	CCfgItem * pItem = m_pFirstItem;

	VO_S8 szLine[256];
	char * pLine = (char *)&szLine[0];
	while (pSect != NULL)
	{
		cmnMemSet (VO_INDEX_MFW_VOMMPLAY, szLine, 0, 256);

		strcpy (pLine, "[");
		strcat (pLine, pSect->m_pName);
		strcat (pLine, "]\r\n");
		dwWrite = cmnFileWrite (hFile, (VO_U8 *)pLine, strlen (pLine));
		if(dwWrite != strlen (pLine))
			return VO_FALSE;

		pItem = m_pFirstItem;
		while (pItem != NULL)
		{
			if (pItem->m_pSection == pSect)
			{
				cmnMemSet (VO_INDEX_MFW_VOMMPLAY, szLine, 0, 256);
				if (pItem->m_pValue == NULL)
					sprintf (pLine, "%s=%d\r\n", pItem->m_pName, (int)pItem->m_nValue);
				else
					sprintf (pLine, "%s=\"%s\"\r\n", pItem->m_pName, pItem->m_pValue);
				dwWrite = cmnFileWrite (hFile, (VO_U8 *)pLine, strlen (pLine));
				if(dwWrite == strlen (pLine))
					return VO_FALSE;
			}

			pItem = pItem->m_pNext;
		}

		strcpy (pLine, "\r\n\r\n");
		dwWrite = cmnFileWrite (hFile, (VO_U8 *)pLine, strlen (pLine));
		if(dwWrite == strlen (pLine))
			return VO_FALSE;

		pSect = pSect->m_pNext;
	}

	cmnFileClose (hFile);

	return VO_TRUE;
}

void CBaseConfig::Release (void)
{
	CCfgItem * pItem = m_pFirstItem;
	CCfgItem * pTempItem = pItem;
	while (pItem != NULL)
	{
		pTempItem = pItem->m_pNext;
		delete pItem;
		pItem = pTempItem;
	}

	m_pFirstItem = NULL;
	m_nItemNum = 0;

	CCfgSect * pSect = m_pFirstSect;
	CCfgSect * pTempSect = pSect;
	while (pSect != NULL)
	{
		pTempSect = pSect->m_pNext;
		delete pSect;
		pSect = pTempSect;
	}

	m_pFirstSect = NULL;
	m_nSectNum = 0;
}

CCfgSect * CBaseConfig::FindSect (VO_PCHAR pSection)
{
	if (pSection == NULL)
		return NULL;

	CCfgSect * pSect = m_pFirstSect;
	CCfgSect * pFound = NULL;

	while (pSect != NULL)
	{
		if (!strcmp (pSect->m_pName, pSection))
		{
			pFound = pSect;
			break;
		}

		pSect = pSect->m_pNext;
	}

	return pFound;
}


CCfgItem * CBaseConfig::FindItem (const char* pSection, const char* pItemName)
{
	if (pSection == NULL || pItemName == NULL)
		return NULL;

	CCfgItem * pItem = m_pFirstItem;
	CCfgItem * pFound = NULL;

	while (pItem != NULL)
	{
		if (!strcmp (pItem->m_pSection->m_pName, pSection))
		{
			if (!strcmp (pItem->m_pName, pItemName))
			{
				pFound = pItem;
				break;
			}
		}

		pItem = pItem->m_pNext;
	}

	return pFound;
}

CCfgItem * CBaseConfig::CreateItem (VO_PCHAR pSection, VO_PCHAR pItemName)
{
	if (pSection == NULL || pItemName == NULL)
		return NULL;

	VO_BOOL	bFound = VO_FALSE;
	CCfgSect *	pSect = m_pFirstSect;
	while (pSect != NULL)
	{
		if (!strcmp (pSect->m_pName, pSection))
		{
			bFound = VO_TRUE;
			break;
		}
		pSect = pSect->m_pNext;
	}
	if (!bFound)
		return NULL;

	bFound = VO_FALSE;
	CCfgItem *	pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		if (!strcmp (pItem->m_pName, pItemName))
		{
			bFound = VO_TRUE;
			break;
		}
		pItem = pItem->m_pNext;
	}
	if (!bFound)
		return NULL;

	CCfgItem * pNewItem  = new CCfgItem ();
	return pNewItem;
}


VO_S8 * CBaseConfig::GetNextLine (VO_S8 * pBuffer, VO_S32 nBufSize, VO_S8 * pLineText, VO_S32 & nLineSize)
{
	VO_BOOL		bFound = VO_FALSE;
	VO_PCHAR	pCurPos = (VO_PCHAR) pBuffer;
	VO_PCHAR	pNextPos = pCurPos;

	while ((VO_S8 *)pNextPos - pBuffer < nBufSize)
	{
		if (!strncmp (pNextPos, ("\n"), 1))
		{
			if (pNextPos - pCurPos > 4)
			{
				if (pCurPos[0] == (';') || pCurPos[0] == ('\\'))
				{
					pCurPos = pNextPos + 1;
				}
				else
				{
					bFound = VO_TRUE;
					break;
				}
			}
			else
			{
				pCurPos = pNextPos + 1;
			}
		}

		pNextPos++;
	}

	if (bFound)
	{
		if (pNextPos - pCurPos > nLineSize)
			return NULL;

		cmnMemSet (VO_INDEX_MFW_VOMMPLAY, pLineText, 0, nLineSize);
		strncpy ((VO_PCHAR)pLineText, pCurPos, pNextPos - pCurPos);
		nLineSize = strlen ((VO_PCHAR)pLineText);

		return (VO_S8 *)pNextPos + 1;
	}

	return NULL;
}
