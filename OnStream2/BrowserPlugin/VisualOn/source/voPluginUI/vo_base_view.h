#pragma once

#include <vector>
#include "windows.h"
#include "CBitmapReader.h"


class vo_rect
{
public:
	int left;
	int right;
	int top;
	int bottom;

	vo_rect():left(0),right(0),top(0),bottom(0){}
	vo_rect( int _left , int _top , int _right , int _bottom ):left(_left),top(_top),right(_right),bottom(_bottom){}

	explicit vo_rect( const vo_rect& rc ):left(rc.left),top(rc.top),right(rc.right),bottom(rc.bottom){}
	explicit vo_rect( const RECT& rc ):left(rc.left),top(rc.top),right(rc.right),bottom(rc.bottom){}

	int get_width() const{ return right - left; }
	int get_height() const{ return bottom - top; }

	bool is_in_rect( int x , int y ){ if( x < left || x > right ) return false; if( y < top || y > bottom ) return false; return true; }

	vo_rect& operator=( const vo_rect& rc ){ left = rc.left;top = rc.top;right = rc.right;bottom = rc.bottom; return *this; } 
	operator RECT(){ RECT rc = { left , top , right , bottom }; return rc; }
};

class vo_base_view
{
public:
	vo_base_view(void);
	virtual ~vo_base_view(void);

	virtual void set_rect( const vo_rect* rc ){ m_rect = *rc;}
	void get_rect( vo_rect *rc ){ *rc = m_rect; }

	void set_border( int border_size ){ m_border_left = m_border_right = m_border_top = m_border_bottom = border_size; }
	void set_border( int left , int top , int right , int bottom ){ m_border_left = left; m_border_right = right; m_border_top = top; m_border_bottom = bottom; }

	virtual void set_id( int id ){ m_id = id; }

	virtual void pre_left_button_down( int mouse_x , int mouse_y );
	virtual void pre_left_button_up( int mouse_x , int mouse_y );
	virtual void pre_mouse_move( int mouse_x , int mouse_y );

	virtual void left_button_down( int mouse_x , int mouse_y );
	virtual void left_button_up( int mouse_x , int mouse_y );
	virtual void mouse_move( int mouse_x , int mouse_y );

	void set_parent( vo_base_view * ptr_parent ){ m_ptr_parent = ptr_parent; }

	virtual void add_children( vo_base_view * ptr_child ){ m_children.push_back( ptr_child ); }
	virtual void remove_all_children();

	virtual void children_need_repaint_callback( const vo_rect* rc );

	virtual void draw( HDC hdc , bool is_force = false );

	virtual void resize( int width , int height );

	virtual void set_capture( vo_base_view * ptr_view );
	virtual void release_capture();

	virtual void destroy();

	virtual void close();

	virtual void set_playback_state( bool is_playback );
	virtual HBITMAP get_bitmap(int nIndex);
	CBitmapReader* get_bitmap_reader(){return &m_bmpReader;};
	virtual void set_resource( int bitmap_id );

	virtual void set_visible(bool is_visible){m_is_visible = is_visible;};
	virtual bool get_visible(){return m_is_visible;};


protected:
	vo_rect m_rect;
	std::vector< vo_base_view * > m_children;

	int m_border_left;
	int m_border_right;
	int m_border_top;
	int m_border_bottom;

	vo_base_view * m_ptr_parent;
	vo_base_view * m_ptr_capture_view;

	CBitmapReader  m_bmpReader;
	int			   m_nBitmapCount;
	int			   m_bitmap_id;
	//bool		   m_is_mouse_on;
	//bool		   m_is_enable_care_mouse_in;
	static vo_base_view * m_ptr_mouse_on;
	int m_id;
	bool		   m_is_visible;

protected:
	virtual vo_base_view* get_mouseon_item( int mouse_x , int mouse_y );
	virtual void calculate_parent_rect( vo_rect * rc );
	virtual void calculate_child_coordinate( vo_base_view * ptr_view , int& mouse_x , int& mouse_y );
	virtual void calculate_parent_coordinate( int& mouse_x , int& mouse_y );
};
