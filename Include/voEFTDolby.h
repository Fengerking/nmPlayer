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


#ifndef __voEFTDolby_H__
#define __voEFTDolby_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voAudio.h>

/*!
 * the structure for dolby effect input parameter
 */
typedef  struct {
	/* Dolby Mobile effect on/off defualt*/
	int DolEnable;

	/* MobileSurround */
	int MSREnable;  
	int MSRRoom;
	int MSRBright;

	/* MobileSurroundUpmixer */
	int MUPEnable;

	/* HighFrequencyEnhancer */
	int HFEEnable;
	int HFEDepth;

	/* MonoToStereoCreator */
	int M2SEnable;

	/* NaturalBass */
	int NBEnable;
	int NBCutoff;
	int NBBoost;
	int NBLevel; 

	/* Upsample */
	int UPSampleEnable;

	/* GraphicEQ */
	int GEQEnable;
	int GEQNband;
	int GEQPreamp;
	int GEQMaxBoost;
	int GEQBand1;
	int GEQBand2;
	int GEQBand3;
	int GEQBand4;
	int GEQBand5;
	int GEQBand6;
	int GEQBand7;

	/* Portable Mode */
	int	PortableModeEnable;
	int	PortableModeGain;
	int PortableModeOrl;

	/* Pulse Downmixer */
	int	PulseDownMix;
	int	PulseDownMixAtten;

} EFFECT_DOLBY_PARAM;

/* EffectDolby Param ID */
#define VO_PID_EFFDOLBY_Mdoule				(VO_INDEX_EFT_DOLBY | 0x40001000)
#define VO_PID_EFFDOLBY_DEFAULT				VO_PID_EFFDOLBY_Mdoule | 0x0001


/* EffectDolby decoder error ID */
#define VO_ERR_EFFDOLBY_Mdoule				(VO_INDEX_EFT_DOLBY | 0x80000000)
#define VO_ERR_EFFDOLBY_INVHEADER			VO_ERR_EFFDOLBY_Mdoule | 0x0001



/**
 * Get Audio effect API interface
 * \param pDecHandle [out] Return the Dobly effect handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetDolbyEffectAPI (VO_AUDIO_CODECAPI * pDecHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voAAC_H__
