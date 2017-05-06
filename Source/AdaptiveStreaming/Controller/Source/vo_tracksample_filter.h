#ifndef __VO_TRACKSAMPLE_FILTER_H__

#define __VO_TRACKSAMPLE_FILTER_H__



#include "voSource2.h"
#include "voProgramInfoOp.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

enum TRACKSTATUS
{
	TRACKSTATUS_INIT = 0,
	TRACKSTATUS_NORMAL = 1,
	TRACKSTATUS_BA =  2,

	TRACKSTATUS_MAX = 255,
};

struct  SELECTINFO
{
	TRACKSTATUS status;
	VO_U32 pre_selected_trackid;
	VO_U32 pre_selected_astrackid;
	VO_U32 cur_selected_trackid;
	VO_U32 cur_selected_astrackid;
};

class vo_tracksample_filter
{
public:
	vo_tracksample_filter( voProgramInfoOp * ptr_programinfo_op );
	~vo_tracksample_filter(void);

	VO_U32 check_sample( VO_U32 type , VO_DATASOURCE_SAMPLE *ptr_sample );

protected:
	VO_VOID fill_selectinfo();

protected:
	voProgramInfoOp * m_ptr_programinfo_op;
	SELECTINFO m_trackselectinfo[255];
};

#ifdef _VONAMESPACE
}
#endif

#endif