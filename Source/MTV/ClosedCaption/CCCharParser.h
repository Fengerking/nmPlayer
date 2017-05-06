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
* @file CCCharParser.h
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#ifndef _CCCHARPARSER_H_
#define _CCCHARPARSER_H_
#include "voType.h"
#include "CvoBaseMemOpr.h"
#include "voCaptionParser.h"
#include "fMacros.h"
#include "CCCharData.h"
#include "CharacterCode.h"
#include "CSubtilePacker.h"
//#define _STRING_INFO_

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef enum
{
	command_rollUp = 0,
	command_Carriage_return,
	command_indent,
	command_EDM,
	command_RCL,
	command_BS,
	command_DER,
	command_FON,
	command_RDC,
	command_TR,
	command_RTD,
	command_ENM,
	command_EOC,
	command_TO,
	command_none
}comand_type;

typedef enum
{
	Char_Type_Normal= 0,	///<1 byte 20~7F
	Char_Type_MidRow,		///<2 byte [11][19]) 20~2F
	Char_Type_Special,		///<2 byte [11][19] 30~3F
	Char_Type_ExtChar,		///<2 byte [12][1A][13][1B] 20~3F
	Char_Type_Preamble,		///<2 byte [13][1B][14][1C]	40~4F[60~6F]
	Char_Type_Carriage,		///<1 byte 0A	
	Char_Type_Unknow
}Char_Type;

typedef enum
{
	Present_Style_PopUp = 0,
	Present_Style_RollUp,
	Present_Style_PaintOn,
	Present_Style_Unknow
}Present_Style;

typedef struct
{
	VO_U32 nCodePageValue;
	const VO_CHAR * strLocale;
}CodePage;

enum
{
	OperatingMode_Caption_Mode,
	OperatingMode_Text_Mode,
	OperatingMode_XDS_Mode,
	OperatingMode_Unknow
};
#define MAX_CHARACTER_COUNT_IN_ONE_LINE 40///<MAX_STANDARD_CHARACTER_COUNT_IN_ONE_LINE * MAX_SIZE_FOR_SPECIAL_CHAR_AND_EXT_CHAR * EXT_SIZE_FOR_PARAMETER_OF_FONT

#ifdef HTML_PACK
class CHTMLPacker;
#endif


class CCBaseWinCharData
{
public:
	CCBaseWinCharData(VO_U32 dwMaxLinCnt,VO_U32 dwMaxColumnCnt);
	virtual ~CCBaseWinCharData();
public:
	CCBaseWinCharData *pNext;
	voSubtitleInfoEntry*		m_pInfoEntry;
public:
	VO_S32						GetCurRowNo(){return m_CurRowNo;}
	VO_BOOL						IsRowNoValid();
	VO_VOID						SetCurRowNo(VO_U32 dwRowNo);
	virtual VO_VOID				PushString(const VO_U16* pString, VO_U32 nStrLen);
	virtual VO_VOID				PushStringNoPara(const VO_U16* pString, VO_U32 nStrLen);
	CCCharData*					GetCharBuffer(){return m_CharData;}
	virtual VO_VOID				Reset();
	virtual VO_VOID				ResetDataInfo();
	virtual VO_VOID				ResetOutputInfo();
	virtual VO_VOID				ResetPopupDataInfo();
	voSubtitleStringInfo*		GetStringInfo(){return m_pCurStrInfo;}
	pvoSubtitleInfoEntry		GetInfoEntry();
protected:
	VO_VOID						InitScreenInfo();
	VO_VOID						SetTextRowDescriptor(VO_U32 nWrap,VO_U32 nScrollDirection);
protected:
	VO_S32						m_CurRowNo;
	CCCharData*					m_CharData;
	voSubtitleStringInfo*		m_pCurStrInfo;
	VO_U16*						m_TmpString;
};
class CCCharParser :public CvoBaseMemOpr
{
public:
	CCCharParser(VO_CAPTION_PARSER_INIT_INFO* pParam,VO_U32 dwMaxLinCnt,VO_U32 dwMaxColumnCnt);
	virtual ~CCCharParser();
public:
	virtual VO_U32			Process(VO_PBYTE pInputData, VO_U32 cbInputData);
	virtual VO_U32			GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer);
	virtual VO_U32			SetParam(VO_PTR pParam){return VO_ERR_PARSER_OK;}
	virtual VO_VOID			Reset();
	virtual VO_VOID			ReStart(){}
	virtual VO_BOOL			GetWindowInfoList(voSubtitleInfo * pSubtitleInfo);
	virtual VO_VOID			OutputStringInfo(voSubtitleInfo * pInfo);
	virtual VO_VOID			SetReset(VO_BOOL bReset){m_bNeedReset = bReset;}
protected:
	virtual	VO_VOID			ResetDataInfo();
	virtual VO_VOID			EndOfDisplay();	
protected:
	VO_U32					m_dwMaxRowCnt;
	VO_U32					m_dwMaxColumnCnt;
	CCBaseWinCharData*		m_pBaseWinData;
	voSubtitleInfo			m_stSubtitleInfo;
	VO_BOOL					m_bNeedReset;
	voSubtitleInfoEntry		m_stNullEntry;
	VO_BOOL					m_bClearScreen;
};


class CC608CharParser :public CCCharParser
{
public:
	CC608CharParser(VO_CAPTION_PARSER_INIT_INFO* pParam,VO_U32 dwMaxLinCnt = MAX_ROW_COUNT,VO_U32 dwMaxColumnCnt = MAX_COLUMN_COUNT);
	virtual ~CC608CharParser();
public:
	VO_U32			Process(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_VOID			Reset();
	VO_VOID			ReStart();
	VO_U32			SetParam(VO_PTR pParam){SwitchUserSelectedDataChannel(*(VO_U32*)pParam); return VO_ERR_PARSER_OK;}
	VO_U32			GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer);
protected:
	VO_VOID			SwitchUserSelectedDataChannel(VO_BYTE nChanellData);
	VO_VOID			SetDefaultDataChannel(){m_nUserSelectedDataChannel = VO_CAPTION_CHANNEL_1;}
	VO_VOID			SetCurrentDataChannel(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_VOID			ResetDataInfo();
protected:
	VO_CAPTION_CHANNEL	m_nUserSelectedDataChannel;
	VO_CAPTION_CHANNEL	m_nCurDataChannel;
	VO_U32				m_nDefaultMode;
	VO_U32				m_nCurMode;
private:
	VO_BOOL			ParserSpecialChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL			ParserStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL			ParserMIDROWChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL			ParserControlChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL			ParserPreambleChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL			ParserTabControlChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL			ParserNonStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL			MapASCIICharToUniCode(VO_BYTE nInputData, VO_U16 &nOutData);
	VO_BOOL			ParserExtendedChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_VOID			SetLastCtrlChar(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_BOOL			ParserAttributeByte(VO_BYTE nInputData,voSubtitleStringInfo* pStringInfo,VO_BOOL BeMidRow);
	VO_VOID			LineNumberInc();
	VO_VOID			BaseLineChange(VO_U32* pNewBaseLine);
	VO_VOID			ReverseDisplay();
	VO_BOOL			ChangePreStyle(Present_Style eStyle);
private:
	VO_U16			m_nLastCtrlChar;
	VO_U8			m_nMaxLineCntForRollUp;///<just for roll-up mode	
	Present_Style	m_PreStyle;
	VO_U32			m_BaseLineNum;

	//urgly walk round by Yi
	VO_BOOL			m_bEmptySend;
	VO_BOOL			m_bNeedSendEmpty;
	//
};

#ifdef _VONAMESPACE
}
#endif
#endif //_CCCHARPARSER_H_