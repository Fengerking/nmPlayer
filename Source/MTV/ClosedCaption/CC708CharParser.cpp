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
* @file CC708CharParser.cpp
* CEA-708 character parser
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/



#include "CC708CharParser.h"
#include "voLog.h"
#include "CDumper.h"
#include "readutil.h"
#ifdef HTML_PACK
#include "CHTMLPacker.h"
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
VO_U32 TranslateColor(VO_U8 ValueRed,VO_U8 ValueGreen,VO_U8 ValueBlue)
{
	///<mini support 8 colors
	VO_U32 ColorValue = 0;
	ValueRed &= 0x2;
	ColorValue = ValueRed << 4;

	ValueGreen &= 0x2;
	ColorValue |= (ValueGreen << 2);

	ValueBlue &= 0x2;
	ColorValue |= ValueBlue;

	switch (ColorValue)
	{
	case 0x0:
		ColorValue = Color_Type_Black;
		break;
	case 0x2:
		ColorValue = Color_Type_Blue;
		break;
	case 0x8:
		ColorValue = Color_Type_Green;
		break;
	case 0x20:
		ColorValue = Color_Type_Red;
		break;
	case 0x2A:
		ColorValue = Color_Type_White;
		break;
	case 0x28:
		ColorValue = Color_Type_Yellow;
		break;
	case 0x22:
		ColorValue = Color_Type_Magenta;
		break;
	case 0x0A:
		ColorValue = Color_Type_Cyan;
		break;
	default:
		ColorValue = Color_Type_White;
		break;
	}

	return ColorValue;

}
#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

const VO_U16 ExtendedG2G3CodeMap[][2] =
{
	{0x20,0x20},
	{0x21,0x20},
	{0x25,0x2026},///<Ellipsis
	{0x2A,0x0160},///<
	{0x2C,0x0152},///<
	{0x30,0x2588},///<solid block
	{0x31,0x2018},
	{0x32,0x2019},
	{0x33,0x201C},
	{0x34,0x201D},
	{0x35,0x2022},
	{0x39,0x2122},
	{0x3A,0x0161},
	{0x3C,0x0153},
	{0x3D,0x2120},
	{0x3F,0x0178},
	{0x76,0x215B},
	{0x77,0x215C},
	{0x78,0x215D},
	{0x79,0x215E},
	{0x7A,0x2014},
	{0x7B,0x2309},
	{0x7C,0x230A},
	{0x7D,0x2014},
	{0x7E,0x230B},
	{0x7F,0x2308},
	{0xA0,0x201D}
};

VO_U8 PresetWinAttri[][4] = 
{
	{0x00,0x00,0x0C,0x00},	
	{0xC0,0x00,0x0C,0x00},
	{0x00,0x00,0x0E,0x00},
	{0x00,0x00,0x4C,0x00},
	{0xC0,0x00,0x4C,0x00},
	{0x00,0x00,0x4E,0x00},
	{0x00,0x00,0x24,0x00},
};

VO_U8 PresetPenAttri[][2] = 
{
	{0x05,0x00},	
	{0x05,0x01},
	{0x05,0x02},
	{0x05,0x03},
	{0x05,0x04},
	{0x05,0x1B},
	{0x05,0x1C},
};

VO_U8 PresetPenColor[][3] = 
{
	{0x2A,0x00,0x00},	
	{0x2A,0x00,0x00},
	{0x2A,0x00,0x00},
	{0x2A,0x00,0x00},
	{0x2A,0x00,0x00},
	{0x2A,0x30,0x00},
	{0x2A,0x30,0x00},
};

//=====================================================================
CC708CharParser::CC708CharParser(VO_CAPTION_PARSER_INIT_INFO* pParam,VO_U32 dwMaxLinCnt,VO_U32 dwMaxColumnCnt)
:CCCharParser(pParam,dwMaxLinCnt,dwMaxColumnCnt)
,m_CurWindowID(-1)
,m_dwWindIDSetting(-1)
{
	VOLOGE("CC708CharParser Enabled");
	m_pBaseWinData = m_WinParaTail = new CCWinCharData(m_dwMaxRowCnt,m_dwMaxColumnCnt);
}
CC708CharParser::~CC708CharParser()
{
	CCWinCharData *pList = (CCWinCharData *)m_pBaseWinData;
	CCWinCharData *pTmp = pList;
	while(pList)
	{
		pList = (CCWinCharData *)pList->pNext;
		delete pTmp;
		pTmp = pList; 
	}
	m_pBaseWinData = NULL;
}

VO_VOID CC708CharParser::ReStart()
{
	m_CurWindowID = -1;
	m_dwWindIDSetting = -1;
	m_WinParaTail = NULL;
	m_bClearScreen = VO_TRUE;

	CCCharParser::Reset();
}
VO_VOID CC708CharParser::PushCharToBuffer(const VO_U16* pInputData, VO_U32 cbInputData)
{
	CCWinCharData *pWind = FindWindByID(m_CurWindowID);
	if (!pWind)
	{
		return;
	}
	pWind->PushString(pInputData,cbInputData);
	pWind->UpdateWindowInfo();
}

VO_BOOL CC708CharParser::ParserStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData,VO_BOOL beExtCode)
{
	VO_U16 nUniCode = *pInputData;
	if (beExtCode)
	{
		VO_U32 nSpeCharCnt = 0;
		for (;nSpeCharCnt < COUNT_G2G3_CHARACTER;nSpeCharCnt++)
		{
			if(nUniCode == ExtendedG2G3CodeMap[nSpeCharCnt][0])
			{
				nUniCode = ExtendedG2G3CodeMap[nSpeCharCnt][1];
				break;
			}
		}
		if (COUNT_G2G3_CHARACTER == nSpeCharCnt)
		{
			nUniCode = 0x5F;
		}
	}
	else if (nUniCode == 0x7F)
	{
		nUniCode = 0x266A;
	}
	PushCharToBuffer(&nUniCode,1);
	return VO_TRUE;
}

VO_U32 CC708CharParser::ParserC0Code(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	VO_U32 ret = 1;
	if ((*pInputData) >= 0x10 && (*pInputData) <= 0x17)
		ret = 2;
	else if((*pInputData) >= 0x18 && (*pInputData) <= 0x1F)
		ret = 3;

	if (cbInputData < ret)
	{
		VOLOGE("ClosedCaption Error:ParserC0Code cbInputData < ret:%d:%d",(int)cbInputData,(int)ret);
		return cbInputData;
	}
	CCWinCharData * pWind = FindWindByID(m_CurWindowID);
	if (!pWind)
	{
		return ret;
	}
	pWind->UpdateWindowInfo();

	CCCharData *pData = pWind->GetCharBuffer();
	if (!pData)
	{
		return ret;
	}
	switch (*pInputData)
	{
	case 0x3:///<ETX
		{

		}
		break;

	case 0x8:///<BS
		{
			if (pWind->IsRowNoValid())
			{
				VO_U32 dwColumnNo = pData->GetCharPosByRowNo(pWind->GetCurRowNo());
				if (dwColumnNo > 0)
				{
					if (pWind)
						pData->MoveCursorToColumn(pWind->GetCurRowNo(),dwColumnNo - 1,VO_TRUE);
				}
			}
		}
		break;

	case 0xC:///<FF
		{
			if (pWind)
			{
				(*pWind).Init();
			}
		}
		break;

	case 0xD:///<CR
		{
			VO_U32 nWindowTopNo = TranslatePosToRowNo(pWind->m_pInfoEntry->stSubtitleRectInfo.stRect.nTop);
#if 0		///< 
			if (pWind->GetCurRowNo()+1 < nWindowTopNo + pWind->rc)
			{
				pWind->SetCurRowNo(pWind->GetCurRowNo()+1);
			}
			else
#endif
			{
				///<do roll-up action
				VO_U32 iRow = 1;
				
				nWindowTopNo += 1;
				if (pWind->IsRowNoValid())
				{
					while(nWindowTopNo  <= pWind->GetCurRowNo())
					{
						pData->CopyChar(nWindowTopNo,nWindowTopNo - 1);
						nWindowTopNo++;
					}
				}
			}
		}
		break;
	case 0xE:///<HCR
		{
			if (pWind && pWind->IsRowNoValid())
				pData->MoveCursorToColumn(pWind->GetCurRowNo(),0,VO_TRUE);
		}
		break;
	case 0x10:
		{
			VOLOGE("ClosedCaption Error:EXT1 should not exist here");
		}
		break;
	case 0x18:
		{
			VOLOGE("ClosedCaption Error:not supported P16");
		}
		break;
	default:
		break;
	}
	return ret;
}

VO_U32 CC708CharParser::ParserC1Code(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	VO_U32 ret = 0;
	if (*pInputData >= 0x98 &&  *pInputData <= 0x9F)///<DF0~DF7
	{
		CCWinCharData *pList = (CCWinCharData *)m_pBaseWinData;
		SetCurWindow(*pInputData - 0x98);
		while(pList)
		{
			if (pList->GetWinID() == -1 || pList->GetWinID() == (*pInputData - 0x98))
			{
				VO_BOOL beCreated = (pList->GetWinID() == -1)?VO_TRUE:VO_FALSE;
				if (beCreated)
				{
					pList->Init();
				}
				ret = pList->Load(pInputData,cbInputData,beCreated);
				break;
			}
			pList = (CCWinCharData *)pList->pNext;
		}
		if (!pList)
		{
			CCWinCharData *pNew = new CCWinCharData(m_dwMaxRowCnt,m_dwMaxColumnCnt);
			pNew->Init();
			ret = pNew->Load(pInputData,cbInputData);
			CCWinCharData *pTmp = (CCWinCharData *)m_pBaseWinData;
			while(pTmp->pNext)
			{
				pTmp = (CCWinCharData *)pTmp->pNext;
			}
			pTmp->pNext = pNew;
		}
	}
	else if (*pInputData >= 0x80 &&  *pInputData <= 0x87)///<CW0~CW7
	{
		SetCurWindow((*pInputData)- 0x80);
		ret = 1;
	}
	else
	{
		switch (*pInputData)
		{
		case 0x88:///<CLW
			{
				VO_U32 dwWinID = 0,bitValue = 1;
				if (cbInputData < 2)
				{
					VOLOGE("ClosedCaption Error:CLW less para");
					return cbInputData;
				}
				pInputData++;
				while (dwWinID < 8)
				{
					if (((*pInputData) & bitValue))
					{
						CCWinCharData * pWind = FindWindByID(dwWinID);
						if (!pWind)
						{
							return 2;
						}					
	/*****/				pWind->ResetDataInfo();
						pWind->UpdateWindowInfo();
					}
					bitValue *= 2;
					dwWinID++;
				}

				return 2;
			}
			break;
		case 0x89:///<DSW
			{
				if (cbInputData < 2)
				{
					VOLOGE("ClosedCaption Error:DSW less para");
					return cbInputData;
				}
				VO_U32 dwWinID = 0,bitValue = 1;
				pInputData++;
				while (dwWinID < 8)
				{
					if (((*pInputData) & bitValue))
					{
						CCWinCharData * pCurWin = FindWindByID(dwWinID);
						if (pCurWin)
						{
							(*pCurWin).SetVisible(VO_TRUE);
						}
					}
					bitValue *= 2;
					dwWinID++;
				}

				return 2;
			}
			break;
		case 0x8A:///<HDW
			{
				if (cbInputData < 2)
				{
					VOLOGE("ClosedCaption Error:HDW less para");
					return cbInputData;
				}
				VO_U32 dwWinID = 0,bitValue = 1;
				pInputData++;
				while (dwWinID < 8)
				{
					if (((*pInputData) & bitValue))
					{
						CCWinCharData * pCurWin = FindWindByID(dwWinID);
						if (pCurWin)
						{
							(*pCurWin).SetVisible(VO_FALSE);
						}
					}
					bitValue *= 2;
					dwWinID++;
				}

				return 2;
			}
			break;
		case 0x8B:///<TGW
			{
				if (cbInputData < 2)
				{
					VOLOGE("ClosedCaption Error:TGW less para");
					return cbInputData;
				}
				VO_U32 dwWinID = 0,bitValue = 1;
				pInputData++;
				while (dwWinID < 8)
				{
					if (((*pInputData) & bitValue))
					{
						CCWinCharData * pCurWin = FindWindByID(dwWinID);
						if (pCurWin)
						{
							(*pCurWin).SetVisible((VO_BOOL)(((*pCurWin).visible  + 1)%2));
						}
					}
					bitValue *= 2;
					dwWinID++;
				}

				return 2;
			}
			break;
		case 0x8C:///<DLW
			{
				if (cbInputData < 2)
				{
					VOLOGE("ClosedCaption Error:DLW less para");
					return cbInputData;
				}
				VO_U32 dwWinID = 0,bitValue = 1;
				pInputData++;
				while (dwWinID < 8)
				{
					if (((*pInputData) & bitValue))
					{
						DeleteWindowByID(dwWinID);
						if (dwWinID == (VO_U32)m_CurWindowID)
						{
							m_CurWindowID = -1;
							SetCurWindow(-1);
							m_bClearScreen = VO_TRUE;
						}
					}
					bitValue *= 2;
					dwWinID++;
				}

				return 2;
			}
			break;
		case 0x8D:///<DLY
			{
				return 2;
			}
			break;
		case 0x8E:///<DLC
			{
				return 1;
			}
			break;
		case 0x8F:///<RET
			{
				return 1;
			}
			break;

		case 0x90:///<SPA
			{
				if (cbInputData < 3)
				{
					VOLOGE("ClosedCaption Error:SPA less para");
					return cbInputData;
				}
				pInputData++;
				cbInputData--;
				CCWinCharData * pCurWin = FindWindByID(m_CurWindowID);
				if (pCurWin)
				{
					pCurWin->SetPenAttri((*pCurWin).penAttri,pInputData,cbInputData);
				}
				ret = 3;
			}
			break;
		case 0x91:///<SPC
			{
				if (cbInputData < 4)
				{
					VOLOGE("ClosedCaption Error:SPC less para");
					return cbInputData;
				}
				pInputData++;
				cbInputData--;
				CCWinCharData * pCurWin = FindWindByID(m_CurWindowID);
				if (pCurWin)
				{
					pCurWin->SetPenColor((*pCurWin).penColor,pInputData,cbInputData);
				}
				ret = 4;
			}
			break;
		case 0x92:///<SPL
			{
				if (cbInputData < 2)
				{
					VOLOGE("ClosedCaption Error:SPL less para");
					return cbInputData;
				}
				pInputData++;
				CCWinCharData * pWind = FindWindByID(m_CurWindowID);
				if (!pWind)
				{
					return cbInputData > 2 ? 3 : 2;
				}
				CCCharData *pData = pWind->GetCharBuffer();
				if (pData)
				{
					VO_U32 nWindowTopNo = TranslatePosToRowNo(pWind->m_pInfoEntry->stSubtitleRectInfo.stRect.nTop);
					VO_U32 nCurRelRow = R4B3(*pInputData++);
					pWind->SetCurRowNo(nWindowTopNo + nCurRelRow);
					VO_U32 dwColNo = 0;
					if (cbInputData > 2)
						dwColNo = R6B5(*pInputData++);

					CCCharData * pDataBuf = pWind->GetCharBuffer();
					LineInfo* pRowInfo = pDataBuf->GetRowDataInfo();

					if (pWind->IsRowNoValid())
					{
						VO_U32 nRowNum = pWind->GetCurRowNo();
						if (pDataBuf->GetCharCntByRowNo(nRowNum) > 0)
						{
							pDataBuf->MoveCursorToColumn(pWind->GetCurRowNo(),dwColNo,VO_FALSE);///<set the column number
						}
						else
							SetRectInfo(&pRowInfo[nRowNum].m_stRowDesc.stDataBox.stRect,0xFFFFFFFF,0xFFFFFFFF,dwColNo,0xFFFFFFFF);
					}
					
				}
				pWind->UpdateWindowInfo();
				ret = (cbInputData > 2) ? 3 : 2;
			}
			break;
		case 0x97:///<SWA
			{
				if (cbInputData < 5)
				{
					VOLOGE("ClosedCaption Error:SWA less para");
					return cbInputData;
				}
				pInputData++;
				cbInputData--;
				CCWinCharData * pWind = FindWindByID(m_CurWindowID);
				if (pWind)
				{
					pWind->SetWindowAttri((*pWind).windowAttri,pInputData,cbInputData);
				}
				ret = 5;
			}
			break;
		default:///<0x93~0x96
			{
				ret = 1;
			}
			break;
		}
	}
	return ret;
}
VO_U32 CC708CharParser::ParserNonStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData,VO_BOOL beExtCode)
{
	VO_U32 ret = 0;
	if (*pInputData >= C0_SET_START_CODE_CEA_708 && *pInputData <= C0_SET_END_CODE_CEA_708)
	{
		if (beExtCode)///<C1
		{
			return (*pInputData)/8 + 1; 
		}
		ret = ParserC0Code(pInputData,cbInputData);
	} 
	else if (*pInputData >= C1_SET_START_CODE_CEA_708 && *pInputData <= C1_SET_END_CODE_CEA_708)
	{
		if (beExtCode)
		{
			if (*pInputData < 0x90)
			{
				return ((*pInputData) - C1_SET_START_CODE_CEA_708 )/8 + 5; 
			}
			else
			{
				if (cbInputData <2)
				{
					VOLOGE("ClosedCaption Error:ParserC1 EXT Code cbInputData <2:%d",(int)cbInputData);
					return cbInputData;
				}
				return ((*(++pInputData))&0x1F)+2;
			}

		}
		else
		{
			ret = ParserC1Code(pInputData,cbInputData);
		}
	}
	return ret;
}
VO_U32 CC708CharParser::Process(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	static int count = 0;
	VOLOGR("count==%d",count++);

	if (pInputData == NULL || cbInputData == 0)
	{
		return VO_ERR_PARSER_FAIL;
	}

	//	VO_U32 nInputData = cbInputData;
	VO_PBYTE pInData = pInputData;
	while (pInData < pInputData + cbInputData)
	{
		VO_BOOL beExtCode = VO_FALSE;
		if (*pInData == EXT_CODE_CEA_708)
		{
			pInData++;
			beExtCode = VO_TRUE;
		}
		if (((*pInData) >= G0_G2_SET_START_CODE_CEA_708 && (*pInData) <= G0_G2_SET_END_CODE_CEA_708)
			||((*pInData) >= G1_G3_SET_START_CODE_CEA_708 && (*pInData) <= G1_G3_SET_END_CODE_CEA_708))///<handle the standard char(0x20~0x7F,0xA0~0xFF)
		{
			ParserStandardChar(pInData,1,beExtCode);
			pInData++;
		}
		else
		{
			VOLOGR("ControlCode:%x",*pInData);
			VO_U32 nDataSize = ParserNonStandardChar(pInData,cbInputData - (pInData - pInputData),beExtCode);
			pInData += nDataSize;
		}
	}
	return VO_ERR_PARSER_OK;
}

VO_BOOL CC708CharParser::GetWindowInfoList(voSubtitleInfo * pSubtitleInfo)
{
	if (!pSubtitleInfo || !m_pBaseWinData)
	{
		return VO_FALSE;
	}
	CCWinCharData * pWin = (CCWinCharData *)m_pBaseWinData;
	
	if ((pWin->nWindID != -1) && (pWin->visible))
	{
		pSubtitleInfo->pSubtitleEntry = pWin->GetInfoEntry();
	}
	else
		pSubtitleInfo->pSubtitleEntry = NULL;
	
	pvoSubtitleInfoEntry* pTmpInfoEntry = NULL;

	if (!pSubtitleInfo->pSubtitleEntry)
	{
		pTmpInfoEntry = &(pSubtitleInfo->pSubtitleEntry);
	}
	else
		pTmpInfoEntry = &(pSubtitleInfo->pSubtitleEntry->pNext);
	
	CCWinCharData * pTmpWin = (CCWinCharData *)m_pBaseWinData->pNext;
	while(pTmpWin)
	{
		if((pTmpWin->nWindID != -1) && (pTmpWin->visible))
		{
			*pTmpInfoEntry = pTmpWin->GetInfoEntry();
			pTmpInfoEntry = &((*pTmpInfoEntry)->pNext);
		}

		pTmpWin = (CCWinCharData *)pTmpWin->pNext;
	}
	return VO_TRUE;
}

VO_U32 CC708CharParser::GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer)
{
	pBuffer->pSubtitleInfo = &m_stSubtitleInfo;
	
	CCWinCharData * pWin = (CCWinCharData *)m_pBaseWinData;
	VO_BOOL bUpdate = VO_FALSE;
	while(pWin)
	{
		if (pWin->nWindID != -1 && pWin->m_bUpdate)
		{
			bUpdate = VO_TRUE;
			break;
		}
		pWin = (CCWinCharData *)pWin->pNext;
	}
	if (m_bClearScreen)
	{
		m_stSubtitleInfo.pSubtitleEntry = &m_stNullEntry;
		m_bClearScreen = VO_FALSE;
	}
	else if (bUpdate)
	{
		GetWindowInfoList(&m_stSubtitleInfo);
	}
	else
		m_stSubtitleInfo.pSubtitleEntry = NULL;
		


	pWin = (CCWinCharData *)m_pBaseWinData;
	while(pWin)
	{
		pWin->m_bUpdate = VO_FALSE;
		pWin = (CCWinCharData *)pWin->pNext;
	}

	if (!pBuffer->pSubtitleInfo->pSubtitleEntry)
	{
		return VO_ERR_PARSER_INPUT_BUFFER_SMALL;
	}
	return VO_ERR_PARSER_OK;
}

CCWinCharData * CC708CharParser::FindWindByID(VO_S8 nWindID)
{
	if (nWindID > 7 || nWindID < 0)
	{
		return NULL;
	}
	CCWinCharData *pList = (CCWinCharData *)m_pBaseWinData;
	while(pList)
	{
		if (pList->GetWinID() == nWindID)
		{
			return pList;
		}
		pList = (CCWinCharData *)pList->pNext;
	}
	return NULL;
}

VO_BOOL CC708CharParser::DeleteWindowByID(VO_U8 nWindID)
{
	if (nWindID > 7 || nWindID < 0)
	{
		return VO_FALSE;
	}
	CCWinCharData *pList = (CCWinCharData *)m_pBaseWinData;
	CCWinCharData *pPre = pList;
	while(pList)
	{
		if (pList->GetWinID() == nWindID)
		{
			pList->Init();
			return VO_TRUE;
		}
		pPre = pList;
		pList = (CCWinCharData *)pList->pNext;
	}
	return VO_FALSE;
}

VO_VOID CC708CharParser::SetCurWindow(VO_S8 byWindowID)
{
	m_CurWindowID = byWindowID;
	m_dwWindIDSetting = m_CurWindowID;///<set current window to default window
}

VO_BOOL CC708CharParser::IsWindowVisible(VO_U8 nWindID)
{
	CCWinCharData * pCurWin = FindWindByID(nWindID);
	if (pCurWin)
	{
		return (VO_BOOL)pCurWin->visible;
	}
	return VO_FALSE;
}


VO_U32 WindAttri::Load(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (cbInputData < 4)
	{
		VOLOGE("ClosedCaption Error:WindAttri less para");
		return 0;
	}
	fo = R2B7(*pInputData);
	fr = R2B5(*pInputData);
	fg = R2B3(*pInputData);
	fb = R2B1(*pInputData++);

	bt0 = R2B7(*pInputData);
	br = R2B5(*pInputData);
	bg = R2B3(*pInputData);
	bb = R2B1(*pInputData++);

	bt2 = R1B7(*pInputData);
	ww = R1B6(*pInputData);
	pd = R2B5(*pInputData);
	sd = R2B3(*pInputData);
	j = R2B1(*pInputData++);

	es = R4B7(*pInputData);
	ed = R2B3(*pInputData);
	de = R2B1(*pInputData++);
	return 4;
}

VO_U32 PenAttri::Load(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (cbInputData < 2)
	{
		VOLOGE("ClosedCaption Error:PenAttri less para");
		return 0;
	}
	tt = R4B7(*pInputData);
	o = R2B3(*pInputData);
	s = R2B1(*pInputData++);

	i = R1B7(*pInputData);
	u = R1B6(*pInputData);
	et = R3B5(*pInputData);
	fs = R3B2(*pInputData++);
	return 2;
}

VO_U32 PenColor::Load(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (cbInputData < 3)
	{
		VOLOGE("ClosedCaption Error:PenColor less para");
		return 0;
	}
	fo = R2B7(*pInputData);
	fr = R2B5(*pInputData);
	fg = R2B3(*pInputData);
	fb = R2B1(*pInputData++);

	bo = R2B7(*pInputData);
	br = R2B5(*pInputData);
	bg = R2B3(*pInputData);
	bb = R2B1(*pInputData++);

	er = R2B5(*pInputData);
	eg = R2B3(*pInputData);
	eb = R2B1(*pInputData++);
	return 3;
}

CCWinCharData::CCWinCharData(VO_U32 dwRowCnt,VO_U32 dwColCnt)
:CCBaseWinCharData(dwRowCnt,dwColCnt)
,nWindID(-1)
,m_bUpdate(VO_FALSE)
,visible(VO_FALSE)
{
}

CCWinCharData::~CCWinCharData()
{
}


VO_U32 CCWinCharData::Load(VO_PBYTE pInputData, VO_U32 cbInputData,VO_BOOL beCreat)
{
	if (cbInputData < 7)
	{
		VOLOGE("ClosedCaption Error:DFCommandPara less para");
		return cbInputData;
	}
	nWindID = R3B2(*pInputData++);

	SetVisible((VO_BOOL)R1B5(*pInputData));
	rl = R1B4(*pInputData);
	cl = R1B3(*pInputData);
	priority = R3B2(*pInputData++);
	m_pInfoEntry->stSubtitleRectInfo.nRectZOrder = priority;

	rp = R1B7(*pInputData);
	av = R7B6(*pInputData++);

	ah = *pInputData++;

	ap = R4B7(*pInputData);
	rc = R4B3(*pInputData++);
	rc += 1;

	cc = R6B5(*pInputData++);
	cc +=1;

	SetWindowRectInfo(ap,(VO_BOOL)rp,av,ah,rc,cc);
	ws = R3B5(*pInputData);
	if (!ws && beCreat)
	{
		ws = 1;
	}
	ps = R3B2(*pInputData++);
	if (!ps && beCreat)
	{
		ps = 1;
	}
	if (ws)
	{
		SetWindowAttri(windowAttri,(VO_PBYTE)PresetWinAttri[ws - 1],4);
	}
	if (ps)
	{
		if (beCreat)
		{
			SetPenAttri(penAttri,(VO_PBYTE)PresetPenAttri[ps - 1],2);
		}
		SetPenColor(penColor,(VO_PBYTE)PresetPenColor[ps - 1],3);
	}

	VO_U32 nWinMinRow = TranslatePosToRowNo(m_pInfoEntry->stSubtitleRectInfo.stRect.nTop);
	VO_U32 nWinMaxRow = TranslatePosToRowNo(m_pInfoEntry->stSubtitleRectInfo.stRect.nBottom) - 1;

	if (IsRowNoValid() && (m_CurRowNo < nWinMinRow || m_CurRowNo > nWinMaxRow))
	{
		VO_U32 nRow = 0;
		while(nRow < nWinMinRow)
		{
			m_CharData->ResetRowBuffer(nRow);
			nRow++;
		}
		nRow = nWinMaxRow + 1;
		while(nRow < m_CharData->GetRowCnt())
		{
			m_CharData->ResetRowBuffer(nRow);
			nRow++;
		}
		m_CurRowNo = nWinMinRow;
		if (windowAttri.sd == 3)
		{
			m_CurRowNo = nWinMaxRow;
		}	
	}
	return 7;

}

VO_VOID CCWinCharData::Init()
{
	nWindID = -1;
	SetVisible(VO_FALSE);
	ResetDataInfo();
}

VO_VOID CCWinCharData::SetWindowRectInfo(VO_U32 nAnchorID,VO_BOOL bRelPos,VO_U32 nAnchorVer,VO_U32 nAnchorHor,VO_U32 nRowCnt,VO_U32 nColCnt)
{

	VO_U32 nMinRow = 0,nMaxRow = 0,nMinCol = 0,nMaxCol = 0;

	if (bRelPos)
	{
		nMinRow = (nAnchorVer*15/100);
		nMinCol =(nAnchorHor*32/100);
	}
	else
	{
		nMinRow = nAnchorVer/5;
		nMinCol = nAnchorHor/5;		
	}

	if (nAnchorID/3 == 0)
	{
		
	}
	else if (nAnchorID/3 == 1)
	{
		if (nMinRow >= nRowCnt/2)
		{
			nMinRow = nMinRow - nRowCnt/2;
		}
	}
	else if (nAnchorID/3 == 2)
	{
		if (nMinRow >= (nRowCnt - 1))
		{
			nMinRow = nMinRow - (nRowCnt - 1);
		}
	}

	if (nAnchorID%3 == 0)
	{
	}
	else if (nAnchorID%3 == 1)
	{
		if (nMinCol >= nColCnt/2)
		{
			nMinCol = nMinCol - nColCnt/2;
		}	
	}
	else if (nAnchorID%3 == 2)
	{
		if (nMinCol >= (nColCnt - 1))
		{
			nMinCol = nMinCol - (nColCnt - 1);
		}
	}

	nMaxRow = nMinRow + (nRowCnt - 1);
	nMaxCol = nMinCol + (nColCnt - 1);

	nMinRow = nMinRow > 14 ? 14 : nMinRow;
	nMaxRow = nMaxRow > 14 ? 14 : nMaxRow;

	nMinCol = nMinCol > 31 ? 31 : nMinCol;
	nMaxCol = nMaxCol > 31 ? 31 : nMaxCol;

	SetRectInfo(&m_pInfoEntry->stSubtitleRectInfo.stRect,nMinRow,nMaxRow,nMinCol,nMaxCol);
}

VO_U32 CCWinCharData::SetWindowAttri(WindAttri& stWinAttri,VO_PBYTE pInputData, VO_U32 cbInputData)
{
	VO_U32  ret = stWinAttri.Load(pInputData,cbInputData);
	LineInfo * pRowInfo = m_CharData->GetRowDataInfo();
	for (VO_U32 nCnt = 0;nCnt < 15;nCnt++)
	{
		pRowInfo[nCnt].m_stRowDesc.nHorizontalJustification = windowAttri.j%3;
		pRowInfo[nCnt].m_stRowDesc.nPrintDirection = windowAttri.pd;
		pRowInfo[nCnt].m_stRowDesc.nVerticalJustification = 0;
	}
	MapColor2RGB(windowAttri.br,windowAttri.bg,windowAttri.bb,&m_pInfoEntry->stSubtitleRectInfo.stRectBorderColor);
//	TranslateColorToRGB((Color_Type)TranslateColor(windowAttri.br,windowAttri.bg,windowAttri.bb),&m_pInfoEntry->stSubtitleRectInfo.stRectBorderColor);
	m_pInfoEntry->stSubtitleRectInfo.stRectBorderColor.nTransparency = 0;

	m_pInfoEntry->stSubtitleRectInfo.nRectBorderType = windowAttri.bt2 << 3 | windowAttri.bt0;
	m_pInfoEntry->stSubtitleRectInfo.stRectDisplayEffct.nEffectType = windowAttri.de;
	m_pInfoEntry->stSubtitleRectInfo.stRectDisplayEffct.nEffectDirection = windowAttri.ed;
	m_pInfoEntry->stSubtitleRectInfo.stRectDisplayEffct.nEffectSpeed = windowAttri.es;
	
	MapColor2RGB(windowAttri.fr,windowAttri.fg,windowAttri.fb,&m_pInfoEntry->stSubtitleRectInfo.stRectFillColor);
//	TranslateColorToRGB((Color_Type)TranslateColor(windowAttri.fr,windowAttri.fg,windowAttri.fb),&m_pInfoEntry->stSubtitleRectInfo.stRectFillColor);
	
	m_pInfoEntry->stSubtitleRectInfo.stRectFillColor.nTransparency = 0x0;
	switch (windowAttri.fo)
	{
	case 0:
	case 1:
		m_pInfoEntry->stSubtitleRectInfo.stRectFillColor.nTransparency = 0xFF;///<100%
		break;
	case 2:
		m_pInfoEntry->stSubtitleRectInfo.stRectFillColor.nTransparency = 0xBF;///<75%
		break;
	case 3:
		m_pInfoEntry->stSubtitleRectInfo.stRectFillColor.nTransparency = 0x0;///<0%
		break;
	default:
		break;
	}

	m_pInfoEntry->stSubtitleDispInfo.stDispDescriptor.nScrollDirection = windowAttri.sd;
	m_pInfoEntry->stSubtitleDispInfo.stDispDescriptor.stWrap = windowAttri.ww;

	UpdateWindowInfo();
	return ret;
}

VO_U32 CCWinCharData::SetPenAttri(PenAttri& stPenAttri,VO_PBYTE pInputData, VO_U32 cbInputData)
{
	VO_U32  ret = stPenAttri.Load(pInputData,cbInputData);

	m_pCurStrInfo->stCharEffect.EdgeType = penAttri.et;

	m_pCurStrInfo->stCharEffect.Italic = penAttri.i;

	m_pCurStrInfo->stCharEffect.Offset = penAttri.o;

	m_pCurStrInfo->stCharEffect.TextTag = penAttri.tt;

	m_pCurStrInfo->stCharEffect.Underline = penAttri.u;

	m_pCurStrInfo->stFontInfo.FontSize = penAttri.s;
	
	m_pCurStrInfo->stFontInfo.FontStyle = penAttri.fs;

	UpdateWindowInfo();
	return ret;

}

VO_U32 CCWinCharData::SetPenColor(PenColor& stPenColor,VO_PBYTE pInputData, VO_U32 cbInputData)
{
	VO_U32 ret = stPenColor.Load(pInputData,cbInputData);

	MapColor2RGB(penColor.fr,penColor.fg,penColor.fb,&m_pCurStrInfo->stFontInfo.stFontColor);
//	TranslateColorToRGB((Color_Type)TranslateColor(penColor.fr,penColor.fg,penColor.fb),&m_pCurStrInfo->stFontInfo.stFontColor);
	if (penColor.fo == 0 || penColor.fo == 1)
	{
		m_pCurStrInfo->stFontInfo.stFontColor.nTransparency = 0xFF;
	}
	else if(penColor.fo == 2)
	{
		m_pCurStrInfo->stFontInfo.stFontColor.nTransparency = 0xBF;
	}
	else
		m_pCurStrInfo->stFontInfo.stFontColor.nTransparency = 0x0;

	MapColor2RGB(penColor.er,penColor.eg,penColor.eb,&m_pCurStrInfo->stCharEffect.EdgeColor);
//	TranslateColorToRGB((Color_Type)TranslateColor(penColor.er,penColor.eg,penColor.eb),&m_pCurStrInfo->stCharEffect.EdgeColor);
	m_pCurStrInfo->stCharEffect.EdgeColor.nTransparency = m_pCurStrInfo->stFontInfo.stFontColor.nTransparency;

	LineInfo * pRowInfo = m_CharData->GetRowDataInfo();
	for (VO_U32 nCnt = 0;nCnt < 15;nCnt++)
	{
		MapColor2RGB(penColor.br,penColor.bg,penColor.bb,&pRowInfo[nCnt].m_stRowDesc.stDataBox.stRectFillColor);
	//	TranslateColorToRGB((Color_Type)TranslateColor(penColor.br,penColor.bg,penColor.bb),&pRowInfo[nCnt].m_stRowDesc.stDataBox.stRectFillColor);

		if (penColor.bo == 0 || penColor.bo == 1)
		{
			pRowInfo[nCnt].m_stRowDesc.stDataBox.stRectFillColor.nTransparency = 0xFF;
		}
		else if(penColor.bo == 2)
		{
			pRowInfo[nCnt].m_stRowDesc.stDataBox.stRectFillColor.nTransparency = 0xBF;
		}
		else
			pRowInfo[nCnt].m_stRowDesc.stDataBox.stRectFillColor.nTransparency = 0x0;

	}
	UpdateWindowInfo();
	return ret;
}

pvoSubtitleInfoEntry CCWinCharData::GetInfoEntry()
{
	LineInfo * pData = m_CharData->GetRowDataInfo();

	VO_U32 nRow = TranslatePosToRowNo(m_pInfoEntry->stSubtitleRectInfo.stRect.nTop);
	VO_U32 nWinMaxRow = TranslatePosToRowNo(m_pInfoEntry->stSubtitleRectInfo.stRect.nBottom) - 1;
	nWinMaxRow = (nWinMaxRow <= (m_CharData->GetRowCnt() - 1)) ? nWinMaxRow : (m_CharData->GetRowCnt() - 1);
	nRow = (nRow <= nWinMaxRow) ? nRow : 0;
	VO_U32 nStrLen = 0;
	pvoSubtitleTextRowInfo pTextRowInfoCur = m_pInfoEntry->stSubtitleDispInfo.pTextRowInfo;
	pvoSubtitleTextRowInfo pTmpRowInfo = pTextRowInfoCur;

	pvoSubtitleTextRowInfo pRowHead = m_pInfoEntry->stSubtitleDispInfo.pTextRowInfo;
	while(pRowHead)
	{
		ResetStructEntry(pRowHead,Struct_Type_voSubtitleTextRowInfo);
		pRowHead = pRowHead->pNext;
	}
	
	while(nRow <= nWinMaxRow)
	{
		if (!pData[nRow].m_nCharCntCurLine)
		{
			nRow++;
			continue;
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

		VO_U32 nWinMinCol = TranslatePosToColNo(m_pInfoEntry->stSubtitleRectInfo.stRect.nLeft);
	//	VO_U32 nWinMaxCol = TranslatePosToColNo(m_pInfoEntry->stSubtitleRectInfo.stRect.nRight) - 1;

		VO_U32 nRowCol = TranslatePosToColNo(pTmpRowInfo->stTextRowDes.stDataBox.stRect.nLeft) + nWinMinCol;
		SetRectInfo(&pTmpRowInfo->stTextRowDes.stDataBox.stRect,0xFFFFFFFF,0xFFFFFFFF,nRowCol,0xFFFFFFFF);

		pvoSubtitleTextInfoEntry pTmpTextInfoEntry = pTmpRowInfo->pTextInfoEntry;
		pvoSubtitleTextInfoEntry pTmp = NULL;

		CharInfo* pRowCharInfo = pData[nRow].m_pCharOfLine;
		voSubtitleStringInfo stStrInfo;
		InitializeStruct(&stStrInfo,Struct_Type_voSubtitleStringInfo);

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

	return m_pInfoEntry;
}

VO_VOID CCWinCharData::Reset()
{
	nWindID = -1;
	m_bUpdate = VO_FALSE;
	CCBaseWinCharData::Reset();
}

VO_VOID CCWinCharData::UpdateWindowInfo(VO_BOOL bForce)
{
	if (visible || bForce)
	{
		m_bUpdate = VO_TRUE;
	}
}

VO_VOID CCWinCharData::SetVisible(VO_BOOL bVis)
{
	if(bVis != visible)
	{
		UpdateWindowInfo(VO_TRUE);
		visible = bVis;
	}
}

VO_BOOL CCWinCharData::MapColor2RGB(VO_U32 uRed,VO_U32 uGreen,VO_U32 uBlue,voSubtitleRGBAColor *voRGBValue)
{
	if (uRed > 3 || uGreen > 3 || uBlue > 3 || NULL == voRGBValue)
	{
		return VO_FALSE;
	}
	voRGBValue->nRed = uRed*CC708RGB_TIMESCALE;
	voRGBValue->nGreen = uGreen*CC708RGB_TIMESCALE;
	voRGBValue->nBlue = uBlue*CC708RGB_TIMESCALE;
	voRGBValue->nTransparency = 0xFF;
	return VO_TRUE;
}