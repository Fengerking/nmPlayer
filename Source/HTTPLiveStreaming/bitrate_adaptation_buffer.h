
#ifndef __BITRATE_ADAPTATION_BUFFER_H__

#define __BITRATE_ADAPTATION_BUFFER_H__

#include "voMTVBase.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

struct FRAME_BUFFER
{
	VO_MTV_FRAME_BUFFER * ptr_buffer;

	FRAME_BUFFER * ptr_next;
};

typedef VO_VOID (*ptr_framecallback)( VO_BOOL is_video , VO_MTV_FRAME_BUFFER * ptr_buffer , VO_PTR ptr_obj );

class bitrate_adaptation_buffer
{
public:
	bitrate_adaptation_buffer();
	~bitrate_adaptation_buffer();

    VO_VOID reset_all();
	VO_VOID set_adaptation();
	VO_VOID set_after_adaptation();

	VO_VOID send_audio( VO_MTV_FRAME_BUFFER * ptr_buffer );
	VO_VOID send_video( VO_MTV_FRAME_BUFFER * ptr_buffer );

	VO_VOID set_callback( VO_PTR ptr_obj , ptr_framecallback callback ){ m_ptr_obj = ptr_obj; m_ptr_callback = callback; }

private:
	VO_VOID add_buffer( FRAME_BUFFER ** pptr_header , FRAME_BUFFER ** pptr_tail , VO_MTV_FRAME_BUFFER * ptr_buffer );

private:
	VO_BOOL m_isaudio_adaptation;
	VO_BOOL m_isvideo_adaptation;

	FRAME_BUFFER * m_ptr_audio_header;
	FRAME_BUFFER * m_ptr_audio_tail;

	FRAME_BUFFER * m_ptr_video_header;
	FRAME_BUFFER * m_ptr_video_tail;

	ptr_framecallback m_ptr_callback;
	VO_PTR	m_ptr_obj;
};

#ifdef _VONAMESPACE
}
#endif

#endif