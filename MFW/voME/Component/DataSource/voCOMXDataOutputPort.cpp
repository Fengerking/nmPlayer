	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXDataOutputPort.cpp

	Contains:	voCOMXDataOutputPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXOSFun.h"
#include "voOMXFile.h"
#include "voOMXMemory.h"
#include "voCOMXDataSource.h"
#include "voCOMXDataOutputPort.h"

#define LOG_TAG "voCOMXDataOutputPort"
#include "voLog.h"

voCOMXDataOutputPort::voCOMXDataOutputPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXPortSource (pParent, nIndex)
	, m_pOtherPort (NULL)
	, m_pClockPort (NULL)
	, m_llLastClockTime (0)
	, m_pLstBuffer (NULL)
	, m_llMaxBufferTime (10000)
	, m_llMinBufferTime (1000)
	, m_nFillBufferCount (0)
	, m_bEOS (OMX_FALSE)
	, m_bDecOnly (OMX_FALSE)
	, m_pCfgComponent (NULL)
	, m_hSourceFile (NULL)
	, m_nSourceFormat (0)
	, m_pSourceBuffer (NULL)
	, m_pCurrentBuffer (NULL)
	, m_llSourceSampleTime (0)
	, m_hDumpFile (NULL)
	, m_nDumpFormat (NULL)
	, m_nLogLevel (0)
	, m_nLogSysStartTime (0)
	, m_nLogSysCurrentTime (0)
	, m_bBufferFull (OMX_FALSE)
	, m_nSeekPos (-1)
{
	strcpy (m_pObjName, __FILE__);

	voOMXMemSet(&mTrackInfo , 0 , sizeof(mTrackInfo));
}

voCOMXDataOutputPort::~voCOMXDataOutputPort(void)
{
	if (m_pLstBuffer != NULL)
		delete m_pLstBuffer;
	if (m_hSourceFile != NULL)
		voOMXFileClose (m_hSourceFile);
	if (m_pSourceBuffer != NULL)
		delete []m_pSourceBuffer;
	if (m_hDumpFile != NULL)
		voOMXFileClose (m_hDumpFile);
}

void voCOMXDataOutputPort::SetConfigFile (CBaseConfig * pCfgFile)
{
	m_pCfgComponent = pCfgFile;
	if (m_pCfgComponent != NULL)
	{
		m_llMaxBufferTime = m_pCfgComponent->GetItemValue (m_pParent->GetName (), (char*)"MaxBufferTime", 10000);
		m_llMinBufferTime = m_pCfgComponent->GetItemValue (m_pParent->GetName (), (char*)"MinBufferTime", 1000);

		m_nLogLevel = m_pCfgComponent->GetItemValue (m_pParent->GetName (), (char*)"ShowLogLevel", 0);
	}
}

OMX_ERRORTYPE voCOMXDataOutputPort::SetClockPort (voCOMXPortClock * pClock)
{
	m_pClockPort = pClock;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE	voCOMXDataOutputPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	switch (nIndex)
	{
	case OMX_IndexParamCommonExtraQuantData:
		{
			OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *) pParam;
			if (pExtData->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorBadPortIndex;

			
			if (mTrackInfo.HeadSize > pExtData->nDataSize)
				return OMX_ErrorBadParameter;

			if (mTrackInfo.HeadSize  == 0)
			{
				pExtData->eType = OMX_ExtraDataNone;
				pExtData->nDataSize = 0;
			}
			else
			{
				voOMXMemCopy (pExtData->data, mTrackInfo.HeadData, mTrackInfo.HeadSize);
				pExtData->nDataSize = mTrackInfo.HeadSize;
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

OMX_ERRORTYPE voCOMXDataOutputPort::GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig)
{
	switch (nIndex)
	{
	case OMX_VO_IndexConfigTimeDuration:
		{
			if (!IsEnable ())
				return OMX_ErrorIncorrectStateOperation;

			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pConfig;
			if (pTime->nPortIndex != m_sType.nPortIndex)
			{
				VOLOGI("Get Duration return failed");
				return OMX_ErrorBadPortIndex;
			}
			
			pTime->nTimestamp = mTrackInfo.Duration;

			VOLOGI("Get duration : %d " , (int)mTrackInfo.Duration);
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

			if (m_pLstBuffer == NULL)
				pTime->nTimestamp = 0;
			else
				pTime->nTimestamp = m_pLstBuffer->GetBuffTime ();
	
			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::GetConfig (nIndex, pConfig);
}

OMX_ERRORTYPE voCOMXDataOutputPort::SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig)
{
	switch (nIndex)
	{
	case OMX_VO_IndexConfigTimeDuration:
		{
			if (!IsEnable ())
				return OMX_ErrorIncorrectStateOperation;

			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pConfig;
			if (pTime->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorBadPortIndex;
			
			m_llMaxBufferTime = pTime->nTimestamp;

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

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXPortSource::SetConfig (nIndex, pConfig);
}

OMX_ERRORTYPE voCOMXDataOutputPort::SetTrackPos (OMX_S64 * pPos)
{
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXDataOutputPort::ReturnBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
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

OMX_ERRORTYPE voCOMXDataOutputPort::Flush (void)
{
	OMX_ERRORTYPE errType =  voCOMXPortSource::Flush ();

	// only seek, we flush buffer list.
	// for PortSettingsChanged, we need keep them.
	if(OMX_FALSE == IsSettingsChanging())
	{
		if (m_pLstBuffer != NULL)
			m_pLstBuffer->Flush ();

		m_nFillBufferCount = 0;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXDataOutputPort::FlushBuffer()
{
	// only seek, we flush buffer list.
	// for PortSettingsChanged, we need keep them.
	if(OMX_FALSE == IsSettingsChanging())
	{
		if (m_pLstBuffer != NULL)
			m_pLstBuffer->Flush ();

		m_nFillBufferCount = 0;
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXDataOutputPort::SendBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
	return OMX_ErrorOverflow;
}

OMX_S32 voCOMXDataOutputPort::GetBufferTime (void)
{
	if (m_pLstBuffer == NULL)
		return -1;

	return m_pLstBuffer->GetBuffTime ();
}

OMX_ERRORTYPE voCOMXDataOutputPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	pBuffer->nFlags = 0;
	pBuffer->nFilledLen = 0;

	if (m_pLstBuffer == NULL)
	{
		voOMXOS_Sleep (2);
		return OMX_ErrorUnderflow;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	while(OMX_ErrorNone == errType)
	{
		errType = m_pLstBuffer->GetBuffer (pBuffer);
		if(OMX_ErrorStreamCorrupt == errType || -1 == m_nSeekPos)
			break;

		if(OMX_ErrorNone == errType && pBuffer->nTimeStamp >= m_nSeekPos)
		{
			m_nSeekPos = -1;
			break;
		}
		if(pBuffer->nFlags&OMX_BUFFERFLAG_CODECCONFIG)
			break;
	}

	if(errType == OMX_ErrorStreamCorrupt)	// PortSettingsChanged
	{
		VOLOGI ("PortSettingsChanged Index %d Type %d Buffer 0x%08X Length %d", (int)m_sType.nPortIndex, (int)mTrackInfo.Type, (int)pBuffer->pBuffer, (int)pBuffer->nFilledLen);

		if(!((voCOMXDataSource *)m_pParent)->IsBlockPortSettingsChangedEvent())
			SettingsChanging();

		// make sure 4 bytes alignment, East 20110922
		OMX_U8 * pTmpBuffer = (OMX_U8 *)voOMXMemAlloc (pBuffer->nFilledLen);
		if(!pTmpBuffer)
		{
			VOLOGE ("allocate temp buffer fail.");
			return OMX_ErrorInsufficientResources;
		}
		voOMXMemCopy(pTmpBuffer, pBuffer->pBuffer, pBuffer->nFilledLen);
		ReconfigPort((VO_LIVESRC_TRACK_INFOEX *)pTmpBuffer);
//		voOMXMemFree(pTmpBuffer);

		if(!((voCOMXDataSource *)m_pParent)->IsBlockPortSettingsChangedEvent())
		{
			voOMXMemFree(pTmpBuffer);
			return errType;
		}
		else
		{
			// transfer head data to decoder
			pBuffer->nFlags = OMX_BUFFERFLAG_CODECCONFIG;	// tell decoder this is head data
			pBuffer->pBuffer += (sizeof(VO_LIVESRC_TRACK_INFOEX) - 12);
			pBuffer->nFilledLen = ((VO_LIVESRC_TRACK_INFOEX *)pTmpBuffer)->HeadSize;
			errType = OMX_ErrorNone;
			voOMXMemFree(pTmpBuffer);
		}
	}

	if (errType != OMX_ErrorNone)
	{
		voOMXOS_Sleep (2);
		return OMX_ErrorUnderflow;
	}

	m_nFillBufferCount++;

	return errType;
}

OMX_ERRORTYPE voCOMXDataOutputPort::SetTrackInfo(VO_SOURCE_TRACKINFO* pti)
{
	 if(mTrackInfo.HeadData != NULL)
	 {
		 delete [] mTrackInfo.HeadData;
		 mTrackInfo.HeadData = NULL;
	 }

	 mTrackInfo.Codec		= pti->Codec;
	 if((int)pti->Duration != -2)	// invalid value, ignore it
		mTrackInfo.Duration	= pti->Duration;
	 mTrackInfo.Start		= pti->Start;
	 mTrackInfo.Type		= pti->Type;
	 mTrackInfo.HeadSize	= pti->HeadSize;
	 if(mTrackInfo.HeadSize > 0)
	 {
		 mTrackInfo.HeadData = new VO_BYTE[mTrackInfo.HeadSize];
		 if( mTrackInfo.HeadData != NULL)
		 {
			 voOMXMemCopy( mTrackInfo.HeadData , pti->HeadData , mTrackInfo.HeadSize);
		 }
		 else
		 {
			 mTrackInfo.HeadSize = 0;
			 return OMX_ErrorInsufficientResources;
		 }
	 }

	 return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXDataOutputPort::GetSeekKeyFrame(OMX_TICKS * pnTimeStamp)
{
	if(NULL == m_pLstBuffer)
		return OMX_ErrorIncorrectStateOperation;

	OMX_TICKS nTimeStamp = *pnTimeStamp;
	OMX_ERRORTYPE errType = OMX_ErrorOverflow;
	while(OMX_ErrorNone != errType && OMX_ErrorNoMore != errType)
	{
		nTimeStamp = *pnTimeStamp;
		errType = m_pLstBuffer->GetSeekKeyFrame(&nTimeStamp);

		if(OMX_ErrorOverflow == errType)
		{
			if(m_bBufferFull)
				errType = OMX_ErrorNone;
		}
		else if(OMX_ErrorNotReady == errType)
		{
			if(m_bBufferFull)
				errType = OMX_ErrorNoMore;
		}
	}

	if(OMX_ErrorNone == errType)
	{
		*pnTimeStamp = nTimeStamp;
		return OMX_ErrorNone;
	}

	return errType;
}
