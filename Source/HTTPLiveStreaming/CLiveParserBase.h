#ifndef _LIVESRC_PARSER_H_
#define	_LIVESRC_PARSER_H_

#include "voString.h"
#include "CDllLoad.h"
#include "voParser.h"
#include "voLiveSource.h"

//#pragma warning (disable : 4996)


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef VO_U32 (VO_API * VOGETPARSERAPI) (VO_PARSER_API * pFunc);

class CLiveParserBase : public CDllLoad
{
public:
	CLiveParserBase(VO_LIVESRC_FORMATTYPE type);
	virtual ~CLiveParserBase(void);

	virtual VO_U32 LoadLib (VO_HANDLE hInst/*, TCHAR* pszParserFileName*/);

	VO_U32 Open(VO_PARSER_INIT_INFO * pInitInfo);
	VO_U32 Close(void);
	VO_U32 Process(VO_PARSER_INPUT_BUFFER * pBuffer);
	VO_U32 SetParam(VO_U32 uID, VO_PTR pParam);
	VO_U32 GetParam(VO_U32 uID, VO_PTR pParam);


protected:
	VO_HANDLE				m_hDemux;		
	VO_PARSER_API			m_ParserFunc;
	VO_LIVESRC_FORMATTYPE	m_nParserType;
};

#ifdef _VONAMESPACE
}
#endif

#endif //_LIVESRC_PARSER_H_
