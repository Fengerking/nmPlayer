#ifndef _INTERFACE_DOWNLOAD_CALLBACK__

#define _INTERFACE_DOWNLOAD_CALLBACK__

#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum CALLBACK_RET
{
	CALLBACK_OK,
	CALLBACK_BUFFER_FULL,
};

enum DOWNLOAD_CALLBACK_NOTIFY_ID
{
	DOWNLOAD_START,
	DOWNLOAD_END,						
	DOWNLOAD_FILESIZE,					//VO_PTR will be vo_s64* type
	DOWNLOAD_TO_FILEEND,				//VO_PTR will be vo_s64* type shows the final filesize
	DOWNLOAD_ERROR,
	DOWNLOAD_SERVER_RESPONSEERROR,
	DOWNLOAD_NEED_PROXY,
	DOWNLOAD_USE_PROXY,
	DOWNLOAD_LIVESRC_TYPE,				//VO_PTR will be VO_LIVESRC_FORMATTYPE type. To notify streaming server protocol
	DOWNLOAD_SERVER_RESPONSE,			//VO_PTR will be VO_CHAR* type tell the response string of server
	DOWNLOAD_SHOUTCAST_METADATA,		//VO_PTR will be VO_CHAR* type tell the metadata of shoutcast
};

class interface_download_callback
{
public:
	virtual CALLBACK_RET received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size ) = 0;
	virtual CALLBACK_RET download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data ) = 0;
};

#ifdef _VONAMESPACE
}
#endif

#endif