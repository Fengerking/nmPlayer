	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseObject.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-31		JBF			Create file

*******************************************************************************/

#ifndef __voCOMXBaseObject_H__
#define __voCOMXBaseObject_H__

#include <string.h>
#include "OMX_Types.h"

class voCOMXBaseObject
{
public:
	voCOMXBaseObject(void);
	virtual ~voCOMXBaseObject(void);

protected:
	OMX_STRING		m_pObjName;

public:
	static	OMX_S32	g_ObjectNum;
};

#endif // __voCOMXBaseObject_H__
