	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXG1VideoCapPort.cpp

	Contains:	voCOMXG1VideoCapPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif

#include <voOMX_Index.h>
#include <voOMXBase.h>
#include <voOMXOSFun.h>
#include <voOMXMemory.h>

#include "voCOMXG1VideoCapPort.h"
#include "voCOMXG1VideoCapture.h"

#define LOG_TAG "voCOMXG1VideoCapPort"
#include "voLog.h"

voCOMXG1VideoCapPort::voCOMXG1VideoCapPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXPortSource (pParent, nIndex)
	, m_nSourceType (0)
	, m_pSendBuffer (NULL)
	, m_nEncFrames (0)
	, m_nFrameRate (0)
	, m_llFirstTime (0)
{
	strcpy (m_pObjName, __FILE__);

	memset (&m_videoType, 0, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));
	m_videoType.nFrameWidth                    = 640;	//320;
	m_videoType.nFrameHeight                   = 480;	//240;

	m_sType.eDomain                            = OMX_PortDomainVideo;
	m_sType.nBufferCountActual                 = 1;
	m_sType.nBufferCountMin                    = 1;
	m_sType.nBufferSize                        = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;
	m_sType.bBuffersContiguous                 = OMX_FALSE;
	m_sType.nBufferAlignment                   = 1;

	m_sType.format.video.nFrameWidth           = m_videoType.nFrameWidth;
	m_sType.format.video.nFrameHeight          = m_videoType.nFrameHeight;
	m_sType.format.video.nStride               = m_videoType.nFrameWidth * 2;
	m_sType.format.video.nSliceHeight          = 16;
	m_sType.format.video.nBitrate              = m_sType.nBufferSize * 20;
	m_sType.format.video.xFramerate            = 20;
	m_sType.format.video.bFlagErrorConcealment = OMX_FALSE;
	m_sType.format.video.eCompressionFormat    = OMX_VIDEO_CodingUnused;
	m_sType.format.video.eColorFormat          = OMX_COLOR_FormatYUV420PackedPlanar; //(20) <==> VO_COLOR_YUV_420_PACK_2(35) in VO
	m_sType.format.video.pNativeWindow         = NULL;
	m_sType.format.video.pNativeRender         = NULL;

	m_pCfgComponent = new CBaseConfig ();
	m_pCfgComponent->Open ((TCHAR*)_T("vomeplay.cfg"));

	m_nFrameRate = m_pCfgComponent->GetItemValue (m_pParent->GetName (), (char*)"FrameRate", 0);

	VOLOGI ("Comp %s, Index %d. Video Size W %d, H %d, CF %d", m_pParent->GetName (), (int)m_sType.nPortIndex, 
		(int)m_videoType.nFrameWidth, (int)m_videoType.nFrameHeight, (int)m_sType.format.video.eColorFormat);
}

voCOMXG1VideoCapPort::~voCOMXG1VideoCapPort(void)
{
	delete m_pCfgComponent;
}

OMX_ERRORTYPE voCOMXG1VideoCapPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	switch (nIndex)
	{
	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pParam;
			pVideoFormat->eCompressionFormat = m_sType.format.video.eCompressionFormat;
			pVideoFormat->eColorFormat       = m_sType.format.video.eColorFormat;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::GetParameter (nIndex, pParam);
}


OMX_ERRORTYPE voCOMXG1VideoCapPort::SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	switch (nIndex)
	{
	case OMX_IndexParamPortDefinition:
		{
			OMX_PARAM_PORTDEFINITIONTYPE * pType = (OMX_PARAM_PORTDEFINITIONTYPE *)pParam;
			if (pType->eDomain != OMX_PortDomainVideo)
				return OMX_ErrorPortsNotCompatible;

			SetPortType (pType);

			voOMXMemCopy (&m_videoType, &pType->format.video, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));

			m_sType.format.video.nFrameWidth  = m_videoType.nFrameWidth;
			m_sType.format.video.nFrameHeight = m_videoType.nFrameHeight;
			m_sType.format.video.nStride      = m_videoType.nFrameWidth;
			m_sType.format.video.nBitrate     = m_videoType.nBitrate;
			m_sType.format.video.xFramerate   = m_videoType.xFramerate;
			m_sType.format.video.eColorFormat = m_videoType.eColorFormat;
			m_sType.nBufferSize = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;

			m_nFrameRate = m_videoType.xFramerate;

			VOLOGI ("Comp %s, Index %d. Video Size W %d, H %d, CF %d", m_pParent->GetName (), (int)m_sType.nPortIndex, 
						(int)m_videoType.nFrameWidth, (int)m_videoType.nFrameHeight, (int)m_sType.format.video.eColorFormat);
		}
		break;

	case OMX_VO_IndexSourceType:
		m_nSourceType = *(OMX_U32 *)pParam;
		break;

	case OMX_VO_IndexSendBuffer:
		{
			if (!m_pParent->IsExecuting ())
			{
				VOLOGW ("Comp %s, Index %d. The Status was wrong.", m_pParent->GetName (), (int)m_sType.nPortIndex);
//				return OMX_ErrorInvalidState;
			}

			if (m_pSendBuffer != NULL)
				return OMX_ErrorNotImplemented;

			voCOMXAutoLock lock (&m_tmSendBuffer);
			m_pSendBuffer = (OMX_BUFFERHEADERTYPE *)pParam;

			VOLOGR ("Comp %s, Index %d. OMX_VO_IndexSendBuffer  Size %d  Time %d.", 
				m_pParent->GetName (), m_sType.nPortIndex, m_pSendBuffer->nFilledLen, (int)m_pSendBuffer->nTimeStamp);

			m_tsSendBuffer.Up ();

			if (m_nSourceType == 1) // The source is file.
			{
				int nTryTimes = 0;
				while (m_pSendBuffer != NULL)
				{
					voOMXOS_Sleep (1);
					nTryTimes++;
					if (nTryTimes > 500)
						break;
				}
			}

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::SetParameter (nIndex, pParam);
}

OMX_ERRORTYPE voCOMXG1VideoCapPort::SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans)
{
	if (sNew == OMX_StateIdle && (sTrans == COMP_TRANSSTATE_PauseToIdle || sTrans == COMP_TRANSSTATE_ExecuteToIdle))
	{
		if (m_tsSendBuffer.Waiting ())
			m_tsSendBuffer.Up ();
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

			return OMX_ErrorNone;
		}
	}

	return voCOMXPortSource::SetNewCompState (sNew, sTrans);
}

OMX_ERRORTYPE voCOMXG1VideoCapPort::Flush (void)
{
	if (m_tsSendBuffer.Waiting ())
		m_tsSendBuffer.Up ();

	m_nEncFrames = 0;

	return voCOMXPortSource::Flush ();
}

OMX_U32 voCOMXG1VideoCapPort::BufferHandle (void)
{
	if (!m_bFlushing)
	{
		if ((m_pParent->GetCompState () == OMX_StatePause &&  m_pParent->GetTransState () == COMP_TRANSSTATE_None) ||
			m_pParent->GetTransState () == COMP_TRANSSTATE_ExecuteToPause)
		{
			m_tsState.Down ();
		}
	}

	if (m_bBuffThreadStop)
		return 0;

	m_tsBuffer.Down ();
	OMX_BUFFERHEADERTYPE * pBuffer = (OMX_BUFFERHEADERTYPE *)m_tqBuffer.Remove ();
	if (pBuffer == NULL)
	{
		//VOLOGI ("Comp %s, Index %d. The buffer is NULL!", m_pParent->GetName (), m_sType.nPortIndex);
		return 0;
	}

	if (m_pParent->GetCompState () <= OMX_StateIdle)
	{
		//VOLOGW ("Comp %s, Index %d. The status was wrong.!", m_pParent->GetName (), m_sType.nPortIndex);
		ReturnBuffer (pBuffer);
		return OMX_ErrorInvalidState;
	}

	if (m_bFlushing)
	{
		ReturnBuffer (pBuffer);
		return 0;
	}

	if (m_pParent->GetTransState () != COMP_TRANSSTATE_None)
	{
		while (m_pParent->GetTransState () == COMP_TRANSSTATE_IdleToExecute || m_pParent->GetTransState () == COMP_TRANSSTATE_PauseToExecute)
			voOMXOS_Sleep (2);
	}

	pBuffer->nFilledLen = 0;
	pBuffer->nTimeStamp = 0;
	OMX_U32 nRC = 0;
	if (m_pParent->IsRunning ())
	{
		nRC = FillBuffer (pBuffer);
	}
	else
	{
		voOMXOS_Sleep (2);
	}

	if (nRC != OMX_ErrorNone)
	{
		if (m_bResetingBuff)
		{
			ReturnBuffer (pBuffer);
		}
		else
		{
			m_tqBuffer.Add (pBuffer);
			m_tsBuffer.Up ();
		}
	}
	else
	{
		if (pBuffer->nFilledLen == 0 && (pBuffer->nFlags & OMX_BUFFERFLAG_EOS) == 0)
		{
			if (m_bResetingBuff)
			{
				ReturnBuffer (pBuffer);
			}
			else
			{
				m_tqBuffer.Add (pBuffer);
				m_tsBuffer.Up ();
			}
		}
		else
		{
			ReturnBuffer (pBuffer);
		}
	}

	return 0;
}

OMX_ERRORTYPE voCOMXG1VideoCapPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	m_tsSendBuffer.Down ();

	if (m_pSendBuffer == NULL)
	{
		voOMXOS_Sleep (1);
		return OMX_ErrorUnderflow;
	}

//	voCOMXAutoLock lock (&m_tmSendBuffer);

//	memcpy (pBuffer->pBuffer, m_pSendBuffer->pBuffer + m_pSendBuffer->nOffset, m_pSendBuffer->nFilledLen);
	pBuffer->pBuffer = m_pSendBuffer->pBuffer + m_pSendBuffer->nOffset;
	pBuffer->nOffset    = 0;
	pBuffer->nFilledLen = m_pSendBuffer->nFilledLen;
	pBuffer->nFlags    |= m_pSendBuffer->nFlags;
	pBuffer->nTickCount = m_pSendBuffer->nTickCount;

	if (m_nFrameRate == 0 || m_nSourceType > 0)
	{
		m_llFrameTime = m_pSendBuffer->nTimeStamp;
		pBuffer->nTimeStamp = m_pSendBuffer->nTimeStamp;
	}
	else
	{
		if (m_llFirstTime == 0)
			m_llFirstTime = m_pSendBuffer->nTimeStamp;
		m_llFrameTime = m_pSendBuffer->nTimeStamp - m_llFirstTime;

		if (m_llFrameTime + 200 < m_nEncFrames * 1000 / m_nFrameRate)
		{
			pBuffer->nFilledLen = 0;
			pBuffer->nTimeStamp = m_llFrameTime + m_llFirstTime;
		}
		else
		{
			int nCount = 1;
			while (m_llFrameTime > ((m_nEncFrames + nCount) * 1000 / m_nFrameRate))
				nCount++;

			if (nCount < 2)
			{
				m_llFrameTime = m_nEncFrames * 1000 / m_nFrameRate;
				m_nEncFrames++;
			}
			else
			{
				m_nEncFrames = (m_nEncFrames + nCount - 2);
				m_llFrameTime = m_nEncFrames * 1000 / m_nFrameRate;
				m_nEncFrames++;
			}

			pBuffer->nTimeStamp = m_llFrameTime + m_llFirstTime;
		}
	}

	m_pSendBuffer = NULL;
		
	return OMX_ErrorNone;
}

