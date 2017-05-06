	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXPortClock.cpp

	Contains:	voCOMXPortClock class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif

#include "voOMXBase.h"
#include "voOMXOSFun.h"
#include "voOMXMemory.h"

#include "voCOMXBaseComponent.h"
#include "voCOMXPortClock.h"

#include "voOMX_Index.h"
#include "voLog.h"

voCOMXPortClock::voCOMXPortClock(voCOMXBaseComponent * pParent, OMX_S32 nIndex, OMX_DIRTYPE nDir)
	: voCOMXBasePort (pParent, nIndex, nDir)
	, m_pClockComp (NULL)
	, m_nClockType (OMX_TIME_RefClockMax)
	, m_nCompClockType (OMX_TIME_RefClockAudio)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	if (nDir == OMX_DirOutput)
		m_pClockComp = (voCOMXClockTime *)m_pParent;

	m_sType.eDomain = OMX_PortDomainOther;
	m_sType.nBufferCountActual = 1;
	m_sType.nBufferCountMin = 1;
	m_sType.nBufferSize = 256;
	m_sType.bBuffersContiguous = OMX_FALSE;
	m_sType.nBufferAlignment = 1;
	m_sType.format.other.eFormat = OMX_OTHER_FormatTime;

	m_nBufferSupplier = OMX_BufferSupplyInput;

	voOMXMemSet (&m_MediaTime, 0, sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE));
	voOMXBase_SetHeader (&m_MediaTime, sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE), NULL);
	m_MediaTime.nPortIndex = nIndex;

	voOMXMemSet (&m_RequestTime, 0, sizeof (OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE));
	voOMXBase_SetHeader (&m_RequestTime, sizeof (OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE), NULL);
	m_RequestTime .nPortIndex = nIndex;


}

voCOMXPortClock::~voCOMXPortClock(void)
{
}

OMX_ERRORTYPE voCOMXPortClock::TunnelRequest(OMX_COMPONENTTYPE * hTunneledComp, OMX_U32 nTunneledPort, 
											 OMX_TUNNELSETUPTYPE* pTunnelSetup)
{


	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_sType.eDir == OMX_DirOutput && hTunneledComp != NULL)
	{
		OMX_PORT_PARAM_TYPE				paramType;
		OMX_PARAM_PORTDEFINITIONTYPE	portType;
		OMX_U32							i = 0;

		m_nClockType = OMX_TIME_RefClockMax;

		voOMXBase_SetHeader (&paramType, sizeof (OMX_PORT_PARAM_TYPE));
		voOMXBase_SetHeader (&portType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));

		errType = hTunneledComp->GetParameter (hTunneledComp, OMX_IndexParamAudioInit, &paramType);
		if (errType == OMX_ErrorNone && paramType.nPorts > 0)
		{
			for (i = paramType.nStartPortNumber; i < paramType.nStartPortNumber + paramType.nPorts; i++)
			{
				errType = hTunneledComp->GetParameter (hTunneledComp, OMX_IndexParamPortDefinition, &portType);
				if (portType.eDir == OMX_DirInput)
				{
					m_nClockType = OMX_TIME_RefClockAudio;
					break;
				}
			}
		}

		if (m_nClockType == OMX_TIME_RefClockMax)
		{
			errType = hTunneledComp->GetParameter (hTunneledComp, OMX_IndexParamVideoInit, &paramType);
			if (errType == OMX_ErrorNone && paramType.nPorts > 0)
			{
				for (i = paramType.nStartPortNumber; i < paramType.nStartPortNumber + paramType.nPorts; i++)
				{
					errType = hTunneledComp->GetParameter (hTunneledComp, OMX_IndexParamPortDefinition, &portType);
					if (portType.eDir == OMX_DirInput)
					{
						m_nClockType = OMX_TIME_RefClockVideo;
						break;
					}
				}
			}
		}

		if (m_nClockType == OMX_TIME_RefClockMax)
			m_nClockType = OMX_TIME_RefClockNone;
	}

	errType = voCOMXBasePort::TunnelRequest (hTunneledComp, nTunneledPort, pTunnelSetup);

	if (errType == OMX_ErrorNone)
	{
		if (m_sType.eDir == OMX_DirOutput)
		{
			pTunnelSetup->eSupplier = OMX_BufferSupplyInput;
			m_nBufferSupplier = OMX_BufferSupplyInput;
		}
	}

	return errType;
}

OMX_ERRORTYPE voCOMXPortClock::SetClockType (OMX_TIME_REFCLOCKTYPE nClockType)
{
	m_nClockType = nClockType;

	return OMX_ErrorNone;
}

OMX_TIME_REFCLOCKTYPE voCOMXPortClock::GetClockType (void)
{
	return m_nClockType;
}

OMX_ERRORTYPE voCOMXPortClock::SetCompClockType (OMX_TIME_REFCLOCKTYPE nClockType)
{
	m_nCompClockType = nClockType;

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXPortClock::SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans)
{


	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (sNew == OMX_StateIdle && sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (IsTunnel () && IsSupplier ())
		{
			errType = AllocTunnelBuffer (m_sType.nPortIndex, m_sType.nBufferSize);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Comp %s, Index %d, AllocTunnelBuffer was failed.", m_pParent->GetName (), (int)m_sType.nPortIndex);
				return errType;
			}
		}
		else
		{
			m_tsAlloc.Down ();
		}
	}
	else if (sNew == OMX_StateIdle && (sTrans == COMP_TRANSSTATE_PauseToIdle || sTrans == COMP_TRANSSTATE_PauseToIdle))
	{
		CancelRequestTime ();
	}
	else if (sTrans == COMP_TRANSSTATE_IdleToLoaded || sTrans == COMP_TRANSSTATE_AnyToInvalid)
	{
		CancelRequestTime ();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXPortClock::Flush (void)
{


	CancelRequestTime ();

	ResetBuffer (OMX_FALSE);

	m_bFlushing = OMX_FALSE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXPortClock::HandleBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	OMX_U32			nPortIndex = 0;
	errType = errType;
	if (!IsEnable ())
		return OMX_ErrorIncorrectStateOperation;

	if (m_sType.eDir == OMX_DirInput)
		nPortIndex = pBuffer->nInputPortIndex;
	else
		nPortIndex = pBuffer->nOutputPortIndex;

	if (nPortIndex != m_sType.nPortIndex)
		return OMX_ErrorBadPortIndex;

	OMX_TIME_CONFIG_TIMESTAMPTYPE * pMediaTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pBuffer->pBuffer;
	pMediaTime->nPortIndex = m_sType.nPortIndex;
	if (m_sType.eDir == OMX_DirOutput)
	{
		if (m_nClockType == m_nCompClockType)
			m_pParent->SetConfig (m_pParent->GetComponent (), OMX_IndexConfigTimeCurrentAudioReference, pMediaTime);
		else
			m_pParent->GetConfig (m_pParent->GetComponent (), OMX_IndexConfigTimeCurrentMediaTime, pMediaTime);
	}

	if (IsTunnel () && IsSupplier ())
	{
		OMX_TRANS_STATE transType = m_pParent->GetTransState ();
		OMX_STATETYPE	stateType = m_pParent->GetCompState ();

		if (IsFlush () || m_sStatus == PORT_TRANS_ENA2DIS)
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}

		if (transType == COMP_TRANSSTATE_ExecuteToIdle || 
			transType == COMP_TRANSSTATE_PauseToIdle)
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}

		if (stateType == OMX_StateIdle && 
			(transType != COMP_TRANSSTATE_IdleToExecute && transType != COMP_TRANSSTATE_IdleToPause))
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}
	}

	if (!IsSupplier ())
		ReturnBuffer (pBuffer);
	else
		m_tqBuffer.Add (pBuffer);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXPortClock::UpdateMediaTime (OMX_TICKS	nMediaTime)
{
	VOLOGR ("Comp %s, Index %d. Time %d", m_pParent->GetName (), (int)m_sType.nPortIndex, (int)nMediaTime);

	if (!IsEnable ())
		return OMX_ErrorIncorrectStateOperation;

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	m_MediaTime.nTimestamp = nMediaTime;
	m_MediaTime.nPortIndex = m_nTunnelPort;

	if (m_nClockType == OMX_TIME_RefClockAudio)
		errType = m_hTunnelComp->SetConfig (m_hTunnelComp, OMX_IndexConfigTimeCurrentAudioReference, &m_MediaTime);
	else if (m_nClockType == OMX_TIME_RefClockVideo)
		errType = m_hTunnelComp->SetConfig (m_hTunnelComp, OMX_IndexConfigTimeCurrentVideoReference, &m_MediaTime);
	else 
		errType = m_hTunnelComp->SetConfig (m_hTunnelComp, OMX_IndexConfigTimeCurrentVideoReference, &m_MediaTime);
/*

	OMX_U32					nTryTimes = 0;
	OMX_BUFFERHEADERTYPE *	pBuffHead = (OMX_BUFFERHEADERTYPE *)m_tqBuffer.Remove ();
	while (pBuffHead == NULL && nTryTimes < 10)
	{
		voOMXOS_Sleep (2);

		pBuffHead = (OMX_BUFFERHEADERTYPE *)m_tqBuffer.Remove ();
		if (pBuffHead != NULL)
			break;

		nTryTimes++;
	}

	pBuffHead->nFilledLen = sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE);
	memcpy (pBuffHead->pBuffer, &m_MediaTime, pBuffHead->nFilledLen);

	errType = ReturnBuffer (pBuffHead);
*/
	return errType;
}

OMX_ERRORTYPE voCOMXPortClock::RequestMediaTime (OMX_TICKS	nMediaTime, OMX_TICKS nOffset)
{
	VOLOGR ("Comp %s, Index %d", m_pParent->GetName (),(int)m_sType.nPortIndex);

	if (!IsEnable ())
		return OMX_ErrorIncorrectStateOperation;

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (!IsTunnel ())
		return OMX_ErrorTunnelingUnsupported;

	m_RequestTime.nMediaTimestamp = nMediaTime;
	m_RequestTime.nOffset = nOffset;
	m_RequestTime.nPortIndex = m_nTunnelPort;

	errType = m_hTunnelComp->SetConfig (m_hTunnelComp, OMX_IndexConfigTimeMediaTimeRequest, &m_RequestTime);

	return errType;
}

OMX_ERRORTYPE voCOMXPortClock::GetMediaTime (OMX_TICKS	* pMediaTime)
{
	if (!IsEnable () || !m_hTunnelComp)
		return OMX_ErrorIncorrectStateOperation;

	m_MediaTime.nPortIndex = m_nTunnelPort;
	OMX_ERRORTYPE errType = m_hTunnelComp->GetConfig (m_hTunnelComp, OMX_IndexConfigTimeCurrentMediaTime, &m_MediaTime);
	if (errType == OMX_ErrorNone)
		*pMediaTime = m_MediaTime.nTimestamp;

	VOLOGR ("Comp %s, Index %d. Time: %d", m_pParent->GetName (), (int)m_sType.nPortIndex, *pMediaTime);

/*
	OMX_U32					nTryTimes = 0;
	OMX_BUFFERHEADERTYPE *	pBuffHead = (OMX_BUFFERHEADERTYPE *)m_tqBuffer.Remove ();
	while (pBuffHead == NULL && nTryTimes < 10)
	{
		voOMXOS_Sleep (2);

		pBuffHead = (OMX_BUFFERHEADERTYPE *)m_tqBuffer.Remove ();
		if (pBuffHead != NULL)
			break;

		nTryTimes++;
	}

	pBuffHead->nFilledLen = sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE);
	memcpy (pBuffHead->pBuffer, &m_MediaTime, pBuffHead->nFilledLen);

	errType = ReturnBuffer (pBuffHead);
	if (errType == OMX_ErrorNone)
		memcpy (&m_MediaTime, pBuffHead->pBuffer, pBuffHead->nFilledLen);

	*pMediaTime = m_MediaTime.nTimestamp;
*/
	return errType;
}

OMX_ERRORTYPE voCOMXPortClock::SetVideoRenderTime(OMX_TICKS nVideoRenderTime)
{
	if(!IsEnable())
		return OMX_ErrorIncorrectStateOperation;

	return m_hTunnelComp->SetConfig(m_hTunnelComp, (OMX_INDEXTYPE)OMX_VO_IndexVideoRenderTime, &nVideoRenderTime);
}

OMX_ERRORTYPE voCOMXPortClock::GetVideoRenderTime(OMX_TICKS* pnVideoRenderTime)
{
	if(!IsEnable())
		return OMX_ErrorIncorrectStateOperation;

	return m_hTunnelComp->GetConfig(m_hTunnelComp, (OMX_INDEXTYPE)OMX_VO_IndexVideoRenderTime, pnVideoRenderTime);
}

OMX_ERRORTYPE voCOMXPortClock::AddRequestTime (OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE * pRequest)
{
	VOLOGR ("Comp %s, Index %d.", m_pParent->GetName (), m_sType.nPortIndex);

	if (!IsEnable ())
		return OMX_ErrorIncorrectStateOperation;

	char szMsg[256];
	sprintf (szMsg, "@@@ AddRequestTime: %d \r\n", (int)pRequest->nMediaTimestamp);
//	voOS_Printf (szMsg);

	m_tqRequest.Add (pRequest);

	m_tmRequest.Down ();

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXPortClock::CheckRequestTime (void)
{
	VOLOGR ("Comp %s, Index %d.", m_pParent->GetName (), (int)m_sType.nPortIndex);

	if (!IsEnable ())
		return OMX_ErrorIncorrectStateOperation;

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE * pRequest = (OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE *)m_tqRequest.First ();
	if (pRequest != NULL)
	{
		errType = m_pParent->GetConfig (m_pParent->GetComponent (), OMX_IndexConfigTimeCurrentMediaTime, &m_MediaTime);

		if (m_MediaTime.nTimestamp >= pRequest->nMediaTimestamp)
		{
			m_tqRequest.Remove ();
			m_tmRequest.Up ();

			char szMsg[256];
			sprintf (szMsg, "@@@ CheckRequestTime: %d \r\n", (int)pRequest->nMediaTimestamp);
//			voOS_Printf (szMsg);

		}
	}

	return errType;
}

OMX_ERRORTYPE voCOMXPortClock::CancelRequestTime (void)
{
	VOLOGR ("Comp %s, Index %d.", m_pParent->GetName (), (int)m_sType.nPortIndex);

	if (!IsEnable ())
		return OMX_ErrorIncorrectStateOperation;

	OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE * pRequest = (OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE *)m_tqRequest.First ();
	if (pRequest != NULL)
	{
		m_tqRequest.Remove ();
		m_tmRequest.Up ();

		char szMsg[256];
		sprintf (szMsg, "@@@ CheckRequestTime: %d \r\n", (int)pRequest->nMediaTimestamp);
//		voOMXOS_Printf (szMsg);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXPortClock::StartWallClock (OMX_U32 nStart)
{
	VOLOGR ("Comp %s, Index %d", m_pParent->GetName (), (int)m_sType.nPortIndex);

	if (!IsEnable ())
		return OMX_ErrorIncorrectStateOperation;

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (!IsTunnel ())
		return OMX_ErrorTunnelingUnsupported;

	errType = m_hTunnelComp->SetConfig (m_hTunnelComp, (OMX_INDEXTYPE)OMX_VO_IndexStartWallClock, (OMX_PTR)nStart);

	return errType;
}

OMX_ERRORTYPE voCOMXPortClock::AudioStreamStop (OMX_U32 nStop)
{
	VOLOGR ("Comp %s, Index %d", m_pParent->GetName (), (int)m_sType.nPortIndex);

	if (!IsEnable ())
		return OMX_ErrorIncorrectStateOperation;

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (!IsTunnel ())
		return OMX_ErrorTunnelingUnsupported;

	errType = m_hTunnelComp->SetConfig (m_hTunnelComp, (OMX_INDEXTYPE)OMX_VO_IndexAudioStreamStop, (OMX_PTR)nStop);

	return errType;
}

void voCOMXPortClock::OutputDebugStatus (OMX_STATETYPE nNewState)
{
}
