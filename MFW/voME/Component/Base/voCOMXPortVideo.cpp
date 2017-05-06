	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXPortAudio.cpp

	Contains:	voCOMXPortAudio class file

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

#include "voOMXBase.h"
#include "voOMXOSFun.h"

#include "voCOMXBaseComponent.h"

#include "voCOMXPortAudio.h"

voCOMXPortAudio::voCOMXPortAudio(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXBasePort (pParent, nIndex, OMX_DirOutput)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
}

voCOMXPortAudio::~voCOMXPortAudio(void)
{
}

