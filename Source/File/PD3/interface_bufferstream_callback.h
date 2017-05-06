
#ifndef __INTERFACE_BUFFERSTREAM_CALLBACK_H__

#define __INTERFACE_BUFFERSTREAM_CALLBACK_H__

#include "voType.h"

enum BUFFERSTREAM_CALLBACK_RET
{
	BUFFERSTREAM_CALLBACK_OK,
	BUFFERSTREAM_CALLBACK_ERROR,
};


class interface_bufferstream_callback
{
public:
	virtual BUFFERSTREAM_CALLBACK_RET need_new_bufferstream( VO_S32 bufferstream_id , VO_S64 new_pos ) = 0;
	virtual BUFFERSTREAM_CALLBACK_RET set_filesize( VO_S64 filesize ) = 0;
};


#endif