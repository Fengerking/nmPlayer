#ifndef __OnStreamCAudioRender_h__
#define __OnStreamCAudioRender_h__

#include "voThread.h"
#include "voOSFunc.h"
#include "CXAudio2Render.h"
#include "CBaseAudioRender.h"
#include "COSWinVomePlayer.h"

class COSWinAudioRender
{
public:
	COSWinAudioRender(COSWinVomePlayer * pEngine);
	virtual	~COSWinAudioRender(void);

	virtual int			Start (void);
	virtual int			Pause (void);
	virtual int			Stop (void);
	virtual int			Flush (void);

	static	int			AudioRenderThreadProc (void * pParam);	
	virtual	int			AudioRenderThreadLoop (void);
	
protected:
	CXAudio2Render *	m_pOutRender;
	voThreadHandle     m_pRenderThread;

	COSWinVomePlayer *	m_pEngine;

	int					m_nStatus;

	VOOSMP_AUDIO_FORMAT	m_sAudioFormat;

	VOMP_BUFFERTYPE *	m_pAudioBuffer;

	unsigned char*		m_pBuffer;
	unsigned int		m_uRenderSize;

	int					m_nRenderNum;
};

#endif //#define __OnStreamCAudioRender_h__

