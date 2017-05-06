	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXClockTime.h

	Contains:	voCOMXClockTime header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXClockTime_H__
#define __voCOMXClockTime_H__

#include "voCOMXBaseComponent.h"
#include "voCOMXPortClock.h"

class voCOMXClockTime : public voCOMXBaseComponent
{
friend class voCOMXPortClock;

public:
	voCOMXClockTime(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXClockTime(void);

	virtual OMX_ERRORTYPE GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex, 
										OMX_INOUT OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex, 
										OMX_IN  OMX_PTR pComponentConfigStructure);

protected:
	virtual OMX_ERRORTYPE	CreatePorts (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);

	virtual OMX_ERRORTYPE	InitPortType (void);


protected:
	virtual OMX_U32			BufferHandle (void);

protected:
	voCOMXPortClock **					m_ppClockPort;

	voCOMXThreadMutex					m_tmClock;


	OMX_TIME_CONFIG_CLOCKSTATETYPE		m_sClockState;
	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE	m_sRefClock;
	OMX_TIME_UPDATETYPE					m_sUpdateType;
	OMX_TIME_CONFIG_TIMESTAMPTYPE		m_sMediaTime;
	OMX_TIME_CONFIG_SCALETYPE			m_nScaleType;

	OMX_TICKS							m_nBaseMediaTime;
	OMX_TICKS							m_nBaseWallTime;

	OMX_TICKS							m_nStartWallTime;

	OMX_BOOL							m_bUpdatedMediaTime;
	OMX_U32								m_nStartWallClock;
	OMX_U32								m_nAudioStreamStop;
	OMX_U32								m_nSourceType;

	OMX_TICKS							m_llAudioPlayTime;
	OMX_TICKS							m_llAudioStepTime;
	OMX_U32								m_nAudioTimeCount;

	OMX_TICKS							m_nVideoRenderTime;
};

#endif //__voCOMXClockTime_H__

