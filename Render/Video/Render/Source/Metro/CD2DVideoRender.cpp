
#include "CD2DVideoRender.h"
#include "voIndex.h"

CD2DVideoRender::CD2DVideoRender()
	:mnVideoWidth(0),
	mnVideoHeight(0),
	mnStride(0),
	m_D2DBitmap(nullptr)
{
	memset(&mShowRect , 0 , sizeof(mShowRect));
}

CD2DVideoRender::~CD2DVideoRender ()
{
}

int 	CD2DVideoRender::SetVideoInfo (int nWidth, int nHeight , int nStride)
{
	mnVideoWidth = nWidth;
	mnVideoHeight = nHeight;
	mnStride  = nStride;

	return VO_ERR_NONE;
}


int 	CD2DVideoRender::SetDispRect (int left , int top , int right , int bottom)
{
	mShowRect.left = left;
	mShowRect.top  = top;
	mShowRect.right = right;
	mShowRect.bottom = bottom;

	return VO_ERR_NONE;
}

int CD2DVideoRender::Render (BYTE * pVideoBuffer )
{
	if(m_d2dContext == nullptr || pVideoBuffer == NULL)
		return -1;

	if(m_D2DBitmap == nullptr)
	{
		D2D_SIZE_U bmpSize;
		bmpSize.width = mnVideoWidth;
		bmpSize.height = mnVideoHeight;

		D2D1_BITMAP_PROPERTIES1 bmpProp;
		bmpProp.dpiX = 96.0f;
		bmpProp.dpiY = 96.0f;
		bmpProp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		bmpProp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
		bmpProp.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE ;
		bmpProp.colorContext = nullptr;
		HRESULT hr = m_d2dContext->CreateBitmap(bmpSize ,  NULL , 0 , bmpProp , &m_D2DBitmap);

		mBmpRect.left = 0;
		mBmpRect.top  = 0;
		mBmpRect.right = mnVideoWidth;
		mBmpRect.bottom = mnVideoHeight;
	}

	if(m_D2DBitmap != nullptr)
	{
		m_D2DBitmap->CopyFromMemory(&mBmpRect , pVideoBuffer , mnVideoWidth * 4);
	}

	m_d2dContext->BeginDraw();

	m_d2dContext->DrawBitmap(m_D2DBitmap.Get() , mShowRect );

	HRESULT hr = m_d2dContext->EndDraw();

	Present();
	return VO_ERR_NONE;
}


