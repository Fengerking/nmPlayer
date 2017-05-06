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

bool CYUV420RGB565MB::CCMBR_G20_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride,
					VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB)
{
	VO_PBYTE pY1 = pYData;
	VO_PBYTE pY2 = pYData;
	VO_PBYTE pU1 = pUData;
	VO_PBYTE pV1 = pVData;
	int *  pRGB1 = (int *)pRGBData;
	int *  pRGB2 = (int *)(pRGBData + nRGBStride);

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1, nPix2, nPix3;

	int nYLinePixs = 0;
	int nXLinePixs = (m_pyMBOutSize[nYMB] * 2 - 4);

	for (int j = 0; j < m_pxMBInSize[nXMB]; j+=2)
	{
		pRGB1 = (int *) (pRGBData + nYLinePixs  * nRGBStride + nXLinePixs);
		pRGB2 = (int *) ((pRGBData + (nYLinePixs + m_pxMBPos[nXMB * 16 + j]) * nRGBStride) + nXLinePixs);

		for (int m = 0; m < m_pyMBInSize[nYMB]; m+=2)
		{
			pY1 = pYData + m* nYStride + j;
			pY2 = pYData + (m + 1) * nYStride + j;
			pU1 = pUData + (m>>1) * nUVStride + (j >> 1);
			pV1 = pVData + (m>>1) * nUVStride + (j >> 1);

			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
			SET_PIX_2_0 (nPix0, nY1);
			SET_PIX_2_1 (nPix1, nY2);

			GET_Y1Y2DATA_R90 ();
			SET_PIX_2_2 (nPix2, nY1);
			SET_PIX_2_3 (nPix3, nY2);

			int nRepeat = m_pyMBPos[nYMB * 16 + m] / 2;
			while (nRepeat > 0)
			{
				*pRGB1-- = nPix0;
				*pRGB2-- = nPix2;
				nRepeat--;
			}

			nRepeat = m_pyMBPos[nYMB * 16 + m + 1] / 2;
			while (nRepeat > 0)
			{
				*pRGB1-- = nPix1;
				*pRGB2-- = nPix3;
				nRepeat--;
			}
		}

		int nRepeat = m_pxMBPos[nXMB * 16 + j] - 1;
		while (nRepeat > 0)
		{
			memcpy (pRGBData + (nYLinePixs + nRepeat) * nRGBStride, pRGBData + nYLinePixs * nRGBStride, m_pyMBOutSize[nYMB] * 2);
			nRepeat--;
		}
		nYLinePixs += m_pxMBPos[nXMB * 16 + j];

		nRepeat = m_pxMBPos[nXMB * 16 + j + 1] - 1;
		while (nRepeat > 0)
		{
			memcpy (pRGBData + (nYLinePixs + nRepeat) * nRGBStride, pRGBData + nYLinePixs * nRGBStride, m_pyMBOutSize[nYMB] * 2);
			nRepeat--;
		}
		nYLinePixs += m_pxMBPos[nXMB * 16 + j + 1];
	}

	return true;
}

bool CYUV420RGB565MB::CCMBR_1020_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride,
					VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB)
{
	VO_PBYTE pY1 = pYData;
	VO_PBYTE pY2 = pYData;
	VO_PBYTE pU1 = pUData;
	VO_PBYTE pV1 = pVData;
	int *  pRGB1 = (int *)pRGBData;
	int *  pRGB2 = (int *)(pRGBData + nRGBStride);

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1, nPix2, nPix3;

	int	nXPixs;
	int nXPos;

	int nYPixs;
	int nYPos;

	int nYLinePixs = 0;
	int nXLinePixs = (m_pyMBOutSize[nYMB] * 2 - 4);


	int nXBlocks = m_ppxMBPixs[nXMB][0];
	int nYBlocks = m_ppyMBPixs[nYMB][0];

	int * pXPixs = &m_ppxMBPixs[nXMB][1];
	for (int j = 0; j < nXBlocks; j++)
	{
		nXPixs = *pXPixs++;
		nXPos = *pXPixs++;

		for (int n = 0; n < nXPixs; n+=2)
		{
			pRGB1 = (int *) (pRGBData + nYLinePixs * 2 * nRGBStride + nXLinePixs);
			pRGB2 = (int *) ((pRGBData + (nYLinePixs * 2 + m_pxMBPos[nXMB * 16 + nXPos + n + 1]) * nRGBStride) + nXLinePixs);

			int * pYPixs = &m_ppyMBPixs[nYMB][1];
			for (int i = 0; i < nYBlocks; i++)
			{
				nYPixs = *pYPixs++;
				nYPos = *pYPixs++;

				for (int m = 0; m < nYPixs; m+=2)
				{
					pY1 = pYData + (nYPos + m) * nYStride + (nXPos + n);
					pY2 = pYData + (nYPos + m + 1) * nYStride + (nXPos + n);
					pU1 = pUData + ((nYPos + m)>>1) * nUVStride + ((nXPos + n) >> 1);
					pV1 = pVData + ((nYPos + m)>>1) * nUVStride + ((nXPos + n) >> 1);

					if (m_pyMBPos[nYMB * 16 + nYPos + m] > 1)
					{
						GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
						SET_PIX_2_0 (nPix0, nY1);
						SET_PIX_2_1 (nPix1, nY2);

						GET_Y1Y2DATA_R90 ();
						SET_PIX_2_2 (nPix2, nY1);
						SET_PIX_2_3 (nPix3, nY2);

						*pRGB1-- = nPix0;
						*pRGB1-- = nPix1;
						*pRGB2-- = nPix2;
						*pRGB2-- = nPix3;
					}
					else
					{
						GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
						SET_PIX_1_0 (nPix0, nY2);
						SET_PIX_1_1 (nPix0, nY1);

						GET_Y1Y2DATA_R90 ();
						SET_PIX_1_2 (nPix1, nY2);
						SET_PIX_1_3 (nPix1, nY1);

						*pRGB1-- = nPix0;
						*pRGB2-- = nPix1;
					}
				}
			}

			if (m_pxMBPos[nXMB * 16 + nXPos + n] > 1)
			{
				memcpy (pRGBData + (nYLinePixs * 2 + 1) * nRGBStride, pRGBData + (nYLinePixs * 2) * nRGBStride, m_pyMBOutSize[nYMB] * 2);
				memcpy (pRGBData + (nYLinePixs * 2 + 3) * nRGBStride, pRGBData + (nYLinePixs * 2 + 2) * nRGBStride, m_pyMBOutSize[nYMB] * 2);
			}

			nYLinePixs += m_pxMBPos[nXMB * 16 + nXPos + n];
		}
	}

	return true;
}

bool CYUV420RGB565MB::CCMBR_G20_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride,
					VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB)
{
	VO_PBYTE pY1 = pYData;
	VO_PBYTE pY2 = pYData;
	VO_PBYTE pU1 = pUData;
	VO_PBYTE pV1 = pVData;
	int *  pRGB1 = (int *)pRGBData;
	int *  pRGB2 = (int *)(pRGBData + nRGBStride);

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1, nPix2, nPix3;

	int nYLinePixs = 0;

	for (int j = 0; j < m_pxMBInSize[nXMB]; j+=2)
	{
		pRGB1 = (int *) (pRGBData - (nYLinePixs + m_pxMBPos[nXMB * 16 + j]) * nRGBStride);
		pRGB2 = (int *) (pRGBData - nYLinePixs  * nRGBStride);

		for (int m = 0; m < m_pyMBInSize[nYMB]; m+=2)
		{
			pY1 = pYData + m* nYStride + j;
			pY2 = pYData + (m + 1) * nYStride + j;
			pU1 = pUData + (m>>1) * nUVStride + (j >> 1);
			pV1 = pVData + (m>>1) * nUVStride + (j >> 1);

			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
			SET_PIX_2_0 (nPix0, nY1);
			SET_PIX_2_1 (nPix1, nY2);

			GET_Y1Y2DATA_R90 ();
			SET_PIX_2_2 (nPix2, nY1);
			SET_PIX_2_3 (nPix3, nY2);

			int nRepeat = m_pyMBPos[nYMB * 16 + m] / 2;
			while (nRepeat > 0)
			{
				*pRGB1++ = nPix2;
				*pRGB2++ = nPix0;
				nRepeat--;
			}

			nRepeat = m_pyMBPos[nYMB * 16 + m + 1] / 2;
			while (nRepeat > 0)
			{
				*pRGB1++ = nPix3;
				*pRGB2++ = nPix1;
				nRepeat--;
			}
		}

		int nRepeat = m_pxMBPos[nXMB * 16 + j] - 1;
		while (nRepeat > 0)
		{
			memcpy (pRGBData - (nYLinePixs + nRepeat) * nRGBStride, pRGBData - nYLinePixs * nRGBStride, m_pyMBOutSize[nYMB] * 2);
			nRepeat--;
		}
		nYLinePixs += m_pxMBPos[nXMB * 16 + j];

		nRepeat = m_pxMBPos[nXMB * 16 + j + 1] - 1;
		while (nRepeat > 0)
		{
			memcpy (pRGBData - (nYLinePixs + nRepeat) * nRGBStride, pRGBData - nYLinePixs * nRGBStride, m_pyMBOutSize[nYMB] * 2);
			nRepeat--;
		}
		nYLinePixs += m_pxMBPos[nXMB * 16 + j + 1];
	}

	return true;
}

bool CYUV420RGB565MB::CCMBR_L10_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride,
					VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB)
{
	VO_PBYTE pY1 = pYData;
	VO_PBYTE pY2 = pYData;
	VO_PBYTE pU1 = pUData;
	VO_PBYTE pV1 = pVData;
	int *  pRGB1 = (int *)pRGBData;
	int *  pRGB2 = (int *)(pRGBData + nRGBStride);

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1;

	int	nXPixs;
	int nXPos;

	int nYPixs;
	int nYPos;

	int nYLinePixs = 0;
	int nXLinePixs = (m_pyMBOutSize[nYMB] * 2 - 4);

	int nXBlocks = m_ppxMBPixs[nXMB][0];
	int nYBlocks = m_ppyMBPixs[nYMB][0];

	int * pXPixs = &m_ppxMBPixs[nXMB][1];
	for (int j = 0; j < nXBlocks; j++)
	{
		nXPixs = *pXPixs++;
		nXPos = *pXPixs++;

		for (int n = 0; n < nXPixs; n+=2)
		{
			pRGB1 = (int *) ((pRGBData + (nYLinePixs * 2) * nRGBStride) + nXLinePixs);
			pRGB2 = (int *) ((pRGBData + (nYLinePixs * 2 + 1) * nRGBStride) + nXLinePixs);

			int * pYPixs = &m_ppyMBPixs[nYMB][1];
			for (int i = 0; i < nYBlocks; i++)
			{
				nYPixs = *pYPixs++;
				nYPos = *pYPixs++;

				for (int m = 0; m < nYPixs; m+=2)
				{
					pY1 = pYData + (nYPos + m) * nYStride + (nXPos + n);
					pY2 = pYData + (nYPos + m + 1) * nYStride + (nXPos + n);
					pU1 = pUData + ((nYPos + m)>>1) * nUVStride + ((nXPos + n) >> 1);
					pV1 = pVData + ((nYPos + m)>>1) * nUVStride + ((nXPos + n) >> 1);

					GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
					SET_PIX_1_0 (nPix0, nY2);
					SET_PIX_1_1 (nPix0, nY1);

					GET_Y1Y2DATA_R90 ();
					SET_PIX_1_2 (nPix1, nY2);
					SET_PIX_1_3 (nPix1, nY1);

					*pRGB1-- = nPix0;
					*pRGB2-- = nPix1;
				}
			}

			nYLinePixs++;
		}
	}

	return true;
}

bool CYUV420RGB565MB::CCMBR_1020_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride,
					VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB)
{
	VO_PBYTE pY1 = pYData;
	VO_PBYTE pY2 = pYData;
	VO_PBYTE pU1 = pUData;
	VO_PBYTE pV1 = pVData;
	int *  pRGB1 = (int *)pRGBData;
	int *  pRGB2 = (int *)(pRGBData + nRGBStride);

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1, nPix2, nPix3;

	int	nXPixs;
	int nXPos;

	int nYPixs;
	int nYPos;

	int nYLinePixs = 0;

	int nXBlocks = m_ppxMBPixs[nXMB][0];
	int nYBlocks = m_ppyMBPixs[nYMB][0];

	int * pXPixs = &m_ppxMBPixs[nXMB][1];
	for (int j = 0; j < nXBlocks; j++)
	{
		nXPixs = *pXPixs++;
		nXPos = *pXPixs++;

		for (int n = 0; n < nXPixs; n+=2)
		{
			pRGB1 = (int *) (pRGBData - (nYLinePixs * 2 + m_pxMBPos[nXMB * 16 + nXPos + n + 1]) * nRGBStride);
			pRGB2 = (int *) (pRGBData - (nYLinePixs * 2) * nRGBStride);

			int * pYPixs = &m_ppyMBPixs[nYMB][1];
			for (int i = 0; i < nYBlocks; i++)
			{
				nYPixs = *pYPixs++;
				nYPos = *pYPixs++;

				for (int m = 0; m < nYPixs; m+=2)
				{
					pY1 = pYData + (nYPos + m) * nYStride + (nXPos + n);
					pY2 = pYData + (nYPos + m + 1) * nYStride + (nXPos + n);
					pU1 = pUData + ((nYPos + m)>>1) * nUVStride + ((nXPos + n) >> 1);
					pV1 = pVData + ((nYPos + m)>>1) * nUVStride + ((nXPos + n) >> 1);

					if (m_pyMBPos[nYMB * 16 + nYPos + m] > 1)
					{
						GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
						SET_PIX_2_0 (nPix0, nY1);
						SET_PIX_2_1 (nPix1, nY2);

						GET_Y1Y2DATA_R90 ();
						SET_PIX_2_2 (nPix2, nY1);
						SET_PIX_2_3 (nPix3, nY2);

						*pRGB1++ = nPix2;
						*pRGB1++ = nPix3;
						*pRGB2++ = nPix0;
						*pRGB2++ = nPix1;
					}
					else
					{
						GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
						SET_PIX_1_0 (nPix0, nY1);
						SET_PIX_1_1 (nPix0, nY2);

						GET_Y1Y2DATA_R90 ();
						SET_PIX_1_2 (nPix1, nY1);
						SET_PIX_1_3 (nPix1, nY2);

						*pRGB1++ = nPix1;
						*pRGB2++ = nPix0;
					}
				}
			}

			if (m_pxMBPos[nXMB * 16 + nXPos + n] > 1)
			{
				memcpy (pRGBData - (nYLinePixs * 2 + 1) * nRGBStride, pRGBData - (nYLinePixs * 2) * nRGBStride, m_pyMBOutSize[nYMB] * 2);
				memcpy (pRGBData - (nYLinePixs * 2 + 3) * nRGBStride, pRGBData - (nYLinePixs * 2 + 2) * nRGBStride, m_pyMBOutSize[nYMB] * 2);
			}

			nYLinePixs += m_pxMBPos[nXMB * 16 + nXPos + n];
		}
	}

	return true;
}

bool CYUV420RGB565MB::CCMBR_L10_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride,
					VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB)
{
	VO_PBYTE pY1 = pYData;
	VO_PBYTE pY2 = pYData;
	VO_PBYTE pU1 = pUData;
	VO_PBYTE pV1 = pVData;
	int *  pRGB1 = (int *)pRGBData;
	int *  pRGB2 = (int *)(pRGBData + nRGBStride);

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1;

	int	nXPixs;
	int nXPos;

	int nYPixs;
	int nYPos;

	int nYLinePixs = 0;

	int nXBlocks = m_ppxMBPixs[nXMB][0];
	int nYBlocks = m_ppyMBPixs[nYMB][0];

	int * pXPixs = &m_ppxMBPixs[nXMB][1];
	for (int j = 0; j < nXBlocks; j++)
	{
		nXPixs = *pXPixs++;
		nXPos = *pXPixs++;

		for (int n = 0; n < nXPixs; n+=2)
		{
			pRGB1 = (int *) (pRGBData - (nYLinePixs * 2 + 1) * nRGBStride);
			pRGB2 = (int *) (pRGBData - (nYLinePixs * 2) * nRGBStride);

			int * pYPixs = &m_ppyMBPixs[nYMB][1];
			for (int i = 0; i < nYBlocks; i++)
			{
				nYPixs = *pYPixs++;
				nYPos = *pYPixs++;

				for (int m = 0; m < nYPixs; m+=2)
				{
					pY1 = pYData + (nYPos + m) * nYStride + (nXPos + n);
					pY2 = pYData + (nYPos + m + 1) * nYStride + (nXPos + n);
					pU1 = pUData + ((nYPos + m)>>1) * nUVStride + ((nXPos + n) >> 1);
					pV1 = pVData + ((nYPos + m)>>1) * nUVStride + ((nXPos + n) >> 1);

					GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
					SET_PIX_1_0 (nPix0, nY1);
					SET_PIX_1_1 (nPix0, nY2);

					GET_Y1Y2DATA_R90 ();
					SET_PIX_1_2 (nPix1, nY1);
					SET_PIX_1_3 (nPix1, nY2);

					*pRGB1++ = nPix1;
					*pRGB2++ = nPix0;
				}
			}

			nYLinePixs++;
		}
	}

	return true;
}

bool CYUV420RGB565MB::CCMBR_G20 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride,
					VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB)
{
	VO_PBYTE pY1 = pYData;
	VO_PBYTE pY2 = pYData;
	VO_PBYTE pU1 = pUData;
	VO_PBYTE pV1 = pVData;
	int *  pRGB1 = (int *)pRGBData;
	int *  pRGB2 = (int *)(pRGBData + nRGBStride);

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1, nPix2, nPix3;

	int nYLines = 0;
	int nXXPixs = m_nOutWidth/ m_nInWidth;

	for (int i = 0; i < m_pyMBInSize[nYMB]; i+=2)
	{
		pRGB1 = (int *) (pRGBData + nYLines * nRGBStride);
		pRGB2 = (int *) (pRGBData + (nYLines + m_pyMBPos[nYMB * 16 + i]) * nRGBStride);

		pY1 = pYData + i * nYStride;
		pY2 = pYData + (i + 1) * nYStride;
		pU1 = pUData + (i>>1) * nUVStride;
		pV1 = pVData + (i>>1) * nUVStride;

		for (int j = 0; j < m_pxMBInSize[nXMB]; j+=2)
		{
			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_2_0 (nPix0, nY1);
			SET_PIX_2_1 (nPix1, nY2);

			GET_Y1Y2DATA ();
			SET_PIX_2_2 (nPix2, nY1);
			SET_PIX_2_3 (nPix3, nY2);

			int nXX;
			nXXPixs = m_pxMBPos[nXMB * 16 + j];
			for (nXX = 0; nXX < nXXPixs / 2; nXX++)
				*pRGB1++ = nPix0;
			nXXPixs = m_pxMBPos[nXMB * 16 + j + 1];
			for (nXX = 0; nXX < nXXPixs / 2; nXX++)
				*pRGB1++ = nPix1;

			nXXPixs = m_pxMBPos[nXMB * 16 + j];
			for (nXX = 0; nXX < nXXPixs / 2; nXX++)
				*pRGB2++ = nPix2;
			nXXPixs = m_pxMBPos[nXMB * 16 + j + 1];
			for (nXX = 0; nXX < nXXPixs / 2; nXX++)
				*pRGB2++ = nPix3;
		}

		int nYYPixs;
		for (nYYPixs = 1; nYYPixs < m_pyMBPos[nYMB * 16 + i]; nYYPixs++)
			memcpy (pRGBData + (nYLines + nYYPixs) * nRGBStride, pRGBData + nYLines * nRGBStride, m_pxMBOutSize[nXMB] * 2);
		nYLines += m_pyMBPos[nYMB * 16 + i];

		for (nYYPixs = 1; nYYPixs < m_pyMBPos[nYMB * 16 + i + 1]; nYYPixs++)
			memcpy (pRGBData + (nYLines + nYYPixs) * nRGBStride, pRGBData + nYLines * nRGBStride, m_pxMBOutSize[nXMB] * 2);
		nYLines += m_pyMBPos[nYMB * 16 + i + 1];
	}

	return true;
}

bool CYUV420RGB565MB::CCMBR_1020 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride,
					VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB)
{
	VO_PBYTE pY1 = pYData;
	VO_PBYTE pY2 = pYData;
	VO_PBYTE pU1 = pUData;
	VO_PBYTE pV1 = pVData;
	int *  pRGB1 = (int *)pRGBData;
	int *  pRGB2 = (int *)(pRGBData + nRGBStride);

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1, nPix2, nPix3;

	int	nXPixs;
	int nXPos;
	int nXSize;

	int nXBlocks = m_ppxMBPixs[nXMB][0];

	int nYLines = 0;

	for (int i = 0; i < m_pyMBInSize[nYMB]; i+=2)
	{
		pRGB1 = (int *) (pRGBData + nYLines * nRGBStride);
		pRGB2 = (int *) (pRGBData + (nYLines + m_pyMBPos[nYMB * 16 + i]) * nRGBStride);

		nXSize = 0;
		int * pXPixs = &m_ppxMBPixs[nXMB][1];

		for (int j = 0; j < nXBlocks; j++)
		{
			nXPixs = *pXPixs++;
			nXPos = *pXPixs++;

			pY1 = pYData + i * nYStride + nXSize;
			pY2 = pYData + (i + 1) * nYStride + nXSize;
			pU1 = pUData + (i>>1) * nUVStride + (nXSize >> 1);
			pV1 = pVData + (i>>1) * nUVStride + (nXSize >> 1);

			if (m_pxMBPos[nXMB * 16 + nXSize] == 1)
			{
				for (int k = 0; k < nXPixs; k+=2)
				{
					GET_Y1Y2UVDATA (); CALCULATE_UV ();
					SET_PIX_1_0 (nPix0, nY1);
					SET_PIX_1_1 (nPix0, nY2);

					GET_Y1Y2DATA ();
					SET_PIX_1_2 (nPix1, nY1);
					SET_PIX_1_3 (nPix1, nY2);

					*pRGB1++ = nPix0;
					*pRGB2++ = nPix1;
				}
			}
			else
			{
				for (int n = 0; n < nXPixs; n+=2)
				{
					GET_Y1Y2UVDATA (); CALCULATE_UV ();
					SET_PIX_2_0 (nPix0, nY1);
					SET_PIX_2_1 (nPix1, nY2);

					GET_Y1Y2DATA ();
					SET_PIX_2_2 (nPix2, nY1);
					SET_PIX_2_3 (nPix3, nY2);

					*pRGB1++ = nPix0;
					*pRGB1++ = nPix1;
					*pRGB2++ = nPix2;
					*pRGB2++ = nPix3;
				}
			}

			nXSize += nXPixs;
		}

		if (m_pyMBPos[nYMB * 16 + i] > 1)
			memcpy (pRGBData + (nYLines + 1) * nRGBStride, pRGBData + nYLines * nRGBStride, m_pxMBOutSize[nXMB] * 2);
		if (m_pyMBPos[nYMB * 16 + i + 1] > 1)
			memcpy (pRGBData + (nYLines + 3) * nRGBStride, pRGBData + (nYLines + 2) * nRGBStride, m_pxMBOutSize[nXMB] * 2);

		nYLines += m_pyMBPos[nYMB * 16 + i];
		nYLines += m_pyMBPos[nYMB * 16 + i + 1];
	}

	return true;
}

bool CYUV420RGB565MB::CCMBR_L10 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride,
					VO_PBYTE pRGBData, int nRGBStride, int nXMB, int nYMB)
{
	VO_PBYTE pY1 = pYData;
	VO_PBYTE pY2 = pYData;
	VO_PBYTE pU1 = pUData;
	VO_PBYTE pV1 = pVData;
	int *  pRGB1 = (int *)pRGBData;
	int *  pRGB2 = (int *)(pRGBData + nRGBStride);

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1;

	int	nXPixs;
	int nXPos;

	int nYPixs;
	int nYPos;

	int nYLinePixs = 0;

	int nXBlocks = m_ppxMBPixs[nXMB][0];
	int nYBlocks = m_ppyMBPixs[nYMB][0];

	int * pYPixs = &m_ppyMBPixs[nYMB][1];

	for (int i = 0; i < nYBlocks; i++)
	{
		nYPixs = *pYPixs++;
		nYPos = *pYPixs++;

		for (int m = 0; m < nYPixs; m+=2)
		{
			pRGB1 = (int *) (pRGBData + nYLinePixs * 2 * nRGBStride);
			pRGB2 = (int *) (pRGBData + (nYLinePixs * 2 + 1) * nRGBStride);

			int * pXPixs = &m_ppxMBPixs[nXMB][1];
			for (int j = 0; j < nXBlocks; j++)
			{
				nXPixs = *pXPixs++;
				nXPos = *pXPixs++;

				pY1 = pYData + (nYPos + m) * nYStride + nXPos;
				pY2 = pYData + (nYPos + m + 1) * nYStride + nXPos;
				pU1 = pUData + ((nYPos + m)>>1) * nUVStride + (nXPos >> 1);
				pV1 = pVData + ((nYPos + m)>>1) * nUVStride + (nXPos >> 1);
				for (int n = 0; n < nXPixs; n+=2)
				{
					GET_Y1Y2UVDATA (); CALCULATE_UV ();
					SET_PIX_1_0 (nPix0, nY1);
					SET_PIX_1_1 (nPix0, nY2);

					GET_Y1Y2DATA ();
					SET_PIX_1_2 (nPix1, nY1);
					SET_PIX_1_3 (nPix1, nY2);

					*pRGB1++ = nPix0;
					*pRGB2++ = nPix1;
				}
			}
			nYLinePixs++;
		}
	}

	return true;
}

bool CYUV420RGB565MB::CCMB05 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER
	DEFINE_RGB_POINTER

	VO_PBYTE	pY2 = pYData + nYStride * 2;
	VO_PBYTE	pU2 = pUData + nUVStride;
	VO_PBYTE	pV2 = pVData + nUVStride;

	int nY1, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1;

	for (int i = 0; i < 16; i+=4)
	{
		for (int j = 0; j < 4; j++)
		{
			GET_YUV05DATA (nY1, pY1, pU1, pV1);	CALCULATE_UV ();
			SET_PIX_1_0 (nPix0, nY1)

			GET_YUV05DATA (nY1, pY1, pU1, pV1);	CALCULATE_UV ();
			SET_PIX_1_1 (nPix0, nY1)

			GET_YUV05DATA (nY1, pY2, pU2, pV2);	CALCULATE_UV ();
			SET_PIX_1_2 (nPix1, nY1)

			GET_YUV05DATA (nY1, pY2, pU2, pV2);	CALCULATE_UV ();
			SET_PIX_1_3 (nPix1, nY1)

			*pRGB1++ = nPix0;
			*pRGB2++ = nPix1;
		}

		pY1 += (nYStride - 4) << 2;
		pU1 += (nUVStride - 4) << 1;
		pV1 += (nUVStride - 4) << 1;

		pY2 += (nYStride - 4) << 2;
		pU2 += (nUVStride - 4) << 1;
		pV2 += (nUVStride - 4) << 1;

		pRGB1 = pRGB1 - 4 + (nRGBStride >> 1);
		pRGB2 = pRGB2 - 4 + (nRGBStride >> 1);
	}

	return true;
}

bool CYUV420RGB565MB::CCMB075 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER
	DEFINE_RGB_POINTER

	VO_PBYTE	pY2 = pYData + nYStride;

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix11, nPix12, nPix13;
	int nPix20, nPix21, nPix22, nPix23;

	for (int i = 0; i < 16; i+=4)
	{
		pY1 = pYData + i * nYStride;
		pY2 = pYData + (i + 1) * nYStride;
		pU1 = pUData + (i/2) * nUVStride;
		pV1 = pVData + (i/2) * nUVStride;

		pRGB1 = (int *)(pRGBData + (i / 4 * 3) * nRGBStride);
		pRGB2 = (int *)(pRGBData + (i / 4 * 3 + 1) * nRGBStride);

		int j;
		for (j = 0; j < 2; j++)
		{
			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix0, nY1)
			SET_PIX_1_1 (nPix0, nY2)

			GET_Y1Y2DATA ();
			SET_PIX_1_2 (nPix20, nY1)
			SET_PIX_1_3 (nPix20, nY2)

			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix11, nY1)
			SET_PIX_1_1 (nPix11, nY2)

			GET_Y1Y2DATA ();
			SET_PIX_1_2 (nPix21, nY1)
			SET_PIX_1_3 (nPix21, nY2)

			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix12, nY1)
			SET_PIX_1_1 (nPix12, nY2)

			GET_Y1Y2DATA ();
			SET_PIX_1_2 (nPix22, nY1)
			SET_PIX_1_3 (nPix22, nY2)

			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix13, nY1)
			SET_PIX_1_1 (nPix13, nY2)

			GET_Y1Y2DATA ();
			SET_PIX_1_2 (nPix23, nY1)
			SET_PIX_1_3 (nPix23, nY2)

			*pRGB1++ = nPix0;
			*pRGB1++ = (nPix11 & 0XFFFF) + ((nPix12 & 0XFFFF) << 16);
			*pRGB1++ = ((nPix12 >> 16) & 0XFFFF) + ((nPix13 & 0XFFFF) << 16);
			*pRGB2++ = nPix20;
			*pRGB2++ = (nPix21 & 0XFFFF) + ((nPix22 & 0XFFFF) << 16);
			*pRGB2++ = ((nPix22 >> 16) & 0XFFFF) + ((nPix23 & 0XFFFF) << 16);
		}

		pRGB1 = (int *)(pRGBData + (i / 4 * 3 + 2) * nRGBStride);
		pY1 = pYData + (i + 2) * nYStride;
		pU1 = pUData + (i/2 + 1) * nUVStride;
		pV1 = pVData + (i/2 + 1) * nUVStride;
		for (j = 0; j < 2; j++)
		{
			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix0, nY1)
			SET_PIX_1_1 (nPix0, nY2)

			GET_Y1Y2DATA ();

			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix11, nY1)
			SET_PIX_1_1 (nPix11, nY2)

			GET_Y1Y2DATA ();

			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix12, nY1)
			SET_PIX_1_1 (nPix12, nY2)

			GET_Y1Y2DATA ();

			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix13, nY1)
			SET_PIX_1_1 (nPix13, nY2)

			GET_Y1Y2DATA ();

			*pRGB1++ = nPix0;
			*pRGB1++ = (nPix11 & 0XFFFF) + ((nPix12 & 0XFFFF) << 16);
			*pRGB1++ = ((nPix12 >> 16) & 0XFFFF) + ((nPix13 & 0XFFFF) << 16);
		}
	}

	return true;
}

bool CYUV420RGB565MB::CCMB10 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	int * pY = (int *)pYData;
	int * pU = (int *)pUData;
	int * pV = (int *)pVData;

	int * pRGB = (int *)pRGBData;

	int nY1, nY2, nU, nV;
	int nUU, nVV, nUV;

	int nPix;

	for (int i = 0; i < 16; i+=2)
	{
		for (int j = 0; j < 2; j++)
		{
			nY1 = *pY;
			nY2 = *(pY++ + nYStride/4);
			nU = *pU++;
			nV = *pV++;

			nUV = (((nU & 0XFF) - 128) * 748683 + ((nV & 0XFF) - 128) * 360710) >> 20;
			nUU = (((nU & 0XFF) - 128) * 1471152) >> 20;
			nVV = (((nV & 0XFF) - 128) * 1859125) >> 20;

			nPix = gtRGB5[((nY1 & 0XFF) + nUU + RB_DITHER_P2)>>3] +
					(gtRGB6[((nY1 & 0XFF) - nUV + G_DITHER_P0)>>2] << 5) +
					(gtRGB5[((nY1 & 0XFF) + nVV + RB_DITHER_P0)>>3] << 11) +
					(gtRGB5[(((nY1 >> 8) & 0XFF) + nUU + RB_DITHER_P3)>>3] << 16) +
					(gtRGB6[(((nY1 >> 8) & 0XFF) - nUV + G_DITHER_P1)>>2] << 21) +
					(gtRGB5[(((nY1 >> 8) & 0XFF) + nVV + RB_DITHER_P1)>>3] << 27);
			*pRGB = nPix;

			nPix = gtRGB5[((nY2 & 0XFF) + nUU + RB_DITHER_P0)>>3]+
					(gtRGB6[((nY2 & 0XFF) - nUV + G_DITHER_P2)>>2] << 5) +
					(gtRGB5[((nY2 & 0XFF) + nVV + RB_DITHER_P2)>>3] << 11) +
					(gtRGB5[(((nY2 >> 8) & 0XFF) + nUU + RB_DITHER_P1)>>3] << 16) +
					(gtRGB6[(((nY2 >> 8) & 0XFF) - nUV + G_DITHER_P3)>>2] << 21) +
					(gtRGB5[(((nY2 >> 8) & 0XFF) + nVV + RB_DITHER_P3)>>3] << 27);
			*(pRGB++ + nRGBStride / 4) = nPix;

			nUV = ((((nU >> 8) & 0XFF) - 128) * 748683 + (((nV >> 8)& 0XFF) - 128) * 360710) >> 20;
			nUU = ((((nU >> 8) & 0XFF) - 128) * 1471152) >> 20;
			nVV = ((((nV >> 8)& 0XFF) - 128) * 1859125) >> 20;

			nPix = gtRGB5[(((nY1 >> 16)& 0XFF) + nUU + RB_DITHER_P2)>>3]+
					(gtRGB6[(((nY1 >> 16) & 0XFF) - nUV + G_DITHER_P0)>>2] << 5) +
					(gtRGB5[(((nY1 >> 16) & 0XFF) + nVV + RB_DITHER_P0)>>3] << 11) +
					(gtRGB5[(((nY1 >> 24) & 0XFF) + nUU + RB_DITHER_P3)>>3] << 16) +
					(gtRGB6[(((nY1 >> 24) & 0XFF) - nUV + G_DITHER_P1)>>2] << 21) +
					(gtRGB5[(((nY1 >> 24) & 0XFF) + nVV + RB_DITHER_P1)>>3] << 27);
			*pRGB = nPix;

			nPix= gtRGB5[(((nY2 >> 16) & 0XFF) + nUU + RB_DITHER_P0)>>3]+
					(gtRGB6[(((nY2 >> 16) & 0XFF) - nUV + G_DITHER_P2)>>2] << 5) +
					(gtRGB5[(((nY2 >> 16) & 0XFF) + nVV + RB_DITHER_P2)>>3] << 11) +
					(gtRGB5[(((nY2 >> 24) & 0XFF) + nUU + RB_DITHER_P1)>>3] << 16) +
					(gtRGB6[(((nY2 >> 24) & 0XFF) - nUV + G_DITHER_P3)>>2] << 21) +
					(gtRGB5[(((nY2 >> 24) & 0XFF) + nVV + RB_DITHER_P3)>>3] << 27);
			*(pRGB++ + nRGBStride / 4) = nPix;

			nY1 = *pY;
			nY2 = *(pY++ + nYStride/4);
			nUV = ((((nU >> 16) & 0XFF) - 128) * 748683 + (((nV >> 16)& 0XFF) - 128) * 360710) >> 20;
			nUU = ((((nU >> 16) & 0XFF) - 128) * 1471152) >> 20;
			nVV = ((((nV >> 16)& 0XFF) - 128) * 1859125) >> 20;

			nPix = gtRGB5[((nY1 & 0XFF) + nUU + RB_DITHER_P2)>>3]+
					 (gtRGB6[((nY1 & 0XFF) - nUV + G_DITHER_P0)>>2] << 5) +
					 (gtRGB5[((nY1 & 0XFF) + nVV + RB_DITHER_P0)>>3] << 11) +
					 (gtRGB5[(((nY1 >> 8) & 0XFF) + nUU + RB_DITHER_P3)>>3] << 16) +
					 (gtRGB6[(((nY1 >> 8) & 0XFF) - nUV + G_DITHER_P1)>>2] << 21) +
					 (gtRGB5[(((nY1 >> 8) & 0XFF) + nVV + RB_DITHER_P1)>>3] << 27);
			*pRGB = nPix;

			nPix = gtRGB5[((nY2 & 0XFF) + nUU + RB_DITHER_P0)>>3]+
					(gtRGB6[((nY2 & 0XFF) - nUV + G_DITHER_P2)>>2] << 5) +
					(gtRGB5[((nY2 & 0XFF) + nVV + RB_DITHER_P2)>>3] << 11) +
					(gtRGB5[(((nY2 >> 8) & 0XFF) + nUU + RB_DITHER_P1)>>3] << 16) +
					(gtRGB6[(((nY2 >> 8) & 0XFF) - nUV + G_DITHER_P3)>>2] << 21) +
					(gtRGB5[(((nY2 >> 8) & 0XFF) + nVV + RB_DITHER_P3)>>3] << 27);
			*(pRGB++ + nRGBStride / 4) = nPix;


			nUV = ((((nU >> 24) & 0XFF) - 128) * 748683 + (((nV >> 24)& 0XFF) - 128) * 360710) >> 20;
			nUU = ((((nU >> 24) & 0XFF) - 128) * 1471152) >> 20;
			nVV = ((((nV >> 24)& 0XFF) - 128) * 1859125) >> 20;

			nPix = gtRGB5[(((nY1 >> 16)& 0XFF) + nUU + RB_DITHER_P2)>>3]+
					(gtRGB6[(((nY1 >> 16) & 0XFF) - nUV + G_DITHER_P0)>>2] << 5) +
					(gtRGB5[(((nY1 >> 16) & 0XFF) + nVV + RB_DITHER_P0)>>3] << 11) +
					(gtRGB5[(((nY1 >> 24) & 0XFF) + nUU + RB_DITHER_P3)>>3] << 16) +
					(gtRGB6[(((nY1 >> 24) & 0XFF) - nUV + G_DITHER_P1)>>2] << 21) +
					(gtRGB5[(((nY1 >> 24) & 0XFF) + nVV + RB_DITHER_P1)>>3] << 27);
			*pRGB = nPix;

			nPix = gtRGB5[(((nY2 >> 16) & 0XFF) + nUU + RB_DITHER_P0)>>3]+
					(gtRGB6[(((nY2 >> 16) & 0XFF) - nUV + G_DITHER_P2)>>2] << 5) +
					(gtRGB5[(((nY2 >> 16) & 0XFF) + nVV + RB_DITHER_P2)>>3] << 11) +
					(gtRGB5[(((nY2 >> 24) & 0XFF) + nUU + RB_DITHER_P1)>>3] << 16) +
					(gtRGB6[(((nY2 >> 24) & 0XFF) - nUV + G_DITHER_P3)>>2] << 21) +
					(gtRGB5[(((nY2 >> 24) & 0XFF) + nVV + RB_DITHER_P3)>>3] << 27);
			*(pRGB++ + nRGBStride / 4) = nPix;


		}

		pY += (nYStride >> 1) - 4;
		pU += (nUVStride - 8) >> 2;
		pV += (nUVStride - 8) >> 2;

		pRGB += (nRGBStride - 16)>>1;
	}

	return true;
}

/*
bool CYUV420RGB565MB::CCMB10 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER
	DEFINE_RGB_POINTER

	VO_PBYTE	pY2 = pYData + nYStride;

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
	int nBrightness = m_nBrightness;
	int nPix0, nPix2;//, nPix2, nPix3;

	for (int i = 0; i < 16; i+=2)
	{
//		for (int j = 0; j < 4; j++)
//		{
			GET_Y1Y2UVDATA (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1)	SET_PIX_1_1 (nPix0, nY2)	*pRGB1++ = nPix0;
			GET_Y1Y2DATA ();				SET_PIX_1_2 (nPix2, nY1)	SET_PIX_1_3 (nPix2, nY2)	*pRGB2++ = nPix2;

			GET_Y1Y2UVDATA (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1)	SET_PIX_1_1 (nPix0, nY2)	*pRGB1++ = nPix0;
			GET_Y1Y2DATA ();				SET_PIX_1_2 (nPix2, nY1)	SET_PIX_1_3 (nPix2, nY2)	*pRGB2++ = nPix2;

			GET_Y1Y2UVDATA (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1)	SET_PIX_1_1 (nPix0, nY2)	*pRGB1++ = nPix0;
			GET_Y1Y2DATA ();				SET_PIX_1_2 (nPix2, nY1)	SET_PIX_1_3 (nPix2, nY2)	*pRGB2++ = nPix2;

			GET_Y1Y2UVDATA (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1)	SET_PIX_1_1 (nPix0, nY2)	*pRGB1++ = nPix0;
			GET_Y1Y2DATA ();				SET_PIX_1_2 (nPix2, nY1)	SET_PIX_1_3 (nPix2, nY2)	*pRGB2++ = nPix2;

			GET_Y1Y2UVDATA (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1)	SET_PIX_1_1 (nPix0, nY2)	*pRGB1++ = nPix0;
			GET_Y1Y2DATA ();				SET_PIX_1_2 (nPix2, nY1)	SET_PIX_1_3 (nPix2, nY2)	*pRGB2++ = nPix2;

			GET_Y1Y2UVDATA (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1)	SET_PIX_1_1 (nPix0, nY2)	*pRGB1++ = nPix0;
			GET_Y1Y2DATA ();				SET_PIX_1_2 (nPix2, nY1)	SET_PIX_1_3 (nPix2, nY2)	*pRGB2++ = nPix2;

			GET_Y1Y2UVDATA (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1)	SET_PIX_1_1 (nPix0, nY2)	*pRGB1++ = nPix0;
			GET_Y1Y2DATA ();				SET_PIX_1_2 (nPix2, nY1)	SET_PIX_1_3 (nPix2, nY2)	*pRGB2++ = nPix2;

			GET_Y1Y2UVDATA (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1)	SET_PIX_1_1 (nPix0, nY2)	*pRGB1++ = nPix0;
			GET_Y1Y2DATA ();				SET_PIX_1_2 (nPix2, nY1)	SET_PIX_1_3 (nPix2, nY2)	*pRGB2++ = nPix2;

//		}

		pY1 += (nYStride - 8) << 1;
		pY2 += (nYStride - 8) << 1;
		pU1  += nUVStride - 8;
		pV1  += nUVStride - 8;

		pRGB1 += (nRGBStride - 16) >> 1;
		pRGB2 += (nRGBStride - 16) >> 1;
	}
	return true;
}*/


bool CYUV420RGB565MB::CCMB150 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER
	DEFINE_RGB_POINTER

	VO_PBYTE	pY2 = pYData + nYStride;

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1, nPix2, nPix3;

	for (int i = 0; i < 16; i+=2)
	{
		pRGB1 = (int *)(pRGBData + (i / 2 * 3) * nRGBStride);
		pRGB2 = (int *)(pRGBData + (i / 2 * 3 + 1) * nRGBStride);

		for (int j = 0; j < 4; j++)
		{
			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix0, nY1)
			SET_PIX_1_1 (nPix0, nY2)

			GET_Y1Y2DATA ();
			SET_PIX_1_2 (nPix2, nY1)
			SET_PIX_1_3 (nPix2, nY2)

			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix1, nY1)
			SET_PIX_1_1 (nPix1, nY2)

			GET_Y1Y2DATA ();
			SET_PIX_1_2 (nPix3, nY1)
			SET_PIX_1_3 (nPix3, nY2)

			*pRGB1++ = nPix0;
			*pRGB1++ = ((nPix0 >> 16) & 0XFFFF) + (nPix1 << 16);
			*pRGB1++ = nPix1;
			*pRGB2++ = nPix2;
			*pRGB2++ = ((nPix2 >> 16) & 0XFFFF) + (nPix3 << 16);
			*pRGB2++ = nPix3;
		}

		memcpy (pRGBData + (i / 2 * 3 + 2) * nRGBStride, pRGBData + (i / 2 * 3 + 1) * nRGBStride, 48);

		pY1 += (nYStride - 8) << 1;
		pY2 += (nYStride - 8) << 1;
		pU1  += nUVStride - 8;
		pV1  += nUVStride - 8;
	}

	return true;
}

bool CYUV420RGB565MB::CCMB20 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER

	VO_PBYTE	pY2 = pYData + nYStride;

	int *	pRGB1;
	int *	pRGB2;

	for (int i = 0; i < 16; i+=2)
	{
		pRGB1 = (int *)(pRGBData + (i * 2) * nRGBStride);
		pRGB2 = (int *)(pRGBData + (i * 2 + 2) * nRGBStride);

		int nY1, nY2, nU, nV, nUV;
	//	int	nRParam, nGParam, nBParam;
	// 	int nBrightness = m_nBrightness;
		int nPix0, nPix1, nPix2, nPix3;

		for (int j = 0; j < 8; j++)
		{
			GET_Y1Y2UVDATA (); CALCULATE_UV ();
			SET_PIX_2_0 (nPix0, nY1)
			SET_PIX_2_1 (nPix1, nY2)

			GET_Y1Y2DATA ();
			SET_PIX_2_2 (nPix2, nY1)
			SET_PIX_2_2 (nPix3, nY2)

			*pRGB1++ = nPix0;
			*pRGB1++ = nPix1;
			*pRGB2++ = nPix2;
			*pRGB2++ = nPix3;
		}

		memcpy (pRGBData + (i * 2 + 1) * nRGBStride, pRGBData + (i * 2) * nRGBStride, 64);
		memcpy (pRGBData + (i * 2 + 3) * nRGBStride, pRGBData + (i * 2 + 2) * nRGBStride, 64);

		pY1 += (nYStride - 8) << 1;
		pY2 += (nYStride - 8) << 1;
		pU1  += nUVStride - 8;
		pV1  += nUVStride - 8;
	}

	return true;
}

bool CYUV420RGB565MB::CCMB05_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER

	VO_PBYTE	pY2 = pYData + nYStride * 2;
	VO_PBYTE	pU2 = pUData + nUVStride;
	VO_PBYTE	pV2 = pVData + nUVStride;

	int * pRGB1;

	int nY1, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1;

	for (int i = 0; i < 16; i+=4)
	{
		for (int j = 0; j < 16; j+=4)
		{
			GET_YUV05DATA (nY1, pY1, pU1, pV1);	CALCULATE_UV ();
			SET_PIX_1_0 (nPix0, nY1)

			GET_YUV05DATA (nY1, pY1, pU1, pV1);	CALCULATE_UV ();
			SET_PIX_1_2 (nPix1, nY1)

			GET_YUV05DATA (nY1, pY2, pU2, pV2);	CALCULATE_UV ();
			SET_PIX_1_1 (nPix0, nY1)

			GET_YUV05DATA (nY1, pY2, pU2, pV2);	CALCULATE_UV ();
			SET_PIX_1_3 (nPix1, nY1)

			pRGB1 = (int *)(pRGBData + (8 - j / 2 - 1) * nRGBStride + i);
			*pRGB1 = nPix0;
			pRGB1 -= nRGBStride>>2;
			*pRGB1 = nPix1;
		}

		pY1 += (nYStride - 4) << 2;
		pU1 += (nUVStride - 4) << 1;
		pV1 += (nUVStride - 4) << 1;

		pY2 += (nYStride - 4) << 2;
		pU2 += (nUVStride - 4) << 1;
		pV2 += (nUVStride - 4) << 1;
	}

	return true;
}

bool CYUV420RGB565MB::CCMB10_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER
	DEFINE_RGB_POINTER

	VO_PBYTE	pY2 = pYData + nYStride;

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1;


	for (int i = 0; i < 16; i+=2)
	{
		pRGB1 = (int *)(pRGBData + 15 * nRGBStride + i * 2);
//		for (int j = 0; j < 16; j+=2)
//		{
			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1); SET_PIX_1_1 (nPix0, nY2);
			GET_Y1Y2DATA_R90 ();					SET_PIX_1_2 (nPix1, nY1); SET_PIX_1_3 (nPix1, nY2);
			*pRGB1 = nPix0;	pRGB1 -= (nRGBStride>>2);	*pRGB1 = nPix1; 	pRGB1 -= (nRGBStride>>2);

			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1); SET_PIX_1_1 (nPix0, nY2);
			GET_Y1Y2DATA_R90 ();					SET_PIX_1_2 (nPix1, nY1); SET_PIX_1_3 (nPix1, nY2);
			*pRGB1 = nPix0;	pRGB1 -= (nRGBStride>>2);	*pRGB1 = nPix1; 	pRGB1 -= (nRGBStride>>2);

			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1); SET_PIX_1_1 (nPix0, nY2);
			GET_Y1Y2DATA_R90 ();					SET_PIX_1_2 (nPix1, nY1); SET_PIX_1_3 (nPix1, nY2);
			*pRGB1 = nPix0;	pRGB1 -= (nRGBStride>>2);	*pRGB1 = nPix1; 	pRGB1 -= (nRGBStride>>2);

			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1); SET_PIX_1_1 (nPix0, nY2);
			GET_Y1Y2DATA_R90 ();					SET_PIX_1_2 (nPix1, nY1); SET_PIX_1_3 (nPix1, nY2);
			*pRGB1 = nPix0;	pRGB1 -= (nRGBStride>>2);	*pRGB1 = nPix1; 	pRGB1 -= (nRGBStride>>2);

			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1); SET_PIX_1_1 (nPix0, nY2);
			GET_Y1Y2DATA_R90 ();					SET_PIX_1_2 (nPix1, nY1); SET_PIX_1_3 (nPix1, nY2);
			*pRGB1 = nPix0;	pRGB1 -= (nRGBStride>>2);	*pRGB1 = nPix1; 	pRGB1 -= (nRGBStride>>2);

			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1); SET_PIX_1_1 (nPix0, nY2);
			GET_Y1Y2DATA_R90 ();					SET_PIX_1_2 (nPix1, nY1); SET_PIX_1_3 (nPix1, nY2);
			*pRGB1 = nPix0;	pRGB1 -= (nRGBStride>>2);	*pRGB1 = nPix1; 	pRGB1 -= (nRGBStride>>2);

			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1); SET_PIX_1_1 (nPix0, nY2);
			GET_Y1Y2DATA_R90 ();					SET_PIX_1_2 (nPix1, nY1); SET_PIX_1_3 (nPix1, nY2);
			*pRGB1 = nPix0;	pRGB1 -= (nRGBStride>>2);	*pRGB1 = nPix1; 	pRGB1 -= (nRGBStride>>2);

			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();	SET_PIX_1_0 (nPix0, nY1); SET_PIX_1_1 (nPix0, nY2);
			GET_Y1Y2DATA_R90 ();					SET_PIX_1_2 (nPix1, nY1); SET_PIX_1_3 (nPix1, nY2);
			*pRGB1 = nPix0;	pRGB1 -= (nRGBStride>>2);	*pRGB1 = nPix1; 	pRGB1 -= (nRGBStride>>2);
//		}

		pY1 += (nYStride - 8) << 1;
		pY2 += (nYStride - 8) << 1;
		pU1  += nUVStride - 8;
		pV1  += nUVStride - 8;
	}

	return true;
}

bool CYUV420RGB565MB::CCMB20_R90 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER
	DEFINE_RGB_POINTER

	VO_PBYTE	pY2 = pYData + nYStride;

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1, nPix2, nPix3;

	int i;
	for (i = 0; i < 16; i+=2)
	{
		for (int j = 0; j < 16; j+=2)
		{
			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
			SET_PIX_2_0 (nPix0, nY1)
			SET_PIX_2_1 (nPix1, nY2)

			GET_Y1Y2DATA_R90 ();
			SET_PIX_2_2 (nPix2, nY1)
			SET_PIX_2_3 (nPix3, nY2)

			pRGB1 = (int *)(pRGBData + (32 - j * 2 - 1) * nRGBStride + i * 4);
			*pRGB1++ = nPix0;
			*pRGB1 = nPix1;

			pRGB1 = (int *)(pRGBData + (32 - j * 2 - 3) * nRGBStride + i * 4);
			*pRGB1++ = nPix2;
			*pRGB1 = nPix3;

		}

		pY1 += (nYStride - 8) << 1;
		pY2 += (nYStride - 8) << 1;
		pU1  += nUVStride - 8;
		pV1  += nUVStride - 8;
	}

	for (i = 0; i < 16; i++)
		memcpy (pRGBData + (i * 2) * nRGBStride, pRGBData + (i * 2 + 1) * nRGBStride, 64);

	return true;
}

bool CYUV420RGB565MB::CCMB05_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER

	VO_PBYTE	pY2 = pYData + nYStride * 2;
	VO_PBYTE	pU2 = pUData + nUVStride;
	VO_PBYTE	pV2 = pVData + nUVStride;

	int * pRGB1;

	int nY1, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1;

	for (int i = 0; i < 16; i+=4)
	{
		for (int j = 0; j < 16; j+=4)
		{
			GET_YUV05DATA (nY1, pY2, pU2, pV2);	CALCULATE_UV ();
			SET_PIX_1_0 (nPix0, nY1)

			GET_YUV05DATA (nY1, pY2, pU2, pV2);	CALCULATE_UV ();
			SET_PIX_1_2 (nPix1, nY1)

			GET_YUV05DATA (nY1, pY1, pU1, pV1);	CALCULATE_UV ();
			SET_PIX_1_1 (nPix0, nY1)

			GET_YUV05DATA (nY1, pY1, pU1, pV1);	CALCULATE_UV ();
			SET_PIX_1_3 (nPix1, nY1)

			pRGB1 = (int *)(pRGBData +  (j >> 1) * nRGBStride + (12 - i));
			*pRGB1 = nPix0;
			pRGB1 += nRGBStride>>2;
			*pRGB1 = nPix1;
		}

		pY1 += (nYStride - 4) << 2;
		pU1 += (nUVStride - 4) << 1;
		pV1 += (nUVStride - 4) << 1;

		pY2 += (nYStride - 4) << 2;
		pU2 += (nUVStride - 4) << 1;
		pV2 += (nUVStride - 4) << 1;
	}

	return true;
}

bool CYUV420RGB565MB::CCMB10_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER
	DEFINE_RGB_POINTER

	VO_PBYTE	pY2 = pYData + nYStride;

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1;

	for (int i = 0; i < 16; i+=2)
	{
		for (int j = 0; j < 16; j+=2)
		{
			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
			SET_PIX_1_0 (nPix0, nY2)
			SET_PIX_1_1 (nPix0, nY1)

			GET_Y1Y2DATA_R90 ();
			SET_PIX_1_2 (nPix1, nY2)
			SET_PIX_1_3 (nPix1, nY1)

			pRGB1 = (int *)(pRGBData + j * nRGBStride + (16 - i - 2) * 2);
			*pRGB1 = nPix0;
			pRGB1 += nRGBStride>>2;
			*pRGB1 = nPix1;
		}

		pY1 += (nYStride - 8) << 1;
		pY2 += (nYStride - 8) << 1;
		pU1  += nUVStride - 8;
		pV1  += nUVStride - 8;
	}

	return true;
}

bool CYUV420RGB565MB::CCMB20_R270 (VO_PBYTE pYData, VO_PBYTE pUData, VO_PBYTE pVData, int nYStride, int nUVStride, VO_PBYTE pRGBData, int nRGBStride)
{
	DEFINE_YUV_POINTER
	DEFINE_RGB_POINTER

	VO_PBYTE	pY2 = pYData + nYStride;

	int nY1, nY2, nU, nV, nUV;
//	int	nRParam, nGParam, nBParam;
//	int nBrightness = m_nBrightness;
	int nPix0, nPix1, nPix2, nPix3;

	int i;
	for (i = 0; i < 16; i+=2)
	{
		for (int j = 0; j < 16; j+=2)
		{
			GET_Y1Y2UVDATA_R90 (); CALCULATE_UV ();
			SET_PIX_2_0 (nPix1, nY1)
			SET_PIX_2_1 (nPix0, nY2)

			GET_Y1Y2DATA_R90 ();
			SET_PIX_2_2 (nPix3, nY1)
			SET_PIX_2_3 (nPix2, nY2)

			pRGB1 = (int *)(pRGBData + (j << 1) * nRGBStride + (56 - i * 4));
			*pRGB1++ = nPix0;
			*pRGB1 = nPix1;

			pRGB1 = (int *)(pRGBData + ((j << 1) + 2) * nRGBStride + (56 - i * 4));
			*pRGB1++ = nPix2;
			*pRGB1 = nPix3;

		}

		pY1 += (nYStride - 8) << 1;
		pY2 += (nYStride - 8) << 1;
		pU1  += nUVStride - 8;
		pV1  += nUVStride - 8;
	}

	for (i = 0; i < 16; i++)
		memcpy (pRGBData + (i * 2 + 1) * nRGBStride, pRGBData + (i * 2) * nRGBStride, 64);

	return true;
}


