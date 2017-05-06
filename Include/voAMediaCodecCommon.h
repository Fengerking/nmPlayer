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
#ifndef __VO_AMEDIACODEC_COMMON__H
#define __VO_AMEDIACODEC_COMMON__H

#define VO_ACODEC_SURFACE          'adSf'    /* Set Java Surface */
#define VO_ACODEC_JVM              'adJM'    /* Set Java VM */
#define VO_ACODEC_RENDER           'adRR'    /* Render the index data */
#define VO_ACODEC_DROP             'adDP'    /* Drop this index data */
#define VO_ACODEC_LIVE             'adLE'    /* Check whether the decoder is living */
#define VO_ACODEC_EOS              'adES'    /* Send a EOS flag into the decoder */
#define VO_ACODEC_DRM              'adRM'    /* Set sample whether is appened the drm information */
#define VO_ACODEC_RESET            'adRt'    /* Reset the hardware decoder only atfer the track information modification */
#define VO_ACODEC_SMODE            'adSD'    /* Set Scaling mode, one is VIDEO_SCALING_MODE_SCALE_TO_FIT 1, VIDEO_SCALING_MODE_SCALE_TO_FIT_WITH_CROPPING 2 */


// in order to reuse the logic that we have integrate iOMX codec, return the same value for deal with the retry
#define VO_ERR_IOMXDEC_BASE						(VO_ERR_BASE | 0x1200000)
#define VO_ERR_IOMXDEC_NeedRetry				(VO_ERR_IOMXDEC_BASE | 0x00005)

#endif // __VO_AMEDIACODEC_COMMON__H
