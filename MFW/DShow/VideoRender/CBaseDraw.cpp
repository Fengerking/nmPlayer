#include "CBaseDraw.h"
#include "CCCRRRFunc.h"
#include "CVideoWindow.h"

#include "voLog.h"


#define swap(x,y) {int s=x;x=y;y=s;}

CBaseDraw::CBaseDraw(HWND hWnd , CVideoWindow *pVW)
: mhWnd(hWnd)
, mpVideoWindow(pVW)
, mnInputWidth(0)
, mnInputHeight(0)
, mzmZoomMode(VO_ZM_PANSCAN)
, mrtRotateType(VO_RT_DISABLE)
, mbUpdateSize(false)
, mnOutHeight(0)
, mnOutWidth(0)
, mnInputVideoH(0)
, mnInputVideoW(0)
, mpCCRRR(NULL)
, mInputColor(VO_COLOR_YUV_PLANAR420)
, mOutputColor(VO_COLOR_RGB565_PACKED)
, mpBitmapInfo(NULL)
, mpBitmapBuffer(NULL)
, mnBitmapStride(0)
, mhBitmap(NULL)
, mhDC(NULL)
, mhMemoryDC(NULL)
{
	cmnMemFillPointer('FTVR');

	if(mpVideoWindow != NULL)
	{
		mpVideoWindow->AddRef();
		mpVideoWindow->SetDraw(this);
	}

	SetRectEmpty(&mrcWnd);
	ZeroMemory(&mvbInBuf , sizeof(mvbInBuf));
	ZeroMemory(&mvbOutBuf , sizeof(mvbOutBuf));

	mnScreenX =  GetSystemMetrics( SM_CXSCREEN );
	mnScreenY =  GetSystemMetrics( SM_CYSCREEN );

	LoadCCRRR();	
}

CBaseDraw::~CBaseDraw(void)
{
	if(mpVideoWindow != NULL)
	{
		mpVideoWindow->Release();
		mpVideoWindow = NULL;
	}
}

bool	CBaseDraw::SetInputSize(int nWidth , int nHeight)
{
	if(mnInputWidth == nWidth && mnInputHeight == nHeight)
		return true;

	mnInputWidth = nWidth;
	mnInputHeight = nHeight;

	mbUpdateSize = true;
	return true;
}

bool	CBaseDraw::SetZoomMode(VO_IV_ZOOM_MODE zm)
{
	if(mzmZoomMode == zm)
		return true;

	mzmZoomMode = zm;

	mbUpdateSize = true;
	return true;
}

bool	CBaseDraw::SetRotate(VO_IV_RTTYPE rt)
{
	if(mrtRotateType == rt)
		return true;

	mrtRotateType = rt;

	mbUpdateSize = true;
	return true;
}

bool	CBaseDraw::SetInputColor(VO_IV_COLORTYPE ct)
{
	if(mInputColor == ct)
		return true;

	mInputColor = ct;

	mbUpdateSize = true;
	return true;
}

bool	CBaseDraw::DrawImage(IMediaSample *pSample)
{
	bool bRC = false;
	if(mbUpdateSize)
	{
		bRC = UpdateSize();
		mbUpdateSize = false;
	}

	if(mhDC == NULL)
	{
		mhDC = mpVideoWindow->GetWindowHDC();
		mhMemoryDC = mpVideoWindow->GetMemoryHDC();
	}

	if(mhBitmap == NULL)
	{
		CreateBitmapInfo();
		mhBitmap = CreateDIBSection(mhDC , (BITMAPINFO *)mpBitmapInfo , DIB_RGB_COLORS , (void **)&mpBitmapBuffer , NULL , 0);
	}

	int nBufLen = pSample->GetActualDataLength();
	BYTE *pBuf = NULL;
	HRESULT hr = pSample->GetPointer(&pBuf);
	if(hr != S_OK || pBuf == NULL)
		return false;

	if(mInputColor == VO_COLOR_YUV_PLANAR420)
	{
		if(nBufLen == 40)
		{
			memcpy(&mvbInBuf , pBuf , sizeof(mvbInBuf));
		}
		else
		{
			mvbInBuf.Buffer[0] = pBuf;
			mvbInBuf.Buffer[1] = pBuf + mnInputHeight * mnInputWidth;
			mvbInBuf.Buffer[2] = pBuf + mnInputHeight * mnInputWidth * 5 / 4;

			mvbInBuf.Stride[0] = mnInputWidth;
			mvbInBuf.Stride[1] = mvbInBuf.Stride[2] = mnInputWidth / 2;
		}

		mvbInBuf.ColorType = VO_COLOR_YUV_PLANAR420;
	}
	else if(mInputColor == VO_COLOR_RGB565_PACKED)
	{
		
	}
	else
	{
		return false;
	}

	mvbOutBuf.Buffer[0] = mpBitmapBuffer;

	mvbOutBuf.Stride[0] = mnOutWidth * 2;
	mvbOutBuf.ColorType = VO_COLOR_RGB565_PACKED;

	if(mnInputHeight != mnInputVideoH )
	{
		int nOffset = (mnInputHeight - mnInputVideoH) / 2;
		mvbInBuf.Buffer[0] += mvbInBuf.Stride[0] * nOffset;
		mvbInBuf.Buffer[1] += mvbInBuf.Stride[1] * nOffset;
		mvbInBuf.Buffer[2] += mvbInBuf.Stride[2] * nOffset;
	}
	else if( mnInputWidth != mnInputVideoW)
	{
		int nOffset = (mnInputWidth - mnInputVideoW) / 2;
		mvbInBuf.Buffer[0] += nOffset;
		mvbInBuf.Buffer[1] += nOffset / 2;
		mvbInBuf.Buffer[2] += nOffset / 2;
	}

	//SaveRawDataToFile(mvbInBuf , mnInputWidth , mnInputHeight , "d:\\Input.yuv");
	VO_U32 nRC = mpCCRRR->Process(&mvbInBuf , &mvbOutBuf , 0 , VO_TRUE);
	if(nRC == 0)
	{
		return RenderImage(mhBitmap);
	}
	
	return bRC;
}

bool	CBaseDraw::RenderImage(HBITMAP hBmp)
{
	if (hBmp == NULL || IsRectEmpty(&mrcWnd) || IsRectEmpty(&mrcDrawWnd))
		return false;
		
	HBITMAP hOldBmp = (HBITMAP)SelectObject (mhMemoryDC, hBmp);


	BOOL bValue = FALSE;
	if (mhWnd == NULL)
	{
		bValue = BitBlt(mhDC,	mrcDrawWnd.left ,mrcDrawWnd.top , mnOutWidth, mnOutHeight,
			   mhMemoryDC,	0, 0, SRCCOPY);

	}
	else
	{
		HDC hdc = GetDC (mhWnd);
		bValue = BitBlt(hdc , mrcDrawWnd.left , mrcDrawWnd.top , mnOutWidth, mnOutHeight,
				 mhMemoryDC,	0, 0, SRCCOPY);

		ReleaseDC (mhWnd, hdc);
	}

	// Put the old bitmap back into the device context so we don't leak
	SelectObject(mhMemoryDC, hOldBmp);

	return true;

}

bool	CBaseDraw::CreateBitmapInfo()
{
	int size = sizeof(BITMAPINFOHEADER);
	size += SIZE_MASKS; // for RGB bitMask;

	if (mpBitmapInfo == NULL)
		mpBitmapInfo = (BITMAPINFO *)new BYTE[size];
	memset (mpBitmapInfo, 0, size);

	mpBitmapInfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER) + SIZE_MASKS;

	mpBitmapInfo->bmiHeader.biWidth			= mnOutWidth;
	mpBitmapInfo->bmiHeader.biHeight		= -mnOutHeight;
	
	mpBitmapInfo->bmiHeader.biBitCount		= 16; 
	mpBitmapInfo->bmiHeader.biCompression	= BI_BITFIELDS;
	mpBitmapInfo->bmiHeader.biXPelsPerMeter	= 0;
	mpBitmapInfo->bmiHeader.biYPelsPerMeter	= 0;


	DWORD *	pBmiColors = (DWORD *)((LPBYTE)mpBitmapInfo + sizeof(BITMAPINFOHEADER));
	for (int i = 0; i < 3; i++)
		*(pBmiColors + i) = bits565[i];


	mpBitmapInfo->bmiHeader.biPlanes			= 1;
	mpBitmapInfo->bmiHeader.biSizeImage		= GetBitmapSize (&mpBitmapInfo->bmiHeader);

	return true;
}

bool	CBaseDraw::UpdateSize()
{
	int		nWndWidth = 0;
	int		nWndHeight = 0;
	
	SetRectEmpty(&mrcDrawWnd);

	GetWindowRect(mhWnd , &mrcWnd);

	nWndHeight = mrcWnd.bottom - mrcWnd.top;
	nWndWidth  = mrcWnd.right  - mrcWnd.left;

	if(nWndWidth == 0 || nWndHeight == 0)
		return false;

	mnScreenX =  GetSystemMetrics( SM_CXSCREEN );
	mnScreenY =  GetSystemMetrics( SM_CYSCREEN );

	mnInputVideoH = mnInputHeight;
	mnInputVideoW = mnInputWidth;

	if(mzmZoomMode == VO_ZM_PANSCAN)
	{
		if(mrtRotateType == VO_RT_90L || mrtRotateType == VO_RT_90R)
		{
			mnOutWidth = nWndWidth ;
			mnOutHeight = nWndHeight;

			
			swap(nWndHeight , nWndWidth);

			if(nWndWidth * mnInputHeight >= nWndHeight * mnInputWidth)
			{
				mnInputVideoW = mnInputWidth;
				mnInputVideoH = nWndHeight * mnInputWidth / nWndWidth;
			}
			else
			{
				mnInputVideoH = mnInputHeight;
				mnInputVideoW = nWndWidth * mnInputVideoH / nWndHeight ;
			}
		}
		else
		{
			mnOutWidth = nWndWidth;
			mnOutHeight = nWndHeight;

			if(nWndWidth * mnInputHeight >= nWndHeight * mnInputWidth)
			{
				mnInputVideoW = mnInputWidth;
				mnInputVideoH = nWndHeight * mnInputWidth / nWndWidth;
			}
			else
			{
				mnInputVideoH = mnInputHeight;
				mnInputVideoW = nWndWidth * mnInputVideoH / nWndHeight ;
			}
		}

		mnInputVideoH = mnInputVideoH / 4 * 4;
		mnInputVideoW = mnInputVideoW / 4 * 4;
	}
	else if(mzmZoomMode == VO_ZM_FITWINDOW)
	{
		if(mrtRotateType == VO_RT_90L || mrtRotateType == VO_RT_90R)
		{
			mnOutWidth  = mrcWnd.bottom - mrcWnd.top;
			mnOutHeight = mrcWnd.right - mrcWnd.left;
		}
		else
		{
			mnOutHeight  = mrcWnd.bottom - mrcWnd.top;
			mnOutWidth	= mrcWnd.right - mrcWnd.left;
		}
	}
	else if(mzmZoomMode == VO_ZM_ORIGINAL)
	{
		if(mrtRotateType == VO_RT_90L || mrtRotateType == VO_RT_90R)
		{
			mnOutWidth = mnInputWidth;
			mnOutHeight = mnInputHeight;
		}
		else
		{
			mnOutWidth = mnInputWidth;
			mnOutHeight = mnInputHeight;
		}
		
	}
	else //VO_ZM_LETTERBOX
	{
		if(mrtRotateType == VO_RT_90L || mrtRotateType == VO_RT_90R)
		{
			swap(nWndHeight , nWndWidth);

			if(nWndWidth * mnInputHeight <= nWndHeight * mnInputWidth)
			{
				mnOutHeight = nWndWidth;
				mnOutWidth  = nWndWidth * mnInputHeight / mnInputWidth;
			}
			else
			{
				mnOutWidth = nWndHeight;
				mnOutHeight = nWndHeight * mnInputWidth / mnInputHeight;
			}
		}
		else
		{
			if(nWndWidth * mnInputHeight <= nWndHeight * mnInputWidth)
			{
				mnOutWidth = nWndWidth;
				mnOutHeight = nWndWidth * mnInputHeight / mnInputWidth;
			}
			else
			{
				mnOutHeight = nWndHeight;
				mnOutWidth  = nWndHeight * mnInputWidth / mnInputHeight;
			}
		}
	}

	mnOutWidth = mnOutWidth / 4 * 4;
	mnOutHeight = mnOutHeight / 2 * 2;

	if(mpCCRRR == NULL)
	{
		return false;
	}

	VOLOGI("MAAA Set size input H : %d W:%d , output H : %d W:%d " , mnInputVideoH , mnInputVideoW , mnOutHeight , mnOutWidth);
	mpCCRRR->SetColorType(mInputColor , mOutputColor);
	if(mrtRotateType == VO_RT_90L || mrtRotateType == VO_RT_90R)
		mpCCRRR->SetCCRRSize((VO_U32 *)&mnInputVideoW , (VO_U32 *)&mnInputVideoH , (VO_U32 *)&mnOutHeight  , (VO_U32 *)&mnOutWidth , mrtRotateType);
	else
		mpCCRRR->SetCCRRSize((VO_U32 *)&mnInputVideoW , (VO_U32 *)&mnInputVideoH , (VO_U32 *)&mnOutWidth , (VO_U32 *)&mnOutHeight , mrtRotateType);

	if(mzmZoomMode != VO_ZM_PANSCAN)
	{
		mrcDrawWnd.left = ((mrcWnd.right - mrcWnd.left) - mnOutWidth) / 2;
		mrcDrawWnd.top  = ((mrcWnd.bottom - mrcWnd.top) - mnOutHeight) / 2;
		mrcDrawWnd.right = mrcDrawWnd.left + mnOutWidth;
		mrcDrawWnd.bottom = mrcDrawWnd.top + mnOutHeight;
	}
	else
	{
		mrcDrawWnd.right = nWndWidth;
		mrcDrawWnd.bottom = nWndHeight;
	}

	if (mhBitmap != NULL)
	{
		DeleteObject (mhBitmap);
		mhBitmap = NULL;
	}

	return true;
}

bool	CBaseDraw::LoadCCRRR()
{
	mpCCRRR = new CCCRRRFunc(VO_FALSE , NULL);
	if(mpCCRRR->LoadLib(g_hInst) == 0)
	{
		delete mpCCRRR;

		mpCCRRR = new CCCRRRFunc(VO_TRUE , NULL);
		if(mpCCRRR->LoadLib(g_hInst) == 0)
		{
			delete mpCCRRR;
			mpCCRRR = NULL;

			return false;
		}
	}

	return mpCCRRR->Init(mhWnd , &g_memOP , 0);
}

void	CBaseDraw::SaveRawDataToFile(VO_VIDEO_BUFFER videoBuf , int nVW , int nVH , char *fileName)
{
	FILE *hFile = fopen(fileName , "wb+");

	if(videoBuf.ColorType == VO_COLOR_YUV_PLANAR420)
	{
		int  nLoops = nVH;
		int  nCopySize = nVW;
		BYTE *p = videoBuf.Buffer[0];
		for (int i = 0 ; i < nLoops ; i++)
		{
			fwrite(p , nCopySize , 1 , hFile);
			p += videoBuf.Stride[0];
		}

		nLoops = nVH / 2;
		nCopySize = nVW / 2;
		p = videoBuf.Buffer[1];
		for (int i = 0 ; i < nLoops ; i++)
		{
			fwrite(p , nCopySize , 1 , hFile);
			p += videoBuf.Stride[1];
		}

		nLoops = nVH / 2;
		nCopySize = nVW / 2;
		p = videoBuf.Buffer[2];
		for (int i = 0 ; i < nLoops ; i++)
		{
			fwrite(p , nCopySize , 1 , hFile);
			p += videoBuf.Stride[2];
		}
	}
	else if(videoBuf.ColorType == VO_COLOR_RGB565_PACKED)
	{
		fwrite(videoBuf.Buffer[0] , videoBuf.Stride[0] * nVH , 1 , hFile);
	}

	fclose(hFile);
}

HRESULT 		CBaseDraw::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if(mpCCRRR != NULL)
	{
		return mpCCRRR->SetParam(nID , pValue);
	}

	return E_FAIL;
}

HRESULT			CBaseDraw::GetParam (VO_U32 nID, VO_PTR pValue)
{
	if(mpCCRRR != NULL)
	{
		return mpCCRRR->GetParam(nID , pValue);
	}

	return E_FAIL;
}