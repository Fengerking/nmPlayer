	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CQ7200CCRRR.h

	Contains:	CQ7200CCRRR header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CQ7200CCRRR_H__
#define __CQ7200CCRRR_H__

#include "../CBaseCCRRR.h"

#include "CQ7200Convert.h"

class CDDrawDisplay;

class CQ7200CCRRR : public CBaseCCRRR
{
public:
	// Used to control the image drawing
	CQ7200CCRRR (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP);
	virtual ~CQ7200CCRRR (void);

	virtual VO_U32 GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);
	virtual VO_U32 GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex);

	virtual VO_U32 SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor);
	virtual VO_U32 SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate);

	virtual VO_U32 GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP);
	virtual VO_U32 SetParam (VO_U32 nID, VO_PTR pValue);

	virtual VO_U32 Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait);

protected:
	LPBYTE	FindPhysBuffer (LPBYTE pBuffer);
	LPBYTE  FindVirBuffer(LPBYTE pBuffer);

	int		GetRegValue (TCHAR * pKey, TCHAR * pValue, int nDefault);
protected:
	CDDrawDisplay *		mpDisplay;
	CQ7200Convert *		mpConvert;

	BmpInfo				mInBmpInfo;
	BmpInfo				mOutBmpInfo;

	int					mnScreenWidth;
	int					mnScreenHeight;
	int					mnRatoteAngle;
	int					m_nFrameSize;

	HWND				mhWnd;
	RECT				mWndRect;
	int					mnLeft;
	int					mnTop;
	bool				mbSameCoordinate;
	int					mnGDIRotateAngle;

	CRITICAL_SECTION	mcsLock;

	BYTE *				mpPicBufVir;
	BYTE *				mpPicBufPhy;
	BYTE *				mpPicBufVirUV;
	BYTE *				mpPicBufPhyUV;

	VO_VIDEO_BUFFER *	m_pVideoBuffer;

	int					m_nLogTime;
	TCHAR				m_szLog[128];

	//Draw thread
protected:
	static DWORD		DrawProc (void * pParam);
	DWORD				DrawLoop (void);

	HANDLE				mhThread;
	DWORD				mdwThreadID;
	HANDLE				meStartDraw;
	HANDLE				meFinishDraw;
	bool				mbStop;

// for share video memory
protected:
	VO_MEM_VIDEO_OPERATOR		g_vmVROP;
	bool						m_bShareVideoMem;

	static VO_U32				voVRMemInit (VO_S32 uID, VO_MEM_VIDEO_INFO * pVideoMem);
	static VO_U32				voVRMemGetBuf (VO_S32 uID, VO_S32 nIndex);
	static VO_U32				voVRMemUninit (VO_S32 uID);

	static VO_MEM_VIDEO_INFO	g_vmVRInfo;
};

#endif // __CQ7200CCRRR_H__
