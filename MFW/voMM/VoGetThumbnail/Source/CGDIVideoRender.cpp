	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "cmnMemory.h"
#include "CGDIVideoRender.h"

#include "voLogoData.h"

#include "voLog.h"

const DWORD bits565[3] = {0X0000F800, 0X000007E0, 0X0000001F,};

CGDIVideoRender::CGDIVideoRender(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseVideoRender (hInst, hView, pMemOP)
	, m_hWinDC(NULL)
	, m_hMemDC(NULL)
	, m_pBitmapBuffer (NULL)
	, m_pBmpInfo (NULL)
	, m_hBitmap (NULL)
	, m_hOldBmp (NULL)
	, m_nGDIWidth (0)
	, m_nGDIHeight (0)
	, m_nPixelBits (32)
	, m_bDeleteBmpOnQuit(true)
{
	VOLOGF ();
	m_bSoftForce = VO_TRUE;

	memset (&m_outBuffer, 0, sizeof (VO_VIDEO_BUFFER));
	m_outBuffer.ColorType = VO_COLOR_RGB565_PACKED;
	if (m_nPixelBits == 24)
		m_outBuffer.ColorType = VO_COLOR_RGB888_PACKED;
	else if (m_nPixelBits == 32)
		m_outBuffer.ColorType = VO_COLOR_RGB32_PACKED;

	if(m_hView)
	{
		GetClientRect ((HWND) m_hView, (LPRECT)&m_rcDisplay);
		m_hWinDC = GetDC ((HWND) m_hView);
		m_hMemDC = ::CreateCompatibleDC (m_hWinDC);
	}
	else
	{
		HDC hdc = GetDC(NULL);
		m_hMemDC = ::CreateCompatibleDC (hdc);
		::ReleaseDC(NULL,hdc);
	}
}

VO_U32 CGDIVideoRender::GetBitmap( HBITMAP* hOutBitmap)
{
	*hOutBitmap = m_hBitmap;
	m_bDeleteBmpOnQuit = false;
	return 0;
}


CGDIVideoRender::~CGDIVideoRender ()
{
	VOLOGF ();

	if (m_hOldBmp != NULL)
		SelectObject (m_hMemDC, m_hOldBmp);

	if (m_pBmpInfo != NULL)
		delete []m_pBmpInfo;

	if (m_hBitmap && m_bDeleteBmpOnQuit)
		DeleteObject (m_hBitmap);

	if (m_hWinDC != NULL)
	{
		ReleaseDC ((HWND)m_hView, m_hWinDC);
	}
	if(m_hMemDC)
		DeleteDC (m_hMemDC);
}

VO_U32 CGDIVideoRender::Start (void)
{
	m_bSoftForce = VO_FALSE;
	ShowOverlay(VO_TRUE);

	return CBaseVideoRender::Start();
}

VO_U32 	CGDIVideoRender::Pause (void)
{
	m_bSoftForce = VO_TRUE;
	ShowOverlay(VO_FALSE);

	return CBaseVideoRender::Pause();
}

VO_U32 	CGDIVideoRender::Stop (void)
{
	m_bSoftForce = VO_TRUE;
	ShowOverlay(VO_FALSE);

	return CBaseVideoRender::Stop();
}
bool CGDIVideoRender::ConvertData (VO_VIDEO_BUFFER * pInBuffer, VO_VIDEO_BUFFER * pOutBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	VOLOGR ();
	return __super::ConvertData ( pInBuffer,  pOutBuffer, nStart, bWait);

	int nYOffset = 0;
	int nUVOffset = 0;

	m_nShowTop = 0;
	m_nShowLeft = 0;
	if (pInBuffer->ColorType == VO_COLOR_YUV_PLANAR420)
	{
		nYOffset = m_nShowTop * pInBuffer->Stride[0] + m_nShowLeft;
		nUVOffset = (m_nShowTop / 2) * pInBuffer->Stride[1] + m_nShowLeft / 2;

		m_inData.Buffer[0] = pInBuffer->Buffer[0]  + nYOffset;
		m_inData.Buffer[1] = pInBuffer->Buffer[1]  + nUVOffset;
		m_inData.Buffer[2] = pInBuffer->Buffer[2]  + nUVOffset;

		m_inData.Stride[0] = pInBuffer->Stride[0];
		m_inData.Stride[1] = pInBuffer->Stride[1];
		m_inData.Stride[2] = pInBuffer->Stride[2];

		m_inData.ColorType = pInBuffer->ColorType;
	}
	else if (pInBuffer->ColorType == VO_COLOR_YUV_420_PACK_2)
	{
		if (m_pUVData == NULL)
			m_pUVData = new VO_BYTE[m_nYUVWidth * m_nYUVHeight / 2];

		VO_PBYTE pUV = pInBuffer->Buffer[1];
		VO_PBYTE pU = m_pUVData;
		VO_PBYTE pV = pU + m_nYUVWidth * m_nYUVHeight / 4;

		for (int i = 0; i < m_nVideoHeight / 2; i++)
		{
			pUV = pInBuffer->Buffer[1] + pInBuffer->Stride[1] * i;
			pU = m_pUVData + (m_nYUVWidth / 2) * i;
			pV = pU + m_nYUVWidth * m_nYUVHeight / 4;

			for (int w = 0; w < m_nVideoWidth / 2; w++)
			{
				*pV++ = *pUV++;
				*pU++ = *pUV++;
			}
		}

		nYOffset = m_nShowTop * pInBuffer->Stride[0] + m_nShowLeft;
		nUVOffset = (m_nShowTop / 2) * pInBuffer->Stride[1] / 2 + m_nShowLeft / 2;

		m_inData.Buffer[0] = pInBuffer->Buffer[0]  + nYOffset;
		m_inData.Buffer[1] = m_pUVData + nUVOffset;
		m_inData.Buffer[2] = m_pUVData + m_nYUVWidth * m_nYUVHeight / 4  + nUVOffset;

		m_inData.Stride[0] = pInBuffer->Stride[0];
		m_inData.Stride[1] = m_nYUVWidth / 2;
		m_inData.Stride[2] = m_nYUVWidth / 2;
		m_inData.ColorType = pInBuffer->ColorType;
		m_inData.Time = 0;
	}

	VO_U32 n = 0;
	if (m_pSoftCCRRR != NULL)
	{
		n = m_pSoftCCRRR->Process (&m_inData, pOutBuffer, nStart, bWait);
	}
	else if (m_bSoftForce && m_pCCRRR != NULL)
	{
		n = m_pCCRRR->Process (&m_inData, pOutBuffer, nStart, bWait);
	}

	return true;
}

VO_U32 CGDIVideoRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	VOLOGR ();

	voCAutoLock lock (&m_csDraw);

	VO_U32 nRC = 0;
	if (m_hBitmap == NULL)
	{
		if (m_pBmpInfo == NULL)
			CreateBitmapInfo ();

		m_hBitmap = CreateDIBSection(m_hMemDC , (BITMAPINFO *)m_pBmpInfo , DIB_RGB_COLORS , (void **)&m_pBitmapBuffer , NULL , 0);
		m_hOldBmp = (HBITMAP)SelectObject (m_hMemDC, m_hBitmap);
	}
	if (m_hBitmap == NULL || m_pBitmapBuffer == NULL)
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

	m_nGDIWidth = ((BITMAPINFO *)m_pBmpInfo)->bmiHeader.biWidth;
	m_nGDIHeight = abs(((BITMAPINFO *)m_pBmpInfo)->bmiHeader.biHeight);//m_rcDisplay.top - m_rcDisplay.bottom
	m_nDrawWidth = m_nGDIWidth;
	m_nDrawHeight = m_nGDIHeight;
	m_outBuffer.Buffer[0] = m_pBitmapBuffer;
	m_outBuffer.Stride[0] = m_nGDIWidth * m_nPixelBits / 8;

	if (m_pCCRRR == NULL)
	{
		CBaseVideoRender::CreateCCRRR ();
	}
	if (m_pCCRRR != NULL)
		m_pCCRRR->SetColorType (VO_COLOR_YUV_PLANAR420, m_outBuffer.ColorType);
	if (m_pSoftCCRRR != NULL)
		m_pSoftCCRRR->SetColorType (VO_COLOR_YUV_PLANAR420, m_outBuffer.ColorType);

	{
		if (m_pCCRRR != NULL)
			m_pCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nDrawWidth, &m_nDrawHeight, VO_RT_DISABLE);
		if (m_pSoftCCRRR != NULL)
			m_pSoftCCRRR->SetCCRRSize (&m_nShowWidth, &m_nShowHeight, &m_nDrawWidth, &m_nDrawHeight, VO_RT_DISABLE);
	}

	if (!ConvertData (pVideoBuffer, &m_outBuffer, nStart, bWait))
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_VIDEO;

	//if (m_hBitmap != NULL)
	//{
	//	PBYTE* pData = NULL;
	//	HDC hdc = GetDC(NULL);
	//	HDC hMemDC = ::CreateCompatibleDC (hdc);
	//	HBITMAP hBitmap = CreateDIBSection(hdc , (BITMAPINFO *)m_pBmpInfo , DIB_RGB_COLORS , (void **)&pData , NULL , 0);
	//	HBITMAP hOld = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	//	BitBlt(hMemDC, ((this->m_rcDisplay.right- m_rcDisplay.left) - m_nGDIWidth)/2,
	//		((this->m_rcDisplay.bottom- m_rcDisplay.top) - m_nGDIHeight), m_nGDIWidth, m_nGDIHeight, m_hMemDC, 0, 0, SRCCOPY);
	//	SelectObject(hMemDC, hOld);
	//	::DeleteDC(hMemDC);
	//	if(hBitmap)
	//	{
	//		::DeleteObject(m_hBitmap);
	//		m_hBitmap = hBitmap;
	//	}
	//	::ReleaseDC(NULL,hdc);
	//}

	return VO_ERR_NONE;
}

VO_U32 CGDIVideoRender::Redraw (void)
{
	VOLOGF ();

	voCAutoLock lock (&m_csDraw);

	if (m_hBitmap != NULL)
	{
		HBITMAP hOldBmp = (HBITMAP)SelectObject (m_hMemDC, m_hBitmap);
		BitBlt(m_hWinDC, m_nDrawLeft, m_nDrawTop, m_nGDIWidth, m_nGDIHeight, m_hMemDC, 0, 0, SRCCOPY);
		SelectObject(m_hMemDC, hOldBmp);
	}

	return VO_ERR_NONE;
}

VO_U32 CGDIVideoRender::UpdateSize (void)
{
	VOLOGF ();
	return 0;

#ifdef _WIN32
#ifndef _WIN32_WCE
	m_nScreenWidth = m_rcDisplay.right - m_rcDisplay.left;
	m_nScreenHeight = m_rcDisplay.bottom - m_rcDisplay.top;
#endif // _WIN32_WCE
#endif // _WIN32

	CBaseVideoRender::UpdateSize ();

	voCAutoLock lock (&m_csDraw);
	if (!m_bRotate)
	{
		m_nGDIWidth = m_nDrawWidth;
		m_nGDIHeight = m_nDrawHeight;
	}
	else
	{
		m_nGDIWidth = m_nDrawHeight;
		m_nGDIHeight = m_nDrawWidth;
	}

	if (m_hBitmap != NULL)
	{
		SelectObject (m_hMemDC, m_hOldBmp);
		DeleteObject (m_hBitmap);

		m_hOldBmp = NULL;
		m_hBitmap = NULL;
	}

	if (m_pBmpInfo != NULL)
	{
		delete []m_pBmpInfo;
		m_pBmpInfo = NULL;
	}

	if (m_pCCRRR != NULL)
		m_pCCRRR->SetColorType (VO_COLOR_YUV_PLANAR420, m_outBuffer.ColorType);
	if (m_pSoftCCRRR != NULL)
		m_pSoftCCRRR->SetColorType (VO_COLOR_YUV_PLANAR420, m_outBuffer.ColorType);

	CreateBitmapInfo ();

	return 0;
}

bool CGDIVideoRender::CreateBitmapInfo (int nW , int nH )
{
	VOLOGF ();

	int nBmpSize = sizeof(BITMAPINFOHEADER);
	if (m_nPixelBits == 16)
		nBmpSize += SIZE_MASKS; // for RGB bitMask;

	if (m_pBmpInfo == NULL)
		m_pBmpInfo = new BYTE[nBmpSize];
	memset (m_pBmpInfo, 0, nBmpSize);

	BITMAPINFO * pBmpInfo = (BITMAPINFO *)m_pBmpInfo;

	pBmpInfo->bmiHeader.biSize			= nBmpSize;

	pBmpInfo->bmiHeader.biWidth			= this->m_rcDisplay.right - m_rcDisplay.left;//m_nGDIWidth;//m_rcDraw.right - m_rcDraw.left;
	pBmpInfo->bmiHeader.biHeight		= m_rcDisplay.top - m_rcDisplay.bottom;//-m_nGDIHeight;//-(m_rcDraw.bottom - m_rcDraw.top);

	pBmpInfo->bmiHeader.biBitCount		= m_nPixelBits;
	if (m_nPixelBits == 16)
		pBmpInfo->bmiHeader.biCompression	= BI_BITFIELDS;
	else
		pBmpInfo->bmiHeader.biCompression	= BI_RGB;
	pBmpInfo->bmiHeader.biXPelsPerMeter	= 0;
	pBmpInfo->bmiHeader.biYPelsPerMeter	= 0;

	if (m_nPixelBits == 16)
	{
		DWORD *	pBmiColors = (DWORD *)((LPBYTE)pBmpInfo + sizeof(BITMAPINFOHEADER));
		for (int i = 0; i < 3; i++)
    		*(pBmiColors + i) = bits565[i];
	}

	pBmpInfo->bmiHeader.biPlanes			= 1;

	int nStride = ((pBmpInfo->bmiHeader.biWidth * pBmpInfo->bmiHeader.biBitCount / 8) + 3) & ~3;

	pBmpInfo->bmiHeader.biSizeImage		= nStride * abs(pBmpInfo->bmiHeader.biHeight);

	return true;
}

VO_U32 CGDIVideoRender::ShowOverlay(VO_BOOL bShow)
{
	if(m_pCCRRR)
	{
//		return m_pCCRRR->SetParam(VO_CCRRR_PMID_ShowOverlay , (VO_PTR)&bShow);
	}

	return VO_ERR_FAILED;
}

VO_U32			voVRMemInit (VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem);
VO_U32			voVRMemGetBuf (VO_S32 uID, VO_S32 nIndex);
VO_U32			voVRMemUninit (VO_S32 uID);

VO_MEM_VIDEO_OPERATOR	g_vmVROP;
VO_MEM_VIDEO_INFO		g_vmVRInfo;

VO_U32					g_yOffset;

VO_U32 CGDIVideoRender::GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	return CBaseVideoRender::GetVideoMemOP (ppVideoMemOP);

	memset (&g_vmVRInfo, 0, sizeof (VO_MEM_VIDEO_INFO));

	g_vmVROP.Init = voVRMemInit;
	g_vmVROP.GetBufByIndex = voVRMemGetBuf;
	g_vmVROP.Uninit = voVRMemUninit;

	*ppVideoMemOP = &g_vmVROP;
	
	return VO_ERR_NONE;
}

VO_U32 voVRMemInit (VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem)
{
	memcpy (&g_vmVRInfo, pVideoMem, sizeof (VO_MEM_VIDEO_INFO));

	g_vmVRInfo.PBuffer = NULL;

	VO_VIDEO_BUFFER * pBuffers =  new VO_VIDEO_BUFFER[g_vmVRInfo.FrameCount];

	for (int i = 0; i < g_vmVRInfo.FrameCount; i++)
	{
		int yStride = g_vmVRInfo.Stride;
		g_yOffset = 0;

		pBuffers[i].Buffer[0] = new BYTE[yStride * (g_vmVRInfo.Height)];
		pBuffers[i].Buffer[0] += g_yOffset;

		pBuffers[i].Buffer[1] = new BYTE[g_vmVRInfo.Stride * g_vmVRInfo.Height / 4];
		pBuffers[i].Buffer[2] = new BYTE[g_vmVRInfo.Stride * g_vmVRInfo.Height / 4];

		pBuffers[i].Stride[0] = yStride;
		pBuffers[i].Stride[1] = g_vmVRInfo.Stride/2;
		pBuffers[i].Stride[2] = g_vmVRInfo.Stride/2;
	}

	g_vmVRInfo.VBuffer = pBuffers;
	pVideoMem->VBuffer = pBuffers;

	return 0;
}

VO_U32 voVRMemGetBuf (VO_S32 uID, VO_S32 nIndex)
{
	if (nIndex < 0 || nIndex >= g_vmVRInfo.FrameCount)
		return VO_ERR_FAILED;

	return VO_ERR_NONE;
}

VO_U32 voVRMemUninit (VO_S32 uID)
{
	if (g_vmVRInfo.VBuffer == NULL)
		return 0;

	for (int i = 0; i < g_vmVRInfo.FrameCount; i++)
	{
		delete [](g_vmVRInfo.VBuffer[i].Buffer[0] - g_yOffset); 
		delete []g_vmVRInfo.VBuffer[i].Buffer[1];
		delete []g_vmVRInfo.VBuffer[i].Buffer[2];
	}

	delete []g_vmVRInfo.VBuffer;
	g_vmVRInfo.VBuffer == NULL;

	return 0;
}
