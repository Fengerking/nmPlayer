
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseTextBlend.h

Contains:	CBaseTextBlend header file

Written by:   Leon Huang
Change History (most recent first):
2011-01-13		Leon		 Create file
*******************************************************************************/

#ifndef __CBaseTextBlend_H__
#define __CBaseTextBlend_H__




#include "videoEditorType.h"
#include "SampleTransformer.h"

class CBaseTextBlend
{
public:
	CBaseTextBlend(void);
	~CBaseTextBlend(void);
public:
	virtual VO_S32 CreateTextBuffer(VO_CHAR* text, VOEDT_IMG_BUFFER *ppData);	
	virtual VO_S32 Init (VO_PTR hView = NULL);
	virtual VO_S32 BlendText(VO_PBYTE buffer,VO_VIDEO_FORMAT vf,VO_U32 nLeft,VO_U32 nTop);
	virtual VO_S32 BlendText(VO_PBYTE pInBuffer, VO_PBYTE pOutBuffer, VO_VIDEO_FORMAT vf,VOEDT_CLIP_POSITION_FORMAT positionFormat);
	virtual VO_S32 SetFont(VOEDT_TEXT_FORMAT* textFormat);
public:
	//VOEDT_CLIP_POSITION_FORMAT targetVFormat;
	VOEDT_TEXT_FORMAT m_sTextFormat;
	VO_PTR m_hView;
	VO_PBYTE m_pTextBuffer;
	VO_S32 m_nWidth;
	VO_S32 m_nHeight;
	//SIZE fontSize;

};

#endif