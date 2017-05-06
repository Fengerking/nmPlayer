/*
 *  CMuxStream.cpp
 *
 *  Created by Lin Jun on 19/03/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CMuxStream.h"
#include "voLog.h"
#include "voOSFunc.h"

vo_mem_stream		m_MuxStream;
typedef VO_S32 (*GETTSWRITERFUNCSET)(VO_SINK_WRITEAPI* pReadHandle, VO_U32 uFlag);


CMuxStream::CMuxStream(VONP_LIB_FUNC* pLibOP, VONP_AUDIO_FORMAT* pAudioFormat, VO_TCHAR * playerPath)
:m_nMaxMemStreamSize(1024*512)
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
,m_nLastAudioTime(0)
,m_nLastVideoTime(0)
,m_pPlayerPath(playerPath)
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

CMuxStream::~CMuxStream(void)
{
	Uninit();
}

int CMuxStream::Init()
{
	Uninit();
	
	LoadTsMuxDll();
	
	int nRet = m_MuxStream.open();
	nRet = OpenTsMux();

	m_nLastAudioTime = 0;
	m_nLastVideoTime = 0;
	
	return nRet;
}

int CMuxStream::Uninit()
{
	m_MuxStream.close();
	
	CloseTsMux();
	
	FreeTsMuxDll();

	return VONP_ERR_None;
}

int CMuxStream::Run()
{
	m_bRunning = true;
	return VONP_ERR_None;
}

int CMuxStream::Stop()
{
	m_bRunning = false;
	return VONP_ERR_None;
}

int CMuxStream::OpenTsMux()
{
	if(!m_hDll)
		return VONP_ERR_Pointer;
	if(m_hMuxer)
		return VONP_ERR_None;
	
	VO_FILE_SOURCE src;
	memset(&src, 0, sizeof(VO_FILE_SOURCE));
	src.nFlag	= VO_FILE_TYPE_NAME;
	src.nMode	= VO_FILE_READ_WRITE;
	src.pSource	= (void *)_T("Writer.ts");
	src.nOffset	= 0;
	src.nLength	= 0;
	
	VO_SINK_OPENPARAM param;
	memset(&param, 0, sizeof(VO_SINK_OPENPARAM));
	
	param.nFlag			= 0;
	param.pSinkOP		= &m_FileOP;
	param.pMemOP		= NULL;
  param.strWorkPath = m_pPlayerPath;

	//param.nAudioCoding	= m_nAudioCodecType;
    if(m_bVideoOnly)
		param.nAudioCoding	= VO_AUDIO_CodingUnused;
	else
		param.nAudioCoding	= m_nAudioCodecType;

	if(m_bAudioOnly)
		param.nVideoCoding	= VO_VIDEO_CodingUnused;
	else
		param.nVideoCoding	= m_nVideoCodecType;
	
	VOLOGI("[OMXAL]TS mux codec: audio %d, video %d", param.nAudioCoding, param.nVideoCoding);
	
	int nRet = m_MuxerAPI.Open(&m_hMuxer, &src, &param);
	if(m_pLogPrintCallback)
	   m_MuxerAPI.SetParam(m_hMuxer, VO_PID_COMMON_LOGFUNC, m_pLogPrintCallback);
	
	m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_DISABLEAUDIOSTEPSIZE, NULL);

	if(nRet == 0 && VO_AUDIO_CodingPCM == m_nAudioCodecType)
	{
		VOLOGI("[OMXAL]Send PCM info sample, %d %d %d", m_AudioFormat.nChannels, m_AudioFormat.nSampleBits, m_AudioFormat.nSampleRate);
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

int CMuxStream::CloseTsMux()
{
	if(m_hMuxer)
	{
		m_MuxerAPI.Close(m_hMuxer);
		m_hMuxer = NULL;
	}
	
	return VONP_ERR_None;
}

//this is sending raw frame,i.e. H264/AAC
int CMuxStream::SendRawFrame(VO_SINK_SAMPLE* pSample, bool bForce)
{
	if((m_MuxStream.size() > m_nMaxMemStreamSize) && !bForce)
	{
//		VOLOGI("[OMXAL]Mem stream is full size %lld, max %d", m_MuxStream.size(), m_nMaxMemStreamSize);
		return VONP_ERR_Retry;
	}
	
	if(!m_hMuxer)
		return VONP_ERR_Pointer;
	
	if(pSample->Size <= 0 || !pSample->Buffer)
	{
		VOLOGW("[OMXAL]buffer size is 0");
		return VONP_ERR_Unknown;
	}
	
//	VOLOGI("[OMXAL]send %s raw frame size %d, time %lld", pSample->nAV ? "video" : "audio", pSample->Size, pSample->Time);
	
	voCAutoLock lock(&m_mutexRW);

	if(pSample->nAV)
		m_nLastVideoTime = (int)pSample->Time;
	else
		m_nLastAudioTime = (int)pSample->Time;

	return m_MuxerAPI.AddSample(m_hMuxer, pSample);
}

int CMuxStream::SendHeadData(VO_SINK_SAMPLE* pSample)
{
  if(m_hMuxer)
  {
    VOLOGI("[OMXAL]Update %s head data, %lu", pSample->nAV==0?"audio":"video", pSample->Size);

    VO_SINK_EXTENSION headdata;
    memset(&headdata, 0, sizeof(VO_SINK_EXTENSION)); 
    headdata.Buffer = pSample->Buffer;
    headdata.Size   = pSample->Size;
    headdata.nAV    = pSample->nAV;

    return m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_EXTDATA, &headdata);
  }

  return VONP_ERR_Pointer;
}

bool CMuxStream::OnAudioFormatChanged(VONP_AUDIO_FORMAT* pFormat)
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

//this is ts buffer sending in
int CMuxStream::SendTsStream(VONP_BUFFERTYPE* pBuffer)
{
	if(m_MuxStream.size() > 0)
	{		
		//VOLOGI("[OMXAL]Mem stream is full %dM", m_MuxStream.size()/(1024*1024));
		return VONP_ERR_Retry;
	}
	
	//direct write TS to mem stream
	voCAutoLock lock(&m_mutexRW);
	m_MuxStream.append(pBuffer->pBuffer, pBuffer->nSize);
	
	return VONP_ERR_None;
}

int CMuxStream::ReadMuxBuffer(char* pBuffer, int* pWantReadSize)
{
	voCAutoLock lock(&m_mutexRW);
	
	int nSize		= *pWantReadSize;
	*pWantReadSize	= m_MuxStream.read((unsigned char*)pBuffer, nSize);
	
	if(*pWantReadSize <= 0)
	{
		*pWantReadSize = 0;
		
//		VOLOGI("[OMXAL]Read from mem stream is NULL %lld", m_MuxStream.size());
		return VONP_ERR_Retry;
	}

//	VOLOGI("[OMXAL]Read mux buffer %d", *pWantReadSize);	
	return VONP_ERR_None;
}

int CMuxStream::Flush()
{
	voCAutoLock lock(&m_mutexRW);

	if(m_hMuxer)
	{
		VOLOGI("[OMXAL]Flush ts mux");
		m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_FLUSH, NULL);
	}
	
    VOLOGI("[OMXAL]+Flush mem stream");
	m_MuxStream.close();
	m_MuxStream.open();
    VOLOGI("[OMXAL]-Flush mem stream");
    
//	ForceWritePATPMT();

    UpdateTSInfo();
    
	m_bInitPCMInfo = false;
    
	return VONP_ERR_None;
}

void CMuxStream::ForceWritePATPMT()
{
	voCAutoLock lock(&m_mutexRW);  

	if(m_hMuxer)
	{  
		//VOLOGI("[NPW]Rewrite PAT PMT");
		m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_ASSEMBLE_PAT_PMT, NULL);
	}  
}

int CMuxStream::ForceOutputWholeStream()
{
	voCAutoLock lock(&m_mutexRW);
	
	if(m_hMuxer)
	{
		VOLOGI("[OMXAL]Force output whole stream");
		m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_FLUSH, NULL);
	}
	
	return VONP_ERR_None;
}

void CMuxStream::FlushInternal()
{
	voCAutoLock lock(&m_mutexRW);
	
	Uninit();
	Init();
}

void CMuxStream::SetMaxMemStreamSize(int nMaxSize)
{
	m_nMaxMemStreamSize = nMaxSize;
}

bool CMuxStream::OnCodecTypeChanged(bool bAudio, int nCodecType)
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
			VOLOGI("[OMXAL]New format -> Found audio codec %x", nCodecType);
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
			VOLOGI("[OMXAL]New format -> Found video codec %x", nCodecType);
			m_nVideoCodecType = nVideoCodec;
			//FlushInternal();
		}
	}
	
	return true; 
}

int CMuxStream::GetMemStreamSize()
{
	return m_MuxStream.size();
}


int CMuxStream::FreeTsMuxDll (void)
{
	//VOLOGI("[OMXAL]Free TS mux lib");
	if(m_hDll != NULL && m_pLibFunc != NULL)
	{
		m_pLibFunc->FreeLib (m_pLibFunc->pUserData, m_hDll, 0);
		memset(&m_MuxerAPI, 0, sizeof(m_MuxerAPI));
		m_hDll = NULL;
	}
	
	return VONP_ERR_None;
}

int CMuxStream::LoadTsMuxDll (void)
{
	//VOLOGI("[OMXAL]+Load TS mux, lib op %x", (unsigned int)m_pLibFunc);
	
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
	//VOLOGI("[OMXAL]Load TS mux lib ok");
	
	GETTSWRITERFUNCSET	pGetFuncSet = NULL;
	pGetFuncSet	= (GETTSWRITERFUNCSET)m_pLibFunc->GetAddress(m_pLibFunc->pUserData, m_hDll, (char*)"voGetTSWriterAPI", 0);
	
	if(pGetFuncSet == NULL)
	{
		VOLOGE("[OMXAL]Get TS mux func set failed");
		return VONP_ERR_Pointer;
	}
	
	pGetFuncSet(&m_MuxerAPI, 0);
	
	if (m_MuxerAPI.Open == 0)
		return VONP_ERR_Pointer;
	
	return VONP_ERR_None;
}

VO_PTR VO_API CMuxStream::Open(VO_FILE_SOURCE * pSource)
{
    return &m_MuxStream;
}

VO_S32 VO_API CMuxStream::Read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
    vo_mem_stream * ptr_stream = ( vo_mem_stream * )pFile;
    return ptr_stream->read( (VO_PBYTE)pBuffer , uSize );
}

VO_S32 VO_API CMuxStream::Write(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize)
{
	// dump info
	static int nWrite = 0;
	
	// check file exist
	if(nWrite == 0)
	{
		VO_S32 nCheckBackDoor = voOS_EnableDebugMode(1);
		
		if(nCheckBackDoor > 0)
		{
			FILE* hCheck = fopen("/data/local/OMXALDUMP/ts_internal.dat", "rb");
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
		//VOLOGI("[OMXAL]write ts stream %d", (int)uSize);
		static FILE* hFile = fopen("/data/local/OMXALDUMP/ts_internal.ts", "wb");
		
		if(!hFile)
			VOLOGI("[OMXAL]ts dump file failed. %s", "/data/local/OMXALDUMP/ts_internal.ts");
		
		if(pBuffer && (uSize>0) && hFile)
		{            
			//VOLOGI("[OMXAL]write %d", uSize);
			fwrite(pBuffer, 1, uSize, hFile);
		}		
	}
	// dump info end
	
    //VOLOGI("[OMXAL]Write stream %d", uSize);
    vo_mem_stream * ptr_stream = ( vo_mem_stream * )pFile;
    return ptr_stream->append( (VO_PBYTE)pBuffer , uSize );
}

VO_S32 VO_API CMuxStream::Flush(VO_PTR pFile)
{
    return 0;
}

VO_S64 VO_API CMuxStream::Seek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag)
{
    return 0;
}

VO_S64 VO_API CMuxStream::Size(VO_PTR pFile)
{
    return 0;
}

VO_S64 VO_API CMuxStream::Save(VO_PTR pFile)
{
    return 0;
}

VO_S32 VO_API CMuxStream::Close(VO_PTR pFile)
{
    vo_mem_stream * ptr_stream = ( vo_mem_stream * )pFile;
    ptr_stream->write_eos();
    return 0;
}

int CMuxStream::SetCacheDuration(int nDuration)
{
    if(m_hMuxer)
	{
		return m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_AV_SYNC_BUFFER_DURATION, &nDuration);
	}
    
    return VONP_ERR_None;
}

void CMuxStream::UpdateTSInfo()
{
  	voCAutoLock lock(&m_mutexRW);
	
	if(m_hMuxer)
	{
		VOLOGI("[OMXAL]Rewrite PAT PMT");
		m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_FORMAT_CHANGE, NULL);
    //m_MuxerAPI.SetParam(m_hMuxer, VO_PID_SINK_ASSEMBLE_PAT_PMT, NULL);
	}
}

void CMuxStream::getLastSampleTime(int * pnLastAudioTime, int * pnLastVideoTime)
{
	voCAutoLock lock(&m_mutexRW);

	if(pnLastAudioTime)
		*pnLastAudioTime = m_nLastAudioTime;

	if(pnLastVideoTime)
		*pnLastVideoTime = m_nLastVideoTime;
}
