	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		VOAudioRender.h

	Contains:	VOAudioRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/
#if !defined __VO_AUDIO_RENDER_H__
#define __VO_AUDIO_RENDER_H__

#include <sys/types.h>
#include <utils/Errors.h>
#include <media/AudioSystem.h>
#include <media/AudioTrack.h>

#include "voCOMXThreadMutex.h"

#include "voBaseAudioRender.h"

namespace android {

// ----------------------------------------------------------------------------
class VOAudioRender : public VOBaseAudioRender
{
public:
	VOAudioRender();
	virtual	~VOAudioRender();

	virtual status_t		open(uint32_t sampleRate, int channelCount, int format, int bufferCount);

	virtual void			start();
	virtual ssize_t			write(const void* buffer, size_t size);
	virtual void			stop();
	virtual void			flush();
	virtual void			pause();
	virtual void			close();

	virtual void			setAudioStreamType(int streamType) { m_nStreamType = streamType; }
	virtual void			setVolume(float left, float right);

private:
	AudioTrack*             m_pTrack;
};

}; // namespace android

#endif //#define __VO_AUDIO_RENDER_H__

