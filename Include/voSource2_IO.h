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

#ifndef __VOSOURCE2_IO_H__

#define __VOSOURCE2_IO_H__

#include "voType.h"

#define VO_SOURCE2_IO_OK						0x00000000
#define VO_SOURCE2_IO_EOS						0x00000001
#define VO_SOURCE2_IO_RETRY						0x00000002
#define VO_SOURCE2_IO_FAIL						0x84700000
#define VO_SOURCE2_IO_PROTOCOL_NOTSUPPORT		( VO_SOURCE2_IO_FAIL | 0x1 )
#define VO_SOURCE2_IO_NULLPOINTOR				( VO_SOURCE2_IO_FAIL | 0x2 )
#define VO_SOURCE2_IO_NOTIMPLEMENT				( VO_SOURCE2_IO_FAIL | 0x3 )

#define VO_SOURCE2_IO_FLAG_OPEN_URL				0X00000001	/*!<Indicate the pSource param is a url*/
#define VO_SOURCE2_IO_FLAG_OPEN_LOCAL_FILE		0X00000002	/*!<Indicate the pSource param is a local file*/
#define VO_SOURCE2_IO_FLAG_OPEN_DRM				0X00000004	/*!<Indicate the IO is DRM mode*/ 
#define VO_SOURCE2_IO_FLAG_OPEN_ASYNC			0X10000000	/*!<Indicate the IO is async mode*/
#define VO_SOURCE2_IO_FLAG_OPEN_SYNC			0X20000000	/*!<Indicate the IO is sync mode*/

#define VO_SOURCE2_IO_PARAMID_BASE						0X10000000
#define VO_SOURCE2_IO_PARAMID_DRMPOINTOR				( VO_SOURCE2_IO_PARAMID_BASE | 0x1 )		/*!< <S> set DRM pointor */

#define VO_SOURCE2_IO_PARAMID_HTTPBASE					( VO_SOURCE2_IO_PARAMID_BASE | 0X01000000 )
#define VO_SOURCE2_IO_PARAMID_HTTPVERIFYCALLBACK		( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0X1 )	/*!< <S> set for the request, response and data callback IO_HTTP_VERIFYCALLBACK_FUNC*  */
#define VO_SOURCE2_IO_PARAMID_HTTPDOWNLOADTIME			( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0X2 )	/*!< <G> get the download time  VO_U64* */
#define VO_SOURCE2_IO_PARAMID_HTTPRANGE					( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0x3 )	/*!< <S> set range info for the io VO_SOURCE2_IO_HTTPRANGE* */
#define VO_SOURCE2_IO_PARAMID_HTTPREDIRECTURL			( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0x4 )	/*!< <G> get the redirect url,  VO_SOURCE2_IO_HTTP_REDIRECT_URL* */
#define VO_SOURCE2_IO_PARAMID_HTTPMAXSPEED				( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0x5 )	/*!< <S> set the max download speed of http i/o, VO_S32* */
#define VO_SOURCE2_IO_PARAMID_DOHTTPVERIFICATION		( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0x6 )	/*!< <S> set the verification info of http i/o VO_SOURCE2_IO_VERIFICATIONINFO * */
#define VO_SOURCE2_IO_PARAMID_HTTPHEADINFO				( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0x7 )	/*!< <S> set the http header info  */
#define VO_SOURCE2_IO_PARAMID_HTTPIOCALLBACK			( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0X8 )	/*!< <S> set for the IO callback VO_SOURCE2_IO_HTTPCALLBACK*  */
#define VO_SOURCE2_IO_PARAMID_DESTROY					( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0X9 )	/*!< <S> tell SourceIO module that it will be destroyed soon ,pParam should be NULL */
#define VO_SOURCE2_IO_PARAMID_HTTPPROXYINFO				( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0xa )	/*!< <S> set the proxy server info  */
#define VO_SOURCE2_IO_PARAMID_HTTPTIMERANGE				( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0xb )    /*!< <S> set range info for the io VO_SOURCE2_IO_HTTPTIMERANGE* */
#define VO_SOURCE2_IO_PARAMID_HTTPRESPONSE				( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0xc )	/*!< <G> get the response string,  VO_SOURCE2_IO_PARAMID_HTTPRESPONSE* */
#define VO_SOURCE2_IO_PARAMID_PAUSE_RESUME_REV_DATA		( VO_SOURCE2_IO_PARAMID_HTTPBASE | 0Xd )	/*!< <G> pause or resume receive data from socket,  0:Pause receiving 1:resume receiving * */
#define VO_SOURCE2_CALLBACKIDBASE_SOURCE		0x10000000
#define VO_SOURCE2_CALLBACKIDBASE_DRM			0x20000000

#define VO_SOURCE2_IO_HTTP_LAST_ERRORNONE				0x12000000			/*!<http IO has no error*/
#define VO_SOURCE2_IO_HTTP_CONNECTION_ERROR				( 0x12000000 | 0x1 )/*!<http IO has connection error*/
#define VO_SOURCE2_IO_HTTP_SERVER_SIDE_ERROR			( 0x12000000 | 0x2 )/*!<the server response tell error status 5xx*/
#define VO_SOURCE2_IO_HTTP_RESPONSE_TOOLARGE			( 0x12000000 | 0x3 )/*!<the response of server is too large*/
#define VO_SOURCE2_IO_HTTP_GET_RESPONSE_TIMEOUT			( 0x12000000 | 0x4 )/*!<when get the reponse , it is timeout*/
#define VO_SOURCE2_IO_HTTP_AUTHENTICATION_FAIL			( 0x12000000 | 0x5 )/*!<when do authorization, it is failed*/
#define VO_SOURCE2_IO_HTTP_CLIENT_SIDE_ERROR			( 0x12000000 | 0x6 )/*!<the server response tell error status 4xx*/
#define VO_SOURCE2_IO_HTTP_REDIRECTION_ERROR			( 0x12000000 | 0x7 )/*!<the server response tell need redirection and with status 3xx*/

enum VO_SOURCE2_IO_HTTP_STATUS
{
	VO_SOURCE2_IO_HTTP_BEGIN = VO_SOURCE2_CALLBACKIDBASE_SOURCE,
	VO_SOURCE2_IO_HTTP_SOCKETCONNECTED,
	VO_SOURCE2_IO_HTTP_REQUESTPREPARED,
	VO_SOURCE2_IO_HTTP_REQUESTSEND,
	VO_SOURCE2_IO_HTTP_RESPONSERECVED,
	VO_SOURCE2_IO_HTTP_RESPONSEANALYSED,
	VO_SOURCE2_IO_HTTP_MAX = 0x7FFFFFFF,
};

enum VO_SOURCE2_IO_HTTP_DOWNLOADSTATUSINFO
{
	VO_SOURCE2_IO_HTTP_STARTDOWNLOAD = VO_SOURCE2_CALLBACKIDBASE_SOURCE + 0x100,	/*!<http IO has start download, pParam1 will be the related url(VO_CHAR*), pParam2 will be NULL */
	VO_SOURCE2_IO_HTTP_DOWNLOADFAILED,												/*!<http IO failed to download, pParam1 will be the related url(VO_CHAR*) ,pParam2 will be the failed reason VO_SOURCE2_IO_FAILEDREASON_DESCRIPTION* */
	VO_SOURCE2_IO_HTTP_DOWNLOADSTATUSMAX = 0x7FFFFFFF,
};

enum VO_SOURCE2_IO_HTTP_DOWNLOADFAIL_REASON
{
	VO_SOURCE2_IO_HTTP_CONNECT_FAILED,												/*!<http IO connect failed*/							
	VO_SOURCE2_IO_HTTP_INVALID_RESPONSE,											/*!<http IO failed to get response or response can't be parsed or response too large*/
	VO_SOURCE2_IO_HTTP_CLIENT_ERROR,												/*!<http IO 4xx error*/
	VO_SOURCE2_IO_HTTP_SERVER_ERROR,												/*!<http IO 5xx error*/
};

typedef struct 
{
	VO_SOURCE2_IO_HTTP_DOWNLOADFAIL_REASON  reason; /*!<failed reason*/
	VO_CHAR * szResponse;			/*!<when the reason is VO_SOURCE2_IO_HTTP_CLIENT_ERROR or VO_SOURCE2_IO_HTTP_SERVER_ERROR,it contain The response string, else it will be NULL*/
	VO_U32 uResponseSize;			/*!<when the reason is VO_SOURCE2_IO_HTTP_CLIENT_ERROR or VO_SOURCE2_IO_HTTP_SERVER_ERROR,it contain The response string size, else it will be 0*/
}VO_SOURCE2_IO_FAILEDREASON_DESCRIPTION;

/**
 * callback function for async open , read or write
 * \param pUserData [in] the pUserData user set in IO_ASYNCE_CALLBACK when do read and write
 * \param uID [in] the id for identify the callback( read, write, open compelete .etc )
 * \param uError [in] indicate the error code for this io operation
 * \param pBuf [in]. If it is read mode, it contains the buffer that readed from the IO, for write mode, it is not used
 * \param uSizeDone[in] it shows that how much data has been read or write
 */
typedef VO_U32 (VO_API * IO_ASYNC_CALLBACK_FUNC) ( VO_PTR pUserData , VO_U32 uID , VO_U32 uError , VO_PBYTE pBuf , VO_U32 uSizeDone );

struct VO_SOURCE2_IO_ASYNC_CALLBACK
{
	VO_PTR pUserData;
	IO_ASYNC_CALLBACK_FUNC Async_CallBack;
};

#define VO_SOURCE2_IO_TYPE_HTTP		0X11000000
#define VO_SOURCE2_IO_TYPE_LOCAL	0X12000000

typedef enum
{
	VO_SOURCE2_IO_SUBTYPE_HTTPREQUEST = VO_SOURCE2_IO_TYPE_HTTP | 0X1,
	VO_SOURCE2_IO_SUBTYPE_HTTPRESPONSE,
	VO_SOURCE2_IO_SUBTYPE_HTTPDATA,

	VO_SOURCE2_IO_SUBTYPE_HTTPMAX				= VO_MAX_ENUM_VALUE
} VO_SOURCE2_IO_SUBTYPE;

/**
 * callback function for verification
 * \param hHandle [in] the hHandle member in VO_SOURCE2_IO_HTTP_VERIFYCALLBACK structure
 * \param uID [in] should use the value defined in VO_SOURCE2_IO_HTTP_STATUS
 * \param pData [in] the data need to send out
 */
typedef VO_U32 (VO_API * IO_HTTP_VERIFYCALLBACK_FUNC)( VO_PTR hHandle , VO_U32 uID , VO_PTR pUserData );

struct VO_SOURCE2_IO_HTTP_VERIFYCALLBACK 
{
	VO_PTR hHandle;
	IO_HTTP_VERIFYCALLBACK_FUNC HTTP_Callback;
};

/**
 * callback function for IO
 * \param hHandle [in] the hHandle member in VO_SOURCE2_IO_HTTPCALLBACK structure
 * \param uID [in] should use the value defined in VO_SOURCE2_IO_HTTP_DOWNLOADSTATUSINFO
 * \param pParam1 [in] will be dependent on the uID
 * \param pParam2 [in] will be dependent on the uID
 */
typedef VO_U32 (VO_API * IO_HTTP_CALLBACK_FUNC)( VO_PTR hHandle , VO_U32 uID , VO_PTR pParam1 , VO_PTR pParam2  );

struct VO_SOURCE2_IO_HTTPCALLBACK 
{
	VO_PTR hHandle;
	IO_HTTP_CALLBACK_FUNC IO_Callback;
};

typedef enum
{
	VO_SOURCE2_IO_POS_BEGIN						= 0X00000000,	/*!< from begin position of file*/
	VO_SOURCE2_IO_POS_CURRENT					= 0X00000001,	/*!< from current position of file*/
	VO_SOURCE2_IO_POS_END						= 0X00000002,	/*!< form end position of file*/

	VO_SOURCE2_IO_POS_MAX						= VO_MAX_ENUM_VALUE
}VO_SOURCE2_IO_POS;

struct VO_SOURCE2_IO_HTTPRANGE
{
	VO_U64 ullOffset;
	VO_U64 ullLength;
};

typedef struct 
{
	VO_U32	hours;
	VO_U32  minutes;
	float	seconds;
}VO_SOURCE2_IO_NPTIME;

typedef struct 
{
	VO_SOURCE2_IO_NPTIME startime;
	VO_SOURCE2_IO_NPTIME endtime;
}VO_SOURCE2_IO_HTTPTIMERANGE;

typedef struct 
{
	VO_PTR pUserData;				/*!<The userdata recved the VO_SOURCE2_IO_HTTP_VERIFYCALLBACK*/
	VO_PTR pData;					/*!<The data that need to do verification*/
	VO_U32 uDataSize;				/*!<The data size*/
	VO_U32 uDataFlag;				/*!<The data flag, if it is 0, it is the request string, other value to not support currently */
	VO_CHAR * szResponse;			/*!<The response string*/
	VO_U32 uResponseSize;			/*!<The response string size*/
}VO_SOURCE2_IO_VERIFICATIONINFO;

struct VO_SOURCE2_IO_HTTP_REDIRECT_URL
{
	VO_CHAR * ptr_url;			/*!< original url */							
	VO_U32	orig_size;			/*!< original url length */
	VO_CHAR * ptr_redirect_url;	/*!< redirect url buffer */
	VO_U32  redirect_size;		/*!< redirect url buffer size */
};

typedef struct
{
	VO_PTR  pIOHandle;
	VO_PTR  pDrmEng;
}voSourceIOHnd;

struct VO_SOURCE2_IO_API
{
	VO_HANDLE hHandle; //the handle of the io instance, for multi-instance please do not use it
/**
 * Initial an io instance
 * This function should be called first, and this function should do nothing but store the source file description and create the io instance
 * \param pHandle [out] io handle.
 * \param pSource [in] source file description, should be an url or fd etc.
 * \param uFlag [in]. It can indicate the source type
 */
	VO_U32 ( VO_API *Init)( VO_HANDLE * pHandle , VO_PTR pSource , VO_U32 uFlag , VO_SOURCE2_IO_ASYNC_CALLBACK * pAsyncCallback );

/**
 * Uninit an io instance
 * \param hHandle [in] io handle
 */
	VO_U32 ( VO_API *UnInit)( VO_HANDLE hHandle );

/**
 * Open the io
 * \param hHandle [in] io handle
 */
	VO_U32 ( VO_API *Open)( VO_HANDLE hHandle , VO_BOOL bIsAsyncOpen );

/**
 * Close the io
 * \param hHandle [in] io handle
 */
	VO_U32 ( VO_API *Close)( VO_HANDLE hHandle );

/**
 * Read data from the io
 * \param hHandle [in] io handle
 * \param pBuf [out] the buffer to get the data
 * \param uSize [in] the size that want to read
 * \param pReadSize [out] the size that read from this io operation
 * \param pAsyncCallBack[in] When uFlag param contain VO_SOURCE2_IO_FLAG_OPEN_ASYNC flag, you must set this callback for Async callback
 */
	VO_U32 ( VO_API *Read)( VO_HANDLE hHandle , VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pReadSize );

/**
 * Write data to the io
 * \param hHandle [in] io handle
 * \param pBuf [in] the buffer to write to the io
 * \param uSize [in] the size that want to write
 * \param pWrittenSize [out] the size that write to the io during this io operation
 * \param pAsyncCallBack[in] When uFlag param contain VO_SOURCE2_IO_FLAG_OPEN_ASYNC flag, you must set this callback for Async callback
 */
	VO_U32 ( VO_API *Write)( VO_HANDLE hHandle , VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pWrittenSize );

/**
 * Do seek for the io
 * \param hHandle [in] io handle
 * \param ullPos [in] the pos that from the RelativePos
 * \param RelativePos [in] indicate the seek start pos
 */
	VO_U32 ( VO_API *SetPos)( VO_HANDLE hHandle , VO_S64 llPos , VO_SOURCE2_IO_POS RelativePos , VO_S64 *llActualPos);

/**
 * Write the data immediately to the file, this is only used in write mode
 * \param hHandle [in] io handle
 */
	VO_U32 ( VO_API *Flush)( VO_HANDLE hHandle );

/**
 * Get the io opened content size
 * \param hHandle [in] io handle
 * \param pSize [out] content size
 */
	VO_U32 ( VO_API *GetSize)( VO_HANDLE hHandle , VO_U64 * pSize );

/**
 * Get the io fail reason
 * \param hHandle [in] io handle
 * \return is the error code
 */
	VO_U32 ( VO_API *GetLastError )( VO_HANDLE hHanle );

/**
 * Get param from this io instance
 * \param hHandle [in] io handle
 * \param uParamID [in] The param ID
 * \param pParam [out] The get value depend on the param ID.
 */
	VO_U32 ( VO_API *GetParam)( VO_HANDLE hHandle , VO_U32 uParamID , VO_PTR pParam );

/**
 * Set param for this io instance
 * \param hHandle [in] io handle
 * \param uParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
	VO_U32 ( VO_API *SetParam)( VO_HANDLE hHandle , VO_U32 uParamID , VO_PTR pParam );
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

VO_VOID VO_API yyGetDataIOFunc( VO_SOURCE2_IO_API * ptr_api );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	//__VOSOURCE2_IO_H__