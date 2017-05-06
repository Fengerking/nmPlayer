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
#include "windows.h"
//#include "Pkfuncs.h"

#include <string.h>
#include "CQ7200CCRRR.h"

#include "CDDrawDisplay.h"
#include "pmem_ids.h"
#include "pmem.h"

#include "..\myceddk.h"
#include "voPackUV.h"
#include "voLogoData.h"

#define CACHE_SYNC_DISCARD      0x001   /* write back & discard all cached data */
#define CACHE_SYNC_INSTRUCTIONS 0x002   /* discard all cached instructions */
#define CACHE_SYNC_WRITEBACK    0x004   /* write back but don't discard data cache*/
#define CACHE_SYNC_FLUSH_I_TLB  0x008   /* flush I-TLB */
#define CACHE_SYNC_FLUSH_D_TLB  0x010   /* flush D-TLB */
#define CACHE_SYNC_FLUSH_TLB    (CACHE_SYNC_FLUSH_I_TLB|CACHE_SYNC_FLUSH_D_TLB)    /* flush all TLB */
#define CACHE_SYNC_L2_WRITEBACK 0x020   /* write-back L2 Cache */
#define CACHE_SYNC_L2_DISCARD   0x040   /* discard L2 Cache */

DWORD CACHE_MASK = 0x0c;
DWORD CACHE_FLAG = 0x0c;

VO_MEM_VIDEO_INFO CQ7200CCRRR::g_vmVRInfo;

CQ7200CCRRR::CQ7200CCRRR(VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseCCRRR (hInst, hView, pMemOP)
	, mnLeft(0)
	, mnTop(0)
	, mbSameCoordinate(true)
	, mpPicBufVir (NULL)
	, mpPicBufPhy (NULL)
	, mpPicBufVirUV (NULL)
	, mpPicBufPhyUV (NULL)
	, mnRatoteAngle (0)
	, m_nFrameSize (0)
	, m_pVideoBuffer (NULL)
	, mhThread(NULL)
	, mbStop(false)
	, mdwThreadID(0)
	, m_nLogTime (0)
	, m_bShareVideoMem (false)
	, mnGDIRotateAngle(0)
{
	strcpy ((VO_PCHAR) m_prop.szName, "Q7200");

	m_prop.nRender = 1;
	m_prop.nOverlay = 1;
	m_prop.nKeyColor = 0;
	m_prop.nRotate = VO_RT_90L | VO_RT_90R;
	m_prop.nOutBuffer = 0;
	m_prop.nFlag = 0;

	OpenCEDDK();

	mpDisplay = new  CDDrawDisplay();
	mpConvert = new CQ7200Convert();

	mhWnd = (HWND)m_hView;

	mnScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	mnScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	ZeroMemory(&mInBmpInfo , sizeof(BmpInfo));
	ZeroMemory(&mOutBmpInfo , sizeof(BmpInfo));

	meFinishDraw = CreateEvent(NULL , TRUE , FALSE , NULL);
	meStartDraw  = CreateEvent(NULL , TRUE , FALSE, NULL);

	g_vmVROP.Init = voVRMemInit;
	g_vmVROP.GetBufByIndex = voVRMemGetBuf;
	g_vmVROP.Uninit = voVRMemUninit;
	memset (&g_vmVRInfo, 0, sizeof (VO_MEM_VIDEO_INFO));

	InitializeCriticalSection(&mcsLock);
}

CQ7200CCRRR::~CQ7200CCRRR ()
{
	DeleteCriticalSection(&mcsLock);

	mbStop = true;
	if(meStartDraw != NULL)
	{
		SetEvent(meStartDraw);
		CloseHandle(meStartDraw);
		meStartDraw = NULL;
	}

	if(meFinishDraw != NULL)
	{
		SetEvent(meFinishDraw);
		CloseHandle(meFinishDraw);
		meFinishDraw = NULL;
	}

	if(mhThread != NULL)
	{
		Sleep(15);
		if(mhThread != NULL)
		{
			ExitThread(mdwThreadID);
			mhThread = NULL;
			mdwThreadID = 0;
		}
	}

	if(mpConvert != NULL)
	{
		delete mpConvert;
		mpConvert = NULL;
	}

	if(mpDisplay != NULL)
	{
		delete mpDisplay;
		mpDisplay = NULL;
	}

	if (mpPicBufVir != NULL)
	{
		FreePhysMem(mpPicBufVir);
		mpPicBufVir = NULL;
	}
	if (mpPicBufVirUV != NULL)
	{
		FreePhysMem(mpPicBufVirUV);
		mpPicBufVirUV = NULL;
	}

	CloseCEDDK();
}

VO_U32 CQ7200CCRRR::GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	if (nIndex == 0)
		*pColorType = VO_COLOR_YUV_PLANAR420;

	if (nIndex >= 0 && nIndex < 1)
		return VO_ERR_NONE;
	else
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CQ7200CCRRR::GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	if (nIndex == 0)
		*pColorType = VO_COLOR_RGB565_PACKED;

	if (nIndex == 0)
		return VO_ERR_NONE;
	else
		return VO_ERR_NOT_IMPLEMENT | VO_INDEX_SNK_CCRRR;
}

VO_U32 CQ7200CCRRR::SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor)
{
	CBaseCCRRR::SetColorType (nInputColor, nOutputColor);

	return VO_ERR_NONE;
}

VO_U32 CQ7200CCRRR::SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate)
{
	if (g_vmVRInfo.FrameCount > 0)
		m_bShareVideoMem = true;

	RECT rcWnd;
	GetWindowRect(mhWnd , &rcWnd);

	if(m_nInputWidth == *pInWidth && m_nInputHeight == *pInHeight && m_nOutputWidth == *pOutWidth && m_nOutputHeight == *pOutHeight && m_nRotate == nRotate )
	{
		if(EqualRect(&rcWnd , &mWndRect))
			return VO_ERR_NONE;
		CopyRect(&mWndRect , &rcWnd);
	}

	m_nInputWidth = *pInWidth;
	m_nInputHeight = *pInHeight;
	m_nOutputWidth = *pOutWidth;
	m_nOutputHeight = *pOutHeight;
	m_nRotate = nRotate;

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	if(nScreenWidth != mnScreenWidth)
	{
		mnScreenWidth =nScreenWidth;
		mnScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		if(mpDisplay != NULL)
			mpDisplay->CreatePrimarySurface();
	}

	mbSameCoordinate = !mpDisplay->IsRotateSurface();
	mnGDIRotateAngle = GetRegValue(TEXT("System\\GDI\\Rotation") , TEXT("Angle") , 0);

	if (m_nFrameSize != m_nInputWidth * m_nInputHeight)
	{
		if (mpPicBufVir != NULL)
			FreePhysMem(mpPicBufVir);
		if (mpPicBufVirUV != NULL)
			FreePhysMem(mpPicBufVirUV);

		if (m_bShareVideoMem)
		{
			m_nFrameSize = g_vmVRInfo.Stride * g_vmVRInfo.Height;;
			mpPicBufVirUV = (BYTE*) AllocPhysMem(m_nFrameSize / 2, PAGE_READWRITE, 0, 0, (PULONG)&mpPicBufPhyUV);
			VirtualSetAttributes(mpPicBufVirUV, m_nFrameSize / 2, CACHE_FLAG, CACHE_MASK, NULL);

			if (mpPicBufVirUV != NULL)
				memset (mpPicBufVirUV, 127, m_nFrameSize / 2);
		}
		else
		{
			m_nFrameSize = m_nInputWidth * m_nInputHeight;
			mpPicBufVir = (BYTE*) AllocPhysMem(m_nFrameSize, PAGE_READWRITE, 0, 0, (PULONG)&mpPicBufPhy);
			VirtualSetAttributes(mpPicBufVir, m_nFrameSize, CACHE_FLAG, CACHE_MASK, NULL);
			mpPicBufVirUV = (BYTE*) AllocPhysMem(m_nFrameSize / 2, PAGE_READWRITE, 0, 0, (PULONG)&mpPicBufPhyUV);
			VirtualSetAttributes(mpPicBufVirUV, m_nFrameSize / 2, CACHE_FLAG, CACHE_MASK, NULL);

			if (mpPicBufVir != NULL)
				memset (mpPicBufVir, 0, m_nFrameSize);
			if (mpPicBufVirUV != NULL)
				memset (mpPicBufVirUV, 127, m_nFrameSize / 2);
		}
	}

	mInBmpInfo.bufPoint.pBuf1 = mpPicBufPhy;
	mInBmpInfo.bufPoint.pBuf2 = mpPicBufPhyUV; 
	mInBmpInfo.bufPoint.pBuf3 = NULL;

	if (m_bShareVideoMem)
	{
		mInBmpInfo.nBmpWidth = g_vmVRInfo.Stride;
		mInBmpInfo.nBmpHeight = m_nInputHeight;
	}
	else
	{
		mInBmpInfo.nBmpWidth = m_nInputWidth;
		mInBmpInfo.nBmpHeight = m_nInputHeight;
	}

	mInBmpInfo.nLeft = 0;
	mInBmpInfo.nTop = 0;
	mInBmpInfo.nPicWidth  = m_nInputWidth ;
	mInBmpInfo.nPicHeight = m_nInputHeight;

	if(nRotate == VO_RT_90L || nRotate == VO_RT_90R)
	{
		mnTop = (rcWnd.bottom + rcWnd.top - m_nOutputWidth) / 2;
		mnLeft = (rcWnd.left + rcWnd.right - m_nOutputHeight) / 2;
	}
	else
	{
		mnTop = (rcWnd.bottom + rcWnd.top - m_nOutputHeight) / 2;
		mnLeft = (rcWnd.left + rcWnd.right - m_nOutputWidth) / 2;
	}
	mnTop = (mnTop + 1) / 2 * 2;
	mnLeft = (mnLeft + 1) / 2 * 2;

	mOutBmpInfo.bufPoint.pBuf2 = NULL;
	mOutBmpInfo.bufPoint.pBuf3 = NULL;
	mOutBmpInfo.nBmpHeight = mnScreenHeight;
	mOutBmpInfo.nBmpWidth  = mnScreenWidth;

	if(!mbSameCoordinate)
	{
		mOutBmpInfo.nPicHeight = m_nOutputWidth;
		mOutBmpInfo.nPicWidth  = m_nOutputHeight;


		mOutBmpInfo.nBmpHeight = mnScreenWidth;
		mOutBmpInfo.nBmpWidth  = mnScreenHeight;

		//#ifdef _Surface_Origin_TL
		if(mnGDIRotateAngle == 90)
		{
			mnRatoteAngle = 270; 
			mOutBmpInfo.nLeft		=  mnTop;
			mOutBmpInfo.nTop		= mnScreenWidth - m_nOutputWidth - mnLeft;
		}
		else if(mnGDIRotateAngle == 270)
		{
			mnRatoteAngle = 90; 
			mOutBmpInfo.nLeft		=  mnScreenHeight - m_nOutputHeight - mnTop;
			mOutBmpInfo.nTop		=  mnLeft - 1;
		}
		else
		{
			mnRatoteAngle = 270; 
			mOutBmpInfo.nLeft		=  mnTop;
			mOutBmpInfo.nTop		= mnScreenWidth - m_nOutputWidth - mnLeft;
		}

	}
	else
	{
		if(nRotate == VO_RT_90L)
		{
			mnRatoteAngle			= 90;
			mOutBmpInfo.nLeft		= mnLeft;
			mOutBmpInfo.nTop		= mnTop;
			mOutBmpInfo.nPicWidth	= m_nOutputHeight;
			mOutBmpInfo.nPicHeight	= m_nOutputWidth;
		}
		else if(nRotate == VO_RT_90R)
		{
			mnRatoteAngle = 270;
			mOutBmpInfo.nLeft		=  mnScreenHeight - m_nOutputHeight - mnTop;
			mOutBmpInfo.nTop		=  mnLeft - 1;
			mOutBmpInfo.nPicWidth	= m_nOutputHeight;
			mOutBmpInfo.nPicHeight	= m_nOutputWidth;
		}
		else
		{
			mnRatoteAngle = 0;
			mOutBmpInfo.nLeft		= mnLeft;
			mOutBmpInfo.nTop		= mnTop;
			mOutBmpInfo.nPicWidth	= m_nOutputWidth;
			mOutBmpInfo.nPicHeight	= m_nOutputHeight;
		}
	}


	if(mhThread == NULL)
	{
		mbStop = false;
		mhThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)DrawProc, this, 0, &mdwThreadID);

		SetEvent(meFinishDraw);
	}

	return VO_ERR_NONE;
}

VO_U32 CQ7200CCRRR::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if (nID == VO_PID_COMMON_START)
	{
		if(mhThread == NULL)
		{
			mbStop = false;
			mhThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)DrawProc, this, 0, &mdwThreadID);
		}

		return VO_ERR_NONE;
	}

	return CBaseCCRRR::SetParam (nID, pValue);
}

VO_U32 CQ7200CCRRR::Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	if (!IsWindowVisible (mhWnd))
		return VO_ERR_NONE;

	WaitForSingleObject(meFinishDraw , INFINITE);
	ResetEvent (meFinishDraw);

	m_pVideoBuffer = pVideoBuffer;

	SetEvent(meStartDraw);

	return VO_ERR_NONE;

/*
	for (int nH = 0; nH < m_nInputHeight; nH++)
		memcpy (mpPicBufVir + nH * m_nInputWidth, pVideoBuffer->Buffer[0] + pVideoBuffer->Stride[0] * nH, m_nInputWidth);
	voPackUV (mpPicBufVir + m_nFrameSize, pVideoBuffer->Buffer[1], pVideoBuffer->Buffer[2], pVideoBuffer->Stride[1], pVideoBuffer->Stride[2], 
				m_nInputHeight / 2, m_nInputWidth / 2, m_nInputWidth);

	mOutBmpInfo.bufPoint.pBuf1 = (BYTE *)mpDisplay->LockSurface();
//	mpConvert->StartConvert(&mInBmpInfo , &mOutBmpInfo , mnRatoteAngle);
	mpDisplay->UnlockSurface();
*/
	return VO_ERR_NONE;
}

DWORD CQ7200CCRRR::DrawProc (void * pParam)
{
	CQ7200CCRRR *pCCRR = (CQ7200CCRRR *)pParam;

	SetThreadPriority (GetCurrentThread (), THREAD_PRIORITY_TIME_CRITICAL);

	pCCRR->DrawLoop();

	return 0;
}
DWORD CQ7200CCRRR::DrawLoop (void)
{
	while(!mbStop)
	{
		WaitForSingleObject(meStartDraw, INFINITE);
		ResetEvent (meStartDraw);
		if(mbStop)
			break;

		int i = 0;

		if (m_bShareVideoMem)
		{
			LPBYTE pPhysBuff = FindPhysBuffer (m_pVideoBuffer->Buffer[0]);
			mInBmpInfo.bufPoint.pBuf1 = pPhysBuff;

			voPackUV (mpPicBufVirUV, m_pVideoBuffer->Buffer[1], m_pVideoBuffer->Buffer[2], m_pVideoBuffer->Stride[1], m_pVideoBuffer->Stride[2], 
					m_nInputHeight / 2, m_nInputWidth / 2, g_vmVRInfo.Stride);

			for (i = 0; i < VOLOGO_DATA_HEIGHT / 2; i++)
				memcpy (mpPicBufVirUV + g_vmVRInfo.Stride * i, voLogoBufferUV + VOLOGO_DATA_WIDTH * i, VOLOGO_DATA_WIDTH);

			mInBmpInfo.bufPoint.pBuf2 = mpPicBufPhyUV;

			LPBYTE pVirBuffer = FindVirBuffer(m_pVideoBuffer->Buffer[0]);
			CacheRangeFlush(pVirBuffer, m_nFrameSize , CACHE_SYNC_WRITEBACK);
			CacheRangeFlush(mpPicBufVirUV, m_nFrameSize / 2 , CACHE_SYNC_WRITEBACK);
		}
		else
		{
			for (int nH = 0; nH < m_nInputHeight; nH++)
				memcpy (mpPicBufVir + nH * m_nInputWidth, m_pVideoBuffer->Buffer[0] + m_pVideoBuffer->Stride[0] * nH, m_nInputWidth);
			voPackUV (mpPicBufVirUV, m_pVideoBuffer->Buffer[1], m_pVideoBuffer->Buffer[2], m_pVideoBuffer->Stride[1], m_pVideoBuffer->Stride[2], 
					m_nInputHeight / 2, m_nInputWidth / 2, m_nInputWidth);

			for (i = 0; i < VOLOGO_DATA_HEIGHT; i++)
				memcpy (mpPicBufVir + i * m_nInputWidth, voLogoBufferY + VOLOGO_DATA_WIDTH * i, VOLOGO_DATA_WIDTH);
			for (i = 0; i < VOLOGO_DATA_HEIGHT / 2; i++)
				memcpy (mpPicBufVirUV + m_nInputWidth * i, voLogoBufferUV + VOLOGO_DATA_WIDTH * i, VOLOGO_DATA_WIDTH);

			CacheRangeFlush(mpPicBufVir, m_nFrameSize * 3 / 2 , CACHE_SYNC_WRITEBACK);
		}


		mOutBmpInfo.bufPoint.pBuf1 = (BYTE *)mpDisplay->LockSurface();

		

		mpConvert->StartConvert(&mInBmpInfo , &mOutBmpInfo , mnRatoteAngle);

		mpDisplay->UnlockSurface();

		SetEvent(meFinishDraw);
	}

	mhThread = NULL;
	mdwThreadID = 0;

	return 0;
}

LPBYTE CQ7200CCRRR::FindPhysBuffer (LPBYTE pBuffer)
{
	LPBYTE pPhysBuff = NULL;

	for (int i = 0; i < g_vmVRInfo.FrameCount; i++)
	{
		if (pBuffer >= g_vmVRInfo.VBuffer[i].Buffer[0] && pBuffer < g_vmVRInfo.VBuffer[i].Buffer[0] +  g_vmVRInfo.Stride *  g_vmVRInfo.Height)
		{
			pPhysBuff = g_vmVRInfo.PBuffer[i].Buffer[0] + (pBuffer - g_vmVRInfo.VBuffer[i].Buffer[0]);
			break;
		}
	}

	return pPhysBuff;
}

LPBYTE  CQ7200CCRRR::FindVirBuffer(LPBYTE pBuffer)
{
	LPBYTE pVirBuff = NULL;

	for (int i = 0; i < g_vmVRInfo.FrameCount; i++)
	{
		if (pBuffer >= g_vmVRInfo.VBuffer[i].Buffer[0] && pBuffer < g_vmVRInfo.VBuffer[i].Buffer[0] +  g_vmVRInfo.Stride *  g_vmVRInfo.Height)
		{
			pVirBuff = g_vmVRInfo.VBuffer[i].Buffer[0];
			break;
		}
	}

	return pVirBuff;
}


VO_U32 CQ7200CCRRR::GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	*ppVideoMemOP = &g_vmVROP;
//	*ppVideoMemOP = NULL;

	return VO_ERR_NONE;
}

VO_U32 CQ7200CCRRR::voVRMemInit (VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem)
{
	memcpy (&g_vmVRInfo, pVideoMem, sizeof (VO_MEM_VIDEO_INFO));

	g_vmVRInfo.PBuffer = NULL;

	VO_VIDEO_BUFFER * pBuffers =  new VO_VIDEO_BUFFER[g_vmVRInfo.FrameCount];
	VO_VIDEO_BUFFER * pPhysBuff =  new VO_VIDEO_BUFFER[g_vmVRInfo.FrameCount];

	for (int i = 0; i < g_vmVRInfo.FrameCount; i++)
	{
		int nFrameSize = g_vmVRInfo.Stride * (g_vmVRInfo.Height + 16);
		int yStride = g_vmVRInfo.Stride;

		pBuffers[i].Buffer[0] = (BYTE*) AllocPhysMem(nFrameSize, PAGE_READWRITE, 0, 0, (PULONG)&pPhysBuff[i].Buffer[0]);
		VirtualSetAttributes(pBuffers[i].Buffer[0], nFrameSize, CACHE_FLAG, CACHE_MASK, NULL);
		pBuffers[i].Buffer[1] = (BYTE*) AllocPhysMem(nFrameSize / 4, PAGE_READWRITE, 0, 0, (PULONG)&pPhysBuff[i].Buffer[1]);
		VirtualSetAttributes(pBuffers[i].Buffer[1], nFrameSize / 4, CACHE_FLAG, CACHE_MASK, NULL);
		pBuffers[i].Buffer[2] = (BYTE*) AllocPhysMem(nFrameSize / 4, PAGE_READWRITE, 0, 0, (PULONG)&pPhysBuff[i].Buffer[2]);
		VirtualSetAttributes(pBuffers[i].Buffer[2], nFrameSize / 4, CACHE_FLAG, CACHE_MASK, NULL);

		pBuffers[i].Stride[0] = yStride;
		pBuffers[i].Stride[1] = g_vmVRInfo.Stride/2;
		pBuffers[i].Stride[2] = g_vmVRInfo.Stride/2;
		pPhysBuff[i].Stride[0] = yStride;
		pPhysBuff[i].Stride[1] = g_vmVRInfo.Stride/2;
		pPhysBuff[i].Stride[2] = g_vmVRInfo.Stride/2;
	}

	g_vmVRInfo.VBuffer = pBuffers;
	g_vmVRInfo.PBuffer = pPhysBuff;

	pVideoMem->VBuffer = pBuffers;
	pVideoMem->PBuffer = pPhysBuff;

	return 0;
}

VO_U32 CQ7200CCRRR::voVRMemGetBuf (VO_S32 uID, VO_S32 nIndex)
{
	if (nIndex < 0 || nIndex >= g_vmVRInfo.FrameCount)
		return VO_ERR_FAILED;

	return VO_ERR_NONE;
}

VO_U32 CQ7200CCRRR::voVRMemUninit (VO_S32 uID)
{
	if (g_vmVRInfo.VBuffer == NULL)
		return 0;

	for (int i = 0; i < g_vmVRInfo.FrameCount; i++)
	{
		FreePhysMem (g_vmVRInfo.VBuffer[i].Buffer[0]);
		FreePhysMem (g_vmVRInfo.VBuffer[i].Buffer[1]);
		FreePhysMem (g_vmVRInfo.VBuffer[i].Buffer[2]);
	}

	delete []g_vmVRInfo.VBuffer;
	g_vmVRInfo.VBuffer = NULL;

	delete []g_vmVRInfo.PBuffer;
	g_vmVRInfo.PBuffer = NULL;

	return 0;
}

int CQ7200CCRRR::GetRegValue (TCHAR * pKey, TCHAR * pValue, int nDefault)
{
	// Get root key
	HKEY key = HKEY_LOCAL_MACHINE;

	// {00000001-4FEF-40D3-B3FA-E0531B897F98}
	TCHAR szKey[1024];
	_tcscpy (szKey, pKey);

	TCHAR * svKey = szKey;
	TCHAR * svNext;

	int nValue = nDefault;

	// Open key hierarchy
	LONG	lResult = -1;
	HKEY	subkey;
	DWORD	dwPerm = KEY_READ;
	while(svKey != NULL)
	{
		svNext = _tcsstr (svKey, _T("\\"));
		if (svNext)
		{
			*svNext = 0;
			svNext++;
		}
		if(svNext==NULL) 
			dwPerm = KEY_WRITE | KEY_READ;

		RegOpenKeyEx (key, svKey, 0, dwPerm, &subkey);

		key = subkey;
		svKey = svNext;
	}

	if (key == NULL)
		return nValue;

	DWORD dwType,dwLen;

	if(RegQueryValueEx(key, pValue, NULL, &dwType, NULL, &dwLen) == ERROR_SUCCESS )
	{
		if (dwType != REG_DWORD)
		{
			RegCloseKey(key);
			return nValue;
		}

		RegQueryValueEx(key, pValue, NULL, &dwType,(LPBYTE)&nValue, &dwLen);
		RegCloseKey(key);
	}
	else
	{
		RegCloseKey(key);
	}

	return nValue;
}


