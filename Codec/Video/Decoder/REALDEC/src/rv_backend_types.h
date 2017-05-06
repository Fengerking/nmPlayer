/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: rv_backend_types.h,v 1.1.1.1.2.1 2005/05/04 18:20:57 hubbe Exp $
 * 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc.
 * All Rights Reserved.
 * 
 * The contents of this file, and the files included with this file,
 * are subject to the current version of the Real Format Source Code
 * Porting and Optimization License, available at
 * https://helixcommunity.org/2005/license/realformatsource (unless
 * RealNetworks otherwise expressly agrees in writing that you are
 * subject to a different license).  You may also obtain the license
 * terms directly from RealNetworks.  You may not use this file except
 * in compliance with the Real Format Source Code Porting and
 * Optimization License. There are no redistribution rights for the
 * source code of this file. Please see the Real Format Source Code
 * Porting and Optimization License for the rights, obligations and
 * limitations governing use of the contents of the file.
 * 
 * RealNetworks is the developer of the Original Code and owns the
 * copyrights in the portions it created.
 * 
 * This file, and the files included with this file, is distributed and
 * made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL
 * SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT
 * OR NON-INFRINGEMENT.
 * 
 * Technology Compatibility Kit Test Suite(s) Location:
 * https://rarvcode-tck.helixcommunity.org
 * 
 * Contributor(s):
 * 
 * ***** END LICENSE BLOCK ***** */

#ifndef RV_BACKEND_TYPES_H
#define RV_BACKEND_TYPES_H

#include "rv_format_info.h"

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */


/* definitions for output parameter notes */

#define RV_DECODE_MORE_FRAMES           0x00000001
#define RV_DECODE_DONT_DRAW             0x00000002
#define RV_DECODE_KEY_FRAME             0x00000004
    /* Indicates that the decompressed image is a key frame. */
    /* Note that enhancement layer EI frames are not key frames, in the */
    /* traditional sense, because they have dependencies on lower layer */
    /* frames. */

#define RV_DECODE_B_FRAME               0x00000008
    /* Indicates that the decompressed image is a B frame. */
    /* At most one of PIA_DDN_KEY_FRAME and PIA_DDN_B_FRAME will be set. */

#define RV_DECODE_DEBLOCKING_FILTER     0x00000010
    /* Indicates that the returned frame has gone through the */
    /* deblocking filter. */

#define RV_DECODE_FRU_FRAME             0x00000020
    /* Indicates that the decompressed image is a B frame. */
    /* At most one of PIA_DDN_KEY_FRAME and PIA_DDN_B_FRAME will be set. */

#define RV_DECODE_SCRAMBLED_BUFFER      0x00000040
    /* Indicates that the input buffer is scrambled for security */
    /* decoder should de-scramble the buffer before use it */

#define RV_DECODE_LAST_FRAME            0x00000200
    /* Indicates that the accompanying input frame is the last in the */
    /* current sequence. If input frame is a dummy frame, the decoder */
    /* flushes the latency frame to the output. */

/* definitions for decoding opaque data in bitstream header */
/* Defines match ilvcmsg.h so that ulSPOExtra == rv10init.invariants */
#define RV40_SPO_FLAG_UNRESTRICTEDMV        0x00000001  /* ANNEX D */
#define RV40_SPO_FLAG_EXTENDMVRANGE         0x00000002  /* IMPLIES NEW VLC TABLES */
#define RV40_SPO_FLAG_ADVMOTIONPRED         0x00000004  /* ANNEX F */
#define RV40_SPO_FLAG_ADVINTRA              0x00000008  /* ANNEX I */
#define RV40_SPO_FLAG_INLOOPDEBLOCK         0x00000010  /* ANNEX J */
#define RV40_SPO_FLAG_SLICEMODE             0x00000020  /* ANNEX K */
#define RV40_SPO_FLAG_SLICESHAPE            0x00000040  /* 0: free running; 1: rect */
#define RV40_SPO_FLAG_SLICEORDER            0x00000080  /* 0: sequential; 1: arbitrary */
#define RV40_SPO_FLAG_REFPICTSELECTION      0x00000100  /* ANNEX N */
#define RV40_SPO_FLAG_INDEPENDSEGMENT       0x00000200  /* ANNEX R */
#define RV40_SPO_FLAG_ALTVLCTAB             0x00000400  /* ANNEX S */
#define RV40_SPO_FLAG_MODCHROMAQUANT        0x00000800  /* ANNEX T */
#define RV40_SPO_FLAG_BFRAMES               0x00001000  /* SETS DECODE PHASE */
#define RV40_SPO_BITS_DEBLOCK_STRENGTH      0x0000e000  /* deblocking strength */
#define RV40_SPO_BITS_NUMRESAMPLE_IMAGES    0x00070000  /* max of 8 RPR images sizes */
#define RV40_SPO_FLAG_FRUFLAG               0x00080000  /* FRU BOOL: if 1 then OFF; */
#define RV40_SPO_FLAG_FLIP_FLIP_INTL        0x00100000  /* FLIP-FLOP interlacing; */
#define RV40_SPO_FLAG_INTERLACE             0x00200000  /* de-interlacing prefilter has been applied; */
#define RV40_SPO_FLAG_MULTIPASS             0x00400000  /* encoded with multipass; */
#define RV40_SPO_FLAG_INV_TELECINE          0x00800000  /* inverse-telecine prefilter has been applied; */
#define RV40_SPO_FLAG_VBR_ENCODE            0x01000000  /* encoded using VBR; */
#define RV40_SPO_BITS_DEBLOCK_SHIFT            13
#define RV40_SPO_BITS_NUMRESAMPLE_IMAGES_SHIFT 16

#define OUT_OF_DATE_DECODER                 0x00000001
#define OK_VERSION                          0x00000000

#define CORRUPTED_BITSTREAM                 0x00
#define OK_DECODE                           0x0f
#define INCOMPLETE_FRAME                    0xffff
#define MALLOC_FAILURE                      0x1111

#define RV10_DITHER_PARAMS                  0x00001001
#define RV10_POSTFILTER_PARAMS              0x00001002
#define RV10_ADVANCED_MP_PARAMS             0x0001003
#define RV10_TEMPORALINTERP_PARAMS          0x00001004

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* RV_BACKEND_TYPES_H */
