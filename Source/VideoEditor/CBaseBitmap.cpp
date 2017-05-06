/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseBitmap.cpp

Contains:	CBaseBitmap class file

Written by:	Bangfei Jin
Modify by:   Leon Huang
Change History (most recent first):
2008-05-20		JBF			Create the file from CBitmapReader.h
2011-02-14		Leon			Modify for Linux
*******************************************************************************/
//#include <windows.h>
//#include <tchar.h>
#include "CBaseBitmap.h"

#pragma warning (disable : 4996)

CBaseBitmap::CBaseBitmap(void)
: m_nWidth (0)
, m_nHeight (0)
, m_nBits (16)
, m_pData (NULL)
//, m_pHandle (NULL)
, m_pBmpInfo (NULL)
, m_nBmpSize (0)
, m_bWide (false)
, m_bCloseHandleWhenRelease(true)
{
}

CBaseBitmap::~CBaseBitmap(void)
{
	Release ();
}

int CBaseBitmap::CreateBitmap (TCHAR * inFileName)
{
	return -1;
}

VO_PBYTE CBaseBitmap::GetBitmapBuffer (void)
{
	return m_pData;
}


void CBaseBitmap::SetBkColor(COLORREF clr)
{
	if(m_pBmpInfo ==NULL || m_pData == NULL)
		return;

	VO_U16 r,g,b;
	r = GetRValue(clr);
	g = GetGValue(clr);
	b = GetBValue(clr);
	if(m_nBits == 16)
	{
		VO_U16 wdClr = (((VO_U16)( b>>3))) + (((g >> 2)<<5)) 
			+ (((r>>3)<<11));	
		for(int i = 0;i<m_pBmpInfo->bmiHeader.biSizeImage;i+=2)
		{
			((VO_U16*)(m_pData+i))[0] = wdClr;
		}
	}
	else
	{
		if(r == g && g == b)
			memset(m_pData,r,m_pBmpInfo->bmiHeader.biSizeImage);
		else
		{
			VO_BYTE bt[3] = {b,g,r};
			for(int i = 0;i<abs(m_pBmpInfo->bmiHeader.biHeight);i++)
			{
				VO_BYTE * pData = m_pData + this->GetBmpDataWidth() * i;
				if(i == 0)
				{
					for(int j = 0;j<m_pBmpInfo->bmiHeader.biWidth;j++)
					{
						memcpy(pData,bt,3);
						pData+=3;
					}
				}
				else
				{
					memcpy(pData,m_pData,this->GetBmpDataWidth());
				}
			}
		}
	}
}
void CBaseBitmap::Release (void)
{
	if (m_pBmpInfo != NULL)
	{
		delete [](VO_PBYTE)m_pBmpInfo;
		m_pBmpInfo = NULL;
	}

	if (m_pData != NULL)
	{
		m_pData = NULL;
	}

	if(m_bCloseHandleWhenRelease)
	{
		/*	if (m_pHandle != NULL)
		{
		DeleteObject (m_pHandle);
		m_pHandle = NULL;
		}*/
	}
}

int CBaseBitmap::GetBmpDataWidth (void)
{
	if (m_pBmpInfo == NULL)
		return -1;

	int nWidth = m_pBmpInfo->bmiHeader.biWidth * m_pBmpInfo->bmiHeader.biBitCount / 8;
	nWidth = (nWidth + 3) & ~3;

	return nWidth;
}
