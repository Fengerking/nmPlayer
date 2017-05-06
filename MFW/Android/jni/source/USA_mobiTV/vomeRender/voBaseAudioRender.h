/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		voBaseAudioRender.h

Contains:	voBaseAudioRender header file

Written by:	Tom Yu Wei 

Change History (most recent first):
2010-08-31		Tom Yu			Create file

*******************************************************************************/

#if !defined __VO_BASE_AUDIO_RENDER_H__
#define __VO_BASE_AUDIO_RENDER_H__

#include <sys/types.h>
#include <utils/Errors.h>
#include <media/AudioSystem.h>


#include "voCOMXThreadMutex.h"

namespace android {

// ----------------------------------------------------------------------------
class VOBaseAudioRender
{
public:
	VOBaseAudioRender();
	virtual	~VOBaseAudioRender();

	virtual status_t		open(uint32_t sampleRate, int channelCount, int format, int bufferCount) = 0;

	virtual void			start() = 0;
	virtual ssize_t			write(const void* buffer, size_t size) = 0;
	virtual void			stop() = 0;
	virtual void			flush() = 0;
	virtual void			pause() = 0;
	virtual void			close();
	virtual void			Init(){};

	virtual void			setAudioStreamType(int streamType) { m_nStreamType = streamType; }
	virtual void			setVolume(float left, float right) = 0;

	virtual bool			isOpen (void) {return m_bOpen;}

protected:
	bool					m_bOpen;
	int                     m_nStreamType;
	float                   m_dLeftVolume;
	float                   m_dRightVolume;

	voCOMXThreadMutex		m_tmTrack;


};

}; // namespace android

#endif //#define __VO_BASE_AUDIO_RENDER_H__

