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
* @file CCCharParser.cpp
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/

#include "CCCharParser.h"
#include "voLog.h"
#include "CDumper.h"
#ifdef _WINDOWS
#include "windows.h"
#else
#include <locale.h>
#include <stdlib.h>
#endif
#ifdef HTML_PACK
#include "CHTMLPacker.h"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CodePage CodePageToUniCode[2] = 
{
	{932,"Japanese_Japan.932"},///<codepage : Shift-JIS To Unicode
	{0,"C"}///<codepage : CP_ACP,ASCII char less then 256
};

const VO_U16 SPECIALCHARCODEMAP[][2] =
{
	{0x30,0xAE},
	{0x31,0xB0},
	{0x32,0xBD},
	{0x33,0xBF},
	{0x34,0x2122},
	{0x35,0xA2},
	{0x36,0xA3},
	{0x37,0x266A},
	{0x38,0xE0},
	{0x39,0x20},///<transparent char
	{0x3A,0xE8},
	{0x3B,0xE2},
	{0x3C,0xEA},
	{0x3D,0xEE},
	{0x3E,0xF4},
	{0x3F,0xFB}
};

const VO_U16 ExtendedSpanishFrenchCodeMap[][2] =
{
	{0x20,0xC1},
	{0x21,0xC9},
	{0x22,0xD3},
	{0x23,0xDA},
	{0x24,0xDC},
	{0x25,0xFC},
	{0x26,0x2018},
	{0x27,0xA1},
	{0x28,0x2A},
	{0x29,0x27},
	{0x2A,0x2014},
	{0x2B,0xA9},
	{0x2C,0x2120},
	{0x2D,0x2022},
	{0x2E,0x201C},
	{0x2F,0x201D},
	{0x30,0xC0},
	{0x31,0xC2},
	{0x32,0xC7},
	{0x33,0xC8},
	{0x34,0xCA},
	{0x35,0xCB},
	{0x36,0xEB},
	{0x37,0xCE},
	{0x38,0xCF},
	{0x39,0xEF},
	{0x3A,0xD4},
	{0x3B,0xD9},
	{0x3C,0xF9},
	{0x3D,0xDB},
	{0x3E,0xAB},
	{0x3F,0xBB}	
};

const VO_U16 ExtendedPortGueseDanishCodeMap[][2] =
{
	{0x20,0xC3},
	{0x21,0xE3},
	{0x22,0xCD},
	{0x23,0xCC},
	{0x24,0xEC},
	{0x25,0xD2},
	{0x26,0xF2},
	{0x27,0xD5},
	{0x28,0xF5},
	{0x29,0x7B},
	{0x2A,0x7D},
	{0x2B,0x5C},
	{0x2C,0x5E},
	{0x2D,0x5F},
	{0x2E,0x7C},
	{0x2F,0x7E},
	{0x30,0xC4},
	{0x31,0xE4},
	{0x32,0xD6},
	{0x33,0xF6},
	{0x34,0xDF},
	{0x35,0xA5},
	{0x36,0xA4},
	{0x37,0xA6},///<use Broken bar replace Vertical bar,just as same as moto's example
	{0x38,0xC5},
	{0x39,0xE5},
	{0x3A,0xD8},
	{0x3B,0xF8},
	{0x3C,0x2308},
	{0x3D,0x2309},
	{0x3E,0x230A},
	{0x3F,0x230B}	
};


CCBaseWinCharData::CCBaseWinCharData(VO_U32 dwMaxLinCnt,VO_U32 dwMaxColumnCnt)
:pNext(NULL)
,m_CurRowNo(-1)
{
	m_CharData = new CCCharData(dwMaxLinCnt,dwMaxColumnCnt);
	m_TmpString = new VO_U16[dwMaxLinCnt * dwMaxColumnCnt];
	m_pInfoEntry = (pvoSubtitleInfoEntry)NewSubTitleEntry(Struct_Type_voSubtitleInfoEntry);
	m_pCurStrInfo = (voSubtitleStringInfo *)NewSubTitleEntry(Struct_Type_voSubtitleStringInfo);

	m_pInfoEntry->stSubtitleRectInfo.stRectBorderColor.nTransparency = 0;
	m_pInfoEntry->stSubtitleRectInfo.stRectFillColor.nTransparency = 0;
}

CCBaseWinCharData::~CCBaseWinCharData()
{
	SAFE_DELETE(m_CharData);
	SAFE_DELETE(m_TmpString);
	if (m_pInfoEntry)
	{
		DelSubTitleEntry(m_pInfoEntry,Struct_Type_voSubtitleInfoEntry);
	}
	if (m_pCurStrInfo)
	{
		DelSubTitleEntry(m_pCurStrInfo,Struct_Type_voSubtitleStringInfo);
	}	
}

VO_VOID	CCBaseWinCharData::Reset()
{
	m_CurRowNo = -1;
	m_CharData->ResetBuffer();
	InitializeStruct((VO_PTR)m_pCurStrInfo,Struct_Type_voSubtitleStringInfo);
	ResetStructEntry(m_pInfoEntry,Struct_Type_voSubtitleInfoEntry);
}

VO_VOID	CCBaseWinCharData::ResetDataInfo()
{
	m_CharData->ResetBuffer();
	InitializeStruct((VO_PTR)m_pCurStrInfo,Struct_Type_voSubtitleStringInfo);
	ResetStructEntry(m_pInfoEntry,Struct_Type_voSubtitleInfoEntry);
	m_pInfoEntry->stSubtitleRectInfo.stRectBorderColor.nTransparency = 0;
	m_pInfoEntry->stSubtitleRectInfo.stRectFillColor.nTransparency = 0;
}

VO_VOID	CCBaseWinCharData::ResetOutputInfo()
{
//	InitializeStruct((VO_PTR)m_pCurStrInfo,Struct_Type_voSubtitleStringInfo);
	ResetStructEntry(m_pInfoEntry,Struct_Type_voSubtitleInfoEntry);
	m_pInfoEntry->stSubtitleRectInfo.stRectBorderColor.nTransparency = 0;
	m_pInfoEntry->stSubtitleRectInfo.stRectFillColor.nTransparency = 0;
}

VO_VOID CCBaseWinCharData::ResetPopupDataInfo()
{
	InitializeStruct((VO_PTR)m_pCurStrInfo,Struct_Type_voSubtitleStringInfo);
	ResetStructEntry(m_pInfoEntry,Struct_Type_voSubtitleInfoEntry);
	m_pInfoEntry->stSubtitleRectInfo.stRectBorderColor.nTransparency = 0;
	m_pInfoEntry->stSubtitleRectInfo.stRectFillColor.nTransparency = 0;
}
VO_VOID CCBaseWinCharData::PushString(const VO_U16* pString, VO_U32 nStrLen)
{
	if (m_CurRowNo < 0)
	{
		return;
	}
	VO_U32 nCnt = 0;
	while (nCnt < nStrLen)
	{
		CharInfo TmpCharInfo;
		TmpCharInfo.character = pString[nCnt];
		TmpCharInfo.info = *m_pCurStrInfo;
		m_CharData->PushChar(&TmpCharInfo,1,m_CurRowNo);
		nCnt++;
	}
}

VO_VOID CCBaseWinCharData::PushStringNoPara(const VO_U16* pString, VO_U32 nStrLen)
{
	if (m_CurRowNo < 0)
	{
		return;
	}

	VO_U32 nCnt = 0;
	while (nCnt < nStrLen)
	{
		CharInfo TmpCharInfo;
		TmpCharInfo.character = pString[nCnt];
		TmpCharInfo.info = *m_pCurStrInfo;
		TmpCharInfo.info.stFontInfo.stFontColor.nTransparency = 0;
		TmpCharInfo.info.stCharEffect.EdgeColor.nTransparency = 0;
		TmpCharInfo.info.stCharEffect.EdgeType = 0;
		TmpCharInfo.info.stCharEffect.Italic = 0;
		TmpCharInfo.info.stCharEffect.Offset = 0;
		TmpCharInfo.info.stCharEffect.TextTag = 0;
		TmpCharInfo.info.stCharEffect.Underline = 0;
		m_CharData->PushChar(&TmpCharInfo,1,m_CurRowNo);
		nCnt++;
	}
}

VO_BOOL CCBaseWinCharData::IsRowNoValid()
{
	if (m_CurRowNo < 0 || m_CurRowNo > MAX_ROW_COUNT)
	{
		return VO_FALSE;
	}
	return VO_TRUE;
}

VO_VOID	CCBaseWinCharData::SetCurRowNo(VO_U32 dwRowNo)
{
	if (dwRowNo > MAX_ROW_COUNT)
	{
		return;
	}
	m_CurRowNo = dwRowNo;
}

VO_VOID CCBaseWinCharData::InitScreenInfo()
{
	voSubtitleRGBAColor stFillCol;
	stFillCol.nTransparency = 0;
	m_pInfoEntry->stSubtitleRectInfo.stRectFillColor = stFillCol;
	InitializeStruct((VO_PTR)m_pInfoEntry,Struct_Type_voSubtitleInfoEntry);
	InitializeStruct((VO_PTR)m_pCurStrInfo,Struct_Type_voSubtitleStringInfo);
}

VO_VOID CCBaseWinCharData::SetTextRowDescriptor(VO_U32 nWrap,VO_U32 nScrollDirection)
{
	m_pInfoEntry->stSubtitleDispInfo.stDispDescriptor.stWrap = nWrap;
	m_pInfoEntry->stSubtitleDispInfo.stDispDescriptor.nScrollDirection = nScrollDirection;
}

pvoSubtitleInfoEntry CCBaseWinCharData::GetInfoEntry()
{
	LineInfo * pData = m_CharData->GetRowDataInfo();

	VO_U32 nRow = 0,nMinRow = 0xFFFFFFFF,nMaxRow = 0,nMinCol = 0xFFFFFFFF,nMaxCol = 0,nStrLen = 0;
	pvoSubtitleTextRowInfo pTextRowInfoCur = m_pInfoEntry->stSubtitleDispInfo.pTextRowInfo;
	pvoSubtitleTextRowInfo pTmpRowInfo = pTextRowInfoCur;

	pvoSubtitleTextRowInfo pRowHead = m_pInfoEntry->stSubtitleDispInfo.pTextRowInfo;
	while(pRowHead)
	{
		ResetStructEntry(pRowHead,Struct_Type_voSubtitleTextRowInfo);
		pRowHead = pRowHead->pNext;
	}

	while(nRow < m_CharData->GetRowCnt())
	{
		if (!pData[nRow].m_nCharCntCurLine)
		{
			nRow++;
			continue;
		}

		if (nMinRow == 0xFFFFFFFF || nRow < nMinRow)
		{
			nMinRow = nRow;
			nMaxRow = nMinRow;
		}
		if (nRow > nMaxRow)
		{
			nMaxRow = nRow;
		}

		if (!pTextRowInfoCur)
		{
			pTextRowInfoCur = (voSubtitleTextRowInfo *)NewSubTitleEntry(Struct_Type_voSubtitleTextRowInfo);
			pvoSubtitleTextRowInfo pTmpListTail = m_pInfoEntry->stSubtitleDispInfo.pTextRowInfo;
			if (!pTmpListTail)
			{
				pTmpListTail = m_pInfoEntry->stSubtitleDispInfo.pTextRowInfo = pTextRowInfoCur;
			}
			else
			{
				while(pTmpListTail->pNext)
				{
					pTmpListTail = pTmpListTail->pNext;
				}
				pTmpListTail->pNext = pTextRowInfoCur;
			}
		}	
		pTmpRowInfo = pTextRowInfoCur;
		pTextRowInfoCur = pTextRowInfoCur->pNext;

		ResetStructEntry(pTmpRowInfo,Struct_Type_voSubtitleTextRowInfo);
		pTmpRowInfo->stTextRowDes = pData[nRow].m_stRowDesc;

		pvoSubtitleTextInfoEntry pTmpTextInfoEntry = pTmpRowInfo->pTextInfoEntry;
		pvoSubtitleTextInfoEntry pTmp = NULL;

		CharInfo* pRowCharInfo = pData[nRow].m_pCharOfLine;
		voSubtitleStringInfo stStrInfo;
		InitializeStruct(&stStrInfo,Struct_Type_voSubtitleStringInfo);

		if (nMinCol == 0xFFFFFFFF || pData[nRow].m_stRowDesc.stDataBox.stRect.nLeft < TranslateColNoToPos(nMinCol))  //using the leftest pos of databox for window box;task 19956
		{
			nMinCol = TranslatePosToColNo(pData[nRow].m_stRowDesc.stDataBox.stRect.nLeft);
			nMaxCol = nMinCol;
		}
		VO_U32 nCurRowRight = TranslatePosToColNo(pData[nRow].m_stRowDesc.stDataBox.stRect.nLeft) + (pData[nRow].m_nCharCntCurLine - 1);
		if (nCurRowRight > nMaxCol)
		{
			nMaxCol = nCurRowRight;
		}
		for(VO_U16 nCount = 0;nCount < pData[nRow].m_nCharCntCurLine;nCount++)
		{
			if (!nCount || memcmp(&stStrInfo,&pRowCharInfo[nCount].info,sizeof(voSubtitleStringInfo)))
			{		
				if(!pTmpTextInfoEntry)
				{
					pTmpTextInfoEntry = (pvoSubtitleTextInfoEntry)NewSubTitleEntry(Struct_Type_voSubtitleTextInfoEntry);
					pvoSubtitleTextInfoEntry pTmpInfoListTail = pTmpRowInfo->pTextInfoEntry;
					if (!pTmpInfoListTail)
					{
						pTmpInfoListTail = pTmpRowInfo->pTextInfoEntry = pTmpTextInfoEntry;
					}
					else
					{
						while(pTmpInfoListTail->pNext)
						{
							pTmpInfoListTail = pTmpInfoListTail->pNext;
						}
						pTmpInfoListTail->pNext = pTmpTextInfoEntry;
					}
					pTmp = pTmpTextInfoEntry;
					pTmpTextInfoEntry = pTmpTextInfoEntry->pNext;
				}
				else
				{
					pTmp = pTmpTextInfoEntry;
					pTmpTextInfoEntry = pTmpTextInfoEntry->pNext;
				}
				pTmp->pString = (VO_PBYTE)(m_TmpString + nStrLen);
				stStrInfo = pTmp->stStringInfo = pRowCharInfo[nCount].info;
			}
			m_TmpString[nStrLen++] = pRowCharInfo[nCount].character;
			pTmp->nSize += 2;

		}	
		nRow++;
	}
	if (nMinRow == 0xFFFFFFFF || nMinCol == 0xFFFFFFFF)
	{
		return NULL;
	}
	SetRectInfo(&m_pInfoEntry->stSubtitleRectInfo.stRect,nMinRow,nMaxRow,nMinCol,nMaxCol);
	return m_pInfoEntry;
}

CCCharParser::CCCharParser(VO_CAPTION_PARSER_INIT_INFO* pParam,VO_U32 dwMaxLinCnt,VO_U32 dwMaxColumnCnt)
:CvoBaseMemOpr(0)
,m_dwMaxRowCnt(dwMaxLinCnt)
,m_dwMaxColumnCnt(dwMaxColumnCnt)
,m_bNeedReset(VO_FALSE)
,m_pBaseWinData(NULL)
,m_bClearScreen(VO_FALSE)
{
	InitializeStruct(&m_stSubtitleInfo,Struct_Type_voSubtitleInfo);
	InitializeStruct(&m_stNullEntry,Struct_Type_voSubtitleInfoEntry);
	SetRectInfo(&m_stNullEntry.stSubtitleRectInfo.stRect,0,14,0,31);
	m_stNullEntry.stSubtitleRectInfo.stRectBorderColor.nTransparency = 0;
	m_stNullEntry.stSubtitleRectInfo.stRectFillColor.nTransparency = 0;
}

VO_VOID CCCharParser::Reset()
{
	InitializeStruct(&m_stSubtitleInfo,Struct_Type_voSubtitleInfo);
	CCBaseWinCharData * pBaseWin = m_pBaseWinData;
	while(pBaseWin)
	{
		pBaseWin->Reset();
		pBaseWin = pBaseWin->pNext;
	}
	m_bNeedReset = VO_FALSE;
}

VO_VOID CCCharParser::ResetDataInfo()
{
	if (m_bNeedReset)
	{
		CCBaseWinCharData * pBaseWin = m_pBaseWinData;
		while(pBaseWin)
		{
			pBaseWin->ResetDataInfo();
			pBaseWin = pBaseWin->pNext;
		}
		m_bNeedReset = VO_FALSE;
	}
	
}

CCCharParser::~CCCharParser()
{
#ifdef HTML_PACK
	SAFE_DELETE(m_HtmlPacker);
#endif
	CCBaseWinCharData *pList = m_pBaseWinData;
	CCBaseWinCharData *pTmp = pList;
	while(pList)
	{
		pList = pList->pNext;
		delete pTmp;
		pTmp = pList;
	}
	m_pBaseWinData = NULL;
}

VO_U32 CCCharParser::Process(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	return VO_ERR_PARSER_OK;
}

VO_U32 CCCharParser::GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer)
{
	pBuffer->pSubtitleInfo = &m_stSubtitleInfo;
	OutputStringInfo(&m_stSubtitleInfo);
	return VO_ERR_PARSER_OK;
}
VO_VOID CCCharParser::EndOfDisplay()
{
	
}

VO_BOOL CCCharParser::GetWindowInfoList(voSubtitleInfo * pSubtitleInfo)
{
	if (!pSubtitleInfo || !m_pBaseWinData)
	{
		return VO_FALSE;
	}
	pSubtitleInfo->pSubtitleEntry = m_pBaseWinData->GetInfoEntry();
	pvoSubtitleInfoEntry pTmpInfoEntry = pSubtitleInfo->pSubtitleEntry;
	CCBaseWinCharData * pTmpWin = m_pBaseWinData->pNext;
	while(pTmpWin)
	{
		if (!pTmpInfoEntry)
		{
			pTmpInfoEntry = pTmpWin->GetInfoEntry();
		}
		else
		{
			pTmpInfoEntry = pTmpInfoEntry->pNext = pTmpWin->GetInfoEntry();
		}
		pTmpWin = pTmpWin->pNext;
	}
	return VO_TRUE;
}

VO_VOID CCCharParser::OutputStringInfo(voSubtitleInfo * pInfo)
{
#if 0
	if (pInfo)
	{
		CDumper::WriteLog("---------OutputStringInfo being------------");
		CDumper::WriteLog2("nTimeStamp",pInfo->nTimeStamp);
		pvoSubtitleInfoEntry pTmpInfoEntry= pInfo->pSubtitleEntry;
		VO_U32 tmp0 = 1,tmp1 = 1,tmp2 = 1;;
		while (pTmpInfoEntry)
		{
			tmp1 = 1;
			CDumper::WriteLog2("----pSubtitleEntry----",(VO_U32)pInfo->pSubtitleEntry);

			CDumper::WriteLog2("---------voSubtitleInfoEntry %dth ------------",tmp0);
			CDumper::WriteLog2("Window nDuration",pInfo->pSubtitleEntry->nDuration);

			CDumper::WriteLog2("Window Rect nTop",pTmpInfoEntry->stSubtitleRectInfo.stRect.nTop);
			CDumper::WriteLog2("Window Rect nLeft",pTmpInfoEntry->stSubtitleRectInfo.stRect.nLeft);
			CDumper::WriteLog2("Window Rect nBottom",pTmpInfoEntry->stSubtitleRectInfo.stRect.nBottom);
			CDumper::WriteLog2("Window Rect nRight",pTmpInfoEntry->stSubtitleRectInfo.stRect.nRight);

			CDumper::WriteLog2("Window nRectBorderType",pTmpInfoEntry->stSubtitleRectInfo.nRectBorderType);

			CDumper::WriteLog2("Window BorderColor nRed",pTmpInfoEntry->stSubtitleRectInfo.stRectBorderColor.nRed);
			CDumper::WriteLog2("Window BorderColor nGreen",pTmpInfoEntry->stSubtitleRectInfo.stRectBorderColor.nGreen);
			CDumper::WriteLog2("Window BorderColor nBlue",pTmpInfoEntry->stSubtitleRectInfo.stRectBorderColor.nBlue);
			CDumper::WriteLog2("Window BorderColor nTransparency",pTmpInfoEntry->stSubtitleRectInfo.stRectBorderColor.nTransparency);

			CDumper::WriteLog2("Window FillColor nRed",pTmpInfoEntry->stSubtitleRectInfo.stRectFillColor.nRed);
			CDumper::WriteLog2("Window FillColor nGreen",pTmpInfoEntry->stSubtitleRectInfo.stRectFillColor.nGreen);
			CDumper::WriteLog2("Window FillColor nBlue",pTmpInfoEntry->stSubtitleRectInfo.stRectFillColor.nBlue);
			CDumper::WriteLog2("Window FillColor nTransparency",pTmpInfoEntry->stSubtitleRectInfo.stRectFillColor.nTransparency);

			CDumper::WriteLog2("Window nRectZOrder",pTmpInfoEntry->stSubtitleRectInfo.nRectZOrder);

			CDumper::WriteLog2("Window nEffectType",pTmpInfoEntry->stSubtitleRectInfo.stRectDisplayEffct.nEffectType);
			CDumper::WriteLog2("Window nEffectDirection",pTmpInfoEntry->stSubtitleRectInfo.stRectDisplayEffct.nEffectDirection);
			CDumper::WriteLog2("Window nEffectSpeed",pTmpInfoEntry->stSubtitleRectInfo.stRectDisplayEffct.nEffectSpeed);

			
			CDumper::WriteLog2("Window Text desp:stWrap",pTmpInfoEntry->stSubtitleDispInfo.stDispDescriptor.stWrap);
			CDumper::WriteLog2("Window Text desp:nScrollDirection",pTmpInfoEntry->stSubtitleDispInfo.stDispDescriptor.nScrollDirection);
			
			CDumper::WriteLog2("-----Window Text Info-----",(VO_U32)pTmpInfoEntry->stSubtitleDispInfo.pTextRowInfo);
			pvoSubtitleTextRowInfo pTmpTextRow = pTmpInfoEntry->stSubtitleDispInfo.pTextRowInfo;
			while (pTmpTextRow)
			{
				tmp2 = 1;
				CDumper::WriteLog2("---------voSubtitle TextRow %dth ------------",tmp1);
				CDumper::WriteLog2("Text Row Rect Pos nTop",pTmpTextRow->stTextRowDes.stDataBox.stRect.nTop);
				CDumper::WriteLog2("Text Row Rect Pos nLeft",pTmpTextRow->stTextRowDes.stDataBox.stRect.nLeft);
				CDumper::WriteLog2("Text Row Rect Pos nBottom",pTmpTextRow->stTextRowDes.stDataBox.stRect.nBottom);
				CDumper::WriteLog2("Text Row Rect Pos nRight",pTmpTextRow->stTextRowDes.stDataBox.stRect.nRight);


				CDumper::WriteLog2("Text Row Rect BorderType",pTmpTextRow->stTextRowDes.stDataBox.nRectBorderType);

				CDumper::WriteLog2("Text Row Rect BorderColor nRed",pTmpTextRow->stTextRowDes.stDataBox.stRectBorderColor.nRed);
				CDumper::WriteLog2("Text Row Rect BorderColor nGreen",pTmpTextRow->stTextRowDes.stDataBox.stRectBorderColor.nGreen);
				CDumper::WriteLog2("Text Row Rect BorderColor nBlue",pTmpTextRow->stTextRowDes.stDataBox.stRectBorderColor.nBlue);
				CDumper::WriteLog2("Text Row Rect BorderColor nTransparency",pTmpTextRow->stTextRowDes.stDataBox.stRectBorderColor.nTransparency);

				CDumper::WriteLog2("Text Row Rect FillColor nRed",pTmpTextRow->stTextRowDes.stDataBox.stRectFillColor.nRed);
				CDumper::WriteLog2("Text Row Rect FillColor nGreen",pTmpTextRow->stTextRowDes.stDataBox.stRectFillColor.nGreen);
				CDumper::WriteLog2("Text Row Rect FillColor nBlue",pTmpTextRow->stTextRowDes.stDataBox.stRectFillColor.nBlue);
				CDumper::WriteLog2("Text Row Rect FillColor nTransparency",pTmpTextRow->stTextRowDes.stDataBox.stRectFillColor.nTransparency);

				CDumper::WriteLog2("Text Row Rect ZOrder",pTmpTextRow->stTextRowDes.stDataBox.nRectZOrder);
				CDumper::WriteLog2("Text Row Rect nEffectType",pTmpTextRow->stTextRowDes.stDataBox.stRectDisplayEffct.nEffectType);
				CDumper::WriteLog2("Text Row Rect nEffectDirection",pTmpTextRow->stTextRowDes.stDataBox.stRectDisplayEffct.nEffectDirection);
				CDumper::WriteLog2("Text Row Rect nEffectSpeed",pTmpTextRow->stTextRowDes.stDataBox.stRectDisplayEffct.nEffectSpeed);

				CDumper::WriteLog2("Text Row String desp nHorizontalJustification",pTmpTextRow->stTextRowDes.nHorizontalJustification);
				CDumper::WriteLog2("Text Row String desp nVerticalJustification",pTmpTextRow->stTextRowDes.nVerticalJustification);
				CDumper::WriteLog2("Text Row String desp nPrintDirection",pTmpTextRow->stTextRowDes.nPrintDirection);


				CDumper::WriteLog2("-----Text Row InfoEnty----",(VO_U32)pTmpTextRow->pTextInfoEntry);

				pvoSubtitleTextInfoEntry pTmpTextInfoEntry = pTmpTextRow->pTextInfoEntry;
				while (pTmpTextInfoEntry)
				{
					CDumper::WriteLog2("---------voSubtitle Row %d Entry------------",tmp2);
					CDumper::WriteLog2("Font Size ",pTmpTextInfoEntry->stStringInfo.stFontInfo.FontSize);
					CDumper::WriteLog2("Font Style",pTmpTextInfoEntry->stStringInfo.stFontInfo.FontStyle);
					CDumper::WriteLog2("Font Color nRed",pTmpTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nRed);
					CDumper::WriteLog2("Font Color nGreen",pTmpTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nGreen);
					CDumper::WriteLog2("Font Color nBlue",pTmpTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nBlue);
					CDumper::WriteLog2("Font Color nTransparency",pTmpTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nTransparency);

					CDumper::WriteLog2("Font Italic",pTmpTextInfoEntry->stStringInfo.stCharEffect.Italic);
					CDumper::WriteLog2("Font Underline",pTmpTextInfoEntry->stStringInfo.stCharEffect.Underline);
					CDumper::WriteLog2("Font TextTag",pTmpTextInfoEntry->stStringInfo.stCharEffect.TextTag);
					CDumper::WriteLog2("Font Offset",pTmpTextInfoEntry->stStringInfo.stCharEffect.Offset);
					CDumper::WriteLog2("Font EdgeType",pTmpTextInfoEntry->stStringInfo.stCharEffect.EdgeType);

					CDumper::WriteLog2("Font EdgeColor nRed",pTmpTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nRed);
					CDumper::WriteLog2("Font EdgeColor nGreen",pTmpTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nGreen);
					CDumper::WriteLog2("Font EdgeColor nBlue",pTmpTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nBlue);
					CDumper::WriteLog2("Font EdgeColor nTransparency",pTmpTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nTransparency);

					CDumper::WriteLog2("<<<<<<<<String Info>>>>>>>>>",pTmpTextInfoEntry->nSize);
					CDumper::WriteLog2("String Count",pTmpTextInfoEntry->nSize);
					wchar_t stInfo[256];
					memcpy(stInfo,pTmpTextInfoEntry->pString,pTmpTextInfoEntry->nSize);
					stInfo[pTmpTextInfoEntry->nSize/2] = 0;
					CDumper::WriteLog("String --");
					CDumper::WriteLog3(stInfo);
					CDumper::WriteLog2("<<<<<<<<String Info>>>>>>>>>",pTmpTextInfoEntry->nSize);
					pTmpTextInfoEntry = pTmpTextInfoEntry->pNext;
					tmp2++;
				}

				pTmpTextRow = pTmpTextRow->pNext;
				tmp1++;
			}
			pTmpInfoEntry = pTmpInfoEntry->pNext;
			tmp0++;
		}
	}
	CDumper::WriteLog("---------OutputStringInfo End------------\r\n\r\n\r\n\r\n\r\n");
#endif
}
//=====================================================================
CC608CharParser::CC608CharParser(VO_CAPTION_PARSER_INIT_INFO* pParam,VO_U32 dwMaxLinCnt,VO_U32 dwMaxColumnCnt)
:CCCharParser(pParam,dwMaxLinCnt,dwMaxColumnCnt)
,m_nUserSelectedDataChannel(VO_CAPTION_CHANNEL_1)
,m_nCurDataChannel(VO_CAPTION_CHANNEL_UNKOWN)
,m_nDefaultMode(OperatingMode_Caption_Mode)
,m_nCurMode(OperatingMode_Unknow)
,m_nLastCtrlChar(0)
,m_nMaxLineCntForRollUp(2)
,m_PreStyle(Present_Style_Unknow)
,m_BaseLineNum(MAX_ROW_COUNT - 1)///<for roll-up,15 is default start line
//urgly walk round by Yi
,m_bEmptySend(VO_FALSE)
,m_bNeedSendEmpty(VO_FALSE)
//
{
	VOLOGE("CC608CharParser Enabled");
	m_pBaseWinData = new CCBaseWinCharData(dwMaxLinCnt,dwMaxColumnCnt);
}
CC608CharParser::~CC608CharParser()
{

}

VO_VOID CC608CharParser::Reset()
{
	m_nCurDataChannel = VO_CAPTION_CHANNEL_UNKOWN;
	m_nUserSelectedDataChannel = VO_CAPTION_CHANNEL_1;
	m_nCurMode = OperatingMode_Unknow;
	m_nDefaultMode = OperatingMode_Caption_Mode;
	m_nLastCtrlChar = 0;
	m_nMaxLineCntForRollUp = 2;
	m_PreStyle = Present_Style_Unknow;
	m_BaseLineNum = MAX_ROW_COUNT - 1;///<for roll-up,15 is default start line
	m_bEmptySend = VO_FALSE;
	m_bNeedSendEmpty = VO_FALSE;
	CCCharParser::Reset();
}

///<reserve the user setting
VO_VOID CC608CharParser::ReStart()
{
	m_nCurDataChannel = VO_CAPTION_CHANNEL_UNKOWN;
	m_nCurMode = OperatingMode_Unknow;
	m_nLastCtrlChar = 0;
	m_nMaxLineCntForRollUp = 2;
	m_PreStyle = Present_Style_Unknow;
	m_BaseLineNum = MAX_ROW_COUNT - 1;///<for roll-up,15 is default start line
	CCCharParser::Reset();
	m_stSubtitleInfo.pSubtitleEntry = &m_stNullEntry;
	m_bEmptySend = VO_FALSE;
	m_bNeedSendEmpty = VO_FALSE;
}

VO_U32 CC608CharParser::GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer)
{
//	VOLOGI("GetData 1:%d",countDolby);
	//urgly walk round by Yi
	if(m_bEmptySend)
	{
		if(m_stSubtitleInfo.pSubtitleEntry == &m_stNullEntry)
		{
			m_stSubtitleInfo.pSubtitleEntry = NULL;         
			m_bEmptySend = VO_FALSE; 
		}
		else
			m_bEmptySend = VO_FALSE; 
	}

	if (Present_Style_RollUp == m_PreStyle || Present_Style_PaintOn == m_PreStyle)
	{
		GetWindowInfoList(&m_stSubtitleInfo);
		if(m_bNeedSendEmpty)
		{
			if(m_stSubtitleInfo.pSubtitleEntry == NULL)
			{
				m_stSubtitleInfo.pSubtitleEntry = &m_stNullEntry;
				m_bNeedSendEmpty = VO_FALSE;
			}
			else
				m_bNeedSendEmpty = VO_FALSE;
		}
	}
	pBuffer->pSubtitleInfo = &m_stSubtitleInfo;

	if (m_stSubtitleInfo.pSubtitleEntry && m_stSubtitleInfo.pSubtitleEntry != &m_stNullEntry)
	{
		SetReset(VO_TRUE);
	}
#ifdef _WIN32
	VO_U32 size = 0;
	if (m_stSubtitleInfo.pSubtitleEntry)
	{
		size = GetSubtitleEntrySize(&m_stSubtitleInfo,Struct_Type_voSubtitleInfo) - 4;
		OutputStringInfo(&m_stSubtitleInfo);
	}
#endif
//	VOLOGI("GetData 2");
	if (!pBuffer->pSubtitleInfo->pSubtitleEntry)
	{
		return VO_ERR_PARSER_INPUT_BUFFER_SMALL;
	}

	//urgly walk round by Yi
	if( !m_bEmptySend  && m_stSubtitleInfo.pSubtitleEntry == &m_stNullEntry )
		m_bEmptySend = VO_TRUE;
	//

	return VO_ERR_PARSER_OK;
}
VO_BOOL CC608CharParser::ParserSpecialChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	for (VO_U8 nSpeCharCnt = 0;nSpeCharCnt < SPECIAL_CHAR_COUNT;nSpeCharCnt++)
	{
		if(pInputData[1] == SPECIALCHARCODEMAP[nSpeCharCnt][0])
		{
			m_pBaseWinData->PushString(&SPECIALCHARCODEMAP[nSpeCharCnt][1],1);
			break;
		}
	}
	return VO_TRUE;
}

VO_BOOL CC608CharParser::ParserControlChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (pInputData[1] < SPECIAL_CHARACTER_MID_ROW_CONTROL_EXTENDED_CHAR_SET_START
		|| pInputData[1] > SPECIAL_CHARACTER_MID_ROW_CONTROL_END)
	{
		VOLOGE("ClosedCaption Error:not supported ControlChar:%x",pInputData[1]);
		return VO_FALSE;
	}
	CCCharData *pDataBuf = m_pBaseWinData->GetCharBuffer();  
	switch (pInputData[1])
	{
	case 0x20:///<resume caption loading
		{
			ChangePreStyle(Present_Style_PopUp);
		}
		break;
	case 0x2F:///<end of caption
		{
			if (m_PreStyle == Present_Style_PopUp)
			{
				ReverseDisplay();
				pDataBuf->ResetBuffer();
			}
			ChangePreStyle(Present_Style_PopUp);
		//	m_PreStyle = Present_Style_PopUp;///<forcely entry into Present_Style_PopUp
			
		}
		break;
	case 0x29:///<resume direct captioning
		{
			ChangePreStyle(Present_Style_PaintOn);
		}
		break;
	case 0x2E:///<erase non-display memory
		{
			if (m_PreStyle == Present_Style_PopUp)
			{
				pDataBuf->ResetBuffer();
			}
			else
			{
				VOLOGE("ClosedCaption Error:not supported Erase Non-display memory")
			}
		}
		break;
	case 0x21:///<back space
		{
			CCCharData * pDataBuf = m_pBaseWinData->GetCharBuffer();
			if (m_pBaseWinData->IsRowNoValid() && pDataBuf && pDataBuf->GetCharCntByRowNo(m_pBaseWinData->GetCurRowNo()))
			{
				pDataBuf->MoveCursorToColumn(m_pBaseWinData->GetCurRowNo(),pDataBuf->GetCharPosByRowNo(m_pBaseWinData->GetCurRowNo()) - 1,VO_TRUE);
			}	
		}
		break;
	case 0x24:///<delete end of row
		break;
	case 0x28:///<flash on
		{
		}
		break;
	case 0x25:///<rull-up caption-2 rows 
	case 0x26:///<rull-up caption-3 rows 
	case 0x27:///<rull-up caption-4 rows 
		{
			ChangePreStyle(Present_Style_RollUp);
			m_nMaxLineCntForRollUp = pInputData[1] - 0x23;
			while ((VO_S32)(m_BaseLineNum - (m_nMaxLineCntForRollUp - 1)) < 0)
			{
				m_BaseLineNum++;
			}
		}
		break;
	case 0x2D:///<carriage return
		{
			if (m_PreStyle != Present_Style_RollUp)
			{
				return VO_FALSE;
			}
			if (m_pBaseWinData->IsRowNoValid() && pDataBuf->GetCharCntByRowNo(m_pBaseWinData->GetCurRowNo()))
			{
				LineNumberInc();///<line No.++ && earse the top line chars
			}
		}
		break;
	case 0x2C:///<erase display memory
		{
			if(Present_Style_RollUp == m_PreStyle || Present_Style_PaintOn == m_PreStyle)
			{
				m_pBaseWinData->ResetDataInfo();
				m_bNeedSendEmpty = VO_TRUE;
			}
			else
			{
			//	m_stSubtitleInfo.pSubtitleEntry = NULL;
				m_stSubtitleInfo.pSubtitleEntry = &m_stNullEntry;
			}
		}
		break;
	case 0x2A:///<text restart
	case 0x2B:///<resume text display
		{
		//	ChangePreStyle(Present_Style_PaintOn);
		//	VOLOGE("ClosedCaption Error:not supported Text Mode");
		}
		break;
	default:
		{
			VOLOGE("ClosedCaption Error:not supported ControlChar:%x",pInputData[1]);
		}
		break;
	}
	return VO_TRUE;
}

VO_BOOL CC608CharParser::ParserAttributeByte(VO_BYTE nInputData,voSubtitleStringInfo* pStringInfo,VO_BOOL BeMidRow)
{
	if(!pStringInfo)
	{
		VOLOGE("pStringInfo is NULL!");
		return VO_FALSE;
	}
	pStringInfo->stCharEffect.Underline = (VO_BOOL)(nInputData & 0x1);
	if ((nInputData >= 0x50 && nInputData <= 0x5F)
		|| (nInputData >= 0x70 && nInputData <= 0x7F))///<just parser the underline
	{
		return VO_TRUE;
	}
	if ((nInputData & 0xF) < 0xE)
	{
		pStringInfo->stCharEffect.Italic = VO_FALSE;
		TranslateColorToRGB((Color_Type)((nInputData & 0xF)/2+Color_Type_White),&pStringInfo->stFontInfo.stFontColor);
	}
	else
	{
		if (!BeMidRow)
		{
			TranslateColorToRGB(Color_Type_White,&pStringInfo->stFontInfo.stFontColor);
		}
		pStringInfo->stCharEffect.Italic = VO_TRUE;
	}
	return VO_TRUE;
}

VO_BOOL CC608CharParser::ParserMIDROWChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	VO_U16 UniChar = ASCII_SPACE_CHAR;
	m_pBaseWinData->PushStringNoPara(&UniChar,1);
	ParserAttributeByte(pInputData[1],m_pBaseWinData->GetStringInfo(),VO_TRUE);
	return VO_TRUE;
}

VO_BOOL CC608CharParser::ParserPreambleChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (pInputData[0] < 0x10 || pInputData[0] > 0x1F
		|| pInputData[1] < 0x40 || pInputData[1] > 0x7F)
	{
		VOLOGE("ClosedCaption Error:not supported PreambleChar %x:%x",pInputData[0],pInputData[1]);
		return VO_FALSE;
	}
	if ((pInputData[0] == 0x10 || pInputData[0] == 0x18)
		&& (pInputData[1] < 0x40 || pInputData[1] > 0x5F))
	{
		return VO_FALSE;
	}

	switch (pInputData[0])
	{
	case PREAMBLE_ADDRESS_ROW_11_SYMBOL_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_11_SYMBOL_DATA_CHANNEL_2:
		m_pBaseWinData->SetCurRowNo(11);///<line 11
		break;
	case SPECIAL_CHARACTER_AND_PREAMBLE_ADDRESS_ROW_1_2_SYMBOL_DATA_CHANNEL_1:
	case SPECIAL_CHARACTER_AND_PREAMBLE_ADDRESS_ROW_1_2_SYMBOL_DATA_CHANNEL_2:
		m_pBaseWinData->SetCurRowNo(1);///<line 1
		break;
	case PREAMBLE_ADDRESS_ROW_3_4_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_3_4_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_2:
		m_pBaseWinData->SetCurRowNo(3);///<line 3
		break;
	case PREAMBLE_ADDRESS_ROW_12_13_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_12_13_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_2:
		m_pBaseWinData->SetCurRowNo(12);
		break;
	case PREAMBLE_ADDRESS_ROW_14_15_AND_CONTROL_1_SYMBOL_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_14_15_AND_CONTROL_1_SYMBOL_DATA_CHANNEL_2:
		m_pBaseWinData->SetCurRowNo(14);
		break;
	case PREAMBLE_ADDRESS_ROW_5_6_SYMBOL_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_5_6_SYMBOL_DATA_CHANNEL_2:
		m_pBaseWinData->SetCurRowNo(5);
		break;
	case PREAMBLE_ADDRESS_ROW_7_8_SYMBOL_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_7_8_SYMBOL_DATA_CHANNEL_2:
		m_pBaseWinData->SetCurRowNo(7);
		break;
	case PREAMBLE_ADDRESS_ROW_9_10_AND_CONTROL_2_SYMBOL_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_9_10_AND_CONTROL_2_SYMBOL_DATA_CHANNEL_2:
		m_pBaseWinData->SetCurRowNo(9);
		break;
	default:
		break;
	}
	if (m_pBaseWinData->GetCurRowNo() != 11)
	{
		m_pBaseWinData->SetCurRowNo(m_pBaseWinData->GetCurRowNo() +((pInputData[1] > 0x5F)?1:0));
	}

	m_pBaseWinData->SetCurRowNo(m_pBaseWinData->GetCurRowNo() - 1);///<set the row number to fit the buffer pos

	if (m_PreStyle == Present_Style_RollUp)
	{
		if (m_pBaseWinData->GetCurRowNo() == 0)
		{
			m_pBaseWinData->SetCurRowNo(1);///<it should be more than 0
		}
		if (m_BaseLineNum != m_pBaseWinData->GetCurRowNo())
		{
			VO_U32 dwRow = m_pBaseWinData->GetCurRowNo();
			BaseLineChange(&dwRow);
			m_pBaseWinData->SetCurRowNo(dwRow);
			m_BaseLineNum = m_pBaseWinData->GetCurRowNo();
		}
	}
	CCCharData * pDataBuf = m_pBaseWinData->GetCharBuffer();
	pDataBuf->MoveCursorToColumn(m_pBaseWinData->GetCurRowNo(),0,VO_TRUE);///<set the column number

	VO_S32 count = 0;
	if (pInputData[1] > 0x4F && pInputData[1] < 0x60)
	{
		count = (pInputData[1]-0x50)/2*4;
	}
	else if (pInputData[1] > 0x6F && pInputData[1] < 0x80)
	{
		count = (pInputData[1]-0x70)/2*4;
	}
#if 0
	while (count-- > 0)///<reset the column number as indent
	{
		VO_U32 dwPosChar = pDataBuf->GetCharPosByRowNo(m_pBaseWinData->GetCurRowNo());
		VO_U32 dwCntChar = pDataBuf->GetCharCntByRowNo(m_pBaseWinData->GetCurRowNo());
		if (dwPosChar== dwCntChar)
		{
			VO_U16 UniChar = ASCII_SPACE_CHAR;
			m_pBaseWinData->PushString(&UniChar,1);///<take 0x20 as indent char
		//	PushCharToBuffer(&UniChar,1);///<take 0x20 as indent char
		}
		else if(dwPosChar < dwCntChar)
			pDataBuf->MoveCursorToColumn(m_pBaseWinData->GetCurRowNo(),dwPosChar+1);
		else
			VOLOGE("ClosedCaption Error: nCurCursorOfLine > m_nCharCntCurLine");
			
	}
#else
	if (count > 0)
	{
		LineInfo* pRowInfo = pDataBuf->GetRowDataInfo();
		VO_U32 nRowNum = m_pBaseWinData->GetCurRowNo();
		SetRectInfo(&pRowInfo[nRowNum].m_stRowDesc.stDataBox.stRect,0xFFFFFFFF,0xFFFFFFFF,count,0xFFFFFFFF);
	}
	
#endif

	if ((pInputData[1] > 0x3F && pInputData[1] < 0x50)
		||(pInputData[1] > 0x5F && pInputData[1] < 0x70)
		||(pInputData[1]%2))///< push color && underline && italic to buffer
	{
		ParserAttributeByte(pInputData[1],m_pBaseWinData->GetStringInfo(),VO_FALSE);
	}
	return VO_TRUE;
}

VO_BOOL CC608CharParser::ParserTabControlChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	///<now the cursor pos is the end of char buf,so we fullfill 0x20 to buffer
	if ((pInputData[0] != 0x17 && pInputData[0] != 0x1F) || pInputData[1] > 0x23 || pInputData[1] < 0x21)
	{
		return VO_FALSE;
	}
	CCCharData * pDataBuf = m_pBaseWinData->GetCharBuffer();

	VO_U32 nCount = pInputData[1] - CONTROL_CHARACTER_START;
	
	if (VO_FALSE == m_pBaseWinData->IsRowNoValid())
	{
		return VO_FALSE;
	}
	VO_U32 dwPosChar = pDataBuf->GetCharPosByRowNo(m_pBaseWinData->GetCurRowNo());
	VO_U32 dwCntChar = pDataBuf->GetCharCntByRowNo(m_pBaseWinData->GetCurRowNo());
	
	if (dwCntChar == 0)
	{
		LineInfo* pRowInfo = pDataBuf->GetRowDataInfo();
		VO_U32 nRowNum = m_pBaseWinData->GetCurRowNo();

		nCount += TranslatePosToColNo(pRowInfo[nRowNum].m_stRowDesc.stDataBox.stRect.nLeft);

		SetRectInfo(&pRowInfo[nRowNum].m_stRowDesc.stDataBox.stRect,0xFFFFFFFF,0xFFFFFFFF,nCount,0xFFFFFFFF);
		return VO_TRUE;
	}
	while(nCount-- > 0)
	{
		dwPosChar = pDataBuf->GetCharPosByRowNo(m_pBaseWinData->GetCurRowNo());
		dwCntChar = pDataBuf->GetCharCntByRowNo(m_pBaseWinData->GetCurRowNo());

		if (dwPosChar == dwCntChar)
		{
			VO_U16 UniChar = ASCII_SPACE_CHAR;
			m_pBaseWinData->PushStringNoPara(&UniChar,1);///<take 0x20 as indent char
		}
		else if(dwPosChar < dwCntChar)
			pDataBuf->MoveCursorToColumn(m_pBaseWinData->GetCurRowNo(),dwPosChar+1);
		else
			VOLOGE("ClosedCaption Error: nCurCursorOfLine > m_nCharCntCurLine");
	}
	
	return VO_TRUE;
}

VO_BOOL CC608CharParser::ParserStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	VO_U16 nUnicode = 0;
	for (VO_U32 Cnt = 0;Cnt < cbInputData;Cnt++)
	{
		if (MapASCIICharToUniCode(pInputData[Cnt],nUnicode))
		{
			m_pBaseWinData->PushString(&nUnicode,1);
		}
	}
	return VO_TRUE;
}
VO_BOOL CC608CharParser::ParserNonStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (NULL == pInputData)
	{
		return VO_FALSE;
	}
	VO_BOOL ret = VO_FALSE;
//	VOLOGI("Ferry Non Standchar is %2x,%2x--%c,%c",pInputData[0],pInputData[1],pInputData[0],pInputData[1]);
	switch (pInputData[0])
	{
	case SPECIAL_CHARACTER_AND_PREAMBLE_ADDRESS_ROW_1_2_SYMBOL_DATA_CHANNEL_1:
	case SPECIAL_CHARACTER_AND_PREAMBLE_ADDRESS_ROW_1_2_SYMBOL_DATA_CHANNEL_2:
		{
			if ((pInputData[1] >= SPECIAL_CHARACTER_MID_ROW_CONTROL_EXTENDED_CHAR_SET_START )
				&& (pInputData[1] <= SPECIAL_CHARACTER_MID_ROW_CONTROL_END))
			{
				ret = ParserMIDROWChar(pInputData,cbInputData);
			}
			else if ((pInputData[1] >= SPECIAL_CHARACTER_SPECIAL_CODE_START )
				&& (pInputData[1] <= SPECIAL_CHARACTER_SPECIAL_CODE_EXTENDED_CHAR_SET_END))
			{
				ret = ParserSpecialChar(pInputData,cbInputData);
			}
			else
			{
				ret = ParserPreambleChar(pInputData,cbInputData);
			}
		}
		break;
	case PREAMBLE_ADDRESS_ROW_3_4_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_3_4_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_2:
	case PREAMBLE_ADDRESS_ROW_12_13_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_12_13_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_2:
		{
			if ((pInputData[1] >= SPECIAL_CHARACTER_MID_ROW_CONTROL_EXTENDED_CHAR_SET_START )
				&& (pInputData[1] <= SPECIAL_CHARACTER_SPECIAL_CODE_EXTENDED_CHAR_SET_END))
			{
				ret = ParserExtendedChar(pInputData,cbInputData);
			}
			else
			{
				ret = ParserPreambleChar(pInputData,cbInputData);
			}
		}
		break;
	case PREAMBLE_ADDRESS_ROW_14_15_AND_CONTROL_1_SYMBOL_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_14_15_AND_CONTROL_1_SYMBOL_DATA_CHANNEL_2:
	case PREAMBLE_ADDRESS_ROW_5_6_SYMBOL_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_5_6_SYMBOL_DATA_CHANNEL_2:
		{
			if ((pInputData[1] >= SPECIAL_CHARACTER_MID_ROW_CONTROL_EXTENDED_CHAR_SET_START )
				&& (pInputData[1] <= SPECIAL_CHARACTER_MID_ROW_CONTROL_END))
			{
				ret = ParserControlChar(pInputData,cbInputData);
			}
			else
			{
				ret = ParserPreambleChar(pInputData,cbInputData);
			}
		}
		break;
	case PREAMBLE_ADDRESS_ROW_9_10_AND_CONTROL_2_SYMBOL_DATA_CHANNEL_1:
	case PREAMBLE_ADDRESS_ROW_9_10_AND_CONTROL_2_SYMBOL_DATA_CHANNEL_2:
		{
			if ((pInputData[1] >= SPECIAL_CHARACTER_MID_ROW_CONTROL_EXTENDED_CHAR_SET_START+1 ) 
				&& (pInputData[1] <= SPECIAL_CHARACTER_MID_ROW_CONTROL_EXTENDED_CHAR_SET_START+3))
			{
				ret = ParserTabControlChar(pInputData,cbInputData);
			}
			else
			{
				ret = ParserPreambleChar(pInputData,cbInputData);
			}
		}
		break;
	default:
		{
			ret = ParserPreambleChar(pInputData,cbInputData);
		}
		break;
	}
	return ret;
}

VO_VOID CC608CharParser::SwitchUserSelectedDataChannel(VO_BYTE nChanellData)
{
	if (m_nUserSelectedDataChannel != nChanellData)
	{
		Reset();
		m_nUserSelectedDataChannel = (VO_CAPTION_CHANNEL)nChanellData;
	}
	
}

VO_VOID CC608CharParser::SetCurrentDataChannel(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (pInputData[0] > 0x0 && pInputData[0] <= 0xF)
	{
		m_nCurMode = OperatingMode_XDS_Mode;
	}
	else if (pInputData[0] >= 0x10 && pInputData[0] <= 0x1F)
	{
		m_nCurMode = OperatingMode_Caption_Mode;
	}

	if (pInputData[1] < 0x20 || pInputData[1] > 0x2F)
	{
		return;
	}
	switch (pInputData[0])
	{
	case PREAMBLE_ADDRESS_ROW_14_15_AND_CONTROL_1_SYMBOL_DATA_CHANNEL_1:
		m_nCurDataChannel = VO_CAPTION_CHANNEL_1;
		break;
	case PREAMBLE_ADDRESS_ROW_5_6_SYMBOL_DATA_CHANNEL_1:
		m_nCurDataChannel = VO_CAPTION_CHANNEL_3;
		break;
	case PREAMBLE_ADDRESS_ROW_14_15_AND_CONTROL_1_SYMBOL_DATA_CHANNEL_2:
		m_nCurDataChannel = VO_CAPTION_CHANNEL_2;
		break;
	case PREAMBLE_ADDRESS_ROW_5_6_SYMBOL_DATA_CHANNEL_2:
		m_nCurDataChannel = VO_CAPTION_CHANNEL_4;
		break;
	default:
		break;
	}
}
VO_U32 CC608CharParser::Process(VO_PBYTE pInputData, VO_U32 cbInputData)
{
//	VOLOGI("Process 1");

	ResetDataInfo();
	
	if (pInputData == NULL || cbInputData < 2)
	{
		return VO_ERR_PARSER_FAIL;
	}
	
	VO_U32 nInputData = cbInputData/2 * 2;///<there must be multiple of char pair
	VO_PBYTE pInData = pInputData;
	while (nInputData)
	{
		///<remove the paity bit
		pInData[0] &= 0x7f;
		pInData[1] &= 0x7f;
		VOLOGR("Char %x:%x",pInData[0],pInData[1]);
		if (m_nLastCtrlChar == (*(VO_U16*)pInData))///< duplicate control char
		{
			m_nLastCtrlChar = 0;
			return VO_TRUE;
		}
		else
			m_nLastCtrlChar = 0;


		if (pInData[0] >= STANDARD_CHARACTER_START 
			&& pInData[0] <= STANDARD_CHARACTER_END)///<handle the standard ascII char(0x20~0x7F)
		{
			if (m_nCurMode == m_nDefaultMode && m_nCurDataChannel == m_nUserSelectedDataChannel)
			{
			//	VOLOGI("Ferry Standchar is %d,%d--%c,%c",pInData[0],pInData[1],pInData[0],pInData[1]);
				ParserStandardChar(pInData,2);
			}
		}
		else if ((pInData[0] >= PREAMBLE_ADDRESS_ROW_11_SYMBOL_DATA_CHANNEL_1 ) ///<preamble address && mid-row && miscellaneous control char
			&& (pInData[0] <= PREAMBLE_ADDRESS_ROW_9_10_AND_CONTROL_2_SYMBOL_DATA_CHANNEL_2) )
		{
			SetLastCtrlChar(pInputData,cbInputData);///<it is a common practice to double all two-byte commands that are starting with 10~1F(conversion608.pdf) 
			SetCurrentDataChannel(pInData,2);
			if (m_nCurMode == m_nDefaultMode && m_nCurDataChannel == m_nUserSelectedDataChannel)
			{
				ParserNonStandardChar(pInData,2);
			//	m_nLastCtrlChar = (*(VO_U16*)pInData);
			}
		}
		else
		{
			///<not-support char 
			SetCurrentDataChannel(pInputData,2);
			if (pInData[0] || pInData[1])
			{
				VOLOGE("ClosedCaption Error:not supported Char %x:%x",pInData[0],pInData[1]);
			}	
		}

		nInputData -= 2;
		pInData += 2;
	}
//	VOLOGI("Process 2");
	return VO_ERR_PARSER_OK;
}

VO_BOOL CC608CharParser::MapASCIICharToUniCode(VO_BYTE nInputData, VO_U16 &nOutData)
{
	if (nInputData < STANDARD_CHARACTER_START || nInputData > STANDARD_CHARACTER_END)
	{
		return VO_FALSE;
	}
	switch(nInputData)
	{
	case 0x2A:
		nOutData = 0xE1;
		break;
	case 0x5C:
		nOutData = 0xE9;
		break;
	case 0x5E:
		nOutData = 0xED;
		break;
	case 0x5F:
		nOutData = 0xF3;
		break;
	case 0x60:
		nOutData = 0xFA;
		break;
	case 0x7B:
		nOutData = 0xE7;
		break;
	case 0x7C:
		nOutData = 0xF7;
		break;
	case 0x7D:
		nOutData = 0xD1;
		break;
	case 0x7E:
		nOutData = 0xF1;
		break;
	case 0x7F:
		nOutData = 0x2588;///<solid block
		break;
	default:
		nOutData = (VO_U16)nInputData;
		break;
	}
	return VO_TRUE;
}

VO_BOOL CC608CharParser::ParserExtendedChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	CCCharData * pDataBuf = m_pBaseWinData->GetCharBuffer();
	if (m_pBaseWinData->IsRowNoValid() && pDataBuf && pDataBuf->GetCharCntByRowNo(m_pBaseWinData->GetCurRowNo()))
	{
		pDataBuf->MoveCursorToColumn(m_pBaseWinData->GetCurRowNo(),pDataBuf->GetCharPosByRowNo(m_pBaseWinData->GetCurRowNo())  - 1,VO_TRUE);
	}	
	if (pInputData[0] == 0x12 || pInputData[0] == 0x1A)
	{
		if (pInputData[1] < 0x20 || pInputData[1] > 0x3F)
		{
			VOLOGE("ClosedCaption Error:error special char:%d",pInputData[1]);
			return VO_TRUE;
		}
		for (VO_U8 nExtCharCnt = 0;nExtCharCnt < EXTENDED_CHAR_COUNT;nExtCharCnt++)
		{
			if(pInputData[1] == ExtendedSpanishFrenchCodeMap[nExtCharCnt][0])
			{
				m_pBaseWinData->PushString(&ExtendedSpanishFrenchCodeMap[nExtCharCnt][1],1);
				break;
			}
		}
			}
	else if (pInputData[0] == 0x13 || pInputData[0] == 0x1B)
	{
		if (pInputData[1] < 0x20 || pInputData[1] > 0x3F)
		{
			VOLOGE("ClosedCaption Error:error special char:%d",pInputData[1]);
			return VO_TRUE;
		}
		for (VO_U8 nExtCharCnt = 0;nExtCharCnt < EXTENDED_CHAR_COUNT;nExtCharCnt++)
		{
			if(pInputData[1] == ExtendedPortGueseDanishCodeMap[nExtCharCnt][0])
			{
				m_pBaseWinData->PushString(&ExtendedPortGueseDanishCodeMap[nExtCharCnt][1],1);
				break;
			}
		}
	}
	return VO_TRUE;
}

VO_VOID CC608CharParser::SetLastCtrlChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (pInputData == NULL || cbInputData < 2)
	{
		m_nLastCtrlChar = 0;
		return;
	}
	m_nLastCtrlChar = (*(VO_U16*)pInputData);
}

///<just for roll-up && paint on style
VO_VOID CC608CharParser::LineNumberInc()
{
	if (m_PreStyle != Present_Style_RollUp)
	{
		VOLOGE("ClosedCaption Error:Not RollUp style");
		return;
	}

	if (m_nMaxLineCntForRollUp <= 0 || m_nMaxLineCntForRollUp > 4)
	{
		VOLOGE("ClosedCaption Error:m_nMaxLineCntForRollUp error:%d",m_nMaxLineCntForRollUp);
	}
	if (m_PreStyle != Present_Style_RollUp)
	{
		return;
	}
	VO_S32 iOldStartLine = 0;
	VO_S32 iNewStartLine = m_BaseLineNum - (m_nMaxLineCntForRollUp - 1);
	while (iNewStartLine < 0)
	{
		VOLOGE("ClosedCaption Error:wrong m_BaseLineNum,revise it");
		m_BaseLineNum++;///<for display character,we revise the base line number
		iNewStartLine++;
	}

	CCCharData *pDataBuf = m_pBaseWinData->GetCharBuffer();
	for (VO_S32 iLine = 3;iLine >= 0;iLine--)
	{
		iOldStartLine = m_BaseLineNum - iLine;
		if (iOldStartLine > 0 && pDataBuf->GetCharCntByRowNo(iOldStartLine))
		{
			if (iOldStartLine - 1>= iNewStartLine)
			{
				pDataBuf->CopyChar(iOldStartLine,iOldStartLine - 1);
			}
			pDataBuf->MoveCursorToColumn(iOldStartLine,0,VO_TRUE);
		}
	}
}

VO_VOID CC608CharParser::BaseLineChange(VO_U32 *pNewBaseLine)
{
	if (m_PreStyle != Present_Style_RollUp || *pNewBaseLine >= MAX_ROW_COUNT)
	{
		VOLOGE("ClosedCaption Error:nNewBaseLine is wrong");
		return;
	}

	VO_S32 iOldStartLine = 0;
	VO_S32 iNewStartLine = *pNewBaseLine - (m_nMaxLineCntForRollUp - 1);
	while (iNewStartLine < 0)
	{
		VOLOGE("ClosedCaption Error:wrong m_nMaxLineCntForRollUp,revise it");
		(*pNewBaseLine)++;
		iNewStartLine ++;
	}
	VO_S32 iLine = 3;
	CCCharData *pDataBuf = m_pBaseWinData->GetCharBuffer();
	if (!pDataBuf)
	{
		return;
	}

	while (iLine >= 0)
	{
		iOldStartLine = m_BaseLineNum - iLine;
		if (iOldStartLine >= 0 && pDataBuf->GetCharCntByRowNo(iOldStartLine))
		{
			if (iLine <= m_nMaxLineCntForRollUp - 1)
			{
				break;
			}
			else
			{
				pDataBuf->MoveCursorToColumn(iOldStartLine,0,VO_TRUE);
			}
		}	
		iLine--;
	}
	if (iLine < 0)
	{
		return ;
	}
	VO_S32 nStartLineCpy = 0;
	if (m_BaseLineNum > *pNewBaseLine)
	{
		nStartLineCpy = iLine;
		while (nStartLineCpy >= 0)
		{
			if (*pNewBaseLine - nStartLineCpy >= 0 && m_BaseLineNum - nStartLineCpy >= 0)
			{
				pDataBuf->CopyChar(m_BaseLineNum - nStartLineCpy,*pNewBaseLine - nStartLineCpy);
			}	
			else
				VOLOGE("ClosedCaption Error:Error nNewBaseLine");

			nStartLineCpy--;
		}	
	}
	else
	{
		nStartLineCpy = 0;
		while (nStartLineCpy < iLine)
		{
			if (*pNewBaseLine - nStartLineCpy >= 0 && m_BaseLineNum - nStartLineCpy >= 0)
			{
				pDataBuf->CopyChar(m_BaseLineNum - nStartLineCpy,*pNewBaseLine - nStartLineCpy);
			}	
			else
				VOLOGE("ClosedCaption Error:Error nNewBaseLine");
			nStartLineCpy++;
		}	
	}
}

VO_VOID CC608CharParser::ReverseDisplay()
{
	GetWindowInfoList(&m_stSubtitleInfo);
}

VO_BOOL CC608CharParser::ChangePreStyle(Present_Style eStyle)
{
	if (eStyle == m_PreStyle)
	{
		return VO_TRUE;
	}

	if (m_PreStyle != Present_Style_RollUp)///<roll-up style display will be not affacted by pop-up or painton 
	{
		m_stSubtitleInfo.pSubtitleEntry = NULL;
	}
	m_pBaseWinData->ResetDataInfo();
		
	m_nMaxLineCntForRollUp = 2;
	m_PreStyle = eStyle;
	m_BaseLineNum = MAX_ROW_COUNT - 1;///<for roll-up,15 is default start line
	return VO_TRUE;
}

VO_VOID CC608CharParser::ResetDataInfo()
{
	if (m_bNeedReset)
	{
		if (m_PreStyle == Present_Style_RollUp || Present_Style_PaintOn == m_PreStyle)
		{
			m_pBaseWinData->ResetOutputInfo();
		}
		else if(m_PreStyle == Present_Style_PopUp)
		{
			m_pBaseWinData->ResetPopupDataInfo();
			m_stSubtitleInfo.pSubtitleEntry = NULL;
		}
		else
		{
			m_pBaseWinData->ResetDataInfo();
			m_stSubtitleInfo.pSubtitleEntry = NULL;
		}
		
		m_bNeedReset = VO_FALSE;
	}
}