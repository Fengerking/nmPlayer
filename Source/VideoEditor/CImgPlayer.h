/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		CImgPlayer.h

Contains:	CImgPlayer header file

Modify by:   Leon Huang

Change History (most recent first):
2011-08-11		Leon        Create the file from vompCPlayer.h

*******************************************************************************/
#ifndef __CImgPlayer_H__
#define __CImgPlayer_H__

#include "voThread.h"
#include "vompCEngine.h"

#include "vompCFile.h"

//#define	WM_TIMER_STATUS		201


#include "vompType.h"
#include "videoEditorType.h"
#include "CPlayerBaseCtrl.h"
#include "ImgEngine.h"
class CImgPlayer : public CPlayerBaseCtrl
{
public:
	// Used to control the image drawing
	CImgPlayer (VO_LIB_OPERATOR* pLibOp);
	virtual ~CImgPlayer (void);

	virtual int 		SetDataSource (void * pSource, int nFlag);

	VO_S32 InitParam();

protected:
	int						m_nPlayMode;

	VO_U32					m_nAudioCodec;
	VO_U32					m_nVideoCodec;
//	VOMP_AUDIO_FORMAT		m_sAudioFormat;
//	VOMP_VIDEO_FORMAT		m_sVideoFormat;
	VOEDT_VIDEO_FORMAT m_sVideoFormat_Thumbnail;

	int						m_nColorType;
	long long				m_llLastVideoMediaTime;
	int						m_nLastVideoSystemTime;

public:
	
	bool				m_bStopPlay;
	bool				m_bExitPlayer;

	TCHAR				m_szStatus[256];

public:
	VO_VOID ReflashLib(void);

//add by leon
public :
	//this param is used in GetVideoSampleBySeek for VideoEditor 
	//reback the pointer to VideoEditor UI
	//the param is be created in GetVideoSampleBySeek first time, it will not be deleted until end of the editor
	VOMP_BUFFERTYPE *	m_pVideoSampleBuffer ;
	virtual VO_S32 SetPosition(VOMP_BUFFERTYPE **pOutBuffer,VO_U32 *pPos, VO_U32 nSampleType);
	VO_S32 GetClipThumbNail(VO_U32 pos, VO_VIDEO_BUFFER **vBuffer );

	VO_S32 Seek(int pos);

	VO_S32 SetAParam(void* pValue);

	
	//VO_S32 GetDuration(int &duration){ return vompCEngine::GetDuration(&duration)}

	//get a sample ,callback to caller
	VO_S32 GetSample(VOMP_BUFFERTYPE **pOutBuffer,VO_U64 syncTimestmp,VO_S32 *pReserve,VO_U32 nSampleType);
	/*reset a clip Param ,before the clip need restart Play or GetSample;*/
	/*important*/
	VO_S32 ResetParam();

	VOEDTDECODERPROC m_proc;
	VO_VOID*      m_pUserData;

	VO_BOOL GetEffectAction();
	VO_BOOL m_bEffectAction;

	float GetVideoTransitionValue();
	float GetAudioTransitionValue();

	VO_VOID SetIncrease(){ m_nDelta = 1;}
	VO_VOID SetDecrease(){ m_nDelta = -1;}
	
	VO_S32 GetClipParam(VO_S32 nPid,VO_VOID *pData);
	VO_S32 SetClipParam(VO_S32 nPid,VO_VOID *pData);

private:
	ImgEngine *m_pImgEngine;	

	int m_nDelta;
	VO_S32 m_nIntervalTime;
	VO_S32 GetASample(VO_S32 nSampletype,VOMP_BUFFERTYPE **pBuffer);
	VO_U32 GetIntervalTime();

	int m_llTime_previous;
	float m_fVideoTransitionStep;
	VO_S32 m_nAudioTransitionStep;
	VOMP_BUFFERTYPE m_Previous_VideoBuffer;
	VOMP_BUFFERTYPE *m_pPresentBuffer;
	
	VOMP_VIDEO_FORMAT		m_sVideoFormat;

	VO_BOOL CheckBlackFrame (VO_VIDEO_BUFFER * pVideoBuffer, VOMP_VIDEO_FORMAT * pVideoSize);
	
	VOMP_BUFFERTYPE *m_pVideoBuffer; 


};

#endif // __CClipPlayer_H__
