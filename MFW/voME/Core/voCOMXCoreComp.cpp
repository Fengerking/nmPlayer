	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCoreComp.cpp

	Contains:	voCOMXCoreComp class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif

#include "voCOMXCoreComp.h"

typedef OMX_ERRORTYPE (OMX_APIENTRY * VOGETHANDLE) (OMX_INOUT OMX_COMPONENTTYPE * pHandle);

voCOMXCoreComp::voCOMXCoreComp(void)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXCoreComp::~voCOMXCoreComp(void)
{
}

