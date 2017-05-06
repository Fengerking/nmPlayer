	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoResize.h

	Contains:	CVideoResize header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CVideoResize_H__
#define __CVideoResize_H__

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32

#include "vomeAPI.h"
#include "voString.h"

#include "voColorConversion.h"
#include "CDllLoad.h"

class CVideoResize : public CDllLoad
{
public:
	// Used to control the image drawing
	CVideoResize (void);
	virtual ~CVideoResize (void);

public:
	OMX_U32		ResizeBuffer (ClrConvData * pVideoInfo);

protected:
	OMX_U32		LoadDll (void);
	OMX_U32		Release (void);

	VO_CLRCONV_DECAPI		m_rsAPI;
	OMX_PTR					m_hResize;

	ClrConvData				m_ccInfo;

};

#endif // __CVideoResize_H__
