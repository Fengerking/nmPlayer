/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		GDITextBlend.h

Contains:	GDITextBlend header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/

#ifndef __GDIText_H__
#define __GDIText_H__

#include <windows.h>

#include "CBaseTextBlend.h"
#include "videoEditorType.h"


class CGDITextBlend:public CBaseTextBlend
{
public:
	CGDITextBlend(void);
	~CGDITextBlend(void);
public:
	virtual VO_S32 CreateTextBuffer(VO_CHAR* text, VOEDT_IMG_BUFFER *ppData);	
	virtual VO_S32 Init (VO_PTR hView);
	virtual VO_S32 BlendText(VO_PBYTE buffer,VO_VIDEO_FORMAT vf,VO_U32 nLeft,VO_U32 nTop);
	virtual VO_S32 SetFont(VOEDT_TEXT_FORMAT* textFormat);

private:
	 HDC m_hWinDC;
	 HDC m_hMemDC;
	 HBITMAP   m_hOldBmp ;
	 HBITMAP   m_pMemBmp ;

};

#endif