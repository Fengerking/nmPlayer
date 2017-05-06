	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXPortInplace.h

	Contains:	voCOMXPortInplace header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXPortInplace_H__
#define __voCOMXPortInplace_H__

#include "voCOMXBasePort.h"

class voCOMXCompInplace;

class voCOMXPortInplace : public voCOMXBasePort
{
friend class voCOMXBaseComponent;

public:
	voCOMXPortInplace(voCOMXBaseComponent * pParent, OMX_S32 nIndex, OMX_DIRTYPE dirType);
	virtual ~voCOMXPortInplace(void);

	virtual OMX_ERRORTYPE	AllocBuffer (OMX_BUFFERHEADERTYPE** pBuffer, OMX_U32 nPortIndex,
										 OMX_PTR pAppPrivate,OMX_U32 nSizeBytes);

	virtual OMX_ERRORTYPE	AllocTunnelBuffer (OMX_U32 nPortIndex, OMX_U32 nSizeBytes);
};

#endif //__voCOMXPortInplace_H__