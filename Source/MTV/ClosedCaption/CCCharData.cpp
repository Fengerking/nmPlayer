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
* @file CCCarData.cpp
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#include "CCCharData.h"
#include "voLog.h"
#include "CDumper.h"
#include "CSubtilePacker.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
VO_VOID TranslateColorToRGB(Color_Type eType,voSubtitleRGBAColor *voRGBValue)
{
	if (!voRGBValue)
	{
		return;
	}
	switch (eType)
	{
	case Color_Type_Black:
		{
			voRGBValue->nRed = 0;
			voRGBValue->nGreen = 0;
			voRGBValue->nBlue = 0;
			voRGBValue->nTransparency = 0xFF;
		}
		break;
	case Color_Type_White:
		{
			voRGBValue->nRed = 255;
			voRGBValue->nGreen = 255;
			voRGBValue->nBlue = 255;
			voRGBValue->nTransparency = 0xFF;
		}
		break;
	case Color_Type_Green:
		{
			voRGBValue->nRed = 0;
			voRGBValue->nGreen = 255;
			voRGBValue->nBlue = 0;
			voRGBValue->nTransparency = 0xFF;
		}
		break;
	case Color_Type_Blue:
		{
			voRGBValue->nRed = 0;
			voRGBValue->nGreen = 0;
			voRGBValue->nBlue = 255;
			voRGBValue->nTransparency = 0xFF;
		}
		break;
	case Color_Type_Cyan:
		{
			voRGBValue->nRed = 0;
			voRGBValue->nGreen = 255;
			voRGBValue->nBlue = 255;
			voRGBValue->nTransparency = 0xFF;
		}
		break;
	case Color_Type_Red:
		{
			voRGBValue->nRed = 255;
			voRGBValue->nGreen = 0;
			voRGBValue->nBlue = 0;
			voRGBValue->nTransparency = 0xFF;
		}
		break;
	case Color_Type_Yellow:
		{
			voRGBValue->nRed = 255;
			voRGBValue->nGreen = 255;
			voRGBValue->nBlue = 0;
			voRGBValue->nTransparency = 0xFF;
		}
		break;
	case Color_Type_Magenta:
		{
			voRGBValue->nRed = 255;
			voRGBValue->nGreen = 0;
			voRGBValue->nBlue = 255;
			voRGBValue->nTransparency = 0xFF;
		}
		break;
	default:
		{
			voRGBValue->nRed = 255;
			voRGBValue->nGreen = 0;
			voRGBValue->nBlue = 255;
			voRGBValue->nTransparency = 0;
		}
		break;
	}
}
#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

LineInfo::LineInfo()
:m_pCharOfLine(NULL)
,m_nCharCntCurLine(0)
,m_nCurCursorOfLine(0)
{
	InitializeStruct(&m_stRowDesc,Struct_Type_voSubtitleTextRowDescriptor);
	SetDefaultValue();
}

LineInfo::~LineInfo()
{
	if (m_pCharOfLine)
	{
		delete []m_pCharOfLine;
		m_pCharOfLine = NULL;
	}
}

VO_VOID LineInfo::SetDefaultValue(Color_Type BgColor /* = Color_Type_Black */,Color_Type BorderColor /* = Color_Type_UnKnow */)
{
	TranslateColorToRGB(BgColor,&m_stRowDesc.stDataBox.stRectFillColor);
	TranslateColorToRGB(BorderColor,&m_stRowDesc.stDataBox.stRectBorderColor);
}
VO_VOID LineInfo::Init(VO_U32 dwColumnCnt)
{
	m_pCharOfLine = new CharInfo[dwColumnCnt];
	m_nCharCntCurLine = 0;
	m_nCurCursorOfLine = 0;
}


CCCharData::CCCharData(VO_U32 dwRowCnt,VO_U32 dwColCnt)
:m_RowCnt(dwRowCnt)
,m_ColumnCnt(dwColCnt)
,m_beBufferChanged(VO_FALSE)
{
	m_strCharLine = new LineInfo[m_RowCnt];
	if(m_strCharLine)
	{
		VO_U32 iRowCnt = 0;
		while (iRowCnt < m_RowCnt)
		{
			SetRectInfo(&m_strCharLine[iRowCnt].m_stRowDesc.stDataBox.stRect,iRowCnt,iRowCnt,0,0);
			m_strCharLine[iRowCnt++].Init(m_ColumnCnt);
		}
	}
}

CCCharData::~CCCharData()
{
	if(m_strCharLine)
	{
		delete []m_strCharLine;
		m_strCharLine = NULL;
	}
}

VO_VOID CCCharData::ResetBuffer()
{
	for (VO_U32 iRowCnt = 0;iRowCnt < m_RowCnt;iRowCnt++)
	{
		m_strCharLine[iRowCnt].m_nCharCntCurLine = 0;
		m_strCharLine[iRowCnt].m_nCurCursorOfLine = 0;
		InitializeStruct(&m_strCharLine[iRowCnt].m_stRowDesc,Struct_Type_voSubtitleTextRowDescriptor);
		m_strCharLine[iRowCnt].SetDefaultValue();
		SetRectInfo(&m_strCharLine[iRowCnt].m_stRowDesc.stDataBox.stRect,iRowCnt,iRowCnt,0,0);
	}
	m_beBufferChanged = VO_TRUE;
}

VO_VOID CCCharData::ResetRowBuffer(VO_U32 dwRowNo)
{
	if (dwRowNo < m_RowCnt)
	{
		m_strCharLine[dwRowNo].m_nCharCntCurLine = 0;
		m_strCharLine[dwRowNo].m_nCurCursorOfLine = 0;
		InitializeStruct(&m_strCharLine[dwRowNo].m_stRowDesc,Struct_Type_voSubtitleTextRowDescriptor);
		m_strCharLine[dwRowNo].SetDefaultValue();
		SetRectInfo(&m_strCharLine[dwRowNo].m_stRowDesc.stDataBox.stRect,dwRowNo,dwRowNo,0,0);
	}
}
VO_U32 CCCharData::PushChar(const CharInfo* pInputData, VO_U32 cbInputData,VO_U32 dwRowNo)
{
	if (!pInputData 
		|| m_strCharLine[dwRowNo].m_nCharCntCurLine + cbInputData > m_ColumnCnt || dwRowNo >= m_RowCnt)
	{
		VOLOGE("ClosedCaption Error:Out of Memory");
		return 0;
	}
	for (VO_U32 nCount = 0;(nCount < cbInputData);nCount++)
	{
		CharInfo* pData = (CharInfo *)m_strCharLine[dwRowNo].m_pCharOfLine;
		pData[m_strCharLine[dwRowNo].m_nCurCursorOfLine++] = pInputData[nCount];
		if (m_strCharLine[dwRowNo].m_nCurCursorOfLine > m_strCharLine[dwRowNo].m_nCharCntCurLine)
		{
			m_strCharLine[dwRowNo].m_nCharCntCurLine++;
		}
		m_beBufferChanged = VO_TRUE;
	}
	return 0;
}

VO_U32 CCCharData::GetString(CharInfo* pOutputData,VO_U32 dwSize)
{
	if (!pOutputData)
	{
		return 0;
	}
//	VO_U32 nSizeTrans = 0;
	VO_U32 iRow = 0;
//	VO_BOOL IsOnlyOneLine = VO_TRUE;
	CharInfo* pData = NULL;
	VO_U32 dwChaCnt = 0;
	while(iRow < m_RowCnt)
	{
		if (!m_strCharLine[iRow].m_nCharCntCurLine)
		{
			iRow++;
			continue;
		}
		pData = m_strCharLine[iRow].m_pCharOfLine;
		for(VO_U16 nCount = 0;nCount < m_strCharLine[iRow].m_nCharCntCurLine;nCount++)
		{
			if (dwChaCnt >= dwSize)
			{
				VOLOGE("ClosedCaption Error:out of UniCodeBuff")
				break;
			}
			pOutputData[dwChaCnt] = pData[nCount];///<solid block.
		}
		iRow++;
	}
	return dwChaCnt;
}

VO_U32 CCCharData::MoveCursorToColumn(VO_U32 dwRowNo,VO_U32 dwColumnNo,VO_BOOL beDel)
{
	if (dwRowNo >= m_RowCnt || dwColumnNo >= m_ColumnCnt)
	{
		return 1;
	}
//	VO_U32 ret = 0;
	CharInfo* pData = NULL;
	if (m_strCharLine[dwRowNo].m_nCurCursorOfLine == dwColumnNo)
	{
		return 0;
	}
	else if (m_strCharLine[dwRowNo].m_nCurCursorOfLine > dwColumnNo)
	{
		m_strCharLine[dwRowNo].m_nCurCursorOfLine = dwColumnNo;
		if (beDel)
		{
			m_strCharLine[dwRowNo].m_nCharCntCurLine = m_strCharLine[dwRowNo].m_nCurCursorOfLine;
		}
		m_beBufferChanged = VO_TRUE;
		return 1;
	}
	while(m_strCharLine[dwRowNo].m_nCurCursorOfLine < dwColumnNo)
	{
		pData = m_strCharLine[dwRowNo].m_pCharOfLine;
		if (m_strCharLine[dwRowNo].m_nCurCursorOfLine ==  m_strCharLine[dwRowNo].m_nCharCntCurLine)
		{
			pData[m_strCharLine[dwRowNo].m_nCurCursorOfLine].character = ASCII_SPACE_CHAR;
			pData[m_strCharLine[dwRowNo].m_nCurCursorOfLine].info.stCharEffect.Italic = 0;
			pData[m_strCharLine[dwRowNo].m_nCurCursorOfLine++].info.stCharEffect.Underline = 0;
			TranslateColorToRGB(Color_Type_White,&(pData[m_strCharLine[dwRowNo].m_nCurCursorOfLine].info.stFontInfo.stFontColor));
			m_strCharLine[dwRowNo].m_nCharCntCurLine++;
		}
		else if (m_strCharLine[dwRowNo].m_nCurCursorOfLine < m_strCharLine[dwRowNo].m_nCharCntCurLine)
		{
			m_strCharLine[dwRowNo].m_nCurCursorOfLine++;
		}
		else
			VOLOGE("ClosedCaption Error:Cursor pos more than char count");		

	}
	m_beBufferChanged = VO_TRUE;
	return 1;
}

VO_U32 CCCharData::GetString(VO_U16* pOutputData,VO_U32 dwSize)
{
	if (!pOutputData)
	{
		return 0;
	}
//	VO_U32 nSizeTrans = 0;
	VO_U32 iRow = 0;
	VO_BOOL IsOnlyOneLine = VO_TRUE;
	CharInfo* pData = NULL;
	VO_U32 dwChaCnt = 0;
	while(iRow < m_RowCnt)
	{
		if (!m_strCharLine[iRow].m_nCharCntCurLine)
		{
			iRow++;
			continue;
		}
		if (!IsOnlyOneLine)
		{
			pOutputData[dwChaCnt++] = 0x000A;///<solid block
		}
		IsOnlyOneLine = VO_FALSE;
		pData = m_strCharLine[iRow].m_pCharOfLine;
		for(VO_U16 nCount = 0;nCount < m_strCharLine[iRow].m_nCharCntCurLine;nCount++)
		{
			if (dwChaCnt >= dwSize)
			{
				VOLOGE("ClosedCaption Error:out of UniCodeBuff")
					break;
			}
			pOutputData[dwChaCnt++] = pData[nCount].character;///<solid block
		}
		iRow++;
	}
	return dwChaCnt;
}

VO_VOID CCCharData::CopyChar(VO_U32 srcRow,VO_U32 desRow)
{
	if (srcRow >= m_RowCnt || desRow >= m_RowCnt || !m_strCharLine[srcRow].m_nCharCntCurLine)
	{
		return;
	}
	memcpy(m_strCharLine[desRow].m_pCharOfLine , m_strCharLine[srcRow].m_pCharOfLine,m_strCharLine[srcRow].m_nCharCntCurLine*sizeof(CharInfo));
	m_strCharLine[desRow].m_nCharCntCurLine = m_strCharLine[srcRow].m_nCharCntCurLine;
	m_strCharLine[desRow].m_nCurCursorOfLine = m_strCharLine[srcRow].m_nCurCursorOfLine;
	m_strCharLine[srcRow].m_nCharCntCurLine = 0;
	m_strCharLine[srcRow].m_nCurCursorOfLine = 0;

//	m_beBufferChanged = VO_TRUE;
}

