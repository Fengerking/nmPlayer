	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseConfig.cpp

	Contains:	voCOMXBaseConfig class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32)
#  include <tchar.h>
#elif defined(_LINUX)
#  include "voString.h"
#endif

#include "voOMXOSFun.h"
#include "voCOMXBaseConfig.h"

#include "voLog.h"
#undef LOG_TAG
#define LOG_TAG "voOMXCfg"

#ifndef _LINUX
#pragma warning (disable : 4996)
#endif



COMXCfgSect::COMXCfgSect (void)
	: m_pName (NULL)
	, m_pData (NULL)
	, m_pNext (NULL)
{
}

COMXCfgSect::~COMXCfgSect (void)
{
	if (m_pName != NULL)
		voOMXMemFree (m_pName);
	if (m_pData != NULL)
		voOMXMemFree (m_pData);
}

COMXCfgItem::COMXCfgItem (void)
	: m_pSection (NULL)
	, m_pName (NULL)
	, m_nValue (0)
	, m_pValue (NULL)
	, m_pNext (NULL)
{
}

COMXCfgItem::~COMXCfgItem (void)
{
	if (m_pName != NULL)
		voOMXMemFree (m_pName);
	if (m_pValue != NULL)
		voOMXMemFree (m_pValue);
}

voCOMXBaseConfig::voCOMXBaseConfig(void)
		: m_pFileName (NULL)
		, m_bUpdated (OMX_FALSE)
		, m_pFirstSect (NULL)
		, m_nSectNum (0)
		, m_pFirstItem (NULL)
		, m_nItemNum (0)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXBaseConfig::~voCOMXBaseConfig(void)
{
	Release ();

	if (m_pFileName != NULL)
		voOMXMemFree (m_pFileName);
}

OMX_S32 voCOMXBaseConfig::GetItemValue (OMX_STRING pSection, OMX_STRING pName, OMX_S32 nDefault)
{
	OMX_S32 nValue = nDefault;
	nValue = nValue;
	COMXCfgItem * pItem = FindItem (pSection, pName);
	if (pItem == NULL)
		return nDefault;

	return pItem->m_nValue;
}

OMX_STRING	voCOMXBaseConfig::GetItemText (OMX_STRING pSection, OMX_STRING pName)
{
	COMXCfgItem * pItem = FindItem (pSection, pName);
	if (pItem == NULL)
		return NULL;

	return pItem->m_pValue;
}

OMX_BOOL voCOMXBaseConfig::AddSection (OMX_STRING pSection)
{
	COMXCfgSect * pNewSect = new COMXCfgSect ();
	pNewSect->m_pName = (OMX_STRING) voOMXMemAlloc (strlen (pSection) + 1);
	if (pNewSect->m_pName == NULL)
		return OMX_FALSE;
	strcpy (pNewSect->m_pName, pSection);

	if (m_pFirstSect == NULL)
		m_pFirstSect = pNewSect;
	else
	{
		COMXCfgSect * pSect = m_pFirstSect;
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

	m_bUpdated = OMX_TRUE;

	return OMX_TRUE;
}

OMX_BOOL voCOMXBaseConfig::RemoveSection (OMX_STRING pSection)
{
	if (pSection == NULL)
		return OMX_FALSE;

	COMXCfgSect * pSect = m_pFirstSect;
	COMXCfgSect * pPrev = m_pFirstSect;
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
			return OMX_TRUE;
		}

		pPrev = pSect;
		pSect = pSect->m_pNext;
	}

	m_bUpdated = OMX_TRUE;

	return OMX_FALSE;
}


OMX_BOOL voCOMXBaseConfig::AddItem (OMX_STRING pSection, OMX_STRING pName, OMX_S32 nValue)
{
	COMXCfgItem * pNewItem = CreateItem (pSection, pName);
	if (pNewItem == NULL)
		return OMX_FALSE;

	pNewItem->m_pName = (OMX_STRING) voOMXMemAlloc (strlen (pName) + 1);
	if (pNewItem->m_pName == NULL)
		return OMX_FALSE;
	strcpy (pNewItem->m_pName, pName);
	pNewItem->m_nValue = nValue;

	if (m_pFirstItem == NULL)
	{
		m_pFirstItem = pNewItem;
	}
	else
	{
		COMXCfgItem * pItem = m_pFirstItem;
		while (pItem != NULL)
		{
			if (pItem->m_pNext == NULL)
			{
				pItem->m_pNext = pNewItem;
				break;
			}

			pItem = pItem->m_pNext;
		}
	}

	m_bUpdated = OMX_TRUE;

	return OMX_TRUE;
}

OMX_BOOL voCOMXBaseConfig::AddItem (OMX_STRING pSection, OMX_STRING pName, OMX_STRING pValue)
{
	COMXCfgItem * pNewItem = CreateItem (pSection, pName);
	if (pNewItem == NULL)
		return OMX_FALSE;

	pNewItem->m_pName = (OMX_STRING) voOMXMemAlloc (strlen (pName) + 1);
	if (pNewItem->m_pName == NULL)
		return OMX_FALSE;
	strcpy (pNewItem->m_pName, pName);
	pNewItem->m_pValue = (OMX_STRING) voOMXMemAlloc (strlen (pValue) + 1);
	if (pNewItem->m_pValue == NULL)
		return OMX_FALSE;
	strcpy (pNewItem->m_pValue, pValue);

	if (m_pFirstItem == NULL)
	{
		m_pFirstItem = pNewItem;
	}
	else
	{
		COMXCfgItem * pItem = m_pFirstItem;
		while (pItem != NULL)
		{
			if (pItem->m_pNext == NULL)
			{
				pItem->m_pNext = pNewItem;
				break;
			}

			pItem = pItem->m_pNext;
		}
	}

	m_bUpdated = OMX_TRUE;

	return OMX_TRUE;
}


OMX_BOOL voCOMXBaseConfig::RemoveItem (OMX_STRING pSection, OMX_STRING pName)
{
	COMXCfgItem * pFound = FindItem (pSection, pName);
	if (pFound == NULL)
		return OMX_FALSE;

	COMXCfgItem * pItem = m_pFirstItem;
	COMXCfgItem * pPrev = m_pFirstItem;
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
			return OMX_TRUE;
		}

		pPrev = pItem;
		pItem = pItem->m_pNext;
	}

	m_bUpdated = OMX_TRUE;

	return OMX_TRUE;
}


OMX_BOOL voCOMXBaseConfig::UpdateItem (OMX_STRING pSection, OMX_STRING pName, OMX_S32 nValue)
{
	COMXCfgItem * pFound = FindItem (pSection, pName);
	if (pFound == NULL)
		return OMX_FALSE;

	if (pFound->m_nValue == nValue)
		return OMX_TRUE;

	pFound->m_nValue = nValue;

	m_bUpdated = OMX_TRUE;

	return OMX_TRUE;
}

OMX_BOOL voCOMXBaseConfig::UpdateItem (OMX_STRING pSection, OMX_STRING pName, OMX_STRING pValue)
{
	COMXCfgItem * pFound = FindItem (pSection, pName);
	if (pFound == NULL)
		return OMX_FALSE;

	if (pFound->m_pValue != NULL)
	{
		if (!strcmp (pFound->m_pValue, pValue))
			return OMX_FALSE;

		voOMXMemFree(pFound->m_pValue);
	}

	pFound->m_pValue = (OMX_STRING) voOMXMemAlloc (strlen (pValue) + 1);
	if (pFound->m_pValue == NULL)
		return OMX_FALSE;
	strcpy (pFound->m_pValue, pValue);

	m_bUpdated = OMX_TRUE;

	return OMX_TRUE;
}

OMX_BOOL voCOMXBaseConfig::Open (OMX_STRING pFile)
{
//	VOLOGI ("Open config file %s", pFile);
	if (m_pFileName != NULL)
		voOMXMemFree (m_pFileName);
	m_pFileName = (OMX_STRING)voOMXMemAlloc (256);
	if (m_pFileName == NULL)
		return OMX_FALSE;

	OMX_PTR hFile = NULL;
#ifdef _WIN32
	_tcscpy ((TCHAR *)m_pFileName, (TCHAR*)pFile);
	hFile = voOMXFileOpen (m_pFileName, VOOMX_FILE_READ_ONLY);
	if (hFile == NULL)
	{
		voOMXOS_GetAppFolder (NULL, m_pFileName, 256);
		_tcscat ((TCHAR *)m_pFileName, (TCHAR*)pFile);
		hFile = voOMXFileOpen (m_pFileName, VOOMX_FILE_READ_ONLY);
	}
#elif defined LINUX
	strcpy (m_pFileName, pFile);
	hFile = voOMXFileOpen (m_pFileName, VOOMX_FILE_READ_ONLY);
	if (hFile == NULL)
	{
		strcpy (m_pFileName, _T("/data/local/voOMXPlayer/"));
		strcat (m_pFileName, pFile);
		hFile = voOMXFileOpen (m_pFileName, VOOMX_FILE_READ_ONLY);
	}
	if (hFile == NULL)
	{
		strcpy (m_pFileName, _T("/etc/"));
		strcat (m_pFileName, pFile);
		hFile = voOMXFileOpen (m_pFileName, VOOMX_FILE_READ_ONLY);
	}
	if (hFile == NULL)
	{
		strcpy (m_pFileName, _T("/system/etc/"));
		strcat (m_pFileName, pFile);
		hFile = voOMXFileOpen (m_pFileName, VOOMX_FILE_READ_ONLY);
	}
#endif // _WIN32

	if (hFile == NULL)
	{
		VOLOGI ("The config file %s could not be opened!", pFile);
		return OMX_FALSE;
	}

	OMX_S32		nFileSize = (OMX_S32) voOMXFileSize (hFile);
	if (nFileSize <= 0)
	{
		VOLOGI ("nFileSize is %d", (int)nFileSize);
		voOMXFileClose (hFile);
		return OMX_FALSE;
	}

	Release ();

	OMX_S8 *	pFileBuffer = (OMX_S8 *) voOMXMemAlloc (nFileSize);
	OMX_U32		dwRead = 0;
	if (pFileBuffer == NULL)
    {
        VOLOGE("pFileBuffer is NULL");
		return OMX_FALSE;
    }

	dwRead = voOMXFileRead (hFile, (OMX_U8 *)pFileBuffer, nFileSize);
    VOLOGI("Read %d bytes", (int)dwRead);
	voOMXFileClose (hFile);

	COMXCfgSect *	pCurSect = NULL;
	COMXCfgSect *	pNewSect = NULL;

	COMXCfgItem *	pCurItem = NULL;
	COMXCfgItem *	pNewItem = NULL;

	OMX_STRING pName = NULL;
	OMX_STRING pPos = 0;
	OMX_STRING pValue = NULL;

	OMX_S32		nLineSize = 256;

	OMX_S8 *	pBuffer = pFileBuffer;
	OMX_S32		nBufSize = nFileSize;
	OMX_S8 *	pNextLine = GetNextLine (pBuffer, nBufSize, (VO_S8*)m_szLineText, nLineSize);
	while (pNextLine != NULL)
	{
		if (m_szLineText[0] == ('['))
		{
			pNewSect = new COMXCfgSect ();
			if (pNewSect == NULL)
			{
				voOMXMemFree (pFileBuffer);
                VOLOGE ("pNewSect is NULL");
				return OMX_FALSE;
			}

			pNewSect->m_pName = (OMX_STRING) voOMXMemAlloc (nLineSize);
			if (pNewSect->m_pName == NULL)
			{
                VOLOGE ("pNewSect->m_pName is NULL");
				voOMXMemFree (pFileBuffer);
				return OMX_FALSE;
			}

			voOMXMemSet (pNewSect->m_pName, 0, nLineSize);

			OMX_S32 nEnd = 0;
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
			pPos = (OMX_STRING) strstr ((const char *)m_szLineText, "=");
			if (pPos != NULL)
			{
				pNewItem = new COMXCfgItem ();
				if (pNewItem == NULL)
				{
					voOMXMemFree (pFileBuffer);
					return OMX_FALSE;
				}

				pValue = pPos + 1;
				*pPos = 0;
				pName = (OMX_STRING)m_szLineText;

				pNewItem->m_pName = (OMX_STRING) voOMXMemAlloc (strlen (pName) + 1);
				if (pNewItem->m_pName == NULL)
				{
                    VOLOGE ("pNewSect->m_pName is NULL");
					voOMXMemFree (pFileBuffer);
					return OMX_FALSE;
				}
				strcpy (pNewItem->m_pName, pName);

				if (pValue[0] == '\"')
				{
					pValue = pValue + 1;
					OMX_STRING pEnd = strstr (pValue, "\"");
					if (pEnd != NULL)
						*pEnd = 0;

					pNewItem->m_pValue = (OMX_STRING) voOMXMemAlloc(strlen (pValue) + 1);
					if (pNewItem->m_pValue == NULL)
					{
                        VOLOGE ("pNewSect->m_pValue is NULL");
						voOMXMemFree (pFileBuffer);
						return OMX_FALSE;
					}
					strcpy (pNewItem->m_pValue, pValue);
				}
				else
				{
					if ((* (pValue + 1)) == 'X' || (* (pValue + 1)) == 'x')
						sscanf (pValue, "%d", (int *)&pNewItem->m_nValue);
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

	voOMXMemFree (pFileBuffer);

	return OMX_TRUE;
}


OMX_BOOL voCOMXBaseConfig::Write (OMX_STRING pFile)
{
	if (!m_bUpdated)
		return OMX_TRUE;

	OMX_PTR hFile = voOMXFileOpen (pFile, VOOMX_FILE_READ_WRITE);
	if (hFile == NULL)
		return OMX_FALSE;

	OMX_U32 dwWrite = 0;

	COMXCfgSect * pSect = m_pFirstSect;
	COMXCfgItem * pItem = m_pFirstItem;

	OMX_S8 szLine[256];
	char * pLine = (char *)&szLine[0];
	while (pSect != NULL)
	{
		voOMXMemSet (szLine, 0, 256);

		strcpy (pLine, "[");
		strcat (pLine, pSect->m_pName);
		strcat (pLine, "]\r\n");
		dwWrite = voOMXFileWrite (hFile, (OMX_U8 *)pLine, strlen (pLine));

		pItem = m_pFirstItem;
		while (pItem != NULL)
		{
			if (pItem->m_pSection == pSect)
			{
				voOMXMemSet (szLine, 0, 256);
				if (pItem->m_pValue == NULL)
					sprintf (pLine, "%s=%d\r\n", pItem->m_pName, (int)pItem->m_nValue);
				else
					sprintf (pLine, "%s=\"%s\"\r\n", pItem->m_pName, pItem->m_pValue);
				dwWrite = voOMXFileWrite (hFile, (OMX_U8 *)pLine, strlen (pLine));
			}

			pItem = pItem->m_pNext;
		}

		strcpy (pLine, "\r\n\r\n");
		dwWrite = voOMXFileWrite (hFile, (OMX_U8 *)pLine, strlen (pLine));

		pSect = pSect->m_pNext;
	}

	voOMXFileClose (hFile);

	return OMX_TRUE;
}

void voCOMXBaseConfig::Release (void)
{
	COMXCfgItem * pItem = m_pFirstItem;
	COMXCfgItem * pTempItem = pItem;
	while (pItem != NULL)
	{
		pTempItem = pItem->m_pNext;
		delete pItem;
		pItem = pTempItem;
	}

	m_pFirstItem = NULL;
	m_nItemNum = 0;

	COMXCfgSect * pSect = m_pFirstSect;
	COMXCfgSect * pTempSect = pSect;
	while (pSect != NULL)
	{
		pTempSect = pSect->m_pNext;
		delete pSect;
		pSect = pTempSect;
	}

	m_pFirstSect = NULL;
	m_nSectNum = 0;
}

COMXCfgSect * voCOMXBaseConfig::FindSect (OMX_STRING pSection)
{
	if (pSection == NULL)
		return NULL;

	COMXCfgSect * pSect = m_pFirstSect;
	COMXCfgSect * pFound = NULL;

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


COMXCfgItem * voCOMXBaseConfig::FindItem (OMX_STRING pSection, OMX_STRING pName)
{
	if (pSection == NULL || pName == NULL)
		return NULL;

	COMXCfgItem * pItem = m_pFirstItem;
	COMXCfgItem * pFound = NULL;

	while (pItem != NULL)
	{
		if (!strcmp (pItem->m_pSection->m_pName, pSection))
		{
			if (!strcmp (pItem->m_pName, pName))
			{
				pFound = pItem;
				break;
			}
		}

		pItem = pItem->m_pNext;
	}

	return pFound;
}

COMXCfgItem * voCOMXBaseConfig::CreateItem (OMX_STRING pSection, OMX_STRING pName)
{
	if (pSection == NULL || pName == NULL)
		return NULL;

	OMX_BOOL	bFound = OMX_FALSE;
	COMXCfgSect *	pSect = m_pFirstSect;
	while (pSect != NULL)
	{
		if (!strcmp (pSect->m_pName, pSection))
		{
			bFound = OMX_TRUE;
			break;
		}
		pSect = pSect->m_pNext;
	}
	if (!bFound)
		return NULL;

	bFound = OMX_FALSE;
	COMXCfgItem *	pItem = m_pFirstItem;
	while (pItem != NULL)
	{
		if (!strcmp (pItem->m_pName, pName))
		{
			bFound = OMX_TRUE;
			break;
		}
		pItem = pItem->m_pNext;
	}
	if (bFound)
		return NULL;

	COMXCfgItem * pNewItem  = new COMXCfgItem ();
	pNewItem->m_pSection = pSect;

	return pNewItem;
}


OMX_S8 * voCOMXBaseConfig::GetNextLine (OMX_S8 * pBuffer, OMX_S32 nBufSize, OMX_S8 * pNextLine, OMX_S32 & nLineSize)
{
	OMX_BOOL	bFound = OMX_FALSE;
	OMX_STRING	pCurPos = (OMX_STRING) pBuffer;
	OMX_STRING	pNextPos = pCurPos;

	while ((OMX_S8 *)pNextPos - pBuffer < nBufSize)
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
					bFound = OMX_TRUE;
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

		voOMXMemSet (pNextLine, 0, nLineSize);
		strncpy ((OMX_STRING)pNextLine, pCurPos, pNextPos - pCurPos);
		nLineSize = strlen ((OMX_STRING)pNextLine);

		return (OMX_S8 *)pNextPos + 1;
	}

	return NULL;
}
