#pragma once
#include "voPDPort.h"
#include "CDFInfo.h"
#include "CBaseSink.h"
#include "CBaseReadIn.h"
#include "CPDEventThread.h"
#include "CBuffering.h"
#include "CDownloader.h"
#include "voPDPort.h"
using namespace VOUtility;
void DumpLog(LPCSTR pLogInfo);


class CNetGet2:MEM_MANAGER
{
public:
	static bool DownloadCallback(long lEventID, long lParam, long lUserData);

public:
	CNetGet2(IPDSource* pSource);
	virtual ~CNetGet2();

public:
	bool			SetParam(long lID, long lValue);
	bool			GetParam(long lID, long* plValue);
	void			SetDownLoadType(VONETDOWNTYPE type){m_Downloader.SetDownLoadType(type);}
	bool			IsSupportSeek();
	bool			SetDownloadStartPos(DWORD dwPos);
	bool			SetDownloadStopPos(DWORD dwPos);
	bool			SetDownloadPos(DWORD dwStart);

	bool			SetUrl(LPCTSTR szUrl);
	bool			StartDownload();
	bool			IsLocalFileCreated();
	bool			IsFinish();

	bool			StartBuffering(int nStart, int nEnd, bool bForceSend, BUFFERINGTYPE btType, int nCurrFilePos);
	void			StopBuffering();
	int				GetBufferingPercent();
	bool			IsBuffering() {return (BT_NONE != m_Buffering.GetBufferingType());}
	int				GetCurrDownloadPercent();
	DWORD			GetCurrDownloadPos();

	bool			SetDownloadThreadPriority(int nPriority);

	bool			StartDownloadB(DWORD dwStart, DWORD dwLen, bool bUseThread = true);
	void			StopDownload();

	bool			SetFileHeaderSize(DWORD dwFileHeaderSize);
	DWORD			GetFileHeaderSize();

	void			UpdateFilePos(DWORD dwFilePos);
	void			UpdateDf2();

	CDFInfo*		GetDFInfo() {return m_pDfInfo;}
	CDFInfo2*		GetDFInfo2() {return m_pDfInfo2;}

	bool			SetLoadState(bool bLoad);
	void			NotifyEvent(long lEventCode, long lParam);

	bool			ReadInDFInfo(PBYTE pBuffer, DWORD dwSize);

	bool			InitByPDHeader();

	bool			SetPDHeaderSize(DWORD dwPDHeaderSize);
	DWORD			GetPDHeaderSize();

	bool			SetSinkType(int nType);
	bool			CloseSink();
	bool			GenerateReadIn(LPCTSTR szUrl, CBaseReadIn** ppReadIn);

	void			OnStartDownloadOk();

	//Background Thread Function
	bool			CreateBGThread();
	void			DestroyBGThread();
	bool			StartBGThread();
	void			StopBGThread();

	bool			Pause(bool bPause);

	SPDInitParam*	GetInitParam();

protected:
	bool			OnCreateFile(long lFileSize);
	bool			OnCmpleted();
	bool			OnDisconnected();
	bool			OnFileSink(PBYTE pBuffer, DWORD dwLen);

	bool			SetDownloadPosB(DWORD dwStart, DWORD dwLen);

protected:
	CBaseSink*			m_pSink;
	CDFInfo*			m_pDfInfo;
	CDFInfo2*			m_pDfInfo2;

	bool				m_bFinished;
	PNotifyEventFunc	m_pNotifyFunc;

	DWORD				m_dwCurrDownloadPos;
	int					m_nLocalFileState;

	CPDEventThread*		m_pBGThread;
	IVOMutex*			m_csBGThread;

	IPDSource*			m_pSource;
	DWORD				m_dwVideoFilePos;
	DWORD				m_dwAudioFilePos;

	bool				m_bLoadState;

	CBuffering			m_Buffering;
	DWORD				m_dwPDHeaderSize;

	PBYTE				m_pMemBuffer;
	DWORD				m_dwMemBuffer;

	IVOMutex*			m_csDownload;
	CDownloader			m_Downloader;

	bool				m_bPause;
	bool				m_bSupportSeek;
	long				m_eventCode;
public:
	long				GetLastEventCode(){return m_eventCode;}
};
