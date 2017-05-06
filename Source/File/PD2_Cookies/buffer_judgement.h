
#ifndef __BUFFER_JUDGEMENT_H__

#define __BUFFER_JUDGEMENT_H__

#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class buffer_judgement
{
public:
	buffer_judgement( VO_S32 judgement_array_size );
	virtual ~buffer_judgement(void);

	VO_VOID push_pos( VO_S64 pos );
	VO_BOOL get_judgement( VO_S64 * ptr_result );
	VO_VOID reset();

private:
	VO_S64 *m_ptr_pos_array;
	VO_S32 m_arraysize;

	VO_S32 m_filled_size;
	VO_S32 m_next_fill_pos;
};

#ifdef _VONAMESPACE
}
#endif

#endif
