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
#include "voOMX_Drm.h"

class voCOMXCompSource : public voCOMXBaseComponent
{
public:
	voCOMXCompSource(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXCompSource(void);

public:
	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_INDEXTYPE nIndex, OMX_IN  OMX_PTR pComponentParameterStructure);

	static VO_U32			voOMXSourceDrmCallBack (VO_PTR pUserData, VO_U32 nFlag, VO_PTR pParam, VO_U32 nReserved);

	static VO_PTR			FileBasedDrm_Open (VO_FILE_SOURCE * pSource);
	static VO_S32			FileBasedDrm_Read (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);
	static VO_S32			FileBasedDrm_Write (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);
	static VO_S32			FileBasedDrm_Flush (VO_PTR pHandle);
	static VO_S64			FileBasedDrm_Seek (VO_PTR pHandle, VO_S64 nPos, VO_FILE_POS uFlag);
	static VO_S64			FileBasedDrm_Size (VO_PTR pHandle);
	static VO_S64			FileBasedDrm_Save (VO_PTR pHandle);
	static VO_S32			FileBasedDrm_Close (VO_PTR pHandle);

protected:
	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);

	virtual OMX_U32			OnDrmCallBack (OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved);

	// 0 - success; 1 - fail; 2 - next request.
	virtual VO_U32			PlayReady_RequestResponse(VO_PTR hPlayReadyDrm, VO_GET_LICENSE_RESPONSE fGetLicenseResponse, VO_PTR pGetLicenseResponseUserData);

protected:
	OMX_VO_DRM_PLAYREADY_API*	m_pPlayReadyAPI;
	OMX_U8 *					m_pPlayReadyDecryptionInfo;

	IVOFileBasedDRM*			m_pIFileBasedDrm;
	IVOWidevineDRM*				m_pIVOWideVineDRM;

};

#endif //__voCOMXCompSource_H__
