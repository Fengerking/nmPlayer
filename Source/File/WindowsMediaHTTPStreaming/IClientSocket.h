#ifndef __IClientSocket_H__
#define __IClientSocket_H__

#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class IClientSocket
{
public:
	~IClientSocket() {}

	virtual VO_S32 BeginConnect() = 0;//
	//virtual VO_S32 Disconnect() = 0;//
	virtual VO_BOOL IsConnected() const = 0;//

	virtual VO_S32 Read(VO_CHAR *, VO_S32) = 0;
	virtual VO_S32 Write(const VO_CHAR *, VO_S32 iLenBuf = 0) = 0;

	virtual VO_S32 Close() = 0;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__IClientSocket_H__