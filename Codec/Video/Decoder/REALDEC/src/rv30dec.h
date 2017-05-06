/*********************************************** BEGIN LICENSE BLOCK ********************************************** 
 * Version: RCSL 1.0 and Exhibits. 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
 * All Rights Reserved. 
 * 
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Community Source 
 * License Version 1.0 (the "RCSL"), including Attachments A though H, 
 * all available at http://www.helixcommunity.org/content/rcsl. 
 * You may also obtain the license terms directly from RealNetworks. 
 * You may not use this file except in compliance with the RCSL and 
 * its Attachments. There are no redistribution rights for the source 
 * code of this file. Please see the applicable RCSL for the rights, 
 * obligations and limitations governing use of the contents of the file. 
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 * https://rarvcode-tck.helixcommunity.org 
 * 
 * Contributor(s): 
 * 
 ************************************************************* END LICENSE BLOCK ********************************************/ 
/******************************************************************************************/
/*    RealNetworks, Inc. Confidential and Proprietary Information.							*/
/*    Copyright (c) 1996, 1996, 1998, 1999, 2000, 2001 RealNetworks, Inc.					*/
/*    All Rights Reserved.																	*/
/*    Do not redistribute.																	*/
/******************************************************************************************/
/************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/
//	DESCRIPTION: 	Definitions for RARV1032 DLL API 
/* $Id: rv30dec.h,v 1.5 2005/03/24 21:02:52 hfrederickson Exp $ */

#ifndef __RV30DEC_H__
#define __RV30DEC_H__

#include "helix_types.h"
#include "helix_utils.h"
#include "helix_codec.h"

#if defined(_MSC_VER)
/* Begin 1-byte structure alignment */
/* RealVideo front end files use 1-byte structure alignment under MSVC, */
/* but the backend is compiled with 8-byte alignment. */
/* This pragma keeps the structure alignment independent of compiler options, */
/* for all structures declared in this file. */
/* 1-byte alignment is now the frontent project default. --karl */
#pragma pack(push, 1)
#endif

/* Byte alignment settings for Mac */
#if defined( _MACINTOSH )
#pragma options align=mac68k
#endif


#ifdef __cplusplus
extern "C" {	/* Assume C declarations for C++ */
#endif /* __cplusplus */


#define RV_DECODE_MORE_FRAMES   	    0x00000001
#define RV_DECODE_DONT_DRAW     	    0x00000002
#define RV_DECODE_KEY_FRAME				0x00000004
    /* Indicates that the decompressed image is a key frame. */
    /* Note that enhancement layer EI frames are not key frames, in the */
    /* traditional sense, because they have dependencies on lower layer */
    /* frames. */

#define RV_DECODE_B_FRAME				0x00000008
    /* Indicates that the decompressed image is a B frame. */
    /* At most one of PIA_DDN_KEY_FRAME and PIA_DDN_B_FRAME will be set. */

#define RV_DECODE_DEBLOCKING_FILTER		0x00000010
	/* Indicates that the returned frame has gone through the */
	/* deblocking filter. */

#define RV_DECODE_FRU_FRAME				0x00000020
    /* Indicates that the decompressed image is a B frame. */
    /* At most one of PIA_DDN_KEY_FRAME and PIA_DDN_B_FRAME will be set. */

#define RV_DECODE_SCRAMBLED_BUFFER      0x00000040
    /* Indicates that the input buffer is scrambled for security */
    /* decoder should de-scramble the buffer before use it */
#define RV_DECODE_LAST_FRAME            0x00000200


typedef struct tag_H263DecoderInParams
{
	VOULONG32 dataLength;
	U8 *pInputFrame;
	VOULONG32 numDataSegments;
	HXCODEC_SEGMENTINFO *pDataSegments;
	VOULONG32 flags;
        /* 'flags' should be initialized by the front-end before each */
        /* invocation to decompress a frame.  It is not updated by the decoder. */
        /* */
        /* If it contains RV_DECODE_MORE_FRAMES, it informs the decoder */
        /* that it is being called to extract the second or subsequent */
        /* frame that the decoder is emitting for a given input frame. */
        /* The front-end should set this only in response to seeing */
        /* an RV_DECODE_MORE_FRAMES indication in H263DecoderOutParams. */
        /* */
        /* If it contains RV_DECODE_DONT_DRAW, it informs the decoder */
        /* that it should decode the image (in order to produce a valid */
        /* reference frame for subsequent decoding), but that no image */
        /* should be returned.  This provides a "hurry-up" mechanism. */
	VOULONG32 timestamp;
} H263DecoderInParams;

typedef struct tag_H263DecoderOutParams
{
	VOULONG32 numFrames;
	VOULONG32 notes;
        /* 'notes' is assigned by the transform function during each call to */
        /* decompress a frame.  If upon return the notes parameter contains */
        /* the indication RV_DECODE_MORE_FRAMES, then the front-end */
        /* should invoke the decoder again to decompress the same image. */
        /* For this additional invocation, the front-end should first set */
        /* the RV_DECODE_MORE_FRAMES bit in the 'H263DecoderInParams.flags' */
        /* member, to indicate to the decoder that it is being invoked to */
        /* extract the next frame. */
        /* The front-end should continue invoking the decoder until the */
        /* RV_DECODE_MORE_FRAMES bit is not set in the 'notes' member. */
        /* For each invocation to decompress a frame in the same "MORE_FRAMES" */
        /* loop, the front-end should send in the same input image. */
        /* */
        /* If the decoder has no frames to return for display, 'numFrames' will */
        /* be set to zero.  To avoid redundancy, the decoder does *not* set */
        /* the RV_DECODE_DONT_DRAW bit in 'notes' in this case. */


	VO_S64 timestamp;
        /* The 'temporal_offset' parameter is used in conjunction with the */
        /* RV_DECODE_MORE_FRAMES note, to assist the front-end in */
        /* determining when to display each returned frame. */
        /* If the decoder sets this to T upon return, the front-end should */
        /* attempt to display the returned image T milliseconds relative to */
        /* the front-end's idea of the presentation time corresponding to */
        /* the input image. */
        /* Be aware that this is a signed value, and will typically be */
        /* negative. */

	VO_PTR		UserData;

	VOULONG32 width;
	VOULONG32 height;
		/* Width and height of the returned frame. */
		/* This is the width and the height as signalled in the bitstream. */
	UINT8	*outData[3];
	VOULONG32	outStride[3];
	VO_VIDEO_FRAMETYPE frame_type;
	I32     InputUsed;
} H263DecoderOutParams;



/* Defines match ilvcmsg.h so that ulSPOExtra == rv10init.invariants */
#define RV40_SPO_FLAG_UNRESTRICTEDMV		0x00000001	/* ANNEX D */
#define RV40_SPO_FLAG_EXTENDMVRANGE			0x00000002	/* IMPLIES NEW VLC TABLES */
#define RV40_SPO_FLAG_ADVMOTIONPRED			0x00000004	/* ANNEX F */
#define RV40_SPO_FLAG_ADVINTRA				0x00000008	/* ANNEX I */
#define RV40_SPO_FLAG_INLOOPDEBLOCK			0x00000010	/* ANNEX J */
#define RV40_SPO_FLAG_SLICEMODE				0x00000020	/* ANNEX K */
#define RV40_SPO_FLAG_SLICESHAPE			0x00000040  /* 0: free running; 1: rect */
#define RV40_SPO_FLAG_SLICEORDER			0x00000080  /* 0: sequential; 1: arbitrary */
#define RV40_SPO_FLAG_REFPICTSELECTION		0x00000100	/* ANNEX N */
#define RV40_SPO_FLAG_INDEPENDSEGMENT		0x00000200	/* ANNEX R */
#define RV40_SPO_FLAG_ALTVLCTAB				0x00000400	/* ANNEX S */
#define RV40_SPO_FLAG_MODCHROMAQUANT		0x00000800	/* ANNEX T */
#define RV40_SPO_FLAG_BFRAMES				0x00001000	/* SETS DECODE PHASE */
#define RV40_SPO_BITS_DEBLOCK_STRENGTH		0x0000e000  /* deblocking strength */
#define RV40_SPO_BITS_NUMRESAMPLE_IMAGES	0x00070000  /* max of 8 RPR images sizes */
#define RV40_SPO_FLAG_FRUFLAG				0x00080000  /* FRU HXBOOL: if 1 then OFF; */
#define RV40_SPO_FLAG_FLIP_FLIP_INTL		0x00100000  /* FLIP-FLOP interlacing; */
#define RV40_SPO_FLAG_INTERLACE				0x00200000  /* de-interlacing prefilter has been applied; */
#define RV40_SPO_FLAG_MULTIPASS				0x00400000  /* encoded with multipass; */
#define RV40_SPO_FLAG_INV_TELECINE			0x00800000  /* inverse-telecine prefilter has been applied; */
#define RV40_SPO_FLAG_VBR_ENCODE			0x01000000  /* encoded using VBR; */
#define RV40_SPO_BITS_DEBLOCK_SHIFT     13
#define RV40_SPO_BITS_NUMRESAMPLE_IMAGES_SHIFT 16



typedef struct tagRV10_INIT
{
	UINT16 outtype;
	UINT16 pels;
	UINT16 lines;
	UINT16 nPadWidth;	/* number of columns of padding on right to get 16 x 16 block*/
	UINT16 nPadHeight;	/* number of rows of padding on bottom to get 16 x 16 block*/

	UINT16 pad_to_32;   /* to keep struct member alignment independent of */
	                    /* compiler options */
	VOULONG32 ulInvariants;
	    /* ulInvariants specifies the invariant picture header bits */
	VOLONG32 packetization;
	VOULONG32 ulStreamVersion;
} RV10_INIT;


#ifdef __cplusplus
}		/* Assume C declarations for C++ */
#endif	/* __cplusplus */

#if defined(_MSC_VER)
#pragma pack(pop)
/*///////////////////////////////// */
/* */
/* End 1-byte structure alignment */
/* */
/*///////////////////////////////// */
#endif

/* Byte alignment settings for Mac */
#if defined( _MACINTOSH )
#pragma options align=reset
#endif

#endif /* __RV20DEC_H__ */



