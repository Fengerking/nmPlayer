	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXRTSPOutputPort.cpp

	Contains:	voCOMXRTSPOutputPort class file

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
#include "voOMXFile.h"

#include "voCOMXRTSPSource.h"
#include "voCOMXRTSPOutputPort.h"

#include "CFileSource.h"
#include "voLog.h"

#define LOG_TAG "voCOMXRTSPOutputPort"

voCOMXRTSPOutputPort::voCOMXRTSPOutputPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXPortSource (pParent, nIndex)
	, m_pSource (NULL)
	, m_pLoadMutex (NULL)
	, m_pVideoPort (NULL)
	, m_nTrackIndex (-1)
	, m_nFourCC (0)
	, m_nStartPos (0)
	, m_nCurrentPos (0)
	, m_bEOS (OMX_FALSE)
	, m_bDecOnly (OMX_FALSE)
	, m_bHadBuffering (OMX_FALSE)
{
	strcpy (m_pObjName, __FILE__);
	voOMXMemSet (&m_Sample, 0, sizeof (VO_SOURCE_SAMPLE));

	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
}

voCOMXRTSPOutputPort::~voCOMXRTSPOutputPort(void)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
}

OMX_ERRORTYPE voCOMXRTSPOutputPort::SetTrack (CFileSource * pSource, OMX_S32 nTrackIndex)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	m_pSource = pSource;
	m_nTrackIndex = nTrackIndex;

	if (pSource == NULL)
		return OMX_ErrorInvalidState;

	m_pSource->GetTrackInfo (m_nTrackIndex, &m_trkInfo);
	m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_CODECCC, &m_nFourCC);

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXRTSPOutputPort::GetStreams (OMX_U32 * pStreams)
{
	*pStreams = 1;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXRTSPOutputPort::SetStream (OMX_U32 nStream)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXRTSPOutputPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	switch (nIndex)
	{
	case OMX_IndexParamCommonExtraQuantData:
		{
			OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *) pParam;
			if (pExtData->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorBadPortIndex;

			if (m_pSource == NULL)
				return OMX_ErrorInvalidState;

			if (m_trkInfo.HeadSize > pExtData->nDataSize)
				return OMX_ErrorBadParameter;

			if (m_trkInfo.HeadSize  == 0)
			{
				pExtData->eType = OMX_ExtraDataNone;
				pExtData->nDataSize = 0;
			}
			else
			{
				voOMXMemCopy (pExtData->data, m_trkInfo.HeadData, m_trkInfo.HeadSize);
				pExtData->nDataSize = m_trkInfo.HeadSize;
				pExtData->eType = OMX_ExtraDataQuantization;
			}

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::GetParameter (nIndex, pParam);
}


OMX_ERRORTYPE voCOMXRTSPOutputPort::GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
	switch (nIndex)
	{
	case OMX_VO_IndexConfigTimeDuration:
		{
			if (!IsEnable ())
				return OMX_ErrorIncorrectStateOperation;

			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pConfig;
			if (pTime->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorBadPortIndex;

			if (m_pSource == NULL)
				return OMX_ErrorInvalidState;

			pTime->nTimestamp = m_trkInfo.Duration;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexConfigTimePosition:
		{
			if (!IsEnable ())
				return OMX_ErrorIncorrectStateOperation;

			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pConfig;
			if (pTime->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorBadPortIndex;

			if (m_pSource == NULL)
				return OMX_ErrorInvalidState;

			pTime->nTimestamp = m_nCurrentPos;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::GetConfig (nIndex, pConfig);
}

OMX_ERRORTYPE voCOMXRTSPOutputPort::SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
	switch (nIndex)
	{
	case OMX_IndexConfigTimePosition:
		{
			if (!IsEnable ())
				return OMX_ErrorIncorrectStateOperation;

			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pConfig;
			if (pTime->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorBadPortIndex;

			if (m_pSource == NULL)
				return OMX_ErrorInvalidState;

			VO_S64 nNewPos =  m_nStartPos + pTime->nTimestamp;
			m_pSource->SetTrackPos (m_nTrackIndex, &nNewPos);

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::SetConfig (nIndex, pConfig);
}

OMX_ERRORTYPE voCOMXRTSPOutputPort::SetTrackPos (OMX_S64 * pPos)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	if (m_pSource == NULL)
		return OMX_ErrorUndefined;

	VO_S64 nNewPos =  *pPos;

	VO_U32 nRC = 0;
	nRC = m_pSource->SetTrackPos (m_nTrackIndex, &nNewPos);
	if (nRC == VO_ERR_SOURCE_END)
	{
		m_Sample.Time = nNewPos;
		m_nStartPos = nNewPos;
		m_nCurrentPos = nNewPos;

		//East 2009/12/25
		m_bEOS = OMX_FALSE;

		return OMX_ErrorNone;
	}
	else if (nRC != VO_ERR_NONE)
	{
		VO_S64 nStartPos = 0;
		nRC = m_pSource->SetTrackPos (m_nTrackIndex, &nStartPos);

		if (nRC != VO_ERR_NONE)
		{
			VOLOGE ("m_pSource->SetTrackPos was failed. 0X%08X", nRC);
			return OMX_ErrorNotImplemented;
		}
	}

	m_Sample.Time = nNewPos;
	m_nStartPos = nNewPos;

	m_nCurrentPos = nNewPos;

	*pPos = nNewPos;
	if ((*pPos) < 0)
		*pPos = 0;

	m_bEOS = OMX_FALSE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXRTSPOutputPort::SetNewCompState (OMX_STATETYPE sNew, OMX_TRANS_STATE sTrans)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	return voCOMXPortSource::SetNewCompState (sNew, sTrans);
}

OMX_ERRORTYPE voCOMXRTSPOutputPort::ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
	VOLOGR ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	if (m_bEOS)
	{
		if (IsTunnel () && IsSupplier ())
		{
			m_tqBuffer.Add (pBuffer);
			return OMX_ErrorNone;
		}
	}

	if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
		m_bEOS = OMX_TRUE;

	return voCOMXPortSource::ReturnBuffer (pBuffer);
}

OMX_ERRORTYPE voCOMXRTSPOutputPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	VOLOGR ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	voCOMXAutoLock lock (m_pLoadMutex);
	if (m_pSource == NULL)
		return OMX_ErrorInvalidState;

	pBuffer->nFlags = 0;

	VO_U32	nRC = 0;
	nRC = m_pSource->GetTrackData (m_nTrackIndex, &m_Sample);
	if (nRC == VO_ERR_NONE)
	{
		m_nCurrentPos = m_Sample.Time;

		pBuffer->nOffset = 0;
		pBuffer->nFilledLen = m_Sample.Size & 0X7FFFFFFF;

		if (m_sType.nBufferCountActual == 1)
			pBuffer->pBuffer = m_Sample.Buffer;
		else
			memcpy (pBuffer->pBuffer, m_Sample.Buffer, pBuffer->nFilledLen);
		pBuffer->nTimeStamp = m_Sample.Time;

		if (m_bHadBuffering)
		{
			m_bHadBuffering = OMX_FALSE;
			pBuffer->nFlags |= OMX_BUFFERFLAG_STARTTIME;
		}

		VOLOGR ("Comp %s, Index %d. The buffer size %d, time %d", m_pParent->GetName (), m_sType.nPortIndex, pBuffer->nFilledLen, (int)pBuffer->nTimeStamp);
	}
	else if (nRC == VO_ERR_SOURCE_END)
	{
		pBuffer->nFilledLen = 0;
		pBuffer->nFlags = OMX_BUFFERFLAG_EOS;

		voOMXOS_Sleep (10);

		VOLOGI ("Comp %s, Index %d.nRC == VO_ERR_SOURCE_END", m_pParent->GetName (), m_sType.nPortIndex);
	}
	else if (nRC == VO_ERR_SOURCE_NEEDRETRY)
	{
		pBuffer->nFlags = 0;
		pBuffer->nFilledLen = 0;
		voOMXOS_Sleep (2);

		VOLOGR ("Comp %s, Index %d.nRC == VO_ERR_SOURCE_NEEDRETRY", m_pParent->GetName (), m_sType.nPortIndex);
	}
	else
	{
		pBuffer->nFlags = 0;
		pBuffer->nFilledLen = 0;
		voOMXOS_Sleep (2);

		VOLOGE ("Comp %s, Index %d.Get track data failed. 0X%08X", m_pParent->GetName (), m_sType.nPortIndex, nRC);
		return OMX_ErrorUndefined;
	}

	return OMX_ErrorNone;
}

