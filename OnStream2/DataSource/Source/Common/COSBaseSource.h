#ifndef __COSBaseSource_H__
#define __COSBaseSource_H__

#include "voCMutex.h"
#include "voOnStreamType.h"
#include "voOnStreamSource.h"
#include "CBaseConfig.h"
#include "voSource2.h"
#include "voCBaseThread.h"
#include "voString.h"
#include "cmnFile.h"
#include "COSTimeText.h"
#include "COSCloseCaption.h"
#include "COSDVBSubtitle.h"
#include "voCaptionParser.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define		VOOS_EVENT_MESSAGE		0x766f6f73 + 1

char* const VideoCodec[] = { "None", "MPEG2", "H263", "S263", "MPEG4", "H264", "WMV", "RealVideo", "MJPEG", "DIVX", "VP6", "VP8", "VP7", "VC1", "HEVC"};
char* const AudioCodec[] = { "None", "PCM", "ADPCM", "AMRNB", "AMRWB", "AMRWBP", "QCELP13", "EVRC", "AAC", "AC3", "FLAC", "MP1", "MP3", "OGG",
							 "WMA", "RealAudio", "MIDI", "DRA", "G729", "EAC3", "APE", "ALAC", "DTS"};
char* const Language[] = {	"Unknow", "English", "Simplified CHINESE", "Traditional Chinese", "Korean", "Japanese", "Spanish", "German"};


typedef VO_S32 (VO_API* VOGETSRCFRAPI) (VO_SOURCE2_API* pHandle);
typedef VO_S32 (VO_API* VOGETLOCALSRCFRAPI) (VO_SOURCE2_API* pHandle, VO_S32 nFlag);

typedef enum{
	VOOSSRC_STATUS_CLOSED				= 0,			/*!<The status is init */
	VOOSSRC_STATUS_OPENED				= 1,			/*!<The status is loading */
	VOOSSRC_STATUS_RUNNING				= 2,			/*!<The status is running */
	VOOSSRC_STATUS_PAUSED				= 3,			/*!<The status is paused */
	VOOSSRC_STATUS_STOPPED				= 4,			/*!<The status is stopped */
	VOOSSRC_STATUS_MAX					= 0X7FFFFFFF
} VOOSSRC_STATUS;

class COSBaseSource
{
public:
	COSBaseSource (VO_SOURCE2_LIB_FUNC *pLibop);
	virtual ~COSBaseSource (void);

	virtual int				Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag);
	virtual int				Uninit(void);
	virtual int 			Open(void);
	virtual int 			Run (void);
	virtual int 			Pause (void);
	virtual int 			Stop (void);
	virtual int 			Close (void);
	virtual int 			GetDuration (long long * pDuration);
	virtual int 			SetCurPos (long long *pCurPos);
	virtual int 			GetParam (int nID, void * pValue);
	virtual int 			SetParam (int nID, void * pValue);
	virtual void*			GetReadBufPtr();

	virtual int 			GetSample(VO_SOURCE2_TRACK_TYPE nTrackType , void* pSample );
	virtual int 			GetProgramCount(unsigned int *pProgreamCount);
	virtual int 			GetProgramInfo(unsigned int nProgram, VO_SOURCE2_PROGRAM_INFO **ppProgramInfo);
	virtual int 			GetCurTrackInfo(VO_SOURCE2_TRACK_TYPE nTrackType , VO_SOURCE2_TRACK_INFO ** ppTrackInfo );
	virtual int				SelectProgram(unsigned int nProgram);
	virtual int				SelectStream(unsigned int nStream);
	virtual int				SelectTrack(unsigned int nTrack);
	virtual int 			SendBuffer(const VO_SOURCE2_SAMPLE& buffer);
	virtual int 			GetDRMInfo(VO_SOURCE2_DRM_INFO **ppDRMInfo);

	virtual int				GetSubLangNum(int *pNum);
	virtual int				GetSubLangItem(int Index, VOOSMP_SUBTITLE_LANGUAGE** ppSubLangItem);
	virtual int				GetSubLangInfo(VOOSMP_SUBTITLE_LANGUAGE_INFO** pSubLangInfo);
	virtual int				SelectLanguage(int Index);


	static	VO_S32 			vomtEventCallBack (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);
	virtual int				HandleEvent (int nID, int nParam1, int nParam2);
	virtual int				onNotifyEvent (int nMsg, int nV1, int nV2);

	static	VO_S32 			vomtSendCallBack (VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);
	virtual int				doSendData(unsigned int nOutputType, void* pData);

	static int				voSrcReadAudio(void * pUserData, VOOSMP_BUFFERTYPE* pBuffer);
	virtual int				doReadAudio(VOOSMP_BUFFERTYPE * pBuffer);
	static int				voSrcReadVideo(void * pUserData, VOOSMP_BUFFERTYPE* pBuffer);
	virtual int				doReadVideo(VOOSMP_BUFFERTYPE * pBuffer);

	virtual int				GetMediaCount(int ssType);
	virtual int				SelectMediaIndex(int ssType, int nIndex);
	virtual int				IsMediaIndexAvailable(int ssType, int nIndex);
	virtual int				GetMediaProperty(int ssType, int nIndex, VOOSMP_SRC_TRACK_PROPERTY **pProperty);
	virtual int				CommetSelection(int bSelect);
	virtual int				GetCurMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);
	virtual int				GetCurPlayingMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);
	virtual int				GetCurSelectedMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex);

protected:
	virtual int				ConvertBufSrc2OS(VO_SOURCE2_SAMPLE* pSampleBuf, VOOSMP_BUFFERTYPE* pBuffer, int ssType);
	virtual int				ConvertAudioCodecSrc2OS(int nCodec);
	virtual int				ConvertVideoCodecSrc2OS(int nCodec);
	virtual unsigned int	ConvertReturnValue(unsigned int nRC);
	

	virtual int				InitSrcParam();
	virtual int				UpdateSrcParam(VO_SOURCE2_PROGRAM_INFO * pProgramInfo);
	virtual int				LoadDll();
	virtual int				unLoadDll();
	virtual int				SelectSubLang(char *pLan);

	virtual int				CheckingStatus(int nCount);

	virtual int				HandleCC(VOOSMP_BUFFERTYPE* pBuffer, int nRC);

	virtual int				postMsgEvent (int nMsg, int nV1 = 0, int nV2 = 0, int nDelayTime = -1);
	virtual int				callBackEvent ();

	virtual int				OpenConfig();
	virtual int				DumpInfo(int ssType, int nRC, VOOSMP_BUFFERTYPE * pBuffer);

	// do licence set parameter
	int                     onSetParam(int nID, void * pValue);

	virtual int				UpdateTrackInfo();
	virtual int				ClearPropertyItem(VOOSMP_SRC_TRACK_PROPERTY *pItemProperty);

protected:
	void *					m_hDll;
	VO_HANDLE				m_hSource;
	int						m_nFlag;

	int						m_nProgramNum;
	int						m_nStreamIndex;
	int						m_nAudioTrackNum;
	int						m_nVideoTrackNum;
	int						m_nAudioCodec;
	int						m_nVideoCodec;

	int						m_nJavaENV;

	int						m_nSeeking;
	int						m_nSeekTime;

	VOOSSRC_STATUS			m_nStatus;
	int						m_nChanging;

	VOOSMP_BUFFER_FORMAT	m_sAudioFormat;
	VOOSMP_BUFFER_FORMAT	m_sVideoFormat;
	VOOSMP_BUFFER_FORMAT	m_sSubtitleFormat;


	VO_SOURCE2_PROGRAM_INFO *	m_pInProgramInfo;		// temp program info for InitSrcParam to avoid dead lock with adaptive streaming parser
	VO_SOURCE2_PROGRAM_INFO *	m_pOutProgramInfo;		// temp program info for GetProgramInfo to avoid crash
	VO_SOURCE2_PROGRAM_INFO *	m_pInternalProgramInfo;	// internal program info, from m_pInProgramInfo, to m_pOutProgramInfo
	VO_U64						m_ullDuration;

	VO_SOURCE2_TRACK_INFO*		m_pAudioCurTrack;
	VO_SOURCE2_TRACK_INFO*		m_pVideoCurTrack;
	VO_SOURCE2_TRACK_INFO*		m_pSubtitleCurTrack;

	unsigned char*				m_pStreamList;

	VO_SOURCE2_LIB_FUNC*		m_pLibop;
	VOOSMP_SENDBUFFER_FUNC*		m_pSendBufFunc;
	
	VO_SOURCE2_API				m_SourceAPI;
	VOOSMP_READBUFFER_FUNC		m_cReadBufFunc;
	VO_SOURCE2_EVENTCALLBACK	m_cEventCallBack;
	VO_SOURCE2_SAMPLECALLBACK	m_cSampleCallBack;

	VOOSMP_LISTENERINFO*		m_pSourceListener;
	VOOSMP_LISTENERINFO*		m_pSourceOnQest;

	voCMutex					m_MutexReadData;
	voCMutex					m_MutexSendEvent;
	voCMutex					m_MutexPostEvent;
	voCMutex					m_MutexProgramInfo;
	//Ferry add mutex here for controller send two event at the same time
	voCMutex					m_MutexEvent;

	voCBaseThread*				m_pMsgThread;
	int							m_nSyncMsg;
	int							m_nDelayTime;
	int							m_nOpenFinished;

	VO_FILE_OPERATOR *			m_pFileOP;

	int							m_nEventID;
	int							m_nParam1;
	int							m_nParam2;

	VO_LOG_PRINT_CB*			m_pbVOLOG;

	VO_TCHAR					m_szPathLib[1024];

	COSBaseSubtitle*			m_pCSubtitle;
	int							m_nSubtitleTrackNum;
	unsigned int				m_nSubtitleCodecType;
	int							m_nSubtitleSelect;

	// config for the debug
	CBaseConfig					m_cfgSource;
	int							m_nLogAudioLevel;
	int							m_nLogVideoLevel;

	char*						m_pAudioDumpFile;
	char*						m_pVideoDumpFile;
	int							m_nDumpAudioLevel;
	int							m_nDumpVideoLevel;
	int							m_nStartSystemTime;

	void*						m_hAudioDumpFile;
	void*						m_hVideoDumpFile;

	// added by gtxia for set information to License module
	void*                       mCheckLib;

	//a/v/s trackinfo select track...
	int							m_nAudioCount;
	int							m_nVideoCount;
	int							m_nSubtitleCount;
	int							m_nExtendSubtitle;

	int							m_pAudioTrackIndex[128];
	int							m_pVideoTrackIndex[32];
	int							m_pSubtitleTrackIndex[128];

	int							m_nAudioSelected;
	int							m_nVideoSelected;
	int							m_nSubtitleSelected;
	int							m_nBAStreamEnable;

	int							m_nAudioCurTrack;
	int							m_nVideoCurTrack;
	int							m_nSubtitleCurTrack;

	VOOSMP_SRC_TRACK_PROPERTY m_szAudioProtery;
	VOOSMP_SRC_TRACK_PROPERTY m_szVideoProtery;
	VOOSMP_SRC_TRACK_PROPERTY m_szSubtitleProtery;

	bool							m_bBuffering;

	VO_CAPTION_LANGUAGE_DESCRIPTION	m_sCCLanguageDescription;
};

class voCBaseSourceEvent : public voCBaseEventItem
{
public:
    voCBaseSourceEvent(COSBaseSource * pSource, int (COSBaseSource::* method)(int, int, int),
					    int nType, int nMsg = 0, int nV1 = 0, int nV2 = 0)
		: voCBaseEventItem (nType, nMsg, nV1, nV2)
	{
		m_pSource = pSource;
		m_fMethod = method;
    }

    virtual ~voCBaseSourceEvent()
	{
	}

    virtual void Fire (void) 
	{
        (m_pSource->*m_fMethod)(m_nMsg, m_nV1, m_nV2);
    }

protected:
    COSBaseSource *		m_pSource;
    int (COSBaseSource::*	m_fMethod) (int, int, int);
};

#endif // __COSBaseSource_H__
