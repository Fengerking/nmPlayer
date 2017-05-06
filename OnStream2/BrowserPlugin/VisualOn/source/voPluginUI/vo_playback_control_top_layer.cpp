#include "vo_playback_control_top_layer.h"
#include "vo_palypause_button.h"
#include "RESOURCE.h"
#include "tchar.h"
#include "WinGDI.h"

#define AUDIO_BUTTON 1
#define PLAY_PAUSE_BUTTON 2
#define FULL_SCREEN_BUTTON 3
#define TIME_SLIDER 4
#define AUDIO_SLIDER 5
#define REC_BUTTON 6
#define SELECT_AUDIO_BUTTON 7
#define SELECT_VIDEO_BUTTON 8
#define SELECT_SUBTITLE_BUTTON 9
#define CLEAR_SELECTION_BUTTON 10
#define COMMIT_SELECTION_BUTTON 11

extern HINSTANCE		g_hInst;

vo_playback_control_top_layer::vo_playback_control_top_layer(void)
:m_is_show(true)
,m_font(0)
{
	m_nBitmapCount = 1;
	m_ptr_time_slider = 0;
	m_ptr_audio_slider = 0;
//	m_ptr_rec_button = 0;
	m_ptr_audio_button = NULL;
	m_ptr_play_button = NULL;
	m_ptr_full_screen_button = NULL;

	m_ptr_MultiAudio_button = NULL;
	m_ptr_MultiVideo_button = NULL;
	m_ptr_MultiSubtile_button = NULL;
	m_ptr_CommitSelection_button = NULL;

	ZeroMemory( m_str_current_time , sizeof(TCHAR) * 20 );
	ZeroMemory( m_str_duration , sizeof(TCHAR) * 20 );

	LOGFONT font;
	ZeroMemory( &font , sizeof(LOGFONT) );
	font.lfHeight = 18;
	font.lfWeight = FW_NORMAL;
	_tcscpy( font.lfFaceName , _T("Times New Roman"));
	m_font = CreateFontIndirect( &font );
}

vo_playback_control_top_layer::~vo_playback_control_top_layer(void)
{
	if( m_font )
	{
		DeleteObject( m_font );
		m_font = 0;
	}
}

void vo_playback_control_top_layer::pre_left_button_down( int mouse_x , int mouse_y )
{
	if( m_is_show )
		vo_base_view::pre_left_button_down( mouse_x , mouse_y );
	else
	{
		left_button_down( mouse_x , mouse_y );
	}
}

void vo_playback_control_top_layer::pre_left_button_up( int mouse_x , int mouse_y )
{
	if( m_is_show )
		vo_base_view::pre_left_button_up( mouse_x , mouse_y );
	else
	{
		left_button_up( mouse_x , mouse_y );
	}
}

void vo_playback_control_top_layer::left_button_down( int mouse_x , int mouse_y )
{
	if( m_is_show )
	{
		if( m_control_panel_rc.is_in_rect( mouse_x , mouse_y ) )
		{
			SendMessage( m_hwnd , WM_USER_CONTROLLING , 0 , 0 );
		}
	}
}

void vo_playback_control_top_layer::left_button_up( int mouse_x , int mouse_y )
{
	if( m_is_show )
	{
		if( m_control_panel_rc.is_in_rect( mouse_x , mouse_y ) )
		{
 			if( m_slidebar_rc.is_in_rect( mouse_x , mouse_y ) )
 			{
 				int seektime = 0;
				
				// david @ 2013/09/16
				// marked off to avoid seek to 0 once clicked in control bar but out of slider bar

 			//	if( m_rect.get_width() > m_rect.get_height() )
 			//	{
 			//		seektime = ((float)(mouse_x - m_slidebar_rc.left))/m_slidebar_rc.get_width() * m_duration;
 			//	}
 			//	else
 			//	{
 			//		seektime = ((float)(mouse_y - m_slidebar_rc.top))/m_slidebar_rc.get_height() * m_duration;
 			//	}
 
				//::PostMessage( m_hwnd , WM_PLAY_SEEK , (WPARAM)seektime , 0 );
 			}
		}
		else
		{
			fadeout();
		}
	}
	else
	{
		fadein();
	}
}

void vo_playback_control_top_layer::mouse_move( int mouse_x , int mouse_y )
{
	__super::mouse_move( mouse_x , mouse_y );
}

void vo_playback_control_top_layer::children_need_repaint_callback( const vo_rect* rc )
{
	if( m_is_show )
	{
		HDC hdc = ::GetDC(m_hwnd);//m_ddraw.GetDrawSurfaceDC();
		if (hdc == NULL)
			return;

		RECT _rc = m_rect;

		HDC memdc = CreateCompatibleDC( hdc );
		if (memdc == NULL) {
			::ReleaseDC(m_hwnd,hdc);
			hdc = NULL;
			return;
		}

		HBITMAP hbitmap = CreateCompatibleBitmap( hdc , m_rect.get_width() , m_rect.get_height() );
		HBITMAP oldbitmap = (HBITMAP)SelectObject( memdc , hbitmap );

		draw( memdc );

		BitBlt(hdc, m_rect.left, m_rect.top, m_rect.get_width() , m_rect.get_height(), memdc, 0, 0, SRCCOPY);

		//m_ddraw.UpdateRect( &_rc , 190 );

		SelectObject( memdc , oldbitmap );
		::ReleaseDC(m_hwnd,hdc);
		//m_ddraw.ReleaseDrawSurfaceDC( hdc );
		::DeleteObject(hbitmap );
		hbitmap = NULL;
		::DeleteDC( memdc );
		memdc = NULL;
	}
}

void vo_playback_control_top_layer::init( HWND hwnd )
{
	m_hwnd = hwnd;

  if(UISHOW.bAudioVolumeBtn)
  {
    m_ptr_audio_button = NULL;
    m_ptr_audio_button = new vo_button();
    if(NULL!=m_ptr_audio_button)
    {
      m_ptr_audio_button->set_parent( this );
      m_ptr_audio_button->set_id( AUDIO_BUTTON );
      m_ptr_audio_button->set_resource( IDB_BITMAP_AUDIO );
      add_children( m_ptr_audio_button );
    }
  }	

  if (UISHOW.bPlayPauseBtn)
  {
    m_ptr_play_button = new vo_palypause_button();
    if(NULL!=m_ptr_play_button)
    {
      m_ptr_play_button->set_parent( this );
      m_ptr_play_button->set_id( PLAY_PAUSE_BUTTON );
      m_ptr_play_button->set_resource( IDB_BITMAP_PLAY );//IDB_PLAYPAUSE
      add_children( m_ptr_play_button );
    }
  }

  if(UISHOW.bFullScreenBtn)
  {
    m_ptr_full_screen_button = NULL;
    m_ptr_full_screen_button = new vo_button();
    if(NULL!=m_ptr_full_screen_button)
    {
      m_ptr_full_screen_button->set_parent( this );
      m_ptr_full_screen_button->set_id( FULL_SCREEN_BUTTON );
      m_ptr_full_screen_button->set_resource( IDB_BITMAP_FULL_SCREEN );
      add_children( m_ptr_full_screen_button );
    }
  }


	/*m_ptr_rec_button = new vo_button();
	m_ptr_rec_button->set_parent( this );
	m_ptr_rec_button->set_id( REC_BUTTON );
	m_ptr_rec_button->set_resource( IDB_BITMAP_REC );
	add_children( m_ptr_rec_button );*/

  if(UISHOW.bAudioTrackBtn)
  {
    m_ptr_MultiAudio_button = new vo_button();
    if(NULL!=m_ptr_MultiAudio_button)
    {
      m_ptr_MultiAudio_button->set_parent( this );
      m_ptr_MultiAudio_button->set_id( SELECT_AUDIO_BUTTON );
      m_ptr_MultiAudio_button->set_resource( IDB_BITMAP_MULTIAUDIO );
      add_children( m_ptr_MultiAudio_button );
    }
  }

  if(UISHOW.bVideoTrackBtn)
  {
    m_ptr_MultiVideo_button = new vo_button();
    if(NULL!=m_ptr_MultiVideo_button)
    {
      m_ptr_MultiVideo_button->set_parent( this );
      m_ptr_MultiVideo_button->set_id( SELECT_VIDEO_BUTTON );
      m_ptr_MultiVideo_button->set_resource( IDB_BITMAP_MULTIVIDEO );
      add_children( m_ptr_MultiVideo_button );
    }
  }

  if(UISHOW.bSubtitleTrackBtn)
  {
    m_ptr_MultiSubtile_button = new vo_button();
    if(NULL!=m_ptr_MultiSubtile_button)
    {
      m_ptr_MultiSubtile_button->set_parent( this );
      m_ptr_MultiSubtile_button->set_id( SELECT_SUBTITLE_BUTTON );
      m_ptr_MultiSubtile_button->set_resource( IDB_BITMAP_MULTISUBTITLE );
      add_children( m_ptr_MultiSubtile_button );
    }
  }

  if(UISHOW.bCommitSelectBtn)
  {
    m_ptr_CommitSelection_button = new vo_button();
    if (NULL != m_ptr_CommitSelection_button)
    {
      m_ptr_CommitSelection_button->set_parent( this );
      m_ptr_CommitSelection_button->set_id( COMMIT_SELECTION_BUTTON );
      m_ptr_CommitSelection_button->set_resource( IDB_BITMAP_COMMITSELECTION );
      add_children( m_ptr_CommitSelection_button );
    }
  }
	
	//m_ddraw.Init( hwnd );
  if (UISHOW.bSeekBarSlide)
  {
    m_ptr_time_slider = new vo_slider();
    if(m_ptr_time_slider)
    {
      m_ptr_time_slider->set_parent( this );
      m_ptr_time_slider->set_id( TIME_SLIDER );
      m_ptr_time_slider->set_resource( IDB_BITMAP_TIME_SLIDER );//IDB_PLAYPAUSE
      m_ptr_time_slider->m_bmpReaderThumb.LoadResourceFromID(IDB_BITMAP_TIME_SLIDER_THUMB,-1,3);
      m_ptr_time_slider->set_max_pos(1000);
      add_children( m_ptr_time_slider );
    }
  }

 // if(UISHOW.bAudioVolumeBtn)
  {
    m_ptr_audio_slider = new vo_slider();
    if(NULL!=m_ptr_audio_slider)
    {
      m_ptr_audio_slider->set_parent( this );
      m_ptr_audio_slider->set_vertical( true );
      m_ptr_audio_slider->set_id( AUDIO_SLIDER );
      m_ptr_audio_slider->set_resource( IDB_BITMAP_AUDIO_SLIDER );//IDB_PLAYPAUSE
      m_ptr_audio_slider->m_bmpReaderThumb.LoadResourceFromID(IDB_BITMAP_AUDIO_SLIDER_THUMB,-1,3);
      m_ptr_audio_slider->set_max_pos(100);
      m_ptr_audio_slider->set_visible(false);
      add_children( m_ptr_audio_slider );
    }

    m_bmpReaderVolumeBack.LoadResourceFromID(IDB_BITMAP_VOLUME_BACK);
  }

  //IDB_BITMAP_BACKGROUD
  this->set_resource(IDB_BITMAP_BACKGROUD);

	calculate_control_rect( m_rect.bottom );
	reset_parent_window_region();
}

void vo_playback_control_top_layer::draw( HDC hdc , bool is_force/* = false*/  )
{
	RECT rc = m_rect;

	if(this->m_bmpReader.GetBitmapHandle(0) )
	{
		HDC memdc = CreateCompatibleDC( hdc );
		HBITMAP oldbitmap = (HBITMAP)SelectObject( memdc , m_bmpReader.GetBitmapHandle(0) );

		//	::StretchBlt( hdc , nLeft , m_rect.top , nWidth , m_rect.bottom - m_rect.top , memdc , 0 , 0 ,m_bmpReader.GetWidth(), m_bmpReader.GetHeight(), SRCCOPY );
		StretchBlt( hdc , rc.left , rc.bottom - m_bmpReader.GetHeight() , m_rect.get_width() , m_bmpReader.GetHeight() , memdc , 0 , 0 ,m_bmpReader.GetWidth(), m_bmpReader.GetHeight(), SRCCOPY );
		SelectObject( memdc , oldbitmap );
		DeleteDC( memdc );
    memdc = NULL;
	}
	else
		FillRect( hdc , &rc , (HBRUSH)GetStockObject( BLACK_BRUSH ) );

	if(m_ptr_audio_slider)
	{
		if(m_ptr_audio_slider->get_visible())
		{
			HDC memdc = CreateCompatibleDC( hdc );
			HBITMAP oldbitmap = (HBITMAP)SelectObject( memdc , m_bmpReaderVolumeBack.GetBitmapHandle(0) );

			//	::StretchBlt( hdc , nLeft , m_rect.top , nWidth , m_rect.bottom - m_rect.top , memdc , 0 , 0 ,m_bmpReader.GetWidth(), m_bmpReader.GetHeight(), SRCCOPY );
			BitBlt( hdc , m_volume_wnd_rc.left , m_volume_wnd_rc.top , m_bmpReaderVolumeBack.GetWidth(), m_bmpReaderVolumeBack.GetHeight() , memdc , 0 , 0 , SRCCOPY );
			SelectObject( memdc , oldbitmap );
			DeleteDC( memdc );
      memdc = NULL;
		}
	}

  if(NULL!=m_ptr_audio_button)
    m_ptr_audio_button->draw( hdc );
  if(NULL!=m_ptr_play_button)
    m_ptr_play_button->draw( hdc );
  if(NULL!=m_ptr_full_screen_button)
    m_ptr_full_screen_button->draw( hdc );
  //m_ptr_rec_button->draw( hdc );

  if(NULL!=m_ptr_MultiAudio_button)
    m_ptr_MultiAudio_button->draw( hdc );
  if(NULL!=m_ptr_MultiVideo_button)
    m_ptr_MultiVideo_button->draw( hdc );
  if(NULL!=m_ptr_MultiSubtile_button)
    m_ptr_MultiSubtile_button->draw( hdc );
  if(NULL!=m_ptr_CommitSelection_button)
    m_ptr_CommitSelection_button->draw(hdc);

  if(NULL!=m_ptr_time_slider)
    m_ptr_time_slider->draw( hdc );
  if(NULL!=m_ptr_audio_slider)
    m_ptr_audio_slider->draw( hdc );
}

void vo_playback_control_top_layer::resize( int width , int height )
{
	m_rect.right = m_rect.left + width;
	m_rect.bottom = m_rect.top + height;
	calculate_control_rect( m_rect.bottom );

	//m_ddraw.Release();
	//m_ddraw.Init( m_hwnd );
}

void vo_playback_control_top_layer::set_capture( vo_base_view * ptr_view )
{
	m_ptr_capture_view = ptr_view;
	SetCapture( m_hwnd );
}

void vo_playback_control_top_layer::release_capture()
{
	m_ptr_capture_view = NULL;
	ReleaseCapture();
}

void vo_playback_control_top_layer::reset_parent_window_region()
{
	m_control_panel_rc;
	HRGN combine_rgn = CreateRectRgn( m_control_panel_rc.left , m_control_panel_rc.top , m_control_panel_rc.right , m_control_panel_rc.bottom );
	if(m_ptr_audio_slider->get_visible())
	{
		HRGN volume_rgn = CreateRectRgn( m_volume_wnd_rc.left , m_volume_wnd_rc.top , m_volume_wnd_rc.right , m_volume_wnd_rc.bottom );
		HRGN combine_rgn1 = CreateRectRgn( 0 , 0 , 1 , 1 );
    if(NULL!=volume_rgn && NULL!=combine_rgn1 )
    {
      CombineRgn( combine_rgn1 , combine_rgn , volume_rgn , RGN_OR );
      DeleteObject( combine_rgn );
      combine_rgn = NULL;
      combine_rgn = combine_rgn1;
      DeleteObject( volume_rgn );
      volume_rgn = NULL;
    }
	}

	if(m_hwnd)
	{
		HRGN rgn = NULL;
		::GetWindowRgn(m_hwnd,rgn);//::GetRe
		if(rgn)
			::DeleteObject(rgn);
	}
	SetWindowRgn(m_hwnd,combine_rgn,TRUE);
//	
}

void vo_playback_control_top_layer::button_clicked( int id )
{
	switch( id )
	{
	case AUDIO_BUTTON:
		{
			//SendMessage( m_hwnd , WM_USER_CONTROLLING , 0 , 0 );
			//SendMessage( m_hwnd , WM_PLAY_PRE , 0 , 0 );
			if(this->m_ptr_audio_slider)
			{
				this->m_ptr_audio_slider->set_visible(!m_ptr_audio_slider->get_visible());
				//reset region of parent window
				reset_parent_window_region();
				children_need_repaint_callback(NULL);
			}
		}
		break;
	case FULL_SCREEN_BUTTON:
		{
			//SendMessage( m_hwnd , WM_USER_CONTROLLING , 0 , 0 );
			SendMessage( m_hwnd , WM_PLAY_FULL_SCREEN , 0 , 0 );
		}
		break;
	case PLAY_PAUSE_BUTTON:
		{
			//SendMessage( m_hwnd , WM_USER_CONTROLLING , 0 , 0 );
			if(NULL!=m_ptr_play_button && (((vo_palypause_button*)m_ptr_play_button)->get_pause_statue() ))
			{
				SendMessage( m_hwnd , WM_PLAY_PLAY , 0 , 0 );
			}
			else
			{
				SendMessage( m_hwnd , WM_PLAY_PAUSE , 0 , 0 );
			}
		}
		break;
	case TIME_SLIDER:
		{
			__int64 nPos = 0;
      if(this->m_ptr_time_slider)
      {
        this->m_ptr_time_slider->get_current_pos(nPos);
      }
			SendMessage( m_hwnd , WM_PLAY_SEEK , (WPARAM)nPos , 0 );
		}
		break;
	case AUDIO_SLIDER:
		{
			__int64 nPos = 0;
      if(this->m_ptr_audio_slider)
      {
        this->m_ptr_audio_slider->get_current_pos(nPos);
      }
			SendMessage( m_hwnd , WM_AUDIO_SLIDER , (WPARAM)nPos , 0 );
		}
		break;
  case SELECT_VIDEO_BUTTON:
      {
          SendMessage( m_hwnd , WM_SELECT_VIDEO , 0 , 0 );
      }
      break;
  case SELECT_AUDIO_BUTTON:
      {
          SendMessage( m_hwnd , WM_SELECT_AUDIO , 0 , 0 );
      }
      break;
  case SELECT_SUBTITLE_BUTTON:
      {
          SendMessage( m_hwnd , WM_SELECT_SUBTITLE , 0 , 0 );
      }
      break;
  case CLEAR_SELECTION_BUTTON:
      {
          SendMessage( m_hwnd , WM_CLEAR_SELECTION , 0 , 0 );
      }
      break;
  case COMMIT_SELECTION_BUTTON:
      {
          SendMessage( m_hwnd , WM_COMMIT_SELECTION , 0 , 0 );
      }
      break;
  default:
      break;
	}
}

void vo_playback_control_top_layer::set_rect( const vo_rect* rc )
{
	if (rc == NULL)
		return;

	m_rect = *rc;
	calculate_control_rect( m_rect.bottom );
}

void vo_playback_control_top_layer::calculate_control_rect( int bottom )
{
	//m_rect.top = m_rect.bottom-m_bmpReader.GetHeight()-m_bmpReaderVolumeBack.GetHeight();
	m_control_panel_rc = m_rect;
	m_control_panel_rc.top = bottom - this->m_bmpReader.GetHeight();

	vo_rect rc(m_control_panel_rc);

  //the butns on the time slide left position
  //only play
  if(m_ptr_play_button)
  {
    rc.left += 5;
    rc.top += 2;
    m_ptr_play_button->set_rect( &rc );  
  }

  //the butns on the time slide right position
  //fullscreen, audio volume, clearselection, commintselection,subtitletrack, vediotrack , audiotrack
  if(m_ptr_full_screen_button)
  {
    int iWidthItem = m_ptr_full_screen_button->get_width();
    rc.left = m_control_panel_rc.right - iWidthItem - 5;
    //now only one fullscreen butn, do not need space
    rc.left +=5;

    m_ptr_full_screen_button->set_rect( &rc );
  }

  //audio butn
  if(m_ptr_audio_button)
  {
    int iWidthItem = m_ptr_audio_button->get_width();
    rc.left = rc.left - iWidthItem;
    m_ptr_audio_button->set_rect( &rc );
  }

	if(this->m_ptr_audio_slider)
	{
		if( m_ptr_audio_button )
			m_ptr_audio_button->get_rect( &rc );
		vo_rect rc2 (m_control_panel_rc);
		rc2.top -= (this->m_bmpReaderVolumeBack.GetHeight()-10);
		rc2.bottom = rc2.top+63;
		rc2.left = (rc.left+rc.right)/2- m_ptr_audio_slider->m_bmpReaderThumb.GetWidth()/2;
		rc2.right = rc2.left+m_ptr_audio_slider->m_bmpReaderThumb.GetWidth();
		m_ptr_audio_slider->set_rect(&rc2);
	}

  if(m_ptr_CommitSelection_button)
  {
    int iWidthItem = m_ptr_CommitSelection_button->get_width();
    rc.left = rc.left - iWidthItem;
    m_ptr_CommitSelection_button->set_rect( &rc );
  }

  if(m_ptr_MultiSubtile_button)
  {
    int iWidthItem = m_ptr_MultiSubtile_button->get_width();
    rc.left = rc.left - iWidthItem;
    m_ptr_MultiSubtile_button->set_rect( &rc );
  }

  if(m_ptr_MultiVideo_button)
  {
    int iWidthItem = m_ptr_MultiVideo_button->get_width();
    rc.left = rc.left - iWidthItem;
    m_ptr_MultiVideo_button->set_rect( &rc );
  }

  if(m_ptr_MultiAudio_button)
  {
    int iWidthItem = m_ptr_MultiAudio_button->get_width();
    rc.left = rc.left - iWidthItem;
    m_ptr_MultiAudio_button->set_rect( &rc );
  }

  if(m_ptr_time_slider)
  {
    vo_rect rcPlay;
    if(m_ptr_play_button)
      m_ptr_play_button->get_rect(&rcPlay);
    m_slidebar_rc.left = rcPlay.right+3;
    m_slidebar_rc.right = rc.left;
    m_slidebar_rc.top = m_control_panel_rc.top+14;
    m_slidebar_rc.bottom = m_control_panel_rc.top + 40;
    m_ptr_time_slider->set_rect(&m_slidebar_rc);
  }

	m_volume_wnd_rc = m_rect;
	if(this->m_ptr_audio_slider)
	{
		m_volume_wnd_rc.top = m_rect.bottom - m_bmpReaderVolumeBack.GetHeight()- m_bmpReader.GetHeight();
		m_volume_wnd_rc.bottom = m_volume_wnd_rc.top + m_bmpReaderVolumeBack.GetHeight();
		m_ptr_audio_slider->get_rect(&rc);
		m_volume_wnd_rc.left = (rc.right + rc.left)/2 - m_bmpReaderVolumeBack.GetWidth()/2;
		m_volume_wnd_rc.right = m_volume_wnd_rc.left + m_bmpReaderVolumeBack.GetWidth();
	}
	
}

void vo_playback_control_top_layer::set_current_play_time( int time )
{
	m_current_play_time = time;
	//_stprintf( m_str_current_time , _T("%02d:%02d:%02d") , m_current_play_time/3600 , m_current_play_time / 60 , m_current_play_time % 60 );
	//children_need_repaint_callback( &m_current_time_rc );
}

void vo_playback_control_top_layer::set_duration( int duration )
{
	m_duration = duration / 1000;
	//_stprintf( m_str_duration , _T("%02d:%02d:%02d") , m_duration/3600 , m_duration / 60 , m_duration % 60 );
	//children_need_repaint_callback( &m_duration_rc );

	
}

void vo_playback_control_top_layer::show()
{
	m_is_show = true;
	children_need_repaint_callback(NULL);
	SendMessage( m_hwnd , WM_PLAYCONTROL_SHOW , 0 , 0 );
}

void vo_playback_control_top_layer::hide()
{
	m_is_show = false;
//#ifdef _WIN32_WM50
	//m_ddraw.HideDrawSurfaceDC();
//#endif
	SendMessage( m_hwnd , WM_PLAYCONTROL_HIDE , 0 , 0 );
}

void vo_playback_control_top_layer::init_play()
{
  if (NULL == m_ptr_play_button)
  {
    return;
  }
	((vo_palypause_button*)m_ptr_play_button)->set_pause_statue();
	set_current_play_time( 0 );

}

void vo_playback_control_top_layer::fadein()
{
	//int array[] = { 90 , 80 , 60 , 30 };

	//m_is_show = true;

	//for( int i = 0 ; i < sizeof(array) / sizeof(int) ; i++ )
	//{
	//	calculate_control_rect( m_rect.bottom + array[i] );
	//	children_need_repaint_callback(NULL);
	//}

	//calculate_control_rect( m_rect.bottom);

	//show();
}

void vo_playback_control_top_layer::fadeout()
{
	//int array[] = { 10 , 30 , 60 , 90 };

	//m_is_show = true;

	//for( int i = 0 ; i < sizeof(array) / sizeof(int) ; i++ )
	//{
	//	calculate_control_rect( m_rect.bottom + array[i] );
	//	children_need_repaint_callback(NULL);
	//}

	//hide();
}
//
//void vo_directui_top_layer::children_need_repaint_callback( HRGN hrgn )
//{
//
//	HRGN combine_rgn = CreateRectRgn( 0 , 0 , 1 , 1 );
//	HRGN show_rgn = CreateRectRgn( m_rect.left , m_rect.top , m_rect.right , m_rect.bottom );
//	CombineRgn( combine_rgn , show_rgn , hrgn , RGN_AND );
//
//	InvalidateRgn( m_hwnd , combine_rgn , TRUE );
//
//	DeleteObject( combine_rgn );
//	DeleteObject( show_rgn );
//}
