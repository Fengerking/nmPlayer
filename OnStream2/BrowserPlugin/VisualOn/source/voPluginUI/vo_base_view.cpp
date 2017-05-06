#include "vo_base_view.h"

vo_base_view * vo_base_view::m_ptr_mouse_on = NULL;

vo_base_view::vo_base_view(void)
:m_ptr_capture_view(NULL)
,m_ptr_parent(NULL)
,m_border_left(1)
,m_border_right(1)
,m_border_top(1)
,m_border_bottom(1)
,m_nBitmapCount(0)
,m_bitmap_id(-1)
,m_id(0)
,m_is_visible(true)
{
	
}

vo_base_view::~vo_base_view(void)
{
	remove_all_children();
}

void vo_base_view::pre_left_button_down( int mouse_x , int mouse_y )
{
	if( m_ptr_capture_view )
	{
		calculate_child_coordinate( m_ptr_capture_view , mouse_x , mouse_y );
		m_ptr_capture_view->pre_left_button_down( mouse_x , mouse_y );
	}
	else
	{
		vo_base_view *ptr_view = get_mouseon_item( mouse_x , mouse_y );

		if( ptr_view )
		{
			calculate_child_coordinate( ptr_view , mouse_x , mouse_y );
			ptr_view->pre_left_button_down( mouse_x , mouse_y );
		}
		else
			left_button_down( mouse_x , mouse_y );
	}
}

void vo_base_view::pre_left_button_up( int mouse_x , int mouse_y )
{
	if( m_ptr_capture_view )
	{
		calculate_child_coordinate( m_ptr_capture_view , mouse_x , mouse_y );
		m_ptr_capture_view->pre_left_button_up( mouse_x , mouse_y );
	}
	else
	{
		vo_base_view *ptr_view = get_mouseon_item( mouse_x , mouse_y );

		if( ptr_view )
		{
			calculate_child_coordinate( ptr_view , mouse_x , mouse_y );
			ptr_view->pre_left_button_up( mouse_x , mouse_y );
		}
		else
			left_button_up( mouse_x , mouse_y );
	}
}

void vo_base_view::pre_mouse_move( int mouse_x , int mouse_y )
{
	if( m_ptr_capture_view )
	{
		calculate_child_coordinate( m_ptr_capture_view , mouse_x , mouse_y );
		m_ptr_capture_view->pre_mouse_move( mouse_x , mouse_y );
	}
	else
	{
		vo_base_view *ptr_view = get_mouseon_item( mouse_x , mouse_y );

		if( ptr_view )
		{
			calculate_child_coordinate( ptr_view , mouse_x , mouse_y );
			ptr_view->pre_mouse_move( mouse_x , mouse_y );
		}
		else
			mouse_move( mouse_x , mouse_y );
	}
}

void vo_base_view::left_button_down( int mouse_x , int mouse_y )
{
	calculate_parent_coordinate( mouse_x , mouse_y );
	if (m_ptr_parent)
		m_ptr_parent->left_button_down( mouse_x , mouse_y );
}

void vo_base_view::left_button_up( int mouse_x , int mouse_y )
{
	calculate_parent_coordinate( mouse_x , mouse_y );
	if (m_ptr_parent)
		m_ptr_parent->left_button_up( mouse_x , mouse_y );
}

void vo_base_view::mouse_move( int mouse_x , int mouse_y )
{
	if(m_ptr_mouse_on!=this)
	{
		m_ptr_mouse_on = this;
		//to redraw
		if(m_ptr_parent)
			m_ptr_parent->children_need_repaint_callback( NULL );
		else
			children_need_repaint_callback( NULL );
	}
	//calculate_parent_coordinate( mouse_x , mouse_y );
	//m_ptr_parent->mouse_move( mouse_x , mouse_y );
}

void vo_base_view::children_need_repaint_callback( const vo_rect* rc )
{
	if (rc == NULL)
		return;

	if( rc->left >= m_rect.right ||
		rc->right <= m_rect.left ||
		rc->bottom <= m_rect.top ||
		rc->top >= m_rect.bottom )
		return;

	vo_rect this_rc( *rc);
	calculate_parent_rect( &this_rc );

	if (m_ptr_parent)
		m_ptr_parent->children_need_repaint_callback( &this_rc );
}

vo_base_view* vo_base_view::get_mouseon_item( int mouse_x , int mouse_y )
{
	std::vector< vo_base_view* >::iterator iter = m_children.begin();
	std::vector< vo_base_view* >::iterator itere = m_children.end();

	while( iter != itere )
	{
		vo_rect rc;

		if ((*iter) != NULL)
			(*iter)->get_rect(&rc);

		if( rc.is_in_rect( mouse_x , mouse_y ) )
			return *iter;

		iter++;
	}

	return NULL;
}

void vo_base_view::draw( HDC hdc , bool is_force/* = false*/  )
{
	std::vector< vo_base_view* >::iterator iter = m_children.begin();
	std::vector< vo_base_view* >::iterator itere = m_children.end();

	while( iter != itere )
	{
		if ((*iter) != NULL)
			(*iter)->draw(hdc);
		iter++;
	}
}

void vo_base_view::remove_all_children()
{
	std::vector< vo_base_view* >::iterator iter = m_children.begin();
	std::vector< vo_base_view* >::iterator itere = m_children.end();

	while( iter != itere )
	{
		if ((*iter) != NULL) {
			delete *iter;
			*iter = NULL;
		}
		iter++;
	}

	m_children.clear();
}

void vo_base_view::resize( int width , int height )
{
	std::vector< vo_base_view* >::iterator iter = m_children.begin();
	std::vector< vo_base_view* >::iterator itere = m_children.end();

	while( iter != itere )
	{
		if ((*iter) != NULL)
			(*iter)->resize( width , height );
		iter++;
	}
}

void vo_base_view::set_capture( vo_base_view * ptr_view )
{
	if( ptr_view != this )
		m_ptr_capture_view = ptr_view;

	if (m_ptr_parent)
	m_ptr_parent->set_capture( this );
}

void vo_base_view::release_capture()
{
	m_ptr_capture_view = NULL;
	if (m_ptr_parent)
	m_ptr_parent->release_capture();
}

void vo_base_view::calculate_parent_rect( vo_rect * rc )
{
	if (rc == NULL)
		return;

	int width,height;
	width = rc->get_width();
	height = rc->get_height();

	rc->left = m_rect.left + rc->left;
	rc->top = m_rect.top + rc->top;
	rc->right = rc->left + width;
	rc->bottom = rc->top + height;
}

void vo_base_view::calculate_child_coordinate( vo_base_view * ptr_view , int& mouse_x , int& mouse_y )
{
	vo_rect rc;
	if (ptr_view )
		ptr_view->get_rect( &rc );

	mouse_x = mouse_x - rc.left;
	mouse_y = mouse_y - rc.top;
}

void vo_base_view::calculate_parent_coordinate( int& mouse_x , int& mouse_y )
{
	mouse_x = mouse_x + m_rect.left;
	mouse_y = mouse_y + m_rect.top;
}

void vo_base_view::destroy()
{
	if (m_ptr_parent)
	m_ptr_parent->destroy();
}

void vo_base_view::set_playback_state( bool is_playback )
{
	std::vector< vo_base_view* >::iterator iter = m_children.begin();
	std::vector< vo_base_view* >::iterator itere = m_children.end();

	while( iter != itere )
	{
		if ((*iter) != NULL)
			(*iter)->set_playback_state( is_playback );
		iter++;
	}
}

void vo_base_view::close()
{
	std::vector< vo_base_view* >::iterator iter = m_children.begin();
	std::vector< vo_base_view* >::iterator itere = m_children.end();

	while( iter != itere )
	{
		if ((*iter) != NULL)
			(*iter)->close();
		iter++;
	}
}
HBITMAP vo_base_view::get_bitmap(int nIndex)
{
	HBITMAP h = m_bmpReader.GetBitmapHandle(nIndex);
	if(m_bitmap_id>=0 && h == NULL && m_nBitmapCount>0)
	{
		m_bmpReader.LoadResourceFromID(m_bitmap_id, -1, m_nBitmapCount);
		h = m_bmpReader.GetBitmapHandle(nIndex);
	}
	return h;
}
void vo_base_view::set_resource( int bitmap_id )
{
	m_bitmap_id = bitmap_id;
	if(m_nBitmapCount>0)
		m_bmpReader.LoadResourceFromID(m_bitmap_id, -1, m_nBitmapCount);
}