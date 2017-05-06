/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
	File:		voParser.h

	Contains:	Parser Header File

	Written by:	Zhihuang Zou

	Change History (most recent first):
	2012-02-103		ZZH			Create file

*******************************************************************************/
#ifndef  __VO_PARSER_H__
#define  __VO_PARSER_H__
#include "voVideo.h"
#include "voVideoParser.h"
#include "voVideoParserDecID.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//used for VP6 VP8 WMV only
#define TRUE   1
#define FALSE  0

typedef struct
{
	VO_VIDEO_CODINGTYPE nCodec;
	VO_U32 nVersion;
	VO_U32 nProfile;
	VO_U32 nLevel;
	VO_VIDEO_FRAMETYPE nFrame_type;
	VO_U32 nWidth;
	VO_U32 nHeight;
	VO_U32 isInterlace;
	VO_U32 isRefFrame;
	VO_U32 isVC1;
	VO_U32 FirstFrameisKey;
	VO_VOID* pCodecUser;
} VO_VIDEO_PARSER;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //

