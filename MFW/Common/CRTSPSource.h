	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CRTSPSource.h

	Contains:	CRTSPSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CRTSPSource_H__
#define __CRTSPSource_H__

#include "CFileSource.h"
#include "voRTSP.h"

#ifdef _HTC
#include "../../Thirdparty/HTC/mmProperty/inc/HTC_mm_property.h"
#include "../../Thirdparty/HTC/mmProperty/inc/HTC_mm_property_name.h"
typedef int (* MMPROPERTYGETAPI) (const char *key, char *value, const char *default_value);
#endif	//_HTC

#define	VO_STREAM_BUFFERSTART		1001
#define	VO_STREAM_BUFFERSTATUS		1002
#define	VO_STREAM_BUFFERSTOP		1003
#define	VO_STREAM_DOWNLODPOS		1004
#define	VO_STREAM_ERROR				1005
#define VO_STREAM_PACKETLOST		1006
#define VO_STREAM_RUNTIMEERROR		1007
#define VO_STREAM_BITRATETHROUGHPUT	1008
#define VO_STREAM_RTCP_RR			1009
#define VO_STREAM_SEEKFINALPOS		1010
#define VO_STREAM_DESCRIBE_FAIL		1011

// RTSP session event
#define VO_STREAM_VALUEOFFSET		(VO_RTSP_EVENT_SESSION_TEARDOWN - VO_STREAM_RTSP_SESSION_TEARDOWN)
#define VO_STREAM_RTSP_SESSION_START		1020
#define VO_STREAM_RTSP_SESSION_END			1021
#define VO_STREAM_RTSP_SESSION_RECVSTART	1022
#define VO_STREAM_RTSP_SESSION_RECVFINISH	1023
#define VO_STREAM_RTSP_SESSION_TEARDOWN		1024

typedef struct VOSTREAMBITRATETHROUGHPUT
{
	VO_U32		nBitrate;		// kbits/s
	VO_U32		nThroughput;	// kbytes/s
} VOStreamBitrateThroughput;

class CRTSPSource : public CFileSource
{
public:
	CRTSPSource (VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB);
	virtual ~CRTSPSource (void);

	virtual VO_U32		LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32		CloseSource (void);

	virtual VO_U32		GetTrackData (VO_U32 nTrack, VO_PTR pTrackData);

	virtual VO_BOOL		CanSeek (void);
	virtual VO_U32		SetTrackPos (VO_U32 nTrack, VO_S64 * pPos);

	virtual VO_U32		Start(void);
	virtual VO_U32		Pause(void);
	virtual VO_U32		Stop(void);

	virtual VO_U32		SetCallBack (VO_PTR pCallBack, VO_PTR pUserData);

	virtual VOStreamInitParam*	GetStreamInitParam() {return &m_initParam;}
	virtual VO_U32		SetInitPlayPos(VO_U32 nInitPlayPos);

	//added by Lin Jun,support private param ID
	virtual VO_U32		SetSourceParam (VO_U32 nID, VO_PTR pValue);

protected:
	virtual void		ReleaseFrameBuffer (void);
	virtual int			HandleStreamEvent (long EventCode, long * EventParam1);

	virtual void		NotifyBitrateThroughput();

protected:
	enum{
		RTSP_STATUS_NULL = 0,
		RTSP_STATUS_PLAY,
		RTSP_STATUS_PAUSE,
		RTSP_STATUS_STOP
	};
	voCMutex					m_csRTSPStatus;
	VOStreamInitParam			m_initParam;
	VO_CHAR						m_szURL[1024*2];
	VO_BOOL						m_bHadOpenned;
	voCMutex					m_csForceClose;

	VO_U32						m_nTracks;
	VO_PBYTE *					m_ppBuffer;

	VO_BOOL						m_bBuffering;
	VO_BOOL						m_bBufferEndMsg;

	VO_U32						m_nAudioSendSamples;
	VO_U32						m_nVideoSendSamples;

	VO_U32						m_nInitPlayPos;

	STREAMING_CALLBACK_NOTIFY	m_pCallBack;
	VO_PTR						m_pUserData;
	VO_BOOL						m_bSendError;

	VO_U32						m_nStatus;

protected:
	static int VO_STREAMING_API	voRTSPStreamCallBack (long EventCode, long * EventParam1, long * userData);
	long				m_lEventCode;
};

#endif // __CRTSPSource_H__
