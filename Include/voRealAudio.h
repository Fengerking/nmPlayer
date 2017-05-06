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



#ifndef __voREALAUDIO_H__
#define __voREALAUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voAudio.h>

typedef struct  
{
	unsigned long ulSampleRate;
	unsigned long ulActualRate;
	unsigned short usBitsPerSample;
	unsigned short usNumChannels;
	unsigned short usAudioQuality;
	unsigned short usFlavorIndex;
	unsigned long ulBitsPerFrame;
	unsigned long ulGranularity;
	unsigned long ulOpaqueDataSize;
	unsigned char*  pOpaqueData;
}VORA_FORMAT_INFO;

typedef struct
{
	VORA_FORMAT_INFO *format;
	unsigned long ulFourCC;
	void*  otherParams;//reserved field
	unsigned long otherParamSize;//reserved field
} VORA_INIT_PARAM;

typedef enum
{
	RA_AAC		= 0,

	RA_G2		,
	RA8_LBR		,
	RA8_HBR		,
	RA_SIPRO	,
}VA_VERSION;

typedef struct  
{
	VA_VERSION version;
	int	samplesPerFrame;//transform size
	int	frameSizeInBits;
	int cplStart;
	int cplQbits;
	int region;
	int	sample_rate;
	int channelNum;
}VORA_RAW_INIT_PARAM;


/* REALAUDIO Param ID */
#define VO_PID_RA_Mdoule				0x42241000
#define VOID_PID_RA_MAXOUTPUTSAMLES		VO_PID_RA_Mdoule | 0x0001
#define VOID_PID_RA_FMT_INIT			VO_PID_RA_Mdoule | 0x0002
#define VOID_PID_RA_RAW_INIT			VO_PID_RA_Mdoule | 0x0003
#define VOID_PID_RA_BUF_INIT			VO_PID_RA_Mdoule | 0x0004
#define VOID_PID_RA_BLOCKSIZE			VO_PID_RA_Mdoule | 0x0005


/* REALAUDIO decoder error ID */
#define VO_ERR_RA_Mdoule				0x82240000
#define VO_ERR_RA_INVFRAME				VO_ERR_RA_Mdoule | 0x0001


/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the REALAUDIO Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API YYGetRADecAPI (VO_AUDIO_CODECAPI * pDecHandle);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voREALAUDIO_H__
