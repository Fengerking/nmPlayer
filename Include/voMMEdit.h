/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __voMMEdit_H__
#define __voMMEdit_H__



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voMMPlay.h"


typedef VO_U32 (VO_API * VOMMPlayCallBack) (VO_PTR pUserData, VO_U32 nID, VO_PTR pValue1, VO_PTR pValue2);

typedef struct
{
	VO_U32 (VO_API * Init) (VO_HANDLE * phPlay, VO_VOMM_INITPARAM * pParam);
	VO_U32 (VO_API * Uninit) (VO_HANDLE hPlay);
	VO_U32 (VO_API * SetCallBack) (VO_HANDLE hPlay, VOMMPlayCallBack pCallBack, VO_PTR pUserData);
	VO_U32 (VO_API * SetViewInfo) (VO_HANDLE hPlay, VO_PTR hView, VO_RECT * pRect);
	VO_U32 (VO_API * Create) (VO_HANDLE hPlay, VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	VO_U32 (VO_API * Run) (VO_HANDLE hPlay);
	VO_U32 (VO_API * Pause) (VO_HANDLE hPlay);
	VO_U32 (VO_API * Stop) (VO_HANDLE hPlay);
	VO_U32 (VO_API * GetDuration) (VO_HANDLE hPlay, VO_U32 * pDuration);
	VO_U32 (VO_API * GetCurPos) (VO_HANDLE hPlay, VO_S32 * pPos);
	VO_U32 (VO_API * SetCurPos) (VO_HANDLE hPlay, VO_S32 nPos);
	VO_U32 (VO_API * SetParam) (VO_HANDLE hPlay, VO_U32 nID, VO_PTR pValue);
	VO_U32 (VO_API * GetParam) (VO_HANDLE hPlay, VO_U32 nID, VO_PTR pValue);

	VO_U32 (VO_API * EditSetParam) (VO_HANDLE hPlay, VO_U32 nID, VO_U32 nValue, VO_PTR pValue);
	VO_U32 (VO_API * EditInit) (VO_HANDLE * phPlay, VO_VOMM_INITPARAM * pParam);

} VOMM_EDITAPI;

/**
 * Get voMM Edit API interface
 * \param pPlay [IN/OUT] Return the play API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_None Succeeded.
 */
VO_S32 VO_API vommGetEditAPI (VOMM_PLAYAPI * pPlay, VO_U32 uFlag);


#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif // __voMMEdit_H__


