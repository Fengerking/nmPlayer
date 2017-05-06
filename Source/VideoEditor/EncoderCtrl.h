/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		EncoderCtrl.h

Contains:	EncoderCtrl header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/
#ifndef __EncoderCtrl_H__
#define __EncoderCtrl_H__
#pragma once

#include "videoEditorType.h"
#include "AudioEncoderCtrl.h"
#include "VideoEncoderCtrl.h"
class CEncoderCtrl 
{
public:
	CEncoderCtrl();
	~CEncoderCtrl(void);
private:
	CAudioEncoderCtrl *m_pAudioEncoderCtrl;
	CVideoEncoderCtrl *m_pVideoEncoderCtrl;
public:
	VO_S32 InitEncoder(VO_S32 isVideo,void* pUserData,VOEDTENCODERCALLBACKPROC proc);
	VO_S32 ToEncoder(VO_S32 isVideo,void* pInData);
	VO_S32 SetEncoderParam(VO_S32 isVideo,void *param);
	VO_S32 UnInit(VO_S32 nVideo);
	VO_U32 SetParam(VO_U32 nPID, VO_VOID* pParam);

	VO_LIB_OPERATOR* m_pLibOp;
};


#endif