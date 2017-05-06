/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CClipPlayer.h

Contains:	CClipPlayer header file

Modify by:   Leon Huang

Change History (most recent first):
2011-03-03		Leon        Create the file from vompCPlayer.h

*******************************************************************************/
#ifndef __CClipPlayer_H__
#define __CClipPlayer_H__

	
//#include <windows.h>

//#include <tchar.h>

#include "voThread.h"
#include "vompCEngine.h"

#include "vompCFile.h"

#define	WM_TIMER_STATUS		201



#include "videoEditorType.h"

#include "CPlayerBaseCtrl.h"
#include "CTrimCtrl.h"
#include "AudioReSampleEngine.h"

class CClipPlayer : public CPlayerBaseCtrl
{
public:
	// Used to control the image drawing
	CClipPlayer (VO_LIB_OPERATOR* pLibOp);
	virtual ~CClipPlayer (void);

	virtual int 		SetDataSource (void * pSource, int nFlag, int nMode,VO_U32 editorType);
	virtual VO_S32 InitParam();

	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual VO_S32		Stop (void);

	virtual int 		SetCurPos (int nCurPos);
	virtual VO_S32 SetPosition(VOMP_BUFFERTYPE **pOutBuffer,VO_U32 *nDestSampe_Time,VO_U32 nSampleType);
	VO_S32 GetPosition(VO_U32 *pos);
	VO_S32 GetDuration(VO_U32 *duration);
	VO_S32 Flush();
	/*reset a clip Param ,before the clip need restart Play or GetSample;*/
	/*important*/
	VO_S32 ResetParam();

	virtual int			HandleEvent (int nID, void * pParam1, void * pParam2);

	static  int			vompThreadCreate (void ** pHandle, int * pID, void * fProc, void * pParam);

protected:

	bool						m_bOpenSync;
	int						m_nPlayMode;

	VO_U32					m_nAudioCodec;
	VO_U32					m_nVideoCodec;
	VOMP_AUDIO_FORMAT		m_sTargetAudioFormat;
	VOMP_AUDIO_FORMAT		m_sOrignalAudioFormat;
	VOMP_VIDEO_FORMAT		m_sVideoFormat;
	VOEDT_VIDEO_FORMAT m_sVideoFormat_Thumbnail;

	int						m_nColorType;
	long long				m_llLastVideoMediaTime;
	int						m_nLastVideoSystemTime;

	bool					m_bSeeking;
	bool					m_bRenderOut;

	vompCBaseFileOP			m_opFile;
//	VOMP_DATASOURCE_FUNC	m_sDataSource;
//	VOMP_LIB_FUNC			m_sLibFunc;
//	VOMP_SOURCEDRM_CALLBACK	m_sDrmCB;

//	VOMP_READBUFFER_FUNC	m_sReadBuffer;
	//vompCSourceBuffer		m_sSourceBuffer;

public:
	
	bool				m_bStopPlay;
	bool				m_bExitPlayer;

	TCHAR				m_szStatus[256];

public:
	static void *		mmLoadLib (void * pUserData, char * pLibName, int nFlag);
	static void *		mmGetAddress (void * pUserData, void * hLib, char * pFuncName, int nFlag);
	static int			mmFreeLib (void * pUserData, void * hLib, int nFlag);
	static int			vompSourceDrmCallBack (void * pUserData, int nFlag, void * pParam, int nReserved);


	static VO_VOID		NotifyStatus (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

	VO_VOID ReflashLib(void);

//add by leon
public :
	//this param is used in GetVideoSampleBySeek for VideoEditor 
	//reback the pointer to VideoEditor UI
	//the param is be created in GetVideoSampleBySeek first time, it will not be deleted until end of the editor
	VOMP_BUFFERTYPE *	m_pVideoSampleBuffer ;
	VO_S32 GetClipThumbNail(VO_U32 pos, VO_VIDEO_BUFFER **vBuffer );

	VO_S32 Seek(int pos);

	VO_S32 SetAParam(void* pValue);
	
	//VO_S32 GetDuration(int &duration){ return vompCEngine::GetDuration(&duration)}

	//get a sample ,callback to caller
	VO_S64 m_syncStartTimestamp;
	VO_S32 GetSample(VOMP_BUFFERTYPE **pOutBuffer,VO_U64 syncTimestmp,VO_S32 *pReserve,VO_U32 nSampleType);

	VOEDTDECODERPROC m_proc;
	VO_VOID*      m_pUserData;

	VO_BOOL GetEffectAction();
	VO_BOOL m_bEffectAction;

	

	float GetVideoTransitionValue();
	float GetAudioTransitionValue();

	VO_VOID SetIncrease(){ m_nDelta = 1;}
	VO_VOID SetDecrease(){ m_nDelta = -1;}
	
	VO_S32 GetClipParam(VO_S32 nPid,VO_VOID *pData);
	virtual VO_S32 SetClipParam(VO_S32 nPid,VO_VOID *pData);

	VO_S32 InitTrimCtrl(VO_VOID* pUserData, VOEDTENCODERCALLBACKPROC pProc);
	VO_S32 GetTrimSample(VO_S32 nSampleType,VO_U32 *nTime);
	int m_nCurVolume;

	//int GetPosition ();

private:
	vompCEngine  *m_pVompCEngine;
	CTrimCtrl			*m_pTrimCtrl;

	int m_nDelta;
	VO_S32 m_nIntervalTime;
	VO_S32 GetASample(VO_S32 nSampletype,VOMP_BUFFERTYPE **pBuffer);
	VO_U32 GetIntervalTime();

	int m_llTime_previous;
	float m_fVideoTransitionStep;
	VO_S32 m_nAudioTransitionStep;
	VOMP_BUFFERTYPE m_Previous_VideoBuffer;
	VOMP_BUFFERTYPE *m_pPresentBuffer;

	/*new version*/
	VO_U32 m_nVideoStride;
	VOMP_BUFFERTYPE m_CurrentVideoBuffer;
	VO_U32 m_nAudioMaxSize;
	VO_U32 m_nVideoMaxSize;
	VOMP_BUFFERTYPE m_CurrentAudioBuffer;



	VO_BOOL CheckBlackFrame (VO_VIDEO_BUFFER * pVideoBuffer, VOMP_VIDEO_FORMAT * pVideoSize);
	
	VOMP_BUFFERTYPE NULL_AUDIO; 
	VOMP_BUFFERTYPE *m_pVideoBuffer; 


	CAudioReSampleEngine m_audioReSampleEngine; 
};

#endif // __CClipPlayer_H__
