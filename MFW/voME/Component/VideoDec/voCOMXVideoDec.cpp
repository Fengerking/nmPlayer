	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXVideoDec.cpp

	Contains:	voCOMXVideoDec class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXBase.h"
#include "voOMX_Index.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"
#include "cmnMemory.h"
#include "voH264.h"
#include "voOMXFile.h"
#include "voCOMXVideoDec.h"
#include "fVideoHeadDataInfo.h"
#include "voDivXDRM.h"

#define LOG_TAG "voCOMXVideoDec"
#include "voLog.h"

extern VO_TCHAR * g_pvoOneWorkingPath;

voCOMXVideoDec::voCOMXVideoDec(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_nCoding (OMX_VIDEO_CodingAutoDetect)
	, m_nExtSize (0)
	, m_pExtData (NULL)
	, m_nFrameTime (1)
	, m_nPrevFrameTime (0)
	, m_bFrameDropped (OMX_FALSE)
	, m_nDropBFrames (0)
	, m_nErrorFrames(0)
	, m_bUseTimeStampOfDecoder(OMX_FALSE)
	, m_llPrevTimeStamp (0x7fffffffffffffffLL)
	, mnSourceType(1)
	, m_pVideoDec (NULL)
	, m_pVideoMemOP (NULL)
	, m_nStartTime (0)
	, m_bDualCore (OMX_FALSE)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Video.Decoder.XXX");

	m_inBuffer.Length = 0;
	m_outBuffer.Time = 0;

	memset (&m_videoType, 0, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;

	VO_TCHAR szCfgFile[256];
	if (g_pvoOneWorkingPath == NULL)
		vostrcpy (szCfgFile, _T("vomeplay.cfg"));
	else
	{
		vostrcpy (szCfgFile, g_pvoOneWorkingPath);
		vostrcat (szCfgFile, _T("vomeplay.cfg"));
	}

	m_pCfgComponent = new CBaseConfig ();
	m_pCfgComponent->Open (szCfgFile);

	m_nDumpRuntimeLog = m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpRuntimeLog", 0);

	m_pCfgCodec = new CBaseConfig ();
	FILE * hFile = fopen ("/sdcard/pvcodec.mmp", "rb");
	if (hFile != NULL)
		fclose (hFile);

	VO_BOOL bOpenCfgFile = VO_FALSE;
	if (hFile != NULL)
	{
		if (g_pvoOneWorkingPath == NULL)
			vostrcpy (szCfgFile, _T("vommcodec_pv.cfg"));
		else
		{
			vostrcpy (szCfgFile, g_pvoOneWorkingPath);
			vostrcat (szCfgFile, _T("vommcodec_pv.cfg"));
		}
		bOpenCfgFile = m_pCfgCodec->Open (szCfgFile);
	}
	
	if (!bOpenCfgFile)
	{
		if (g_pvoOneWorkingPath == NULL)
			vostrcpy (szCfgFile, _T("vommcodec.cfg"));
		else
		{
			vostrcpy (szCfgFile, g_pvoOneWorkingPath);
			vostrcat (szCfgFile, _T("vommcodec.cfg"));
		}
		m_pCfgCodec->Open (szCfgFile);
	}

	if (m_pCfgComponent->GetItemValue (m_pName, (char *)"DumpLog", 0) > 0)
	{
		m_nPfmFrameSize = 10240;
		m_pPfmFrameTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmCodecThreadTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmCompnThreadTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmMediaTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmSystemTime = new OMX_U32[m_nPfmFrameSize];
	}

	if (m_pCfgComponent->GetItemValue (m_pName, (char *)"DualCore", 0) > 0)
		m_bDualCore = OMX_TRUE;

	char * pFile = m_pCfgComponent->GetItemText (m_pName, (char*)"DumpInputDataFile");
	if (pFile != NULL)
	{
#ifdef _WIN32
		TCHAR szFile[256];
		memset (szFile, 0, sizeof (szFile));
		MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); \
		m_pDumpInputFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_WRITE_ONLY);		
#else
		m_pDumpInputFile = voOMXFileOpen (pFile, VOOMX_FILE_WRITE_ONLY);
#endif // _WIN32
		m_nDumpInputFormat = m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpInputDataFormat", 0);
		VOLOGI ("Create Input file %s, Format is %d The handle is 0X%08X.", pFile, (int)m_nDumpInputFormat,(int)m_pDumpInputFile);
	}
	pFile = m_pCfgComponent->GetItemText (m_pName, (char*)"DumpOututDataFile");
	if (pFile != NULL)
	{
#ifdef _WIN32
		TCHAR szFile[256];
		memset (szFile, 0, sizeof (szFile));
		MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); \
		m_pDumpOutputFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_WRITE_ONLY);		
#else
		m_pDumpOutputFile = voOMXFileOpen (pFile, VOOMX_FILE_WRITE_ONLY);
#endif // _WIN32
		m_nDumpOutputFormat = m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpOutputDataFormat", 0);
		VOLOGI ("Create Input file %s, Format is %d The handle is 0X%08X.", pFile, (int)m_nDumpOutputFormat, (int)m_pDumpOutputFile);
	}
	m_nPassCodec = m_pCfgComponent->GetItemValue (m_pName, (char*)"PassCodec", 0);
	m_nLogLevel = m_pCfgComponent->GetItemValue (m_pName, (char*)"ShowLogLevel", 0);

	OMX_PTR hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoMPEG4Dec.so"), VOOMX_FILE_READ_ONLY);
	if (hCodecFile != NULL)
	{
		m_nCoding = OMX_VIDEO_CodingMPEG4;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoH264Dec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_VIDEO_CodingAVC;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoWMVDec.so"), VOOMX_FILE_READ_ONLY)) != NULL ||
		(hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoVC1Dec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_VIDEO_CodingWMV;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoRealVideoDec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_VIDEO_CodingRV;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoMPEG2Dec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_VIDEO_CodingMPEG2;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoMJPEGDec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_VIDEO_CodingMJPEG;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoVP6Dec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingVP6;
		voOMXFileClose (hCodecFile);
	}
}

voCOMXVideoDec::~voCOMXVideoDec(void)
{
	if (m_pExtData != NULL)
	{
		voOMXMemFree (m_pExtData);
		m_pExtData = NULL;
	}

	if (m_pVideoDec != NULL)
	{
		delete m_pVideoDec;
		m_pVideoDec = NULL;
	}

	delete m_pCfgCodec;

//	m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);
}


OMX_ERRORTYPE voCOMXVideoDec::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nParamIndex)
	{
	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			if (pVideoFormat->nPortIndex == 0)
			{
				pVideoFormat->eCompressionFormat = m_nCoding;
				//pVideoFormat->eCompressionFormat = OMX_VIDEO_CodingRV; // temp modified gtxia
				pVideoFormat->eColorFormat = OMX_COLOR_FormatUnused;
			}
			else
			{
				pVideoFormat->eCompressionFormat = OMX_VIDEO_CodingUnused;
				pVideoFormat->eColorFormat = OMX_COLOR_FormatYUV420Planar;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexTimeStampOwner:
		{
			*((OMX_U32*)pComponentParameterStructure) = m_bUseTimeStampOfDecoder ? 1 : 0;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoProfileLevelQuerySupported:
		{
			OMX_VIDEO_PARAM_PROFILELEVELTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PROFILELEVELTYPE *) pComponentParameterStructure;
			errType = CheckParam ((OMX_COMPONENTTYPE *)hComponent, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_PROFILELEVELTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			errType = OMX_ErrorNone;
		}
		break;

	default:
		errType = voCOMXCompFilter::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXVideoDec::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE		errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{
	case OMX_IndexParamPortDefinition:
		{
			OMX_PARAM_PORTDEFINITIONTYPE * pType = (OMX_PARAM_PORTDEFINITIONTYPE *)pComponentParameterStructure;
			errType = CheckParam (pComp, pType->nPortIndex, pType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pType->eDomain != OMX_PortDomainVideo)
				return OMX_ErrorPortsNotCompatible;

			errType = m_ppPorts[pType->nPortIndex]->SetPortType (pType);
			if (errType != OMX_ErrorNone)
				return errType;

			if (pType->nPortIndex == 0)
			{
				voOMXMemCopy (&m_videoType, &pType->format.video, sizeof (OMX_VIDEO_PORTDEFINITIONTYPE));

				OMX_PARAM_PORTDEFINITIONTYPE * pOutType;
				m_pOutput->GetPortType (&pOutType);
				pOutType->format.video.nFrameWidth = m_videoType.nFrameWidth;
				pOutType->format.video.nFrameHeight = m_videoType.nFrameHeight;
				pOutType->format.video.nStride = m_videoType.nFrameWidth;
				pOutType->format.video.nBitrate = m_videoType.nBitrate;
				pOutType->format.video.xFramerate = m_videoType.xFramerate;

				pOutType->nBufferSize = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;

				if (m_videoType.xFramerate == 0)
					m_nFrameTime = 1;
				else
				{
					if (((m_videoType.xFramerate >> 16) & 0XFFFF) == 0)
						m_nFrameTime = 40;
					else
						m_nFrameTime = 1000 * (m_videoType.xFramerate & 0XFFFF) / ((m_videoType.xFramerate >> 16) & 0XFFFF);
				}
			}
		}
		break;

	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex == 0)
			{
				m_nCoding = pVideoFormat->eCompressionFormat;

				if (m_nCoding == OMX_VIDEO_CodingMPEG2)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingH263)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingMPEG4)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingWMV)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingRV)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingAVC)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingMJPEG)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_VIDEO_CodingRV)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_VIDEO_CODINGTYPE)OMX_VO_VIDEO_CodingS263)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_VIDEO_CODINGTYPE)OMX_VO_VIDEO_CodingVP6)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_VIDEO_CODINGTYPE)OMX_VO_VIDEO_CodingDIV3)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_VIDEO_CODINGTYPE)OMX_VO_VIDEO_CodingVP8)
					errType = OMX_ErrorNone;
				else
					errType = OMX_ErrorComponentNotFound;
			}
			else
			{
				errType = OMX_ErrorNone;
			}
		}
		break;

	case OMX_IndexParamVideoMpeg4:
		{
			OMX_VIDEO_PARAM_MPEG4TYPE * pVideoFormat = (OMX_VIDEO_PARAM_MPEG4TYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_MPEG4TYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			if (pVideoFormat->bGov == OMX_TRUE)
				return OMX_ErrorUndefined;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoWmv:
		{
			OMX_VIDEO_PARAM_WMVTYPE * pVideoFormat = (OMX_VIDEO_PARAM_WMVTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pVideoFormat->nPortIndex, pVideoFormat, sizeof (OMX_VIDEO_PARAM_WMVTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pVideoFormat->nPortIndex != 0)
				return OMX_ErrorBadPortIndex;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamCommonExtraQuantData:
		{
			OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *)pComponentParameterStructure;
			
			//errType = voOMXBase_CheckHeader(pExtData, sizeof(OMX_OTHER_EXTRADATATYPE));
			//if (errType != OMX_ErrorNone)
			//	return errType;

			if (m_pExtData != NULL)
				voOMXMemFree (m_pExtData);
			m_pExtData = NULL;
			m_nExtSize = 0;

			if (pExtData->nDataSize > 0)
			{
				m_nExtSize = pExtData->nDataSize;
				m_pExtData = (OMX_S8*)voOMXMemAlloc (m_nExtSize);
				if (m_pExtData != NULL)
					voOMXMemCopy (m_pExtData, pExtData->data, m_nExtSize);
				else
					m_nExtSize = 0;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexVideoMemOperator:
		{
			m_pVideoMemOP = (VO_MEM_VIDEO_OPERATOR *)pComponentParameterStructure;
			if (m_pVideoDec != NULL)
				m_pVideoDec->SetParam (VO_PID_VIDEO_VIDEOMEMOP, m_pVideoMemOP);

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamStandardComponentRole:
		{
			OMX_PARAM_COMPONENTROLETYPE * pRoleType = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;

			errType = voOMXBase_CheckHeader(pRoleType, sizeof(OMX_PARAM_COMPONENTROLETYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			m_nCoding = OMX_VIDEO_CodingUnused;
			if (!strcmp ((char *)pRoleType->cRole, "video_decoder.avc"))
				m_nCoding = OMX_VIDEO_CodingAVC;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.mpeg4"))
				m_nCoding = OMX_VIDEO_CodingMPEG4;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.wmv"))
				m_nCoding = OMX_VIDEO_CodingWMV;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.h263"))
				m_nCoding = OMX_VIDEO_CodingH263;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.mpeg2"))
				m_nCoding = OMX_VIDEO_CodingMPEG2;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.s263"))
				m_nCoding = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingS263;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.vp6"))
				m_nCoding = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingVP6;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.div3"))
				m_nCoding = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingDIV3;
			else if (!strcmp ((char *)pRoleType->cRole, "video_decoder.vp8"))
				m_nCoding = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingVP8;			

			if (m_nCoding != OMX_VIDEO_CodingUnused)
				errType = OMX_ErrorNone;
			else
				errType = OMX_ErrorInvalidComponent;

			if(NULL == m_pInput)
				CreatePorts();

			OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
			m_pInput->GetPortType (&pPortType);
			pPortType->nBufferCountActual = 2;
			pPortType->nBufferSize = 1024 * 64;

			m_pOutput->GetPortType (&pPortType);
			pPortType->nBufferCountActual = 2;
		}
		break;

	case OMX_VO_IndexTimeStampOwner:
		{
			m_bUseTimeStampOfDecoder = (*((OMX_U32*)pComponentParameterStructure) == 1) ? OMX_TRUE : OMX_FALSE;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexSourceType:
		{
			mnSourceType = *(OMX_U32 *)pComponentParameterStructure;
			if(m_pVideoDec != NULL)
				m_pVideoDec->SetSourceType(mnSourceType);

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexCPUCoreNum:
		{
			if(*(OMX_U32 *)pComponentParameterStructure > 1)
				m_bDualCore = OMX_TRUE;

			if(m_pVideoDec != NULL)
				m_pVideoDec->SetParam(VO_PID_COMMON_CPUNUM, pComponentParameterStructure);

			errType = OMX_ErrorNone;
		}
		break;
		
	case OMX_VO_IndexSourceDrm:
		{
			VOLOGI("OMX_VO_IndexSourceDrm");

			m_pDrm = (OMX_VO_SOURCEDRM_CALLBACK *)pComponentParameterStructure;

			errType = OMX_ErrorNone;
		}
		break;

	default:
		errType = voCOMXCompFilter::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXVideoDec::ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_OUT OMX_U8 *cRole,
													OMX_IN OMX_U32 nIndex)
{
	strcat((char*)cRole, "\0");
	if (nIndex == 0)
		strcpy ((char*)cRole, "video_decoder.avc");
	else if (nIndex == 1)
		strcpy ((char*)cRole, "video_decoder.mpeg4");
	else if (nIndex == 2)
		strcpy ((char*)cRole, "video_decoder.wmv");
	else if (nIndex == 3)
		strcpy ((char*)cRole, "video_decoder.h263");
	else if (nIndex == 4)
		strcpy ((char*)cRole, "video_decoder.rv");
	else if (nIndex == 5)
		strcpy ((char*)cRole, "video_decoder.s263");
	else if (nIndex == 6)
		strcpy ((char*)cRole, "video_decoder.vp6");
	else if (nIndex == 7)
		strcpy ((char*)cRole, "video_decoder.div3");
	else if (nIndex == 8)
		strcpy ((char*)cRole, "video_decoder.mpeg2");
	else if (nIndex == 9)
		strcpy ((char*)cRole, "video_decoder.vp8");
	else 
		return OMX_ErrorNoMore;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoDec::SetNewState (OMX_STATETYPE newState)
{
	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_pVideoDec == NULL)
			CreateVideoDec ();
	}
	else if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{
		if (m_pVideoDec != NULL)
			m_pVideoDec->Flush ();

		m_inBuffer.Length = 0;
		m_nDropBFrames = 0;
	}
	else if (newState == OMX_StateIdle)
	{
		if (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle)
			ResetPortBuffer ();

		if (m_pVideoDec != NULL)
			m_pVideoDec->Flush ();

		m_inBuffer.Length = 0;
		m_nDropBFrames = 0;

		m_nStartTime = 0;
		m_llPrevTimeStamp = 0x7fffffffffffffffLL;
	}

	m_nPrevFrameTime = 0;
	m_bFrameDropped = OMX_FALSE;

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXVideoDec::CreateVideoDec (void)
{
	VO_U32 nCodec = 0;

	if (m_nCoding == OMX_VIDEO_CodingMPEG4)
		nCodec = VO_VIDEO_CodingMPEG4;
	else if (m_nCoding == OMX_VIDEO_CodingH263)
		nCodec = VO_VIDEO_CodingH263;
	else if (m_nCoding == (OMX_VIDEO_CODINGTYPE)OMX_VO_VIDEO_CodingS263)
		nCodec = VO_VIDEO_CodingS263;
	else if (m_nCoding == (OMX_VIDEO_CODINGTYPE)OMX_VO_VIDEO_CodingVP6)
		nCodec = VO_VIDEO_CodingVP6;
	else if (m_nCoding == (OMX_VIDEO_CODINGTYPE)OMX_VO_VIDEO_CodingDIV3)
		nCodec = VO_VIDEO_CodingDIVX;
	else if (m_nCoding == (OMX_VIDEO_CODINGTYPE)OMX_VO_VIDEO_CodingVP8)
		nCodec = VO_VIDEO_CodingVP8;
	else if (m_nCoding == OMX_VIDEO_CodingAVC)
		nCodec = VO_VIDEO_CodingH264;
	else if (m_nCoding == OMX_VIDEO_CodingWMV)
	{// need to check wmv or vc1 for visualon software decoder
		nCodec = VO_VIDEO_CodingWMV;
		if(m_nExtSize>0)
		{
			VO_CODECBUFFER buf;
			buf.Buffer = (VO_PBYTE)m_pExtData;
			buf.Length = m_nExtSize;
			if(!voIsWMVCodec(&buf))
				nCodec = VO_VIDEO_CodingVC1;
		}	
	}
	else if (m_nCoding == OMX_VIDEO_CodingMJPEG)
		nCodec = VO_VIDEO_CodingMJPEG;
	else if (m_nCoding == OMX_VIDEO_CodingRV)
		nCodec = VO_VIDEO_CodingRV;
	else if (m_nCoding == OMX_VIDEO_CodingMPEG2)
		nCodec = VO_VIDEO_CodingMPEG2;

	m_pVideoDec = new CVideoDecoder (NULL, nCodec, m_pMemOP);
	if (m_pVideoDec == NULL)
	{
		VOLOGE ("It could not create the video decoder.");
		return OMX_ErrorInsufficientResources;
	}
	m_pVideoDec->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibOP);
	m_pVideoDec->SetWorkPath ((VO_TCHAR *)m_pWorkPath);
	m_pVideoDec->SetParam (VO_PID_VIDEO_VIDEOMEMOP, m_pVideoMemOP);

	m_pVideoDec->SetConfig (m_pCfgCodec);
	m_pVideoDec->SetOMXComp (VO_TRUE);

	VO_VIDEO_FORMAT	fmtVideo;
	fmtVideo.Width = m_videoType.nFrameWidth;
	fmtVideo.Height = m_videoType.nFrameHeight;

	if (m_videoType.cMIMEType != NULL)
		m_pVideoDec->SetFourCC (*(VO_U32 *)m_videoType.cMIMEType);

	OMX_U32 nRC = VO_ERR_NONE;
	VOLOGI("Video Decoder Init ExtSize = %d " , (int)m_nExtSize);
	nRC = m_pVideoDec->Init ((VO_PBYTE)m_pExtData, m_nExtSize, &fmtVideo);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_pVideoDec->Init was failed. 0X%08X", (int)nRC);
		m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_CodecUnsupported, 0, 0, NULL);
		return OMX_ErrorResourcesLost;
	}

	m_pVideoDec->SetSourceType(mnSourceType);

	if(m_bDualCore)
	{
		int DualCore = 2;
		m_pVideoDec->SetParam (VO_PID_COMMON_CPUNUM, &DualCore);
	}

//	int nOutputMode = 1;
//	m_pVideoDec->SetParam (VO_PID_VIDEO_OUTPUTMODE, &nOutputMode);

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoDec::Flush (OMX_U32	nPort)
{
	voCOMXAutoLock lokc (&m_mutCodec);

	OMX_ERRORTYPE errType = voCOMXCompFilter::Flush (nPort);

	m_inBuffer.Length = 0;
	m_nStartTime = 0;

	m_nPrevFrameTime = 0;
	m_bFrameDropped = OMX_FALSE;
	m_nDropBFrames = 0;

	m_llPrevTimeStamp = 0x7fffffffffffffffLL;

	if(m_nCoding == OMX_VIDEO_CodingAVC && m_bUseTimeStampOfDecoder)
	{
		if (m_pVideoDec != NULL)
			delete m_pVideoDec;
		m_pVideoDec = NULL;
	}
	else
	{
		if (m_pVideoDec != NULL)
			m_pVideoDec->Flush ();
	}

	return errType;
}

OMX_ERRORTYPE voCOMXVideoDec::OnDisablePort (OMX_BOOL bFinished, OMX_U32 nPort)
{
	if(OMX_TRUE == bFinished && 0 == nPort)
	{
		if (m_pVideoDec != NULL)
		{
			delete m_pVideoDec;
			m_pVideoDec = NULL;
		}
	}

	return voCOMXCompFilter::OnDisablePort (bFinished, nPort);
}

OMX_ERRORTYPE voCOMXVideoDec::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	VOLOGR ("Name: %s. Size: %d, Time %d", m_pName, pInput->nFilledLen, (int)pInput->nTimeStamp);
	voCOMXAutoLock lokc (&m_mutCodec);

	if (m_pVideoDec == NULL)
		CreateVideoDec ();
	if (m_pVideoDec == NULL)
	{
		*pEmptied = OMX_TRUE;
		m_inBuffer.Length = 0;

		pOutput->nFilledLen = 0;
		*pFilled = OMX_TRUE;
		return OMX_ErrorUndefined;
	}

	OMX_U32 nRC = 0;

	if (pInput->nFilledLen <= 0)
	{
		*pEmptied = OMX_TRUE;
		return OMX_ErrorNone;
	}

	if (m_nFrameTime <= 1)
	{
		if (m_nPrevFrameTime > 0)
			m_nFrameTime = (OMX_U32)(pInput->nTimeStamp - m_nPrevFrameTime);
	}

	if ((pInput->nFlags & OMX_BUFFERFLAG_DECODEONLY) == OMX_BUFFERFLAG_DECODEONLY)
		m_pVideoDec->SetDelayTime (500);
	else
		m_pVideoDec->SetDelayTime ((VO_S32)pInput->pOutputPortPrivate);

	if (m_inBuffer.Length == 0)
	{
		m_inBuffer.Buffer = pInput->pBuffer + pInput->nOffset;
		m_inBuffer.Length = pInput->nFilledLen;
		m_inBuffer.Time = pInput->nTimeStamp;

		VOLOGR ("Time %d, Size %d, Type %d", (VO_U32)m_inBuffer.Time, m_inBuffer.Length, m_pVideoDec->GetFrameType (&m_inBuffer));

		int nDelayTime = (m_nDropBFrames + 1) * 80;
		if (((VO_S32)pInput->pOutputPortPrivate > nDelayTime) || ((pInput->nFlags & OMX_BUFFERFLAG_DECODEONLY) == OMX_BUFFERFLAG_DECODEONLY) || m_bFrameDropped)
		{
		//	if (m_pVideoDec->GetFrameType (&m_inBuffer) == VO_VIDEO_FRAME_B)
		// Modified by Eric, for some special files, B frames are also used as reference frame, cannot be dropped, only for h264 will IsRefFrame make sense 
			if (m_pVideoDec->GetFrameType (&m_inBuffer) == VO_VIDEO_FRAME_B && !m_pVideoDec->IsRefFrame(&m_inBuffer))
			{
				*pEmptied = OMX_TRUE;
				m_inBuffer.Length = 0;

				pOutput->nFilledLen = 0;
				*pFilled = OMX_FALSE;

				VOLOGR ("Drop the B frame. timestamp %d delaytime %d", (VO_U32)pInput->nTimeStamp, (VO_S32)pInput->pOutputPortPrivate);

				m_nDropBFrames++;

				return OMX_ErrorNone;
			}

			m_bFrameDropped = OMX_FALSE;
		}

		if ((pInput->nFlags & OMX_BUFFERFLAG_SYNCFRAME) == OMX_BUFFERFLAG_SYNCFRAME)
		{
			if (pInput->nTimeStamp - m_nPrevFrameTime + 8 >= m_nFrameTime * 2)
				m_bFrameDropped = OMX_TRUE;
		}
		m_nPrevFrameTime = pInput->nTimeStamp;

		m_nDropBFrames = 0;
		nRC = m_pVideoDec->SetInputData (&m_inBuffer);
		if (nRC != VO_ERR_NONE)
		{
			if(nRC != VO_ERR_INPUT_BUFFER_SMALL && m_pCallBack)
				m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_CodecPassableError, 2, 0, NULL);

			*pEmptied = OMX_TRUE;
			m_inBuffer.Length = 0;

			pOutput->nFilledLen = 0;
			*pFilled = OMX_FALSE;

			VOLOGW ("Set Input Failed! 0X%08X", (int)nRC);
			return OMX_ErrorNone;
		}

		m_nStartTime = pInput->nTimeStamp;
	}

	if (m_nLogLevel > 0)
	{
		if (m_nLogSysStartTime == 0)
			m_nLogSysStartTime = voOMXOS_GetSysTime ();

		VOLOGI ("Sys Time: %d  frame time is %d, Diff %d, size %d", (int)(voOMXOS_GetSysTime () - m_nLogSysStartTime), (int)pInput->nTimeStamp, (int)((voOMXOS_GetSysTime () - m_nLogSysStartTime) - pInput->nTimeStamp),(int)pInput->nFilledLen);
		m_nLogSysCurrentTime = voOMXOS_GetSysTime ();
	}

	memset(&m_fmtOutputVideo, 0, sizeof(VO_VIDEO_FORMAT));
	m_outBuffer.Time = 0;

	if(m_nDumpRuntimeLog)
	{
		VOLOGI ("systime %d, input time %d", (int)voOMXOS_GetSysTime(), (int)pInput->nTimeStamp);
	}
	VO_BOOL bContinuous = VO_TRUE;
	nRC = GetOutputData (&m_outBuffer, &m_fmtOutputVideo, (VO_U32)pInput->nTimeStamp, &bContinuous);
	if(m_nDumpRuntimeLog)
	{
		VOLOGI ("systime %d, nRC 0x%08X, bContinuous %d", (int)voOMXOS_GetSysTime(), (int)nRC, bContinuous);
	}

	if (m_nLogLevel > 0)
	{
		VOLOGI ("GetOutputData used time %d., Buffer: %08X, Time: %d   Result 0X%08X", (int)(voOMXOS_GetSysTime () - m_nLogSysCurrentTime), (int)m_outBuffer.Buffer[0], (int)m_outBuffer.Time, (int)nRC);
	}

	if(VO_ERR_NONE != nRC && VO_ERR_INPUT_BUFFER_SMALL != nRC)
	{
		if(m_pCallBack)
		{
			if(m_nCoding == OMX_VIDEO_CodingAVC &&  m_fmtOutputVideo.Type != VO_VIDEO_FRAME_B)
			{
				m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_CodecPassableError, m_nCoding , 0 , 0);
			}
			else
				m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_CodecPassableError, 2, 0, NULL);
		}
	}

	VOLOGR ("Name: %s. GetOutputData Buffer Addr: 0X%08X, Result 0X%08X. Video Size: W %d H %d", m_pName, m_outBuffer.Buffer[0], nRC, m_fmtOutputVideo.Width, m_fmtOutputVideo.Height);

	OMX_BOOL bH263NoChanged = OMX_FALSE;
	if (nRC == VO_ERR_NONE && m_fmtOutputVideo.Width > 0 && m_fmtOutputVideo.Height > 0)
	{
		if (m_nCoding == OMX_VIDEO_CodingH263 && 
			m_videoType.nFrameWidth == 320 && m_fmtOutputVideo.Width == 352 && 
			m_videoType.nFrameHeight == 240 && m_fmtOutputVideo.Height == 288)
				bH263NoChanged = OMX_TRUE;

		m_nErrorFrames = 0;

//		if (m_videoType.nFrameWidth != fmtVideo.Width || m_videoType.nFrameHeight != fmtVideo.Height)
		if (!bH263NoChanged && (m_videoType.nFrameWidth <= (OMX_U32)m_fmtOutputVideo.Width - 16 ||  m_videoType.nFrameWidth >= (OMX_U32)m_fmtOutputVideo.Width + 16 || 
			m_videoType.nFrameHeight <= (OMX_U32)m_fmtOutputVideo.Height - 16 || m_videoType.nFrameHeight >= (OMX_U32)m_fmtOutputVideo.Height + 16))
		{
			VOLOGI ("Name: %s. Video size was changed. W: %d  H %d", m_pName, (int)m_fmtOutputVideo.Width, (int)m_fmtOutputVideo.Height);

			pOutput->nFilledLen = 0;
			m_pOutput->ReturnBuffer (pOutput);
			m_pOutputBuffer = NULL;
			*pFilled = OMX_FALSE;

			m_videoType.nFrameWidth = m_fmtOutputVideo.Width;
			m_videoType.nFrameHeight = m_fmtOutputVideo.Height;

			OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
			m_pOutput->GetPortType (&pPortType);
			pPortType->format.video.nFrameWidth = m_fmtOutputVideo.Width;
			pPortType->format.video.nFrameHeight = m_fmtOutputVideo.Height;
			pPortType->nBufferSize = m_videoType.nFrameWidth * m_videoType.nFrameHeight * 2;

			m_pOutput->SettingsChanging();

			if (m_pCallBack != NULL)
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventPortSettingsChanged, 1, 0, NULL);

			return OMX_ErrorNone;
		}
	}

	if (nRC == VO_ERR_NONE && !bContinuous)
		m_inBuffer.Length = 0;

	if (nRC == VO_ERR_INPUT_BUFFER_SMALL)
	{
		*pEmptied = OMX_TRUE;
		m_inBuffer.Length = 0;

		pOutput->nFilledLen = 0;
		if(pInput->nFlags & OMX_BUFFERFLAG_EOS)
			*pFilled = OMX_TRUE;
		else
			*pFilled = OMX_FALSE;
	}
	else if (nRC == VO_ERR_NONE && m_outBuffer.Buffer[0] != NULL)
	{
		m_nErrorFrames = 0;

		if ((pInput->nFlags & OMX_BUFFERFLAG_DECODEONLY) == OMX_BUFFERFLAG_DECODEONLY)
		{
			if ((pInput->nFlags & OMX_BUFFERFLAG_EOS) == 0)
			{
				if(m_inBuffer.Length <= 0)
					*pEmptied = OMX_TRUE;

				pOutput->nFilledLen = 0;
				*pFilled = OMX_FALSE;

				return  OMX_ErrorNone;
			}
		}

		if (m_pOutput->IsTunnel ())
		{
			pOutput->nFilledLen = sizeof (VO_VIDEO_BUFFER);
			memcpy (pOutput->pBuffer, &m_outBuffer, pOutput->nFilledLen);

			pOutput->pOutputPortPrivate = &m_fmtOutputVideo;
		}
		else
		{
			VO_S32 nYUVWidth = (m_fmtOutputVideo.Width + 15) & ~15;
			VO_S32 nYUVHeight = m_fmtOutputVideo.Height;
			if(bH263NoChanged)
			{
				nYUVWidth = (m_videoType.nFrameWidth + 15) & ~15;
				nYUVHeight = m_videoType.nFrameHeight;
			}

			VO_PBYTE pDstBuffer = pOutput->pBuffer;
			for (VO_S32 i = 0; i < nYUVHeight; i++)
				memcpy (pDstBuffer + nYUVWidth * i, m_outBuffer.Buffer[0] + m_outBuffer.Stride[0] * i, nYUVWidth);

			VO_U32 nHalfWidth = nYUVWidth / 2;
			pDstBuffer = pOutput->pBuffer + nYUVWidth * nYUVHeight;
			for (VO_S32 i = 0; i < nYUVHeight / 2; i++)
				memcpy (pDstBuffer + nHalfWidth * i, m_outBuffer.Buffer[1] + m_outBuffer.Stride[1] * i, nHalfWidth);

			pDstBuffer = pOutput->pBuffer + nYUVWidth * nYUVHeight * 5 / 4;
			for (VO_S32 i = 0; i < nYUVHeight / 2; i++)
				memcpy (pDstBuffer + nHalfWidth * i, m_outBuffer.Buffer[2] + m_outBuffer.Stride[2] * i, nHalfWidth);

			pOutput->nFilledLen = nYUVWidth * nYUVHeight * 3 / 2;
		}

		VOLOGR ("m_bUseTimeStampOfDecoder %d, decoder time %d, external time %d", m_bUseTimeStampOfDecoder, (OMX_U32)m_outBuffer.Time, (OMX_U32)m_nStartTime);
		pOutput->nTimeStamp = m_bUseTimeStampOfDecoder ? m_outBuffer.Time : m_nStartTime;
		m_nStartTime = m_nStartTime + m_nFrameTime;

		if (!m_bUseTimeStampOfDecoder)
		{
			if(m_llPrevTimeStamp != 0x7fffffffffffffffLL && m_llPrevTimeStamp + 5 > pOutput->nTimeStamp && m_llPrevTimeStamp < pOutput->nTimeStamp + 30000)
				pOutput->nTimeStamp = m_llPrevTimeStamp + m_nFrameTime;
			m_llPrevTimeStamp = pOutput->nTimeStamp;
		}

		*pFilled = OMX_TRUE;

		if(m_inBuffer.Length <= 0)
			*pEmptied = OMX_TRUE;
	}
	else
	{
		if (nRC == VO_ERR_NONE && m_inBuffer.Length > 0)
		{
			*pEmptied = OMX_FALSE;
			m_nErrorFrames = 0;
		}
		else
		{
			if(m_nCoding == OMX_VIDEO_CodingAVC && nRC == (VO_H264_ERR_NotSupportProfile & 0X8000FFFF) && m_pCallBack != NULL) 
			{
				m_nErrorFrames++;
				VOLOGI ("Video: Not supported profile  ! m_nErrorFrames: %d ", (int)m_nErrorFrames);
				if(m_nErrorFrames > 15)
					m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventError, 0, 0, 0);
			}

			*pEmptied = OMX_TRUE;
			m_inBuffer.Length = 0;
		}

		pOutput->nFilledLen = 0;
		if(pInput->nFlags & OMX_BUFFERFLAG_EOS)
			*pFilled = OMX_TRUE;
		else
			*pFilled = OMX_FALSE;

	}

	return OMX_ErrorNone;
}

VO_U32 voCOMXVideoDec::GetOutputData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat, VO_U32 nStart, VO_BOOL * pbContinuous)
{
	if (m_pVideoDec == NULL)
		return 0;

	if (m_nPfmFrameIndex < m_nPfmFrameSize)
	{
		m_pPfmFrameTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime ();
		m_pPfmCodecThreadTime[m_nPfmFrameIndex] = voOMXOS_GetThreadTime (NULL);
		m_pPfmCompnThreadTime[m_nPfmFrameIndex] = m_pPfmCodecThreadTime[m_nPfmFrameIndex];
		m_pPfmMediaTime[m_nPfmFrameIndex] = (VO_U32) nStart;
		m_pPfmSystemTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime ();
	}

	VO_U32	nRC = m_pVideoDec->GetOutputData (pOutput, pVideoFormat, pbContinuous);

	if (nRC == VO_ERR_NONE && pOutput->Buffer[0] !=  NULL)
	{
		if (m_nPfmFrameIndex < m_nPfmFrameSize)
		{
			m_pPfmFrameTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime () - m_pPfmFrameTime[m_nPfmFrameIndex];
			m_pPfmCodecThreadTime[m_nPfmFrameIndex] = voOMXOS_GetThreadTime (NULL) - m_pPfmCodecThreadTime[m_nPfmFrameIndex];
		}
		m_nPfmFrameIndex++;
	}

	return nRC;
}


OMX_ERRORTYPE voCOMXVideoDec::InitPortType (void)
{
	m_portParam[OMX_PortDomainVideo].nPorts = 2;
	m_portParam[OMX_PortDomainVideo].nStartPortNumber = 0;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
	m_pInput->GetPortType (&pPortType);

	pPortType->eDomain = OMX_PortDomainVideo;
	pPortType->nBufferCountActual = 1;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 20480;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;
	pPortType->format.video.nFrameWidth = 0;
	pPortType->format.video.nFrameHeight = 0;
	pPortType->format.video.nStride = 0;
	pPortType->format.video.nSliceHeight = 16;
	pPortType->format.video.nBitrate = 0;
	pPortType->format.video.xFramerate = 0;
	pPortType->format.video.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.video.eCompressionFormat = OMX_VIDEO_CodingAutoDetect;
	pPortType->format.video.eColorFormat = OMX_COLOR_FormatUnused;
	pPortType->format.video.pNativeWindow = NULL;
	pPortType->format.video.pNativeRender = NULL;

	m_pOutput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainVideo;
	pPortType->nBufferCountActual = (OMX_TRUE == m_bDualCore) ? 2 : 1;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 0;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;

	pPortType->format.video.nFrameWidth = 0;
	pPortType->format.video.nFrameHeight = 0;
	pPortType->format.video.nStride = 0;
	pPortType->format.video.nSliceHeight = 16;
	pPortType->format.video.nBitrate = 0;
	pPortType->format.video.xFramerate = 0;
	pPortType->format.video.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
	pPortType->format.video.eColorFormat = OMX_COLOR_FormatYUV420Planar;
	pPortType->format.video.pNativeWindow = NULL;
	pPortType->format.video.pNativeRender = NULL;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXVideoDec::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	if (pBuffer->nFlags & 0x80000000)
	{
		VOLOGI("EmptyThisBuffer decryption");

		if(!m_pDrm)
			return OMX_ErrorUndefined;

		VO_SOURCEDRM_DATA dataDrm;
		dataDrm.sDataInfo.nDataType = VO_DRMDATATYPE_JITSAMPLE;
		dataDrm.sDataInfo.pInfo = pBuffer->pBuffer + (pBuffer->nFilledLen  & 0x7FFFFFFF);
		dataDrm.pData = pBuffer->pBuffer;
		dataDrm.nSize = pBuffer->nFilledLen & 0x7FFFFFFF;
		dataDrm.ppDstData = 0;
		dataDrm.pnDstSize = 0;

		if (m_pDrm->fCallback(m_pDrm->pUserData, VO_SOURCEDRM_FLAG_DRMDATA, &dataDrm, 0) != OMX_VO_ERR_DRM_OK)
			return OMX_ErrorInsufficientResources;

		VOLOGI("!VO_SOURCE_SAMPLE_FLAG_DivXDRM");
	}

	return voCOMXCompFilter::EmptyThisBuffer (hComponent, pBuffer);
}
