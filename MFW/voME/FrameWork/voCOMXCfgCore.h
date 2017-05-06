	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCfgCore.h

	Contains:	voCOMXCfgCore header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voCOMXCfgCore_H__
#define __voCOMXCfgCore_H__

#include "voCOMXBaseConfig.h"

class voCOMXCfgCore : public voCOMXBaseConfig
{
public:
	voCOMXCfgCore(void);
	virtual ~voCOMXCfgCore(void);

	virtual	OMX_BOOL	Open (OMX_STRING pFile);


	virtual COMXCfgSect *	GetCoreSect (void);

	virtual OMX_STRING		GetSourceCompName (OMX_STRING pExt);

	virtual OMX_STRING		GetVideoSinkCompName (OMX_U32 nPriority);
	virtual OMX_STRING		GetAudioSinkCompName (OMX_U32 nPriority);
	virtual OMX_STRING		GetClockTimeCompName (OMX_U32 nPriority);
};

#endif // __voCOMXCfgCore_H__