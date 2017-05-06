/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		EncoderCtrl.cpp

Contains:	EncoderCtrl class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/
#include "EncoderCtrl.h"

CEncoderCtrl::CEncoderCtrl()
:m_pVideoEncoderCtrl(NULL)
,m_pAudioEncoderCtrl(NULL)
{
	m_pLibOp = NULL;
}

CEncoderCtrl::~CEncoderCtrl(void)
{
	UnInit(1);
	UnInit(0);
}

VO_S32 CEncoderCtrl::ToEncoder(VO_S32 isVideo,void* pInData)
{
	CEncoderBaseCtrl *encoderctrl;
	isVideo ? encoderctrl = m_pVideoEncoderCtrl :encoderctrl = m_pAudioEncoderCtrl;
	encoderctrl->ToEncoder(pInData);

	return 0;
}
VO_S32 CEncoderCtrl::SetEncoderParam(VO_S32 isVideo,void *param)
{
	CEncoderBaseCtrl *encoderctrl;
	isVideo ? encoderctrl = m_pVideoEncoderCtrl :encoderctrl = m_pAudioEncoderCtrl;
	encoderctrl->SetEncoderParam(param);

	return 0;
}

VO_S32 CEncoderCtrl::InitEncoder(VO_S32 isVideo,void* pUserData,VOEDTENCODERCALLBACKPROC proc)
{

	if(isVideo)
	{
		if(m_pVideoEncoderCtrl) delete m_pVideoEncoderCtrl;
		m_pVideoEncoderCtrl = new CVideoEncoderCtrl(m_pLibOp,pUserData,proc);
	}
	else
	{
		if(m_pAudioEncoderCtrl) delete m_pAudioEncoderCtrl;
		m_pAudioEncoderCtrl = new CAudioEncoderCtrl(m_pLibOp,pUserData,proc);
	}
	
	return 0;
}

VO_S32 CEncoderCtrl::UnInit(VO_S32 nVideo)
{
	if(nVideo && m_pVideoEncoderCtrl ) 
	{
		delete m_pVideoEncoderCtrl;	
		m_pVideoEncoderCtrl = NULL; 
	}
	if(!nVideo && m_pAudioEncoderCtrl)	
	{
		delete m_pAudioEncoderCtrl;	
		m_pAudioEncoderCtrl = NULL;	
	}

	return 0;
}
VO_U32 CEncoderCtrl::SetParam(VO_U32 nPID, VO_VOID* pParam)
{
	VO_U32 nRc = VO_ERR_NONE;
	switch (nPID)
	{
	case VOEDT_PID_PLAYER_LIB_OPERATOR:
		m_pLibOp =(VO_LIB_OPERATOR* ) pParam;
		break;
	}
	return nRc;
}
