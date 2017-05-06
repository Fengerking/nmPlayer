#include "420n422ToRgb16_mb.h"
#include "yuv2rgb16_mb.h"

void Yuv2yuv_R90_inter_c (VO_U8 * pInput, int nWidth, int nHeight, int nInStride, VO_U8 * pOutput, int nOutStride)
{
	unsigned int * pSourBuf1, * pSourBuf2, * pSourBuf3, * pSourBuf4;
	unsigned int nPix1, nPix2, nPix3, nPix4;
	int i, j;

	unsigned int * pDestStart	= (unsigned int * )(pOutput + nOutStride - 4);
	unsigned int * pDestOut	=  pDestStart;

	for (i = 0; i < nHeight; i+=4)
	{
		pSourBuf1 = (unsigned int *)(pInput + i * nInStride);
		pSourBuf2 = (unsigned int *)(pInput + (i + 1) * nInStride);
		pSourBuf3 = (unsigned int *)(pInput + (i + 2) * nInStride);
		pSourBuf4 = (unsigned int *)(pInput + (i + 3) * nInStride);

		pDestOut = pDestStart - (i >> 2);

		for (j = 0; j < nWidth; j+=4)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;
			nPix3 = *pSourBuf3++;
			nPix4 = *pSourBuf4++;

			*pDestOut = (nPix4 & 0XFF) + ((nPix3 & 0XFF) << 8) + ((nPix2 & 0XFF) << 16) + ((nPix1 & 0XFF) << 24);
			pDestOut += nOutStride / 4;

			*pDestOut = ((nPix4 & 0XFF00) >> 8) + ((nPix3 & 0XFF00)) + ((nPix2 & 0XFF00) << 8) + ((nPix1 & 0XFF00) << 16);
			pDestOut += nOutStride / 4;

			*pDestOut = ((nPix4 & 0XFF0000) >> 16) + ((nPix3 & 0XFF0000) >> 8) + ((nPix2 & 0XFF0000)) + ((nPix1 & 0XFF0000) << 8);
			pDestOut += nOutStride / 4;

			*pDestOut = ((nPix4 & 0XFF000000) >> 24) + ((nPix3 & 0XFF000000) >> 16) + ((nPix2 & 0XFF000000) >> 8) + ((nPix1 & 0XFF000000));
			pDestOut += nOutStride / 4;
		}
	}
}

void Yuv2yuv_L90_inter_c (VO_U8 * pInput, int nWidth, int nHeight, int nInStride, VO_U8 * pOutput, int nOutStride)
{
	unsigned int * pSourBuf1, * pSourBuf2, * pSourBuf3, * pSourBuf4;
	unsigned int nPix1, nPix2, nPix3, nPix4;
	int i, j;

	unsigned int * pDestStart	= (unsigned int * )(pOutput + (nWidth - 1) * nOutStride);
	unsigned int * pDestOut	=  pDestStart;

	for (i = 0; i < nHeight; i+=4)
	{
		pSourBuf1 = (unsigned int *)(pInput + i * nInStride);
		pSourBuf2 = (unsigned int *)(pInput + (i + 1) * nInStride);
		pSourBuf3 = (unsigned int *)(pInput + (i + 2) * nInStride);
		pSourBuf4 = (unsigned int *)(pInput + (i + 3) * nInStride);

		pDestOut = pDestStart + (i >> 2);

		for (j = 0; j < nWidth; j+=4)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;
			nPix3 = *pSourBuf3++;
			nPix4 = *pSourBuf4++;

			*pDestOut = (nPix1 & 0XFF) + ((nPix2 & 0XFF) << 8) + ((nPix3 & 0XFF) << 16) + ((nPix4 & 0XFF) << 24);
			pDestOut -= nOutStride / 4;

			*pDestOut = ((nPix1 & 0XFF00) >> 8) + ((nPix2 & 0XFF00)) + ((nPix3 & 0XFF00) << 8) + ((nPix4 & 0XFF00) << 16);
			pDestOut -= nOutStride / 4;

			*pDestOut = ((nPix1 & 0XFF0000) >> 16) + ((nPix2 & 0XFF0000) >> 8) + ((nPix3 & 0XFF0000)) + ((nPix4 & 0XFF0000) << 8);
			pDestOut -= nOutStride / 4;

			*pDestOut = ((nPix1 & 0XFF000000) >> 24) + ((nPix2 & 0XFF000000) >> 16) + ((nPix3 & 0XFF000000) >> 8) + ((nPix4 & 0XFF000000));
			pDestOut -= nOutStride / 4;
		}
	}
}

void Yuv2yuv_180_inter_c (VO_U8 * pInput, int nWidth, int nHeight, int nInStride, VO_U8 * pOutput, int nOutStride)
{
	int * pSourBuf = (int *)pInput;
	int * pDestBuf = (int *)pOutput;
	int temp;

	int nPix = 0;
	int i, j;

	for (i = 0; i < nHeight; i++)
	{
		pSourBuf = (int *)(pInput + i * nInStride + nWidth - 4);
		pDestBuf = (int *)(pOutput + (nHeight -i) * nOutStride);

		for (j = 0; j < nWidth; j+=4)
		{
			nPix = *pSourBuf--;
			*pDestBuf++ = (nPix & 0XFF) << 24 + (nPix & 0XFF00) << 8 + (nPix & 0XFF0000) >> 8 + (nPix & 0XFF000000) >> 24;
		}
	}
}

void Yuv2yuv_L90 (unsigned char *y, unsigned char *u, unsigned char *v,
							  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							  unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end
		Yuv2yuv_L90_inter (y, in_width, in_height, in_stride, out_buf, in_stride);
		Yuv2yuv_L90_inter (u, in_width/2, in_height/2, uin_stride, out_buf + in_stride*in_height, uin_stride);
		Yuv2yuv_L90_inter (v, in_width/2, in_height/2, vin_stride, out_buf + in_stride*in_height + uin_stride*in_height/2, vin_stride);

}
void Yuv2yuv_R90  (unsigned char *y, unsigned char *u, unsigned char *v,
							  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							  unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end
		Yuv2yuv_R90_inter (y, in_width, in_height, in_stride, out_buf, in_stride);
		Yuv2yuv_R90_inter (u, in_width/2, in_height/2, uin_stride, out_buf + in_stride*in_height, uin_stride);
		Yuv2yuv_R90_inter (v, in_width/2, in_height/2, vin_stride, out_buf + in_stride*in_height + uin_stride*in_height/2, vin_stride);
}
void Yuv2yuv_180  (unsigned char *y, unsigned char *u, unsigned char *v,
							  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							  unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end
		Yuv2yuv_180_inter (y, in_width, in_height, in_stride, out_buf, in_stride);
		Yuv2yuv_180_inter (u, in_width/2, in_height/2, in_stride/2, out_buf + in_width*in_height, in_stride/2);
		Yuv2yuv_180_inter (v, in_width/2, in_height/2, vin_stride, out_buf + in_stride*in_height + uin_stride*in_height/2, vin_stride);
}
