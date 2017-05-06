	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXPortAudio.h

	Contains:	voCOMXPortAudio header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXPortAudio_H__
#define __voCOMXPortAudio_H__

#include "voCOMXBasePort.h"

class voCOMXPortAudio : public voCOMXBasePort
{
public:
	voCOMXPortAudio(voCOMXBaseComponent * pParent, OMX_S32 nIndex);
	virtual ~voCOMXPortAudio(void);
};

#endif //__voCOMXPortAudio_H__