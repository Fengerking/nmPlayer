//#include "StdAfx.h"
#include "vo_file_buffer.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_file_buffer::vo_file_buffer(void)
{
}

vo_file_buffer::~vo_file_buffer(void)
{
}

VO_VOID vo_file_buffer::buffer_notify( BUFFER_CALLBACK_NOTIFY_ID notify_id , VO_PTR ptr_data )
{
	;
}

VO_BOOL vo_file_buffer::init( VOPDInitParam * ptr_param )
{
	return VO_TRUE;
}

VO_VOID vo_file_buffer::uninit()
{
	;
}

VO_S64 vo_file_buffer::read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread )
{
	return 0;
}

VO_S64 vo_file_buffer::write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite )
{
	return 0;
}

VO_S64 vo_file_buffer::seek( VO_S64 physical_pos )
{
	return 0;
}

VO_VOID vo_file_buffer::reset()
{
	;
}

VO_VOID vo_file_buffer::get_buffer_info( BUFFER_INFO * ptr_info )
{
	;
}
