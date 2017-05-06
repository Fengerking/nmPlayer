#pragma once
#include "vo_base_view.h"

class vo_slider :
	public vo_base_view
{
public:
	vo_slider(void);
	virtual ~vo_slider(void);

	virtual void draw( HDC hdc , bool is_force = false  );
	virtual void left_button_down( int mouse_x , int mouse_y );
	virtual void pre_left_button_up( int mouse_x , int mouse_y );
	virtual void mouse_move( int mouse_x , int mouse_y );
	virtual void set_rect( const vo_rect* rc );

	virtual void set_vertical( bool is_vertical ){ m_is_vertical = is_vertical; }
	virtual void set_current_pos(__int64 curr_pos){m_current_pos = curr_pos;};
	virtual void get_current_pos(__int64& curr_pos){curr_pos = m_current_pos;};
	virtual void set_max_pos(__int64 max_pos){m_max_pos = max_pos;};

  void SetEnable(bool bEnable){m_bEnable = bEnable;};

protected:
	virtual void click();
	void get_thumb_rect(vo_rect* rc );
	void compute_current_pos(int mouse_x , int mouse_y);

public:
	CBitmapReader  m_bmpReaderThumb;

protected:
	bool m_is_capture;
	bool m_is_vertical;

	__int64 m_current_pos;
	__int64 m_max_pos;

  bool m_bEnable;
};
