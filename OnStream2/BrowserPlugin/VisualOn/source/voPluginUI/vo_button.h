#pragma once
#include "vo_base_view.h"

class vo_button :
	public vo_base_view
{
public:
	vo_button(void);
	virtual ~vo_button(void);

	virtual void draw( HDC hdc , bool is_force = false  );
	virtual void left_button_down( int mouse_x , int mouse_y );
	virtual void pre_left_button_up( int mouse_x , int mouse_y );
	virtual void mouse_move( int mouse_x , int mouse_y );
	virtual void set_rect( const vo_rect* rc );
	virtual void set_checked( bool is_check ){m_is_checked = is_check;};
	virtual bool get_checked( ){ return m_is_checked;};

	virtual void set_vertical( bool is_vertical ){ m_is_vertical = is_vertical; }

	int     get_width();
protected:
	virtual void click();

protected:
	bool m_is_capture;
	bool m_is_vertical;
	bool m_is_checked;

};
