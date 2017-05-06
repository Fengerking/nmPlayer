/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
	File:		CSurfaceVideoRender.h

	Contains:	CSurfaceVideoRender header file

	Written by:	Tom Yu Wei 

	Change History (most recent first):
	2011-06-08		JBF			Create file

*******************************************************************************/

#ifndef __CNativeWindowVideoRender_h__
#define __CNativeWindowVideoRender_h__

#include <sys/types.h>
#include <utils/Errors.h>

//#include <ui/android_native_buffer.h>
#include <ui/GraphicBufferMapper.h>
#include <media/AudioTrack.h>
#include <cutils/properties.h>

#if defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
#include <utils/MemoryBase.h>
#include <utils/MemoryHeapBase.h>
#include <utils/MemoryHeapPmem.h>
#elif defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__ || defined __VONJ_GINGERBREAD__
#include <binder/MemoryBase.h>
#include <binder/MemoryHeapBase.h>
#include <binder/MemoryHeapPmem.h>
#endif
 
#include <jni.h>

#include "vompType.h"

namespace android {

// ----------------------------------------------------------------------------
class CNativeWindowVideoRender
{
	typedef enum 
	{
		VO_BOARDPLATFORM_EXYNOS4, 
		VO_BOARDPLATFORM_RK30XX, 
		VO_BOARDPLATFORM_OTHERS = 0X7FFFFFFF
	} VO_BOARDPLATFORM_TYPE;

public:
	CNativeWindowVideoRender(void * pSurface, int nFlag);
	virtual	~CNativeWindowVideoRender(void);

	virtual int Lock (VOMP_VIDEO_BUFFER * pVideoInfo, VOMP_RECT * pVideoSize);
	virtual int UnLock (void);
	virtual int GetParam (int nID, void * pValue);
	virtual int SetParam (int nID, void * pValue);

protected:
	sp<ANativeWindow>			m_piNativeWindow;

	int							m_nWidth;
	int							m_nHeight;
	android_native_buffer_t *	m_pBuf;

	char						m_szProductBrand[PROPERTY_VALUE_MAX];
	char						m_szProductModel[PROPERTY_VALUE_MAX];
	char						m_szBoardPlatform[PROPERTY_VALUE_MAX];
	int							m_nVersionNumber[3];
	VO_BOARDPLATFORM_TYPE		m_eBoardPlatformType;

#ifdef _VOLOG_INFO
	bool						m_bFirstLock;
#endif	// _VOLOG_INFO
};

}; // namespace android

#endif //#define __CNativeWindowVideoRender_h__

