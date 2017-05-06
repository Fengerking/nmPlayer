	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCEditPlayer.h

	Contains:	voCEditPlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifndef __voCEditPlayer_H__
#define __voCEditPlayer_H__

#include "voCBasePlayer.h"

#include "vompEdit.h"
#include "vompDrm.h"

class voCMediaPlayer;

class voCEditPlayer : public voCBasePlayer
{
public:
	voCEditPlayer(VOMPListener pListener, void * pUserData);
	virtual ~voCEditPlayer(void);

	virtual int 		SetDataSource (void * pSource, int nFlag);
	virtual int			GetVideoBuffer (VOMP_BUFFERTYPE ** ppBuffer);
	virtual int			GetAudioBuffer (VOMP_BUFFERTYPE ** ppBuffer);
	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual int 		Stop (void);
	virtual int 		Flush (void);
	virtual int 		GetStatus (VOMP_STATUS * pStatus);
	virtual int 		GetDuration (int * pDuration);
	virtual int 		GetCurPos (int * pCurPos);
	virtual int 		SetCurPos (int nCurPos);
	virtual int 		GetParam (int nID, void * pValue);
	virtual int 		SetParam (int nID, void * pValue);

	static  int			OnListener (void * pUserData, int nID, void * pParam1, void * pParam2);
	virtual int			HandleEvent (int nID, void * pParam1, void * pParam2);

protected:
	virtual int			CheckSource (VOMP_EDITOR_SOURCE * pSource);
	virtual int			CreatePlayer (int nIndex);
	virtual int			LoadNextSource (bool bSync);

protected:
	voCMutex					m_mtPlayer;
	VOMP_EDITOR_SOURCE			m_sSource;
	VOMP_EDITOR_ITEM **			m_ppItems;
	VO_TCHAR **					m_ppURL;

	voCMediaPlayer **			m_ppPlayer;
	int							m_nIndex;
	bool						m_bNextReady;

	VO_U64						m_nDuration;
	VO_AUDIO_FORMAT				m_sAudioFormat;
	VO_VIDEO_FORMAT				m_sVideoFormat;
	bool						m_bAudioNewFile;
	bool						m_bVideoNewFile;
	bool						m_bAudioInLoop;
	bool						m_bVideoInLoop;

	int							m_nSeekMode;
	VOTDThreadCreate			m_fThreadCreate;
	VOMP_LIB_FUNC *				m_pLibFunc;
	VOMP_SOURCEDRM_CALLBACK *	m_pDrmCB;
	int							m_nDisableDropVideoFrame;
	int							m_nVideoDropRender;
	int							m_nCheckFastPerformance;
	int							m_nGetThumbnail;
	int							m_nColorType;

};

#endif // __voCEditPlayer_H__