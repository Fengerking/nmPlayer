/*
 *  CMuxStream.h
 *
 *  Created by Lin Jun on 19/03/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _CMuxStream_H__
#define _CMuxStream_H__

#include "voNPWrapType.h"
#include "CDllLoad.h"
#include "vo_mem_stream.h"
#include "voSink.h"


class CMuxStream
{
public:
	CMuxStream(VONP_LIB_FUNC* pLibOP, VONP_AUDIO_FORMAT* pAudioFormat, VO_TCHAR * playerPath);
	virtual ~CMuxStream(void);
	
public:
	int Init();
	int Uninit();
	int Run();
	int Stop();
	int SendTsStream(VONP_BUFFERTYPE* pBuffer);
	int SendRawFrame(VO_SINK_SAMPLE* pSample, bool bForce=false);
	int ReadMuxBuffer(char* pBuffer, int* pWantReadSize);
	int Flush();
	void SetLogPrintfCB(VO_LOG_PRINT_CB* pLogPrintCallback){m_pLogPrintCallback = pLogPrintCallback;}
	int ForceOutputWholeStream();
	void SetAudioOnly(bool bAudioOnly){m_bAudioOnly = bAudioOnly;}
  void SetVideoOnly(bool bVideoOnly){m_bVideoOnly = bVideoOnly;}

  int SendHeadData(VO_SINK_SAMPLE* pSample);
  void ForceWritePATPMT(void);
    
public:
	int		GetMemStreamSize();
	void	SetMaxMemStreamSize(int nMaxSize);
	bool	OnCodecTypeChanged(bool bAudio, int nCodecType);
	bool	OnAudioFormatChanged(VONP_AUDIO_FORMAT* pFormat);
	int     GetCodecType(bool bAudio){return bAudio?m_nAudioCodecType:m_nVideoCodecType;}
	int     SetCacheDuration(int nDuration);
	void	getLastSampleTime(int * pnLastAudioTime, int * pnLastVideoTime);
    
protected:
	int		OpenTsMux();
	int		CloseTsMux();
	void	FlushInternal();
    void    UpdateTSInfo();
	
private:
	int LoadTsMuxDll (void);
	int FreeTsMuxDll (void);
	
	static VO_PTR VO_API Open(VO_FILE_SOURCE * pSource);
	static VO_S32 VO_API Read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);
	static VO_S32 VO_API Write(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);
	static VO_S32 VO_API Flush(VO_PTR pFile);
	static VO_S64 VO_API Seek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag);
	static VO_S64 VO_API Size(VO_PTR pFile);
	static VO_S64 VO_API Save(VO_PTR pFile);
	static VO_S32 VO_API Close(VO_PTR pFile);
	
	//static vo_mem_stream* GetMemStream(){return &m_MuxStream;};
	
private:
	int					m_nMaxMemStreamSize;
	bool				m_bRunning;
	
	void *				m_hDll;
	VONP_LIB_FUNC*      m_pLibFunc;
	
	VO_PTR				m_hMuxer;
	VO_SINK_WRITEAPI	m_MuxerAPI;
	
	VO_FILE_OPERATOR	m_FileOP;
	voCMutex			m_mutexRW;
	
	int					m_nAudioCodecType;
	int					m_nVideoCodecType;
	
	bool				m_bInitPCMInfo;
	
	VONP_AUDIO_FORMAT   m_AudioFormat;
	VO_LOG_PRINT_CB*	m_pLogPrintCallback;
	bool				m_bAudioOnly;
    bool                m_bVideoOnly;

	int					m_nLastAudioTime;
	int					m_nLastVideoTime;
  VO_TCHAR *  m_pPlayerPath;
};


#endif //_CMuxStream_H_
