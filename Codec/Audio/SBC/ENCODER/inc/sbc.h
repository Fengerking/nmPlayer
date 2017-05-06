//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

sbc.h

Abstract:

SBC basic head file.

Author:

Witten Wen 29-December-2009

Revision History:

*************************************************************************/

#ifndef __SBC_H
#define __SBC_H

#include "sbc_math.h"
#include "voSBCmemory.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VOSBC_SYNCWORD	0x9C

/* channel mode */
#define VOSBR_CH_MONO		0x00
#define VOSBR_CH_DUAL_CHANNEL	0x01
#define VOSBR_CH_STEREO		0x02
#define VOSBR_CH_JOINT_STEREO	0x03

/* nrof_blocks */
#define NB4_VOSBC	0x00
#define NB8_VOSBC	0x01
#define NB12_VOSBC	0x02
#define NB16_VOSBC	0x03

/* sampling frequency, support 16KHz, 32KHz, 44.1KHz, 48KHz */
#define VOSBC_MODE_16	0x00
#define VOSBC_MODE_32	0x01
#define VOSBC_MODE_44 	0x02
#define VOSBC_MODE_48	0x03

/* allocation mode */
#define VOSBC_AM_LOUDNESS	0x00
#define VOSBC_AM_SNR		0x01


/* This structure contains an unpacked SBC frame.
   Yes, there is probably quite some unused space herein */
struct sbc_frame {
	unsigned short m_SampleRate;	/* in kHz */
	unsigned char  m_Blocks;
	unsigned char  channel_mode;
	unsigned char  m_Channels;
    unsigned char  allocation_method;
	unsigned short m_Codesize;
	unsigned char  m_Subbands;
	unsigned char  m_Bitpool;
	unsigned char  m_Length;
	unsigned char  m_Join;
	unsigned char  m_ScaleFactor[2][8];
	int            m_SBSampleF[16][2][8];
	short          m_PCMSample[2][16*8];	/* original pcm audio samples */
};

struct sbc_encoder_state {
	int m_Subbands;
	int m_X[2][80];
};

struct sbc_priv {
	int					m_Init;
	struct sbc_frame	m_Frame;
	struct sbc_encoder_state m_EncState;
};

struct sbc_struct {
//	unsigned long flags;

	int m_Rate;
	int m_Channels;
	int m_Joint;
	int m_Allocation;
	int m_Blocks;
	int m_Subbands;
	int m_Bitpool;
	int m_Swap;	//1 for Bigger End, 0 for Little End

	void *m_pPriv;		//priv

	/* Witten added */
	VO_U8	*m_pInput;
	VO_U32	m_InSize;
	VO_U32	m_Consumed;
	VO_U32	m_FrameSize;

	VO_U8	*m_pFramBuf, *m_pNextBegin;
	VO_U32	m_TempLen;

	VO_MEM_OPERATOR *vopMemOP;
	
	//voCheck lib need
	VO_PTR				hCheck;
};

typedef struct sbc_struct sbc_t;

int SBCInit(sbc_t *sbc, unsigned long flags);
int SBCReinit(sbc_t *sbc, unsigned long flags);
int SBCEncode(sbc_t *sbc, 
			  VO_U8 *input, 
			  VO_U32 input_len, 
			  VO_U8 *output,
		      int output_len, 
			  VO_U32 *written
			  );
int SBCGetFrameLength(sbc_t *sbc);
int SBCGetFrameDuration(sbc_t *sbc);
int SBCGetCodesize(sbc_t *sbc);
void SBCFinish(sbc_t *sbc);

VO_U32 voSBCSetInit(sbc_t *psbc);


#ifdef __cplusplus
}
#endif

#endif /* __SBC_H */ 
