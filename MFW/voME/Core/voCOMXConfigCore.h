	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXConfigCore.h

	Contains:	voCOMXConfigCore header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voCOMXConfigCore_H__
#define __voCOMXConfigCore_H__

#include "voCOMXBaseConfig.h"

class voCOMXConfigCore : public voCOMXBaseConfig
{
public:
	voCOMXConfigCore(void);
	virtual ~voCOMXConfigCore(void);

	virtual OMX_STRING	GetCompName (OMX_U32 nIndex);

	virtual OMX_STRING	GetCompFile (OMX_STRING pCompName);
	virtual OMX_STRING	GetCompAPI (OMX_STRING pCompName);
	virtual OMX_U32		GetCompPriority (OMX_STRING pCompName);
	virtual OMX_U32		GetCompGroupID (OMX_STRING pCompName);

};

#endif // __voCOMXConfigCore_H__
