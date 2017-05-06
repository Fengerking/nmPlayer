#pragma once
#include "vo_base_view.h"
//#include "CDDrawAdmin.h"
#include "vo_button.h"
#include "vo_slider.h"

#define WM_USER_CONTROLLING WM_USER+2000
#define WM_PLAYCONTROL_SHOW	WM_USER+2001
#define WM_PLAYCONTROL_HIDE WM_USER+2002
#define WM_PLAY_PAUSE		WM_USER+2003
#define WM_PLAY_PLAY		WM_USER+2004
#define WM_PLAY_FULL_SCREEN			WM_USER+2005
#define WM_AUDIO_SLIDER		WM_USER+2006
#define WM_PLAY_SEEK		WM_USER+2007
#define WM_SELECT_AUDIO		WM_USER+2008
#define WM_SELECT_VIDEO		WM_USER+2009
#define WM_SELECT_SUBTITLE	WM_USER+2010
#define WM_CLEAR_SELECTION  WM_USER+2011
#define WM_COMMIT_SELECTION WM_USER+2012

typedef struct _ui_show
{
  bool bPlayPauseBtn;
  bool bAudioVolumeBtn;
  bool bFullScreenBtn;
  bool bAudioTrackBtn;
  bool bVideoTrackBtn;
  bool bSubtitleTrackBtn;
  bool bCommitSelectBtn;
  bool bSeekBarSlide;

  _ui_show()
  {
    bPlayPauseBtn = false;
    bAudioVolumeBtn = false;
    bFullScreenBtn = true;
    bAudioTrackBtn = false;
    bVideoTrackBtn = false;
    bSubtitleTrackBtn = false;
    bCommitSelectBtn = false;
    bSeekBarSlide = false;
  }

}UI_SHOW;


class vo_playback_control_top_layer
	:public vo_base_view
{
public:
	vo_playback_control_top_layer(void);
	~vo_playback_control_top_layer(void);

	virtual void set_rect( const vo_rect* rc );

	void init( HWND hwnd );

	virtual void pre_left_button_down( int mouse_x , int mouse_y );
	virtual void pre_left_button_up( int mouse_x , int mouse_y );

	virtual void left_button_down( int mouse_x , int mouse_y );
	virtual void left_button_up( int mouse_x , int mouse_y );
	virtual void mouse_move( int mouse_x , int mouse_y );

	virtual void children_need_repaint_callback( const vo_rect* rc );

	virtual void draw( HDC hdc , bool is_force = false  );

	virtual void resize( int width , int height );

	virtual void set_capture( vo_base_view * ptr_view );
	virtual void release_capture();

	virtual void button_clicked( int id );

	virtual void set_current_play_time( int time );
	virtual void set_duration( int duration );

	virtual void show();
	virtual void hide();
	void fadein();
	void fadeout();

	virtual void init_play();
	void reset_parent_window_region();
	void calculate_control_rect( int bottom );

protected:

public:
	vo_slider * m_ptr_time_slider;
	vo_slider * m_ptr_audio_slider;
	vo_button * m_ptr_play_button;

private:
	HWND m_hwnd;
	//CDDrawAdmin m_ddraw;
	CBitmapReader  m_bmpReaderVolumeBack;

	//vo_rect m_top_wnd_rc;
	vo_rect m_control_panel_rc;
	//vo_rect m_current_time_rc;
	//vo_rect m_duration_rc;
	vo_rect m_slidebar_rc;
	vo_rect m_volume_wnd_rc;

	vo_button * m_ptr_audio_button;
	vo_button * m_ptr_full_screen_button;
//	vo_button * m_ptr_rec_button;

	vo_button * m_ptr_MultiAudio_button;
	vo_button * m_ptr_MultiVideo_button;
	vo_button * m_ptr_MultiSubtile_button;
  vo_button * m_ptr_CommitSelection_button;
 
	HFONT m_font;

	bool m_is_show;

	int m_current_play_time;
	int m_duration;

	TCHAR m_str_duration[20];
	TCHAR m_str_current_time[20];

	UI_SHOW UISHOW;
};
