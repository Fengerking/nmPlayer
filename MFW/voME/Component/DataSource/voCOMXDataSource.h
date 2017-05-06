	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXDataSource.h

	Contains:	voCOMXDataSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#ifndef __voCOMXDataSource_H__
#define __voCOMXDataSource_H__

#include "voCOMXCompSource.h"
#include "voCOMXDataAudioPort.h"
#include "voCOMXDataVideoPort.h"
#include "voCOMXPortClock.h"
#include "CBaseLiveSource.h"

class voCOMXDataSource : public voCOMXCompSource
{
public:
	voCOMXDataSource(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXDataSource(void);

public:
	static VO_VOID	LiveSourceStatusCallback(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	static VO_VOID	LiveSourceDataCallback(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);

public:
	virtual OMX_ERRORTYPE GetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nParamIndex, 
										OMX_INOUT OMX_PTR pComponentParameterStructure);


	virtual OMX_ERRORTYPE SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex, 
										OMX_IN  OMX_PTR pComponentParameterStructure);


	virtual OMX_ERRORTYPE SetConfig (OMX_IN  OMX_HANDLETYPE hComponent, 
									 OMX_IN  OMX_INDEXTYPE nIndex, 
									 OMX_IN  OMX_PTR pComponentConfigStructure);

public:
	virtual VO_VOID		OnLiveSourceStatus(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	virtual VO_VOID		OnLiveSourceData(VO_U16 nOutputType, VO_PTR pData);

	virtual OMX_BOOL	IsBlockPortSettingsChangedEvent() {return m_bBlockPortSettingsChangedEvent;}

protected:
	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	OpenLiveSource (OMX_U32 nStreamingType, OMX_STRING pUrl);
	virtual OMX_ERRORTYPE	CreatePort(OMX_PORTDOMAINTYPE eType, OMX_U32 nIndex);
	virtual OMX_ERRORTYPE	CreateClockPort();
	virtual OMX_ERRORTYPE	ReleasePort (void);

protected:
	voCOMXDataAudioPort *		m_pAudioPort;
	voCOMXDataVideoPort *		m_pVideoPort;
	voCOMXPortClock *			m_pClockPort;

	CBaseLiveSource *			m_pLiveSource;
	VO_BOOL						m_bCancelSendBuffer;
	VO_S32						m_nLiveSourceDuration;
	VO_S32						m_nLiveSourceTracks;
	VO_S32						m_nLiveSourceInitializedTracks;
	VO_S64						m_llSeekPos;

	OMX_BOOL					m_bBlockPortSettingsChangedEvent;
};

#endif //__voCOMXDataSource_H__
