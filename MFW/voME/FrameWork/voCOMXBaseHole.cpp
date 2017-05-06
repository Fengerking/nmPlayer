	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseHole.cpp

	Contains:	voCOMXBaseHole class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include "voOMXBase.h"
#include "voOMXOSFun.h"
#include "voCOMXCompBaseChain.h"
#include "voCOMXBaseHole.h"
#include "voIVCommon.h"
#include "voCOMXThirdPartyDef.h"

#define LOG_TAG "voCOMXBaseHole"
#include "voLog.h"

#define OMX_COLOR_Format_SamSuangFK 0x7F000010 // Fake SamSung's hardware decoder

//#define _DUMP_INPUT_DEC_DATA

#if defined(_DUMP_INPUT_DEC_DATA)
#    include <sys/types.h>
#    include <sys/stat.h>
#    include <fcntl.h>
#    include <string.h>
#    include <errno.h>
int  g_input = -1;
#endif


voCOMXBufferHeaderList::voCOMXBufferHeaderList()
	: m_ppBufferHeaders(NULL)
	, m_nBufferHeaders(0)
	, m_nCount(0)
{
}

voCOMXBufferHeaderList::~voCOMXBufferHeaderList()
{
	if(m_ppBufferHeaders)
		voOMXMemFree(m_ppBufferHeaders);
}

OMX_BOOL voCOMXBufferHeaderList::Create(OMX_S32 nBufferHeaders)
{
	if(m_ppBufferHeaders)
		voOMXMemFree(m_ppBufferHeaders);

	m_nBufferHeaders = 0;
	m_nCount = 0;

	if(nBufferHeaders > 0)
	{
		m_ppBufferHeaders = (OMX_BUFFERHEADERTYPE**)voOMXMemAlloc(nBufferHeaders * sizeof (OMX_BUFFERHEADERTYPE*));
		if(m_ppBufferHeaders == NULL)
			return OMX_FALSE;

		voOMXMemSet(m_ppBufferHeaders, 0, nBufferHeaders * sizeof(OMX_BUFFERHEADERTYPE*));
	}

	m_nBufferHeaders = nBufferHeaders;

	return OMX_TRUE;
}

OMX_BOOL voCOMXBufferHeaderList::Add(OMX_BUFFERHEADERTYPE* pBufferHeader)
{
	for(OMX_S32 i = 0; i < m_nBufferHeaders; i++)
	{
		if(m_ppBufferHeaders[i] == NULL)
		{
			m_ppBufferHeaders[i] = pBufferHeader;
			m_nCount++;

			return OMX_TRUE;
		}
	}

	return OMX_FALSE;
}

OMX_BOOL voCOMXBufferHeaderList::Remove(OMX_BUFFERHEADERTYPE* pBufferHeader)
{
	OMX_S32 nDelete = -1;
	OMX_S32 i = 0;
	for(i = 0; i < m_nBufferHeaders; i++)
	{
		if(m_ppBufferHeaders[i] == pBufferHeader)
		{
			nDelete = i;
			break;
		}
	}

	if(nDelete < 0)
		return OMX_FALSE;

	for(i = nDelete; i < m_nBufferHeaders - 1; i++)
	{
		m_ppBufferHeaders[i] = m_ppBufferHeaders[i + 1];
		if(m_ppBufferHeaders[i + 1] == NULL)
			break;
	}

	if(i == m_nBufferHeaders - 1)
		m_ppBufferHeaders[i] = NULL;

	m_nCount--;
	return OMX_TRUE;
}

OMX_BUFFERHEADERTYPE* voCOMXBufferHeaderList::GetByBuffer(OMX_U8* pBuffer)
{
	for(OMX_S32 i = 0; i < m_nBufferHeaders && m_ppBufferHeaders[i]; i++)
	{
		if(m_ppBufferHeaders[i]->pBuffer == pBuffer)
			return m_ppBufferHeaders[i];
	}

	return NULL;
}

void voCOMXBufferHeaderList::RemoveAll()
{
	for(OMX_S32 i = 0; i < m_nBufferHeaders; i++)
		m_ppBufferHeaders[i] = NULL;
	m_nCount = 0;
}

voCOMXBaseHole::voCOMXBaseHole(voCOMXBaseBox * pBox, OMX_U32 nIndex)
	: m_pParent (pBox)
	, m_pComponent (pBox->GetComponent ())
	, m_nIndex (nIndex)
	, m_bSupplier (OMX_FALSE)
	, m_pConnected (NULL)
	, m_bTunnel (OMX_FALSE)
	, m_nTrans (PORT_TRANS_DONE)
	, m_bFlush (OMX_FALSE)
	, m_bStartBuffer (OMX_FALSE)
	, m_bEOS (OMX_FALSE)
	, m_nBuffCount (0)
	, m_ppBuffHead (NULL)
	, m_ppBuffData (0)
	, m_bGetHeadData (OMX_TRUE)
	, m_pHeadData (NULL)
	, m_nHeadSize (0)
	, m_nTimeDivision (1)
	, m_nHardwareComponent (pBox->GetHardwareComponentType())
	, m_nMinUndequeuedBuffers (1)
	, m_bChanging2IdleState (OMX_FALSE)
	, m_b3DVideoDetected(OMX_FALSE)
	, m_bUseNativeWindow(OMX_TRUE)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	FillPortType();

	if(VOBOX_HARDWARECOMPONENT_QCOM_ENCODER == m_nHardwareComponent && OMX_DirOutput == m_portType.eDir)
		m_bGetHeadData = OMX_FALSE;

	if (!strcmp (m_pParent->GetComponentName (), "OMX.qcom.flo.source"))
		m_nTimeDivision = 1000;
	
	// added by gtxia for huawei 
	if(!strncmp(m_pParent->GetComponentName (), "HISI.OMX.PV", 11))
		m_nTimeDivision = 1000;

	if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent) && OMX_DirOutput == m_portType.eDir)
	{
		m_bUseNativeWindow = m_pParent->NativeWindow_IsUsable();
		VOLOGI("m_bUseNativeWindow %d", int(m_bUseNativeWindow));
		if(m_bUseNativeWindow)
		{
			// Enable use of GraphicBuffer as the output for this node
			OMX_INDEXTYPE nIndex;
			OMX_ERRORTYPE errType = m_pComponent->GetExtensionIndex(m_pComponent, const_cast<OMX_STRING>("OMX.google.android.index.enableAndroidNativeBuffers"), &nIndex);
			if (errType == OMX_ErrorNone)
			{
				EnableAndroidNativeBuffersParams params;
				params.nSize = sizeof(EnableAndroidNativeBuffersParams);
				memset(&params.nVersion.s, 0, sizeof(params.nVersion.s));
				params.nVersion.s.nVersionMajor = 1;
				params.nPortIndex = m_nIndex;
				params.enable = OMX_TRUE;
				errType = m_pComponent->SetParameter(m_pComponent, nIndex, &params);
				VOLOGI ("Enable use of GraphicBuffer 0x%08X 0x%08X", nIndex, errType);
			}
			else
				VOLOGE("OMX_GetExtensionIndex failed 0x%08X", errType);
		}
	}
	getAndroidNativeBufferVer(mUANBVer);

#if defined(_DUMP_INPUT_DEC_DATA)
	if(VOBOX_HARDWARECOMPONENT_STE_DECODER == m_nHardwareComponent && OMX_DirInput == m_portType.eDir)
	{
		g_input = open("/data/local/input_dec_data.dat", O_CREAT|O_RDWR, 0644);
		if(g_input == -1)
			VOLOGE("can not open /data/local/input_dec_data.dat file the reason is %s\n", strerror(errno));
	}
#endif
}

voCOMXBaseHole::~voCOMXBaseHole()
{
	if (m_ppBuffHead != NULL){
		voOMXMemFree (m_ppBuffHead);
		m_ppBuffHead = NULL;
	}
	
	if (m_ppBuffData != NULL)
	{
		for (OMX_S32 i = 0; i < m_nBuffCount; i++)
		{
			if (m_ppBuffData[i] != NULL){
				voOMXMemFree (m_ppBuffData[i]);
				m_ppBuffData[i] = NULL;
			}
		}
		voOMXMemFree (m_ppBuffData);
	}

	if (m_pHeadData != NULL){
		delete []m_pHeadData;
		m_pHeadData = NULL;
	}
#if defined(_DUMP_INPUT_DEC_DATA)
	if(VOBOX_HARDWARECOMPONENT_STE_DECODER == m_nHardwareComponent && OMX_DirInput == m_portType.eDir)
	{
		if(g_input != -1)
		{
			close(g_input);
			g_input = -1;	
		}
	}
#endif
}

void voCOMXBaseHole::SetCallBack (VOMECallBack pCallBack, OMX_PTR pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;
}

OMX_ERRORTYPE voCOMXBaseHole::ConnectedTo (voCOMXBaseHole* pInputHole, OMX_BOOL bTunnel)
{
	if (pInputHole == NULL)
	{
		VOLOGE ("pInputHole == NULL");
		return OMX_ErrorBadParameter;
	}	
	if (m_portType.eDir == OMX_DirInput || !pInputHole->IsInput ())
	{
		VOLOGE ("The dir is same.");
		return OMX_ErrorPortsNotCompatible;
	}
	
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_portType.eDir == OMX_DirOutput)
	{
		FillPortType ();

		if (m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent) && m_bUseNativeWindow)
		{
			// Increase the buffer count by one to allow for the ANativeWindow to hold on one of the buffers.
			// Nvidia is 1, QCM is 2 or 3
			m_nMinUndequeuedBuffers = NativeWindow_GetMinUndequeuedBuffers();

			// added by gtxia 2012-3-19
			if(VOBOX_HARDWARECOMPONENT_FSL_DECODER == m_nHardwareComponent)
				m_nMinUndequeuedBuffers = 0;

			if(m_portType.nBufferCountActual < m_portType.nBufferCountMin + m_nMinUndequeuedBuffers)
				m_portType.nBufferCountActual = m_portType.nBufferCountMin + m_nMinUndequeuedBuffers;
			errType = m_pParent->SetParameter(OMX_IndexParamPortDefinition, &m_portType);
			VOLOGI ("errType 0x%08X, nBufferCountActual %d, nBufferCountMin %d, Min Un-dequeued Buffers %d, Width %d, Height %d, Color %d", 
				errType, (int)m_portType.nBufferCountActual, (int)m_portType.nBufferCountMin, (int)m_nMinUndequeuedBuffers, 
				(int)m_portType.format.video.nFrameWidth, (int)m_portType.format.video.nFrameHeight, (int)m_portType.format.video.eColorFormat);
		}

// 		if (VOBOX_HARDWARECOMPONENT_QCOM_DECODER == m_nHardwareComponent)
// 		{
// 			OMX_BOOL bHoldBuffer = OMX_TRUE;
// 			pInputHole->GetParentBox()->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexRenderHoldBuffer, &bHoldBuffer);
// 		}
	}

	if (bTunnel)
	{
		OMX_COMPONENTTYPE * pCompOutput = m_pComponent;
		OMX_U32				nPortOutput	= m_nIndex;

		OMX_COMPONENTTYPE * pCompInput = pInputHole->GetParentBox ()->GetComponent ();
		OMX_U32				nPortInput = pInputHole->GetPortIndex ();

		if (pCompOutput->ComponentTunnelRequest != NULL && pCompInput->ComponentTunnelRequest != NULL)
		{
			OMX_TUNNELSETUPTYPE * pTunnelSetup = (OMX_TUNNELSETUPTYPE *)voOMXMemAlloc (sizeof (OMX_TUNNELSETUPTYPE));
			if (pTunnelSetup == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemSet (pTunnelSetup, 0, sizeof (OMX_TUNNELSETUPTYPE));

			errType = pCompOutput->ComponentTunnelRequest (pCompOutput, nPortOutput, pCompInput, nPortInput, pTunnelSetup);
			if (errType == OMX_ErrorNone)
			{
				errType = pCompInput->ComponentTunnelRequest (pCompInput, nPortInput, pCompOutput, nPortOutput, pTunnelSetup);
				if (errType != OMX_ErrorNone)
				{
					pCompOutput->ComponentTunnelRequest (pCompOutput, nPortOutput, NULL, 0, NULL);
				}
			}

			voOMXMemFree (pTunnelSetup);

			if (errType == OMX_ErrorNone)
			{
				m_pConnected = pInputHole;
				m_bTunnel = OMX_TRUE;

				VOLOGI ("Component %s connected %s with tunnel mode.", m_pParent->GetComponentName (), pInputHole->GetParentBox ()->GetComponentName ());
				return OMX_ErrorNone;
			}
		}
	}
	voCOMXBaseBox * pInputBox = pInputHole->GetParentBox ();
	OMX_U32			nInputIndex = pInputHole->GetPortIndex ();

	if (m_portType.eDomain == OMX_PortDomainAudio)
		errType = ConnectAudioPort (m_pParent->GetComponent (), m_nIndex, pInputBox->GetComponent (), nInputIndex);
	else if (m_portType.eDomain == OMX_PortDomainVideo)
		errType = ConnectVideoPort (m_pParent->GetComponent (), m_nIndex, pInputBox->GetComponent (), nInputIndex);

	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("Component %s connected %s was failed. \n",(char *)m_pParent->GetComponent (), pInputHole->GetParentBox ()->GetComponentName ());
		return errType;
	}

	if (m_portType.eDomain == OMX_PortDomainVideo && (!strcmp(pInputBox->GetComponentName(), "OMX.qcom.video.decoder.vc1") 
		|| !strcmp(m_pParent->GetComponentName(), "OMX.ST.VFM.VC1Dec") || !strcmp(m_pParent->GetComponentName(), "OMX.Freescale.std.video_decoder.wmv9.hw-based")
			))	// use QCM vc1 decoder
	{
		VO_U32 nFourcc = m_pParent->GetVideoFourCC();
		if(!(memcmp((char*)&nFourcc, (char*)"WVC1", 4)))	// WVC1
		{
			// we should add 00 00 01 0d in front of every video frame
			OMX_U8 FrameHeader[] = {0x0, 0x0, 0x1, 0xd};
			OMX_VO_BUFFER voBuffer;
			voBuffer.Buffer = FrameHeader;
			voBuffer.Size = 4;
			errType = m_pParent->SetParameter((OMX_INDEXTYPE)OMX_VO_IndexVideoFrameHeader, &voBuffer);
		}
	}

	m_pConnected = pInputHole;
	VOLOGI ("Component %s connected %s with Untunnel mode.", m_pParent->GetComponentName (), pInputHole->GetParentBox ()->GetComponentName ());

	FillPortType ();

	// Nvidia HW component output port default maybe not enabled in HoneyComb 3.1, so we do work round, East 20110621
	if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent) && OMX_DirOutput == m_portType.eDir && OMX_FALSE == m_portType.bEnabled)
	{
		errType = m_pParent->SendCommand (OMX_CommandPortEnable, m_nIndex, NULL);
		if (errType != OMX_ErrorNone)
			VOLOGE ("Component %s, Index %d SendCommand OMX_CommandPortEnable failed 0X%08X.", m_pParent->GetComponentName (), (int)m_nIndex, errType);
	}

	CreateBufferHead ();

	if (!m_pConnected->IsSupplier ())
		m_bSupplier = OMX_TRUE;

	return errType;
}

OMX_ERRORTYPE voCOMXBaseHole::SetConnected (voCOMXBaseHole* pOutputHole, OMX_BOOL bTunnel)
{
	if (m_portType.eDir == OMX_DirOutput)
		return OMX_ErrorPortsNotCompatible;

	m_bTunnel = bTunnel;
	m_pConnected = pOutputHole;

	FillPortType ();

	if (!IsTunnel ())
		CreateBufferHead ();

	return OMX_ErrorNone;
}

voCOMXBaseHole * voCOMXBaseHole::GetConnected (void)
{
	return m_pConnected;
}

OMX_ERRORTYPE voCOMXBaseHole::AllocBuffer (void)
{
	if (IsTunnel () || (!IsEnable () && m_nTrans != PORT_TRANS_DIS2ENA))
		return OMX_ErrorNone;

	if (m_ppBuffHead == NULL)
	{
		VOLOGE ("m_ppBuffHead == NULL.");
		return OMX_ErrorNotReady;
	}

	if (m_ppBuffHead[0] != NULL)
		return OMX_ErrorNone;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_S32 i = 0;
	int     nTimes = 0;
	if (m_bSupplier)
	{
		if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent) && OMX_DirOutput == m_portType.eDir && m_bUseNativeWindow)
		{
			NativeWindow_AllocBuffers();
			for (i = 0; i < m_nBuffCount; i++)
			{
				m_ppBuffHead[i]->pInputPortPrivate = m_ppBuffHead[i]->pPlatformPrivate;	// save pPlatformPrivate to pInputPortPrivate
				m_listBuffEmpty.Add(m_ppBuffHead[i]);
			}
		}
		else
		{
			for (i = 0; i < m_nBuffCount; i++)
			{
				nTimes = 0;
				if(VOBOX_HARDWARECOMPONENT_STE_DECODER == m_nHardwareComponent)
				{
					do
					{
						if(OMX_DirInput == m_portType.eDir && !strncmp(m_pParent->GetComponentName (), "OMX.ST.VFM.MPEG4Dec", 19))
							m_portType.nBufferSize = 1024*1024 ;
					 
						errType = m_pComponent->AllocateBuffer (m_pComponent, &m_ppBuffHead[i], m_nIndex, this, m_portType.nBufferSize);
						nTimes++;
						voOMXOS_Sleep(1);
					} while( errType == OMX_ErrorIncorrectStateOperation && nTimes < 10);
				}
				else
					errType = m_pComponent->AllocateBuffer (m_pComponent, &m_ppBuffHead[i], m_nIndex, this, m_portType.nBufferSize);
					
				if (errType != OMX_ErrorNone)
				{
					VOLOGE ("Component %s, Index %d AllocBuffer failed 0X%08X.", m_pParent->GetComponentName (), (int)m_nIndex, errType);
					return errType;
				}
				m_listBuffEmpty.Add(m_ppBuffHead[i]);
			}
		}
	}
	else
	{
		m_pConnected->AllocBuffer ();

		OMX_BUFFERHEADERTYPE **			ppConnectedBuffer = m_pConnected->GetBufferHead ();

		VOLOGI ("Connected buffer %08X", (unsigned int)ppConnectedBuffer);

		if (m_pConnected->GetBufferCount () != (OMX_U32)m_nBuffCount || ppConnectedBuffer == NULL)
		{
			if (m_ppBuffData == NULL)
			{
				m_ppBuffData = (OMX_U8**)voOMXMemAlloc (m_nBuffCount * sizeof (OMX_U8 *));
				if (m_ppBuffData == NULL)
					return OMX_ErrorInsufficientResources;
				for (i = 0; i < m_nBuffCount; i++)
				{
					m_ppBuffData[i] = (OMX_U8*)voOMXMemAlloc (m_portType.nBufferSize);
					if (m_ppBuffData[i] == NULL)
						return OMX_ErrorInsufficientResources;
				}
			}
		}

		for (i = 0; i < m_nBuffCount; i++)
		{
			if (m_pConnected->GetBufferCount () == (OMX_U32)m_nBuffCount && ppConnectedBuffer != NULL)
			{
				errType = m_pComponent->UseBuffer (m_pComponent, &m_ppBuffHead[i], m_nIndex, this,  m_portType.nBufferSize, ppConnectedBuffer[i]->pBuffer);
			}
			else
			{
				errType = m_pComponent->UseBuffer (m_pComponent, &m_ppBuffHead[i], m_nIndex, this, m_portType.nBufferSize, m_ppBuffData[i]);
			}

			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Component %s, Index %d UseBuffer failed 0X%08X.", m_pParent->GetComponentName (), (int)m_nIndex, errType);
				return errType;
			}

			m_listBuffEmpty.Add(m_ppBuffHead[i]);
		}
	}
	return errType;
}

OMX_ERRORTYPE voCOMXBaseHole::FreeBuffer (void)
{
	if (m_bTunnel)
		return OMX_ErrorNone;

	if (!IsEnable ())
		return OMX_ErrorNone;

	if (m_ppBuffHead == NULL)
		return OMX_ErrorNotReady;
		
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	for (OMX_S32 i = 0; i < m_nBuffCount; i++)
	{
		if(m_pParent->isHoneyComb() &&VO_IS_HW_DECODER(m_nHardwareComponent) && OMX_DirOutput == m_portType.eDir && m_bUseNativeWindow)
		{
			if (m_ppBuffHead[i] != NULL)
			{
				NativeWindow_FreeBuffer(m_ppBuffHead[i]->pAppPrivate);
				m_ppBuffHead[i]->pPlatformPrivate = m_ppBuffHead[i]->pInputPortPrivate;
			}
		}
		errType = m_pComponent->FreeBuffer (m_pComponent, m_nIndex, m_ppBuffHead[i]);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("Component %s, Index %d FreeBuffer failed 0X%08X.", m_pParent->GetComponentName (), (int)m_nIndex, errType);
			return errType;
		}

		m_ppBuffHead[i] = NULL;
	}

	m_listBuffEmpty.RemoveAll();

	
	// added by gtxia 2011-5-6 for fixing Pantech issue 5148. In fact we should clean up m_ppBuffHead after it's content has been modified. 
	if (m_ppBuffHead != NULL){
		voOMXMemFree (m_ppBuffHead);
		m_ppBuffHead = NULL;
	}
	return errType;
}

OMX_ERRORTYPE voCOMXBaseHole::Disable (void)
{
	m_nTrans = PORT_TRANS_ENA2DIS;

//	voCOMXAutoLock lock (&m_tmBuffer);

	OMX_ERRORTYPE errType = m_pParent->SendCommand (OMX_CommandPortDisable, m_nIndex, NULL);
	if (errType != OMX_ErrorNone)
		VOLOGE ("Component %s, Index %d SendCommand OMX_CommandPortDisable failed 0X%08X.", m_pParent->GetComponentName (), (int)m_nIndex, errType);
	// according openmax_il_spec_1_1_2.pdf (1.2 For each buffer returned, the IL client shall call FreeBuffer on the connected)
	// so we must wait each buffer returned, East 20110412

	int nTryTimes = 0;
	while(nTryTimes < 500)
	{
		if(GetBufferCount() == GetBufferEmpty())
			break;

		voOMXOS_Sleep(4);
		nTryTimes++;
	}
	if(nTryTimes >= 500)
	{
		VOLOGW ("Check buffer status was time out. %s, Index %d, Count %d, Empty %d, Try %d", m_pParent->GetComponentName(), (int)m_nIndex, 
				(int)GetBufferCount(), (int)GetBufferEmpty(), (int)nTryTimes);
	}
	return FreeBuffer ();
}

OMX_ERRORTYPE voCOMXBaseHole::Enable (void)
{
	m_nTrans = PORT_TRANS_DIS2ENA; 
	OMX_ERRORTYPE errType = m_pParent->SendCommand (OMX_CommandPortEnable, m_nIndex, NULL);
	if (errType != OMX_ErrorNone)
		VOLOGE ("Component %s, Index %d SendCommand OMX_CommandPortEnable failed 0X%08X.", m_pParent->GetComponentName (), (int)m_nIndex, errType);
	return AllocBuffer ();
}

OMX_ERRORTYPE voCOMXBaseHole::StartBuffer (void)
{
	if (m_bTunnel)
		return OMX_ErrorNone;

	if (m_pConnected == NULL)
		return OMX_ErrorNone;

	if (m_ppBuffHead == NULL)
	{
		VOLOGE ("Component %s, Index %d m_ppBuffHead == NULL.", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorNotImplemented;
	}

//	if (!IsSupplier () || m_portType.eDir != OMX_DirOutput)
	if (m_portType.eDir != OMX_DirOutput)
		return OMX_ErrorNone;

	voCOMXAutoLock lock (&m_tmBuffer);

	if (m_pParent->GetChain ()->IsFlushing ())
	{
		VOLOGE ("Component %s, Index %d IsFlushing.", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorNotImplemented;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	// get the head data first
	if (!m_bGetHeadData)
		errType = m_pComponent->FillThisBuffer (m_pComponent, m_ppBuffHead[0]);


	if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent) && OMX_DirOutput == m_portType.eDir && m_bUseNativeWindow)
	{
		// native window will show last frame when power on/off
		// for showing frame, if we not call queueBuffer, call lockBuffer to it will cause hang
		// showing frame always at the last of buffer list, so we will use it after queueBuffer
		// East guess and debug 20110903
		for (OMX_S32 i = 0; i < m_nBuffCount; i++)
			NativeWindow_CancelBuffer(m_ppBuffHead[m_nBuffCount - i - 1]->pAppPrivate);
  	}

	for (OMX_S32 i = 0; i < m_nBuffCount; i++)
	{
		/*
		if (m_nTrans == PORT_TRANS_ENA2DIS ||m_pParent->IsPortSettingsChanging())
			break;
		*/
		OMX_BUFFERHEADERTYPE* pBufferHeader = m_ppBuffHead[i];
		if (m_portType.eDir == OMX_DirOutput)
		{
			if(m_pParent->isHoneyComb())
			{
				if(VO_IS_HW_DECODER(m_nHardwareComponent) && m_bUseNativeWindow)
				{
					if(VOBOX_HARDWARECOMPONENT_SAMSUNG_DECODER == m_nHardwareComponent
					   || VOBOX_HARDWARECOMPONENT_FSL_DECODER == m_nHardwareComponent)
						
					{
						if(i >= m_nBuffCount - (OMX_S32)m_nMinUndequeuedBuffers)
							break;
					}
					else
					{
						// keep two native buffer in native window
						if(i >= m_nBuffCount - ((m_nMinUndequeuedBuffers < 2) ? 2 : (OMX_S32)m_nMinUndequeuedBuffers))
							break;
					}
					pBufferHeader = NativeWindow_PrepareBuffer();
					if(pBufferHeader)
						pBufferHeader->pPlatformPrivate = pBufferHeader->pInputPortPrivate;
				}
			}
			else
			{
				if(VOBOX_HARDWARECOMPONENT_QCOM_DECODER == m_nHardwareComponent)
				{
					if(i >= m_nBuffCount - 3)
					{
						VOLOGI ("VOBOX_HARDWARECOMPONENT_QCOM_DECODER i %d, buffer count %d", (int)i,(int)m_nBuffCount);
						break;
					}
				}
			}

			if(pBufferHeader)
			{
				pBufferHeader->nFlags = 0;
				errType = m_pComponent->FillThisBuffer (m_pComponent, pBufferHeader);
				if (errType != OMX_ErrorNone)
				{
					VOLOGE ("Component %s, Index %d FillThisBuffer was failed. 0X%08X.", m_pParent->GetComponentName (), (int)m_nIndex, errType);
					return errType;
				}
			}
		}
		else
		{
			errType = m_pComponent->EmptyThisBuffer (m_pComponent, pBufferHeader);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Component %s, Index %d EmptyThisBuffer was failed. 0X%08X.", m_pParent->GetComponentName (), (int)m_nIndex, errType);
				return errType;
			}
		}
		if(pBufferHeader)
			m_listBuffEmpty.Remove(pBufferHeader);
	}

	m_bEOS = OMX_FALSE;

	return errType;
}

OMX_ERRORTYPE voCOMXBaseHole::EmptyBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
//	voCOMXAutoLock lockComp (m_pParent->GetCompMutex ());
	m_tmBuffer.Lock ();
	m_listBuffEmpty.Add(pBuffer);
	m_tmBuffer.Unlock ();
	
	if (m_pParent->GetChain ()->IsSeeking ())
	{
		VOLOGR ("Component %s, Index %d Seeking", m_pParent->GetComponentName (), m_nIndex);
		return OMX_ErrorNone;
	}

	if (m_pParent->GetChain ()->IsFlushing ())
	{
		VOLOGR ("Component %s, Index %d IsFlushing", m_pParent->GetComponentName (), m_nIndex);
		return OMX_ErrorNone;
	}

	if (m_pParent->GetTransState () == COMP_TRANSSTATE_ExecuteToIdle ||
		m_pParent->GetTransState () == COMP_TRANSSTATE_PauseToIdle)
	{
		VOLOGR ("Component %s, Index %d State is wrong", m_pParent->GetComponentName (), m_nIndex);
		return OMX_ErrorNone;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_STATETYPE state = m_pParent->GetCompState ();
	if (state == OMX_StatePause || state == OMX_StateExecuting)
	{
		errType = m_pConnected->FillBuffer (pBuffer);
		if (errType != OMX_ErrorNone)
			VOLOGE ("Component %s, Index %d FillBuffer was failed. 0X%08X", m_pParent->GetComponentName (), (int)m_nIndex, errType);
	}

	return errType;
}

OMX_ERRORTYPE voCOMXBaseHole::FillBufferDone (OMX_BUFFERHEADERTYPE* pBuffer)
{
	pBuffer->nTimeStamp = pBuffer->nTimeStamp / m_nTimeDivision;

	// check 3d video for QCM h/w decoder
	if(m_nHardwareComponent == VOBOX_HARDWARECOMPONENT_QCOM_DECODER && pBuffer->nFilledLen>0)
		ProcessSEIData(pBuffer, pBuffer->nFlags);

	// check it is head data or not
	if (!m_bGetHeadData)
	{
		m_nHeadSize = pBuffer->nFilledLen;
		if (m_pHeadData == NULL)
			m_pHeadData = new OMX_U8[pBuffer->nAllocLen + m_nHeadSize];
		memcpy (m_pHeadData, pBuffer->pBuffer + pBuffer->nOffset, pBuffer->nFilledLen);

		m_bGetHeadData = OMX_TRUE;
		// VOLOGI ("Head size is %d", pBuffer->nFilledLen);
		return OMX_ErrorNone;
	}

	if (m_nHeadSize > 0)
	{
		memcpy (m_pHeadData + m_nHeadSize, pBuffer->pBuffer + pBuffer->nOffset, pBuffer->nFilledLen);
		memcpy (pBuffer->pBuffer + pBuffer->nOffset, m_pHeadData, pBuffer->nFilledLen + m_nHeadSize);
		pBuffer->nFilledLen += m_nHeadSize;
		m_nHeadSize = 0;
	}

//	voCOMXAutoLock lockComp (m_pParent->GetCompMutex ());

	{
		voCOMXAutoLock lock (&m_tmBuffer);
		m_listBuffEmpty.Add(pBuffer);

		if(m_pParent->IsPortSettingsChanging())
			return OMX_ErrorNone;

		if(m_bStartBuffer == OMX_FALSE)
		{
			m_bStartBuffer = OMX_TRUE;
			//some video stream no OMX_EventPortSettingsChanged, so we add here!
			m_pParent->SetPortSettingsChanged(OMX_TRUE);
		}
	}
	
	if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
		m_bEOS = OMX_TRUE;

	OMX_BOOL bForceReturn = OMX_FALSE;
	if (m_nTrans == PORT_TRANS_ENA2DIS)
	{
		VOLOGR ("Component %s, Index %d PORT_TRANS_ENA2DIS", m_pParent->GetComponentName (), m_nIndex);
		bForceReturn = OMX_TRUE;
	}
	else if (m_pParent->GetChain ()->IsSeeking ())
	{
		VOLOGR ("Component %s, Index %d Seeking", m_pParent->GetComponentName (), m_nIndex);
		bForceReturn = OMX_TRUE;
	}
	else if (m_pParent->GetChain ()->IsFlushing ())
	{
		VOLOGR ("Component %s, Index %d IsFlushing", m_pParent->GetComponentName (), m_nIndex);
		bForceReturn = OMX_TRUE;
	}
	else if (m_pParent->GetTransState () == COMP_TRANSSTATE_ExecuteToIdle || m_pParent->GetTransState () == COMP_TRANSSTATE_PauseToIdle)
	{
		VOLOGR ("Component %s, Index %d State is wrong", m_pParent->GetComponentName (), m_nIndex);
		bForceReturn = OMX_TRUE;
	}

	if(bForceReturn)
	{
		if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent) && m_bUseNativeWindow)
			NativeWindow_CancelBuffer(pBuffer->pAppPrivate);

		return OMX_ErrorNone;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_STATETYPE state = m_pParent->GetCompState ();
	if (state == OMX_StatePause || state == OMX_StateExecuting)
	{
		if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent) && m_bUseNativeWindow) // HoneyComb use pAppPrivate as GraphicBuffer, so set to pPlatformPrivate
			pBuffer->pPlatformPrivate = pBuffer->pAppPrivate;
		
		errType = m_pConnected->EmptyBuffer (pBuffer);
		if (errType != OMX_ErrorNone)
			VOLOGE ("Component %s, Index %d EmptyBuffer was failed. 0X%08X", m_pParent->GetComponentName (), (int)m_nIndex, errType);

		if (errType == OMX_ErrorOverflow)
			errType = FillBuffer (pBuffer);
	}

	return errType;
}

OMX_ERRORTYPE voCOMXBaseHole::FillBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
	if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent) && m_bUseNativeWindow)
		NativeWindow_CancelBuffer(pBuffer->pPlatformPrivate);
	
//	voCOMXAutoLock lockComp (m_pParent->GetCompMutex ());

	if (IsSupplier () && !m_bStartBuffer)
		return OMX_ErrorNone;

	if (m_portType.eDir != OMX_DirOutput)
	{
		VOLOGE ("Component %s, Index %d The dir is wrong", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorNone;
	}

	if (m_bEOS)
	{
		VOLOGR ("Component %s, Index %d It was EOF.", m_pParent->GetComponentName (), m_nIndex);
		return OMX_ErrorNone;
	}

	OMX_BUFFERHEADERTYPE* pFillBuff = NULL;

	if(!m_pParent->isHoneyComb() || !VO_IS_HW_DECODER(m_nHardwareComponent) || !m_bUseNativeWindow)
	{
//		OMX_S32 i = 0;
		OMX_U32	nTryTimes = 0;
		while (nTryTimes < 250)
		{
			m_tmBuffer.Lock ();

			if(VOBOX_HARDWARECOMPONENT_QCOM_DECODER == m_nHardwareComponent)
				pFillBuff = NULL;
			else
				pFillBuff = m_listBuffEmpty.GetByBuffer(pBuffer->pBuffer);

			if(pFillBuff == NULL)
				pFillBuff = m_listBuffEmpty.GetHead();

			m_tmBuffer.Unlock ();

			if (pFillBuff != NULL)
				break;

			voOMXOS_Sleep (2);
			nTryTimes++;

			if (m_pParent->GetChain ()->IsSeeking ())
			{
				VOLOGW ("Component %s, Index %d It is seeking.", m_pParent->GetComponentName (), (int)m_nIndex);
				return OMX_ErrorNone;
			}
		}
	}

	if (m_nTrans == PORT_TRANS_ENA2DIS)
	{
		VOLOGW ("Component %s, Index %d The state was wrong.", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorNone;
	}

	if (m_pParent->GetChain ()->IsSeeking ())
	{
		VOLOGW ("Component %s, Index %d It is seeking.", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorNone;
	}

	// make sure when flush, no FillThisBuffer and EmptyThisBuffer
	// fix QCM HoneyComb seek hang issue, East 20110903
	voCOMXAutoLock lockFlush (&m_tmFlush);
	if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent))
	{
		if (m_bFlush)
		{
			VOLOGW ("Component %s, Index %d It is flushing.", m_pParent->GetComponentName (), (int)m_nIndex);
			return OMX_ErrorNone;
		}

		if (m_bChanging2IdleState)
		{
			VOLOGW ("Component %s, Index %d It is Changeing to Idle State.", m_pParent->GetComponentName (), (int)m_nIndex);
			return OMX_ErrorNone;
		}
	}

	voCOMXAutoLock lock (&m_tmBuffer);
// 	if(VOBOX_HARDWARECOMPONENT_QCOM_DECODER == m_nHardwareComponent)
// 	{
// 		if(pFillBuff->pBuffer != pBuffer->pBuffer)
// 			VOLOGE("different addr!!input 0x%08X, addr 0x%08X, result 0x%08X, addr 0x%08X", pBuffer, pBuffer->pBuffer, pFillBuff, pFillBuff->pBuffer);
// 	}

	if( m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent) && m_bUseNativeWindow)
	{
		pFillBuff = NativeWindow_PrepareBuffer();
		if(pFillBuff)
			pFillBuff->pPlatformPrivate = pFillBuff->pInputPortPrivate;
		else if(VOBOX_HARDWARECOMPONENT_FSL_DECODER == m_nHardwareComponent)
			return OMX_ErrorNone;
	}

	if (pFillBuff != NULL)
	{
		pFillBuff->nFlags = 0;
		//Ferry@20110929.Do Remove before FillThisBuffer to avoid
		//probably issue like EmptyThisBuffer
		m_listBuffEmpty.Remove(pFillBuff);
		OMX_ERRORTYPE errType = m_pComponent->FillThisBuffer (m_pComponent, pFillBuff);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("Component %s, Index %d FillThisBuffer was failed. 0X%08X", m_pParent->GetComponentName (), (int)m_nIndex, errType);
			m_listBuffEmpty.Add(pFillBuff);
		}
	}
	else
	{
		VOLOGE ("Component %s, Index %d There was not empty buffer.", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorOverflow;
	}
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseHole::EmptyBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
//	voCOMXAutoLock lockComp (m_pParent->GetCompMutex ());

	// added by gtxia 2011-5-12
	pBuffer->nTimeStamp = pBuffer->nTimeStamp*m_nTimeDivision;
	
	if (m_pParent->GetOMXError() == OMX_TRUE) 
	{
		voOMXOS_Sleep (20);
		return OMX_ErrorNone;
	}

	if (m_ppBuffHead == NULL)
		return OMX_ErrorNotReady;

	if (m_portType.eDir != OMX_DirInput)
	{
		VOLOGE ("Component %s, Index %d The dir is wrong", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorPortsNotCompatible;
	}

	OMX_BUFFERHEADERTYPE* pEmptyBuff = NULL;
	//OMX_S32 i = 0;
	OMX_U32	nTryTimes = 0;
	while (nTryTimes < 250)
	{
		m_tmBuffer.Lock ();

		pEmptyBuff = m_listBuffEmpty.GetByBuffer(pBuffer->pBuffer);

		if(pEmptyBuff == NULL)
			pEmptyBuff = m_listBuffEmpty.GetHead();

		m_tmBuffer.Unlock ();

		if (pEmptyBuff != NULL)
			break;

		voOMXOS_Sleep (2);
		nTryTimes++;

		if (m_pParent->GetChain ()->IsSeeking ())
		{
			VOLOGW ("Component %s, Index %d Seeking", m_pParent->GetComponentName (), (int)m_nIndex);
			return OMX_ErrorNone;
		}
	}

	if (m_nTrans == PORT_TRANS_ENA2DIS)
	{
		VOLOGW ("Component %s, Index %d  m_nTrans == PORT_TRANS_ENA2DIS ", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorNone;
	}

	if (m_pParent->GetChain ()->IsSeeking ())
	{
		VOLOGW ("Component %s, Index %d  It is seeking ", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorNone;
	}

	if (!m_pConnected->IsStartBuffer ())
	{
		VOLOGW ("Component %s, Index %d  The connected was not start buffer ", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorNone;
	}

	// make sure when flush, no FillThisBuffer and EmptyThisBuffer
	// fix QCM HoneyComb seek hang issue, East 20110903
	voCOMXAutoLock lockFlush (&m_tmFlush);
	if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent))
	{
		if (m_bFlush)
		{
			VOLOGW ("Component %s, Index %d It is flushing.", m_pParent->GetComponentName (), (int)m_nIndex);
			return OMX_ErrorNone;
		}

		if (m_bChanging2IdleState)
		{
			VOLOGW ("Component %s, Index %d It is Changeing to Idle State.", m_pParent->GetComponentName (), (int)m_nIndex);
			return OMX_ErrorNone;
		}
	}

	voCOMXAutoLock lock (&m_tmBuffer);
	if (pEmptyBuff != NULL)
	{
		OMX_ERRORTYPE errType = CopyBuffer (pEmptyBuff, pBuffer);
		if (errType == OMX_ErrorNone)
		{
			if((pEmptyBuff->nFlags & OMX_BUFFERFLAG_CODECCONFIG) && 
			   (!strcmp(m_pParent->GetComponentName(), "OMX.qcom.video.decoder.vc1")||
				(!strcmp(m_pParent->GetComponentName(), "OMX.ST.VFM.VC1Dec")) ||
				(!strcmp(m_pParent->GetComponentName(), "OMX.Nvidia.vc1.decode")) ||
				(!strcmp(m_pParent->GetComponentName(), "OMX.Freescale.std.video_decoder.wmv9.hw-based"))
				   ))
			{
				// if it is WMV9
				if(pEmptyBuff->nFilledLen > sizeof(VO_BITMAPINFOHEADER))
				{
					VO_BITMAPINFOHEADER* pBmpHeader = (VO_BITMAPINFOHEADER *)pEmptyBuff->pBuffer;
					if(pBmpHeader->biSize == pEmptyBuff->nFilledLen && !memcmp((char*)&pBmpHeader->biCompression, (char*)"WMV3", 4))
					{
						if(pEmptyBuff->nAllocLen < sizeof(OMX_QCOMVC1WMV9_CONFIGDATA))
						{
							VOLOGW ("nAllocLen too small %d", (int)pEmptyBuff->nAllocLen);
						}
						else
						{
							// added by gtxia 2012-3-23 
							if(!strcmp(m_pParent->GetComponentName(), "OMX.Freescale.std.video_decoder.wmv9.hw-based"))
							{
								pEmptyBuff->nFilledLen = pBmpHeader->biSize - sizeof(VO_BITMAPINFOHEADER);
								memcpy(pEmptyBuff->pBuffer, (pEmptyBuff->pBuffer + sizeof(VO_BITMAPINFOHEADER)), pEmptyBuff->nFilledLen);
							}
							else
							{
								OMX_QCOMVC1WMV9_CONFIGDATA qcmConfigData;
								memset(&qcmConfigData, 0, sizeof(OMX_QCOMVC1WMV9_CONFIGDATA));
								qcmConfigData.start_code = 0xc5ffffff;
								qcmConfigData.head_data_size = 4;
								memcpy(qcmConfigData.head_data, pEmptyBuff->pBuffer + sizeof(VO_BITMAPINFOHEADER), qcmConfigData.head_data_size);
								qcmConfigData.height = pBmpHeader->biHeight;
								qcmConfigData.width = pBmpHeader->biWidth;
								qcmConfigData.c_data_size = 12;

								pEmptyBuff->nFilledLen = sizeof(OMX_QCOMVC1WMV9_CONFIGDATA);
								memcpy(pEmptyBuff->pBuffer, &qcmConfigData, sizeof(OMX_QCOMVC1WMV9_CONFIGDATA));
							}
						}
					}
				}
			}

#if defined(_DUMP_INPUT_DEC_DATA)
	if(VOBOX_HARDWARECOMPONENT_STE_DECODER == m_nHardwareComponent && OMX_DirInput == m_portType.eDir)
	{
		if(g_input != -1)
		{
			write(g_input, pEmptyBuff->pBuffer, pEmptyBuff->nFilledLen);
		}
		// for test VC1
		if(pEmptyBuff->nFilledLen == 22)
		{
			int nTemp = open("/data/local/head.dat", O_RDONLY);
			if(nTemp != -1)
			{
				read(nTemp,  pEmptyBuff->pBuffer,  pEmptyBuff->nFilledLen);
				close(nTemp);
			}
		
		}
	}
#endif	
			//Ferry@20110929.Do Remove before EmptyThisBuffer to avoid
			//Sync mode VideoSink CheckBoxStatus time out issue	
			m_listBuffEmpty.Remove(pEmptyBuff);
			errType = m_pComponent->EmptyThisBuffer (m_pComponent, pEmptyBuff);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Component %s, Index %d EmptyThisBuffer was failed. 0X%08X", m_pParent->GetComponentName (), (int)m_nIndex, errType);
				m_listBuffEmpty.Add(pEmptyBuff);
			}
		}
	}
	else
	{
		VOLOGE ("Component %s, Index %d There was not empty buffer.", m_pParent->GetComponentName (), (int)m_nIndex);
		return OMX_ErrorOverflow;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseHole::CopyBuffer (OMX_BUFFERHEADERTYPE * pDest, OMX_BUFFERHEADERTYPE * pSour)
{
	if (pDest == NULL || pSour == NULL)
		return OMX_ErrorBadParameter;

	pDest->nFilledLen = pSour->nFilledLen;
	pDest->hMarkTargetComponent = pSour->hMarkTargetComponent;
	pDest->pMarkData = pSour->pMarkData;

	pDest->nTimeStamp = pSour->nTimeStamp;
	pDest->nFlags = pSour->nFlags;
	pDest->pPlatformPrivate = pSour->pPlatformPrivate;

	if (!IsSupplier ())
	{
		pDest->pBuffer = pSour->pBuffer;
		pDest->nOffset = pSour->nOffset;
	}
	else
	{
		if (pDest->pBuffer == pSour->pBuffer)
		{
			pDest->nOffset = pSour->nOffset;
		}
		else
		{
			if (pDest->nAllocLen < pSour->nFilledLen)
				return OMX_ErrorInsufficientResources;

			voOMXMemCopy (pDest->pBuffer, pSour->pBuffer + pSour->nOffset, pSour->nFilledLen);
			pDest->nOffset = 0;
		}
	}

	return OMX_ErrorNone;
}

void voCOMXBaseHole::SetFlush (OMX_BOOL	bFlush)
{
	if (!IsEnable ())
		return;

	if (!bFlush && !m_bTunnel)
	{
		// wait for all buffer return
		OMX_U32 nTryTimes = 0;
		while (nTryTimes < 250)
		{
			m_tmBuffer.Lock ();
			OMX_S32 nBuffCount = m_listBuffEmpty.Count();
			m_tmBuffer.Unlock ();
			if(m_nBuffCount <= nBuffCount)
			{
				VOLOGR ("Component %s, Index %d finish flush", m_pParent->GetComponentName (), m_nIndex);
				break;
			}

			voOMXOS_Sleep (2);
			nTryTimes++;
		}
		if (nTryTimes >= 250)
			VOLOGE ("Component %s, Index %d was Time out. Buffer %d, Empty %d", m_pParent->GetComponentName (), (int)m_nIndex, 
					(int)m_nBuffCount, (int)m_listBuffEmpty.Count());
	}
	
	if(!bFlush)
		m_bStartBuffer = OMX_FALSE;

	if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent))
		m_tmFlush.Lock();
	m_bFlush = bFlush;
	if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent))
		m_tmFlush.Unlock();
}

void voCOMXBaseHole::SetChanging2IdleState(OMX_BOOL bChanging2IdleState)
{
	if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent))
		m_tmFlush.Lock();
	m_bChanging2IdleState = bChanging2IdleState;
	if(m_pParent->isHoneyComb() && VO_IS_HW_DECODER(m_nHardwareComponent))
		m_tmFlush.Unlock();
}

OMX_BOOL voCOMXBaseHole::GetFlush (void)
{
	return m_bFlush;
}

OMX_BOOL voCOMXBaseHole::IsStartBuffer (void)
{
	return m_bStartBuffer;
}

OMX_BOOL voCOMXBaseHole::IsInput (void)
{
	if (m_portType.eDir == OMX_DirInput)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}

OMX_BOOL voCOMXBaseHole::IsSupplier (void)
{
	return m_bSupplier;
}

OMX_BOOL voCOMXBaseHole::IsTunnel (void)
{
	return m_bTunnel;
}

OMX_BOOL voCOMXBaseHole::IsEnable (void)
{
	// sometimes, bEnabled written first, then eDir (QCM HW codec component)
	// so we wait all value finish writing, East 20110502
	voCOMXAutoLock lock(&m_tmPortType);
	return m_portType.bEnabled;
}

PORT_TRANS_STATUS voCOMXBaseHole::GetTrans (void)
{
	return m_nTrans;
}

OMX_ERRORTYPE voCOMXBaseHole::FillPortType (void)
{
	voCOMXAutoLock lock(&m_tmPortType);

	m_nTrans = PORT_TRANS_DONE;

	m_pParent->SetHeadder (&m_portType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	m_portType.nPortIndex = m_nIndex;

	OMX_ERRORTYPE errType = m_pParent->GetParameter (OMX_IndexParamPortDefinition, &m_portType);	
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("Component %s, Index %d GetParameter (OMX_IndexParamPortDefinition) was failed. 0X%08X ", m_pParent->GetComponentName (), 
				(int)m_nIndex, errType);
		return errType;
	}

	if (m_portType.eDir == OMX_DirOutput)
		m_bSupplier = OMX_TRUE;
	else
		m_bSupplier = OMX_FALSE;

	OMX_PARAM_BUFFERSUPPLIERTYPE bufSupplier;
	m_pParent->SetHeadder (&bufSupplier, sizeof (OMX_PARAM_BUFFERSUPPLIERTYPE));
	bufSupplier.nPortIndex = m_nIndex;
	errType = m_pParent->GetParameter (OMX_IndexParamCompBufferSupplier, &bufSupplier);
	if (errType == OMX_ErrorNone)
	{
		if (bufSupplier.eBufferSupplier == OMX_BufferSupplyInput && m_portType.eDir == OMX_DirInput)
			m_bSupplier = OMX_TRUE;
		else if (bufSupplier.eBufferSupplier == OMX_BufferSupplyOutput && m_portType.eDir == OMX_DirOutput)
			m_bSupplier = OMX_TRUE;
		else if (bufSupplier.eBufferSupplier == OMX_BufferSupplyUnspecified && (m_pConnected && !m_pConnected->IsSupplier ()))
			m_bSupplier = OMX_TRUE;
		else
			m_bSupplier = OMX_FALSE;
	}

	if(VOBOX_HARDWARECOMPONENT_QCOM_DECODER == m_nHardwareComponent 
	  || VOBOX_HARDWARECOMPONENT_QCOM_ENCODER == m_nHardwareComponent
	  || VOBOX_HARDWARECOMPONENT_STE_DECODER == m_nHardwareComponent
	  || VOBOX_HARDWARECOMPONENT_SEU_DECODER == m_nHardwareComponent
	  || VOBOX_HARDWARECOMPONENT_TI_DECODER == m_nHardwareComponent
	  || VOBOX_HARDWARECOMPONENT_SAMSUNG_DECODER == m_nHardwareComponent
	  || VOBOX_HARDWARECOMPONENT_FSL_DECODER == m_nHardwareComponent 
	  )
		m_bSupplier = OMX_TRUE;
	else if(!strncmp(m_pParent->GetComponentName (), "OMX.TI.720P.Decoder", 18))
		m_bSupplier = OMX_FALSE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseHole::CreateBufferHead (void)
{
	if (m_ppBuffHead != NULL)
		voOMXMemFree (m_ppBuffHead);

	m_nBuffCount = m_portType.nBufferCountActual;
	
	if (m_nBuffCount <= 0)
		return OMX_ErrorNone;
	m_ppBuffHead = (OMX_BUFFERHEADERTYPE **)voOMXMemAlloc (m_nBuffCount * sizeof (OMX_BUFFERHEADERTYPE *));
	if (m_ppBuffHead != NULL)
		voOMXMemSet (m_ppBuffHead, 0, m_nBuffCount * sizeof (OMX_BUFFERHEADERTYPE *));

	m_listBuffEmpty.Create(m_nBuffCount);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseHole::ConnectVideoPort (OMX_COMPONENTTYPE * pPrev, OMX_U32 nOutputPort,
												OMX_COMPONENTTYPE * pNext, OMX_U32 nInputPort)
{
	if (pPrev == NULL || pNext == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE		errType = OMX_ErrorNone;

	OMX_S8				szPrevName[128];
	OMX_S8				szNextName[128];
	OMX_VERSIONTYPE		verMainPrev;
	OMX_VERSIONTYPE		verMainNext;
	OMX_VERSIONTYPE		verSpec;
	OMX_UUIDTYPE		uuidType;

	memset (szPrevName, 0, 128);
	memset (szNextName, 0, 128);

	verMainPrev.s.nVersionMajor = SPEC_VERSION_MAJOR;
	verMainPrev.s.nVersionMinor = SPEC_VERSION_MINOR;
	verMainPrev.s.nRevision = SPEC_REVISION;
	verMainPrev.s.nStep = SPEC_STEP;
	verMainNext.s.nVersionMajor = SPEC_VERSION_MAJOR;
	verMainNext.s.nVersionMinor = SPEC_VERSION_MINOR;
	verMainNext.s.nRevision = SPEC_REVISION;
	verMainNext.s.nStep = SPEC_STEP;

	if (pPrev->GetComponentVersion != NULL)
	{
		errType = pPrev->GetComponentVersion (pPrev, (OMX_STRING) szPrevName, &verMainPrev, &verSpec, &uuidType);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("pPrev->GetComponentVersion was failed. 0X%08X", errType);
			return errType;
		}
		// added by gtxia for fixing the Freescale studpid bug 2012-3-8
		if(!strncmp((char*)szPrevName, "OMX.Freescale.std.",18))
		{
			verMainPrev.s.nVersionMajor = SPEC_VERSION_MAJOR;
			verMainPrev.s.nVersionMinor = SPEC_VERSION_MINOR;
		}
	}

	if (pNext->GetComponentVersion != NULL)
	{
		errType = pNext->GetComponentVersion (pNext, (OMX_STRING) szNextName, &verMainNext, &verSpec, &uuidType);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("pNext->GetComponentVersion was failed. 0X%08X", errType);
			return errType;
		}
		// added by gtxia for fixing the Freescale studpid bug 2012-3-8
		if(!strncmp((char*)szNextName, "OMX.Freescale.std.",18))
		{
			verMainNext.s.nVersionMajor = SPEC_VERSION_MAJOR;
			verMainNext.s.nVersionMinor = SPEC_VERSION_MINOR;
		}
	}
	OMX_PARAM_PORTDEFINITIONTYPE ptOutput;
	voOMXBase_SetHeader (&ptOutput, sizeof (OMX_PARAM_PORTDEFINITIONTYPE), &verMainPrev);
	ptOutput.nPortIndex = nOutputPort;
	errType = pPrev->GetParameter (pPrev, OMX_IndexParamPortDefinition, &ptOutput);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("pPrev->GetParameter (OMX_IndexParamPortDefinition) was failed. 0X%08X", errType);
		return errType;
	}

	// added by gtxia for deal with STE platform special requirements
	if(!strncmp((char*)szPrevName, "OMX.ST.VFM.",11))
	{
		int nWidth, nHeight;
		nWidth = ptOutput.format.video.nFrameWidth ;
		nHeight = ptOutput.format.video.nFrameHeight ;
		if(!strncmp((char*)szPrevName, "OMX.ST.VFM.H264Dec",18) && 
		   (nWidth == 1920) && (nHeight == 1080 || nHeight == 1088))
		{
			if(ptOutput.nBufferCountMin + 2 <= STE_H264DEC_MAX_1080P_BUFFERS)
				ptOutput.nBufferCountActual = ptOutput.nBufferCountMin + 2;
			else
				ptOutput.nBufferCountActual = ptOutput.nBufferCountMin;
		}
		else
		{
			ptOutput.nBufferCountActual += 2;
		}

		errType = pPrev->SetParameter (pPrev, OMX_IndexParamPortDefinition, &ptOutput);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("pPrev->SetParameter (OMX_IndexParamPortDefinition) was failed. 0X%08X", errType);
			return errType;
		}
	}

	if (ptOutput.eDomain != OMX_PortDomainVideo)
	{
		VOLOGE ("It was not video domain.");
		return OMX_ErrorPortsNotCompatible;
	}

	voCOMXBaseBox * pNextBox = new voCOMXBaseBox (NULL);
	OMX_STRING temp = (OMX_STRING)"TempComp";
	pNextBox->SetComponent (pNext, temp);
	pNextBox->CreateHoles ();
	
	const char* pTIComp = "OMX.TI.DUCATI1.VIDEO.DECODER" ;
	if (pNextBox->GetHoleCount () == 2 && nInputPort == 0 && strncmp((char*)szNextName, "OMX.ST.VFM.",11) && strncmp((char*)szNextName, pTIComp,strlen(pTIComp)))
	{
		voCOMXBaseHole * pOutHole = pNextBox->GetHole (1);
		if (pOutHole != NULL)
		{
			if (pOutHole->IsEnable () && !pOutHole->IsInput ())
			{
				OMX_PARAM_PORTDEFINITIONTYPE ptVideoOutput;
				voOMXBase_SetHeader (&ptVideoOutput, sizeof (OMX_PARAM_PORTDEFINITIONTYPE), &verMainNext);
				ptVideoOutput.nPortIndex = 1;
				errType = pNext->GetParameter (pNext, OMX_IndexParamPortDefinition, &ptVideoOutput);
				if (errType == OMX_ErrorNone)
				{
					ptVideoOutput.format.video.nFrameWidth = ptOutput.format.video.nFrameWidth;
					ptVideoOutput.format.video.nFrameHeight = ptOutput.format.video.nFrameHeight;
					ptVideoOutput.format.video.nStride = ptOutput.format.video.nStride;
					if (ptVideoOutput.format.video.eCompressionFormat < OMX_VIDEO_CodingMPEG2 || 
						ptVideoOutput.format.video.eCompressionFormat >= OMX_VIDEO_CodingKhronosExtensions)
					{
						// modified by daivd to keep the primary value of buffer size
						int ndefsize = ptVideoOutput.nBufferSize;
						ptVideoOutput.nBufferSize = (OMX_U32)ptVideoOutput.format.video.nStride * ptVideoOutput.format.video.nFrameHeight;
						if (ptVideoOutput.nBufferSize < (OMX_U32)ndefsize)
							ptVideoOutput.nBufferSize = ndefsize;
					}

					errType = pNext->SetParameter (pNext, OMX_IndexParamPortDefinition, &ptVideoOutput);
					if (errType != OMX_ErrorNone)
					{
						VOLOGW ("pNext->SetParameter (OMX_IndexParamPortDefinition) was failed. 0X%08X", errType);
					}
				}
			}
		}
	}
	
	delete pNextBox;
	
	OMX_PARAM_PORTDEFINITIONTYPE ptInput;
	voOMXBase_SetHeader (&ptInput, sizeof (OMX_PARAM_PORTDEFINITIONTYPE), &verMainNext);
	ptInput.nPortIndex = nInputPort;
	errType = pNext->GetParameter (pNext, OMX_IndexParamPortDefinition, &ptInput);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("pNext->GetParameter (OMX_IndexParamPortDefinition) was failed. 0X%08X", errType);
		return errType;
	}

	int nOldBuffCount = ptInput.nBufferCountActual;

	if (strstr ((char *)szNextName, "VisualOn") != NULL)
		ptInput.nBufferCountActual = ptOutput.nBufferCountActual;
	
	// comment by gtxia 2011-5-6
	// using the uniform code, because different vendor has differnet implement. so we skip the the char*
	memcpy (((char*)&ptInput.format.video)+sizeof(char*), ((char*)(&ptOutput.format.video))+sizeof(char*), (sizeof (OMX_VIDEO_PORTDEFINITIONTYPE)-sizeof(char*)));

	errType = pNext->SetParameter (pNext, OMX_IndexParamPortDefinition, &ptInput);
	if (errType != OMX_ErrorNone)
	{
		ptInput.nBufferCountActual = nOldBuffCount;
		errType = pNext->SetParameter (pNext, OMX_IndexParamPortDefinition, &ptInput);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("pNext->SetParameter (OMX_IndexParamPortDefinition) was failed. 0X%08X", errType);
			return errType;
		}
	}

	OMX_VIDEO_PARAM_PORTFORMATTYPE vfOutput;
	voOMXBase_SetHeader (&vfOutput, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE), &verMainPrev);
	
	vfOutput.nPortIndex = nOutputPort;
	errType = pPrev->GetParameter (pPrev, OMX_IndexParamVideoPortFormat, &vfOutput);
	
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("pPrev->GetParameter (OMX_IndexParamVideoPortFormat) was failed. 0X%08X", errType);
		return errType;
	}
	
	voOMXMemCopy (&vfOutput.nVersion, &verMainNext, sizeof (OMX_VERSIONTYPE));
	vfOutput.nPortIndex = nInputPort;

	// comment by gtxia 2012-1-8
	// because SAMSUNG hardware decoder' color's format is OMX_COLOR_FormatYUV420Planar so I have to fake some other value to deal this case
	// thus can not conflit with our own decoder
	const char*  pSamName = "OMX.SEC." ;
	const char*  pFslName = "OMX.Freescale.std." ;
	if(!strncmp((char*)szPrevName, pSamName, strlen(pSamName)) || !strncmp((char*)szPrevName, pFslName, strlen(pFslName)))
	{
		if(vfOutput.eColorFormat == OMX_COLOR_FormatYUV420Planar && m_pParent->isHoneyComb())
			vfOutput.eColorFormat = (OMX_COLOR_FORMATTYPE)OMX_COLOR_Format_SamSuangFK;
	}
	errType = pNext->SetParameter (pNext, OMX_IndexParamVideoPortFormat, &vfOutput);

	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("pNext->SetParameter (OMX_IndexParamVideoPortFormat) was failed. 0X%08X", errType);
		return errType;
	}

	OMX_U32 uDataSize = 512;
	OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *)voOMXMemAlloc (sizeof (OMX_OTHER_EXTRADATATYPE) + uDataSize);
	if (pExtData == NULL)
		return OMX_ErrorInsufficientResources;

	voOMXBase_SetHeader (pExtData, sizeof (OMX_OTHER_EXTRADATATYPE) + uDataSize, &verMainPrev);
	pExtData->nPortIndex = nOutputPort;
	pExtData->nDataSize = uDataSize;
	errType = pPrev->GetParameter (pPrev, OMX_IndexParamCommonExtraQuantData, pExtData);
	
	if (errType == OMX_ErrorNone && pExtData->nDataSize > 0)
	{
		voOMXMemCopy (&pExtData->nVersion, &verMainNext, sizeof (OMX_VERSIONTYPE));
		pExtData->nPortIndex = nInputPort;
		errType = pNext->SetParameter (pNext, OMX_IndexParamCommonExtraQuantData, pExtData);
		if (errType != OMX_ErrorNone)
		{
			VOLOGW ("pNext->SetParameter (OMX_IndexParamCommonExtraQuantData) was failed. 0X%08X", errType);
		}
	}
	voOMXMemFree (pExtData);
	// East add profile level support to VisualOn component, 2011/03/29
/*	OMX_VIDEO_PARAM_PROFILELEVELTYPE profileLevelOut;
	voOMXBase_SetHeader(&profileLevelOut, sizeof(OMX_VIDEO_PARAM_PROFILELEVELTYPE), &verMainPrev);
	profileLevelOut.nPortIndex = nOutputPort;
	errType = pPrev->GetParameter(pPrev, OMX_IndexParamVideoProfileLevelCurrent, &profileLevelOut);
	if(OMX_ErrorNone == errType)
	{
		OMX_VIDEO_PARAM_PROFILELEVELTYPE profileLevelIn;
		voOMXBase_SetHeader(&profileLevelIn, sizeof(OMX_VIDEO_PARAM_PROFILELEVELTYPE), &verMainNext);
		profileLevelIn.nPortIndex = nInputPort;
		OMX_U32 i = 0;
		for(i = 0; i < 3000; i++)	// avoid some component just return OMX_ErrorNone simply, 3000 is enough
		{
			profileLevelIn.nProfileIndex = i;
			errType = pNext->GetParameter(pNext, OMX_IndexParamVideoProfileLevelQuerySupported, &profileLevelIn);
			if(OMX_ErrorNone != errType)
			{
				VOLOGW("pNext->GetParameter(OMX_IndexParamVideoProfileLevelQuerySupported) was failed. 0X%08X", errType);
				break;
			}

			if(profileLevelOut.eProfile == profileLevelIn.eProfile && profileLevelOut.eLevel == profileLevelIn.eLevel)
			{
				VOLOGR("Out profile %d, level %d, In profile %d, level %d, pair successfully", profileLevelOut.eProfile, profileLevelOut.eLevel, profileLevelIn.eProfile, profileLevelIn.eLevel);
				break;
			}
		}
		if(i >= 3000)
		{
			VOLOGW("pNext implement OMX_IndexParamVideoProfileLevelQuerySupported illegally");
		}
	}*/
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseHole::ConnectAudioPort (OMX_COMPONENTTYPE * pPrev, OMX_U32 nOutputPort,
												OMX_COMPONENTTYPE * pNext, OMX_U32 nInputPort)
{
	if (pPrev == NULL || pNext == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE		errType = OMX_ErrorNone;
	OMX_S8				szName[128];
	OMX_VERSIONTYPE		verMainPrev;
	OMX_VERSIONTYPE		verMainNext;
	OMX_VERSIONTYPE		verSpec;
	OMX_UUIDTYPE		uuidType;

	verMainPrev.s.nVersionMajor = SPEC_VERSION_MAJOR;
	verMainPrev.s.nVersionMinor = SPEC_VERSION_MINOR;
	verMainPrev.s.nRevision = SPEC_REVISION;
	verMainPrev.s.nStep = SPEC_STEP;
	verMainNext.s.nVersionMajor = SPEC_VERSION_MAJOR;
	verMainNext.s.nVersionMinor = SPEC_VERSION_MINOR;
	verMainNext.s.nRevision = SPEC_REVISION;
	verMainNext.s.nStep = SPEC_STEP;

	if (pPrev->GetComponentVersion != NULL)
	{
		errType = pPrev->GetComponentVersion (pPrev, (OMX_STRING) szName, &verMainPrev, &verSpec, &uuidType);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("pPrev->GetComponentVersion was failed. 0X%08X", errType);
			return errType;
		}
	}

	if (pNext->GetComponentVersion != NULL)
	{
		errType = pNext->GetComponentVersion (pNext, (OMX_STRING) szName, &verMainNext, &verSpec, &uuidType);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("pNext->GetComponentVersion was failed. 0X%08X", errType);
			return errType;
		}
	}

	OMX_PARAM_PORTDEFINITIONTYPE portType;
	voOMXBase_SetHeader (&portType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE), &verMainPrev);
	portType.nPortIndex = nOutputPort;
	errType = pPrev->GetParameter (pPrev, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("pPrev->GetParameter (OMX_IndexParamPortDefinition) was failed. 0X%08X", errType);
		return errType;
	}

	if (portType.eDomain != OMX_PortDomainAudio)
	{
		VOLOGE ("It was not audio domain.");
		return OMX_ErrorPortsNotCompatible;
	}

	voOMXMemCopy (&portType.nVersion, &verMainNext, sizeof (OMX_VERSIONTYPE));
	portType.nPortIndex = nInputPort;
	portType.eDir = OMX_DirInput;
	errType = pNext->SetParameter (pNext, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("pNext->SetParameter (OMX_IndexParamPortDefinition) was failed. 0X%08X", errType);
		return errType;
	}

	OMX_AUDIO_PARAM_PORTFORMATTYPE tpAudioFormat;
	voOMXBase_SetHeader (&tpAudioFormat, sizeof (OMX_AUDIO_PARAM_PORTFORMATTYPE), &verMainPrev);
	tpAudioFormat.nPortIndex = nOutputPort;
	errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioPortFormat, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("pPrev->GetParameter (OMX_IndexParamAudioPortFormat) was failed. 0X%08X", errType);
		return errType;
	}

	voOMXMemCopy (&tpAudioFormat.nVersion, &verMainNext, sizeof (OMX_VERSIONTYPE));
	tpAudioFormat.nPortIndex = nInputPort;
	errType = pNext->SetParameter (pNext, OMX_IndexParamAudioPortFormat, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("pNext->SetParameter (OMX_IndexParamAudioPortFormat) was failed. 0X%08X", errType);
		return errType;
	}

	// try to set output parameter. No port setting changing event
	voCOMXBaseBox * pNextBox = new voCOMXBaseBox (NULL);
	OMX_STRING temp = (OMX_STRING)"TempComp";
	pNextBox->SetComponent (pNext, temp);
	pNextBox->CreateHoles ();
	if (pNextBox->GetHoleCount () == 2 && nInputPort == 0)
	{
		voCOMXBaseHole * pOutHole = pNextBox->GetHole (1);
		if (pOutHole != NULL)
		{
			if (pOutHole->IsEnable () && !pOutHole->IsInput ())
			{
				OMX_AUDIO_PARAM_PCMMODETYPE tpPCMFormat;
				voOMXBase_SetHeader (&tpPCMFormat, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
				tpPCMFormat.nPortIndex = nOutputPort;
				errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioPcm, &tpPCMFormat);
				if (errType == OMX_ErrorNone)
				{
					OMX_AUDIO_PARAM_PCMMODETYPE ptAudioOutput;
					voOMXBase_SetHeader (&ptAudioOutput, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE), &verMainNext);
					ptAudioOutput.nPortIndex = 1;
					errType = pNext->GetParameter (pNext, OMX_IndexParamAudioPcm, &ptAudioOutput);
					if (errType == OMX_ErrorNone)
					{
						ptAudioOutput.nChannels = tpPCMFormat.nChannels;
						ptAudioOutput.nBitPerSample = tpPCMFormat.nBitPerSample;
						ptAudioOutput.nSamplingRate = tpPCMFormat.nSamplingRate;

						errType = pNext->SetParameter (pNext, OMX_IndexParamAudioPcm, &ptAudioOutput);
						if (errType != OMX_ErrorNone)
						{
							VOLOGW ("pNext->SetParameter (OMX_IndexParamAudioPcm) was failed. 0X%08X", errType);
						}
					}
				}
			}
		}
	}
	delete pNextBox;

	if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingPCM)
	{
		OMX_AUDIO_PARAM_PCMMODETYPE tpPCMFormat;
		voOMXBase_SetHeader (&tpPCMFormat, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
		tpPCMFormat.nPortIndex = nOutputPort;
		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioPcm, &tpPCMFormat);
		if (errType != OMX_ErrorNone)
		{
			VOLOGE ("pPrev->GetParameter (OMX_IndexParamAudioPcm) was failed. 0X%08X", errType);
			return errType;
		}

		voOMXMemCopy (&tpPCMFormat.nVersion, &verMainNext, sizeof (OMX_VERSIONTYPE));
		tpPCMFormat.nPortIndex = nInputPort;
		errType = pNext->SetParameter (pNext, OMX_IndexParamAudioPcm, &tpPCMFormat);
		if (errType != OMX_ErrorNone)
		{
			voOMXBase_SetHeader (&tpPCMFormat, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE), &verMainNext);
			tpPCMFormat.nPortIndex = nInputPort;
			errType = pNext->GetParameter (pNext, OMX_IndexParamAudioPcm, &tpPCMFormat);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("pNext->GetParameter (OMX_IndexParamAudioPcm) was failed. 0X%08X", errType);
				return errType;
			}

			voOMXMemCopy (&tpPCMFormat.nVersion, &verMainNext, sizeof (OMX_VERSIONTYPE));
			tpPCMFormat.nPortIndex = nOutputPort;
			errType = pPrev->SetParameter (pPrev, OMX_IndexParamAudioPcm, &tpPCMFormat);
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("pPrev->SetParameter (OMX_IndexParamAudioPcm) was failed. 0X%08X", errType);
				return errType;
			}
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingMP3)
	{
		OMX_AUDIO_PARAM_MP3TYPE tpMP3Format;
		voOMXBase_SetHeader (&tpMP3Format, sizeof (OMX_AUDIO_PARAM_MP3TYPE));
		tpMP3Format.nPortIndex = nOutputPort;
		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioMp3, &tpMP3Format);
		if (errType == OMX_ErrorNone)
		{
			voOMXMemCopy (&tpMP3Format.nVersion, &verMainNext, sizeof (OMX_VERSIONTYPE));
			tpMP3Format.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioMp3, &tpMP3Format);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingRA)
	{
		OMX_AUDIO_PARAM_RATYPE tpRAFormat;
		voOMXBase_SetHeader (&tpRAFormat, sizeof (OMX_AUDIO_PARAM_RATYPE));
		tpRAFormat.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioRa, &tpRAFormat);
		if (errType == OMX_ErrorNone)
		{
			tpRAFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioRa, &tpRAFormat);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingAAC)
	{
		OMX_AUDIO_PARAM_AACPROFILETYPE tpAACFormat;
		voOMXBase_SetHeader (&tpAACFormat, sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));
		tpAACFormat.nPortIndex = nOutputPort;
		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioAac, &tpAACFormat);
		if (errType == OMX_ErrorNone)
		{
			voOMXMemCopy (&tpAACFormat.nVersion, &verMainNext, sizeof (OMX_VERSIONTYPE));
			tpAACFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioAac, &tpAACFormat);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingWMA)
	{
		OMX_AUDIO_PARAM_WMATYPE tpWMAFormat;
		voOMXBase_SetHeader (&tpWMAFormat, sizeof (OMX_AUDIO_PARAM_WMATYPE));
		tpWMAFormat.nPortIndex = nOutputPort;
		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioWma, &tpWMAFormat);
		if (errType != OMX_ErrorNone)
		{
			voOMXMemCopy (&tpWMAFormat.nVersion, &verMainNext, sizeof (OMX_VERSIONTYPE));
			tpWMAFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioWma, &tpWMAFormat);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingAMR)
	{
		OMX_AUDIO_PARAM_AMRTYPE tpAMRFormat;
		voOMXBase_SetHeader (&tpAMRFormat, sizeof (OMX_AUDIO_PARAM_AMRTYPE));
		tpAMRFormat.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioAmr, &tpAMRFormat);
		if (errType == OMX_ErrorNone)
		{
			tpAMRFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioAmr, &tpAMRFormat);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingADPCM)
	{
		OMX_AUDIO_PARAM_ADPCMTYPE tpADPCMFormat;
		voOMXBase_SetHeader (&tpADPCMFormat, sizeof (OMX_AUDIO_PARAM_ADPCMTYPE));
		tpADPCMFormat.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioAdpcm, &tpADPCMFormat);
		if (errType == OMX_ErrorNone)
		{
			tpADPCMFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioAdpcm, &tpADPCMFormat);
		}
	}
	else if(tpAudioFormat.eEncoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingDTS)
	{
		OMX_VO_AUDIO_PARAM_DTSTYPE tpDTSFormat;

		errType = pPrev->GetParameter (pPrev, (OMX_INDEXTYPE)OMX_VO_IndexAudioParamDTS, &tpDTSFormat);
		if (errType == OMX_ErrorNone)
		{
			tpDTSFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, (OMX_INDEXTYPE)OMX_VO_IndexAudioParamDTS, &tpDTSFormat);
		}
	}
	else if(tpAudioFormat.eEncoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAC3)
	{
		OMX_VO_AUDIO_PARAM_AC3TYPE tpAC3Format;

		errType = pPrev->GetParameter (pPrev, (OMX_INDEXTYPE)OMX_VO_IndexAudioParamAC3, &tpAC3Format);
		if (errType == OMX_ErrorNone)
		{
			tpAC3Format.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, (OMX_INDEXTYPE)OMX_VO_IndexAudioParamAC3, &tpAC3Format);
		}
	}

	OMX_U32 uDataSize = 512;
	OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *)voOMXMemAlloc (sizeof (OMX_OTHER_EXTRADATATYPE) + uDataSize);
	if (pExtData == NULL)
		return OMX_ErrorInsufficientResources;

	voOMXBase_SetHeader (pExtData, sizeof (OMX_OTHER_EXTRADATATYPE) + uDataSize);
	pExtData->nPortIndex = nOutputPort;
	pExtData->nDataSize = uDataSize;

	errType = pPrev->GetParameter (pPrev, OMX_IndexParamCommonExtraQuantData, pExtData);
	if (errType == OMX_ErrorNone && pExtData->nDataSize > 0)
	{
		voOMXMemCopy (&pExtData->nVersion, &verMainNext, sizeof (OMX_VERSIONTYPE));
		pExtData->nPortIndex = nInputPort;
		errType = pNext->SetParameter (pNext, OMX_IndexParamCommonExtraQuantData, pExtData);
		if (errType != OMX_ErrorNone)
		{
			VOLOGW ("pNext->SetParameter (OMX_IndexParamCommonExtraQuantData) was failed. 0X%08X", errType);
		}
	}
	voOMXMemFree (pExtData);

	return OMX_ErrorNone;
}


OMX_S32 GUseGraphicBuffer(OMX_PTR pUserData, OMX_PTR pUseAndroidNativeBufferParams)
{
	voCOMXBaseHole* pHole = (voCOMXBaseHole*)pUserData;
	return pHole->UseGraphicBuffer(pUseAndroidNativeBufferParams);
}

OMX_S32 voCOMXBaseHole::UseGraphicBuffer(OMX_PTR pUseAndroidNativeBufferParams)
{
	OMX_INDEXTYPE nIndex;
	if(mUANBVer == 1)
	{
		OMX_ERRORTYPE errType = m_pComponent->GetExtensionIndex(m_pComponent, const_cast<OMX_STRING>("OMX.google.android.index.useAndroidNativeBuffer"), &nIndex);
		if(errType != OMX_ErrorNone)
		{
			VOLOGE("OMX_GetExtensionIndex failed 0x%08X", errType);
			return -1;
		}

	
		errType = m_pComponent->SetParameter(m_pComponent, nIndex, pUseAndroidNativeBufferParams);
		if(errType != OMX_ErrorNone)
		{
			VOLOGE("OMX_UseAndroidNativeBuffer failed with error 0x%08x", errType);
			return -1;
		}

	}
	else if(2 == mUANBVer)
	{
		struct VO_NativeParams
		{
			OMX_BUFFERHEADERTYPE** ppHeader;
			void*                  pPrivate;
			unsigned char*         pBuffers;
		};
		VO_NativeParams* pParams = (VO_NativeParams*)(pUseAndroidNativeBufferParams);
		OMX_ERRORTYPE errType = m_pComponent->UseBuffer(m_pComponent, pParams->ppHeader, m_nIndex, pParams->pPrivate, m_portType.nBufferSize, pParams->pBuffers);
		VOLOGE("TI_TI errType = %x *pParams->ppHeader = %p\n", errType, *pParams->ppHeader);
		if(errType != OMX_ErrorNone)
		{
			VOLOGE("OMX_UseAndroidNativeBuffer failed with error 0x%08x", errType);
			return -1;
		}
	}
	return 0;
}

OMX_S32 voCOMXBaseHole::NativeWindow_AllocBuffers()
{
	VOME_NATIVEWINDOWALLOCBUFFER sAllocBuffer;
	sAllocBuffer.nPortIndex = m_nIndex;
	sAllocBuffer.nWidth = m_portType.format.video.nFrameWidth;
	sAllocBuffer.nHeight = m_portType.format.video.nFrameHeight;
	sAllocBuffer.nColor = m_portType.format.video.eColorFormat;
	sAllocBuffer.ppBufferHeader = (OMX_PTR*)m_ppBuffHead;
	sAllocBuffer.nBufferCount = m_portType.nBufferCountActual;
	sAllocBuffer.fUseGraphicBuffer = GUseGraphicBuffer;
	sAllocBuffer.pUserData = this;
	sAllocBuffer.nBufferSize = m_portType.nBufferSize;
	getGraphicBufferUsage(sAllocBuffer.mUsages);
	sAllocBuffer.mUANBVer = mUANBVer;

	VOME_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOME_NATIVEWINDOW_AllocBuffers;
	sCommand.pParam1 = &sAllocBuffer;
	
	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = m_pCallBack(m_pUserData, VOME_CID_PLAY_NATIVEWINDOWCOMMAND, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOME_NATIVEWINDOW_AllocBuffers callback error.");
	return nRet;
}

OMX_PTR voCOMXBaseHole::NativeWindow_DequeueBuffer()
{
	VOME_NATIVEWINDOWBUFFERHEADERS sBufferHeaders;
	sBufferHeaders.nBufferCount = m_nBuffCount;
	sBufferHeaders.ppBufferHeader = (OMX_PTR*)m_ppBuffHead;

	OMX_PTR pNativeBuffer = NULL;
	VOME_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOME_NATIVEWINDOW_DequeueBuffer;
	sCommand.pParam1 = &pNativeBuffer;
	sCommand.pParam2 = &sBufferHeaders;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = m_pCallBack(m_pUserData, VOME_CID_PLAY_NATIVEWINDOWCOMMAND, &sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOME_NATIVEWINDOW_DequeueBuffer callback error.");

	VOLOGR ("VOME_NATIVEWINDOW_DequeueBuffer 0x%08X", pNativeBuffer);

	return pNativeBuffer;
}

OMX_S32 voCOMXBaseHole::NativeWindow_CancelBuffer(OMX_PTR pNativeBuffer)
{
	VOME_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOME_NATIVEWINDOW_CancelBuffer;
	sCommand.pParam1 = pNativeBuffer;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = m_pCallBack(m_pUserData, VOME_CID_PLAY_NATIVEWINDOWCOMMAND, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOME_NATIVEWINDOW_CancelBuffer callback error.");

	return nRet;
}

OMX_S32 voCOMXBaseHole::NativeWindow_QueueBuffer(OMX_PTR pNativeBuffer)
{
	VOME_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOME_NATIVEWINDOW_QueueBuffer;
	sCommand.pParam1 = pNativeBuffer;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = m_pCallBack(m_pUserData, VOME_CID_PLAY_NATIVEWINDOWCOMMAND, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOME_NATIVEWINDOW_QueueBuffer callback error.");

	return nRet;
}

OMX_S32 voCOMXBaseHole::NativeWindow_LockBuffer(OMX_PTR pNativeBuffer)
{
	VOME_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOME_NATIVEWINDOW_LockBuffer;
	sCommand.pParam1 = pNativeBuffer;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = m_pCallBack(m_pUserData, VOME_CID_PLAY_NATIVEWINDOWCOMMAND, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOME_NATIVEWINDOW_LockBuffer callback error.");

	return nRet;
}

OMX_S32 voCOMXBaseHole::NativeWindow_SetCrop(OMX_S32 nLeft, OMX_S32 nTop, OMX_S32 nRight, OMX_S32 nBottom)
{
	VOME_NATIVEWINDOWCROP sCrop;
	sCrop.nLeft = nLeft;
	sCrop.nTop = nTop;
	sCrop.nRight = nRight;
	sCrop.nBottom = nBottom;

	VOME_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOME_NATIVEWINDOW_SetCrop;
	sCommand.pParam1 = &sCrop;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = m_pCallBack(m_pUserData, VOME_CID_PLAY_NATIVEWINDOWCOMMAND, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOME_NATIVEWINDOW_SetCrop callback error.");
	return nRet;
}

OMX_S32 voCOMXBaseHole::NativeWindow_FreeBuffer(OMX_PTR pNativeBuffer)
{
	VOME_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOME_NATIVEWINDOW_FreeBuffer;
	sCommand.pParam1 = pNativeBuffer;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = m_pCallBack(m_pUserData, VOME_CID_PLAY_NATIVEWINDOWCOMMAND, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOME_NATIVEWINDOW_FreeBuffer callback error.");

	return nRet;
}

OMX_BOOL voCOMXBaseHole::NativeWindow_IsBufferDequeued(OMX_PTR pNativeBuffer)
{
	OMX_U32 nDequeued = 0;

	VOME_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOME_NATIVEWINDOW_IsBufferDequeued;
	sCommand.pParam1 = pNativeBuffer;
	sCommand.pParam2 = &nDequeued;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = m_pCallBack(m_pUserData, VOME_CID_PLAY_NATIVEWINDOWCOMMAND, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
		VOLOGE("VOME_NATIVEWINDOW_IsBufferDequeued callback error.");

	VOLOGR("pNativeBuffer 0x%08X isDequeued %d", nDequeued);
	return (nDequeued == 0) ? OMX_FALSE : OMX_TRUE;
}

OMX_BUFFERHEADERTYPE* voCOMXBaseHole::NativeWindow_PrepareBuffer()
{
	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);

	OMX_PTR pNativeBuffer = NativeWindow_DequeueBuffer();
	if(pNativeBuffer == NULL)
		VOLOGE("Can not dequeue Buffer");

	for (OMX_S32 i = 0; i < m_nBuffCount; i++)
	{
		if (m_ppBuffHead[i] && m_ppBuffHead[i]->pAppPrivate == pNativeBuffer)
		{	
			NativeWindow_LockBuffer(pNativeBuffer);
			return m_ppBuffHead[i];
		}
	}
	
	return NULL;
}

OMX_U32 voCOMXBaseHole::NativeWindow_GetMinUndequeuedBuffers()
{
	OMX_U32 nMinUndequeuedBuffers = 0;

	VOME_NATIVEWINDOWCOMMAND sCommand;
	sCommand.nCommand = VOME_NATIVEWINDOW_GetMinUndequeuedBuffers;
	sCommand.pParam1 = &nMinUndequeuedBuffers;
	sCommand.pParam2 = NULL;

	voCOMXAutoLock lock(&m_tmNaviveWindowCommand);
	OMX_S32 nRet = m_pCallBack(m_pUserData, VOME_CID_PLAY_NATIVEWINDOWCOMMAND, (OMX_PTR)&sCommand, 0);
	if(nRet != 0)
	{
		VOLOGW("VOME_NATIVEWINDOW_GetMinUndequeuedBuffers not supported, so use default value 1");
		return 1;
	}

	VOLOGR("VOME_NATIVEWINDOW_GetMinUndequeuedBuffers %d", nMinUndequeuedBuffers);
	return nMinUndequeuedBuffers;
}

OMX_U32 voCOMXBaseHole::ProcessSEIData(OMX_BUFFERHEADERTYPE *pBuffer, OMX_U32 flags)
{
	if(m_b3DVideoDetected == OMX_TRUE)
		return OMX_ErrorNone;
	
	m_b3DVideoDetected = OMX_TRUE;

	OMX_QCOM_FRAME_PACK_ARRANGEMENT arrangementInfo;
	arrangementInfo.cancel_flag = 1;
	OMX_ERRORTYPE err = GetParentBox()->GetConfig(
		(OMX_INDEXTYPE)OMX_QcomIndexConfigVideoFramePackingArrangement, &arrangementInfo);
	
	VOLOGI("arrangementInfo.cancel_flag=%d, type=%d, err=%d",(int)arrangementInfo.cancel_flag, (int)arrangementInfo.type, (int)err);

	if(err != OMX_ErrorNone)
		return OMX_ErrorNone;

	if(arrangementInfo.cancel_flag != 1)
		m_pCallBack(m_pUserData, VOME_CID_PLAY_3DVIDEOTYPE, (OMX_PTR)&arrangementInfo.type, 0);

	return err;	
}

OMX_ERRORTYPE voCOMXBaseHole::getGraphicBufferUsage(OMX_U32& usage)
{
	OMX_INDEXTYPE index;
    OMX_ERRORTYPE err = m_pComponent->GetExtensionIndex(m_pComponent,
            const_cast<OMX_STRING>(
                    "OMX.google.android.index.getAndroidNativeBufferUsage"),
            &index);

    if (err != OMX_ErrorNone) {
		// according to the SF Hints, it is not the fatal error, so can ignore it
        VOLOGE("OMX_GetExtensionIndex failed and set usage with zero");
		usage = 0;
		return err;
    }

	
    OMX_VERSIONTYPE ver;
    ver.s.nVersionMajor = 1;
    ver.s.nVersionMinor = 0;
    ver.s.nRevision = 0;
    ver.s.nStep = 0;
    GetAndroidNativeBufferUsageParams params = {
        sizeof(GetAndroidNativeBufferUsageParams), ver, m_nIndex, 0,
    };

    err = m_pComponent->GetParameter(m_pComponent, index, &params);

    if (err != OMX_ErrorNone) {
        VOLOGE("OMX_GetAndroidNativeBufferUsage failed with error %d (0x%08x)",
                err, err);
        return err;
    }

    usage = params.nUsage;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXBaseHole::getAndroidNativeBufferVer(OMX_U32& inVer)
{
	OMX_INDEXTYPE index;
	inVer = 0;
    if (m_pComponent->GetExtensionIndex(
        m_pComponent,
        const_cast<OMX_STRING>("OMX.google.android.index.useAndroidNativeBuffer2"),
        &index) == OMX_ErrorNone)
	{
		inVer = 2;
		return OMX_ErrorNone;
	}
	if(m_pComponent->GetExtensionIndex(
       m_pComponent,
       const_cast<OMX_STRING>("OMX.google.android.index.useAndroidNativeBuffer"),
       &index) == OMX_ErrorNone)
	{
		inVer = 1;
	}   
	return OMX_ErrorNone;
}
