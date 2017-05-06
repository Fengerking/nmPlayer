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
* @file CCCharData.h
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#ifndef _CCCHARDATA_H_
#define _CCCHARDATA_H_
#include "voType.h"
#include "fMacros.h"
#include "CharacterCode.h"
#include "voSubtitleType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef struct  
{
	VO_U16 character;
	VO_U16 nReserved;
	voSubtitleStringInfo info;
}CharInfo;

class LineInfo
{
public:
	LineInfo();
	virtual ~LineInfo();
public:
	VO_VOID		Init(VO_U32 dwColumnCnt);
	VO_VOID		SetDefaultValue(Color_Type BgColor = Color_Type_Black,Color_Type BorderColor = Color_Type_UnKnow);
public:
	CharInfo *  m_pCharOfLine;
	VO_U32		m_nCharCntCurLine;
	VO_U32		m_nCurCursorOfLine;
	voSubtitleTextRowDescriptor	m_stRowDesc;
};


VO_VOID TranslateColorToRGB(Color_Type eType,voSubtitleRGBAColor *voRGBValue);

class CCCharData
{
public:
	CCCharData(VO_U32 dwRowCnt,VO_U32 dwColCnt);
	virtual ~CCCharData();
	VO_VOID ResetBuffer();
	VO_VOID ResetRowBuffer(VO_U32 dwRowNo);
public:
	VO_U32		GetCharCntByRowNo(VO_U32 dwRowNo){return m_strCharLine[dwRowNo].m_nCharCntCurLine;} 
	VO_U32		GetCharPosByRowNo(VO_U32 dwRowNo){return m_strCharLine[dwRowNo].m_nCurCursorOfLine;}
	VO_U32		GetRowCnt(){return m_RowCnt;}
	VO_U32		PushChar(const CharInfo* pInputData, VO_U32 cbInputData,VO_U32 dwRowNo);
	LineInfo*	GetRowDataInfo(){return m_strCharLine;}
	VO_U32		GetString(CharInfo* pOutputData,VO_U32 dwSize);
	VO_U32		GetString(VO_U16* pOutputData,VO_U32 dwSize);
	VO_U32		MoveCursorToColumn(VO_U32 dwRowNo,VO_U32 dwColumnNo,VO_BOOL beDel = VO_FALSE);
	VO_BOOL		IsBufferChanged(){return m_beBufferChanged;}
	VO_VOID		SetFlag(VO_BOOL Value){m_beBufferChanged = Value;}
	VO_VOID     CopyChar(VO_U32 srcRow,VO_U32 desRow);
protected:
private:
	LineInfo*		m_strCharLine;
	VO_U32			m_RowCnt;
	VO_U32			m_ColumnCnt;
	VO_BOOL			m_beBufferChanged;
};

#ifdef _VONAMESPACE
}
#endif
#endif //_CCCHARDATA_H_