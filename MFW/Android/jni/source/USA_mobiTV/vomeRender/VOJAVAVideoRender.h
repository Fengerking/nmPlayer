	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		VOJAVAVideoRender.h

	Contains:	VOJAVAVideoRender header file

	Written by:	Yu Wei (Tom)

	Change History (most recent first):
	2010-09-09		Tom			Create file

*******************************************************************************/

#ifndef ANDROID_VOJAVAVideoRender_H
#define ANDROID_VOJAVAVideoRender_H

#include <jni.h>

#include "voCCRR.h"
#include "VOBaseVideoRender.h"

#include "voOMXThread.h"
#include "voCOMXThreadMutex.h"
#include "voCOMXThreadSemaphore.h"
#include "voCOMXTaskQueue.h"

#include "voType.h"
#include "vojniconst.h"

#define JAVAVR_SETSURFACE    			0X1001
#define JAVAVR_SETVIDEOSIZE    			0X1002
#define JAVAVR_RENDER	     			0X1004
#define JAVAVR_EXIT		    			0X1008

namespace android {

class VOJAVAVideoRender : public VOBaseVideoRender
{
public:
	VOJAVAVideoRender(void);
	virtual ~VOJAVAVideoRender(void);

	virtual int		SetVideoSurface (const sp<Surface>& surface);
	virtual int		SetVideoSize (int nWidth, int nHeight);
	virtual bool	CheckColorType (VO_ANDROID_COLORTYPE nColorType);
	virtual bool	Render(VO_ANDROID_VIDEO_BUFFERTYPE* pVideoBuffer);

	void			Init();		

	void			RenderVideoLoop(JNIEnv * env);
	static void		vomeVRThreadProc (OMX_PTR pparam);

protected:
    voCCRR  *				m_pCCRR;
    VO_VIDEO_BUFFER			m_inBuf;
    VO_VIDEO_BUFFER			m_outBuf;

	jclass		 mClass;
	jmethodID	 mMethodInit;
	jmethodID	 mMethodRender;
	jbyteArray   mVD;

	bool	  m_bThreadEventStop;
	bool	  m_bThreadExit;

	VOA_NOTIFYEVENT				m_sEvent;
	voCOMXThreadSemaphore		m_tsEventStart;
	voCOMXThreadSemaphore		m_tsEventFinish;
	voCOMXThreadMutex		    m_tmTrack; 
};

}; // namespace android

#endif // ANDROID_VOJAVAVideoRender_H

