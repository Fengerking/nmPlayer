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
* @file CTimedTextMLParser.h
*
* @author  Mingbo Li
* @author  Ferry Zhang
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#ifndef __CTimedTextMLParser_H__
#define __CTimedTextMLParser_H__

#include "CBaseSubtitleParser.h"
#include "CPtrList.h"


class	CTimedTextMLParser : public CBaseSubtitleParser
{
public:
	CTimedTextMLParser(void);
	virtual ~CTimedTextMLParser(void);

public:
	virtual	bool	Parse (void);
	static bool	StrToTime (char* pVal, int& nHour1, int& nMin1, int& nSec1, int& nMS1);
	static void 	FillAttribute(TT_Style* stl, char* subNodeBegin);
	VO_VOID SetBaseTime(VO_S64 llBaseTime){m_llBaseTime = llBaseTime;}
	VO_BOOL			SetBeginEndTime(char* xmlBuffer, char* outAttributeVal, VO_S64 llBaseTime = 0);
protected:
	virtual bool	ParseLine (char* pPage, char* pDivEnd);
public:
	VO_S64		m_llBaseTime;
	VO_S32		m_ColumnCnt;
	VO_S32		m_RowCnt;
	VO_U32		m_uTimeContainer;///<0:seq 1:other

};

#endif // __CTimedTextMLParser_H__
