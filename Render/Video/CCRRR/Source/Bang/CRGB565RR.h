	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CRGB565RR.h

	Contains:	CRGB565RR header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-07_08		JBF			Create file

*******************************************************************************/

#ifndef __CRGB565RR_H__
#define __CRGB565RR_H__

#include "CBaseCCRR.h"

class CRGB565RR : public CBaseCCRR
{
public:
	CRGB565RR(void);
	virtual ~CRGB565RR(void);

	virtual bool	SetVideoSize (int nInWidth, int nInHeight, int nVideoWidth, int nVideoHeight, int nOutWidth, int nOutHeight, int nAngle);
	virtual bool	CCRRData (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB);

private:
	bool	Resize10 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize025 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize05 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize075 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	Resize20 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize30 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize40 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	ResizeLess05 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	ResizeAll (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	Resize10_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize025_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize05_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	Resize20_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize30_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize40_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	ResizeAll_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	Resize10_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize025_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize05_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	Resize20_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize30_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize40_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	ResizeAll_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	Resize10_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize025_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize05_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	Resize20_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize30_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);
	bool	Resize40_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);

	bool	ResizeAll_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride);


	bool	BuildTable (void);

private:
	int *		m_pXTable;
	int *		m_pYTable;
	int			m_nXSize;
};

#endif //__CRGB565RR_H__