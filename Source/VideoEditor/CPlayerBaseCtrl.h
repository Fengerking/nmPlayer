/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CPlayerBaseCtrl.h

Contains:	CPlayerBaseCtrl header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-05		Leon			Create file
*******************************************************************************/

#ifndef __CPlayerBaseCtrl_H__
#define __CPlayerBaseCtrl_H__
#pragma once
#include "videoEditorType.h"
typedef void (VO_API *VOEDTDECODERPROC)(void* pData);
typedef struct  
{
	VOEDTDECODERPROC			pProc;
	VO_VOID*			    pUserData;
}CPLAYER_INIT_INFO;
class CPlayerBaseCtrl
{
public:
		CPlayerBaseCtrl(VO_LIB_OPERATOR* pLibOp);
		virtual ~CPlayerBaseCtrl(void);
		virtual VO_S32 SetDataSource(void *pSource,VO_S32 nFlag);
		virtual VO_S32 GetClipParam(VO_S32 nPid,VO_VOID *pData);
		virtual VO_S32 SetClipParam(VO_S32 nPid,VO_VOID *pData);
		virtual VO_S32 GetClipThumbNail(VO_U32 pos, VO_VIDEO_BUFFER **vBuffer );
		virtual VO_S32 GetDuration(VO_U32* pDuration);
		virtual VO_S32 GetPosition(VO_U32* pPos);
		virtual VO_S32 SetPosition(VO_U32* pPos);
		virtual VO_S32 SetPosition(VOMP_BUFFERTYPE **pOutBuffer,VO_U32* pPos,VO_U32 nSampleType);
		virtual float GetVideoTransitionValue();
		virtual VO_S32 GetSample(VOMP_BUFFERTYPE **pOutBuffer,VO_U64 syncTimestmp,VO_S32 *pReserve,VO_U32 nSampleType);
		virtual int 		Run (void) {return 0;}
		virtual VO_S32 Stop();
		virtual VO_S32 Flush();
		virtual VO_S32 ResetParam();
		virtual VO_S32 InitParam();
		virtual VO_S32 ReleaseBuffer();
		virtual VOEDT_CLIP_INFO* GetClipInfo(){return &m_clip_info;};//{m_clip_info.pThumbnail = &m_pDataBuffer; return &m_clip_info;};
public:
	VOEDT_CLIP_INFO m_clip_info;
	//indicate that the clip's type audio/video/audio&video/text
	VO_U32 m_nClipType;
	VO_VIDEO_BUFFER m_pDataBuffer;
	VO_CHAR m_cTitle[255];
	VO_BOOL m_bStopPlay;
	VOEDT_CLIP_PARAM m_sClipParam;
	VO_U32 m_bDropBlackFrame;
	VO_LIB_OPERATOR *m_pLibOp;
	int m_nCurPosNow;
	VOMP_VIDEO_FORMAT       m_sVideoOutFormat;
};

#endif