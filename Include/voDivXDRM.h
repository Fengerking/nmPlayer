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

#ifndef __voDivXDRM_H__
#define __voDivXDRM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voDRM.h"

typedef struct
{
	VO_PBYTE			pStrdChunk;
	VO_U32				nStrdChunk;
} VO_DIVXDRM_INFO;

#define VO_DIVXDRM_DD_INFO_LENGTH		10

typedef enum
{
	DIVX_DRM_DEVICE_REGISTERED		= 0x00,		/*!< device activated */
	DIVX_DRM_DEVICE_NOTREGISTERED	= 0x01,		/*!< device not activated */
	DIVX_DRM_DEVICE_NEVERREGISTERED	= 0x02,		/*!< device never activated */
	DIVX_DRM_DEVICE_MAX				= VO_MAX_ENUM_VALUE
} VODIVXDRMDEVICESTATUS;

typedef enum
{
	DIVX_DRM_ID_INITDRMMEMORY		= 0x01,		/*!< <S> initialize drm memory, call without Open API, NULL */
	DIVX_DRM_ID_RANDOMSAMPLE		= 0x02,		/*!< <S> set random sample, NULL */
	//DIVX_DRM_ID_MODE				= 0x03,		/*!< <S> set normal mode or JIT mode, iVersion */

	DIVX_DRM_ID_ACTIVATIONSTATUS	= 0x11,		/*!< <G> get device activation status, VODIVXDRMDEVICESTATUS* */
	DIVX_DRM_ID_REGCODESTRING		= 0x12,		/*!< <G> get registration code string, char[16], null terminate string */
	DIVX_DRM_ID_DEACTCODESTRING		= 0x13,		/*!< <G> get deactivation code string, char[16], null terminate string */
	DIVX_DRM_ID_ISDRMMEMINITIALIZED = 0x14,		/*!< <G> get the drm memory status */

	//DIVX_DRM_ID_DECODER_INIT		= 0x20,		/*!< <S> init from decoder for JIT, NULL*/


	DIVX_DRM_ID_MAX					= VO_MAX_ENUM_VALUE
} VODIVXDRMID;

typedef enum 
{
    DIVX_DRM_SUCCESS                = 0x00,
    DIVX_DRM_NOT_AUTHORIZED         = 0x01,
    DIVX_DRM_NOT_REGISTERED         = 0x02,
    DIVX_DRM_RENTAL_EXPIRED         = 0x03,
    DIVX_DRM_GENERAL_ERROR          = 0x04,
    DIVX_DRM_NEVER_REGISTERED       = 0x04,
	DIVX_DRM_MAX					= VO_MAX_ENUM_VALUE
} VODIVXDRMRETURN;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voDivXDRM_H__
