/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
	File:		vome2CAudioRender.h

	Contains:	vome2CAudioRender header file

	Written by:	Tom Yu Wei 

	Change History (most recent first):
	2010-09-25		Bang			Create file

*******************************************************************************/
#ifndef __vome2CAudioRender_h__
#define __vome2CAudioRender_h__

//#include <sys/types.h>
//#include <utils/Errors.h>

//#include <sys/types.h>
//#include <utils/Errors.h>

#include "vome2CBaseRender.h"
#include "CBaseAudioRender.h"

class vome2CAudioRender : public vome2CBaseRender
{
public:
	vome2CAudioRender(vompCEngine * pEngine);
	virtual	~vome2CAudioRender(void);

	virtual int			Start (void);
	virtual int			Pause (void);
	virtual int			Stop (void);
	virtual int			Flush (void);

	virtual int			SetVolume(float leftVolume, float rightVolume);

	virtual	int			vome2RenderThreadLoop (void);
	virtual	void		vome2RenderThreadStart (void);
	
protected:
	
	virtual	int			Open (int sampleRate, int channelCount, int format, int bufferCount);

private:
	void				initAudioRender();
	
protected:
	CBaseAudioRender*	m_pAudioRender;
	int					m_nStreamType;
	float				m_dLeftVolume;
	float				m_dRightVolume;

	VOMP_BUFFERTYPE *	m_pAudioBuffer;
	unsigned int		m_uRenderSize;
	
	bool				m_bSetBufferTime;
};

#endif //#define __vome2CAudioRender_h__

