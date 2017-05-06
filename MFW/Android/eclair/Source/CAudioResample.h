	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioResample.h

	Contains:	CAudioResample header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CAudioResample_H__
#define __CAudioResample_H__

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32

#include "vomeAPI.h"
#include "voString.h"

#include "voResample.h"

#include "CDllLoad.h"

class CAudioResample : public CDllLoad
{
public:
	// Used to control the image drawing
	CAudioResample (void);
	virtual ~CAudioResample (void);

public:
	OMX_U32		SetFormat (OMX_S32 nInRate, OMX_S32 nInChannels, OMX_S32 nOutRate, OMX_U32 nOutChannels);
	OMX_U32		ResampleAudio (VO_CODECBUFFER * pInBuf, VO_CODECBUFFER * pOutBuf);

protected:
	OMX_U32		LoadDll (void);
	OMX_U32		Release (void);


	VO_AUDIO_CODECAPI		m_rsAPI;
	OMX_PTR					m_hResample;

	VO_AUDIO_OUTPUTINFO		m_outInfo;
};

#endif // __CAudioResample_H__
