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
 * @file COSNdkVideoRender.h
 * video render wrap class for android.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#ifndef __COSNdkVideoRender_H__
#define __COSNdkVideoRender_H__

#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <android/native_window.h>
#include "vompType.h"
#include "voOnStreamType.h"
#include "voAndroidVR.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define VO_RENDER_PID_RENDERTYPE		0
#define VO_RENDER_PID_DRAW_COLOR		1
#define VO_RENDER_PID_SURFACE			2
#define	VO_RENDER_PID_BITMAP			3
#define	VO_RENDER_PID_JAVAVM			14 // avoid equal with VOOSMP_PID_FUNC_LIB
#define	VO_RENDER_PID_LIBOP				5
#define	VO_RENDER_PID_VIDEOFORAMT		6
#define	VO_RENDER_PID_DRAW_RECT			7
#define	VO_RENDER_PID_DRAW_MODE			8
#define VO_RENDER_PID_YUVBUFFERINFO		9

typedef void* (*ANativeWindow_fromSurface_t)(JNIEnv *env, jobject surface);
typedef void (*ANativeWindow_release_t)(void *window);
typedef int (*ANativeWindow_setBuffersGeometry_t)(void *window, int width, int height, int format);
typedef int (* ANativeWindow_lock_t)(void *window, void *outBuffer, void *inOutDirtyBounds);
typedef int (* ANativeWindow_unlockAndPost_t)(void *window);

//bitmap render define
enum AndroidBitmapFormat {
    ANDROID_BITMAP_FORMAT_NONE      = 0,
    ANDROID_BITMAP_FORMAT_RGBA_8888 = 1,
    ANDROID_BITMAP_FORMAT_RGB_565   = 4,
    ANDROID_BITMAP_FORMAT_RGBA_4444 = 7,
    ANDROID_BITMAP_FORMAT_A_8       = 8,
};

typedef struct {
    uint32_t    width;
    uint32_t    height;
    uint32_t    stride;
    int32_t     format;
    uint32_t    flags; // 0 for now
} AndroidBitmapInfo;

typedef int (* bitmap_getInfo)(JNIEnv* env, jobject jbitmap, AndroidBitmapInfo* info);
typedef int (* bitmap_lockPixels)(JNIEnv* env, jobject jbitmap, void** addrPtr);
typedef int (* bitmap_unlockPixels)(JNIEnv* env, jobject jbitmap);


class COSNdkVideoRender
{
public:
	COSNdkVideoRender ();
	virtual ~COSNdkVideoRender (void);

	int	SetParam(int nID, void* pValue);
	int	GetParam(int nID, void* pValue);
	
	int SurfaceLock(VOMP_BUFFERTYPE *pBuffer, void* context);
	int SurfaceUnlock(VOMP_BUFFERTYPE *pBuffer, void* context);
	
protected:
	int									Load_Native();
	int									Load_CLib();
	int									Load_Bitmap();
	int									InitNativeWndRender(jobject surface);
	int									InitNativeCRender(jobject surface);
	int									InitBitmapRender(jobject bitmap);

	int									NativeLock(VOMP_VIDEO_BUFFER* pBuffer, VOMP_RECT* rect);

	//Android native windows render
	void*								m_hAndroidDll;		
	bool								m_bNativeWndAvaiable;
	ANativeWindow*						m_pNativeWnd;
	ANativeWindow_fromSurface_t			m_pANativeWindow_fromSurface;
	ANativeWindow_release_t				m_pANativeWindow_release;
	ANativeWindow_setBuffersGeometry_t	m_pANativeWindow_setBuffersGeometry;
	ANativeWindow_lock_t				m_pANativeWindow_lock;
	ANativeWindow_unlockAndPost_t		m_pANativeWindow_unlockAndPost;

	//Android native C render
	char					m_szProp[64];
	void *					m_hLibRender;
	VOAVR_FUNCSET			m_sVRFuncSet;
	VOMP_VIDEO_BUFFER		m_sVideoBuffer;
	VOMP_RECT				m_sVideoRect;
	void*					m_pSurface;
	VOMP_LIB_FUNC*			m_pLibOp;

	// Android bitmap render
	void*					m_hmodule_graphics;
	bitmap_getInfo 			m_getinfo;
	bitmap_lockPixels 		m_lockpixels;
	bitmap_unlockPixels		m_unlockpixels;
	jobject 				m_h_jbitmap;


	VOMP_VIDEO_FORMAT		m_fmtVideo;
	int						m_nRenderType;
	int						m_nColorType;
	int						m_nZoomMode;
	JavaVM *				m_nJavaVM;
	jobject					m_jSurface;
	jobject					m_jBitmap;
};

#ifdef _VONAMESPACE
}
#endif

#endif  //__COSNdkVideoRender_H__
