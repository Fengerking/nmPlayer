#ifndef __VOSOURCE2ADAPTIVESTREAMING_H__

#define __VOSOURCE2ADAPTIVESTREAMING_H__

#include "voSource2Source.h"


class voSource2SourceAdaptiveStreaming :
	public voSource2Source
{
public:
	voSource2SourceAdaptiveStreaming( const VO_TCHAR * str_modulename , const VO_TCHAR * str_apiname );
	virtual ~voSource2SourceAdaptiveStreaming(void);
};


#endif
