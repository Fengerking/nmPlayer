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
2011-08-02		Leon			Create file
*******************************************************************************/
#ifndef _WIN32

#ifndef __LinuxText_H__
#define __LinuxText_H__


#include "CBaseTextBlend.h"
#include "videoEditorType.h"


class CLinuxTextBlend:public CBaseTextBlend
{
public:
	CLinuxTextBlend(void);
	~CLinuxTextBlend(void);
public:
	virtual VO_S32 CreateTextBuffer(VO_CHAR* text, VO_VIDEO_BUFFER *ppData);	
	virtual VO_S32 Init (VO_PTR hView = NULL);
	virtual VO_S32 BlendText(VO_PBYTE buffer,VO_VIDEO_FORMAT vf,VO_U32 nLeft,VO_U32 nTop);
	virtual VO_S32 SetFont(VOEDT_TEXT_FORMAT* textFormat);

private:
	// HDC m_hWinDC;
	//	 HDC m_hMemDC;
	// HBITMAP   m_hOldBmp ;
	// HBITMAP   m_pMemBmp ;

};

#endif

#endif // _WIN32