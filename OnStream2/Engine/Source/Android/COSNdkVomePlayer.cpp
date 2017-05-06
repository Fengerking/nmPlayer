/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
 
VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

/************************************************************************
 * @file COSNdkVomePlayer.cpp
 *  wrap class of player for jni.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <jni.h>
#include <cpu-features.h>
#include <sys/system_properties.h>
#include "vodlfcn.h"
#include "voVideoParser.h"

#include "voAMediaCodec.h"
#include "CJniEnvUtil.h"
#include "voOSFunc.h"
#include "COSNdkVomePlayer.h"
#include "COSNdkVideoRender.h"
#include "CJavaParcelWrapOSMP.h"

#define  LOG_TAG    "COSNdkVomePlayer"
#include "voLog.h"

COSNdkVomePlayer::COSNdkVomePlayer()
	: m_lRenderType (VOOSMP_RENDER_TYPE_NATIVE)
	, m_pVideoRender (NULL)
	, m_pSurface (0)
	, m_pBitmap(0)
	, m_nZoomMode(VOOSMP_ZOOM_LETTERBOX)
	, m_nColorType (VOMP_COLOR_ARGB32_PACKED)
	, m_nVideoRndNum (0)
	, m_nVideoBufferSize(0)
	, mADumpFd(-1)
	, m_bShowBlackScreen(VO_FALSE)
{
	memset(&m_sBufVideo, 0, sizeof(VOMP_BUFFERTYPE));
	memset(&m_sBufAudio, 0, sizeof(VOMP_BUFFERTYPE));
	memset(&audioRenderFormat, 0, sizeof(VOOSMP_AUDIO_RENDER_FORMAT));

	m_pBufVideo = &m_sBufVideo;
	m_pBufAudio = &m_sBufAudio;

	memset (&m_fmtVideo, 0, sizeof (VOMP_VIDEO_FORMAT));

	m_sLibFunc.pUserData = this;
	m_sLibFunc.LoadLib = vomtLoadLib;
	m_sLibFunc.GetAddress = vomtGetAddress;
	m_sLibFunc.FreeLib = vomtFreeLib;

	m_nStartSysTime = voOS_GetSysTime();

	if(access("/data/local/ndk_audio_dump.txt", F_OK) == 0)
	{
		mADumpFd = open("/data/local/dump/ndkdump.pcm", O_CREAT|O_RDWR|O_TRUNC, 0644);
		if(mADumpFd < 0)
		{
			VOLOGE("can not create the file /data/local/dump/ndkdump.pcm, the reason is %s\n", strerror(errno));
		}
	}
}

COSNdkVomePlayer::~COSNdkVomePlayer ()
{
	Uninit();

	if(m_pVideoRender) 
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}
	if(mADumpFd > 0)
	{
		close(mADumpFd);
		mADumpFd = -1;
	}

}

int	COSNdkVomePlayer::Init(void)
{
	m_nStartSysTime = voOS_GetSysTime();
	int nRC = COSVomePlayer::Init();
	if(nRC) return nRC;

	nRC = COSVomeEngine::SetParam (VOOSMP_PID_DRAW_COLOR, &m_nColorType);

	int nRndNum = 6;
	nRC = COSVomeEngine::SetParam (VOMP_PID_VIDEO_RNDNUM, &nRndNum);

	int nPlayMode = VOMP_PULL_MODE;
	nRC = COSVomeEngine::SetParam(VOMP_PID_AUDIO_PLAYMODE, &nPlayMode);
	if (nRC != VOOSMP_ERR_None) return ReturnCode(nRC);
	nRC = COSVomeEngine::SetParam(VOMP_PID_VIDEO_PLAYMODE, &nPlayMode);
	if (nRC != VOOSMP_ERR_None) return ReturnCode(nRC);

	int CPUNum = voOS_GetCPUNum();
	nRC = SetParam (VOOSMP_PID_CPU_NUMBER, &CPUNum);

	return nRC;
}

int	COSNdkVomePlayer::Uninit(void)
{
	int nStartTime = voOS_GetSysTime();
	int nRC = COSVomePlayer::Uninit();

	if(m_pVideoRender) 
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}

	int nEndTime = voOS_GetSysTime();

	VOLOGI("Uninit() using the Time %d", nEndTime - nStartTime);

	nEndTime = nStartTime;

	return nRC;
}

int	COSNdkVomePlayer::SetDataSource (void * pSource, int nFlag)
{
	int nRC = InitVideoRender();

	nRC = COSVomePlayer::SetDataSource(pSource, nFlag);
	if(nRC) return nRC;	
	
	return nRC;
}

int	COSNdkVomePlayer::SetView(void* pView)
{
	if (pView == NULL)
		return VOOSMP_ERR_Pointer;

	int nRC = VOOSMP_ERR_None;

	voCAutoLock lockGetAudio (&m_mtGetAudio);
	voCAutoLock lockGetVideo (&m_mtGetVideo);

	m_pSurface = (jobject)pView;
	m_pBitmap = (jobject)pView;

	if(m_lRenderType == VOOSMP_RENDER_TYPE_NATIVE || 
	   m_lRenderType == VOOSMP_RENDER_TYPE_NATIVE_C ||
	   m_lRenderType == VOOSMP_RENDER_TYPE_HW_RENDER ||
	   VOOSMP_RENDER_TYPE_JMHW_RENDER == m_lRenderType )
	{
		m_pSurface = (jobject)pView;
		if(m_pVideoRender)
			nRC = m_pVideoRender->SetParam(VO_RENDER_PID_SURFACE, m_pSurface);

		if(m_lRenderType == VOOSMP_RENDER_TYPE_HW_RENDER)
		{
			void* ppView = NULL;

			if(m_pVideoRender)
				m_pVideoRender->GetParam(VO_RENDER_PID_SURFACE, &ppView);

			nRC = COSVomeEngine::SetParam(VOMP_PID_DRAW_VIEW, ppView);
		}
		else if(VOOSMP_RENDER_TYPE_JMHW_RENDER == m_lRenderType)
		{
			nRC = COSVomeEngine::SetParam(VOMP_PID_DRAW_VIEW, pView);
		}
	}
	else if(m_lRenderType == VOOSMP_RENDER_TYPE_BITMAP)
	{
		m_pBitmap = (jobject)pView;
		if(m_pVideoRender)
			nRC = m_pVideoRender->SetParam(VO_RENDER_PID_BITMAP, m_pBitmap);
	}

	VOMP_STATUS mpStatus = VOMP_STATUS_INIT;
	GetStatus((int *)&mpStatus);
	if(mpStatus == VOMP_STATUS_RUNNING)
		COSVomeEngine::SetParam(VOMP_PID_VIDEO_PLAY_BACK, pView);

	return nRC;
}

int	COSNdkVomePlayer::InitVideoRender()
{
	int nRC = VOOSMP_ERR_None;

	if(m_pVideoRender)
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}

	m_pVideoRender = new COSNdkVideoRender();
	if(m_pVideoRender == NULL)
		return VOOSMP_ERR_OutMemory;

	nRC = m_pVideoRender->SetParam(VO_RENDER_PID_LIBOP, m_pLibFunc);
	if(nRC) return nRC;
	nRC = m_pVideoRender->SetParam(VO_RENDER_PID_JAVAVM, m_JavaVM);
	if(nRC) return nRC;
	nRC = m_pVideoRender->SetParam(VO_RENDER_PID_RENDERTYPE, &m_lRenderType);
	if(nRC)	return nRC;
	nRC = m_pVideoRender->SetParam(VO_RENDER_PID_DRAW_COLOR, &m_nColorType);
	if(nRC) return nRC;
	m_pVideoRender->SetParam(VOOSMP_PID_FUNC_LIB, m_szPathLib);
	if(m_lRenderType == VOOSMP_RENDER_TYPE_NATIVE || 
	   m_lRenderType == VOOSMP_RENDER_TYPE_NATIVE_C || 
	   m_lRenderType == VOOSMP_RENDER_TYPE_HW_RENDER ||
	   m_lRenderType == VOOSMP_RENDER_TYPE_JMHW_RENDER	)
	{
		nRC = m_pVideoRender->SetParam(VO_RENDER_PID_SURFACE, m_pSurface);
		if(nRC) return nRC;	
	}
	else if(m_lRenderType == VO_RENDER_PID_BITMAP)
	{
//		nRC = m_pVideoRender->SetParam(VO_RENDER_PID_LIBOP, m_pBitmap);
		nRC = m_pVideoRender->SetParam(VO_RENDER_PID_BITMAP, m_pBitmap);
		if(nRC) return nRC;
	}

	if(m_lRenderType == VOOSMP_RENDER_TYPE_HW_RENDER)
	{
		void* pView = NULL;

		if(m_pVideoRender)
			m_pVideoRender->GetParam(VO_RENDER_PID_SURFACE, &pView);

		nRC = COSVomeEngine::SetParam(VOMP_PID_DRAW_VIEW, pView);
	}
	else if (m_lRenderType == VOOSMP_RENDER_TYPE_JMHW_RENDER)
	{
		//LOGE("VIEW_SET   m_pSurface = %p\n", m_pSurface);
	}
	return nRC;
}

int	COSNdkVomePlayer::Run ()
{
	voCAutoLock lockGetAudio (&m_mtGetAudio);
	voCAutoLock lockGetVideo (&m_mtGetVideo);

	return COSVomePlayer::Run ();
}

int	COSNdkVomePlayer::Pause ()
{
	voCAutoLock lockGetAudio (&m_mtGetAudio);
	voCAutoLock lockGetVideo (&m_mtGetVideo);

	int nRC = VOOSMP_ERR_None;
	nRC = COSVomePlayer::Pause ();

	if(VO_TRUE == m_bShowBlackScreen)
		nRC = ShowBlackFrame();

	return nRC;
}
	
int	COSNdkVomePlayer::Stop ()
{		
	int nStartTime = voOS_GetSysTime();
	voCAutoLock lockGetAudio (&m_mtGetAudio);
	voCAutoLock lockGetVideo (&m_mtGetVideo);	

	int nRC = COSVomePlayer::Stop ();
	int nEndTime = voOS_GetSysTime();

	VOLOGI("Stop() using the Time %d", nEndTime - nStartTime);
	nEndTime = nStartTime;

	m_nStartSysTime = voOS_GetSysTime();
	m_nVideoRndNum = 0;

	//Rogine add black frame to fix issue #24365
	int res = ShowBlackFrame();
	if (res)
	{
		VOLOGW("Failed to ShowBlackFrame!");
	}
	else
	{
		VOLOGI("Success to ShowBlackFrame!");
	}
	m_nVideoBufferSize = 0;
	
	return nRC;
}

int	COSNdkVomePlayer::Flush (void)
{
	voCAutoLock lockGetAudio (&m_mtGetAudio);
	voCAutoLock lockGetVideo (&m_mtGetVideo);
	m_nVideoBufferSize = 0;

	return COSVomePlayer::Flush ();
}

int	COSNdkVomePlayer::SetPos (int nCurPos)
{
	m_nStartSysTime = voOS_GetSysTime();
	m_nVideoRndNum = 0;
	m_nVideoBufferSize = 0;
	
	voCAutoLock lockGetAudio (&m_mtGetAudio);
	voCAutoLock lockGetVideo (&m_mtGetVideo);

	return COSVomePlayer::SetPos(nCurPos);
}
	
int	COSNdkVomePlayer::Close()
{
	int nStartTime = voOS_GetSysTime();
	int nRC = Stop();
	int nEndTime = voOS_GetSysTime();

	VOLOGI("Close() using the Time %d", nEndTime - nStartTime);
	nEndTime = nStartTime;

    memset (&m_fmtVideo, 0, sizeof (VOMP_VIDEO_FORMAT));
    
	return nRC;
}

int COSNdkVomePlayer::SetParam (int nID, void * pValue)
{

	int nRC = VOOSMP_ERR_None;
	m_nVideoBufferSize = 0;

	if (nID == VOOSMP_PID_BITMAP_HANDLE)
	{
		VOLOGI(" VOOSMP_PID_BITMAP_HANDLE -> VO_RENDER_PID_BITMAP");
		m_pVideoRender->SetParam (VO_RENDER_PID_BITMAP,  pValue);
		return VOOSMP_ERR_None;
	}
	else if (nID == VOOSMP_PID_IOMX_PROBE)
	{
		void *pView = pValue;
		if (pView == NULL)
			return VOOSMP_ERR_Pointer;

		int nRC = VOOSMP_ERR_None;

		voCAutoLock lockGetVideo (&m_mtGetVideo);

		m_pSurface = (jobject)pView;
		m_pBitmap = (jobject)pView;

		if(m_lRenderType == VOOSMP_RENDER_TYPE_NATIVE || 
				m_lRenderType == VOOSMP_RENDER_TYPE_NATIVE_C ||
				m_lRenderType == VOOSMP_RENDER_TYPE_HW_RENDER ||
				VOOSMP_RENDER_TYPE_JMHW_RENDER == m_lRenderType )
		{
			m_pSurface = (jobject)pView;
			if(m_pVideoRender)
				nRC = m_pVideoRender->SetParam(VO_RENDER_PID_SURFACE, m_pSurface);

			if(m_lRenderType == VOOSMP_RENDER_TYPE_HW_RENDER)
			{
				void* ppView = NULL;
				InitVideoRender();
				long r_type = 5;
				m_pVideoRender->SetParam(VO_RENDER_PID_RENDERTYPE, &r_type);

				if(m_pVideoRender)
					m_pVideoRender->GetParam(VO_RENDER_PID_SURFACE, &ppView);

				nRC = COSVomeEngine::SetParam(VOMP_PID_IOMX_PROBE, ppView);
				VOLOGI("here probe nRC is %d",nRC);
				return nRC;
			}
			else if(VOOSMP_RENDER_TYPE_JMHW_RENDER == m_lRenderType)
			{
				//LOGE("VIEW_SET   pView = %p\n", pView);
				nRC = COSVomeEngine::SetParam(VOMP_PID_DRAW_VIEW, pView);
			}
		}
		else if(m_lRenderType == VOOSMP_RENDER_TYPE_BITMAP)
		{
			m_pBitmap = (jobject)pView;
			if(m_pVideoRender)
				nRC = m_pVideoRender->SetParam(VO_RENDER_PID_BITMAP, m_pBitmap);
		}

		VOMP_STATUS mpStatus = VOMP_STATUS_INIT;
		GetStatus((int *)&mpStatus);
		if(mpStatus == VOMP_STATUS_RUNNING)
			COSVomeEngine::SetParam(VOMP_PID_VIDEO_PLAY_BACK, pView);

		return nRC;
	}
	else if (nID == VOOSMP_PID_DRAW_COLOR)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;

		m_nColorType = ConvertColorOS2MP((VOOSMP_COLORTYPE *)pValue);

        if(VOOSMP_RENDER_TYPE_NATIVE_C == m_lRenderType)
        {
			char m_szProp[64];
			memset(m_szProp, 0, 64);
			__system_property_get ("ro.build.version.release", m_szProp);
			if(strstr (m_szProp, "4.") == m_szProp)
				m_nColorType = VOOSMP_COLOR_YUV_PLANAR420;
			else
				m_nColorType = VOOSMP_COLOR_ARGB32_PACKED;
        }

		if(m_pVideoRender)
		{
			m_pVideoRender->SetParam(VO_RENDER_PID_DRAW_COLOR, &m_nColorType);
		}

        if(VOOSMP_RENDER_TYPE_NATIVE_C == m_lRenderType)
		{
			return COSVomePlayer::SetParam(nID, &m_nColorType);
		}

		if(m_lRenderType == VOOSMP_RENDER_TYPE_HW_RENDER)
		{
			m_nColorType = (unsigned int)0x7F000001;
			return VOOSMP_ERR_None;
		}
		else if (VOOSMP_RENDER_TYPE_JMHW_RENDER ==  m_lRenderType)
		{
			m_nColorType = VO_ACODEC_COLOR_TYPE;
			return VOOSMP_ERR_None;
		}
	}
	else if(nID == VOOSMP_PID_VIDEO_RENDER_TYPE)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;

		m_lRenderType = *((int*)pValue);

		if(m_pVideoRender)
		{
			m_pVideoRender->SetParam(VO_RENDER_PID_RENDERTYPE, &m_lRenderType);
			VOLOGI("RENDERTYPE is %d!!!", m_lRenderType);
		}

		if(m_lRenderType == VOOSMP_RENDER_TYPE_HW_RENDER)
		{
			int nRndNum = 1;
			nRC = COSVomeEngine::SetParam (VOMP_PID_VIDEO_RNDNUM, &nRndNum);

			m_nColorType = (unsigned int)0x7F000001;
			nRC = COSVomeEngine::SetParam (VOMP_PID_DRAW_COLOR, &m_nColorType);
		}
		else if(VOOSMP_RENDER_TYPE_JMHW_RENDER == m_lRenderType)
		{
			// make the vome2 engine use main thread 
			int nRndNum = 1;
			nRC = COSVomeEngine::SetParam (VOMP_PID_VIDEO_RNDNUM, &nRndNum);
			nRC = COSVomeEngine::SetParam(VOMP_PID_JAVA_VM, m_JavaVM);

			m_nColorType = VO_ACODEC_COLOR_TYPE;
			nRC = COSVomeEngine::SetParam (VOMP_PID_DRAW_COLOR, &m_nColorType);
		}
        else if(VOOSMP_RENDER_TYPE_NATIVE_C == m_lRenderType)
        {
				char m_szProp[64];
				memset(m_szProp, 0, 64);
				__system_property_get ("ro.build.version.release", m_szProp);
				if(strstr (m_szProp, "4.") == m_szProp)
					m_nColorType = VOMP_COLOR_YUV_PLANAR420;
				else
					m_nColorType = VOMP_COLOR_ARGB32_PACKED;
                nRC = COSVomeEngine::SetParam (VOMP_PID_DRAW_COLOR, &m_nColorType);
        }
		else
		{
			//m_nColorType = VOMP_COLOR_ARGB32_PACKED;
			int nRndNum = 6;
			nRC = COSVomeEngine::SetParam (VOMP_PID_VIDEO_RNDNUM, &nRndNum);
			nRC = COSVomeEngine::SetParam (VOMP_PID_DRAW_COLOR, &m_nColorType);
		}

		return nRC;
	}
	else if(nID == VOOSMP_PID_DRAW_RECT)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;
		VOOSMP_RECT *pRect = NULL;
		pRect = (VOOSMP_RECT *)pValue;
		if(pRect != NULL)
		{
			m_rcDraw.left = pRect->nLeft;
			m_rcDraw.top = pRect->nTop;
			m_rcDraw.right = pRect->nRight;
			m_rcDraw.bottom = pRect->nBottom;
		}

		if(m_pVideoRender)
		{
			m_pVideoRender->SetParam(VO_RENDER_PID_DRAW_RECT, &m_rcDraw);
		}

		nRC = COSVomeEngine::SetParam(VOMP_PID_DRAW_RECT, &m_rcDraw); 

		return nRC;
	}
	else if(nID == VOOSMP_PID_ZOOM_MODE)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;

		m_nZoomMode = *((VOOSMP_ZOOM_MODE *)pValue);

		if(m_pVideoRender)
		{
			m_pVideoRender->SetParam(VO_RENDER_PID_DRAW_MODE, &m_nZoomMode);
		}

		int ZoomMode = m_nZoomMode;

		if(m_nZoomMode == VOOSMP_ZOOM_LETTERBOX)
		{
			ZoomMode = VO_ZM_LETTERBOX;
		}
		else if(m_nZoomMode == VOOSMP_ZOOM_PANSCAN)
		{
			ZoomMode = VO_ZM_PANSCAN;		
		}
		else if(m_nZoomMode == VOOSMP_ZOOM_FITWINDOW)
		{
			ZoomMode = VO_ZM_FITWINDOW;
		}
		else if(m_nZoomMode == VOOSMP_ZOOM_ORIGINAL)
		{
			ZoomMode = VO_ZM_ORIGINAL;
		}
		else if(m_nZoomMode == VOOSMP_ZOOM_ZOOMIN)
		{
			ZoomMode = VO_ZM_ZOOMIN;
		}

		nRC = COSVomeEngine::SetParam(VOMP_PID_ZOOM_MODE, &ZoomMode); 

		return nRC;
	}
	else if(nID == VOOSMP_PID_PLAYER_PATH)
	{
		if(pValue == NULL)
			return VOOSMP_ERR_Pointer;

		vostrcpy(m_szPathLib, (VO_TCHAR *)pValue);
		nRC = SetParam(VOOSMP_PID_FUNC_LIB, &m_sLibFunc);
		if (nRC != VOOSMP_ERR_None) return ReturnCode(nRC);
	}
	else if(nID == VOOSMP_PID_VIEW_ACTIVE)
	{
		return VOOSMP_ERR_None; 
	}
	else if(VOOSMP_PID_APPLICATION_RESUME == nID || VOOSMP_PID_VIDEO_REDRAW == nID)
	{
		if(NULL == m_pVideoRender || VO_TRUE == m_bShowBlackScreen)
			return VOOSMP_ERR_None;

		memset (m_pBufVideo, 0, sizeof (VOMP_BUFFERTYPE));
		m_pBufVideo->pBuffer = NULL;
		m_pBufVideo->nSize = sizeof(VOMP_BUFFERTYPE);
		COSVomePlayer::SetParam(nID, pValue);
		if(VOOSMP_RENDER_TYPE_BITMAP == m_lRenderType)
		{
			nRC = m_pVideoRender->SurfaceLock(m_pBufVideo, pValue);
		}
		else
		{//// some customer's player will crash when get value from pValue, add this to avoid crash.
			int param = 2; 
			nRC = m_pVideoRender->SurfaceLock(m_pBufVideo, &param);
		}
		if(nRC == VOOSMP_ERR_FormatChange)
		{
			COSVomeEngine::GetParam (VOMP_PID_VIDEO_FORMAT, &m_fmtVideo);
			if(m_pVideoRender)
			{
				m_pVideoRender->SetParam(VO_RENDER_PID_VIDEOFORAMT, &m_fmtVideo);
			}
			m_pVideoRender->SurfaceUnlock(m_pBufVideo, pValue);
			return VOOSMP_ERR_Retry;
		}
		else if(nRC != VOOSMP_ERR_None)
		{
			VOLOGI ("Surface Lock error %d", nRC);
			return nRC;
		}

		VOMP_VIDEO_BUFFER sYUVBufferInfo;
		memset(&sYUVBufferInfo, 0, sizeof(VOMP_VIDEO_BUFFER));
		sYUVBufferInfo.Buffer[0] = (char *)m_pBufVideo->pBuffer;
		nRC = m_pVideoRender->GetParam(VO_RENDER_PID_YUVBUFFERINFO, &sYUVBufferInfo);
		if(VOOSMP_ERR_None == nRC)
		{
			m_pBufVideo->nFlag |= VOMP_FLAG_BUFFER_YUVBUFFER;
			m_pBufVideo->pData = &sYUVBufferInfo;
		}

		nRC = GetVideoBuffer (&m_pBufVideo);
		nRC = m_pVideoRender->SurfaceUnlock(m_pBufVideo, pValue);
		internalReturnVideoBuffer(1);
		return nRC;
	}
	else if(VOOSMP_PID_AUDIO_DECODER_TYPE == nID)
	{
		if(*((int*)pValue) == 1)
			COSVomeEngine::SetParam(VOMP_PID_JAVA_VM, m_JavaVM);
	}
	else if(VOOSMP_PID_CLEAR_VIDEO_AREA == nID)
	{
		if(NULL == pValue)
			return VOOSMP_ERR_Pointer;

		m_bShowBlackScreen = *(VO_BOOL*)pValue;
		return VOOSMP_ERR_None;
	}
	else if(VOOSMP_PID_AUDIO_RENDER_FORMAT == nID)
	{
		if(NULL != pValue)
		{
			audioRenderFormat.SampleRate = ((VOOSMP_AUDIO_RENDER_FORMAT*)pValue)->SampleRate;
			audioRenderFormat.Channels = ((VOOSMP_AUDIO_RENDER_FORMAT*)pValue)->Channels;
			audioRenderFormat.SampleBits = ((VOOSMP_AUDIO_RENDER_FORMAT*)pValue)->SampleBits;
			audioRenderFormat.BufferSize = ((VOOSMP_AUDIO_RENDER_FORMAT*)pValue)->BufferSize;
		}
		if(NULL != m_pVideoRender && NULL != pValue)
		{
			if(VOOSMP_ERR_None == m_pVideoRender->SetParam(nID, pValue))
				return VOOSMP_ERR_None;
		}
		return VOOSMP_ERR_Pointer;
	}

	return COSVomePlayer::SetParam(nID, pValue);
}

int COSNdkVomePlayer::GetParam (int nID, void * pValue)
{
	if (nID == VOOSMP_PID_VIDEO_RENDER_TYPE)
	{
		*((int*)pValue) = m_lRenderType;
		return VOOSMP_ERR_None;
	}
	else if(nID == VOOSMP_PID_PLAYER_TYPE)
	{
		*((int*)pValue) = VOOSMP_VOME2_PLAYER;
		return VOOSMP_ERR_None;
	}
	else if(VOOSMP_PID_AUDIO_RENDER_LATENCY == nID)
	{
		if(NULL != m_pVideoRender && NULL != pValue)
		{
			if(VOOSMP_ERR_None == m_pVideoRender->GetParam(nID, pValue))
			{
				VOLOGI("Get audio buffer time from VR succeed !");
			}
			else if(0 != audioRenderFormat.SampleRate && 0 != audioRenderFormat.Channels && 0 != audioRenderFormat.SampleBits)
			{
				VOLOGI("Cannot get audio buffer time from VR !");
				*(int*)pValue = audioRenderFormat.BufferSize * 1000 / (audioRenderFormat.SampleRate * audioRenderFormat.Channels * audioRenderFormat.SampleBits / 8) + 100;
			}
			return VOOSMP_ERR_None;
		}
		return VOOSMP_ERR_Pointer;
	}


	return COSVomePlayer::GetParam(nID, pValue);
}

int COSNdkVomePlayer::SetJavaVM(void *pJavaVM, void* jobj)
{
	m_JavaVM = (JavaVM *)pJavaVM;
	m_jOSmpEng = (jobject)jobj;
	return VOOSMP_ERR_None;
}

int COSNdkVomePlayer::GetVideoData (unsigned char * pData, int nSize, JNIEnv *pEnv)
{
	voCAutoLock lockGetVideo (&m_mtGetVideo);
	memset (m_pBufVideo, 0, sizeof (VOMP_BUFFERTYPE));
	
	m_pBufVideo->nFlag = VOMP_FLAG_BUFFER_TRY_ONLY;
	int nRC = GetVideoBuffer (&m_pBufVideo);
	if (nRC == VOOSMP_ERR_FormatChange)
	{
		COSVomeEngine::GetParam (VOMP_PID_VIDEO_FORMAT, &m_fmtVideo);
		if(m_pVideoRender)
		{
			m_pVideoRender->SetParam(VO_RENDER_PID_VIDEOFORAMT, &m_fmtVideo);
		}
        
        if(m_nVideoRndNum == 0)
        {
            //Rogine add black frame to fix issue #24365
            if(VOOSMP_RENDER_TYPE_NATIVE == m_lRenderType)
            {
                ShowBlackFrame ();
                voOS_Sleep (30);
            }
        }

		VOLOGI("VOOSMP_ERR_FormatChange for VOMP_FLAG_BUFFER_TRY_ONLY %d %d", m_fmtVideo.Width, m_fmtVideo.Height);

		return VOOSMP_ERR_FormatChange;
	}
	else if(nRC != VOOSMP_ERR_None)
	{
		//Rogine delete sleep in here since there is sleep in vompEngn, mainly to fixed issue on android 4.4
		//voOS_Sleep (2);
		return nRC;
	}

//	VOLOGI("m_nVideoRndNum %d, %d %d nRC 0x%08X", m_nVideoRndNum, m_fmtVideo.Width, m_fmtVideo.Height, nRC);

	if(m_nVideoRndNum == 0 && (nRC == VOOSMP_ERR_None || nRC == VOOSMP_ERR_FormatChange))
	{
		// check if resolution changed
		VOMP_VIDEO_FORMAT fmtVideo;
		memset(&fmtVideo, 0, sizeof(VOMP_VIDEO_FORMAT));
		COSVomeEngine::GetParam(VOMP_PID_VIDEO_FORMAT, &fmtVideo);

		if(m_fmtVideo.Width != fmtVideo.Width || m_fmtVideo.Height != fmtVideo.Height)
		{
			memcpy(&m_fmtVideo, &fmtVideo, sizeof(VOMP_VIDEO_FORMAT));

			if(m_pVideoRender)
				m_pVideoRender->SetParam(VO_RENDER_PID_VIDEOFORAMT, &m_fmtVideo);

			VOLOGI("VOOSMP_ERR_FormatChange for VOMP_PID_VIDEO_FORMAT %d %d", m_fmtVideo.Width, m_fmtVideo.Height);

			return VOOSMP_ERR_FormatChange;
		}
	}
	
	if(m_lRenderType == VOOSMP_RENDER_TYPE_HW_RENDER || m_lRenderType == VOOSMP_RENDER_TYPE_JMHW_RENDER || 
		(m_pSurface == 0 && (m_lRenderType == VOOSMP_RENDER_TYPE_NATIVE || m_lRenderType == VOOSMP_RENDER_TYPE_NATIVE_C)))
	{
		m_nVideoRndNum++;
		return VOOSMP_ERR_Retry;
	}

	memset (m_pBufVideo, 0, sizeof (VOMP_BUFFERTYPE));
	m_pBufVideo->pBuffer = pData;
	m_pBufVideo->nSize = nSize;

	if (m_pVideoRender == NULL)
		return VOOSMP_ERR_None;

	nRC = m_pVideoRender->SurfaceLock(m_pBufVideo, (void *)pEnv);

	if(nRC == VOOSMP_ERR_FormatChange)
	{
		COSVomeEngine::GetParam (VOMP_PID_VIDEO_FORMAT, &m_fmtVideo);
		if(m_pVideoRender)
		{
			m_pVideoRender->SetParam(VO_RENDER_PID_VIDEOFORAMT, &m_fmtVideo);
		}
		
		m_pVideoRender->SurfaceUnlock(m_pBufVideo, (void *)pEnv);
        
		return VOOSMP_ERR_FormatChange;
	}
	else if(nRC != VOOSMP_ERR_None)
	{
		VOLOGI ("Surface Lock error %d", nRC);
		return nRC;
	}

	VOMP_VIDEO_BUFFER sYUVBufferInfo;
	memset(&sYUVBufferInfo, 0, sizeof(VOMP_VIDEO_BUFFER));
	sYUVBufferInfo.Buffer[0] = (char *)m_pBufVideo->pBuffer;
	nRC = m_pVideoRender->GetParam(VO_RENDER_PID_YUVBUFFERINFO, &sYUVBufferInfo);
	if(VOOSMP_ERR_None == nRC)
	{
		m_pBufVideo->nFlag |= VOMP_FLAG_BUFFER_YUVBUFFER;
		m_pBufVideo->pData = &sYUVBufferInfo;
	}

	nRC = GetVideoBuffer (&m_pBufVideo);
	if(VOOSMP_ERR_None == nRC)
		m_nVideoBufferSize = m_pBufVideo->nReserve;
	else
	{
		VOLOGI ("get video data error %d(%x), should not run in", nRC, nRC);
	}

	nRC = m_pVideoRender->SurfaceUnlock(m_pBufVideo, (void *)pEnv);
	
	// rendered over, so return buffer immediately
	internalReturnVideoBuffer(1);

	if(m_nVideoRndNum == 0)
	{
		if(m_bSeeking)
		{
			VOLOGI ("Seeking to the first frame Using Time %d", voOS_GetSysTime() - m_nStartSysTime);
		}
		else
		{
			VOLOGI ("Open to the first frame Using Time %d", voOS_GetSysTime() - m_nStartSysTime);
		}
	}

	m_nVideoRndNum++;

	if(nRC == VOOSMP_ERR_None)
		return VOOSMP_ERR_Retry;

	return VOOSMP_ERR_None;
}
	
int COSNdkVomePlayer::GetAudioData (unsigned char * pData, unsigned int nSize, int * pFilled)
{
	voCAutoLock lockGetAudio (&m_mtGetAudio);

	*pFilled = 0;

	memset (m_pBufAudio, 0, sizeof (VOMP_BUFFERTYPE));
	m_pBufAudio->pBuffer = pData;
	m_pBufAudio->nSize = nSize;

	int nRC = GetAudioBuffer (&m_pBufAudio);
	if (nRC == 0)
	{
		*pFilled = m_pBufAudio->nSize;
		// added by gtxia for debuging
		if(mADumpFd > 0)
		{
			write(mADumpFd, m_pBufAudio->pBuffer, *pFilled);
		}
	}

	if(nRC != VOOSMP_ERR_None)
		voOS_Sleep (2);

	return nRC;
}

int COSNdkVomePlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	int nRC = COSVomePlayer::HandleEvent(nID, pParam1, pParam2);
	if(nRC) return nRC;

	int nParam1 = 0;
	int nParam2 = 0;

	if(pParam1)
		nParam1 = *(((int *)pParam1));
	if(pParam2)	
		nParam2 = *(((int *)pParam2));

	CJniEnvUtil	env(m_JavaVM);

	if(env.getEnv() == NULL)
		return VOOSMP_ERR_Unknown;

	jclass clazz = env.getEnv()->GetObjectClass(m_jOSmpEng);
	if (clazz == NULL) {
		return VOOSMP_ERR_Unknown;
	}

	jmethodID eventCallBack = env.getEnv()->GetMethodID(clazz, "osmpCallBack", "(IIILjava/lang/Object;)V");
	if (eventCallBack == NULL) {
		return VOOSMP_ERR_Unknown;
	}

	jobject jobj = NULL;
	if(VOOSMP_CB_SEI_INFO == nID)
	{
		if(pParam1!= NULL)
		{
			VOOSMP_SEI_INFO* pInfo = (VOOSMP_SEI_INFO*)pParam1;
			if(pInfo->nFlag == VOOSMP_FLAG_SEI_PIC_TIMING)
			{
				VOOSMP_SEI_PIC_TIMING* pPicTimingInfo = (VOOSMP_SEI_PIC_TIMING*)pInfo->pInfo;
				int size = sizeof(VOOSMP_SEI_PIC_TIMING)/sizeof(int);
				jintArray arr = env.getEnv()->NewIntArray(size);
				env.getEnv()->SetIntArrayRegion(arr, 0, size, (const jint*)pPicTimingInfo);

				jobj = arr;
				nParam1 = VOOSMP_FLAG_SEI_PIC_TIMING;
				if(pInfo!=NULL && jobj==NULL)
					VOLOGE("VOOSMP_FLAG_SEI_EVENT_PIC_TIMING pInfo!=NULL && jobj==NULL");
				VOLOGI("VOOSMP_FLAG_SEI_EVENT_PIC_TIMING pInfo=%d && jobj=%d",(int)pInfo, (int)jobj);
			}
			if(pInfo->nFlag == VO_SEI_USER_DATA_UNREGISTERED)
			{
				CJavaParcelWrap	javaParcel(env.getEnv());
				VO_H264_USERDATA_Params* pInfo2 = (VO_H264_USERDATA_Params*)pInfo->pInfo;
				javaParcel.writeInt32(pInfo2->count);
				if(pInfo2->count>255)
					VOLOGE("VO_SEI_USER_DATA_UNREGISTERED pInfo->count>255");
				int all=0;
				for(int i=0;i<(int)(pInfo2->count)  ;i++)
				{
					all+=pInfo2->size[i];
					javaParcel.writeInt32(pInfo2->size[i]);
				}
				javaParcel.write(pInfo2->buffer, (VO_U32) all);

				jobj = javaParcel.getParcel();
				nParam1 = VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED;
				if(pInfo!=NULL && jobj==NULL)
					VOLOGE("VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED pInfo!=NULL && jobj==NULL");
				VOLOGI("VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED pInfo=%d && jobj=%d all=%d",(int)pInfo, (int)jobj,all);
			}
		}
	}
	else if(VOOSMP_CB_Metadata_Arrive == nID)
	{
		if(VOOSMP_SS_IMAGE == nParam1)
		{
			CJavaParcelWrap	javaParcel(env.getEnv());
			//	VOLOGI ("CloseCaption GetSubtileSample 555!");
			CJavaParcelWrap::getParcelFromSubtitleInfo((voSubtitleInfo*)nParam2, &javaParcel);
			//	VOLOGI ("CloseCaption GetSubtileSample 777!");
			jobj = javaParcel.getParcel();
		}
	}else if(VOOSMP_CB_PCM_OUTPUT == nID)
	{
		
		VOOSMP_PCMBUFFER* pInfo = (VOOSMP_PCMBUFFER*)pParam1;
		CJavaParcelWrapOSMP parc(env.getEnv());
		parc.fillParcelData(pInfo);
		jobj = parc.getParcel();
		if(pInfo!=NULL && jobj==NULL)
			VOLOGE("VOOSMP_CB_PCM_OUTPUT pInfo!=NULL && jobj==NULL");
		VOLOGI("VOOSMP_CB_PCM_OUTPUT pInfo=%d && jobj=%d",(int)pInfo, (int)jobj);
		VOLOGI("nID is %d  && jobj=%d nParam1 = %d nParam2 = %d ",nID, (int)jobj);
	    env.getEnv()->CallVoidMethod(m_jOSmpEng, eventCallBack, nID, nParam1, nParam2, jobj);
        env.getEnv()->DeleteLocalRef(clazz);
		return nRC;
	}
	else if(VOOSMP_CB_VideoAspectRatio == nID)
	{
		m_nVideoBufferSize = 0;
	}
    VOLOGI("nID is %d  && jobj=%d nParam1 = %d nParam2 = %d ",nID, (int)jobj);
	env.getEnv()->CallVoidMethod(m_jOSmpEng, eventCallBack, nID, nParam1, nParam2, jobj);


	env.getEnv()->DeleteLocalRef(clazz);

	return nRC;
}


void * COSNdkVomePlayer::vomtGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag)
{
	void * ptr = voGetAddressSymbol (hLib, pFuncName);

	VOLOGI( "Func Name: %s  %p" , pFuncName , ptr );
	return ptr;
}

int COSNdkVomePlayer::vomtFreeLib (void * pUserData, void * hLib, int nFlag)
{
	VOLOGI ("Free Lib: %0xd", (unsigned int) hLib);
	voFreeLibrary (hLib);
	return 0;
}

void * COSNdkVomePlayer::vomtLoadLib (void * pUserData, char * pLibName, int nFlag)
{
	COSNdkVomePlayer *pPlayer = (COSNdkVomePlayer *)pUserData;

	char szFile[1024];
	char tempFile[1024];
	uint64_t nCpuFeature;

	strcpy (szFile, pPlayer->m_szPathLib);
//	strcat (szFile, "lib/");

	memset(tempFile, 0, sizeof(tempFile));
	strcpy(tempFile, "/data/local/tmp/OnStreamPlayer/lib/");
	if (strstr(pLibName, "lib") == 0)
	{
		strcat(szFile, "lib");
		strcat(tempFile, "lib");
	}

	if(android_getCpuFamily() != ANDROID_CPU_FAMILY_ARM)
	{
		VOLOGI("Not an ARM CPU !");
	    //return NULL;
	}

	nCpuFeature = android_getCpuFeatures();
	if(nCpuFeature == ANDROID_CPU_ARM_FEATURE_NEON)
	{
		strcat(szFile,pLibName);  // use NEON-optimized routines and call visualon v7 lib
		strncat(szFile,"_v7",4);

		strcat(tempFile, pLibName);
		strncat(tempFile,"_v7",4);
	}
	else
	{
		strcat(szFile, pLibName); // use non-NEON routines and call v6 lib
		strcat(tempFile, pLibName);
	}

	if( strstr( szFile , ".so" ) == 0 )
	{
		strcat( szFile , ".so" );
		strcat( tempFile, ".so" );
	}

	
	int n;
	if(pPlayer->m_nPrefixEnable)
	{
		char PreFile[1024];
		char *p = strstr(szFile, "libvo");

		if(p)
		{
			p = p + 5;
			n = (int)(p - szFile);
			memset(PreFile, 0, 1024);
			strncpy(PreFile, szFile, n);
			strcat(PreFile, pPlayer->m_cPrefix);
			strcat(PreFile, p);
			strcpy(szFile, PreFile);
		}

		p = strstr(tempFile, "libvo");
		if(p)
		{
			p = p + 5;
			n = (int)(p - tempFile);
			memset(PreFile, 0, 1024);
			strncpy(PreFile, tempFile, n);
			strcat(PreFile, pPlayer->m_cPrefix);
			strcat(PreFile, p);

			strcpy(tempFile, PreFile);
		}
	}

	if(pPlayer->m_nSuffixEnable)
	{
		char SufFile[1024];
		char *p = strstr(szFile, ".so");

		if(p)
		{
			n = (int)(p - szFile);
			memset(SufFile, 0, 1024);
			strncpy(SufFile, szFile, n);
			strcat(SufFile, pPlayer->m_cSuffix);
			strcat(SufFile, p);
			strcpy(szFile, SufFile);
		}

		p = strstr(tempFile, ".so");
		if(p)
		{
			n = (int)(p - tempFile);
			memset(SufFile, 0, 1024);
			strncpy(SufFile, tempFile, n);
			strcat(SufFile, pPlayer->m_cSuffix);
			strcat(SufFile, p);

			strcpy(tempFile, SufFile);
		}
	}

	VOLOGI("Load lib name: %s!!!",szFile);

	void * hDll = NULL;
	
	if(voOS_EnableDebugMode(0))
	{
		hDll = voLoadLibrary (tempFile, RTLD_NOW);
		VOLOGI("Load Debug lib name: %s",tempFile);
	}

	if (hDll == NULL)
	{
		hDll = voLoadLibrary (szFile, RTLD_NOW);

		if (hDll == NULL)
		{
			strcpy (szFile, pPlayer->m_szPathLib);
			strcat (szFile, "lib/");
			strcat (szFile, pLibName);

	        if( strstr( szFile , ".so" ) == 0 )
	            strcat( szFile , ".so" );

			hDll = voLoadLibrary (szFile, RTLD_NOW);
		}
		VOLOGI ("Load Library File %s, Handle %p", szFile, hDll);
	}else
	{
		VOLOGI ("Load Library File %s, Handle %p", tempFile, hDll);
	}

	return hDll;
}

int COSNdkVomePlayer::ShowBlackFrame()
{
	if (m_pVideoRender == NULL)
		return -1;

	voCAutoLock lockGetVideo (&m_mtGetVideo);
	VOMP_BUFFERTYPE bufVideo;
	VOMP_BUFFERTYPE* pBufVideo = &bufVideo;
	
	pBufVideo->pBuffer = NULL;
	pBufVideo->nReserve = 0;
	if (m_nColorType == VOMP_COLOR_RGB32_PACKED || m_nColorType == VOMP_COLOR_ARGB32_PACKED)
	{
		pBufVideo->nSize = m_fmtVideo.Width*m_fmtVideo.Height*4;
	}
	else if (m_nColorType == VOMP_COLOR_RGB888_PACKED || m_nColorType == VOMP_COLOR_RGB888_PLANAR)
	{
		pBufVideo->nSize = m_fmtVideo.Width*m_fmtVideo.Height*3;
	}
	else if (m_nColorType == VOMP_COLOR_RGB565_PACKED)
	{
		pBufVideo->nSize = m_fmtVideo.Width*m_fmtVideo.Height*2;
	}
	else
	{
		pBufVideo->nSize = 0;
	}	

	int bufSize = -1;
	int nRC = m_pVideoRender->SurfaceLock(pBufVideo, &bufSize);	

	if(pBufVideo->pBuffer && bufSize > 0)
	{
		memset(pBufVideo->pBuffer, 0, bufSize);
	}

	nRC = m_pVideoRender->SurfaceUnlock(pBufVideo, NULL);

	return nRC; 
}

void COSNdkVomePlayer::internalReturnVideoBuffer(const int rendered) {
	
	VOMP_SIGNALBUFFERRETURNED br;
	br.nRendered = rendered;
	br.nTrackType = VOMP_SS_Video;
	br.pBuffer = m_pBufVideo;
	COSVomeEngine::SetParam(VOMP_PID_SIGNAL_BUFFER_RETURNED, (void*)&br);
}

