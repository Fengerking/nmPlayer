	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voAudioDec.cpp

	Contains:	Get audio dec api cpp file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <OMX_Component.h>
#include <voComponentEntry.h>

#include <voCOMXAudioDoblyEffect.h>
#include "voOMX_CompHead.h"

OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetAudioDoblyEffectHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle)
{
	if (pHandle == NULL)
		return OMX_ErrorBadParameter;

	voCOMXAudioDobyEffect * pvoAudioDoblyEffect = new voCOMXAudioDobyEffect (pHandle);
	if (pvoAudioDoblyEffect == NULL)
		return OMX_ErrorInsufficientResources;

	pHandle->pComponentPrivate = pvoAudioDoblyEffect;
	APIComponentFillInterface (pHandle);

	return OMX_ErrorNone;
}
