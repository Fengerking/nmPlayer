

#include "vosource2source.h"
class voSource2SourceOther :
	public voSource2Source
{
public:
	voSource2SourceOther( const VO_TCHAR * str_modulename , const VO_TCHAR * str_apiname );
	virtual ~voSource2SourceOther(void);

	virtual VO_U32 Init ( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam );
};
