	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCSourceBuffer.h

	Contains:	file operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-09-03		JBF			Create file

*******************************************************************************/

#ifndef __vompCSourceBuffer_H__
#define __vompCSourceBuffer_H__

#include "vompAPI.h"
#include "voType.h"

int vompReadAudio (void * pUserData, VOMP_BUFFERTYPE * pBuffer);
int	vompReadVideo (void * pUserData, VOMP_BUFFERTYPE * pBuffer);

class vompCSourceBuffer
{
public:
    vompCSourceBuffer(void);
    virtual ~vompCSourceBuffer(void);

	int			ReadAudioBuffer (VOMP_BUFFERTYPE * pBuffer);
	int			ReadVideoBuffer (VOMP_BUFFERTYPE * pBuffer);

protected:
	VO_PBYTE		m_pAudioBuffer;
	int				m_nAudioFileSize;
	VO_PBYTE		m_pAudioStartData;
	VO_PBYTE		m_pAudioNextData;
	long long		m_llAudioTime;

	VO_PBYTE		m_pVideoBuffer;
	int				m_nVideoFileSize;
	VO_PBYTE		m_pVideoStartData;
	VO_PBYTE		m_pVideoNextData;
	long long		m_llVideoTime;
};

#endif // __vompCSourceBuffer_H__
