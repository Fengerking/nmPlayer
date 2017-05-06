#pragma once

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32

#include "vomeAPI.h"
#include "voString.h"

#include "COMXEngine.h"

#include "vo_m3u_manager.h"
#include "vo_http_stream.h"

#include "voTSParser.h"
#include "voParser.h"
#include "voMTVBase.h"
#include "vo_http_live_streaming.h"

class voLiveStreamingPlayer :
	public COMXEngine
{
public:
	voLiveStreamingPlayer(void);
	~voLiveStreamingPlayer(void);

	OMX_ERRORTYPE SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay);

	OMX_ERRORTYPE Playback (VOME_SOURCECONTENTTYPE * pSource);
	OMX_ERRORTYPE Close (void);

	OMX_ERRORTYPE Run (void);
	OMX_ERRORTYPE Pause (void);
	OMX_ERRORTYPE Stop (void);

	OMX_ERRORTYPE GetDuration (OMX_S32 * pDuration);
	OMX_ERRORTYPE GetCurPos (OMX_S32 * pCurPos);
	OMX_ERRORTYPE SetCurPos (OMX_S32 nCurPos);

protected:
	OMX_COMPONENTTYPE *		m_pCompSrc;
	OMX_COMPONENTTYPE *		m_pCompAD;
	OMX_COMPONENTTYPE *		m_pCompVD;
	OMX_COMPONENTTYPE *		m_pCompAR;
	OMX_COMPONENTTYPE *		m_pCompVR;

	OMX_COMPONENTTYPE *		m_pCompClock;

	OMX_VO_DISPLAYAREATYPE	m_dspArea;

public:
	HANDLE					m_hThreadMedia;
	OMX_U32					m_nMediaThreadID;
	bool					m_bMediaStop;

	static int	ReadMediaDataProc (OMX_PTR pParam);
	int			ReadMediaDataLoop (void);

	VO_VOID data_callback( OMX_BUFFERHEADERTYPE * ptr_buffer );
	VO_VOID change_buffer_time( VO_S32 time );

	VO_VOID wait_reconnect_graph();
	VO_VOID send_eos();

	virtual int			CallBackFunc (int nID, void * pParam1, void * pParam2);

protected:
	vo_http_live_streaming m_streaming;
	VO_BOOL m_is_reloading;

#ifdef WIN32
	HMODULE m_tsparser;
#endif
};
