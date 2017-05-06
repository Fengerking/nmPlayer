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

#ifndef __vompDrm_H__
#define __vompDrm_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "vompType.h"

/**
* Drm Error code
*/
#define VOMP_ERR_DRM_OK					VOMP_ERR_None
#define VOMP_ERR_DRM_BASE				0x86600000
#define VOMP_ERR_DRM_MODULENOTFOUND		(VOMP_ERR_DRM_BASE | 0x0001)			/*!< can not find specific DRM module */
#define VOMP_ERR_DRM_BADPARAMETER		(VOMP_ERR_DRM_BASE | 0x0002)			/*!< parameter is not valid */
#define VOMP_ERR_DRM_ERRORSTATE			(VOMP_ERR_DRM_BASE | 0x0003)			/*!< error state */
#define VOMP_ERR_DRM_BADDRMINFO			(VOMP_ERR_DRM_BASE | 0x0004)			/*!< the DRM information in file can not be parsed correctly */
#define VOMP_ERR_DRM_BADDRMDATA			(VOMP_ERR_DRM_BASE | 0x0005)			/*!< the DRM encrypted data can not be decrypted correctly */
#define VOMP_ERR_DRM_USERCANCEL			(VOMP_ERR_DRM_BASE | 0x0006)			/*!< user cancel the operation */
#define VOMP_ERR_DRM_BADRESOLUTION		(VOMP_ERR_DRM_BASE | 0x0007)			/*!< user cancel the operation */
#define VOMP_ERR_DRM_UNDEFINED			(VOMP_ERR_DRM_BASE | 0xFFFF)			/*!< Undefined error */


/**
 * DRM type
 */
typedef enum
{
	VOMP_DRMTYPE_UNKNOWN				= 0,
	VOMP_DRMTYPE_DIVX					= 1,			/*!< DivX DRM */
	VOMP_DRMTYPE_WindowsMedia			= 2,			/*!< WM DRM */
	VOMP_DRMTYPE_MAX					= 0x7FFFFFFF
} VOMP_DRM_TYPE;

/**
* DRM data type
*/
typedef enum
{
	VOMP_DRMDATATYPE_UNKNOWN			= 0, 
	VOMP_DRMDATATYPE_MEDIASAMPLE		= 1, 
	VOMP_DRMDATATYPE_AUDIOSAMPLE		= 2, 
	VOMP_DRMDATATYPE_VIDEOSAMPLE		= 3, 
	VOMP_DRMDATATYPE_PACKETDATA			= 4, 
	VOMP_DRMDATATYPEE_MAX				= 0x7FFFFFFF
} VOMP_DRM_DATATYPE;

typedef struct
{
	VOMP_DRM_DATATYPE		nDataType;
	void *					pInfo;
	int						nReserved[6];
} VOMP_DRM_DATAINFO;


/**
* DRM callback
*/
#define VOMP_SOURCEDRM_FLAG_DRMINFO			0x1
#define VOMP_SOURCEDRM_FLAG_DRMDATA			0x2
#define VOMP_SOURCEDRM_FLAG_FLUSH			0x10
#define VOMP_SOURCEDRM_FLAG_ISSUPPORTED		0x20

typedef struct
{
	VOMP_DRM_TYPE			nType;			//DRM type
	void *					pDrmInfo;		//DRM information in file, the format is decided by nType
	int						nReserved[6];	//reserved for future
} VOMP_SOURCEDRM_INFO;

typedef struct
{
	VOMP_DRM_DATAINFO			sDataInfo;		//information needed by DecryptDataXXX
	unsigned char *				pData;			//encrypted data buffer
	int							nSize;			//encrypted data size
	unsigned char **			ppDstData;		//reserved for size-changed DRM
	int*						pnDstSize;		//reserved for size-changed DRM
	int							nReserved[4];	//reserved for future
} VOMP_SOURCEDRM_DATA;

/**
* VisualOn file parser DRM callback.
* \param pUserData: user data set in.
* \param nFlag: VOMP_SOURCEDRM_FLAG_XXX.
* \param pParam: VOMP_SOURCEDRM_FLAG_DRMINFO - VOMP_SOURCEDRM_INFO*; VOMP_SOURCEDRM_FLAG_DRMDATA - VOMP_SOURCEDRM_DATA*; VO_SOURCEDRM_FLAG_ISSUPPORTED - VOMP_DRM_TYPE*.
* \param nReserved: reserved.
* \retval VOMP_ERR_NONE Succeeded, VOMP_ERR_DRM_XXX.
*/
typedef int (* VOMPSOURCEDRMCALLBACK)(void * pUserData, int nFlag, void * pParam, int nReserved);

typedef struct
{
	VOMPSOURCEDRMCALLBACK	fCallback;		//callback function pointer
	void *					pUserData;		//user data
} VOMP_SOURCEDRM_CALLBACK;

//The object buffer not include id(GUID)
typedef struct
{
	unsigned char *			pContentEncryptionObject;
	int						nContentEncryptionObject;
	unsigned char *			pExtendedContentEncryptionObject;
	int						nExtendedContentEncryptionObject;
} VOMP_WMDRM_INFO;

typedef struct
{
	unsigned char *			pStrdChunk;
	int						nStrdChunk;
} VOMP_DIVXDRM_INFO;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __vompDrm_H__
