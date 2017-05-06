/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
	File:		voVP6Parser.h

	Contains:	 VP6 Parser Header File

	Written by:	Zhihuang Zou

	Change History (most recent first):
	2012-02-103		ZZH			Create file

*******************************************************************************/
#ifndef  __VO_VP6_PARSER_H__
#define  __VO_VP6_PARSER_H__
#include "voParser.h"
#include "voVideoParser.h"
#include "voVideoParserDecID.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct 
{
	VO_U32 lowvalue;
	VO_U32 range;
	VO_U32 value;
	VO_S32 count;
	VO_U32 pos;
    VO_U8 *buffer;
	VO_U8 *buffer_end;
	VO_U32 errorflag;
	VO_U32  MeasureCost;
	VO_U32 BitCounter;
} VP6_BOOL_CODER;

typedef struct FRAME_HEADER
{
    VO_U8 *buffer;
    VO_U32 value;
    VO_S32  bits_available;
    VO_U32 pos;
} VP6_FRAME_HEADER;

VO_S32 voVP6Init(VO_HANDLE *pParHandle);
VO_S32 voVP6Parser(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData);
VO_S32 voVP6GetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue);
VO_S32 voVP6Uninit(VO_HANDLE pParHandle);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VO_VP6PARSER_H__

