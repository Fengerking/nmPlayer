/** 
* \file netdownbase.h
* \brief Common constants, types, strucs of VisualOn net download SDK.
* \version 0.1
* \date 11/07/2007 created
* \author VisualOn
*/

#ifndef __VO_NET_DOWNLOAD_H_
#define __VO_NET_DOWNLOAD_H_

#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus

#ifdef WINDOWS
#define VONDAPI	__cdecl
#else//WINDOWS
#define VONDAPI	
#endif//WINDOWS

typedef void* HVONETDOWN;

/**
* net download return code
*/
typedef enum
{
	VORC_NETDOWN_OK						= 0,			/*!< Successful */
	VORC_NETDOWN_NULL_POINTER			= 0x0001,		/*!< Null Pointer */
	VORC_NETDOWN_NOT_IMPLEMENT			= 0x0002,		/*!< this function is not implemented */
	VORC_NETDOWN_CONNECT_NET_FAIL		= 0x0004,		/*!< connect network fail */
	VORC_NETDOWN_LOCAL_FILE_CREATE_FAIL	= 0x0008,		/*!< create local file fail */
	VORC_NETDOWN_NEED_RETRY				= 0x0010,		/*!< error, but if retry, you maybe success */
	VORC_NETDOWN_OUTOF_MEMORY			= 0x0020,		/*!< run out of memory */
	VORC_NETDOWN_NETWORK_TIMEOUT		= 0x0040,		/*!< network timeout */
	VORC_NETDOWN_NETWORK_ERROR          = 0x0080,       /*!< bad network */
	VORC_NETDOWN_INVALID_URL            = 0x0100,       /*!< invalid url */
	VORC_NETDOWN_COMMUNICATION_FAIL     = 0x0200,       /*!< communicate by error */
	VORC_NETDOWN_USER_ABORT				= 0x0400,		/*!< user abort */
	VORC_NETDOWN_UNKNOWN_ERROR			= 0xFFFF,		/*!< unknown error */
} VONETDOWNRETURNCODE;


typedef enum
{
	VOSTATE_NETDOWN_NONE = 0,			/*!< download is not start */
	VOSTATE_NETDOWN_DOWNLOADING,		/*!< downloading now */
	VOSTATE_NETDOWN_FINISH,				/*!< download have been done successfully */
	VOSTATE_NETDOWN_ERROR_BREAK,		/*!< download is error end */
} VONETDOWNSTATE;

typedef struct tagNDBuffer 
{
	unsigned char*	buffer;
	unsigned int	size;
} NDBuffer, *PNDBuffer;

typedef enum
{
	NETDOWN_COMPLETED = 0,				/*!< download completed */
	NETDOWN_DISCONNECTED,				/*!< network disconnected */
	NETDOWN_FILE_SINK,					/*!< write file, param is NDBuffer* */
	NETDOWN_CREATE_FILE,				/*!< write file, param is file size, DWORD */		
} VONETDOWNEVENT;


/**
* net download parameter ID
*/
#define VOID_NETDOWN_BASE	0x53000000
typedef enum
{
	VOID_NETDOWN_CONNECT_TIMEOUT		= VOID_NETDOWN_BASE | 0x0001,  /*!< set connect timeout <MS> */
	VOID_NETDOWN_SEND_TIMEOUT		    = VOID_NETDOWN_BASE | 0x0002,  /*!< set send timeout <MS> */
	VOID_NETDOWN_RECV_TIMEOUT	    	= VOID_NETDOWN_BASE | 0x0003,  /*!< set receive timeout <MS> */
	VOID_NETDOWN_STATE					= VOID_NETDOWN_BASE | 0x0004,  /*!< get download state */
	VOID_NETDOWN_EVENT_NOTIFY           = VOID_NETDOWN_BASE | 0x0005,  /*!< set event notify function, NETDOWNCALLBACK */
	VOID_NETDOWN_FILE_SIZE				= VOID_NETDOWN_BASE | 0x0006,  /*!< get file size after download */
	VOID_NETDOWN_DOWN_SIZE				= VOID_NETDOWN_BASE | 0x0007,  /*!< get current download size */
	VOID_NETDOWN_START_POS				= VOID_NETDOWN_BASE | 0x0008,  /*!< set download start position, DWORD */
	VOID_NETDOWN_END_POS				= VOID_NETDOWN_BASE | 0x0009,  /*!< set download end position, DWORD */
	VOID_NETDOWN_SUPPORT_SEEK			= VOID_NETDOWN_BASE | 0x000A,  /*!< whether http server support seek, must be called after first voNetDownStart, bool* */
	VOID_NETDOWN_EN_USER_DATA			= VOID_NETDOWN_BASE | 0x000B,  /*!< set event notify user data */
	VOID_NETDOWN_USER_AGENT				= VOID_NETDOWN_BASE | 0x000C,  /*!< set user agent, char* */
	VOID_NETDOWN_DUMP_LOG_FUNC			= VOID_NETDOWN_BASE | 0x000D,  /*!< set dump log function pointer, DUMPLOGFUNC */
	VOID_NETDOWN_BYTES_PER_SEC			= VOID_NETDOWN_BASE | 0x000E,  /*!< get download throughput, DWORD */
	VOID_NETDOWN_RECV_BYTES_ONCE		= VOID_NETDOWN_BASE | 0x000F,  /*!< set receive bytes once, DWORD */
	VOID_NETDOWN_REDIRECT_URL			= VOID_NETDOWN_BASE | 0x0010,  /*!< get redirect url, TCHAR* */
	VOID_NETDOWN_THREAD_PRIORITY		= VOID_NETDOWN_BASE | 0x0011,  /*!< set receive thread priority, THREAD_PRIORITY_XXX */
	VOID_NETDOWN_HTTP_PROTOCOL			= VOID_NETDOWN_BASE | 0x0012,  /*!< set http protocol, 0: http 1.0, 1:http 1.1 */
	VOID_NETDOWN_DOWNLOAD_TYPE			= VOID_NETDOWN_BASE | 0x0013,  /*!< get the streaming type:normal pd,shout cast,wma live */
	VOID_NETDOWN_CONTENT_TYPE			= VOID_NETDOWN_BASE | 0x0014,  /*!< get the content-type string */


} VONETDOWNPARAMETERID;

typedef enum
{
	VO_NETDOWN_TYPE_NORMAL				= 0, 
	VO_NETDOWN_TYPE_SHOUT_CAST			= 1, 
	VO_NETDOWN_TYPE_WMS					= 2, 
	VO_NETDOWN_TYPE_FLV					= 3, 
} VONETDOWNTYPE;
typedef struct  
{
	VONETDOWNTYPE downType;
	int			  fileFormat;
}VONETDOWNFORMAT;
#define MAX_URL			1040		//MAX_PATH * 4

/**
* callback function
*/
typedef bool (VONDAPI* NETDOWNCALLBACK)(long lEventID, long lParam, long lUserData);
typedef void (VONDAPI* DUMPLOGFUNC)(LPCSTR);

VONETDOWNRETURNCODE VONDAPI voNetDownInit();
VONETDOWNRETURNCODE VONDAPI voNetDownCleanup();
VONETDOWNRETURNCODE VONDAPI voNetDownOpen(HVONETDOWN* ppNetDown, const char* szUrl, const char* szProxyName, VONETDOWNTYPE type);
VONETDOWNRETURNCODE VONDAPI voNetDownStart(HVONETDOWN pNetDown, bool* pbCancel);
VONETDOWNRETURNCODE VONDAPI voNetDownStop(HVONETDOWN pNetDown);
VONETDOWNRETURNCODE VONDAPI voNetDownClose(HVONETDOWN pNetDown);
VONETDOWNRETURNCODE VONDAPI voNetDownSetParameter(HVONETDOWN pNetDown, long lID, long lValue);
VONETDOWNRETURNCODE VONDAPI voNetDownGetParameter(HVONETDOWN pNetDown, long lID, long* plValue);


#ifdef __cplusplus
}	//extern "C"
#endif	//__cplusplus

#endif	//__VO_NET_DOWNLOAD_H_