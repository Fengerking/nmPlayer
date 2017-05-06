	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBS_SMIL.h

	Contains:	CBS_SMIL head file

	Written by:	Aiven

	Change History (most recent first):
	2013-07-08		Aiven			Create file

*******************************************************************************/
#pragma once
#ifndef _CCBS_SMIL_H_
#define _CCBS_SMIL_H_

#include "Smil.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CBS_SMIL:public CSmil{
public:
	CBS_SMIL();
	~CBS_SMIL();
	VO_U32 Parse(VO_PBYTE pBuffer, VO_U32 nSize, VO_PTR* ppSmilStruct);
protected:
	VO_U32 ParseCBS_SMIL(VO_SMIL_STRUCT* pSmil, VO_CBS_SMIL_VIDEO** ppCBSSMIL);
	VO_U32 ReleaseCBS_SMIL(VO_CBS_SMIL_VIDEO** ppCBSSMIL);
	VO_U32 ParseCBS_Video(VO_ELEMENT_STRUCT* pElement, VO_CBS_SMIL_VIDEO** ppCBSSMIL);

protected:
	VO_CBS_SMIL_VIDEO*	m_pCBS_SMIL;	
};

#ifdef _VONAMESPACE
}
#endif

#endif//_CCBS_SMIL_H_





