	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CvoIOMXInPort.cpp

	Contains:	Android IOMX component input port source file

	Written by:	East Zhou

	Change History (most recent first):
	2012-04-19	East		Create file

*******************************************************************************/
#include "voIOMXDec.h"
#include "voOMXOSFun.h"
#include "CvoIOMXComponent.h"
#include "CvoIOMXInPort.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "CvoIOMXInPort"
#endif

#include "voLog.h"

CvoIOMXInPort::CvoIOMXInPort(CvoIOMXComponent * pComponent, OMX_U32 nPortIndex)
	: CvoIOMXPort(pComponent, nPortIndex)
	, m_hVideoParser(NULL)
	, m_nAspectRatio(-1)
	, m_bEmptyBufferNeedRetry(OMX_TRUE)
{
	memset(&m_sVideoParserAPI, 0, sizeof(m_sVideoParserAPI));
}

CvoIOMXInPort::~CvoIOMXInPort()
{
	if(m_hVideoParser)
	{
		m_sVideoParserAPI.Uninit(m_hVideoParser);
		m_hVideoParser = NULL;
	}
}

#define PROFILE_VO2OMX(profile)		case VO_VIDEO_##profile:sAVC.eProfile = OMX_VIDEO_##profile;break;
#define LEVEL_VO2OMX(level)			case VO_VIDEO_##level:sAVC.eLevel = OMX_VIDEO_##level;break;

OMX_ERRORTYPE CvoIOMXInPort::SetPortSettings(OMX_VIDEO_CODINGTYPE eCodec, OMX_U32 nWidth, OMX_U32 nHeight)
{
	VOLOGI("Port %d, Codec %d, Width %d, Height %d", m_nIndex, eCodec, nWidth, nHeight);

	OMX_ERRORTYPE errType = UpdatePortFormat();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to UpdatePortFormat 0x%08X", errType);
		return errType;
	}

	m_sPortFormat.eCompressionFormat = eCodec;
	m_sPortFormat.eColorFormat = OMX_COLOR_FormatUnused;
	errType = m_pComponent->SetParameter(OMX_IndexParamVideoPortFormat, &m_sPortFormat, sizeof(m_sPortFormat));
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_IndexParamVideoPortFormat 0x%08X", errType);
		return errType;
	}

	// use video parser
	eVOCodec = VO_VIDEO_CodingUnused;
	if(eCodec == OMX_VIDEO_CodingMPEG4)
		eVOCodec = VO_VIDEO_CodingMPEG4;
	else if(eCodec == OMX_VIDEO_CodingAVC)
		eVOCodec = VO_VIDEO_CodingH264;
	else
	{
		VOLOGE("currently we just support mpeg4, avc. video codec %d", eCodec);
		return OMX_ErrorNotImplemented;
	}

	if(m_pComponent->m_pHeadData && m_pComponent->m_nHeadData)
	{
		VO_U32 nRC = VO_ERR_NONE;
		if(NULL == m_hVideoParser)
		{
			// initialize video parser
			nRC = voGetVideoParserAPI(&m_sVideoParserAPI, eVOCodec);
			if(VO_ERR_NONE == nRC)
				nRC = m_sVideoParserAPI.Init(&m_hVideoParser);
		}

		if(VO_ERR_NONE == nRC)
		{
			VO_CODECBUFFER sBuffer;
			sBuffer.Length = m_pComponent->m_nHeadData;
			sBuffer.Buffer = m_pComponent->m_pHeadData;
			nRC = m_sVideoParserAPI.Process(m_hVideoParser, &sBuffer);
			if(VO_ERR_NONE == nRC)
			{
				nRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_WIDTH, &nWidth);
				nRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_HEIGHT, &nHeight);

				VOLOGI("width %d, height %d from video parser", nWidth, nHeight);

				VO_PARSER_ASPECTRATIO sRatio;
				memset(&sRatio, 0, sizeof(VO_PARSER_ASPECTRATIO));
				sRatio.mode = -1;
				nRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_RATIO, &sRatio);
				if(VO_ERR_NONE == nRC)
				{
					VOLOGI("aspect ratio %d, width %d, height %d", sRatio.mode, sRatio.width, sRatio.height);

					if(sRatio.mode == VO_RATIO_MAX)
					{
						VOLOGI("handle undefined aspect ratio, %dx%d", sRatio.width, sRatio.height);

						VO_S32 nWidth = sRatio.width;
						VO_S32 nHeight = sRatio.height;
						if(nWidth > 0x7fff || nHeight > 0x7fff)
						{
							nWidth = (nWidth + 512) >> 10;
							nHeight = (nHeight + 512) >> 10;
						}
						m_nAspectRatio= (nWidth << 16) | nHeight;
					}
					else
						m_nAspectRatio = sRatio.mode;
				}
				else
				{
					VOLOGW("failed to VO_PID_VIDEOPARSER_RATIO 0x%08X", nRC);
				}
			}
		}
	}

#ifdef _GB
	if(OMX_VIDEO_CodingAVC == eCodec && COMP_TI_OMAP4 == m_pComponent->m_nType)
	{
		// TI OMAP4 GB need set OMX_IndexParamVideoAvc
		// we will parse header data to get such information
		VOLOGI("DUCATI1 GB AVC");

		if(m_hVideoParser)
		{
			OMX_VIDEO_PARAM_AVCTYPE sAVC;
			m_pComponent->SetHeader(&sAVC, sizeof(sAVC));
			sAVC.nPortIndex = m_nIndex;
			errType = m_pComponent->GetParameter(OMX_IndexParamVideoAvc, &sAVC, sizeof(sAVC));
			if(OMX_ErrorNone == errType)
			{
				VO_U32 nRC = VO_ERR_NONE;
				// profile
				VO_VIDEO_PROFILETYPE eProfile = VO_VIDEO_AVCProfileBaseline;
				nRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_PROFILE, &eProfile);
				// VO Profile -> Standard OMX Profile
				switch(eProfile)
				{
					PROFILE_VO2OMX(AVCProfileBaseline)
					PROFILE_VO2OMX(AVCProfileMain)
					PROFILE_VO2OMX(AVCProfileExtended)
					PROFILE_VO2OMX(AVCProfileHigh)
					PROFILE_VO2OMX(AVCProfileHigh10)
					PROFILE_VO2OMX(AVCProfileHigh422)
					PROFILE_VO2OMX(AVCProfileHigh444)

				default:
					sAVC.eProfile = OMX_VIDEO_AVCProfileMax;
					break;
				}

				// level
				VO_VIDEO_LEVELTYPE eLevel = VO_VIDEO_AVCLevel1;
				nRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_LEVEL, &eLevel);
				// VO Level -> Standard OMX Level
				switch(eLevel)
				{
					LEVEL_VO2OMX(AVCLevel1)
					LEVEL_VO2OMX(AVCLevel1b)
					LEVEL_VO2OMX(AVCLevel11)
					LEVEL_VO2OMX(AVCLevel12)
					LEVEL_VO2OMX(AVCLevel13)
					LEVEL_VO2OMX(AVCLevel2)
					LEVEL_VO2OMX(AVCLevel21)
					LEVEL_VO2OMX(AVCLevel22)
					LEVEL_VO2OMX(AVCLevel3)
					LEVEL_VO2OMX(AVCLevel31)
					LEVEL_VO2OMX(AVCLevel32)
					LEVEL_VO2OMX(AVCLevel4)
					LEVEL_VO2OMX(AVCLevel41)
					LEVEL_VO2OMX(AVCLevel42)
					LEVEL_VO2OMX(AVCLevel5)
					LEVEL_VO2OMX(AVCLevel51)

				default:
					sAVC.eLevel = OMX_VIDEO_AVCLevelMax;
					break;
				}

				// reference frames
				VO_U32 nRefFrames = 1;
				nRC = m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_REFNUM, &nRefFrames);
				sAVC.nRefFrames = nRefFrames;

				VOLOGI("AVC profile %d, level %d, ref frames %d", eProfile, eLevel, nRefFrames);

				errType = m_pComponent->SetParameter(OMX_IndexParamVideoAvc, &sAVC, sizeof(sAVC));
				if(OMX_ErrorNone != errType)
				{
					VOLOGW("failed to set OMX_IndexParamVideoAvc 0x%08X", errType);
				}
			}
		}
	}
#endif	// _GB

	errType = UpdatePortDefinition();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to get OMX_IndexParamPortDefinition 0x%08X", errType);
		return errType;
	}

#ifdef _GB
	if(COMP_TI_OMAP4 == m_pComponent->m_nType)
	{
		m_sPortDefinition.nBufferSize = nWidth * nHeight;
		m_sPortDefinition.nBufferCountActual = 4;
	}
#endif	// _GB
	// make sure buffer size >= 64K
	if(m_sPortDefinition.nBufferSize < 0x10000)
		m_sPortDefinition.nBufferSize = 0x10000;

	if(COMP_RKCHIPS_A11 == m_pComponent->m_nType)
	{
		m_sPortDefinition.format.video.nFrameWidth = ((nWidth + 15) & ~15);
		m_sPortDefinition.format.video.nFrameHeight = ((nHeight + 15) & ~15);
	}
	else if(COMP_RKCHIPS_MK808 == m_pComponent->m_nType)
	{
		m_sPortDefinition.format.video.nFrameWidth = ((nWidth + 31) & ~31);
		m_sPortDefinition.format.video.nFrameHeight = ((nHeight + 31) & ~31);
	}
	else
	{
		m_sPortDefinition.format.video.nFrameWidth = nWidth;
		m_sPortDefinition.format.video.nFrameHeight = nHeight;
	}
	m_sPortDefinition.format.video.eCompressionFormat = eCodec;
	m_sPortDefinition.format.video.eColorFormat = OMX_COLOR_FormatUnused;

	errType = m_pComponent->SetParameter(OMX_IndexParamPortDefinition, &m_sPortDefinition, sizeof(m_sPortDefinition));
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to set OMX_IndexParamPortDefinition 0x%08X", errType);
		return errType;
	}

	// get final port definition
	errType = UpdatePortDefinition();
	if(OMX_ErrorNone != errType)
	{
		VOLOGE("failed to get OMX_IndexParamPortDefinition 0x%08X", errType);
		return errType;
	}

	VOLOGI("input port buffer count %d, buffer size %d, width %d, height %d, stride %d, slice height %d", m_sPortDefinition.nBufferCountActual, m_sPortDefinition.nBufferSize, 
		m_sPortDefinition.format.video.nFrameWidth, m_sPortDefinition.format.video.nFrameHeight, m_sPortDefinition.format.video.nStride, m_sPortDefinition.format.video.nSliceHeight);

	return errType;
}

OMX_ERRORTYPE CvoIOMXInPort::AllocateBufferHandle()
{
	m_nBufferCount = m_sPortDefinition.nBufferCountActual;
	m_pBuffers = new voIOMXPortBuffer[m_nBufferCount];
	if(!m_pBuffers)
	{
		VOLOGE("failed to allocate buffer pointer!");
		return OMX_ErrorInsufficientResources;
	}
	memset(m_pBuffers, 0, m_nBufferCount * sizeof(voIOMXPortBuffer));

	if(OMX_FALSE == m_listBufferEmpty.Create(m_nBufferCount))
	{
		VOLOGE("failed to create empty buffer list!");
		return OMX_ErrorInsufficientResources;
	}

	size_t nTotalSize = m_sPortDefinition.nBufferSize * m_nBufferCount;
	m_pMemoryDealer = new MemoryDealer(nTotalSize, "voIOMXInputPort");

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	for(OMX_U32 i = 0; i < m_nBufferCount; i++)
	{
		sp<IMemory> piMemory = m_pMemoryDealer->allocate(m_sPortDefinition.nBufferSize);

		IOMX::buffer_id hBuffer = NULL;
		errType = m_pComponent->AllocateBufferWithBackup(m_nIndex, piMemory, &hBuffer);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("failed to AllocateBufferWithBackup 0x%08X", errType);
			return errType;
		}

		m_pBuffers[i].hBuffer = hBuffer;
		m_pBuffers[i].pData = piMemory->pointer();

		VOLOGR("AllocateBufferWithBackup 0x%08X, 0x%08X", hBuffer, piMemory->pointer());

		m_listBufferEmpty.Add(m_pBuffers + i);
	}

	return errType;
}

OMX_ERRORTYPE CvoIOMXInPort::EmptyBuffer(VO_CODECBUFFER * pInput, VO_BOOL bEOS /* = VO_FALSE */)
{
	VOLOGR("111 EmptyBuffer length %d, time %d, eos %d, system time %d", pInput->Length, (int)pInput->Time, int(bEOS), voOMXOS_GetSysTime());

	if(0 == pInput->Length && VO_FALSE == bEOS)
	{
		VOLOGW("input null buffer, return OMX_ErrorNone directly!");
		return OMX_ErrorNone;
	}

	// To avoid performance issue, we wait max 6 ms for valid buffer, otherwise return retry, East 20130112
	voIOMXPortBuffer * pEmptyBuffer = NULL;
	OMX_U32	nTryTimes = 0;
	while(nTryTimes < 3)
	{
		{
			voCOMXAutoLock lock(&m_tmBuffer);
			pEmptyBuffer = m_listBufferEmpty.GetHead();
		}

		if(NULL != pEmptyBuffer)
			break;
		else
		{
			if(OMX_FALSE == m_bEmptyBufferNeedRetry)
				return (OMX_ERRORTYPE)VO_ERR_IOMXDEC_NeedRetry;
		}

		voOMXOS_Sleep(2);
		nTryTimes++;

		if(OMX_FALSE == CanBuffersWork())
			return OMX_ErrorNone;
	}

	if(OMX_FALSE == CanBuffersWork())
		return OMX_ErrorNone;

	voCOMXAutoLock lock(&m_tmBuffer);
	if(NULL != pEmptyBuffer)
	{
		VOLOGR("222 EmptyBuffer 0x%08X, 0x%08X, system time %d", pEmptyBuffer->hBuffer, pEmptyBuffer->pData, voOMXOS_GetSysTime());

		m_listBufferEmpty.Remove(pEmptyBuffer);
		// copy buffer to IMemory
		if(pInput->Length > 0)
			memcpy(pEmptyBuffer->pData, pInput->Buffer, pInput->Length);

		OMX_U32 nOffset = 0;
		OMX_U32 nLength = pInput->Length;
		OMX_U32 nFlags = 0;
		if(pInput->Length > 0)
			nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
		// if buffer is video sequence header, add OMX_BUFFERFLAG_CODECCONFIG flag
		if(m_pComponent->m_nHeadData == pInput->Length && !memcmp(m_pComponent->m_pHeadData, pInput->Buffer, pInput->Length))
			nFlags |= OMX_BUFFERFLAG_CODECCONFIG;
		if(bEOS)
			nFlags |= OMX_BUFFERFLAG_EOS;
		OMX_TICKS nTimeStamp = pInput->Time * 1000;		// VO (ms) --> OMX (¦Ìs)

		// some HW decoders (DUCATI) will be abnormal if we call EmptyThisBuffer when PortSettingsChanged, East 20130109
		m_pComponent->m_tmPortSettingsChanging.Lock();
		OMX_ERRORTYPE errType;
		unsigned int nFrameType;
		if(m_bDropFrame == OMX_TRUE)
		{
			if(NULL == m_hVideoParser)
			{
				// initialize video parser
				VO_U32 nRC = voGetVideoParserAPI(&m_sVideoParserAPI, eVOCodec);
				if(VO_ERR_NONE == nRC)
					nRC = m_sVideoParserAPI.Init(&m_hVideoParser);
			}
			m_sVideoParserAPI.Process(m_hVideoParser, pInput);
			m_sVideoParserAPI.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_FRAMETYPE, &nFrameType);

			if(nFrameType != VO_VIDEO_FRAME_I || (nLength == 0 && strstr(m_pComponent->m_szDevName, "Sony_LT25")))
			{
				VOLOGI("frame type %d length %d, drop it!!", nFrameType, nLength);
				errType = OMX_ErrorUndefined;
			}
			else
			{
				VOLOGI("frame type %d length %d, stop drop.", nFrameType, nLength);
				m_bDropFrame = OMX_FALSE;

				errType = m_pComponent->EmptyBuffer(pEmptyBuffer->hBuffer, nOffset, nLength, nFlags, nTimeStamp);
			}
		}
		else
			errType = m_pComponent->EmptyBuffer(pEmptyBuffer->hBuffer, nOffset, nLength, nFlags, nTimeStamp);

		m_pComponent->m_tmPortSettingsChanging.Unlock();

		if(OMX_ErrorNone != errType)
		{
			VOLOGW("failed to EmptyBuffer 0x%08X", errType);
			m_listBufferEmpty.Add(pEmptyBuffer);
		}
	}
	else
	{
		VOLOGR("failed to get valid buffer");
		return (OMX_ERRORTYPE)VO_ERR_IOMXDEC_NeedRetry;
	}

	return OMX_ErrorNone;
}

void CvoIOMXInPort::OnEmptyBufferDone(IOMX::buffer_id hBuffer)
{
	VOLOGR("EmptyBufferDone 0x%08X", hBuffer);

	voCOMXAutoLock lock(&m_tmBuffer);
	for(OMX_U32 i = 0; i < m_nBufferCount; i++)
	{
		if(m_pBuffers[i].hBuffer == hBuffer)
		{
			m_listBufferEmpty.Add(m_pBuffers + i);
			break;
		}
	}
}

OMX_ERRORTYPE CvoIOMXInPort::SetEmptyBufferNeedRetry(OMX_BOOL bNeedRetry)
{
	VOLOGI("SetEmptyBufferNeedRetry %d", bNeedRetry);

	m_bEmptyBufferNeedRetry = bNeedRetry;

	return OMX_ErrorNone;
}
