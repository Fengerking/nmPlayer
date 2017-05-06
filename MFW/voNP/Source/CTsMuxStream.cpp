/*
 *  CTsMuxStream.cpp
 *
 *  Created by Lin Jun on 19/03/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CTsMuxStream.h"
#include "voLog.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


typedef VO_S32 (*GETTSWRITERFUNCSET)(VO_SINK_WRITEAPI* pReadHandle, VO_U32 uFlag);


CTsMuxStream::CTsMuxStream(VONP_LIB_FUNC* pLibOP, VONP_AUDIO_FORMAT* pAudioFormat)
:CBaseMuxStream(pLibOP, pAudioFormat)
,m_nMaxMemStreamSize(1024*512)
,m_bRunning(false)
,m_hDll(NULL)
,m_pLibFunc(pLibOP)
,m_hMuxer(NULL)
,m_nAudioCodecType(VO_AUDIO_CodingAAC)//VO_AUDIO_CodingPCM
,m_nVideoCodecType(VO_VIDEO_CodingH264)
,m_bInitPCMInfo(false)
,m_pLogPrintCallback(NULL)
,m_bAudioOnly(false)
,m_bVideoOnly(false)
,m_nFirstTimeStamp(-1)
,m_nLastTimeStamp(-1)
,m_nFirstTimeStampAudio(-1)
,m_nLastTimeStampAudio(-1)
,m_nFirstTimeStampVideo(-1)
,m_nLastTimeStampVideo(-1)
{
	memset(&m_MuxerAPI, 0, sizeof(VO_SINK_WRITEAPI));
	memset(&m_FileOP, 0, sizeof(VO_FILE_OPERATOR));
	
	m_AudioFormat.nChannels		= pAudioFormat->nChannels;
	m_AudioFormat.nSampleRate	= pAudioFormat->nSampleRate;
	m_AudioFormat.nSampleBits	= pAudioFormat->nSampleBits;
	
	m_FileOP.Open	= Open;
	m_FileOP.Close	= Close;
	m_FileOP.Flush	= Flush;
	m_FileOP.Read	= Read;
	m_FileOP.Save	= Save;
	m_FileOP.Seek	= Seek;
	m_FileOP.Size	= Size;
	m_FileOP.Write	= Write;	
}

CTsMuxStream::~CTsMuxStream(void)
{
	Uninit();
}

int CTsMuxStream::Init()
{
	Uninit();
	
	LoadTsMuxDll();
	
	int nRet = 	m_MuxStream.open();
	
	nRet = OpenTsMux();
	
	return nRet;
}

int CTsMuxStream::Uninit()
{
	m_MuxStream.close();
	
	CloseTsMux();
	
	FreeTsMuxDll();
    
    CTsMuxStream::Write(NULL, NULL, 0);

	return VONP_ERR_None;
}

int CTsMuxStream::Run()
{
	m_bRunning = true;
	return VONP_ERR_None;
}

int CTsMuxStream::Stop()
{
	m_bRunning = false;
	return VONP_ERR_None;
}

int CTsMuxStream::OpenTsMux()
{
#ifndef _IOS
	if(!m_hDll)
		return VONP_ERR_Pointer;
#endif
    
	if(m_hMuxer)
		return VONP_ERR_None;
	
	VO_FILE_SOURCE src;
	memset(&src, 0, sizeof(VO_FILE_SOURCE));
	src.nFlag	= VO_FILE_TYPE_NAME;
	src.nMode	= VO_FILE_READ_WRITE;
	src.pSource	= (void *)this;
	src.nOffset	= 0;
	src.nLength	= 0;
	
	VO_SINK_OPENPARAM param;
	memset(&param, 0, sizeof(VO_SINK_OPENPARAM));
	
	param.nFlag			= 0;
	param.pSinkOP		= &m_FileOP;
	param.pMemOP		= NULL;

    // right now not process pure video
	param.nAudioCoding	= m_nAudioCodecType;
//    if(m_bVideoOnly)
//		param.nAudioCoding	= VO_AUDIO_CodingUnused;
//	else
//		param.nAudioCoding	= m_nAudioCodecType;

	if(m_bAudioOnly)
		param.nVideoCoding	= VO_VIDEO_CodingUnused;
	else
		param.nVideoCoding	= m_nVideoCodecType;
	
	VOLOGI("[NPW]TS mux codec: audio %lu, video %lu", param.nAudioCoding, param.nVideoCoding);
	
	int nRet = m_MuxerAPI.Open(&m_hMuxer, &src, &param);
	if(m_pLogPrintCallback)
	   m_MuxerAPI.SetParam(m_hMuxer, VO_PID_COMMON_LOGFUNC, m_pLogPrintCallback);
    
    m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_INPUT_PTS, NULL);
	
	if(nRet == 0 && VO_AUDIO_CodingPCM == m_nAudioCodecType)
	{
		VOLOGI("[NPW]Send PCM info sample, %d %d %d", m_AudioFormat.nChannels, m_AudioFormat.nSampleBits, m_AudioFormat.nSampleRate);
		VO_SINK_MEDIA_INFO PCMInfo;
		
		PCMInfo.AudioFormat.nChannels		= m_AudioFormat.nChannels;
		PCMInfo.AudioFormat.nSample_bits	= m_AudioFormat.nSampleBits;
		PCMInfo.AudioFormat.nSample_rate	= m_AudioFormat.nSampleRate;
		
		VO_SINK_SAMPLE sample;
		memset(&sample, 0, sizeof(VO_SINK_SAMPLE));
		sample.Time		= 0;
		sample.Buffer	=(VO_PBYTE)&PCMInfo;
		sample.Size		= sizeof(PCMInfo);
		sample.nAV		= 0;
		sample.DTS		= -1;
		nRet = m_MuxerAPI.AddSample(m_hMuxer, &sample);
	}	
	
	return nRet;
}

int CTsMuxStream::CloseTsMux()
{
	if(m_hMuxer)
	{
		m_MuxerAPI.Close(m_hMuxer);
		m_hMuxer = NULL;
	}
	
	return VONP_ERR_None;
}

void CTsMuxStream::UpdateTimestamp(VO_SINK_SAMPLE* pSample, long long llTime)
{
    if(pSample == NULL)
    {
        //reset
        m_nFirstTimeStamp       = -1;
        m_nLastTimeStamp        = -1;
        
        m_nFirstTimeStampAudio  = -1;
        m_nLastTimeStampAudio   = -1;

        m_nFirstTimeStampVideo  = -1;
        m_nLastTimeStampVideo   = -1;
        
        return;
    }
    
    if(m_nFirstTimeStamp == -1)
        m_nFirstTimeStamp = llTime;
    m_nLastTimeStamp = llTime;
    
    if(pSample->nAV == 0)
    {
        if(m_nFirstTimeStampAudio == -1)
            m_nFirstTimeStampAudio = llTime;
        
        m_nLastTimeStampAudio = llTime;
    }
    
    if(pSample->nAV == 1)
    {
        if(m_nFirstTimeStampVideo == -1)
            m_nFirstTimeStampVideo = llTime;
        
        m_nLastTimeStampVideo = llTime;
    }
}

//this is sending raw frame,i.e. H264/AAC
int CTsMuxStream::SendRawFrame(VO_SINK_SAMPLE* pSample, long long llOrgTime, bool bForce)
{
	if( (m_MuxStream.size() > m_nMaxMemStreamSize) && !bForce)
	{		
		VOLOGI("[NPW]Mem stream is full %dK", (int)m_MuxStream.size()/(1024));
		return VONP_ERR_Retry;
	}
	
	if(!m_hMuxer)
		return VONP_ERR_Pointer;
	
	if(pSample->Size <= 0 || !pSample->Buffer)
	{
		VOLOGI("[NPW]buffer size is 0");
		return VONP_ERR_Unknown;
	}
    
    UpdateTimestamp(pSample, llOrgTime);
    
	//if(pSample->nAV==1)
		//VOLOGI("[NPW]Recv %s frame size %06d, pointer %x, time %06lld", pSample->nAV==0?"audio":"video", (unsigned int)pSample->Size, (unsigned int)pSample->Buffer, pSample->Time);
	
	voCAutoLock lock(&m_mutexRW);
	return m_MuxerAPI.AddSample(m_hMuxer, pSample);
}

int CTsMuxStream::SendHeadData(VO_SINK_SAMPLE* pSample)
{
	if(m_hMuxer)
	{
		VOLOGI("[NPW]Update %s head data, %lu", pSample->nAV==0?"audio":"video", pSample->Size);
        
        VO_SINK_EXTENSION headdata;
        memset(&headdata, 0, sizeof(VO_SINK_EXTENSION));
        headdata.Buffer = pSample->Buffer;
        headdata.Size   = pSample->Size;
        headdata.nAV    = pSample->nAV;
        
		return m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_EXTDATA, &headdata);
	}

    return VONP_ERR_Pointer;
}

bool CTsMuxStream::OnAudioFormatChanged(VONP_AUDIO_FORMAT* pFormat)
{
	voCAutoLock lock(&m_mutexRW);
	
	if(pFormat->nChannels != m_AudioFormat.nChannels
	   || pFormat->nSampleBits != m_AudioFormat.nSampleBits
	   || pFormat->nSampleRate != m_AudioFormat.nSampleRate)
	{
		m_AudioFormat.nChannels		= pFormat->nChannels;
		m_AudioFormat.nSampleBits	= pFormat->nSampleBits;
		m_AudioFormat.nSampleRate	= pFormat->nSampleRate;
		FlushInternal();
		return true;
	}
	
	return false;
}

//this is raw buffer sending in
int CTsMuxStream::SendRawStream(VONP_BUFFERTYPE* pBuffer)
{
	if(m_MuxStream.size() > 0)
	{		
		//VOLOGI("[NPW]Mem stream is full %dM", m_MuxStream.size()/(1024*1024));
		return VONP_ERR_Retry;
	}
	
	//direct write TS to mem stream
	voCAutoLock lock(&m_mutexRW);
	m_MuxStream.append(pBuffer->pBuffer, pBuffer->nSize);
	
	return VONP_ERR_None;
}

int CTsMuxStream::ReadMuxBuffer(char* pBuffer, int* pWantReadSize)
{
	voCAutoLock lock(&m_mutexRW);
	
	int nSize		= *pWantReadSize;
	*pWantReadSize	= m_MuxStream.read((unsigned char*)pBuffer, nSize);
	
	if(*pWantReadSize <= 0)
	{
		*pWantReadSize = 0;
		
		VOLOGW("[NPW]Read from mem stream is NULL");
		
		//if(m_MuxStream.size() <= 0)
			return VONP_ERR_Retry;
	}

	//VOLOGI("[NPW]Read mux buffer %d", *pWantReadSize);	
	return VONP_ERR_None;
}

int CTsMuxStream::Flush()
{
	voCAutoLock lock(&m_mutexRW);

	if(m_hMuxer)
	{
		VOLOGI("[NPW]Flush ts mux");
		m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_FLUSH, NULL);
	}
	
    VOLOGI("[NPW]+Flush mem stream");
	m_MuxStream.close();
	m_MuxStream.open();
    VOLOGI("[NPW]-Flush mem stream");
    
    ForceWritePATPMT();
    
	m_bInitPCMInfo = false;
    
    UpdateTimestamp(NULL, -1);
    
	return VONP_ERR_None;
}

int CTsMuxStream::ForceOutputWholeStream()
{
	voCAutoLock lock(&m_mutexRW);
	
	if(m_hMuxer)
	{
		//VOLOGI("[NPW]Force output whole stream");
		m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_FLUSH, NULL);
	}
    
	return VONP_ERR_None;
}

void CTsMuxStream::FlushInternal()
{
	voCAutoLock lock(&m_mutexRW);
	
	Uninit();
	Init();
}

void CTsMuxStream::SetMaxMemStreamSize(int nMaxSize)
{
	m_nMaxMemStreamSize = nMaxSize;
}

bool CTsMuxStream::OnCodecTypeChanged(bool bAudio, int nCodecType)
{	
	if(bAudio)
	{
		int nAudioCodec = VO_AUDIO_CodingAAC;
		
		if(VONP_AUDIO_CodingAAC == nCodecType)
		{
			nAudioCodec = VO_AUDIO_CodingAAC;
		}
		else if(VONP_AUDIO_CodingPCM == nCodecType)
		{
			nAudioCodec = VO_AUDIO_CodingPCM;
		}
		else if(VONP_AUDIO_CodingMP3 == nCodecType)
		{
			nAudioCodec = VO_AUDIO_CodingMP3;
		}
		else
		{
			//not support codec
			return false;
		}
		
		if(nCodecType != m_nAudioCodecType)
		{
			VOLOGI("[NPW]New format -> Found audio codec %x", nCodecType);
			m_nAudioCodecType = nAudioCodec;
			//FlushInternal();
		}
	}
	else
	{
		// currently only support H264 video
		int nVideoCodec = VO_VIDEO_CodingH264 ;
		if(VONP_VIDEO_CodingH264 == nCodecType)
		{
			nVideoCodec = VO_VIDEO_CodingH264;
		}
		else
		{
			//not support codec
			return false;
		}
		
		if(m_nVideoCodecType != nVideoCodec)
		{
			VOLOGI("[NPW]New format -> Found video codec %x", nCodecType);
			m_nVideoCodecType = nVideoCodec;
			//FlushInternal();
		}
	}
	
	return true; 
}

int CTsMuxStream::GetMemStreamSize()
{
	return m_MuxStream.size();
}


int CTsMuxStream::FreeTsMuxDll (void)
{
	//VOLOGI("[NPW]Free TS mux lib");
	if(m_hDll != NULL && m_pLibFunc != NULL)
	{
		m_pLibFunc->FreeLib (m_pLibFunc->pUserData, m_hDll, 0);
		memset(&m_MuxerAPI, 0, sizeof(m_MuxerAPI));
		m_hDll = NULL;
	}
	
	return VONP_ERR_None;
}

int CTsMuxStream::LoadTsMuxDll (void)
{
	//VOLOGI("[NPW]+Load TS mux, lib op %x", (unsigned int)m_pLibFunc);
    
	GETTSWRITERFUNCSET	pGetFuncSet = NULL;
    
#ifdef _IOS
    pGetFuncSet = voGetTSWriterAPI;
#else

	if(!m_pLibFunc)
		return VONP_ERR_Pointer;
	
	if (m_hDll != 0) 
	{
		m_pLibFunc->FreeLib (m_pLibFunc->pUserData, m_hDll, 0);
		memset(&m_MuxerAPI, 0, sizeof(m_MuxerAPI));
	}
	
	m_hDll = m_pLibFunc->LoadLib(m_pLibFunc->pUserData, (char*)"voTsFW" , 0 );
	if (!m_hDll) 
	{
		return VONP_ERR_Unknown;
	}
	
    pGetFuncSet	= (GETTSWRITERFUNCSET)m_pLibFunc->GetAddress(m_pLibFunc->pUserData, m_hDll, (char*)"voGetTSWriterAPI", 0);
#endif
	
	if(pGetFuncSet == NULL)
	{
		VOLOGE("[NPW]Get TS mux func set failed");
		return VONP_ERR_Pointer;
	}
	
	pGetFuncSet(&m_MuxerAPI, 0);
	
	if (m_MuxerAPI.Open == 0)
		return VONP_ERR_Pointer;
    
    VOLOGI("[NPW]Load TS mux lib ok");
	
	return VONP_ERR_None;
}

VO_S32 CTsMuxStream::doRead(VO_PTR pBuffer, VO_U32 uSize)
{
    return m_MuxStream.read( (VO_PBYTE)pBuffer , uSize );
}

VO_S32 CTsMuxStream::doWrite(VO_PTR pBuffer, VO_U32 uSize)
{
    return m_MuxStream.append( (VO_PBYTE)pBuffer , uSize );
}

VO_PTR VO_API CTsMuxStream::Open(VO_FILE_SOURCE * pSource)
{
    return (VO_PTR)pSource->pSource;
}

VO_S32 VO_API CTsMuxStream::Read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
    CTsMuxStream * ptr_stream = ( CTsMuxStream * )pFile;
    return ptr_stream->doRead( (VO_PBYTE)pBuffer , uSize );
}

VO_S32 VO_API CTsMuxStream::Write(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
    int nRet = 0;
    
    //VOLOGI("[NPW]Write stream %u", uSize);
    CTsMuxStream * ptr_stream = ( CTsMuxStream * )pFile;
    
    if(ptr_stream)
    {
        nRet = ptr_stream->doWrite( (VO_PBYTE)pBuffer , uSize );
    }
    
	// dump info
#if defined (_IOS) || defined (_MAC_OS)
    static int nWrite = 0;
#else
    static int nWrite = 0;
#endif
	
	// check file exist
	if(nWrite == 0)
	{
		VO_S32 nCheckBackDoor = voOS_EnableDebugMode(1);
		
		if(nCheckBackDoor > 0)
		{
#if defined(_IOS) || defined(_MAC_OS)
            char szDir[1024];
            voOS_GetAppFolder(szDir, 1024);
            strcat(szDir, "voDebugFolder/");
            strcat(szDir, "ts_internal.dat");
            FILE* hCheck = fopen(szDir, "rb");
#else
			FILE* hCheck = fopen("/data/local/tmp/ts_internal.dat", "rb");
#endif
            
			if(hCheck)
			{
				nWrite = 1;
				fclose(hCheck);
				hCheck = NULL;
			}
			else
				nWrite = -1;
		}
		else
		{
			nWrite = -1;
		}
	}
	
	if(nWrite == 1)
	{
#if defined(_IOS) || defined(_MAC_OS)
        char* pszDump = NULL;
        char szTmp[1024];
        memset(szTmp, 0, 1024);
        voOS_GetAppFolder(szTmp, 1024);
        strcat(szTmp, "voDebugFolder/");
        strcat(szTmp, "ts_internal.ts");
        pszDump = szTmp;
#else
        char* pszDump = "/data/local/tmp/ts_internal.ts";
#endif
		static FILE* hFile = NULL;

        if(!hFile&&pBuffer!=NULL && uSize!=0)
        {
            hFile = fopen(pszDump, "wb");
        }

		if(!hFile)
			VOLOGI("[NPW]ts dump file failed. %s", pszDump);
        
        if(pBuffer==NULL && uSize==0)
        {
            if(hFile)
                fclose(hFile);
            hFile = NULL;
            return nRet;
        }
		
		if(pBuffer && (uSize>0) && hFile)
		{            
			//VOLOGI("[NPW]write %d", uSize);
			fwrite(pBuffer, 1, uSize, hFile);
		}		
	}
	// dump info end
	
    return nRet;
}

VO_S32 VO_API CTsMuxStream::Flush(VO_PTR pFile)
{
    return 0;
}

VO_S64 VO_API CTsMuxStream::Seek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag)
{
    return 0;
}

VO_S64 VO_API CTsMuxStream::Size(VO_PTR pFile)
{
    return 0;
}

VO_S64 VO_API CTsMuxStream::Save(VO_PTR pFile)
{
    return 0;
}

VO_S32 VO_API CTsMuxStream::Close(VO_PTR pFile)
{
    CTsMuxStream * ptr_stream = ( CTsMuxStream * )pFile;
    ptr_stream->doClose();
    return 0;
}

VO_S32 CTsMuxStream::doClose()
{
    m_MuxStream.write_eos();
    return 0;
}

int CTsMuxStream::SetCacheDuration(int nDuration)
{
    if(m_hMuxer)
	{
		return m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_AV_SYNC_BUFFER_DURATION, &nDuration);
	}
    
    return VONP_ERR_None;
}

void CTsMuxStream::ForceWritePATPMT()
{
  	voCAutoLock lock(&m_mutexRW);
	
	if(m_hMuxer)
	{
		//VOLOGI("[NPW]Rewrite PAT PMT");
		m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_ASSEMBLE_PAT_PMT, NULL);
	}
}


int CTsMuxStream::GetMemStreamDuration()
{
    if(m_nLastTimeStamp==-1 || m_nFirstTimeStamp==-1)
        return 0;
    
    int nDur = 0;
    
    if(m_bVideoOnly)
        nDur =  m_nLastTimeStampVideo - m_nFirstTimeStampVideo;
    else
        nDur =  m_nLastTimeStampAudio - m_nFirstTimeStampAudio;
    
    //nDur =  (m_nLastTimeStampVideo - m_nFirstTimeStampVideo)/90;
    
    //nDur = m_nLastTimeStamp - m_nFirstTimeStamp;
    //VOLOGI("Memory stream duration %d, A only %d, V only %d", nDur, m_bAudioOnly?1:0, m_bVideoOnly?1:0);
    
    if(nDur >= 1000)
    {
        //VOLOGI("[NPW]Mem duration %lld, A %lld, V %lld.(%lld %lld %lld %lld)", (m_nLastTimeStamp - m_nFirstTimeStamp), (m_nLastTimeStampAudio - m_nFirstTimeStampAudio), (m_nLastTimeStampVideo - m_nFirstTimeStampVideo),      m_nLastTimeStampAudio, m_nFirstTimeStampAudio, m_nLastTimeStampVideo, m_nFirstTimeStampVideo);
    }
    
    return nDur;
}


void CTsMuxStream::FlushMem()
{
    m_MuxStream.close();
	m_MuxStream.open();
}

void CTsMuxStream::ResetTimestamp()
{
    UpdateTimestamp(NULL, -1);
}

long long CTsMuxStream::GetLastTime()
{
    return m_bVideoOnly?m_nLastTimeStampVideo:m_nLastTimeStampAudio;
}

void CTsMuxStream::OnFormatChanged()
{
    voCAutoLock lock(&m_mutexRW);
	
	if(m_hMuxer)
	{
		VOLOGI("[NPW]Notify mux: format changed");
		m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_FORMAT_CHANGE, NULL);
	}
}



