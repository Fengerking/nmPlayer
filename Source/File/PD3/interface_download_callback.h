#ifndef _INTERFACE_DOWNLOAD_CALLBACK__

#define _INTERFACE_DOWNLOAD_CALLBACK__

#include "voType.h"

enum CALLBACK_RET
{
	CALLBACK_OK,
	CALLBACK_BUFFER_FULL,
	CALLBACK_BUFFER_WRITEFAIL,
};

enum DOWNLOAD_CALLBACK_NOTIFY_ID
{
	DOWNLOAD_START,
	DOWNLOAD_END,						
	DOWNLOAD_DEBUG_STRING,
	DOWNLOAD_FILESIZE,					//VO_PTR will be vo_s64* type
	DOWNLOAD_TO_FILEEND,				//VO_PTR will be vo_s64* type shows the final filesize
	DOWNLOAD_ERROR,
};

class interface_download_callback
{
public:
	virtual CALLBACK_RET received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size ) = 0;
	virtual CALLBACK_RET download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data ) = 0;
};


#endif