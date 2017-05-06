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

#ifndef __voDRM2_H__
#define __voDRM2_H__
#include "voIndex.h"
#include "voSource2_IO.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
* VisualOn file parser DRM callback.
* \param pUserData: user data set in.
* \param nFlag: VO_DRM_CALLBACK_FLAG.
* \param pParam: VO_SOURCEDRM_FLAG_DRMINFO - VO_SOURCEDRM_INFO*; VO_SOURCEDRM_FLAG_DRMDATA - VO_SOURCEDRM_DATA*; VO_SOURCEDRM_FLAG_ISSUPPORTED - VO_DRM_TYPE*.
* \param eSourceFormat: VO_DRM2_SOURCE_FORMAT.
* \retval VO_ERR_NONE Succeeded, VO_ERR_DRM2_XXX.
*/
typedef VO_U32 (VO_API * VOSOURCEDRMCALLBACK2)(VO_PTR pUserData, VO_U32 nFlag, VO_PTR pParam, VO_U32 eSourceFormat);


/**
* Error code
*/
#define VO_ERR_DRM2_OK					VO_ERR_NONE
#define VO_ERR_DRM2_BASE				0x86600000
#define VO_ERR_DRM2_MODULENOTFOUND		(VO_ERR_DRM2_BASE | 0x0001)			/*!< can not find specific DRM module */
#define VO_ERR_DRM2_BADPARAMETER		(VO_ERR_DRM2_BASE | 0x0002)			/*!< parameter is not valid */
#define VO_ERR_DRM2_ERRORSTATE			(VO_ERR_DRM2_BASE | 0x0003)			/*!< error state */
#define VO_ERR_DRM2_BADDRMINFO			(VO_ERR_DRM2_BASE | 0x0004)			/*!< the DRM information in file can not be parsed correctly */
#define VO_ERR_DRM2_BADDRMDATA			(VO_ERR_DRM2_BASE | 0x0005)			/*!< the DRM encrypted data can not be decrypted correctly */
#define VO_ERR_DRM2_USERCANCEL			(VO_ERR_DRM2_BASE | 0x0006)			/*!< user cancel the operation */
#define VO_ERR_DRM2_BADRESOLUTION		(VO_ERR_DRM2_BASE | 0x0007)			/*!< resolution is not support */
#define VO_ERR_DRM2_NOTENCRYPTION		(VO_ERR_DRM2_BASE | 0x0008)			/*!< the data is not encryted */
#define VO_ERR_DRM2_NO_DRM_API			(VO_ERR_DRM2_BASE | 0x0009)			/*!< the drm interface is null */
#define VO_ERR_DRM2_NEEDRETRY			(VO_ERR_DRM2_BASE | 0x0010)			/*!< please try again */
#define VO_ERR_DRM2_OUTPUT_BUF_SMALL	(VO_ERR_DRM2_BASE | 0x0011)			/*!< the output buffer is not enough */
#define VO_ERR_DRM2_NOTIMPLEMENTED		(VO_ERR_DRM2_BASE | 0x0012)			/*!< the feature is not implemented */
#define VO_ERR_DRM2_MODIFIED_INPUT		(VO_ERR_DRM2_BASE | 0x0013)			/*!< the input data has been modified, such as url, playlist*/
#define VO_ERR_DRM2_UNDEFINED			(VO_ERR_DRM2_BASE | 0xFFFF)			/*!< Undefined error */


/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_DRM2_BASE					0x43200000					/*!< the base param ID for DRM2 modules */
#define VO_PID_DRM2_PackagePath				(VO_PID_DRM2_BASE | 0x0001) /*!< <S>set the path of the package*/
#define	VO_PID_DRM2_CB						(VO_PID_DRM2_BASE | 0x0002) /*!< <G>get the VO's DRM callback pointer*/
#define VO_PID_DRM2_UNIQUE_IDENTIFIER		(VO_PID_DRM2_BASE | 0x0003) /*!< <S>like IMEI or MAC of the device*/
#define VO_PID_DRM2_AUTHENTICATION_SERVER	(VO_PID_DRM2_BASE | 0x0004) /*!< <S>the server used to entitle authentication*/
#define VO_PID_DRM2_CUSTOMIZATION_DATA		(VO_PID_DRM2_BASE | 0x0005) /*!< <S>set specified data of customer. refer to VO_DRM2_CUSTOMIZATION_DATA*/
#define VO_PID_DRM2_EXIT					(VO_PID_DRM2_BASE | 0x0006) /*!< <S>notify stop signal to DRM module. VO_TURE*/
#define VO_PID_DRM2_VODRM_COMMON_API		(VO_PID_DRM2_BASE | 0x0007) /*!< <S>set the pointer of VO common DRM object. refer to VOOSMPDRM*/
#define VO_PID_DRM2_IOAPI					(VO_PID_DRM2_BASE | 0x0008)	/*!< <S>set the Source2 IO api pointer, VO_SOURCE2_IO_API* */
#define VO_PID_DRM2_WRITABLE_PATH			(VO_PID_DRM2_BASE | 0x0009) /*!< <S>Set a writable path*/
#define VO_PID_DRM2_RUNNING					(VO_PID_DRM2_BASE | 0x000A) /*!< <S>If true, DRM running. If false, DRM destroy. temprary for 33579, this feature will be explict interface(open/close).*/



/**
* event callback ID
*/
#define VO_EVENTID_DRM2_BASE						0xcbd00000
#define VO_EVENTID_DRM2_CUSTOMIZATION_DATA			(VO_EVENTID_DRM2_BASE | 0x0001)	/*!<call back to get some specified data of customer*/

#define VO_EVENTID_DRM2_ERR_BASE					0xcbe30000
#define VO_EVENTID_DRM2_ERR_TIME_NOT_TRUSTED		(VO_EVENTID_DRM2_ERR_BASE | 0x0001)	/*!<Time rollback detected, connection to the secure time server failed or the device system time is before the trusted time from time server*/
#define VO_EVENTID_DRM2_ERR_NOT_SECURE				(VO_EVENTID_DRM2_ERR_BASE | 0x0002)	/*!<Device is rooted.*/
#define VO_EVENTID_DRM2_ERR_POLICY_FAIL				(VO_EVENTID_DRM2_ERR_BASE | 0x0003)	/*!<This typically means that the device uses a/v output device but the license doesnt allows it.*/

#define VO_SOURCE2_CB_DRM_INITDATA					(VO_SOURCE2_CALLBACKIDBASE_DRM | 0x0001)/*!<call back the DRM init data*/


/**
 * VisualOn DRM engine open param structure
 */
typedef struct
{
	VO_U32					nFlag;			/*!< open flags  */
	VO_LIB_OPERATOR *		pLibOP;			/*!< Library operator function set. */
	VO_U32					nReserve;		/*!< reserved. */
} VO_DRM_OPENPARAM;

/**
 * Structure of DRM callback interface
 */
typedef struct
{
	VOSOURCEDRMCALLBACK2	fCallback;		//callback function pointer
	VO_PTR					pUserData;		//user data
	VO_CHAR					szDRMTYPE[128];	//DRM TYPE
} VO_SOURCEDRM_CALLBACK2;


/**
 * Enumeration for callback function param flag
 */
enum VO_DRM_CALLBACK_FLAG
{
	VO_DRM_FLAG_DRMINFO			= 0x1,		/*!< set DRM header. param struct VO_DRM2_INFO for local file reader and entire raw file; or VO_DRM2_INFO_ADAPTIVESTREAMING for adaptive streaming */
	VO_DRM_FLAG_DRMDATA			= 0x2,		/*!< decrypt encrypted data. param struct VO_DRM2_DATA */
	VO_DRM_FLAG_GETINFOLOCATION	= 0x4,		/*!< get the position of DRM header. param struct VO_DRM2_INFO_GET_LOCATION */
	VO_DRM_FLAG_CONVERTLOCATION	= 0x8,		/*!< get the actual position for IO. param struct VO_DRM2_DATA_CONVERT_LOCATION */
	VO_DRM_FLAG_FLUSH			= 0x10,
	VO_DRM_FLAG_ISSUPPORTED		= 0x20,
	VO_DRM_FLAG_PLAYLIST		= 0x40,		/*!< transform the playlist for customer. param struct VO_DRM2_CONVERT_PLAYLIST */
	VO_DRM_FLAG_NEWAPI			= 0x80,		/*!< get a new DRM wrapper for multi-thread */
	VO_DRM_FLAG_CONVERTURL		= 0x100,	/*!< transform the URL for customer. param struct VO_DRM2_CONVERT_URL */
	VO_DRM_FLAG_GETORIGINALSIZE	= 0x200,	/*!< get the original file size (without DRM length) */
	VO_DRM_FLAG_DRM_PROCECTION	= 0x400,	/*!< set Protection System Specific Header box */
	VO_DRM_FLAG_DRM_TRACK_INFO	= 0x800,	/*!< set Track Encryption Box */
	VO_DRM_FLAG_DELETE_CONTENT	= 0x1000	/*!< delete stored drm info*/
};

typedef struct
{
	VO_PBYTE	pTrackEncryptionBox; // Track encryption box
	VO_U32		uTrackEncryptionBox; // the size of Track encryption box
	VO_U32		uTrackID;    // the unique identifier of track
} VO_DRM2_TRACKDECYPTION_INFO;

typedef struct
{
	VO_PBYTE	pSampleEncryptionBox;        // Entire Sample encryption box (containing IVs and DRM data)
	VO_U32		uSampleEncryptionBoxSize;
	VO_U32		uSampleIndex;               // Index of the sample data within this movie fragment.
	VO_U32		uTrackId;
} VO_DRM2_SAMPLEDECYPTION_INFO;

typedef enum
{
	VO_DRM2_INFO_UNKNOWN		= 0,
	VO_DRM2_INFO_PROCECTION		= 0x1,	//VO_DRM2_INFO_GENERAL_DATA
	VO_DRM2_INFO_TRACKDECYPTION	= 0x10,	//VO_DRM2_TRACKDECYPTION_INFO

	VO_DRM2_INFO_MAX			= VO_MAX_ENUM_VALUE
} VO_DRM2_INFO_TYPE;

/**
 * Enumeration for DRM source format type
 */
typedef enum
{
	VO_DRM2SRC_MEDIAFORMAT			= 0,			/*!< Media format, like AVI, MP4, ASF. File reader pass DRM header and encrypted content to DRM engine.*/

	VO_DRM2SRC_CHUNK				= 0x8000,						/*!< File chunk. All chunk could be encrypted */
	VO_DRM2SRC_CHUNK_HLS			= (VO_DRM2SRC_CHUNK | 0x100),	/*!< HTTP Live streaming chunk */
	VO_DRM2SRC_CHUNK_SSTR			= (VO_DRM2SRC_CHUNK | 0x200),	/*!< Smooth streaming chunk */
	VO_DRM2SRC_CHUNK_DASH			= (VO_DRM2SRC_CHUNK | 0x400),	/*!< DASH streaming chunk */

	VO_DRM2SRC_RAWFILE				= 0x800000,		/*!< The whole file could be encrypted*/

	VO_DRM2SRC_DOWNLOAD				= 0x10000000,										/*!< For Download Manager. Store nessecery DRM info to support offline playback.*/
	VO_DRM2SRC_DOWNLOAD_HLS			= (VO_DRM2SRC_DOWNLOAD | VO_DRM2SRC_CHUNK_HLS),		/*!< HTTP Live streaming in download manager*/
	VO_DRM2SRC_DOWNLOAD_SSTR		= (VO_DRM2SRC_DOWNLOAD | VO_DRM2SRC_CHUNK_SSTR),	/*!< Smooth streaming chunk in download manager*/
	VO_DRM2SRC_DOWNLOAD_DASH		= (VO_DRM2SRC_DOWNLOAD | VO_DRM2SRC_CHUNK_DASH),	/*!< DASH streaming chunk  in download manager*/

	VO_DRM2SRC_MAX					= VO_MAX_ENUM_VALUE
} VO_DRM2_SOURCE_FORMAT;


/**
 * Structure used to transfer URL for special customer
 */
typedef struct
{
	const VO_CHAR*	urlSrc; //<in>original URL 
	VO_CHAR*		urlDes; //<out>modified URL

	VO_PTR			pReserved;
} VO_DRM2_CONVERT_URL;


/**
 * Enumeration of possible DRM type
 */
typedef enum
{
	VO_DRM2TYPE_UNKNOWN				= 0,
	VO_DRM2TYPE_DIVX				= 1,			/*!< DivX DRM */
	VO_DRM2TYPE_WindowsMedia		= 2,			/*!< WM DRM */
	VO_DRM2TYPE_PlayReady			= 3,			/*!< PlayReady */
	VO_DRM2TYPE_Widevine			= 4,			/*!< Widevine */
	VO_DRM2TYPE_Irdeto				= 5,			/*!< Irdeto in HLS*/
	VO_DRM2TYPE_AES128				= 6,			/*!< AES 128*/
	VO_DRM2TYPE_MAX					= VO_MAX_ENUM_VALUE
} VO_DRM2_TYPE;

/**
 * Structure of DRM info, used in local file reader and entire raw file.
 */
typedef struct
{
	VO_DRM2_TYPE		nType;			//DRM type
	VO_PTR				pDrmInfo;		//the format is decided by nType, like VO_DRM2_INFO_GENERAL_DATA
	VO_U32				nReserved[6];	//reserved for future
} VO_DRM2_INFO;

/**
 * Structure used to store DRM header info.
 */
typedef struct
{
	VO_BYTE*	pData;	// data of DRM header info
	VO_U32		uSize;	// size of DRM header info
} VO_DRM2_INFO_GENERAL_DATA;

/**
 * Structure of DRM info for Adaptive Streaming
 */	
typedef struct
{
	VO_CHAR				szDRMTYPE[128];

	VO_CHAR*			pURL;			// Manifest URL
    VO_BYTE*			pManifestData;	//data of Manifest
    VO_U32				uSizeManifest;	//size of Manifest

	VO_SOURCE2_IO_API*	pAPI_IO;		//the API of source IO

	VO_PTR				pReserved;
} VO_DRM2_INFO_ADAPTIVESTREAMING;

/**
* new memory function pointer
* \param uSize: wanted space
* \retval the pointer of allocated. if NULL, means no enough space
*/
typedef unsigned char* (VO_API * ALLOCATEMEM)(VO_U32 uSize);

/**
 * Structure used to store DRM header info and receive some response
 */
typedef struct
{
	VO_BYTE*	pData;	// data of DRM header info
	VO_U32		uSize;	// size of DRM header info

	VO_BYTE*	pRetData;	// data of custom DRM header info
	VO_U32		uRetSize;	// size of custom DRM header info

	ALLOCATEMEM fAllocate;
} VO_DRM2_INFO_WITH_RESPONSE;


/**
* Enumeration of DRM data type
*/
typedef enum
{
	VO_DRM2DATATYPE_UNKNOWN			= 0,
	VO_DRM2DATATYPE_MEDIASAMPLE		= 1,
	VO_DRM2DATATYPE_AUDIOSAMPLE		= 2,
	VO_DRM2DATATYPE_VIDEOSAMPLE		= 3,
	VO_DRM2DATATYPE_PACKETDATA		= 4,
	VO_DRM2DATATYPE_JITSAMPLE		= 5,
	VO_DRM2DATATYPE_SEQUENCEHEADER	= 6,


	VO_DRM2DATATYPE_CHUNK_BEGIN			= 10,	// for Live Source Streaming
	VO_DRM2DATATYPE_CHUNK_PROCESSING	= 11,
	VO_DRM2DATATYPE_CHUNK_END			= 12,

	VO_DRM2DATATYPE_CENC_PSSH		= 40,
	VO_DRM2DATATYPE_CENC_TENC		= 41,

	VO_DRM2DATATYPE_RAWFILE			= 100,		// for entire file be encrypted

	VO_DRM2DATATYPEE_MAX			= VO_MAX_ENUM_VALUE
} VO_DRM2_DATATYPE;

typedef enum
{
	VO_DRM2_DATAINFO_UNKNOWN				= 0,
	VO_DRM2_DATAINFO_SAMPLEDECYPTION_BOX	= 0x1,	//Sample Encryption Box
	VO_DRM2_DATAINFO_SINGLE_SAMPLEDECYPTION	= 0x2,	//the related sub-sample encryption data

	VO_DRM2_DATAINFO_MAX						= VO_MAX_ENUM_VALUE
} VO_DRM2_DATAINFO_TYPE;

/**
 * Structure related to decrypt data.
 */
typedef struct
{
	VO_DRM2_DATATYPE	nDataType;	// type of DRM data
	VO_PTR				pInfo;		// this info is according to nDataType
	VO_U32				nReserved[6];
} VO_DRM2_DATAINFO;

/**
 * Structure used to pass encrypted data to decrypt.
 */
typedef struct
{
	VO_DRM2_DATAINFO	sDataInfo;		//<in> information needed by DecryptDataXXX
	VO_PBYTE			pData;			//<in> encrypted data buffer
	VO_U32				nSize;			//<in> encrypted data size
	VO_PBYTE*			ppDstData;		//<out> reserved for size-changed DRM
	VO_U32*				pnDstSize;		//<out> reserved for size-changed DRM
	VO_U32				nReserved[4];	//<in> for adaptive streaming, when reserved[0] is 1, it identify chunk data end; reserved[1] store offset from start of unit. reserved[2] store identifier of the chunk
										//<in> when VO_DRM2SRC_RAWFILE, reserved[1] is used to store the pointer to actual file position
} VO_DRM2_DATA;


/**
 * Structure used to transfer streaming play list for special customer
 */
typedef struct
{
	VO_BYTE*	pPlaylist;		//<in_out>convert original play list 
	VO_U32		uSizeBuffer;	//<in> the total size of pPlayList buffer
	VO_U32*		puSizePlaylist;	//<out> the size of converted play list.

	VO_PTR		pReserved;
} VO_DRM2_CONVERT_PLAYLIST;


/**
 * Structure used get the position of DRM header info
 */
typedef struct
{
	VO_CHAR*	pURI;			//file path or web link
	VO_U64*		pullPos;		//<out> the position of DRM header
	VO_U32*		puSize;			//<out> the size of DRM header

	VO_PTR		pReserved;
} VO_DRM2_INFO_GET_LOCATION;


/**
 * Structure used transfer data position for entire encrypted file
 */
typedef struct
{
	VO_U64	ullWantedPos;	//<in> the position from which want to read
	VO_U64*	pullActualPos;	//<out> the actual position corresponding to wanted position

	VO_U32	uWantedSize;	//<in> the size which want to read
	VO_U32*	puActualSize;	//<out> the actual size corresponding to wanted size

	VO_PTR	pReserved;
} VO_DRM2_DATA_CONVERT_LOCATION;


/**
 * Structure used to call back event to client 
 */
typedef struct
{
	VO_PTR pUserData; //callback instance
	/**
	* \param pUserData [in] user data.
	* \param nID [in] The status type. refer to VO_EVENTID_DRM2_BASE.
	* \param nParam1 [in] status specific parameter 1.
	* \param nParam2 [in] status specific parameter 2.
	*/
	VO_S32 (VO_API * callback) (VO_PTR pUserData, VO_U32 nID, VO_PTR nParam1, VO_U32 nParam2);
} VO_DRM2_CALLBACK;


/**
 * Structure of customization data
 */
typedef struct 
{
	VO_PTR	pUserData;	/*!<The userdata recved the VO_DRM2_CUSTOMIZATION_CALLBACK*/

	VO_PTR	pData;		/*!<The customization data*/
	VO_U32	uSize;		/*!<The size of customization*/

	VO_U32	uFlag;		/*!<The data flag, reserved*/

	VO_PTR	pReserved;
} VO_DRM2_CUSTOMIZATION_DATA;


/**
* VisualOn DRM engine function set
*/
typedef struct
{
	/**
	* Initialize the VisualOn DRM engine.
	* \param ppHandle [out] DRM engine handle.
	* \param pParam [IN] The DRM open param
	* \retval VO_ERR_DRM2_OK Succeeded.
	*/
	VO_U32 (VO_API * Init)(VO_PTR* ppHandle, VO_DRM_OPENPARAM *pParam);

	/**
	* Destroy the DRM engine
	* \param pHandle [in] The DRM engine handle, created by Init().
	* \retval VO_ERR_DRM2_OK Succeeded.
	*/
	VO_U32 (VO_API * Uninit)(VO_PTR pHandle);

	/**
	* Set customer's API set into engine
	* \param pParam [IN] Set the third-party DRM operator handle.
	* \retval VO_ERR_DRM2_OK Succeeded.
	*/
	VO_U32 (VO_API * SetThirdpartyAPI)(VO_PTR pHandle, VO_PTR pParam);

	/**
	* Get the param from engine
	* \param pParam [OUT] Return our internal DRM operator handle. refer to VO_SOURCEDRM_CALLBACK2
	* \retval VO_ERR_DRM2_OK Succeeded.
	*/
	VO_U32 (VO_API * GetInternalAPI)(VO_PTR pHandle, VO_PTR *ppParam);

	/**
	* Set the param for special target
	* \param nID [IN] The param ID.
	* \param pParam [IN] The param value depend on the ID>
	* \retval VO_ERR_DRM2_OK Succeeded.
	*/
	VO_U32 (VO_API * SetParameter)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	/**
	* Get the param for special target
	* \param nID [IN] The param ID.
	* \param pParam [OUT] The param value depend on the ID>
	* \retval VO_ERR_DRM2_OK Succeeded.
	*/
	VO_U32 (VO_API * GetParameter) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
} VO_DRM2_API;

/**
 * Get DRM engine API interface
 * \param pDRMHandle [out] Return the DRM engine API set.
 * \param uFlag [in] Reserved.
 * \retval VO_ERR_DRM2_OK Succeeded.
 */
VO_S32 VO_API voGetDRMAPI(VO_DRM2_API * pDRMHandle, VO_U32 uFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voDRM2_H__