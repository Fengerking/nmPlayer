	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voDRMShell.h

	Contains:	Windows Media HTTP Streaming Protocol function define header file

	Written by:	Chenhui Liu

	Change History (most recent first):
	2010-02-02		C.H.Liu			Create file

*******************************************************************************/

#ifndef __voDRMShell_H__
#define __voDRMShell_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voDRM2.h"

/**
 * Get DRM engine API interface
 * \param pDRMHandle [out] Return the DRM engine API handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetDRMShellAPI (VO_DRM2_API * pDRMHandle, VO_U32 uFlag);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voDRMShell_H__