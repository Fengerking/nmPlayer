	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXG1AudioCapPort.cpp

	Contains:	voCOMXG1AudioCapPort class file

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

#include "voCOMXG1AudioCapPort.h"
#include "voCOMXG1AudioCapture.h"

#define LOG_TAG "voCOMXG1AudioCapPort"
#include "voLog.h"

voCOMXG1AudioCapPort::voCOMXG1AudioCapPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXPortSource (pParent, nIndex)
	, m_pSendBuffer (NULL)
{
	strcpy (m_pObjName, __FILE__);

	voOMXBase_SetHeader (&m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	m_pcmType.nPortIndex     = 0;
	m_pcmType.nChannels      = 1;	 // ok
	m_pcmType.nSamplingRate  = 8000; // ok
	m_pcmType.eNumData       = OMX_NumericalDataSigned;
	m_pcmType.eEndian        = OMX_EndianBig;
	m_pcmType.bInterleaved   = OMX_TRUE;
	m_pcmType.nBitPerSample  = 16;
	m_pcmType.ePCMMode       = OMX_AUDIO_PCMModeLinear;

	VOLOGI ("Comp %s, Index %d. SampleRate %d Channels %d", m_pParent->GetName (), (int)m_sType.nPortIndex,
		(int)m_pcmType.nSamplingRate, (int)m_pcmType.nChannels);
}

voCOMXG1AudioCapPort::~voCOMXG1AudioCapPort(void)
{
}

OMX_ERRORTYPE voCOMXG1AudioCapPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	switch (nIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pParam;
			if (pAudioFormat->nPortIndex == 0)
			{
				pAudioFormat->eEncoding = OMX_AUDIO_CodingPCM;
				return OMX_ErrorNone;
			}
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pParam;
			if (pPCMFormat->nPortIndex == 0)
			{
				voOMXMemCopy (pPCMFormat, &m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
				return OMX_ErrorNone;
			}
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::GetParameter (nIndex, pParam);
}


OMX_ERRORTYPE voCOMXG1AudioCapPort::SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;

	switch (nIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pParam;
			if (pAudioFormat->nPortIndex == 0)
			{
				if (pAudioFormat->eEncoding == OMX_AUDIO_CodingPCM)
					errType = OMX_ErrorNone;

				return errType;
			}
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pParam;
			if (pPCMFormat->nPortIndex == 0)
			{
				voOMXMemCopy (&m_pcmType, pPCMFormat, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));

				VOLOGI ("Comp %s, Index %d. SampleRate %d Channels %d", m_pParent->GetName (), (int)m_sType.nPortIndex,
							(int)m_pcmType.nSamplingRate, (int)m_pcmType.nChannels);

				return OMX_ErrorNone;
			}
		}
		break;

	case OMX_VO_IndexSendBuffer:
		{
			if (!m_pParent->IsExecuting ())
			{
				VOLOGE ("AudCap : Comp %s, Index %d. The Status was wrong.", m_pParent->GetName (), (int)m_sType.nPortIndex);
				return OMX_ErrorInvalidState;
			}

			if (m_pSendBuffer != NULL)
			{
				VOLOGE ("AudCap : Comp %s, Index %d. m_pSendBuffer != NULL.", m_pParent->GetName (), (int)m_sType.nPortIndex);
				return OMX_ErrorNotImplemented;
			}

			{
				voCOMXAutoLock lock (&m_tmSendBuffer);
				m_pSendBuffer = (OMX_BUFFERHEADERTYPE *)pParam;
				VOLOGR ("Comp %s, Index %d. OMX_VO_IndexSendBuffer  Size %d  Time %d.", 
					m_pParent->GetName (), m_sType.nPortIndex, m_pSendBuffer->nFilledLen, (int)m_pSendBuffer->nTimeStamp);
			}

			m_tsSendBuffer.Up ();

			// the audio data could not dropped
			int nTryTimes = 0;
			OMX_STATETYPE nState = m_pParent->GetCompState();
			OMX_TRANS_STATE nTrans = m_pParent->GetTransState();

			while (m_pSendBuffer != NULL )
			{
				if( nState <= OMX_StateIdle || nTrans == COMP_TRANSSTATE_ExecuteToIdle || nTrans == COMP_TRANSSTATE_PauseToIdle)
				{
					VOLOGI("AudCap :Quit waiting loop for component state = %d nTrans = %d", nState, nTrans);
					voCOMXAutoLock lock (&m_tmSendBuffer);
					m_pSendBuffer = NULL;
					break;
				}

				voOMXOS_Sleep (1);
				nTryTimes++;
				if (nTryTimes > 1000)
					break;

				nState = m_pParent->GetCompState();
				nTrans = m_pParent->GetTransState();
			}

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::SetParameter (nIndex, pParam);
}

OMX_ERRORTYPE voCOMXG1AudioCapPort::SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans)
{
	if (sNew == OMX_StateIdle && (sTrans == COMP_TRANSSTATE_PauseToIdle || sTrans == COMP_TRANSSTATE_ExecuteToIdle))
	{
		if (m_tsSendBuffer.Waiting ())
			m_tsSendBuffer.Up ();
	}

	return voCOMXPortSource::SetNewCompState (sNew, sTrans);
}

OMX_ERRORTYPE voCOMXG1AudioCapPort::Flush (void)
{
	if (m_tsSendBuffer.Waiting ())
		m_tsSendBuffer.Up ();

	return voCOMXPortSource::Flush ();
}

OMX_ERRORTYPE voCOMXG1AudioCapPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	m_tsSendBuffer.Down ();

	voCOMXAutoLock lock (&m_tmSendBuffer);

	if (m_pSendBuffer == NULL)
		return OMX_ErrorUnderflow;

	memcpy (pBuffer->pBuffer, m_pSendBuffer->pBuffer + m_pSendBuffer->nOffset, m_pSendBuffer->nFilledLen);
	pBuffer->nOffset = 0;
	pBuffer->nFilledLen = m_pSendBuffer->nFilledLen;
	pBuffer->nFlags = m_pSendBuffer->nFlags;
	pBuffer->nTimeStamp = m_pSendBuffer->nTimeStamp;
	pBuffer->nTickCount = m_pSendBuffer->nTickCount;

	m_pSendBuffer = NULL;

	return OMX_ErrorNone;
}
