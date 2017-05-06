
#if !defined __VOMSM_RENDER__
#define __VOMSM_RENDER__

#include "voCMutex.h"
#include "voastruct.h"

class vomsmrender {

public:
	vomsmrender();
	virtual ~vomsmrender();

	VO_U32 		SetFormat (VO_AUDIO_FORMAT*);
	VO_U32 		Start (void);
	VO_U32 		Pause (void);
	VO_U32 		Stop (void);
	VO_U32 		Render (VO_PBYTE, VO_U32, VO_U64);
	VO_U32 		Flush (void);
	VO_U32 		GetPlayingTime (VO_S64*);
	VO_U32 		GetBufferTime (VO_S32*);
	VO_U32 		SetParam (VO_U32, VO_PTR);
	VO_U32 		GetParam (VO_U32, VO_PTR);

protected:
	__attribute__((always_inline)) bool open();
	__attribute__((always_inline)) void close();

	VO_U32 		RenderData (VO_PBYTE, VO_U32, VO_U64);


private:
	int									m_fd;
	int									m_nminbufsize;
	int									m_nminbuftime;
	int									m_nstartcount;
	bool								m_bstarted;
	long long							m_nstartplay;
	long long							m_nwallclock;

	VOARSTAT							m_status;
	PVOAUDIOPARAM					m_pvoaparam;
	voCMutex							m_csTime;

	VO_PBYTE							m_pBuffer;
	VO_U32								m_nBuffSize;
	VO_U32								m_nRenderNum;
};

#endif
