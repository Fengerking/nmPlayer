#ifndef __SDP_PARSE_H__
#define __SDP_PARSE_H__

#if defined(_WIN32) || defined(_WIN32_CE) || defined(WIN32)
#  include <tchar.h>
#elif defined(LINUX)
#  include "vocrstypes.h"
#endif
 
#include "list_T.h"

class CSDPSession;
class CSDPMedia;

class CSDPParse
{
public:
	CSDPParse();
	~CSDPParse();

public:
	bool Open(const TCHAR * _sdpFilePath);
	bool OpenSDP(const char * _sdpText);

	CSDPSession * GetSDPSession();
	CSDPMedia   * GetSDPMedia(int index);
	int           GetSDPMediaCount();

private:
	CSDPSession       * m_pSDPSession;
	list_T<CSDPMedia *> m_listSDPMedia;
};


#endif //__SDP_PARSE_H__
