#include <voPDPort.h>
#include <stdio.h>

#include "filebase.h"
#include "macro.h"
#include "CNetGet.h"

//#include "CPDTrackWrapper2.h"
#include "CHttpDownloadFile.h"
#include "CFileSink.h"
#include "CMemSink.h"
#include "CFileReadIn.h"
#include "CMemReadIn.h"
#include "UFileReader.h"

enum
{
	LOCAL_FILE_UNPROCESS		= 0, 
	LOCAL_FILE_CREATE_FAIL		= 1, 
	LOCAL_FILE_CREATE_SUCCESS	= 2,
};

#ifdef R_TEST
const char DUMP_FILE_PATH[] = ("c:\\visualon\\vopd\\PDSource.txt");
#else//R_TEST
const char DUMP_FILE_PATH[] = ("\\PDSource.txt");
#endif//R_TEST

extern int g_nLog;
void WriteLogFile(LPCSTR pLogInfo)
{

	FILE* hFile = fopen(DUMP_FILE_PATH,"a+");//CreateFile(DUMP_FILE_PATH, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD)0, NULL);
	if(hFile)
	{
		fseek(hFile,0,SEEK_END);

		DWORD dwWrite = 0;
		fprintf(hFile, "%s",pLogInfo);//, strlen(pLogInfo), &dwWrite, NULL);
		fclose(hFile);
	}

}
#ifdef R_TEST
int	g_nLog = 1;
#else//R_TEST
int	g_nLog = 0;
#endif//R_TEST
void DumpLog(LPCSTR pLogInfo)
{
#ifdef _HTC
	if(g_nLog == 1)
	{
		char szTmp[256];
		sprintf(szTmp, "[PD][%d]%s", voGetCurrentTime(), pLogInfo);
		WriteLogFile(szTmp);
	}
	else if(g_nLog == 2)
	{
#ifdef _WIN32_WCE
		TCHAR szTmp[256];
		MultiByteToWideChar(CP_ACP, 0, pLogInfo, -1, szTmp, sizeof(szTmp));
		RETAILMSG(1, (L"[PD][%d]%s", voGetCurrentTime(), szTmp));
#endif
	}
#else//_HTC
	voLog(LL_DEBUG,"PDSource.txt",pLogInfo);
#endif//_HTC
	
}

void DumpErrLog(LPCSTR pLogInfo)
{
#ifdef _WIN32_WCE
	TCHAR szTmp[256];
	MultiByteToWideChar(CP_ACP, 0, pLogInfo, -1, szTmp, sizeof(szTmp));
	RETAILMSG(1, (L"[PD][%d]%s", voGetCurrentTime(), szTmp));
#endif
}

bool CNetGet2::DownloadCallback(long lEventID, long lParam, long lUserData)
{
	CNetGet2* pThis = (CNetGet2*)lUserData;
	if(!pThis || UFileReader::g_bCancel)
		return false;

	switch(lEventID)
	{
	case NETDOWN_CREATE_FILE:
		return pThis->OnCreateFile(lParam);
		break;

	case NETDOWN_COMPLETED:
		return pThis->OnCmpleted();
		break;

	case NETDOWN_DISCONNECTED:
		return pThis->OnDisconnected();
		break;

	case NETDOWN_FILE_SINK:
		{
			PNDBuffer pnb = (PNDBuffer)lParam;
			return pThis->OnFileSink(pnb->buffer, pnb->size);
		}
		break;
#if MULTI_PROTOCOL
	case VO_PD_EVENT_GET_ASX:
		pThis->NotifyEvent(lEventID,lParam);
		return true;
#endif//MULTI_PROTOCOL
	default:
		break;
	}

	return false;
}

CNetGet2::CNetGet2(IPDSource* pSource)
	: m_pSource(pSource)
	, m_nLocalFileState(LOCAL_FILE_UNPROCESS)
	, m_bFinished(false)
	, m_pNotifyFunc(NULL)
	, m_dwCurrDownloadPos(0)
	, m_pSink(NULL)
	, m_pMemBuffer(NULL)
	, m_dwMemBuffer(0)
	, m_pDfInfo(NULL)
	, m_pDfInfo2(NULL)
	, m_dwVideoFilePos(0)
	, m_dwAudioFilePos(0)
	, m_bLoadState(true)
	, m_Buffering(this)
	, m_dwPDHeaderSize(0)
	, m_Downloader(this)
	, m_pBGThread(NULL)
	, m_bPause(false)
	, m_bSupportSeek(true)
	,m_eventCode(0)
{
	CREATE_MUTEX(m_csBGThread);//=VOCPFactory::CreateOneMutex();
	CREATE_MUTEX(m_csDownload);//=VOCPFactory::CreateOneMutex();
	CreateBGThread();
}

CNetGet2::~CNetGet2()
{
	DestroyBGThread();
	SAFE_DELETE(m_pSink);
	SAFE_DELETE(m_pDfInfo);
	SAFE_DELETE(m_pDfInfo2);
	SAFE_DELETE(m_pNotifyFunc);
	VO_SAFE_DELETE(m_pMemBuffer);
	DELETE_MUTEX(m_csBGThread);
	DELETE_MUTEX(m_csDownload);
}

bool CNetGet2::CloseSink()
{
	if(m_pSink)
		m_pSink->Close();

	return true;
}

bool CNetGet2::SetParam(long lID, long lValue)
{
	if(VOID_FILEREAD_CALLBACK_NOTIFY == lID)
	{
		if(m_pNotifyFunc)
			delete m_pNotifyFunc;
		m_pNotifyFunc = new(MEM_CHECK) NotifyEventFunc;
		if(!m_pNotifyFunc)
			return false;
		PNotifyEventFunc pFunc = (PNotifyEventFunc)lValue;
		m_pNotifyFunc->funtcion = pFunc->funtcion;
		m_pNotifyFunc->parent = pFunc->parent;

		return true;
	}

	return m_Downloader.SetParam(lID, lValue);
}

bool CNetGet2::GetParam(long lID, long* plValue)
{
	return m_Downloader.GetParam(lID, plValue);
}

bool CNetGet2::SetDownloadStartPos(DWORD dwPos)
{
	return SetParam(VOID_NETDOWN_START_POS, dwPos);
}

bool CNetGet2::SetDownloadStopPos(DWORD dwPos)
{
	return SetParam(VOID_NETDOWN_END_POS, dwPos);
}

bool CNetGet2::IsSupportSeek()
{
	bool bSupport = true;
	if(GetParam(VOID_NETDOWN_SUPPORT_SEEK, (long*)&bSupport))
		m_bSupportSeek = bSupport;

	return m_bSupportSeek;
}

bool CNetGet2::OnCreateFile(long lFileSize)
{
	DumpLog("CNetGet2::OnCreateFile\r\n");
	SPDInitParam* pInitParam = GetInitParam();
	if(LOCAL_FILE_UNPROCESS == m_nLocalFileState)
	{
		DWORD dwPDHeader1 = (0 == pInitParam->nTempFileLocation) ? 0 : m_pSource->GetPDHeaderRemainSize(true);
		DWORD dwPDHeader2 = (0 == pInitParam->nTempFileLocation) ? 0 : m_pSource->GetPDHeaderRemainSize(false);

		if(lFileSize + dwPDHeader1 <= pInitParam->nMaxBuffer * 1024)	//DF
		{
			m_pDfInfo = new(MEM_CHECK) CDFInfo;
			if(!m_pDfInfo)
				return false;
			m_pDfInfo->SetFileSize(lFileSize);
		}
		else
		{
			m_pDfInfo2 = new(MEM_CHECK) CDFInfo2;
			if(!m_pDfInfo2)
				return false;
			m_pDfInfo2->SetFileSize(lFileSize);
			lFileSize = pInitParam->nMaxBuffer * 1024 - dwPDHeader2;
			m_pDfInfo2->SetContSize(lFileSize);
		}

		bool bRet = false;
		if(0 == pInitParam->nTempFileLocation)
		{
			if(m_pMemBuffer)
				VO_SAFE_DELETE(m_pMemBuffer);//delete [] m_pMemBuffer;
			m_pMemBuffer = (PBYTE)allocate(lFileSize, MEM_CHECK);//new BYTE[lFileSize];
			if(!m_pMemBuffer)
			{
				DumpLog("memory is not enough for create memory storage.\r\n");
#ifdef _HTC
				NotifyEvent(HP_EVENT_INSUFFICIENT_SPACE, NULL);
#else	//_HTC
				NotifyEvent(VO_PD_EVENT_INSUFFICIENT_SPACE, NULL);
#endif	//_HTC
				return false;
			}
			m_dwMemBuffer = lFileSize;

			VOMemSinkOpenParam param;
			param.memory_buffer = m_pMemBuffer;
			param.memory_buffer_size = lFileSize;

			bRet = m_pSink->Open(&param);
		}
		else if(1 == pInitParam->nTempFileLocation)
		{
			m_dwPDHeaderSize = (NULL != m_pDfInfo) ? dwPDHeader1 : dwPDHeader2;
			lFileSize += m_dwPDHeaderSize;

			VOFileSinkOpenParam param;
			param.file_path = pInitParam->mFilePath;
			param.file_size = lFileSize;
			param.if_create = true;

			bRet = m_pSink->Open(&param);

			if(bRet)
			{
				m_pSink->SetOffet(m_dwPDHeaderSize);
				m_pSink->SetSinkPos(0);
				CHttpDownloadFile::g_dwOffset = m_dwPDHeaderSize;
			}
		}

		if(bRet)
			m_nLocalFileState = LOCAL_FILE_CREATE_SUCCESS;
		else
		{
			DWORD dwErr = m_pSink->GetLastErr();
			if(VOPD_SINK_ERR_NO_ENOUGH_SPACE == dwErr)
			{
				DumpLog("disk space is not enough for create local file.\r\n");
#ifdef _HTC
				NotifyEvent(HP_EVENT_INSUFFICIENT_SPACE, NULL);
#else	//_HTC
				NotifyEvent(VO_PD_EVENT_INSUFFICIENT_SPACE, NULL);
#endif	//_HTC
			}
			else
			{
				voLog(LL_TRACE,"error.txt", "local file create file.[maybe local file name error] %d\n",GetLastError2());
#ifdef _HTC
				NotifyEvent(HP_EVENT_SDK_ERR, NULL);
#else	//_HTC
				NotifyEvent(VO_PD_EVENT_SDK_ERR, NULL);
#endif	//_HTC
			}
			
			m_nLocalFileState = LOCAL_FILE_CREATE_FAIL;

			return false;
		}
	}

	return true;
}

bool CNetGet2::OnFileSink(PBYTE pBuffer, DWORD dwLen)
{
	if(m_pDfInfo)		//small size file
	{
		DWORD dwStart = m_pSink->GetSinkPos();

		//write file
		if(!m_pSink->Sink(pBuffer, dwLen))
		{
			DumpLog("write local file fail.\r\n");
#ifdef _HTC
			NotifyEvent(HP_EVENT_SDK_ERR, NULL);
#else	//_HTC
			NotifyEvent(VO_PD_EVENT_SDK_ERR, NULL);
#endif	//_HTC
		}

		dwLen = m_pSink->GetSinkPos() - dwStart;

		m_pDfInfo->AddFragment(dwStart, dwLen);
		m_dwCurrDownloadPos = m_pSink->GetSinkPos();
		m_Buffering.SetCurrentDownloadFilePos(m_dwCurrDownloadPos);
	}
	else if(m_pDfInfo2)
	{
		if(!m_pDfInfo2->CanWrite(dwLen, NULL))
		{
			UpdateDf2();

			if(!m_pDfInfo2->CanWrite(dwLen, NULL))
			{
				if(m_bLoadState)
				{
#ifdef _HTC
					NotifyEvent(HP_EVENT_INSUFFICIENT_SPACE, NULL);
#else	//_HTC
					NotifyEvent(VO_PD_EVENT_INSUFFICIENT_SPACE, NULL);
#endif	//_HTC
					DumpLog("download file header need more disk space!\r\n");
				}
				else
				{
					if(!m_bPause)
					{
						if(StartBGThread())
							DumpLog("file content is full, start watch thread to start download when space is permit!\r\n");
					}

					//if current buffering, must stop it, or it will hang!!
					m_Buffering.Stop(true);
				}
				return false;
			}
		}

		//write file
		if(!m_pSink->Sink(pBuffer, dwLen))
		{
			DumpLog("write local file fail.\r\n");
#ifdef _HTC
			NotifyEvent(HP_EVENT_SDK_ERR, NULL);
#else	//_HTC
			NotifyEvent(VO_PD_EVENT_SDK_ERR, NULL);
#endif	//_HTC
		}
		m_pDfInfo2->AddFragment(dwLen);

		m_dwCurrDownloadPos = m_pDfInfo2->ContPos2FilePos(m_pSink->GetSinkPos() - m_pSink->GetFileHeaderSize());
		m_Buffering.SetCurrentDownloadFilePos(m_dwCurrDownloadPos);
	}

	return true;
}

bool CNetGet2::OnCmpleted()
{
	DumpLog("CNetGet2::OnCmpleted\r\n");
	if(m_pDfInfo)
	{
		DWORD dwStart = 0, dwLen = 0;
		if(m_pDfInfo->GetBlankFragment(dwStart, dwLen))
		{
			if(g_nLog)
			{
				char sz[256];
				sprintf(sz, "[OnCmpleted]SetDownloadPosB: start: %d, len: %d\r\n", dwStart, dwLen);
				DumpLog(sz);
			}
			SetDownloadPosB(dwStart, dwLen);
		}
		else
		{
			m_bFinished = true;
			m_pSink->Close();
		}
	}

	return true;
}

bool CNetGet2::OnDisconnected()
{
	DumpLog("OnDisconnected\r\n");
#ifdef _HTC
	NotifyEvent(HS_EVENT_CONNECT_FAIL, E_NETWORK_TIMEOUT);
#else	//_HTC
	NotifyEvent(VO_EVENT_CONNECT_FAIL, NULL);
#endif	//_HTC

	if(!m_bPause)
	{
		if(StartBGThread())
			DumpLog("network disconnected or timeout, start watch thread to start download when space is permit!\r\n");
	}

	return true;
}

bool CNetGet2::SetDownloadPos(DWORD dwStart)
{
	if(!IsSupportSeek())
		return false;

	if(m_pDfInfo)
	{
		DWORD dwLen = 0;
		if(!m_pDfInfo->GetBlankFragmentByStart(dwStart, dwLen))
			return false;

		StopBGThread();

		if(g_nLog)
		{
			char sz[256];
			sprintf(sz, "[SetPos]SetDownloadPosB: start: %d, len: %d\r\n", dwStart, dwLen);
			DumpLog(sz);
		}

		if(m_bPause)
		{
			DumpLog("SetPos when AP is PAUSE_DOWNLOAD!!\r\n");
			m_bPause = false;
		}
		return SetDownloadPosB(dwStart, dwLen);
	}
	else if(m_pDfInfo2)
	{
		if(m_pDfInfo2->InContent(dwStart))
		{
//			m_pDfInfo2->RemoveFragment(dwStart);
			return false;
		}

		StopBGThread();

		if(g_nLog)
		{
			char sz[256];
			sprintf(sz, "[SetPos]SetDownloadPosB: start: %d\r\n", dwStart);
			DumpLog(sz);
		}
		if(m_bPause)
		{
			DumpLog("SetPos when AP is PAUSE_DOWNLOAD!!\r\n");
			m_bPause = false;
		}
		return SetDownloadPosB(dwStart, MAXDWORD);
	}

	return false;
}

bool CNetGet2::SetDownloadPosB(DWORD dwStart, DWORD dwLen)
{
	CAutoLock lock(m_csDownload);
	m_Downloader.StopDownload();

	if(m_pDfInfo)
		m_pSink->SetSinkPos(dwStart);
	else if(m_pDfInfo2)
	{
		m_dwAudioFilePos = 0;
		m_dwVideoFilePos = 0;

		m_pDfInfo2->Reset(dwStart);
		m_pSink->SetSinkPos(m_pSink->GetFileHeaderSize());
	}

	if(g_nLog)
	{
		char sz[256];
		sprintf(sz, "set download position[start: %d, len: %d]\r\n", dwStart, dwLen);
		DumpLog(sz);
	}	

	if(!StartDownloadB(dwStart, dwLen))
		return false;

	return true;
}

bool CNetGet2::SetUrl(LPCTSTR szUrl)
{
	return m_Downloader.SetUrl(szUrl);
}

bool CNetGet2::StartDownload()
{
	m_eventCode = 0;
	m_nLocalFileState = LOCAL_FILE_UNPROCESS;

	if(!StartDownloadB(0, MAXDWORD, false))
		return false;

	for(int i = 0; i < 1000 && !UFileReader::g_bCancel; i++)
	{
		if(LOCAL_FILE_UNPROCESS != m_nLocalFileState)
			break;
		IVOThread::Sleep(10);
	}

	return (LOCAL_FILE_CREATE_SUCCESS == m_nLocalFileState);
}

bool CNetGet2::StartDownloadB(DWORD dwStart, DWORD dwLen, bool bUseThread /* = true */)
{
	return m_Downloader.StartDownload(dwStart, dwLen, bUseThread);
}

void CNetGet2::StopDownload()
{
	m_Downloader.StopDownload();
}

bool CNetGet2::StartBuffering(int nStart, int nEnd, bool bForceSend, BUFFERINGTYPE btType, int nCurrFilePos)
{
	if(nStart < 0)
	{
		DumpLog("buffer start is less than zero!!\r\n");
		nStart = 0;
	}

	DWORD dwFileSize = 0;
	if(m_pDfInfo)
		dwFileSize = m_pDfInfo->GetFileSize();
	else if(m_pDfInfo2)
		dwFileSize = m_pDfInfo2->GetFileSize();

	if(nEnd > dwFileSize)
	{
		DumpLog("buffer end is more than file size!!\r\n");
		nEnd = dwFileSize;
	}



	return m_Buffering.Start(nStart, nEnd, bForceSend, btType, nCurrFilePos);
}

void CNetGet2::StopBuffering()
{
	DumpLog("CNetGet2::StopBuffering\r\n");

	return m_Buffering.Stop();
}

int CNetGet2::GetBufferingPercent()
{
	return m_Buffering.GetPercent();
}

bool CNetGet2::SetDownloadThreadPriority(int nPriority)
{
	return m_Downloader.SetThreadPriority(nPriority);
}

int CNetGet2::GetCurrDownloadPercent()
{
	int percent=0;
	if(m_pDfInfo)
	{
		percent= m_pDfInfo->GetCurrPlayDFEnd() * 100 / m_pDfInfo->GetFileSize();
	}
	else if(m_pDfInfo2)
	{
		percent= m_dwCurrDownloadPos * 100 / m_pDfInfo2->GetFileSize();
	}
	voLog(LL_DEBUG,"seek.txt","@@CurrDownloadPercent=%d\n",percent);
	return percent;
}

DWORD CNetGet2::GetCurrDownloadPos()
{
	if(m_pDfInfo)
		return m_pDfInfo->GetCurrPlayDFEnd();
	else if(m_pDfInfo2)
		return m_dwCurrDownloadPos;
	return 0;
}

void CNetGet2::NotifyEvent(long lEventCode, long lParam)
{
#ifdef _HTC
	if(HS_EVENT_CONNECT_FAIL == lEventCode && lParam != E_NETWORK_TIMEOUT)
		m_bFinished = true;
#else	//_HTC
	if(VO_EVENT_CONNECT_FAIL == lEventCode && lParam != E_NETWORK_TIMEOUT)
		m_bFinished = true;
	if(lEventCode== VO_PD_EVENT_GET_ASX)
	{
		m_eventCode = lEventCode;
		voLog(1,"asx.txt",(char*)lParam);
	}
#endif	//_HTC
	voLog(LL_DEBUG,"event.txt","%d,param=%d\n",lEventCode,lParam);
	m_pSource->NotifyEvent(lEventCode,(void*)lParam);
	if(m_pNotifyFunc)
	{
		if(PDGlobalData::data.version==1)
		{
			if(lEventCode==VO_EVENT_BUFFERING_END)
			{
				long percent=100;
				m_pNotifyFunc->funtcion(VO_EVENT_BUFFERING_PERCENT,(long)&percent, (long*)m_pNotifyFunc->parent);
			}
			m_pNotifyFunc->funtcion(lEventCode, (long)&lParam, (long*)m_pNotifyFunc->parent);
		}
		else
			m_pNotifyFunc->funtcion(lEventCode, lParam, (long*)m_pNotifyFunc->parent);
	}
}

bool CNetGet2::IsLocalFileCreated()
{
	return (LOCAL_FILE_CREATE_SUCCESS == m_nLocalFileState);
}

bool CNetGet2::IsFinish()
{
	return m_bFinished;
}

bool CNetGet2::SetFileHeaderSize(DWORD dwFileHeaderSize)
{
	if(m_pDfInfo2)
	{
		m_pSink->SetFileHeaderSize(dwFileHeaderSize);
		m_pDfInfo2->RemoveHeader(dwFileHeaderSize);

		return true;
	}
	else
		return false;
}

DWORD CNetGet2::GetFileHeaderSize()
{
	return m_pSink ? m_pSink->GetFileHeaderSize() : 0;
}

void CNetGet2::UpdateFilePos(DWORD dwFilePos)
{
	if(m_pDfInfo2)
	{
		long  trackType= m_pSource->GetCurrentTrackType();
		if(trackType == TT_AUDIO )
		{
			m_dwAudioFilePos = dwFilePos;
			voLog(LL_TRACE,"audioTS.txt","updatefilepos=%d\n",dwFilePos);
		}
		else if(trackType == TT_VIDEO ) 
		{
			m_dwVideoFilePos = dwFilePos;
			voLog(LL_TRACE,"videoTS.txt","updatefilepos=%d\n",dwFilePos);
		}
		else
		{
			m_dwAudioFilePos =m_dwVideoFilePos = dwFilePos;
			voLog(LL_TRACE,"videoTS.txt","updatefilepos_0=%d\n",dwFilePos);
			voLog(LL_TRACE,"audioTS.txt","updatefilepos_0=%d\n",dwFilePos);
		}
	}
}

void CNetGet2::UpdateDf2()
{
	if(m_pDfInfo2)
		m_pDfInfo2->RemoveFragment(MIN(m_dwVideoFilePos, m_dwAudioFilePos));
}

bool CNetGet2::SetLoadState(bool bLoad)
{
	m_bLoadState = bLoad;
	return true;
}

bool CNetGet2::ReadInDFInfo(PBYTE pBuffer, DWORD dwSize)
{
	if(!m_pDfInfo)
		m_pDfInfo = new(MEM_CHECK) CDFInfo;

	if(!m_pDfInfo)
		return false;

	return m_pDfInfo->ReadIn(pBuffer, dwSize);
}

bool CNetGet2::InitByPDHeader()
{
	if(!m_pDfInfo)
		return false;

	SPDInitParam* pInitParam = GetInitParam();
	VOFileSinkOpenParam param;
	param.file_path = pInitParam->mFilePath;
	param.file_size = 0;
	param.if_create = false;
	if(!m_pSink)
		m_pSink = new(MEM_CHECK) CFileSink;
	if(!m_pSink)
		return false;
	if(m_pSink->Open(&param))
	{
		m_pSink->SetOffet(m_dwPDHeaderSize);
		CHttpDownloadFile::g_dwOffset = m_dwPDHeaderSize;
		m_pSink->SetFileSize(m_pDfInfo->GetFileSize() + m_dwPDHeaderSize);
		m_nLocalFileState = LOCAL_FILE_CREATE_SUCCESS;
	}
	else
	{
		DWORD dwErr = m_pSink->GetLastErr();
		if(VOPD_SINK_ERR_NO_ENOUGH_SPACE == dwErr)
		{
			DumpLog("disk space is not enough for create local file.\r\n");
#ifdef _HTC
			NotifyEvent(HP_EVENT_INSUFFICIENT_SPACE, NULL);
#else	//_HTC
			NotifyEvent(VO_PD_EVENT_INSUFFICIENT_SPACE, NULL);
#endif	//_HTC
		}
		else
		{
			DumpLog("local file create file.[maybe local file name error]\r\n");
#ifdef _HTC
			NotifyEvent(HP_EVENT_SDK_ERR, NULL);
#else	//_HTC
			NotifyEvent(VO_PD_EVENT_SDK_ERR, NULL);
#endif	//_HTC
		}

		m_nLocalFileState = LOCAL_FILE_CREATE_FAIL;

		return false;
	}

	return OnCmpleted();
}

bool CNetGet2::SetPDHeaderSize(DWORD dwPDHeaderSize)
{
	m_dwPDHeaderSize = dwPDHeaderSize;
	return true;
}

DWORD CNetGet2::GetPDHeaderSize()
{
	return m_dwPDHeaderSize;
}

bool CNetGet2::SetSinkType(int nType)
{
	SAFE_DELETE(m_pSink);

	if(0 == nType)
		m_pSink = new(MEM_CHECK) CMemSink;
	else if(1 == nType)
		m_pSink = new(MEM_CHECK) CFileSink;
	else
		return false;

	return (NULL != m_pSink);
}

bool CNetGet2::GenerateReadIn(LPCTSTR szUrl, CBaseReadIn** ppReadIn)
{
	if(LOCAL_FILE_CREATE_SUCCESS != m_nLocalFileState)
		return false;

	SPDInitParam* pInitParam = GetInitParam();
	if(0 == pInitParam->nTempFileLocation)		//memory
	{
		CMemReadIn* pReadIn = new(MEM_CHECK) CMemReadIn;
		if(!pReadIn)
			return false;

		VOMemReadInOpenParam param;
		param.memory_buffer = m_pMemBuffer;
		param.memory_buffer_size = m_dwMemBuffer;

		if(!pReadIn->Open(&param))
		{
			delete pReadIn;
			return false;
		}

		*ppReadIn = pReadIn;
		return true;
	}
	else if(1 == pInitParam->nTempFileLocation)	//file
	{
		CFileReadIn* pReadIn = new(MEM_CHECK) CFileReadIn;
		if(!pReadIn)
			return false;

		if(!pReadIn->Open(pInitParam->mFilePath))
		{
			delete pReadIn;
			return false;
		}

		*ppReadIn = pReadIn;
		return true;
	}
	else
		return false;
}

void CNetGet2::OnStartDownloadOk()
{
	m_bFinished = false;
}

bool CNetGet2::CreateBGThread()
{
	CAutoLock lock(m_csBGThread);
	if(m_pBGThread)
		return true;
	m_pBGThread = new(MEM_CHECK) CPDEventThread;
	return (NULL != m_pBGThread);
}

void CNetGet2::DestroyBGThread()
{
	CAutoLock lock(m_csBGThread);
	SAFE_DELETE(m_pBGThread);
}

bool CNetGet2::StartBGThread()
{
	CAutoLock lock(m_csBGThread);
	if(!UFileReader::g_bCancel && m_pBGThread)
		return m_pBGThread->Create(this);

	return false;
}

void CNetGet2::StopBGThread()
{
	CAutoLock lock(m_csBGThread);
	if(m_pBGThread)
		m_pBGThread->Destroy();
}

bool CNetGet2::Pause(bool bPause)
{
	if(m_bPause == bPause)	//state not change!!
		return true;

	m_bPause = bPause;
	if(m_bPause)
	{
		StopBGThread();
		StopDownload();
	}
	else
		StartBGThread();

	return true;
}

SPDInitParam* CNetGet2::GetInitParam()
{
	return m_pSource ? m_pSource->GetInitParam() : NULL;
}