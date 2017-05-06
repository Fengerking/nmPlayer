	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CSubtitleAPIWrapper.h

	Contains:	CSubtitleAPIWrapper header file

	Written by:	Tom Yu Wei

	Change History (most recent first):
	2010-11-11		YW			Create file

*******************************************************************************/

#ifndef __CSubtitleAPIWrapper_H__
#define __CSubtitleAPIWrapper_H__

#include "voYYDef_Common.h"
#include "subtitle3API.h"

#include "voType.h"
#include "voString.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class	CSubtitleAPIWrapper
{
public:
	CSubtitleAPIWrapper(void);
	virtual ~CSubtitleAPIWrapper(void);

protected:
	VO_SUBTITLE_3_API	m_APISubtitle;
	VO_PTR				m_hSubtitle;
	VO_PTR				m_hDll;

	int	 LoadDll(char* chPath);
	void Release();

public:
	int  Init (char* chPath, void* jniEnv);
	bool SetMediaFile(VO_TCHAR* szMediaFilePath);
	bool SetParameter(long lID, long lValue);
	bool GetParameter(long lID, long *plValue);
	bool GetSubtitleObject (long nTimeCurrent, voSubtitleInfo ** pSubtitle, int nTrackIndex);

};

#ifdef _VONAMESPACE
}
#endif

#endif // __CSubtitleAPIWrapper_H__
