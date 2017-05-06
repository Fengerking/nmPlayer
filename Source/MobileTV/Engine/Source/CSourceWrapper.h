#ifndef _MTV_SOURCE_WRAPPER_H_
#define	_MTV_SOURCE_WRAPPER_H_

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32

#include "voString.h"
#include "CDllLoad.h"
#include "vortpsrc.h"

#pragma warning (disable : 4996)

typedef VO_U32 (VO_API * VOGETSOURCEAPI) (VO_RTPSRC_API * pFunc);

class CMTVSourceWrapper : public CDllLoad
{
public:
	CMTVSourceWrapper(void);
	virtual ~CMTVSourceWrapper(void);

	int		Open(char * szSDPData, VO_RTPSRC_INIT_INFO * pInitInfo);
	int		Close(void);

	int		Start(void);
	int		Stop(void);

	int		GetParam(VO_U32 uID, VO_PTR pParam);
	int		SetParam(VO_U32 uID, VO_PTR pParam);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	VO_HANDLE		m_hMTVSrc;		
	VO_RTPSRC_API	m_MTVSrcFunc;

private:
};

#endif //_MTV_SOURCE_WRAPPER_H_
