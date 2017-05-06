/*
 *  CMacOSVideoRender.cpp
 *  vompEngn
 *
 *  Created by Lin Jun on 11/30/10.
 *  Copyright 2010 VisualOn. All rights reserved.
 *
 */

#include "CMacOSVideoRender.h"
#import "voGLRenderBase.h"
#include "voLog.h"
#include "voOSFunc.h"
#include "voGLRenderFactory.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define m_voOSXRender ((voGLRenderBase*)m_pRender)

CMacOSVideoRender::CMacOSVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
:CBaseVideoRender(hInst, hView, pMemOP)
,m_bRunning(VO_FALSE)
,m_lastTimeStamp(0)
{
    m_nOutputColor = VO_COLOR_ARGB32_PACKED;
    
    m_hView = hView;
    
    m_pRender = [voGLRenderBase layer];
    [m_voOSXRender retain];
	[m_voOSXRender setView:(CALayer *)hView];
    
#ifdef _DUMP_YUV
	m_hYUV = fopen("/Users/huangjiafa/Desktop/yuv420_data_'640x480'25f.yuv", "wb");
#endif
	
#ifdef _DUMP_RGB
    m_hRGB = fopen("/Users/huangjiafa/Desktop/dataOri@640x480'25f.rgb32", "wb");
#endif		
}

CMacOSVideoRender::~CMacOSVideoRender (void)
{
    [m_voOSXRender setView:nil];
    [m_voOSXRender release];
    
#ifdef _DUMP_YUV
	if (m_hYUV)
	{
		fclose(m_hYUV);
		m_hYUV = NULL;
	}
#endif
	
#ifdef _DUMP_RGB
	if (m_hRGB)
	{
		fclose(m_hRGB);
		m_hRGB = NULL;
	}
#endif		
}

VO_U32 CMacOSVideoRender::Redraw (void)
{
	voCAutoLock lock (&m_csDraw);
	
	if (m_voOSXRender)
	{
		[m_voOSXRender redraw];
	}
	
	return VO_ERR_NONE;
}

VO_U32 	CMacOSVideoRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
#ifdef _VOLOG_INFO
    static int nTimeLastStart = voOS_GetSysTime();
    
    int nTimeTmp = voOS_GetSysTime();
    if (nTimeTmp - nTimeLastStart > (pVideoBuffer->Time - m_lastTimeStamp + 5)) {
        VOLOGI("Render enter diff time:%d", nTimeTmp - nTimeLastStart);
    }
    nTimeLastStart = nTimeTmp;
#endif
    
	voCAutoLock lock (&m_csDraw);
    
    if ((NULL == m_pRender) || (NULL == pVideoBuffer)) {
        return VO_ERR_FAILED;
    }
    
    if (0 != m_lastTimeStamp) {
        [m_voOSXRender setFrameDiffTime:(int)(pVideoBuffer->Time - m_lastTimeStamp)];
    }
    
    m_lastTimeStamp = pVideoBuffer->Time;
    
#ifdef _DUMP_YUV
	if(m_hYUV)
	{
#ifdef _YUV_PLAYER
		int nWidthUV = m_nVideoWidth / 2;
		int nHeightUV = m_nVideoHeight / 2;
		{
			int i = 0;
			for (i = 0; i < m_nVideoHeight; i++)
				fwrite (pVideoBuffer->Buffer[0] + pVideoBuffer->Stride[0] * i, m_nVideoWidth, 1, m_hYUV);
			for (i = 0; i < nHeightUV; i++)
				fwrite (pVideoBuffer->Buffer[1] + pVideoBuffer->Stride[1] * i, nWidthUV, 1, m_hYUV);
			for (i = 0; i < nHeightUV; i++)
				fwrite (pVideoBuffer->Buffer[2] + pVideoBuffer->Stride[2] * i, nWidthUV, 1, m_hYUV);
		}
#else
		for (int n=0; n<3; n++)
		{
			if (pVideoBuffer->Buffer[0] && pVideoBuffer->Stride[0]>0)
			{
				fwrite(pVideoBuffer->Buffer[n], pVideoBuffer->Stride[n]*m_nVideoHeight, 1, m_hYUV);
			}
		}
#endif //_YUV_PLAYER
	}
#endif //_DUMP_YUV
	
	if (m_fCallBack != NULL)
		return CBaseVideoRender::Render (pVideoBuffer, nStart, bWait);
    
    if (VO_GL_RET_OK == [m_voOSXRender renderYUV:pVideoBuffer]) {
        return VO_ERR_NONE;
    }
    
	VO_VIDEO_BUFFER buf;
	
    buf.Buffer[0] = (VO_BYTE*)[m_voOSXRender lockFrameData];
    int iTry = 0;
    while (NULL == buf.Buffer[0]) {
        ++iTry;
        if ((iTry > 100) || (VO_FALSE == m_bRunning)) {
            VOLOGI("LockFrameData LockFrameData fail iTry:%d m_bRunning:%d\n", iTry, m_bRunning);
            // Don't need unlock if you havn't got any buffer.
            return VO_ERR_NONE;
        }
        voOS_Sleep(2);
        buf.Buffer[0] = (VO_BYTE*)[m_voOSXRender lockFrameData];
    }
    
	// tag:linjun m_nVideoColor change to m_nOutputColor
	if (m_nOutputColor == VO_COLOR_RGB565_PACKED || m_nOutputColor == VO_COLOR_RGB555_PACKED)
	{
		buf.Stride[0] = [m_voOSXRender getTextureWidth]*2;
	}
	else if(m_nOutputColor == VO_COLOR_ARGB32_PACKED)
		buf.Stride[0] = [m_voOSXRender getTextureWidth]*4;
    else if (m_nOutputColor == VO_COLOR_RGB888_PACKED || m_nOutputColor == VO_COLOR_RGB888_PLANAR)
		buf.Stride[0] = [m_voOSXRender getTextureWidth]*3;
    
	buf.Stride[1] = 0;
	buf.Stride[2] = 0;
	
	if (!ConvertData (pVideoBuffer, &buf, nStart, bWait)) {
        [m_voOSXRender unlockFrameData:NO unlockBuffer:buf.Buffer[0]];
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;
    }
	
#ifdef _DUMP_RGB
	if(m_hRGB)
	{
		fwrite(buf.Buffer[0], 1, buf.Stride[0]*[m_voOSXRender getTextureHeight], m_hRGB);
	}
#endif	
	[m_voOSXRender unlockFrameData:YES unlockBuffer:buf.Buffer[0]];
    
    [m_voOSXRender renderFrameData];
	

#ifdef _VOLOG_INFO
    if (20 < (voOS_GetSysTime() - nTimeLastStart)) {
        VOLOGI("Render use time:%d", voOS_GetSysTime() - nTimeLastStart);
    }
#endif
    
    //VOLOGI("Render use time:%f, current:%f, buffer:%d", 1000.0 * CFAbsoluteTimeGetCurrent() - nTime,  1000.0 * CFAbsoluteTimeGetCurrent(), (int)(buf.Buffer[0]));
    
	return VO_ERR_NONE;
}

VO_U32 	CMacOSVideoRender::SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
	[m_voOSXRender setVideoSize:nWidth InputHeight:nHeight];
	
	return CBaseVideoRender::SetVideoInfo(nWidth, nHeight, nColor);
}

VO_U32 	CMacOSVideoRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor)
{	
	voCAutoLock lock (&m_csDraw);
    
    m_hView = hView;
    
    if (nil != m_pRender) {
        [m_voOSXRender setView:(CALayer *)hView];
    }
    
	return VO_ERR_NONE;
}

VO_U32 CMacOSVideoRender::SetEventCallBack (VOVIDEO_EVENT_CALLBACKPROC pCallBack, VO_PTR pUserData)
{
    CBaseVideoRender::SetEventCallBack(pCallBack, pUserData);
    if (nil != m_pRender) {
        [m_voOSXRender setEventCallBack:pCallBack userData:pUserData];
    }
    
	return VO_ERR_NONE;
}

VO_U32 CMacOSVideoRender::UpdateSize (void)
{
	voCAutoLock lock (&m_csDraw);
    
	if (m_pCCRRR == NULL)
		CreateCCRRR ();
    
	if (m_pCCRRR != NULL)
		m_pCCRRR->SetColorType (m_nVideoColor, m_nOutputColor);
    
    if (m_pCCRRR != NULL)
        m_pCCRRR->SetCCRRSize (&m_nVideoWidth, &m_nVideoHeight, &m_nVideoWidth, &m_nVideoHeight, VO_RT_DISABLE);
    if (m_pSoftCCRRR != NULL)
        m_pSoftCCRRR->SetCCRRSize (&m_nVideoWidth, &m_nVideoHeight, &m_nVideoWidth, &m_nVideoHeight, VO_RT_DISABLE);
    
	return VO_ERR_NONE;
}

VO_U32 CMacOSVideoRender::SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio)
{
    if (nil != m_pRender) {
        [m_voOSXRender setDispType:nZoomMode ration:nRatio];
    }
    
	return VO_ERR_NONE;
}

bool CMacOSVideoRender::CreateCCRRR (void)
{
    if (m_pRender && (VO_GL_SUPPORT_RGB == [m_voOSXRender getSupportType])) {
        return CBaseVideoRender::CreateCCRRR();
    }
    
    return false;
}

VO_U32 CMacOSVideoRender::GetParam (VO_U32 nID, VO_PTR pValue)
{
	if (nID == VO_VR_PMID_DrawRect)
	{
		VO_RECT * pRect = (VO_RECT *)pValue;
		if ((pRect != NULL) && (NULL != m_pRender))
		{
            Rect cR = [m_voOSXRender getDrawRect];
			pRect->left = cR.left;
			pRect->top = cR.top;
			pRect->right = cR.right;
			pRect->bottom = cR.bottom;
		}
        
		return VO_ERR_NONE;
	}
    
	return CBaseVideoRender::GetParam(nID, pValue);
}

VO_U32 CMacOSVideoRender::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if (nID == VO_PID_VIDEO_UPSIDEDOWN)
	{
		m_bVideoUpsizeDown = *(VO_BOOL*)pValue;
        
        if (m_voOSXRender)
        {
            if (VO_TRUE == m_bVideoUpsizeDown) {
                [m_voOSXRender setRotation:VO_GL_ROTATION_180FLIP];
            }
            else {
                [m_voOSXRender setRotation:VO_GL_ROTATION_0];
            }
        }
	}
    
	return VO_ERR_WRONG_PARAM_ID | VO_INDEX_SNK_VIDEO;
}

VO_U32 CMacOSVideoRender::Start (void)
{
    VOLOGI("Start");
    
    m_bRunning = VO_TRUE;
    
    if (m_voOSXRender)
    {
        [m_voOSXRender setView:(CALayer *)m_hView];
    }
    
	return VO_ERR_NONE;
}

VO_U32 CMacOSVideoRender::Pause (void)
{
    //m_bRunning = VO_FALSE;
    
    return CBaseVideoRender::Pause();
}

VO_U32 CMacOSVideoRender::Stop (void)
{
    VOLOGI("Stop");
    
    m_bRunning = VO_FALSE;
    m_lastTimeStamp = 0;
    
    if (m_voOSXRender)
    {
        [m_voOSXRender setView:nil];
    }
    
    if (m_pCCRRR != NULL)
        delete m_pCCRRR;
    m_pCCRRR = NULL;
    if (m_pSoftCCRRR != NULL)
        delete m_pSoftCCRRR;
    m_pSoftCCRRR = NULL;
    
    memset (&m_rcDisplay, 0, sizeof(m_rcDisplay));
    
	return CBaseVideoRender::Stop();
}
