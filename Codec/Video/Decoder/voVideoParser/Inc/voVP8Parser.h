/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
	File:		voVP8Parser.h

	Contains:	 VP8 Parser Header File

	Written by:	Zhihuang Zou

	Change History (most recent first):
	2012-02-14		ZZH			Create file

*******************************************************************************/
#ifndef  __VO_VP8_PARSER_H__
#define  __VO_VP8_PARSER_H__
#include "voParser.h"
#include "voVideoParser.h"
#include "voVideoParserDecID.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

VO_S32 voVP8Init(VO_HANDLE *pParHandle);
VO_S32 voVP8Parser(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData);
VO_S32 voVP8GetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue);
VO_S32 voVP8Uninit(VO_HANDLE pParHandle);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VO_VP6PARSER_H__

