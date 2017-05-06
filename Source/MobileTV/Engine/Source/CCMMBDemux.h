#ifndef _CCMMBDEMUX_H_
#define	_CCMMBDEMUX_H_

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32

#include "voString.h"
#include "CDllLoad.h"
#include "voParser.h"
#include "voCMMBParser.h"

#pragma warning (disable : 4996)

typedef VO_U32 (VO_API * VOGETPARSERAPI) (VO_PARSER_API * pFunc);

class CCMMBDemux : public CDllLoad
{
public:
	CCMMBDemux(void);
	virtual ~CCMMBDemux(void);

	int		Open(VO_PARSER_INIT_INFO * pInitInfo);
	int		Close(void);

	int		Process(VO_PARSER_INPUT_BUFFER * pBuffer);
	int		SetCASType(int nType);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	bool	LoadDemux(void);

protected:
	VO_HANDLE		m_hDemux;		
	VO_PARSER_API	m_ParserFunc;

private:
};

#endif //_CCMMBDEMUX_H_
