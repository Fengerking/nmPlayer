
#include "bitrate_adaptation_buffer.h"
#include "voString.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

bitrate_adaptation_buffer::bitrate_adaptation_buffer()
:m_isaudio_adaptation(VO_FALSE)
,m_isvideo_adaptation(VO_FALSE)
,m_ptr_audio_header(0)
,m_ptr_audio_tail(0)
,m_ptr_video_header(0)
,m_ptr_video_tail(0)
{
}

bitrate_adaptation_buffer::~bitrate_adaptation_buffer()
{
	FRAME_BUFFER * ptr_item = m_ptr_audio_header;
	while( ptr_item )
	{
		VO_PBYTE ptr_bytes = (VO_PBYTE)ptr_item->ptr_buffer;
		delete []ptr_bytes;
		FRAME_BUFFER * ptr_temp = ptr_item;
		ptr_item = ptr_temp->ptr_next;
		delete ptr_temp;
	}
	m_ptr_audio_header = m_ptr_audio_tail = 0;

	ptr_item = m_ptr_video_header;
	while( ptr_item )
	{
		VO_PBYTE ptr_bytes = (VO_PBYTE)ptr_item->ptr_buffer;
		delete []ptr_bytes;
		FRAME_BUFFER * ptr_temp = ptr_item;
		ptr_item = ptr_temp->ptr_next;
		delete ptr_temp;
	}
	
	m_ptr_video_header = m_ptr_video_tail = 0;
}



VO_VOID bitrate_adaptation_buffer::reset_all()
{
	FRAME_BUFFER * ptr_item = m_ptr_audio_header;
	while( ptr_item )
	{
		VO_PBYTE ptr_bytes = (VO_PBYTE)ptr_item->ptr_buffer;
		delete []ptr_bytes;
		FRAME_BUFFER * ptr_temp = ptr_item;
		ptr_item = ptr_temp->ptr_next;
		delete ptr_temp;
	}
	m_ptr_audio_header = m_ptr_audio_tail = 0;

	ptr_item = m_ptr_video_header;
	while( ptr_item )
	{
		VO_PBYTE ptr_bytes = (VO_PBYTE)ptr_item->ptr_buffer;
		delete []ptr_bytes;
		FRAME_BUFFER * ptr_temp = ptr_item;
		ptr_item = ptr_temp->ptr_next;
		delete ptr_temp;
	}
	m_ptr_video_header = m_ptr_video_tail = 0;

	m_isaudio_adaptation = VO_FALSE;
    m_isvideo_adaptation = VO_FALSE;
}

VO_VOID bitrate_adaptation_buffer::set_adaptation()
{
	m_isaudio_adaptation = VO_TRUE;
	m_isvideo_adaptation = VO_TRUE;
}

VO_VOID bitrate_adaptation_buffer::set_after_adaptation()
{
	m_isaudio_adaptation = VO_FALSE;
	m_isvideo_adaptation = VO_FALSE;

	FRAME_BUFFER * ptr_item = m_ptr_audio_header;
	while( ptr_item )
	{
		VO_PBYTE ptr_bytes = (VO_PBYTE)ptr_item->ptr_buffer;
		delete []ptr_bytes;
		FRAME_BUFFER * ptr_temp = ptr_item;
		ptr_item = ptr_temp->ptr_next;
		delete ptr_temp;
	}
	m_ptr_audio_header = m_ptr_audio_tail = 0;

	ptr_item = m_ptr_video_header;
	while( ptr_item )
	{
		VO_PBYTE ptr_bytes = (VO_PBYTE)ptr_item->ptr_buffer;
		delete []ptr_bytes;
		FRAME_BUFFER * ptr_temp = ptr_item;
		ptr_item = ptr_temp->ptr_next;
		delete ptr_temp;
	}
	m_ptr_video_header = m_ptr_video_tail = 0;
}

VO_VOID bitrate_adaptation_buffer::send_audio( VO_MTV_FRAME_BUFFER * ptr_buffer )
{
	//VOLOGE( "send_audio %lld , %s , %p" , ptr_buffer->nStartTime , m_isaudio_adaptation == VO_TRUE ? "TRUE" : "FALSE" , m_ptr_audio_header );
	if( m_isaudio_adaptation )
	{
		if( m_ptr_audio_header == 0 )
		{
			m_ptr_callback( VO_FALSE , ptr_buffer , m_ptr_obj );
			return;
		}

		if( ptr_buffer->nStartTime > m_ptr_audio_tail->ptr_buffer->nStartTime )
			return;

		FRAME_BUFFER * ptr_item = m_ptr_audio_header;
		m_ptr_audio_tail = ptr_item;

		while( ptr_item )
		{
			if( ptr_item->ptr_buffer->nStartTime >= ptr_buffer->nStartTime )
				break;

			m_ptr_audio_tail = ptr_item;
			ptr_item = ptr_item->ptr_next;
		}

		if( ptr_item == m_ptr_audio_header )
		{
			m_ptr_audio_header = m_ptr_audio_tail = 0;
		}

		while( ptr_item )
		{
			VO_PBYTE ptr_bytes = (VO_PBYTE)ptr_item->ptr_buffer;
			delete []ptr_bytes;
			FRAME_BUFFER * ptr_temp = ptr_item;
			ptr_item = ptr_temp->ptr_next;
			delete ptr_temp;
		}

		if( m_ptr_audio_tail )
			m_ptr_audio_tail->ptr_next = 0;


		ptr_buffer->nFrameType = 0xff;
		ptr_item = m_ptr_audio_header;


		while( ptr_item )
		{		
		    ptr_item->ptr_buffer->nFrameType = 0xFE;
			m_ptr_callback( VO_FALSE , ptr_item->ptr_buffer , m_ptr_obj );

			delete []ptr_item->ptr_buffer;
			FRAME_BUFFER * ptr_temp = ptr_item;
			ptr_item = ptr_temp->ptr_next;
			delete ptr_temp;
		}

		m_ptr_audio_header = m_ptr_audio_tail = 0;

		m_ptr_callback( VO_FALSE , ptr_buffer , m_ptr_obj );
	}
	else
		add_buffer( &m_ptr_audio_header , &m_ptr_audio_tail , ptr_buffer );
}

VO_VOID bitrate_adaptation_buffer::send_video( VO_MTV_FRAME_BUFFER * ptr_buffer )
{
	//VOLOGE( "send_video %lld %s" , ptr_buffer->nStartTime , ptr_buffer->nFrameType == 0? "Key Frame" : "" );
	if( m_isvideo_adaptation )
	{
		if( m_ptr_video_header == 0 )
		{
			m_ptr_callback( VO_TRUE , ptr_buffer , m_ptr_obj );
			return;
		}

		if( ptr_buffer->nFrameType != 0 )
			return;

		FRAME_BUFFER *ptr_item = m_ptr_video_header;

		while( ptr_item )
		{
			if( ptr_item->ptr_buffer->nStartTime >= ptr_buffer->nStartTime )
				break;

			m_ptr_video_tail = ptr_item;
			ptr_item = ptr_item->ptr_next;
		}

		if( ptr_item == m_ptr_video_header )
		{
			m_ptr_video_header = m_ptr_video_tail = 0;
		}

		if( m_ptr_video_tail )
			m_ptr_video_tail->ptr_next = NULL;

		while( ptr_item )
		{
			delete []ptr_item->ptr_buffer;
			FRAME_BUFFER * ptr_temp = ptr_item;
			ptr_item = ptr_temp->ptr_next;
			delete ptr_temp;
		}

		ptr_buffer->nFrameType = 0xff;

		ptr_item = m_ptr_video_header;

        VOLOGE("+Start output last url buffer!");
		while( ptr_item )
		{
            ptr_item->ptr_buffer->nFrameType = 0xFE;
			m_ptr_callback( VO_TRUE , ptr_item->ptr_buffer , m_ptr_obj );

			delete []ptr_item->ptr_buffer;
			FRAME_BUFFER * ptr_temp = ptr_item;
			ptr_item = ptr_temp->ptr_next;
			delete ptr_temp;
		}

        VOLOGE("+End output last url buffer!");

		m_ptr_video_header = m_ptr_video_tail = 0;

		m_ptr_callback( VO_TRUE , ptr_buffer , m_ptr_obj );
	}
	else
		add_buffer( &m_ptr_video_header , &m_ptr_video_tail , ptr_buffer );
}

VO_VOID bitrate_adaptation_buffer::add_buffer( FRAME_BUFFER ** pptr_header , FRAME_BUFFER ** pptr_tail , VO_MTV_FRAME_BUFFER * ptr_buffer )
{
	VO_PBYTE ptr_bytes = new VO_BYTE[ sizeof( VO_MTV_FRAME_BUFFER ) + ptr_buffer->nSize ];
	VO_MTV_FRAME_BUFFER * ptr_new_buffer = ( VO_MTV_FRAME_BUFFER *)ptr_bytes;
	memcpy( ptr_new_buffer , ptr_buffer , sizeof( VO_MTV_FRAME_BUFFER ) );
	ptr_new_buffer->pData = ptr_bytes + sizeof( VO_MTV_FRAME_BUFFER );
	memcpy( ptr_new_buffer->pData , ptr_buffer->pData , ptr_buffer->nSize );

	FRAME_BUFFER * ptr_frame = new FRAME_BUFFER;
	ptr_frame->ptr_buffer = ptr_new_buffer;
	ptr_frame->ptr_next = 0;

	if( *pptr_header == NULL )
		*pptr_header = *pptr_tail = ptr_frame;
	else
	{
		(*pptr_tail)->ptr_next = ptr_frame;
		*pptr_tail = ptr_frame;
	}
}
