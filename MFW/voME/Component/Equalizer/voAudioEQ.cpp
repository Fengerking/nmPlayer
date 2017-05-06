	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voAudioEQ.cpp

	Contains:	Get audio equalizer api cpp file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <OMX_Component.h>
#include <voComponentEntry.h>

#include <voCOMXAudioEQ.h>

#include "voOMX_CompHead.h"

OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetAudioEQHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle)
{
	if (pHandle == NULL)
		return OMX_ErrorBadParameter;

	voCOMXAudioEQ * pvoAudioDec = new voCOMXAudioEQ (pHandle);
	if (pvoAudioDec == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pvoAudioDec;
	APIComponentFillInterface (pHandle);

	return OMX_ErrorNone;
}
