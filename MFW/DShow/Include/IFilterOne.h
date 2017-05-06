	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		IFilterOne.h

	Contains:	IIFilterOne interface header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-12-02		JBF			Create file

*******************************************************************************/

#ifndef __IFilterOne_H__
#define __IFilterOne_H__

#include <streams.h>

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_INTERFACE_(IFilterOne, IUnknown)
{
	STDMETHOD(SetParam) (THIS_ int nID, int nValue) PURE;
	STDMETHOD(GetParam) (THIS_ int nID, int * pValue) PURE;
};

#ifdef __cplusplus
}
#endif

#endif //__IFilterOne_H__
