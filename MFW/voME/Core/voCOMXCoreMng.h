	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreMng.h

	Contains:	voCOMXCoreMng header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voCOMXCoreMng_H__
#define __voCOMXCoreMng_H__

#include <OMX_Core.h>
#include <OMX_Component.h>

#include "voCOMXBaseObject.h"

#include "voCOMXConfigCore.h"

struct voOMXCompInfo
{
	OMX_COMPONENTTYPE *	Handle;
	OMX_PTR				File;
	OMX_U32				Priority;
	OMX_U32				GroupID;
};

class voCOMXCoreMng : public voCOMXBaseObject
{
public:
	voCOMXCoreMng(void);
	virtual ~voCOMXCoreMng(void);

	virtual OMX_ERRORTYPE	Init (OMX_PTR hInst);
	virtual OMX_STRING		GetName (OMX_U32 nIndex);

	virtual OMX_ERRORTYPE	LoadComponent (OMX_COMPONENTTYPE * pHandle, OMX_STRING pName);
	virtual OMX_ERRORTYPE	FreeComponent (OMX_COMPONENTTYPE * pHandle);


protected:
	voCOMXConfigCore	m_Config;
};

#endif // __voCOMXCoreMng_H__
