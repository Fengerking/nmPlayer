	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoIOMXComponent.cpp

	Contains:	Android IOMX component source file

	Written by:	East Zhou

	Change History (most recent first):
	2012-04-19	East		Create file

*******************************************************************************/
#if defined (_GB) || defined (_FRY)
#include <binder/IServiceManager.h>
#include <media/IMediaPlayerService.h>
#endif	// _GB, _FRY

#ifdef _ICS
#include <media/IMediaDeathNotifier.h>
#endif	// _ICS

#include "voIOMXDec.h"
#include "voOMXOSFun.h"
#include "voOMXMemory.h"
#include "CvoIOMXComponent.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "CvoIOMXComponent"
#endif
#include "voLog.h"

#define VOME_CHECK_STATE if(m_piOMX.get() == NULL)\
	return OMX_ErrorInvalidState;

// 1. DTS handling in OMX component
typedef struct VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE {
	/// @brief Size of the structure
	OMX_U32 nSize;
	/// @brief Version of this structure
	OMX_VERSIONTYPE nVersion;
	/// @brief TRUE to indicate timestamps are in display order in a decoder
	/// Default value is FALSE
	OMX_BOOL bDisplayOrder;
} VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE;

// 2. Buffer recycling delay in OMX decoder output
typedef struct VFM_PARAM_RECYCLINGDELAYTYPE {
	/// @brief Size of the structure
	OMX_U32 nSize;
	/// @brief Version of this structure
	OMX_VERSIONTYPE nVersion;
	/// @brief Recycling delay, in number of frames in the output, to retain some frames.
	/// Default value is 0 (no delay)
	OMX_U32 nRecyclingDelay;
} VFM_PARAM_RECYCLINGDELAYTYPE;

// 3. MMHwBuffer handle in pPlatformPrivate
typedef struct VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE {
	/// @brief Size of the structure
	OMX_U32 nSize;
	/// @brief Version of this structure
	OMX_VERSIONTYPE nVersion;
	/// @brief TRUE to indicate that the pPlatformPrivate must contain the shared chunk metadata on FillBufferDone
	/// Default value is FALSE
	/// Can be used in Android to provide data to the renderer
	OMX_BOOL bSharedchunkInPlatformprivate;
} VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE;

/// @brief Used to set static dpb size
typedef struct VFM_PARAM_STATICDPBSIZE_TYPE
{
	/// @brief Size of the structure
	OMX_U32 nSize;
	/// @brief Version of this structure
	OMX_VERSIONTYPE nVersion;
	/// @brief OMX_TRUE if it static dpb size is required
	OMX_BOOL bStaticDpbSize;
} VFM_PARAM_STATICDPBSIZE_TYPE;

struct voIOMXObserver : public BnOMXObserver
{
    voIOMXObserver() : m_pListener(NULL) {}

    void SetComponent(CvoIOMXComponent * pComp)
	{
        m_pListener = pComp;
    }

    // from IOMXObserver
    virtual void onMessage(const omx_message &msg)
	{
		if(m_pListener)
			m_pListener->OnMessage(msg);
    }

#if defined (_GB) || defined (_FRY)
	// QCOM will use this virtual function
	virtual void registerBuffers(const sp<IMemoryHeap> &mem)
	{
	}
#endif	// _GB, _FRY

protected:
    virtual ~voIOMXObserver() {}

private:
	voIOMXObserver(const voIOMXObserver &);
	voIOMXObserver &operator=(const voIOMXObserver &);

private:
    CvoIOMXComponent *	m_pListener;
};

OMX_U32 CvoIOMXComponent::FindNALHeaderInBuffer(OMX_U8 * pBuffer, OMX_U32 nLength)
{
	// find 00000001
	if(nLength < 4)
		return -1;

	OMX_U8 * pHead = pBuffer;
	OMX_U8 * pTail = pBuffer + nLength - 3;
	while(pHead < pTail)
	{
		if(pHead[0])
		{
			pHead++;
			continue;
		}

		// 00 found
		if(pHead[1])
		{
			pHead += 2;
			continue;
		}

		// 0000 found
		if(pHead[2])
		{
			pHead += 3;
			continue;
		}

		// 000000 found
		if(1 != pHead[3])
		{
			if(!pHead[3])
				pHead++;
			else
				pHead += 4;

			continue;
		}

		// 00000001 found
		return (pHead - pBuffer);
	}

	return -1;
}

OMX_U32 CvoIOMXComponent::EventHandleProc(OMX_PTR pParam)
{
	CvoIOMXComponent * pComponent = (CvoIOMXComponent *)pParam;

#ifdef _LINUX
	voSetThreadName("IOMXComponentEventHandle");
#endif

	pComponent->EventHandle();

	return 0;
}

CvoIOMXComponent::CvoIOMXComponent(OMX_VIDEO_CODINGTYPE eCodec)
	: m_hNode(NULL)
	, m_nType(COMP_UNKNOWN)
	, m_eState(OMX_StateInvalid)
	, m_eTransStatus(COMP_TRANS_None)
	, m_bOMXError(OMX_FALSE)
	, m_bFlushing(OMX_FALSE)
	, m_bPortSettingsChanging(OMX_FALSE)
	, m_pInPort(NULL)
	, m_pOutPort(NULL)
	, m_eCodec(eCodec)
	, m_pHeadData(NULL)
	, m_nHeadData(0)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nOriginalLeft(0)
	, m_nOriginalTop(0)
	, m_hEventThread(NULL)
	, m_nEventThreadID(0)
	, m_bEventThreadStop(OMX_TRUE)
{
	m_uMainVersion.s.nVersionMajor = 1;
	m_uMainVersion.s.nVersionMinor = 0;
	m_uMainVersion.s.nRevision = 0;
	m_uMainVersion.s.nStep = 0;

	// get device name: brand_model_platform_androidversionnumber
	char szProp[PROPERTY_VALUE_MAX];
	property_get("ro.product.brand", m_szDevName, "");
	property_get("ro.product.model", szProp, "");
	strcat(m_szDevName, "_");
	strcat(m_szDevName, szProp);
	property_get("ro.board.platform", m_szBoardPlatform, "");
	strcat(m_szDevName, "_");
	strcat(m_szDevName, m_szBoardPlatform);
	property_get("ro.build.version.release", szProp, "");
	strcat(m_szDevName, "_");
	strcat(m_szDevName, szProp);

	// version number
	memset(&m_nVersionNumber, 0, sizeof(m_nVersionNumber));
	char szTmp[PROPERTY_VALUE_MAX];
	char * pStart = szProp;
	char * pTmp = strchr(pStart, '.');
	VO_S32 nIndex = 0;
	while(pTmp)
	{
		memset(szTmp, 0, sizeof(szTmp));
		memcpy(szTmp, pStart, pTmp - pStart);
		szTmp[pTmp - szProp] = '\0';

		if(nIndex < 3)
			m_nVersionNumber[nIndex] = atoi(szTmp);
		nIndex++;

		pStart = pTmp + 1;
		if(nIndex >= 3 || pStart >= szProp + strlen(szProp))
			break;

		pTmp = strchr(pStart, '.');
	}

	if(nIndex < 3 && pStart < szProp + strlen(szProp))
		m_nVersionNumber[nIndex] = atoi(pStart);

	VOLOGR("device name: %s, board platform %s, version number %d %d %d", m_szDevName, m_szBoardPlatform, m_nVersionNumber[0], m_nVersionNumber[1], m_nVersionNumber[2]);

	// create event thread
	m_bEventThreadStop = OMX_FALSE;
	voOMXThreadCreate(&m_hEventThread, &m_nEventThreadID, (voOMX_ThreadProc)EventHandleProc, this, 0);
}

CvoIOMXComponent::~CvoIOMXComponent()
{
	Uninit();

	// destroy event thread
	m_bEventThreadStop = OMX_TRUE;
	if(m_tsEvent.Count() == 0)
		m_tsEvent.Up();

	VO_U32 nTryTimes = 0;
	while(NULL != m_hEventThread)
	{
		voOMXOS_Sleep(1);
		if(++nTryTimes > 1000)
		{
			VOLOGW("OMX_ErrorTimeout");
			break;
		}
	}

#ifdef _ICS
	status_t err = OK;
	if(NULL != m_piNativeWindow.get())
	{
		err = native_window_api_disconnect(m_piNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);
		if(OK != err)
		{
			VOLOGW("failed to native_window_api_disconnect %s (%d)", strerror(-err), -err);
		}
	}
#endif	// _ICS
}

OMX_ERRORTYPE CvoIOMXComponent::Connect()
{
#if defined (_GB) || defined (_FRY)
	sp<IServiceManager> piServiceManager = defaultServiceManager();
	sp<IBinder> piBinder = piServiceManager->getService(String16("media.player"));
	sp<IMediaPlayerService>	piMediaPlayerService = interface_cast<IMediaPlayerService>(piBinder);
#endif	// _GB, _FRY

#ifdef _ICS
	sp<IMediaPlayerService>	piMediaPlayerService = IMediaDeathNotifier::getMediaPlayerService();
#endif	// _ICS

	if(piMediaPlayerService.get() == NULL)
	{
		VOLOGE("failed to get MediaPlayerService!");
		return OMX_ErrorHardware;
	}

	m_piOMX = piMediaPlayerService->getOMX();

	if(m_piOMX.get() == NULL)
	{
		VOLOGE("failed to connect to IOMX!");
		return OMX_ErrorHardware;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CvoIOMXComponent::SetHeader(OMX_PTR pHeader, OMX_U32 nSize)
{
	if(pHeader == NULL)
		return OMX_ErrorBadParameter;

	memset(pHeader, 0, nSize);
	*((OMX_U32 *)pHeader) = nSize;
	memcpy((OMX_S8 *)pHeader + 4, &m_uMainVersion, sizeof(OMX_VERSIONTYPE));

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CvoIOMXComponent::CreateComponent()
{
	char szVideoDecRole[OMX_MAX_STRINGNAME_SIZE];
	if(m_eCodec == OMX_VIDEO_CodingMPEG4)
		strcpy(szVideoDecRole, "video_decoder.mpeg4");
	else if(m_eCodec == OMX_VIDEO_CodingAVC)
		strcpy(szVideoDecRole, "video_decoder.avc");
	else
	{
		VOLOGE("currently we just support mpeg4, avc. video codec %d", m_eCodec);
		return OMX_ErrorNotImplemented;
	}

	OMX_ERRORTYPE errType = Connect();
	if(OMX_ErrorNone != errType)
		return errType;

	char * szCompName = NULL;
	if(!strcmp(m_szDevName, "dasan_H910_tcc88xx_2.3.5"))
	{
		VOLOGI("Telechips platform can't support IOMX::listNodes, Role %s", szVideoDecRole);
		m_nType = COMP_TELECHIPS;
	}
	else
	{
		List<IOMX::ComponentInfo> lstCompInfo;
		errType = (OMX_ERRORTYPE)m_piOMX->listNodes(&lstCompInfo);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to list nodes 0x%08X", errType);
			return errType;
		}

		List<IOMX::ComponentInfo>::iterator itrCompInfo;
		for(itrCompInfo = lstCompInfo.begin(); itrCompInfo != lstCompInfo.end(); ++itrCompInfo)
		{
			IOMX::ComponentInfo & sCompInfo = *itrCompInfo;
			VOLOGR("Component name %s", (const char *)sCompInfo.mName);

			// skip software decoder component
			if(strncmp("OMX.", (const char *)sCompInfo.mName, 4) ||				// ics omap4 found, such as "ARC.OMX.avcdec"
				!strncmp("OMX.PV.", (const char *)sCompInfo.mName, 7) ||		// froyo found
				!strncmp("OMX.google.", (const char *)sCompInfo.mName, 11) ||	// ics found
				!strncmp("OMX.ARC.", (const char *)sCompInfo.mName, 8) ||		// Samsung GT-S7500 found
				!strncmp("OMX.ARICENT.", (const char *)sCompInfo.mName, 12) ||	// HTC Butterfly found
				!strncmp("OMX.VisualOn.", (const char *)sCompInfo.mName, 13) || // HTC Incredible S found
				strstr((const char *)sCompInfo.mName, ".sw.dec"))				// Samsung Galaxy S4 found, like OMX.SEC.avc.sw.dec
				continue;

			List<String8>::iterator itrRole;
			for(itrRole = sCompInfo.mRoles.begin(); itrRole != sCompInfo.mRoles.end(); ++itrRole)
			{
				const char * szRole = (const char *)(*itrRole);
				VOLOGR("Role %s", szRole);
				if(!strcmp(szRole, szVideoDecRole))
				{
					szCompName = (char *)(const char *)sCompInfo.mName;
					break;
				}
			}
			if(szCompName != NULL)
				break;
		}

		if(szCompName != NULL)
		{
			VOLOGI("Component Name %s, Role %s", szCompName, szVideoDecRole);

			// we must copy here, otherwise, itrCompInfo destroy, the content of szCompName will be illegal, East 20130118
			strcpy(m_szCompName, szCompName);
		}
	}

	if(COMP_TELECHIPS == m_nType)
	{
		if(m_eCodec == OMX_VIDEO_CodingMPEG4)
			strcpy(m_szCompName, "OMX.TCC.mpeg4dec");
		else if(m_eCodec == OMX_VIDEO_CodingAVC)
			strcpy(m_szCompName, "OMX.TCC.avcdec");
		else
		{
			VOLOGE("currently we just support mpeg4, avc. video codec %d", m_eCodec);
			return OMX_ErrorNotImplemented;
		}
	}
	else if(NULL == szCompName)
	{
		VOLOGE("failed to find component");
		return OMX_ErrorNotImplemented;
	}

	sp<voIOMXObserver> observer = new voIOMXObserver;
	observer->SetComponent(this);
	errType = (OMX_ERRORTYPE)m_piOMX->allocateNode(m_szCompName, observer, &m_hNode);
	if(m_hNode == NULL)
	{
		VOLOGE("failed to create the component errType 0x%08X!", errType);
		return OMX_ErrorHardware;
	}

	OMX_PARAM_COMPONENTROLETYPE sRole;
	SetHeader(&sRole, sizeof(sRole));
	memcpy(sRole.cRole, szVideoDecRole, OMX_MAX_STRINGNAME_SIZE);
	errType = SetParameter(OMX_IndexParamStandardComponentRole, &sRole, sizeof(sRole));
	if(0x80000000 == (unsigned int)errType)
	{
		// TI use 1.1.0.0 and return 0x80000000 if 1.0.0.0, retry here
		sRole.nVersion.s.nVersionMinor = 1;
		errType = SetParameter(OMX_IndexParamStandardComponentRole, &sRole, sizeof(sRole));
		if(OMX_ErrorNone == errType)
			m_uMainVersion.s.nVersionMinor = sRole.nVersion.s.nVersionMinor;
	}
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to SetParameter OMX_IndexParamStandardComponentRole %s, 0x%08X", szVideoDecRole, errType);
		return errType;
	}

	if(!strncmp(m_szCompName, "OMX.qcom.video.", 15))
		m_nType = COMP_QCOM;
	else if(!strncmp(m_szCompName, "OMX.TI.", 7))
	{
		if(strstr(m_szCompName, "DUCATI1"))
			m_nType = COMP_TI_OMAP4;
		else
			m_nType = COMP_TI_OMAP3;
	}
	else if(!strncmp(m_szCompName, "OMX.Nvidia.", 11))
		m_nType = COMP_NVIDIA;
	else if(!strncmp(m_szCompName, "HISI.OMX.PV", 11))
		m_nType = COMP_HUAWEI;
	else if(!strncmp(m_szCompName, "OMX.MARVELL", 11))
		m_nType = COMP_MARVELL;
	else if(!strncmp(m_szCompName, "OMX.ST.VFM", 10))
		m_nType = COMP_STE;
	else if(!strncmp(m_szCompName, "OMX.hantro", 10))
		m_nType = COMP_SEU;
	else if(!strncmp(m_szCompName, "OMX.SEC.", 8) || !strncmp(m_szCompName, "OMX.Exynos.", 11))	// from Nexus 10 4.2.1, Samsung modify component name to OMX.Exynos.XXX
		m_nType = COMP_SAMSUNG;
	else if(!strncmp(m_szCompName, "OMX.TCC.", 8))
		m_nType = COMP_TELECHIPS;
	else if(!strncmp(m_szCompName, "OMX.rk.video_decoder", 20))
	{
		char m_szProductModel[PROPERTY_VALUE_MAX];
		property_get("ro.product.model", m_szProductModel, "");
		if(!strncmp(m_szProductModel, "A11", 3))
			m_nType = COMP_RKCHIPS_A11;
		else
			m_nType = COMP_RKCHIPS_MK808;
	}

	if(COMP_STE == m_nType)
	{
		OMX_INDEXTYPE nIndex = OMX_IndexComponentStartUnused;
		if(OMX_VIDEO_CodingAVC == m_eCodec)
		{
			errType = GetExtensionIndex("OMX.ST.VFM.StaticDPBSize", &nIndex);
			if(OMX_ErrorNone == errType)
			{
				VFM_PARAM_STATICDPBSIZE_TYPE sStaticDpbSize;
				SetHeader(&sStaticDpbSize, sizeof(sStaticDpbSize));
				sStaticDpbSize.bStaticDpbSize = OMX_FALSE;

				errType = SetParameter(nIndex, &sStaticDpbSize, sizeof(sStaticDpbSize));
				if(OMX_ErrorNone != errType)
				{
					VOLOGW("failed to set OMX.ST.VFM.StaticDPBSize 0x%08X", errType);
				}
			}
		}

		if(OMX_VIDEO_CodingAVC == m_eCodec || OMX_VIDEO_CodingMPEG4 == m_eCodec/* || OMX_VIDEO_CodingVC1 == m_eCodec*/)
		{
			errType = GetExtensionIndex("OMX.ST.VFM.TimestampDisplayOrder", &nIndex);
			if(OMX_ErrorNone == errType)
			{
				VFM_PARAM_TIMESTAMPDISPLAYORDERTYPE sTimestampDisplayOrder;
				SetHeader(&sTimestampDisplayOrder, sizeof(sTimestampDisplayOrder));
				sTimestampDisplayOrder.bDisplayOrder = OMX_TRUE;
				errType = SetParameter(nIndex, &sTimestampDisplayOrder, sizeof(sTimestampDisplayOrder));
				if(OMX_ErrorNone != errType)
				{
					VOLOGW("failed to set OMX.ST.VFM.TimestampDisplayOrder 0x%08X", errType);
				}
			}

			errType = GetExtensionIndex("OMX.ST.VFM.RecyclingDelay", &nIndex);
			if(OMX_ErrorNone == errType)
			{
				VFM_PARAM_RECYCLINGDELAYTYPE sRecyclingDelay;
				SetHeader(&sRecyclingDelay, sizeof(sRecyclingDelay));
				sRecyclingDelay.nRecyclingDelay = 1;
				errType = SetParameter(nIndex, &sRecyclingDelay, sizeof(sRecyclingDelay));
				if(OMX_ErrorNone != errType)
				{
					VOLOGW("failed to set OMX.ST.VFM.RecyclingDelay 0x%08X", errType);
				}
			}

			if(OMX_VIDEO_CodingAVC == m_eCodec/* || OMX_VIDEO_CodingVC1 == m_eCodec*/)
			{
				errType = GetExtensionIndex("OMX.ST.VFM.SharedchunkInPlatformprivate", &nIndex);
				if(OMX_ErrorNone == errType)
				{
					VFM_PARAM_SHAREDCHUNKINPLATFORMPRIVATETYPE sSharedChunkInPP;
					SetHeader(&sSharedChunkInPP, sizeof(sSharedChunkInPP));
					sSharedChunkInPP.bSharedchunkInPlatformprivate = OMX_TRUE;
					errType = SetParameter(nIndex, &sSharedChunkInPP, sizeof(sSharedChunkInPP));
					if(OMX_ErrorNone != errType)
					{
						VOLOGW("failed to set OMX.ST.VFM.SharedchunkInPlatformprivate 0x%08X", errType);
					}
				}
			}
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CvoIOMXComponent::CreatePorts()
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	// video decoder component must has two ports, one is input and another is output
	OMX_PARAM_PORTDEFINITIONTYPE sPortDefinition;
	for(OMX_U32 i = 0; i < 2; i++)
	{
		SetHeader(&sPortDefinition, sizeof(sPortDefinition));
		sPortDefinition.nPortIndex = i;
		errType = GetParameter(OMX_IndexParamPortDefinition, &sPortDefinition, sizeof(sPortDefinition));
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to GetParameter OMX_IndexParamPortDefinition. return error %08X!", errType);
			return errType;
		}

		if(OMX_DirInput == sPortDefinition.eDir && NULL == m_pInPort)
			m_pInPort = new CvoIOMXInPort(this, i);
		else if(OMX_DirOutput == sPortDefinition.eDir && NULL == m_pOutPort)
			m_pOutPort = new CvoIOMXOutPort(this, i);
	}

	VOLOGR("in port 0x%08X, out port 0x%08X", m_pInPort, m_pOutPort);
	return errType;
}

OMX_ERRORTYPE CvoIOMXComponent::CreateVideoRenderer(OMX_S32 nNumOfOpBuffers /* = -1 */)
{
#if defined (_GB) || defined (_FRY)
	OMX_COLOR_FORMATTYPE eColor = m_pOutPort->GetColorFormat();
	OMX_U32 nVideoWidth = 0, nVideoHeight = 0, nDispWidth = 0, nDispHeight = 0;
	m_pOutPort->GetDecodeResolution(&nVideoWidth, &nVideoHeight);
	m_pOutPort->GetResolution(&nDispWidth, &nDispHeight);

	// Telechips platform stride & slice height are 0, so we workaround here, East 20120712
	if(!nVideoWidth)
		nVideoWidth = nDispWidth;
	if(!nVideoHeight)
		nVideoHeight = nDispHeight;

	VOLOGI("color format 0x%08X, video width %d, height %d, display width %d, height %d, op buffers %d", 
		eColor, nVideoWidth, nVideoHeight, nDispWidth, nDispHeight, nNumOfOpBuffers);

#ifdef OMAP_ENHANCEMENT
	if(COMP_TI_OMAP4 == m_nType)
		m_piOMXRenderer = CreateRenderer(m_piSurface, m_szCompName, eColor, nVideoWidth, nVideoHeight, nDispWidth, nDispHeight, 0, 0, nNumOfOpBuffers);
	else
#endif	// OMAP_ENHANCEMENT
		m_piOMXRenderer = CreateRenderer(m_piSurface, m_szCompName, eColor, nVideoWidth, nVideoHeight, nDispWidth, nDispHeight, 0);
	
	if(NULL == m_piOMXRenderer.get())
	{
		VOLOGE("failed to create renderer");
		return OMX_ErrorUndefined;
	}

	return OMX_ErrorNone;
#endif	// _GB, _FRY

	return OMX_ErrorNotImplemented;
}

CvoIOMXPort * CvoIOMXComponent::GetPort(OMX_U32 nIndex)
{
	if(m_pInPort && m_pInPort->GetIndex() == nIndex)
		return m_pInPort;

	if(m_pOutPort && m_pOutPort->GetIndex() == nIndex)
		return m_pOutPort;

	return NULL;
}

OMX_ERRORTYPE CvoIOMXComponent::Loaded2Idle()
{
	if(COMP_TRANS_None != m_eTransStatus)
	{
		VOLOGE("current trans status %d", m_eTransStatus);
		return OMX_ErrorIncorrectStateTransition;
	}

	if(OMX_StateLoaded != m_eState)
	{
		VOLOGE("error state %d", m_eState);
		return OMX_ErrorInvalidState;
	}

	// update input port settings
	OMX_ERRORTYPE errType = m_pInPort->SetPortSettings(m_eCodec, m_nWidth, m_nHeight);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set input port settings, 0x%08X", errType);
		return errType;
	}

	m_pInPort->GetResolution(&m_nWidth, &m_nHeight);

	// update output port settings
	errType = m_pOutPort->SetPortSettings(m_nWidth, m_nHeight);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set output port settings, 0x%08X", errType);
		return errType;
	}

	// set state to Idle
	m_eTransStatus = COMP_TRANS_Loaded2Idle;
	errType = SendCommand(OMX_CommandStateSet, OMX_StateIdle);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_StateIdle 0x%08X", errType);
		return errType;
	}

	// allocate ports' buffer
	errType = m_pInPort->AllocateBufferHandle();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to allocate buffer handle to input port 0x%08X", errType);
		return errType;
	}

	errType = m_pOutPort->AllocateBufferHandle();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to allocate buffer handle to output port 0x%08X", errType);
		return errType;
	}

	// wait for Idle state
	VO_U32 nTryTimes = 0;
	while(!(m_eTransStatus == COMP_TRANS_None && m_eState == OMX_StateIdle))
	{
		voOMXOS_Sleep(2);
		if(++nTryTimes > 2500)
		{
			VOLOGE("OMX_ErrorTimeout");
			return OMX_ErrorTimeout;
		}
	}

#ifdef _ICS
	// if output port only has 1 output buffer (buffer count - min dequeue buffer count)
	if(m_pOutPort->GetOutputBufferCount() <= 1)
		m_pInPort->SetEmptyBufferNeedRetry(OMX_FALSE);
#endif	// _ICS

	return errType;
}

OMX_ERRORTYPE CvoIOMXComponent::Idle2Executing()
{
	if(COMP_TRANS_None != m_eTransStatus)
	{
		VOLOGE("current trans status %d", m_eTransStatus);
		return OMX_ErrorIncorrectStateTransition;
	}

	if(OMX_StateIdle != m_eState)
	{
		VOLOGE("error state %d", m_eState);
		return OMX_ErrorInvalidState;
	}

	m_eTransStatus = COMP_TRANS_Idle2Execute;
	OMX_ERRORTYPE errType = SendCommand(OMX_CommandStateSet, OMX_StateExecuting);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_StateExecuting 0x%08X", errType);
		return errType;
	}

	// wait for Executing state
	VO_U32 nTryTimes = 0;
	while(!(m_eTransStatus == COMP_TRANS_None && m_eState == OMX_StateExecuting))
	{
		voOMXOS_Sleep(2);
		if(++nTryTimes > 2500)
		{
			VOLOGE("OMX_ErrorTimeout");
			return OMX_ErrorTimeout;
		}
	}

	errType = m_pOutPort->StartBuffer();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to fill all output buffers 0x%08X", errType);
		return errType;
	}

	return errType;
}

OMX_ERRORTYPE CvoIOMXComponent::Executing2Idle()
{
	if(COMP_TRANS_None != m_eTransStatus)
	{
		VOLOGE("current trans status %d", m_eTransStatus);
		return OMX_ErrorIncorrectStateTransition;
	}

	if(OMX_StateExecuting != m_eState)
	{
		VOLOGE("error state %d", m_eState);
		return OMX_ErrorInvalidState;
	}

	m_eTransStatus = COMP_TRANS_Execute2Idle;
	// set state to Idle
	OMX_ERRORTYPE errType = SendCommand(OMX_CommandStateSet, OMX_StateIdle);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_StateIdle 0x%08X", errType);
		return errType;
	}

	// wait for Idle state
	VO_U32 nTryTimes = 0;
	while(!(m_eTransStatus == COMP_TRANS_None && m_eState == OMX_StateIdle))
	{
		voOMXOS_Sleep(2);
		if(++nTryTimes > 2500)
		{
			VOLOGE("OMX_ErrorTimeout");
			return OMX_ErrorTimeout;
		}
	}

	return errType;
}

OMX_ERRORTYPE CvoIOMXComponent::Idle2Loaded()
{
	if(COMP_TRANS_None != m_eTransStatus)
	{
		VOLOGE("current trans status %d", m_eTransStatus);
		return OMX_ErrorIncorrectStateTransition;
	}

	if(OMX_StateIdle != m_eState)
	{
		VOLOGE("error state %d", m_eState);
		return OMX_ErrorInvalidState;
	}

	m_eTransStatus = COMP_TRANS_Idle2Loaded;
	OMX_ERRORTYPE errType = SendCommand(OMX_CommandStateSet, OMX_StateLoaded);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_StateLoaded 0x%08X", errType);
		return errType;
	}

	errType = m_pOutPort->FreeBufferHandle();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to free output port buffer handle 0x%08X", errType);
		return errType;
	}

	errType = m_pInPort->FreeBufferHandle();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to free input port buffer handle 0x%08X", errType);
		return errType;
	}

	VO_U32	nTryTimes = 0;
	while(!(m_eTransStatus == COMP_TRANS_None && m_eState == OMX_StateLoaded))
	{
		voOMXOS_Sleep(2);
		if(++nTryTimes > 2500)
		{
			VOLOGE("OMX_ErrorTimeout");
			return OMX_ErrorTimeout;
		}
	}

	return errType;
}

OMX_ERRORTYPE CvoIOMXComponent::Flush()
{
	// add protection for external calling, East 20130122
	// since m_tmStatus has been used outside (CvoIOMXComponent::SetParam), so we needn't use it here
	if(OMX_StateInvalid == m_eState || OMX_StateLoaded == m_eState)
		return OMX_ErrorIncorrectStateOperation;

	// we need block flush when processing port settings changed event, East 20130121
	voCOMXAutoLock lock(&m_tmPortSettingsChanging);

	m_bFlushing = OMX_TRUE;
	m_pInPort->SetFlushing(OMX_TRUE);
	m_pOutPort->SetFlushing(OMX_TRUE);

	OMX_ERRORTYPE errType = SendCommand(OMX_CommandFlush, OMX_ALL);
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_CommandFlush 0X%08X", errType);
		return errType;
	}

	VO_U32	nTryTimes = 0;
	while(OMX_TRUE == m_bFlushing)
	{
		voOMXOS_Sleep(2);
		if(++nTryTimes > 2500)
		{
			VOLOGE("OMX_ErrorTimeout");
			return OMX_ErrorTimeout;
		}
	}

        if(strstr(m_szDevName,"S_s5pc110_4.1.2") == NULL)
                m_pInPort->m_bDropFrame = OMX_TRUE;
	errType = m_pOutPort->StartBuffer();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to fill all output buffers 0x%08X", errType);
		return errType;
	}

	return errType;
}

OMX_ERRORTYPE CvoIOMXComponent::UpdateCrop()
{
	OMX_CONFIG_RECTTYPE sRect;
	SetHeader(&sRect, sizeof(sRect));
	sRect.nPortIndex = m_pOutPort->GetIndex();
	OMX_ERRORTYPE errType = GetConfig(OMX_IndexConfigCommonOutputCrop, &sRect, sizeof(sRect));
	if(OMX_ErrorNone == errType)
	{
		VOLOGI("OMX_IndexConfigCommonOutputCrop left %d, top %d, width %d, height %d", sRect.nLeft, sRect.nTop, sRect.nWidth, sRect.nHeight);
		m_nOriginalLeft = sRect.nLeft;
		m_nOriginalTop = sRect.nTop;
#ifdef _ICS
		OMX_S32 nRet = NativeWindow_SetCrop(sRect.nLeft, sRect.nTop, sRect.nLeft + sRect.nWidth, sRect.nTop + sRect.nHeight);
		if(nRet)
		{
			VOLOGE("failed to SetCrop 0x%08X", nRet);
			errType = OMX_ErrorHardware;
		}
#endif	// _ICS
	}
	else
	{
		VOLOGE("failed to GetConfig OMX_IndexConfigCommonOutputCrop 0x%08X", errType);
	}

	return errType;
}

OMX_ERRORTYPE CvoIOMXComponent::Init()
{
	voCOMXAutoLock lock(&m_tmStatus);

	OMX_ERRORTYPE errType = CreateComponent();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to create component 0x%08X, video codec %d", errType, m_eCodec);
		return errType;
	}

	errType = CreatePorts();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to create ports 0x%08X", errType);
		return errType;
	}

	m_eState = OMX_StateLoaded;

	return errType;
}

void CvoIOMXComponent::Uninit()
{
	voCOMXAutoLock lock(&m_tmStatus);

	if(NULL == m_hNode)
		return;

	if(OMX_StateLoaded != m_eState && OMX_StateExecuting != m_eState)
	{
		VOLOGE("Error state %d when Uninit", m_eState);
	}

	if(OMX_StateExecuting == m_eState)
	{
		// executing -> idle
		OMX_ERRORTYPE errType = Executing2Idle();
		if(OMX_ErrorNone == errType)
		{
			// idle -> loaded
			errType = Idle2Loaded();
			if(OMX_ErrorNone != errType)
			{
				VOLOGW("failed to set idle to loaded 0x%08X", errType);
			}
		}
		else
		{
			VOLOGW("failed to set Idle 0x%08X", errType);
		}
	}

	delete m_pInPort;
	m_pInPort = NULL;

	delete m_pOutPort;
	m_pOutPort = NULL;

//	voOMXOS_Sleep(10);

	if(m_hNode && m_piOMX.get())
	{
		m_piOMX->freeNode(m_hNode);
		m_hNode = NULL;
	}

	m_eState = OMX_StateInvalid;

	if(m_pHeadData)
	{
		delete [] m_pHeadData;
		m_pHeadData = NULL;
	}
}

OMX_ERRORTYPE CvoIOMXComponent::SetInputData(VO_CODECBUFFER * pInput)
{
	voCOMXAutoLock lock(&m_tmStatus);

	if(m_bOMXError) 
		return (OMX_ERRORTYPE)VO_ERR_CODEC_UNSUPPORTED;
		//return (OMX_ERRORTYPE)VO_ERR_IOMXDEC_Hardware;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if(OMX_StateLoaded == m_eState)
	{
		// prepare state to Idle
		errType = Loaded2Idle();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to set to idle, 0x%08X", errType);
			m_bOMXError = OMX_TRUE;
			return errType;
		}

		errType = Idle2Executing();
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to set to executing, 0x%08X", errType);
			m_bOMXError = OMX_TRUE;
			return errType;
		}

		if((m_nHeadData > 0 && m_pHeadData) && (pInput->Length != m_nHeadData || memcmp(m_pHeadData, pInput->Buffer, m_nHeadData)))
		{
			VO_CODECBUFFER sBuffer;
			memset(&sBuffer, 0, sizeof(sBuffer));
			if(OMX_VIDEO_CodingAVC == m_eCodec && !strncmp(m_szDevName, "samsung_GT-S5830_msm7k", 22))
			{
				// SAMSUNG GT-S5830T need transfer SPS, PPS separately
				// otherwise OMX_EventError -2147479542 0
				VOLOGR("SAMSUNG GT-S5830T need transfer SPS, PPS separately");
				OMX_U32 nNal1 = -1, nNal2 = -1;
				nNal1 = FindNALHeaderInBuffer(m_pHeadData, m_nHeadData);
				if(-1 != (OMX_S32)nNal1 && (OMX_S32)m_nHeadData - (OMX_S32)nNal1 > 4)
					nNal2 = FindNALHeaderInBuffer(m_pHeadData + nNal1 + 4, m_nHeadData - nNal1 - 4);
				if(-1 == (OMX_S32)nNal2)
				{
					VOLOGE("failed to separate sequence header!");
				}
				else
				{
					nNal2 += (nNal1 + 4);	// re-locate position in total buffer
					VOLOGR("nNal1 %d, nNal2 %d", nNal1, nNal2);

					sBuffer.Buffer = m_pHeadData + nNal1;
					sBuffer.Length = nNal2 - nNal1;

					errType = m_pInPort->EmptyBuffer(&sBuffer);
					if(OMX_ErrorNone != errType)
					{
						VOLOGE("failed to fill sequence data 0x%08X", errType);
						return errType;
					}

					sBuffer.Buffer = m_pHeadData + nNal2;
					sBuffer.Length = m_nHeadData - nNal2;

					errType = m_pInPort->EmptyBuffer(&sBuffer);
                                        if(strstr(m_szDevName,"S_s5pc110_4.1.2") == NULL)
                                                 m_pInPort->m_bDropFrame = OMX_TRUE;
					if(OMX_ErrorNone != errType)
					{
						VOLOGE("failed to fill sequence data 0x%08X", errType);
						return errType;
					}
				}
			}
			else
			{
				sBuffer.Buffer = m_pHeadData;
				sBuffer.Length = m_nHeadData;

				errType = m_pInPort->EmptyBuffer(&sBuffer);
				if(!m_bProbeMode)
                	                m_pInPort->m_bDropFrame = OMX_TRUE;
				if(OMX_ErrorNone != errType)
				{
					VOLOGE("failed to fill sequence data 0x%08X", errType);
					return errType;
				}
			}
		}
	}

	return m_pInPort->EmptyBuffer(pInput);
}

OMX_ERRORTYPE CvoIOMXComponent::GetOutputData(VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
	voCOMXAutoLock lock(&m_tmStatus);

	if(m_bOMXError) 
		return (OMX_ERRORTYPE)VO_ERR_CODEC_UNSUPPORTED;
		//return (OMX_ERRORTYPE)VO_ERR_IOMXDEC_Hardware;

	voIOMXPortBuffer * pBuffer = NULL;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
#ifdef _ICS
	if(m_pOutPort->GetOutputBufferCount() <= 1)
	{
		VO_U32 nTryTimes = 0;
		while(true)
		{
			errType = m_pOutPort->GetFilledBuffer(&pBuffer);
			if(OMX_ErrorNone != errType)
			{
				if((OMX_ERRORTYPE)VO_ERR_IOMXDEC_NeedRetry == errType)
				{
					nTryTimes++;

					if(nTryTimes > 5)
						return errType;
					else
					{
						voOMXOS_Sleep(2);
						continue;
					}
				}
				else
					return errType;
			}
			else
				break;
		}
	}
	else
	{
#endif	// _ICS
		errType = m_pOutPort->GetFilledBuffer(&pBuffer);
		if(OMX_ErrorNone != errType)
			return errType;
#ifdef _ICS
	}
#endif	// _ICS

	if(pOutInfo)
	{
		pOutInfo->Format.Width = m_nWidth;
		pOutInfo->Format.Height = m_nHeight;
		pOutInfo->Flag = 0;
	}

	if(pOutBuffer)
	{
		pOutBuffer->Buffer[0] = (VO_PBYTE)pBuffer;
		pOutBuffer->ColorType = (VO_IV_COLORTYPE)VO_COLOR_IOMX_PRIVATE;
		pOutBuffer->Time = pBuffer->nTimeStamp;

		VOLOGR("pBuffer 0x%08X, hBuffer 0x%08X, time %d, system time %d", pBuffer, pBuffer->hBuffer, (int)pOutBuffer->Time, voOMXOS_GetSysTime());
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE CvoIOMXComponent::SetParam(VO_S32 uParamID, VO_PTR pData)
{
	voCOMXAutoLock lock(&m_tmStatus);

	VOLOGI ("ParamID 0x%08X, pData 0x%08X", uParamID, pData);

	OMX_ERRORTYPE errType = OMX_ErrorNotImplemented;
	switch (uParamID)
	{
	case VO_PID_COMMON_HEADDATA:
		{
			VO_CODECBUFFER * pBuffer = (VO_CODECBUFFER *)pData;

			if(m_nHeadData > 0 && m_pHeadData)
			{
				if((m_nHeadData == pBuffer->Length) && !memcmp(m_pHeadData, pBuffer->Buffer, m_nHeadData))	// same head data, we can ignore
					return OMX_ErrorNone;

				delete [] m_pHeadData;
				m_pHeadData = NULL;
			}

			m_nHeadData = pBuffer->Length;
			m_pHeadData = new OMX_U8[m_nHeadData];
			if(NULL == m_pHeadData)
				return OMX_ErrorInsufficientResources;

			memcpy(m_pHeadData, pBuffer->Buffer, m_nHeadData);

#ifdef _VOLOG_RUN
			for(OMX_U32 i = 0; i < m_nHeadData; i++)
			{
				VOLOGR("VO_PID_COMMON_HEADDATA %d, 0x%02X", i, m_pHeadData[i]);
			}
#endif	// _VOLOG_RUN

			errType = OMX_ErrorNone;
		}
		break;

	case VO_PID_COMMON_FLUSH:
		errType = Flush();
		break;

	case VO_PID_COMMON_START:
		errType = OMX_ErrorNone;
		break;

	case VO_PID_COMMON_PAUSE:
		errType = OMX_ErrorNone;
		break;

	case VO_PID_COMMON_STOP:
		errType = OMX_ErrorNone;
		break;

	case VO_PID_VIDEO_FORMAT:
		{
			VO_VIDEO_FORMAT * pFormat = (VO_VIDEO_FORMAT *)pData;
			VOLOGI("VO_PID_VIDEO_FORMAT Width %d, Height %d", pFormat->Width, pFormat->Height);
			if(m_nWidth != OMX_U32(pFormat->Width) || m_nHeight != OMX_U32(pFormat->Height))
			{
				m_nWidth = pFormat->Width;
				m_nHeight = pFormat->Height;
			}
			errType = OMX_ErrorNone;
		}
		break;

	case VO_PID_IOMXDEC_SetSurface:
		{
#if defined (_GB) || defined (_FRY)
			m_piSurface = (Surface *)pData;
			if(NULL == m_piSurface.get())
			{
				VOLOGE("NULL surface");
				return OMX_ErrorBadParameter;
			}

			errType = OMX_ErrorNone;
#endif	// _GB, _FRY

#ifdef _ICS
			status_t err = OK;
			if(NULL != m_piNativeWindow.get())
			{
				err = native_window_api_disconnect(m_piNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);
				if(OK != err)
				{
					VOLOGW("failed to native_window_api_disconnect %s (%d)", strerror(-err), -err);
				}
			}

			m_piNativeWindow = (ANativeWindow *)pData;
			if(NULL == m_piNativeWindow.get())
			{
				VOLOGE("NULL native window");
				return OMX_ErrorBadParameter;
			}

			// Android call it at MediaCodec and MediaPlayerService, we need follow it
			// Nexus 10 4.2.1 can't work if we haven't called it, East 20130116
			err = native_window_api_connect(m_piNativeWindow.get(), NATIVE_WINDOW_API_MEDIA);
			if(OK != err)
			{
				VOLOGW("failed to native_window_api_connect %s (%d)", strerror(-err), -err);
			}

			errType = OMX_ErrorNone;
#endif	// _ICS
		}
		break;

	case VO_PID_IOMXDEC_RenderData:
		{
			if(NULL == pData)
			{
				VOLOGW("VO_PID_IOMXDEC_RenderData pData is NULL!!");
				return OMX_ErrorBadParameter;
			}
			VO_VIDEO_BUFFER * pVideoBuffer = (VO_VIDEO_BUFFER *)pData;
			if(NULL == pVideoBuffer->Buffer[0])
			{
				VOLOGW("VO_PID_IOMXDEC_RenderData pVideoBuffer->Buffer[0] is NULL!!");
				return OMX_ErrorBadParameter;
			}
			voIOMXPortBuffer * pBuffer = (voIOMXPortBuffer *)pVideoBuffer->Buffer[0];
			VOLOGR("pBuffer 0x%08X, hBuffer 0x%08X, system time %d", pBuffer, pBuffer->hBuffer, voOMXOS_GetSysTime());

			if(OMX_FALSE == m_pOutPort->IsLegalBuffer(pBuffer->hBuffer))
			{
				VOLOGW("Render an invalid buffer!!!");
				return OMX_ErrorBadParameter;
			}

#if defined (_GB) || defined (_FRY)
			if(NULL == m_piOMXRenderer.get())
			{
				errType = CreateVideoRenderer();
				if(OMX_ErrorNone != errType)
				{
					VOLOGE("failed to create video renderer 0x%08X", errType);
					return errType;
				}
			}

			m_piOMXRenderer->render(pBuffer->hBuffer);
#endif	// _GB, _FRY

#ifdef _ICS
			NativeWindow_QueueBuffer(pBuffer);
#endif	// _ICS

			// return hBuffer to component
			errType = m_pOutPort->FillBuffer(pBuffer->hBuffer);
		}
		break;

	case VO_PID_IOMXDEC_CancelData:
		{
			if(NULL == pData)
			{
				VOLOGW("VO_PID_IOMXDEC_CancelData pData is NULL!!");
				return OMX_ErrorBadParameter;
			}
			VO_VIDEO_BUFFER * pVideoBuffer = (VO_VIDEO_BUFFER *)pData;
			if(NULL == pVideoBuffer->Buffer[0])
			{
				VOLOGW("VO_PID_IOMXDEC_CancelData pVideoBuffer->Buffer[0] is NULL!!");
				return OMX_ErrorBadParameter;
			}
			voIOMXPortBuffer * pBuffer = (voIOMXPortBuffer *)pVideoBuffer->Buffer[0];
			VOLOGR("pBuffer 0x%08X, hBuffer 0x%08X, system time %d", pBuffer, pBuffer->hBuffer, voOMXOS_GetSysTime());

			if(OMX_FALSE == m_pOutPort->IsLegalBuffer(pBuffer->hBuffer))
			{
				VOLOGW("Cancel an invalid buffer!!!");
				return OMX_ErrorBadParameter;
			}

#if defined (_GB) || defined (_FRY)
			if(NULL == m_piOMXRenderer.get())
			{
				errType = CreateVideoRenderer();
				if(OMX_ErrorNone != errType)
				{
					VOLOGE("failed to create video renderer 0x%08X", errType);
					return errType;
				}
			}
#endif	// _GB, _FRY

#ifdef _ICS
			NativeWindow_CancelBuffer(pBuffer);
#endif	// _ICS

			// return hBuffer to component
			errType = m_pOutPort->FillBuffer(pBuffer->hBuffer, OMX_TRUE);
		}
		break;

	case VO_PID_IOMXDEC_ForceOutputAll:
		{
			VO_CODECBUFFER sBuffer;
			memset(&sBuffer, 0, sizeof(sBuffer));
			errType = m_pInPort->EmptyBuffer(&sBuffer, VO_TRUE);
		}
		break;

	case VO_PID_IOMXDEC_SetCrop:
		{
#ifdef _ICS
			if(NULL == pData)
			{
				VOLOGW("VO_PID_IOMXDEC_SetCrop pData is NULL!!");
				return OMX_ErrorBadParameter;
			}

			VO_RECT * pRect = (VO_RECT *)pData;
			VOLOGR("VO_PID_IOMXDEC_SetCrop ol %d, ot %d, l %d, t %d, r %d, b %d", m_nOriginalLeft, m_nOriginalTop, pRect->left, pRect->top, pRect->right, pRect->bottom);

			// we need consider original offset (left & top) for HW decoder, East 20120222
			OMX_S32 nRet = NativeWindow_SetCrop(m_nOriginalLeft + pRect->left, m_nOriginalTop + pRect->top, m_nOriginalLeft + pRect->right, m_nOriginalTop + pRect->bottom);
			if(nRet)
				errType = OMX_ErrorUndefined;
			else
				errType = OMX_ErrorNone;
#else	// _ICS
			VOLOGE("failed to VO_PID_IOMXDEC_SetCrop! just ICS and above support.");
			return OMX_ErrorNotImplemented;
#endif	// _ICS
		}
		break;

	default:
		break;
	}

	return errType;
}

OMX_ERRORTYPE CvoIOMXComponent::GetParam(VO_S32 uParamID, VO_PTR pData)
{
	voCOMXAutoLock lock(&m_tmStatus);

	if(NULL == pData)
	{
		VOLOGE("parameter data is null!!");
		return OMX_ErrorBadParameter;
	}

	OMX_ERRORTYPE errType = OMX_ErrorNotImplemented;
	switch (uParamID)
	{
	case VO_PID_VIDEO_ASPECTRATIO:
		{
			if(!m_pInPort)
				return OMX_ErrorNotReady;

			VO_S32 nAspectRatio = m_pInPort->GetAspectRatio();
			if(-1 != nAspectRatio)
			{
				*((VO_U32 *)pData) = nAspectRatio;
				errType = OMX_ErrorNone;
			}
		}
		break;

	case VO_PID_IOMXDEC_IsWorking:
		{
			*((VO_BOOL *)pData) = (OMX_StateInvalid == m_eState || OMX_StateLoaded == m_eState) ? VO_FALSE : VO_TRUE;
			errType = OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	VOLOGI ("ParamID 0x%08X, pData 0x%08X, 0x%08X", uParamID, pData, errType);

	return errType;
}

OMX_ERRORTYPE CvoIOMXComponent::SendCommand(OMX_COMMANDTYPE nCmd, OMX_S32 nParam)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->sendCommand(m_hNode, nCmd, nParam);
}

OMX_ERRORTYPE CvoIOMXComponent::GetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pParams, size_t nSize)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->getParameter(m_hNode, nIndex, pParams, nSize);
}

OMX_ERRORTYPE CvoIOMXComponent::SetParameter(OMX_INDEXTYPE nIndex, const OMX_PTR pParams, size_t nSize)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->setParameter(m_hNode, nIndex, pParams, nSize);
}

OMX_ERRORTYPE CvoIOMXComponent::GetConfig(OMX_INDEXTYPE nIndex, OMX_PTR pParams, size_t nSize)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->getConfig(m_hNode, nIndex, pParams, nSize);
}

OMX_ERRORTYPE CvoIOMXComponent::SetConfig(OMX_INDEXTYPE nIndex, const OMX_PTR pParams, size_t nSize)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->setConfig(m_hNode, nIndex, pParams, nSize);
}

OMX_ERRORTYPE CvoIOMXComponent::GetState(OMX_STATETYPE * peState)
{
	VOME_CHECK_STATE

#ifndef _ICS
	return OMX_ErrorNotImplemented;
#else
	return (OMX_ERRORTYPE)m_piOMX->getState(m_hNode, peState);
#endif	//_ICS
}

OMX_ERRORTYPE CvoIOMXComponent::AllocateBuffer(OMX_U32 nPortIndex, size_t nBufferSize, IOMX::buffer_id * phBuffer, void ** ppBuffer)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->allocateBuffer(m_hNode, nPortIndex, nBufferSize, phBuffer, ppBuffer);
}

OMX_ERRORTYPE CvoIOMXComponent::AllocateBufferWithBackup(OMX_U32 nPortIndex, const sp<IMemory>& piMemory, IOMX::buffer_id * phBuffer)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->allocateBufferWithBackup(m_hNode, nPortIndex, piMemory, phBuffer);
}

OMX_ERRORTYPE CvoIOMXComponent::FreeBuffer(OMX_U32 nPortIndex, IOMX::buffer_id hBuffer)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->freeBuffer(m_hNode, nPortIndex, hBuffer);
}

OMX_ERRORTYPE CvoIOMXComponent::FillBuffer(IOMX::buffer_id hBuffer)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->fillBuffer(m_hNode, hBuffer);
}

OMX_ERRORTYPE CvoIOMXComponent::EmptyBuffer(IOMX::buffer_id hBuffer, OMX_U32 nOffset, OMX_U32 nLength, OMX_U32 nFlags, OMX_TICKS nTimeStamp)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->emptyBuffer(m_hNode, hBuffer, nOffset, nLength, nFlags, nTimeStamp);
}

OMX_ERRORTYPE CvoIOMXComponent::GetExtensionIndex(const char * szName, OMX_INDEXTYPE * pnIndex)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->getExtensionIndex(m_hNode, szName, pnIndex);
}

#if defined (_GB) || defined (_FRY)
sp<IOMXRenderer> CvoIOMXComponent::CreateRenderer(const sp<Surface>& piSurface, const OMX_STRING szCompName, OMX_COLOR_FORMATTYPE eColor, 
												  size_t nVideoWidth, size_t nVideoHeight, size_t nDispWidth, size_t nDispHeight, OMX_S32 nRotationDegrees)
{
	if(m_piOMX.get() == NULL)
		return NULL;

#ifdef _GB
	return m_piOMX->createRenderer(piSurface, szCompName, eColor, nVideoWidth, nVideoHeight, nDispWidth, nDispHeight, nRotationDegrees);
#else	// _GB
	return m_piOMX->createRenderer(piSurface, szCompName, eColor, nVideoWidth, nVideoHeight, nDispWidth, nDispHeight);
#endif	// _GB
}

#ifdef OMAP_ENHANCEMENT
sp<IOMXRenderer> CvoIOMXComponent::CreateRenderer(const sp<Surface>& piSurface, const OMX_STRING szCompName, OMX_COLOR_FORMATTYPE eColor, 
												  size_t nVideoWidth, size_t nVideoHeight, size_t nDispWidth, size_t nDispHeight, OMX_S32 nRotationDegrees, 
												  OMX_S32 nIsS3D, OMX_S32 nNumOfOpBuffers /* = -1 */)
{
	if(m_piOMX.get() == NULL)
		return NULL;

	return m_piOMX->createRenderer(piSurface, szCompName, eColor, nVideoWidth, nVideoHeight, nDispWidth, nDispHeight, nRotationDegrees, nIsS3D, nNumOfOpBuffers);
}

Vector< sp<IMemory> > CvoIOMXComponent::GetBuffers(const sp<IOMXRenderer>& piRenderer)
{
	return piRenderer->getBuffers();
}

OMX_ERRORTYPE CvoIOMXComponent::UseBuffer(OMX_U32 nPortIndex, const sp<IMemory>& piMemory, IOMX::buffer_id * phBuffer, size_t nSize)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->useBuffer(m_hNode, nPortIndex, piMemory, phBuffer, nSize);
}

#endif	// OMAP_ENHANCEMENT
#endif	// _GB, _FRY

#ifdef _ICS
OMX_ERRORTYPE CvoIOMXComponent::EnableGraphicBuffers(OMX_U32 nPortIndex, OMX_BOOL bEnable)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->enableGraphicBuffers(m_hNode, nPortIndex, bEnable);
}

#ifdef _KK
OMX_ERRORTYPE CvoIOMXComponent::PrepareForAdaptivePlayback(OMX_U32 nPortIndex, OMX_BOOL bEnable, OMX_U32 maxFrameWidth, OMX_U32 maxFrameHeight)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->prepareForAdaptivePlayback(m_hNode, nPortIndex, OMX_TRUE, maxFrameWidth, maxFrameHeight);
}
#endif // _KK

OMX_ERRORTYPE CvoIOMXComponent::GetGraphicBufferUsage(OMX_U32 nPortIndex, OMX_U32 * pnUsage)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->getGraphicBufferUsage(m_hNode, nPortIndex, pnUsage);
}

OMX_ERRORTYPE CvoIOMXComponent::UseGraphicBuffer(OMX_U32 nPortIndex, const sp<GraphicBuffer>& piGraphicBuffer, IOMX::buffer_id * phBuffer)
{
	VOME_CHECK_STATE

	return (OMX_ERRORTYPE)m_piOMX->useGraphicBuffer(m_hNode, nPortIndex, piGraphicBuffer, phBuffer);
}

OMX_S32 CvoIOMXComponent::NativeWindow_CancelBuffer(voIOMXPortBuffer * pBuffer)
{
	voCOMXAutoLock lock(&m_tmNativeWindow);

	if(pBuffer->bDequeued == OMX_FALSE)
		return OK;

	VOLOGR("cancelBuffer 0x%08X", pBuffer->piGraphicBuffer.get());
#ifdef _JB43
	status_t err = m_piNativeWindow->cancelBuffer(m_piNativeWindow.get(), pBuffer->piGraphicBuffer.get(), -1);
#else
	status_t err = m_piNativeWindow->cancelBuffer(m_piNativeWindow.get(), pBuffer->piGraphicBuffer.get());
#endif
	if(OK != err)
	{
		VOLOGE("failed to cancelBuffer %s (%d)", strerror(-err), -err);
		return err;
	}

	pBuffer->bDequeued = OMX_FALSE;
	return err;
}

OMX_S32 CvoIOMXComponent::NativeWindow_QueueBuffer(voIOMXPortBuffer * pBuffer)
{
	voCOMXAutoLock lock(&m_tmNativeWindow);

	VOLOGR("queueBuffer 0x%08X", pBuffer->piGraphicBuffer.get());
#ifdef _JB43
	status_t err = m_piNativeWindow->queueBuffer(m_piNativeWindow.get(), pBuffer->piGraphicBuffer.get(), -1);
#else
	status_t err = m_piNativeWindow->queueBuffer(m_piNativeWindow.get(), pBuffer->piGraphicBuffer.get());
#endif
	if(OK != err)
	{
		VOLOGE("failed to queueBuffer %s (%d)", strerror(-err), -err);
		return err;
	}

	pBuffer->bDequeued = OMX_FALSE;
	return err;
}

OMX_S32 CvoIOMXComponent::NativeWindow_SetCrop(OMX_S32 nLeft, OMX_S32 nTop, OMX_S32 nRight, OMX_S32 nBottom)
{
	voCOMXAutoLock lock(&m_tmNativeWindow);
        android_native_rect_t sNativeRect;
	sNativeRect.left = nLeft;
	sNativeRect.top = nTop;
	sNativeRect.right = nRight;
	sNativeRect.bottom = nBottom;
        VOLOGI("Current set crop %d %d %d %d",nLeft,nTop, nRight, nBottom);
	status_t err = native_window_set_crop(m_piNativeWindow.get(), &sNativeRect);
	if(OK != err)
	{
		VOLOGW("failed to native_window_set_crop %s (%d)", strerror(-err), -err);
	}

	return err;
}

voIOMXPortBuffer * CvoIOMXComponent::NativeWindow_PrepareBuffer(voIOMXPortBuffer * pBuffers, OMX_U32 nBuffers)
{
	voCOMXAutoLock lock(&m_tmNativeWindow);

	ANativeWindowBuffer * pNativeWindowBuffer;
	// from 4.2, Android support native_window_dequeue_buffer_and_wait, but it seems old API can work fine, East 20130116
#ifdef _JB43
	status_t err = native_window_dequeue_buffer_and_wait(m_piNativeWindow.get(), &pNativeWindowBuffer);
#else
	status_t err = m_piNativeWindow->dequeueBuffer(m_piNativeWindow.get(), &pNativeWindowBuffer);
#endif
	if(OK != err)
	{
		VOLOGE("failed to dequeueBuffer %s (%d)", strerror(-err), -err);
		return NULL;
	}

	for(OMX_U32 i = 0; i < nBuffers; i++)
	{
		if(pBuffers[i].piGraphicBuffer->handle == pNativeWindowBuffer->handle)
		{
			pBuffers[i].bDequeued = OMX_TRUE;
			// from 4.2, Android needn't call lockBuffer anymore, but old API seems do nothing, so we keep it for "backwards compatibility", East 20130116
#ifndef _JB43
			err = m_piNativeWindow->lockBuffer(m_piNativeWindow.get(), pBuffers[i].piGraphicBuffer.get());
#endif
			if(OK != err)
			{
				VOLOGW("failed to lockBuffer %s (%d)", strerror(-err), -err);
			}

			VOLOGR("dequeueBuffer & lockBuffer 0x%08X", pBuffers[i].piGraphicBuffer.get());
			return pBuffers + i;
		}
	}

	return NULL;
}
#endif	// _ICS

OMX_U32 CvoIOMXComponent::EventHandle()
{
	while(!m_bEventThreadStop)
	{
		m_tsEvent.Down();
		EVENT_TYPE * pEvent = (EVENT_TYPE *)m_tqEvent.Remove();
		if(pEvent)
		{
			ProcEvent(pEvent->eEvent, pEvent->nData1, pEvent->nData2);
			voOMXMemFree(pEvent);
		}
	}

	m_hEventThread = NULL;
	return 0;
}

void CvoIOMXComponent::OnEvent(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2)
{
	VOLOGI("Event %d, Data1 %d, Data2 %d", eEvent, nData1, nData2);

	if(eEvent == OMX_EventPortSettingsChanged)
	{
		// use thread to process it
		EVENT_TYPE * pEvent = (EVENT_TYPE *)voOMXMemAlloc(sizeof(EVENT_TYPE));
		if(NULL != pEvent)
		{
			pEvent->eEvent = eEvent;
			pEvent->nData1 = nData1;
			pEvent->nData2 = nData2;

			m_tqEvent.Add(pEvent);
			m_tsEvent.Up();
		}
	}
	else
		ProcEvent(eEvent, nData1, nData2);
}

void CvoIOMXComponent::ProcEvent(OMX_EVENTTYPE eEvent, OMX_U32 nData1, OMX_U32 nData2)
{
	switch(eEvent)
	{
	case OMX_EventCmdComplete:
		{
			switch(nData1)
			{
			case OMX_CommandStateSet:
				{
					VOLOGI("OMX_EventCmdComplete OMX_CommandStateSet state %d", nData2);
					m_eState = (OMX_STATETYPE)nData2;
					m_eTransStatus = COMP_TRANS_None;

					if(OMX_StateIdle == nData2)
					{
						m_pInPort->SetChanging2IdleState(OMX_FALSE);
						m_pOutPort->SetChanging2IdleState(OMX_FALSE);
					}
				}
				break;

			case OMX_CommandFlush:
				{
					VOLOGI("OMX_EventCmdComplete OMX_CommandFlush %d", nData2);

					CvoIOMXPort * pPort = GetPort(nData2);
					if(pPort)
						pPort->SetFlushing(OMX_FALSE);

					if(!m_pInPort->IsFlushing() && !m_pOutPort->IsFlushing())
						m_bFlushing = OMX_FALSE;
				}
				break;

			case OMX_CommandPortDisable:
			case OMX_CommandPortEnable:
				{
					VOLOGI("OMX_EventCmdComplete %d port %d", nData1, nData2);

					CvoIOMXPort * pPort = GetPort(nData2);
					if(pPort)
						pPort->UpdatePortDefinition();

#ifdef _ICS
					// after allocate buffer and before start buffer, East 20130109
					if((OMX_CommandPortEnable == nData1) && (OMX_TRUE == m_bPortSettingsChanging))
						UpdateCrop();
#endif	// _ICS
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
			VOLOGE("OMX_EventError %d %d", nData1, nData2);
			m_bOMXError = OMX_TRUE;
		}
		break;

	case OMX_EventMark:
		break;

	case OMX_EventPortSettingsChanged:
		{
			VOLOGI("OMX_EventPortSettingsChanged port %d, nData2 %d", nData1, nData2);

			OMX_ERRORTYPE errType = OMX_ErrorNone;
			// until ICS, we need support following settings, East 20121230
			// OMX_IndexParamPortDefinition, OMX_IndexConfigCommonOutputCrop, OMX_IndexConfigCommonScale
			switch(nData2)
			{
			case 0:
			case OMX_IndexParamPortDefinition:
				{
					voCOMXAutoLock lock(&m_tmPortSettingsChanging);

					m_bPortSettingsChanging = OMX_TRUE;

					CvoIOMXPort * pPort = GetPort(nData1);
					if(pPort)
					{
						errType = pPort->Reconfigure();
						if(OMX_ErrorNone != errType)
						{
							m_bOMXError = OMX_TRUE;
							VOLOGE("failed to Reconfigure 0x%08X", errType);
						}
					}
				}
				break;

			case OMX_IndexConfigCommonScale:
				{
					if(m_pOutPort->GetIndex() == nData1)
					{
						OMX_CONFIG_SCALEFACTORTYPE sScaleFactor;
						SetHeader(&sScaleFactor, sizeof(sScaleFactor));
						sScaleFactor.nPortIndex = nData1;
						errType = GetConfig(OMX_IndexConfigCommonScale, &sScaleFactor, sizeof(sScaleFactor));
						if(OMX_ErrorNone == errType)
						{
							VOLOGI("OMX_IndexConfigCommonScale (aspect ratio) w %d h %d", sScaleFactor.xWidth, sScaleFactor.xHeight);
						}
						else
						{
							VOLOGE("failed to GetConfig OMX_IndexConfigCommonScale 0x%08X", errType);
						}
					}
					else
					{
						VOLOGE("OMX_IndexConfigCommonScale should happen at output port!");
					}
				}
				break;

			case OMX_IndexConfigCommonOutputCrop:
				{
					if(m_pOutPort->GetIndex() == nData1)
						errType = UpdateCrop();
					else
					{
						VOLOGE("OMX_IndexConfigCommonOutputCrop should happen at output port!");
					}
				}
				break;

			default:
				{
					VOLOGE("Unknown OMX_EventPortSettingsChanged %d", nData2);
				}
				break;
			}
		}
		break;

	case OMX_EventBufferFlag:
		{
			if(nData2 & OMX_BUFFERFLAG_EOS)
			{
				VOLOGI("OMX_EventBufferFlag OMX_BUFFERFLAG_EOS flag 0x%08X port %d", nData2, nData1);
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

	default:
		break;
	}
}

void CvoIOMXComponent::OnMessage(const omx_message &msg)
{
	voCOMXAutoLock lock(&m_tmLock);

	switch(msg.type)
	{
	case omx_message::EVENT:
		{
			OnEvent(msg.u.event_data.event, msg.u.event_data.data1, msg.u.event_data.data2);
		}
		break;

	case omx_message::EMPTY_BUFFER_DONE:
		{
			m_pInPort->OnEmptyBufferDone(msg.u.buffer_data.buffer);
		}
		break;

	case omx_message::FILL_BUFFER_DONE:
		{
			m_pOutPort->OnFillBufferDone(msg.u.extended_buffer_data.buffer, 
				msg.u.extended_buffer_data.range_offset, msg.u.extended_buffer_data.range_length, 
				msg.u.extended_buffer_data.flags, msg.u.extended_buffer_data.timestamp, 
				msg.u.extended_buffer_data.platform_private, msg.u.extended_buffer_data.data_ptr);
		}
		break;

	default:
		{
			VOLOGW("OnMessage illegal message type %d", msg.type);
		}
		break;
	}
}

