#pragma once
#include "vo_button.h"

class vo_palypause_button :
	public vo_button
{
public:
	vo_palypause_button(void);
	~vo_palypause_button(void);

	virtual void draw( HDC hdc , bool is_force = false  );
	virtual void set_pause_statue( bool is_pause = true ){ this->set_checked(is_pause); }
	virtual bool get_pause_statue(){ return get_checked(); }

protected:
	virtual void click();

private:
	//bool m_is_pause;
};
