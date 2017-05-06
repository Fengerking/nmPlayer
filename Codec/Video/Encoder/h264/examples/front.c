
#include "voH264.h"
#include "memory.h"
#include "stdlib.h"
#include "H264_C.h"
#include "H264_C_Type.h"
#include "H264_E_InternalMain.h"
#include "H264_E_Main.h"


typedef struct
{
	VO_U32						nConfig;
#ifdef LICENSE_CHECK
	VO_PTR phLicenseCheck;
#endif
	vtINTERNALPARAMETERS		AnInternalParametersRecrod;
	vtPARAMETERS				vH264par;
	vtSEGMENT					m_segment;

} VOH264ENCHND;

#define CONFIG_OK 0xFF00


VO_U32 StartEncoderLib(VO_HANDLE hCodec)
{
	VOH264ENCHND *hH264Enc		= (VOH264ENCHND *)hCodec;
	vtINTERNALPARAMETERS *ip	= &hH264Enc->AnInternalParametersRecrod; 
	int size, i;
	vtRETURNCODE rc;
	vtPARAMETERS *m_264Par		= &hH264Enc->vH264par;

	memset((VO_U8*)m_264Par, 0, sizeof(vtPARAMETERS));
	vtInitializeCodingParameters(m_264Par, vt_QCIF_NTSC);

	m_264Par->extras					= (void *) ip;
	m_264Par->inputFrameWidth			= 176;
	m_264Par->inputFrameHeight			= 144;

	m_264Par->tvStandard				= H264_NTSC_VF;
	//!m_264Par->videoSourceType			= AVC_PROGRESSIVE_VIDEO;
	m_264Par->frameRateCode				= FRAME_RATE_CODE_SUB30;
	m_264Par->colorDepth				= COLOR_DEPTH_8;
	m_264Par->colorSpace				= AVC_PLANAR_YUV420;
	m_264Par->chromaticFormat			= AVC_CF_420;
	m_264Par->numberOfFrames			= 300;
	m_264Par->profile					= H264_BASELINE_PROFILE;
	m_264Par->level						= H264_LEVEL_11;
	m_264Par->minimumBitRate			= 0;
	m_264Par->targetBitRate				= 0;		//!< 450000;//28000;
	m_264Par->maximumBitRate			= 900000;//88000;
	m_264Par->bitrateControl			= AVC_VARIABLE_BITRATE;
	m_264Par->defaultCodingSegmentSize	= 1;
	m_264Par->maximumIntraRefreshPeriod	= 15;
	m_264Par->maximumReferenceCount		= 1;	//2;
	m_264Par->motionType				= AVC_HIGH_MOTION; //AVC_MEDIUM_MOTION;
	m_264Par->initialQPI				= 32;	//28;//33;
	m_264Par->initialQPR				= 32;//35;
	m_264Par->initialQPN				= 32;//37;
	m_264Par->initialQS					= 36;
#ifdef QCIF15FPS128KBPS
	m_264Par->initialQPI				= 20;	//28;//33;
	m_264Par->initialQPR				= 21;//35;
	m_264Par->initialQPN				= 21;//37;
	m_264Par->initialQS					= 36;
#endif
#ifdef QVGA15FPS384KBPS
	m_264Par->initialQPI				= 18;	//28;//33;
	m_264Par->initialQPR				= 19;//35;
	m_264Par->initialQPN				= 19;//37;
	m_264Par->initialQS					= 36;
#endif
	m_264Par->entropyCodingMode			= AVC_CAVLC;
	m_264Par->frameCoding				= AVC_TRUE;
	m_264Par->fieldCoding				= AVC_FALSE;
	m_264Par->noBPictures				= AVC_TRUE;		//!< for baseline conformance
	m_264Par->noMBAFF					= AVC_TRUE;
	m_264Par->noDeblockFiltering		= AVC_FALSE;//AVC_TRUE;	//AVC_FALSE;
	m_264Par->lowDelay					= AVC_FALSE;	//!< slow down the speed a lot
	m_264Par->constrainIntraPrediction	= AVC_FALSE;

	//!m_264Par->videoSourceType = AVC_INTERLACED_VIDEO;
	//!m_264Par->codingPattern = AVC_UNSPECIFIED_CODING_PATTERN;
	//!m_264Par->maximumIDRRefreshPeriod = 88888888;

	//! internal settings
	ip->noQuarterPelMVs					= AVC_TRUE;
	ip->MBBlocksOnly					= AVC_TRUE;
	ip->ignoreChromaInSkippingCost		= AVC_TRUE;

	return VO_ERR_NONE;
}





VO_U32 EndEncoderLib(VOH264ENCHND *pH264EncHnd)
{
	int i;
	vtPARAMETERS *p264Par	= &pH264EncHnd->vH264par;
	vtSEGMENT*pSegment		= &pH264EncHnd->m_segment;
	vtRETURNCODE rc;

//	Free_Memory(p264Par);
//	for (i = 0; i < m_264Par->defaultCodingSegmentSize; i++){
//	Free_Memory(pSegment.frames[0].frameBuffer);
//	}



	if (!vtCheckRC(rc = vtEndSequence())){
		return VO_ERR_FAILED;
	}
	if (!vtCheckRC(rc = vtEnd())) {
		return VO_ERR_FAILED;
	}
	free(pSegment->frames);
	free(pH264EncHnd);

	return VO_ERR_NONE;
}

VO_U32 VO_API voH264ENCInit(VO_HANDLE * phEnc, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VO_U32 codec_idx;
	VO_LIB_OPERATOR *pLibOP = NULL;
	VO_PTR phLicenseCheck = NULL;
	vtPARAMETERS *pH264par;

	switch(vType){
	case VO_VIDEO_CodingH264:
		codec_idx = VO_INDEX_ENC_H264;
		break;
	default:
		return VO_ERR_NOT_IMPLEMENT;
	}

#ifdef LICENSE_CHECK
	if(pUserData){
		if(pUserData->memflag & 0XF0){//valid libOperator flag
			pLibOP = pUserData->libOperator;
		}
	}
	voCheckLibInit (&phLicenseCheck, codec_idx, 0, voH264EnchInst, pLibOP);
#endif

	//if(!((*phEnc) = (VOH264ENCHND *) Allocate_Memory(sizeof(VOH264ENCHND), AVC_ALIGN8)))
	if(!((*phEnc) = (VOH264ENCHND *) malloc(sizeof(VOH264ENCHND))))
		return VO_ERR_OUTOF_MEMORY;
	memset((*phEnc), 0, sizeof(VOH264ENCHND));

	StartEncoderLib(*phEnc);

	pH264par = &(((VOH264ENCHND *)(*phEnc))->vH264par);

	//((VOH264ENCHND *)(*phEnc))->m_segment.frames = (vtFRAME *) Allocate_Memory(sizeof(vtFRAME) * pH264par->defaultCodingSegmentSize, AVC_ALIGN8 );
	((VOH264ENCHND *)(*phEnc))->m_segment.frames = (vtFRAME *) malloc(sizeof(vtFRAME) * pH264par->defaultCodingSegmentSize);
	if(((VOH264ENCHND *)(*phEnc))->m_segment.frames == NULL)
		return VO_ERR_OUTOF_MEMORY;



#ifdef LICENSE_CHECK	
	((VOH264ENCHND *)(*phEnc))->phLicenseCheck = phLicenseCheck; 
#endif

	return VO_ERR_NONE;
}


/**
 * Close the H264 Encoder instance, release any resource it used.
 * \param hCodec [in] MPEG4 Encoder instance handle, returned by voMPEG4ENCInit().
 * \return The function should always return VO_ERR_NONE.
 */
VO_U32 VO_API voH264ENCUninit(VO_HANDLE hH264Enc)
{
#ifdef LICENSE_CHECK
	voCheckLibUninit (((VOH264ENCHND *)hH264Enc)->phLicenseCheck);
#endif

	EndEncoderLib((VOH264ENCHND *)hH264Enc);
//	vompeg4_enc_finish((VOH264ENCHND *)hH264Enc);
	return VO_ERR_NONE;
}


/**
 * Set parameter of the Encoder instance.
 * \param hCodec [in] H264 Encoder instance handle, returned by voMPEG4ENCInit().
 * \param nID [in] Parameter ID, Supports: VOMPEG4PARAMETERID
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VO_ERR_NONE Succeeded.
 * \retval VO_ERR_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_U32 VO_API voH264ENCSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	VOH264ENCHND *enc_hnd		= (VOH264ENCHND *)hCodec;
	vtPARAMETERS *m_264Par		= &enc_hnd->vH264par;

	switch(uParamID){
	case VO_PID_ENC_H264_PROFILE:
		m_264Par->profile					= *((VO_S32*)pData);
//		enc_hnd->nConfig |= 1;
		break;
	case VO_PID_ENC_H264_LEVEL:
		m_264Par->level						= *((VO_S32*)pData);
//		enc_hnd->nConfig |= (1<<1);
		break;
	case VO_PID_ENC_H264_BITRATE: 
		m_264Par->targetBitRate				= *((VO_S32*)pData);		//!< 450000;//28000;
//		enc_hnd->nConfig |= (1<<2);
		break;
	case VO_PID_ENC_H264_FRAMERATE:
		m_264Par->frameRateCode				= *((VO_S32*)pData);
//		enc_hnd->nConfig |= (1<<3);
		break;
	case VO_PID_ENC_H264_RATECONTROL:
		m_264Par->bitrateControl			= *((VO_S32*)pData);
//		enc_hnd->nConfig |= (1<<4);
		break;
	case VO_PID_ENC_H264_WIDTH:
		m_264Par->inputFrameWidth			= *((VO_S32*)pData);
		enc_hnd->nConfig |= (1<<5);
		break;
	case VO_PID_ENC_H264_HEIGHT:
		m_264Par->inputFrameHeight			= *((VO_S32*)pData);
		enc_hnd->nConfig |= (1<<6);
		break;
	case VO_PID_ENC_H264_KEY_FRAME_INTERVAL:
		m_264Par->maximumIntraRefreshPeriod	= *((VO_S32*)pData);
		enc_hnd->nConfig |= (1<<7);
		break;
	case VO_PID_ENC_H264_VIDEO_QUALITY:
//		enc_hnd->custom_config.enc_quality = *((VO_S32*)pData);
//		enc_hnd->nConfig |= (1<<5);
		break;
//	case VO_PID_ENC_H264_FORCE_KEY_FRAME:
//		enc_hnd->frame_info.force_intra_frame = *((VO_S32*)pData);
		break;
//	case VO_PID_ENC_H264_INPUT_ROTATION:
//		enc_hnd->custom_config.rt_type = *((VO_S32*)pData);
		break;
//	case VO_PID_ENC_H264_VP_SIZE:
//		enc_hnd->vp_byte_size = *((VO_S32*)pData);
//		break;
	default :
		return VO_ERR_WRONG_PARAM_ID;
	}
	/* parameters setting completed*/
	if(enc_hnd->nConfig == 0x60){

		if (!vtCheckRC(vtStart(m_264Par))) {
			return VO_ERR_ENC_H264_CONFIG;
		}

		if (!vtCheckRC(vtStartSequence(m_264Par))) {
			return VO_ERR_ENC_H264_CONFIG;
		}
		enc_hnd->nConfig = CONFIG_OK; 
	}

	return VO_ERR_NONE;
}

/**
 * Get parameter of the Encoder .
 * \param hCodec [in] MPEG4 Encoder instance handle, returned by voMPEG4ENCInit().
 * \param nID [in] Parameter ID, Supports: VOMPEG4PARAMETERID
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VO_ERR_NONE Succeeded.
 * \retval VO_ERR_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_U32 VO_API voH264ENCGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	VOH264ENCHND *enc_hnd = (VOH264ENCHND *)hCodec;
	vtPARAMETERS *m_264Par		= &enc_hnd->vH264par;

	if(enc_hnd->nConfig != CONFIG_OK)
		return VO_ERR_WRONG_STATUS;

	switch(uParamID){
	case VO_PID_ENC_H264_OUTBUF_SIZE: 
		*((VO_U32 *)pData) = (m_264Par->inputFrameWidth*m_264Par->inputFrameHeight*3/2)*m_264Par->defaultCodingSegmentSize;
		break;
//	case VO_PID_ENC_H264_VOL_HEADER:
//		((VO_CODECBUFFER *)pData)->Length = vompeg4_writeVlH((ENCHND *)hCodec, ((VO_CODECBUFFER *)pData)->Buffer);
//		break;
	default :
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}


/**
 * Encode one frame data.
 * \param hCodec [in] H264 Encoder instance handle, returned by voMPEG4ENCInit().
 * \param pInData [in] Input buffer pointer. 
 * \param pOutData [in/out] Output buffer pointer and length. 
 *  pOutData->buffer should be alloced by Application, buffer size can get from voMPEG4ENCGetParameter(), ID is VO_PID_ENC_MPEG4_OUTBUF_SIZE.
 *  When the function return, pOutData->length indicates the encoded stream size.

 * \param FrameType [out] Output encoded frame type. 
 * \retval VO_ERR_NONE Succeeded.
 * \retval VO_ERR_WRONG_STATUS The Encoder is not ready to encode. Usually this indicates more parameters needed. See voMPEG4SetParameter().
 */
VO_U32 VO_API voH264ENCProcess(VO_HANDLE hDec, VO_VIDEO_BUFFER * pInput, VO_CODECBUFFER * pOutput, VO_VIDEO_FRAMETYPE * pType)
{
	VOH264ENCHND *enc_hnd	= (VOH264ENCHND *)hDec;
	vtSEGMENT *pSegment		= &(enc_hnd->m_segment);

#ifdef LICENSE_CHECK
	VO_VIDEO_FORMAT	video_format;
#endif

	if(enc_hnd->nConfig != CONFIG_OK)
		return VO_ERR_WRONG_STATUS;
#if 0
	if(pInput->ColorType == VO_COLOR_YUV_PLANAR420){
		enc_hnd->frame_info.input_type = VO_COLOR_YUV_PLANAR420;
	}else if(pInput->ColorType == VO_COLOR_UYVY422_PACKED){
		enc_hnd->frame_info.input_type = VO_COLOR_UYVY422_PACKED;
	}else if(pInput->ColorType == VO_COLOR_RGB565_PACKED){
		enc_hnd->frame_info.input_type = VO_COLOR_RGB565_PACKED;
	}else if(pInput->ColorType == VO_COLOR_YVU_PLANAR420){
		enc_hnd->frame_info.input_type = VO_COLOR_YVU_PLANAR420;
	}else if(pInput->ColorType == VO_COLOR_VYUY422_PACKED_2){
		enc_hnd->frame_info.input_type = VO_COLOR_VYUY422_PACKED_2;
	}else if(pInput->ColorType == VO_COLOR_UYVY422_PACKED_2){
		enc_hnd->frame_info.input_type = VO_COLOR_UYVY422_PACKED_2;
	}else if(pInput->ColorType == VO_COLOR_YUYV422_PACKED){
		enc_hnd->frame_info.input_type = VO_COLOR_YUYV422_PACKED;
	}else if(pInput->ColorType == VO_COLOR_YUV_PLANAR422_12){
		enc_hnd->frame_info.input_type = VO_COLOR_YUV_PLANAR422_12;
	}else if(pInput->ColorType == VO_COLOR_YUV_420_PACK_2){
		enc_hnd->frame_info.input_type = VO_COLOR_YUV_420_PACK_2;
	}else{
		return VO_ERR_ENC_MPEG4_VIDEOTYPE;
	}
#endif
#ifdef LICENSE_CHECK
	video_format.Height = enc_hnd->img_height;
	video_format.Width  = enc_hnd->img_width;
	video_format.Type = VO_VIDEO_FRAME_I;
	voCheckLibCheckVideo (enc_hnd->phLicenseCheck, pInput, &video_format);
#endif

	pSegment->frameCount = 1;
	pSegment->frames[0].frameBuffer = pInput->Buffer[0];

	if (!vtCheckRC(pOutput->Length = vtCodeASegment(pSegment, pOutput->Buffer, pOutput->Length))) {
		return VO_ERR_ENC_H264_FRAME;
	}

	pSegment->firstFrameNumberInSequence += 1;

//	*pType = enc_hnd->frame_info.frame_type;
//	memset(&enc_hnd->frame_info, 0, sizeof(FRAME_INFO_CONFIG));

	return VO_ERR_NONE;

}



VO_S32 VO_API voGetH264EncAPI (VO_VIDEO_ENCAPI * pDecHandle)
{
	VO_VIDEO_ENCAPI *pH264Enc = pDecHandle;

	if(!pH264Enc)
		return VO_ERR_INVALID_ARG;

	pH264Enc->Init		= voH264ENCInit;
	pH264Enc->Uninit	= voH264ENCUninit;
	pH264Enc->SetParam	= voH264ENCSetParameter;
	pH264Enc->GetParam	= voH264ENCGetParameter;
	pH264Enc->Process	= voH264ENCProcess;

	return VO_ERR_NONE;
}
