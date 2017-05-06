	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXClockTime.cpp

	Contains:	voCOMXClockTime class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"
#include "voOMX_Index.h"
#include "voCOMXClockTime.h"

#define LOG_TAG "voCOMXClockTime"
#include "voLog.h"

voCOMXClockTime::voCOMXClockTime(OMX_COMPONENTTYPE * pComponent)
	: voCOMXBaseComponent (pComponent)
	, m_ppClockPort (NULL)
	, m_sUpdateType (OMX_TIME_UpdateMax)
	, m_nBaseMediaTime (0)
	, m_nBaseWallTime (0)
	, m_nStartWallTime (0)
	, m_bUpdatedMediaTime (OMX_FALSE)
	, m_nStartWallClock (1)
	, m_nAudioStreamStop (0)
	, m_nSourceType (0)
	, m_llAudioPlayTime (0)
	, m_llAudioStepTime (0)
	, m_nAudioTimeCount (0)
	, m_nVideoRenderTime (-1)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Clock.Time");

	voOMXMemSet (&m_sClockState, 0, sizeof (OMX_TIME_CONFIG_CLOCKSTATETYPE));
	voOMXBase_SetHeader (&m_sClockState, sizeof (OMX_TIME_CONFIG_CLOCKSTATETYPE), NULL);

	voOMXMemSet (&m_sRefClock, 0, sizeof (OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE));
	voOMXBase_SetHeader (&m_sRefClock, sizeof (OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE), NULL);
	m_sRefClock.eClock = OMX_TIME_RefClockAudio;

	voOMXMemSet (&m_sMediaTime, 0, sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE));
	voOMXBase_SetHeader (&m_sMediaTime, sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE), NULL);

	voOMXMemSet (&m_nScaleType, 0, sizeof (OMX_TIME_CONFIG_SCALETYPE));
	voOMXBase_SetHeader (&m_nScaleType, sizeof (OMX_TIME_CONFIG_SCALETYPE), NULL);
	m_nScaleType.xScale = 1 << 16;
}

voCOMXClockTime::~voCOMXClockTime(void)
{
}

OMX_ERRORTYPE voCOMXClockTime::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
						OMX_IN  OMX_INDEXTYPE nIndex,
						OMX_INOUT OMX_PTR pComponentConfigStructure)
{
	voCOMXAutoLock lock (&m_tmClock);

	switch (nIndex)
	{
	case OMX_IndexConfigTimeScale:
		{
			OMX_TIME_CONFIG_SCALETYPE * pScale = (OMX_TIME_CONFIG_SCALETYPE *)pComponentConfigStructure;

			pScale->xScale = m_nScaleType.xScale;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexConfigTimeClockState:
		{
			OMX_TIME_CONFIG_CLOCKSTATETYPE * pState = (OMX_TIME_CONFIG_CLOCKSTATETYPE *)pComponentConfigStructure;

			voOMXMemCopy (pState, &m_sClockState, sizeof (OMX_TIME_CONFIG_CLOCKSTATETYPE));

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexConfigTimeActiveRefClock:
		{
			OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE * pRefClock = (OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE *)pComponentConfigStructure;

			pRefClock->eClock = m_sRefClock.eClock;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexConfigTimeCurrentMediaTime:
		{
			OMX_TIME_CONFIG_TIMESTAMPTYPE * pMediaTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pComponentConfigStructure;

			if ((m_nBaseMediaTime == 0 && m_nBaseWallTime == 0) && (m_sRefClock.eClock == OMX_TIME_RefClockAudio && m_nAudioStreamStop == 0))
			{
				pMediaTime->nTimestamp = 0;
				return OMX_ErrorNone;
			}

			if (m_nBaseWallTime == 0)
			{
				m_nBaseWallTime = voOMXOS_GetSysTime ();
				// pMediaTime->nTimestamp = m_nBaseMediaTime;
				// return OMX_ErrorNone;
			}

			OMX_TICKS tmNow = 0;
			if (m_nStartWallClock > 0)
			{
				OMX_U32 uSysTime = (OMX_U32)((voOMXOS_GetSysTime () - m_nBaseWallTime) * m_nScaleType.xScale / 0X10000);

				if (m_nSourceType == 0 && m_sRefClock.eClock == OMX_TIME_RefClockAudio && m_nAudioStreamStop == 0)
				{
					if (uSysTime > (OMX_U32)m_llAudioStepTime)
						uSysTime = (OMX_U32)m_llAudioStepTime;
				}

				tmNow = m_nBaseMediaTime + uSysTime;
			}
			else
			{
				tmNow = m_nBaseMediaTime;
			}

			pMediaTime->nTimestamp = tmNow;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexConfigTimeCurrentWallTime:
		{
			OMX_TIME_CONFIG_TIMESTAMPTYPE * pWallTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pComponentConfigStructure;

			pWallTime->nTimestamp = voOMXOS_GetSysTime () - m_nStartWallTime;

			return OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexVideoRenderTime:
		{
			if(m_nVideoRenderTime == -1)
				return OMX_ErrorNotReady;

			*((OMX_S64*)pComponentConfigStructure) = m_nVideoRenderTime;
			return OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexAudioStreamStop:
		{
			*((OMX_U32*)pComponentConfigStructure) = m_nAudioStreamStop;
			return OMX_ErrorNone;
		}

	default:
		break;
	}

	return voCOMXBaseComponent::GetConfig (hComponent, nIndex, pComponentConfigStructure);
}


OMX_ERRORTYPE voCOMXClockTime::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
													OMX_IN  OMX_INDEXTYPE nIndex,
													OMX_IN  OMX_PTR pComponentConfigStructure)
{
	voCOMXAutoLock lock (&m_tmClock);

	switch (nIndex)
	{
	case OMX_IndexConfigTimeScale:
		{
			OMX_TIME_CONFIG_SCALETYPE * pScale = (OMX_TIME_CONFIG_SCALETYPE *)pComponentConfigStructure;

			m_nScaleType.xScale = pScale->xScale;

			return OMX_ErrorNone;
		}

	case OMX_IndexConfigTimeClockState:
		{
			OMX_TIME_CONFIG_CLOCKSTATETYPE * pState = (OMX_TIME_CONFIG_CLOCKSTATETYPE *)pComponentConfigStructure;
			voOMXMemCopy (&m_sClockState, pState, sizeof (OMX_TIME_CONFIG_CLOCKSTATETYPE));

			if (pState->eState == OMX_TIME_ClockStateRunning)
			{
				if (m_nStartWallTime == 0)
					m_nStartWallTime = voOMXOS_GetSysTime ();
			}
			else
			{
				for (OMX_U32 i = 0; i < m_uPorts; i++)
					m_ppClockPort[i]->CancelRequestTime ();
			}

			return OMX_ErrorNone;
		}

	case OMX_IndexConfigTimeActiveRefClock:
		{
			OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE * pRefClock = (OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE *)pComponentConfigStructure;

			m_sRefClock.eClock = pRefClock->eClock;

			for (OMX_U32 i = 0; i < m_uPorts; i++)
				m_ppClockPort[i]->SetCompClockType (m_sRefClock.eClock);

			return OMX_ErrorNone;
		}

	case OMX_IndexConfigTimeCurrentAudioReference:
		{
			OMX_TIME_CONFIG_TIMESTAMPTYPE * pAudioRef = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pComponentConfigStructure;

			if (m_sRefClock.eClock == OMX_TIME_RefClockAudio && m_sState == OMX_StateExecuting)
			{
				m_nAudioTimeCount++;
				m_llAudioPlayTime = m_llAudioPlayTime + ( pAudioRef->nTimestamp - m_nBaseMediaTime);
				m_llAudioStepTime = (m_llAudioPlayTime / m_nAudioTimeCount) * 2;
				if (m_llAudioStepTime < 100)
					m_llAudioStepTime = 100;

				m_nBaseMediaTime = pAudioRef->nTimestamp;
				m_nBaseWallTime = voOMXOS_GetSysTime ();
			}

			return OMX_ErrorNone;
		}

	case OMX_IndexConfigTimeCurrentVideoReference:
		{
			OMX_TIME_CONFIG_TIMESTAMPTYPE * pVideoRef = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pComponentConfigStructure;

			// reference video or audio has stopped, East 2010/09/02
			if (m_sRefClock.eClock == OMX_TIME_RefClockVideo || m_nAudioStreamStop != 0)
			{
				if (!m_bUpdatedMediaTime)
				{
					m_nBaseMediaTime = pVideoRef->nTimestamp;
					m_nBaseWallTime = voOMXOS_GetSysTime ();
					m_bUpdatedMediaTime = OMX_TRUE;
				}
			}

			return OMX_ErrorNone;
		}

	case OMX_IndexConfigTimeMediaTimeRequest:
		{
			OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE * pRequest = (OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE *)pComponentConfigStructure;

			if (m_sRefClock.eClock == OMX_TIME_RefClockAudio)
			{
				if (m_ppClockPort[pRequest->nPortIndex]->GetClockType () != OMX_TIME_RefClockVideo)
					return OMX_ErrorNone;
			}
/*
			else if (m_sRefClock.eClock == OMX_TIME_RefClockVideo)
			{
				if (m_ppClockPort[pRequest->nPortIndex]->GetClockType () != OMX_TIME_RefClockAudio)
					return OMX_ErrorNone;
			}
*/
			//if (m_sClockState.eState == OMX_TIME_ClockStateRunning)
				m_ppClockPort[pRequest->nPortIndex]->AddRequestTime (pRequest);

			return OMX_ErrorNone;
		}

	case OMX_IndexConfigTimeClientStartTime:
		{
			OMX_TIME_CONFIG_TIMESTAMPTYPE * pStartTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pComponentConfigStructure;

			if (m_sClockState.eState == OMX_TIME_ClockStateWaitingForStartTime)
			{
				m_nBaseMediaTime = pStartTime->nTimestamp;
				m_nBaseWallTime = voOMXOS_GetSysTime ();

				m_sClockState.eState = OMX_TIME_ClockStateRunning;

				m_bUpdatedMediaTime = OMX_TRUE;
			}

			return OMX_ErrorNone;
		}

	case OMX_VO_IndexStartWallClock:
		{
			m_nStartWallClock = (OMX_U32) pComponentConfigStructure;
			if (m_nStartWallClock > 0)
				m_nBaseWallTime = voOMXOS_GetSysTime ();
			else
			{
				if (m_sRefClock.eClock == OMX_TIME_RefClockVideo || m_nAudioStreamStop != 0)
					m_bUpdatedMediaTime = OMX_FALSE;
			}

			VOLOGI ("OMX_VO_IndexStartWallClock: %d ", (int)m_nStartWallClock);
			return OMX_ErrorNone;
		}

	case OMX_VO_IndexAudioStreamStop:
		{
			m_nAudioStreamStop = (OMX_U32) pComponentConfigStructure;

			VOLOGI ("OMX_VO_IndexAudioStreamStop: %d ", (int)m_nAudioStreamStop);
			return OMX_ErrorNone;
		}

	case OMX_VO_IndexSourceType:
		{
			m_nSourceType = *((OMX_U32 *)pComponentConfigStructure);

			VOLOGI ("OMX_VO_IndexSourceType: %d ", (int)m_nSourceType);
			return OMX_ErrorNone;
		}

	case OMX_VO_IndexVideoRenderTime:
		{
			m_nVideoRenderTime = *((OMX_S64*)pComponentConfigStructure);
			return OMX_ErrorNone;
		}

	default:
		break;
	}

	return voCOMXBaseComponent::SetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXClockTime::CreatePorts (void)
{
	if (m_uPorts == 0)
	{
		m_uPorts = 3;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXPortClock *));
		if (m_ppPorts == NULL)
			return OMX_ErrorInsufficientResources;

		for (OMX_U32 i = 0; i < m_uPorts; i++)
		{
			m_ppPorts[i] = new voCOMXPortClock (this, i, OMX_DirOutput);
			if (m_ppPorts[i] == NULL)
				return OMX_ErrorInsufficientResources;
			m_ppPorts[i]->SetCallbacks (m_pCallBack, m_pAppData);
		}

		m_ppClockPort = (voCOMXPortClock**)m_ppPorts;

		InitPortType ();
	}

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXClockTime::SetNewState (OMX_STATETYPE newState)
{
	if (newState == OMX_StateExecuting)
	{
		if (m_sClockState.eState == OMX_TIME_ClockStateStopped)
			m_sClockState.eState = OMX_TIME_ClockStateRunning;

		m_nBaseWallTime = 0;

		if (m_sRefClock.eClock == OMX_TIME_RefClockVideo || m_nAudioStreamStop != 0)
			m_bUpdatedMediaTime = OMX_FALSE;
	}
	else if (newState == OMX_StateIdle)
	{
		m_bUpdatedMediaTime = OMX_FALSE;

		m_nBaseMediaTime = 0;
		m_nBaseWallTime = 0;

		m_sClockState.eState = OMX_TIME_ClockStateStopped;

		m_llAudioPlayTime = 0;
		m_llAudioStepTime = 0;
		m_nAudioTimeCount = 0;

		m_nVideoRenderTime = -1;

		if (m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
			return OMX_ErrorNone;
	}

	return voCOMXBaseComponent::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXClockTime::Flush (OMX_U32	nPort)
{
	voCOMXAutoLock lock (&m_tmClock);

	m_bUpdatedMediaTime = OMX_FALSE;

	m_nBaseMediaTime = 0;
	m_nBaseWallTime = 0;
	
	m_nVideoRenderTime = -1;

	return voCOMXBaseComponent::Flush (nPort);
}

OMX_U32 voCOMXClockTime::BufferHandle (void)
{
/*
	if (!m_bUpdatedMediaTime)
	{
		voOS_Sleep (10);
		return 0;
	}
*/

//	if (m_sClockState.eState == OMX_TIME_ClockStateRunning)
	{
		for (OMX_U32 i = 0; i < m_uPorts; i++)
		{
			m_ppClockPort[i]->CheckRequestTime ();
			voOMXOS_Sleep (5);
		}
	}
//	else
	{
//		voOS_Sleep (10);
	}

	return 0;
}


OMX_ERRORTYPE voCOMXClockTime::InitPortType (void)
{
	m_portParam[OMX_PortDomainOther].nPorts = 3;
	m_portParam[OMX_PortDomainOther].nStartPortNumber = 0;

	return OMX_ErrorNone;
}
