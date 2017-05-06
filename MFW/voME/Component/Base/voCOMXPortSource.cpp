	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXPortSource.cpp

	Contains:	voCOMXPortSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXBase.h"
#include "voOMX_Index.h"
#include "voOMXOSFun.h"
#include "voCOMXBaseComponent.h"
#include "voCOMXPortSource.h"

#define LOG_TAG "voCOMXPortSource"
#include "voLog.h"

voCOMXPortSource::voCOMXPortSource(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXBasePort (pParent, nIndex, OMX_DirOutput)
	, m_hBuffThread (NULL)
	, m_nBuffThreadID (0)
	, m_bBuffThreadStop(OMX_TRUE)
	, m_bResetingBuff (OMX_FALSE)
	, m_pWorkBuffer (NULL)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXPortSource::~voCOMXPortSource(void)
{
	if (m_hBuffThread != NULL)
		ExitBufferThread ();
}

OMX_ERRORTYPE voCOMXPortSource::SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans)
{
	if (m_tsState.Waiting ())
		m_tsState.Up ();

	if (!IsEnable ())
		return OMX_ErrorNone;

	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	if (sNew == OMX_StateIdle && sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_hBuffThread == NULL)
		{
			m_bBuffThreadStop = OMX_FALSE;
			voOMXThreadCreate (&m_hBuffThread, &m_nBuffThreadID, (voOMX_ThreadProc) voPortBufferHandleProc, this, 0);
			if (m_hBuffThread == NULL)
			{
				VOLOGE ("Comp %s, Index %d. Create the source buffer thread was failed.", m_pParent->GetName (), (int)m_sType.nPortIndex);
				return OMX_ErrorUndefined;
			}

			voOMXOS_Sleep (10);
		}

		if (IsTunnel () && IsSupplier ())
		{
			errType = AllocTunnelBuffer (m_sType.nPortIndex, m_sType.nBufferSize);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Comp %s, Index %d. AllocTunnelBuffer was failed. 0X%08X.", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
				return errType;
			}
		}
		else
		{
			m_tsAlloc.Down ();
		}
	}
	else if (sNew == OMX_StateIdle && (sTrans == COMP_TRANSSTATE_PauseToIdle || sTrans == COMP_TRANSSTATE_ExecuteToIdle))
	{
		ResetBuffer (OMX_FALSE);
	}
	else if (sTrans == COMP_TRANSSTATE_IdleToLoaded || sTrans == COMP_TRANSSTATE_AnyToInvalid)
	{
		ExitBufferThread ();

		if (IsTunnel () && IsSupplier ())
		{
			errType = FreeTunnelBuffer (m_sType.nPortIndex);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Comp %s, Index %d. FreeTunnelBuffer was failed. 0X%08X.", m_pParent->GetName (), (int)m_sType.nPortIndex, errType);
				return errType;
			}
		}
		else
		{
			m_tsAlloc.Down ();
		}
	}
	else if (sNew == OMX_StateExecuting && (sTrans == COMP_TRANSSTATE_IdleToExecute || sTrans == COMP_TRANSSTATE_PauseToExecute))
	{
		if (IsTunnel () && IsSupplier ())
		{
			OMX_STATETYPE	nState = OMX_StateLoaded;
			OMX_U32			nTryTimes = 0;
			m_hTunnelComp->GetState (m_hTunnelComp, &nState);

			while (nState != OMX_StateExecuting && nState != OMX_StatePause)
			{
				voOMXOS_Sleep (10);
				m_hTunnelComp->GetState (m_hTunnelComp, &nState);

				if (nTryTimes > 100)
				{
					VOLOGE ("Comp %s, Index %d. m_hTunnelComp->GetState was not run.", m_pParent->GetName (), (int)m_sType.nPortIndex);
					break;
				}
				nTryTimes++;
			}

			if (sTrans == COMP_TRANSSTATE_IdleToExecute || m_bResetBuffer)
			{
				m_bResetBuffer = OMX_FALSE;
				for (OMX_U32 i = 0; i < m_tqBuffer.Count (); i++)
					m_tsBuffer.Up ();
			}
		}
	}

	OutputDebugStatus (sNew);

	return OMX_ErrorNone;
}

OMX_U32 voCOMXPortSource::voPortBufferHandleProc (OMX_PTR pParam)
{
	voCOMXPortSource *	pSourcePort = (voCOMXPortSource *)pParam;

#ifdef _LINUX
	char threadname[255];
	memset( threadname , 0 , sizeof(threadname) );
	strcpy( threadname , pSourcePort->m_pParent->GetName() );

	if( strstr( threadname , "VisualOn.Video.Sink" ) )
		strcpy( threadname , "VO.VS." );
	else if( strstr( threadname , "VisualOn.Audio.Sink" ) )
		strcpy( threadname , "VO.AS." );
	else if( strstr( threadname , "VisualOn.Video.Decoder" ) )
		strcpy( threadname , "VO.VD." );
	else if( strstr( threadname , "VisualOn.Audio.Decoder" ) )
		strcpy( threadname , "VO.AD." );
	else if( strstr( threadname , "VisualOn.FileSource" ) )
		strcpy( threadname , "VO.FS." );
	else if( strstr( threadname , "VisualOn.Clock.Time" ) )
		strcpy( threadname , "VO.CT." );

	strcat( threadname , "PBufHandle" );
	
	voSetThreadName( threadname );
#endif

	OMX_STATETYPE	nState = pSourcePort->m_pParent->GetCompState ();
	OMX_U32			nTryTimes = 0;
	while (nState != OMX_StateExecuting && nState != OMX_StatePause)
	{
		voOMXOS_Sleep (10);
		nState = pSourcePort->m_pParent->GetCompState ();

		if (pSourcePort->m_bBuffThreadStop)
			break;

		if (nState == OMX_StateLoaded || nState == OMX_StateInvalid)
			break;

		if (pSourcePort->m_pParent->GetTransState () == COMP_TRANSSTATE_IdleToLoaded)
			break;

		nTryTimes++;
		if (nTryTimes > 500)
		{
			voCOMXBaseComponent *	pComp = pSourcePort->m_pParent;
			VOLOGE ("Comp %s, Index %d. m_pParent->GetState was not run.", pComp->GetName (), (int)(pSourcePort->m_sType.nPortIndex));
			break;
		}
	}

	pSourcePort->BeforeHandleBuffer ();

	while (!pSourcePort->m_bBuffThreadStop)
	{
		pSourcePort->BufferHandle ();
	}

	pSourcePort->AfterHandleBuffer ();

	voOMXThreadClose (pSourcePort->m_hBuffThread, 0);
	pSourcePort->m_hBuffThread = NULL;

	return 0;
}

OMX_ERRORTYPE voCOMXPortSource::BeforeHandleBuffer (void)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXPortSource::AfterHandleBuffer (void)
{
	return OMX_ErrorNone;
}

OMX_U32 voCOMXPortSource::BufferHandle (void)
{
	if (!m_bFlushing)
	{
		if ((m_pParent->GetCompState () == OMX_StatePause &&  m_pParent->GetTransState () == COMP_TRANSSTATE_None) ||
			m_pParent->GetTransState () == COMP_TRANSSTATE_ExecuteToPause)
		{
			m_tsState.Down ();
		}
	}

	m_tsBuffer.Down ();
	m_pWorkBuffer = (OMX_BUFFERHEADERTYPE *)m_tqBuffer.Remove ();
	if (m_pWorkBuffer == NULL)
		return 0;

//	if (m_pParent->GetCompState () == OMX_StateIdle && m_pParent->GetTransState () == COMP_TRANSSTATE_IdleToLoaded)
	if (m_pParent->GetCompState () <= OMX_StateIdle)
	{
		// VOLOGW ("Comp %s, Index %d. The status was wrong.!", m_pParent->GetName (), m_sType.nPortIndex);
		ReturnBuffer (m_pWorkBuffer);
		m_pWorkBuffer = NULL;
		return OMX_ErrorInvalidState;
	}

	if (m_bBuffThreadStop || m_bFlushing)
	{
		ReturnBuffer (m_pWorkBuffer);
		m_pWorkBuffer = NULL;
		return 0;
	}

	if (m_pParent->GetTransState () != COMP_TRANSSTATE_None)
	{
		while (m_pParent->GetTransState () == COMP_TRANSSTATE_IdleToExecute || m_pParent->GetTransState () == COMP_TRANSSTATE_PauseToExecute)
			voOMXOS_Sleep (2);
	}

	m_pWorkBuffer->nFilledLen = 0;
	m_pWorkBuffer->nTimeStamp = 0;
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pParent->IsRunning () && !m_bSettingsChanging)
		errType = FillBuffer (m_pWorkBuffer);
	else
		voOMXOS_Sleep (2);

	if (OMX_ErrorNone != errType)
	{
		if (m_bResetingBuff)
		{
			ReturnBuffer (m_pWorkBuffer);
			m_pWorkBuffer = NULL;
		}
		else
		{
			m_tqBuffer.Add (m_pWorkBuffer);
			m_tsBuffer.Up ();
			m_pWorkBuffer = NULL;
		}

		if(OMX_ErrorStreamCorrupt == errType)
			m_pCallBack->EventHandler(m_pParent->GetComponent(), m_pAppData, OMX_EventPortSettingsChanged, m_sType.nPortIndex, 0, NULL);
	}
	else
	{
		if (m_pWorkBuffer->nFilledLen == 0 && (m_pWorkBuffer->nFlags & OMX_BUFFERFLAG_EOS) == 0)
		{
			if (m_bResetingBuff)
			{
				ReturnBuffer (m_pWorkBuffer);
				m_pWorkBuffer = NULL;
			}
			else
			{
				m_tqBuffer.Add (m_pWorkBuffer);
				m_tsBuffer.Up ();
				m_pWorkBuffer = NULL;
			}
		}
		else
		{
			ReturnBuffer (m_pWorkBuffer);
			m_pWorkBuffer = NULL;
		}
	}

	return 0;
}

OMX_ERRORTYPE voCOMXPortSource::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	pBuffer->nFilledLen = 0;

	return OMX_ErrorNotImplemented;
}

OMX_ERRORTYPE voCOMXPortSource::ResetBuffer (OMX_BOOL bRelease)
{
	if (m_sType.bPopulated == OMX_FALSE)
		return OMX_ErrorNone;

	if (!IsEnable ())
		return OMX_ErrorNone;

	m_bResetingBuff = OMX_TRUE;

	OMX_U32	nResetBuffers = 0;
	if (IsTunnel () && IsSupplier ())
		nResetBuffers = m_sType.nBufferCountActual;

	OMX_U32 nTryTimes = 0;
	while (m_tqBuffer.Count () !=  nResetBuffers || m_pWorkBuffer != NULL)
	{
		if (m_tsState.Waiting ())
		{
			m_tsState.Up ();

			// wait for BufferHandle since m_tsStage.Down -> m_tqBuffer.Remove need some time
			voOMXOS_Sleep (5);
			nTryTimes += 5;
		}
		else
		{
			voOMXOS_Sleep (1);
			nTryTimes++;
		}

		if (bRelease)
		{
			if (m_tsBuffer.Waiting ())
				m_tsBuffer.Up ();
		}

		if (nTryTimes > 1000)
		{
			VOLOGE ("Comp %s, Index %d, Time Out", m_pParent->GetName (), (int)m_sType.nPortIndex);
			break;
		}
	}

//	if (IsSupplier ()  && m_sType.eDomain != OMX_PortDomainOther)
	if (!bRelease)
	{
		if (m_ppBufferHead != NULL)
		{
			for (OMX_U32 i = 0; i < m_sType.nBufferCountActual; i++)
			{
				if (m_ppBufferHead[i] != NULL)
				{
					m_ppBufferHead[i]->nFilledLen = 0;
					m_ppBufferHead[i]->nTimeStamp = 0;
					m_ppBufferHead[i]->hMarkTargetComponent = NULL;
					m_ppBufferHead[i]->pMarkData = NULL;
					m_ppBufferHead[i]->nFlags = 0;
				}
			}
		}
	}

	m_bResetingBuff = OMX_FALSE;
	m_bResetBuffer = OMX_TRUE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXPortSource::ExitBufferThread (void)
{
	m_bBuffThreadStop = OMX_TRUE;
	m_tsBuffer.Up ();

	OMX_U32 nTryTimes = 0;

	while (m_hBuffThread != NULL)
	{
		if (m_tsState.Waiting ())
			m_tsState.Up ();

		if (m_tsBuffer.Waiting ())
			m_tsBuffer.Up ();
		voOMXOS_Sleep (1);

		nTryTimes++;
		if (nTryTimes > 1000)
		{
			VOLOGE ("Comp %s, Index %d. It was time out to stop buffer thread.", m_pParent->GetName (), (int)m_sType.nPortIndex);
			break;
		}
	}

	return OMX_ErrorNone;
}
