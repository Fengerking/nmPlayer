	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		IFilterSource.h

	Contains:	IFilterSource interface header file


*******************************************************************************/

#ifndef __IFilterSource_H__
#define __IFilterSource_H__

#include <streams.h>

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_INTERFACE_(IFilterSource, IUnknown)
{
	STDMETHOD(SetParam) (THIS_ int nID, int nValue) PURE;
	STDMETHOD(GetParam) (THIS_ int nID, int * pValue) PURE;
};

#ifdef __cplusplus
}
#endif

#endif //__IFilterSource_H__
