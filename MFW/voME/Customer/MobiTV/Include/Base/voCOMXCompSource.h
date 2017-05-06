	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCompSource.h

	Contains:	voCOMXCompSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXCompSource_H__
#define __voCOMXCompSource_H__

#include "voCOMXBaseComponent.h"

class voCOMXCompSource : public voCOMXBaseComponent
{
public:
	voCOMXCompSource(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXCompSource(void);

protected:
	virtual OMX_ERRORTYPE		SetNewState (OMX_STATETYPE newState);

};

#endif //__voCOMXCompSource_H__
