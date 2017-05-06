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

#ifndef __voDRM_H__
#define __voDRM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"

/**
* Error code
*/
#define VO_ERR_DRM_OK					VO_ERR_NONE
#define VO_ERR_DRM_BASE					0x86600000
#define VO_ERR_DRM_MODULENOTFOUND		(VO_ERR_DRM_BASE | 0x0001)			/*!< can not find specific DRM module */
#define VO_ERR_DRM_BADPARAMETER			(VO_ERR_DRM_BASE | 0x0002)			/*!< parameter is not valid */
#define VO_ERR_DRM_ERRORSTATE			(VO_ERR_DRM_BASE | 0x0003)			/*!< error state */
#define VO_ERR_DRM_BADDRMINFO			(VO_ERR_DRM_BASE | 0x0004)			/*!< the DRM information in file can not be parsed correctly */
#define VO_ERR_DRM_BADDRMDATA			(VO_ERR_DRM_BASE | 0x0005)			/*!< the DRM encrypted data can not be decrypted correctly */
#define VO_ERR_DRM_USERCANCEL			(VO_ERR_DRM_BASE | 0x0006)			/*!< user cancel the operation */
#define VO_ERR_DRM_BADRESOLUTION		(VO_ERR_DRM_BASE | 0x0007)			/*!< user cancel the operation */
#define VO_ERR_DRM_UNDEFINED			(VO_ERR_DRM_BASE | 0xFFFF)			/*!< Undefined error */


/**
 * DRM type
 */
typedef enum
{
	VO_DRMTYPE_UNKNOWN				= 0,
	VO_DRMTYPE_DIVX					= 1,			/*!< DivX DRM */
	VO_DRMTYPE_WindowsMedia			= 2,			/*!< WM DRM */
	VO_DRMTYPE_PlayReady			= 3,			/*!< PlayReady */
	VO_DRMTYPE_Widevine				= 4,			/*!< Widevine */
	VO_DRMTYPE_Irdeto				= 5,			/*!< Irdeto in HLS*/
	VO_DRMTYPE_AES128				= 6,			/*!< AES 128*/
	VO_DRMTYPE_MAX					= VO_MAX_ENUM_VALUE
} VO_DRM_TYPE;

/**
* DRM data type
*/
typedef enum
{
	VO_DRMDATATYPE_UNKNOWN			= 0,
	VO_DRMDATATYPE_MEDIASAMPLE		= 1,
	VO_DRMDATATYPE_AUDIOSAMPLE		= 2,
	VO_DRMDATATYPE_VIDEOSAMPLE		= 3,
	VO_DRMDATATYPE_PACKETDATA		= 4,
	VO_DRMDATATYPE_JITSAMPLE		= 5,
	VO_DRMDATATYPEE_MAX				= VO_MAX_ENUM_VALUE
} VO_DRM_DATATYPE;

typedef enum
{
	VO_DRM_DATAINFO_UNKNOWN				= 0,
	VO_DRM_DATAINFO_SAMPLEDECYPTION_BOX	= 0x1,	//Sample Encryption Box
	VO_DRM_DATAINFO_SINGLE_SAMPLEDECYPTION	= 0x2,	//the related sub-sample encryption data

	VO_DRM_DATAINFO_MAX						= VO_MAX_ENUM_VALUE
} VO_DRM_DATAINFO_TYPE;

typedef struct
{
	VO_DRM_DATATYPE		nDataType;
	VO_PTR				pInfo;
	VO_U32				nReserved[6];
} VO_DRM_DATAINFO;

typedef enum
{
	VO_DRMMODE_NONE					= 0x00,		/*!< unknown mode */
	VO_DRMMODE_PURCHASE				= 0x01,		/*!< purchase mode */
	VO_DRMMODE_RENTAL_COUNT			= 0x02,		/*!< rental mode, control used count */
	VO_DRMMODE_RENTAL_TIME			= 0x03,		/*!< rental mode, control used time */
	VO_DRMMODE_MAX					= VO_MAX_ENUM_VALUE
} VO_DRM_MODE;


typedef enum
{
	VO_DRM_API_VO_BASE			= 0x000,
	VO_DRM_API_DIVX_BASE		= 0x100,
	VO_DRM_API_WMDRM_BASE		= 0x200,
	VO_DRM_API_PLAYREADY_BASE	= 0x300,
	VO_DRM_API_LG_PLAYREADY		= 0x301,

}VO_DRM_API_TYPE;

typedef struct
{
	VO_PBYTE			pDRMHeader;
	VO_U32				nDRMHeader;
} VO_DRM_INFO;

typedef struct
{
	VO_U32				nTotal;
	VO_U32				nUsed;
	VO_U32				nReserved[2];
} VO_DRM_RENTAL_COUNT_INFO;

typedef struct
{
	VO_U32				nFrom;
	VO_U32				nTo;
	VO_U32				nReserved[2];
} VO_DRM_RENTAL_TIME_INFO;

typedef struct
{
    VO_U8           m_btCgmsa;
    VO_U8           m_btAcptb;
    VO_U8           m_btDigitalProtection;
} VO_DRM_OUTPUT_PROTECT_INFO;

typedef struct
{
	VO_DRM_MODE			nMode;		//DRM mode
	union
	{
		VO_DRM_RENTAL_COUNT_INFO	iRentalCount;
		VO_DRM_RENTAL_TIME_INFO		iRentalTime;
	} uInfo;

	VO_DRM_OUTPUT_PROTECT_INFO 		iOutputProtect;
	
	VO_U32				nReserved[3];
} VO_DRM_FORMAT;

/**
 * DRM function set
 */
typedef struct
{
	/**
	 * Open the DRM and return DRM handle
	 * \param ppHandle [OUT] Return the DRM operator handle
	 * \param nType	[IN] The DRM type
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Open)(VO_PTR * ppHandle);

	/**
	 * Close the opened DRM.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * Close)(VO_PTR pHandle);

	/**
	* Set param for special target.
	* \param pHandle [IN] The handle which was create by open function.
	* \param uID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID
	* \retval VO_ERR_NONE Succeeded. 
	*/
	VO_U32 (VO_API * SetParameter)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	/**
	* Get param for special target.
	* \param pHandle [IN] The handle which was create by open function.
	* \param uID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID
	* \retval VO_ERR_NONE Succeeded.
	*/
	VO_U32 (VO_API * GetParameter)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	/**
	* Tell the DRM data in file
	* \param ppHandle [OUT] Return the DRM operator handle
	* \param nFlag [IN] reserved flag
	* \param pDrmInfo [IN] The DRM information in file
	* \retval VO_ERR_NONE Succeeded.
	*/
	VO_U32 (VO_API * SetDrmInfo)(VO_PTR pHandle, VO_U32 nFlag, VO_PTR pDrmInfo);

	/**
	 * Get DRM format.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param pFormat [OUT] DRM format.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetDrmFormat)(VO_PTR pHandle, VO_DRM_FORMAT* pDrmFormat);

	/**
	 * Check the resolution for protected files.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param nWidth  [IN] Video width.
	 * \param nHeight [IN] Height width.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * CheckCopyRightResolution)(VO_PTR pHandle, VO_U32 nWidth, VO_U32 nHeight);


	/**
	* Do Commit.
	* \param pHandle [IN] The handle which was create by open function.
	* \retval VO_ERR_NONE Succeeded.
	*/
	VO_U32 (VO_API * Commit)(VO_PTR pHandle);

	/**
	* Decrypt DRM encrypted data.
	* for fixed size DRM
	* \param pHandle [IN] The handle which was create by open function.
	* \param pDataInfo [IN] The information data needed by DecryptDataXXX.
	* \param pData [IN/OUT] The DRM data buffer.
	* \param nSize [IN] The DRM data size.
	* \retval VO_ERR_NONE Succeeded.
	*/
	VO_U32 (VO_API * DecryptData)(VO_PTR pHandle, VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pData, VO_U32 nSize);

	/**
	* Decrypt DRM encrypted data.
	* for changed size DRM, reserved, not used
	* \param pHandle [IN] The handle which was create by open function.
	* \param pDataInfo [IN] The information data needed by DecryptDataXXX.
	* \param pSrcData [IN] The DRM data buffer.
	* \param nSrcSize [IN] The DRM data size.
	* \param ppDstData [OUT] The pointer of raw data buffer.
	* \param pnDstSize [OUT] The pointer of raw data size.
	* \retval VO_ERR_NONE Succeeded.
	*/
	VO_U32 (VO_API * DecryptData2)(VO_PTR pHandle, VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE* ppDstData, VO_U32* pnDstSize);
} VO_DRM_API;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voDRM_H__
