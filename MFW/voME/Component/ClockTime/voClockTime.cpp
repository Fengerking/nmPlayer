	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voFileSource.cpp

	Contains:	Get file source api cpp file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <OMX_Component.h>

#include <voComponentEntry.h>
#include <voCOMXClockTime.h>

#include "voOMX_CompHead.h"

OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetClockTimeHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle)
{
	if (pHandle == NULL)
		return OMX_ErrorBadParameter;

	voCOMXClockTime * pvoClockTime = new voCOMXClockTime (pHandle);
	if (pvoClockTime == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pvoClockTime;
	APIComponentFillInterface (pHandle);

	return OMX_ErrorNone;
}
