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
* @file CHTMLPacker.h
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#ifndef _HTMLPACKER_H_
#define _HTMLPACKER_H_

#include "voType.h"
#include <memory.h>
#include <string.h>
#include <wchar.h>
#include "CCCharData.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

//#define  MAX_SUPPORTED_TAG_COUNT 1
const VO_U32 MaxCntSupportTag = 40;
const VO_U32 MaxCntEntities = 15;
typedef enum
{
	TagName_Span = 1,
	TagName_Para = 2,
	TagName_Span_Without_Padding = 3,
	TagName_All
}TagName;

typedef struct
{
	VO_U16		UniChar;
	wchar_t		EntityChar[6];
}EntitiesMap;

class CHTMLPacker
{
public:
	CHTMLPacker();
	~CHTMLPacker();
	VO_U32		PackCharToParagraph(voSubtitleStringInfo* pChar,VO_U32 cbChar);
	VO_U32		DecodeFontParam(voSubtitleStringInfo nFontParam);
	VO_VOID		Reset();
	VO_PBYTE	GetStrBuffer(){return (VO_PBYTE)m_strParagraphy;}
	VO_U32		GetUniStrCount(){return m_CntUniChar;}
	VO_BOOL		TranslateWcharToUnicode(wchar_t *&pString);
	VO_BOOL		InitParagraph(voSubtitleStringInfo* pChar,VO_U32 & curPos,VO_U32 cbChar);
	VO_BOOL		InitLine(voSubtitleStringInfo* pChar,VO_U32 & curPos,VO_U32 cbChar);
private:
	VO_BOOL		FinishTag(TagName eTag = TagName_All);
	VO_BOOL		PushTagToString(TagName eTag);
	VO_BOOL		TranscodeSpecialChar(VO_U16 CharValue);
	VO_BOOL		AddString(const wchar_t * str);
private:
	wchar_t*	m_strParagraphy;
	VO_U32		m_CntUniChar;
	VO_U32		m_TagOrder[MaxCntSupportTag];	 

};

#ifdef _VONAMESPACE
}
#endif

#endif //_HTMLPACKER_H_