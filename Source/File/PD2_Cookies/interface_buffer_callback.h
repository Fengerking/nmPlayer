
#ifndef _INTERFACE_BUFFER_CALLBACK__H

#define _INTERFACE_BUFFER_CALLBACK__H

#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum BUFFER_CALLBACK_NOTIFY_ID
{
	BUFFER_FULL,
	BUFFER_LOW,						//pDATA will be VO_BUFFER_NEED* type
	BUFFER_GET_CACHE,				//pDATA will be VO_BUFFER_NEED* type
	BUFFER_LACK,					//pDATA will be VO_BUFFER_NEED* type
	BUFFER_HEADER_CONTROL,			//just for control headerdata buffer consume speed 
	BUFFER_HIGH						//in dlna case, if memory had been consumed a lot ,we should limit the buffer consume speed
};

enum BUFFER_CALLBACK_RET
{
	BUFFER_CALLBACK_OK,
	BUFFER_CALLBACK_SHOULD_STOP,
	BUFFER_CALLBACK_ERROR,
};

struct VO_BUFFER_NEED
{
	VO_S64 want_physical_pos;
	VO_S64 want_size;
};

class interface_buffer_callback
{
public:
	virtual BUFFER_CALLBACK_RET buffer_notify( BUFFER_CALLBACK_NOTIFY_ID notify_id , VO_PTR ptr_data ) = 0;
};

#ifdef _VONAMESPACE
}
#endif

#endif