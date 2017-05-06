// CDirectFBDraw.h
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CSDLDraw.h

    aim:    for providing the  reader based on SDL

	Written by:	Xia GuangTai

	Change History (most recent first):
	2007-11-29		gtxia			Create file

*******************************************************************************/

#ifndef _CSDL_DRAW2_H
#define _CSDL_DRAW2_H

#include <SDL.h>
#include "CBaseVideoRender.h"


class CSDLDraw : public CBaseVideoRender
{
public:
	CSDLDraw(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	~CSDLDraw();

public:
	bool   InitDevice();
	bool   CloseDevice();

    VO_U32 	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);


protected:	
	bool   onDrawImage(VO_VIDEO_BUFFER* pBuffer);


private:
	bool   prepareFrame(VO_VIDEO_BUFFER* lpdata);
	void   getSDLVideoInfo();


private:
	bool             mIsHWSurface;
	SDL_Surface*     mScreen;  // main surface to be draw image on it

    VO_VIDEO_BUFFER	 mOutBuffer;

#if defined(_RGB_OUTPUT)	
	SDL_Surface*     mSurface; 
#elif defined(_YUV_OUTPUT)
	SDL_Overlay*    mOverlay;
#endif	

};

#endif

