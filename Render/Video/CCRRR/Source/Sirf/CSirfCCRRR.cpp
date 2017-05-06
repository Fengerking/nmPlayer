#include "CSirfCCRRR.h"
#include "bhsurfmgr.h "

CSirfCCRRR::CSirfCCRRR(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseCCRRR (hInst, hView, pMemOP)
	, mhRender(NULL)
	, mhWnd(NULL)
	, mbFullScreen(VO_FALSE)
	, mbPrimary(VO_TRUE)
	, mbShowOverlay(VO_TRUE)
{
	strcpy ((VO_PCHAR) m_prop.szName, "SiRF");
	m_prop.nRender = 1;
	m_prop.nOverlay = 1;
	m_prop.nKeyColor = 0;
	m_prop.nRotate = VO_RT_DISABLE;
	m_prop.nOutBuffer = 0;
	m_prop.nFlag = 0;

	mhWnd = (HWND)hView;

	RETAILMSG( 1 , (L"Create Sirf CCRRR\r\n"));
}

CSirfCCRRR::~CSirfCCRRR(void)
{
	RETAILMSG( 1 , (L"Release Sirf CCRRR\r\n"));
	Uninit();
}

VO_U32 CSirfCCRRR::GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	if (nIndex == 0)
		*pColorType = VO_COLOR_YUV_PLANAR420;

	if (nIndex >= 0 && nIndex < 1)
		return VO_ERR_NONE;
	else
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CSirfCCRRR::GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	if (nIndex == 0)
		*pColorType = VO_COLOR_RGB565_PACKED;

	if (nIndex == 0)
		return VO_ERR_NONE;
	else
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CSirfCCRRR::SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor)
{
	CBaseCCRRR::SetColorType (nInputColor, nOutputColor);

	return VO_ERR_NONE;
}

VO_U32 CSirfCCRRR::SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate)
{
	if(*pInHeight == m_nInputWidth && *pInHeight == m_nInputHeight && *pOutWidth == m_nOutputWidth && *pOutHeight == m_nOutputHeight)
		return VO_ERR_NONE;

	VO_U32 nRC = CBaseCCRRR::SetCCRRSize(pInWidth , pInHeight , pOutWidth , pOutHeight , nRotate);

	VO_U32 nOutWidth = *pOutWidth & ~3;
	VO_U32 nOutHeight = *pOutHeight & ~1;

	mnInputWidth = *pInWidth & ~16;
	mnInputHeight = *pInHeight & ~16;

	if(mhRender == NULL)
	{
		nRC = Init();
		if(nRC != VO_ERR_NONE)
			return nRC;
	}

	if(mbPrimary)
		CVR_SetRenderLayer(mhRender, VR_LAYER_PRIMARY, 0);
	else
		CVR_SetRenderLayer(mhRender, VR_LAYER_OVERLAY, 1);

	VR_SRCRECT mrcSource;
	mrcSource.left = 0;
	mrcSource.top  = 0;
	mrcSource.right = mnInputWidth;
	mrcSource.bottom = mnInputHeight;

	mnYStride = mnInputWidth;
	mnUVStride = mnInputWidth / 2;

	VR_RESULT hr = CVR_SetVideoFormat(mhRender, VR_FORMAT_I420, mnInputWidth ,mnInputHeight ,&mrcSource,(int *)&mnYStride, (int *)&mnUVStride);
	if(VR_OK != hr)
		return VO_ERR_FAILED | VO_INDEX_SNK_CCRRR;

	mnScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	mnScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	VR_DSTRECT dr = {0, 0, 1.0, 1.0};
	VR_DISP_MODE_DESC mode;
	int nMinOutWidth = (mnInputWidth * nOutHeight / mnInputHeight) & ~0x7;
	if(nOutWidth < nMinOutWidth)
		mode.dmode = VR_DISP_FULL;
	else
	{
		int nMaxOutWidth = nMinOutWidth + 16;
		if(nOutWidth > nMaxOutWidth)
			mode.dmode = VR_DISP_FULL;
		else
			mode.dmode = VR_DISP_NORMAL;
	}
	mode.smode = VR_SCR_NORMAL;


	RECT rcWnd;
	::GetWindowRect(mhWnd, &rcWnd);
	if(rcWnd.right-rcWnd.left >= mnScreenWidth && rcWnd.bottom - rcWnd.top >= mnScreenHeight)
	{
		mode.rmode = VR_REN_FULL;
		hr = CVR_SetDisplayMode(mhRender, mode);
		if(VR_OK != hr)
			return VO_ERR_FAILED | VO_INDEX_SNK_CCRRR;
	}
	else
	{
#ifdef _SIRF_FLIP_MODE
		mode.rmode = VR_REN_FULL;
#else	//_SIRF_FLIP_MODE
		mode.rmode = VR_REN_NORMAL;
#endif	//_SIRF_FLIP_MODE		
		hr = CVR_SetDisplayMode(mhRender, mode);
		if(VR_OK != hr)
			return VO_ERR_FAILED | VO_INDEX_SNK_CCRRR;

		int nLeft = (rcWnd.right - rcWnd.left - nOutWidth) / 2 + rcWnd.left;
		int nTop = (rcWnd.bottom - rcWnd.top - nOutHeight) / 2 + rcWnd.top;
		::SetWindowPos(mhWnd, NULL, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, SWP_NOMOVE);

		dr.left = float(nLeft) / mnScreenWidth;
		dr.top = float(nTop) / mnScreenHeight;
		dr.right = float(nLeft + nOutWidth) / mnScreenWidth;
		dr.bottom = float(nTop + nOutHeight) / mnScreenHeight;
	}

	//set dest rect!!
	hr = CVR_SetDstRect(mhRender, dr);
	if(VR_OK != hr)
		return VO_ERR_FAILED | VO_INDEX_SNK_CCRRR;

	if(m_pSurf != NULL)
	{
		CVR_DestroySurface(mhRender , m_pSurf);
		m_pSurf = NULL;
	}


	return VO_ERR_NONE;
}

VO_U32 CSirfCCRRR::Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	VR_RESULT hr = VR_OK;
	if(0 == pVideoBuffer->Stride[0])
	{
		if(!pVideoBuffer->Buffer[0])
			return  VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;;

		DWORD dwPtr;
		memcpy(&dwPtr, pVideoBuffer->Buffer[0], 4);
		if(dwPtr)
		{
			if(mbFullScreen)
			{
				hr = CVR_WaitComplete(mhRender);
#ifdef _SIRF_FLIP_MODE
				BSM_OutputSurf((SXMemSurfDesc*)dwPtr, SX_NOWAIT_VSYNC);
#else	//_SIRF_FLIP_MODE
				BSM_OutputSurf((SXMemSurfDesc*)dwPtr, SX_ASYNC_WAITVSYNC);
#endif	//_SIRF_FLIP_MODE
			}
			else
				BSM_OutputSurf((SXMemSurfDesc*)dwPtr, SX_NOWAIT_VSYNC);
		}
	}
	else	//normal WYUV data
	{
		if(!m_pSurf)
		{
			hr = CVR_CreateSurface(mhRender, &m_pSurf, 1);
			if(VR_OK != hr || !m_pSurf)
				return VO_ERR_FAILED | VO_INDEX_SNK_CCRRR;
		}

		hr = CVR_LockSurface(mhRender, m_pSurf);
		if(VR_OK != hr)
			return VO_ERR_FAILED | VO_INDEX_SNK_CCRRR;

		//copy data into m_pSurf
		VO_S32 i = 0;
		VO_S32 nHeight = mnInputHeight;

		VO_BYTE * pDstBuf = m_pSurf->pFrameBuf;
		VO_BYTE * pSrcBuf = pVideoBuffer->Buffer[0];

		if(pDstBuf == NULL || pSrcBuf == NULL)
			return VO_ERR_FAILED | VO_INDEX_SNK_CCRRR;

		for(i = 0; i < nHeight; i++)
		{
			memcpy(pDstBuf, pSrcBuf, mnInputWidth);
			pDstBuf += mnYStride;
			pSrcBuf +=pVideoBuffer->Stride[0];
		}

		nHeight = mnInputHeight / 2;
		VO_S32 nCopySize =  mnInputWidth / 2;

		pDstBuf =  m_pSurf->pFrameBuf + mnInputHeight * mnYStride;
		pSrcBuf =pVideoBuffer->Buffer[1];
		for(i = 0; i < nHeight; i++)
		{
			memcpy(pDstBuf , pSrcBuf,nCopySize);
			pDstBuf += mnUVStride;
			pSrcBuf +=pVideoBuffer->Stride[1];
		}

		
		pDstBuf =  m_pSurf->pFrameBuf + mnInputHeight * mnYStride * 5 / 4;
		pSrcBuf =pVideoBuffer->Buffer[2];
		for(i = 0; i < nHeight; i++)
		{
			memcpy(pDstBuf , pSrcBuf, nCopySize);
			pDstBuf += mnUVStride;
			pSrcBuf +=pVideoBuffer->Stride[2];
		}

		hr = CVR_UnlockSurface(mhRender, m_pSurf);
		if(mbFullScreen)
		{
			hr = CVR_WaitComplete(mhRender);
#ifdef _SIRF_FLIP_MODE
			hr = CVR_Render(mhRender, m_pSurf, VR_NOWAIT_VSYNC);
#else	//_SIRF_FLIP_MODE
			hr = CVR_Render(mhRender, m_pSurf, VR_ASYNC_WAITVSYNC);
#endif	//_SIRF_FLIP_MODE
		}
		else
			hr = CVR_Render(mhRender, m_pSurf, VR_NOWAIT_VSYNC);

		if(VR_OK != hr)
			return VO_ERR_FAILED | VO_INDEX_SNK_CCRRR;
	}


	return VO_ERR_NONE;
}


VO_U32 CSirfCCRRR::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if(VO_CCRRR_PMID_ShowOverlay == nID)
	{
		ShowOverlay(*(VO_BOOL *)pValue);

		return VO_ERR_NONE;
	}

	return CBaseCCRRR::SetParam(nID , pValue);
}

VO_U32 CSirfCCRRR::GetParam (VO_U32 nID, VO_PTR pValue)
{
	return CBaseCCRRR::GetParam(nID , pValue);
}

VO_U32	CSirfCCRRR::Init()
{
	CVR_CreateVideoRender(&mhRender, mhWnd);
	BSM_Init(mhRender);
	if(!mhRender)
		return VO_ERR_FAILED | VO_INDEX_SNK_CCRRR;

	mnYStride = 0;
	mnUVStride = 0;

	return VO_ERR_NONE;
}

VO_U32	CSirfCCRRR::Uninit()
{
	if(mhRender)
	{
		BSM_DeInit();
		CVR_DeleteVideoRender(mhRender);
		mhRender = NULL;
	}

	return VO_ERR_NONE;
}

VO_U32	CSirfCCRRR::ShowOverlay(VO_BOOL bShow)
{
	if(mhRender && mbPrimary == VO_FALSE)
	{
		if(bShow)
		{
			CVR_SetRenderLayer(mhRender, VR_LAYER_OVERLAY, 1);
		}
		else
		{
			CVR_SetRenderLayer(mhRender, VR_LAYER_PRIMARY, 1);
		}
	}

	return VO_ERR_NONE;
}