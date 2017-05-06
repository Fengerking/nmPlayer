
#ifndef __INTERFACE_BUFFER_CALLBACK_H__

#define __INTERFACE_BUFFER_CALLBACK_H__


enum BUFFER_CALLBACK_RET
{
	BUFFER_CALLBACK_OK,
	BUFFER_CALLBACK_DATA_LACKTOOMUCH,
	BUFFER_CALLBACK_SHOULD_STOP,
	BUFFER_CALLBACK_ERROR,
};

class interface_buffer_callback
{
public:
	virtual BUFFER_CALLBACK_RET buffer_lack( VO_S64 lack_pos , VO_S64 lack_size ) = 0;
	virtual BUFFER_CALLBACK_RET buffer_low( VO_S64 startdownload_pos ) = 0;
	virtual BUFFER_CALLBACK_RET buffer_newstart( VO_S64 start_pos ) = 0;
};

#endif