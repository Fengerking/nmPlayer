/*******************************************************************************
 *	File:		voOSPlayerData.cpp
 *	
 *	Contains:	voOSPlayerData class file
 *	
 *	Written by:	Jeff huang
 *	
 *	Change History (most recent first):
 *	2012-10-04		Jeff			Create file
 *	
 ******************************************************************************/

#include "voOSPlayerData.h"

#include "voOSFunc.h"
#include "voLog.h"
#include "voIVCommon.h"

#include "COSVomePlayer.h"
#include "voOnStreamType.h"

voOSPlayerData::voOSPlayerData()
:m_hWnd(NULL)
,m_nColorType (VOMP_COLOR_ARGB32_PACKED)
,m_nGetThumbnail(0)
,m_nZoomMode(VO_ZM_LETTERBOX)
,m_nAspectRatio(VO_RATIO_00)
,m_nAspectRatioSet(VOOSMP_RATIO_AUTO)
,m_nEnalbeSubTitleSetting(0)
,m_bSubtitleDisplay(false)
{
	memset(&m_cDraw, 0, sizeof (m_cDraw));
    memset(&m_cSubTitleSetting, 0, sizeof(m_cSubTitleSetting));
    memset(&m_cSubTitleCB, 0, sizeof(m_cSubTitleCB));
}

voOSPlayerData::~voOSPlayerData ()
{
}

int voOSPlayerData::ConvertZoomModeOS2IV(VOOSMP_ZOOM_MODE *pParam)
{
    if (NULL == pParam) {
        return VOOSMP_ERR_Pointer;
    }
    
    VOOSMP_ZOOM_MODE eMode = *((VOOSMP_ZOOM_MODE*)pParam);
    
    VO_IV_ZOOM_MODE eRet = VO_ZM_LETTERBOX;
    
	switch (eMode)
	{
        case VOOSMP_ZOOM_LETTERBOX:
            eRet = VO_ZM_LETTERBOX;
            break;
        case VOOSMP_ZOOM_PANSCAN:
            eRet = VO_ZM_PANSCAN;
            break;
        case VOOSMP_ZOOM_FITWINDOW:
            eRet = VO_ZM_FITWINDOW;
            break;
        case VOOSMP_ZOOM_ORIGINAL:
        default:
            eRet = VO_ZM_ORIGINAL;
            break;
	}
    
	return eRet;
}

int voOSPlayerData::ConvertAspectRatioOS2IV(VOOSMP_ASPECT_RATIO *pParam)
{
    if (NULL == pParam) {
        return VOOSMP_ERR_Pointer;
    }
    
    VOOSMP_ASPECT_RATIO eMode = *((VOOSMP_ASPECT_RATIO*)pParam);
    
    int eRet = VO_RATIO_11;
    
	switch (eMode)
	{
        case VOOSMP_RATIO_00:
            eRet = VO_RATIO_00;
            break;
        case VOOSMP_RATIO_11:
            eRet = VO_RATIO_11;
            break;
        case VOOSMP_RATIO_43:
            eRet = VO_RATIO_43;
            break;
        case VOOSMP_RATIO_169:
            eRet = VO_RATIO_169;
            break;
        case VOOSMP_RATIO_21:
            eRet = VO_RATIO_21;
            break;
        case VOOSMP_RATIO_2331:
            eRet = 0x00E90064;
            break;
        default:
            eRet = eMode;
            break;
	}
    
	return eRet;
}

