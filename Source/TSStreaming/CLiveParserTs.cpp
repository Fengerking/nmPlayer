#include "CLiveParserTs.h"

CLiveParserTs::CLiveParserTs(VO_LIVESRC_FORMATTYPE type)
:CLiveParserBase(type)
{
	memset (&m_ParserFunc, 0, sizeof (VO_PARSER_API));
}

CLiveParserTs::~CLiveParserTs(void)
{
}

VO_U32 CLiveParserTs::LoadLib(VO_HANDLE hInst)
{
	voGetParserAPI(&m_ParserFunc);
	
	return VO_ERR_PARSER_OK;
}
