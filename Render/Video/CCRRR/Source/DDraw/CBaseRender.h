	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseRender.h

	Contains:	CBaseRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-10-28	JBF			Create file

*******************************************************************************/

#ifndef __CBaseRender_H__
#define __CBaseRender_H__

#include "DDraw.h"
#include "voCCRRR.h"

//#define _TEST_PERFORMANCE
//#define _OUTPUT_DEBUG

typedef HRESULT (WINAPI * DDCREATE) (GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);

static DDPIXELFORMAT YUYVFormats = 
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, MAKEFOURCC('Y','U','Y','V'),0,0,0,0,0};  // YUYV
static DDPIXELFORMAT UYVYFormats = 
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, MAKEFOURCC('U','Y','V','Y'),0,0,0,0,0};  // UYVY
static DDPIXELFORMAT YV12Formats = 
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, MAKEFOURCC('Y','V','1','2'),0,0,0,0,0};  // YUV420
static DDPIXELFORMAT NV12Formats = 
	{sizeof(DDPIXELFORMAT), DDPF_FOURCC, MAKEFOURCC('N','V','1','2'),0,0,0,0,0};  // YUV420
static DDPIXELFORMAT RGB565Formats = 
	{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0xF800, 0x07e0, 0x001F, 0};  // RGB565

typedef enum
{
	DDVIDEO_YV12		= 0,
	DDVIDEO_NV12		= 1,
	DDVIDEO_YUYV		= 2,
	DDVIDEO_UYVY		= 3,
	DDVIDEO_UNKNOWN		= 100
}
DDVIDEOTYPE;

class CBaseRender
{
public:
	CBaseRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CBaseRender(void);

	virtual VO_U32 GetProperty (VO_CCRRR_PROPERTY * pProperty);
	virtual VO_U32 GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	virtual VO_U32 GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	virtual VO_U32 SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor);
	virtual VO_U32 SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate);
	virtual VO_U32 Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait);
	virtual VO_U32 WaitDone (void);
	virtual VO_U32 SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData);
	virtual VO_U32 GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);
	virtual VO_U32 SetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 GetParam (VO_U32 nID, VO_PTR pValue);

protected:
	virtual VO_U32				Init();
	virtual VO_U32				Close (void);
	virtual int					CreateDrawSurface (void);
	virtual VO_U32				ShowOverlay (bool bShow);

	virtual bool				FillMem (VO_VIDEO_BUFFER * inData, LPBYTE pOutBuffer, int nOutStride);
	virtual bool				PackUV(void* dstUV, void* srcU, void* srcV, int strideU, int strideV, int rows, int width);

	virtual int					GetThreadTime (HANDLE hThread);
	void						GetPropertyFromCfg();
protected:
	VO_CCRRR_PROPERTY		m_prop;
	VO_MEM_OPERATOR *		m_pMemOP;

	HWND					m_hWnd;
	VO_IV_COLORTYPE			m_nInputColor;	
	VO_IV_COLORTYPE			m_nOutputColor;
	VO_IV_RTTYPE			m_nRotate;
	int						m_nInWidth;
	int						m_nInHeight;
	int						m_nOutWidth;
	int						m_nOutHeight;

	VO_BOOL					mbUpsideDown;

	VOVIDEOCALLBACKPROC		m_fCallBack;
	VO_PTR					m_pUserData;

protected:
	HMODULE					m_hDDLib;
	DDCREATE				m_fDDCreate;

#ifdef _WIN32_WCE

#ifdef _WIN32_WCE50
	IDirectDraw4 *			m_pDD;
	IDirectDrawSurface4 *	m_pPrmSurface;
	IDirectDrawSurface4 *	m_pDrwSurface;
	DDSURFACEDESC2			m_DDSurfaceDesc;
#else
	IDirectDraw *			m_pDD;
	IDirectDrawSurface *	m_pPrmSurface;
	IDirectDrawSurface *	m_pDrwSurface;
	DDSURFACEDESC			m_DDSurfaceDesc;
#endif // _WIN32_WCE50

#else
	LPDIRECTDRAW7 			m_pDD;
	LPDIRECTDRAWSURFACE7 	m_pPrmSurface;
	LPDIRECTDRAWSURFACE7 	m_pDrwSurface;
	DDSURFACEDESC2			m_DDSurfaceDesc;
#endif // _WIN32_WCE

	DDCAPS					m_DDCaps;
	DDOVERLAYFX				m_ddOverlayFX;
	DDBLTFX					m_ddBltFX;
	DDVIDEOTYPE				m_ddVideoType;

	bool					m_bOverlay;
	bool					m_bOverlayUpdate;
	bool					m_bOverlayShow;
	bool					m_bOverride;
	int						m_nSurfaceWidth;
	int						m_nSurfaceHeight;

	int						m_nScreenWidth;
	int						m_nScreenHeight;
	RECT					m_rcWnd;
	RECT					m_rcVideo;

	int						m_nRenderFrames;

	int						mnAlignBoundarySrc;
	int						mnAlignSizeSrc;
	int						mnAlignBoundaryDest;
	int						mnAlignSizeDest;

	TCHAR *					mstrWorkPath;
};

#endif //__CBaseRender_H__