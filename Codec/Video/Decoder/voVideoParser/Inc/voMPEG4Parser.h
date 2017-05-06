/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
	File:		voMPEG4Parser.h

	Contains:	 MPEG4 Parser Header File

	Written by:	Harry Yang

	Change History (most recent first):
	2012-02-14		YH			Create file

*******************************************************************************/
#ifndef  __VO_MPEG4_PARSER_H__
#define  __VO_MPEG4_PARSER_H__
#include "voParser.h"
#include "voVideoParser.h"
#include "voVideoParserDecID.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



VO_S32 voMPEG4Init(VO_HANDLE *pParHandle);
VO_S32 voMPEG4Parser(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData);
VO_S32 voMPEG4GetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue);
VO_S32 voMPEG4Uninit(VO_HANDLE pParHandle);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif   //__VO_MPEG4_PARSER_H__

