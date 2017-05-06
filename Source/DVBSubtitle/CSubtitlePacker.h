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
* @file CSubtitlePacker.cpp
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#ifndef _CSUBTITLEPACKER_H_
#define _CSUBTITLEPACKER_H_

#include "voDVBSubtitleParser.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef enum structType
{
	Struct_Type_voSubtitleInfo = 0,
	Struct_Type_voSubtitleInfoEntry,
	Struct_Type_voSubtitleRectInfo,
	Struct_Type_voSubtitleDisplayInfo,
	Struct_Type_voSubtitleRect,
	Struct_Type_voSubtitleRGBAColor,
	Struct_Type_voSubtitleDisPlayEffect,
	Struct_Type_voSubtitleTextDisplayDescriptor,
	Struct_Type_voSubtitleImageInfoDescriptor,
	Struct_Type_voSubtitleTextRowInfo,
	Struct_Type_voSubtitleImageInfo,
	Struct_Type_voSubtitleImageInfoData,
	Struct_Type_voSubtitleTextInfoEntry,
	Struct_Type_voSubtitleTextRowDescriptor,
	Struct_Type_voSubtitleStringInfo,
	Struct_Type_voSubtitleFontInfo,
	Struct_Type_voSubtitleFontEffect,
}Struct_Type;

VO_VOID InitializeStruct(VO_VOID* pIns,Struct_Type eType);
VO_VOID* NewSubTitleEntry(Struct_Type eType);
VO_VOID DelSubTitleEntry(VO_VOID* pIns,Struct_Type eType);

#ifdef _VONAMESPACE
}
#endif

#endif //_CSUBTITLEPACKER_H_