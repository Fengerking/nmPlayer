#include "vo_slider.h"
#include "vo_playback_control_top_layer.h"

extern HINSTANCE		g_hInst;

vo_slider::vo_slider(void)
:m_is_capture( false)
,m_is_vertical(false)
,m_current_pos(0)
,m_max_pos(0)
,m_bEnable(true)
{
	m_nBitmapCount = 2;
}

vo_slider::~vo_slider(void)
{
}

void vo_slider::draw( HDC hdc , bool is_force/* = false*/  )
{
	if (hdc == NULL)
		return;

	int bitmap_id = m_bitmap_id;

	{
		int nIndex = 0;
		HBITMAP hbitmap = get_bitmap(nIndex);//LoadBitmap( g_hInst , MAKEINTRESOURCE(bitmap_id) );
		if(hbitmap == NULL)
			return;

		nIndex = 0;
		if( m_is_capture )
			nIndex = 1;
		else
		{
			if(this == m_ptr_mouse_on)
				nIndex = 2;
		}

		HBITMAP hbitmapThumb = m_bmpReaderThumb.GetBitmapHandle(nIndex);//get_bitmap(1);
		if(hbitmapThumb == NULL)
			return;

		if(m_is_vertical)
		{
			//if(m_bmpReaderThumb.GetHeight()>0)
			//	m_rect.bottom = m_rect.top + m_bmpReaderThumb.GetHeight();
			
			int nWidth = m_bmpReader.GetWidth();
			int nLeft = m_rect.left + (m_rect.right - m_rect.left - nWidth)/2;

			HDC memdc = CreateCompatibleDC( hdc );
			if (memdc == NULL)
				return;

			HBITMAP oldbitmap = (HBITMAP)SelectObject( memdc , hbitmap );

			::StretchBlt( hdc , nLeft , m_rect.top , nWidth , m_rect.bottom - m_rect.top , memdc , 0 , 0 ,m_bmpReader.GetWidth(), m_bmpReader.GetHeight(), SRCCOPY );
			//BitBlt( hdc , m_rect.left , m_rect.top , m_bmpReader.GetWidth() , m_bmpReader.GetHeight() , memdc , 0 , 0 , SRCCOPY );
			SelectObject( memdc , oldbitmap );

			if(m_max_pos>=0)
			{
				if(m_current_pos>m_max_pos)
					m_max_pos = m_current_pos;
				//to compute current pos
				int nHeight = 0;
				if(m_max_pos>0)
					nHeight = m_current_pos/(float)m_max_pos*(m_rect.bottom - m_rect.top);
				oldbitmap = (HBITMAP)SelectObject( memdc , get_bitmap(1) );
				::StretchBlt( hdc , nLeft , m_rect.bottom - nHeight , nWidth , nHeight , memdc , 0 , 0 ,m_bmpReader.GetWidth(), m_bmpReader.GetHeight(), SRCCOPY );
				SelectObject( memdc , oldbitmap );

				oldbitmap = (HBITMAP)SelectObject( memdc , hbitmapThumb );
				BitBlt( hdc , m_rect.left , m_rect.bottom - nHeight-m_bmpReaderThumb.GetHeight()/2 , m_bmpReaderThumb.GetWidth() , m_bmpReaderThumb.GetHeight() , memdc , 0 , 0 , SRCCOPY );
				SelectObject( memdc , oldbitmap );
			}
			DeleteDC( memdc );
      memdc = NULL;
		}
		else
		{
			if(m_bmpReaderThumb.GetHeight()>0)
				m_rect.bottom = m_rect.top + m_bmpReaderThumb.GetHeight();
			
			int nHeight = m_bmpReader.GetHeight();
			int nTop = m_rect.top + (m_rect.bottom - m_rect.top - nHeight)/2;

			HDC memdc = CreateCompatibleDC( hdc );
			if (memdc == NULL)
				return;


			HBITMAP oldbitmap = (HBITMAP)SelectObject( memdc , hbitmap );

			::StretchBlt( hdc , m_rect.left , nTop , m_rect.right - m_rect.left , nHeight , memdc , 0 , 0 ,m_bmpReader.GetWidth(), m_bmpReader.GetHeight(), SRCCOPY );
			//BitBlt( hdc , m_rect.left , m_rect.top , m_bmpReader.GetWidth() , m_bmpReader.GetHeight() , memdc , 0 , 0 , SRCCOPY );
			SelectObject( memdc , oldbitmap );

			if(m_max_pos>=0)
			{
				if(m_current_pos>m_max_pos)
					m_max_pos = m_current_pos;
				//to compute current pos
				int nWidth = 0;
				if(m_max_pos>0)
					nWidth = m_current_pos/(float)m_max_pos*(m_rect.right - m_rect.left);
				oldbitmap = (HBITMAP)SelectObject( memdc , get_bitmap(1) );
				::StretchBlt( hdc , m_rect.left , nTop , nWidth , nHeight , memdc , 0 , 0 ,m_bmpReader.GetWidth(), m_bmpReader.GetHeight(), SRCCOPY );
				SelectObject( memdc , oldbitmap );

				oldbitmap = (HBITMAP)SelectObject( memdc , hbitmapThumb );
				BitBlt( hdc , m_rect.left +nWidth - m_bmpReaderThumb.GetWidth()/2 , m_rect.top , m_bmpReaderThumb.GetWidth() , m_bmpReaderThumb.GetHeight() , memdc , 0 , 0 , SRCCOPY );
				SelectObject( memdc , oldbitmap );
			}
			DeleteDC( memdc );
      memdc = NULL;
		}
	}
}

void vo_slider::left_button_down( int mouse_x , int mouse_y )
{
  if(FALSE == m_bEnable)
    return;

  if (NULL == m_ptr_parent)
  {
    return;
  }

	set_capture( this );
	m_is_capture = true;
	m_ptr_parent->children_need_repaint_callback( NULL );
}

void vo_slider::compute_current_pos(int mouse_x , int mouse_y)
{
}

void vo_slider::pre_left_button_up( int mouse_x , int mouse_y )
{
  if (NULL == m_ptr_parent)
  {
    return;
  }

	if( m_is_capture )
	{
		m_is_capture = false;
		release_capture();

		//to set current pos
		if(m_max_pos>0 && m_rect.right- m_rect.left>0)
		{
			//to compute current pos
			if(m_is_vertical)
			{
				int nHeight = (m_rect.bottom- m_rect.top) - mouse_y;// - m_rect.left;
				if(nHeight<0) nHeight = 0;
				if(nHeight>(m_rect.bottom- m_rect.top))
					 nHeight = (m_rect.bottom- m_rect.top);
				if ((m_rect.bottom- m_rect.top))
					m_current_pos = m_max_pos*nHeight/(m_rect.bottom- m_rect.top);//(float)m_max_pos*(m_rect.right - m_rect.left);
				if(m_current_pos>m_max_pos)
					m_max_pos = m_current_pos;
			}
			else
			{
				int nWidth = mouse_x;// - m_rect.left;
				if(nWidth<0) nWidth = 0;
				if(nWidth>m_rect.right- m_rect.left)
					 nWidth = m_rect.right- m_rect.left;
				if ((m_rect.right- m_rect.left))
					m_current_pos = m_max_pos*nWidth/(m_rect.right- m_rect.left);//(float)m_max_pos*(m_rect.right - m_rect.left);
				if(m_current_pos>m_max_pos)
					m_max_pos = m_current_pos;
			}
			m_ptr_parent->children_need_repaint_callback( NULL );
		}

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
	m_ptr_parent->children_need_repaint_callback( NULL );
}

void vo_slider::click()
{
  if (NULL == m_ptr_parent)
  {
    return;
  }

  if (FALSE == m_bEnable)
  {
    return;
  }

	vo_playback_control_top_layer * ptr_playback_control = (vo_playback_control_top_layer *)m_ptr_parent;
	ptr_playback_control->button_clicked( m_id );
}
void vo_slider::mouse_move( int mouse_x , int mouse_y )
{
  if (NULL == m_ptr_parent)
  {
    return;
  }

	{
		vo_rect rc;
		get_thumb_rect(&rc );
		if(rc.is_in_rect(mouse_x,mouse_y))
		{
			if(m_ptr_mouse_on!=this)
			{
				m_ptr_mouse_on = this;
				//to redraw
        if(m_ptr_parent)
				  m_ptr_parent->children_need_repaint_callback( NULL );
			}
		}
		else
		{
			if(m_ptr_mouse_on != NULL)
			{
				m_ptr_mouse_on = NULL;
        if(m_ptr_parent)
				  m_ptr_parent->children_need_repaint_callback( NULL );
			}
		}
	}
	//calculate_parent_coordinate( mouse_x , mouse_y );
	//m_ptr_parent->mouse_move( mouse_x , mouse_y );
}
void vo_slider::set_rect( const vo_rect* rc )
{
  if (NULL == rc)
  {
    return;
  }

	m_rect = *rc;
	if(m_is_vertical)
	{
		if(m_bmpReaderThumb.GetWidth()>0)
			m_rect.right = m_rect.left + m_bmpReaderThumb.GetWidth();
	}
	else
	{
		if(m_bmpReaderThumb.GetHeight()>0)
			m_rect.bottom = m_rect.top + m_bmpReaderThumb.GetHeight();
	}
}
void vo_slider::get_thumb_rect(vo_rect* rc )
{
  if(NULL == rc)
    return;

		if(m_is_vertical)
		{
			int nWidth = m_bmpReader.GetWidth();
			int nLeft = m_rect.left + (m_rect.right - m_rect.left - nWidth)/2;
			if(m_max_pos>=0)
			{
				if(m_current_pos>m_max_pos)
					m_max_pos = m_current_pos;
				//to compute current pos
				int nHeight = 0;
				if(m_max_pos>0)
					nHeight = m_current_pos/(float)m_max_pos*(m_rect.bottom - m_rect.top);
				//BitBlt( hdc , m_rect.left , m_rect.bottom - nHeight-m_bmpReaderThumb.GetHeight()/2 , m_bmpReaderThumb.GetWidth() , m_bmpReaderThumb.GetHeight() , memdc , 0 , 0 , SRCCOPY );
				rc->left = 0;
				rc->top = m_rect.bottom-m_rect.top - nHeight-m_bmpReaderThumb.GetHeight()/2;
				rc->right = rc->left + m_bmpReaderThumb.GetWidth();
				rc->bottom = rc->top + m_bmpReaderThumb.GetHeight();
			}
		}
		else
		{
			int nHeight = m_bmpReader.GetHeight();
			int nTop = m_rect.top + (m_rect.bottom - m_rect.top - nHeight)/2;

			if(m_max_pos>=0)
			{
				if(m_current_pos>m_max_pos)
					m_max_pos = m_current_pos;
				//to compute current pos
				int nWidth = 0;
				if(m_max_pos>0)
					nWidth = m_current_pos/(float)m_max_pos*(m_rect.right - m_rect.left);
				//BitBlt( hdc , m_rect.left +nWidth - m_bmpReaderThumb.GetWidth()/2 , m_rect.top , m_bmpReaderThumb.GetWidth() , m_bmpReaderThumb.GetHeight() , memdc , 0 , 0 , SRCCOPY );
				rc->left = nWidth - m_bmpReaderThumb.GetWidth()/2;
				rc->top = 0;//m_rect.bottom-m_rect.top - nHeight-m_bmpReaderThumb.GetHeight()/2;
				rc->right = rc->left + m_bmpReaderThumb.GetWidth();
				rc->bottom = rc->top + m_bmpReaderThumb.GetHeight();
			}
		}
}