	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreComp.h

	Contains:	voCOMXCoreComp header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voCOMXCoreComp_H__
#define __voCOMXCoreComp_H__

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "voCOMXBaseObject.h"

class voCOMXCoreComp : public voCOMXBaseObject
{
public:
	voCOMXCoreComp(void);
	virtual ~voCOMXCoreComp(void);


};

#endif // __voCOMXCoreComp_H__