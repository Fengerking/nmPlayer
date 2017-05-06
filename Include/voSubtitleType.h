/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef _SUBTITLESTRUCT_H_
#define _SUBTITLESTRUCT_H_

#ifdef _MAC_OS
#include <stdlib.h>
#endif

typedef unsigned char VO_U8;
typedef unsigned short VO_U16;

/********** below typedef is copy from voType.h **********/
/** VO_U32 is a 32 bit unsigned quantity that is 32 bit word aligned */
#ifdef _MAC_OS
	typedef u_int32_t VO_U32;
#else
	typedef unsigned long VO_U32;	
#endif // end _MAC_OS

/* Users with compilers that cannot accept the "long long" designation should
   define the VO_SKIP64BIT macro.  It should be noted that this may cause
   some components to fail to compile if the component was written to require
   64 bit integral types.  However, these components would NOT compile anyway
   since the compiler does not support the way the component was written.
*/
#ifndef VO_SKIP64BIT
#ifdef _WIN32
/** VO_U64 is a 64 bit unsigned quantity that is 64 bit word aligned */
typedef unsigned __int64  VO_U64;
/** VO_S64 is a 64 bit signed quantity that is 64 bit word aligned */
typedef signed   __int64  VO_S64;
#else // WIN32
/** VO_U64 is a 64 bit unsigned quantity that is 64 bit word aligned */
typedef unsigned long long VO_U64;
/** VO_S64 is a 64 bit signed quantity that is 64 bit word aligned */
typedef signed long long VO_S64;
#endif // WIN32
#endif // VO_SKIP64BIT

typedef unsigned char* VO_PBYTE;

typedef enum SubtitleType
{
	voSubtitleType_None = 0,
	voSubtitleType_ClosedCaption,
	voSubtitleType_ExternFile,
	voSubtitleType_ExternImage,
	voSubtitleType_ExternText,
}voSubtitleType;


typedef enum SubtitleCodingType
{
	VO_SUBTITLE_CodingUnused = 0,
	VO_SUBTITLE_CodingCC608,
	VO_SUBTITLE_CodingCC708,
	VO_SUBTITLE_CodingTTML,
	VO_SUBTITLE_CodingSSTTML,
	VO_SUBTITLE_CodingWEBVTT,
	VO_SUBTITLE_CodingDVBTeleText,
	VO_SUBTITLE_CodingDVBSystem,
	VO_SUBTITLE_CodingSRT,
	VO_SUBTITLE_CodingSMI,
	VO_SUBTITLE_CodingLRC,
	VO_SUBTITLE_CodingSMPTETT
}voSubtitleCodingType;

typedef enum SubtitleImageType
{
	VO_IMAGE_Unused = 0,
	VO_IMAGE_RGB565 = 1,
	VO_IMAGE_RGB24 = 2,
	VO_IMAGE_RGBA32 = 3,
	VO_IMAGE_ARGB32 = 4,
	VO_IMAGE_JPEG = 5,
	VO_IMAGE_BMP = 6,
	VO_IMAGE_PNG = 7,
	VO_IMAGE_MAX = 0x7fffffff
}voSubtitleImageType;

typedef enum SubtitleFontStyle
{
	FontStyle_Default = 0,
	FontStyle_Monospaced_with_serifs,
	FontStyle_Default_Proportionally_spaced_with_serifs,
	FontStyle_Default_Monospaced_without_serifs,
	FontStyle_Default_Proportionally_spaced_without_serifs,
	FontStyle_Default_Casual,
	FontStyle_Default_Cursive,
	FontStyle_Default_Small_capitals,
	FontStyle_Monospaced,
	FontStyle_SansSerif,
	FontStyle_Serif,
	FontStyle_ProportionalSansSerif,
	FontStyle_ProportionalSerif,
	FontStyle_Times_New_Roman,
	FontStyle_Courier,
	FontStyle_Helvetica,
	FontStyle_Arial,
	FontStyle_Dom,
	FontStyle_Coronet,
	FontStyle_Gothic,
	FontStyle_MonospaceSansSerif
}voSubtitleFontStyle;

typedef enum SubtitleFondTextTag
{
	TextTag_Dialog = 0,
	TextTag_Source_speaker_ID,
	TextTag_Electronically_reproduced_voice,
	TextTag_Dialog_language_other_than_primary,
	TextTag_Dialog_Voiceover,
	TextTag_Dialog_Audible_Translation,
	TextTag_Dialog_Subtitle_Translation,
	TextTag_Dialog_Voice_quality_description,
	TextTag_Dialog_Song_Lyrics,
	TextTag_Dialog_Sound_effect_description,
	TextTag_Dialog_Musical_score_description,
	TextTag_Dialog_Expletive,
	TextTag_Dialog_Text_not_to_be_displayed,
}voSubtitleFondTextTag;

typedef struct SubtitleRect 
{
	VO_U32 	nTop;///<0xFFFFFFFF indicate the default valye
	VO_U32	nLeft;///<0xFFFFFFFF indicate the default valye
	VO_U32 	nBottom;///<0xFFFFFFFF indicate the default valye
	VO_U32	nRight;///<0xFFFFFFFF indicate the default valye
}voSubtitleRect;

typedef struct SubtitleRGBAColor    {
	VO_U8   nRed;
	VO_U8   nGreen;
	VO_U8   nBlue;
	VO_U8   nTransparency;///<0~0xFF 
}voSubtitleRGBAColor;

typedef struct SubtitleFontInfo
{					 ///<10000:for support the pixel,add the 10000 as the base,ex,10012 indicate the pixel should be 10012-10000=12pixel
	VO_U16	FontSize;///<0:STANDARD,1:SMALL, 2:LARGE ,1000:for support the percent,add the 1000 as the base,ex,1200 indicate the percent should be 1200-100=200%
	VO_U16	FontStyle;///<refer to voSubtitleFontStyle
	voSubtitleRGBAColor	stFontColor;
}voSubtitleFontInfo;

typedef struct SubtitleFontEffect
{
	VO_U32	TextTag;///<ref to voSubtitleFondTextTag
	VO_U8	Italic;
	VO_U8	Underline;
	VO_U16	EdgeType;///<0:NONE,1:RAISED,2:DEPRESSED,3:UNIFORM,4:LEFT_DROP_SHADOW,5:RIGHT_DROP_SHADOW
	VO_U32	Offset;///<0: NORMAL,1:SUBSCRIPT,2:SUPERSCRIPT
	voSubtitleRGBAColor  EdgeColor;
}voSubtitleFontEffect;

typedef struct SubtitleDisPlayEffect
{
	VO_U16					nEffectType;///<0:SNAP, 1:FADE, 2:WIPE
	VO_U16					nEffectDirection;///<0:LEFT_TO_RIGHT;1:RIGHT_TO_LEFT;2:TOP_TO_BOTTOM;3:BOTTOM_TO_TOP
	VO_U32					nEffectSpeed;///<second
}voSubtitleDisPlayEffect;

typedef struct SubtitleRectInfo
{
	voSubtitleRect			stRect;///<nBottom ==0&&nRight==0 indicate the width and height is unknow,so use the reasonable size for Rectangle
	VO_U32					nRectBorderType;///<0:NONE, 1:RAISED, 2:DEPRESSED, 3:UNIFORM,  4:SHADOW_LEFT, 5:SHADOW_RIGHT
	voSubtitleRGBAColor		stRectBorderColor;
	voSubtitleRGBAColor		stRectFillColor; 
	VO_U32					nRectZOrder;///< More negative layer values are towards the viewer:for example :0,1,2
	voSubtitleDisPlayEffect	stRectDisplayEffct;
}voSubtitleRectInfo;

typedef struct SubtitleStringInfo
{
	voSubtitleFontInfo		stFontInfo;
	voSubtitleFontEffect	stCharEffect;
}voSubtitleStringInfo;

typedef struct SubtitleTextInfoEntry
{
	VO_U32							nSize;///<Unicode string length*2
	VO_PBYTE						pString;///<Unicode string
	voSubtitleStringInfo			stStringInfo;
	struct SubtitleTextInfoEntry*	pNext;
	
}voSubtitleTextInfoEntry,*pvoSubtitleTextInfoEntry;


//text Description
typedef struct SubtitleTextDisplayDescriptor
{
	VO_U32					stWrap;///<0:No Wrap;1:Wrap
	VO_U32					nScrollDirection;///<0:LEFT_TO_RIGHT;1:RIGHT_TO_LEFT;2:TOP_TO_BOTTOM;3:BOTTOM_TO_TOP
}voSubtitleTextDisplayDescriptor;

typedef struct SubtitleTextRowDescriptor
{
	voSubtitleRectInfo		stDataBox;///<the text background box
	VO_U16					nHorizontalJustification;///<0:Left;1:Right;2:Center
	VO_U16                  nVerticalJustification;///<0:Top;1:Bottpm;2:Center
	VO_U32					nPrintDirection;///<0:Left-Right;1:Right-Left;2:Top-Bottom;3:Bottom-Top
}voSubtitleTextRowDescriptor;

typedef struct SubtitleTextRowInfo 
{
	pvoSubtitleTextInfoEntry			pTextInfoEntry;///<every voSubTitleTextInfoEntry own the same display effect
	voSubtitleTextRowDescriptor			stTextRowDes;

	struct SubtitleTextRowInfo			*pNext;
}voSubtitleTextRowInfo,*pvoSubtitleTextRowInfo;

typedef struct SubtitleImageInfoData
{
	voSubtitleImageType					nType;
	VO_U32								nWidth;
	VO_U32								nHeight;
	VO_U32								nSize;
	VO_PBYTE							pPicData;///<picture data
}voSubtitleImageInfoData;

typedef struct SubtitleImageInfoDescriptor
{
	voSubtitleRectInfo	stImageRectInfo;
}voSubtitleImageInfoDescriptor;

typedef struct SubtitleImageInfo
{
	voSubtitleImageInfoData				stImageData;
	voSubtitleImageInfoDescriptor		stImageDesp;

	struct SubtitleImageInfo *			pNext;			
}voSubtitleImageInfo,*pvoSubtitleImageInfo;

typedef struct SubtitleDisplayInfo
{
	pvoSubtitleTextRowInfo				pTextRowInfo;///<every voSubTitleTextInfoEntry own the same display effect
	voSubtitleTextDisplayDescriptor		stDispDescriptor;///<describe the common style used to all voSubTitleTextInfoEntry
	pvoSubtitleImageInfo				pImageInfo;	
}voSubtitleDisplayInfo;

typedef struct SubtitleInfoEntry ///<Every SubTitleEntry describe a palette ,and the palettes could be overlapped
{	
	VO_U32					nDuration;	///<0xFFFFFFFF indicate no duration information
	voSubtitleRectInfo		stSubtitleRectInfo; ///< information of Rect which text in  
	voSubtitleDisplayInfo	stSubtitleDispInfo; ///<all the text && picture info

	struct SubtitleInfoEntry* pNext;
}voSubtitleInfoEntry,*pvoSubtitleInfoEntry;

typedef struct SubtitleInfo
{
	VO_U64					nTimeStamp;
	pvoSubtitleInfoEntry    pSubtitleEntry;
}voSubtitleInfo;
#endif///<_SUBTITLESTRUCT_H_