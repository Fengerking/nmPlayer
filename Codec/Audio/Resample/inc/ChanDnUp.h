/**********************************************************************
*
* VisualOn voplayer
*
* CopyRight (C) VisualOn SoftWare Co., Ltd. All rights are reserved.
*
***********************************************************************/

/*********************************************************************
*
* Filename:	
*		ChanDnUp.h
* 
* Abstact:	
*
*		Channel up and down mix functions and definations head file.
*
* Author:
*
*		Witten Wen 2010-7-27
*
* Revision History:
*
******************************************************/
#ifndef __VO_CHANNELS_H_
#define __VO_CHANNELS_H_

#include "commontables.h"

//Channel down and up mix transform

#   define CHDN_FRAC_BITS (23)
#   define MULT_CHDN(a,b) MULT_HI32_SHIFT(a, b, CHDN_FRAC_BITS)
//#   define CHDN_FROM_FLOAT(flt) FRAC_FROM_FLOAT(flt, CHDN_FRAC_BITS)

#define DNMIXROUNGE(a) (((a)>0x7fff)?0x7fff:(((a)<(VO_S32)0xffff8000)?0xffff8000:(a)))

#ifndef _SPEAKER_POSITIONS_
#define _SPEAKER_POSITIONS_
/* Speaker Positions for ChannelMask: from ksmedia.h */
#define VO_CHANNEL_FRONT_LEFT              0x1
#define VO_CHANNEL_FRONT_RIGHT             0x2
#define VO_CHANNEL_FRONT_CENTER            0x4
#define VO_CHANNEL_LOW_FREQUENCY           0x8
#define VO_CHANNEL_BACK_LEFT               0x10
#define VO_CHANNEL_BACK_RIGHT              0x20
#define VO_CHANNEL_FRONT_LEFT_OF_CENTER    0x40
#define VO_CHANNEL_FRONT_RIGHT_OF_CENTER   0x80
#define VO_CHANNEL_BACK_CENTER             0x100
#define VO_CHANNEL_SIDE_LEFT               0x200
#define VO_CHANNEL_SIDE_RIGHT              0x400
#define VO_CHANNEL_TOP_CENTER              0x800
#define VO_CHANNEL_TOP_FRONT_LEFT          0x1000
#define VO_CHANNEL_TOP_FRONT_CENTER        0x2000
#define VO_CHANNEL_TOP_FRONT_RIGHT         0x4000
#define VO_CHANNEL_TOP_BACK_LEFT           0x8000
#define VO_CHANNEL_TOP_BACK_CENTER         0x10000
#define VO_CHANNEL_TOP_BACK_RIGHT          0x20000
#endif /* _SPEAKER_POSITIONS_ */

#define MCMASK_MONO   VO_CHANNEL_FRONT_CENTER
#define MCMASK_STEREO (VO_CHANNEL_FRONT_LEFT | VO_CHANNEL_FRONT_RIGHT)
#define MCMASK_THREE  (VO_CHANNEL_FRONT_CENTER | MCMASK_STEREO)
#define MCMASK_BACK   (VO_CHANNEL_BACK_LEFT | VO_CHANNEL_BACK_RIGHT)
#define MCMASK_QUAD   (MCMASK_STEREO | MCMASK_BACK)
#define MCMASK_5      (MCMASK_THREE | MCMASK_BACK)
#define MCMASK_5DOT1  (MCMASK_5 | VO_CHANNEL_LOW_FREQUENCY)
#define MCMASK_6DOT1  (MCMASK_5DOT1 | VO_CHANNEL_BACK_CENTER)
#define MCMASK_7DOT1  (MCMASK_5DOT1 | VO_CHANNEL_FRONT_LEFT_OF_CENTER | VO_CHANNEL_FRONT_RIGHT_OF_CENTER)

#endif	//__VO_CHANNELS_H_