#ifndef __CD2DVideoRender_H__
#define __CD2DVideoRender_H__

#include "voType.h"
#include "voIVCommon.h"
#include "DirectXBase.h"

ref class CD2DVideoRender sealed :  public DirectXBase
{
public:
	// Used to control the image drawing
	CD2DVideoRender ();
	virtual ~CD2DVideoRender (void);

	int 	SetVideoInfo (int nWidth, int nHeight , int nStride);
	int 	SetDispRect (int left , int top , int right , int bottom);
	int  	Render (BYTE* pVideoBuffer);

private:
	//video info
	int				mnVideoWidth;
	int				mnVideoHeight;
	int				mnStride;

	//show info
	D2D1_RECT_F		mShowRect;

	//Bitmap 
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>				m_D2DBitmap;
	D2D_RECT_U											mBmpRect;
};

#endif // __CD2DVideoRender_H__
