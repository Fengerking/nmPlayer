/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		SampleTransformer.h

Contains:	SampleTransformer header file

Written by:	Leon Huang

Change History (most recent first):
2011-02-16		Leon			Create file
*******************************************************************************/
#ifndef __SampleTransformer_H__
#define __SampleTransformer_H__

#pragma once

#include "CCCRRRFunc.h"
#include "videoEditorType.h"

class CSampleTransformer
{
public:
	CSampleTransformer();
	~CSampleTransformer(void);

	void Init();
	void SetLibOp(VO_LIB_OPERATOR *pLibOp);
public:
	CCCRRRFunc *m_pCCRRR;
	VO_VIDEO_BUFFER	m_VideoOutBuffer;
	VO_LIB_OPERATOR *m_pLibOp;

	VO_S32 DoTransformer(VO_VIDEO_BUFFER *pInData,VO_VIDEO_BUFFER *pOutData,VOEDT_CLIP_POSITION_FORMAT inFormat,VOEDT_CLIP_POSITION_FORMAT outFormat,VO_S64 nStart);
	VO_S32 DoTransformer2(VO_VIDEO_BUFFER *pInData,VO_VIDEO_BUFFER *pOutData,VO_VIDEO_FORMAT inFormat,VO_VIDEO_FORMAT outFormat,VO_S64 nStart);
	
};

#endif