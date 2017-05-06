	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompBaseChain.cpp

	Contains:	voCOMXCompBaseChain class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include <OMX_Core.h>
#include "voOMX_Index.h"
#include "voOMXOSFun.h"
#include "voCOMXCompBaseChain.h"
#include "voCOMXBaseGraph.h"
#include "voLiveSource.h"

#define LOG_TAG "voCOMXCompBaseChain"
#include "voLog.h"

voCOMXCompBaseChain::voCOMXCompBaseChain(voCOMXBaseGraph * pGraph, voCOMXCompList * pCompLisst,
										 voCOMXCfgCore * pCfgCore,  voCOMXCfgComp * pCfgComp)
	: m_pGraph (pGraph)
	, m_pCompList (pCompLisst)
	, m_pCfgCore (pCfgCore)
	, m_pCfgComp (pCfgComp)
	, m_nBoxs (0)
	, m_pSeekBox (NULL)
	, m_pClockBox (NULL)
	, m_pVideoDec (NULL)
	, m_pAudioDec (NULL)
	, m_pVideoSink (NULL)
	, m_pAudioSink (NULL)
	, m_pSource (NULL)
	, m_nSourceType (0)
	, m_bEndOfAudio (OMX_TRUE)
	, m_bEndOfVideo (OMX_TRUE)
	, m_bEndOfSource (OMX_FALSE)
	, m_sState (OMX_StateInvalid)
	, m_bFlushing (OMX_FALSE)
	, m_bSeeking (OMX_FALSE)
	, m_bSeeked (OMX_FALSE)
	, m_bFlushed (OMX_FALSE)
	, m_bSeekVideoRender (OMX_FALSE)
	, m_pReconfigPortHole (NULL)
	, m_pClientCB (NULL)
	, m_pDrmCB (NULL)
	, m_pLibOP (NULL)
	, m_pWorkPath (NULL)
	, mnCurPos(0)
	, m_nExitPos(0)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	m_CallBack.EventHandler = voOMXEventHandler;
	m_CallBack.EmptyBufferDone = voOMXEmptyBufferDone;
	m_CallBack.FillBufferDone = voOMXFillBufferDone;

	for (OMX_U32 i = 0; i < VOMAX_BOX_NUM; i++)
		m_aBox[i] = NULL;
}

voCOMXCompBaseChain::~voCOMXCompBaseChain()
{
	ReleaseBoxes ();
}

void voCOMXCompBaseChain::SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;
}

OMX_ERRORTYPE voCOMXCompBaseChain::ReleaseBoxes (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	Stop ();

	if (m_pSeekBox != NULL && m_pSeekBox->GetCompState () == OMX_StateLoaded)
	{
		VOLOGI ("Send Command to Source Comp to loaded status!");
		m_pSeekBox->GetComponent ()->SendCommand (m_pSeekBox->GetComponent (), OMX_CommandStateSet, OMX_StateLoaded, NULL);
	}

	OMX_ERRORTYPE errType = SendBoxCommand (VOBOX_CMD_StateLoaded);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("SendBoxCommand (VOBOX_CMD_StateLoaded) was failed. 0X%08X", errType);
	}

	errType = SendBoxCommand (VOBOX_CMD_FreeBuffer);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("SendBoxCommand (VOBOX_CMD_FreeBuffer) was failed. 0X%08X", errType);
	}

	errType = CheckBoxStatus (VOBOX_STATUS_Loaded);
	if (errType != OMX_ErrorNone)
	{
		VOLOGW ("CheckBoxStatus (VOBOX_STATUS_Loaded) was failed. 0X%08X", errType);
	}

	voOMXOS_Sleep (10);

//	for (OMX_S32 i = VOMAX_BOX_NUM - 1; i >= 0;  i--)
	for (OMX_S32 i = 0; i < VOMAX_BOX_NUM;  i++)
	{
		if (m_aBox[i] != NULL)
		{
			m_pCompList->FreeComponent (m_aBox[i]->GetComponent ());
			delete m_aBox[i];
		}
		m_aBox[i] = NULL;
	}

	m_pSeekBox = NULL;
	m_pClockBox = NULL;
	m_pVideoDec = NULL;
	m_pAudioDec = NULL;
	m_pVideoSink = NULL;
	m_pAudioSink = NULL;
	m_pSource = NULL;

	m_sState = OMX_StateInvalid;

	return OMX_ErrorNone;
}

voCOMXBaseBox * voCOMXCompBaseChain::AddSource (VOME_SOURCECONTENTTYPE * pSource)
{
	if (pSource == NULL)
		return NULL;

	if ( pSource->pLibOP != NULL)
		m_pLibOP = pSource->pLibOP;
	if (pSource->pDrmCB != NULL)
		m_pDrmCB = pSource->pDrmCB;

	OMX_STRING		pExt = NULL;
	
	m_bEndOfSource = OMX_FALSE;

	m_nSourceType = 0;

//	VOLOGI ("Source Type is %d ", pSource->nType);

	if (pSource->nType == VOME_SOURCE_File)
	{
		char szFile[1024];
		memset (szFile, 0, 1024);
#ifdef _WIN32
		WideCharToMultiByte (CP_ACP, 0, (LPWSTR) pSource->pSource, -1, szFile, 1024, NULL, NULL);
#elif defined(_LINUX)
	   strcpy(szFile, (OMX_STRING)pSource->pSource);
#endif // _WIN32

//		VOLOGI ("The URL is %s", szFile);

		if (!strncmp (szFile, ("RTSP://"), 6))
		{
			strcpy (szFile, "RTSP");
			pExt = szFile;
			m_nSourceType = 1;
		}
		else if (!strncmp (szFile, ("rtsp://"), 6))
		{
			strcpy (szFile, "RTSP");
			pExt = szFile;
			m_nSourceType = 1;
		}
		else if (!strncmp (szFile, ("MMS://"), 5))
		{
			strcpy (szFile, "RTSP");
			pExt = szFile;
			m_nSourceType = 1;
		}
		else if (!strncmp (szFile, ("mms://"), 5))
		{
			strcpy (szFile, "RTSP");
			pExt = szFile;
			m_nSourceType = 1;
		}
		else if (!strncmp (szFile, ("HTTP://"), 6))
		{
			strcpy (szFile, "HTTP");
			pExt = szFile;
			m_nSourceType = 2;
		}
		else if (!strncmp (szFile, ("http://"), 6))
		{
			strcpy (szFile, "HTTP");
			pExt = szFile;
			m_nSourceType = 2;
		}
		else
		{
			pExt = strstr (szFile, "://");
			if (pExt != NULL)
			{
				*pExt = 0;
				pExt = szFile;
			}
		}

		if (pExt == NULL)
		{
			pExt = strrchr (szFile, '.');
			if (pExt != NULL)
				pExt++;
		}

		char * pChar = pExt;
		if (pExt != NULL)
		{
			OMX_U32 nChars = strlen (pExt);
			for (OMX_U32 i = 0; i < nChars; i++)
			{
				if ((*pChar) <= 'z' && (*pChar) >= 'a')
					*pChar = *pChar - ('a' - 'A');
				pChar++;
			}
		}
	}

//	VOLOGI ("The Ext is %s", pExt);

	OMX_STRING pCompName = m_pCfgCore->GetSourceCompName (pExt);
	if (pCompName == NULL)
	{
		pCompName = m_pCfgCore->GetSourceCompName (NULL);
	}

	if (pCompName == NULL)
	{
		VOLOGE ("It could not find the source component.");
		return NULL;
	}

	voCOMXBaseBox * pSourceBox = AddBox (pCompName);
	if (pSourceBox == NULL)
	{
		VOLOGE ("It could not add the source component %s.", pCompName);
		return NULL;
	}
	m_pSeekBox = pSourceBox;

	pSourceBox->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexSourcePipe, pSource->pPipe);
	pSourceBox->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexSourceDrm, m_pDrmCB);
	pSourceBox->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexLibraryOperator, m_pLibOP);
	pSourceBox->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexSourceOffset, &pSource->nOffset);
	pSourceBox->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexSourceLength, &pSource->nLength);
	if ((VO_LIVESRC_FT_MAX != pSource->nReserved) && (VO_LIVESRC_FT_FILE != pSource->nReserved))
		pSourceBox->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexLiveStreamingType, &(pSource->nReserved));

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (pSource->nType == VOME_SOURCE_File)
	{
		if(pSource->pHeaders)
			pSourceBox->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexSourceHeaders, pSource->pHeaders);

#ifdef _WIN32
		OMX_U32 uSize = (wcslen ((wchar_t *)pSource->pSource) + 1) * 2;
#elif defined _LINUX
		OMX_U32 uSize = (strlen ((OMX_STRING)pSource->pSource) + 1);
#endif // _WIN32

		OMX_PARAM_CONTENTURITYPE * pContent = (OMX_PARAM_CONTENTURITYPE *)voOMXMemAlloc (uSize + 8);
		if (pContent == NULL)
			return NULL;

		pSourceBox->SetHeadder (pContent, uSize + 8);
		voOMXMemCopy (pContent->contentURI, pSource->pSource, uSize);
		errType = pSourceBox->SetParameter (OMX_IndexParamContentURI, pContent);
		voOMXMemFree (pContent);

		VOLOGI ("##########    File Name is %s  Result 0X%08X ", (char *)pSource->pSource, errType);
	}
	else if (pSource->nType == VOME_SOURCE_Handle)
	{
		errType = pSourceBox->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexSourceHandle, pSource->pSource);
	}
	else if (pSource->nType == VOME_SOURCE_ID)
	{
		errType = pSourceBox->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexSourceID, pSource->pSource);
	}

	if (errType != OMX_ErrorNone)
	{
		//RemoveBox (pSourceBox);
		VOLOGE ("It could not load the source component.");
		return NULL;
	}
	pSourceBox->CreateHoles ();

	OMX_TIME_CONFIG_TIMESTAMPTYPE tmInfo;
	pSourceBox->SetHeadder (&tmInfo, sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE));

	tmInfo.nPortIndex = 0;
	errType = pSourceBox->GetConfig ((OMX_INDEXTYPE)OMX_VO_IndexConfigTimeDuration, &tmInfo);
	if (errType == OMX_ErrorNone)
		m_pSeekBox = pSourceBox;

	m_pSource = pSourceBox;

	return pSourceBox;
}

voCOMXBaseBox *	voCOMXCompBaseChain::AddClock (void)
{
	if (m_pClockBox != NULL)
		return m_pClockBox;

	OMX_STRING pClockName = m_pCfgCore->GetClockTimeCompName (1);
	if (pClockName == NULL)
		return NULL;

	m_pClockBox = AddBox (pClockName);

	return m_pClockBox;
}

voCOMXBaseBox * voCOMXCompBaseChain::AddBox (OMX_STRING pCompName)
{
	voCOMXBaseBox * pNewBox = new voCOMXBaseBox (this);
	pNewBox->SetCallBack (m_pCallBack, m_pUserData);

	OMX_COMPONENTTYPE * pComp = m_pCompList->CreateComponent (pCompName, pNewBox, &m_CallBack);
	if (pComp == NULL)
	{
		delete pNewBox;
		VOLOGI ("The component %s could not be created!", pCompName);
		return NULL;
	}

	if (pNewBox->SetComponent (pComp, pCompName) != OMX_ErrorNone)
	{
		delete pNewBox;
		VOLOGE ("The component %s could not be wrapped by BOX!", pCompName);
		return NULL;
	}

	for (OMX_U32 i = 0; i < VOMAX_BOX_NUM;  i++)
	{
		if (m_aBox[i] == NULL)
		{
			m_aBox[i] = pNewBox;
			m_nBoxs++;
			break;
		}
	}

	// added by gtxia for temp
	const char* pTIComp = "OMX.TI.DUCATI1.VIDEO.DECODER" ;
	if(strncmp(pCompName, pTIComp, strlen(pTIComp)))
	{
		 // check it is clock time component or not.
		 OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE refClockType;
		 OMX_ERRORTYPE errType = pNewBox->GetConfig (OMX_IndexConfigTimeActiveRefClock, &refClockType);
		 if (errType == OMX_ErrorNone && m_pClockBox == NULL)
			 m_pClockBox = pNewBox;
	}

	if (m_pLibOP != NULL)
		pNewBox->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexLibraryOperator, m_pLibOP);

	if (m_pWorkPath != NULL)
		pNewBox->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexWorkPath, m_pWorkPath);

	return pNewBox;
}

voCOMXBaseBox *	voCOMXCompBaseChain::FindBox (OMX_COMPONENTTYPE * pComponent)
{
	for (OMX_U32 i = 0; i < VOMAX_BOX_NUM;  i++)
	{
		if (m_aBox[i] != NULL && m_aBox[i]->GetComponent () == pComponent)
		{
			return m_aBox[i];
		}
	}

	return NULL;
}

OMX_ERRORTYPE voCOMXCompBaseChain::RemoveBox (voCOMXBaseBox * pBox)
{
	if (pBox == NULL)
		return OMX_ErrorBadParameter;

	for (OMX_U32 i = 0; i < VOMAX_BOX_NUM;  i++)
	{
		if (m_aBox[i] == pBox)
		{
			voCOMXBaseHole * pConnectHole = NULL;
			for (OMX_U32 j = 0; j < pBox->GetHoleCount (); j++)
			{
				pConnectHole = pBox->GetHole (j)->GetConnected ();
				if (pConnectHole != NULL)
					pConnectHole->SetConnected (NULL, pConnectHole->IsTunnel());
			}

			if (m_pSeekBox == pBox)
				m_pSeekBox = NULL;
			if (m_pClockBox == pBox)
				m_pClockBox = NULL;
			if (m_pVideoDec == pBox)
				m_pVideoDec = NULL;
			if (m_pAudioDec == pBox)
				m_pAudioDec = NULL;
			if (m_pVideoSink == pBox)
				m_pVideoSink = NULL;
			if (m_pAudioSink == pBox)
				m_pAudioSink = NULL;
			if (m_pSource == pBox)
				m_pSource = NULL;

			m_pCompList->FreeComponent (m_aBox[i]->GetComponent ());
			delete m_aBox[i];
			m_aBox[i] = NULL;

			m_nBoxs--;

			break;
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::RemoveComp (OMX_COMPONENTTYPE * pComp)
{
	if (pComp == NULL)
		return OMX_ErrorBadParameter;

	OMX_BOOL bRemoved = OMX_FALSE;
	OMX_U32  i = 0;

	for (i = 0; i < VOMAX_BOX_NUM;  i++)
	{
		if (m_aBox[i] != NULL && m_aBox[i]->GetComponent () == pComp)
		{
			bRemoved = OMX_TRUE;
			break;
		}
	}

	if (bRemoved)
		return RemoveBox (m_aBox[i]);
	else
		return OMX_ErrorInvalidComponent;
}

OMX_ERRORTYPE voCOMXCompBaseChain::ConnectPorts (OMX_COMPONENTTYPE * pOutputComp, OMX_U32 nOutputPort,
												 OMX_COMPONENTTYPE * pInputComp, OMX_U32 nInputPort, OMX_BOOL bTunnel)
{
	if (pOutputComp == NULL || pInputComp == NULL)
		return OMX_ErrorBadParameter;

	voCOMXBaseBox * pOutputBox = FindBox (pOutputComp);
	if (pOutputBox == NULL)
	{
		VOLOGE ("pOutputBox == NULL");
		return OMX_ErrorComponentNotFound;
	}

	voCOMXBaseHole * pOutputHole = pOutputBox->GetHole (nOutputPort);
	if (pOutputHole == NULL)
	{
		VOLOGE ("pOutputHole == NULL");
		return OMX_ErrorBadPortIndex;
	}

	voCOMXBaseBox * pInputBox = FindBox (pInputComp);
	if (pInputBox == NULL)
	{
		VOLOGE ("pInputBox == NULL");
		return OMX_ErrorComponentNotFound;
	}

	voCOMXBaseHole * pInputHole = pInputBox->GetHole (nInputPort);
	if (pInputHole == NULL)
	{
		VOLOGE ("pInputHole == NULL");
		return OMX_ErrorBadPortIndex;
	}

	return Connect (pOutputHole, pInputHole, bTunnel);
}


OMX_ERRORTYPE voCOMXCompBaseChain::Connect (voCOMXBaseHole * pOutput, voCOMXBaseHole * pInput, OMX_BOOL bTunnel)
{
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (pOutput == NULL || pInput == NULL)
	{
		VOLOGE ("pOutput == NULL || pInput == NULL");
		return OMX_ErrorBadParameter;
	}

	errType = pOutput->ConnectedTo (pInput, bTunnel);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("pOutput->ConnectedTo was failed! 0X%08X", errType);
		return errType;
	}

	pInput->SetConnected (pOutput, pOutput->IsTunnel ());

	return errType;
}

OMX_ERRORTYPE voCOMXCompBaseChain::Run (OMX_BOOL bStartBuffer)
{
	voCOMXAutoLock lock (&m_mutStatus);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_sState == OMX_StateInvalid || m_sState == OMX_StateIdle || m_bSeeked || m_bFlushed)
		bStartBuffer = OMX_TRUE;
	m_bSeeked = OMX_FALSE;
	m_bFlushed = OMX_FALSE;

	if (m_sState == OMX_StateInvalid)
	{
		errType = SendBoxCommand (VOBOX_CMD_DisablePort);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("SendBoxCommand (VOBOX_CMD_DisablePort) was failed! 0X%08X", errType);
			return errType;
		}
		errType = CheckPortsStatus (PORT_TRANS_DONE);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("CheckPortsStatus (PORT_TRANS_DONE) was failed! 0X%08X", errType);
			return errType;
		}

		errType = SendBoxCommand (VOBOX_CMD_StateIdle);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("SendBoxCommand (VOBOX_CMD_StateIdle) was failed! 0X%08X", errType);
			return errType;
		}

		errType = SendBoxCommand (VOBOX_CMD_AllocBuffer);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("SendBoxCommand (VOBOX_CMD_AllocBuffer) was failed! 0X%08X", errType);
			return errType;
		}

		errType = CheckBoxStatus (VOBOX_STATUS_Idle);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("CheckBoxStatus (VOBOX_STATUS_Idle) was failed! 0X%08X", errType);
			if(m_pCallBack)
				m_pCallBack(m_pUserData, VOME_CID_PLAY_ERROR, 0, 0);
			return errType;
		}

		m_sState = OMX_StateIdle;
	}

	if (m_sState == OMX_StateIdle || m_sState == OMX_StatePause)
	{
		errType = SendBoxCommand (VOBOX_CMD_StateExecute);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("SendBoxCommand (VOBOX_CMD_StateExecute) was failed! 0X%08X", errType);
			return errType;
		}

		errType = CheckBoxStatus (VOBOX_STATUS_Executing);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("CheckBoxStatus (VOBOX_STATUS_Executing) was failed! 0X%08X", errType);
			if(m_pCallBack)
				m_pCallBack(m_pUserData, VOME_CID_PLAY_ERROR, 0, 0);
			return errType;
		}

		m_sState = OMX_StateExecuting;
	}

	if (bStartBuffer)
	{
		errType = SendBoxCommand (VOBOX_CMD_StartBuffer);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("SendBoxCommand (VOBOX_CMD_StartBuffer) was failed! 0X%08X", errType);
			return errType;
		}
	}

	if ((m_pAudioSink != NULL && m_bEndOfAudio) && m_pClockBox != NULL)
		m_pClockBox->SetConfig ((OMX_INDEXTYPE)OMX_VO_IndexStartWallClock, (OMX_PTR)1);

	m_bEndOfSource = OMX_FALSE;

	//wait for OMX_EventPortSettingsChanged
	CheckBoxPortSettingChanged ();

	return errType;
}

OMX_ERRORTYPE voCOMXCompBaseChain::Pause (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	OMX_BOOL		bStartBuffer = OMX_FALSE;

	if (m_sState == OMX_StateInvalid || m_sState == OMX_StateIdle)
		bStartBuffer = OMX_TRUE;

	errType = SendBoxCommand (VOBOX_CMD_StatePause);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("SendBoxCommand (VOBOX_CMD_StatePause) was failed! 0X%08X", errType);
		return errType;
	}

	errType = CheckBoxStatus (VOBOX_STATUS_Pause);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("CheckBoxStatus (VOBOX_STATUS_Pause) was failed! 0X%08X", errType);
		if(m_pCallBack)
			m_pCallBack(m_pUserData, VOME_CID_PLAY_ERROR, 0, 0);
		return errType;
	}

	if (m_pClockBox != NULL)
	{
		if ((m_pAudioSink != NULL && m_bEndOfAudio))
		{
			OMX_TIME_CONFIG_TIMESTAMPTYPE	MediaTime;
			errType = m_pClockBox->GetConfig ( OMX_IndexConfigTimeCurrentMediaTime, &MediaTime);
			if (errType == OMX_ErrorNone)
			{
				errType = m_pClockBox->SetConfig ( OMX_IndexConfigTimeCurrentAudioReference, &MediaTime);
			}
		}
		m_pClockBox->SetConfig ((OMX_INDEXTYPE)OMX_VO_IndexStartWallClock, (OMX_PTR)0);
	}


	m_sState = OMX_StatePause;

	if (bStartBuffer)
	{
		errType = SendBoxCommand (VOBOX_CMD_StartBuffer);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("SendBoxCommand (VOBOX_CMD_StartBuffer) was failed! 0X%08X", errType);
			return errType;
		}
	}

	return errType;
}

OMX_ERRORTYPE voCOMXCompBaseChain::Stop (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_sState != OMX_StatePause && m_sState != OMX_StateExecuting)
		return OMX_ErrorNone;

	errType = SendBoxCommand (VOBOX_CMD_StateIdle);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("SendBoxCommand (VOBOX_CMD_StateIdle) was failed! 0X%08X", errType);
		return errType;
	}

	errType = CheckBoxStatus (VOBOX_STATUS_Idle);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("CheckBoxStatus (VOBOX_STATUS_Idle) was failed! 0X%08X", errType);
		if(m_pCallBack)
			m_pCallBack(m_pUserData, VOME_CID_PLAY_ERROR, 0, 0);
		return errType;
	}

	m_sState = OMX_StateIdle;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::Flush (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	m_bFlushing = OMX_TRUE;

	errType = SendBoxCommand (VOBOX_CMD_Flush);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("SendBoxCommand (VOBOX_CMD_Flush) was failed! 0X%08X", errType);
		return errType;
	}

	errType = CheckBoxStatus (VOBOX_STATUS_Flushing);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("CheckBoxStatus (VOBOX_STATUS_Flushing) was failed! 0X%08X", errType);
		if(m_pCallBack)
			m_pCallBack(m_pUserData, VOME_CID_PLAY_ERROR, 0, 0);
		return errType;
	}

	m_bFlushed = OMX_TRUE;
	m_bFlushing = OMX_FALSE;

	return OMX_ErrorNone;
}

OMX_S32 voCOMXCompBaseChain::GetDuration (void)
{
	if (m_pSeekBox != NULL)
		return m_pSeekBox->GetDuration ();

	return 0;
}

OMX_S32 voCOMXCompBaseChain::GetPos (void)
{
//	voCOMXAutoLock lock (&m_mutStatus);

	if (m_bEndOfSource)
	{
		//if (m_pSeekBox != NULL)
		//	return m_pSeekBox->GetDuration ();

		return m_nExitPos;
	}

	if (m_sState != OMX_StatePause && m_sState != OMX_StateExecuting)
		return mnCurPos;
	
	OMX_S32 nAudioPos = 0, nVideoPos = 0;
	if (m_pAudioSink != NULL)
		nAudioPos = m_pAudioSink->GetPos ();
	if (m_pVideoSink != NULL)
		nVideoPos = m_pVideoSink->GetPos ();

	OMX_S32 nPos = (nAudioPos > nVideoPos) ? nAudioPos : nVideoPos;

// 	if (nPos == 0 && m_pSeekBox != NULL)
// 		nPos = m_pSeekBox->GetPos ();

	if (0 == nPos &&  m_sState == OMX_StateExecuting)
		return  mnCurPos;

	mnCurPos = nPos;

	return nPos;
}

OMX_S32 voCOMXCompBaseChain::SetPos (OMX_S32 nPos)
{
	voCOMXAutoLock lock (&m_mutStatus);

	if (m_pSeekBox == NULL)
	{
		VOLOGE ("It was not correct status.");
		return -1;
	}

	if (m_sState != OMX_StatePause && m_sState != OMX_StateExecuting)
	{
		mnCurPos = nPos;
		return m_pSeekBox->SetPos (nPos);
	}

	if (m_nSourceType == 0 && nPos > 0 && m_sState != OMX_StatePause)
	{
		//Try 1 second to wait
		OMX_S32 nAudioPos = 2000;
		OMX_S32 nVideoPos = 2000;
		VO_U32 dwTryCount = 0;
		while(dwTryCount < 200)
		{
			nAudioPos = 2000;
			nVideoPos = 2000;
			if (m_pAudioSink != NULL && !m_bEndOfAudio)
				nAudioPos = m_pAudioSink->GetPos ();
			if (m_pVideoSink != NULL && !m_bEndOfVideo)
				nVideoPos = m_pVideoSink->GetPos ();
			if (nAudioPos >= 100 && nVideoPos >= 100)
				break;

			dwTryCount++;
			voOMXOS_Sleep(5);
		}

		if (nAudioPos < 100 || nVideoPos < 100)
			return 0;
	}

	OMX_STATETYPE	state = m_sState;
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	m_bSeeking = OMX_TRUE;

	if (m_sState == OMX_StateExecuting)
	{
		errType = Pause ();
		if (errType != OMX_ErrorNone)
		{
			m_bSeeking = OMX_FALSE;
			return -1;
		}
	}

	errType = Flush ();
	if (errType != OMX_ErrorNone)
	{
		m_bSeeking = OMX_FALSE;
		return -1;
	}

	OMX_S32 nNewPos = m_pSeekBox->SetPos (nPos);
	if (m_pAudioSink != NULL)
		m_pAudioSink->SetPos (nNewPos);
	if (m_pVideoSink != NULL)
		m_pVideoSink->SetPos (nNewPos);

	mnCurPos = nNewPos;

	m_bSeeking = OMX_FALSE;

	if (state == OMX_StateExecuting)
	{
		Run (OMX_TRUE);
	}
	else if (state == OMX_StatePause)
	{
		m_bSeeked = OMX_TRUE;

		if (m_pVideoSink != NULL)
		{
			OMX_BOOL	bPause = OMX_TRUE;

			for (OMX_S32 i = 0; i < VOMAX_BOX_NUM;  i++)
			{
				if (m_aBox[i] != NULL)
					m_aBox[i]->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexSetPosInPause, &bPause);
			}
			
			m_bSeekVideoRender = OMX_FALSE;
			Run ();
			
			// Some file has little key frame, so 1 second is not enough, add to 5 seconds
			// East 2010/08/19
			if(m_nSourceType == 0)//modified by Eric, for streaming, 5s will cause ANR
			{
				int nTryTimes = 0;
				while (nTryTimes < 5000)
				{
					if (m_bSeekVideoRender)
						break;
					nTryTimes++;
					voOMXOS_Sleep (1);
				}
			}			

			Pause ();
			bPause = OMX_FALSE;
			for (OMX_S32 i = 0; i < VOMAX_BOX_NUM;  i++)
			{
				if (m_aBox[i] != NULL)
					m_aBox[i]->SetParameter ((OMX_INDEXTYPE)OMX_VO_IndexSetPosInPause, &bPause);
			}
		}
	}

	m_bEndOfSource = OMX_FALSE;

	return nNewPos;
}

OMX_ERRORTYPE voCOMXCompBaseChain::FlushAll (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	if (m_sState != OMX_StatePause && m_sState != OMX_StateExecuting)
		return OMX_ErrorInvalidState;

	OMX_STATETYPE	state = m_sState;
	OMX_ERRORTYPE	errType = OMX_ErrorNone;

	if (m_sState == OMX_StateExecuting)
	{
		errType = Pause ();
		if (errType != OMX_ErrorNone)
			return OMX_ErrorInvalidState;
	}

	errType = Flush ();
	if (errType != OMX_ErrorNone)
			return OMX_ErrorInvalidState;

	if (state == OMX_StateExecuting)
	{
		Run (OMX_TRUE);
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::ReconfigPort(voCOMXBaseHole* pHole, OMX_BOOL bUseThread)
{
	if(bUseThread == OMX_FALSE)
		return ReconfigPortB(pHole);

	m_pReconfigPortHole = pHole;
	voOMXThreadHandle hThread;
	OMX_U32 nThreadID;
	voOMXThreadCreate(&hThread, &nThreadID, (voOMX_ThreadProc)ReconfigPortProc, this, 0);

	return OMX_ErrorNone;
}

OMX_U32 voCOMXCompBaseChain::ReconfigPortProc(OMX_PTR pParam)
{
	voCOMXCompBaseChain* pChain = (voCOMXCompBaseChain*)pParam;
	
	return pChain->ReconfigPortProcB();
}

OMX_U32 voCOMXCompBaseChain::ReconfigPortProcB()
{
	if(m_pReconfigPortHole)
	{
		ReconfigPortB(m_pReconfigPortHole);

		m_pReconfigPortHole = NULL;
	}

	return 0;
}

OMX_ERRORTYPE voCOMXCompBaseChain::ReconfigPortB(voCOMXBaseHole* pHole)
{
//	voCOMXAutoLock lock(&m_mutStatus);

	if(pHole == NULL)
	{
		VOLOGE("The Hole is NULL!");
		return OMX_ErrorInvalidState;
	}

	voCOMXBaseHole* pConnectedHole = pHole->GetConnected();
	if(pConnectedHole == NULL)
	{
		VOLOGE ("The Connected Hole is NULL!");
		return OMX_ErrorInvalidState;
	}

	OMX_BOOL bTunnel = pHole->IsTunnel();

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	errType = pHole->Disable();
	errType = pConnectedHole->Disable();
	OMX_U32 nTryTimes = 0;

	while(nTryTimes < 1000)
	{
		if(!pHole->IsEnable() && !pConnectedHole->IsEnable())
			break;
		voOMXOS_Sleep(2);
		nTryTimes++;
	}
	if(nTryTimes >= 1000)
		VOLOGW("It was time out when disable the port!");

	errType = pHole->ConnectedTo(pConnectedHole, bTunnel);
	if(errType != OMX_ErrorNone)
	{
		VOLOGE ("The result is 0X%08X to connect other hole!", errType);

		pHole->GetParentBox()->SetPortSettingsChanging(OMX_FALSE);
		return errType;
	}
	pConnectedHole->SetConnected(pHole, pHole->IsTunnel());
	errType = pHole->Enable();
	errType = pConnectedHole->Enable();
	nTryTimes = 0;
	while(nTryTimes < 1000)
	{
		if(pHole->IsEnable() && pConnectedHole->IsEnable())
			break;

		voOMXOS_Sleep(2);
		nTryTimes++;
	}
	if(nTryTimes >= 1000)
		VOLOGW("It was time out when enable the port!");
	errType = pHole->StartBuffer();
	errType = pConnectedHole->StartBuffer();
	pHole->GetParentBox()->SetPortSettingsChanged(OMX_TRUE);
	pConnectedHole->GetParentBox()->SetPortSettingsChanged(OMX_TRUE);

	pHole->GetParentBox()->SetPortSettingsChanging(OMX_FALSE);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::DisablePort(voCOMXBaseHole * pHole)
{
	if(NULL == pHole)
	{
		VOLOGE("The Hole is NULL!");
		return OMX_ErrorInvalidState;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	errType = pHole->Disable();

	OMX_U32 nTryTimes = 0;
	while(nTryTimes < 1000)
	{
		if(!pHole->IsEnable())
			break;

		voOMXOS_Sleep(2);
		nTryTimes++;
	}

	if(nTryTimes >= 1000)
	{
		VOLOGW("It was time out when disable the port!");
		return OMX_ErrorTimeout;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::EnablePort(voCOMXBaseHole * pHole)
{
	if(NULL == pHole)
	{
		VOLOGE("The Hole is NULL!");
		return OMX_ErrorInvalidState;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	errType = pHole->Enable();

	OMX_U32 nTryTimes = 0;
	while(nTryTimes < 1000)
	{
		if(pHole->IsEnable())
			break;

		voOMXOS_Sleep(2);
		nTryTimes++;
	}

	if(nTryTimes >= 1000)
	{
		VOLOGW("It was time out when enable the port!");
		return OMX_ErrorTimeout;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::ShutdownVideoDecoder(OMX_STATETYPE eStateBeforeCS)
{
	if(OMX_StateIdle != eStateBeforeCS && OMX_StateInvalid != eStateBeforeCS)
	{
		m_pSource->SetPortSettingsChanging(OMX_TRUE);
		m_pVideoDec->SetPortSettingsChanging(OMX_TRUE);
		m_pVideoSink->SetPortSettingsChanging(OMX_TRUE);
	}

	if(OMX_StateInvalid != eStateBeforeCS)
	{
		OMX_U32 nHole = m_pVideoDec->GetHoleCount();
		for(OMX_U32 i = 0; i < nHole; i++)
		{
			voCOMXBaseHole * pHole = m_pVideoDec->GetHole(i);
			DisablePort(pHole->GetConnected());
		}
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	VO_U32 nTryTimes = 0;

	if(OMX_StateIdle != eStateBeforeCS && OMX_StateInvalid != eStateBeforeCS)
	{
		errType = m_pVideoDec->SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to set OMX_StateIdle 0x%08X", errType);
			return errType;
		}

		// wait for Idle state
		nTryTimes = 0;
		while(OMX_StateIdle != m_pVideoDec->GetCompState())
		{
			voOMXOS_Sleep(2);
			if(++nTryTimes > 2500)
			{
				VOLOGE("OMX_ErrorTimeout");
				return OMX_ErrorTimeout;
			}
		}
	}

	if(OMX_StateInvalid != eStateBeforeCS)
	{
		errType = m_pVideoDec->SendCommand(OMX_CommandStateSet, OMX_StateLoaded, NULL);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to set OMX_StateLoaded 0x%08X", errType);
			return errType;
		}

		errType = m_pVideoDec->FreeBuffer();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to set FreeBuffer 0x%08X", errType);
			return errType;
		}

		// wait for Loaded state
		nTryTimes = 0;
		while(OMX_StateLoaded != m_pVideoDec->GetCompState())
		{
			voOMXOS_Sleep(2);
			if(++nTryTimes > 2500)
			{
				VOLOGE("OMX_ErrorTimeout");
				return OMX_ErrorTimeout;
			}
		}
	}

	RemoveBox(m_pVideoDec);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::RecreateVideoDecoder(OMX_STATETYPE eStateBeforeCS)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_U32 nHole = m_pSource->GetHoleCount();
	for(OMX_U32 i = 0; i < nHole; i++)
	{
		voCOMXBaseHole * pHole = m_pSource->GetHole(i);
		OMX_PARAM_PORTDEFINITIONTYPE * pPortDefinition = pHole->GetPortType();
		if(OMX_PortDomainVideo == pPortDefinition->eDomain && OMX_DirOutput == pPortDefinition->eDir)
		{
			voCOMXBaseBox * pVideoDec = NULL;
			OMX_U32 nTryTimes = 0;
			OMX_BOOL bExit = OMX_FALSE;
			while(nTryTimes < 5)
			{
				pVideoDec = m_pGraph->TryConnectVideoDec(pHole, nTryTimes, bExit);
				if(pVideoDec || bExit)
					break;

				nTryTimes++;
			}

			if(NULL == pVideoDec)
			{
				VOLOGE ("failed to connect the video decoder component!");
				return OMX_ErrorComponentNotFound;
			}

			m_pVideoDec = pVideoDec;

			m_pVideoDec->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexSourceDrm, m_pDrmCB);

			voCOMXBaseHole * pOutputHole = m_pVideoDec->GetHole(1);
			for (OMX_U32 j = 0; j < m_pVideoSink->GetHoleCount(); j++)
			{
				voCOMXBaseHole* pInputHole = m_pVideoSink->GetHole(j);
				if(OMX_PortDomainOther != pInputHole->GetPortType()->eDomain)
				{
					errType = Connect(pOutputHole, pInputHole, m_pGraph->m_bTunnelMode);
					if(OMX_ErrorNone != errType)
					{
						VOLOGE("failed to connect the video decoder with render component! 0x%08X", errType);
						return errType;
					}
				}
			}
		}
	}

	if(OMX_StateInvalid == eStateBeforeCS)
		return OMX_ErrorNone;

	errType = m_pVideoDec->SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_StateIdle 0x%08X", errType);
		return errType;
	}

	errType = m_pVideoDec->AllocBuffer();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set AllocBuffer 0x%08X", errType);
		return errType;
	}

	// wait for Idle state
	VO_U32 nTryTimes = 0;
	while(OMX_StateIdle != m_pVideoDec->GetCompState())
	{
		voOMXOS_Sleep(2);
		if(++nTryTimes > 2500)
		{
			VOLOGE("OMX_ErrorTimeout");
			return OMX_ErrorTimeout;
		}
	}

	if(OMX_StateIdle != eStateBeforeCS)
	{
		m_pVideoSink->SetPortSettingsChanging(OMX_FALSE);
		m_pVideoDec->SetPortSettingsChanging(OMX_FALSE);
		m_pSource->SetPortSettingsChanging(OMX_FALSE);
	}

	nHole = m_pVideoDec->GetHoleCount();
	for(OMX_U32 i = 0; i < nHole; i++)
	{
		voCOMXBaseHole * pHole = m_pVideoDec->GetHole(i);
		voCOMXBaseHole * pConnectHole = pHole->GetConnected();
		if(pConnectHole)
		{
			EnablePort(pConnectHole);

			// force source component send configure data
			if(pConnectHole->GetParentBox() == m_pSource)
			{
				OMX_VO_OUTPUTCONFIGDATATYPE sOutputConfigData;
				m_pSource->SetHeadder(&sOutputConfigData, sizeof(sOutputConfigData));
				sOutputConfigData.nPortIndex = pConnectHole->GetPortIndex();
				sOutputConfigData.bOutputConfigData = OMX_TRUE;
				errType = m_pSource->SetConfig((OMX_INDEXTYPE)OMX_VO_IndexOutputConfigData, &sOutputConfigData);
				VOLOGI("OMX_VO_IndexOutputConfigData 0x%08X", errType);
				if(OMX_ErrorNone != errType)
				{
					VOLOGW("failed to SetConfig OMX_VO_IndexOutputConfigData! 0x%08X", errType);
				}
			}
		}
	}

	if(OMX_StateIdle != eStateBeforeCS)
	{
		errType = m_pVideoDec->SendCommand(OMX_CommandStateSet, OMX_StatePause, NULL);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to set OMX_StatePause 0x%08X", errType);
			return errType;
		}

		// wait for Pause state
		nTryTimes = 0;
		while(OMX_StatePause != m_pVideoDec->GetCompState())
		{
			voOMXOS_Sleep(2);
			if(++nTryTimes > 2500)
			{
				VOLOGE("OMX_ErrorTimeout");
				return OMX_ErrorTimeout;
			}
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::ChangeSurface(VOMEUpdateSurfaceCallBack fCallBack, OMX_PTR pUserData)
{
	voCOMXAutoLock lock(&m_mutStatus);

	if(NULL == m_pVideoDec || !strncmp(m_pVideoDec->GetComponentName(), "OMX.VisualOn.", 13))	// no/SW decoder
	{
		fCallBack(pUserData);
		return OMX_ErrorNone;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if(OMX_StateInvalid == m_sState)
	{
		errType = ShutdownVideoDecoder(m_sState);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to ShutdownVideoDecoder 0x%08X", errType);
			fCallBack(pUserData);
			return errType;
		}

		fCallBack(pUserData);

		errType = RecreateVideoDecoder(m_sState);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to RecreateVideoDecoder 0x%08X", errType);
			return errType;
		}

		return errType;
	}

	if(OMX_StatePause != m_sState && OMX_StateExecuting != m_sState && OMX_StateIdle != m_sState)
	{
		VOLOGE("failed to ChangeSurface, current state %d", (int)m_sState);
		fCallBack(pUserData);
		return OMX_ErrorInvalidState;
	}

	OMX_STATETYPE eStateBeforeCS = m_sState;
	if(OMX_StateExecuting == eStateBeforeCS)
	{
		errType = Pause();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to Pause 0x%08X", errType);
			fCallBack(pUserData);
			return errType;
		}
	}

	OMX_S32 nPosBeforeCS = -1;
	if(OMX_StateIdle != eStateBeforeCS)
		nPosBeforeCS = GetPos();
	VOLOGI("play position [%d] before change surface", nPosBeforeCS);

	errType = ShutdownVideoDecoder(eStateBeforeCS);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to ShutdownVideoDecoder 0x%08X", errType);
		fCallBack(pUserData);
		return errType;
	}

	fCallBack(pUserData);

	// create new Hardware decoder
	errType = RecreateVideoDecoder(eStateBeforeCS);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to RecreateVideoDecoder 0x%08X", errType);
		return errType;
	}

	// set previous position (accurate mode)
	if(-1 != nPosBeforeCS)
		SetPos(nPosBeforeCS);

	// restore state
	if(OMX_StateExecuting == eStateBeforeCS)
	{
		errType = Run();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to Run 0x%08X", errType);
			return errType;
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::ShutdownAudioDecoder(OMX_STATETYPE eStateBeforeST)
{
	if(OMX_StateIdle != eStateBeforeST && OMX_StateInvalid != eStateBeforeST)
	{
		m_pSource->SetPortSettingsChanging(OMX_TRUE);
		m_pAudioDec->SetPortSettingsChanging(OMX_TRUE);
		m_pAudioSink->SetPortSettingsChanging(OMX_TRUE);
	}
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	VO_U32 nTryTimes = 0;
	if(OMX_StateIdle != eStateBeforeST && OMX_StateInvalid != eStateBeforeST)
	{
		errType = m_pAudioSink->Flush();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to flush audiosink when shutdown audiodec 0x%08x",errType);
			return errType;
		}
		errType = m_pAudioDec->Flush();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to flush audiodecoder when shutdown audiodec 0x%08x",errType);
			return errType;
		}
		errType = m_pAudioDec->SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to set OMX_StateIdle 0x%08X", errType);
			return errType;
		}

		// wait for Idle state
		nTryTimes = 0;
		while(OMX_StateIdle != m_pAudioDec->GetCompState())
		{	
			voOMXOS_Sleep(2);
			if(++nTryTimes > 2500)
			{
				VOLOGE("OMX_ErrorTimeout");
				return OMX_ErrorTimeout;
			}
		}
	}
	
	if(OMX_StateInvalid != eStateBeforeST)
	{
		errType = m_pAudioDec->SendCommand(OMX_CommandStateSet, OMX_StateLoaded, NULL);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to set OMX_StateLoaded 0x%08X", errType);
			return errType;
		}

		if(OMX_StateInvalid != eStateBeforeST)
		{
			OMX_U32 nHole = m_pAudioDec->GetHoleCount();
			for(OMX_U32 i = 0; i < nHole; i++)
			{
				voCOMXBaseHole * pHole = m_pAudioDec->GetHole(i);
				DisablePort(pHole->GetConnected());
			}
		}
		
		errType = m_pAudioDec->FreeBuffer();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to set FreeBuffer 0x%08X", errType);
			return errType;
		}

		// wait for Loaded state
		nTryTimes = 0;
		while(OMX_StateLoaded != m_pAudioDec->GetCompState())
		{
			voOMXOS_Sleep(2);
			if(++nTryTimes > 2500)
			{
				VOLOGE("OMX_ErrorTimeout");
				return OMX_ErrorTimeout;
			}
		}
	}

	RemoveBox(m_pAudioDec);
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::RecreateAudioDecoder(OMX_STATETYPE eStateBeforeST)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_U32 nHole = m_pSource->GetHoleCount();
	for(OMX_U32 i = 0; i < nHole; i++)
	{
		voCOMXBaseHole * pHole = m_pSource->GetHole(i);
		OMX_PARAM_PORTDEFINITIONTYPE * pPortDefinition = pHole->GetPortType();
		if(OMX_PortDomainAudio == pPortDefinition->eDomain && OMX_DirOutput == pPortDefinition->eDir)
		{
			voCOMXBaseBox * pAudioDec = NULL;
			OMX_U32 nTryTimes = 0;
			OMX_BOOL bExit = OMX_FALSE;
			while(nTryTimes < 5)
			{
				pAudioDec = m_pGraph->TryConnectAudioDec(pHole, nTryTimes, bExit);
				if(pAudioDec || bExit)
					break;

				nTryTimes++;
			}

			if(NULL == pAudioDec)
			{
				VOLOGE ("failed to connect the Audio decoder component!");
				return OMX_ErrorComponentNotFound;
			}

			m_pAudioDec = pAudioDec;

		//	m_pAudioDec->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexSourceDrm, m_pDrmCB);
			
			voCOMXBaseHole * pOutputHole = m_pAudioDec->GetHole(1);
			for (OMX_U32 j = 0; j < m_pAudioSink->GetHoleCount(); j++)
			{
				voCOMXBaseHole* pInputHole = m_pAudioSink->GetHole(j);
				if(OMX_PortDomainAudio == pInputHole->GetPortType()->eDomain)
				{
					errType = Connect(pOutputHole, pInputHole, m_pGraph->m_bTunnelMode);
					if(OMX_ErrorNone != errType)
					{
						VOLOGE("failed to connect the Audio decoder with render component! 0x%08X", errType);
						return errType;
					}
				}
			}
		}
	}
	
	if(OMX_StateInvalid == eStateBeforeST)
		return OMX_ErrorNone;

	errType = m_pAudioDec->SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_StateIdle 0x%08X", errType);
		return errType;
	}

	if(OMX_StateInvalid != eStateBeforeST)
	{
		nHole = m_pAudioDec->GetHoleCount();
		for(OMX_U32 i = 0; i < nHole; i++)
		{
			voCOMXBaseHole * pHole = m_pAudioDec->GetHole(i);
			EnablePort(pHole->GetConnected());
		}
	}
	
	errType = m_pAudioDec->AllocBuffer();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set AllocBuffer 0x%08X", errType);
		return errType;
	}
	errType = m_pAudioDec->Flush();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to flush audiodecoder when recreate audiodec 0x%08x",errType);
		return errType;
	}
	// wait for Idle state
	VO_U32 nTryTimes = 0;
	while(OMX_StateIdle != m_pAudioDec->GetCompState())
	{
		VOLOGI("Trans is %d",m_pAudioDec->GetTransState());
		voOMXOS_Sleep(2);
		if(++nTryTimes > 2500)
		{
			VOLOGE("OMX_ErrorTimeout");
			return OMX_ErrorTimeout;
		}
	}

	if(OMX_StateIdle != eStateBeforeST)
	{
		m_pAudioSink->SetPortSettingsChanging(OMX_FALSE);
		m_pAudioDec->SetPortSettingsChanging(OMX_FALSE);
		m_pSource->SetPortSettingsChanging(OMX_FALSE);
	}

	nHole = m_pAudioDec->GetHoleCount();
	for(OMX_U32 i = 0; i < nHole; i++)
	{
		voCOMXBaseHole * pHole = m_pAudioDec->GetHole(i);
		voCOMXBaseHole * pConnectHole = pHole->GetConnected();
		if(pConnectHole)
		{
		//	EnablePort(pConnectHole);

			// force source component send configure data
			if(pConnectHole->GetParentBox() == m_pSource)
			{
				OMX_VO_OUTPUTCONFIGDATATYPE sOutputConfigData;
				m_pSource->SetHeadder(&sOutputConfigData, sizeof(sOutputConfigData));
				sOutputConfigData.nPortIndex = pConnectHole->GetPortIndex();
				sOutputConfigData.bOutputConfigData = OMX_TRUE;
				errType = m_pSource->SetConfig((OMX_INDEXTYPE)OMX_VO_IndexOutputConfigData, &sOutputConfigData);
				VOLOGI("OMX_VO_IndexOutputConfigData 0x%08X", errType);
				if(OMX_ErrorNone != errType)
				{
					VOLOGW("failed to SetConfig OMX_VO_IndexOutputConfigData! 0x%08X", errType);
				}
			}
		}
	}

	if(OMX_StateIdle != eStateBeforeST)
	{
		errType = m_pAudioDec->SendCommand(OMX_CommandStateSet, OMX_StatePause, NULL);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to set OMX_StatePause 0x%08X", errType);
			return errType;
		}

		// wait for Pause state
		nTryTimes = 0;
		while(OMX_StatePause != m_pAudioDec->GetCompState())
		{
			voOMXOS_Sleep(2);
			if(++nTryTimes > 2500)
			{
				VOLOGE("OMX_ErrorTimeout");
				return OMX_ErrorTimeout;
			}
		}
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::SelectTrack(OMX_S32 nIndex,VOME_TRACKTYPE nType)
{
	VOLOGI("SelectTrack entering!nIndex is %d,nType is %d",nIndex,nType);
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if(nType == VOME_SOURCE_VIDEO)
	{
		VOLOGE("Now it is not supported for select video track");
		return OMX_ErrorNotImplemented;
	}

	if(OMX_StatePause != m_sState && OMX_StateExecuting != m_sState && OMX_StateIdle != m_sState)
	{
		VOLOGE("failed to Selecttrack, current state %d", (int)m_sState);
		return OMX_ErrorInvalidState;
	}
	OMX_STATETYPE eStateBeforeST = m_sState;
	if(OMX_StateExecuting == eStateBeforeST)
	{
		errType = Pause();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to Pause 0x%08X", errType);
			return errType;
		}
	}
	OMX_S32 nPosBeforeST = -1;
	if(OMX_StateIdle != eStateBeforeST)
		nPosBeforeST = GetPos();
	VOLOGI("play position [%d] before selectTrack", nPosBeforeST);
	if(nType == VOME_SOURCE_AUDIO)
	{
		errType = ShutdownAudioDecoder(eStateBeforeST);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to ShutdownAudioDecoder 0x%08x",errType);
			return errType;
		}
		VOME_TRACKSET trackSet;
		trackSet.nIndex = nIndex;
		trackSet.nTimeStamp = nPosBeforeST;
		m_pSource->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexSetAudioTrack,&trackSet);
		errType = RecreateAudioDecoder(eStateBeforeST);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to RecreateAudioDecoder 0x%08x",errType);
			return errType;
		}
	}
	// restore state
	if(OMX_StateExecuting == eStateBeforeST)
	{
		errType = Run();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to Run 0x%08X",errType);
			return errType;
		}
	}
	return OMX_ErrorNone;
}

OMX_U32 voCOMXCompBaseChain::EndOfStream (voCOMXBaseBox * pBox, voCOMXBaseHole * pHole)
{
	if (pBox == m_pVideoSink)
		m_bEndOfVideo = OMX_TRUE;
	if (pBox == m_pAudioSink)
		m_bEndOfAudio = OMX_TRUE;

	if (m_bEndOfVideo && m_bEndOfAudio)
	{
		VOLOGI ("Playback Finished!");

		if (m_pVideoSink != NULL)
			m_bEndOfVideo = OMX_FALSE;
		if (m_pAudioSink != NULL)
			m_bEndOfAudio = OMX_FALSE;

		m_nExitPos = GetPos();

		m_bEndOfSource = OMX_TRUE;

		m_pGraph->NotifyCallbackMsg(VOME_CID_PLAY_FINISHED, NULL, NULL);
	}
	return 0;
}

OMX_ERRORTYPE voCOMXCompBaseChain::SetCompCallBack (OMX_CALLBACKTYPE * pCompCallBack)
{
	m_pClientCB = pCompCallBack;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::SendBoxCommand (VOBOX_Command nCmd)
{
	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	char			szDebugMsg[128];
	bool			bError = false;

	// handle the clock time comp first for runing
	if (nCmd == VOBOX_CMD_StateExecute)
	{
		if (m_pClockBox != NULL)
		{
			errType =m_pClockBox->SendCommand (OMX_CommandStateSet, OMX_StateExecuting, NULL);
			if (!bError)
				strcat (szDebugMsg, "    Set State OMX_StatePause");
		}
	}

	// for (OMX_S32 i = VOMAX_BOX_NUM - 1; i >= 0;  i--)
	for (OMX_S32 i = 0; i < VOMAX_BOX_NUM;  i++)
	{
		if (m_aBox[i] != NULL)
		{
			if (nCmd == VOBOX_CMD_StateExecute || nCmd == VOBOX_CMD_StateIdle)
			{
				if (m_aBox[i] == m_pClockBox)
					continue;
			}

			if (!bError)
			{
				strcpy (szDebugMsg, "VOME Error in SendCommand ");
				strcat (szDebugMsg, m_aBox[i]->GetComponentName ());
			}

			switch (nCmd)
			{
			case VOBOX_CMD_DisablePort:
				errType = m_aBox[i]->DisablePorts ();
				if (!bError)
					strcat (szDebugMsg, "    DisablePorts");
				break;

			case VOBOX_CMD_AllocBuffer:
				errType = m_aBox[i]->AllocBuffer ();
				if (!bError)
					strcat (szDebugMsg, "    AllocBuffer");
				break;

			case VOBOX_CMD_FreeBuffer:
				errType = m_aBox[i]->FreeBuffer ();
				if (!bError)
					strcat (szDebugMsg, "    FreeBuffer");
				break;

			case VOBOX_CMD_StartBuffer:
				errType = m_aBox[i]->StartBuffer ();
				if (!bError)
					strcat (szDebugMsg, "    StartBuffer");
				break;

			case VOBOX_CMD_Flush:
				errType = m_aBox[i]->Flush ();
				if (!bError)
					strcat (szDebugMsg, "    Flush");
				break;

			case VOBOX_CMD_StateLoaded:
				errType = m_aBox[i]->SendCommand (OMX_CommandStateSet, OMX_StateLoaded, NULL);
				if (!bError)
					strcat (szDebugMsg, "    Set State OMX_StateLoaded");
				break;

			case VOBOX_CMD_StateIdle:
				errType = m_aBox[i]->SendCommand (OMX_CommandStateSet, OMX_StateIdle, NULL);
				if (!bError)
					strcat (szDebugMsg, "    Set State OMX_StateIdle");
				break;

			case VOBOX_CMD_StatePause:
				errType = m_aBox[i]->SendCommand (OMX_CommandStateSet, OMX_StatePause, NULL);
				if (!bError)
					strcat (szDebugMsg, "    Set State OMX_StatePause");
				break;

			case VOBOX_CMD_StateExecute:
				errType = m_aBox[i]->SendCommand (OMX_CommandStateSet, OMX_StateExecuting, NULL);
				if (!bError)
					strcat (szDebugMsg, "    Set State OMX_StateExecuting");
				break;

			default:
				break;
			}

			if (errType != OMX_ErrorNone && !bError)
				bError = true;
		}
	}

	// set the idle for clock component at last.
	if (nCmd == VOBOX_CMD_StateIdle)
	{
		if (m_pClockBox != NULL)
		{
			errType =m_pClockBox->SendCommand (OMX_CommandStateSet, OMX_StateIdle, NULL);
			if (!bError)
				strcat (szDebugMsg, "    Set State OMX_StatePause");
		}
	}

	if (bError)
		VOLOGE ("%s", szDebugMsg);

	return errType;
}

OMX_ERRORTYPE voCOMXCompBaseChain::CheckBoxStatus (VOBOX_Statue sStatus, OMX_U32 nTimeOut /* = VOMAX_TIMEOUT */)
{
	OMX_U32			nStart = voOMXOS_GetSysTime ();
	OMX_BOOL		bDone = OMX_FALSE;
	OMX_S32			i = 0;
	while (!bDone)
	{
		bDone = OMX_TRUE;
		// for (i = VOMAX_BOX_NUM - 1; i >= 0;  i--)
		for (i = 0; i < VOMAX_BOX_NUM;  i++)
		{
			if (m_aBox[i] != NULL)
			{
				if (sStatus < VOBOX_STATUS_WaitForResource)
				{
					if (sStatus == VOBOX_STATUS_Idle)
					{
						VO_U32 nHoles = m_aBox[i]->GetHoleCount ();
						for (OMX_U32 j = 0; j < nHoles; j++)
						{
							voCOMXBaseHole * pHole = m_aBox[i]->GetHole (j);
							if (pHole != NULL && pHole->IsEnable ())
							{
								int nTryTimes = 0;
								while (nTryTimes < 500)
								{
									if (pHole->GetBufferCount () == pHole->GetBufferEmpty ())
										break;

									voOMXOS_Sleep (2);
									nTryTimes++;
								}
								if (nTryTimes >= 500)
								{
									VOLOGW ("Check buffer status was time out. %s, Index %d, Count %d, Empty %d, Try %d", m_aBox[i]->GetComponentName(), (int)j, (int)pHole->GetBufferCount (), (int)pHole->GetBufferEmpty (), nTryTimes);
								}
							}
						}
					}

					if (m_aBox[i]->GetCompState () != (OMX_STATETYPE)sStatus)
					{
						if (m_aBox[i]->GetOMXError() != OMX_TRUE) {
							bDone = OMX_FALSE;
							break;
						}
					}
				}
				else
				{
					if (sStatus == VOBOX_STATUS_Flushing)
					{
						if (m_aBox[i]->IsFlush ())
						{
							bDone = OMX_FALSE;
							break;
						}
					}
				}
			}
		}

		if (!bDone)
		{
			if (voOMXOS_GetSysTime () - nStart > nTimeOut)
			{
				VOLOGE ("The status %d of component %s was wrong!", m_aBox[i]->GetCompState (), m_aBox[i]->GetComponentName ());
				return OMX_ErrorTimeout;
			}

			voOMXOS_Sleep (1);
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::CheckPortsStatus (PORT_TRANS_STATUS nStatus, OMX_U32 nTimeOut /* = VOMAX_TIMEOUT */)
{
	OMX_U32			nStart = voOMXOS_GetSysTime ();
	OMX_BOOL		bDone = OMX_FALSE;
	OMX_S32			i = 0;
	OMX_S32			boxNum = 0;
	while (!bDone)
	{
		bDone = OMX_TRUE;
		// for (i = VOMAX_BOX_NUM - 1; i >= 0;  i--)
		for (i = 0; i < VOMAX_BOX_NUM;  i++)
		{
			if (m_aBox[i] != NULL)
			{
				voCOMXBaseHole * pHole = NULL;

				for (int j = 0; j < (int)m_aBox[i]->GetHoleCount (); j++)
				{
					pHole = m_aBox[i]->GetHole (j);
					if (pHole != NULL)
					{
						if (pHole->GetTrans () != nStatus)
						{
							bDone = OMX_FALSE;
							boxNum = i;
							break;
						}
					}
				}
			}
		}

		if (!bDone)
		{
			if (voOMXOS_GetSysTime () - nStart > nTimeOut)
			{
				VOLOGE ("The port status %d of component %s was wrong!", m_aBox[boxNum]->GetCompState (), m_aBox[boxNum]->GetComponentName ());
				return OMX_ErrorTimeout;
			}

			voOMXOS_Sleep (1);		
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::CheckBoxPortSettingChanged (OMX_U32 nTimeOut /* = VOMAX_TIMEOUT */)
{
	OMX_U32			nStart = voOMXOS_GetSysTime ();
	OMX_BOOL		bDone = OMX_FALSE;
	while (!bDone)
	{
		bDone = OMX_TRUE;
		for (OMX_S32 i = 0; i < VOMAX_BOX_NUM; i++)
		{
			if (m_aBox[i] != NULL && m_aBox[i]->IsPortSettingsChanged() == OMX_FALSE)
			{
				bDone = OMX_FALSE;
				break;
			}
		}

		if (!bDone)
		{
			if (voOMXOS_GetSysTime () - nStart > nTimeOut)
			{
				VOLOGE ("CheckBoxPortSettingChanged TimeOut");
				return OMX_ErrorTimeout;
			}

			voOMXOS_Sleep (2);
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXCompBaseChain::voOMXEventHandler (OMX_IN OMX_HANDLETYPE hComponent,
									OMX_IN OMX_PTR pAppData,
									OMX_IN OMX_EVENTTYPE eEvent,
									OMX_IN OMX_U32 nData1,
									OMX_IN OMX_U32 nData2,
									OMX_IN OMX_PTR pEventData)
{

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	voCOMXBaseBox * pBox = (voCOMXBaseBox *)pAppData;
	if (pBox == NULL)
		return errType;

//	voCOMXAutoLock lock (pBox->GetChain ()->GetMutexCallBack ());

	OMX_CALLBACKTYPE * pCallBack = pBox->GetChain ()->GetCompCallBack ();
	if (pCallBack != NULL)
		pCallBack->EventHandler (hComponent, pAppData, eEvent, nData1, nData2, pEventData);

	if (eEvent == (OMX_EVENTTYPE)OMX_VO_Event_VideoRenderFrames)
	{
		pBox->GetChain ()->m_bSeekVideoRender = OMX_TRUE;
		return OMX_ErrorNone;
	}
	else if (eEvent == (OMX_EVENTTYPE)OMX_VO_Event_ComponentComplete)
	{
		pBox->GetChain ()->GetGraph ()->NotifyCallbackMsg(VOME_CID_REC_COMPLETE, NULL, NULL);
		return OMX_ErrorNone;
	}
	else if (eEvent == (OMX_EVENTTYPE)OMX_VO_Event_ComponentMaxFileSize)
	{
		pBox->GetChain ()->GetGraph ()->NotifyCallbackMsg(VOME_CID_REC_MAXFILESIZE, NULL, NULL);
		return OMX_ErrorNone;
	}
	else if (eEvent == (OMX_EVENTTYPE)OMX_VO_Event_ComponentMaxDuration)
	{
		pBox->GetChain ()->GetGraph ()->NotifyCallbackMsg(VOME_CID_REC_MAXDURATION, NULL, NULL);
		return OMX_ErrorNone;
	}
	//Rogine add for MFW Self-test
	else if (eEvent == (OMX_EVENTTYPE)OMX_VO_Event_AudioRenderTimeStamp)
	{
		pBox->GetChain ()->GetGraph ()->NotifyCallbackMsg(VOME_CID_PLAY_ARTIMESTAMP, pEventData, NULL);
		return OMX_ErrorNone;
	}
	else if (eEvent == (OMX_EVENTTYPE)OMX_VO_Event_VideoRenderTimeStamp)
	{
		pBox->GetChain ()->GetGraph ()->NotifyCallbackMsg(VOME_CID_PLAY_VRTIMESTAMP, pEventData, NULL);
		return OMX_ErrorNone;
	}
	else if (eEvent == (OMX_EVENTTYPE)OMX_VO_Event_AudioPlayTime)
	{
		pBox->GetChain ()->GetGraph ()->NotifyCallbackMsg(VOME_CID_PLAY_AUDIOPLAYTIME, pEventData, NULL);
		return OMX_ErrorNone;
	}

	errType = pBox->EventHandler (eEvent, nData1, nData2, pEventData);

	return errType;
}

OMX_ERRORTYPE voCOMXCompBaseChain::voOMXEmptyBufferDone (OMX_IN OMX_HANDLETYPE hComponent,
									OMX_IN OMX_PTR pAppData,
									OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	voCOMXBaseBox * pBox = (voCOMXBaseBox *)pAppData;
	if (pBox == NULL)
		return errType;

//	voCOMXAutoLock lock (pBox->GetChain ()->GetMutexCallBack ());

	OMX_CALLBACKTYPE * pCallBack = pBox->GetChain ()->GetCompCallBack ();
	if (pCallBack != NULL)
		pCallBack->EmptyBufferDone (hComponent, pAppData, pBuffer);

	errType = pBox->EmptyBufferDone (pBuffer);

	return errType;
}

OMX_ERRORTYPE voCOMXCompBaseChain::voOMXFillBufferDone (OMX_IN OMX_HANDLETYPE hComponent,
									OMX_IN OMX_PTR pAppData,
									OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	voCOMXBaseBox * pBox = (voCOMXBaseBox *)pAppData;
	if (pBox == NULL)
		return errType;

//	voCOMXAutoLock lock (pBox->GetChain ()->GetMutexCallBack ());

	OMX_CALLBACKTYPE * pCallBack = pBox->GetChain ()->GetCompCallBack ();
	if (pCallBack != NULL)
		pCallBack->FillBufferDone (hComponent, pAppData, pBuffer);

	errType = pBox->FillBufferDone (pBuffer);

	return errType;
}
