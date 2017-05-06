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
#include <voCOMXFileSource.h>
#include "voOMX_CompHead.h"

#define LOG_TAG "voGetFileSourceHandle"

OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetFileSourceHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle)
{
	if (pHandle == NULL)
		return OMX_ErrorBadParameter;

	voCOMXFileSource * pvoFileSource = new voCOMXFileSource (pHandle);
	if (pvoFileSource == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pvoFileSource;
	APIComponentFillInterface (pHandle);

	return OMX_ErrorNone;
}
