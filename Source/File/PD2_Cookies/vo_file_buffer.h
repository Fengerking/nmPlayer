#pragma once
#include "vo_buffer.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class vo_file_buffer :
	public vo_buffer
{
public:
	vo_file_buffer(void);
	virtual ~vo_file_buffer(void);

	//interface_buffer_callback
	virtual VO_VOID buffer_notify( BUFFER_CALLBACK_NOTIFY_ID notify_id , VO_PTR ptr_data );
	//

	//vo_buffer
	virtual VO_BOOL init( VOPDInitParam * ptr_param );
	virtual VO_VOID uninit();
	virtual VO_S64 read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread );
	virtual VO_S64 write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite );
	virtual VO_S64 seek( VO_S64 physical_pos );
	virtual VO_VOID reset();
	virtual VO_VOID get_buffer_info( BUFFER_INFO * ptr_info );
	//
};
#ifdef _VONAMESPACE
}
#endif
