/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CVideoEditorEngine.h

Contains:	CVideoEditorEngine header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/

#ifndef ___CVideoEditorEngine_H__
#define ___CVideoEditorEngine_H__


#define _MULTITHREAD

#pragma once
#include "CClipPlayer.h"
#include "CPtrList.h"


#include "EncoderCtrl.h"
#include "FileSinkCtrl.h"
#include "CTrimCtrl.h"

#include "videoEditorType.h"
#include "SampleTransformer.h"
#include "config.h"

#include "EffectionCtrl.h"

#include "TextPlayer.h"
#include "CImgPlayer.h"


#define NUM_ARRAY 20
class CVideoEditorEngine
{
public:
	CVideoEditorEngine();
	~CVideoEditorEngine(void);
	//display window
	VOMP_RECT				m_rcDraw;
	//now clip index
	VO_S32 m_nIndex;
	
	//init 
	CTextPlayer *tPlayer;


public:
	CObjectList<CClipPlayer>	m_listCPlayer_V;
	CObjectList<CClipPlayer>	m_listCPlayer_A;
	CObjectList<CPlayerBaseCtrl>	m_listCPlayer_Base;

	void InitVideoEditorEngine(VOEDT_INIT_INFO *pParam);
	//add a clip file VO_S32o list
	/*new*/
	VO_S32 AddMediaClip(VO_S32 nPid, VO_VOID* pSource,VO_U32 nFlag,VOEDT_CLIP_HANDLE **ppOutClip = NULL);
	VO_S32 AddTextClip(VO_S32 nPid, VO_VOID* pSource,VO_U32 nFlag,VOEDT_CLIP_HANDLE **ppOutClip = NULL);
	VO_S32 AddImgClip(VO_S32 nPid, VO_VOID* pSource,VO_U32 nFlag,VOEDT_CLIP_HANDLE **ppOutClip = NULL);

	/*new*/
	VO_S32 GetClipPosition(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *position);
	VO_S32 GetEditorPosition(VO_U32 *position);
	VO_S32 GetClipDuration(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *duration);
	VO_S32 GetEditorDuration(VO_U32 *duration);
	VO_S32 SetClipPosition(VOEDT_CLIP_HANDLE *pClip, VO_U32 position);
	VO_S32 SetEditorPosition(VO_U32 position);

	//VO_S32 SetPosition(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 position);
	VO_S32 GetEditorStatus(VO_U32* nStatus);
	VO_S32 SetThumbNailFormat(VOEDT_VIDEO_FORMAT *format);
	VO_S32 GetThumbNailFormat(VOEDT_VIDEO_FORMAT *format);
	VO_S32 SetVideoFormat(VOEDT_VIDEO_FORMAT *format);
	VO_S32 SetAudioFormat(VOEDT_AUDIO_FORMAT *format);
	VO_S32 GetOutputFormat(VOEDT_VIDEO_FORMAT *format);
	VO_S32 GetEditorVideoFormat(VOEDT_VIDEO_FORMAT *format);
	VO_S32 GetEditorAudioFormat(VOEDT_AUDIO_FORMAT *format);
	VO_S32 GetClipThumbNail(VOEDT_CLIP_HANDLE *pClipHandle ,VO_S32 position);
	VO_S32 GetEditorThumbNail(VO_U32 position);
	VO_S32 GetEditorAudioSample(VO_U32 position);
	VO_S32 UpdateData(VOEDT_CLIP_HANDLE *pClipHandle);
	VO_S32 DeleteClip(VOEDT_CLIP_HANDLE *pClipHandle);
	VO_S32 DeleteAllClip();
	VO_S32 JumpClipTo(VOEDT_CLIP_HANDLE *pClipHandle,VO_S32 nTargetPos);
	VO_S32 CheckClip(VOEDT_CLIP_HANDLE *pClipHandle);
	VO_S32 SetOutputParam(VOEDT_OUTPUT_PARAM *pParam);
	VO_S32 GetOutputParam(void* pValue);
	VO_S32 GetActionMode(VO_S32 *nMode);
	VO_S32 SetActionMode(VO_S32 *nMode);
	VO_S32 SetAudioOutputParam(VOEDT_OUTPUT_AUDIO_PARAM* pValue);
	VO_S32 SetVideoOutputParam(VOEDT_OUTPUT_VIDEO_PARAM* pValue);
	VO_S32 SetSinkOutputParam(VOEDT_OUTPUT_SINK_PARAM* pValue);
	VO_S32 GetSinkOutputParam(VOEDT_OUTPUT_SINK_PARAM* pValue);
	VO_S32 GetAudioOutputParam(VOEDT_OUTPUT_AUDIO_PARAM* pValue);
	VO_S32 GetVideoOutputParam(VOEDT_OUTPUT_VIDEO_PARAM* pValue);
	VO_S32 GetClipParam(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPid,VO_VOID *pValue);
	VO_S32 SetClipParam(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 nPid,VO_VOID *pValue);
	VO_S32 SetTextContent(VOEDT_CLIP_HANDLE *pClipHandle,VO_PTR pValue,VO_U32 nFlag);
	VO_S32 SetTextFormat(VOEDT_CLIP_HANDLE *pClipHandle,VOEDT_TEXT_FORMAT *pFormat);
	VO_S32 GetTextFormat(VOEDT_CLIP_HANDLE *pClipHandle,VOEDT_TEXT_FORMAT *pFormat);

	VO_S32 SetFrameRate(int *pValue){m_nFrameRate = *pValue; return 0;}

	VO_S32 SaveConfig(char* pDest);
	VO_S32 LoadConfig(char* pSource);
	VO_S32  PlayEditor();
	VO_S32	StopEditor();
	
	VO_S32 GetCallbackData(VOEDT_CALLBACK_BUFFER *pBuffer);
	
	//update some editor data
	VO_S32 UpdateEDITOR();

	static void EncoderCallback(VO_S32 nPid,VOEDT_PLAYER_CALLBACK_BUFFER *pValue);
private:
	VO_U32 m_nAudioSize_count;
	VO_BOOL m_bPlayerEditor;

	//the data must equal to VOEDT_OUTPUT_VIDEO_PARAM->VideoParam(width ,height,nColorType)
	//and this data will be set before any operation. 

	VO_VOID ReSetBuffer();

	//all media duration
	VO_S32 GetEditorDurationTime(VO_U32 *pDuration);
	VO_S32 GetAClipDurationTime(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *pDuration);
	VO_S32 GetAClipPosition(VOEDT_CLIP_HANDLE *pClipHandle,VO_U32 *pPos);
	VO_S32 FindEditorPosition(CPlayerBaseCtrl **ppPlayer,VO_S32 ppPos[],bool ppTransition[],VO_S32 nInPos,VO_S32 *nNum,VO_S32 nCliptype);
	VO_S32 FindEditor(CPlayerBaseCtrl **ppPlayer,VO_S32 *nNum,VO_S32 nCliptype);
	VO_S32 SetAClipPosition(CPlayerBaseCtrl *player,VO_U32 *pPos);
	VO_S32 GetAClipThumbNail(CPlayerBaseCtrl *player,VO_S32 *pPos,VOMP_BUFFERTYPE **outBuf,VO_S32 nFlag = 0);
	VO_S32 SetAudioPosition(VOMP_BUFFERTYPE **ppOutBuffer,VO_U32 *pPos);
	VO_S32 SetVideoPosition(VOMP_BUFFERTYPE **ppOutBuffer,VO_U32 *pPos);
	VO_S32 SeekEditor(VO_U32 pos);
	VO_S32 DoVideo(CPlayerBaseCtrl* pPlayer,VOMP_BUFFERTYPE** ppOutBuffer,bool isTransition,VO_U32 position,VO_S32 *relativeTime);
	VO_S32 DoAudio(CClipPlayer *player,VOMP_BUFFERTYPE** pOutBuffer,bool isTransition,VO_U32 position,VO_S32 *relativeTime);
	VO_S32 ReSortEditor(CObjectList<CPlayerBaseCtrl> &listCPlayer);
	VO_S32 FindListPos(CObjectList<CPlayerBaseCtrl> &listCPlayer,CPlayerBaseCtrl *player,POSITION &pos);
	VO_S32 UpdateEditorEverything();
	VO_S32 RefreshClipsVideoFormat(VOEDT_VIDEO_FORMAT *format);
	VO_S32 RefreshClipsAudioFormat(VOEDT_AUDIO_FORMAT *format);
	VO_S32 CallBackFunc(VO_S32 nPid, VOEDT_CALLBACK_BUFFER *callback);
	
	VOEDTCALLBACK m_pCallbackFunc;

	VO_TCHAR m_cWorkingPath[255];
	VO_LIB_OPERATOR *m_pLibOp;
	VO_VOID	*m_pUserData;

	CClipPlayer *CreateAClip(void* pSource,VO_U32 editorType);
	CClipPlayer *m_vompCurPlayer;
	CClipPlayer *m_vompFollowPlayer;

	VO_S32 m_nActionMode;
	VOEDT_OUTPUT_PARAM m_outputParam;

	VO_U64 m_nStartPosition;
	VO_U64	m_nVideoSample_nums;


	VO_U32  *m_syncTime;
	VO_U32  m_sampleTime_video;
	VO_U32  m_sampleTime_audio;
	VO_U32  m_nSyncSystemTime;
	VO_U32  m_nSyncSystemTime_Start;
	int			m_nFrameRate;
#ifdef _USE_YUV
	void ReleaseOutputBuffer(CPLAYER_OUTPUT_BUFFER *pData);
#endif
	void ReleaseList(CObjectList<CPlayerBaseCtrl> &listCPlayer);
	void CheckAndInitList();
	//for re_encoder
	bool GetSample(CObjectList<CPlayerBaseCtrl> &listCPlayer,VO_U32 nSampletype);
	//for trim
	bool GetSample(VO_S32 nSampleType);
	void ResetEachClip();
	void UnloadAllCtrl();



	VO_BOOL m_bSetAudioOutputParam;
	VO_BOOL m_bSetVideoOutputParam;
	VO_BOOL m_bSetSinkOutputParam;

	CEncoderCtrl			m_encoderCtrl;
	CFileSinkCtrl			m_FileSinkCtrl;
	CEffectionCtrl			m_EffectionCtrl;
	CTrimCtrl			   *m_TrimCtrl;
	FILE *out_file_encoder ;
	FILE *out_file_pcm;
	FILE *out_file_yuv;
	FILE *out_file_ARGB32;
	/*old*/
	//	CClipPlayer *FindPlayerByID(VO_S32 nIndex,POSITION &pos);
	//	CClipPlayer *FindPlayer(VO_S32 nIndex);
	//VO_S32 AddAClipToPos(CClipPlayer *player,VO_S32 &nIndex);
	//VO_S32 AddAClip(void* pSource,VO_U32 sourceType,VO_S32 &nIndex);
	//VO_S32 GetAClip(VO_S32 nIndex,void* pValue);
	//VO_S32 SetAVideoEditorClipPos(VO_S32 nIndex,VO_S32 pos);	
	//	VO_S32 GetAClipSample(VO_S32 nIndex, VO_S32 nDestSampe_Time);
	//	void InitBuffer();
public:
	//CRenderCtrl m_renderCtrl;
	
	static VO_U32 PlaySyncProc_SystemTime (VO_PTR pParam);
	VO_S32			PlaySyncLoop_SystemTime();
	voThreadHandle		m_hThreadSystemTimeLoop;



#ifndef _MULTITHREAD
	static	VO_U32			PlayListProc (VO_PTR pParam);
	VO_S32				PlayListLoop (void);
	voThreadHandle		m_hThreadListPlay;
	VO_BOOL				m_bPlayListLoopOK;
	VO_S32 DoLoop();
#else
	static	VO_U32			PlayListProc_Audio (VO_PTR pParam);
	static	VO_U32			PlayListProc_Video (VO_PTR pParam);
	VO_S32				PlayListLoop_Audio (void);
	VO_S32				PlayListLoop_Video (void);

	voThreadHandle		m_hThreadListPlay_Video;
	voThreadHandle		m_hThreadListPlay_Audio;
	VO_BOOL				m_bPlayListLoopOK_Video;
	VO_BOOL				m_bPlayListLoopOK_Audio;
	VO_S32 DoLoop_Audio();
	VO_S32 DoLoop_Video();
#endif


	bool				m_bStopPlay;
	bool				m_bExitEDITOR;

	VOMP_BUFFERTYPE m_videoOutputBuffer;
	CPLAYER_OUTPUT_BUFFER *m_video_outputbuffer ;
	CPLAYER_OUTPUT_BUFFER* m_audio_outputbuffer;
	
	//editor Thumbnailformat;
	VOEDT_VIDEO_FORMAT m_sVideoFormat_Thumbnail;
	VOEDT_VIDEO_FORMAT m_sVideoFormat; 
	VOEDT_VIDEO_FORMAT m_sVideoFormat_Encoder;
	VOEDT_VIDEO_FORMAT *m_pVideoFormat_PresentSelected;
	VOEDT_AUDIO_FORMAT m_sAudioFormat; 

	//VO_VIDEO_BUFFER m_ThumbnailBuffer ;
	VO_PBYTE m_AudioTransfer_buffer;
#if 0		
	CSampleTransformer m_SampleTransformer;
#endif
#ifdef _USE_YUV
	VO_VIDEO_BUFFER m_Transfer_buffer ;
	VO_VIDEO_BUFFER m_video_encoderbuffer;
	VOMP_BUFFERTYPE m_ThumbnailBuffer ;
#endif

};


#endif