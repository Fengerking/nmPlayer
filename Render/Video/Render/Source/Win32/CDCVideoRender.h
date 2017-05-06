	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDCVideoRender.h

	Contains:	CDCVideoRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CDCVideoRender_H__
#define __CDCVideoRender_H__

#include <windows.h>
#include <ddraw.h>
#include <mmsystem.h>

#include "voOSFunc.h"
#include "voOnstreamType.h"
#include "..\CBaseVideoRender.h"

#define iMASK_COLORS 3
#define SIZE_MASKS (iMASK_COLORS * sizeof(DWORD))

static const int GL_BUFFER_COUNT = 2;
static const int GL_INVAILD = -1;

typedef struct _VO_DCVIDEORENDERDATA
{
	HBITMAP		hBitmap;
	LPBYTE		pBitmapBuffer;
} VO_DCVRDATA;

class CDCVideoRender : public CBaseVideoRender
{
public:
	// Used to control the image drawing
	CDCVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CDCVideoRender (void);

public:
	virtual VO_U32 	SetDispRect (VO_PTR hView, VO_RECT * pDispRect, VO_IV_COLORTYPE nColor = VO_COLOR_RGB565_PACKED);

	virtual VO_U32	Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32	Redraw (void);

	virtual VO_U32 	GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);

	virtual VO_U32 	Start (void);
	virtual VO_U32 	Pause (void);
	virtual VO_U32 	Stop (void);
	virtual VO_U32  SetParam(VO_U32 nID, VO_PTR pParam);

	virtual VO_U32  ShowOverlay(VO_BOOL bShow);

	virtual VO_U32  SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor);

	virtual VO_BOOL	reInitVideoBuffer();
	virtual void	deleteBuffer();
	virtual unsigned char *	lockFrameData();
	virtual void	unlockFrameData(bool bWriteOk,unsigned char* unlockBuffer);

	virtual	VO_BOOL	savePausedVRData(VO_U32 nWidth, VO_U32 nHeight);
	virtual void	deletePausedVRData();

	virtual VO_U32 	SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio);

protected:
	virtual VO_U32	UpdateSize (void);
	bool			CreateBitmapInfo (void);
	virtual void	EraseBackGround();

protected:
	HDC					m_hWinDC;
	HDC					m_hMemDC;

	VO_U32				m_nGDIWidth;
	VO_U32				m_nGDIHeight;
	VO_U32				m_nPixelBits;

	VO_VIDEO_BUFFER		m_outBuffer;
	VO_VIDEO_BUFFER *	m_pInputBuffer;

    voCMutex            m_cDCMutex;
	LPBYTE				m_pBmpInfo;
	HBITMAP				m_hOrigBitmap;
	HBITMAP				m_hPreBitmap;

	voCMutex            m_cBufferMutex;
    VO_DCVRDATA			m_sVRData[GL_BUFFER_COUNT];
    int                 m_iLastOkBuffer;
    int                 m_iUsingBuffer;
    bool                m_bGetNewBuffer;
    bool                m_bIsLastInWriting;

	bool				m_bAspectRatioChanged;
	HBRUSH				m_hBlackBrush;

	bool				m_bRendering;

	// save bitmap for pause state
	VO_BOOL				m_bIsPaused;
	VO_DCVRDATA			m_sPausedVRData;
	VO_U32				m_nPausedVideoWidth;
	VO_U32				m_nPausedVideoHeight;
};

#endif // __CDCVideoRender_H__
