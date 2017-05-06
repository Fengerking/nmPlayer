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
* @file CC708CharParser.h
*
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/

#ifndef _CC708CHARPARSER_H_
#define _CC708CHARPARSER_H_
#include "CCCharParser.h"

///<=======================================
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef enum
{
	LEFT = 0,
	RIGHT,
	CENTERR,
	FULL
}justification;


typedef enum
{
	LEFT_TO_RIGHT = 0,
	RIGHT_TO_LEFT,
	TOP_TO_BOTTOM,
	BOTTOM_TO_TOP
}print_scroll_effect_direction;

typedef enum
{
	SNAP = 0,
	FADE,
	WIPE
}display_effect;

typedef enum
{
	SOLID = 0,
	FLASH,
	TRANSLUCENT,
	TRANSPARENT
}fill_opacity;

typedef enum
{
	NONE = 0,
	RAISED,
	DEPRESSED,
	UNIFORM,
	SHADOW_LEFT,
	SHADOW_RIGHT
}border_type;

VO_U32	TranslateColor(VO_U8 ValueRed,VO_U8 ValueGreen,VO_U8 ValueBlue);

typedef struct
{
	VO_U8 fo	:2;///<window && window border fill opacity
	VO_U8 fr	:2;///<window interior color RGB
	VO_U8 fg	:2;///<window interior color RGB
	VO_U8 fb	:2;///<window interior color RGB

	VO_U8 bt0	:2;///<border type 3bit,0th&&1th bit
	VO_U8 br	:2;///<window border color RGB
	VO_U8 bg	:2;///<window border color RGB
	VO_U8 bb	:2;///<window border color RGB

	VO_U8 bt2	:1;///<window border type 3bit,2t bit
	VO_U8 ww	:1;///<word wrap
	VO_U8 pd	:2;///<print direction,refer print_scroll_effect_direction
	VO_U8 sd	:2;///<scroll direction,refer print_scroll_effect_direction
	VO_U8 j		:2;///<justify ,refer justification

	VO_U8 es	:4;///<effect speed ,0.5s as a unit 
	VO_U8 ed	:2;///<effect direction,refer print_scroll_effect_direction
	VO_U8 de	:2;///<display effect ,refer justification
	
	VO_U32 Load(VO_PBYTE pInputData, VO_U32 cbInputData);
}WindAttri;
typedef struct
{
	VO_U8 tt	:4;
	VO_U8 o		:2;
	VO_U8 s		:2;

	VO_U8 i		:1;
	VO_U8 u		:1;
	VO_U8 et	:3;
	VO_U8 fs	:3;

	VO_U32 Load(VO_PBYTE pInputData, VO_U32 cbInputData);
}PenAttri;


typedef struct
{
	VO_U8 fo	:2;
	VO_U8 fr	:2;
	VO_U8 fg	:2;
	VO_U8 fb	:2;

	VO_U8 bo	:2;
	VO_U8 br	:2;
	VO_U8 bg	:2;
	VO_U8 bb	:2;

	VO_U8 er	:2;
	VO_U8 eg	:2;
	VO_U8 eb	:2;
	VO_U32 Load(VO_PBYTE pInputData, VO_U32 cbInputData);
}PenColor;
#if 0
typedef struct PenLoca
{
	VO_U8 row;
	VO_U8 colume;
	VO_VOID Init(){row = 0;colume = 0;}
	VO_U32 Load(VO_PBYTE pInputData, VO_U32 cbInputData);
};
#endif
class CCWinCharData:public CCBaseWinCharData
{
public:
	CCWinCharData(VO_U32 dwRowCnt,VO_U32 dwColCnt);
	virtual ~CCWinCharData();

	VO_U32						Load(VO_PBYTE pInputData, VO_U32 cbInputData,VO_BOOL beCreat = VO_TRUE);
	VO_S8						GetWinID(){return nWindID;}
	VO_VOID						Init();
	VO_VOID						SetWindowRectInfo(VO_U32 nAnchorID,VO_BOOL bRelPos,VO_U32 nAnchorVer,VO_U32 nAnchorHor,VO_U32 nRowCnt,VO_U32 nColCnt);
	VO_U32						SetWindowAttri(WindAttri& stWinAttri,VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_U32						SetPenAttri(PenAttri& stPenAttri,VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_U32						SetPenColor(PenColor& stPenColor,VO_PBYTE pInputData, VO_U32 cbInputData);
	pvoSubtitleInfoEntry		GetInfoEntry();
	VO_VOID						Reset();
	VO_VOID						UpdateWindowInfo(VO_BOOL bForce = VO_FALSE);
	VO_VOID						SetVisible(VO_BOOL bVis);
	VO_BOOL						MapColor2RGB(VO_U32 uRed,VO_U32 uGreen,VO_U32 uBlue,voSubtitleRGBAColor *voRGBValue);
public:
	VO_S8 nWindID; ///<window ID
	VO_U8 reserved	: 2;
	VO_U8 visible	: 1;
	VO_U8 rl		: 1;///<row lock 
	VO_U8 cl		: 1;///<column lock
	VO_U8 priority	: 3;///<priority

	VO_U8 rp		: 1;///<relative positioning 
	VO_U8 av		: 7;///<anchor vertical 

	VO_U8 ah;			///<anchor horizontal 

	VO_U8 ap		: 4;///<anchor point
	VO_U8 rc		: 4;///<row count

	VO_U8 cc;		///<: low 6;  column count

	VO_U8 ws		:3;///window style ID
	VO_U8 ps		:3;///<pen style ID
	PenColor		penColor;
	PenAttri		penAttri;
	WindAttri		windowAttri;
//	CCWinCharData*	pNext;
	VO_BOOL			m_bUpdate;
};

class CC708CharParser :public CCCharParser
{
public:
	CC708CharParser(VO_CAPTION_PARSER_INIT_INFO* pParam,VO_U32 dwMaxLinCnt = MAX_ROW_COUNT,VO_U32 dwMaxColumnCnt = MAX_COLUMN_COUNT_16_9);
	virtual ~CC708CharParser();
	VO_U32				Process(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_U32				GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer);
public:
	VO_VOID				ReStart();
protected:
	virtual VO_VOID		PushCharToBuffer(const VO_U16* pInputData, VO_U32 cbInputData);
protected:
	VO_BOOL				ParserStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData,VO_BOOL beExtCode = VO_FALSE);
	VO_U32				ParserNonStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData,VO_BOOL beExtCode = VO_FALSE);
	VO_U32				ParserC0Code(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_U32				ParserC1Code(VO_PBYTE pInputData, VO_U32 cbInputData);
	CCWinCharData		*FindWindByID(VO_S8 nWindID);
	VO_BOOL				DeleteWindowByID(VO_U8 nWindID);
	VO_VOID				SetCurWindow(VO_S8 byWindowID);
	VO_BOOL				IsWindowVisible(VO_U8 nWindID);
	VO_BOOL				GetWindowInfoList(voSubtitleInfo * pSubtitleInfo);
	
private:
	CCWinCharData*	m_WinParaTail;
	VO_S8			m_CurWindowID;
	VO_U8			m_dwWindIDSetting;
};

#ifdef _VONAMESPACE
}
#endif

#endif //_CC708CHARPARSER_H_