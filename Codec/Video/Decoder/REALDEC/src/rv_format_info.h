/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: rv_format_info.h,v 1.1.1.1.2.1 2005/05/04 18:20:57 hubbe Exp $
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

#ifndef RV_FORMAT_INFO_H
#define RV_FORMAT_INFO_H

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

#include "helix_types.h"

typedef struct rv_format_info_struct
{
    VOUINT32   ulLength;
    VOUINT32   ulMOFTag;
    VOUINT32   ulSubMOFTag;
    UINT16   usWidth;
    UINT16   usHeight;
    UINT16   usBitCount;
    UINT16   usPadWidth;
    UINT16   usPadHeight;
    UFIXED32 ufFramesPerSecond;
    VOUINT32   ulOpaqueDataSize;
    BYTE*    pOpaqueData;
} rv_format_info;

/*
 * RV frame struct.
 */
typedef struct rv_segment_struct
{
    HXBOOL bIsValid;
    VOUINT32 ulOffset;
} rv_segment;

typedef struct rv_frame_struct
{
    VOUINT32             ulDataLen;
    BYTE*              pData;
    VOUINT32             ulTimestamp;
    UINT16             usSequenceNum;
    UINT16             usFlags;
    HXBOOL             bLastPacket;
    VOUINT32             ulNumSegments;
    rv_segment* pSegment;
} rv_frame;

#define BYTE_SWAP_UINT16(A)  ((((UINT16)(A) & 0xff00) >> 8) | \
                              (((UINT16)(A) & 0x00ff) << 8))
#define BYTE_SWAP_UINT32(A)  ((((VOUINT32)(A) & 0xff000000) >> 24) | \
                              (((VOUINT32)(A) & 0x00ff0000) >> 8)  | \
                              (((VOUINT32)(A) & 0x0000ff00) << 8)  | \
                              (((VOUINT32)(A) & 0x000000ff) << 24))

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* #ifndef RV_FORMAT_INFO_H */
