	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		IFilterVideoRender.h

	Contains:	IFilterVideoRender interface header file


*******************************************************************************/

#ifndef __IFilterVideoRender_H__
#define __IFilterVideoRender_H__

#include <streams.h>
#include "voIVCommon.h"

#ifdef __cplusplus
extern "C" {
#endif


DECLARE_INTERFACE_(IFilterVideoRender, IUnknown)
{
	STDMETHOD(SetZoomMode)	(THIS_ VO_IV_ZOOM_MODE nMode) PURE;
	STDMETHOD(SetRotate)	(THIS_ VO_IV_RTTYPE nRtType) PURE;
	STDMETHOD(SetParam)		(THIS_ int nID, int nValue) PURE;
	STDMETHOD(GetParam)		(THIS_ int nID, int * pValue) PURE;
};

#ifdef __cplusplus
}
#endif

#endif //__IFilterVideoRender_H__