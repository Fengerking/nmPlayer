	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOBaseVideoRender.h

	Contains:	CVOBaseVideoRender header file

	Written by:	Yu Wei (Tom)

	Change History (most recent first):
	2010-7-7		Tom			Create file

*******************************************************************************/
#ifndef ANDROID_VOBaseVideoRender_H
#define ANDROID_VOBaseVideoRender_H

#include <stdio.h>

#if !defined __VONJ_FROYO__
#include <ui/ISurface.h>
#include <ui/SurfaceComposerClient.h>
#else
#include <surfaceflinger/ISurface.h>
#include <surfaceflinger/SurfaceComposerClient.h>
#endif


#include "vojniconst.h"




namespace android {

class VOBaseVideoRender
{
public:
	VOBaseVideoRender(void)
	{
		m_pSurface = NULL;
		m_nVideoWidth = 0;
		m_nVideoHeight = 0;
		strcpy (m_szWorkingPath, "");
	}
	virtual ~VOBaseVideoRender(void)
	{
		if (m_pSurface != NULL)
			m_pSurface.clear ();
	}

	virtual int					SetVideoSurface (const sp<Surface>& surface) {m_pSurface = surface;};
	virtual int					SetVideoSize (int nWidth, int nHeight) {m_nVideoWidth = nWidth; m_nVideoHeight = nHeight;}
	virtual bool				CheckColorType (VO_ANDROID_COLORTYPE nColorType) {return true;};

	virtual bool				Render(VO_ANDROID_VIDEO_BUFFERTYPE* pVideoBuffer) = 0;

	virtual unsigned char *		GetOutputBuffer (void) {return NULL;};
	virtual void				setWorkingPath(const char* path) {strcpy (m_szWorkingPath, path);}

protected:
	sp<Surface>				m_pSurface;

  	int						m_nVideoWidth;
 	int						m_nVideoHeight;

	char					m_szWorkingPath[256];

};

}; // namespace android

#endif // ANDROID_VOBaseVideoRender_H

