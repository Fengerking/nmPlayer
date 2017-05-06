	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voComponentEntry.cpp

	Contains:	component api cpp file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <OMX_Component.h>

#include "voComponentEntry.h"
#include "voCOMXBaseComponent.h"

#ifdef _WIN32
#include <windows.h>
//BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
//{
//    return TRUE;
//}
#endif // _WIN32

#define CHECK_OMX_POINT if (hComponent == NULL)\
	return OMX_ErrorInvalidComponent;\
OMX_COMPONENTTYPE * pCompType = (OMX_COMPONENTTYPE *)hComponent;\
if (pCompType->pComponentPrivate == NULL)\
	return OMX_ErrorInvalidComponent;\
voCOMXBaseComponent * pComp = (voCOMXBaseComponent *)pCompType->pComponentPrivate;\

OMX_ERRORTYPE APIGetComponentVersion(OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_OUT OMX_STRING pComponentName,
									OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
									OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
									OMX_OUT OMX_UUIDTYPE* pComponentUUID)
{
	CHECK_OMX_POINT

	return pComp->GetComponentVersion (hComponent, pComponentName, pComponentVersion, pSpecVersion, pComponentUUID);
}

OMX_ERRORTYPE APISendCommand (OMX_IN  OMX_HANDLETYPE hComponent,
								OMX_IN  OMX_COMMANDTYPE Cmd,
								OMX_IN  OMX_U32 nParam1,
								OMX_IN  OMX_PTR pCmdData)
{
	CHECK_OMX_POINT

	return pComp->SendCommand (hComponent, Cmd, nParam1, pCmdData);
}

OMX_ERRORTYPE APIGetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
								OMX_IN  OMX_INDEXTYPE nParamIndex,  
								OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	CHECK_OMX_POINT

	return pComp->GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
}


OMX_ERRORTYPE APISetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
								OMX_IN  OMX_INDEXTYPE nIndex,
								OMX_IN  OMX_PTR pComponentParameterStructure)
{
	CHECK_OMX_POINT

	return pComp->SetParameter (hComponent, nIndex, pComponentParameterStructure);
}


OMX_ERRORTYPE APIGetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
							OMX_IN  OMX_INDEXTYPE nIndex, 
							OMX_INOUT OMX_PTR pComponentConfigStructure)
{
	CHECK_OMX_POINT

	return pComp->GetConfig (hComponent, nIndex, pComponentConfigStructure);
}


OMX_ERRORTYPE APISetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
							OMX_IN  OMX_INDEXTYPE nIndex, 
							OMX_IN  OMX_PTR pComponentConfigStructure)
{
	CHECK_OMX_POINT

	return pComp->SetConfig (hComponent, nIndex, pComponentConfigStructure);
}


OMX_ERRORTYPE APIGetExtensionIndex (OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_IN  OMX_STRING cParameterName,
									OMX_OUT OMX_INDEXTYPE* pIndexType)
{
	CHECK_OMX_POINT

	return pComp->GetExtensionIndex (hComponent, cParameterName, pIndexType);
}

OMX_ERRORTYPE APIGetState (OMX_IN  OMX_HANDLETYPE hComponent,
							OMX_OUT OMX_STATETYPE* pState)
{
	CHECK_OMX_POINT

	return pComp->GetState (hComponent, pState);
}


OMX_ERRORTYPE APIComponentTunnelRequest (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_U32 nPort,
											OMX_IN  OMX_HANDLETYPE hTunneledComp,
											OMX_IN  OMX_U32 nTunneledPort,
											OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup) 
{
	CHECK_OMX_POINT

	return pComp->ComponentTunnelRequest (hComponent, nPort, hTunneledComp, nTunneledPort, pTunnelSetup);
}

OMX_ERRORTYPE APIUseBuffer (OMX_IN OMX_HANDLETYPE hComponent,
							OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
							OMX_IN OMX_U32 nPortIndex,
							OMX_IN OMX_PTR pAppPrivate,
							OMX_IN OMX_U32 nSizeBytes,
							OMX_IN OMX_U8* pBuffer)
{
	CHECK_OMX_POINT

	return pComp->UseBuffer (hComponent, ppBufferHdr, nPortIndex, pAppPrivate, nSizeBytes, pBuffer);
}

OMX_ERRORTYPE APIAllocateBuffer (OMX_IN OMX_HANDLETYPE hComponent,
									OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
									OMX_IN OMX_U32 nPortIndex,
									OMX_IN OMX_PTR pAppPrivate,
									OMX_IN OMX_U32 nSizeBytes)
{
	CHECK_OMX_POINT

	return pComp->AllocateBuffer (hComponent, ppBuffer, nPortIndex, pAppPrivate, nSizeBytes);
}

OMX_ERRORTYPE APIFreeBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
								OMX_IN  OMX_U32 nPortIndex,
								OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	CHECK_OMX_POINT

	return pComp->FreeBuffer (hComponent, nPortIndex, pBuffer);
}

OMX_ERRORTYPE APIEmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	CHECK_OMX_POINT

	return pComp->EmptyThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE APIFillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	CHECK_OMX_POINT

	return pComp->FillThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE APISetCallbacks (OMX_IN  OMX_HANDLETYPE hComponent,
								OMX_IN  OMX_CALLBACKTYPE* pCallbacks, 
								OMX_IN  OMX_PTR pAppData)
{
	CHECK_OMX_POINT

	return pComp->SetCallbacks (hComponent, pCallbacks, pAppData);
}

OMX_ERRORTYPE APIComponentDeInit (OMX_IN  OMX_HANDLETYPE hComponent)
{
	CHECK_OMX_POINT

	delete pComp;
	pCompType->pComponentPrivate = NULL;
	return OMX_ErrorNone;
}

OMX_ERRORTYPE APIUseEGLImage (OMX_IN OMX_HANDLETYPE hComponent,
								OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
								OMX_IN OMX_U32 nPortIndex,
								OMX_IN OMX_PTR pAppPrivate,
								OMX_IN void* eglImage)
{
	CHECK_OMX_POINT

	return pComp->UseEGLImage (hComponent, ppBufferHdr, nPortIndex, pAppPrivate, eglImage);
}

OMX_ERRORTYPE APIComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
									OMX_OUT OMX_U8 *cRole,
									OMX_IN OMX_U32 nIndex)
{
	CHECK_OMX_POINT

	return pComp->ComponentRoleEnum (hComponent, cRole, nIndex);
}

OMX_ERRORTYPE APIComponentFillInterface (OMX_COMPONENTTYPE * pCompType)
{
	if (pCompType == 0)
		return OMX_ErrorBadParameter;

	pCompType->AllocateBuffer = APIAllocateBuffer;
	pCompType->ComponentRoleEnum = APIComponentRoleEnum;
	pCompType->ComponentTunnelRequest = APIComponentTunnelRequest;
	pCompType->EmptyThisBuffer = APIEmptyThisBuffer;
	pCompType->FillThisBuffer = APIFillThisBuffer;
	pCompType->FreeBuffer = APIFreeBuffer;
	pCompType->GetComponentVersion = APIGetComponentVersion;
	pCompType->GetConfig = APIGetConfig;
	pCompType->GetExtensionIndex = APIGetExtensionIndex;
	pCompType->GetParameter = APIGetParameter;
	pCompType->GetState = APIGetState;
	pCompType->SendCommand = APISendCommand;
	pCompType->SetCallbacks = APISetCallbacks;
	pCompType->SetConfig = APISetConfig;
	pCompType->SetParameter = APISetParameter;
	pCompType->UseBuffer = APIUseBuffer;
	pCompType->UseEGLImage = APIUseEGLImage;
	pCompType->ComponentDeInit = APIComponentDeInit;

	return OMX_ErrorNone;
}
