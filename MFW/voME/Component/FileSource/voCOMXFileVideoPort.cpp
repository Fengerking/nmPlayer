	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFileVideoPort.cpp

	Contains:	voCOMXFileVideoPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMX_Index.h"
#include "voOMXOSFun.h"
#include "voOMXFile.h"
#include "voCOMXPortClock.h"
#include "voCOMXBaseComponent.h"
#include "voOMXMemory.h"
#include "voCOMXFileVideoPort.h"

#define LOG_TAG "voCOMXFileVideoPort"
#include "voLog.h"


voCOMXFileVideoPort::voCOMXFileVideoPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXFileOutputPort (pParent, nIndex)
	, m_pClockPort (NULL)
	, m_nReadSamples (0)
	, m_nReadResult (VO_ERR_NOT_IMPLEMENT)
	, m_pFrameHeader (NULL)
	, m_nFrameHeaderSize (0)
	, m_nJumpState(0)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	m_sType.eDomain = OMX_PortDomainVideo;

	m_fmtVideo.Width = 320;
	m_fmtVideo.Height = 240;
	m_fmtVideo.Type = VO_VIDEO_FRAME_NULL;

	memset(&m_frameFmt, 0, sizeof(VO_VIDEO_FORMAT));
	m_frameFmt.Type = VO_VIDEO_FRAME_NULL;
}

voCOMXFileVideoPort::~voCOMXFileVideoPort(void)
{
	if(m_pFrameHeader)
		voOMXMemFree(m_pFrameHeader);
}

OMX_ERRORTYPE voCOMXFileVideoPort::SetTrack (CBaseSource * pSource, OMX_S32 nTrackIndex)
{
	OMX_ERRORTYPE errType = voCOMXFileOutputPort::SetTrack (pSource, nTrackIndex);
	if (errType != OMX_ErrorNone)
		return errType;

	m_pSource->GetVideoFormat (m_nTrackIndex, &m_fmtVideo);
	if (m_fmtVideo.Width < 0 || m_fmtVideo.Height < 0)
		return OMX_ErrorUndefined;

	OMX_U32	nBufferSize = 409600;
	VO_U32	nRC = m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_MAXSAMPLESIZE, &nBufferSize);
	if (nRC == VO_ERR_NONE)
		m_sType.nBufferSize = nBufferSize + 10240;
	else
		m_sType.nBufferSize = 409600;

	m_sType.nBufferCountActual = 1;
	m_sType.nBufferCountMin = 1;
	m_sType.bBuffersContiguous = OMX_FALSE;
	m_sType.nBufferAlignment = 1;

	m_sType.format.video.pNativeRender = NULL;
	m_sType.format.video.bFlagErrorConcealment = OMX_FALSE;

	m_sType.format.video.nFrameWidth = m_fmtVideo.Width;
	m_sType.format.video.nFrameHeight =m_fmtVideo.Height;
	m_sType.format.video.nStride = m_sType.format.video.nFrameWidth;
	m_sType.format.video.nSliceHeight = 16;
	m_sType.format.video.nBitrate = 0;
	m_sType.format.video.xFramerate = 0;
	m_sType.format.video.eColorFormat = OMX_COLOR_FormatUnused;
	m_sType.format.video.pNativeWindow = NULL;

	VO_U32 nFrameTime = 0;
	nRC = m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_FRAMETIME, &nFrameTime);
	if (VO_ERR_NONE == nRC && nFrameTime > 0)
		m_sType.format.video.xFramerate = ((10000000 / nFrameTime) << 16) + 100;

	VO_U32 nBitrate = 0;
	nRC = m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_BITRATE, &nBitrate);
	if(VO_ERR_NONE == nRC && nBitrate > 0)
		m_sType.format.video.nBitrate = nBitrate * 8;	// History reason, VO_PID_SOURCE_BITRATE is for Byte/Sec

	memcpy (m_pMIMEType, &m_nFourCC, 4);
	m_sType.format.video.cMIMEType = m_pMIMEType;
	if (m_trkInfo.Codec == VO_VIDEO_CodingMPEG4)
	{
		strcpy (m_pMIMEType, "MPV4");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingH263)
	{
		strcpy (m_pMIMEType, "H263");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingH263;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingS263)
	{
		strcpy (m_pMIMEType, "S263");
		m_sType.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingS263;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingH264)
	{
		strcpy (m_pMIMEType, "H264");
		//strcpy (m_pMIMEType, "0CVA");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingMPEG2)
	{
		if(!strcmp(m_pMIMEType, "MPG1")||!strcmp(m_pMIMEType, "mpg1"))
			strcpy (m_pMIMEType, "MPV1");
		else
			strcpy (m_pMIMEType, "MPV2");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG2;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingVP6)
	{
		strcpy (m_pMIMEType, "VP6");
		m_sType.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingVP6;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingVP7)
	{
		strcpy (m_pMIMEType, "VP7");
		m_sType.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingVP7;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingVP8)
	{
		strcpy (m_pMIMEType, "VP8");
		m_sType.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingVP8;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingMJPEG)
	{
		strcpy (m_pMIMEType, "MJPG");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingMJPEG;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingRV)
	{
		strcpy (m_pMIMEType, "RV");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingRV;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingDIVX)
	{
		strcpy (m_pMIMEType, "DIV3");
		m_sType.format.video.eCompressionFormat = (OMX_VIDEO_CODINGTYPE) OMX_VO_VIDEO_CodingDIV3;
	}
	else if (m_trkInfo.Codec == VO_VIDEO_CodingWMV || m_trkInfo.Codec == VO_VIDEO_CodingVC1)
	{
		strcpy (m_pMIMEType, "WMV");
		m_sType.format.video.eCompressionFormat = OMX_VIDEO_CodingWMV;

		VO_U32 nRC = m_pSource->GetTrackParam (nTrackIndex, VO_PID_SOURCE_BITMAPINFOHEADER, &m_trkInfo.HeadData);
		if (nRC == VO_ERR_NONE)
		{
			VO_BITMAPINFOHEADER * pBmpInfo = (VO_BITMAPINFOHEADER *)m_trkInfo.HeadData;
			m_trkInfo.HeadSize = pBmpInfo->biSize;
		}
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXFileVideoPort::SetClockPort (voCOMXPortClock * pClock)
{
	m_pClockPort = pClock;

	return OMX_ErrorNone;
}

#define PROFILE_VO2OMX(profile)		case VO_VIDEO_##profile:pProfileLevelType->eProfile = OMX_VIDEO_##profile;break;
#define PROFILE_VO2OMXVO(profile)	case VO_VIDEO_##profile:pProfileLevelType->eProfile = OMX_VO_VIDEO_##profile;break;
#define LEVEL_VO2OMX(level)			case VO_VIDEO_##level:pProfileLevelType->eLevel = OMX_VIDEO_##level;break;

OMX_ERRORTYPE voCOMXFileVideoPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	switch (nIndex)
	{
	case OMX_IndexParamVideoPortFormat:
		{
			OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pParam;
			if (pVideoFormat->nIndex > 0)
				return OMX_ErrorNoMore;

			pVideoFormat->eCompressionFormat = m_sType.format.video.eCompressionFormat;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoProfileLevelCurrent:
		{
			if(!m_pSource)
				return OMX_ErrorInvalidState;

			VO_VIDEO_PROFILELEVEL pl;
			memset(&pl, 0, sizeof(pl));
			VO_U32 nRC = m_pSource->GetTrackParam(m_nTrackIndex, VO_PID_SOURCE_VIDEOPROFILELEVEL, &pl);
			if(VO_ERR_NONE != nRC)
				return OMX_ErrorNotImplemented;

			OMX_VIDEO_PARAM_PROFILELEVELTYPE* pProfileLevelType = (OMX_VIDEO_PARAM_PROFILELEVELTYPE*)pParam;
			// VO Profile -> Standard OMX Profile
			switch(pl.Profile)
			{
			PROFILE_VO2OMX(MPEG2ProfileSimple)
			PROFILE_VO2OMX(MPEG2ProfileMain)
			PROFILE_VO2OMX(MPEG2Profile422)
			PROFILE_VO2OMX(MPEG2ProfileSNR)
			PROFILE_VO2OMX(MPEG2ProfileSpatial)
			PROFILE_VO2OMX(MPEG2ProfileHigh)

			PROFILE_VO2OMX(H263ProfileBaseline)
			PROFILE_VO2OMX(H263ProfileH320Coding)
			PROFILE_VO2OMX(H263ProfileBackwardCompatible)
			PROFILE_VO2OMX(H263ProfileISWV2)
			PROFILE_VO2OMX(H263ProfileISWV3)
			PROFILE_VO2OMX(H263ProfileHighCompression)
			PROFILE_VO2OMX(H263ProfileInternet)
			PROFILE_VO2OMX(H263ProfileInterlace)
			PROFILE_VO2OMX(H263ProfileHighLatency)

			PROFILE_VO2OMX(MPEG4ProfileSimple)
			PROFILE_VO2OMX(MPEG4ProfileSimpleScalable)
			PROFILE_VO2OMX(MPEG4ProfileCore)
			PROFILE_VO2OMX(MPEG4ProfileMain)
			PROFILE_VO2OMX(MPEG4ProfileNbit)
			PROFILE_VO2OMX(MPEG4ProfileScalableTexture)
			PROFILE_VO2OMX(MPEG4ProfileSimpleFace)
			PROFILE_VO2OMX(MPEG4ProfileSimpleFBA)
			PROFILE_VO2OMX(MPEG4ProfileBasicAnimated)
			PROFILE_VO2OMX(MPEG4ProfileHybrid)
			PROFILE_VO2OMX(MPEG4ProfileAdvancedRealTime)
			PROFILE_VO2OMX(MPEG4ProfileCoreScalable)
			PROFILE_VO2OMX(MPEG4ProfileAdvancedCoding)
			PROFILE_VO2OMX(MPEG4ProfileAdvancedCore)
			PROFILE_VO2OMX(MPEG4ProfileAdvancedScalable)
			PROFILE_VO2OMX(MPEG4ProfileAdvancedSimple)

			PROFILE_VO2OMX(WMVFormat7)
			PROFILE_VO2OMX(WMVFormat8)
			PROFILE_VO2OMX(WMVFormat9)
			PROFILE_VO2OMXVO(WMVFormatWVC1)

			PROFILE_VO2OMXVO(DivX311)
			PROFILE_VO2OMXVO(DivX4)
			PROFILE_VO2OMXVO(DivX5)
			PROFILE_VO2OMXVO(DivX6)

			PROFILE_VO2OMX(RVFormat8)
			PROFILE_VO2OMX(RVFormat9)
			PROFILE_VO2OMX(RVFormatG2)

			PROFILE_VO2OMX(AVCProfileBaseline)
			PROFILE_VO2OMX(AVCProfileMain)
			PROFILE_VO2OMX(AVCProfileExtended)
			PROFILE_VO2OMX(AVCProfileHigh)
			PROFILE_VO2OMX(AVCProfileHigh10)
			PROFILE_VO2OMX(AVCProfileHigh422)
			PROFILE_VO2OMX(AVCProfileHigh444)

			default:
				pProfileLevelType->eProfile = 0x7fffffff;
				break;
			}

			// VO Level -> Standard OMX Level
			switch(pl.Level)
			{
			LEVEL_VO2OMX(MPEG2LevelLL)
			LEVEL_VO2OMX(MPEG2LevelML)
			LEVEL_VO2OMX(MPEG2LevelH14)
			LEVEL_VO2OMX(MPEG2LevelHL)

			LEVEL_VO2OMX(H263Level10)
			LEVEL_VO2OMX(H263Level20)
			LEVEL_VO2OMX(H263Level30)
			LEVEL_VO2OMX(H263Level40)
			LEVEL_VO2OMX(H263Level45)
			LEVEL_VO2OMX(H263Level50)
			LEVEL_VO2OMX(H263Level60)
			LEVEL_VO2OMX(H263Level70)

			LEVEL_VO2OMX(MPEG4Level0)
			LEVEL_VO2OMX(MPEG4Level0b)
			LEVEL_VO2OMX(MPEG4Level1)
			LEVEL_VO2OMX(MPEG4Level2)
			LEVEL_VO2OMX(MPEG4Level3)
			LEVEL_VO2OMX(MPEG4Level4)
			LEVEL_VO2OMX(MPEG4Level4a)
			LEVEL_VO2OMX(MPEG4Level5)

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
				pProfileLevelType->eLevel = 0x7fffffff;
				break;
			}

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoAvc:
		{
			if(!m_pSource)
				return OMX_ErrorInvalidState;

			VO_VIDEO_PARAM_AVCTYPE videoParaAVCType = {0};

			VO_U32 nRC = m_pSource->GetTrackParam(m_nTrackIndex, VO_PID_VIDEO_ParamVideoAvc, &videoParaAVCType);	
			if(VO_ERR_NONE != nRC)
				return OMX_ErrorNotImplemented;

			OMX_VIDEO_PARAM_AVCTYPE *pVideParamAVCType = (OMX_VIDEO_PARAM_AVCTYPE*)pParam;
			switch(videoParaAVCType.eProfile)
			{
			case VO_VIDEO_AVCProfileBaseline:
				pVideParamAVCType->eProfile = OMX_VIDEO_AVCProfileBaseline;break;
			case VO_VIDEO_AVCProfileMain:
				pVideParamAVCType->eProfile = OMX_VIDEO_AVCProfileMain;break;
			case VO_VIDEO_AVCProfileExtended:
				pVideParamAVCType->eProfile = OMX_VIDEO_AVCProfileExtended;break;
			case VO_VIDEO_AVCProfileHigh:
				pVideParamAVCType->eProfile = OMX_VIDEO_AVCProfileHigh;break;
			case VO_VIDEO_AVCProfileHigh10:
				pVideParamAVCType->eProfile = OMX_VIDEO_AVCProfileHigh10;break;
			case VO_VIDEO_AVCProfileHigh422:
				pVideParamAVCType->eProfile = OMX_VIDEO_AVCProfileHigh422;break;
			case VO_VIDEO_AVCProfileHigh444:
				pVideParamAVCType->eProfile = OMX_VIDEO_AVCProfileHigh444;break;
			default:
				pVideParamAVCType->eProfile = OMX_VIDEO_AVCProfileMax;
				break;
			}

			// VO Level -> Standard OMX Level
			switch(videoParaAVCType.eLevel)
			{
			case VO_VIDEO_AVCLevel1:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel1;break;
			case VO_VIDEO_AVCLevel1b:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel1b;break;
			case VO_VIDEO_AVCLevel11:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel11;break;
			case VO_VIDEO_AVCLevel12:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel12;break;
			case VO_VIDEO_AVCLevel13:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel13;break;
			case VO_VIDEO_AVCLevel2:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel2;break;
			case VO_VIDEO_AVCLevel21:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel21;break;
			case VO_VIDEO_AVCLevel22:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel22;break;
			case VO_VIDEO_AVCLevel3:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel3;break;
			case VO_VIDEO_AVCLevel31:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel31;break;
			case VO_VIDEO_AVCLevel32:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel32;break;
			case VO_VIDEO_AVCLevel4:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel4;break;
			case VO_VIDEO_AVCLevel41:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel41;break;
			case VO_VIDEO_AVCLevel42:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel42;break;
			case VO_VIDEO_AVCLevel5:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel5;break;
			case VO_VIDEO_AVCLevel51:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevel51;break;
			default:
				pVideParamAVCType->eLevel = OMX_VIDEO_AVCLevelMax;
				break;
			}

			pVideParamAVCType->nSliceHeaderSpacing = videoParaAVCType.nSliceHeaderSpacing;
			pVideParamAVCType->nPFrames = videoParaAVCType.nPFrames;
			pVideParamAVCType->nBFrames = videoParaAVCType.nBFrames;
			pVideParamAVCType->bUseHadamard = videoParaAVCType.bUseHadamard ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->nRefFrames = videoParaAVCType.nRefFrames;
			pVideParamAVCType->nRefIdx10ActiveMinus1 = videoParaAVCType.nRefIdx10ActiveMinus1;
			pVideParamAVCType->nRefIdx11ActiveMinus1 = videoParaAVCType.nRefIdx11ActiveMinus1;
			pVideParamAVCType->bEnableUEP = videoParaAVCType.bEnableUEP ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->bEnableFMO = videoParaAVCType.bEnableFMO ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->bEnableASO = videoParaAVCType.bEnableASO ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->bEnableRS = videoParaAVCType.bEnableRS ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->nAllowedPictureTypes = videoParaAVCType.nAllowedPictureTypes;
			pVideParamAVCType->bFrameMBsOnly = videoParaAVCType.bFrameMBsOnly ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->bEntropyCodingCABAC = videoParaAVCType.bEntropyCodingCABAC ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->bWeightedPPrediction = videoParaAVCType.bWeightedPPrediction ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->nWeightedBipredicitonMode = videoParaAVCType.nWeightedBipredicitonMode;
			pVideParamAVCType->bconstIpred = videoParaAVCType.bconstIpred ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->bDirect8x8Inference = videoParaAVCType.bDirect8x8Inference ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->bDirectSpatialTemporal = videoParaAVCType.bDirectSpatialTemporal ? OMX_TRUE : OMX_FALSE;
			pVideParamAVCType->nCabacInitIdc = videoParaAVCType.nCabacInitIdc;

			return OMX_ErrorNone;			
		}
		break;

	case OMX_VO_IndexVideoRotation:
		{
			if(!m_pSource)
				return OMX_ErrorInvalidState;

			video_matrix sVideoMatrix;
			memset(&sVideoMatrix, 0, sizeof(video_matrix));
			VO_U32 nRC = m_pSource->GetSourceParam(VO_PID_FILE_GETVIDEOMATRIX, &sVideoMatrix);
			if(VO_ERR_NONE != nRC)
				return OMX_ErrorNotImplemented;

			*((OMX_S32*)pParam) = sVideoMatrix.rotation;
			return OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexVideoUpsideDown:
		{
			if(!m_pSource)
				return OMX_ErrorInvalidState;

			VO_BOOL bUpSideDown;
			VO_U32 nRc = m_pSource->GetTrackParam(m_nTrackIndex, VO_PID_VIDEO_UPSIDEDOWN, &bUpSideDown);
			if(VO_ERR_NONE != nRc)
				return OMX_ErrorNotImplemented;
			*((OMX_BOOL*)pParam) = (bUpSideDown == VO_TRUE) ? OMX_TRUE : OMX_FALSE;
			return OMX_ErrorNone;
		}
		break;
   case OMX_VO_IndexVideoS3d:
	   {
		   if(!m_pSource)
				return OMX_ErrorInvalidState;
		   VO_S3D_params sVideoS3d;
		   memset(&sVideoS3d, 0, sizeof(VO_S3D_params));
		   VO_U32 nRc = m_pSource->GetTrackParam(m_nTrackIndex, VO_PID_VIDEO_S3D, &sVideoS3d);
		   if(VO_ERR_NONE != nRc)
				return OMX_ErrorNotImplemented;
		   OMX_VO_S3D_params* p = (OMX_VO_S3D_params*)pParam;

		   p->active = (OMX_BOOL) sVideoS3d.active;
		   p->fmt	  =	(OMX_U32)sVideoS3d.fmt;
		   p->mode = (OMX_U32)sVideoS3d.mode;
		   p->order = (OMX_U32)sVideoS3d.order;
		   p->subsampling = (OMX_U32)sVideoS3d.subsampling;
		   return OMX_ErrorNone;


	   }

	case OMX_VO_IndexCodecFCC:
		{
			if(!m_pSource)
				return OMX_ErrorInvalidState;

			VO_U32  nFourCC;
			VO_U32 nRc = m_pSource->GetTrackParam(m_nTrackIndex, VO_PID_SOURCE_CODECCC, &nFourCC);
			if(VO_ERR_NONE != nRc)
				return OMX_ErrorNotImplemented;
			
			*((OMX_U32*)pParam) = nFourCC;
			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXFileOutputPort::GetParameter (nIndex, pParam);
}

OMX_ERRORTYPE voCOMXFileVideoPort::SetParameter(OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	if(nIndex == (OMX_INDEXTYPE)(OMX_VO_IndexVideoFrameHeader))
	{
		OMX_VO_BUFFER* pVOBuffer = (OMX_VO_BUFFER*)pParam;
		if(!pVOBuffer)
			return OMX_ErrorBadParameter;

		if(m_pFrameHeader)
			voOMXMemFree(m_pFrameHeader);

		m_nFrameHeaderSize = pVOBuffer->Size;
		m_pFrameHeader = (OMX_U8*)voOMXMemAlloc(m_nFrameHeaderSize);
		if(!m_pFrameHeader)
			return OMX_ErrorInsufficientResources;

		memcpy(m_pFrameHeader, pVOBuffer->Buffer, m_nFrameHeaderSize);
		return OMX_ErrorNone;
	}

	return voCOMXFileOutputPort::SetParameter(nIndex, pParam);
}

void voCOMXFileVideoPort::SetSourceConfig (CBaseConfig * pConfig)
{
	voCOMXFileOutputPort::SetSourceConfig (pConfig);

	if (m_pConfig != NULL)
	{
		char * pFile = m_pConfig->GetItemText (m_pParent->GetName (), (char*)"VideoSourceFile");
		if (pFile != NULL)
		{
#ifdef _WIN32
			TCHAR szFile[256];
			memset (szFile, 0, sizeof (szFile));
			MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); \
			m_hDataSourceFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_READ_ONLY);		
#else
			m_hDataSourceFile = voOMXFileOpen (pFile, VOOMX_FILE_READ_ONLY);
#endif // _WIN32
			VOLOGI ("Open source file %s, The handle is 0X%08X.", pFile, (int)m_hDataSourceFile);
		}

		m_nLogLevel = m_pConfig->GetItemValue (m_pParent->GetName (), (char*)"ShowLogLevel", 0);
	}
}

OMX_ERRORTYPE voCOMXFileVideoPort::SetTrackPos (OMX_S64 * pPos)
{
	if (m_pSource == NULL)
		return OMX_ErrorUndefined;

	VO_S64 nNewPos =  *pPos;
	if (m_nFirstPos != -1)
		 nNewPos =  m_nFirstPos + *pPos;

	m_nSeekPos = nNewPos;
	if (m_nSeekPos != 0)
		m_nJumpState = 1;

	VO_U32 nRC = m_pSource->SetTrackPos (m_nTrackIndex, &nNewPos);

	if (m_nSeekMode == OMX_TIME_SeekModeFast)
		m_nSeekPos = nNewPos;

	if (nRC == VO_ERR_SOURCE_END)
	{
		m_Sample.Time = nNewPos;
		m_nStartPos = nNewPos;
		m_nCurrentPos = nNewPos - m_nFirstPos;

		return OMX_ErrorNone;
	}
	else if (nRC != VO_ERR_NONE)
	{
		VO_S64 nStartPos = 0;
		nRC = m_pSource->SetTrackPos (m_nTrackIndex, &nStartPos);

		if (nRC != VO_ERR_NONE)
		{
			VOLOGE ("m_pSource->SetTrackPos was failed. 0X%08X", (int)nRC);
			return OMX_ErrorNotImplemented;
		}
	}

	m_Sample.Time = nNewPos;
	m_nStartPos = nNewPos;
	m_nCurrentPos = nNewPos - m_nFirstPos;

	*pPos = nNewPos - m_nFirstPos;
	if ((*pPos) < 0)
		*pPos = 0;

	m_bEOS = OMX_FALSE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXFileVideoPort::Flush (void)
{
	OMX_ERRORTYPE errType = voCOMXFileOutputPort::Flush ();

	m_nReadSamples = 0;
	m_nReadResult = VO_ERR_NOT_IMPLEMENT;

	return errType;
}

OMX_ERRORTYPE voCOMXFileVideoPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	if ((m_nLogLevel & 0X0F) > 0)
	{
		if (m_nLogSysStartTime == 0)
			m_nLogSysStartTime = voOMXOS_GetSysTime ();

		VOLOGI ("Sys Time: %d", (int)(voOMXOS_GetSysTime () - m_nLogSysStartTime));

		m_nLogSysCurrentTime = voOMXOS_GetSysTime ();
	}

	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;
	pBuffer->nFlags = 0;

	if (m_hDataSourceFile != NULL)
	{
		int nReadSize = voOMXFileRead (m_hDataSourceFile, (OMX_U8 *)&pBuffer->nFilledLen, 4);
		if (nReadSize != 4)
		{
			voOMXFileSeek (m_hDataSourceFile, 0, VOOMX_FILE_BEGIN);
			voOMXFileRead (m_hDataSourceFile, (OMX_U8 *)&pBuffer->nFilledLen, 4);
		}

		voOMXFileRead (m_hDataSourceFile, (OMX_U8*)&pBuffer->nTimeStamp, 8);
		voOMXFileRead (m_hDataSourceFile, pBuffer->pBuffer, pBuffer->nFilledLen);

		pBuffer->nFlags = 0;
		pBuffer->nTickCount = 1;

		return OMX_ErrorNone;
	}

	voCOMXAutoLock lock (m_pLoadMutex);
	if (m_pSource == NULL)
		return OMX_ErrorInvalidState;

	// Dump log info
	if (m_nLogFrameSize > 0 && m_pLogFrameSpeed != NULL)
	{
		if (m_nLogFrameIndex < m_nLogFrameSize)
		{
			m_pLogFrameSpeed[m_nLogFrameIndex] = voOMXOS_GetSysTime ();
		}
	}

	if (m_nReadSamples > 0 && m_nReadResult == VO_ERR_NONE)
	{
		OMX_TICKS	mediaTime = 0;
		if (m_pClockPort != NULL && !m_pParent->TranscodeMode ())
			m_pClockPort->GetMediaTime (&mediaTime);

		if (mediaTime > 90)
		{
			pBuffer->pOutputPortPrivate = (OMX_PTR)(mediaTime - m_Sample.Time);
			if (m_sType.nBufferCountActual <= 2)
				m_Sample.Time = mediaTime - 90;
			else 
				m_Sample.Time = mediaTime + 30 * m_sType.nBufferCountActual;
		}

		VOLOGR ("Playing Time: %d", (int)mediaTime);
	}

	if (m_pSource->HasError () != 0)
	{
		// m_pCallBack->EventHandler (m_pParent->GetComponent (), m_pAppData, OMX_EventError, 0, 0, 0);
		// VOLOGW ("There was error in source module!");
		return OMX_ErrorUndefined;
	}

	if(m_pParent->DumpRuntimeLog())
	{
		VOLOGI ("Time %d, systime %d", (int)m_Sample.Time, (int)voOMXOS_GetSysTime());
	}
	VO_U32	nRC = m_pSource->GetTrackData (m_nTrackIndex, &m_Sample);
	if(m_pParent->DumpRuntimeLog())
	{
		VOLOGI ("nRC 0x%08X, sync frame 0x%08X, Size %d, Time %d, flag %d, systime %d", (int)nRC, (int)(m_Sample.Size & 0x80000000), (int)(m_Sample.Size & 0x7fffffff), (int)m_Sample.Time, (int)m_Sample.Flag, (int)voOMXOS_GetSysTime());
	}

	if ((m_nLogLevel & 0X0F) > 0)
	{
		VOLOGI ("Read video used time is %d, Sample Size %d, Time: %d, Result 0X%08X", (int)(voOMXOS_GetSysTime () - m_nLogSysCurrentTime), (int)(m_Sample.Size & 0X7FFFFFFF), (int)m_Sample.Time, (int)nRC);
	}
	
	m_nReadResult = nRC;
	if (nRC == VO_ERR_NONE)
	{
//		if (m_nFirstPos == -1)
//			m_nFirstPos = m_Sample.Time;
		
		// this is workaround for Issue 7805 , in one GOP if video size changes, don't send it to decoder
		VO_U32 ffSource = m_pSource->GetFormat();
		if (m_sType.format.video.eCompressionFormat == OMX_VIDEO_CodingH263 && ffSource != VO_FILE_FFSTREAMING_RTSP
			&& ffSource != VO_FILE_FFAPPLICATION_SDP && ffSource!= VO_FILE_FFSTREAMING_HTTPPD)
		{
			VO_U32 index = (m_Sample.Buffer[4]>>2)&0X7;
			if(index> 0 && index<6)//0 forbid, 6 and 7 are reserved 
			{
				static const VO_S32 size[8][2] = {{ 0,0 },{ 128,96 },{ 176,144 },{ 352,288 },{ 704,576 },{ 1408,1152 }};

				if((m_Sample.Size &0x80000000)==0 && m_frameFmt.Type!= VO_VIDEO_FRAME_NULL)
				{
					//not key frame
					if(size[index][0] != m_frameFmt.Width || size[index][1] != m_frameFmt.Height)
					{
						pBuffer->nFilledLen = 0;
						pBuffer->nFlags = OMX_BUFFERFLAG_EOS;
						if(m_pCallBack)
							m_pCallBack->EventHandler(m_pParent->GetComponent(), m_pAppData,OMX_EventError, OMX_ErrorUndefined, 0, NULL);
						return OMX_ErrorUndefined;
					}
				}
				// if key frame, just update frame format
				if((m_Sample.Size & 0x80000000)>0)
				{
					m_frameFmt.Width = size[index][0];
					m_frameFmt.Height= size[index][1];
					m_frameFmt.Type  = VO_VIDEO_FRAME_I;
				}
			}			
		}

		if (m_Sample.Flag & VO_SOURCE_SAMPLE_FLAG_FRAMEDROPPED)
		{
			m_nJumpState = 1;
			m_pCallBack->EventHandler (m_pParent->GetComponent (), m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_FrameDropped, 0, 0, NULL);
		}

		if (m_nJumpState && (m_sType.format.video.eCompressionFormat == OMX_VIDEO_CodingMPEG2 || m_sType.format.video.eCompressionFormat == OMX_VIDEO_CodingWMV))
		{
			VO_VIDEO_FRAMETYPE frameType = VO_VIDEO_FRAME_NULL;
			if(m_Sample.Size & 0x80000000)
				frameType = VO_VIDEO_FRAME_I;
			else
				frameType = GetSampleFrameType(m_Sample.Buffer, m_Sample.Size & 0x7FFFFFFF);
				
			if (VO_VIDEO_FRAME_NULL != frameType)
			{
				switch(m_nJumpState)
				{
					case 1: // Start Skip I frame
					{
						if(VO_VIDEO_FRAME_I != frameType)
						{
							pBuffer->nFlags = 0;
							pBuffer->nFilledLen = 0;
							return OMX_ErrorNone;

						}  //After seek, first frame not I frame, stop drop
						else
							m_nJumpState = 2;	//start Drop
					}
					break;

					case 2: // Start Drop B Frame
					{
						if(VO_VIDEO_FRAME_B == frameType)
						{
							pBuffer->nFlags = 0;
							pBuffer->nFilledLen = 0;
							return OMX_ErrorNone;
						}
						else 
							m_nJumpState = 0;	// Stop Drop frame
					}
					break;

					default:
						break;
				}
			}
		}

		if (m_Sample.Flag & VO_SOURCE_SAMPLE_FLAG_CODECCONFIG)
			pBuffer->nFlags |= OMX_BUFFERFLAG_CODECCONFIG;

		m_nCurrentPos = m_Sample.Time;

		VO_U32 nSampleSize = m_Sample.Size & 0x7FFFFFFF;
		const static OMX_U8 sPESHeader[3] = {0, 0, 1};
		if((m_pFrameHeader && m_nFrameHeaderSize > 0) && !(m_Sample.Flag & VO_SOURCE_SAMPLE_FLAG_CODECCONFIG) && 
			((nSampleSize >= 3 && memcmp(sPESHeader, m_Sample.Buffer, 3)) || nSampleSize < 3))
		{
			memcpy (pBuffer->pBuffer, m_pFrameHeader, m_nFrameHeaderSize);
			memcpy (pBuffer->pBuffer + m_nFrameHeaderSize, m_Sample.Buffer, nSampleSize);
			pBuffer->nFilledLen = nSampleSize + m_nFrameHeaderSize;
		}
		else
		{
			if (m_sType.nBufferCountActual == 1)
				pBuffer->pBuffer = m_Sample.Buffer;
			else
				memcpy (pBuffer->pBuffer, m_Sample.Buffer, nSampleSize);
			pBuffer->nFilledLen = nSampleSize;
		}

		pBuffer->nTimeStamp = m_Sample.Time;
		static OMX_TICKS lastts = 0;
		lastts = pBuffer->nTimeStamp;
		
		if (pBuffer->nTimeStamp < m_nSeekPos && m_nSeekMode == OMX_TIME_SeekModeAccurate)
		{
			pBuffer->nFlags |= OMX_BUFFERFLAG_DECODEONLY;
			m_bDecOnly = OMX_TRUE;
		}
		else
		{
			m_nSeekPos = 0;
			m_bDecOnly = OMX_FALSE;
		}
		
		pBuffer->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME ;
		
		if (m_bHadBuffering)
		{
			m_bHadBuffering = OMX_FALSE;
			pBuffer->nFlags |= OMX_BUFFERFLAG_STARTTIME;
		}
		if (m_Sample.Size & 0x80000000)
			pBuffer->nFlags |= OMX_BUFFERFLAG_SYNCFRAME;

		m_nReadSamples++;

		if (m_Sample.Flag & VO_SOURCE_SAMPLE_FLAG_DivXDRM)
		{
			pBuffer->nFlags |= 0x80000000;
		}

		if (m_hDumpFile != NULL)
		{
			voOMXFileWrite (m_hDumpFile, pBuffer->pBuffer, pBuffer->nFilledLen);
//			voOMXFileFlush (m_hDumpFile);
		}

		if (m_nLogFrameSize > 0 && m_pLogMediaTime != NULL)
		{
			if (m_nLogFrameIndex < m_nLogFrameSize)
			{
				m_pLogMediaTime[m_nLogFrameIndex] = (OMX_U32)pBuffer->nTimeStamp;
				m_pLogSystemTime[m_nLogFrameIndex] = voOMXOS_GetSysTime ();
				m_pLogFrameSize[m_nLogFrameIndex] = m_Sample.Size;
				m_pLogFrameSpeed[m_nLogFrameIndex] = voOMXOS_GetSysTime () - m_pLogFrameSpeed[m_nLogFrameIndex];
			}
			m_nLogFrameIndex++;
		}

		VOLOGR ("Comp %s, Index %d. The buffer size %d, time %d", m_pParent->GetName (), m_sType.nPortIndex, pBuffer->nFilledLen, (int)pBuffer->nTimeStamp);
	}
	else if (nRC == VO_ERR_SOURCE_END)
	{
		pBuffer->nFilledLen = 0;
		pBuffer->nFlags = OMX_BUFFERFLAG_EOS;

		VOLOGI ("Comp %s, Index %d.nRC == VO_ERR_SOURCE_END", m_pParent->GetName (), (int)m_sType.nPortIndex);
	}
	else if (nRC == VO_ERR_SOURCE_NEEDRETRY)
	{
		pBuffer->nFlags = 0;
		pBuffer->nFilledLen = 0;
		voOMXOS_Sleep (2);

		return OMX_ErrorNone;
	}
	else
	{
		pBuffer->nFlags = 0;
		pBuffer->nFilledLen = 0;
		voOMXOS_Sleep (2);

		VOLOGE ("Comp %s, Index %d.Get track data failed. 0X%08X", m_pParent->GetName (), (int)m_sType.nPortIndex, (int)nRC);

		return OMX_ErrorUndefined;
	}

	return OMX_ErrorNone;
}

/* MPEG2 video frame type check*/
VO_VIDEO_FRAMETYPE	voCOMXFileVideoPort::GetSampleFrameType(VO_PBYTE pSample, OMX_U32 nSize)
{
	if (OMX_VIDEO_CodingMPEG2 == m_sType.format.video.eCompressionFormat)
		return GetMpeg2FrameType(pSample, nSize);
	else if (OMX_VIDEO_CodingWMV == m_sType.format.video.eCompressionFormat)
		return GetWMVFrameType(pSample, nSize);
	else
		return VO_VIDEO_FRAME_NULL;
}

VO_VIDEO_FRAMETYPE	voCOMXFileVideoPort::GetMpeg2FrameType(VO_PBYTE pSample, OMX_U32 nSize)
{
	if(nSize < 6)
		return VO_VIDEO_FRAME_NULL;

	VO_BYTE b = *(pSample + 5);
	b = b >> 3;
	b = b & 0x7;

	if(1 == b)
		return VO_VIDEO_FRAME_I;
	else if(2 == b)
		return VO_VIDEO_FRAME_P;
	else if(3 == b)
		return VO_VIDEO_FRAME_B;
	else
		return VO_VIDEO_FRAME_NULL;
}

VO_VIDEO_FRAMETYPE	voCOMXFileVideoPort::GetWMVFrameType(VO_PBYTE pSample, OMX_U32 nSize)
{
	VO_SOURCE_TRACKINFO	trkInfo;
	VO_U32 nFourCC = 0;
	m_pSource->GetTrackParam(m_nTrackIndex, VO_PID_SOURCE_CODECCC, &nFourCC);
	m_pSource->GetTrackInfo (m_nTrackIndex, &trkInfo);

	struct SEQHEADERINFO{
		VO_U32 bInterlacedSource;//for WVC1
		VO_U32 bSeqInterpolation;//for WMV3
		VO_U32 bNumBFrames;//for WMV3
		VO_U32 bPreProRange;//for WMV3
	} pSeqInfo;

	VO_BYTE b0=0,b1=0; 
	VO_PBYTE ptr=NULL;

	//if(nFourCC == '3VMW')
	if(memcmp((char*)"WMV3", (char*)&nFourCC, 4) == 0)
	{//WMV3
		if((trkInfo.HeadSize)<4)
			return VO_VIDEO_FRAME_NULL;
		ptr = (trkInfo.HeadData);
		b0 = *(ptr+2);
		b1 = *(ptr+3);
		pSeqInfo.bPreProRange = (b1&0x80);
		pSeqInfo.bNumBFrames	= (b1&0x70); //25 bit
		pSeqInfo.bSeqInterpolation = (b1&0x02);
		pSeqInfo.bInterlacedSource=0;
	}
	else if(memcmp((char*)"WVC1", (char*)&nFourCC, 4) == 0)
	{//WVC1
		if((nSize) < 10 )
			return VO_VIDEO_FRAME_NULL;
		b0 = *(trkInfo.HeadData + 10);
		pSeqInfo.bInterlacedSource= (b0&0x40);
	}

	nSize = nSize & 0x0FFFFFFF;
    VO_BYTE b[6];

    if(memcmp((char*)"WVC1", (char*)&nFourCC, 4) == 0) //WVC1
	{
		if(nSize<5)
			return VO_VIDEO_FRAME_NULL;
        ptr = pSample;
        b[0] = (*ptr)&0x80;b[1] = (*ptr)&0x40;
        b[2] = (*ptr)&0x20;b[3] = (*ptr)&0x10;
        b[4] = (*ptr)&0x08;b[5] = (*ptr)&0x04;
        if(pSeqInfo.bInterlacedSource)
        {            
            if( b[0] == 0)
            {
                 if(b[1] ==0 )
                    return VO_VIDEO_FRAME_P;
                else if(b[2] ==0 )
                    return VO_VIDEO_FRAME_B;
                else if(b[3] ==0 )
                    return VO_VIDEO_FRAME_I;
                else if(b[4] == 0)//BI actually
                    return VO_VIDEO_FRAME_B;
                else//skip
                    return VO_VIDEO_FRAME_S;
            }
            else 
            {
                if(b[2] ==0 )
                    return VO_VIDEO_FRAME_P;
                else if(b[3] ==0 )
                    return VO_VIDEO_FRAME_B;
                else if(b[4] ==0 )
                    return VO_VIDEO_FRAME_I;
                else if(b[5] == 0)//BI actually
                     return VO_VIDEO_FRAME_B;
                else//skip
                    return VO_VIDEO_FRAME_S;              
            }
        }
        else
        {
            if(b[0] ==0 )
                return VO_VIDEO_FRAME_P;
            else if(b[1] ==0 )
                return VO_VIDEO_FRAME_B;
            else if(b[2] ==0 )
                return VO_VIDEO_FRAME_I;
            else if(b[3] == 0)//BI actually
                return VO_VIDEO_FRAME_B;
            else//skip
                return VO_VIDEO_FRAME_S;
        }
	}
	else if(memcmp((char*)"WMV3", (char*)&nFourCC, 4) == 0)//WMV3
	{		
		VO_U32 pos=0;
		if(nSize<1)
			return VO_VIDEO_FRAME_NULL;

        ptr = pSample;        
        b[0] = *ptr & 0x80;b[1] = *ptr & 0x40;
        b[2] = *ptr & 0x20;b[3] = *ptr & 0x10;
        b[4] = *ptr & 0x08;b[5] = *ptr & 0x04;
        if(pSeqInfo.bSeqInterpolation)
            pos += 1;

        pos += 2;
        if(pSeqInfo.bPreProRange)
            pos += 1;

        if(b[pos] )
            return VO_VIDEO_FRAME_P;
        else
        {
            if(pSeqInfo.bNumBFrames ==0)
                 return VO_VIDEO_FRAME_I;
            else
            {
                if(b[pos+1])
                    return VO_VIDEO_FRAME_I;
                else
                    return VO_VIDEO_FRAME_B;
            }
        }
	}

	return VO_VIDEO_FRAME_NULL;
}

