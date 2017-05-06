
#ifndef _LIVESRC_PARSER_TS_H_

#define _LIVESRC_PARSER_TS_H_

#include "CLiveParserBase.h"


class CLiveParserTs: public CLiveParserBase
{
public:
	CLiveParserTs(VO_LIVESRC_FORMATTYPE type);
	~CLiveParserTs(void);
	
	virtual VO_U32 LoadLib (VO_HANDLE hInst);
};

#endif
