#include "vo_button.h"
#include "tchar.h"
#include "vo_playback_control_top_layer.h"

extern HINSTANCE		g_hInst;

vo_button::vo_button(void)
:m_is_capture( false )
,m_is_checked(false)
{
	m_nBitmapCount = 4;
}

vo_button::~vo_button(void)
{
}

void vo_button::draw( HDC hdc , bool is_force/* = false*/  )
{
	if (hdc == NULL)
		return;

	int bitmap_id = m_bitmap_id;

	if( m_is_capture )
	{
		int nIndex = 1;
		if(m_is_checked)
			nIndex += 4;
		HBITMAP hbitmap = get_bitmap(nIndex);//LoadBitmap( g_hInst , MAKEINTRESOURCE(bitmap_id) );
		if (hbitmap == NULL)
			return;

		HDC memdc = CreateCompatibleDC( hdc );
		if (memdc == NULL)
			return;

		HBITMAP oldbitmap = (HBITMAP)SelectObject( memdc , hbitmap );

		BitBlt( hdc , m_rect.left , m_rect.top , this->m_bmpReader.GetWidth() , m_bmpReader.GetHeight() , memdc , 0 , 0 , SRCCOPY );

		SelectObject( memdc , oldbitmap );
		DeleteDC( memdc );
	}
	else
	{
		int nIndex = 0;
		if(m_is_checked)
			nIndex = 4;
		if(this == m_ptr_mouse_on)
			nIndex += 2;
		//nIndex = 4;
		HBITMAP hbitmap = get_bitmap(nIndex);//LoadBitmap( g_hInst , MAKEINTRESOURCE(bitmap_id) );
		if (hbitmap == NULL)
			return;

		HDC memdc = CreateCompatibleDC( hdc );
		if (memdc == NULL)
			return;

		HBITMAP oldbitmap = (HBITMAP)SelectObject( memdc , hbitmap );

		//if(nIndex ==0)
		BitBlt( hdc , m_rect.left , m_rect.top , m_bmpReader.GetWidth() , m_bmpReader.GetHeight() , memdc , 0 , 0 , SRCCOPY );

		SelectObject( memdc , oldbitmap );
		DeleteDC( memdc );
	}
}

void vo_button::left_button_down( int mouse_x , int mouse_y )
{
	set_capture( this );
	m_is_capture = true;
	if (m_ptr_parent)
	m_ptr_parent->children_need_repaint_callback( NULL );
}

void vo_button::pre_left_button_up( int mouse_x , int mouse_y )
{
	if( m_is_capture )
	{
		m_is_capture = false;
		release_capture();

		calculate_parent_coordinate( mouse_x , mouse_y );
		if( m_rect.is_in_rect( mouse_x , mouse_y ) )
		{
			click();
		}
	}
	else
	{
		vo_base_view::pre_left_button_up( mouse_x , mouse_y );
	}

	if (m_ptr_parent)
		m_ptr_parent->children_need_repaint_callback( NULL );
}

void vo_button::click()
{
	vo_playback_control_top_layer * ptr_playback_control = (vo_playback_control_top_layer *)m_ptr_parent;

	if (ptr_playback_control)
		ptr_playback_control->button_clicked( m_id );
}
void vo_button::mouse_move( int mouse_x , int mouse_y )
{
	if(m_ptr_mouse_on!=this)
	{
		m_ptr_mouse_on = this;
		//to redraw
		if (m_ptr_parent)
			m_ptr_parent->children_need_repaint_callback( NULL );
	}
	//calculate_parent_coordinate( mouse_x , mouse_y );
	//m_ptr_parent->mouse_move( mouse_x , mouse_y );
}
void vo_button::set_rect( const vo_rect* rc )
{
	if (rc == NULL)
		return;

	m_rect = *rc;
	CBitmapReader *pr = get_bitmap_reader();
	if (pr == NULL)
		return;

	if(pr->GetHeight()>0)
		m_rect.bottom = m_rect.top + pr->GetHeight();
	if(pr->GetWidth()>0)
		m_rect.right = m_rect.left + pr->GetWidth();
}

int vo_button::get_width()
{
  CBitmapReader *pr = get_bitmap_reader();
  if (pr == NULL)
    return 0;

  return pr->GetWidth();
}