/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: helix_codec.h,v 1.1.1.1.2.1 2005/05/04 18:21:51 hubbe Exp $
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

#ifndef HELIX_CODEC_H
#define HELIX_CODEC_H

#include "helix_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

/* HX_MOF base Media object format struct */
typedef struct tag_HX_MOF
{
    VOUINT32      cbLength;    /* size of structure in bytes        */
    HX_MOFTAG   moftag;      /* identifier of media format        */
    HX_MOFTAG   submoftag;   /* identifier of object format       */
} HX_MOF;

/* Media Native HX_MOF struct */
typedef struct tag_HX_FORMAT_NATIVE
{
    VOUINT32      cbLength;    /* the size of this struct in bytes  */
    HX_MOFTAG   moftag;      /* always == HX_MEDIA_NATIVE         */
    HX_MOFTAG   submoftag;   /* media format identifier           */
    UINT8       data[1];     /* format native initialization data */
} HX_FORMAT_NATIVE;

/* Generic Audio HX_MOF struct */
typedef struct tag_HX_FORMAT_AUDIO
{
    VOUINT32      cbLength;           /* the size of this struct in bytes   */
    HX_MOFTAG   moftag;             /* always == HX_MEDIA_AUDIO           */
    HX_MOFTAG   submoftag;          /* audio format identifier            */
/* General attributes of the audio stream independent of bitwise encoding */
    UINT16      uiBitsPerSample;    /* number of bits per audio sample    */
    UINT16      uiNumChannels;      /* number of audio channels in stream */
    UINT16      uiBytesPerSample;   /* number of bytes per sample         */
    VOUINT32      ulSampleRate;       /* audio sample rate                  */
    VOUINT32      ulAvgBytesPerSec;   /* average bytes/sec of audio stream  */
} HX_FORMAT_AUDIO;

/* Generic Image HX_MOF struct */
typedef struct tag_HX_FORMAT_IMAGE
{
    VOUINT32        cbLength;         /* the size of this struct in bytes   */
    HX_MOFTAG     moftag;           /* always == HX_MEDIA_IMAGE           */
    HX_MOFTAG     submoftag;        /* image format identifier            */
/* General attributes of the image stream independent of bitwise encoding */
    UINT16        uiWidth;          /* width of the image in pixels       */
    UINT16        uiHeight;         /* height of the image in pixels      */
    UINT16        uiBitCount;       /* color depth in bits                */
    UINT16        uiPadWidth;       /* # of padded columns for codecs that */
                                    /* need certian block sizes e.g. 8x8  */
    UINT16        uiPadHeight;      /* # of padded rows for codecs that   */
                                    /* need certian block sizes e.g. 8x8  */
} HX_FORMAT_IMAGE;

/* Generic Image HX_MOF struct */
typedef struct tag_HX_FORMAT_IMAGE2
{
    VOUINT32        cbLength;         /* the size of this struct in bytes   */
    HX_MOFTAG     moftag;           /* always == HX_MEDIA_IMAGE           */
    HX_MOFTAG     submoftag;        /* image format identifier            */
/* General attributes of the image stream independent of bitwise encoding */
    UINT16        uiWidth;          /* width of the image in pixels       */
    UINT16        uiHeight;         /* height of the image in pixels      */
    UINT16        uiBitCount;       /* color depth in bits                */
    UINT16        uiPadWidthLeft;   /* # of padded columns for codecs that */
                                    /* need certian block sizes e.g. 8x8  */
    UINT16        uiPadHeightTop;   /* # of padded rows for codecs that   */
    UINT16        uiPadWidthRight;  /* # of padded columns for codecs that */
                                    /* need certian block sizes e.g. 8x8  */
    UINT16        uiPadHeightBottom;/* # of padded rows for codecs that   */
                                    /* need certian block sizes e.g. 8x8  */
} HX_FORMAT_IMAGE2;

/* Generic Video HX_MOF struct */
typedef struct tag_HX_FORMAT_VIDEO
{
    VOUINT32        cbLength;         /* the size of this struct in bytes   */
    HX_MOFTAG     moftag;           /* always == HX_MEDIA_VIDEO           */
    HX_MOFTAG     submoftag;        /* video format identifier            */
/* General attributes of the video stream independent of bitwise encoding */
    UINT16        uiWidth;          /* width of the image in pixels       */
    UINT16        uiHeight;         /* height of the image in pixels      */
    UINT16        uiBitCount;       /* color depth in bits                */
    UINT16        uiPadWidth;       /* # of padded columns for codecs that */
                                    /* need certian block sizes e.g. 8x8  */
    UINT16        uiPadHeight;      /* # of padded rows for codecs that   */
                                    /* need certian block sizes e.g. 8x8  */
    UFIXED32      framesPerSecond;  /* frames per second                  */

} HX_FORMAT_VIDEO;


/* Unfortunately we did not 4 byte align the HX_FORMAT_VIDEO struct. Since
 * this struct must be 26 bytes in size we cannot do sizeof(HX_FORMAT_VIDEO)
 * on several UNIX platforms and PowerPC macs. We are therefore going to
 * hardcode the size to 26 bytes.
 */
#define SIZE_OF_HX_FORMAT_VIDEO 26


/* Segment info struct */
typedef struct tag_HXCODEC_SEGMENTINFO
{
    VOINT32  bIsValid;
    VOUINT32 ulSegmentOffset;
} HXCODEC_SEGMENTINFO;

#define HXCODEC_SEGMENTINFO_SIZE    (8)


/* HXCODEC_DATA struct */
typedef struct tag_HXCODEC_DATA
{
    VOUINT32      dataLength;
    UCHAR      *data;
    VOUINT32      timestamp;
    UINT16      sequenceNum;
    UINT16      flags;
    BOOL        lastPacket;
    VOUINT32      numSegments;
    HXCODEC_SEGMENTINFO *Segments;
} HXCODEC_DATA;

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* #ifndef HELIX_CODEC_H */
