	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCLivePlayer.h

	Contains:	voCLivePlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifndef __voCLivePlayer_H__
#define __voCLivePlayer_H__

#include "voCMediaPlayer.h"
#include "voCMixDataSource.h"

class voCLivePlayer : public voCMediaPlayer
{
public:
	voCLivePlayer(VOMPListener pListener, void * pUserData);
	virtual ~voCLivePlayer(void);

	virtual int 		SetDataSource (void * pSource, int nFlag);
	virtual int 		SendBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer);
	virtual int			GetAudioBuffer (VOMP_BUFFERTYPE ** ppBuffer);

	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual int 		Stop (void);
	virtual int 		Flush (void);
	virtual int 		GetDuration (int * pDuration);
	virtual int 		GetCurPos (int * pCurPos);
	virtual int 		SetCurPos (int nCurPos);

	virtual int			ReadAudioSample (void);

	virtual int			onRenderAudio (int nMsg, int nV1, int nV2);
	virtual int			onRenderVideo (int nMsg, int nV1, int nV2);

	virtual int			SendAudioSample (VOMP_BUFFERTYPE * pBuffer);
	virtual int			SendVideoSample (VOMP_BUFFERTYPE * pBuffer);

protected:
	virtual int			Reset (void);

	virtual bool		CreateAudioDec (void);
	virtual int			DecAudioSample (void);	
	virtual int			RenderAudioSample (void);
	virtual int			DoAudioRender (VOMP_BUFFERTYPE ** ppBuffer);

	virtual bool		CreateVideoDec (void);
	virtual int			DecVideoSample (void);	
	virtual int			RenderVideoSample (void);
	virtual int			CheckVideoRenderTime (void);
	virtual int			DoVideoRender (VOMP_BUFFERTYPE ** ppBuffer);

	virtual VO_U64		GetPlayingTime (VO_BOOL bForUser = VO_FALSE);

	virtual int			ReleaseAudioBuffer (void);

protected:
	int					m_nBufferMode;
	VO_S64				m_llAVOffsetTime;
	voCMixDataSource *	m_pMixSource;

	// for multi audio frames output
	voCMutex					m_mtAudioRender;
	VO_U32						m_nAudioRenderCount;
	VOMP_BUFFERTYPE **			m_ppAudioOutBuffers;
	VO_U32						m_nAudioOutFillIndex;
	VO_U32						m_nAudioOutPlayIndex;

	VOMP_AUDIO_FORMAT			m_sAudioRndFmt;
	VOMP_BUFFERTYPE *			m_pAudioRndBuff;
	VOMP_BUFFERTYPE				m_sAudioEmptyData;

	// for audio source buffer
	voCMutex					m_mtAudioSource;
	VO_U32						m_nAudioSourceCount;
	VOMP_BUFFERTYPE **			m_ppAudioSrcBuffers;
	VO_U32						m_nAudioSrcFillIndex;
	VO_U32						m_nAudioSrcPlayIndex;

	VO_S64						m_llLastAudioRndTime;

	VO_S64						m_llStartVideoTime;
};

#endif // __voCLivePlayer_H__
