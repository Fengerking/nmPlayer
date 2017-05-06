/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/


#ifndef __voAAC_H__
#define __voAAC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voAudio.h>

/*!
 * Object Type of MPEG4 Audio
 */
typedef enum {
	VOAAC_NULL_OBJECT		= 0,	/*!< NULL Object */
	VOAAC_AAC_MAIN 			= 1,	/*!< AAC Main Object */
	VOAAC_AAC_LC			= 2,	/*!< AAC Low Complexity(LC) Object */
	VOAAC_AAC_SSR 			= 3,	/*!< AAC Scalable Sampling Rate(SSR) Object */
	VOAAC_AAC_LTP 			= 4,	/*!< AAC Long Term Predictor(LTP) Object */
	VOAAC_SBR	 			= 5,	/*!< AAC SBR object */
	VOAAC_AAC_SCAL 			= 6,	/*!< AAC Scalable Object */
	VOAAC_TWIN_VQ 			= 7,	/*!< TwinVQ Object */
	VOAAC_CELP 				= 8,	/*!< CELP Object */
	VOAAC_HVXC				= 9,	/*!< HVXC Object */
	VOAAC_RSVD_10 			= 10,	/*!< (reserved) */
	VOAAC_RSVD_11 			= 11,	/*!< (reserved) */
	VOAAC_TTSI 				= 12,	/*!< TTSI Object(not supported) */
	VOAAC_MAIN_SYNTH		= 13,	/*!< Main Synthetic Object(not supported) */
	VOAAC_WAV_TAB_SYNTH		= 14,	/*!< Wavetable Synthesis Object(not supported) */
	VOAAC_GEN_MIDI 			= 15,	/*!< General MIDI Object(not supported) */
	VOAAC_ALG_SYNTH_AUD_FX	= 16,	/*!< Algorithmic Synthesis and Audio FX Object(not supported) */

	VOAAC_ER_AAC_LC			= 17,	/*!< Error Resilient(ER) AAC Low Complexity(LC) Object */
	VOAAC_RSVD_18 			= 18,	/*!< (reserved) */
	VOAAC_ER_AAC_LTP		= 19,	/*!< Error Resilient(ER) AAC Long Term Predictor(LTP) Object */
	VOAAC_ER_AAC_SCAL		= 20,	/*!< Error Resilient(ER) AAC Scalable Object */
	VOAAC_ER_TWIN_VQ		= 21,	/*!< Error Resilient(ER) TwinVQ Object */
	VOAAC_ER_BSAC			= 22,	/*!< Error Resilient(ER) BSAC Object */
	VOAAC_ER_AAC_LD			= 23,	/*!< Error Resilient(ER) AAC LD Object */
	VOAAC_ER_CELP			= 24,	/*!< Error Resilient(ER) CELP Object */
	VOAAC_ER_HVXC			= 25,	/*!< Error Resilient(ER) HVXC Object */
	VOAAC_ER_HILN			= 26,	/*!< Error Resilient(ER) HILN Object */
	VOAAC_ER_PARA			= 27,	/*!< Error Resilient(ER) Parametric Object */
	VOAAC_SSC	 			= 28,	/*!< SSC Object */
	VOAAC_HE_PS 			= 29,	/*!< AAC High Efficiency with Parametric Stereo coding (HE-AAC v2, object type PS) */
	VOAAC_RSVD_30 			= 30,	/*!< (reserved) */
	VOAAC_RSVD_31 			= 31,	/*!< (reserved) */

	VOAAC_Layer_1			= 32,   /*!< Layer-1 Object */
	VOAAC_Layer_2			= 33,   /*!< Layer-2 Object */
	VOAAC_Layer_3			= 34,   /*!< Layer-3 Object */
	VOAAC_DST				= 35,   /*!< DST Object */

    VOAAC_AAC_SLS           = 37,   /*!< AAC + SLS */
	VOAAC_SLS               = 38,   /*!< SLS       */
	VOAAC_ER_AAC_ELD        = 39,   /*!< AAC Enhanced Low Delay  */
	VOAAC_USAC              = 42,   /*!< USAC                    */
	VOAAC_SAOC              = 43,   /*!< SAOC                    */
	VOAAC_LD_MPEGS          = 44,   /*!< Low Delay MPEG Surround */
	VOAAC_RSVD50            = 50,   /*!< Interim AOT for Rsvd50  */

	VOAAC_OT_MAX			= VO_MAX_ENUM_VALUE
} VOAACOBJECTTYPE;

/*!
 * the frame type that the decoder supports
 */
typedef enum {
	VOAAC_RAWDATA			= 0,	/*!<contains only aac data in frame*/
	VOAAC_ADTS				= 1,	/*!<contains ADTS header data in frame*/
	VOAAC_ADIF				= 2,	/*!<contains ADIF header data in frame*/
	VOAAC_LATM				= 3,	/*!<contains LATM header data in frame*/
	VOAAC_LOAS				= 4,	/*!<contains LOAS header data in frame*/
	VOAAC_FT_MAX			= VO_MAX_ENUM_VALUE
} VOAACFRAMETYPE;

/*!
 * the structure for AAC encoder input parameter
 */
typedef  struct {
  int	  sampleRate;          /*! audio file sample rate */
  int	  bitRate;             /*! encoder bit rate in bits/sec */
  short   nChannels;		   /*! number of channels on input (1,2) */
  short   adtsUsed;			   /*! whether write adts header */
} AACENC_PARAM;

/* AAC Param ID */
#define VO_PID_AAC_Mdoule				0x42211000
#define VO_PID_AAC_PROFILE	 			VO_PID_AAC_Mdoule | 0x0001  /*!< Audio Object Type ID of MPEG4 audio. Now only supports VOAAC_AAC_LC and VOAAC_ER_BSAC, the parameter is a LONG integer */
#define VO_PID_AAC_FRAMETYPE 			VO_PID_AAC_Mdoule | 0x0002  /*!< the frame type that the decoder supports, the parameter is a LONG integer */
#define VO_PID_AAC_CHANNELSPEC 			VO_PID_AAC_Mdoule | 0x0003  /*!< channelSpec  signals the decoder to do special process, such as  down matrix 5.1 channels to 2 channels, the parameter is VO_AUDIO_CHANNELCONFIG */
#define VO_PID_AAC_DISABLEAACPLUSV1		VO_PID_AAC_Mdoule | 0x0004  /*!< disable the AAC+v1(eaac),  the parameter is a LONG integer,1:disable,0:enable*/
#define VO_PID_AAC_DISABLEAACPLUSV2		VO_PID_AAC_Mdoule | 0x0005  /*!< disable the AAC+v2(eaac+), the parameter is a LONG integer,1:disable,0:enable*/
#define VO_PID_AAC_SELECTCHS			VO_PID_AAC_Mdoule | 0x0006  /*!< select special channels for decoding if the sample is multichannel,the value is the combination of VO_AUDIO_CHANNELTYPE,eg.VO_CHANNEL_FRONT_LEFT|VO_CHANNEL_FRONT_RIGHT*/
#define VO_PID_AAC_CHANNELMODE 			VO_PID_AAC_Mdoule | 0x0007  /*!< get the channel type of aac,it is the type of VO_AUDIO_CHANNELMODE  */
#define VO_PID_AAC_CHANNELPOSTION		VO_PID_AAC_Mdoule | 0x0008  /*!< get the array of all channel postion, it is the type of the pointer to int or VO_AUDIO_CHANNELTYPE */
#define VO_PID_AAC_ENCPARAM				VO_PID_AAC_Mdoule | 0x0040  /*!< set AAC encoder parameter, it is the type of AACENC_PARAM */

/* AAC decoder error ID */
#define VO_ERR_AAC_Mdoule				0x82210000
#define VO_ERR_AAC_UNSFILEFORMAT		(VO_ERR_AAC_Mdoule | 0xF001)
#define VO_ERR_AAC_UNSPROFILE			(VO_ERR_AAC_Mdoule | 0xF002)
#define VO_ERR_AAC_INVADTS				(VO_ERR_AAC_Mdoule | 0x4103)
#define VO_ERR_AAC_INVSTREAM			(VO_ERR_AAC_Mdoule | 0x4104)
#define VO_ERR_AAC_INVSBRSTREAM			(VO_ERR_AAC_Mdoule | 0x4120)
#define VO_ERR_AAC_FAILDECSBR			(VO_ERR_AAC_Mdoule | 0x4121)

/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the AAC Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API yyGetAACDecFunc (VO_AUDIO_CODECAPI * pDecHandle);

/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the AAC Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAACEncAPI (VO_AUDIO_CODECAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voAAC_H__
