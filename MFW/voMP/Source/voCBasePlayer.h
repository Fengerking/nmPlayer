	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCBasePlayer.h

	Contains:	voCBasePlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifndef __voCBasePlayer_H__
#define __voCBasePlayer_H__

#include "vompAPI.h"

#include "voCMutex.h"
#include "voThread.h"

#include "CBaseConfig.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

class voCBasePlayer
{
public:
	voCBasePlayer(VOMPListener pListener, void * pUserData);
	virtual ~voCBasePlayer(void);

	virtual int 		SetDataSource (void * pSource, int nFlag);
	virtual int 		SendBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer);
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
	virtual int			CloseSource (bool bSync);

	virtual void		SetInstance (VO_PTR hInst) {m_hInst = hInst;}

protected:
	voCMutex			m_mtListen;
	VOMPListener 		m_pListener;
	void *				m_pUserData;
	VO_PTR				m_hInst;

	VOTDThreadCreate	m_fThreadCreate;
	
	VO_LOG_PRINT_CB		m_cbVOLOG;
};

#endif // __voCBasePlayer_H__