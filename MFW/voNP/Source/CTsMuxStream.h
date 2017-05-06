/*
 *  CMuxStream.h
 *
 *  Created by Lin Jun on 19/03/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#ifndef _CTsMuxStream_H__
#define _CTsMuxStream_H__

#include "voNPWrapType.h"
#include "CDllLoad.h"
#include "CMemStream.h"
#include "voSink.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CBaseMuxStream
{
public:
	CBaseMuxStream(VONP_LIB_FUNC* pLibOP, VONP_AUDIO_FORMAT* pAudioFormat){};
	virtual ~CBaseMuxStream(void){};
	
public:
	virtual int Init()=0;
	virtual int Uninit()=0;
	virtual int Run()=0;
	virtual int Stop()=0;
	virtual int SendRawStream(VONP_BUFFERTYPE* pBuffer)=0;
	virtual int SendRawFrame(VO_SINK_SAMPLE* pSample, long long llOrgTime, bool bForce=false)=0;
	virtual int ReadMuxBuffer(char* pBuffer, int* pWantReadSize)=0;
	virtual int Flush()=0;
	virtual int ForceOutputWholeStream()=0;
    virtual int GetMemStreamDuration()=0;
    virtual int	GetMemStreamSize()=0;
    virtual int SetCacheDuration(int nDuration)=0;
    virtual void SetMaxMemStreamSize(int nMaxSize)=0;
    virtual void FlushMem()=0;
    virtual void SetAudioOnly(bool bAudioOnly)=0;
    virtual void SetVideoOnly(bool bVideoOnly)=0;
    virtual void ResetTimestamp()=0;
    virtual long long GetLastTime()=0;
    virtual void OnFormatChanged()=0;
    virtual int SendHeadData(VO_SINK_SAMPLE* pSample)=0;
};


class CTsMuxStream : public CBaseMuxStream
{
public:
	CTsMuxStream(VONP_LIB_FUNC* pLibOP, VONP_AUDIO_FORMAT* pAudioFormat);
	virtual ~CTsMuxStream(void);
	
public:
	virtual int Init();
	virtual int Uninit();
	virtual int Run();
	virtual int Stop();
	virtual int SendRawStream(VONP_BUFFERTYPE* pBuffer);
	virtual int SendRawFrame(VO_SINK_SAMPLE* pSample, long long llOrgTime, bool bForce=false);
	virtual int ReadMuxBuffer(char* pBuffer, int* pWantReadSize);
	virtual int Flush();
	virtual int ForceOutputWholeStream();
    virtual int GetMemStreamDuration();
    virtual int	GetMemStreamSize();
    virtual int SetCacheDuration(int nDuration);
    virtual void SetMaxMemStreamSize(int nMaxSize);
    virtual void FlushMem();
    virtual void ResetTimestamp();
	virtual long long GetLastTime();
    virtual void OnFormatChanged();
    virtual int SendHeadData(VO_SINK_SAMPLE* pSample);
    
    virtual void SetAudioOnly(bool bAudioOnly){m_bAudioOnly = bAudioOnly;};
    virtual void SetVideoOnly(bool bVideoOnly){m_bVideoOnly = bVideoOnly;};
    void SetLogPrintfCB(VO_LOG_PRINT_CB* pLogPrintCallback){m_pLogPrintCallback = pLogPrintCallback;};
    
public:
	bool	OnCodecTypeChanged(bool bAudio, int nCodecType);
	bool	OnAudioFormatChanged(VONP_AUDIO_FORMAT* pFormat);
	int     GetCodecType(bool bAudio){return bAudio?m_nAudioCodecType:m_nVideoCodecType;};
	void    ForceWritePATPMT();
    
protected:
	int		OpenTsMux();
	int		CloseTsMux();
	void	FlushInternal();
	void    UpdateTimestamp(VO_SINK_SAMPLE* pSample, long long llTime);
    
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
    
    VO_S32 doRead(VO_PTR pBuffer, VO_U32 uSize);
	VO_S32 doWrite(VO_PTR pBuffer, VO_U32 uSize);
    VO_S32 doClose();
	
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
    
    long long           m_nFirstTimeStamp;
    long long           m_nLastTimeStamp;
    long long           m_nFirstTimeStampAudio;
    long long           m_nLastTimeStampAudio;
    long long           m_nFirstTimeStampVideo;
    long long           m_nLastTimeStampVideo;
    
    CMemStream          m_MuxStream;
};

#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE

#endif //_CTsMuxStream_H__
