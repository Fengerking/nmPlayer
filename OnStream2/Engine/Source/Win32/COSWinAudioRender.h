#ifndef __OnStreamCAudioRender_h__
#define __OnStreamCAudioRender_h__

#include "voThread.h"
#include "voOSFunc.h"
#include "CWaveOutRender.h"
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
	CWaveOutRender *	m_pOutRender;
	voThreadHandle     m_pRenderThread;

	COSWinVomePlayer *	m_pEngine;

	int					m_nStatus;

	VOOSMP_AUDIO_FORMAT	m_sAudioFormat;

	VOMP_BUFFERTYPE *	m_pAudioBuffer;

	unsigned char*		m_pBuffer;
	unsigned int		m_uRenderSize;

	int					m_nRenderNum;
	bool					m_bNewRenderStart;
};

#endif //#define __OnStreamCAudioRender_h__

