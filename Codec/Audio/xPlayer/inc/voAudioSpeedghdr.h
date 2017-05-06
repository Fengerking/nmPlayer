/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

/************************************************************************
* @file voAudioSpeedghdr.h
*
* Audio Speed global header filter
*
* @author  Huaping Liu
* @date    2013-05-02
************************************************************************/
#ifndef  __VO_AUDIOSPEEDGHDR_H__
#define __VO_AUDIOSPEEDGHDR_H__

#include "voAudio.h"
#include "voxPlayerStrech.h"

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

#define AUDIO_SPEED_LOW   0.5     // auto setting low tempo range (-50%)
#define AUDIO_SPEED_TOP   2.0     // auto setting top tempo range (+100%)

// sequence-ms setting values at above low & top tempo
#define SEQ_MIN      100.0
#define SEQ_MAX      50.0
#define K_SEQ           ((SEQ_MAX - SEQ_MIN) / (AUDIO_SPEED_TOP - AUDIO_SPEED_LOW))
#define C_SEQ          (SEQ_MIN - (K_SEQ) * (AUDIO_SPEED_LOW))

// seek-window-ms setting values at above low & top tempo
#define SEEK_MIN      25.0
#define SEEK_MAX     15.0
#define K_SEEK          ((SEEK_MAX - SEEK_MIN) / (AUDIO_SPEED_TOP - AUDIO_SPEED_LOW))
#define C_SEEK          (SEEK_MIN - (K_SEEK) * (AUDIO_SPEED_LOW))

#define voClip(x, mi, ma) (((x) < (mi)) ? (mi) : (((x) > (ma)) ? (ma) : (x)))

typedef struct
{
	VO_S32							nChs;
	VO_S32							nSampleRate;
	VO_S32                        nBitsSample;
	VO_S16							*pIn;
	VO_S32							nInLens;
	VO_S32                        nTemp;
	VO_S16							*pOut;
	VO_MEM_OPERATOR      *pvoMemop;
	VO_MEM_OPERATOR      voMemoprator;
	voxPlayerStrech             *pTsm;
	VO_PTR                         hCheck;
}voxPlayerGlobal;

static const VO_S16 mFastOffset[5][24]={
	{ 124,  186,  248,  310,  372,  434,  
	   496,  558,  620,  682,  744, 806,
	   868,  930,  992, 1054, 1116, 1178, 
	   1240, 1302, 1364, 1426, 1488,   0},
	{-100,  -75,  -50,  -25,   25,   
	  50,   75,  100,    0,    0,    0,   
	  0,  0,    0,    0,    0,    0,    0,    
	  0,    0,    0,    0,    0,   0},
	{ -20,  -15,  -10,   -5,    5,   
	  10,   15,   20,    0,    0,    0,   
	  0,  0,    0,    0,    0,    0,    0,    
	  0,    0,    0,    0,    0,   0},
	{  -4,   -3,   -2,   -1,    1,    2,   
	   3,    4,    0,    0,    0,   0,
	  0,    0,    0,    0,    0,    0,    
	  0,    0,    0,    0,    0,   0},
	{ 121,  114,   97,  114,   98,  
	   105,  108,   32,  104,   99,  117,  
	   111, 116,  100,  110,  117,  111,  
	   115,    0,    0,    0,    0,    0,   0}
};


#endif //__VO_AUDIOSPEEDGHDR_H__
