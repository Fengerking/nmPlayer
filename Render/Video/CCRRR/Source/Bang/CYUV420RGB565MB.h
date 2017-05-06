	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CYUV420RGB565MB.h

	Contains:	CYUV420RGB565MB header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-07_08		JBF			Create file

*******************************************************************************/

#ifndef __CYUV420RGB565MB_H__
#define __CYUV420RGB565MB_H__

#include "CBaseCCRR.h"
#include "RGBTable.h"

 //#define _SUPPORT_BRIGHTNESS

/***************
	pix_0 | pix_1
	-------------
	pix_2 | pix_3
****************/

#define DEFINE_YUV_POINTER 	VO_PBYTE	pY1 = pYData;\
	VO_PBYTE	pU1 = pUData;\
	VO_PBYTE	pV1 = pVData;\

#define DEFINE_RGB_POINTER 	int *	pRGB1 = (int *)pRGBData;\
int *	pRGB2 = (int *)(pRGBData + nRGBStride);


#define GET_Y1Y2UVDATA(){\
	nY1 = *(pY1++);\
	nY2 = *(pY1++);\
	nU = *(pU1++);\
	nV = *(pV1++);\
}

#define GET_Y1Y2DATA(){\
	nY1 = *(pY2++);\
	nY2 = *(pY2++);\
}

#define GET_YUV05DATA(nY,pY, pU, pV){\
	nY = *(pY++);\
	pY++;\
	nU = *(pU++);\
	nV = *(pV++);\
}

#define GET_Y1Y2UVDATA_R90(){\
	nY1 = *(pY1++);\
	nY2 = *(pY2++);\
	nU = *(pU1++);\
	nV = *(pV1++);\
}

#define GET_Y1Y2DATA_R90(){\
	nY1 = *(pY1++);\
	nY2 = *(pY2++);\
}

#define CALCULATE_UV(){\
	nUV = ((nU - 128) * 748683 + (nV - 128) * 360710) >> 20;\
	nU = ((nU - 128) * 1471152) >> 20;\
	nV = ((nV - 128) * 1859125) >> 20;\
}

#ifdef _SUPPORT_BRIGHTNESS

#define SET_PIX_1_0(nPix, nY){\
nPix = gtRGB5[((nY+nBrightness) + nU + RB_DITHER_P2)>>3];\
nPix += gtRGB6[((nY+nBrightness) - nUV + G_DITHER_P0)>>2] << 5;\
nPix += gtRGB5[((nY+nBrightness) + nV + RB_DITHER_P0)>>3] << 11;\
}

#define SET_PIX_1_1(nPix, nY){\
nPix += gtRGB5[((nY+nBrightness) + nU + RB_DITHER_P3)>>3] << 16;\
nPix += gtRGB6[((nY+nBrightness) - nUV + G_DITHER_P1)>>2] << 21;\
nPix += gtRGB5[((nY+nBrightness) + nV + RB_DITHER_P1)>>3] << 27;\
}

#define SET_PIX_1_2(nPix, nY){\
nPix = gtRGB5[((nY+nBrightness) + nU + RB_DITHER_P0)>>3];\
nPix += gtRGB6[((nY+nBrightness) - nUV + G_DITHER_P2)>>2] << 5;\
nPix += gtRGB5[((nY+nBrightness) + nV + RB_DITHER_P2)>>3] << 11;\
}

#define SET_PIX_1_3(nPix, nY){\
nPix += gtRGB5[((nY+nBrightness) + nU + RB_DITHER_P1)>>3] << 16;\
nPix += gtRGB6[((nY+nBrightness) - nUV + G_DITHER_P3)>>2] << 21;\
nPix += gtRGB5[((nY+nBrightness) + nV + RB_DITHER_P3)>>3] << 27;\
}

#define SET_PIX_2_0(nPix, nY){\
nPix = gtRGB5[((nY+nBrightness) + nU)>>3];\
nPix += gtRGB6[((nY+nBrightness) - nUV)>>2] << 5;\
nPix += gtRGB5[((nY+nBrightness) + nV)>>3] << 11;\
nPix += (nPix << 16);\
}

#define SET_PIX_2_1(nPix, nY){\
nPix = gtRGB5[((nY+nBrightness) + nU)>>3];\
nPix += gtRGB6[((nY+nBrightness) - nUV)>>2] << 5;\
nPix += gtRGB5[((nY+nBrightness) + nV)>>3] << 11;\
nPix += (nPix << 16);\
}

#define SET_PIX_2_2(nPix, nY){\
nPix = gtRGB5[((nY+nBrightness) + nU)>>3];\
nPix += gtRGB6[((nY+nBrightness) - nUV)>>2] << 5;\
nPix += gtRGB5[((nY+nBrightness) + nV)>>3] << 11;\
nPix += (nPix << 16);\
}

#define SET_PIX_2_3(nPix, nY){\
nPix = gtRGB5[((nY+nBrightness) + nU)>>3];\
nPix += gtRGB6[((nY+nBrightness) - nUV)>>2] << 5;\
nPix += gtRGB5[((nY+nBrightness) + nV)>>3] << 11;\
nPix += (nPix << 16);\
}

#else // _SUPPORT_BRIGHTNESS
 
#define SET_PIX_1_0(nPix, nY){\
nPix = gtRGB5[(nY + nU + RB_DITHER_P2)>>3];\
nPix += gtRGB6[(nY - nUV + G_DITHER_P0)>>2] << 5;\
nPix += gtRGB5[(nY + nV + RB_DITHER_P0)>>3] << 11;\
}

#define SET_PIX_1_1(nPix, nY){\
nPix += gtRGB5[(nY + nU + RB_DITHER_P3)>>3] << 16;\
nPix += gtRGB6[(nY - nUV + G_DITHER_P1)>>2] << 21;\
nPix += gtRGB5[(nY + nV + RB_DITHER_P1)>>3] << 27;\
}

#define SET_PIX_1_2(nPix, nY){\
nPix = gtRGB5[(nY + nU + RB_DITHER_P0)>>3];\
nPix += gtRGB6[(nY - nUV + G_DITHER_P2)>>2] << 5;\
nPix += gtRGB5[(nY + nV + RB_DITHER_P2)>>3] << 11;\
}

#define SET_PIX_1_3(nPix, nY){\
nPix += gtRGB5[(nY + nU + RB_DITHER_P1)>>3] << 16;\
nPix += gtRGB6[(nY - nUV + G_DITHER_P3)>>2] << 21;\
nPix += gtRGB5[(nY + nV + RB_DITHER_P3)>>3] << 27;\
}

#define SET_PIX_2_0(nPix, nY){\
nPix = gtRGB5[(nY + nU)>>3];\
nPix += gtRGB6[(nY - nUV)>>2] << 5;\
nPix += gtRGB5[(nY + nV)>>3] << 11;\
nPix += (nPix << 16);\
}

#define SET_PIX_2_1(nPix, nY){\
nPix = gtRGB5[(nY + nU)>>3];\
nPix += gtRGB6[(nY - nUV)>>2] << 5;\
nPix += gtRGB5[(nY + nV)>>3] << 11;\
nPix += (nPix << 16);\
}

#define SET_PIX_2_2(nPix, nY){\
nPix = gtRGB5[(nY + nU)>>3];\
nPix += gtRGB6[(nY - nUV)>>2] << 5;\
nPix += gtRGB5[(nY + nV)>>3] << 11;\
nPix += (nPix << 16);\
}

#define SET_PIX_2_3(nPix, nY){\
nPix = gtRGB5[(nY + nU)>>3];\
nPix += gtRGB6[(nY - nUV)>>2] << 5;\
nPix += gtRGB5[(nY + nV)>>3] << 11;\
nPix += (nPix << 16);\
}

#endif // _SUPPORT_BRIGHTNESS

class CYUV420RGB565MB : public CBaseCCRR
{
public:
	CYUV420RGB565MB(void);
	~CYUV420RGB565MB(void);

	virtual bool	SetVideoSize (int nInWidth, int nInHeight, int nVideoWidth, int nVideoHeight, int nOutWidth, int nOutHeight, int nAngle);
	virtual bool	CCRRData (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB);

	virtual bool	SetBrightness (int nBrightness) {m_nBrightness = nBrightness; return true;}

private:
	bool		CC (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB);
	bool		CC_R90 (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB);

	bool		CC_R (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB);
	bool		CC_R_R90 (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB);

	// MB CC_R_R function
	bool		CCMBR_L10 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, 
					   VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB);
	bool		CCMBR_1020 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, 
					   VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB);
	bool		CCMBR_G20 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, 
					   VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB);

	bool		CCMBR_L10_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, 
					   VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB);
	bool		CCMBR_1020_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, 
					   VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB);
	bool		CCMBR_G20_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, 
					   VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB);

	bool		CCMBR_L10_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, 
					   VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB);
	bool		CCMBR_1020_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, 
					   VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB);
	bool		CCMBR_G20_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, 
					   VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB);

	bool		CCMB05 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);
	bool		CCMB075 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);
	bool		CCMB10 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);
	bool		CCMB150 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);
	bool		CCMB20 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);

	bool		CCMB10_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);
	bool		CCMB05_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);
	bool		CCMB20_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);

	bool		CCMB10_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);
	bool		CCMB05_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);
	bool		CCMB20_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride);

	// Table function
	bool		BuildMBTable (void);
	bool		ReleaseMBTable (void);

	bool		AdjustPixPos (int * pPixsInfo,int nMBS);
	bool		CreateMBBlock (int * pMBPixPos, int ** ppBlockInfo, int nMBS);

	bool		DumpPixsInfo (int * pPixsInfo,int nMBS);
	bool		DumpBlockInfo (int ** ppBlockInfo,int nMBS);

private:
	int			m_nYStride;
	int			m_nUStride;
	int			m_nVStride;

	int			m_nRGBStride;

	int	*		m_pxMBOutSize;
	int	*		m_pyMBOutSize;
	int	*		m_pxMBInSize;
	int	*		m_pyMBInSize;

	int *		m_pxMBPos;
	int *		m_pyMBPos;

	int **		m_ppxMBPixs;
	int **		m_ppyMBPixs;

	int			m_nWidthRest;
	int			m_nHeightRest;
	int	*		m_pxMBRest;
	int	*		m_pxMBRestY;
	int	*		m_pyMBRest;
	int	*		m_pyMBRestX;

	int			m_nBrightness;
};

#endif //__CYUV420RGB565MB_H__