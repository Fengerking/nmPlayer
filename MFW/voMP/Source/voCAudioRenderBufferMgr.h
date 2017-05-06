	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCAudioRenderBufferMgr.h

	Contains:	voCAudioRenderBufferMgr header file

	Written by:	East Zhou

	Change History (most recent first):
	2013-11-25		East		Create file

*******************************************************************************/
#ifndef __voCAudioRenderBufferMgr_H__
#define __voCAudioRenderBufferMgr_H__

#include "voAudio.h"

typedef struct  
{
	VO_U32				nSize;		// size of PCM data, for 1s audio
	VO_PBYTE			pData;		// PCM data
	VO_AUDIO_FORMAT		sFormat;	// format of PCM data

	VO_U32				nLength;	// length of PCM data, a little more than step time
	VO_S64				llTime;		// time stamp of PCM data
} VOMP_AUDIORENDERBUFFER;

class voCARBList
{
public:
	voCARBList();
	virtual ~voCARBList();

public:
	VO_BOOL						create(VO_U32 nBuffers);
	VO_BOOL						add(VOMP_AUDIORENDERBUFFER * pBuffer);
	VO_BOOL						remove(VOMP_AUDIORENDERBUFFER * pBuffer);
	void						removeAll();
	VOMP_AUDIORENDERBUFFER *	getHead() { if(m_ppBuffers) return m_ppBuffers[0]; else return NULL;}
	VO_U32						getCount() {return m_nCount;}

protected:
	VOMP_AUDIORENDERBUFFER **		m_ppBuffers;
	VO_U32							m_nBuffers;
	VO_U32							m_nCount;
};

class voCAudioRenderBufferMgr
{
public:
	voCAudioRenderBufferMgr(VO_U32 nBufferCount);
	virtual ~voCAudioRenderBufferMgr();

public:
	// need retry / ok / fail
	VO_U32			getWritableBuffer(VOMP_AUDIORENDERBUFFER ** ppARB, VO_AUDIO_FORMAT * pFormat);
	void			finishWriting(VOMP_AUDIORENDERBUFFER * pARB);
	// we need try to make *pnGot == nToGet except new audio format / EOS
	// if nToGet is -1, return whole buffer of VOMP_AUDIORENDERBUFFER
	// if audio format of VOMP_AUDIORENDERBUFFER is not same as *pFormat, return VO_ERR_FINISH and re-write *pFormat
	VO_U32			getPCMData(VO_PBYTE pToGet, VO_U32 nToGet, VO_U32 * pnGot, VO_S64 * pllTime, VO_AUDIO_FORMAT * pFormat);
	void			flush();

	VO_BOOL			isFull() { return (m_listIdle.getCount() == 0) ? VO_TRUE : VO_FALSE; }
	VO_U32			getBufferCount() { return m_listValid.getCount(); }

protected:
	VOMP_AUDIORENDERBUFFER *	m_pARBs;
	VO_U32						m_nBufferCount;
	voCARBList					m_listIdle;
	voCARBList					m_listValid;
};

#endif // __voCAudioRenderBufferMgr_H__
