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

#define	VO_STREAM_BUFFERSTART		1001
#define	VO_STREAM_BUFFERSTATUS		1002
#define	VO_STREAM_BUFFERSTOP		1003
#define	VO_STREAM_DOWNLODPOS		1004
#define	VO_STREAM_ERROR				1005

class CRTSPSource : public CFileSource
{
public:
	static CRTSPSource * g_pRTSPSource;

public:
	CRTSPSource (VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP);
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


protected:
	virtual void		ReleaseFrameBuffer (void);
	virtual int			HandleStreamEvent (long EventCode, long * EventParam1);

protected:
	VOStreamInitParam			m_initParam;
	VO_CHAR						m_szURL[1024];
	VO_BOOL						m_bHadOpenned;

	VO_U32						m_nTracks;
	VO_PBYTE *					m_ppBuffer;

	VO_BOOL						m_bBuffering;
	VO_BOOL						m_bBufferEndMsg;

	VO_S64						m_nAudioFirstTime;
	VO_S64						m_nAudioSendTime;

	VO_U32						m_nAudioSendSamples;
	VO_U32						m_nVideoSendSamples;

	STREAMING_CALLBACK_NOTIFY	m_pCallBack;
	VO_PTR						m_pUserData;
	VO_BOOL						m_bSendError;

protected:
	static int VO_STREAMING_API	voRTSPStreamCallBack (long EventCode, long * EventParam1, long * userData);
	long				m_lEventCode;
	long *				m_pEventParam;
};

#endif // __CRTSPSource_H__
