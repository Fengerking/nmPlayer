	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		IFilterVideoDecoder.h

	Contains:	IFilterVideoDecoder interface header file


*******************************************************************************/

#ifndef __IFilterVideoDecoder_H__
#define __IFilterVideoDecoder_H__

#include <streams.h>

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_INTERFACE_(IFilterVideoDecoder, IUnknown)
{
	STDMETHOD(SetParam) (THIS_ int nID, int nValue) PURE;
	STDMETHOD(GetParam) (THIS_ int nID, int * pValue) PURE;
};

#ifdef __cplusplus
}
#endif

#endif //__IFilterVideoDecoder_H__
