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

#ifndef __OMX_VO_LibDrm_H__
#define __OMX_VO_LibDrm_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "voOMX_Types.h"

/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_DRM_BASE						0x43200000							/*!< the base param ID for DRM modules */
#define VO_PID_DRM_DRMVersion				(VO_PID_DRM_BASE | 0x0001)			/*!< <S> init from decoder for JIT, NULL*/

/**
* Drm Error code
*/
#define OMX_VO_ERR_DRM_OK					OMX_ErrorNone
#define OMX_VO_ERR_DRM_BASE					0x86600000
#define OMX_VO_ERR_DRM_MODULENOTFOUND		(OMX_VO_ERR_DRM_BASE | 0x0001)			/*!< can not find specific DRM module */
#define OMX_VO_ERR_DRM_BADPARAMETER			(OMX_VO_ERR_DRM_BASE | 0x0002)			/*!< parameter is not valid */
#define OMX_VO_ERR_DRM_ERRORSTATE			(OMX_VO_ERR_DRM_BASE | 0x0003)			/*!< error state */
#define OMX_VO_ERR_DRM_BADDRMINFO			(OMX_VO_ERR_DRM_BASE | 0x0004)			/*!< the DRM information in file can not be parsed correctly */
#define OMX_VO_ERR_DRM_BADDRMDATA			(OMX_VO_ERR_DRM_BASE | 0x0005)			/*!< the DRM encrypted data can not be decrypted correctly */
#define OMX_VO_ERR_DRM_USERCANCEL			(OMX_VO_ERR_DRM_BASE | 0x0006)			/*!< user cancel the operation */
#define OMX_VO_ERR_DRM_BADRESOLUTION		(OMX_VO_ERR_DRM_BASE | 0x0007)			/*!< user cancel the operation */
#define OMX_VO_ERR_DRM_UNDEFINED			(OMX_VO_ERR_DRM_BASE | 0xFFFF)			/*!< Undefined error */


/**
 * DRM type
 */
typedef enum
{
	OMX_VO_DRMTYPE_UNKNOWN				= 0,
	OMX_VO_DRMTYPE_DIVX					= 1,			/*!< DivX DRM */
	OMX_VO_DRMTYPE_WindowsMedia			= 2,			/*!< WM DRM */
	OMX_VO_DRMTYPE_PlayReady			= 3,			/*!< PlayReady */
	OMX_VO_DRMTYPE_Widevine				= 4,			/*!< Widevine */
	OMX_VO_DRMTYPE_MAX					= 0x7FFFFFFF
} OMX_VO_DRM_TYPE;

/**
* DRM data type
*/
typedef enum
{
	OMX_VO_DRMDATATYPE_UNKNOWN			= 0, 
	OMX_VO_DRMDATATYPE_MEDIASAMPLE		= 1, 
	OMX_VO_DRMDATATYPE_AUDIOSAMPLE		= 2, 
	OMX_VO_DRMDATATYPE_VIDEOSAMPLE		= 3, 
	OMX_VO_DRMDATATYPE_PACKETDATA		= 4, 
	OMX_VO_DRMDATATYPE_JITSAMPLE		= 5,
	OMX_VO_DRMDATATYPEE_MAX				= 0x7FFFFFFF
} OMX_VO_DRM_DATATYPE;

typedef struct
{
	OMX_VO_DRM_DATATYPE		nDataType;
	OMX_PTR					pInfo;
	OMX_U32					nReserved[6];
} OMX_VO_DRM_DATAINFO;


/**
* DRM callback
*/
#define OMX_VO_SOURCEDRM_FLAG_DRMINFO			0x1
#define OMX_VO_SOURCEDRM_FLAG_DRMDATA			0x2
#define OMX_VO_SOURCEDRM_FLAG_FLUSH				0x10
#define OMX_VO_SOURCEDRM_FLAG_ISSUPPORTED		0x20

typedef struct
{
	OMX_VO_DRM_TYPE			nType;			//DRM type
	OMX_PTR					pDrmInfo;		//DRM information in file, the format is decided by nType
	OMX_U32					nReserved[6];	//reserved for future
} OMX_VO_SOURCEDRM_INFO;

typedef struct
{
	OMX_VO_DRM_DATAINFO		sDataInfo;		//information needed by DecryptDataXXX
	OMX_U8 *				pData;			//encrypted data buffer
	OMX_U32					nSize;			//encrypted data size
	OMX_U8 **				ppDstData;		//reserved for size-changed DRM
	OMX_U32*				pnDstSize;		//reserved for size-changed DRM
	OMX_U32					nReserved[4];	//reserved for future
} OMX_VO_SOURCEDRM_DATA;

/**
* VisualOn file parser DRM callback.
* \param pUserData: user data set in.
* \param nFlag: OMX_VO_SOURCEDRM_FLAG_XXX.
* \param pParam: OMX_VO_SOURCEDRM_FLAG_DRMINFO - OMX_VO_SOURCEDRM_INFO*; OMX_VO_SOURCEDRM_FLAG_DRMDATA - OMX_VO_SOURCEDRM_DATA*; VO_SOURCEDRM_FLAG_ISSUPPORTED - OMX_VO_DRM_TYPE*.
* \param nReserved: reserved.
* \retval OMX_VO_ERR_NONE Succeeded, OMX_VO_ERR_DRM_XXX.
*/
typedef OMX_U32 (OMX_APIENTRY * OMXVOSOURCEDRMCALLBACK)(OMX_PTR pUserData, OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved);

typedef struct
{
	OMXVOSOURCEDRMCALLBACK	fCallback;		//callback function pointer
	OMX_PTR					pUserData;		//user data
} OMX_VO_SOURCEDRM_CALLBACK;

//The object buffer not include id(GUID)
typedef struct
{
	OMX_U8 *			pContentEncryptionObject;
	OMX_U32				nContentEncryptionObject;
	OMX_U8 *			pExtendedContentEncryptionObject;
	OMX_U32				nExtendedContentEncryptionObject;
} OMX_VO_WMDRM_INFO;

typedef struct
{
	OMX_U8 *			pStrdChunk;
	OMX_U32				nStrdChunk;
} OMX_VO_DIVXDRM_INFO;

typedef struct
{
	OMX_U8 *			pDrmHeader;
	OMX_U32				nDrmHeader;
} OMX_VO_PLAYREADY_INFO;

/** For Decrypt Content protected by PlayReady DRM */
typedef struct
{
	unsigned long long int qwInitializationVector;	// IV, this is changed by each payload or fragment
	unsigned long long int qwBlockOffset;			// Block offset within the payload or fragment
	unsigned char          ucByteOffset;			// Byte offset within the current block
} OMX_VO_PLAYREADY_AES_CTX;

/**
* Decryption APIs for PlayReady DRM( Partial Encrypted File )
*/
typedef struct
{
	void* (*Initialize) (
		OMX_IN    unsigned char  *pucDrmHeader,					// Base64 encoded PlayReady Header Data
		OMX_IN    unsigned int    uDrmHeaderLen);				// Length of pDrmHeader

	int (*Destory) (
		OMX_IN    void           *pHandle);						// Handle from Initialize

	int (*Get_License_Challenge) (
		OMX_IN    void           *pHandle,						// Handle from Initialize
		OMX_INOUT unsigned char  *pucChallengeBuffer,			// Challenge Data
		OMX_INOUT unsigned int   *puChallengeBufferLen,			// Length of pChallengeBuffer
		OMX_INOUT char           *pcUrlData,					// Server URL to process HTTP POST / HTTP GET
		OMX_INOUT unsigned int   *puUrlDataLen,					// Length of pUrlData
		OMX_INOUT char           *pcHTTPHeaderData,				// Additional HTTP header about "SOAP Action"
		OMX_INOUT unsigned int   *puHTTPHeaderDataLen);			// Length of pHTTPHeaderData

	int (*Set_License_Response) (
		OMX_IN    void          *pHandle,						// Handle from Initialize
		OMX_IN    char          *pcMimeType,					// Mime type of Response Data
		OMX_IN    unsigned char *pucResponse,					// Response Data from PlayReady Server
		OMX_IN    unsigned int   uResponseLen);					// Length of pResponse

	int (*Commit) (
		OMX_IN    unsigned char *pucDrmHeader,					// PlayReady Header Data
		OMX_IN    unsigned int   uDrmHeaderLen,					// Length of pDrmHeader
		OMX_INOUT void          *pvDecryptionInfo,				// Decryption Information
		OMX_INOUT unsigned int  *puDecryptionInfoLen);			// Length of pDecryptionInfo

	int (*Init_Decrypt) (
		OMX_IN    void          *pvDecryptionInfo,				// Decryption Information
		OMX_IN    unsigned char *pucLast15byte,					// (only WMDRM) Last 15 byte of payload
		OMX_IN    unsigned int   uLast15byteLen);				// (only WMDRM) Length of pLast15byte

	int (*Decrypt) (
		OMX_IN    void                     *pvDecryptionInfo,	// Decryption Information
		OMX_IN    OMX_VO_PLAYREADY_AES_CTX *pstAESCtx,			// IV, BlockOffset, ByteOffset
		OMX_INOUT unsigned char            *pucBuffer,			// Encrypted Data
		OMX_IN    unsigned int              uBufferLen);		// Length of pBuffer

} OMX_VO_DRM_PLAYREADY_API;

typedef enum
{
	OMX_VO_DRM_FILEBASED_NONE,			// Not DRM Contents
	OMX_VO_DRM_FILEBASED_OMA,			// OMA DRM Contents
//	OMX_VO_DRM_FILEBASED_DIVX,			// DivX DRM Contents
//	OMX_VO_DRM_FILEBASED_ANDROID, 		// Android(Widevine) DRM Contents
//	OMX_VO_DRM_FILEBASED_PLAYREADY,		// Playready DRM Contents
	OMX_VO_DRM_FILEBASED_SKT = 5, 		// SKT DRM Contents
	OMX_VO_DRM_FILEBASED_LGU,			// LGU DRM Contents
	OMX_VO_DRM_FILEBASED_UNKNOWN
} OMX_VO_DRM_FILEBASED_TYPE;

typedef struct
{
	void	*Handle;						// each DRM's file IO handle
	int		LogicalFileOffset;				// Current file offset of DRM file IO ( is not equal to the DRM file's real file offset )
	int		LogicalFileSize;				// Size of original content which is protected by each DRM. ( is not equal to the DRM file's size ). ADF DRM does not support this feature.
	char	CID[256 + 1];					// Only for LGT UPLUS DRM
//	FILE	*InternalFile;					// Only for LGT UPLUS DRM
	void	*InternalFile;					// Only for LGT UPLUS DRM
} OMX_VO_DRM_FILEBASED_IO;

typedef enum {
	/* Start of file position */
	OMX_VO_DRM_FILEBASED_SEEK_SET = 0,
	/* Current file position */
	OMX_VO_DRM_FILEBASED_SEEK_CUR,
	/* End of file position */
	OMX_VO_DRM_FILEBASED_SEEK_END
} OMX_VO_DRM_FILEBASED_SEEKMODE;

class IVOFileBasedDRM 
{
public:
	IVOFileBasedDRM();
	virtual ~IVOFileBasedDRM();

	virtual int Open(OMX_INOUT OMX_VO_DRM_FILEBASED_IO *pVODrmIo, OMX_IN const int fd)  = 0;
	virtual int Open(OMX_INOUT OMX_VO_DRM_FILEBASED_IO *pVODrmIo, OMX_IN const char *file) = 0;
	virtual int Read(OMX_INOUT OMX_VO_DRM_FILEBASED_IO *pVODrmIo, OMX_OUT void * pBuffer, OMX_IN int uSize) = 0;
	virtual int ReadAt(OMX_INOUT OMX_VO_DRM_FILEBASED_IO *pVODrmIo, OMX_IN int nPos, OMX_OUT void *pBuffer, OMX_IN int uSize) = 0;
	virtual int Seek(OMX_INOUT OMX_VO_DRM_FILEBASED_IO *pVODrmIo, OMX_IN int nPos, OMX_IN OMX_VO_DRM_FILEBASED_SEEKMODE uFlag) = 0;
	virtual int Size(OMX_INOUT OMX_VO_DRM_FILEBASED_IO *pVODrmIo) = 0;
	virtual int Tell(OMX_INOUT OMX_VO_DRM_FILEBASED_IO *pVODrmIo) = 0;
	virtual int Close(OMX_INOUT OMX_VO_DRM_FILEBASED_IO *pVODrmIo) = 0;
	virtual int getDRMType() const = 0;
};

class VOWidewineDrmBuffer
{
public:
	OMX_U8*		data;
	OMX_U32		length;

	VOWidewineDrmBuffer()
		: data(0)
		, length(0)
	{
	}

	VOWidewineDrmBuffer(OMX_U8* dataBytes, OMX_U32 dataLength)
		: data(dataBytes)
		, length(dataLength)
	{
	}
};

class IVOWidevineDRM
{
public:
	IVOWidevineDRM();
	virtual ~IVOWidevineDRM();

	/**
	* Open the decrypt session to decrypt the given protected content
	*
	* @param[in] uri Path of the protected content to be decrypted
	* @return
	*     Returns Widevine_DRM_NO_ERROR for success, Widevine_DRM_ERROR_UNKNOWN for failure 
	*/
	virtual int openDecryptSession(OMX_IN const char* url) = 0;

	/**
	* Open the decrypt session to decrypt the given protected content
	* @param[in] fd File descriptor of the protected content to be decrypted
	* @param[in] offset Start position of the content
	* @param[in] length The length of the protected content
	* @return
	*    Returns Widevine_DRM_NO_ERROR for success, Widevine_DRM_ERROR_UNKNOWN for failure 
	*/
	virtual int openDecryptSession(OMX_IN int fd, OMX_IN long long offset, OMX_IN long long length) = 0;

	/**
	* Close the decrypt session for the given handle
	*
	* @return status_t
	*     Returns Widevine_DRM_NO_ERROR for success, Widevine_DRM_ERROR_UNKNOWN for failure
	*/
	virtual int closeDecryptSession() = 0;

	/**
	* Initialize decryption for the given unit of the protected content
	* @param[in] decryptUnitId ID which specifies decryption unit, such as track ID
	* @param[in] headerInfo Information for initializing decryption of this decrypUnit
	* @return status_t
	*     Returns Widevine_DRM_NO_ERROR for success, Widevine_DRM_ERROR_UNKNOWN for failure
	*/
	virtual int initializeDecryptUnit(OMX_IN int decryptUnitId, OMX_IN const VOWidewineDrmBuffer* headerInfo) = 0;

	/**
	* Decrypt the protected content buffers for the given unit
	* This method will be called any number of times, based on number of
	* encrypted streams received from application.
	*
	* @param[in] decryptUnitId ID which specifies decryption unit, such as track ID
	* @param[in] encBuffer Encrypted data block
	* @param[out] decBuffer Decrypted data block
	* @param[in] IV Optional buffer
	* @return status_t
	*     Returns the error code for this API
	*     Widevine_DRM_NO_ERROR for success, and one of Widevine_DRM_ERROR_UNKNOWN, Widevine_DRM_ERROR_LICENSE_EXPIRED
	*     Widevine_DRM_ERROR_SESSION_NOT_OPENED, Widevine_DRM_ERROR_DECRYPT_UNIT_NOT_INITIALIZED,
	*     Widevine_DRM_ERROR_DECRYPT for failure.
	*/
	virtual int decrypt(OMX_IN int decryptUnitId, OMX_IN const VOWidewineDrmBuffer* encBuffer, OMX_OUT VOWidewineDrmBuffer** decBuffer, OMX_IN VOWidewineDrmBuffer* IV = 0) = 0;

	/**
	* Finalize decryption for the given unit of the protected content
	*
	* @param[in] decryptUnitId ID which specifies decryption unit, such as track ID
	* @return status_t
	*         Returns Widevine_DRM_NO_ERROR for success, Widevine_DRM_ERROR_UNKNOWN for failure
	*/
	virtual int finalizeDecryptUnit(OMX_IN int decryptUnitId) = 0;
};

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __OMX_VO_LibDrm_H__
