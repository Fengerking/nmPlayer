/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		VideoEncoderCtrl.h

Contains:	VideoEncoderCtrl header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-18		Leon			Create file
*******************************************************************************/

#ifndef __CVideoEncoderCtrl_H__
#define __CVideoEncoderCtrl_H__
#pragma once

#include "EncoderBaseCtrl.h"
#include "CVideoEncoder.h"
//encoder param

class CVideoEncoderCtrl : public CEncoderBaseCtrl
{
public:
	CVideoEncoderCtrl(VO_LIB_OPERATOR* pParam,void* pUserData,VOEDTENCODERCALLBACKPROC proc);

	virtual ~CVideoEncoderCtrl(void);
public:
	virtual VO_S32 SetEncoderParam(void *param);
	virtual VO_S32  ToEncoder(void *InData);
	virtual VO_VOID ReleaseCtrl();
	
private:
	CVideoEncoder *m_videoEncoder;
	VOEDT_VIDEO_CODINGTYPE m_nCodecType;
};
#endif