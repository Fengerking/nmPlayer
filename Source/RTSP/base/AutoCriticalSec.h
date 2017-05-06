#ifndef __AUTOCRITICALSEC_H__
#define __AUTOCRITICALSEC_H__
#include "voRTSPSDK.h"
#include "utility.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

using namespace vo_cross_platform_streaming;
class CAutoCriticalSec
{
public:
	CAutoCriticalSec(IVOMutex* pMutex)
	{
		m_pMutex = pMutex;
		m_pMutex->Lock();
		if(CUserOptions::UserOptions.m_bMakeLog==1)
			SIMPLE_LOG("rtsp.txt","@!@lock\n");
	}

	~CAutoCriticalSec()
	{
		m_pMutex->UnLock();
		if(CUserOptions::UserOptions.m_bMakeLog==1)
			SIMPLE_LOG("rtsp.txt","@!@unlock\n");
	}

protected:
	IVOMutex*			  m_pMutex;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__AUTOCRITICALSEC_H__