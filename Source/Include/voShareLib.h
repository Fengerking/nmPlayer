
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voShareLib.h

	Contains:	data type define header file

	Written by:	Leon Huang

	Change History (most recent first):
	2013-03-04		Leon Huang			Create file

*******************************************************************************/

#pragma once

#include "voShareLibType.h"

#if defined __cplusplus
extern "C" {
#endif



typedef struct
{
	VO_HANDLE hHandle;

	VO_U32 (VO_API * Init)(VO_PTR* ppHandle, VO_PTR pReserved);
	VO_U32 (VO_API * Uninit)(VO_PTR pHandle);

	VO_U32 (VO_API * SetParameter)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
	VO_U32 (VO_API * GetParameter)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam, COPY_FUNC pFunc );

} VO_ShareLib_API;

VO_S32 VO_API voGetShareLibAPI(VO_ShareLib_API * pShareLibHandle, VO_U32 uFlag);

#if defined __cplusplus
}
#endif
