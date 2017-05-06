/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2011		*
*																		*
************************************************************************/
/*******************************************************************************
	File:		CAudioRenderManager.h

	Contains:	CAudioRenderManager header file

	Written by:	Jim Lin 

	Change History (most recent first):
	2011-11-16		Jim			Create file

*******************************************************************************/
#ifndef __CAUDIORENDER_MANAGER_h__
#define __CAUDIORENDER_MANAGER_h__

#include "voBaseRender.h"
#include "CBaseAudioRender.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class CAudioRenderManager : public voBaseRender
{
public:
	CAudioRenderManager(void* pUserData, RenderQueryCallback pQuery);
	virtual	~CAudioRenderManager(void);

	virtual int			Start (void);
	virtual int			Pause (void);
	virtual int			Stop (void);
	virtual int			Flush (void);

	virtual int			SetVolume(float leftVolume, float rightVolume);

	virtual	int			RenderThreadLoop (void);
	
protected:
	virtual	int			Open (int sampleRate, int channelCount, int format, int bufferCount);
    static  int         AudioCallBack(VO_PTR pUserData, VO_CODECBUFFER * pAudioBuffer, VO_AUDIO_FORMAT * pAudioFormat, VO_S32 nStart);
    virtual int         doAudioCallBack(VO_CODECBUFFER * pAudioBuffer, VO_AUDIO_FORMAT * pAudioFormat, VO_S32 nStart);
    virtual int         fillData(VO_CODECBUFFER * pAudioBuffer, VO_AUDIO_FORMAT * pAudioFormat, VO_S32 nFilledSize);
    
private:
	void				InitAudioRender();
	
protected:
    bool                m_bNeedInit;
	CBaseAudioRender*	m_pAudioRender;
	int					m_nStreamType;
	float				m_dLeftVolume;
	float				m_dRightVolume;

	VOMP_BUFFERTYPE *	m_pAudioBuffer;
	unsigned int		m_uRenderSize;
	
	int                 m_nSetBufferTime;
    
    int                 m_nReadSize;
};

#endif // __CAUDIORENDER_MANAGER_h__

