	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voJAVAAudioRender.h

	Contains:	voJAVAAudioRender header file

	Written by:	Tom Yu Wei

	Change History (most recent first):
	2010-08-31		Tom Yu			Create file

*******************************************************************************/
#if !defined __VO_JAVA_AUDIO_RENDER_H__
#define __VO_JAVA_AUDIO_RENDER_H__

#include "voOMXThread.h"
#include "voCOMXThreadMutex.h"
#include "voCOMXThreadSemaphore.h"
#include "voCOMXTaskQueue.h"
#include "voType.h"
#include "vojniconst.h"

#include "voBaseAudioRender.h"


#define JAVAAR_OPEN	     			0X1001
#define JAVAAR_START	     		0X1002
#define JAVAAR_WRITE	     		0X1003
#define JAVAAR_STOP	     			0X1004
#define JAVAAR_FLUSH	     		0X1005
#define JAVAAR_PAUSE     			0X1006
#define JAVAAR_CLOSE	     		0X1007
#define JAVAAR_SETVOLUME	    	0X1008
#define JAVAAR_EXIT			    	0X1009

namespace android {

// ----------------------------------------------------------------------------
class VOJAVAAudioRender : public VOBaseAudioRender
{
public:
	VOJAVAAudioRender();
	virtual	~VOJAVAAudioRender();

	virtual status_t		open(uint32_t sampleRate, int channelCount, int format, int bufferCount);
	virtual void			start();
	virtual ssize_t			write(const void* buffer, size_t size);
	virtual void			stop();
	virtual void			flush();
	virtual void			pause();
	virtual void			close();
	virtual void			setVolume(float left, float right);

public:
	static void				vomeARThreadProc (OMX_PTR pparam);
	void					RenderAudioLoop(JNIEnv * env);

	void					Init();
	inline void				AllocateBuffer(JNIEnv* env, int nBufferSize);

protected:

	jclass						mClass;
	jmethodID 					mMethodOpen;
	jmethodID 					mMethodStart;
	jmethodID 					mMethodWrite;
	jmethodID 					mMethodStop;
	jmethodID 					mMethodFlush;
	jmethodID 					mMethodPause;
	jmethodID 					mMethodClose;
	jmethodID 					mMethodSetVolume;
	jbyteArray					mRD;
	jfloat    					mLeftVolume;
	jfloat	  					mRightVolume;

	VOA_NOTIFYEVENT				m_sEvent;
	voCOMXThreadSemaphore		m_tsEventStart;
	voCOMXThreadSemaphore		m_tsEventFinish;

	bool	  					m_bThreadEventStop;
    bool						m_bThreadExit;
    bool						m_bStoped;
	int							m_nCmdEvent;
	int       					m_nBufferSize;
};

}; // namespace android

#endif //#define __VO_JAVA_AUDIO_RENDER_H__

