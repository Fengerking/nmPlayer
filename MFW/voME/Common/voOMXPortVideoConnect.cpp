	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXPortVideoConnect.cpp

	Contains:	voOMXPortVideoConnect class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif

#include <voOMXMemory.h>

#include "voOMXBase.h"
#include "voOMXOSFun.h"

#include "voOMXPortVideoConnect.h"

OMX_ERRORTYPE voOMXConnectVideoPort (OMX_COMPONENTTYPE * pPrev, OMX_U32 nOutputPort,
										OMX_COMPONENTTYPE * pNext, OMX_U32 nInputPort)
{
	if (pPrev == NULL || pNext == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE		errType = OMX_ErrorNone;
	OMX_S8				szName[128];
	OMX_VERSIONTYPE		verMainPrev;
	OMX_VERSIONTYPE		verMainNext;
	OMX_VERSIONTYPE		verSpec;
	OMX_UUIDTYPE		uuidType;

	verMainPrev.s.nVersionMajor = SPEC_VERSION_MAJOR;
	verMainPrev.s.nVersionMinor = SPEC_VERSION_MINOR;
	verMainPrev.s.nRevision = SPEC_REVISION;
	verMainPrev.s.nStep = SPEC_STEP;
	verMainNext.s.nVersionMajor = SPEC_VERSION_MAJOR;
	verMainNext.s.nVersionMinor = SPEC_VERSION_MINOR;
	verMainNext.s.nRevision = SPEC_REVISION;
	verMainNext.s.nStep = SPEC_STEP;

	if (pPrev->GetComponentVersion != NULL)
	{
		errType = pPrev->GetComponentVersion (pPrev, (OMX_STRING) szName, &verMainPrev, &verSpec, &uuidType);
		if (errType != OMX_ErrorNone)
			return errType;
	}

	if (pNext->GetComponentVersion != NULL)
	{
		errType = pNext->GetComponentVersion (pNext, (OMX_STRING) szName, &verMainNext, &verSpec, &uuidType);
		if (errType != OMX_ErrorNone)
			return errType;
	}

	OMX_PARAM_PORTDEFINITIONTYPE portOutputType;
	voOMXBase_SetHeader (&portOutputType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE), &verMainPrev);
	portOutputType.nPortIndex = nOutputPort;

	errType = pPrev->GetParameter (pPrev, OMX_IndexParamPortDefinition, &portOutputType);
	if (errType != OMX_ErrorNone)
		return errType;

	if (portOutputType.eDomain != OMX_PortDomainVideo)
		return OMX_ErrorPortsNotCompatible;

	portOutputType.nPortIndex = nInputPort;
	errType = pNext->SetParameter (pNext, OMX_IndexParamPortDefinition, &portOutputType);
	if (errType != OMX_ErrorNone)
		return errType;

	OMX_VIDEO_PARAM_PORTFORMATTYPE tpOutoutVideoFormat;
	voOMXBase_SetHeader (&tpOutoutVideoFormat, sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE));
	tpOutoutVideoFormat.nPortIndex = nOutputPort;

	errType = pPrev->GetParameter (pPrev, OMX_IndexParamVideoPortFormat, &tpOutoutVideoFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	tpOutoutVideoFormat.nPortIndex = nInputPort;
	errType = pNext->SetParameter (pNext, OMX_IndexParamVideoPortFormat, &tpOutoutVideoFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	OMX_U32 uDataSize = 512;
	OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *)voOMXMemAlloc (sizeof (OMX_OTHER_EXTRADATATYPE) + uDataSize);
	if (pExtData == NULL)
		return OMX_ErrorNone;

	voOMXBase_SetHeader (pExtData, sizeof (OMX_OTHER_EXTRADATATYPE) + uDataSize);
	pExtData->nPortIndex = nOutputPort;
	pExtData->nDataSize = uDataSize;

	errType = pPrev->GetParameter (pPrev, OMX_IndexParamCommonExtraQuantData, pExtData);
	if (errType == OMX_ErrorNone && pExtData->nDataSize > 0)
	{
		pExtData->nPortIndex = nInputPort;
		errType = pNext->SetParameter (pNext, OMX_IndexParamCommonExtraQuantData, pExtData);
	}
	voOMXMemFree (pExtData);

	return OMX_ErrorNone;
}

