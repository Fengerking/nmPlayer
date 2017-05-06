
#ifndef __VO_NETWORK_JUDGMENT_H__

#define __VO_NETWORK_JUDGMENT_H__

#include "voType.h"
#ifdef _IOS_TEST
namespace _DASH{
#endif
class vo_network_judgment
{
public:
	vo_network_judgment( VO_S32 judge_size );
	~vo_network_judgment();

	VO_VOID add_item( VO_S64 speed );
	VO_S64 get_judgment( VO_S64 current_bandwidth , VO_BOOL is_videodelay );

protected:
	VO_VOID get_machine_top_speed();

private:

	VO_S64 m_average_speed;
	VO_S32 m_listsize;
	VO_S32 m_usedsize;
	VO_S32 m_pos;
	VO_S64 * m_ptr_judgelist;
	VO_S32 m_total_number;
	VO_S64 m_total_average_speed;
	VO_S64 m_machine_top_speed;
};

#ifdef _IOS_TEST
}
#endif

#endif