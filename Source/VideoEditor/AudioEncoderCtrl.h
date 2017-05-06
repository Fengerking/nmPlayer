
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		AudioEncoderCtrl.h

Contains:	AudioEncoderCtrl header file

Written by:   Leon Huang
Change History (most recent first):
2011-01-13		Leon		 Create file
*******************************************************************************/
#ifndef __AudioEncoderCtrl_H__
#define __AudioEncoderCtrl_H__
#pragma once

#include "EncoderBaseCtrl.h"
#include "CAudioEncoder.h"
class CAudioEncoderCtrl : public CEncoderBaseCtrl
{
public:
	CAudioEncoderCtrl(VO_LIB_OPERATOR* pLibOp,void* pUserData,VOEDTENCODERCALLBACKPROC proc);
	~CAudioEncoderCtrl(void);
public:
	virtual VO_S32 SetEncoderParam(VO_VOID *param);
	virtual VO_S32 ToEncoder(VO_VOID *InData);
	virtual VO_VOID ReleaseCtrl();

	CAudioEncoder *m_audioEncoder;
	VO_U32 m_nPreviousTime ;

};

#endif