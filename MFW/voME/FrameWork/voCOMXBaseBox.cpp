	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseBox.cpp

	Contains:	voCOMXBaseBox class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#ifdef _LINUX_ANDROID
#include <cutils/properties.h>
#endif // _LINUX_ANDROID

#include "voOMXBase.h"
#include "voCOMXBaseGraph.h"
#include "voCOMXBaseBox.h"
#include "voOMXOSFun.h"
#include "voCOMXThirdPartyDef.h"

#define LOG_TAG "voCOMXBaseBox"
#include "voLog.h"

const VO_U32 gMaxHoleCount = 16;

voCOMXBaseBox::voCOMXBaseBox(voCOMXCompBaseChain * pChain)
	: m_pChain (pChain)
	, m_pComponent (NULL)
	, m_uType (vomeBOX_TYPE_UNKNOWN)
	, m_sState (OMX_StateInvalid)
	, m_sTrans (COMP_TRANSSTATE_None)
	, m_bFlush (OMX_FALSE)
	, m_nHoles (0)
	, m_ppHole (NULL)
	, m_bOMXError (OMX_FALSE)
	, m_bPortSettingsChanged (OMX_TRUE)
	, m_bPortSettingsChanging (OMX_FALSE)
	, m_nHardwareComponent (0)
	, m_bPretendPause (OMX_FALSE)
	, mnVideoFourCC(0)
	, mnAudioFourCC(0)
	, mIsHComb(false)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	strcpy ((OMX_STRING)m_szCompName, "");

#ifdef _LINUX_ANDROID
	char cVer[512];
	property_get("ro.build.version.release", cVer, "2.3");
	int nVer = atoi(cVer);
	if(nVer >= 3)
		mIsHComb = true;
#endif	//_LINUX_ANDROID
}

voCOMXBaseBox::~voCOMXBaseBox()
{
	ReleaseHoles ();
}

void voCOMXBaseBox::SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;
}

OMX_ERRORTYPE voCOMXBaseBox::SetComponent (OMX_COMPONENTTYPE * pComponent, OMX_STRING pName)
{
	if (pName != NULL)
		strcpy ((OMX_STRING)m_szCompName, pName);

	if(!strncmp(pName, "OMX.qcom.video.", 15))
	{
		if(strstr(pName, "decoder"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_QCOM_DECODER;
		else if(strstr(pName, "encoder"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_QCOM_ENCODER;
	}
	else if(!strncmp(pName, "OMX.TI.", 7))
	{
		if(strstr(pName, "Decoder")||strstr(pName, "DECODER"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_TI_DECODER;
		else if(strstr(pName, "Encoder"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_TI_ENCODER;
	}
	else if(!strncmp(pName, "OMX.Nvidia.", 11))
	{
		if(strstr(pName, "decode"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_NVIDIA_DECODER;
		else if(strstr(pName, "encode"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_NVIDIA_ENCODER;
	}
	else if (!strncmp(pName, "HISI.OMX.PV", 11))
	{
		if(strstr(pName, "dec"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_HUAWEI_DECODER;
		else if(strstr(pName, "enc"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_HUAWEI_ENCODER;
	}
	else if( !strncmp(pName, "OMX.MARVELL",11))
	{
		if(strstr(pName, "DEC"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_MARVELL_DECODER;
		else if(strstr(pName, "ENC"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_MARVELL_ENCODER;
	}
	else if( !strncmp(pName, "OMX.ST.VFM",10))
	{
		if(strstr(pName, "Dec"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_STE_DECODER;
		else if(strstr(pName, "Enc"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_STE_ENCODER;
	}
	else if( !strncmp(pName, "OMX.hantro",10)) {
		if(strstr(pName, "Dec"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_SEU_DECODER;
	}
	else if( !strncmp(pName, "OMX.SEC.",8))
	{
		if(strstr(pName, "Decoder"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_SAMSUNG_DECODER;
		else if(strstr(pName, "Encoder"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_SAMSUNG_ENCODER;
	}
	else if(!strncmp(pName, "OMX.Freescale.std.",18))
	{
		if(strstr(pName, "video_decoder"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_FSL_DECODER;
		else if(strstr(pName, "video_encoder"))
			m_nHardwareComponent = VOBOX_HARDWARECOMPONENT_FSL_ENCODER;
	}

	if(VOBOX_HARDWARECOMPONENT_QCOM_DECODER == m_nHardwareComponent 
	  || VOBOX_HARDWARECOMPONENT_NVIDIA_DECODER == m_nHardwareComponent
	  || VOBOX_HARDWARECOMPONENT_STE_DECODER == m_nHardwareComponent
	  || VOBOX_HARDWARECOMPONENT_SEU_DECODER == m_nHardwareComponent 
	  || VOBOX_HARDWARECOMPONENT_TI_DECODER == m_nHardwareComponent
	  || VOBOX_HARDWARECOMPONENT_FSL_DECODER == m_nHardwareComponent)
		m_bPortSettingsChanged = OMX_FALSE;

	if (m_pComponent != NULL)
		ReleaseHoles ();

	m_pComponent = pComponent;
	if (m_pComponent == NULL)
		return OMX_ErrorNone;

	if (m_pComponent->GetComponentVersion != NULL)
	{
		m_pComponent->GetComponentVersion (m_pComponent, (OMX_STRING)m_szName, &m_verMain,
											&m_verSpec, &m_uuidType);

		//commed by gtxia for fixing the Freescale issue 2012-3-22
		if(m_verMain.s.nVersionMajor == 0)
		{
			m_verMain.s.nVersionMajor = SPEC_VERSION_MAJOR ;
			m_verMain.s.nVersionMinor = SPEC_VERSION_MINOR;
			m_verMain.s.nRevision = SPEC_REVISION;
			m_verMain.s.nStep = SPEC_STEP;
		}
	}
	else
	{
		m_verMain.s.nVersionMajor = SPEC_VERSION_MAJOR;
		m_verMain.s.nVersionMinor = SPEC_VERSION_MINOR;
		m_verMain.s.nRevision = SPEC_REVISION;
		m_verMain.s.nStep = SPEC_STEP;
	}
	m_pComponent->GetState (m_pComponent, &m_sState);
	// we must set OMX_QCOM_FramePacking_OnlyOneCompleteFrame for QCM VC1 HW component, East 20110425
	if(VOBOX_HARDWARECOMPONENT_QCOM_DECODER == m_nHardwareComponent)
	{
		OMX_ERRORTYPE errType = OMX_ErrorNone;
		if(!isHoneyComb())	// ICS QCOM will crash because of extra data handle, so I block it, we will re-open it after QCOM fix it, East 20111215
		{
			// Enable decoder to report if there is any SEI data
			QOMX_ENABLETYPE enable_sei_reporting;
			enable_sei_reporting.bEnable = OMX_TRUE;
			errType = SetParameter((OMX_INDEXTYPE)OMX_QcomIndexParamFrameInfoExtraData, &enable_sei_reporting);
			if(OMX_ErrorNone != errType)
			{
				VOLOGE("OMX.qcom.video.decoder OMX_SetParameter OMX_QcomIndexParamFrameInfoExtraData 0x%08X", errType);
			}
		}
	
		if(strstr(pName, "vc1"))
		{
			OMX_QCOM_PARAM_PORTDEFINITIONTYPE inputPortFmt;
			voOMXBase_SetHeader(&inputPortFmt, sizeof(OMX_QCOM_PARAM_PORTDEFINITIONTYPE));
			inputPortFmt.nFramePackingFormat = OMX_QCOM_FramePacking_OnlyOneCompleteFrame;
			inputPortFmt.nPortIndex = 0;
			errType = SetParameter((OMX_INDEXTYPE)OMX_QcomIndexPortDefn, &inputPortFmt);
			if(OMX_ErrorNone != errType)
			{
				VOLOGW ("OMX.qcom.video.decoder.vc1 OMX_SetParameter OMX_QcomIndexPortDefn 0x%08X", errType);
			}
		}		
	}	
	
	if (!strncmp(pName, "OMX.ST.VFM.H264Dec", 18)) {
		OMX_INDEXTYPE nIndex;
		OMX_ERRORTYPE errType = m_pComponent->GetExtensionIndex(m_pComponent, const_cast<OMX_STRING>("OMX.ST.VFM.StaticDPBSize"), &nIndex);

		if (errType == OMX_ErrorNone) {
			VFM_PARAM_STATICDPBSIZE_TYPE staticDpbParam;
			voOMXBase_SetHeader(&staticDpbParam, sizeof(VFM_PARAM_STATICDPBSIZE_TYPE));
			staticDpbParam.bStaticDpbSize = OMX_FALSE;
			
			errType = SetParameter(nIndex, &staticDpbParam);
			if(errType != OMX_ErrorNone)
				VOLOGW("OMX.ST.VFM.H264Dec SetParameter with OMX.ST.VFM.StaticDPBSize failed %x\n", errType);
		}
	}
	if (((!strncmp(pName, "OMX.ST.VFM.H264Dec", 18)) ||
         (!strncmp(pName, "OMX.ST.VFM.MPEG4Dec", 19)) ||
         (!strncmp(pName, "OMX.ST.VFM.VC1Dec", 17)))) {
        OMX_INDEXTYPE nIndex;
		OMX_ERRORTYPE errType = m_pComponent->GetExtensionIndex(m_pComponent, const_cast<OMX_STRING>("OMX.ST.VFM.TimestampDisplayOrder"), &nIndex);
       
        if (errType == OMX_ErrorNone) {
            VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE timestampdisplayorder;
			voOMXBase_SetHeader(&timestampdisplayorder, sizeof(VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE));
			
            timestampdisplayorder.bDisplayOrder = OMX_TRUE;
            errType = SetParameter(nIndex, &timestampdisplayorder);
			if(errType != OMX_ErrorNone)
				VOLOGW("OMX.ST.VFM SetParameter with OMX.ST.VFM.TimestampDisplayOrder failed %x\n", errType);
        }

		errType = m_pComponent->GetExtensionIndex(m_pComponent, const_cast<OMX_STRING>("OMX.ST.VFM.RecyclingDelay"), &nIndex);
        if (errType == OMX_ErrorNone) {
            VFM_PARAM_RECYCLINGDELAYTYPE recyclingdelay;
			voOMXBase_SetHeader(&recyclingdelay, sizeof(VFM_PARAM_RECYCLINGDELAYTYPE));
			
            recyclingdelay.nRecyclingDelay = 1;
            errType = SetParameter(nIndex, &recyclingdelay);
			if(errType != OMX_ErrorNone)
				VOLOGW("OMX.ST.VFM SetParameter with OMX.ST.VFM.RecyclingDelay failed %x\n", errType);
        }

        if ((!strncmp(pName, "OMX.ST.VFM.H264Dec", 18)) ||
            (!strncmp(pName, "OMX.ST.VFM.VC1Dec", 17))) {
			
            errType = m_pComponent->GetExtensionIndex(m_pComponent, const_cast<OMX_STRING>("OMX.ST.VFM.SharedchunkInPlatformprivate"), &nIndex);
			
            if (errType == OMX_ErrorNone) {
                VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE sharedChunkInPP;
                voOMXBase_SetHeader(&sharedChunkInPP, sizeof(VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE));
                sharedChunkInPP.bSharedchunkInPlatformprivate = OMX_TRUE;
                errType = SetParameter(nIndex, &sharedChunkInPP);
				if(errType != OMX_ErrorNone)
					VOLOGW("OMX.ST.VFM SetParameter with OMX.ST.VFM VC1 and H264Dec.SharedchunkInPlatformprivate failed %x\n", errType);
            }
        }
    }
	return OMX_ErrorNone;
}

OMX_COMPONENTTYPE * voCOMXBaseBox::GetComponent (void)
{
	return m_pComponent;
}

OMX_STRING voCOMXBaseBox::GetComponentName (void)
{
	return (OMX_STRING)m_szCompName;
}

OMX_ERRORTYPE voCOMXBaseBox::SetHeadder (OMX_PTR pHeader, OMX_U32 nSize)
{
	if (pHeader == NULL)
		return OMX_ErrorBadParameter;

	voOMXMemSet (pHeader, 0, nSize);
	*((OMX_U32*)pHeader) = nSize;
	voOMXMemCopy ((OMX_S8 *)pHeader + 4, &m_verMain, sizeof (OMX_VERSIONTYPE));

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseBox::CreateHoles (void)
{
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	ReleaseHoles ();

	OMX_ERRORTYPE		errType = OMX_ErrorNone;
	OMX_PORT_PARAM_TYPE	typePort;
	OMX_U32	nAudioPorts = 0;
	OMX_U32	nVideoPorts = 0;
	
	voOMXBase_SetHeader(&typePort , sizeof(OMX_PORT_PARAM_TYPE));
	errType = m_pComponent->GetParameter (m_pComponent, OMX_IndexParamAudioInit, &typePort);
	if (errType == OMX_ErrorNone)
	{
		m_nHoles += typePort.nPorts;
		nAudioPorts = typePort.nPorts;
	}
	errType = m_pComponent->GetParameter (m_pComponent, OMX_IndexParamVideoInit, &typePort);
	if (errType == OMX_ErrorNone)
	{
		m_nHoles += typePort.nPorts;
		nVideoPorts = typePort.nPorts;
	}
	if (nAudioPorts <= 1 && nVideoPorts <= 1)
	{
		errType = m_pComponent->GetParameter (m_pComponent, OMX_IndexParamOtherInit, &typePort);
		if (errType == OMX_ErrorNone)
			m_nHoles += typePort.nPorts;
	}

	if(m_nHoles == 0)
	{
		for(VO_U32 ii = 0 ; ii < gMaxHoleCount ; ii++)
		{
			OMX_PARAM_PORTDEFINITIONTYPE portDef;
			voOMXBase_SetHeader(&portDef , sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
			portDef.nPortIndex = ii;
			errType = m_pComponent->GetParameter(m_pComponent , OMX_IndexParamPortDefinition, &portDef);
			if(errType != OMX_ErrorNone)
			{
				break;
			}

			switch (portDef.eDomain)
			{
			case OMX_PortDomainAudio: 
				nAudioPorts ++;
				m_nHoles ++;
				break;
			case OMX_PortDomainVideo:
				nVideoPorts ++;
				m_nHoles ++;
				break;
			case OMX_PortDomainImage:
				m_nHoles ++;
				break;
			case OMX_PortDomainOther:
				m_nHoles ++;
				break;
			default:
				break;
			}
		}
	}

	m_ppHole = (voCOMXBaseHole**)voOMXMemAlloc (m_nHoles * sizeof (voCOMXBaseHole *));
	if (m_ppHole == NULL)
		return OMX_ErrorInsufficientResources;

	voOMXMemSet (m_ppHole, 0, m_nHoles * sizeof (voCOMXBaseHole *));

	for (OMX_U32 i = 0; i < m_nHoles; i++)
	{
		m_ppHole[i] = new voCOMXBaseHole (this, i);
		if (m_ppHole[i] == NULL)
			return OMX_ErrorInsufficientResources;

		m_ppHole[i]->SetCallBack(m_pCallBack, m_pUserData);
	}

	return OMX_ErrorNone;
}

OMX_U32 voCOMXBaseBox::GetHoleCount (void)
{
	return m_nHoles;
}

voCOMXBaseHole * voCOMXBaseBox::GetHole (OMX_U32 nIndex)
{
	if (m_nHoles <= 0)
		CreateHoles ();

	if (nIndex >= m_nHoles)
		return NULL;

	return m_ppHole[nIndex];
}

OMX_S32 voCOMXBaseBox::GetDuration (void)
{
	OMX_ERRORTYPE errType;
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	OMX_TIME_CONFIG_TIMESTAMPTYPE tmInfo;
	SetHeadder (&tmInfo, sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE));

	OMX_U32 nDuration = 0;

	for (OMX_U32 i = 0; i < m_nHoles; i++)
	{
		tmInfo.nPortIndex = i;
		errType = GetConfig ((OMX_INDEXTYPE)OMX_VO_IndexConfigTimeDuration, &tmInfo);
		if (errType == OMX_ErrorNone)
		{
			if (tmInfo.nTimestamp > nDuration)
				nDuration = (OMX_U32)tmInfo.nTimestamp;
		}
	}
	VOLOGI ("The duration is %d", (int)nDuration);

	return nDuration;
}

OMX_S32 voCOMXBaseBox::GetPos (void)
{
	OMX_ERRORTYPE errType;
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	OMX_TIME_CONFIG_TIMESTAMPTYPE tmInfo;
	SetHeadder (&tmInfo, sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE));

	OMX_U32 nPos = 0;

	for (OMX_U32 i = 0; i < m_nHoles; i++)
	{
		tmInfo.nPortIndex = i;
		errType = GetConfig (OMX_IndexConfigTimePosition, &tmInfo);
		if (errType == OMX_ErrorNone)
		{
			if (tmInfo.nTimestamp > nPos)
				nPos = (OMX_U32)tmInfo.nTimestamp;
		}
	}
	VOLOGR ("The current pos is %d", nPos);

	return nPos;
}

OMX_S32 voCOMXBaseBox::SetPos (OMX_S32 nPos)
{
	OMX_ERRORTYPE errType;
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	OMX_TIME_CONFIG_TIMESTAMPTYPE tmInfo;
	SetHeadder (&tmInfo, sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE));
	tmInfo.nTimestamp = nPos;

	errType = SetConfig (OMX_IndexConfigTimePosition, &tmInfo);
	if (errType != OMX_ErrorNone)
		VOLOGE ("The result is 0X%08X from SetConfig in Component %s.", errType, (OMX_STRING)m_szCompName);

	nPos = (OMX_S32)tmInfo.nTimestamp;

	return nPos;
}

OMX_ERRORTYPE voCOMXBaseBox::SendCommand (OMX_COMMANDTYPE nCmd, OMX_U32 nParam, OMX_PTR pData)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	if (nCmd == OMX_CommandStateSet)
	{
		if (m_sTrans != COMP_TRANSSTATE_None)
		{
			VOLOGE ("m_sTrans != COMP_TRANSSTATE_None");
			return OMX_ErrorIncorrectStateTransition;
		}

		if (m_sState == (OMX_S32)nParam)
		{
			VOLOGE ("m_sState == nParam");
			return OMX_ErrorSameState;
		}

		if (nParam == OMX_StateLoaded)
		{
			if (m_sState == OMX_StateIdle)
				m_sTrans = COMP_TRANSSTATE_IdleToLoaded;
			else if (m_sState == OMX_StateWaitForResources)
				m_sTrans = COMP_TRANSSTATE_WaitResToLoaded;
			else
				return OMX_ErrorIncorrectStateTransition;
		}
		else if (nParam == OMX_StateWaitForResources)
		{
			if (m_sState == OMX_StateLoaded)
				m_sTrans = COMP_TRANSSTATE_LoadedToWaitRes;
			else
				return OMX_ErrorIncorrectStateTransition;
		}
		else if (nParam == OMX_StateIdle)
		{
			if (m_sState == OMX_StateLoaded)
				m_sTrans = COMP_TRANSSTATE_LoadedToIdle;
			else if (m_sState == OMX_StateWaitForResources)
				m_sTrans = COMP_TRANSSTATE_WaitResToIdle;
			else if (m_sState == OMX_StateExecuting)
				m_sTrans = COMP_TRANSSTATE_ExecuteToIdle;
			else if (m_sState == OMX_StatePause)
				m_sTrans = COMP_TRANSSTATE_PauseToIdle;
			else
				return OMX_ErrorIncorrectStateTransition;

			for(OMX_U32 i = 0; i < m_nHoles; i++)
				m_ppHole[i]->SetChanging2IdleState(OMX_TRUE);
		}
		else if (nParam == OMX_StatePause)
		{
			if (m_sState == OMX_StateIdle)
				m_sTrans = COMP_TRANSSTATE_IdleToPause;
			else if (m_sState == OMX_StateExecuting)
			{
				if (VOBOX_HARDWARECOMPONENT_QCOM_DECODER == m_nHardwareComponent)
				{
					m_sState = OMX_StatePause;
					m_bPretendPause = OMX_TRUE;

					return OMX_ErrorNone;
				}
				m_sTrans = COMP_TRANSSTATE_ExecuteToPause;
			}
			else
				return OMX_ErrorIncorrectStateTransition;
		}
		else if (nParam == OMX_StateExecuting)
		{
			if (m_sState == OMX_StateIdle)
				m_sTrans = COMP_TRANSSTATE_IdleToExecute;
			else if (m_sState == OMX_StatePause)
			{
				if (m_bPretendPause)
				{
					m_sState = OMX_StateExecuting;
					m_bPretendPause = OMX_FALSE;

					return OMX_ErrorNone;
				}
				m_sTrans = COMP_TRANSSTATE_PauseToExecute;
			}
			else
				return OMX_ErrorIncorrectStateTransition;
		}
		else if (nParam == OMX_StateInvalid)
		{
			m_sTrans = COMP_TRANSSTATE_AnyToInvalid;
		}
	}

	// QCM 8x60 component will wait all buffers returned, so do it async for hang issue, East 20110516
	if(m_nHardwareComponent == VOBOX_HARDWARECOMPONENT_QCOM_DECODER ||
	   m_nHardwareComponent == VOBOX_HARDWARECOMPONENT_STE_DECODER ||
	   m_nHardwareComponent == VOBOX_HARDWARECOMPONENT_NVIDIA_DECODER 
	   /* ||m_nHardwareComponent == VOBOX_HARDWARECOMPONENT_TI_DECODER*/ 
		)
	{
		if(nCmd == OMX_CommandStateSet && nParam == OMX_StateIdle)
		{
			voOMXThreadHandle hThread;
			OMX_U32 nThreadID;
			voOMXThreadCreate(&hThread, &nThreadID, (voOMX_ThreadProc)SetStateIdleProc, this, 0);
			errType = OMX_ErrorNone;
		}
		else if(nCmd == OMX_CommandFlush)
		{
			voOMXThreadHandle hThread;
			OMX_U32 nThreadID;
			voOMXThreadCreate(&hThread, &nThreadID, (voOMX_ThreadProc)FlushProc, this, 0);
			errType = OMX_ErrorNone;
		}
		else
			errType = m_pComponent->SendCommand (m_pComponent, nCmd, nParam, pData);
	}
	else
		errType = m_pComponent->SendCommand (m_pComponent, nCmd, nParam, pData);

	if (errType != OMX_ErrorNone)
		VOLOGE ("The result is 0X%08X from SendCommand in Component %s.", errType, (OMX_STRING)m_szCompName);

	return errType;
}

OMX_U32 voCOMXBaseBox::SetStateIdleProc(OMX_PTR pParam)
{
	voCOMXBaseBox* pBox = (voCOMXBaseBox*)pParam;

	return pBox->SetStateIdleProcB();
}

OMX_U32 voCOMXBaseBox::SetStateIdleProcB()
{
	//OMX_ERRORTYPE errType = m_pComponent->SendCommand (m_pComponent, OMX_CommandStateSet, OMX_StateIdle, NULL);
	m_pComponent->SendCommand (m_pComponent, OMX_CommandStateSet, OMX_StateIdle, NULL);
	return 0;
}

OMX_U32 voCOMXBaseBox::FlushProc(OMX_PTR pParam)
{
	voCOMXBaseBox* pBox = (voCOMXBaseBox*)pParam;

	return pBox->FlushProcB();
}

OMX_U32 voCOMXBaseBox::FlushProcB()
{
	//OMX_ERRORTYPE errType = m_pComponent->SendCommand (m_pComponent, OMX_CommandFlush, OMX_ALL, NULL);
	m_pComponent->SendCommand (m_pComponent, OMX_CommandFlush, OMX_ALL, NULL);
	return 0;
}

OMX_ERRORTYPE voCOMXBaseBox::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	return m_pComponent->GetParameter (m_pComponent, nIndex, pParam);
}

OMX_ERRORTYPE voCOMXBaseBox::SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	return m_pComponent->SetParameter (m_pComponent, nIndex, pParam);
}

OMX_ERRORTYPE voCOMXBaseBox::GetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig)
{
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	return m_pComponent->GetConfig (m_pComponent, nIndex, pConfig);
}

OMX_ERRORTYPE voCOMXBaseBox::SetConfig (OMX_INDEXTYPE nIndex, OMX_PTR pConfig)
{
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	return m_pComponent->SetConfig (m_pComponent, nIndex, pConfig);
}

OMX_ERRORTYPE voCOMXBaseBox::DisablePorts (void)
{
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	for (OMX_U32 i = 0; i < m_nHoles; i++)
	{
		if (m_ppHole[i]->GetConnected () == NULL)
			m_ppHole[i]->Disable ();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseBox::AllocBuffer (void)
{
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_nHoles; i++)
	{
		errType = m_ppHole[i]->AllocBuffer ();
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("The result is 0X%08X on hole %d in Component %s.", errType, (int)i, (OMX_STRING)m_szCompName);
			return errType;
		}
	}

	return errType;
}

OMX_ERRORTYPE voCOMXBaseBox::FreeBuffer (void)
{
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	for (OMX_U32 i = 0; i < m_nHoles; i++)
	{
		errType = m_ppHole[i]->FreeBuffer ();
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("The result is 0X%08X on hole %d in Component %s.", errType, (int)i, (OMX_STRING)m_szCompName);
			return errType;
		}
	}

	return errType;
}


OMX_ERRORTYPE voCOMXBaseBox::StartBuffer (void)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

	for (OMX_U32 i = 0; i < m_nHoles; i++)
	{
		errType = m_ppHole[i]->StartBuffer ();
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("The result is 0X%08X on hole %d in Component %s.", errType, (int)i, (OMX_STRING)m_szCompName);
			return errType;
		}
	}

	return errType;
}

OMX_ERRORTYPE voCOMXBaseBox::Flush (void)
{
	if (m_pComponent == NULL)
		return OMX_ErrorIncorrectStateOperation;

// 	if (m_bPortSettingChanged == OMX_FALSE)
// 		return OMX_ErrorNone;

	m_bFlush = OMX_TRUE;
	
	for (OMX_U32 i = 0; i < m_nHoles; i++)
		m_ppHole[i]->SetFlush (OMX_TRUE);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	// added by gtxia 2012-3-31
	// because Freescale Hardware decoder do not support it
	if(m_nHardwareComponent == VOBOX_HARDWARECOMPONENT_FSL_DECODER)
		errType = SendCommand(OMX_CommandFlush, 0, NULL);
	else		
		errType = SendCommand (OMX_CommandFlush, OMX_ALL, NULL);

	if (errType != OMX_ErrorNone)
		VOLOGE ("The result is 0X%08X from SendCommand in Component %s.", errType, (OMX_STRING)m_szCompName);

	return errType;
}

OMX_BOOL voCOMXBaseBox::IsFlush (void)
{
	return m_bFlush;
}

OMX_STATETYPE voCOMXBaseBox::GetCompState (void)
{
	return m_sState;
}

OMX_TRANS_STATE voCOMXBaseBox::GetTransState (void)
{
	return m_sTrans;
}

vomeBOX_TYPE voCOMXBaseBox::GetCompType (void)
{
	return m_uType;
}

OMX_ERRORTYPE voCOMXBaseBox::EventHandler (OMX_EVENTTYPE eEvent,OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	switch (eEvent)
	{
	case OMX_EventCmdComplete:
		{
			switch (nData1)
			{
			case OMX_CommandStateSet:
				{
					VOLOGI ("Component %s OMX_EventCmdComplete OMX_CommandStateSet, new state %d", m_szCompName, (int)nData2);

					m_sState = (OMX_STATETYPE) nData2;
					m_sTrans = COMP_TRANSSTATE_None;
					m_bPretendPause = OMX_FALSE;

					if(OMX_StateIdle == m_sState)
					{
						for(OMX_U32 i = 0; i < m_nHoles; i++)
							m_ppHole[i]->SetChanging2IdleState(OMX_FALSE);
					}
				}
				break;

			case OMX_CommandFlush:
				{
					// becasue Freescale hardware decoder can not support Flush OMX_ALL, I have to emulate it.
					// commented by gtxia  2012-3-31
					if(m_nHardwareComponent == VOBOX_HARDWARECOMPONENT_FSL_DECODER)
					{
						if( 0 == nData2)
							SendCommand(OMX_CommandFlush, 1, NULL);
					}
					OMX_BOOL bDone = OMX_TRUE;

					if (nData2 >= 0 && nData2 < m_nHoles)
						m_ppHole[nData2]->SetFlush (OMX_FALSE);

					for (OMX_U32 i = 0; i < m_nHoles; i++)
					{
						if (m_ppHole[i]->GetFlush ())
						{
							bDone = OMX_FALSE;
							break;
						}
					}

					if (bDone)
						m_bFlush = OMX_FALSE;
				}
				break;

			case OMX_CommandPortDisable:
				{
					VOLOGI ("Component %s OMX_EventCmdComplete OMX_CommandPortDisable port is %d", m_szCompName, (int)nData2);
					if (nData2 >= 0 && nData2 < m_nHoles)
						m_ppHole[nData2]->FillPortType ();
				}
				break;

			case OMX_CommandPortEnable:
				{
					VOLOGI ("Component %s OMX_EventCmdComplete OMX_CommandPortEnable port is %d", m_szCompName, (int)nData2);
					if (nData2 >= 0 && nData2 < m_nHoles)
						m_ppHole[nData2]->FillPortType ();
				}
				break;

			case OMX_CommandMarkBuffer:
				break;

			default:
				break;
			}
		}
		break;

	case OMX_EventError:
		{
			VOLOGI ("Component %s OMX_EventError Event: %d, Data1 0X%08X, Data2 %d", m_szCompName, (int)eEvent, (unsigned int)nData1, (int)nData2);
			m_bOMXError = OMX_TRUE;
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_ERROR, 0, NULL);
		}
		break;

	case OMX_EventMark:
		break;

	case OMX_EventPortSettingsChanged:
		{
			VOLOGI ("Component %s OMX_EventPortSettingsChanged The port is %d, nData2 %d", m_szCompName, (int)nData1, (int)nData2);
			if(isHoneyComb() && nData2 == OMX_IndexConfigCommonOutputCrop)
			{
				OMX_CONFIG_RECTTYPE rectType;
				SetHeadder(&rectType, sizeof(OMX_CONFIG_RECTTYPE));
				rectType.nPortIndex = nData1;
				errType = GetConfig(OMX_IndexConfigCommonOutputCrop, &rectType);
				if(OMX_ErrorNone == errType)
				{
					voCOMXBaseHole * pHole = GetHole(nData1);
					if(OMX_TRUE == pHole->IsUseNativeWindow())
						pHole->NativeWindow_SetCrop(rectType.nLeft, rectType.nTop, rectType.nLeft + rectType.nWidth, rectType.nTop + rectType.nHeight);
				}
				return errType;
			}

			m_bPortSettingsChanging = OMX_TRUE;
			OMX_BOOL bUseThread = OMX_FALSE;
			if(VOBOX_HARDWARECOMPONENT_QCOM_DECODER == m_nHardwareComponent 
				|| VOBOX_HARDWARECOMPONENT_NVIDIA_DECODER == m_nHardwareComponent
				|| VOBOX_HARDWARECOMPONENT_STE_DECODER == m_nHardwareComponent
				|| VOBOX_HARDWARECOMPONENT_SEU_DECODER == m_nHardwareComponent
			    || VOBOX_HARDWARECOMPONENT_TI_DECODER == m_nHardwareComponent
			    || VOBOX_HARDWARECOMPONENT_FSL_DECODER == m_nHardwareComponent
			  )
				bUseThread = OMX_TRUE;
			m_pChain->ReconfigPort(GetHole(nData1), bUseThread);
		}
		break;

	case OMX_EventBufferFlag:
		{
			if (nData2 & OMX_BUFFERFLAG_EOS)
			{
				VOLOGI ("Component %s OMX_EventBufferFlag OMX_BUFFERFLAG_EOS The port is %d", m_szCompName, (int)nData1);

				m_pChain->EndOfStream (this, m_ppHole[nData1]);
			}
		}
		break;

	case OMX_EventResourcesAcquired:
		break;

	case OMX_EventComponentResumed:
		break;

	case OMX_EventDynamicResourcesAvailable:
		break;

	case OMX_EventPortFormatDetected:
		break;

	case OMX_VO_Event_BufferStart:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_BUFFERSTART, (OMX_PTR)nData1, 0);
		}
		break;

	case OMX_VO_Event_Bufferstatus:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_BUFFERING, (OMX_PTR)nData1, 0);
		}
		break;

	case OMX_VO_Event_BufferStop:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_BUFFERSTOP, (OMX_PTR)nData1, 0);
		}
		break;

	case OMX_VO_Event_DownloadPos:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_DOWNLOADPOS, (OMX_PTR)nData1, 0);
		}
		break;

	case OMX_VO_Event_StreamError:
 		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg(VOME_CID_PLAY_STREAMERROREVENT, (OMX_PTR)nData1, 0);
			if(nData2 == 1)	//runtime error
			{
				//if(nData1 == E_CONNECTION_GOODBYE)
				if(nData1 == OMX_U32((-1000) - 600))
					pGraph->NotifyCallbackMsg(VOME_CID_PLAY_SERVERGOODBYE, 0, 0);
				pGraph->NotifyCallbackMsg (VOME_CID_PLAY_ERROR, 0, 0);
			}
 		}
		break;

	case OMX_VO_Event_PacketLost:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_PACKETLOST, 0, 0);
		}
		break;

	case OMX_VO_Event_CodecPassableError:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_CODECPASSABLEERR, (OMX_PTR)nData1, 0);
		}
		break;

	case OMX_VO_Event_MediaContentType:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_MEDIACONTENTTYPE, (OMX_PTR)nData1, 0);
		}
		break;

	case OMX_VO_Event_MediaFourcc:
		{
			if(nData1 == 1)
				mnVideoFourCC = nData2;
			else if(nData1 == 0)
				mnAudioFourCC = nData2;

			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_MEDIAFOURCC, (OMX_PTR)nData1, (OMX_PTR)nData2);
		}
		break;

	case OMX_VO_Event_FormatUnsupported:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_UNSUPPORTEDFORMAT, 0, 0);
		}
		break;

	case OMX_VO_Event_CodecUnsupported:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_UNSUPPORTEDCODEC, 0, 0);
		}
		break;

	case OMX_VO_Event_FrameDropped:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_FRAMEDROPPED, 0, 0);
		}
		break;

	case OMX_VO_Event_NotSeekable:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_NOTSEEKABLE, 0, 0);
		}
		break;

	case OMX_VO_Event_StreamBitrateThroughput:
		{
			voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			pGraph->NotifyCallbackMsg (VOME_CID_PLAY_STREAMBITRATETHROUGHPUT, (OMX_PTR)nData1, (OMX_PTR)nData2);
		}
		break;

	case OMX_VO_Event_StreamRtcpRR:
		{
			//voCOMXBaseGraph * pGraph = m_pChain->GetGraph ();
			m_pChain->GetGraph ();
			m_pCallBack (m_pUserData, VOME_CID_PLAY_STREAMRTCPRR, (OMX_PTR)nData1, 0);
		}
		break;

	case OMX_VO_Event_RTSPSessionEvent:
		{
			VOLOGR ("Event Code %d, Event Param 0x%08X", nData1, nData2);
		}
		break;

	case OMX_EventComponentLog:
		{
			sprintf(m_szLog, "VOLOG: Info  EventHandler - Log %s \n", (OMX_STRING)pEventData);
			m_pCallBack (m_pUserData, VOME_CID_PLAY_DEBUGTEXT, m_szLog, NULL);
		}
		break;

	default:
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXBaseBox::EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
	voCOMXBaseHole * pHole = (voCOMXBaseHole *)pBuffer->pAppPrivate;

	return pHole->EmptyBufferDone (pBuffer);
}

OMX_ERRORTYPE voCOMXBaseBox::FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
	voCOMXBaseHole * pHole = (voCOMXBaseHole *)pBuffer->pAppPrivate;

	if(isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent))	// HoneyComb use pAppPrivate as GraphicBuffer
		pHole = m_ppHole[1];

	return pHole->FillBufferDone (pBuffer);
}

void voCOMXBaseBox::ReleaseHoles (void)
{
	if (m_ppHole != NULL)
	{
		for (OMX_U32 i = 0; i < m_nHoles; i++)
		{
			if (m_ppHole[i] != NULL)
			{
				delete m_ppHole[i];
				m_ppHole[i] = NULL;	
			}
		}

		voOMXMemFree (m_ppHole);
		m_ppHole = NULL;
		m_nHoles = 0;
	}
}

bool voCOMXBaseBox::isHoneyComb()
{
	return mIsHComb;
}

OMX_BOOL voCOMXBaseBox::NativeWindow_IsUsable()
{
	OMX_BOOL bIsUsable = OMX_TRUE;

	VOME_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOME_NATIVEWINDOW_IsUsable;
	sCommand.pParam1 = &bIsUsable;
	sCommand.pParam2 = NULL;

	OMX_S32 nRet = m_pCallBack(m_pUserData, VOME_CID_PLAY_NATIVEWINDOWCOMMAND, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
	{
		VOLOGW("VOME_NATIVEWINDOW_IsUsable not supported, so use default value true");
		return OMX_TRUE;
	}

	return bIsUsable;
}
