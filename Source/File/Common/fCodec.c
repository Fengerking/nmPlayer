	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		fCodec.c

	Contains:	fourcc and twocc function implement code

	Written by:	East Zhou

	Change History (most recent first):
	2009-03-25		East		Create file

*******************************************************************************/
#include "fCodec.h"
#include "fCC.h"


VO_VIDEO_CODINGTYPE	fCodecGetVideoCodec(VO_U32 uFourcc)
{
	switch(uFourcc)
	{
	case FOURCC_XVID:
	case FOURCC_xvid:
	case FOURCC_XVIX:
	case FOURCC_XviD:

	case FOURCC_DIV4:
	case FOURCC_DIV5:
	case FOURCC_DIV6:
	case FOURCC_AP41:
	case FOURCC_COL1:
	case FOURCC_DVX3:
	case FOURCC_MPG3:

	case FOURCC_DIVX:
	case FOURCC_divx:
	case FOURCC_MP4S:
	case FOURCC_MP4V:
	case FOURCC_mp4v:
	case FOURCC_M4S2:

	case FOURCC_DX50:
	case FOURCC_BLZ0:
	case FOURCC_DXGM:

	case FOURCC_3IV2:
	case FOURCC_3IVX:
	case FOURCC_RMP4:
	case FOURCC_DM4V:
	case FOURCC_WV1F:
	case FOURCC_FMP4:
	case FOURCC_HDX4:
	case FOURCC_DVX1:
	case FOURCC_MPV4:
	case FOURCC_SEDG:
		return VO_VIDEO_CodingMPEG4;

	case FOURCC_div3:
	case FOURCC_DIV3:
		return VO_VIDEO_CodingDIVX;

	case FOURCC_MJPG:
	case FOURCC_LJPG:
		return VO_VIDEO_CodingMJPEG;

	case FOURCC_h263:
	case FOURCC_H263:
	case FOURCC_D263:
	case FOURCC_L263:
	case FOURCC_M263:
	case FOURCC_X263:
		return VO_VIDEO_CodingH263;

	case FOURCC_S263:
	case FOURCC_FLV1:
		return VO_VIDEO_CodingS263;

	case FOURCC_H264:
	case FOURCC_h264:
	case FOURCC_X264:
	case FOURCC_VSSH:
	case FOURCC_DAVC:
	case FOURCC_AVC1:
	case FOURCC_avc1:
		return VO_VIDEO_CodingH264;

	case FOURCC_WMV1:
	case FOURCC_WMV2:
	case FOURCC_WMVA:
	case FOURCC_WMVP:
	case FOURCC_WVP2:
	case FOURCC_MP41:
	case FOURCC_MPG4:
	case FOURCC_DIV1:
	case FOURCC_MP42:
	case FOURCC_DIV2:
	case FOURCC_MP43:
		return VO_VIDEO_CodingWMV;
	
	case FOURCC_WMV3:
	case FOURCC_wmv3:
	case FOURCC_WVC1:
	case FOURCC_wvc1:
		return VO_VIDEO_CodingVC1;

	case FOURCC_MPEG:
	case FOURCC_MPG1:
	case FOURCC_mpg1:
	case FOURCC_MPG2:
	case FOURCC_mpg2:
		return VO_VIDEO_CodingMPEG2;

	case FOURCC_FLV4:
	case FOURCC_VP60:
	case FOURCC_VP61:
	case FOURCC_VP62:
	case FOURCC_VP6F:
		return VO_VIDEO_CodingVP6;
	case FOURCC_VP70:
		return VO_VIDEO_CodingVP7;

	case FOURCC_VP80:
		return VO_VIDEO_CodingVP8;

	case FOURCC_REAL:
		return VO_VIDEO_CodingRV;
	case FOURCC_h265:
	case FOURCC_hvc1:
	case FOURCC_HVC1:
	case FOURCC_hev1:
		return VO_VIDEO_CodingH265;
	default:
		return VO_VIDEO_CodingUnused;
	}
}

VO_AUDIO_CODINGTYPE	fCodecGetAudioCodec(VO_U16 uTwocc)
{
	switch(uTwocc)
	{
	case AudioFlag_MP3:
		return VO_AUDIO_CodingMP3;

	case AudioFlag_MP1:
		return VO_AUDIO_CodingMP1;

	case AudioFlag_MS_ADPCM:
	case AudioFlag_MS_ALAW:
	case AudioFlag_MS_MULAW:
	case AudioFlag_IMA_ADPCM:
	case AudioFlag_IT_G726_ADPCM:
	case AudioFlag_MS_ADPCM_2:
		return VO_AUDIO_CodingADPCM;

	case AudioFlag_MS_PCM:
	case AudioFlag_IEEE_Float:
		return VO_AUDIO_CodingPCM;

	case AudioFlag_WMA_V1:
	case AudioFlag_WMA_V2:
	case AudioFlag_WMA_PRO_V9:
	case AudioFlag_WMA_LOS_V9:
	case AudioFlag_WMA_PRO:
	case AudioFlag_MS_WM_SPEECH:
	case AudioFlag_MS_WM_RT_VOICE:
		return VO_AUDIO_CodingWMA;

	case AudioFlag_ADIF_AAC:
	case AudioFlag_ADTS_AAC:
	case AudioFlag_LATM_AAC:
	case AudioFlag_LOAS_AAC:
	case AudioFlag_AAC:
	case AudioFlag_ISOMPEG4_AAC:
	case AudioFlag_MPEG_RAW_AAC:
		return VO_AUDIO_CodingAAC;

	case AudioFlag_AC3:
		return VO_AUDIO_CodingAC3;

	case AudioFlag_EAC3:
		return VO_AUDIO_CodingEAC3;

	case AudioFlag_AMR_NB:
		return VO_AUDIO_CodingAMRNB;

	case AudioFlag_AMR_WB:
		return VO_AUDIO_CodingAMRWB;

	case AudioFlag_AMR_WB_PLUS:
		return VO_AUDIO_CodingAMRWBP;

	case AudioFlag_FLAC:
		return VO_AUDIO_CodingFLAC;

	case AudioFlag_RA_1_2_144:
	case AudioFlag_RA_1_2_288:
	case AudioFlag_RA_G2:
	case AudioFlag_RA_DNET:
	case AudioFlag_RA_AAC:
	case AudioFlag_RA_AAC_PLUS:
		return VO_AUDIO_CodingRA;

	case AudioFlag_OGG_1:
	case AudioFlag_OGG_2:
	case AudioFlag_OGG_3:
	case AudioFlag_OGG_1_PLUS:
	case AudioFlag_OGG_2_PLUS:
	case AudioFlag_OGG_3_PLUS:
	case AudioFlag_OGG_4_PLUS:
		return VO_AUDIO_CodingOGG;

	case AudioFlag_QCELP:
		return VO_AUDIO_CodingQCELP13;

	case AudioFlag_EVRC:
		return VO_AUDIO_CodingEVRC;
	case AudioFlag_APE:
		return VO_AUDIO_CodingAPE;
	case AudioFlag_ALAC:
		return VO_AUDIO_CodingALAC;
	case AudioFlag_DTS:
		return VO_AUDIO_CodingDTS;
	case AudioFlag_MS_GSM_610:
		return VO_AUDIO_CodingGSM610;

	default:
		return VO_AUDIO_CodingUnused;
	}
}

VO_BOOL	fCodecIsDivX(VO_U32 uFourcc)
{
	switch(uFourcc)
	{
	case FOURCC_div3:
	case FOURCC_DIV3:
	case FOURCC_DIV4:
	case FOURCC_DIV5:
	case FOURCC_DIV6:

	case FOURCC_DIVX:
	case FOURCC_divx:

	case FOURCC_DX50:
		return VO_TRUE;

	default:
		return VO_FALSE;
	}	
}

VO_BOOL	fCodecIsXviD(VO_U32 uFourcc)
{
	switch(uFourcc)
	{
	case FOURCC_XVID:
	case FOURCC_xvid:
		return VO_TRUE;

	default:
		return VO_FALSE;
	}
}