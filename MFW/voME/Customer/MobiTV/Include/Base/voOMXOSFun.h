	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXOSFun.h

	Contains:	voOMXOSFun header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voOMXOSFun_H__
#define __voOMXOSFun_H__

#include <stdio.h>
#include <string.h>
#include <OMX_Types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void			voOMXOS_Sleep (OMX_U32 nTime);
OMX_U32			voOMXOS_GetSysTime (void);
OMX_U32			voOMXOS_GetThreadTime (OMX_PTR hThread);
void			voOMXOS_Printf (OMX_STRING pString);
OMX_U32			voOMXOS_GetAppFolder (OMX_PTR hModule, OMX_STRING pFolder, OMX_U32 nSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__voOMXOSFun_H__
