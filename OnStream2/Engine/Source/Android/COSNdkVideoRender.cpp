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
 * @file COSNdkVideoRender.cpp
 * video render wrap class for android.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#include <dlfcn.h>
#include "COSNdkVideoRender.h"
#include "CJniEnvUtil.h"
#include <sys/system_properties.h>

#define  LOG_TAG    "COSNdkVideoRender"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

COSNdkVideoRender::COSNdkVideoRender()
: m_hAndroidDll(0)
, m_bNativeWndAvaiable(true)
, m_pNativeWnd(0)
, m_pANativeWindow_fromSurface(0)
, m_pANativeWindow_release(0)
, m_pANativeWindow_setBuffersGeometry(0)
, m_pANativeWindow_lock(0)
, m_pANativeWindow_unlockAndPost(0)
, m_hLibRender(0)
, m_pSurface(NULL)
, m_pLibOp(0)
, m_hmodule_graphics(0)
, m_getinfo(0)
, m_lockpixels(0)
, m_unlockpixels(0)
, m_h_jbitmap(0)
, m_nRenderType(VOOSMP_RENDER_TYPE_NATIVE)
, m_nColorType (VOMP_COLOR_ARGB32_PACKED)
, m_nZoomMode(1)
, m_nJavaVM(NULL)
, m_jSurface(NULL)
, m_jBitmap(NULL)
{
	
	memset(&m_sVRFuncSet, 0, sizeof(VOAVR_FUNCSET));
	memset (m_szProp, 0, 64);
	__system_property_get ("ro.build.version.release", m_szProp);

	memset(&m_fmtVideo, 0, sizeof(VOMP_VIDEO_FORMAT));
	memset(&m_sVideoBuffer, 0, sizeof(VOMP_VIDEO_BUFFER));
	memset(&m_sVideoRect, 0, sizeof(VOMP_RECT));
}

COSNdkVideoRender::~COSNdkVideoRender()
{
	if (m_pNativeWnd)
		m_pANativeWindow_release(m_pNativeWnd);
	if (m_hAndroidDll != NULL)
		dlclose (m_hAndroidDll);

	if(m_h_jbitmap)
		InitBitmapRender(0);
	if (m_hmodule_graphics)
		dlclose (m_hmodule_graphics);

	if (m_sVRFuncSet.hRender != NULL && m_sVRFuncSet.hRender != NULL)
		m_sVRFuncSet.Uninit (m_sVRFuncSet.hRender);

	if (m_hLibRender && m_pLibOp) 
	{
		m_pLibOp->FreeLib(m_pLibOp->pUserData, m_hLibRender, 0);
		m_hLibRender = 0;
	}
}

int COSNdkVideoRender::Load_Native()
{
	if(m_hAndroidDll) return VOOSMP_ERR_None;

	m_bNativeWndAvaiable = false;

	m_hAndroidDll = dlopen("libandroid.so", RTLD_NOW);
	VOLOGI ("CNativeWndRender libandroid.so %p", m_hAndroidDll);

	if (m_hAndroidDll) 
	{
		m_pANativeWindow_fromSurface = (ANativeWindow_fromSurface_t)dlsym(m_hAndroidDll, "ANativeWindow_fromSurface");
		m_pANativeWindow_release = (ANativeWindow_release_t)dlsym(m_hAndroidDll, "ANativeWindow_release");
		m_pANativeWindow_setBuffersGeometry = (ANativeWindow_setBuffersGeometry_t) dlsym(m_hAndroidDll, "ANativeWindow_setBuffersGeometry");
		m_pANativeWindow_lock = (ANativeWindow_lock_t) dlsym(m_hAndroidDll, "ANativeWindow_lock");
		m_pANativeWindow_unlockAndPost = (ANativeWindow_unlockAndPost_t)dlsym(m_hAndroidDll, "ANativeWindow_unlockAndPost");
		if (!m_pANativeWindow_fromSurface || !m_pANativeWindow_release || !m_pANativeWindow_setBuffersGeometry
			|| !m_pANativeWindow_lock || !m_pANativeWindow_unlockAndPost)
		{
			dlclose (m_hAndroidDll);
			m_hAndroidDll = 0;
			m_bNativeWndAvaiable = false;
		}
		else
		{
			VOLOGI ("CNativeWndRender API successfully loaded");				
			m_bNativeWndAvaiable = true;
			return VOOSMP_ERR_None;
		}
	}

	if(m_bNativeWndAvaiable == false)
	{
		if(m_hLibRender) return VOOSMP_ERR_None;

		if(m_pLibOp == NULL || m_pLibOp->LoadLib == NULL)
			return VOOSMP_ERR_Implement;

		char szLibRenderName[256];

		VOLOGI("Current device android version is %S", m_szProp);
		if (strstr (m_szProp, "3.") == m_szProp)
			strcpy(szLibRenderName, "libvoAndroidVR_S23.so");
		else if (strstr (m_szProp, "4.3") == m_szProp)
			strcpy(szLibRenderName, "libvoAndroidVR_S43.so");
		else if (strstr (m_szProp, "4.1") == m_szProp || strstr (m_szProp, "4.2") == m_szProp)
			strcpy(szLibRenderName, "libvoAndroidVR_S41.so");
		else if (strstr (m_szProp, "4.") == m_szProp)
			strcpy(szLibRenderName, "libvoAndroidVR_S40.so");
		else if (strstr (m_szProp, "2.3") == m_szProp)
			strcpy(szLibRenderName, "libvoAndroidVR_S23.so");
		else if (strstr (m_szProp, "2.2") == m_szProp)
			strcpy(szLibRenderName, "libvoAndroidVR_S22.so");
		else if (strstr (m_szProp, "2.1") == m_szProp)
			strcpy(szLibRenderName, "libvoAndroidVR_S20.so");
		else if (strstr (m_szProp, "2.0") == m_szProp)
			strcpy(szLibRenderName, "libvoAndroidVR_S20.so");
		else if (strstr (m_szProp, "Eclair") == m_szProp)
			strcpy(szLibRenderName, "libvoAndroidVR_S20.so");
		else
			strcpy(szLibRenderName, "libvoAndroidVR_S16.so");


		m_hLibRender = m_pLibOp->LoadLib(m_pLibOp->pUserData, szLibRenderName, 0);
		if (m_hLibRender == NULL)
		{
			VOLOGW ("It could not load the module %s, error: %s", szLibRenderName, dlerror ());
			return VOOSMP_ERR_Unknown;
		}

		VOLOGI ("Load module %s", szLibRenderName);
		VOAVRGETFUNCSET fGetFunc = (VOAVRGETFUNCSET) dlsym (m_hLibRender, ("voavrGetFuncSet"));
		if (fGetFunc == NULL) return VOOSMP_ERR_Unknown;
		fGetFunc (&m_sVRFuncSet);

		return VOOSMP_ERR_None;
	}		

	return VOOSMP_ERR_None;
}
	
int COSNdkVideoRender::Load_CLib()
{
	if(m_hLibRender) return VOOSMP_ERR_None;

	if(m_pLibOp == NULL || m_pLibOp->LoadLib == NULL)
		return VOOSMP_ERR_Implement;

	char szLibRenderName[256];

	if (strstr (m_szProp, "3.") == m_szProp)
		strcpy(szLibRenderName, "libvoAndroidVR_S23.so");
	else if (strstr (m_szProp, "4.3") == m_szProp || strstr (m_szProp, "4.4") == m_szProp)
		strcpy(szLibRenderName, "libvoAndroidVR_S43.so");
	else if (strstr (m_szProp, "4.1") == m_szProp || strstr (m_szProp, "4.2") == m_szProp)
		strcpy(szLibRenderName, "libvoAndroidVR_S41.so");
	else if (strstr (m_szProp, "4.") == m_szProp)
		strcpy(szLibRenderName, "libvoAndroidVR_S40.so");
	else if (strstr (m_szProp, "2.3") == m_szProp)
		strcpy(szLibRenderName, "libvoAndroidVR_S23.so");
	else if (strstr (m_szProp, "2.2") == m_szProp)
		strcpy(szLibRenderName, "libvoAndroidVR_S22.so");
	else if (strstr (m_szProp, "2.1") == m_szProp)
		strcpy(szLibRenderName, "libvoAndroidVR_S20.so");
	else if (strstr (m_szProp, "2.0") == m_szProp)
		strcpy(szLibRenderName, "libvoAndroidVR_S20.so");
	else
		strcpy(szLibRenderName, "libvoAndroidVR_S16.so");

	m_hLibRender = m_pLibOp->LoadLib(m_pLibOp->pUserData, szLibRenderName, 0);
	if (m_hLibRender == NULL)
	{
		VOLOGW ("It could not load the module %s, error: %s", szLibRenderName, dlerror ());
		return VOOSMP_ERR_Unknown;
	}

	VOLOGI ("Load module %s", szLibRenderName);
	VOAVRGETFUNCSET fGetFunc = (VOAVRGETFUNCSET)m_pLibOp->GetAddress(m_pLibOp->pUserData, m_hLibRender, ("voavrGetFuncSet"), 0);
	if (fGetFunc == NULL) return VOOSMP_ERR_Unknown;
	fGetFunc (&m_sVRFuncSet);

	//m_bNativeWndAvaiable = false;

	return VOOSMP_ERR_None;
}
	
int COSNdkVideoRender::Load_Bitmap()
{
	if(m_hmodule_graphics)
	{
		dlclose (m_hmodule_graphics);
		m_hmodule_graphics = 0;
	}

	m_hmodule_graphics = dlopen("libjnigraphics.so" , RTLD_NOW );

	if( m_hmodule_graphics )
	{
		m_getinfo = (bitmap_getInfo)dlsym( m_hmodule_graphics , "AndroidBitmap_getInfo" );
		m_lockpixels = (bitmap_lockPixels) dlsym ( m_hmodule_graphics , "AndroidBitmap_lockPixels");
		m_unlockpixels = (bitmap_unlockPixels) dlsym ( m_hmodule_graphics , "AndroidBitmap_unlockPixels");

		VOLOGI("libjnigraphics loaded %p, %p, %p", m_getinfo, m_lockpixels, m_unlockpixels);
	}
	else
	{
		return VOOSMP_ERR_Unknown;
	}	

	return VOOSMP_ERR_None; 
}

int COSNdkVideoRender::NativeLock(VOMP_VIDEO_BUFFER* pBuffer, VOMP_RECT* rect)
{
	if (!m_pNativeWnd) return VOOSMP_ERR_Pointer;

	int err = 0;
	int format = 0;
	int strideMultipler = 1;
	ANativeWindow_Buffer buffer;

	switch (pBuffer->ColorType){
		case VOMP_COLOR_ARGB32_PACKED:
			format = WINDOW_FORMAT_RGBA_8888;
			strideMultipler = 4;
			break;
		case VOMP_COLOR_RGB32_PACKED:
			format = WINDOW_FORMAT_RGBX_8888;
			strideMultipler = 4;
			break;
		case VOMP_COLOR_RGB565_PACKED:
		default:
			format = WINDOW_FORMAT_RGB_565;
			strideMultipler = 2;
			break;
	}
	err = m_pANativeWindow_setBuffersGeometry(m_pNativeWnd, rect->right, rect->bottom, format);
	//VOLOGI ("CNativeWndRender::set geomtry %d, %dx%d, fmt %d", err, rect->right, rect->bottom, format);
	if (err) return err;

	err = m_pANativeWindow_lock(m_pNativeWnd, (void*)&buffer, NULL);
	if (!err) {
		pBuffer->Buffer[0] = (char*)buffer.bits;
		pBuffer->Stride[0] = buffer.stride * strideMultipler;		
		rect->right = buffer.width;
		rect->bottom = buffer.height;
		format = buffer.format;
	}

	return err;
}


int COSNdkVideoRender::SurfaceLock(VOMP_BUFFERTYPE *pBuffer, void *context)
{
	int nRC = VOOSMP_ERR_None;

	VOMP_RECT	sVideoRect;
	
	switch(m_nRenderType) {
		case VOOSMP_RENDER_TYPE_BITMAP:
			if(!m_hmodule_graphics || !m_lockpixels || !m_h_jbitmap  || !context) 
				return VOOSMP_ERR_Pointer;
			m_lockpixels((JNIEnv*)context, m_h_jbitmap, (void**)&(pBuffer->pBuffer));
			break;

		case VOOSMP_RENDER_TYPE_NATIVE:
			sVideoRect.top = m_sVideoRect.top;
			sVideoRect.left = m_sVideoRect.left;
			sVideoRect.right = m_sVideoRect.right;
			sVideoRect.bottom = m_sVideoRect.bottom;		

			//VOLOGI ("m_sVideoRect::set Width %d,Height %d", m_sVideoRect.right, m_sVideoRect.bottom);
			if(m_bNativeWndAvaiable)
			{
				nRC = NativeLock(&m_sVideoBuffer, &sVideoRect);
			}
			else
			{
				if(m_sVRFuncSet.hRender == NULL) return VOOSMP_ERR_Pointer;
				nRC = m_sVRFuncSet.Lock (m_sVRFuncSet.hRender, &m_sVideoBuffer, &sVideoRect);
			}
			if(-1 == *(int*)context)
				*(int*)context = sVideoRect.bottom * m_sVideoBuffer.Stride[0];
			//VOLOGI ("m_sVideoRect::set Width %d,Height %d", m_sVideoRect.right, m_sVideoRect.bottom);
			if (m_sVideoBuffer.Buffer[0] != NULL)
			{
				pBuffer->pBuffer = (unsigned char *)m_sVideoBuffer.Buffer[0];
				pBuffer->pData = &m_sVideoBuffer.Stride[0];
			}

			if (nRC == 0 && (m_sVideoRect.right != sVideoRect.right || m_sVideoRect.bottom != sVideoRect.bottom))
			{
				return VOOSMP_ERR_FormatChange;
			}
			break;
		case VOOSMP_RENDER_TYPE_NATIVE_C:
			sVideoRect.top = m_sVideoRect.top;
			sVideoRect.left = m_sVideoRect.left;
			sVideoRect.right = m_sVideoRect.right;
			sVideoRect.bottom = m_sVideoRect.bottom;	
		
			if(m_sVRFuncSet.hRender == NULL) return VOOSMP_ERR_Pointer; 
			nRC = m_sVRFuncSet.Lock (m_sVRFuncSet.hRender, &m_sVideoBuffer, &sVideoRect);

			if (m_sVideoBuffer.Buffer[0] != NULL)
			{
				pBuffer->pBuffer = (unsigned char *)m_sVideoBuffer.Buffer[0];
				pBuffer->pData = &m_sVideoBuffer.Stride[0];
			}
			if(-1 == *(int*)context)
				*(int*)context = sVideoRect.bottom * m_sVideoBuffer.Stride[0];

			if (nRC == 0 && (m_sVideoRect.right != sVideoRect.right || m_sVideoRect.bottom != sVideoRect.bottom))
			{
				return VOOSMP_ERR_FormatChange;
			}
			break;
		default:
			break;
	}
	
	return nRC;
}

int COSNdkVideoRender::SurfaceUnlock(VOMP_BUFFERTYPE *pBuffer, void *context)
{
	int nRC = VOOSMP_ERR_Unknown;

	switch(m_nRenderType) {
		case VOOSMP_RENDER_TYPE_BITMAP:
			if( m_hmodule_graphics == 0 || !m_unlockpixels || !m_h_jbitmap  || !context) 
				return VOOSMP_ERR_Pointer;
			m_unlockpixels((JNIEnv*)context, m_h_jbitmap);
		//	nRC = VOOSMP_ERR_None;
			nRC = VOOSMP_ERR_Unknown;
			break;
		case VOOSMP_RENDER_TYPE_NATIVE:
			if (m_sVideoBuffer.Buffer[0] != NULL) 
			{
				if (m_bNativeWndAvaiable) 
				{
					if (!m_pNativeWnd) return VOOSMP_ERR_Pointer;
					nRC = m_pANativeWindow_unlockAndPost(m_pNativeWnd);
				} 
				else 
				{
					if(m_sVRFuncSet.hRender == NULL) return VOOSMP_ERR_Pointer;
					m_sVRFuncSet.UnLock (m_sVRFuncSet.hRender);					
				}
				m_sVideoBuffer.Buffer[0] = NULL;
			}
			nRC = VOOSMP_ERR_None;
			break;
		case VOOSMP_RENDER_TYPE_NATIVE_C:
			if (m_sVideoBuffer.Buffer[0] != NULL) 
			{
				if(m_sVRFuncSet.hRender == NULL) return VOOSMP_ERR_Pointer; 
				
				m_sVRFuncSet.UnLock (m_sVRFuncSet.hRender);
				m_sVideoBuffer.Buffer[0] = NULL;
			}
			nRC = VOOSMP_ERR_None;
			break;
		default:
			break;
	}
	
	return nRC;
}

int	COSNdkVideoRender::SetParam(int nID, void* pValue)
{
	int nRC = VOOSMP_ERR_None;

	if(pValue == NULL)
		return VOOSMP_ERR_Pointer;

	switch(nID)
	{
	case VO_RENDER_PID_RENDERTYPE:
		m_nRenderType = *((int *)pValue);
		if(m_nRenderType == VOOSMP_RENDER_TYPE_NATIVE && m_jSurface)
		{
			if (m_bNativeWndAvaiable) 
			{
				nRC = InitNativeWndRender(m_jSurface);
				if(nRC == VOOSMP_ERR_None) 	return nRC;
			}

			nRC = InitNativeCRender(m_jSurface);
		}
		else if(m_nRenderType == VOOSMP_RENDER_TYPE_BITMAP && m_jBitmap)
		{
			nRC = InitBitmapRender(m_jBitmap);
		}
		else if(m_nRenderType == VOOSMP_RENDER_TYPE_NATIVE_C && m_jSurface)
		{
			nRC = InitNativeCRender(m_jSurface);
		}
		else if(m_nRenderType == VOOSMP_RENDER_TYPE_HW_RENDER && m_jSurface)
		{
			if (strstr (m_szProp, "2.") == m_szProp || strstr (m_szProp, "1.") == m_szProp)
			{
				nRC = InitNativeCRender(m_jSurface);
			}
			else
			{
				if (m_bNativeWndAvaiable) 
				{
					nRC = InitNativeWndRender(m_jSurface);
					if(nRC == VOOSMP_ERR_None) 	return nRC;
				}

				nRC = InitNativeCRender(m_jSurface);
			}
		}
		break;
	case VO_RENDER_PID_DRAW_COLOR:
		m_nColorType =	*((int *)pValue);
		m_sVideoBuffer.ColorType = m_nColorType;
		break;
	case VO_RENDER_PID_SURFACE:
		if(m_nRenderType == VOOSMP_RENDER_TYPE_NATIVE)
		{
			m_jSurface = (jobject)pValue;			
			
			if (m_bNativeWndAvaiable) 
			{
				nRC = InitNativeWndRender(m_jSurface);
				if(nRC == VOOSMP_ERR_None) 	return nRC;
			}
			
			nRC = InitNativeCRender(m_jSurface);
		}
		else if(m_nRenderType == VOOSMP_RENDER_TYPE_NATIVE_C)
		{
			m_jSurface = (jobject)pValue;
			
			//if (strstr (m_szProp, "3.") == m_szProp || strstr (m_szProp, "2.") == m_szProp || strstr (m_szProp, "1.") == m_szProp)
                        if(1)
			{
				nRC = InitNativeCRender(m_jSurface);
			}
			else
			{
				nRC = InitNativeWndRender(m_jSurface);
				if(nRC == VOOSMP_ERR_None)
				{
					m_nRenderType = VOOSMP_RENDER_TYPE_NATIVE;					
				}
			}
		}
		else if(m_nRenderType == VOOSMP_RENDER_TYPE_HW_RENDER)
		{
			m_jSurface = (jobject)pValue;	

			if (strstr (m_szProp, "2.") == m_szProp || strstr (m_szProp, "1.") == m_szProp)
			{
				nRC = InitNativeCRender(m_jSurface);
			}
			else
			{
				if (m_bNativeWndAvaiable) 
				{
					nRC = InitNativeWndRender(m_jSurface);
					if(nRC == VOOSMP_ERR_None) 	return nRC;
				}

				nRC = InitNativeCRender(m_jSurface);
			}
		}
		break;
	case VO_RENDER_PID_BITMAP:
		VOLOGI ("VO_RENDER_PID_BITMAP");
		if(m_nRenderType == VOOSMP_RENDER_TYPE_BITMAP)
		{
		VOLOGI ("VO_RENDER_PID_BITMAP, InitBitmapRender");
			m_jBitmap = (jobject)pValue;
			nRC = InitBitmapRender(m_jBitmap);
		}
		break;
	case VO_RENDER_PID_JAVAVM:
		m_nJavaVM = (JavaVM *)pValue;
		break;
	case VO_RENDER_PID_LIBOP:
		m_pLibOp = (VOMP_LIB_FUNC*)pValue;
		break;
	case VO_RENDER_PID_VIDEOFORAMT:
		m_fmtVideo.Width = ((VOMP_VIDEO_FORMAT*)pValue)->Width;
		m_fmtVideo.Height = ((VOMP_VIDEO_FORMAT*)pValue)->Height;

		m_fmtVideo.Type = ((VOMP_VIDEO_FORMAT*)pValue)->Type;
		
		if(m_nZoomMode == 1)
		{
			m_sVideoRect.top = 0;
			m_sVideoRect.left = 0;
			m_sVideoRect.right = m_fmtVideo.Width;
			m_sVideoRect.bottom = m_fmtVideo.Height;
		}
		break;
	case VO_RENDER_PID_DRAW_RECT:
		m_sVideoRect.top = ((VOMP_RECT*)pValue)->top - ((VOMP_RECT*)pValue)->top;
		m_sVideoRect.left = ((VOMP_RECT*)pValue)->left -  ((VOMP_RECT*)pValue)->left;
		m_sVideoRect.right = ((VOMP_RECT*)pValue)->right -  ((VOMP_RECT*)pValue)->left;
		m_sVideoRect.bottom = ((VOMP_RECT*)pValue)->bottom - ((VOMP_RECT*)pValue)->top;

		VOLOGI ("m_sVideoRect.top %d, m_sVideoRect.left %d, m_sVideoRect.bottom %d, m_sVideoRect.right %d", m_sVideoRect.top, m_sVideoRect.left, m_sVideoRect.bottom, m_sVideoRect.right);
		break;
	case VO_RENDER_PID_DRAW_MODE:
		m_nZoomMode = *(int *)pValue;
		break;
	case VOOSMP_PID_AUDIO_RENDER_FORMAT:
		Load_CLib();
		if(NULL != m_sVRFuncSet.SetAudioInfo && NULL != pValue)
		{
			nRC = m_sVRFuncSet.SetAudioInfo(pValue);
		}
		else
		{
			return VOOSMP_ERR_Pointer;
		}
		break;
	case VOOSMP_PID_FUNC_LIB:
		Load_CLib();
		if(NULL != m_sVRFuncSet.SetWorkPath && NULL != pValue)
		{
			nRC = m_sVRFuncSet.SetWorkPath(pValue);
		}
		else
		{
			return VOOSMP_ERR_Pointer;
		}
		break;	
	}

	return nRC;
}
	
int	COSNdkVideoRender::GetParam(int nID, void* pValue)
{
	int nRC = VOOSMP_ERR_None;

	if(pValue == NULL)
		return VOOSMP_ERR_Pointer;

	if(nID == VO_RENDER_PID_SURFACE)
	{
		if(m_bNativeWndAvaiable)
			*((ANativeWindow**)pValue) = m_pNativeWnd;
		else
			*((int**)pValue) = (int *)m_pSurface;

		return VOOSMP_ERR_None;
	}
	else if(nID == VO_RENDER_PID_DRAW_RECT)
	{
		*((VOMP_RECT**)pValue) = &m_sVideoRect;
		return VOOSMP_ERR_None;
	}
	else if(VOOSMP_PID_AUDIO_RENDER_LATENCY == nID)
	{
		Load_CLib();
		if(NULL != m_sVRFuncSet.GetAudioBufTime && NULL != pValue)
		{
			nRC = m_sVRFuncSet.GetAudioBufTime(pValue);
		}
		else
		{
			return VOOSMP_ERR_Pointer;
		}
	}
	else if(VO_RENDER_PID_YUVBUFFERINFO == nID)
	{
		if(VOOSMP_RENDER_TYPE_NATIVE_C == m_nRenderType)
		{
			if(m_sVRFuncSet.hRender == NULL)
				return VOOSMP_ERR_Pointer; 

			return m_sVRFuncSet.GetParam(m_sVRFuncSet.hRender, nID, pValue);
		}

		return VOOSMP_ERR_Implement;
	}

	return nRC;
}


int	COSNdkVideoRender::InitNativeWndRender(jobject surface)
{
	if(m_hAndroidDll == NULL)
	{
		int nRC = Load_Native();
		if(nRC) return nRC;
	}

	if(m_hAndroidDll == NULL) return VOOSMP_ERR_Unknown;

	if (m_pNativeWnd && m_pANativeWindow_release) {
		m_pANativeWindow_release(m_pNativeWnd);
		m_pNativeWnd = 0;
	}

	CJniEnvUtil	env(m_nJavaVM);
	if (!env.getEnv() || !m_pANativeWindow_fromSurface){
		VOLOGI ("It is all null");
		return VOOSMP_ERR_Pointer;
	}
	
	m_pNativeWnd = (ANativeWindow *)m_pANativeWindow_fromSurface(env.getEnv(), surface);

	if(m_pNativeWnd == NULL)
	{
		m_bNativeWndAvaiable = false;
		return VOOSMP_ERR_Unknown;
	}

	return VOOSMP_ERR_None;
}

int	COSNdkVideoRender::InitNativeCRender(jobject surface)
{
	int nRC = VOOSMP_ERR_Unknown;
	if(m_hLibRender == NULL)
	{
		nRC = Load_CLib();
		if(nRC) return nRC;
		m_bNativeWndAvaiable = false;
	}

	if (!m_hLibRender) return VOOSMP_ERR_Unknown;
	
	if (m_sVRFuncSet.hRender != NULL && m_sVRFuncSet.hRender != NULL)
	{
		m_sVRFuncSet.Uninit (m_sVRFuncSet.hRender);
		m_sVRFuncSet.hRender = 0;
	}
	
        if(strstr (m_szProp, "4.") == m_szProp)
        {
                if(m_hAndroidDll == NULL)
                {
                        int nRC = Load_Native();
                        if(nRC) return nRC;
                }

                if(m_hAndroidDll == NULL) return VOOSMP_ERR_Unknown;

                if (m_pNativeWnd && m_pANativeWindow_release) {
                        m_pANativeWindow_release(m_pNativeWnd);
                        m_pNativeWnd = 0;
                }

                CJniEnvUtil	env(m_nJavaVM);
                if (!env.getEnv() || !m_pANativeWindow_fromSurface){
                        VOLOGI ("Finally++ It is all null");
                        return VOOSMP_ERR_Pointer;
                }

                m_pNativeWnd = (ANativeWindow *)m_pANativeWindow_fromSurface(env.getEnv(), surface);

                VOLOGI ("load native C surface %p", m_pNativeWnd);
                if (m_sVRFuncSet.Init != NULL && m_pNativeWnd)
                {
                        nRC = m_sVRFuncSet.Init (&m_sVRFuncSet.hRender, m_pNativeWnd, 0);
                        VOLOGI ("m_sVRFuncSet.Init returns %08X", nRC);
                }

                //env.getEnv()->DeleteLocalRef(surfaceCls);
        }
        else
        {
                CJniEnvUtil	env(m_nJavaVM);
                if (!env.getEnv()) return VOOSMP_ERR_Pointer;

                jclass surfaceCls = env.getEnv()->FindClass("android/view/Surface");
                if (surfaceCls == NULL) return VOOSMP_ERR_Unknown;

                jfieldID surface_native = NULL;
                if (strstr (m_szProp, "3.") == m_szProp)
                        surface_native = env.getEnv()->GetFieldID(surfaceCls, "mNativeSurface", "I");
                else if (strstr (m_szProp, "2.3") == m_szProp)
                        surface_native = env.getEnv()->GetFieldID(surfaceCls, "mNativeSurface", "I");
                else
                        surface_native = env.getEnv()->GetFieldID(surfaceCls, "mSurface", "I");
                if (surface_native == NULL) return VOOSMP_ERR_Unknown;

                void *pSurface = (void *)env.getEnv()->GetIntField(surface, surface_native);

                m_pSurface = pSurface;
                VOLOGI ("load native C surface %p", pSurface);
                if (m_sVRFuncSet.Init != NULL && m_pSurface)
                {
                        nRC = m_sVRFuncSet.Init (&m_sVRFuncSet.hRender, m_pSurface, 0);
                        VOLOGI ("m_sVRFuncSet.Init returns %08X", nRC);
                }

                env.getEnv()->DeleteLocalRef(surfaceCls);
        }

	return nRC;
}

int	COSNdkVideoRender::InitBitmapRender(jobject bitmap)
{
	if(m_hmodule_graphics == NULL)
	{
		int nRC = Load_Bitmap();
		if(nRC) return nRC;
	}
	
	if(!m_hmodule_graphics || !m_nJavaVM) 
		return VOOSMP_ERR_Pointer;

	CJniEnvUtil	env(m_nJavaVM);
	if (env.getEnv()) {
		if( m_h_jbitmap ) {
			env.getEnv()->DeleteGlobalRef( m_h_jbitmap );
			m_h_jbitmap = 0;
		}

		if( bitmap != 0 )
		{
			m_h_jbitmap = env.getEnv()->NewGlobalRef( bitmap );
			
			if( m_getinfo )
			{
				AndroidBitmapInfo info;
				m_getinfo( env.getEnv() , m_h_jbitmap , &info );
			}
		}	
		VOLOGI("libjnigraphics set bitmap %p", m_h_jbitmap);
		return VOMP_ERR_None;
	}
	
	return VOMP_ERR_Pointer;
}
