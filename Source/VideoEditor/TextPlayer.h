/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		TextPlayer.h

Contains:	TextPlayer header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-18		Leon			Create file
*******************************************************************************/

#ifndef __CTextPlayer_H__
#define __CTextPlayer_H__

#pragma once

#include "videoEditorType.h"
#include "CPlayerBaseCtrl.h"
#include "CBaseTextBlend.h"
#include "CImgPlayer.h"

class CTextPlayer : public CPlayerBaseCtrl
{
public:
	CTextPlayer(VO_LIB_OPERATOR* pLibOp);
	~CTextPlayer(void);
public:
	virtual VO_S32 SetClipParam(VO_S32 nPid,VO_VOID *pData);
	virtual VO_S32 GetClipParam(VO_S32 nPid,VO_VOID *pData);	
	virtual VO_S32 SetDataSource(VO_VOID* pSource,VO_S32 nFlag);

	VO_S32 BlendText(VO_PBYTE buffer,VO_VIDEO_FORMAT vf);
	VO_S32 Init(VO_PTR hView);
private:
	CImgPlayer *m_pImagePlayer;
	CBaseTextBlend *m_pTextBlend;
	VOEDT_TEXT_FORMAT m_sTextFormat;
	VO_BOOL m_bChanged;
};

#endif