#pragma once

#include "filebase.h"
#include "CNetGet.h"
#include "voPDEngineSDK.h"
#define E_VO_USER_ABORT				((HRESULT)0x81000001L)

#ifdef _WIN32_WCE
#define _FUNC _T
#else
#define _FUNC
#endif // _WIN32_WCE

class CPDTrackWrapper;
class CPDSessionWrapper:public IPDSource
{
public:
	CPDSessionWrapper(void* userData);
	virtual ~CPDSessionWrapper();

public:
	virtual HRESULT	OpenSource(TCHAR* pFile);
	virtual bool	Close();

	virtual CPDTrackWrapper*		GetVideoTrack() {return m_pVideoTrack;}
	virtual CPDTrackWrapper*		GetAudioTrack() {return m_pAudioTrack;}

	virtual DWORD	PDSetParam(LONG nID, LONG lValue);
	virtual DWORD	PDGetParam(LONG nID, LONG* plValue);

	virtual DWORD	TrackSetParam(HVOFILETRACK pPDTrack, LONG nID, LONG lValue);
	virtual DWORD	TrackGetParam(HVOFILETRACK pPDTrack, LONG nID, LONG* plValue);

	virtual DWORD TrackOpen(HVOFILETRACK* pPDTrack,int nTrackIndex);
	virtual DWORD TrackClose(HVOFILETRACK pPDTrack);
	virtual DWORD	TrackGetInfo(HVOFILETRACK pPDTrack, VOTRACKINFO* pTrackInfo);
	virtual DWORD	TrackGetSampleByIndex(HVOFILETRACK pPDTrack, VOSAMPLEINFO* pSampleInfo);
	virtual DWORD	TrackGetSampleByTime(HVOFILETRACK pPDTrack, VOSAMPLEINFO* pSampleInfo);
	virtual int		TrackGetNextKeySample(HVOFILETRACK pPDTrack, int nIndex, int nDirectionFlag);
	virtual int		TrackGetFilePosByIndex(HVOFILETRACK pPDTrack, int nIndex);
	
	virtual	void	OnDataLack();
	//bDF: file size < max buffer size
	virtual	DWORD	GetPDHeaderRemainSize(bool bDF);

	SPDInitParam*	GetInitParam(){return &m_initParam;};

protected:
	virtual bool	Stop();
	virtual bool	Pause();
	virtual bool	Run();
	virtual bool	Start();

	virtual bool	SetPos(long start);
	virtual bool	CheckPDHeader();
	virtual bool	CheckBuffer(PBYTE pBuffer, DWORD dwSize);
	virtual bool	WritePDHeader();
	virtual bool	WriteBuffer(PBYTE pBuffer, DWORD dwSize, DWORD dwFileStartCopy = 0);

	virtual bool	GetCurrPlayFilePos(int* pnFilePos,int startTime=0);
	virtual DWORD	GetCurrBufferingTime();
public:
	CPDTrackWrapper*			GetTrackWrapper(HVOFILETRACK hTrack);

protected:
	
	VOFILEOP			m_opFile;
	DWORD				m_dwDuration;

	CPDTrackWrapper*			m_pAudioTrack;
	CPDTrackWrapper*			m_pVideoTrack;
	//TO_DO
	SPDInitParam					m_initParam;
	VONETDOWNTYPE			m_downLoadType;
	long										m_downLoadThreadPriority;
	TCHAR*								m_url;
protected:
	HVOFILEREAD					m_hSource;
	VOFILEOPEN						m_fPDOpen;
	VOFILECLOSE						m_fPDClose;
	VOFILESETPARAMETER				m_fPDSetParam;
	VOFILEGETPARAMETER				m_fPDGetParam;
	VOFILEFP2MT						m_fPDFilePos2MediaTime;
	VOFILEMT2FP						m_fPDMediaTime2FilePos;
	VOTRACKOPEN						m_fTrackOpen;
	VOTRACKCLOSE					m_fTrackClose;
	VOTRACKSETPARAMETER				m_fTrackSetParam;
	VOTRACKGETPARAMETER				m_fTrackGetParam;
	VOTRACKGETINFO					m_fTrackGetInfo;
	VOTRACKGETSAMPLEBYINDEX			m_fGetSampleByIndex;
	VOTRACKGETSAMPLEBYTIME			m_fGetSampleByTime; 
	VOTRACKGETNEXTKEYFRAME			m_fGetNextKeyFrame;
	VOTRACKGETFPBYINDEX				m_fTrackGetFilePosByIndex;

protected:
	CNetGet2			m_NetGet;
	bool				m_bNoWritePDHeader;
	IVOMutex*			m_csRead;

	DWORD				m_dwClipBitrate;
	char			m_fileName[MAX_FNAME_LEN];
	long			m_currentTrackType;
public:
	VONETDOWNTYPE		GetDownloadType(){m_downLoadType;}
	void		SetReaderAPI(TFileReaderAPI* api);
	long			GetCurrentTrackType(){return m_currentTrackType;}
};
