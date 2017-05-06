	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CYUV420RGB565MB.cpp

	Contains:	CYUV420RGB565MB class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-07-08	JBF			Create file

*******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "CYUV420RGB565MB.h"

CYUV420RGB565MB::CYUV420RGB565MB(void)
	: CBaseCCRR ()
	, m_nYStride (0)
	, m_nUStride (0)
	, m_nVStride (0)
	, m_nRGBStride (0)
	, m_pxMBOutSize (NULL)
	, m_pyMBOutSize (NULL)
	, m_pxMBInSize (NULL)
	, m_pyMBInSize (NULL)
	, m_pxMBPos (NULL)
	, m_pyMBPos (NULL)
	, m_ppxMBPixs (NULL)
	, m_ppyMBPixs (NULL)
	, m_nWidthRest (0)
	, m_nHeightRest (0)
	, m_pxMBRest (NULL)
	, m_pxMBRestY (NULL)
	, m_pyMBRest (NULL)
	, m_pyMBRestX (NULL)
	, m_nBrightness (0)
{
	m_vtIn = VO_COLOR_YUV_PLANAR420;
	m_vtOut = VO_COLOR_RGB565_PACKED;
}

CYUV420RGB565MB::~CYUV420RGB565MB(void)
{
	ReleaseMBTable ();

	if (m_pxMBRest != NULL)
	{
		delete []m_pxMBRest;
		m_pxMBRest = NULL;
		delete []m_pxMBRestY;
	}
	if (m_pyMBRest != NULL)
	{
		delete []m_pyMBRest;
		m_pyMBRest = NULL;
		delete []m_pyMBRestX;
	}
}

bool CYUV420RGB565MB::SetVideoSize (int nInWidth, int nInHeight, int nVideoWidth, int nVideoHeight, int nOutWidth, int nOutHeight, int nAngle)
{
	ReleaseMBTable ();

	CBaseCCRR::SetVideoSize (nInWidth, nInHeight, nVideoWidth, nVideoHeight, nOutWidth, nOutHeight, nAngle);

	BuildMBTable ();

	return true;
}

bool CYUV420RGB565MB::CCRRData (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB)
{
	if (m_nAngle != 0 && m_nAngle != 90 && m_nAngle != 270)
		return false;

	if (m_nAngle == 0)
	{
		if ((m_nInWidth == m_nOutWidth && m_nInHeight == m_nOutHeight) || 
			(m_nInWidth * 2 == m_nOutWidth && m_nInHeight * 2 == m_nOutHeight) || 
			(m_nInWidth * 3 == m_nOutWidth * 4 && m_nInHeight * 3 == m_nOutHeight * 4) || 
			(m_nInWidth * 3 == m_nOutWidth * 2 && m_nInHeight * 3 == m_nOutHeight * 2) || 
			(m_nInWidth == m_nOutWidth * 2 && m_nInHeight == m_nOutHeight * 2))
		{
			CC (inData, outData, pMB);
		}
		else
		{
			CC_R (inData, outData, pMB);
		}
	}
	else
	{
		if ((m_nInWidth == m_nOutWidth && m_nInHeight == m_nOutHeight) || 
			(m_nInWidth * 2 == m_nOutWidth && m_nInHeight * 2 == m_nOutHeight) || 
			(m_nInWidth == m_nOutWidth * 2 && m_nInHeight == m_nOutHeight * 2))
		{
			CC_R90 (inData, outData, pMB);
		}
		else
		{
			CC_R_R90 (inData, outData, pMB);
		}
	}

	return true;
}

bool CYUV420RGB565MB::CC (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB)
{
	VO_PBYTE pCCMB = pMB;

	m_nYStride = inData->Stride[0];
	m_nUStride = inData->Stride[1];
	m_nVStride = inData->Stride[2];

	m_nRGBStride = outData->Stride[0];

	int nXMBs = m_nInWidth / 16;
	int nYMBs = m_nInHeight / 16;

	VO_PBYTE pY = inData->Buffer[0];
	VO_PBYTE pU = inData->Buffer[1];
	VO_PBYTE pV = inData->Buffer[2];

	VO_PBYTE pRGB = outData->Buffer[0];

	int nRGBLines = 16;
	int nRGBMB = 32;
	if (m_nInWidth == m_nOutWidth)
	{
		nRGBLines = 16;
		nRGBMB = 32;
	}
	else if (m_nInWidth == m_nOutWidth * 2)
	{
		nRGBLines = 8;
		nRGBMB = 16;
	}
	else if (m_nInWidth * 2 == m_nOutWidth)
	{
		nRGBLines = 32;
		nRGBMB = 64;
	}
	else if (m_nInWidth * 3 == m_nOutWidth * 4)
	{
		nRGBLines = 12;
		nRGBMB = 24;
	}
	else if (m_nInWidth * 3 == m_nOutWidth * 2)
	{
		nRGBLines = 24;
		nRGBMB = 48;
	}

	int i, j;
	for (i = 0; i < nYMBs; i++)
	{
		pY = inData->Buffer[0] + (i << 4) * m_nYStride;
		pU = inData->Buffer[1] + (i << 3) * m_nUStride;
		pV = inData->Buffer[2] + (i << 3) * m_nVStride;

		pRGB = outData->Buffer[0] + (i * nRGBLines) * m_nRGBStride;
		if (pCCMB != NULL)
			pCCMB = pMB + i * (m_nVideoWidth / 16);

		for (j = 0; j < nXMBs; j++)
		{
			if (pCCMB == NULL || *pCCMB++ == 0)
			{
				if (m_nInWidth == m_nOutWidth)
					CCMB10 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
				else if (m_nInWidth == m_nOutWidth * 2)
					CCMB05 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
				else if (m_nInWidth * 3 == m_nOutWidth * 4)
					CCMB075 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
				else if (m_nInWidth * 3 == m_nOutWidth * 2)
					CCMB150 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
				else if (m_nInWidth * 2 == m_nOutWidth)
					CCMB20 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
			}

			pY+=16;
			pU+=8;
			pV+=8;
			pRGB+=nRGBMB;
		}

		if (pCCMB != NULL && m_nWidthRest > 0)
				pCCMB++;
	}

	int nScale = (m_nOutWidth * 4) / m_nInWidth;

	if (m_nHeightRest > 0)
	{
		int nMBHeight = nYMBs * 16;

		pY = inData->Buffer[0] + nMBHeight * m_nYStride;
		pU = inData->Buffer[1] + (nMBHeight / 2) * m_nUStride;
		pV = inData->Buffer[2] + (nMBHeight / 2) * m_nVStride;

		pRGB = outData->Buffer[0] + (nMBHeight * nScale / 4) * m_nRGBStride;

		if (pMB != NULL)
			pCCMB = pMB + nYMBs * (m_nInYUVWidth / 16);

		for (int j = 0; j < nXMBs; j++)
		{
			if (pCCMB == NULL || *pCCMB++ == 0)
			{
				if (m_nInWidth >= m_nOutWidth)
				{
					CCMBR_L10 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, nYMBs);
				}
				else if ((m_nInWidth < m_nOutWidth) && (m_nOutWidth <= m_nInWidth * 2))
				{
					CCMBR_1020 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, nYMBs);
				}
			}

			pY+=16;
			pU+=8;
			pV+=8;
			pRGB+=nRGBMB;
		}
	}


	if (m_nWidthRest > 0)
	{
		int nMBWidth = nXMBs * 16;

		for (int i = 0; i < nYMBs; i++)
		{
			if (pMB != NULL)
				pCCMB = pMB + i * nXMBs;

			if (pCCMB == NULL || *pCCMB == 0)
			{
				pY = inData->Buffer[0] + (i << 4) * m_nYStride + nMBWidth;
				pU = inData->Buffer[1] + (i << 3) * m_nUStride + nMBWidth / 2;
				pV = inData->Buffer[2] + (i << 3) * m_nVStride + nMBWidth / 2;

				pRGB = outData->Buffer[0] + (i * nRGBLines) * m_nRGBStride + nMBWidth * nScale / 2;

				if (m_nInWidth >= m_nOutWidth)
				{
					CCMBR_L10 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
				}
				else if ((m_nInWidth < m_nOutWidth) && (m_nOutWidth <= m_nInWidth * 2))
				{
					CCMBR_1020 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
				}
			}

			pY+=16;
			pU+=8;
			pV+=8;
			pRGB+=nRGBMB;
		}
	}

	if (m_nWidthRest > 0 && m_nHeightRest > 0)
	{
		if (pMB != NULL)
		{
			pCCMB = pMB + nYMBs * (m_nInYUVWidth  / 16) + nXMBs;
			if (*pCCMB != 0)
				return true;
		}

		int nMBHeight = nYMBs * 16;
		int nMBWidth = nXMBs * 16;

		pY = inData->Buffer[0] + nMBHeight * m_nYStride + nMBWidth;
		pU = inData->Buffer[1] + (nMBHeight / 2) * m_nUStride + nMBWidth / 2;
		pV = inData->Buffer[2] + (nMBHeight / 2) * m_nVStride + nMBWidth / 2;

		pRGB = outData->Buffer[0] + (nMBHeight * nScale / 4) * m_nRGBStride + nMBWidth * nScale / 2;

		if (m_nInWidth >= m_nOutWidth)
		{
			CCMBR_L10 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
		}
		else if ((m_nInWidth < m_nOutWidth) && (m_nOutWidth <= m_nInWidth * 2))
		{
			CCMBR_1020 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
		}
	}

	return true;
}

bool CYUV420RGB565MB::CC_R (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB)
{
	VO_PBYTE	pCCMB = pMB;

	m_nYStride = inData->Stride[0];
	m_nUStride = inData->Stride[1];
	m_nVStride = inData->Stride[2];

	m_nRGBStride = outData->Stride[0];

	int nXMBs = m_nInYUVWidth / 16;
	int nYMBs = m_nInYUVHeight / 16;

	VO_PBYTE pY = inData->Buffer[0];
	VO_PBYTE pU = inData->Buffer[1];
	VO_PBYTE pV = inData->Buffer[2];

	VO_PBYTE pRGB = outData->Buffer[0];

	int	nOutYLines = 0;

	for (int i = 0; i < nYMBs; i++)
	{
		pY = inData->Buffer[0] + (i << 4) * m_nYStride;
		pU = inData->Buffer[1] + (i << 3) * m_nUStride;
		pV = inData->Buffer[2] + (i << 3) * m_nVStride;

		pRGB = outData->Buffer[0] + nOutYLines * m_nRGBStride;
		if (pCCMB != NULL)
			pCCMB = pMB + i * (m_nVideoWidth / 16);

		for (int j = 0; j < nXMBs; j++)
		{
			if (pCCMB == NULL || *pCCMB++ == 0)
			{
				if (m_nInWidth >= m_nOutWidth)
				{
					if (m_pyMBOutSize[i] > 0 && m_pxMBOutSize[j] > 0)
						CCMBR_L10 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, i);
				}
				else if ((m_nInWidth < m_nOutWidth) && (m_nOutWidth <= m_nInWidth * 2))
				{
					CCMBR_1020 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, i);
				}
				else
				{
					CCMBR_G20 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, i);
				}
			}

			pY	+= 16;
			pU	+= 8;
			pV	+= 8;

			pRGB += m_pxMBOutSize[j] * 2;
		}

		nOutYLines += m_pyMBOutSize[i];
	}

	return true;
}

bool CYUV420RGB565MB::CC_R90 (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB)
{
	VO_PBYTE pCCMB = pMB;

	m_nYStride = inData->Stride[0];
	m_nUStride = inData->Stride[1];
	m_nVStride = inData->Stride[2];

	m_nRGBStride = outData->Stride[0];

	int nXMBs = m_nInWidth / 16;
	int nYMBs = m_nInHeight / 16;

	VO_PBYTE pY = inData->Buffer[0];
	VO_PBYTE pU = inData->Buffer[1];
	VO_PBYTE pV = inData->Buffer[2];

	VO_PBYTE pRGB = outData->Buffer[0];

	int nRGBOffset = 4;
	if (m_nInWidth == m_nOutWidth)
		nRGBOffset = 4;
	else if (m_nInWidth == m_nOutWidth * 2)
		nRGBOffset = 3;
	else if (m_nInWidth * 2 == m_nOutWidth)
		nRGBOffset = 5;
	int nScale = (m_nOutWidth * 2) / m_nInWidth;

	int i, j;
	for (i = 0; i < nYMBs; i++)
	{
		pY = inData->Buffer[0] + (i << 4) * m_nYStride;
		pU = inData->Buffer[1] + (i << 3) * m_nUStride;
		pV = inData->Buffer[2] + (i << 3) * m_nVStride;

		if (pCCMB != NULL)
			pCCMB = pMB + i * (m_nVideoWidth / 16);

		for (j = 0; j < nXMBs; j++)
		{
			if (pCCMB == NULL || *pCCMB++ == 0)
			{
				if (m_nAngle == 90)
				{
					pRGB = outData->Buffer[0] + (((nXMBs - j - 1) << nRGBOffset) + m_nWidthRest * nScale / 2) * m_nRGBStride + ((i << (nRGBOffset + 1)));

					if (m_nInWidth == m_nOutWidth)
						CCMB10_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
					else if (m_nInWidth == m_nOutWidth * 2)
						CCMB05_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
					else if (m_nInWidth * 2 == m_nOutWidth)
						CCMB20_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
				}
				else
				{
					pRGB = outData->Buffer[0] + (j << nRGBOffset) * m_nRGBStride + ((nYMBs - i - 1) << (nRGBOffset + 1)) + m_nHeightRest * nScale;

					if (m_nInWidth == m_nOutWidth)
						CCMB10_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
					else if (m_nInWidth == m_nOutWidth * 2)
						CCMB05_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
					else if (m_nInWidth * 2 == m_nOutWidth)
						CCMB20_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0]);
				}
			}

			pY+=16;
			pU+=8;
			pV+=8;
		}
		if (m_nWidthRest > 0 && pCCMB != NULL)
			pCCMB++;
	}

	if (m_nHeightRest > 0)
	{
		int nMBHeight = nYMBs * 16;

		pY = inData->Buffer[0] + nMBHeight * m_nYStride;
		pU = inData->Buffer[1] + (nMBHeight / 2) * m_nUStride;
		pV = inData->Buffer[2] + (nMBHeight / 2) * m_nVStride;

		if (pMB != NULL)
			pCCMB = pMB + nYMBs * (m_nInYUVWidth / 16);

		for (int j = 0; j < nXMBs; j++)
		{
			if (pMB == NULL || *pCCMB++ == 0)
			{
				if (m_nAngle == 90)
				{
					pRGB = outData->Buffer[0] + (m_nOutWidth - (j * 16 * nScale / 2) - 1) * m_nRGBStride + nMBHeight * nScale;

					if (m_nInWidth >= m_nOutWidth)
						CCMBR_L10_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, nYMBs);
					else if (m_nInWidth < m_nOutWidth && m_nOutWidth <= m_nInWidth * 2)
						CCMBR_1020_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, nYMBs);
				}
				else
				{
					pRGB = outData->Buffer[0] + (j * 16 * nScale / 2) * m_nRGBStride;

					if (m_nInWidth >= m_nOutWidth)
						CCMBR_L10_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, nYMBs);
					else if (m_nInWidth < m_nOutWidth && m_nOutWidth <= m_nInWidth * 2)
						CCMBR_1020_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, nYMBs);
				}
			}

			pY	+= 16;
			pU	+= 8;
			pV	+= 8;
		}
	}

	if (m_nWidthRest > 0)
	{
		int nMBWidth = nXMBs * 16;

		for (int i = 0; i < nYMBs; i++)
		{
			pY = inData->Buffer[0] + (i << 4) * m_nYStride + nMBWidth;
			pU = inData->Buffer[1] + (i << 3) * m_nUStride + nMBWidth / 2;
			pV = inData->Buffer[2] + (i << 3) * m_nVStride + nMBWidth / 2;

			if (pMB != NULL)
				pCCMB = pMB + i * nXMBs;

			if (pCCMB == NULL || *pCCMB == 0)
			{
				if (m_nAngle == 90)
				{
					pRGB = outData->Buffer[0] + (m_nWidthRest * nScale / 2 - 1) * m_nRGBStride + (i * 16) * nScale;

					if (m_nInWidth >= m_nOutWidth)
							CCMBR_L10_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
					else if (m_nInWidth < m_nOutWidth && m_nOutWidth <= m_nInWidth * 2)
						CCMBR_1020_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
				}
				else
				{
					pRGB = outData->Buffer[0] + (nMBWidth * nScale / 2) * m_nRGBStride + ((nYMBs - i - 1) * 16 + m_nWidthRest) * nScale;

					if (m_nInWidth >= m_nOutWidth)
						CCMBR_L10_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
					else if (m_nInWidth < m_nOutWidth && m_nOutWidth <= m_nInWidth * 2)
						CCMBR_1020_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
				}
			}

			pY	+= 16;
			pU	+= 8;
			pV	+= 8;
		}
	}

	if (m_nWidthRest > 0 && m_nHeightRest > 0)
	{
		if (pMB != NULL)
		{
			pCCMB = pMB + nYMBs * (m_nInYUVWidth  / 16) + nXMBs;
			if (*pCCMB != 0)
				return true;
		}

		int nMBHeight = nYMBs * 16;
		int nMBWidth = nXMBs * 16;

		pY = inData->Buffer[0] + nMBHeight * m_nYStride + nMBWidth;
		pU = inData->Buffer[1] + (nMBHeight / 2) * m_nUStride + nMBWidth / 2;
		pV = inData->Buffer[2] + (nMBHeight / 2) * m_nVStride + nMBWidth / 2;

		if (m_nAngle == 90)
		{
			pRGB = outData->Buffer[0] + (m_nWidthRest * nScale / 2 - 1) * m_nRGBStride + (nYMBs * 16) * nScale;

			if (m_nInWidth >= m_nOutWidth)
					CCMBR_L10_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
			else if (m_nInWidth < m_nOutWidth && m_nOutWidth <= m_nInWidth * 2)
				CCMBR_1020_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
		}
		else
		{
			pRGB = outData->Buffer[0] + (nMBWidth * nScale / 2) * m_nRGBStride;

			if (m_nInWidth >= m_nOutWidth)
				CCMBR_L10_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
			else if (m_nInWidth < m_nOutWidth && m_nOutWidth <= m_nInWidth * 2)
				CCMBR_1020_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], nXMBs, i);
		}
	}

	return true;
}

bool CYUV420RGB565MB::CC_R_R90 (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB)
{
	VO_PBYTE	pCCMB = pMB;

	m_nYStride = inData->Stride[0];
	m_nUStride = inData->Stride[1];
	m_nVStride = inData->Stride[2];

	m_nRGBStride = outData->Stride[0];

	int nXMBs = m_nInYUVWidth / 16;
	int nYMBs = m_nInYUVHeight / 16;

	VO_PBYTE pY = inData->Buffer[0];
	VO_PBYTE pU = inData->Buffer[1];
	VO_PBYTE pV = inData->Buffer[2];

	VO_PBYTE pRGB = outData->Buffer[0];

	int	nXPixs = 0;
	int nYPixs = 0;

	for (int i = 0; i < nYMBs; i++)
	{
		pY = inData->Buffer[0] + (i << 4) * m_nYStride;
		pU = inData->Buffer[1] + (i << 3) * m_nUStride;
		pV = inData->Buffer[2] + (i << 3) * m_nVStride;

		if (pCCMB != NULL)
			pCCMB = pMB + i * (m_nVideoWidth / 16);

		if (m_nAngle == 270)
			nXPixs += m_pyMBOutSize[i];
		nYPixs = 0;
		for (int j = 0; j < nXMBs; j++)
		{
			if (pMB == NULL || *pMB++ == 0)
			{
				if (m_nAngle == 90)
				{
					pRGB = outData->Buffer[0] + (m_nOutWidth - nYPixs - 1) * m_nRGBStride + (nXPixs * 2);

					if (m_nInWidth >= m_nOutWidth)
					{
						if (m_pyMBOutSize[i] > 0 && m_pxMBOutSize[j] > 0)
							CCMBR_L10_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, i);
					}
					else if (m_nInWidth < m_nOutWidth && m_nOutWidth <= m_nInWidth * 2)
					{
						CCMBR_1020_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, i);
					}
					else
					{
						CCMBR_G20_R90 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, i);
					}
				}
				else
				{
					pRGB = outData->Buffer[0] + nYPixs * m_nRGBStride + (m_nOutHeight - nXPixs) * 2;

					if (m_nInWidth >= m_nOutWidth)
					{
						if (m_pyMBOutSize[i] > 0 && m_pxMBOutSize[j] > 0)
							CCMBR_L10_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, i);
					}
					else if (m_nInWidth < m_nOutWidth && m_nOutWidth <= m_nInWidth * 2)
					{
						CCMBR_1020_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, i);
					}
					else
					{
						CCMBR_G20_R270 (pY, pU, pV, inData->Stride[0], inData->Stride[1], pRGB, outData->Stride[0], j, i);
					}
				}
			}

			pY	+= 16;
			pU	+= 8;
			pV	+= 8;

			nYPixs += m_pxMBOutSize[j];
		}
		if (m_nAngle == 90)
			nXPixs += m_pyMBOutSize[i];
	}

	return true;
}

bool CYUV420RGB565MB::BuildMBTable (void)
{
	m_nWidthRest = 0;
	m_nHeightRest = 0;
/*
	if ((m_nInWidth == m_nOutWidth && m_nInHeight == m_nOutHeight) || 
		(m_nInWidth * 2 == m_nOutWidth && m_nInHeight * 2 == m_nOutHeight) || 
		(m_nInWidth == m_nOutWidth * 2 && m_nInHeight == m_nOutHeight * 2))
*/
	{
		int i;

		m_nWidthRest = m_nInWidth % 16;
		m_nHeightRest = m_nInHeight % 16;
		if (m_nWidthRest != 0 || m_nHeightRest != 0)
		{
			if (m_pxMBRest == NULL)
			{
				m_pxMBRest = new int[16];
				m_pyMBRestX = new int[16];
			}
			memset (m_pxMBRest, 0, 16 * 4);
			for (i = 0; i < m_nWidthRest; i++)
			{
				if (m_nInWidth == m_nOutWidth)
					m_pxMBRest[i] = 1;
				else if (m_nInWidth * 2 == m_nOutWidth)
					m_pxMBRest[i] = 2;
				else
				{
					if (i % 2 == 0)
						m_pxMBRest[i] = 1;
				}
			}
			for (i = 0; i < 16; i++)
			{
				if (m_nInWidth == m_nOutWidth)
					m_pyMBRestX[i] = 1;
				else if (m_nInWidth * 2 == m_nOutWidth)
					m_pyMBRestX[i] = 2;
				else
				{
					if (i % 2 == 0)
						m_pyMBRestX[i] = 1;
				}
			}

			if (m_pyMBRest == NULL)
			{
				m_pyMBRest = new int[16];
				m_pxMBRestY = new int[16];
			}
			memset (m_pyMBRest, 0, 16 * 4);
			for (i = 0; i < m_nHeightRest; i++)
			{
				if (m_nInHeight == m_nOutHeight)
					m_pyMBRest[i] = 1;
				else if (m_nInHeight * 2 == m_nOutHeight)
					m_pyMBRest[i] = 2;
				else
				{
					if (i % 2 == 0)
						m_pyMBRest[i] = 1;
				}
			}
			for (i = 0; i < 16; i++)
			{
				if (m_nInWidth == m_nOutWidth)
					m_pxMBRestY[i] = 1;
				else if (m_nInWidth * 2 == m_nOutWidth)
					m_pxMBRestY[i] = 2;
				else
				{
					if (i % 2 == 0)
						m_pxMBRestY[i] = 1;
				}
			}
		}
	}

	int i, j;
	int nXMBs = m_nInYUVWidth / 16;
	int nYMBs = m_nInYUVHeight / 16;
	if (m_pxMBOutSize != NULL)
	{
		delete []m_pxMBOutSize;
		delete []m_pyMBOutSize;
		delete []m_pxMBInSize;
		delete []m_pyMBInSize;
		delete []m_pxMBPos;
		delete []m_pyMBPos;

		if (m_ppxMBPixs != NULL)
		{
			for (j = 0; j < nXMBs; j++)
				delete []m_ppxMBPixs[j];
			for (i = 0; i < nYMBs; i++)
				delete []m_ppyMBPixs[i];

			delete []m_ppxMBPixs;
			delete []m_ppyMBPixs;
			m_ppxMBPixs = NULL;
		}
	}

	int nWidth = 0;
	int nHeight = 0;

	m_pxMBOutSize = new int [nXMBs];
	m_pyMBOutSize = new int [nYMBs];
	m_pxMBInSize = new int [nXMBs];
	m_pyMBInSize = new int [nYMBs];
	m_pxMBPos = new int[m_nInYUVWidth];
	memset (m_pxMBPos, 0, m_nInYUVWidth * sizeof (int));
	m_pyMBPos = new int[m_nInYUVHeight];
	memset (m_pyMBPos, 0, m_nInYUVHeight * sizeof (int));

	double dScale = (double) m_nOutWidth / m_nInWidth;
	nWidth = 0;
	for (i = 1; i <= m_nInYUVWidth; i++)
	{
		while ((i * dScale - 0.0001) >= nWidth)
		{
			m_pxMBPos[i-1]++;
			nWidth++;
		}
		if (nWidth >= m_nOutWidth)
			break;
	}
	AdjustPixPos (m_pxMBPos, nXMBs);
	for (i = 0; i < nXMBs; i++)
	{
		m_pxMBOutSize[i] = 0;
		m_pxMBInSize[i] = 0;
		for (j = 0; j < 16; j++)
		{
			m_pxMBOutSize[i] += m_pxMBPos[i * 16 + j];
			if (m_pxMBPos[i * 16 + j] > 0)
				m_pxMBInSize[i]++;
		}
	}

	dScale = (double) m_nOutHeight / m_nInHeight;
	nHeight = 0;
	for (i = 1; i <= m_nInYUVHeight; i++)
	{
		while ((i * dScale - 0.0001) >= nHeight)
		{
			m_pyMBPos[i-1]++;
			nHeight++;
		}

		if (nHeight >= m_nOutHeight)
			break;
	}
	AdjustPixPos (m_pyMBPos, nYMBs);

	for (i = 0; i < nYMBs; i++)
	{
		m_pyMBOutSize[i] = 0;
		m_pyMBInSize[i] = 0;
		for (j = 0; j < 16; j++)
		{
			m_pyMBOutSize[i] += m_pyMBPos[i * 16 + j];
			if (m_pyMBPos[i * 16 + j] > 0)
				m_pyMBInSize[i]++;
		}
	}

	m_ppxMBPixs = new int* [nXMBs ];
	m_ppyMBPixs = new int* [nYMBs];

	CreateMBBlock (m_pxMBPos, m_ppxMBPixs, nXMBs);
	CreateMBBlock (m_pyMBPos, m_ppyMBPixs, nYMBs);


	DumpPixsInfo (m_pxMBPos, nXMBs);
	DumpBlockInfo (m_ppxMBPixs, nXMBs);

	DumpPixsInfo (m_pyMBPos, nYMBs);
	DumpBlockInfo (m_ppyMBPixs, nYMBs);

	return true;
}

bool CYUV420RGB565MB::ReleaseMBTable (void)
{
	int i, j;
	int nXMBs = m_nInYUVWidth / 16;
	int nYMBs = m_nInYUVHeight / 16;
	if (m_pxMBOutSize != NULL)
	{
		delete []m_pxMBOutSize;
		delete []m_pyMBOutSize;
		delete []m_pxMBInSize;
		delete []m_pyMBInSize;
		delete []m_pxMBPos;
		delete []m_pyMBPos;

		if (m_ppxMBPixs != NULL)
		{
			for (j = 0; j < nXMBs; j++)
				delete []m_ppxMBPixs[j];
			for (i = 0; i < nYMBs; i++)
				delete []m_ppyMBPixs[i];

			delete []m_ppxMBPixs;
			delete []m_ppyMBPixs;
			m_ppxMBPixs = NULL;
		}
	}

	m_pxMBOutSize = NULL;

	return true;
}

bool CYUV420RGB565MB::AdjustPixPos (int * pPixsInfo,int nMBS)
{
	int nMax = 0;
	int nIndex = 0;
	for (nIndex = 0; nIndex < nMBS * 16; nIndex++)
	{
		if (nMax < pPixsInfo[nIndex])
			nMax = pPixsInfo[nIndex];
	}
	nMax = nMax - 1;

	for (nIndex = 0; nIndex < nMBS * 16; nIndex++)
		pPixsInfo[nIndex] = pPixsInfo[nIndex] - nMax;

	int nRest = 0;
	int nMBPixs = 0;

	int nMB, nPix;
	for (nMB = 0; nMB < nMBS; nMB++)
	{
		if (nRest > 0)
		{
			for (nPix = 15; nPix >= 0; nPix--)
			{
				if (pPixsInfo[nMB * 16 + nPix] == 0)
				{
					pPixsInfo[nMB * 16 + nPix] = 1;
					nRest--;
					break;
				}
			}
		}
		if (nRest < 0)
		{
			for (nPix = 15; nPix >= 0; nPix--)
			{
				if (pPixsInfo[nMB * 16 + nPix] == 1)
				{
					pPixsInfo[nMB * 16 + nPix] = 0;
					nRest++;
					break;
				}
			}
		}

		nMBPixs = 0;
		for (nPix = 0; nPix < 16; nPix++)
		{
			if (pPixsInfo[nMB * 16 + nPix] == 1)
			{
				if (nMBPixs == 0 && (nPix % 2))
				{
					pPixsInfo[nMB * 16 + nPix] = 0;
					nRest++;

					int nPos = nPix;
					while (nPos < 16)
					{
						nPos++;
						if (pPixsInfo[nMB * 16 + nPos] == 0)
						{
							pPixsInfo[nMB * 16 + nPos] = 1;
							nRest--;
							break;
						}
					}
					continue;
				}
				nMBPixs++;
			}
			else
			{
				if (nMBPixs == 0)
					continue;

				if (nMBPixs % 2)
				{
					pPixsInfo[nMB * 16 + nPix] = 1;
					nRest--;

					int nPos = nPix;
					while (nPos < 16)
					{
						nPos++;
						if (pPixsInfo[nMB * 16 + nPos] == 1)
						{
							pPixsInfo[nMB * 16 + nPos] = 0;
							nRest++;
							break;
						}
					}
				}

				nMBPixs = 0;
			}
		}
	}

	for (nIndex = 0; nIndex < nMBS * 16; nIndex++)
		pPixsInfo[nIndex] = pPixsInfo[nIndex] + nMax;

	if (nMax >= 2)
	{
		for (nMB = 0; nMB < nMBS; nMB++)
		{
			for (nIndex = 0; nIndex < 16; nIndex+=2)
			{
				if (pPixsInfo[nMB * 16 + nIndex] % 2)
				{
					pPixsInfo[nMB * 16 + nIndex] += 1;
					pPixsInfo[nMB * 16 + nIndex+1] -= 1;
				}
			}
		}
	}

	return true;
}


bool CYUV420RGB565MB::CreateMBBlock (int * pMBPixPos, int ** ppBlockInfo, int nMBS)
{
	int nMBBlock = 0;
	int nPixs = 0;
	int nPos = 0;
	int nPPixs = 0;
	int nMB, nIndex;

	for (nMB = 0; nMB < nMBS; nMB++)
	{
		nMBBlock = 0;
		nPPixs = pMBPixPos[nMB * 16];
		for (nIndex = 0; nIndex < 16; nIndex++)
		{
			if (pMBPixPos[nMB * 16 + nIndex] != nPPixs)
			{
				if (nPPixs > 0)
					nMBBlock++;
				nPPixs = pMBPixPos[nMB * 16 + nIndex];
			}
		}
		if (nPPixs > 0)
			nMBBlock++;

		ppBlockInfo[nMB] = new int[nMBBlock * 2 + 1];
		ppBlockInfo[nMB][0] = nMBBlock;
	}

	for (nMB = 0; nMB < nMBS; nMB++)
	{
		nMBBlock = 0;
		nPixs = 0;
		nPos = 0;

		nPPixs = pMBPixPos[nMB * 16];
		for (nIndex = 0; nIndex < 16; nIndex++)
		{
			if (pMBPixPos[nMB * 16 + nIndex] != nPPixs)
			{
				if (nPPixs > 0)
				{
					ppBlockInfo[nMB][nMBBlock * 2 + 1] = nPixs;
					ppBlockInfo[nMB][nMBBlock * 2 + 2] = nPos;
					nMBBlock++;
				}

				nPPixs = pMBPixPos[nMB * 16 + nIndex];

				nPos = nIndex;
				if (pMBPixPos[nMB * 16 + nIndex] > 0)
					nPixs = 1;
				else
					nPixs = 0;
			}
			else
			{
				nPixs++;
			}
		}

		if (nPPixs > 0)
		{
			ppBlockInfo[nMB][nMBBlock * 2 + 1] = nPixs;
			ppBlockInfo[nMB][nMBBlock * 2 + 2] = nPos;
		}
	}

	return true;
}

bool CYUV420RGB565MB::DumpPixsInfo (int * pPixsInfo, int nMBS)
{
#ifdef _DUMP_FILE 
	CString strValue;
	DWORD dwWrite = 0;

	HANDLE hFile = CreateFile(("F:\\XMB.TXT"), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);
	SetFilePointer (hFile, 0, NULL, FILE_END);
	WriteFile (hFile, "\r\n", 2, &dwWrite, NULL);

	int nPixs = 0;
	for (int i = 0; i < nMBS * 16; i++)
		nPixs += pPixsInfo[i];
	strValue.Format ("%04d  ", nPixs);
	WriteFile (hFile, strValue, strValue.GetLength (), &dwWrite, NULL);

	for (int i = 0; i < nMBS; i++)
	{
		nPixs = 0;
		for (int j = 0; j < 16; j++)
			nPixs += pPixsInfo[i * 16 + j];
		strValue.Format ("P%02d  ", nPixs);
		WriteFile (hFile, strValue, strValue.GetLength (), &dwWrite, NULL);

		for (int j = 0; j < 16; j++)
		{
			strValue.Format ("%d", pPixsInfo[i * 16 + j]);
			WriteFile (hFile, strValue, strValue.GetLength (), &dwWrite, NULL);
		}
		WriteFile (hFile, "  ", 2, &dwWrite, NULL);
	}
	CloseHandle (hFile);
#endif //_DUMP_FILE

	return true;
}

bool CYUV420RGB565MB::DumpBlockInfo (int ** ppBlockInfo,int nMBS)
{
#ifdef _DUMP_FILE
	DWORD dwWrite;

	HANDLE hFile = CreateFile(("F:\\XMB.TXT"), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);
	SetFilePointer (hFile, 0, NULL, FILE_END);
	WriteFile (hFile, "\r\n     ", 7, &dwWrite, NULL);

	CString strBlock;
	int nMBBlock;

	for (int i = 0; i < nMBS; i++)
	{
		nMBBlock = ppBlockInfo[i][0];
		int * pBMBlock = &ppBlockInfo[i][1];
		strBlock.Format (" B%02d  ", nMBBlock);

		for (int j = 0; j < nMBBlock; j++)
		{
			int nPixs = *pBMBlock++;
			int nXPos = *pBMBlock++;

			CString strPos;
			strPos.Format ("%d:%d ", nPixs, nXPos);

			strBlock += strPos;
		}
		WriteFile (hFile, strBlock, strBlock.GetLength (), &dwWrite, NULL);

		int nSpaces = 23 - strBlock.GetLength ();
		while (nSpaces > 0)
		{
			WriteFile (hFile, " ", 1, &dwWrite, NULL);
			nSpaces--;
		}
	}
	CloseHandle (hFile);
#endif //_DUMP_FILE

	return true;
}


