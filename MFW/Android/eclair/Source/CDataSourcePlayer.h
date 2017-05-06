	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDataSourcePlayer.h

	Contains:	CDataSourcePlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/
#ifndef __CDataSourcePlayer_H__
#define __CDataSourcePlayer_H__

#include "CVOMEPlayer.h"

class CDataSourcePlayer : public CVOMEPlayer
{
public:
	CDataSourcePlayer (void);
	virtual ~CDataSourcePlayer (void);

public:
	virtual int			BuildGraph (void);

	virtual int			SetPos (int	nPos);
	virtual int			GetPos (int * pPos);
	virtual int			GetDuration (int * pPos);

	virtual int			SendAudioData (VOA_DATA_BUFFERTYPE * pData, int nTime);
	virtual int			SendVideoData (VOA_DATA_BUFFERTYPE * pData, int nTime);

	virtual int			Uninit (bool bSuspend = false);

protected:
	OMX_ERRORTYPE		AddComponent (OMX_STRING pName, OMX_COMPONENTTYPE ** ppComponent);
	OMX_ERRORTYPE		RemoveComponent (OMX_COMPONENTTYPE * pComponent);
	OMX_ERRORTYPE		ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
										OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel);

protected:
	OMX_COMPONENTTYPE *		m_pCompSrc;
	OMX_COMPONENTTYPE *		m_pCompAD;
	OMX_COMPONENTTYPE *		m_pCompVD;
	OMX_COMPONENTTYPE *		m_pCompAR;
	OMX_COMPONENTTYPE *		m_pCompVR;

	OMX_COMPONENTTYPE *		m_pCompClock;

	OMX_BUFFERHEADERTYPE	m_bufAudioHead;
	OMX_BUFFERHEADERTYPE	m_bufVideoHead;

	voCOMXThreadMutex		m_mutDataSource;

};

#endif // __CDataSourcePlayer_H__
