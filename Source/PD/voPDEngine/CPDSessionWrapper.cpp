//#include <voPDPort.h>
#include <tchar.h>
#include <stdio.h>

#include "CPDSessionWrapper.h"
#include "CPDTrackWrapper.h"

#include "CHttpDownloadFile.h"
#include "UFileReader.h"
#include "PDHeaderDataStruct.h"
#include "macro.h"
#include "voutility.h"
extern int g_nLog;

#define VO_REMAIN_FILE_FRAGMENT_COUNT			30
void		CPDSessionWrapper::SetReaderAPI(TFileReaderAPI* api)
{
		 m_fPDOpen=	api->open;
		 m_fPDClose=	api->close;
		 m_fPDSetParam=api->setParam;
		 m_fPDGetParam=api->getParam;
		 m_fPDFilePos2MediaTime=api->pos2time;
		 m_fPDMediaTime2FilePos=api->time2pos;
		 m_fTrackOpen=api->openTrack;
		 m_fTrackClose=api->closeTrack;
		 m_fTrackSetParam=api->setTrackParam;
		 m_fTrackGetParam=api->getTrackParam;
		 m_fTrackGetInfo=api->getTrackInfo;
		 m_fGetSampleByIndex=api->getSampleByIndex;
		 m_fGetSampleByTime=api->getSampleByTime;
		 m_fGetNextKeyFrame=api->getNextKeyFrame;
		 m_fTrackGetFilePosByIndex=api->getPosByIndex;
}
CPDSessionWrapper::CPDSessionWrapper(void* userData)
	: 
	m_hSource(NULL)
	, m_pAudioTrack(NULL)
	, m_pVideoTrack(NULL)
	, m_dwDuration(0)
	, m_fPDOpen(NULL)
	, m_fPDClose(NULL)
	, m_fPDSetParam(NULL)
	, m_fPDGetParam(NULL)
	, m_fPDFilePos2MediaTime(NULL)
	, m_fPDMediaTime2FilePos(NULL)
	, m_fTrackOpen(NULL)
	, m_fTrackClose(NULL)
	, m_fTrackSetParam(NULL)
	, m_fTrackGetParam(NULL)
	, m_fTrackGetInfo(NULL)
	, m_fGetSampleByIndex(NULL)
	, m_fGetSampleByTime(NULL)
	, m_fGetNextKeyFrame(NULL)
	, m_fTrackGetFilePosByIndex(NULL)
	, m_NetGet(this)
#ifdef _HTC
	, m_bNoWritePDHeader(false)
#else
	, m_bNoWritePDHeader(true)
#endif
	, m_dwClipBitrate(0)
	,m_downLoadThreadPriority(0)
	,m_downLoadType(VO_NETDOWN_TYPE_NORMAL)
	,m_url(NULL)
	,m_currentTrackType(0)
{
	m_opFile.voOpenFile = UFileReader::voOpenHttpFile;
	m_opFile.voCloseFile = UFileReader::voCloseFile;
	m_opFile.voReadFile = UFileReader::voReadFile;
	m_opFile.voSeekFile = UFileReader::voSeekFile;
	m_opFile.voGetFileSize = UFileReader::voGetFileSize;
	//TO_DO:init params
	memset(&m_initParam,0,sizeof(m_initParam));
	m_initParam.nBufferTime = 5;			//<S>
	m_initParam.nPlayBufferTime = 90;		//<S>
	m_initParam.nHTTPDataTimeOut = 15000;	//<MS>
	m_initParam.nMaxBuffer = 5120;			//<KB>
	m_initParam.nTempFileLocation = 1;		//storage
	m_initParam.nPacketLength = 5;			//<KB>
	m_initParam.nHttpProtocol = 1;
	strcpy((char*)m_initParam.mUserAgent, "VisualOn-PD Player");
#ifdef _WIN32_WCE
	_tcscpy(m_initParam.mFilePath, _T("\\tmp.3gp"));
#else
	_tcscpy(m_initParam.mFilePath, _T("C:\\visualon\\VOPD\\PDSource.mp4"));
#endif
	CREATE_MUTEX(m_csRead);
}

CPDSessionWrapper::~CPDSessionWrapper()
{
	DumpLog("~CPDSessionWrapper() start\r\n");
	Close();
	SAFE_DELETE_ARRAY(m_url);
	DumpLog("~CPDSessionWrapper() end\r\n");
	DELETE_MUTEX(m_csRead);
}

HRESULT CPDSessionWrapper::OpenSource(TCHAR* pFile)
{
	DumpLog("CPDSessionWrapper::OpenSource\r\n");
	if(UFileReader::g_bCancel)
		return E_VO_USER_ABORT;
	int urlLen=_tcsclen(pFile);
	m_url = new TCHAR[urlLen+1];
	_tcscpy(m_url,pFile);
	SPDInitParam* pInitParam = &m_initParam;
	VONETDOWNTYPE type=m_downLoadType;
	m_NetGet.SetDownLoadType(type);
	TCHAR* filetype=pInitParam->mFilePath;//m_szFileSource;


	
	m_NetGet.SetLoadState(true);
	m_NetGet.SetSinkType(pInitParam->nTempFileLocation);
	if(m_downLoadThreadPriority)
		m_NetGet.SetDownloadThreadPriority(m_downLoadThreadPriority);
	m_NetGet.SetUrl((LPCTSTR)pFile);
	
	CHttpDownloadFile::g_pNetGet = &m_NetGet;
	CHttpDownloadFile::g_dwOffset = 0;

	if(UFileReader::g_bCancel)
		return E_VO_USER_ABORT;

	DumpLog("CheckPDHeader\r\n");

	if(CheckPDHeader())
	{
		if(!m_NetGet.InitByPDHeader())
		{
			DumpLog("InitByPDHeader fail\r\n");
			return E_FAIL;
		}
	}
	else
	{
		DumpLog("m_NetGet.StartDownload start\r\n");

		//start http download file
		if(!m_NetGet.StartDownload())
		{
			DumpLog("m_NetGet.StartDownload fail end\r\n");
#if MULTI_PROTOCOL
			if(m_NetGet.GetLastEventCode()==VO_PD_EVENT_GET_ASX)
				return VO_PD_EVENT_GET_ASX;
			else
#endif//MULTI_PROTOCOL
				return E_NETWORK_ERROR;
		}

		DumpLog("m_NetGet.StartDownload ok end\r\n");
	}

	DWORD dwTimeRecord = voGetCurrentTime();

	//open local file
	if(!m_fPDOpen)
	{
		DumpLog("can't find open function in reader.\r\n");
		return E_POINTER;
	}

	if(UFileReader::g_bCancel)
		return E_VO_USER_ABORT;

	DumpLog("start parse file\r\n");
#if 0
	int seconds=20;
	do 
	{
		IVOThread::Sleep(1000);
	} while(seconds-->0);
#endif//
	DWORD rc = m_fPDOpen(&m_hSource, (char*)filetype, &m_opFile);
	if(VORC_FILEREAD_OK != rc || !m_hSource)
	{
		DumpLog("parse file fail.\r\n");
		return VFW_E_INVALID_FILE_FORMAT;
	}

	DWORD dwFileHeaderSize = 0;
	PDGetParam(VOID_FILEREAD_HEADER_SIZE, (LONG*)&dwFileHeaderSize);
	m_NetGet.SetFileHeaderSize(dwFileHeaderSize);

	PDGetParam(VOID_COM_DURATION, (LONG*)&m_dwDuration);

	CDFInfo* pDfInfo = m_NetGet.GetDFInfo();
	CDFInfo2* pDfInfo2 = m_NetGet.GetDFInfo2();
	int fileSize=LIVE_SIZE;//default
	if(pDfInfo)
		fileSize=pDfInfo->GetFileSize();
	else if(pDfInfo2)
		fileSize=pDfInfo2->GetFileSize();

	if(fileSize!=LIVE_SIZE)
	{
		if(pDfInfo)
			m_dwClipBitrate = LONGLONG(pDfInfo->GetFileSize() - dwFileHeaderSize) * 1000 / m_dwDuration;
		else if(pDfInfo2)
		{
			
			m_dwClipBitrate = LONGLONG(fileSize - dwFileHeaderSize) * 1000 / m_dwDuration;
			if(dwFileHeaderSize + LONGLONG(fileSize - dwFileHeaderSize) * MIN(m_dwDuration, pInitParam->nBufferTime * 1000 * 4) / m_dwDuration > pInitParam->nMaxBuffer * 1024)
			{
	#ifdef _HTC
				m_NetGet.NotifyEvent(HP_EVENT_INSUFFICIENT_SPACE, NULL);
	#else	//_HTC
				m_NetGet.NotifyEvent(VO_PD_EVENT_INSUFFICIENT_SPACE, NULL);
	#endif	//_HTC
				DumpLog("nMaxBuffer is too small!\r\n");
				return E_SMALL_BUFFER;
			}
		}
	}
	else
	{
		m_dwDuration	= MAXLONG;
		m_dwClipBitrate = 64*1024;
		voLog(LL_TRACE,"flow.txt","this is a live stream\n");
	}
	//decide if select this clip, if not select this clip, it will return E_NOT_ENOUGH_BANDWIDTH
	//TO_DO:remove it
	AUTOSELECTCALLBACK pfAutoSelectCallback = NULL;//m_pSourceFilter->GetAutoSelectCallbackFunc();
	if(pfAutoSelectCallback && !m_NetGet.IsFinish())
	{
		//wait at least 1 second to compute throughput!!
		while(voGetCurrentTime() - dwTimeRecord < 1000 && !UFileReader::g_bCancel && !m_NetGet.IsFinish())
			IVOThread::Sleep(10);

		DWORD dwBytesPerSec = 0;
		while(!m_NetGet.GetParam(VOID_NETDOWN_BYTES_PER_SEC, (long*)&dwBytesPerSec) && !m_NetGet.IsFinish())
		{
			if(UFileReader::g_bCancel)
				return E_VO_USER_ABORT;

			DumpLog("download still not start, please wait!\r\n");
			IVOThread::Sleep(10);
		}

		if(!pfAutoSelectCallback(m_dwClipBitrate, dwBytesPerSec))
		{
			m_bNoWritePDHeader = true;
			return E_NOT_ENOUGH_BANDWIDTH;
		}
	}
	m_NetGet.SetLoadState(false);

	DumpLog("CPDSessionWrapper::OpenSource ok end\r\n");
	return S_OK;
}


bool CPDSessionWrapper::Close()
{
	DumpLog("CPDSessionWrapper::Close()\r\n");

	m_NetGet.DestroyBGThread();
	DumpLog("m_NetGet.DestroyBGThread()\r\n");

	m_NetGet.StopDownload();
	DumpLog("m_NetGet.StopDownload()\r\n");

	m_NetGet.CloseSink();
	DumpLog("m_NetGet.CloseSink()\r\n");

	if(!m_hSource || !m_fPDClose)
		return true;
	DumpLog("m_fTrackClose2\r\n");

	m_fPDClose(m_hSource);
	m_hSource = NULL;
	DumpLog("m_fPDClose(m_hSource)\r\n");

	WritePDHeader();
	DumpLog("WritePDHeader()\r\n");

	return true;
}

bool CPDSessionWrapper::Stop()
{
	return true;
}

bool CPDSessionWrapper::Pause()
{
	return true;
}

bool CPDSessionWrapper::Run()
{
	m_NetGet.StopBuffering();
	return true;
}

bool CPDSessionWrapper::Start()
{
	if(m_fPDMediaTime2FilePos)
	{
		DWORD dwCurrBufferingTime = GetCurrBufferingTime();
		int nStart = 0, nEnd = 0;
		if(VORC_FILEREAD_OK == m_fPDMediaTime2FilePos(m_hSource, 0, &nStart, true) && 
			VORC_FILEREAD_OK == m_fPDMediaTime2FilePos(m_hSource, dwCurrBufferingTime, &nEnd, false))
			m_NetGet.StartBuffering(nStart, nEnd, true, BT_SEEK, m_NetGet.GetCurrDownloadPos());
	}
	return true;
}

bool CPDSessionWrapper::SetPos(long start)
{
	CAutoLock lock(m_csRead);
	DumpLog("[CPDSessionWrapper::SetPos]\r\n");

	//if download is finished
	if(m_NetGet.IsFinish())
		return true;

	//if download not support seek or file read not support seek
	if(!m_NetGet.IsSupportSeek() || !m_fPDMediaTime2FilePos)
		return false;

	//get file position according media time
	int nStart = 0;
#if 1
	//start-=200;
	//if(start<0)
	//	start=0;
	if(!GetCurrPlayFilePos(&nStart,start))
		return false;
#else	
	if(m_pAudioTrack)		
		m_pAudioTrack->SetPos(start);
	if(m_pVideoTrack)		
		m_pVideoTrack->SetPos(start);
	start-=500;
	if(start<0)
		start=0;
	if(VORC_FILEREAD_OK != m_fPDMediaTime2FilePos(m_hSource, start, &nStart , false))
		return false;
#endif	
	DWORD dwCurrBufferingTime = GetCurrBufferingTime();
	int nEnd = 0;
	if(VORC_FILEREAD_OK != m_fPDMediaTime2FilePos(m_hSource, start+ dwCurrBufferingTime, &nEnd, false))
		return false;
	//int curPos=m_NetGet.GetCurrDownloadPos();
    //nStart=max(curPos,nStart);
	voLog(LL_TRACE,"seek.txt","start=%d,pos(%d-%d)\n",start,nStart,nEnd);
	if(!m_NetGet.SetDownloadPos(nStart))
		return false;
	
	return m_NetGet.StartBuffering(nStart, nEnd, false, BT_SEEK, -1);
}

DWORD CPDSessionWrapper::PDSetParam(LONG nID, LONG lValue)
{
	if(VOID_FILEREAD_CALLBACK_NOTIFY == nID)
	{
		m_NetGet.SetParam(nID, lValue);
		return VORC_OK;
	}
	switch(nID)
	{
	case VOID_FILEREAD_ACTION_STOP:
		{

			if(!Stop())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_START:
		{

			if(!Start())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_PAUSE:
		{

			if(!Pause())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_RUN:
		{
			if(!Run())
				return VORC_FILEREAD_NEED_RETRY;

			return VORC_FILEREAD_OK;
		}
	case VOID_FILEREAD_ACTION_SEEK:
		{

			if(!SetPos(lValue))
				return VORC_FILEREAD_NEED_RETRY;
  		    return VORC_FILEREAD_OK;
		}
#ifndef _HTC
	case VOID_STREAMING_INIT_PARAM:
		{
			PVOPDInitParam param=(PVOPDInitParam)lValue;
			memcpy(&m_initParam,param,sizeof(VOPDInitParam));
			return VORC_FILEREAD_OK;
		}
#endif//_HTC
	}
#ifdef _HTC
	if(ID_HTC_PD_PAUSE_DOWNLOAD == nID)
#else	//_HTC
	if(VOID_STREAMING_PAUSE_DOWNLOAD == nID)
#endif	//_HTC
	{
		bool bPauseDownload = (bool)lValue;
		if(bPauseDownload)
			DumpLog("ID_HTC_PD_PAUSE_DOWNLOAD, pause.\r\n");
		else
			DumpLog("ID_HTC_PD_PAUSE_DOWNLOAD, continue.\r\n");

		m_NetGet.Pause(bPauseDownload);
		return VORC_OK;
	}

	if(!m_fPDSetParam || !m_hSource)
		return (DWORD)VORC_FAIL;

	return m_fPDSetParam(m_hSource, nID, lValue);
}

DWORD CPDSessionWrapper::PDGetParam(LONG nID, LONG* plValue)
{
	if(!m_fPDGetParam || !m_hSource)
		return (DWORD)VORC_FAIL;
	
	if(VOID_FILEREAD_BUFFERING_PROGRESS == nID)
	{
		*plValue = m_NetGet.GetBufferingPercent();
		return VORC_OK;
	}

#ifdef _HTC
	if(ID_HTC_STREAMING_INFO == nID)
	{
		HS_PDStreamingInfo* pInfo = (HS_PDStreamingInfo*)plValue;
		pInfo->clip_type = PROGRESSIVE;

		SPDInitParam* pInitParam = GetInitParam();
		WideCharToMultiByte(CP_ACP, 0, pInitParam->mFilePath, -1, pInfo->clip_title, MAX_PATH, NULL, NULL);

		pInfo->clip_duration = m_dwDuration / 1000;
		pInfo->clip_bitrate = m_dwClipBitrate;
		pInfo->clip_SupportPAUSE = 1;
		if(m_NetGet.IsFinish())
			pInfo->clip_SupportDirectSeek = 1;
		else
			pInfo->clip_SupportDirectSeek = m_NetGet.IsSupportSeek() ? 1 : 0;

		if(m_pVideoTrack)
		{
			VOAVCODEC	avCodec;
			m_pVideoTrack->GetParam(VOID_FILEREAD_CODEC, (LONG*)&avCodec);

			if(avCodec == VC_H264 || avCodec == VC_AVC)
				pInfo->codecType[0] = kCodecTypeH264BL;
			else if(avCodec == VC_MPEG4)
				pInfo->codecType[0] = kCodecTypeMPEG4SP;
			else if(avCodec == VC_H263)
				pInfo->codecType[0] = kCodecTypeH263BL;

			m_pVideoTrack->GetParam(VOID_COM_VIDEO_WIDTH, (LONG*)&pInfo->clip_width);
			m_pVideoTrack->GetParam(VOID_COM_VIDEO_HEIGHT, (LONG*)&pInfo->clip_height);
			m_pVideoTrack->GetParam(VOID_COM_VIDEO_FRAME_RATE, (LONG*)&pInfo->clip_frame_rate);
		}
		else
		{
			pInfo->codecType[0] = kCodecTypeUnknown;
			pInfo->clip_width = 0;
			pInfo->clip_height = 0;
			pInfo->clip_frame_rate = 0;
		}

		if(m_pAudioTrack)
		{
			VOAVCODEC	avCodec;
			m_pAudioTrack->GetParam(VOID_FILEREAD_CODEC, (LONG*)&avCodec);
			if(avCodec == AC_AAC)
				pInfo->codecType[1] = kCodecTypeMPEG4AAC;
			else if(avCodec == AC_AMR)
				pInfo->codecType[1] = kCodecTypeGSMAMRNB;
			else if(avCodec == AC_AWB)
				pInfo->codecType[1] = kCodecTypeGSMAMRWB;
			else if (avCodec == AC_EVRC)
				pInfo->codecType[1] = kCodecTypeEVRC;
			else if (avCodec == AC_QCELP)
				pInfo->codecType[1] = kCodecTypeQCELP;
		}
		else
			pInfo->codecType[1] = kCodecTypeUnknown;

		CDFInfo* pDfInfo = m_NetGet.GetDFInfo();
		CDFInfo2* pDfInfo2 = m_NetGet.GetDFInfo2();
		if(pDfInfo)
			pInfo->clip_file_length = pDfInfo->GetFileSize();
		else if(pDfInfo2)
			pInfo->clip_file_length = pDfInfo2->GetFileSize();

		return VORC_OK;
	}
	else if(ID_HTC_PD_THROUGHPUT == nID)
	{
		if(m_NetGet.GetParam(VOID_NETDOWN_BYTES_PER_SEC, plValue))
			return VORC_OK;
		else
			return VORC_NETDOWN_UNKNOWN_ERROR;
	}
#else	//_HTC
	if(VOID_STREAMING_INFO == nID)
	{
		VOStreamingInfo* pInfo = (VOStreamingInfo*)plValue;
		pInfo->clipType = ST_PROGRESSIVE;

		SPDInitParam* pInitParam = GetInitParam();
		//WideCharToMultiByte(CP_ACP, 0, pInitParam->mFilePath, -1, pInfo->clip_title, MAX_PATH, NULL, NULL);
		wcstombs(pInfo->clip_title,pInitParam->mFilePath,MAX_PATH);
		pInfo->clip_bitrate = 0;
		pInfo->clip_SupportPAUSE = 1;

		if(m_pVideoTrack)
		{
			VOAVCODEC	avCodec;
			m_pVideoTrack->GetParam(VOID_FILEREAD_CODEC, (LONG*)&avCodec);

			if(avCodec == VC_H264 || avCodec == VC_AVC)
				pInfo->codecType[0] = VOCodecTypeH264BL;
			else if(avCodec == VC_MPEG4)
				pInfo->codecType[0] = VOCodecTypeMPEG4SP;
			else if(avCodec == VC_H263)
				pInfo->codecType[0] = VOCodecTypeH263BL;

			m_pVideoTrack->GetParam(VOID_COM_VIDEO_WIDTH, (LONG*)&pInfo->clip_width);
			m_pVideoTrack->GetParam(VOID_COM_VIDEO_HEIGHT, (LONG*)&pInfo->clip_height);
		}
		else
		{
			pInfo->codecType[0] = VOCodecTypeUnknown;
			pInfo->clip_width = 0;
			pInfo->clip_height = 0;
		}

		if(m_pAudioTrack)
		{
			VOAVCODEC	avCodec;
			m_pAudioTrack->GetParam(VOID_FILEREAD_CODEC, (LONG*)&avCodec);
			if(avCodec == AC_AAC)
				pInfo->codecType[1] = VOCodecTypeMPEG4AAC;
			else if(avCodec == AC_AMR)
				pInfo->codecType[1] = VOCodecTypeGSMAMRNB;
			else if(avCodec == AC_AWB)
				pInfo->codecType[1] = VOCodecTypeGSMAMRWB;
			else if (avCodec == AC_EVRC)
				pInfo->codecType[1] = VOCodecTypeEVRC;
			else if (avCodec == AC_QCELP)
				pInfo->codecType[1] = VOCodecTypeQCELP;
		}
		else
			pInfo->codecType[1] = VOCodecTypeUnknown;

		return VORC_OK;
	}
#endif	//_HTC
#ifdef _HTC
	else if(ID_HTC_STREAMING_BUFFERING_PROGRESS == nID)
#else	//_HTC
	else if(VOID_STREAMING_BUFFERING_PROGRESS == nID)
#endif	//_HTC
	{
		*plValue = m_NetGet.GetBufferingPercent();
		return VORC_OK;
	}
#ifdef _HTC
	else if(ID_HTC_PD_DOWNLOADING_TIME == nID)
#else	//_HTC
	else if(VOID_STREAMING_DOWNLOADING_TIME == nID)
#endif	//_HTC
	{
#ifdef _RETURN_DOWNLOAD_PERCENT_MEDIA_TIME
		if(m_fPDFilePos2MediaTime)
		{
			DWORD dwTime = 0;
			DWORD dwCurrDownloadPos = m_NetGet.GetCurrDownloadPos();
			m_fPDFilePos2MediaTime(m_hSource, dwCurrDownloadPos, (int*)&dwTime, true);
			*plValue = (LONGLONG(dwTime) * 100 + m_dwDuration / 2) / m_dwDuration;
			if(g_nLog)
			{
				char sz[256];
				sprintf(sz, "[ID_HTC_PD_DOWNLOADING_TIME]current download: %d<B>, media time: %d<MS>, duration: %d<MS>\r\n", dwCurrDownloadPos, dwTime, m_dwDuration);
				DumpLog(sz);
			}
		}
		else
			*plValue = m_NetGet.GetCurrDownloadPercent();
#else
		*plValue = m_NetGet.GetCurrDownloadPercent();
#endif	//_RETURN_DOWNLOAD_PERCENT_MEDIA_TIME
		
		return VORC_OK;
	}

	return m_fPDGetParam(m_hSource, nID, plValue);
}

DWORD CPDSessionWrapper::TrackSetParam(HVOFILETRACK pPDTrack, LONG nID, LONG lValue)
{
	if(!m_fTrackSetParam || !m_hSource)
		return (DWORD)VORC_FAIL;

	return m_fTrackSetParam(m_hSource, pPDTrack, nID, lValue);
}

DWORD CPDSessionWrapper::TrackGetParam(HVOFILETRACK pPDTrack, LONG nID, LONG* plValue)
{
	if(!m_fTrackGetParam || !m_hSource)
		return (DWORD)VORC_FAIL;

	return m_fTrackGetParam(m_hSource, pPDTrack, nID, plValue);
}

DWORD CPDSessionWrapper::TrackGetInfo(HVOFILETRACK pPDTrack, VOTRACKINFO* pTrackInfo)
{
	if(!m_fTrackGetInfo || !m_hSource)
		return (DWORD)VORC_FAIL;

	return m_fTrackGetInfo (m_hSource, pPDTrack, pTrackInfo);
}
CPDTrackWrapper*			CPDSessionWrapper::GetTrackWrapper(HVOFILETRACK hTrack)
{
	CPDTrackWrapper*	track=NULL;
	if(m_pAudioTrack&&m_pAudioTrack->GetTrack()==hTrack)
	{
		track = m_pAudioTrack;
	}
	else if(m_pVideoTrack&&m_pVideoTrack->GetTrack()==hTrack)
	{
		track = m_pVideoTrack;
	}
	return track;	 
}
DWORD CPDSessionWrapper::TrackGetSampleByIndex(HVOFILETRACK pPDTrack, VOSAMPLEINFO* pSampleInfo)
{ 
	CAutoLock lock(m_csRead);
	if(!m_fGetSampleByIndex || !m_hSource)
		return (DWORD)VORC_FAIL;
	CPDTrackWrapper*  track=GetTrackWrapper(pPDTrack);
	m_currentTrackType=track->IsVideo()?TT_VIDEO:TT_AUDIO;
	long rc=m_fGetSampleByIndex(m_hSource, pPDTrack, pSampleInfo);
	if(	rc==VORC_FILEREAD_NEED_RETRY&&pSampleInfo->uIndex>1)
	{
		int pos=m_fTrackGetFilePosByIndex(m_hSource,pPDTrack,pSampleInfo->uIndex);
		if(m_currentTrackType==TT_VIDEO)
			voLog(LL_TRACE,"videoTS.txt","data lack:index=%d,pos=%d\n",pSampleInfo->uIndex,pos);
		else
			voLog(LL_TRACE,"audioTS.txt","data lack:index=%d,pos=%d\n",pSampleInfo->uIndex,pos);

		OnDataLack();
	}
	m_currentTrackType = 0;

	return rc;
	
}

DWORD CPDSessionWrapper::TrackGetSampleByTime(HVOFILETRACK pPDTrack, VOSAMPLEINFO* pSampleInfo)
{
	if(!m_fGetSampleByTime || !m_hSource)
		return (DWORD)VORC_FAIL;

	return m_fGetSampleByTime(m_hSource, pPDTrack, pSampleInfo);
}

int CPDSessionWrapper::TrackGetNextKeySample(HVOFILETRACK pPDTrack, int nIndex, int nDirectionFlag)
{
	if(!m_fGetNextKeyFrame || !m_hSource)
		return (DWORD)VORC_FAIL;

	return m_fGetNextKeyFrame(m_hSource, pPDTrack, nIndex, nDirectionFlag);
}
DWORD CPDSessionWrapper::TrackOpen(HVOFILETRACK* pPDTrack,int nTrackIndex)
{
	if(!m_fTrackOpen || !m_hSource)
		return (DWORD)VORC_FAIL;
	int rc=m_fTrackOpen (pPDTrack, m_hSource, nTrackIndex);
	if(rc==0)
	{
		VOTRACKINFO		infoTrack;
		int rc2 = TrackGetInfo(*pPDTrack, &infoTrack);
		if(rc2==0)
		{
			if(infoTrack.uType == TT_AUDIO)
				m_pAudioTrack= new CPDTrackWrapper(this,*pPDTrack,false);
			else if(infoTrack.uType == TT_VIDEO)
				m_pVideoTrack=new CPDTrackWrapper(this,*pPDTrack,true);
		}
	}
	return rc;
}
 DWORD CPDSessionWrapper:: TrackClose(HVOFILETRACK pPDTrack)
 {
	 if(!m_fTrackClose || !m_hSource)
		 return (DWORD)VORC_FAIL;
	 return m_fTrackClose ( m_hSource, pPDTrack);
 }
int CPDSessionWrapper::TrackGetFilePosByIndex(HVOFILETRACK pPDTrack, int nIndex)
{
	if(!m_fTrackGetFilePosByIndex || !m_hSource)
		return (DWORD)VORC_FAIL;

	return m_fTrackGetFilePosByIndex(m_hSource, pPDTrack, nIndex);
}


void CPDSessionWrapper::OnDataLack()
{
	if(m_NetGet.IsBuffering() || !m_fPDMediaTime2FilePos)
		return;
#if 1
	//get current media time
	int		 llMediaTime = 0;
	int		 llTmp = 0;
	CPDTrackWrapper* pTrack = GetAudioTrack();
	if(pTrack)
	{
		llTmp=pTrack->GetCurTime();
		if(llTmp > llMediaTime)
			llMediaTime = llTmp;
	}

	pTrack = GetVideoTrack();
	if(pTrack)
	{
		llTmp=pTrack->GetCurTime();
		if(llTmp > llMediaTime)
			llMediaTime = llTmp;
	}

	DWORD dwCurrBufferingTime = GetCurrBufferingTime();
	int nMediaTime = int(llMediaTime);
	int nEnd = 0;
	if(VORC_FILEREAD_OK != m_fPDMediaTime2FilePos(m_hSource, nMediaTime + dwCurrBufferingTime, &nEnd, false))
		return;

	int nStart = m_NetGet.GetCurrDownloadPos();
	//old: when data lack reach RESERVE_BUFFER_SIZE, we do buffering!!
	//now: once data lack, we do buffering!!
	if(nEnd > nStart/* + RESERVE_BUFFER_SIZE*/)
	{
		voLog(LL_TRACE,"buffering.txt", "[OnDataLack]nMediaTime: %d, nStart: %d, nEnd: %d\r\n", nMediaTime, nStart, nEnd);
		m_NetGet.StartBuffering(nStart, nEnd, false, BT_PLAY, -1);		
	}
	else
	{
		voLog(LL_TRACE,"error.txt","start=%d,nEnd (%d)< nStart(%d)\n",nMediaTime,nEnd,nStart);
	}
#endif//
}

bool CPDSessionWrapper::CheckPDHeader()
{
	SPDInitParam* pInitParam = GetInitParam();
	if(0 == pInitParam->nTempFileLocation)	//storage to memory
		return false;
#ifdef UNICODE
	wcstombs(m_fileName,pInitParam->mFilePath,MAX_FNAME_LEN);
#else
	tcscpy(&m_fileName,pInitParam->mFilePath);
#endif
	//FILE* hFile = CreateFile(pInitParam->mFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
	//	OPEN_EXISTING, 0, NULL);
	FILE* hFile = fopen(m_fileName,"a");
	if(NULL== hFile)
		return false;

	PDHChunk pc;
	DWORD dwReaded = 0;
	//if(!ReadFile(hFile, &pc, PD_Header_Chunk_Size, &dwReaded, NULL) || dwReaded < PD_Header_Chunk_Size || FOURCC_PD_Header != pc.fcc)
	dwReaded=fread(&pc,PD_Header_Chunk_Size,1,hFile);
	if(dwReaded < PD_Header_Chunk_Size || FOURCC_PD_Header != pc.fcc)
	{
		//CloseHandle(hFile);
		fclose(hFile);
		return false;
	}

	PBYTE pPDHeader = new BYTE[pc.size];
	if(!pPDHeader)
		return false;
	memcpy(pPDHeader, &pc, PD_Header_Chunk_Size);
	dwReaded=fread(pPDHeader + PD_Header_Chunk_Size,pc.size - PD_Header_Chunk_Size,1,hFile);
	//if(!ReadFile(hFile, pPDHeader + PD_Header_Chunk_Size, pc.size - PD_Header_Chunk_Size, &dwReaded, NULL) || dwReaded < pc.size - PD_Header_Chunk_Size)
	if(dwReaded < pc.size - PD_Header_Chunk_Size)
	{
		delete [] pPDHeader;
		fclose(hFile);
		return false;
	}

	bool bRet = CheckBuffer(pPDHeader, pc.size);
	if(!bRet)
	{
		memset(pPDHeader, 0, pc.size);
		//SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		fseek(hFile,0,SEEK_SET);
		//WriteFile(hFile, pPDHeader, pc.size, &dwReaded, NULL);
		fwrite(pPDHeader,pc.size,1,hFile);
	}

	delete [] pPDHeader;
	fclose(hFile);
	return bRet;
}

DWORD CPDSessionWrapper::GetPDHeaderRemainSize(bool bDF)
{
	if(m_bNoWritePDHeader)
		return 0;
	DWORD dwHeaderSize = PD_Header_Chunk_Size;
	//source file url
	dwHeaderSize += (PD_Header_Chunk_Size + _tcsclen(m_url) );

	//download time
	dwHeaderSize += (PD_Header_Chunk_Size + sizeof(SYSTEMTIME));

	//source file length
	dwHeaderSize += (PD_Header_Chunk_Size + sizeof(DWORD));

	//file mapping
	if(bDF)
		dwHeaderSize += (PD_Header_Chunk_Size + sizeof(DWORD) + VO_REMAIN_FILE_FRAGMENT_COUNT * sizeof(PDHFileFragment));
	else
		dwHeaderSize += (PD_Header_Chunk_Size + sizeof(PDHFileMapping2));

	return dwHeaderSize;
}

bool CPDSessionWrapper::WritePDHeader()
{
	DumpLog("CPDSessionWrapper::WritePDHeader()\r\n");
	if(m_bNoWritePDHeader)
		return true;

	SPDInitParam* pInitParam = GetInitParam();
	if(0 == pInitParam->nTempFileLocation)
		return true;

	//compute size
	DWORD dwHeaderSize = PD_Header_Chunk_Size;
	//source file url
	dwHeaderSize += (PD_Header_Chunk_Size + _tcsclen(m_url) );

	//download time
	dwHeaderSize += (PD_Header_Chunk_Size + sizeof(SYSTEMTIME));

	//source file length
	dwHeaderSize += (PD_Header_Chunk_Size + sizeof(DWORD));

	//file mapping
	DWORD dwFragNum = 0;
	if(m_NetGet.GetDFInfo())
	{
		CDFInfo* pDFInfo = m_NetGet.GetDFInfo();
		dwFragNum = pDFInfo->GetFragmentNum();
		dwHeaderSize += (PD_Header_Chunk_Size + sizeof(DWORD) + dwFragNum * sizeof(PDHFileFragment));
		DWORD dwPDHeaderRemainSize = GetPDHeaderRemainSize(true);
		if(dwHeaderSize < dwPDHeaderRemainSize)
			dwHeaderSize = dwPDHeaderRemainSize;
	}
	else if(m_NetGet.GetDFInfo2())
		dwHeaderSize += (PD_Header_Chunk_Size + sizeof(PDHFileMapping2));

	PBYTE pPDHeader = new BYTE[dwHeaderSize];
	if(!pPDHeader)
		return false;
	memset(pPDHeader, 0, dwHeaderSize);

	PDHChunk pc;
	//generate PD header information
	//PD Header
	PBYTE pTmp = pPDHeader;
	pc.fcc = FOURCC_PD_Header;
	pc.size = dwHeaderSize;
	memcpy(pTmp, &pc, PD_Header_Chunk_Size);
	pTmp += PD_Header_Chunk_Size;

	//source file url
	pc.fcc = FOURCC_PD_Source_File_Url;
	pc.size = PD_Header_Chunk_Size + _tcsclen(m_url);
	memcpy(pTmp, &pc, PD_Header_Chunk_Size);
	pTmp += PD_Header_Chunk_Size;
	memcpy(pTmp, m_url, _tcsclen(m_url));
	pTmp +=_tcsclen(m_url);

#if 1//
	//download time
	int downLoadTime=voGetCurrentTime();
	int  timeSize=sizeof(long);//strlen(timeBuf);
	pc.fcc = FOURCC_PD_Download_Time;
	pc.size = PD_Header_Chunk_Size +timeSize ;
	memcpy(pTmp, &pc, PD_Header_Chunk_Size);
	pTmp += PD_Header_Chunk_Size;
	//SYSTEMTIME st;
	//GetSystemTime(&st);
	//memcpy(pTmp, &st, sizeof(SYSTEMTIME));
	//pTmp += sizeof(SYSTEMTIME);
	memcpy(pTmp,&downLoadTime,timeSize);
	pTmp+=timeSize;
#endif
	if(m_NetGet.GetDFInfo())
	{
		CDFInfo* pDFInfo = m_NetGet.GetDFInfo();
		//source file length
		pc.fcc = FOURCC_PD_Source_File_Length;
		pc.size = PD_Header_Chunk_Size + sizeof(DWORD);
		memcpy(pTmp, &pc, PD_Header_Chunk_Size);
		pTmp += PD_Header_Chunk_Size;
		DWORD dwFileSize = pDFInfo->GetFileSize();
		memcpy(pTmp, &dwFileSize, sizeof(DWORD));
		pTmp += sizeof(DWORD);

		//file mapping
		pc.fcc = FOURCC_PD_DFInfo;
		pc.size = PD_Header_Chunk_Size + sizeof(DWORD) + dwFragNum * sizeof(PDHFileFragment);
		memcpy(pTmp, &pc, PD_Header_Chunk_Size);
		pTmp += PD_Header_Chunk_Size;
		memcpy(pTmp, &dwFragNum, sizeof(DWORD));
		pTmp += sizeof(DWORD);

		pDFInfo->WriteOut(pTmp);
	}
	else if(m_NetGet.GetDFInfo2())
	{
		CDFInfo2* pDFInfo2 = m_NetGet.GetDFInfo2();
		//source file length
		pc.fcc = FOURCC_PD_Source_File_Length;
		pc.size = PD_Header_Chunk_Size + sizeof(DWORD);
		memcpy(pTmp, &pc, PD_Header_Chunk_Size);
		pTmp += PD_Header_Chunk_Size;
		DWORD dwFileSize = pDFInfo2->GetFileSize();
		memcpy(pTmp, &dwFileSize, sizeof(DWORD));
		pTmp += sizeof(DWORD);

		//file mapping
		pc.fcc = FOURCC_PD_DFInfo2;
		pc.size = PD_Header_Chunk_Size + sizeof(PDHFileMapping2);
		memcpy(pTmp, &pc, PD_Header_Chunk_Size);
		pTmp += PD_Header_Chunk_Size;

		pDFInfo2->WriteOut(pTmp);
	}

	//write to file header
	bool bRet = WriteBuffer(pPDHeader, dwHeaderSize, m_NetGet.GetPDHeaderSize());
	delete [] pPDHeader;
	return bRet;
}

bool CPDSessionWrapper::CheckBuffer(PBYTE pBuffer, DWORD dwSize)
{
	PDHChunk pc;
	//PD Header
	PBYTE pTmp = pBuffer;
	memcpy(&pc, pTmp, PD_Header_Chunk_Size);
	pTmp += PD_Header_Chunk_Size;
	if(FOURCC_PD_Header != pc.fcc || dwSize != pc.size)
		return false;

	DWORD dwPDHeaderSize = pc.size;
	DWORD dwFileSize = 0;
	DWORD dwFragNum = 0;
	bool bExit = false;
	while(pTmp - pBuffer < dwSize && !bExit)
	{
		memcpy(&pc, pTmp, PD_Header_Chunk_Size);
		pTmp += PD_Header_Chunk_Size;

		switch(pc.fcc)
		{
		case FOURCC_PD_Source_File_Url:			//source file url
			{
				TCHAR* pszFileSource = new TCHAR[(pc.size - PD_Header_Chunk_Size) / sizeof(TCHAR) + 1];
				if(!pszFileSource)
					return false;
				memcpy(pszFileSource, pTmp, pc.size - PD_Header_Chunk_Size);
				pszFileSource[(pc.size - PD_Header_Chunk_Size) / sizeof(TCHAR)] = _T('\0');
				//TO_DO
				if(_tcscmp(m_url, pszFileSource))	//not this url
				{
					delete [] pszFileSource;
					return false;
				}
				delete [] pszFileSource;
			}
			break;

		case FOURCC_PD_Download_Time:			//download time
			{
				
				long stFile;
				memcpy(&stFile, pTmp, sizeof(long));
				long st=voGetCurrentTime();
				const long dayms=24*60*60*1000;
				int nDay = (st - stFile+dayms/2) / dayms;
				if(nDay > 30)
					return false;
				return true;

			}
			break;

		case FOURCC_PD_Source_File_Length:		//source file length
			{
				memcpy(&dwFileSize, pTmp, sizeof(DWORD));
				SPDInitParam* pInitParam = GetInitParam();
				if(dwFileSize + dwSize > pInitParam->nMaxBuffer * 1024)
					return false;
			}
		    break;

		case FOURCC_PD_DFInfo:					//file mapping 1
			{
				memcpy(&dwFragNum, pTmp, sizeof(DWORD));

				m_NetGet.ReadInDFInfo(pTmp + sizeof(DWORD), dwFragNum);
				CDFInfo* pDFInfo = m_NetGet.GetDFInfo();
				pDFInfo->SetFileSize(dwFileSize);
			}
		    break;

		case FOURCC_PD_DFInfo2:					//file mapping 2
			{
				return false;
			}
			break;

		default:
			bExit = true;
		    break;
		}

		if(!bExit)
			pTmp += (pc.size - PD_Header_Chunk_Size);
	}

	//connect server will waste much time, so we do not check it;
	//we can check it after start download in the future.
//	if(dwFileSize != m_NetGet.GetSourceFileLenWithoutDownload())
//		return false;
	if(m_bNoWritePDHeader==false)
		m_NetGet.SetPDHeaderSize(dwPDHeaderSize);
	if(dwFragNum == 1)
	{
		CDFInfo* pDFInfo = m_NetGet.GetDFInfo();
		if(pDFInfo)
		{
			DWORD dwStart = 0, dwLen = 0;
			if(!pDFInfo->GetBlankFragment(dwStart, dwLen))
				m_bNoWritePDHeader = true;
		}
	}
	return true;
}

bool CPDSessionWrapper::WriteBuffer(PBYTE pBuffer, DWORD dwSize, DWORD dwFileStartCopy /* = 0 */)
{
	SPDInitParam* pInitParam = GetInitParam();
	///HANDLE hFileRead = CreateFile(pInitParam->mFilePath, GENERIC_READ, FILE_SHARE_WRITE, NULL, 
	//	OPEN_ALWAYS, 0, NULL);
#ifdef UNICODE
	wcstombs(m_fileName,pInitParam->mFilePath,MAX_FNAME_LEN);
#else
	tcscpy(&m_fileName,pInitParam->mFilePath);
#endif
	FILE* hFileRead = fopen(m_fileName,"rb");
	if(NULL == hFileRead)
		return false;

	//HANDLE hFileWrite = CreateFile(pInitParam->mFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
	//	OPEN_ALWAYS, 0, NULL);
	FILE* hFileWrite = fopen(m_fileName,"wb");
	if(NULL == hFileWrite)
	{
		fclose(hFileRead);
		return false;
	}

	DWORD dwTmpFileBuffer = MAX(dwSize, 65536);
	PBYTE pTmpFileBuffer = new BYTE[dwTmpFileBuffer];
	if(!pTmpFileBuffer)
	{
		fclose(hFileRead);
		fclose(hFileWrite);
		return false;
	}

	DWORD dwReaded = 0, dwWrited = 0;
	if(dwFileStartCopy >= dwSize)
	{
		//WriteFile(hFileWrite, pBuffer, dwSize, &dwWrited, NULL);
		dwWrited=fwrite(pBuffer,dwSize,1,hFileWrite);

/*		if(dwFileStartCopy > dwSize)
		{
			//Ë³Ðòcopy
			SetFilePointer(hFileRead, dwFileStartCopy, NULL, FILE_BEGIN);
			while(true)
			{
				if(!ReadFile(hFileRead, pTmpFileBuffer, dwTmpFileBuffer, &dwReaded, NULL))
					break;

				WriteFile(hFileWrite, pTmpFileBuffer, dwReaded, &dwWrited, NULL);

				if(dwReaded < dwTmpFileBuffer)
					break;
			}
			SetEndOfFile(hFileWrite);
		}*/
	}
	else
	{
		DWORD dwOffset = dwSize - dwFileStartCopy;

		DWORD dwFileLen = ftell(hFileRead);//GetFileSize(hFileRead, NULL);
		//SetFilePointer(hFileWrite, dwFileLen + dwOffset, NULL, FILE_BEGIN);
		fseek(hFileRead,dwFileLen,SEEK_SET);
		//SetEndOfFile(hFileWrite);
		fseek(hFileWrite,0,SEEK_END);
		//ÄæÐòcopy
		DWORD dwLeft = dwFileLen - dwFileStartCopy;
		DWORD dwToProcess = 0;
		DWORD dwLastFilePos = dwFileLen;
		while(dwLeft > 0)
		{
			dwToProcess = MIN(dwLeft, dwTmpFileBuffer);

			//SetFilePointer(hFileRead, dwLastFilePos - dwToProcess, NULL, FILE_BEGIN);
			fseek(hFileRead,dwLastFilePos - dwToProcess,SEEK_SET);
			dwReaded=fread(pTmpFileBuffer, dwToProcess,1,hFileRead);
			//if(!ReadFile(hFileRead, pTmpFileBuffer, dwToProcess, &dwReaded, NULL))
			if(ferror(hFileRead))
				break;

			//SetFilePointer(hFileWrite, dwOffset + dwLastFilePos - dwToProcess, NULL, FILE_BEGIN);
			fseek(hFileWrite,dwOffset + dwLastFilePos - dwToProcess,SEEK_SET);
			//WriteFile(hFileWrite, pTmpFileBuffer, dwReaded, &dwWrited, NULL);
			dwWrited=fwrite( pTmpFileBuffer, dwReaded,1,hFileWrite);
			dwLeft -= dwToProcess;
			dwLastFilePos -= dwToProcess;
		}

		//SetFilePointer(hFileWrite, 0, NULL, FILE_BEGIN);
		fseek(hFileWrite,0,SEEK_SET);
		//WriteFile(hFileWrite, pBuffer, dwSize, &dwWrited, NULL);
		dwWrited=fwrite( pBuffer, dwSize,1,hFileWrite);
	}

	delete [] pTmpFileBuffer;
	fclose(hFileRead);
	fclose(hFileWrite);
	return true;
}

bool CPDSessionWrapper::GetCurrPlayFilePos(int* pnFilePos,int startTime)
{
	int nFilePos = MAXLONG;
	int nTmpFilePos = 0;
	CPDTrackWrapper* pTracks[2] = {m_pAudioTrack, m_pVideoTrack};
	for(int i = 0; i < 2; i++)
	{
		if(pTracks[i])
		{
			pTracks[i]->SetPos(startTime);
			nTmpFilePos = pTracks[i]->GetCurrPlayFilePos();
			if(nTmpFilePos < nFilePos)
				nFilePos = nTmpFilePos;
		}
	}

	if(pnFilePos)
		*pnFilePos = nFilePos;

	return true;
}

DWORD CPDSessionWrapper::GetCurrBufferingTime()
{
	SPDInitParam* pInitParam = GetInitParam();
	DWORD dwBytesPerSec = 0;
	if(m_NetGet.GetParam(VOID_NETDOWN_BYTES_PER_SEC, (long*)&dwBytesPerSec))
	{
		if(g_nLog)
		{
			char sz[256];
			sprintf(sz, "[CPDSessionWrapper::GetCurrBufferingTime]throughput: %d, clip bitrate: %d\r\n", dwBytesPerSec, m_dwClipBitrate);
			DumpLog(sz);
		}
		if(m_dwClipBitrate >= dwBytesPerSec)
		{
			DWORD dwBufferingTime = pInitParam->nPlayBufferTime;
			dwBufferingTime *= 1000;
			dwBufferingTime = dwBufferingTime / (1 + (float(dwBytesPerSec) / m_dwClipBitrate));
			if(g_nLog)
			{
				char sz[128];
				sprintf(sz, "[CPDSessionWrapper::GetCurrBufferingTime]buffering time: %d\r\n", dwBufferingTime);
				DumpLog(sz);
			}
			return dwBufferingTime;
		}
		else
			return (pInitParam->nBufferTime * 1000);
	}
	else
		return (pInitParam->nBufferTime * 1000);
}



int VOFILEAPI  voPDTrackGetFPByIndex(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, int nIndex)
{
	if(pPDSession==NULL)
		return VORC_FILEREAD_NULL_POINTER;
	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	return pPDSessionWrapper->TrackGetFilePosByIndex(pPDTrack, nIndex);
}

VOFILEREADRETURNCODE VOFILEAPI voPDSessionCreate(HVOFILEREAD * ppPDSession,void* userData)
{
	CPDSessionWrapper * pPDSession = new CPDSessionWrapper(userData);
	if(pPDSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;
	*ppPDSession = pPDSession;
	return VORC_FILEREAD_OK;
}

VOFILEREADRETURNCODE VOFILEAPI voPDSessionOpenURL(HVOFILEREAD pPDSession, TCHAR * pPDLink, TFileReaderAPI* pFileReader)
{
	if(pPDSession==NULL)
		return VORC_FILEREAD_NULL_POINTER;
	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	pPDSessionWrapper->SetReaderAPI(pFileReader);
	if(pPDSessionWrapper->OpenSource((TCHAR*)pPDLink)==0)
	{
		return VORC_FILEREAD_OK;
	}
	else
	{
		return VORC_FILEREAD_OPEN_FAILED;
	}
}

VOFILEREADRETURNCODE VOFILEAPI voPDSessionClose(HVOFILEREAD pPDSession)
{
	if(pPDSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	SAFE_DELETE(pPDSessionWrapper);
	return VORC_FILEREAD_OK;
}

VOFILEREADRETURNCODE VOFILEAPI voPDSessionSetParameter(HVOFILEREAD pPDSession, LONG lID, LONG lValue) 
{
	if(pPDSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	return (VOFILEREADRETURNCODE)pPDSessionWrapper->PDSetParam(lID,lValue);
}

VOFILEREADRETURNCODE VOFILEAPI voPDSessionGetParameter(HVOFILEREAD pPDSession, LONG lID, LONG * plValue) 
{
	if(pPDSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	return (VOFILEREADRETURNCODE) pPDSessionWrapper->PDGetParam(lID,plValue);
}

VOFILEREADRETURNCODE VOFILEAPI voPDTrackOpen(HVOFILETRACK * ppPDTrack, HVOFILEREAD pPDSession, int nIndex) 
{
	if(pPDSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	return (VOFILEREADRETURNCODE)pPDSessionWrapper->TrackOpen(ppPDTrack,nIndex);

}

VOFILEREADRETURNCODE VOFILEAPI voPDTrackClose(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack) 
{
	if(pPDSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	return (VOFILEREADRETURNCODE)pPDSessionWrapper->TrackClose(pPDTrack);
}

VOFILEREADRETURNCODE VOFILEAPI voPDTrackSetParameter(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, LONG lID, LONG lValue)
{
	if(pPDSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	return (VOFILEREADRETURNCODE)pPDSessionWrapper->TrackSetParam(pPDTrack,lID,lValue);
}

VOFILEREADRETURNCODE VOFILEAPI voPDTrackGetParameter(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, LONG lID, LONG * plValue)
{
	if(pPDSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	return (VOFILEREADRETURNCODE)pPDSessionWrapper->TrackGetParam(pPDTrack,lID,plValue);
}

VOFILEREADRETURNCODE VOFILEAPI voPDTrackInfo(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, VOTRACKINFO * pTrackInfo)
{
	VOFILEREADRETURNCODE rc=VORC_FILEREAD_NULL_POINTER;
	if(pPDSession == NULL)
		return rc;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	CPDTrackWrapper* track=pPDSessionWrapper->GetTrackWrapper(pPDTrack);
	if(track)
		rc= (VOFILEREADRETURNCODE)track->GetInfo(pTrackInfo);
	return rc;
}

VOFILEREADRETURNCODE VOFILEAPI voPDTrackGetSampleByIndex(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, VOSAMPLEINFO * pSampleInfo)
{
	VOFILEREADRETURNCODE rc=VORC_FILEREAD_NULL_POINTER;
	if(pPDSession == NULL)
		return rc;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	CPDTrackWrapper* track=pPDSessionWrapper->GetTrackWrapper(pPDTrack);
	if(track)
		rc= (VOFILEREADRETURNCODE)track->GetSampleByIndex(pSampleInfo);
	return rc;
}
int VOFILEAPI	voPDTrackGetSampleByTime(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, VOSAMPLEINFO *pSampleInfo)
{
	VOFILEREADRETURNCODE rc=VORC_FILEREAD_NULL_POINTER;
	if(pPDSession == NULL)
		return rc;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	CPDTrackWrapper* track=pPDSessionWrapper->GetTrackWrapper(pPDTrack);
	if(track)
		rc= (VOFILEREADRETURNCODE)track->GetSampleByTime(pSampleInfo);
	return rc;
}

int VOFILEAPI voPDTrackGetNextKeyFrame(HVOFILEREAD pPDSession, HVOFILETRACK pPDTrack, int nIndex, int nDirectionFlag)
{

	if(pPDSession == NULL)
		return VORC_FILEREAD_NULL_POINTER;

	CPDSessionWrapper * pPDSessionWrapper = (CPDSessionWrapper *)pPDSession;
	return (VOFILEREADRETURNCODE)pPDSessionWrapper->TrackGetNextKeySample(pPDTrack,nIndex,nDirectionFlag);

}

