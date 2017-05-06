/************************************************************************
VisualOn Proprietary
Copyright (c) 2003, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
/************************************************************************
* @file CBaseSubtitleTrack.cpp
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#define LOG_TAG "CBaseSubtitleTrack"

#include "CBaseSubtitleTrack.h"
#include "voLog.h"
#include "base64.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//#pragma warning (disable : 4996)

CBaseSubtitleTrack::CBaseSubtitleTrack(void)
	: m_nTrackType (SUBTITLE_TRACK_TYPE_UNKNOWN)
	, m_nLanguage(SUBTITLE_LANGUAGE_UNKNOWN)
	, m_nFileFormat(SUBTITLE_FILEFORMAT_NORMAL)
	, m_pCurItem(NULL)
	, m_nCurrIndex(0)
	, m_pFirstItem(NULL)
	, m_pNewTextItem(NULL)
	, m_nNewTextItemIndex(0)
	, m_bNewItemDropped(false)

{	
	m_szTag[0] = 0;
	memset(m_chLang,0,MAX_LANGUAGE_STRING_BUFFER_SIZE*sizeof(VO_CHAR));
}

CBaseSubtitleTrack::~CBaseSubtitleTrack(void)
{
	Release();
}

Subtitle_Language	CBaseSubtitleTrack::GetLanguage()
{
	return m_nLanguage;
}

Subtitle_File_Format	CBaseSubtitleTrack::GetFileFormat()
{
	return m_nFileFormat;
}

bool	CBaseSubtitleTrack::SetFileFormat(Subtitle_File_Format nFileFormat)
{
	m_nFileFormat = nFileFormat;
	return true;
}


CBufferItem*	CBaseSubtitleTrack::CreateNewBuffer()
{
	CBufferItem * pNewBuffer = NULL;
	pNewBuffer = new CBufferItem ();
	if (pNewBuffer == NULL)
		return NULL;

	if (m_pFirstItem == NULL)
		m_pFirstItem = pNewBuffer;
	else
	{
		CBufferItem * pTempBuffer = m_pFirstItem;
		while (pTempBuffer->m_pNext != NULL)
			pTempBuffer = pTempBuffer->m_pNext;
		pTempBuffer->m_pNext = pNewBuffer;
	}

	int nBufSize = 1024 * sizeof (CTextItem);

	pNewBuffer->m_pBuffer = new VO_BYTE[nBufSize];
	if(!pNewBuffer->m_pBuffer)
		return NULL;
	pNewBuffer->m_pNext = NULL;
	memset (pNewBuffer->m_pBuffer, 0, nBufSize);

	return pNewBuffer;
}

void	CBaseSubtitleTrack::Release()
{
	if (m_pFirstItem == NULL)
		return;

	CTextItem * pTextItem = NULL;
	pTextItem = (CTextItem *)m_pFirstItem->m_pBuffer;
	while (pTextItem != NULL)
	{
		for (int i = 0; i < TEXT_ITEM_COUNT; i++)
		{
			if (pTextItem->m_aText[i] != NULL)
			{
				delete []pTextItem->m_aText[i];
				pTextItem->m_aText[i] = NULL;
			}
		}
		if (pTextItem->m_pImage)
		{
			if (pTextItem->m_pImage->pBuf)
			{
				delete pTextItem->m_pImage->pBuf;
			}
			delete pTextItem->m_pImage;
		}

		pTextItem = pTextItem->m_pNext;
	}

	CBufferItem * pBufNext = m_pFirstItem;
	CBufferItem * pBufDelItem = NULL;
	while (pBufNext != NULL)
	{
		pBufDelItem = pBufNext;
		pBufNext = pBufNext->m_pNext;
		delete []pBufDelItem->m_pBuffer;
		pBufDelItem->m_pBuffer = NULL;
		delete pBufDelItem;
		pBufDelItem = NULL;
	}

	m_pFirstItem = NULL;
}

VO_CHAR*	CBaseSubtitleTrack::GetCurrSubtitleItem(VO_CHAR* o_szTextLine1, VO_CHAR* o_szTextLine2, VO_CHAR* o_szTextLine3,VO_CHAR* o_szTextLine4, VO_CHAR* o_szTextLine5,VO_CHAR* o_szTextLine6, 
													VO_CHAR* o_szTextLine7, VO_CHAR* o_szTextLine8,VO_CHAR* o_szTextLine9, int* o_StartTime, int* o_EndTime, int* o_Count, bool* o_IsEnd)
{
	*o_IsEnd = false;

	if (m_pCurItem == NULL)
	{
		//VOLOGE("m_pCurItem is NULL");
		return NULL;
	}


	CTextItem * pTextItem = (CTextItem *)(m_pCurItem->m_pBuffer + sizeof(CTextItem)*m_nCurrIndex);

	if (pTextItem->m_pNext == NULL)
		*o_IsEnd = true;

	if (pTextItem->m_pNext)

		if (pTextItem->m_aText[0])
		{
			strcpy(o_szTextLine1, pTextItem->m_aText[0]);
		}

		if (pTextItem->m_aText[1])
			strcpy(o_szTextLine2, pTextItem->m_aText[1]);
		if (pTextItem->m_aText[2])
			strcpy(o_szTextLine3, pTextItem->m_aText[2]);
		if (pTextItem->m_aText[3])
			strcpy(o_szTextLine4, pTextItem->m_aText[3]);
		if (pTextItem->m_aText[4])
			strcpy(o_szTextLine5, pTextItem->m_aText[4]);
		if (pTextItem->m_aText[5])
			strcpy(o_szTextLine6, pTextItem->m_aText[5]);
		if (pTextItem->m_aText[6])
			strcpy(o_szTextLine7, pTextItem->m_aText[6]);
		if (pTextItem->m_aText[7])
			strcpy(o_szTextLine8, pTextItem->m_aText[7]);
		if (pTextItem->m_aText[8])
			strcpy(o_szTextLine9, pTextItem->m_aText[8]);


		*o_StartTime = pTextItem->m_nStartTime;
		*o_EndTime	 = pTextItem->m_nEndTime;
		*o_Count = 1;

		return (VO_CHAR*)pTextItem;
}

VO_CHAR*	CBaseSubtitleTrack::GetSubtitleItem(VO_CHAR* o_szTextLine1, VO_CHAR* o_szTextLine2, VO_CHAR* o_szTextLine3,  VO_CHAR* o_szTextLine4,VO_CHAR* o_szTextLine5,  VO_CHAR* o_szTextLine6, 
												VO_CHAR* o_szTextLine7,VO_CHAR* o_szTextLine8,  VO_CHAR* o_szTextLine9, int* o_StartTime, int* o_EndTime, int* o_Count, bool* o_IsEnd)
{
	*o_IsEnd = false;

	if (m_pCurItem == NULL)
	{
		//VOLOGI("GetCurrentText m_pCurItem is NULL, set the current item to First item");
		m_pCurItem = m_pFirstItem;
	}

	if (m_pCurItem == NULL)
	{
		//VOLOGE("GetCurrentText m_pCurItem is NULL");
		return NULL;
	}

	if (m_nCurrIndex >= 1024)
	{
		m_pCurItem = m_pCurItem->m_pNext;
		m_nCurrIndex = 0;

		if (m_pCurItem == NULL)
		{
			*o_IsEnd = true;
			//VOLOGI("GetCurrentText Is End");
			return NULL;
		}
	}


	CTextItem * pTextItem = (CTextItem *)(m_pCurItem->m_pBuffer + sizeof(CTextItem)*m_nCurrIndex);
	m_nCurrIndex++;

	if (pTextItem->m_pNext == NULL)
		*o_IsEnd = true;

	*o_StartTime = pTextItem->m_nStartTime;
	*o_EndTime	 = pTextItem->m_nEndTime;
	*o_Count = 1;
	if(o_szTextLine1 == NULL)
	{
		return (VO_CHAR*)pTextItem;
	}

	if (pTextItem->m_aText[0])
	{
		strcpy(o_szTextLine1, pTextItem->m_aText[0]);
	}

	if (pTextItem->m_aText[1])
		strcpy(o_szTextLine2, pTextItem->m_aText[1]);
	if (pTextItem->m_aText[2])
		strcpy(o_szTextLine3, pTextItem->m_aText[2]);
	if (pTextItem->m_aText[3])
		strcpy(o_szTextLine4, pTextItem->m_aText[3]);
	if (pTextItem->m_aText[4])
		strcpy(o_szTextLine5, pTextItem->m_aText[4]);
	if (pTextItem->m_aText[5])
		strcpy(o_szTextLine6, pTextItem->m_aText[5]);
	if (pTextItem->m_aText[6])
		strcpy(o_szTextLine7, pTextItem->m_aText[6]);
	if (pTextItem->m_aText[7])
		strcpy(o_szTextLine8, pTextItem->m_aText[7]);
	if (pTextItem->m_aText[8])
		strcpy(o_szTextLine9, pTextItem->m_aText[8]);


	return (VO_CHAR*)pTextItem;
}

VO_CHAR*	CBaseSubtitleTrack::GetCurSubtitleItem()
{
	if (m_pCurItem == NULL)
	{
		//VOLOGI("GetCurrentText m_pCurItem is NULL, set the current item to First item");
		m_pCurItem = m_pFirstItem;
	}

	if (m_pCurItem == NULL || m_nCurrIndex <= 0)
	{
		//VOLOGE("GetCurrentText m_pCurItem is NULL");
		return NULL;
	}
	
	CTextItem * pTextItem = (CTextItem *)(m_pCurItem->m_pBuffer + sizeof(CTextItem)*(m_nCurrIndex-1));

	if (pTextItem == NULL)
		return NULL;
	return (VO_CHAR*)pTextItem;
}

bool	CBaseSubtitleTrack::SetLanguage(Subtitle_Language nLanguage, bool bForce)
{
	if (m_nLanguage == nLanguage)
	{
		return true;
	}
	else if (m_nLanguage == SUBTITLE_LANGUAGE_UNKNOWN)
	{
		m_nLanguage = nLanguage;
		return true;
	}
	else
	{
		if (bForce)
		{	
			m_nLanguage = nLanguage;
			return true;
		}
		else
		{
			return false;
		}	
	}
	
}

VO_BOOL CBaseSubtitleTrack::SetLanguageString(VO_PCHAR chLang)
{
	if (chLang)
	{
		VO_U32 uStrLen = strlen(chLang) > (MAX_LANGUAGE_STRING_BUFFER_SIZE - 1) ? (MAX_LANGUAGE_STRING_BUFFER_SIZE - 1) : strlen(chLang);
		strncpy(m_chLang,chLang,uStrLen);
		m_chLang[uStrLen] = 0;
		return VO_TRUE;
	}
	return VO_FALSE;
}
CTextItem*	CBaseSubtitleTrack::GetNewTextItem()
{
	if (m_bNewItemDropped)
	{
		m_bNewItemDropped = false;

		if (m_pNewTextItem)
		{
			m_nNewTextItemIndex++;
			return m_pNewTextItem;
		}
	}

	if (m_pFirstItem == NULL)
	{
		CBufferItem * pItemBuffer = CreateNewBuffer ();
		if (pItemBuffer == NULL)
			return NULL;

		m_pNewTextItem = (CTextItem *)pItemBuffer->m_pBuffer;
		if(!m_pNewTextItem)
			return NULL;
		m_nNewTextItemIndex = 0;
		m_pNewTextItem->m_nVertical = -1;
		m_pNewTextItem->m_nLine = -1;
		m_pNewTextItem->m_nPosition = -1;
		m_pNewTextItem->m_nSize = -1;
		m_pNewTextItem->m_nAlign = -1;
		return m_pNewTextItem;
	}

	m_nNewTextItemIndex++;

	
	if (m_pNewTextItem && (m_nNewTextItemIndex % 1024 != 0))
	{
		
		m_pNewTextItem->m_pNext = m_pNewTextItem + 1;	
		m_pNewTextItem = m_pNewTextItem->m_pNext;
	}
	else
	{
		CBufferItem * pItemBuffer = CreateNewBuffer ();
		if (!pItemBuffer)
			return NULL;
		if(!m_pNewTextItem)
			return NULL;
		m_pNewTextItem->m_pNext = (CTextItem*)pItemBuffer->m_pBuffer;
		m_pNewTextItem = (CTextItem *)pItemBuffer->m_pBuffer;		
	}
	if(!m_pNewTextItem)
			return NULL; 	//what a fuck to check so many times!!!
	m_pNewTextItem->m_nVertical = -1;
	m_pNewTextItem->m_nLine = -1;
	m_pNewTextItem->m_nPosition = -1;
	m_pNewTextItem->m_nSize = -1;
	m_pNewTextItem->m_nAlign = -1;
	
	return m_pNewTextItem;	
}


bool	CBaseSubtitleTrack::DropLastTextItem()
{
	m_bNewItemDropped = true;

	m_nNewTextItemIndex--;
	return ClearCurrTextItem();
}

bool	CBaseSubtitleTrack::FinishAddNewTextItem()
{
	if (!m_bNewItemDropped)
		return true;

	if (m_pFirstItem == NULL)
		return true;
	

	if (m_nNewTextItemIndex % 1024 != 0)
	{
		m_nNewTextItemIndex--;

		m_pNewTextItem = m_pNewTextItem -1;
		m_pNewTextItem->m_pNext = NULL;		
	}
	else
	{
		m_nNewTextItemIndex--;

		CBufferItem * pBufNext = m_pFirstItem;
		CBufferItem * pBufLast = m_pFirstItem;

	
		while (pBufNext->m_pNext != NULL)
		{
			pBufLast = pBufNext;
			pBufNext = pBufNext->m_pNext;		
		}

		if (pBufLast == m_pFirstItem)
			return true;

		CBufferItem * pBufItem = pBufLast->m_pNext;
		delete []pBufItem->m_pBuffer; 
		pBufItem->m_pBuffer = NULL;
		delete pBufItem;
		pBufItem = NULL;

		pBufLast->m_pNext = NULL;			
	}

	return true;
}
VO_BOOL CBaseSubtitleTrack::AddCurrTextItemImage(ImageInfo* pImageInfo, VO_BOOL bBase64En)
{
	if (m_pNewTextItem == NULL || pImageInfo == NULL)
		return VO_FALSE;
	
	m_pNewTextItem->m_pImage = new ImageInfo;
	ImageInfo* pInfo = m_pNewTextItem->m_pImage;

	*pInfo = *(ImageInfo*)pImageInfo;

	if (bBase64En)
	{
		VO_BYTE tmp = pImageInfo->pBuf[pImageInfo->uBufSize];
		pImageInfo->pBuf[pImageInfo->uBufSize] = 0;

		pInfo->pBuf = (VO_PCHAR)Base64Decode(pInfo->pBuf,pInfo->uBufSize);

		pImageInfo->pBuf[pImageInfo->uBufSize] = tmp;
	}
	else
	{
		pInfo->pBuf = new VO_CHAR[pInfo->uBufSize];
		memcpy(pInfo->pBuf,pImageInfo->pBuf,pInfo->uBufSize);
	}

	return VO_TRUE;
}

VO_CHAR*	CBaseSubtitleTrack::AddCurrTextItemText(char* szText)
{
	if (m_pNewTextItem == NULL || szText == NULL || szText[0] == 0)
		return NULL;

	int i = 0;
	for (i = 0; i < TEXT_ITEM_COUNT; i++)
	{
		if (m_pNewTextItem->m_aText[i] == NULL)
			break;
	}
	if (i >= TEXT_ITEM_COUNT)
		return NULL;

	m_pNewTextItem->m_aText[i] = new char[strlen (szText)+1];
	if(!m_pNewTextItem->m_aText[i])
		return NULL;
	strcpy (m_pNewTextItem->m_aText[i], szText);
	//FilterSpecialChar(m_pNewTextItem->m_aText[i]);
	return m_pNewTextItem->m_aText[i];
}

void	CBaseSubtitleTrack::FilterSpecialChar(VO_CHAR* pText)
{
#ifndef _WIN32
	VO_CHAR* pTargetText = NULL;
	int nLen = vostrlen(pText);

	pTargetText = vostrstr(pText, "<i>");
	if (pTargetText != NULL)
	{
		int nCopyLen = nLen - (pTargetText - pText) - 3;
		memcpy(pTargetText, pTargetText+3, nCopyLen);
		pText[nLen -3] = pText[nLen -2] = pText[nLen - 1] = '\0';
		nLen = nLen - 3;
	}

	pTargetText = vostrstr(pText, "</i>");
	if (pTargetText != NULL)
	{
		int nCopyLen = nLen - (pTargetText - pText) - 4;
		memcpy(pTargetText, pTargetText+4, nCopyLen);
		pText[nLen - 4] = pText[nLen -3] = pText[nLen -2] = pText[nLen - 1] = '\0';
		nLen = nLen - 4;
	}
#endif	

}

bool	CBaseSubtitleTrack::SetCurrTextItemStartTime(int nStartTime)
{
	if (m_pNewTextItem == NULL)
		return false;
	m_pNewTextItem->m_nStartTime = nStartTime;
	m_nLastBegin	= nStartTime;
	return true;
}

bool	CBaseSubtitleTrack::SetCurrTextItemEndTime(int nEndTime)
{
	if (m_pNewTextItem == NULL)
		return false;
	m_pNewTextItem->m_nEndTime = nEndTime;
	m_nDuration = nEndTime;
	return true;
}

bool	CBaseSubtitleTrack::SetCurrTextItemColor(int nColor)
{
	if (m_pNewTextItem == NULL)
		return false;

	m_pNewTextItem->m_nColor = nColor;
	return true;
}

bool	CBaseSubtitleTrack::SetCurrTextItemVertical(int vertical)
{
	if (m_pNewTextItem == NULL)
		return false;

	m_pNewTextItem->m_nVertical = vertical;
	return true;
}

bool	CBaseSubtitleTrack::SetCurrTextItemLine(int line)
{
	if (m_pNewTextItem == NULL)
		return false;

	m_pNewTextItem->m_nLine= line;
	return true;
}

bool	CBaseSubtitleTrack::SetCurrTextItemPosition(int position)
{
	if (m_pNewTextItem == NULL)
		return false;

	m_pNewTextItem->m_nPosition = position;
	return true;
}

bool	CBaseSubtitleTrack::SetCurrTextItemSize(int size)
{
	if (m_pNewTextItem == NULL)
		return false;

	m_pNewTextItem->m_nSize = size;
	return true;
}

bool	CBaseSubtitleTrack::SetCurrTextItemAlign(int align)
{
	if (m_pNewTextItem == NULL)
		return false;

	m_pNewTextItem->m_nAlign = align;
	return true;
}

bool	CBaseSubtitleTrack::ClearCurrTextItem()
{
	if (m_pNewTextItem == NULL)
		return true;

	for (int i = 0; i < TEXT_ITEM_COUNT; i++)
	{
		VO_CHAR* pText = m_pNewTextItem->m_aText[i];
		if (pText)
		{
			delete []pText;
			pText = NULL;
		}
	}

	memset(m_pNewTextItem, 0, sizeof(CTextItem));
	return true;
}

bool	CBaseSubtitleTrack::CloneCurrTextItem(CTextItem* io_pTextItem)
{
	if (m_pNewTextItem == NULL || io_pTextItem == NULL)
		return false;

	memset(io_pTextItem, 0, sizeof(CTextItem));
	for (int i = 0; i < TEXT_ITEM_COUNT; i++)
	{
		if (m_pNewTextItem->m_aText[i] == NULL)
			break;

		io_pTextItem->m_aText[i] = new char[strlen (m_pNewTextItem->m_aText[i])+1];
		if(!io_pTextItem->m_aText[i])
			return false;
		strcpy (io_pTextItem->m_aText[i], m_pNewTextItem->m_aText[i]);
	}

	io_pTextItem->m_nColor		= m_pNewTextItem->m_nColor;
	io_pTextItem->m_nStartTime	= m_pNewTextItem->m_nStartTime;
	io_pTextItem->m_nEndTime	= m_pNewTextItem->m_nEndTime;
	io_pTextItem->m_pNext		= NULL;	

	return true;
}

bool	CBaseSubtitleTrack::CloneAndCommitCurrTextItem(CTextItem* io_pTextItem)
{
	if (m_pNewTextItem == NULL || io_pTextItem == NULL)
		return false;

	memcpy(io_pTextItem, m_pNewTextItem, sizeof(CTextItem));

	for (int i = 0; i < TEXT_ITEM_COUNT; i++)
		io_pTextItem->m_aText[i] = NULL;
	io_pTextItem->m_pNext = NULL;

	char* pFirstText = m_pNewTextItem->m_aText[0];
	if (pFirstText == NULL)
		DropLastTextItem();

	return true;
}

VO_CHAR*	CBaseSubtitleTrack::GetCurrTextItemLastText()
{
	if (m_pNewTextItem == NULL)
		return NULL;

	int i = 0;
	for (i = 0; i < TEXT_ITEM_COUNT; i++)
	{
		if (m_pNewTextItem->m_aText[i] == NULL)
			break;
	}
	i--;

	if (i < 0)
		return NULL;
	else
		return m_pNewTextItem->m_aText[i];
}

bool	CBaseSubtitleTrack::SetPos(int nPos)
{
	if (m_pFirstItem == NULL)
	{
		m_nCurrIndex = 0;
		m_pCurItem = NULL;
		return false;
	}

	CBufferItem* pItem		= m_pFirstItem;
	CBufferItem* pItemPrev	= m_pFirstItem;

	while(pItem)
	{
		CTextItem* pTextItem = (CTextItem *)pItem->m_pBuffer;

		if (pTextItem->m_nEndTime > nPos)
			break;

		pItemPrev = pItem;
		pItem = pItem->m_pNext;
	}


	CTextItem* pTextItem	 = (CTextItem *)pItemPrev->m_pBuffer;
	CTextItem* pTextItemPrev = pTextItem;

	int nCurrPos = 0;
	for (int i = 0; i < 1024; i++)
	{
		if (pTextItem->m_nStartTime < nPos && nPos < pTextItem->m_nEndTime)
		{
			nCurrPos = i;
			break;
		}

		pTextItemPrev = pTextItem;
		pTextItem = pTextItemPrev->m_pNext;

		if (pTextItem == NULL)
		{
			m_nCurrIndex = 0;
			m_pCurItem = NULL;
			return false;
		}
	}

	m_pCurItem   = pItemPrev;
	m_nCurrIndex = nCurrPos;	

	return true;
}

bool	CBaseSubtitleTrack::FillEndTime()
{
	if (m_pFirstItem == NULL)
		return false;

	CBufferItem* pItem		= m_pFirstItem;

	CTextItem* pTextItem	 = NULL;
	CTextItem* pTextItemPrev = NULL;
	
	while(pItem)
	{
		pTextItem = (CTextItem *)pItem->m_pBuffer;
		for (int i = 0; i < 1024; i++)
		{		
			if (pTextItem == NULL)		
				return false;

			if (pTextItemPrev != NULL)
			{
				if (pTextItem->m_nStartTime >= pTextItemPrev->m_nStartTime)
				{
					if (pTextItemPrev->m_nEndTime == 0)
						pTextItemPrev->m_nEndTime = pTextItem->m_nStartTime;
				}
				else
				{
					CTextItem* pTextSearchItem	   = pTextItem->m_pNext;
					CTextItem* pTextSearchPrevItem = pTextItem;

					while(pTextSearchItem)
					{
						if (pTextSearchItem->m_nStartTime > pTextItemPrev->m_nStartTime)
						{	
							pTextItemPrev->m_nEndTime = pTextSearchItem->m_nStartTime;

							int nLastTime = pTextSearchPrevItem->m_nStartTime;
							FixEndTimeForWrongTime(pTextSearchPrevItem, nLastTime, pTextItemPrev);

							break;
						}

						pTextSearchPrevItem = pTextSearchItem;
						pTextSearchItem = pTextSearchItem->m_pNext;
					}

					if (pTextSearchItem == NULL)
						pTextItemPrev->m_nEndTime = pTextItemPrev->m_nEndTime + 3000;

				}
			
			}

			pTextItemPrev = pTextItem;
			pTextItem = pTextItem->m_pNext;
		}
		
		pItem = pItem->m_pNext;
	}
	
	return true;
}

bool	CBaseSubtitleTrack::SmartLangCheck()
{

	if (m_pFirstItem == NULL)
		return false;

	CBufferItem* pItem		= m_pFirstItem;

	CTextItem* pTextItem	 = NULL;
	//CTextItem* pTextItemPrev = NULL;

	int nCountEng = 0;
	int nCountChinese = 0;
	
	pTextItem = (CTextItem *)pItem->m_pBuffer;
	for (int i = 0; i < 240; i++)
	{		
		if (pTextItem == NULL)		
			break;

		if (i % 7 != 0)
		{
			pTextItem = pTextItem->m_pNext;
			continue;
		}

		if (pTextItem->m_aText[0] == NULL)
		{
			pTextItem = pTextItem->m_pNext; 
			continue;
		}

		int nLen = 0;
#ifndef _WIN32
		nLen = vostrlen(pTextItem->m_aText[0]);
#endif
		if (nLen > 6)
			nLen = 6;

		for (int i = 0; i < nLen; i++ )
		{
			if (IsEnglishChar(pTextItem->m_aText[0][i]))
				nCountEng++;
			else
				nCountChinese++;
		}
		pTextItem = pTextItem->m_pNext;
	}

	pItem = pItem->m_pNext;
	
	if (nCountChinese > nCountEng/2)
	{
		if (GetLanguage() == SUBTITLE_LANGUAGE_ENGLISH || GetLanguage() == SUBTITLE_LANGUAGE_UNKNOWN)
			SetLanguage(SUBTITLE_LANGUAGE_SCHINESE, true);
	}
	else
		if (nCountChinese < 10)
			if (GetLanguage() == SUBTITLE_LANGUAGE_SCHINESE || GetLanguage() == SUBTITLE_LANGUAGE_UNKNOWN)
				SetLanguage(SUBTITLE_LANGUAGE_ENGLISH, true);


	return true;
}


bool	CBaseSubtitleTrack::IsEnglishChar(VO_CHAR cChar)
{
	if (cChar >= 63 && cChar <=122)
		return true;
	else if (cChar >= 32 && cChar <= 48)
		return true;
	else if (IsNumberChar(cChar))
		return true;
	else
		return false;
}

bool	CBaseSubtitleTrack::IsNumberChar(VO_CHAR cChar)
{
	if (cChar >= 49 && cChar <=57)
		return true;
	else
		return false;
}

int	CBaseSubtitleTrack::IsSameTag(char* szTag)
{
	if (strlen(m_szTag) == 0)
	{
		SetTag(szTag);
		return 2;
	}

	char szTemp[32];
	memset(szTemp,0,32);
	int nLen = strlen(szTag);
	if(nLen>4)
		nLen = 4;
	memcpy(szTemp,szTag,nLen);

	if (szTemp[2] == '>')
		return (memcmp(m_szTag, szTemp, 2) == 0) ? 1: 0;
	else
		return (memcmp(m_szTag, szTemp, 4) == 0) ? 1: 0;
}

void	CBaseSubtitleTrack::SetTag(char* szTag)
{
	int nLen = strlen(szTag);
	if(nLen>4)
		nLen = 4;
	memset(m_szTag,0,sizeof(m_szTag));
	memcpy(m_szTag, szTag, nLen);
	m_szTag[nLen] = 0;
}

void	CBaseSubtitleTrack::FixEndTimeForWrongTime(CTextItem* pWrongItem, int nTime, CTextItem* pLastItem)
{
	if (m_pFirstItem == NULL)
		return;

	CBufferItem* pItem		= m_pFirstItem;

	CTextItem* pTextItem	 = NULL;
	CTextItem* pFounded		 = NULL;

	bool bQuit = false;

	while(pItem)
	{
		pTextItem = (CTextItem *)pItem->m_pBuffer;
		for (int i = 0; i < 1024; i++)
		{		
			if (pTextItem == NULL)		
				return;

			if (pLastItem == pTextItem)
			{
				bQuit = true;
				break;
			}

			if (nTime > pTextItem->m_nStartTime && nTime < pTextItem->m_nEndTime)
			{
				bQuit = true;
				pFounded = pTextItem;
				break;
			}

			pTextItem = pTextItem->m_pNext;
		}

		if (bQuit)
			break;
		pItem = pItem->m_pNext;
	}

	if (pFounded)
	{
		pWrongItem->m_nEndTime = pFounded->m_nEndTime;
		pFounded->m_nEndTime = nTime;
	}
	else
	{
		pItem		= m_pFirstItem;
		pTextItem = (CTextItem *)pItem->m_pBuffer;

		if (pTextItem && pTextItem->m_nStartTime > nTime)
		{
			pWrongItem->m_nEndTime = pTextItem->m_nStartTime;
		}
	}

	return;
}
void CBaseSubtitleTrack::AppendTrackData(CBaseSubtitleTrack* pTrack)
{
	if(!pTrack)
	{
		VOLOGE("pTrack is a NULL pointer!");
		return;
	}
	CBufferItem* pCurr = pTrack->m_pFirstItem;
	while(pCurr)
	{
		CTextItem * pTextItem = (CTextItem *)pCurr->m_pBuffer;
		if(pTextItem)
		{
			this->GetNewTextItem();
			this->SetCurrTextItemStartTime(pTextItem->m_nStartTime);
			this->SetCurrTextItemEndTime(pTextItem->m_nEndTime);
			this->AddCurrTextItemText(pTextItem->m_aText[0]);
		}
		
		pCurr = pCurr->m_pNext;
	}
}
