#ifndef __CWMHTTP_H__
#define __CWMHTTP_H__

#include "HTTP_Transaction.h"
#include "voCSemaphore.h"
#include "voThread.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CWMHTTP : public CThread
{
public:
	class Delegate {
	public:
		~Delegate() {}
		virtual VO_VOID some(VO_CHAR *, VO_S32) = 0;
		virtual VO_VOID NotifyEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2) = 0;
	};

	CWMHTTP(CWMHTTP::Delegate *pDelegate);
	~CWMHTTP(void);

	VO_S32 open(VO_CHAR *szURL);
	VO_S32 close();

	VO_S32 play(VO_S64 llfromTime = 0, VO_U64 llfromPos = 0xFFFFFFFFFFFFFFFFLL);
	VO_S32 stop(VO_S32 iMaxTime = 0x7FFFFFFF);

	virtual VO_VOID ThreadMain();

protected:
	VO_S32 doDescribe();
	VO_S32 doPipeline();
	VO_S32 doStart();
	VO_S32	doStop();
	VO_S32	doLog();
	VO_S32		doKeepAlive();

	VO_S32 ArrangeData();

	VO_S32 MustRead(VO_CHAR *, VO_S32);
	VO_S32 MustSave(VO_S32);
	VO_S32 MustSkip(VO_S32);

	VO_S32 DataPacket();
	VO_S32 DataPacket_forVOReader();
	VO_S32 EndPacket();
	VO_S32 HeaderPacket();
	VO_S32 MetadataPacket();
	VO_S32 PacketPairPacket();
	VO_S32 TestPacket();

	VO_S32 find_playlist_gen_id();

	VO_VOID NotifyEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

private:
	enum STATE_WMHTTP {
		WMHTTP_UnInit,
		WMHTTP_Inited,	//while the state of media server is Idle.
		WMHTTP_Running,	//while the state of media server is Streaming.

		WMHTTP_Stopping	//only for non-Pipelined mode
	};

	STATE_WMHTTP	m_eStateWMHTTP;
	CHTTP_Transaction m_HTTPTransaction;

	CWMHTTP::Delegate* m_pDelegate;

	unsigned m_uiPacketSize;

	VO_CHAR m_szURL[1024];
	VO_CHAR m_szGUIDClient[40];

	VO_CHAR m_clientID[32];
	VO_CHAR m_playlist_gen_id[32];
	int m_LinkBW;

	VO_BOOL m_bBroadcast;
	VO_BOOL m_bPipelined;

	VO_S64 m_llSeekSkipSize;

	VO_S64	m_llfromTime;
	VO_U64	m_llfromPos;

	voCSemaphore	m_eventThreadLoop;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CWMHTTP_H__
