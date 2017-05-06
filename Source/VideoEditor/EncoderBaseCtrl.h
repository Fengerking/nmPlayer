/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		EncoderBaseCtrl.h

Contains:	EncoderBaseCtrl header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/

#ifndef __EncoderBaseCtrl_H__
#define __EncoderBaseCtrl_H__
#pragma once

#include "videoEditorType.h"


class CEncoderBaseCtrl
{
public:
	CEncoderBaseCtrl(VO_LIB_OPERATOR* pParam,void* pUserData,VOEDTENCODERCALLBACKPROC proc);
	virtual ~CEncoderBaseCtrl(void);

	virtual VO_S32 SetEncoderParam(void *encParam);
	virtual VO_S32  ToEncoder(void *InData);
	virtual VO_VOID ReleaseCtrl();

	VOEDTENCODERCALLBACKPROC m_proc;
	VO_VOID*      m_pUserData;
	VO_CODECBUFFER m_OutData;
	VO_BOOL m_bExit;
	VO_LIB_OPERATOR*		m_pLibOp;
private:
	

};

#endif
