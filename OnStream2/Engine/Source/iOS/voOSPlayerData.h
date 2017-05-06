	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -     		*
	*																		*
	************************************************************************/
/*******************************************************************************
 *	File:		voOSPlayerData.h
 *
 *	Contains:	voOSPlayerData header file
 *
 *	Written by:	Jeff huang
 *
 *	Change History (most recent first):
 *	2012-10-04		Jeff			Create file
 *
 ******************************************************************************/

#ifndef __VO_OS_PLAYER_DATA_H__
#define __VO_OS_PLAYER_DATA_H__

#include "voOnStreamType.h"

class voOSPlayerData
{
public:
	voOSPlayerData();
	virtual ~voOSPlayerData (void);
    
    static int ConvertZoomModeOS2IV(VOOSMP_ZOOM_MODE *pParam);
    static int ConvertAspectRatioOS2IV(VOOSMP_ASPECT_RATIO *pParam);
    
    bool                        m_bSubtitleDisplay;
	int                         m_nColorType;
	int                         m_nGetThumbnail;
    int                         m_nZoomMode;
    int                         m_nAspectRatio;
    int                         m_nAspectRatioSet;
    
    int                         m_nEnalbeSubTitleSetting;
    
	void*                       m_hWnd;
    
    VOOSMP_RECT                 m_cDraw;
    VOOSMP_SUBTITLE_SETTINGS    m_cSubTitleSetting;
    VOOSMP_READ_SUBTITLE_FUNC   m_cSubTitleCB;
};

#endif // __VO_OS_PLAYER_DATA_H__
