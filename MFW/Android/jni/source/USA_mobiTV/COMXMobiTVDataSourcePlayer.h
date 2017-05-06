	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COMXMobiTVDataSourcePlayer.h

	Contains:	COMXMobiTVDataSourcePlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __COMXMobiTVDataSourcePlayer_H__
#define __COMXMobiTVDataSourcePlayer_H__

#ifdef _WIN32
//#define WIN32_LEAN_AND_MEAN 1
//#include <windows.h>
//#include <tchar.h>
#endif // _WIN32

#include "vomeAPI.h"
#include "voString.h"

#include "COMXEngine.h"
#include "MKPlayer/Player.h"
#include "MKPlayer/Result.h"
#include "MKBase/Log.h"
#include "DRM/DRMExternalApi.h"
#include "voCOMXBaseConfig.h"
#include "TestDumpFile.h"
typedef void* HANDLE;
#if !defined(VORTSPIMP)&&!defined(_WIN32_XP)
#define VO_MOBITV_USE_SHARE_LIB 1
#endif//#if !defined(VORTSPIMP)&&!defined(_WIN32_XP)

#define FUNC_PPOINT2(a) a##_FP VO_##a;
typedef MK_Player*			(*MK_Player_Create_FP)(void);	
typedef void				(*MK_Log_Discard_FP)(void);
typedef void				(*MK_Log_ToFunc_FP)(MK_Log_MsgFunc aMsgFunc, MK_Log_EndFunc aEndFunc, void* aObj, MK_U32 aMinLev);
typedef void				(*MK_Player_Destroy_FP)(MK_Player* aPlay);
typedef void				(*MK_Sample_Destroy_FP)(MK_Sample* aSample);
typedef MK_Result			(*MK_Player_Open_FP)(MK_Player* aPlay, const MK_Char* aURL);
typedef MK_Result                       (*MK_Player_HTTP_SetUseBA_FP)(MK_Player* aPlay, MK_Bool onOff);
typedef void		                (*MK_Player_Close_FP)(MK_Player* aPlay, MK_Bool aFull);
typedef MK_Result			(*MK_Player_SetBufferFunc_FP)(MK_Player* aPlay, MK_BufferFunc aFunc, void* aObj);
typedef MK_Result			(*MK_Player_SetErrorFunc_FP)(MK_Player* aPlay, MK_ErrorFunc aFunc, void* aObj);
typedef MK_Result			(*MK_Player_SetEventFunc_FP)(MK_Player* aPlay, MK_EventFunc aFunc, void* aObj);
typedef MK_Result			(*MK_Player_SetStateFunc_FP)(MK_Player* aPlay, MK_StateFunc aFunc, void* aObj);
typedef MK_U32				(*MK_Player_GetTrackCount_FP)(const MK_Player* aPlay);
typedef const MK_TrackInfo*             (*MK_Player_GetTrackInfo_FP)(const MK_Player* aPlay, MK_U32 aIdx);
typedef MK_Result			(*MK_Player_EnableTrack_FP)(MK_Player* aPlay, MK_U32 aIdx);
typedef MK_Sample*			(*MK_Player_GetTrackSample_FP)(MK_Player* aPlay, MK_U32 aIdx);
typedef MK_Result			(*MK_Player_Play_FP)(MK_Player* aPlay);
typedef MK_Result			(*MK_Player_Seek_FP)(MK_Player* aPlay, MK_Time aTime);
typedef MK_State			(*MK_Player_GetState_FP)(const MK_Player* aPlay);
typedef MK_Time                         (*MK_Player_GetStartTime_FP)(const MK_Player* aPlay);
typedef MK_Result			(*MK_Player_Pause_FP)(MK_Player* aPlay);
typedef MK_Result			(*MK_Player_Resume_FP)(MK_Player* aPlay);
typedef MK_Result			(*MK_Player_Stop_FP)(MK_Player* aPlay);
typedef MK_S32				(*MK_Player_Wait_FP)(MK_Player* aPlay, MK_U32 aMSec);
typedef		MK_U32  		(*MK_TrackInfo_GetH264ConfSPS_FP)(const MK_TrackInfo* aInfo, MK_U32 aIdx, MK_U8* aBuf, MK_U32 aSz);
typedef		MK_U32  		(*MK_TrackInfo_GetH264ConfPPS_FP)(const MK_TrackInfo* aInfo, MK_U32 aIdx, MK_U8* aBuf, MK_U32 aSz);
typedef	MK_Result  			(*MK_Log_ToFile_FP)(const MK_Char* aPath, MK_U32 aMinLev);

/* Extended API */
typedef	MK_Bool     (*MK_MobiDrmContentIsEncrypted_FP)      (const MK_U8* aBuf, MK_U32 aSize);
typedef	MK_Result   (*MK_MobiDrmGetContentInfo_FP)          (const MK_U8* aBuf, MK_U32 aSize, sMK_MobiDrmMbdcHeader* aHeader);
typedef MK_Result   (*MK_MobiDrmGetLicenseInfo_FP)          (const MK_U8* aBuf, MK_U32 aSize, const MK_U8* aPrivKey, 
							     MK_U32 aPrivKeyLen, MK_U32 aTime,
							     sMK_MobiDrmIDs* aIDs, sMK_MobiDrmLicense* aLicense, MK_List* aKeys);

typedef	MK_Result   (*MK_Player_Net_SetTimeout_FP)          (MK_Player* aPlay, MK_U32 aMSec);
typedef	MK_Result   (*MK_Player_MobiDRM_SetKey_FP)          (MK_Player* aPlay, const void* aKey, MK_U32 aSz);
typedef	MK_Result   (*MK_Player_MobiDRM_SetDeviceIDs_FP)    (MK_Player* aPlay, const void* aDID, MK_U32 aDSz, const void* aXID, MK_U32 aXSz);
typedef	MK_Result   (*MK_Player_MobiDRM_SetUserID_FP)       (MK_Player* aPlay, const void* aUID, MK_U32 aSz);
typedef	MK_Result   (*MK_Player_MobiDRM_SetLicense_FP)      (MK_Player* aPlay, const void* aLic, MK_U32 aSz, MK_U32 aNow);
typedef	MK_Result   (*MK_Player_HTTP_SetProxy_FP)           (MK_Player* aPlay, const MK_Char* aHost, MK_U16 aPort);
typedef	MK_Result   (*MK_Player_HTTP_SetUserAgentVals_FP)   (MK_Player* aPlay, const MK_Char* aName, const MK_Char* aVer, const MK_Char* aDev, const MK_Char* aExtra);
typedef MK_Result   (*MK_Player_HTTP_SetUseKeepAlive_FP)    (MK_Player* aPlay, MK_Bool aUse);
typedef MK_Result   (*MK_Player_HTTP_SetBALimits_FP)        (MK_Player* aPlay, MK_U32 aLower, MK_U32 aUpper);
typedef	MK_Result   (*MK_Player_Media_SetBufferLimits_FP)   (MK_Player* aPlay, MK_U32 aMinMSec, MK_U32 aLowMSec, MK_U32 aMaxMSec);
typedef MK_Result   (*MK_Player_SetOption_FP)               (MK_Player* aPlay, const MK_Char* aKey, const MK_Char* aObj);
typedef MK_Result   (*MK_Player_SetOptions_FP)              (MK_Player* aPlay, const MK_Char* aObj);
typedef MK_Result   (*MK_Player_SetOptionsF_FP)             (MK_Player* aPlay, const MK_Char* aPath);

typedef struct{

	FUNC_PPOINT2(MK_Player_Create) //MK_Player_Create_FP		VO_MK_Player_Create;
	FUNC_PPOINT2(MK_Log_Discard)
	FUNC_PPOINT2(MK_Log_ToFunc)
	FUNC_PPOINT2(MK_Player_Destroy)
	FUNC_PPOINT2(MK_Sample_Destroy)
	FUNC_PPOINT2(MK_Player_Open)
	FUNC_PPOINT2(MK_Player_HTTP_SetUseBA)
	FUNC_PPOINT2(MK_Player_Close)
	FUNC_PPOINT2(MK_Player_SetBufferFunc)
	FUNC_PPOINT2(MK_Player_SetErrorFunc)
	FUNC_PPOINT2(MK_Player_SetEventFunc)
	FUNC_PPOINT2(MK_Player_SetStateFunc)
	FUNC_PPOINT2(MK_Player_GetTrackCount)
	FUNC_PPOINT2(MK_Player_GetTrackInfo)
	FUNC_PPOINT2(MK_Player_EnableTrack)
	FUNC_PPOINT2(MK_Player_GetTrackSample)
	FUNC_PPOINT2(MK_Player_Play)
	FUNC_PPOINT2(MK_Player_Seek)
	FUNC_PPOINT2(MK_Player_GetState)
	FUNC_PPOINT2(MK_Player_GetStartTime)
	FUNC_PPOINT2(MK_Player_Stop)
	FUNC_PPOINT2(MK_Player_Wait)

	FUNC_PPOINT2(MK_TrackInfo_GetH264ConfSPS)
	FUNC_PPOINT2(MK_TrackInfo_GetH264ConfPPS)
	FUNC_PPOINT2(MK_Log_ToFile)

	FUNC_PPOINT2(MK_MobiDrmContentIsEncrypted)
	FUNC_PPOINT2(MK_MobiDrmGetContentInfo)
	FUNC_PPOINT2(MK_MobiDrmGetLicenseInfo)
	FUNC_PPOINT2(MK_Player_Net_SetTimeout)
	FUNC_PPOINT2(MK_Player_MobiDRM_SetKey)
	FUNC_PPOINT2(MK_Player_MobiDRM_SetDeviceIDs)
	FUNC_PPOINT2(MK_Player_MobiDRM_SetUserID)
	FUNC_PPOINT2(MK_Player_MobiDRM_SetLicense)
	FUNC_PPOINT2(MK_Player_HTTP_SetProxy)
	FUNC_PPOINT2(MK_Player_HTTP_SetUserAgentVals)
	FUNC_PPOINT2(MK_Player_HTTP_SetUseKeepAlive)
	FUNC_PPOINT2(MK_Player_HTTP_SetBALimits)
	FUNC_PPOINT2(MK_Player_Media_SetBufferLimits)
	FUNC_PPOINT2(MK_Player_SetOption)
	FUNC_PPOINT2(MK_Player_SetOptions)
	FUNC_PPOINT2(MK_Player_SetOptionsF)

	void* Instance;
	MK_Player* lPlayer;
	int	  functionMiss;
}TMKPlayerFunc;
typedef struct{
	unsigned long	localTime;
	unsigned long	timestamp;
}TDeliverTime;
class COMXMobiTVDataSourcePlayer : public COMXEngine

{
	//Dump Test
	CTestDumpFile		m_testDumpfile;
	bool				m_bDisableInputVOME;
	OMX_ERRORTYPE					Input2Vome(OMX_BUFFERHEADERTYPE *bufHead,TDeliverTime* preTime);
	bool				m_bIsBuffering;
public:
	// Used to control the image drawing
	COMXMobiTVDataSourcePlayer (void);
	virtual ~COMXMobiTVDataSourcePlayer (void);

	OMX_ERRORTYPE SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay);

	OMX_ERRORTYPE Playback (VOME_SOURCECONTENTTYPE * pSource);
	OMX_ERRORTYPE Close (void);

	OMX_ERRORTYPE Run (void);
	OMX_ERRORTYPE Pause (void);
	OMX_ERRORTYPE Stop (void);

	OMX_ERRORTYPE GetDuration (OMX_S32 * pDuration);
	OMX_ERRORTYPE GetCurPos (OMX_S32 * pCurPos);
	OMX_ERRORTYPE SetCurPos (OMX_S32 nCurPos);
	OMX_ERRORTYPE GetAudioFormat(VOME_AUDIOFORMATTYPE* audioFormat)
	{
		audioFormat->nChannels		= m_channelNum;
		audioFormat->nSampleRate	= m_sampleRate;
		audioFormat->nSampleBits   = 16;
		return OMX_ErrorNone;
	}
	OMX_ERRORTYPE  GetVideoSize (VOME_VIDEOSIZETYPE* videoSize)
	{
		videoSize->nHeight = m_height;
		videoSize->nWidth  = m_width;
		return OMX_ErrorNone;
	}
//    static void* LoadMKPlayerLib(char* aLibPath);
//    static bool SourcePlayer_MobiDrmContentIsEncrypted(char* aLibPath, const char* aBuf, unsigned long aSize);
//    static OMX_U32 MKPlayerStaticApi (char* aLibPath, OMX_U32 aId, void* aArgs);

//    long SourcePlayer_Player_MobiDRM_SetKey(const void* aKey, unsigned long aSz);
//           OMX_U32 MKPlayerDynamicApi (void* aPlayer, OMX_U32 aId, void* aArgs);

	OMX_ERRORTYPE SetMsgHandler(VOACallBack pFunc, void * pUserData);
	MK_Result lRes_open() { return m_lRes_open; };

	void MyBufferFunc(MK_Bool aOn, MK_U32 aFill);
	void MyErrorFunc(MK_Result aErr);
	void MyEventFunc(MK_Event aEvt, const void* aArg);
	void MyStateFunc(MK_State aWas, MK_State aIs);
protected:
	OMX_COMPONENTTYPE *		m_pCompSrc;
	OMX_COMPONENTTYPE *		m_pCompAD;
	OMX_COMPONENTTYPE *		m_pCompVD;
	OMX_COMPONENTTYPE *		m_pCompAR;
	OMX_COMPONENTTYPE *		m_pCompVR;

	OMX_COMPONENTTYPE *		m_pCompClock;

	OMX_VO_DISPLAYAREATYPE	m_dspArea;
	bool					m_bNetError;
public:
	HANDLE		m_hThreadVideo;
	bool		m_bVideoStop;

	static int	ReadVideoDataProc (OMX_PTR pParam);
	int			ReadVideoDataLoop (void);

	HANDLE		m_hThreadAudio;
	bool		m_bAudioStop;

	static int	ReadAudioDataProc (OMX_PTR pParam);
	int			ReadAudioDataLoop (void);
	
	HANDLE		m_hThreadTrigger;
	static int	TriggerProc (OMX_PTR pParam);
	int				TriggerLoop();
	int			StopSourceThread (void);

//MTKPlayer fields
private:
//	void* lPlayer;
	int lVideoTrackIdx,lAudioTrackIdx;
	int m_videoTimeRes;
	int m_audioTimeRes;
	int m_width,m_height;
	int m_sampleRate,m_channelNum;
	int m_duration;
	OMX_S32 m_currentPos;
	int m_endTime;
	int m_mobiTVEOS;
	bool m_seeking;
	//bool m_isEnd;
	int TryToSendEOS(OMX_BUFFERHEADERTYPE* pBufHead);
#define MAX_H264HEADSIZE	512
#define ADTS_HEADSIZE			7
	unsigned char  m_h264HeadData[MAX_H264HEADSIZE];
	int						m_h264HeadDataSize;
	unsigned char  m_adtsHead[ADTS_HEADSIZE];
	void WriteADTSHead(int sampFreq, int chanNum,int framelen,unsigned char *adtsbuf);
	void UpdateADTSHead(int framelen,unsigned char *adtsbuf);
	bool IsPaused()
	{
		VOMESTATUS status=VOME_STATUS_Unknown	;
		GetStatus(&status);
		return status==VOME_STATUS_Paused;
	}

	MK_Result m_lRes_open;
	// For buffer function
	VOACallBack m_pCallBackMsgHandler;
	void *m_pUserData;
	
//	TMKPlayerFunc	m_mkPlyer;


	// dump log and data files
	OMX_S32					m_nAudioDataFormat;
	OMX_PTR					m_hAudioDataFile;

	OMX_S32					m_nVideoDataFormat;
	OMX_PTR					m_hVideoDataFile;

	OMX_S32					m_nLogLevel;
	voCOMXBaseConfig *		m_pConfig;
	int						m_vrType;
	int						m_arType;
	char					m_deviceName[256];
public:
	
	int OpenURL(char* url);
	OMX_ERRORTYPE SetParam (OMX_S32 nID, OMX_PTR pValue);	
	OMX_ERRORTYPE GetParam (OMX_S32 nID, OMX_PTR pValue);	
	virtual OMX_U32		Release (void);
	bool	IsEndOfStream(){return m_mobiTVEOS!=0;};
	bool	IsNetError(){return m_bNetError;};
};
enum{
	VOME_C_RENDER			= 1,
	VOME_JAVA_RENDER		= 8,
};
#define MOBITV_FUNCSET_ID		 0X20100114
#define MOBITV_FORCE_STOP		 0X20100115
#define MOBITV_EOS_PARAM		 0X20100116
#define MOBITV_VR_TYPE			 0X20100117
#define MOBITV_AR_TYPE			 0X20100118
#define MOBITV_BUFFERING		 0X20100119
#define MOBITV_BUFFERING_BEGIN   0X20110001
#define MOBITV_BUFFERING_END	 0X20110002
extern TMKPlayerFunc gMKPlayerFunc;

#endif // __COMXMobiTVDataSourcePlayer_H__
