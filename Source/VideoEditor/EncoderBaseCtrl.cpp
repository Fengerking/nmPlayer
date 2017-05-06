/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		EncoderBaseCtrl.cpp

Contains:	EncoderBaseCtrl class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/
#include "EncoderBaseCtrl.h"

CEncoderBaseCtrl::CEncoderBaseCtrl(VO_LIB_OPERATOR* pParam,void* pUserData,VOEDTENCODERCALLBACKPROC proc)
:m_pUserData(pUserData)
,m_pLibOp(pParam)
,m_proc(proc)
,m_bExit(VO_FALSE)
{
	m_OutData.Buffer = NULL;
	m_OutData.Length = 0;
	m_OutData.Time = 0;
}

CEncoderBaseCtrl::~CEncoderBaseCtrl(void)
{
	m_bExit = VO_FALSE;
	voOS_Sleep(100);
	if(m_OutData.Buffer) free(m_OutData.Buffer);
}

VO_S32 CEncoderBaseCtrl::SetEncoderParam(void *param)
{	
	return 0;
}
VO_S32  CEncoderBaseCtrl::ToEncoder(void *InData)
{
	return VO_ERR_NONE;
}
VO_VOID CEncoderBaseCtrl::ReleaseCtrl()
{

}
VO_S32 SetParam(VO_U32 nID, VO_VOID* pParam)
{
	return VO_ERR_NONE;
}