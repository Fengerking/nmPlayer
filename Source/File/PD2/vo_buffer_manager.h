#pragma once
#include "vo_buffer.h"
#include "vo_headerdata_buffer.h"

class vo_buffer_manager :
	public vo_buffer,
	public interface_buffer_callback
{
public:
	vo_buffer_manager(void);
	virtual ~vo_buffer_manager(void);

	//interface_buffer_callback
	virtual BUFFER_CALLBACK_RET buffer_notify( BUFFER_CALLBACK_NOTIFY_ID notify_id , VO_PTR ptr_data );
	//

	//vo_buffer
	virtual VO_BOOL init( VOPDInitParam * ptr_param );
	virtual VO_VOID uninit();
	virtual VO_S64 read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread );
	virtual VO_S64 write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite );
	virtual VO_S64 seek( VO_S64 physical_pos );
	virtual VO_VOID reset();
	//

	virtual VO_VOID set_initmode( VO_BOOL isset );

	virtual VO_VOID set_filesize( VO_S64 filesize );

	virtual VO_BOOL rerrange_cache( headerdata_element * ptr_info , VO_S32 info_number );
	virtual VO_VOID get_buffer_info( BUFFER_INFO * ptr_info );

protected:
	vo_buffer * m_ptr_buffer;
	VOPDInitParam m_initparam;
};
