#pragma once

#include "filebase.h"
#include "CNetGet.h"
#include "voPDEngineSDK.h"
#include "voPD.h"
#include "IReaderLoader.h"
#define E_VO_USER_ABORT				((HRESULT)0x81000001L)

#ifdef _WIN32_WCE
#define _FUNC _T
#else
#define _FUNC
#endif // _WIN32_WCE

#include "CPDTrackWrapper2.h"
#define MAX_TRACK_COUNT 4
#define DELAY_OPEN_URL 1
class CPDSessionWrapper2:public IPDSource,MEM_MANAGER
{
	VO_SOURCE_OPENPARAM		m_param;
	VO_SOURCE_INFO						m_sourceInfo;
	VO_SOURCE_READAPI				m_newReaderAPI;
	CPDTrackWrapper2*						m_tracks[MAX_TRACK_COUNT];
	IReaderLoader*									m_dllLoader;
	bool												m_isLoading;
	int													m_nAutoExtendBufTime;
public:
	CPDSessionWrapper2(VO_SOURCE_OPENPARAM * pParam);
	virtual ~CPDSessionWrapper2();

public:
	virtual HRESULT	OpenSource(TCHAR* pFile);
	virtual bool	Close();
	virtual DWORD	PDSetParam(LONG nID, void* lValue);
	virtual DWORD	PDGetParam(LONG nID, void* plValue);
	int 	OpenURL()
	{
		m_isLoading = true;
		int ret= OpenSource((TCHAR *) m_param.pSource);
		m_isLoading = false;
		voLog(LL_DEBUG,"PDSource.txt","OpenSource Done ret=%X",ret);
		return ret;
	}
	VO_U32	GetSourceInfo(VO_SOURCE_INFO * pSourceInfo)
	{
		memcpy(pSourceInfo,&m_sourceInfo,sizeof(VO_SOURCE_INFO));
		return 0;
	}
	VO_U32 GetTrackInfo (VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo)
	{
		int ret=-1;
		if(m_tracks[nTrack])
		{
			ret = m_tracks[nTrack]->GetTrackInfo(pTrackInfo);
		}
		return ret;
	}
	VO_U32 GetSample (VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample);
	VO_U32 SetPos (VO_U32 nTrack, VO_S32 * pPos)
	{
		
		if((unsigned long)(voGetCurrentTime()-m_previousSeekTime)>1000)//avoid repeated seek
		{
			VO_S32 newPos=*pPos;
			m_previousSeekTime = voGetCurrentTime();
			SetPos(newPos);
			*pPos=newPos;
		}
		return 0;
	}
	VO_U32 SetSourceParam ( VO_U32 uID, VO_PTR pParam)
	{


#if DELAY_OPEN_URL
		if (uID==VOID_STREAMING_OPENURL)
		{
			return OpenURL();
		}
#endif//DELAY_OPEN_URL

		return PDSetParam( uID,  pParam );
	}
	VO_U32 GetSourceParam (VO_U32 uID, VO_PTR pParam)
	{
		return PDGetParam( uID,  pParam );
	}
	VO_U32 SetTrackParam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
	{
		return m_newReaderAPI.SetTrackParam(m_hSource,nTrack,uID,pParam);
	}
	VO_U32 GetTrackParam (VO_U32 nTrack, VO_U32 uID, VO_PTR pParam)
	{
		return m_newReaderAPI.GetTrackParam(m_hSource,nTrack,uID,pParam);
	}
	VO_SOURCE_READAPI	*			GetReaderAPI(){return &m_newReaderAPI;}
	
	virtual	void	OnDataLack();
	//bDF: file size < max buffer size
	virtual	DWORD	GetPDHeaderRemainSize(bool bDF);

	SPDInitParam*	GetInitParam(){return &m_initParam;};
	
protected:
	virtual bool	Stop();
	virtual bool	Pause();
	virtual bool	Run();
	virtual bool	Start();

	virtual bool	SetPos(long& start);
	virtual bool	CheckPDHeader();
	virtual bool	CheckBuffer(PBYTE pBuffer, DWORD dwSize);
	virtual bool	WritePDHeader();
	virtual bool	WriteBuffer(PBYTE pBuffer, DWORD dwSize, DWORD dwFileStartCopy = 0);

	virtual DWORD	GetCurrBufferingTime();
	int													CreateReaderAPI(TCHAR* url);
protected:
	
	VO_FILE_OPERATOR			m_opFile;
	DWORD								m_dwDuration;

	//TO_DO
	SPDInitParam					m_initParam;
	VONETDOWNTYPE			m_downLoadType;
	long										m_downLoadThreadPriority;
	TCHAR*								m_url;
	VOMediaType						m_nMediaType;
protected:
	HVOFILEREAD					m_hSource;
	VOFILEMT2FP						m_fPDMediaTime2FilePos;
public:
	long Time2FilePos( int nMediaTime, int* pnFilePos, bool bStart);
	HVOFILEREAD					GerReaderHandler(){return m_hSource;}	
protected:
	CNetGet2			m_NetGet;
	bool				m_bNoWritePDHeader;
	IVOMutex*			m_csRead;

	DWORD				m_dwClipBitrate;
	char			m_fileName[MAX_FNAME_LEN];
	long			m_currentTrackType;
public:
	VONETDOWNTYPE		GetDownloadType(){m_downLoadType;}
	long			GetCurrentTrackType(){return m_currentTrackType;}
	void			NotifyEvent(int id,void* param);
private:
	int		CreateSourceByURL(TCHAR* szFile);
	void  SetMediaTypeByURL(TCHAR* url);
	void  InitMediaTypeMap();
	void	UpdateTheTmpFileByURL(const TCHAR* pExtName);
	int    m_newPos;
	long  m_previousSeekTime;
	void  SetReaderPos()
	{
#if 0
		VO_S64 start64= m_newPos; 
		for (int i=0;i<m_sourceInfo.Tracks;i++)
		{
			if(m_newReaderAPI.SetPos(m_hSource,i,&start64)!=0)
				voLog(LL_TRACE,"seek.txt","seek fail\n");
			voLog(LL_TRACE,"seek.txt","seek actual pos=%d\n",start64);
		}
		m_newPos = -1;
#endif
	}
};
