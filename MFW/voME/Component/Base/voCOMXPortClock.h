	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXPortClock.h

	Contains:	voCOMXPortClock header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXPortClock_H__
#define __voCOMXPortClock_H__

#include "voCOMXBasePort.h"

#include "voCOMXThreadMutex.h"
#include "voCOMXThreadSemaphore.h"
#include "voCOMXTaskQueue.h"
#include "voOMXThread.h"

class voCOMXClockTime;

class voCOMXPortClock : public voCOMXBasePort
{
friend class voCOMXClockTime;

public:
	voCOMXPortClock(voCOMXBaseComponent * pParent, OMX_S32 nIndex, OMX_DIRTYPE nDir);
	virtual ~voCOMXPortClock(void);

	virtual OMX_ERRORTYPE	TunnelRequest(OMX_COMPONENTTYPE * hTunneledComp, OMX_U32 nTunneledPort, 
										  OMX_TUNNELSETUPTYPE* pTunnelSetup);

	virtual OMX_ERRORTYPE	SetClockType (OMX_TIME_REFCLOCKTYPE nClockType);
	OMX_TIME_REFCLOCKTYPE	GetClockType (void);
	virtual OMX_ERRORTYPE	SetCompClockType (OMX_TIME_REFCLOCKTYPE nClockType);

	virtual OMX_ERRORTYPE	SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans);
	virtual OMX_ERRORTYPE	Flush (void);

	 virtual OMX_ERRORTYPE	HandleBuffer (OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	UpdateMediaTime (OMX_TICKS	nMediaTime);
	virtual OMX_ERRORTYPE	RequestMediaTime (OMX_TICKS	nMediaTime, OMX_TICKS nOffset);
	virtual OMX_ERRORTYPE	GetMediaTime (OMX_TICKS	* pMediaTime);

	virtual OMX_ERRORTYPE	AddRequestTime (OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE * pRequest);
	virtual OMX_ERRORTYPE	CheckRequestTime (void);
	virtual OMX_ERRORTYPE	CancelRequestTime (void);

	virtual OMX_ERRORTYPE	StartWallClock (OMX_U32 nStart);
	virtual OMX_ERRORTYPE	AudioStreamStop (OMX_U32 nStop);

	virtual OMX_ERRORTYPE	SetVideoRenderTime(OMX_TICKS nVideoRenderTime);
	virtual OMX_ERRORTYPE	GetVideoRenderTime(OMX_TICKS* pnVideoRenderTime);

	virtual void	OutputDebugStatus (OMX_STATETYPE nNewState);

protected:
	voCOMXClockTime *						m_pClockComp;
	OMX_TIME_REFCLOCKTYPE					m_nClockType;
	OMX_TIME_REFCLOCKTYPE					m_nCompClockType;

	voCOMXTaskQueue							m_tqRequest;
	voCOMXThreadSemaphore					m_tmRequest;

	OMX_TIME_CONFIG_TIMESTAMPTYPE			m_MediaTime;
	OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE	m_RequestTime;
};

#endif //__voCOMXPortClock_H__
