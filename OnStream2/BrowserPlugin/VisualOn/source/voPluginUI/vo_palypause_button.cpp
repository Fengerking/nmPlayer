#include "vo_palypause_button.h"
#include "vo_playback_control_top_layer.h"

extern HINSTANCE		g_hInst;

vo_palypause_button::vo_palypause_button(void)
//:m_is_pause(true)
{
	m_nBitmapCount = 8;
	//m_is_checked = true;
}

vo_palypause_button::~vo_palypause_button(void)
{
}

void vo_palypause_button::draw( HDC hdc , bool is_force/* = false*/  )
{
	__super::draw(hdc,is_force);
}

void vo_palypause_button::click()
{
  if(NULL == m_ptr_parent)
    return;

	//m_is_pause = !m_is_pause;
	m_is_checked = !m_is_checked;

	m_ptr_parent->children_need_repaint_callback( NULL );

	vo_playback_control_top_layer * ptr_playback_control = (vo_playback_control_top_layer *)m_ptr_parent;
	ptr_playback_control->button_clicked( m_id );
}
