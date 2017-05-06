#pragma once

#include "CBaseCCRRR.h"
#include"renderersdk.h"

class CSirfCCRRR :	public CBaseCCRRR
{
public:
	CSirfCCRRR(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	~CSirfCCRRR(void);

	virtual VO_U32 GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	virtual VO_U32 GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	virtual VO_U32 SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor);
	virtual VO_U32 SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate);
	virtual VO_U32 Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait);

	virtual VO_U32 SetParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32 GetParam (VO_U32 nID, VO_PTR pValue);
protected:
	VO_U32			Init();
	VO_U32			Uninit();

	VO_U32			ShowOverlay(VO_BOOL bShow);
protected:
	VR_HANDLE			mhRender;
	HWND				mhWnd;
	VR_FRAME_SURFACE*	m_pSurf;

	VO_U32				mnYStride;
	VO_U32				mnUVStride;

	VO_U32				mnInputWidth;
	VO_U32				mnInputHeight;

	VO_U32					mnScreenWidth;
	VO_U32					mnScreenHeight;
	VO_BOOL					mbPrimary;
	VO_BOOL					mbFullScreen;

	BOOL					mbShowOverlay;
};
