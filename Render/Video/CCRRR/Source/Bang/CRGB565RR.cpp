	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CRGB565RR.cpp

	Contains:	CRGB565RR class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-07-08	JBF			Create file

*******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "CRGB565RR.h"

CRGB565RR::CRGB565RR(void)
	: CBaseCCRR ()
	, m_pXTable (NULL)
	, m_pYTable (NULL)
	, m_nXSize (0)
{
	m_vtIn = VO_COLOR_RGB565_PACKED;
	m_vtOut = VO_COLOR_RGB565_PACKED;
}

CRGB565RR::~CRGB565RR(void)
{
	if (m_pXTable != NULL)
	{
		delete []m_pXTable;
		m_pXTable = NULL;
		delete []m_pYTable;
		m_pYTable = NULL;
	}
}

bool CRGB565RR::SetVideoSize (int nInWidth, int nInHeight, int nVideoWidth, int nVideoHeight, int nOutWidth, int nOutHeight, int nAngle)
{
	CBaseCCRR::SetVideoSize (nInWidth, nInHeight, nVideoWidth, nVideoHeight, nOutWidth, nOutHeight, nAngle);

	return BuildTable ();
}

bool CRGB565RR::CCRRData (VO_VIDEO_BUFFER * inData, VO_VIDEO_BUFFER * outData, VO_PBYTE pMB)
{
	if (m_nAngle == 0)
	{
		if (m_nInWidth == m_nOutWidth && m_nInHeight == m_nOutHeight)
			Resize10 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth == m_nOutWidth * 4 && m_nInHeight == m_nOutHeight * 4)
			Resize025 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth == m_nOutWidth * 2 && m_nInHeight == m_nOutHeight * 2)
			Resize05 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 3 == m_nOutWidth * 4  && m_nInHeight * 3 == m_nOutHeight * 4)
			Resize075 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 2 == m_nOutWidth && m_nInHeight * 2 == m_nOutHeight)
			Resize20 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 3 == m_nOutWidth && m_nInHeight * 3 == m_nOutHeight)
			Resize30 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 4 == m_nOutWidth  && m_nInHeight * 4 == m_nOutHeight)
			Resize40 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth > m_nOutWidth * 2)
			ResizeLess05 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else
			ResizeAll (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
	}
	else if (m_nAngle == 90)
	{
		if (m_nInWidth == m_nOutWidth && m_nInHeight == m_nOutHeight)
			Resize10_R90 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth == m_nOutWidth * 4 && m_nInHeight == m_nOutHeight * 4)
			Resize025_R90 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth == m_nOutWidth * 2 && m_nInHeight == m_nOutHeight * 2)
			Resize05_R90 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 2 == m_nOutWidth && m_nInHeight * 2 == m_nOutHeight)
			Resize20_R90 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 3 == m_nOutWidth && m_nInHeight * 3 == m_nOutHeight)
			Resize30_R90 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 4 == m_nOutWidth  && m_nInHeight * 4 == m_nOutHeight)
			Resize40_R90 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else
			ResizeAll_R90 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
	}
	else if (m_nAngle == 180)
	{
		if (m_nInWidth == m_nOutWidth && m_nInHeight == m_nOutHeight)
			Resize10_R180 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth == m_nOutWidth * 4 && m_nInHeight == m_nOutHeight * 4)
			Resize025_R180 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth == m_nOutWidth * 2 && m_nInHeight == m_nOutHeight * 2)
			Resize05_R180 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 2 == m_nOutWidth && m_nInHeight * 2 == m_nOutHeight)
			Resize20_R180 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 3 == m_nOutWidth && m_nInHeight * 3 == m_nOutHeight)
			Resize30_R180 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 4 == m_nOutWidth  && m_nInHeight * 4 == m_nOutHeight)
			Resize40_R180 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else
			ResizeAll_R180 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
	}
	else if (m_nAngle == 270)
	{
		if (m_nInWidth == m_nOutWidth && m_nInHeight == m_nOutHeight)
			Resize10_R270 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth == m_nOutWidth * 4 && m_nInHeight == m_nOutHeight * 4)
			Resize025_R270 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth == m_nOutWidth * 2 && m_nInHeight == m_nOutHeight * 2)
			Resize05_R270 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 2 == m_nOutWidth && m_nInHeight * 2 == m_nOutHeight)
			Resize20_R270 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 3 == m_nOutWidth && m_nInHeight * 3 == m_nOutHeight)
			Resize30_R270 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else if (m_nInWidth * 4 == m_nOutWidth  && m_nInHeight * 4 == m_nOutHeight)
			Resize40_R270 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
		else
			ResizeAll_R270 (inData->Buffer[0], inData->Stride[0], outData->Buffer[0], outData->Stride[0]);
	}

	return true;
}

bool CRGB565RR::Resize10 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	VO_PBYTE pSourBuf = pSource;
	VO_PBYTE pDestBuf = pDest;

	for (int i = 0; i < m_nInHeight; i++)
	{
		memcpy (pDestBuf + (m_nOutHeight - i - 1) * nOutStride, pSourBuf + i * nInStride, m_nInWidth * 2);
	}

	return true;
}

bool CRGB565RR::Resize025 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *) pSource;
	int * pDestBuf = (int *) pDest;

	for (int i = 0; i < m_nInHeight - 3; i+=4)
	{
		pSourBuf = (int *)(pSource + i * nInStride);
		pDestBuf = (int *)(pDest + (m_nOutHeight - i / 4 - 1) * nOutStride);

		for (int j = 0; j < m_nInWidth - 7; j+=8)
		{
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf+2) & 0XFFFF) << 16);
			pSourBuf+=4;
		}
	}

	return true;
}

bool CRGB565RR::Resize05 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *) pSource;
	int * pDestBuf = (int *) pDest;

	int i, j;
	for (i = 0; i < m_nInHeight-1; i+=2)
	{
		pSourBuf = (int *)(pSource + i * nInStride);
		pDestBuf = (int *)(pDest + (m_nOutHeight - i / 2 - 1) * nOutStride);

		for (j = 0; j < m_nInWidth - 15; j+=16)
		{
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf+1) & 0XFFFF) << 16);	pSourBuf+=2;
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf+1) & 0XFFFF) << 16);	pSourBuf+=2;
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf+1) & 0XFFFF) << 16);	pSourBuf+=2;
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf+1) & 0XFFFF) << 16);	pSourBuf+=2;
		}
		while (j < m_nInWidth)
		{
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf+1) & 0XFFFF) << 16);	pSourBuf+=2;
			j+=4;
		}

	}

	return true;
}

bool CRGB565RR::Resize075 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	VO_BYTE * pSourBuf = NULL;
	VO_BYTE * pDestBuf = NULL;

	int nDestLine = 0;
	for (int i = 0; i < m_nInHeight; i++)
	{
		if (i % 4 == 3)	
			continue;

		pSourBuf = pSource + i * nInStride;
		pDestBuf = pDest + (m_nOutHeight - nDestLine - 1) * nOutStride;

		for (int j = 0; j < m_nInWidth; j+=4)
		{
			memcpy (pDestBuf, pSourBuf, 6);
			pSourBuf += 8;
			pDestBuf += 6;
		}

		nDestLine++;
	}

	return true;
}

bool CRGB565RR::Resize20 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *) pSource;
	int * pDestBuf = (int *) pDest;

	for (int i = 0; i < m_nInHeight; i++)
	{
		pSourBuf = (int *)(pSource + (m_nInHeight - i - 1) * nInStride);
		pDestBuf = (int *)(pDest + (i * 2) * nOutStride);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF0000) | ((*(pSourBuf++) & 0XFFFF0000) >> 16);
		}

		memcpy (pDest + (i * 2 + 1) * nOutStride, pDest + (i * 2) * nOutStride, m_nOutWidth * 2);
	}

	return true;
}

bool CRGB565RR::Resize30 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *) pSource;
	int * pDestBuf = (int *) pDest;

	for (int i = 0; i < m_nInHeight; i++)
	{
		pSourBuf = (int *)(pSource + (m_nInHeight - i - 1) * nInStride);
		pDestBuf = (int *)(pDest + (i * 3) * nOutStride);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			*(pDestBuf++) = (*(pSourBuf) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);
			*(pDestBuf++) = (*(pSourBuf) & 0XFFFF) | ((*(pSourBuf) & 0XFFFF0000));
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF0000) | ((*(pSourBuf++) & 0XFFFF0000) >> 16);
		}
		memcpy (pDest + (i * 3 + 1) * nOutStride, pDest + i * 3 * nOutStride, m_nOutWidth * 2);
		memcpy (pDest + (i * 3 + 2) * nOutStride, pDest + i * 3 * nOutStride, m_nOutWidth * 2);
	}

	return true;
}

bool CRGB565RR::Resize40 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *) pSource;
	int * pDestBuf = (int *) pDest;

	for (int i = 0; i < m_nInHeight; i++)
	{
		pSourBuf = (int *)(pSource + (m_nInHeight - i - 1) * nInStride);
		pDestBuf = (int *)(pDest + i * 4 * nOutStride);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF0000) | ((*(pSourBuf) & 0XFFFF0000) >> 16);
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF0000) | ((*(pSourBuf++) & 0XFFFF0000) >> 16);
		}
		memcpy (pDest + (i * 4 + 1) * nOutStride, pDest + i * 4 * nOutStride, m_nOutWidth * 2);
		memcpy (pDest + (i * 4 + 2) * nOutStride, pDest + i * 4 * nOutStride, m_nOutWidth * 2);
		memcpy (pDest + (i * 4 + 3) * nOutStride, pDest + i * 4 * nOutStride, m_nOutWidth * 2);
	}

	return true;
}

bool CRGB565RR::ResizeLess05 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	VO_PBYTE  pSourBuf = NULL;
	VO_PBYTE  pDestBuf = NULL;

	for (int i = 0; i < m_nOutHeight; i++)
	{
		pSourBuf = pSource + m_pYTable[i] * nInStride;
		pDestBuf = pDest + (m_nOutHeight - i - 1) * nOutStride;

		for (int j = 0; j < m_nOutWidth; j++)
		{
			*pDestBuf++ = * (pSourBuf + m_pXTable[j]);
			*pDestBuf++ = * (pSourBuf + m_pXTable[j] + 1);
		}
	}

	return true;
}

bool CRGB565RR::ResizeAll (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	VO_PBYTE  pSourBuf = NULL;
	VO_PBYTE  pDestBuf = NULL;

	int		nOutLen = 0;

	for (int i = 0; i < m_nOutHeight; i++)
	{
		pSourBuf = pSource + m_pYTable[i] * nInStride;
		pDestBuf = pDest + (m_nOutHeight - i - 1) * nOutStride;
		nOutLen = 0;

		for (int j = 0; j < m_nXSize; j++)
		{
			memcpy (pDestBuf + nOutLen, pSourBuf + m_pXTable[j * 2 + 1], m_pXTable[j * 2]);
			nOutLen += m_pXTable[j * 2];
		}

		while (m_pYTable[i+1] == m_pYTable[i])
		{
			memcpy (pDest + (m_nOutHeight - i - 2) * nOutStride, pDest + (m_nOutHeight - i - 1) * nOutStride, m_nOutWidth * 2);
			i++;
			if (i >= m_nOutHeight)
				break;
		}
	}

	return true;
}

bool CRGB565RR::Resize10_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *)pSource;
	int * pDestBuf = (int *)pDest;

	int nPix = 0;

	for (int i = 0; i < m_nInHeight; i++)
	{
		pSourBuf = (int *)(pSource + i * nInStride + m_nInWidth * 2 - 4);
		pDestBuf = (int *)(pDest + i * nOutStride);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			nPix = *pSourBuf--;
			*pDestBuf = (nPix >> 16) & 0XFFFF;
			*pDestBuf++ += (nPix << 16) & 0XFFFF0000;
		}
	}

	return true;
}

bool CRGB565RR::Resize025_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *) pSource;
	int * pDestBuf = (int *) pDest;

	for (int i = 0; i < m_nInHeight - 3; i+=4)
	{
		pSourBuf = (int *)(pSource + i * nInStride + m_nInWidth * 2 - 4);
		pDestBuf = (int *)(pDest + i / 4 * nOutStride);

		for (int j = 0; j < m_nInWidth - 7; j+=8)
		{
			*(pDestBuf++) = (* (pSourBuf + 2) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);
			pSourBuf-=4;
		}
	}

	return true;
}

bool CRGB565RR::Resize05_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *) pSource;
	int * pDestBuf = (int *) pDest;

	int i, j;
	for (i = 0; i < m_nInHeight-1; i+=2)
	{
		pSourBuf = (int *)(pSource + i * nInStride + m_nInWidth * 2 - 4);
		pDestBuf = (int *)(pDest + i / 2 * nOutStride);

		for (j = 0; j < m_nInWidth - 15; j+=16)
		{
			*(pDestBuf++) = (* (pSourBuf + 1) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);	pSourBuf-=2;
			*(pDestBuf++) = (* (pSourBuf + 1) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);	pSourBuf-=2;
			*(pDestBuf++) = (* (pSourBuf + 1) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);	pSourBuf-=2;
			*(pDestBuf++) = (* (pSourBuf + 1) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);	pSourBuf-=2;
		}
		while (j < m_nInWidth)
		{
			*(pDestBuf++) = (* (pSourBuf + 1) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);	pSourBuf-=2;
			j+=4;
		}

	}

	return true;
}

bool CRGB565RR::Resize20_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *) pSource;
	int * pDestBuf = (int *) pDest;

	for (int i = 0; i < m_nInHeight; i++)
	{
		pSourBuf = (int *)(pSource + i * nInStride + m_nInWidth * 2 - 4);
		pDestBuf = (int *)(pDest + (i * 2) * nOutStride);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF0000) | ((*(pSourBuf) & 0XFFFF0000) >> 16);
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf--) & 0XFFFF) << 16);
		}

		memcpy (pDest + (i * 2 + 1) * nOutStride, pDest + (i * 2) * nOutStride, m_nOutWidth * 2);
	}

	return true;
}

bool CRGB565RR::Resize30_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *) pSource;
	int * pDestBuf = (int *) pDest;

	for (int i = 0; i < m_nInHeight; i++)
	{
		pSourBuf = (int *)(pSource + i * nInStride + m_nInWidth * 2 - 4);
		pDestBuf = (int *)(pDest + (i * 3) * nOutStride);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF0000) | ((*(pSourBuf) & 0XFFFF0000) >> 16);
			*(pDestBuf++) = (*(pSourBuf) & 0XFFFF) | ((*(pSourBuf) & 0XFFFF0000));
			*(pDestBuf++) = (*(pSourBuf) & 0XFFFF) | ((* (pSourBuf--) & 0XFFFF) << 16);
		}
		memcpy (pDest + (i * 3 + 1) * nOutStride, pDest + i * 3 * nOutStride, m_nOutWidth * 2);
		memcpy (pDest + (i * 3 + 2) * nOutStride, pDest + i * 3 * nOutStride, m_nOutWidth * 2);
	}

	return true;
}

bool CRGB565RR::Resize40_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	int * pSourBuf = (int *) pSource;
	int * pDestBuf = (int *) pDest;

	for (int i = 0; i < m_nInHeight; i++)
	{
		pSourBuf = (int *)(pSource + i * nInStride + m_nInWidth * 2 - 4);
		pDestBuf = (int *)(pDest + i * 4 * nOutStride);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF0000) | ((*(pSourBuf) & 0XFFFF0000) >> 16);
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF0000) | ((*(pSourBuf) & 0XFFFF0000) >> 16);
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf) & 0XFFFF) << 16);
			*(pDestBuf++) = (* (pSourBuf) & 0XFFFF) | ((* (pSourBuf--) & 0XFFFF) << 16);
		}
		memcpy (pDest + (i * 4 + 1) * nOutStride, pDest + i * 4 * nOutStride, m_nOutWidth * 2);
		memcpy (pDest + (i * 4 + 2) * nOutStride, pDest + i * 4 * nOutStride, m_nOutWidth * 2);
		memcpy (pDest + (i * 4 + 3) * nOutStride, pDest + i * 4 * nOutStride, m_nOutWidth * 2);
	}

	return true;
}

bool CRGB565RR::ResizeAll_R180 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	VO_PBYTE  pSourBuf = NULL;
	VO_PBYTE  pDestBuf = NULL;

	for (int i = 0; i < m_nOutHeight; i++)
	{
		pSourBuf = pSource + m_pYTable[i] * nInStride;
		pDestBuf = pDest + i * nOutStride;

		for (int j = 0; j < m_nOutWidth; j++)
		{
			*pDestBuf++ = * (pSourBuf + m_pXTable[m_nOutWidth - j - 1]);
			*pDestBuf++ = * (pSourBuf + m_pXTable[m_nOutWidth - j - 1] + 1);
		}
	}

	return true;
}

bool CRGB565RR::Resize10_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest + ((m_nOutWidth - 1) * nOutStride) + m_nOutHeight * 2 - 4);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight; i+=2)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 1) * nInStride);

		pDestOut = pDestStart - (i >> 1);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;

			*pDestOut = (nPix2 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			pDestOut -= nOutStride / 4;

			*pDestOut = ((nPix2 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			pDestOut -= nOutStride / 4;
		}
	}

	return true;
}

bool CRGB565RR::Resize025_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest + ((m_nOutWidth - 1) * nOutStride) + m_nOutHeight * 2 - 4);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight - 7; i+=8)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 4) * nInStride);

		pDestOut = pDestStart - (i >> 3);

		for (int j = 0; j < m_nInWidth - 7; j+=8)
		{
			nPix1 = (*pSourBuf1 & 0XFFFF) + ((*(pSourBuf1 + 2) & 0XFFFF) << 16);
			nPix2 = (*pSourBuf2 & 0XFFFF) + ((*(pSourBuf2 + 2) & 0XFFFF) << 16);

			*pDestOut = (nPix2 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			pDestOut -= nOutStride / 4;

			*pDestOut = ((nPix2 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			pDestOut -= nOutStride / 4;

			pSourBuf1 += 4;
			pSourBuf2 += 4;
		}
	}

	return true;
}

bool CRGB565RR::Resize05_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest + ((m_nOutWidth - 1) * nOutStride) + m_nOutHeight * 2 - 4);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight - 3; i+=4)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 2) * nInStride);

		pDestOut = pDestStart - (i >> 2);

		for (int j = 0; j < m_nInWidth - 3; j+=4)
		{
			nPix1 = (*pSourBuf1 & 0XFFFF) + ((*(pSourBuf1 + 1) & 0XFFFF) << 16);
			nPix2 = (*pSourBuf2 & 0XFFFF) + ((*(pSourBuf2 + 1) & 0XFFFF) << 16);

			*pDestOut = (nPix2 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			pDestOut -= nOutStride / 4;

			*pDestOut = ((nPix2 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			pDestOut -= nOutStride / 4;

			pSourBuf1 += 2;
			pSourBuf2 += 2;
		}
	}

	return true;
}

bool CRGB565RR::Resize20_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest + ((m_nOutWidth - 1) * nOutStride) + m_nOutHeight * 2 - 8);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight; i+=2)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 1) * nInStride);

		pDestOut = pDestStart - i;

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;

			*pDestOut = (nPix2 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			*(pDestOut + 1) = (nPix1 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			pDestOut -= nOutStride / 2;

			*pDestOut = ((nPix2 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			*(pDestOut + 1) = ((nPix1 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			pDestOut -= nOutStride / 2;
		}
	}

	for (int nHeight = 0; nHeight < m_nInWidth; nHeight++)
		memcpy (pDest + ((m_nOutWidth - nHeight * 2 - 2) * nOutStride), pDest + ((m_nOutWidth - nHeight * 2 - 1) * nOutStride), m_nOutHeight * 2);

	return true;
}

bool CRGB565RR::Resize30_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest + ((m_nOutWidth - 1) * nOutStride) + m_nOutHeight * 2 - 12);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight; i+=2)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 1) * nInStride);

		pDestOut = pDestStart - (i * 3 / 2);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;

			*pDestOut = (nPix2 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			*(pDestOut + 1) = (nPix2 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			*(pDestOut + 2) = (nPix1 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			pDestOut -= nOutStride * 3 / 4;

			*pDestOut = ((nPix2 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			*(pDestOut + 1) = ((nPix2 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			*(pDestOut + 2) = ((nPix1 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			pDestOut -= nOutStride * 3 / 4;
		}
	}

	for (int nHeight = 0; nHeight < m_nInWidth; nHeight++)
	{
		memcpy (pDest + ((m_nOutWidth - nHeight * 3 - 2) * nOutStride), pDest + ((m_nOutWidth - nHeight * 3 - 1) * nOutStride), m_nOutHeight * 2);
		memcpy (pDest + ((m_nOutWidth - nHeight * 3 - 3) * nOutStride), pDest + ((m_nOutWidth - nHeight * 3 - 1) * nOutStride), m_nOutHeight * 2);
	}

	return true;
}

bool CRGB565RR::Resize40_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest + ((m_nOutWidth - 1) * nOutStride) + m_nOutHeight * 2 - 16);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight; i+=2)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 1) * nInStride);

		pDestOut = pDestStart - i * 2;

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;

			*pDestOut = (nPix2 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			*(pDestOut + 1) = *pDestOut;
			*(pDestOut + 2) = (nPix1 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			*(pDestOut + 3) = *(pDestOut + 2);
			pDestOut -= nOutStride;

			*pDestOut = ((nPix2 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			*(pDestOut + 1) = *pDestOut;
			*(pDestOut + 2) = ((nPix1 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			*(pDestOut + 3) = *(pDestOut + 2);
			pDestOut -= nOutStride;
		}
	}

	for (int nHeight = 0; nHeight < m_nInWidth; nHeight++)
	{
		memcpy (pDest + ((m_nOutWidth - nHeight * 4 - 2) * nOutStride), pDest + ((m_nOutWidth - nHeight * 4 - 1) * nOutStride), m_nOutHeight * 2);
		memcpy (pDest + ((m_nOutWidth - nHeight * 4 - 3) * nOutStride), pDest + ((m_nOutWidth - nHeight * 4 - 1) * nOutStride), m_nOutHeight * 2);
		memcpy (pDest + ((m_nOutWidth - nHeight * 4 - 4) * nOutStride), pDest + ((m_nOutWidth - nHeight * 4 - 1) * nOutStride), m_nOutHeight * 2);
	}

	return true;

	return true;
}

bool CRGB565RR::ResizeAll_R90 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	VO_PBYTE  pSourBuf1 = NULL;
	VO_PBYTE  pSourBuf2 = NULL;

	unsigned int * pDestStart	= (unsigned int * )(pDest + ((m_nOutWidth - 1) * nOutStride) + m_nOutHeight * 2 - 4);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nOutHeight; i+=2)
	{
		pSourBuf1 = pSource + m_pYTable[i] * nInStride;
		pSourBuf2 = pSource + m_pYTable[i+1] * nInStride;

		pDestOut = pDestStart - (i >> 1);

		for (int j = 0; j < m_nOutWidth; j+=2)
		{
			nPix1 = *(pSourBuf1 + m_pXTable[j]);
			nPix1 += (*(pSourBuf1 + m_pXTable[j] + 1)) << 8;
			nPix1 += (*(pSourBuf1 + m_pXTable[j + 1])) << 16;
			nPix1 += (*(pSourBuf1 + m_pXTable[j + 1] + 1)) << 24;

			nPix2 = *(pSourBuf2 + m_pXTable[j]);
			nPix2 += (*(pSourBuf2 + m_pXTable[j] + 1)) << 8;
			nPix2 += (*(pSourBuf2 + m_pXTable[j + 1])) << 16;
			nPix2 += (*(pSourBuf2 + m_pXTable[j + 1] + 1)) << 24;

			*pDestOut = (nPix2 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			pDestOut -= nOutStride / 4;

			*pDestOut = ((nPix2 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			pDestOut -= nOutStride / 4;
		}
	}

	return true;
}


bool CRGB565RR::Resize10_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight; i+=2)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 1) * nInStride);

		pDestOut = pDestStart + (i >> 1);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;

			*pDestOut = (nPix1 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			pDestOut += nOutStride / 4;

			*pDestOut = ((nPix1 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			pDestOut += nOutStride / 4;
		}
	}

	return true;
}

bool CRGB565RR::Resize025_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight - 7; i+=8)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 4) * nInStride);

		pDestOut = pDestStart + (i >> 3);

		for (int j = 0; j < m_nInWidth - 7; j+=8)
		{
			nPix1 = (*pSourBuf1 & 0XFFFF) + ((*(pSourBuf1 + 2) & 0XFFFF) << 16);
			nPix2 = (*pSourBuf2 & 0XFFFF) + ((*(pSourBuf2 + 2) & 0XFFFF) << 16);

			*pDestOut = (nPix1 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			pDestOut += nOutStride / 4;

			*pDestOut = ((nPix1 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			pDestOut += nOutStride / 4;

			pSourBuf1 += 4;
			pSourBuf2 += 4;
		}
	}

	return true;
}

bool CRGB565RR::Resize05_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight - 3; i+=4)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 2) * nInStride);

		pDestOut = pDestStart + (i >> 2);

		for (int j = 0; j < m_nInWidth - 3; j+=4)
		{
			nPix1 = (*pSourBuf1 & 0XFFFF) + ((*(pSourBuf1 + 1) & 0XFFFF) << 16);
			nPix2 = (*pSourBuf2 & 0XFFFF) + ((*(pSourBuf2 + 1) & 0XFFFF) << 16);

			*pDestOut = (nPix1 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			pDestOut += nOutStride / 4;

			*pDestOut = ((nPix1 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			pDestOut += nOutStride / 4;

			pSourBuf1 += 2;
			pSourBuf2 += 2;
		}
	}

	return true;
}

bool CRGB565RR::Resize20_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight; i+=2)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 1) * nInStride);

		pDestOut = pDestStart + i;

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;

			*pDestOut = (nPix1 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			*(pDestOut + 1) = (nPix2 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			pDestOut += nOutStride / 2;

			*pDestOut = ((nPix1 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			*(pDestOut + 1) = ((nPix2 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			pDestOut += nOutStride / 2;
		}
	}

	for (int nHeight = 0; nHeight < m_nInWidth; nHeight++)
		memcpy (pDest + ((m_nOutWidth - nHeight * 2 - 1) * nOutStride), pDest + ((m_nOutWidth - nHeight * 2 - 2) * nOutStride), m_nOutHeight * 2);

	return true;
}

bool CRGB565RR::Resize30_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight; i+=2)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 1) * nInStride);

		pDestOut = pDestStart + (i * 3 / 2);

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;

			*pDestOut = (nPix1 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			*(pDestOut + 1) = (nPix1 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			*(pDestOut + 2) = (nPix2 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			pDestOut += nOutStride * 3 / 4;

			*pDestOut = ((nPix1 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			*(pDestOut + 1) = ((nPix1 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			*(pDestOut + 2) = ((nPix2 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			pDestOut += nOutStride * 3 / 4;
		}
	}

	for (int nHeight = 0; nHeight < m_nInWidth; nHeight++)
	{
		memcpy (pDest + ((m_nOutWidth - nHeight * 3 - 1) * nOutStride), pDest + ((m_nOutWidth - nHeight * 3 - 3) * nOutStride), m_nOutHeight * 2);
		memcpy (pDest + ((m_nOutWidth - nHeight * 3 - 2) * nOutStride), pDest + ((m_nOutWidth - nHeight * 3 - 3) * nOutStride), m_nOutHeight * 2);
	}

	return true;
}

bool CRGB565RR::Resize40_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	unsigned int * pSourBuf1 = (unsigned int *)pSource;
	unsigned int * pSourBuf2 = (unsigned int *)pSource;

	unsigned int * pDestStart	= (unsigned int * )(pDest);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nInHeight; i+=2)
	{
		pSourBuf1 = (unsigned int *)(pSource + i * nInStride);
		pSourBuf2 = (unsigned int *)(pSource + (i + 1) * nInStride);

		pDestOut = pDestStart + i * 2;

		for (int j = 0; j < m_nInWidth; j+=2)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;

			*pDestOut = (nPix1 & 0XFFFF) + ((nPix1 & 0XFFFF) << 16);
			*(pDestOut + 1) = *pDestOut;
			*(pDestOut + 2) = (nPix2 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			*(pDestOut + 3) = *(pDestOut + 2);
			pDestOut += nOutStride;

			*pDestOut = ((nPix1 & 0XFFFF0000) >> 16) + (nPix1 & 0XFFFF0000);
			*(pDestOut + 1) = *pDestOut;
			*(pDestOut + 2) = ((nPix2 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			*(pDestOut + 3) = *(pDestOut + 2);
			pDestOut += nOutStride;
		}
	}

	for (int nHeight = 0; nHeight < m_nInWidth; nHeight++)
	{
		memcpy (pDest + ((m_nOutWidth - nHeight * 4 - 1) * nOutStride), pDest + ((m_nOutWidth - nHeight * 4 - 4) * nOutStride), m_nOutHeight * 2);
		memcpy (pDest + ((m_nOutWidth - nHeight * 4 - 2) * nOutStride), pDest + ((m_nOutWidth - nHeight * 4 - 4) * nOutStride), m_nOutHeight * 2);
		memcpy (pDest + ((m_nOutWidth - nHeight * 4 - 3) * nOutStride), pDest + ((m_nOutWidth - nHeight * 4 - 4) * nOutStride), m_nOutHeight * 2);
	}

	return true;

	return true;
}

bool CRGB565RR::ResizeAll_R270 (VO_PBYTE pSource, int nInStride, VO_PBYTE pDest, int nOutStride)
{
	VO_PBYTE  pSourBuf1 = NULL;
	VO_PBYTE  pSourBuf2 = NULL;

	unsigned int * pDestStart	= (unsigned int * )(pDest);
	unsigned int * pDestOut	=  pDestStart;

	unsigned int nPix1 = 0;
	unsigned int nPix2 = 0;

	for (int i = 0; i < m_nOutHeight; i+=2)
	{
		pSourBuf1 = pSource + m_pYTable[i] * nInStride;
		pSourBuf2 = pSource + m_pYTable[i+1] * nInStride;

		pDestOut = pDestStart + (i >> 1);

		for (int j = 0; j < m_nOutWidth; j+=2)
		{
			nPix1 = *(pSourBuf1 + m_pXTable[j]);
			nPix1 += (*(pSourBuf1 + m_pXTable[j] + 1)) << 8;
			nPix1 += (*(pSourBuf1 + m_pXTable[j + 1])) << 16;
			nPix1 += (*(pSourBuf1 + m_pXTable[j + 1] + 1)) << 24;

			nPix2 = *(pSourBuf2 + m_pXTable[j]);
			nPix2 += (*(pSourBuf2 + m_pXTable[j] + 1)) << 8;
			nPix2 += (*(pSourBuf2 + m_pXTable[j + 1])) << 16;
			nPix2 += (*(pSourBuf2 + m_pXTable[j + 1] + 1)) << 24;

			*pDestOut = (nPix1 & 0XFFFF) + ((nPix2 & 0XFFFF) << 16);
			pDestOut += nOutStride / 4;

			*pDestOut = ((nPix1 & 0XFFFF0000) >> 16) + (nPix2 & 0XFFFF0000);
			pDestOut += nOutStride / 4;
		}
	}

	return true;
}

bool CRGB565RR::BuildTable (void)
{
	if (m_pXTable != NULL)
	{
		delete []m_pXTable;
		delete []m_pYTable;
	}
	m_nXSize = 0;

	int nWidth;
	int nHeight;

	m_pYTable = new int[m_nOutHeight];
	if (m_pYTable == NULL)
		return false;

	double dScale = (double)m_nInHeight / m_nOutHeight;
	for (nHeight = 0; nHeight < m_nOutHeight; nHeight++)
		m_pYTable[nHeight] = (int)(nHeight * dScale);

	if ((m_nOutHeight * 2 < m_nInHeight) || (m_nAngle != 0))
	{
		m_pXTable = new int[m_nOutWidth];
		if (m_pXTable == NULL)
			return false;

		for (nWidth = 0; nWidth < m_nOutWidth; nWidth++)
			m_pXTable[nWidth] = (int)(nWidth * dScale) * 2;
		return true;
	}

	int nXPrev = 0;
	int nXPos = 0;
	dScale = (double)m_nInWidth / m_nOutWidth;
	for (nWidth = 1; nWidth < m_nOutWidth; nWidth++)
	{
		nXPos = (int)(nWidth * dScale);
		if (nXPos - nXPrev != 1)
			m_nXSize++;
		nXPrev = nXPos;
	}
	m_nXSize++;
	m_pXTable = new int[m_nXSize * 2];

	m_nXSize = 0;
	nXPrev = 0;
	int nCount = 1;
	int	nStart = 0;
	for (nWidth = 1; nWidth < m_nOutWidth; nWidth++)
	{
		nXPos = (int)(nWidth * dScale);
		if (nXPos - nXPrev != 1)
		{
			m_pXTable[m_nXSize * 2] = nCount * 2;
			m_pXTable[m_nXSize * 2 + 1] = nStart * 2;
			m_nXSize++;

			nCount = 1;
			nStart = - 1;
		}
		else
		{
			nCount++;
		}
		if (nStart < 0)
			nStart = nXPos;
		nXPrev = nXPos;
	}
	m_pXTable[m_nXSize * 2] = nCount * 2;
	m_pXTable[m_nXSize * 2 + 1] = nStart * 2;
	m_nXSize++;

	return true;
}

