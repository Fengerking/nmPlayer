	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CWaveOutRender.h

	Contains:	CWaveOutRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/

#ifndef __CWaveOutRender_H__
#define __CWaveOutRender_H__

#include <stdio.h>

#include "CPtrList.h"
#include "voCMutex.h"

#include "CBaseAudioRender.h"

typedef enum {
	VOMM_AR_LOADED		= 0,
	VOMM_AR_RUNNING,
	VOMM_AR_PAUSED,
	VOMM_AR_STOPPED
} VOMM_AR_STATUS;

class CWaveOutRender : public CBaseAudioRender
{
public:
	static bool CALLBACK VoiceWaveOutProc(HWAVEOUT hwo, UINT uMsg,  DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

public:
	CWaveOutRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP);
	virtual ~CWaveOutRender (void);

	virtual VO_U32 		SetFormat (VO_AUDIO_FORMAT * pFormat);
	virtual VO_U32 		Start (void);
	virtual VO_U32 		Pause (void);
	virtual VO_U32 		Stop (void);
	virtual VO_U32 		Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait);
	virtual VO_U32 		Flush (void);
	virtual VO_U32 		GetPlayingTime (VO_S64	* pPlayingTime);
	virtual VO_U32 		GetBufferTime (VO_S32	* pBufferTime);

protected:
	virtual bool		AllocBuffer (void);
	virtual bool		ReleaseBuffer (void);
	virtual	bool		InitDevice (void);
	virtual bool		CloseDevice (void);

	virtual bool		AudioDone (WAVEHDR * pWaveHeader);

protected:
	HWAVEOUT				m_hWaveOut;
	VOMM_AR_STATUS			m_status;
	bool					m_bFlush;

	voCMutex				m_csList;
	CObjectList<WAVEHDR>	m_lstFull;
	CObjectList<WAVEHDR>	m_lstFree;

	WAVEHDR *				mCurWaveHdr;

	VO_U32					m_nBufSize;
	WAVEFORMATEX 			m_wavFormat;

	voCMutex				m_csTime;
	DWORD					m_dwSysTime;
	int						m_nPlayingTime;
	int						m_nBufferTime;

	VO_MEM_INFO				m_memInfo;
};

#endif // __CWaveOutRender_H__