	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voComponentEntry.h

	Contains:	voComponentEntry header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voComponentEntry_H__
#define __voComponentEntry_H__

#include <OMX_Component.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


OMX_ERRORTYPE APIGetComponentVersion (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_OUT OMX_STRING pComponentName,
										OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
										OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
										OMX_OUT OMX_UUIDTYPE* pComponentUUID);

OMX_ERRORTYPE APISendCommand (OMX_IN  OMX_HANDLETYPE hComponent,
								OMX_IN  OMX_COMMANDTYPE Cmd,
								OMX_IN  OMX_U32 nParam1,
								OMX_IN  OMX_PTR pCmdData);

OMX_ERRORTYPE APIGetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
								OMX_IN  OMX_INDEXTYPE nParamIndex,  
								OMX_INOUT OMX_PTR pComponentParameterStructure);


OMX_ERRORTYPE APISetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
								OMX_IN  OMX_INDEXTYPE nIndex,
								OMX_IN  OMX_PTR pComponentParameterStructure);


OMX_ERRORTYPE APIGetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
							OMX_IN  OMX_INDEXTYPE nIndex, 
							OMX_INOUT OMX_PTR pComponentConfigStructure);


OMX_ERRORTYPE APISetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
							OMX_IN  OMX_INDEXTYPE nIndex, 
							OMX_IN  OMX_PTR pComponentConfigStructure);


OMX_ERRORTYPE APIGetExtensionIndex (OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_IN  OMX_STRING cParameterName,
									OMX_OUT OMX_INDEXTYPE* pIndexType);

OMX_ERRORTYPE APIGetState (OMX_IN  OMX_HANDLETYPE hComponent,
								OMX_OUT OMX_STATETYPE* pState);


OMX_ERRORTYPE APIComponentTunnelRequest (OMX_IN  OMX_HANDLETYPE hComp,
											OMX_IN  OMX_U32 nPort,
											OMX_IN  OMX_HANDLETYPE hTunneledComp,
											OMX_IN  OMX_U32 nTunneledPort,
											OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup); 

OMX_ERRORTYPE APIUseBuffer (OMX_IN OMX_HANDLETYPE hComponent,
							OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
							OMX_IN OMX_U32 nPortIndex,
							OMX_IN OMX_PTR pAppPrivate,
							OMX_IN OMX_U32 nSizeBytes,
							OMX_IN OMX_U8* pBuffer);

OMX_ERRORTYPE APIAllocateBuffer (OMX_IN OMX_HANDLETYPE hComponent,
									OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
									OMX_IN OMX_U32 nPortIndex,
									OMX_IN OMX_PTR pAppPrivate,
									OMX_IN OMX_U32 nSizeBytes);

 OMX_ERRORTYPE APIFreeBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
								OMX_IN  OMX_U32 nPortIndex,
								OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

OMX_ERRORTYPE APIEmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

OMX_ERRORTYPE APIFillThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
									OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

 OMX_ERRORTYPE APISetCallbacks (OMX_IN  OMX_HANDLETYPE hComponent,
								OMX_IN  OMX_CALLBACKTYPE* pCallbacks, 
								OMX_IN  OMX_PTR pAppData);

OMX_ERRORTYPE APIComponentDeInit (OMX_IN  OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE APIUseEGLImage (OMX_IN OMX_HANDLETYPE hComponent,
								OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
								OMX_IN OMX_U32 nPortIndex,
								OMX_IN OMX_PTR pAppPrivate,
								OMX_IN void* eglImage);

OMX_ERRORTYPE APIComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
									OMX_OUT OMX_U8 *cRole,
									OMX_IN OMX_U32 nIndex);

OMX_ERRORTYPE APIComponentFillInterface (OMX_COMPONENTTYPE * pCompType);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__voComponentEntry_H__
