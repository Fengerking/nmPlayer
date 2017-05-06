	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voVideoDec.cpp

	Contains:	Get Video dec api cpp file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <OMX_Component.h>
#include <voComponentEntry.h>

#include <voCOMXVideoDec.h>

#include "voOMX_CompHead.h"

OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetVideoDecHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle)
{
	if (pHandle == NULL)
		return OMX_ErrorBadParameter;

	voCOMXVideoDec * pvoVideoDec = new voCOMXVideoDec (pHandle);
	if (pvoVideoDec == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pvoVideoDec;
	APIComponentFillInterface (pHandle);

	return OMX_ErrorNone;
}
