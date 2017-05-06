#include "voSource2WrapperPushPlay.h"
#include "voOSFunc.h"
#include "fMacros.h"
#include "voLog.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voSource2WrapperPushPlay::voSource2WrapperPushPlay(void)
: m_eStatePushPlay(PushPlay_Stop)
, m_ullTimeA(0)
, m_ullTimeV(0)
, m_ullCurTimeA(0)
, m_ullCurTimeV(0)
, m_uBufferTime(10000)
, m_bNeedBufing(VO_TRUE)
, m_PushPlayBuffer(this)
{
	memset(&m_SrcIOPush,0,sizeof(voSourceSwitch));
}

voSource2WrapperPushPlay::~voSource2WrapperPushPlay(void)
{
}

VO_U32 voSource2WrapperPushPlay::Init(VO_PTR pSource, VO_U32 nFlag, VO_SOURCE2_INITPARAM* pParam)
{
	m_nOpenFlag |= VO_SOURCE_OPENPARAM_FLAG_PUSHPLAY;

	if ( NULL != pParam &&
		(pParam->uFlag & VO_SOURCE2_FLAG_INIT_ACTUALFILESIZE) )
	{
		VO_SOURCE2_COMMONINITPARAM* p = static_cast<VO_SOURCE2_COMMONINITPARAM*>(pParam->pInitParam);

		m_ullActualFileSize = p->ullActualFileSize;
	}

	return voSource2ParserWrapper::Init(pSource, nFlag, pParam);
}

VO_U32 voSource2WrapperPushPlay::Open()
{
	if ( !(m_nFlag & VO_SOURCE2_FLAG_OPEN_ASYNC) )
	{
		VO_U32 uRet = voSource2ParserWrapper::Open();
		if (VO_RET_SOURCE2_OK != uRet) {
			VOLOGE("voSource2ParserWrapper::Open 0x%x", uRet);
			return uRet;
		}
	}

	SetStatus(PushPlay_Running);

	vo_thread::begin();

	return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2WrapperPushPlay::OnClose()
{
	VO_U32 uRet = 0;

	VO_SOURCE2_IO_API* OperIO2 = (VO_SOURCE2_IO_API*)m_SrcIOPush.oprIO2;
	if (OperIO2 && OperIO2->Close && m_SrcIOPush.phndIO2)
	{
		VO_U32 uRet = OperIO2->Close(m_SrcIOPush.phndIO2);
		m_SrcIOPush.phndIO2 = NULL;
		if (uRet)
		{
			VOLOGE("IO close 0x%x", uRet);
		}
	}

	OperIO2 = (VO_SOURCE2_IO_API*)m_SrcIO.oprIO2;
	if (OperIO2 && OperIO2->Close && m_SrcIO.phndIO2)
	{
		VO_U32 uRet = OperIO2->Close(m_SrcIO.phndIO2);
		m_SrcIO.phndIO2 = NULL;
		if (uRet)
		{
			VOLOGE("IO 1 close 0x%x", uRet);
		}
	}


	m_Event.Signal();
	
	m_PushPlayBuffer.SignalEvent();

	vo_thread::stop();

	m_PushPlayBuffer.StopThread();

	uRet = voSource2ParserWrapper::OnClose();
	if (VO_RET_SOURCE2_OK != uRet)
	{
		VOLOGE("voSource2ParserWrapper::Open 0x%x", uRet);
	}

	return uRet;
}

VO_U32 voSource2WrapperPushPlay::Start()
{
	SetStatus(PushPlay_Running);

	m_Event.Signal();

	m_PushPlayBuffer.SignalEvent();

	return voSource2ParserWrapper::Start();
}

VO_U32 voSource2WrapperPushPlay::Stop()
{
	SetStatus(PushPlay_Stop);

	return voSource2ParserWrapper::Stop();
}

VO_U32 voSource2WrapperPushPlay::OnGetFileLength(VO_U64 * pLength)
{
	CBaseReader* pReader = (CBaseReader*)m_Handle;
	double	percent = 0.;
	VO_U64	CurrentFileSize = 0;

	if( !pReader )
		return VO_RET_SOURCE2_EMPTYPOINTOR;
	
	VO_SOURCE2_IO_API* OperIO2 = (VO_SOURCE2_IO_API*)m_SrcIO.oprIO2;

	if (OperIO2 && OperIO2->GetSize && m_SrcIO.phndIO2){
		OperIO2->GetSize(m_SrcIO.phndIO2, &CurrentFileSize);
	}

	VO_SOURCE_INFO SourceInfo;
	memset( &SourceInfo , 0 , sizeof( VO_SOURCE_INFO ) );
	pReader->GetInfo( &SourceInfo );

	if(m_ullActualFileSize){
		percent = (double)CurrentFileSize/(double)m_ullActualFileSize;
	}

	*pLength = percent * SourceInfo.Duration;
	
	VOLOGI("CurrentFileSize=%llu, m_ullActualFileSize=%llu, Duration=%lu, pDuration=%llu", CurrentFileSize, m_ullActualFileSize, SourceInfo.Duration, *pLength);

	return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2WrapperPushPlay::Seek(VO_U64* pTimeStamp)
{
	VOLOGR("+Seek %lld", *pTimeStamp);
	VO_U64 ullSize = 0;
	VO_U32 uRet = 0;

	///<first we should reset the event to make sure the buffer thread will wait
	m_PushPlayBuffer.ResetEvent();
	///<set play status,it will let buffer thread to entry pause status
	SetStatus(PushPlay_Seeking);
	///<wait buffer thread to entry pause 
	while(!m_PushPlayBuffer.PauseThead())

	voCAutoLock lock(&m_lock);

	VO_SOURCE2_IO_API* OperIO2 = (VO_SOURCE2_IO_API*)m_SrcIO.oprIO2;
	if (OperIO2 && OperIO2->GetSize && m_SrcIO.phndIO2)
	{
		uRet = OperIO2->GetSize(m_SrcIO.phndIO2, &ullSize);
		if (uRet)
		{
			VOLOGE("GetSize %x", uRet);

			if (m_ptr_EventCallback &&
				(m_nFlag&VO_SOURCE2_FLAG_OPEN_ASYNC) &&
				m_ptr_EventCallback->SendEvent)
			{
				VOLOGI("CB Seek %lu, TimeStamp %llu", uRet, *pTimeStamp);		
				m_ptr_EventCallback->SendEvent(m_ptr_EventCallback->pUserData, VO_EVENTID_SOURCE2_SEEKCOMPLETE, (VO_U32)pTimeStamp, (VO_U32)&uRet);
			}

			return uRet;
		}
	}

	VO_FILE_MEDIATIMEFILEPOS s_timepos = {0};
	s_timepos.llFilePos = ullSize;
	VOLOGI("size %lld", s_timepos.llFilePos);

	uRet = GetParam(VO_PID_SOURCE_GETSEEKTIMEBYPOS, &s_timepos);
	if (uRet)
	{
		VOLOGE("!VO_PID_SOURCE_GETSEEKTIMEBYPOS %x", uRet);

		if (m_ptr_EventCallback &&
			(m_nFlag&VO_SOURCE2_FLAG_OPEN_ASYNC) &&
			m_ptr_EventCallback->SendEvent)
		{
			VOLOGI("CB Seek %lu, TimeStamp %llu", uRet, *pTimeStamp);		
			m_ptr_EventCallback->SendEvent(m_ptr_EventCallback->pUserData, VO_EVENTID_SOURCE2_SEEKCOMPLETE, (VO_U32)pTimeStamp, (VO_U32)&uRet);
		}

		return uRet;//TODO
	}
	else if(*pTimeStamp > s_timepos.llMediaTime)
	{
		*pTimeStamp = s_timepos.llMediaTime;
	}
	VOLOGI("time %lld", s_timepos.llMediaTime);

	Flush();
	VO_U32 ret = voSource2ParserWrapper::Seek(pTimeStamp);
	return ret;
}

void voSource2WrapperPushPlay::thread_function()
{
	VO_U32 uRet = VO_RET_SOURCE2_OK;

	if (m_nFlag & VO_SOURCE2_FLAG_OPEN_ASYNC)
		uRet = voSource2ParserWrapper::Open();

	m_PushPlayBuffer.BeginThread();

	uRet = VO_RET_SOURCE2_FAIL;///<disable the double Io design because the drm crash
	if (VO_RET_SOURCE2_OK == uRet)
	{
		OpenPushIO();
		while (PushPlay_Stop != GetStatus() && VO_RET_SOURCE2_FAIL != uRet)
		{	
			uRet = GenerateIndex();
		//	voOS_Sleep(2000);
			if (VO_MAXU64 == m_ullTimeV && VO_MAXU64 == m_ullTimeA)
				m_Event.Wait();
		}
	}
}

VO_U32 voSource2WrapperPushPlay::SetTrackPos(VO_U32 nTrack, VO_U64 * pPos)
{
	VO_U32 uRet = voSource2ParserWrapper::SetTrackPos(nTrack, pPos);

	if (0 == uRet)
	{
		if (m_uVideoTrack == nTrack)
		{
			SetCurTime(VO_SOURCE2_TT_VIDEO,*pPos);
		}
		else if (m_uAudioTrack == nTrack)
		{
			SetCurTime(VO_SOURCE2_TT_AUDIO,*pPos);
		}
	}
	return uRet;
}

VO_U32 voSource2WrapperPushPlay::AddSample(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_U64 &ullTime)
{
	VO_U32 uRet = VO_RET_SOURCE2_OK;

	VO_SOURCE2_SAMPLE s_Sample2 = {0};
	s_Sample2.uTime = ullTime;

	uRet = voSource2ParserWrapper::GetSample(nOutPutType, &s_Sample2);
	if (uRet == VO_RET_SOURCE2_OK)
	{
		ullTime = s_Sample2.uTime;
		while (m_SourceBuf.AddBuffer(nOutPutType, &s_Sample2) == VO_RET_SOURCE2_NEEDRETRY && PushPlay_Running == GetStatus())
			voOS_Sleep(100);
	}
	else if (uRet != VO_RET_SOURCE2_NEEDRETRY)
	{
		ullTime = VO_MAXU64;
		AddEOS(nOutPutType);
	}
	return uRet;
}
VO_U32 voSource2WrapperPushPlay::AddEOS(VO_SOURCE2_TRACK_TYPE nOutPutType)
{
	VOLOGR("!EOS Type %d", nOutPutType);

	VO_BYTE data[20] = {0};

	VO_SOURCE2_SAMPLE s_Sample2;
	memset( &s_Sample2, 0, sizeof(VO_SOURCE2_SAMPLE) );

	s_Sample2.uSize		= 20;
	s_Sample2.uFlag		= VO_SOURCE2_FLAG_SAMPLE_EOS;
	s_Sample2.pBuffer	= data;

	while (m_SourceBuf.AddBuffer(nOutPutType, &s_Sample2) == VO_RET_SOURCE2_NEEDRETRY && PushPlay_Running == GetStatus())
		voOS_Sleep(500);

	return VO_RET_SOURCE2_OK;
}

VO_U32 voSource2WrapperPushPlay::GenerateIndex()
{
	VOLOGR("++GenerateIndex++");
	VO_SOURCE_SAMPLE s_Sample2 = {0};
	VO_U32 uRet = 0;

	CBaseReader* pReader = (CBaseReader*)m_Handle;
	if (pReader == NULL)
	{
		return VO_RET_SOURCE2_FAIL;
	}
	VO_U32 uTrack = 0;
	if (m_ullTimeV <= m_ullTimeA)
	{
		uTrack = m_uVideoTrack;
		m_ullTimeV = VO_MAXU64;
	}
	else
	{
		uTrack = m_uAudioTrack;
		m_ullTimeA = VO_MAXU64;
	}
	VO_U64 ullTs = 0;

	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrack);
	if(!pTrack || !pTrack->IsInUsed())
	{
		VOLOGR("--GenerateIndex--%d track is null",uTrack);
		return VO_RET_SOURCE2_ERRORDATA;
	}

	if(pTrack->IsEndOfStream())
	{
		VOLOGI("track is end!!");

		return VO_RET_SOURCE2_END;
	}
	uRet = pTrack->GetParameter(VO_PID_SOURCE_NEXT_FRAME_INFO,&s_Sample2);
	if (uRet == VO_ERR_NOT_IMPLEMENT)
	{
		VOLOGE("VO_PID_SOURCE_NEXT_FRAME_INFO fail:%d",s_Sample2.Time);
		return VO_RET_SOURCE2_FAIL;
	}

	ullTs = uRet ? VO_MAXU64 : s_Sample2.Time;

	if(uTrack == m_uAudioTrack)
	{
		m_ullTimeA = ullTs;
	}
	else
	{
		m_ullTimeV = ullTs;
	}
	VOLOGR("--GenerateIndex--uRet=%d,m_ullTimeA=%d,m_ullTimeV=%d",uRet,m_ullTimeA,m_ullTimeV);
	return uRet;
}
VO_U32 voSource2WrapperPushPlay::OpenPushIO()
{
	if (m_SrcIOPush.oprIO2)
	{
		delete (VO_SOURCE2_IO_API*)m_SrcIOPush.oprIO2;
		m_SrcIOPush.oprIO2 = NULL;
	}

	m_SrcIOPush.oprIO2 = new VO_SOURCE2_IO_API;
	memcpy(m_SrcIOPush.oprIO2, m_SrcIO.oprIO2, sizeof(VO_SOURCE2_IO_API));

	VO_FILE_SOURCE Source;
	memset( &Source , 0 , sizeof( VO_FILE_SOURCE ) );

	if(m_FileHandle != NULL)
	{
		Source.pSource = m_FileHandle;
		Source.nFlag = VO_FILE_TYPE_IMFBYTESTREAM;
	}
	else
	{
		Source.pSource = m_Url;
		Source.nFlag = VO_FILE_TYPE_NAME;
	}
	Source.nReserve = (VO_U32)&m_SrcIOPush;

	Source.nMode = VO_FILE_READ_ONLY;

	VO_U32 uFlag = 0;
	if (m_SrcIO.bDrm)///<check m_SrcIO drm info for m_SrcIOPush
	{
		uFlag |= VO_SOURCE2_IO_FLAG_OPEN_DRM;
	}
	VO_SOURCE2_IO_API* OperIO2 = (VO_SOURCE2_IO_API*)m_SrcIOPush.oprIO2;

	if (!OperIO2 || !(OperIO2->Init))
	{
		return VO_RET_SOURCE2_OPENFAIL;
	}

	VO_U32 ret = OperIO2->Init(&m_SrcIOPush.phndIO2,Source.pSource,uFlag,NULL);
	if (ret != VO_SOURCE2_IO_OK || !m_SrcIOPush.phndIO2)
	{
		return VO_ERR_OUTOF_MEMORY;
	}

	VOLOGI("VO_SOURCE2_IO_PARAMID_DRMPOINTOR %p", m_ptr_drmcallback);
	ret = OperIO2->SetParam(m_SrcIOPush.phndIO2, VO_SOURCE2_IO_PARAMID_DRMPOINTOR, m_ptr_drmcallback);
	if (ret) {
		VOLOGE("!Set DRM POINTOR 0x%08x", ret);
		return ret;
	}

	ret = OperIO2->SetParam(m_SrcIOPush.phndIO2, VO_PID_SOURCE2_ACTUALFILESIZE, &m_ullActualFileSize);
	if (ret) {
		VOLOGW("!Set ACTUAL FILE SIZE 0x%08x", ret);
	}

	CBaseReader* pReader = (CBaseReader*)m_Handle;

	if( !pReader )
		return VO_RET_SOURCE2_OK;
	return pReader->SetParameter(VO_PID_SOURCE_SECOND_IO,&Source);
}

VO_U32 voSource2WrapperPushPlay::GetSample(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_PTR pSample)
{
	VO_U32 ret = VO_RET_SOURCE2_OK;
	ret = m_SourceBuf.GetBuffer(nOutPutType,pSample);
	if (ret != VO_RET_SOURCE2_OK)
	{
		VO_SOURCE2_SAMPLE* pTmp = (VO_SOURCE2_SAMPLE*)pSample;
		VOLOGR("ret = %d,Time=%lld",ret,pTmp->uTime);
	}
	return ret;
}

STATE_PushPlay voSource2WrapperPushPlay::GetStatus()
{
	voCAutoLock lock(&m_lock);
	return m_eStatePushPlay;
}

VO_VOID voSource2WrapperPushPlay::SetStatus(STATE_PushPlay eStatus)
{
	voCAutoLock lock(&m_lock);
	m_eStatePushPlay = eStatus;
}

VO_U32 voSource2WrapperPushPlay::Flush()
{
#ifndef PLAYPUSH_BUFFER_THREAD
	m_bNeedBufing = VO_TRUE;
#else
	m_SourceBuf.Flush();
	m_ullCurTimeA = 0;
	m_ullCurTimeV = 0;
#endif
	return VO_RET_SOURCE2_OK;
}
VO_U64 voSource2WrapperPushPlay::GetCurTime(VO_SOURCE2_TRACK_TYPE nOutPutType)
{
	voCAutoLock lock(&m_lock);
	if (nOutPutType == VO_SOURCE2_TT_AUDIO)
	{
		return m_ullCurTimeA;
	}
	else if (nOutPutType == VO_SOURCE2_TT_VIDEO)
	{
		return m_ullCurTimeV;
	}
	return 0;
}

VO_VOID voSource2WrapperPushPlay::SetCurTime(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_U64 llTs)
{
	voCAutoLock lock(&m_lock);
	if (nOutPutType == VO_SOURCE2_TT_AUDIO)
	{
		m_ullCurTimeA = llTs;
	}
	else if (nOutPutType == VO_SOURCE2_TT_VIDEO)
	{
		m_ullCurTimeV = llTs;
	}
}

#ifndef PLAYPUSH_BUFFER_THREAD
VO_U32	voSource2WrapperPushPlay::AddSample()
{
	voCAutoLock lock(&m_lock);

	VO_SOURCE2_SAMPLE s_Sample2 = {0};
	VO_U32 uRet = 0;

	if (m_ullTimeV <= m_ullTimeA)
	{
		s_Sample2.uTime = m_ullTimeV;

		uRet = voSource2ParserWrapper::GetSample(VO_SOURCE2_TT_VIDEO, &s_Sample2);
		if (uRet)
		{
			m_ullTimeV = VO_MAXU64;
			AddEOS(VO_SOURCE2_TT_VIDEO);
		}
		else
		{
			m_ullTimeV = s_Sample2.uTime;

			while (m_SourceBuf.AddBuffer(VO_SOURCE2_TT_VIDEO, &s_Sample2) == VO_RET_SOURCE2_NEEDRETRY && PushPlay_Running == GetStatus())
				voOS_Sleep(100);
		}
	}
	else
	{
		s_Sample2.uTime = m_ullTimeA;

		uRet = voSource2ParserWrapper::GetSample(VO_SOURCE2_TT_AUDIO, &s_Sample2);
		if (uRet)
		{
			m_ullTimeA = VO_MAXU64;
			AddEOS(VO_SOURCE2_TT_AUDIO);
		}
		else
		{
			m_ullTimeA = s_Sample2.uTime;

			while (m_SourceBuf.AddBuffer(VO_SOURCE2_TT_AUDIO, &s_Sample2) == VO_RET_SOURCE2_NEEDRETRY && PushPlay_Running == GetStatus() )
				voOS_Sleep(100);
		}
	}

	return uRet;
}

VO_U32 voSource2WrapperPushPlay::CheckBufferStatus()
{
	VO_PUSH_TRACK_INFO VideoInfo = {0};
	VO_PUSH_TRACK_INFO AudioInfo = {0};

	VO_BOOL bVideoOK = VO_FALSE;
	VO_BOOL bAudioOK = VO_FALSE;

	VO_U32 ret = VO_ERR_SOURCE_OK;
	ret = GetTrackBufferDuration(VO_SOURCE2_TT_VIDEO,&VideoInfo);
	if (VO_ERR_SOURCE_END == ret)
	{
		bVideoOK = VO_TRUE;
	}
	else if (VO_ERR_SOURCE_OK == ret)
	{
		VOLOGR("m_bNeedBufing =%d,EndOfFile =%d,llCurDuration=%d ,llCurTs==%d",m_bNeedBufing,VideoInfo.EndOfFile,VideoInfo.llCurDuration, VideoInfo.llCurTs);
		if (VideoInfo.EndOfFile)
		{
			bVideoOK = VO_TRUE;
		}
		else if (VideoInfo.llCurDuration - VideoInfo.llCurTs <= 0)
		{
			m_bNeedBufing = VO_TRUE;
		}
		else
		{
			bVideoOK = IsTrackBufferFull(VO_SOURCE2_TT_VIDEO,VideoInfo.llCurDuration - VideoInfo.llCurTs,m_bNeedBufing,m_uBufferTime);
		}	
	}
	else
	{
		VOLOGE("Get Video TrackBufferDuration fail ret= %d",ret);
	}
	

	ret = GetTrackBufferDuration(VO_SOURCE2_TT_AUDIO,&AudioInfo);
	if (VO_ERR_SOURCE_END == ret)
	{
		bAudioOK = VO_TRUE;
	}
	else if (VO_ERR_SOURCE_OK == ret)
	{
		VOLOGR("m_bNeedBufing =%d,EndOfFile =%d,llCurDuration=%d ,llCurTs==%d",m_bNeedBufing,AudioInfo.EndOfFile,AudioInfo.llCurDuration, AudioInfo.llCurTs);
		if (AudioInfo.EndOfFile)
		{
			bAudioOK = VO_TRUE;
		}
		else if (AudioInfo.llCurDuration - AudioInfo.llCurTs <= 0)
		{
			m_bNeedBufing = VO_TRUE;
		}
		else
		{
			bAudioOK = IsTrackBufferFull(VO_SOURCE2_TT_AUDIO,AudioInfo.llCurDuration - AudioInfo.llCurTs,m_bNeedBufing,m_uBufferTime);
		}
	}
	else
	{
		VOLOGE("Get Video TrackBufferDuration fail ret= %d",ret);
	}
	
	if (bAudioOK && bVideoOK)
	{
		m_bNeedBufing = VO_FALSE;
		return VO_RET_SOURCE2_OK;
	}
	return VO_RET_SOURCE2_NEEDRETRY;
}

VO_U32 voSource2WrapperPushPlay::GetTrackBufferDuration(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_PUSH_TRACK_INFO* pTrackInfo)
{
	if(!m_Handle || !pTrackInfo)
		return VO_ERR_SOURCE_ERRORDATA;

	VO_U32  uTrack;
	if( nOutPutType == VO_SOURCE2_TT_VIDEO )
		uTrack = m_uVideoTrack;
	else if( nOutPutType == VO_SOURCE2_TT_AUDIO )
		uTrack = m_uAudioTrack;
	else 
	{
		return VO_ERR_SOURCE_END;
	}

	CBaseReader* pReader = (CBaseReader*)m_Handle;
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrack);
	if(!pTrack || !pTrack->IsInUsed())
	{
		return VO_ERR_SOURCE_END;
	}

	if(pTrack->IsEndOfStream())
	{
		return VO_ERR_SOURCE_END;
	}
	VO_U32 rc = pTrack->GetParameter(VO_PID_SOURCE_CURRENT_FILE_INFO,pTrackInfo);
	return rc;
	
}

VO_BOOL voSource2WrapperPushPlay::IsTrackBufferFull(VO_SOURCE2_TRACK_TYPE nOutPutType, VO_U32 uDutation, VO_BOOL bBuffering, VO_U32 uBufferTime)
{
	if(!m_Handle)
		return VO_TRUE;
	VO_U32  uTrack;

	if( nOutPutType == VO_SOURCE2_TT_VIDEO )
		uTrack = m_uVideoTrack;
	else if( nOutPutType == VO_SOURCE2_TT_AUDIO )
		uTrack = m_uAudioTrack;
	else 
	{
		return VO_TRUE;
	}

	CBaseReader* pReader = (CBaseReader*)m_Handle;
	CBaseTrack* pTrack = pReader->GetTrackByIndex(uTrack);
	if(!pTrack || !pTrack->IsInUsed())
	{
		return VO_TRUE;
	}

	if(pTrack->IsEndOfStream())
	{
		return VO_TRUE;
	}

	if (bBuffering)
	{
		if(uDutation > uBufferTime)
		{
			return VO_TRUE;
		}	
	}
	else if(uDutation > 0)
	{
		return VO_TRUE;
	}

	return VO_FALSE;
}

#endif

CPushPlayBuff::CPushPlayBuff(voSource2WrapperPushPlay *pWrapper)
: m_pWrapper(pWrapper)
, m_Status(ThreadStatus_Running)
{

}

CPushPlayBuff::~CPushPlayBuff()
{

}

void CPushPlayBuff::thread_function()
{
	if (NULL == m_pWrapper)
	{
		return;
	}
	VO_U32 uRet = VO_RET_SOURCE2_OK;

	VO_U64  ullTime = 0;
	VO_SOURCE2_TRACK_TYPE uType;

	while (PushPlay_Stop != m_pWrapper->GetStatus())
	{
		if (PushPlay_Seeking == m_pWrapper->GetStatus())
		{
			m_Status = ThreadStatus_Pause;
			m_Event.Wait();
			m_Status = ThreadStatus_Running;
		}
		
		if (m_pWrapper->GetCurTime(VO_SOURCE2_TT_VIDEO) <= m_pWrapper->GetCurTime(VO_SOURCE2_TT_AUDIO))
		{
			ullTime = m_pWrapper->GetCurTime(VO_SOURCE2_TT_VIDEO);
			uType = VO_SOURCE2_TT_VIDEO;
		}
		else
		{
			ullTime = m_pWrapper->GetCurTime(VO_SOURCE2_TT_AUDIO);
			uType = VO_SOURCE2_TT_AUDIO;
		}
		m_pWrapper->AddSample(uType,ullTime);
		m_pWrapper->SetCurTime(uType,ullTime);

		if (m_pWrapper->GetCurTime(VO_SOURCE2_TT_VIDEO) == VO_MAXU64 &&  m_pWrapper->GetCurTime(VO_SOURCE2_TT_AUDIO) == VO_MAXU64)
		{
			m_Status = ThreadStatus_Pause;
			m_Event.Wait();
			m_Status = ThreadStatus_Running;
		}
	}

	m_pWrapper->AddEOS(VO_SOURCE2_TT_AUDIO);
	m_pWrapper->AddEOS(VO_SOURCE2_TT_VIDEO);
}