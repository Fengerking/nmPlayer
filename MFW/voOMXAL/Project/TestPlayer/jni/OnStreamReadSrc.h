#ifndef __OnStreamSrc_H__
#define __OnStreamSrc_H__

#include <stdio.h>
#include <string.h>
#include "osCMutex.h"
#include "voOnStreamType.h"

#define PLAY_VIDEO_ONLY		0
#define PLAY_AUDIO_ONLY		1
#define	AV_FILE_PATH		2
#define VIDEO_FRAME_RATE	3

class OnStreamReadSrc
{
public:
	// Used to control the image drawing
	OnStreamReadSrc ();
	virtual ~OnStreamReadSrc (void);

	virtual int			Init(void * pSource, int nFlag);
	virtual int			Uninit(void);
	virtual int 		Open(void);
	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual int 		Stop (void);
	virtual int 		Close (void);
	virtual int 		Flush (void);
	virtual int 		GetDuration (int * pDuration);
	virtual int 		SetCurPos (int nCurPos);
	virtual int 		GetParam (int nID, void * pValue);
	virtual int 		SetParam (int nID, void * pValue);
	virtual void*		GetReadBufPtr();

	static int			voSrcReadAudio(void * pUserData, VOOSMP_BUFFERTYPE * pBuffer);
	static int			voSrcReadVideo(void * pUserData, VOOSMP_BUFFERTYPE * pBuffer);
	
protected:
	virtual int			onReadAudio(VOOSMP_BUFFERTYPE * pBuffer);
	virtual int			onReadVideo(VOOSMP_BUFFERTYPE * pBuffer);

	int 				FillAudioBuffer();
	int 				FillVideoBuffer();

	int					GetAACFrame();
	int					GetH264Frame();

	int					GetAVDuration();

	VOOSMP_READBUFFER_FUNC  m_fReadBuf;

	FILE*					m_fH264;
	unsigned char*			m_pVideoBuffer;
	int						m_nVideoUseLength;
	int						m_nVideoFilledlen;
	int						m_nVideoFileLeft;
	int						m_nVideoFileLength;

	int						m_nVideoFameNum;

	FILE*					m_fAAC;
	unsigned char*			m_pAudioBuffer;
	int						m_nAudioUseLength;
	int						m_nAudioFilledlen;
	int						m_nAudioFileLeft;
	int						m_nAudioFileLength;

	int						m_nSampleIndex;
	int						m_nAudioSampleRate;
	int						m_nAudioFameNum;

	int						m_nAudioRun;
	int						m_nVideoRun;
	int						m_nAudioCodec;
	int						m_nVideoCodec;
	int						m_nOnlyAudio;
	int						m_nOnlyVideo;
	int						m_nAudioEOS;
	int						m_nVideoEOS;

	int						m_nVideoFrameRate;
	long long				m_nVideoFameTime;

	char					m_sFilePath[1024];

	int						m_nStauts;

	int						m_nDuration;

	osCMutex				m_MutexAudioRead;
	osCMutex				m_MutexVideoRead;
};

#endif // __OnStreamEngine_H__
