	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CALSARender.h

	Contains:	CALSARender header file

	Written by:	david

	Change History (most recent first):
	2009-05-06		Create file

*******************************************************************************/

#ifndef __CALSARender_H__
#define __CALSARender_H__

#include <stdio.h>
#include <alsa/asoundlib.h>

#include "voCMutex.h"
#include "CBaseAudioRender.h"
#include "voastruct.h"


class CALSARender : public CBaseAudioRender {

public:
	CALSARender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CALSARender (void);

	VO_U32 		SetFormat (VO_AUDIO_FORMAT * pFormat);
	VO_U32 		Start (void);
	VO_U32 		Pause (void);
	VO_U32 		Stop (void);
	VO_U32 		Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait);
	VO_U32 		Flush (void);
	VO_U32 		GetPlayingTime (VO_S64	* pPlayingTime);
	VO_U32 		GetBufferTime (VO_S32	* pBufferTime);
	VO_U32 		SetParam (VO_U32 nID, VO_PTR pValue);
	VO_U32 		GetParam (VO_U32 nID, VO_PTR pValue);

protected:
	bool		InitDevice (void);
	bool		CloseDevice (void);

	inline int setprm();//
	inline int sethwparams();
	inline int setswparams();
	inline snd_pcm_format_t getfmt();
	inline int delayed();
	inline int output(const VO_PBYTE, const VO_U32);
	inline bool	mono2stero(VO_PBYTE pdst, const VO_PBYTE psrc, const int nsize);

protected:
	snd_pcm_t*						m_hdev;
	snd_mixer_t*					m_hmixer;
  snd_mixer_elem_t*			m_pmixerelem;

	snd_pcm_uframes_t			m_buffersize;
	snd_pcm_uframes_t			m_periodsize;
	snd_pcm_hw_params_t*	m_phwparams;
	snd_pcm_sw_params_t*	m_pswparams;

	unsigned char*				m_pmonobuf;
	char*									m_szswdev;
	char*									m_szswctl;
	bool									m_bstereo4mono;
	VOARSTAT							m_status;
	PVOAUDIOPARAM					m_pvoaparam;
	int										m_nPlayingTime;
	voCMutex							m_csTime;
	unsigned short				m_wsamplesize;
};

#endif // __CALSARender_H__

