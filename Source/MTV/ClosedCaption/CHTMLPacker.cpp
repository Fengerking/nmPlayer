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
* @file CHTMLPacker.cpp
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#include "CHTMLPacker.h"
#include "CCCharParser.h"
#include "CharacterCode.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

///<define
#if 1
const wchar_t FondColorWhite[]		= {L"color:rgb(255,255,255);"};///<24 characters
const wchar_t FondColorGreen[]		= {L"color:rgb(0,255,0);"};///<20 characters
const wchar_t FondColorBlue[]		= {L"color:rgb(0,0,255);"};///<20 characters
const wchar_t FondColorCyan[]		= {L"color:rgb(0,255,255);"};///<22 characters
const wchar_t FondColorRed[]		= {L"color:rgb(255,0,0);"};///<20 characters
const wchar_t FondColorYellow[]		= {L"color:rgb(255,255,0);"};///<22characters
const wchar_t FondColorMagenta[]	= {L"color:rgb(255,0,255);"};///<22 characters
#else
const wchar_t FondColorWhite[]		= {L"color:%23FFFFFF;"};///<17 characters
const wchar_t FondColorGreen[]		= {L"color:%2300FF00;"};///<17 characters
const wchar_t FondColorBlue[]		= {L"color:%230000FF;"};///<17 characters
const wchar_t FondColorCyan[]		= {L"color:%2300FFFF;"};///<17 characters
const wchar_t FondColorRed[]		= {L"color:%23FF0000;"};///<17 characters
const wchar_t FondColorYellow[]		= {L"color:%23FFFF00;"};///<17 characters
const wchar_t FondColorMagenta[]	= {L"color:%23FF00FF;"};///<17 characters
#endif
const wchar_t FontUnderline[]		= {L"text-decoration:underline;"};///<27 characters
const wchar_t FontItalic[]			= {L"font-style:italic;"};///<19 characters

const wchar_t OpentagPara[] = {L"<p style=\"margin-top : 1%;margin-bottom : 1%;\">"};
const wchar_t ClosetagPara[] = {L"</p>"};
const wchar_t OpentagSpanWithStyle[] = {L"<Span style=\"background-color:rgb(0,0,0);padding-left: 3%;padding-right: 3%;padding-top: 1%;padding-bottom: 1%;"};
const wchar_t OpentagSpanWithoutPadding[] = {L"<Span style=\"background-color:rgb(0,0,0);"};
const wchar_t ClosetagSpan[] = {L"</Span>"};
const wchar_t ParamStyle[] = {L"style=\""};
const wchar_t ParamStyleEnd[] = {L"\""};
const wchar_t OpentagEnd[] = {L">"};
const wchar_t ClosetagBR[] = {L"<br />"};
//const wchar_t StylePadding[] = {L"padding-left: 3%;padding-right: 3%;"};

const EntitiesMap EntitiesTable[] = 
{
//	{0x20,L"&#160"},///<   space 0x20
	{0x3C,L"&#60"},///<    <		0x3C
	{0x3E,L"&#62"},///<    >		0x3E
	{0x26,L"&#38"},///<    &		0x26
	{0x22,L"&#34"},///<    "		0x22
	{0x27,L"&#39"},///<    '		0x27
	{0x30,L"&#48"},///<		0
	{0x31,L"&#49"},///<     1
	{0x32,L"&#50"},///<     2
	{0x33,L"&#51"},///<     3
	{0x34,L"&#52"},///<     4
	{0x35,L"&#53"},///<     5
	{0x36,L"&#54"},///<		6
	{0x37,L"&#55"},///<     7
	{0x38,L"&#56"},///<     8
	{0x39,L"&#57"},///<     9
};
#if 0
const wchar_t StyleMargin[] = {L"style=\"color:rgb(255,255,255);\""};
const wchar_t OpentagPre[] = {L"<pre>"};
const wchar_t ClosetagPre[] = {L"</pre>"};
#endif
///<(OpentagPara+(OpentagSpanWithStyle+FondColorWhite+FontUnderline+FontItalic+ParamStyleEnd
///<+OpentagEnd+char+ClosetagSpan)*40+ClosetagPara
const VO_U32 StrLenParagraphy = 6*1024;//3+1+(42+24+27+19+1+1+5+7)*40+4;
///<CHTMLPacker
CHTMLPacker::CHTMLPacker()
:m_CntUniChar(0)
{
	m_strParagraphy = new wchar_t[StrLenParagraphy];
	memset(m_TagOrder,0x0,MaxCntSupportTag*sizeof(VO_U32));
}

CHTMLPacker::~CHTMLPacker()
{
	if(m_strParagraphy)
	{
		delete []m_strParagraphy;
		m_strParagraphy = NULL;
	}
}

VO_VOID CHTMLPacker::Reset()
{
	if (m_strParagraphy)
	{
		memset(m_strParagraphy,0x0,StrLenParagraphy*sizeof(wchar_t));
	}
	m_CntUniChar = 0;
	memset(m_TagOrder,0x0,MaxCntSupportTag*sizeof(VO_U32));
}
VO_BOOL CHTMLPacker::PushTagToString(TagName eTag)
{
	switch (eTag)
	{
	case TagName_Span:
		AddString(OpentagSpanWithStyle);
		break;
	case TagName_Para:
		AddString(OpentagPara);
		break;
	case TagName_Span_Without_Padding:
		AddString(OpentagSpanWithoutPadding);
		break;
	default:
		break;
	}
	VO_U32 nOrder = 0;
	while(nOrder < MaxCntSupportTag)
	{
		if (m_TagOrder[nOrder] == 0x0)
		{
			m_TagOrder[nOrder] = eTag;
			break;
		}
		nOrder++;
	}
	return VO_TRUE;
}
VO_U32 CHTMLPacker::DecodeFontParam(voSubtitleStringInfo nFontParam)
{
#if 0
	FinishTag(TagName_Span_Without_Padding);
	PushTagToString(TagName_Span_Without_Padding);

	switch (nFontParam.color)
	{
	case Color_Type_White:
		AddString(FondColorWhite);
		break;
	case Color_Type_Green:
		AddString(FondColorGreen);
		break;
	case Color_Type_Blue:
		AddString(FondColorBlue);
		break;
	case Color_Type_Cyan:
		AddString(FondColorCyan);
		break;
	case Color_Type_Red:
		AddString(FondColorRed);
		break;
	case Color_Type_Yellow:
		AddString(FondColorYellow);
		break;
	case Color_Type_Magenta:
		AddString(FondColorMagenta);
		break;
	default:
		break;
	}
	if (nFontParam.italic)
	{
		AddString(FontItalic);
	}
	if (nFontParam.underline)
	{
		AddString(FontUnderline);
	}
	AddString(ParamStyleEnd);
	AddString(OpentagEnd);///<for span
#endif
	return 0;
}

VO_BOOL CHTMLPacker::TranslateWcharToUnicode(wchar_t *&pString)
{
	if (!pString)
	{
		return VO_FALSE;
	}
	m_CntUniChar = wcslen(pString);
	VO_U16 *pUniStr = (VO_U16 *)pString;
	for (VO_U32 nCnt = 0;nCnt < m_CntUniChar;nCnt++)
	{
		pUniStr[nCnt] = (VO_U16)pString[nCnt];
	//	VOLOGI("pUniStr=%x",pUniStr[nCnt]);
	}
	
	return VO_TRUE;
}
VO_BOOL CHTMLPacker::FinishTag(TagName eTag)
{
	VO_U32 nOrder = 0;
	VO_BOOL  beFinish = VO_FALSE;
	while (nOrder < MaxCntSupportTag)///<find the last tag
	{
		if (!m_TagOrder[nOrder])
		{
			break;
		}
		nOrder++;
	}
	if (nOrder <= 0 || (eTag != TagName_All && (VO_U32)eTag != m_TagOrder[nOrder - 1]))
	{
		return VO_TRUE;
	}
	while(!beFinish && (--nOrder)>=0)
	{
		switch(m_TagOrder[nOrder])
		{
		case TagName_Span:
		case TagName_Span_Without_Padding:
			AddString(ClosetagSpan);
			if (eTag != TagName_All)
			{
				beFinish = VO_TRUE;
			}
			break;
		case TagName_Para:
			AddString(ClosetagPara);
			if (eTag != TagName_All)
			{
				beFinish = VO_TRUE;
			}
			break;
		default:
			break;
		}
		m_TagOrder[nOrder] = 0x0;
	}
	return VO_TRUE;
}

VO_BOOL CHTMLPacker::InitParagraph(voSubtitleStringInfo* pChar,VO_U32 & curPos,VO_U32 cbChar)
{
	return InitLine(pChar,curPos,cbChar);
}

VO_BOOL CHTMLPacker::InitLine(voSubtitleStringInfo* pChar,VO_U32 &curPos,VO_U32 cbChar)
{
	FinishTag();
	PushTagToString(TagName_Para);
#if 0
	while(pChar[curPos].character == 0x20)///<take the 0x20 in the font of line as indent and no backgroup color
	{
		AddString(L"&#160");
		curPos++;
	}
#endif
	PushTagToString(TagName_Span);
	AddString(ParamStyleEnd);
	AddString(OpentagEnd);
	
	return VO_TRUE;
}
VO_U32 CHTMLPacker::PackCharToParagraph(voSubtitleStringInfo* pChar,VO_U32 cbChar)
{
#if 0
	Reset();
	if (!pChar || !cbChar) 
	{
		return VO_FALSE;
	}
	VO_U32 nCnt = 0;
	voSubtitleStringInfo *pBuf = pChar;

	voSubtitleStringInfo info;
	info.color = Color_Type_White;
	info.italic = 0;
	info.underline = 0;

	if (cbChar == 1 && pBuf[nCnt].character == (VO_U16)FONT_PARAM_SRART_CODE)
	{
		PushTagToString(TagName_Para);
	}
	else
	{
		InitParagraph(pChar,nCnt,cbChar);
		while (nCnt < cbChar)
		{
			if (pBuf[nCnt].color != info.color || pBuf[nCnt].italic != info.italic
				 || pBuf[nCnt].underline != info.underline)
			{
				info = pBuf[nCnt];
				DecodeFontParam(pBuf[nCnt]);
			}
			if (pBuf[nCnt].character == (VO_U16)CARRIAGE_RETURN_CHAR)
			{
				InitLine(pChar,nCnt,cbChar);
			}
			else
			{
				TranscodeSpecialChar(pBuf[nCnt].character);
			}
			nCnt++;
		}
	}
	FinishTag();
	TranslateWcharToUnicode(m_strParagraphy);
#endif
	return VO_ERR_PARSER_OK;

}

VO_BOOL CHTMLPacker::TranscodeSpecialChar(VO_U16 CharValue)
{
	for (VO_U32 cnt = 0;cnt < MaxCntEntities;cnt++)
	{
		if (CharValue == EntitiesTable[cnt].UniChar)
		{
			AddString(EntitiesTable[cnt].EntityChar);
			return VO_TRUE;
		}
	}
	wchar_t tmpStr[2] = {0};
	{
		tmpStr[0] = (wchar_t)CharValue;
		tmpStr[1] = 0x0;
	}
	AddString(tmpStr);
	return VO_TRUE;
}
VO_BOOL CHTMLPacker::AddString(const wchar_t * str)
{
	if (wcslen(m_strParagraphy) + wcslen(str) < StrLenParagraphy )
	{
		wcscat(m_strParagraphy,str);
		return VO_TRUE;
	}
	return VO_FALSE;
}