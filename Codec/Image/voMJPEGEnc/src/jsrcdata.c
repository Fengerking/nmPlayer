#include "jencoder.h"

#define NOSUPPORT4		1	

void get422pblockdata_asm(VO_S32,INT16 **,UINT8 *,UINT8 *,UINT8 *,UINT8 *,UINT8 *,UINT8 *,VO_S32,VO_S32);
void get420_data_asm(VO_S32,INT16 **,UINT8 *,UINT8 *,UINT8 *,UINT8 *,UINT8 *,UINT8 *,VO_S32,VO_S32);
void get422iYUYVblockdata_asm(VO_S32,INT16 **,UINT8 *);
void get422iYVYUblockdata_asm(VO_S32,INT16 **,UINT8 *);
void get422iUYVYblockdata_asm(VO_S32,INT16 **,UINT8 *);
void get422iVYUYblockdata_asm(VO_S32,INT16 **,UINT8 *);

INT32	get444pblockdata(VO_VIDEO_BUFFER*	indata, 
						 JPEG_MCU_BLOCK*		outblock,
						 PICTURE_ATT*			picture,
						 UINT32					xi,
						 UINT32					yi)
{
	INT32 i, j, k;
	INT32 blockwidth;
	INT32 blockheight;
	INT32 WB;
	INT32 HB;
	INT16 **block;
	INT16 *blockdata;
	UINT8 *Y;
	UINT8 *U;
	UINT8 *V;
	INT16 T128 = 128;

	block = outblock->WorkBlock;
	blockwidth  = xi * picture->MCU_width;
	blockheight = yi * picture->MCU_heigth;

	Y = indata->Buffer[0] + indata->Stride[0] *  blockheight + blockwidth;
	U = indata->Buffer[1] + indata->Stride[1] *  blockheight + blockwidth;
	V = indata->Buffer[2] + indata->Stride[2] *  blockheight + blockwidth;

	if((xi < picture->MCU_XNum - 1) || (yi < picture->MCU_YNum - 1))
	{
		for(i = 0; i < 8; i++)
		{
			k = i << 3;
			blockdata = block[0] + k;
			*blockdata++ = Y[0] - T128;
			*blockdata++ = Y[1] - T128;
			*blockdata++ = Y[2] - T128;
			*blockdata++ = Y[3] - T128;
			*blockdata++ = Y[4] - T128;
			*blockdata++ = Y[5] - T128;
			*blockdata++ = Y[6] - T128;
			*blockdata++ = Y[7] - T128;

			blockdata = block[1] + k;
			*blockdata++ = U[0] - T128;
			*blockdata++ = U[1] - T128;
			*blockdata++ = U[2] - T128;
			*blockdata++ = U[3] - T128;
			*blockdata++ = U[4] - T128;
			*blockdata++ = U[5] - T128;
			*blockdata++ = U[6] - T128;
			*blockdata++ = U[7] - T128;

			blockdata = block[2] + k;
			*blockdata++ = V[0] - T128;
			*blockdata++ = V[1] - T128;
			*blockdata++ = V[2] - T128;
			*blockdata++ = V[3] - T128;
			*blockdata++ = V[4] - T128;
			*blockdata++ = V[5] - T128;
			*blockdata++ = V[6] - T128;
			*blockdata++ = V[7] - T128;			

			Y += indata->Stride[0];
			U += indata->Stride[1];
			V += indata->Stride[2];
		}		
	}
	else	
	{
		WB = MIN(picture->width - blockwidth, 8);
		HB = MIN(picture->heigth - blockheight, 8);

		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);

		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				block[0][k] = Y[j] - T128;
				block[1][k] = U[j] - T128;
				block[2][k] = V[j] - T128;
				k++;
			}
			Y += indata->Stride[0];
			U += indata->Stride[1];
			V += indata->Stride[2];
		}			
		
	}

	return 0;
}

INT32	get444iblockdata(VO_VIDEO_BUFFER*	indata, 
						 JPEG_MCU_BLOCK*		outblock,
						 PICTURE_ATT*			picture,
						 UINT32					xi,
						 UINT32					yi)
						 
{
	register INT32 i, j, k;
	INT32 blockwidth;
	INT32 blockheight;
	INT32 WB;
	INT32 HB;
	INT16 **block;
	INT16 *blockdata;
	UINT8 *Y;
	UINT8 *U;
	UINT8 *V;
	INT16 T128 = 128;

	blockwidth  = xi * picture->MCU_width;
	blockheight = yi * picture->MCU_heigth;
	Y = indata->Buffer[0] + indata->Stride[0] *  blockheight + blockwidth * 3;
	block = outblock->WorkBlock;

	if((xi < picture->MCU_XNum - 1) || (yi < picture->MCU_YNum - 1))
	{
		for(i = 0; i < 8; i++)
		{
			k = i << 3;
			blockdata = block[0] + k;
			*blockdata++ = Y[ 0] - T128;
			*blockdata++ = Y[ 3] - T128;
			*blockdata++ = Y[ 6] - T128;
			*blockdata++ = Y[ 9] - T128;
			*blockdata++ = Y[12] - T128;
			*blockdata++ = Y[15] - T128;
			*blockdata++ = Y[18] - T128;
			*blockdata++ = Y[21] - T128;

			blockdata = block[1] + k;
			*blockdata++ = Y[ 1] - T128;
			*blockdata++ = Y[ 4] - T128;
			*blockdata++ = Y[ 7] - T128;
			*blockdata++ = Y[10] - T128;
			*blockdata++ = Y[13] - T128;
			*blockdata++ = Y[16] - T128;
			*blockdata++ = Y[19] - T128;
			*blockdata++ = Y[22] - T128;

			blockdata = block[2] + k;
			*blockdata++ = Y[ 2] - T128;
			*blockdata++ = Y[ 5] - T128;
			*blockdata++ = Y[ 8] - T128;
			*blockdata++ = Y[11] - T128;
			*blockdata++ = Y[14] - T128;
			*blockdata++ = Y[17] - T128;
			*blockdata++ = Y[20] - T128;
			*blockdata++ = Y[23] - T128;

			Y += indata->Stride[0];
		}
	}
	else
	{
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);

		WB = MIN(picture->width - blockwidth, 8);
		HB = MIN(picture->heigth - blockheight, 8);

		U = Y + 1;
		V = Y + 2;		

		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				block[0][k] = Y[j*3] - T128;
				block[1][k] = U[j*3] - T128;
				block[2][k] = V[j*3] - T128;
				k++;
			}

			Y += indata->Stride[0];
			U += indata->Stride[0];
			V += indata->Stride[0];
		}
	}

	return 0;
}

INT32	get422pblockdata(VO_VIDEO_BUFFER*	indata, 
						 JPEG_MCU_BLOCK*		outblock,
						 PICTURE_ATT*			picture,
						 UINT32					xi,
						 UINT32					yi)
{
	register INT32 i, j, k;
	INT32 blockwidthY,  blockwidthUV;
	INT32 blockheightY, blockheightUV;
	INT32 WB;
	INT32 HB;
	INT16 **block;
#if !(defined(VOARMV7) || defined(VOARMV6))	
	INT16 *blockdata;
#endif
	UINT8 *Y1;
	UINT8 *Y2;
#if (defined(VOARMV7) || defined(VOARMV6))	
	UINT8 *Y3 = NULL;
	UINT8 *Y4 = NULL;
#endif
	UINT8 *U;
	UINT8 *V;
	INT16 T128 = 128;

	blockwidthY   = xi * picture->MCU_width;
	blockheightY  = yi * picture->MCU_heigth;
	blockwidthUV  = xi << 3;
	blockheightUV = yi << 3;

	Y1 = indata->Buffer[0] + indata->Stride[0] * blockheightY  + blockwidthY;
	U  = indata->Buffer[1] + indata->Stride[1] * blockheightUV + blockwidthUV;
	V  = indata->Buffer[2] + indata->Stride[2] * blockheightUV + blockwidthUV;

	block = outblock->WorkBlock;
	if((xi < picture->MCU_XNum - 1) || (yi < picture->MCU_YNum - 1))
	{
#ifdef VOARMV7
		get422pblockdata_asm(indata->Stride[0],block,Y1,Y1 + 8,Y3,Y4,U,V,indata->Stride[1],indata->Stride[2]);
#else
	#ifdef VOARMV6
		get422pblockdata_asm(indata->Stride[0],block,Y1,Y1 + 8,Y3,Y4,U,V,indata->Stride[1],indata->Stride[2]);
	#else
		Y2 = Y1 + 8;
		for(i = 0; i < 8; i++)
		{
			k = i << 3;
			blockdata = block[0] + k;
			*blockdata++ = Y1[0] - T128;
			*blockdata++ = Y1[1] - T128;
			*blockdata++ = Y1[2] - T128;
			*blockdata++ = Y1[3] - T128;
			*blockdata++ = Y1[4] - T128;
			*blockdata++ = Y1[5] - T128;
			*blockdata++ = Y1[6] - T128;
			*blockdata++ = Y1[7] - T128;
			
			blockdata = block[1] + k;
			*blockdata++ = Y2[0] - T128;
			*blockdata++ = Y2[1] - T128;
			*blockdata++ = Y2[2] - T128;
			*blockdata++ = Y2[3] - T128;
			*blockdata++ = Y2[4] - T128;
			*blockdata++ = Y2[5] - T128;
			*blockdata++ = Y2[6] - T128;
			*blockdata++ = Y2[7] - T128;
			
			
			blockdata = block[2] + k;
			*blockdata++ = U[0] - T128;
			*blockdata++ = U[1] - T128;
			*blockdata++ = U[2] - T128;
			*blockdata++ = U[3] - T128;
			*blockdata++ = U[4] - T128;
			*blockdata++ = U[5] - T128;
			*blockdata++ = U[6] - T128;
			*blockdata++ = U[7] - T128;

			blockdata = block[3] + k;
			*blockdata++ = V[0] - T128;
			*blockdata++ = V[1] - T128;
			*blockdata++ = V[2] - T128;
			*blockdata++ = V[3] - T128;
			*blockdata++ = V[4] - T128;
			*blockdata++ = V[5] - T128;
			*blockdata++ = V[6] - T128;
			*blockdata++ = V[7] - T128;
			
			Y1 += indata->Stride[0];
			Y2 += indata->Stride[0];
			U += indata->Stride[1];
			V += indata->Stride[2];
		}
	#endif
#endif

	}
	else
	{
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);

		WB = MIN(picture->width - blockwidthY, 8);
		HB = MIN(picture->heigth - blockheightY, 8);
		
		Y2 = Y1;
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				block[0][k] = Y2[j] - T128;
				k++;
			}
			Y2 += indata->Stride[0];
		}
		
		WB = MIN(MAX(picture->width - blockwidthY - 8, 0), 8);
		Y2 = Y1;
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				block[1][k] = Y2[j + 8] - T128;
				k++;
			}
			Y2 += indata->Stride[0];
		}

		WB = MIN((picture->width - blockwidthY) >> 1, 8);
		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				block[2][k] = U[j] - T128;
				block[3][k] = V[j] - T128;
				k++;
			}

			U += indata->Stride[1];
			V += indata->Stride[2];
		}
		
	}

	return 0;
}

INT32	get422iYUYVblockdata(VO_VIDEO_BUFFER*	indata, 
							 JPEG_MCU_BLOCK*		outblock,
							 PICTURE_ATT*			picture,
							 UINT32					xi,
							 UINT32					yi)							 
{
	INT32 i, j, k;
	INT32 blockwidthY;
	INT32 blockheightY;	
	INT32 WB;
	INT32 HB;
	INT16 **block;
	INT16 *blockdata;
	UINT8 *Y1;
	UINT8 *Y2;
	INT16 T128 = 128;	

	blockwidthY   = xi * picture->MCU_width;
	blockheightY  = yi * picture->MCU_heigth;
	Y1 = indata->Buffer[0] + indata->Stride[0] * blockheightY + (blockwidthY << 1);
	
	block = outblock->WorkBlock;

	if((xi < picture->MCU_XNum - 1) || (yi < picture->MCU_YNum - 1))
	{

#ifdef VOARMV7
		get422iYUYVblockdata_asm(indata->Stride[0],block,Y1);
#else
	#ifdef VOARMV6
		get422iYUYVblockdata_asm(indata->Stride[0],block,Y1);
	#else
		Y2 = Y1;
		for(i = 0; i < 8; i++)
		{
			k = i << 3;

			blockdata = block[0] + k;
			*blockdata++ = Y2[ 0] - T128;
			*blockdata++ = Y2[ 2] - T128;
			*blockdata++ = Y2[ 4] - T128;
			*blockdata++ = Y2[ 6] - T128;
			*blockdata++ = Y2[ 8] - T128;
			*blockdata++ = Y2[10] - T128;
			*blockdata++ = Y2[12] - T128;
			*blockdata++ = Y2[14] - T128;

			blockdata = block[1] + k;
			*blockdata++ = Y2[16] - T128;
			*blockdata++ = Y2[18] - T128;
			*blockdata++ = Y2[20] - T128;
			*blockdata++ = Y2[22] - T128;
			*blockdata++ = Y2[24] - T128;
			*blockdata++ = Y2[26] - T128;
			*blockdata++ = Y2[28] - T128;
			*blockdata++ = Y2[30] - T128;

			blockdata = block[2] + k;
			*blockdata++ = Y2[ 1] - T128;
			*blockdata++ = Y2[ 5] - T128;
			*blockdata++ = Y2[ 9] - T128;
			*blockdata++ = Y2[13] - T128;
			*blockdata++ = Y2[17] - T128;
			*blockdata++ = Y2[21] - T128;
			*blockdata++ = Y2[25] - T128;
			*blockdata++ = Y2[29] - T128;

			blockdata = block[3] + k;
			*blockdata++ = Y2[ 3] - T128;
			*blockdata++ = Y2[ 7] - T128;
			*blockdata++ = Y2[11] - T128;
			*blockdata++ = Y2[15] - T128;
			*blockdata++ = Y2[19] - T128;
			*blockdata++ = Y2[23] - T128;
			*blockdata++ = Y2[27] - T128;
			*blockdata++ = Y2[31] - T128;		
			
			Y2 += indata->Stride[0];
		}
	#endif
#endif
	}
	else
	{
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);
		
		WB = MIN(picture->width  - blockwidthY, 8);
		HB = MIN(picture->heigth - blockheightY, 8);

		Y2 = Y1;
		blockdata = block[0];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				blockdata[k] = Y2[(j << 1)    ] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}

		WB = MIN(MAX(picture->width  - blockwidthY - 8, 0), 8);
		Y2 = Y1;
		blockdata = block[1];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				blockdata[k] = Y2[(j << 1) + 16] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}

		WB = MIN((picture->width  - blockwidthY) >> 1, 8);
		HB = MIN(picture->heigth - blockheightY, 8);
		Y2 = Y1;
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				block[2][k] = Y2[(j << 2) + 1] - T128;
				block[3][k] = Y2[(j << 2) + 3] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}
	}
	
	return 0;
}

INT32	get422iYVYUblockdata(VO_VIDEO_BUFFER*	indata, 
							 JPEG_MCU_BLOCK*		outblock,
							 PICTURE_ATT*			picture,
							 UINT32					xi,
							 UINT32					yi)
							 
{
	INT32 i, j, k;
	INT32 blockwidthY;
	INT32 blockheightY;	
	INT32 WB;
	INT32 HB;
	INT16 **block;
	INT16 *blockdata;
	UINT8 *Y1;
	UINT8 *Y2;
	INT16 T128 = 128;	

	blockwidthY   = xi * picture->MCU_width;
	blockheightY  = yi * picture->MCU_heigth;

	Y1 = indata->Buffer[0] + indata->Stride[0] * blockheightY  + (blockwidthY << 1);
	block = outblock->WorkBlock;

	if((xi < picture->MCU_XNum - 1) || (yi < picture->MCU_YNum - 1))
	{
		Y2 = Y1;
#ifdef VOARMV7
		get422iYVYUblockdata_asm(indata->Stride[0],block,Y1);
#else
	#ifdef VOARMV6
		get422iYVYUblockdata_asm(indata->Stride[0],block,Y1);
	#else
		for(i = 0; i < 8; i++)
		{
			k = i << 3;

			blockdata = block[0] + k;
			*blockdata++ = Y2[ 0] - T128;
			*blockdata++ = Y2[ 2] - T128;
			*blockdata++ = Y2[ 4] - T128;
			*blockdata++ = Y2[ 6] - T128;
			*blockdata++ = Y2[ 8] - T128;
			*blockdata++ = Y2[10] - T128;
			*blockdata++ = Y2[12] - T128;
			*blockdata++ = Y2[14] - T128;

			blockdata = block[1] + k;
			*blockdata++ = Y2[16] - T128;
			*blockdata++ = Y2[18] - T128;
			*blockdata++ = Y2[20] - T128;
			*blockdata++ = Y2[22] - T128;
			*blockdata++ = Y2[24] - T128;
			*blockdata++ = Y2[26] - T128;
			*blockdata++ = Y2[28] - T128;
			*blockdata++ = Y2[30] - T128;

			blockdata = block[3] + k;
			*blockdata++ = Y2[ 1] - T128;
			*blockdata++ = Y2[ 5] - T128;
			*blockdata++ = Y2[ 9] - T128;
			*blockdata++ = Y2[13] - T128;
			*blockdata++ = Y2[17] - T128;
			*blockdata++ = Y2[21] - T128;
			*blockdata++ = Y2[25] - T128;
			*blockdata++ = Y2[29] - T128;

			blockdata = block[2] + k;
			*blockdata++ = Y2[ 3] - T128;
			*blockdata++ = Y2[ 7] - T128;
			*blockdata++ = Y2[11] - T128;
			*blockdata++ = Y2[15] - T128;
			*blockdata++ = Y2[19] - T128;
			*blockdata++ = Y2[23] - T128;
			*blockdata++ = Y2[27] - T128;
			*blockdata++ = Y2[31] - T128;		
			
			Y2 += indata->Stride[0];
		}
#endif
#endif
	}
	else
	{
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);
		
		WB = MIN(picture->width  - blockwidthY, 8);
		HB = MIN(picture->heigth - blockheightY, 8);

		Y2 = Y1;
		blockdata = block[0];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				blockdata[k] = Y2[(j << 1)    ] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}

		WB = MIN(MAX(picture->width  - blockwidthY - 8, 0), 8);
		Y2 = Y1;
		blockdata = block[1];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				blockdata[k] = Y2[(j << 1) + 16] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}

		WB = MIN((picture->width  - blockwidthY) >> 1, 8);
		HB = MIN(picture->heigth - blockheightY, 8);
		Y2 = Y1;

		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				block[2][k] = Y2[(j << 2) + 3] - T128;
				block[3][k] = Y2[(j << 2) + 1] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}
	}
	
	return 0;
}

INT32	get422iUYVYblockdata(VO_VIDEO_BUFFER*	indata, 
							 JPEG_MCU_BLOCK*		outblock,
							 PICTURE_ATT*			picture,
							 UINT32					xi,
							 UINT32					yi)
							 
{
	INT32 i, j, k;
	INT32 blockwidthY;
	INT32 blockheightY;	
	INT32 WB;
	INT32 HB;
	INT16 **block;
	INT16 *blockdata;
	UINT8 *Y1;
	UINT8 *Y2;
	INT16 T128 = 128;
	
	blockwidthY   = xi * picture->MCU_width;
	blockheightY  = yi * picture->MCU_heigth;

	Y1 = indata->Buffer[0] + indata->Stride[0] * blockheightY + (blockwidthY << 1);
	block = outblock->WorkBlock;

	if((xi < picture->MCU_XNum - 1) || (yi < picture->MCU_YNum - 1))
	{
		
		Y2 = Y1;
#ifdef VOARMV7
		get422iUYVYblockdata_asm(indata->Stride[0],block,Y1);
#else
	#ifdef VOARMV6
		get422iUYVYblockdata_asm(indata->Stride[0],block,Y1);
	#else
		for(i = 0; i < 8; i++)
		{
			k = i << 3;
			blockdata = block[0] + k;
			*blockdata++ = Y2[ 1] - T128;
			*blockdata++ = Y2[ 3] - T128;
			*blockdata++ = Y2[ 5] - T128;
			*blockdata++ = Y2[ 7] - T128;
			*blockdata++ = Y2[ 9] - T128;
			*blockdata++ = Y2[11] - T128;
			*blockdata++ = Y2[13] - T128;
			*blockdata++ = Y2[15] - T128;

			blockdata = block[1] + k;
			*blockdata++ = Y2[17] - T128;
			*blockdata++ = Y2[19] - T128;
			*blockdata++ = Y2[21] - T128;
			*blockdata++ = Y2[23] - T128;
			*blockdata++ = Y2[25] - T128;
			*blockdata++ = Y2[27] - T128;
			*blockdata++ = Y2[29] - T128;
			*blockdata++ = Y2[31] - T128;

			blockdata = block[2] + k;
			*blockdata++ = Y2[ 0] - T128;
			*blockdata++ = Y2[ 4] - T128;
			*blockdata++ = Y2[ 8] - T128;
			*blockdata++ = Y2[12] - T128;
			*blockdata++ = Y2[16] - T128;
			*blockdata++ = Y2[20] - T128;
			*blockdata++ = Y2[24] - T128;
			*blockdata++ = Y2[28] - T128;

			blockdata = block[3] + k;
			*blockdata++ = Y2[ 2] - T128;
			*blockdata++ = Y2[ 6] - T128;
			*blockdata++ = Y2[10] - T128;
			*blockdata++ = Y2[14] - T128;
			*blockdata++ = Y2[18] - T128;
			*blockdata++ = Y2[22] - T128;
			*blockdata++ = Y2[26] - T128;
			*blockdata++ = Y2[30] - T128;		
			
			Y2 += indata->Stride[0];
		}
#endif
#endif
	}
	else
	{
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);
		
		WB = MIN(picture->width  - blockwidthY, 8);
		HB = MIN(picture->heigth - blockheightY, 8);

		Y2 = Y1;
		blockdata = block[0];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				blockdata[k] = Y2[(j << 1) + 1] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}

		WB = MIN(MAX(picture->width  - blockwidthY - 8, 0), 8);
		Y2 = Y1;
		blockdata = block[1];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				blockdata[k] = Y2[(j << 1) + 17] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}

		WB = MIN((picture->width  - blockwidthY) >> 1, 8);
		HB = MIN(picture->heigth - blockheightY, 8);
		Y2 = Y1;

		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				block[2][k] = Y2[(j << 2)    ] - T128;
				block[3][k] = Y2[(j << 2) + 2] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}

	}
	
	return 0;
}

INT32	get422iVYUYblockdata(VO_VIDEO_BUFFER*	indata, 
							 JPEG_MCU_BLOCK*		outblock,
							 PICTURE_ATT*			picture,
							 UINT32					xi,
							 UINT32					yi)
							 
{
	INT32 i, j, k;
	INT32 blockwidthY;
	INT32 blockheightY;	
	INT32 WB;
	INT32 HB;
	INT16 **block;
	INT16 *blockdata;
	UINT8 *Y1;
	UINT8 *Y2;
	INT16 T128 = 128;
	
	blockwidthY   = xi * picture->MCU_width;
	blockheightY  = yi * picture->MCU_heigth;

	Y1 = indata->Buffer[0] + indata->Stride[0] * blockheightY + (blockwidthY << 1);
	block = outblock->WorkBlock;

	if((xi < picture->MCU_XNum - 1) || (yi < picture->MCU_YNum - 1))
	{
#ifdef VOARMV7
		get422iVYUYblockdata_asm(indata->Stride[0],block,Y1);
#else	
	#ifdef VOARMV6
		get422iVYUYblockdata_asm(indata->Stride[0],block,Y1);
   #else
		Y2 = Y1;
		for(i = 0; i < 8; i++)
		{
			k = i << 3;

			blockdata = block[0] + k;
			*blockdata++ = Y2[ 1] - T128;
			*blockdata++ = Y2[ 3] - T128;
			*blockdata++ = Y2[ 5] - T128;
			*blockdata++ = Y2[ 7] - T128;
			*blockdata++ = Y2[ 9] - T128;
			*blockdata++ = Y2[11] - T128;
			*blockdata++ = Y2[13] - T128;
			*blockdata++ = Y2[15] - T128;

			blockdata = block[1] + k;
			*blockdata++ = Y2[17] - T128;
			*blockdata++ = Y2[19] - T128;
			*blockdata++ = Y2[21] - T128;
			*blockdata++ = Y2[23] - T128;
			*blockdata++ = Y2[25] - T128;
			*blockdata++ = Y2[27] - T128;
			*blockdata++ = Y2[29] - T128;
			*blockdata++ = Y2[31] - T128;

			blockdata = block[3] + k;
			*blockdata++ = Y2[ 0] - T128;
			*blockdata++ = Y2[ 4] - T128;
			*blockdata++ = Y2[ 8] - T128;
			*blockdata++ = Y2[12] - T128;
			*blockdata++ = Y2[16] - T128;
			*blockdata++ = Y2[20] - T128;
			*blockdata++ = Y2[24] - T128;
			*blockdata++ = Y2[28] - T128;

			blockdata = block[2] + k;
			*blockdata++ = Y2[ 2] - T128;
			*blockdata++ = Y2[ 6] - T128;
			*blockdata++ = Y2[10] - T128;
			*blockdata++ = Y2[14] - T128;
			*blockdata++ = Y2[18] - T128;
			*blockdata++ = Y2[22] - T128;
			*blockdata++ = Y2[26] - T128;
			*blockdata++ = Y2[30] - T128;		
			
			Y2 += indata->Stride[0];
		}
	#endif
#endif
	}
	else
	{
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);
		WB = MIN(picture->width  - blockwidthY, 8);
		HB = MIN(picture->heigth - blockheightY, 8);

		Y2 = Y1;
		blockdata = block[0];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				blockdata[k] = Y2[(j << 1) + 1] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}

		WB = MIN(MAX(picture->width  - blockwidthY - 8, 0), 8);
		Y2 = Y1;
		blockdata = block[1];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				blockdata[k] = Y2[(j << 1) + 17] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}

		WB = MIN((picture->width  - blockwidthY) >> 1, 8);
		HB = MIN(picture->heigth - blockheightY, 8);
		Y2 = Y1;

		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				block[2][k] = Y2[(j << 2) + 2] - T128;
				block[3][k] = Y2[(j << 2)    ] - T128;
				k++;
			}
			
			Y2 += indata->Stride[0];
		}

	}
	
	return 0;
}

INT32	get420pblockdata(VO_VIDEO_BUFFER*	indata, 
						 JPEG_MCU_BLOCK*		outblock,
						 PICTURE_ATT*			picture,
						 UINT32					xi,
						 UINT32					yi)
{
	register INT32 i, j, k;
	INT32 blockwidthY,  blockwidthUV;
	INT32 blockheightY, blockheightUV;
	INT32 WB;
	INT32 HB;
	INT16 **block;
	INT16 *blockdata;
	UINT8 *Y1;
	UINT8 *Y2;
	UINT8 *Y3;
	UINT8 *Y4;
	UINT8 *U =NULL;
	UINT8 *V = NULL;
	INT16 T128 = 128;

	blockwidthY   = xi * picture->MCU_width;
	blockheightY  = yi * picture->MCU_heigth;
	blockwidthUV   = xi << 3;
	blockheightUV  = yi << 3;
	
	block = outblock->WorkBlock;
	Y1 = indata->Buffer[0] + indata->Stride[0] * blockheightY + blockwidthY;
	Y2 = Y1 + 8;
	Y3 = Y1 + (indata->Stride[0] << 3);
	Y4 = Y3 + 8;
	
	//if(indata->ColorType == VOYUV_PLANAR420)  
	if(indata->ColorType == VO_COLOR_YUV_PLANAR420) 
	{
		U = indata->Buffer[1] + indata->Stride[1] * blockheightUV + blockwidthUV;
		V = indata->Buffer[2] + indata->Stride[2] * blockheightUV + blockwidthUV;
	}
	else if(indata->ColorType == VO_COLOR_YVU_PLANAR420)//VOYVU_PLANAR420)
	{
		V = indata->Buffer[1] + indata->Stride[1] * blockheightUV + blockwidthUV;
		U = indata->Buffer[2] + indata->Stride[2] * blockheightUV + blockwidthUV;
	}
	
	if((xi < picture->MCU_XNum - 1) || (yi < picture->MCU_YNum - 1))
	{
#ifdef VOARMV7
		get420_data_asm(indata->Stride[0],block,Y1,Y2,Y3,Y4,U,V,indata->Stride[1],indata->Stride[2]);
#else
	#ifdef VOARMV6
		get420_data_asm(indata->Stride[0],block,Y1,Y2,Y3,Y4,U,V,indata->Stride[1],indata->Stride[2]);
	#else
		for(i = 0; i < 8; i++)
		{
			k = i << 3;
			
			blockdata = block[0] + k;
			*blockdata++ = Y1[0] - T128;
			*blockdata++ = Y1[1] - T128;
			*blockdata++ = Y1[2] - T128;
			*blockdata++ = Y1[3] - T128;
			*blockdata++ = Y1[4] - T128;
			*blockdata++ = Y1[5] - T128;
			*blockdata++ = Y1[6] - T128;
			*blockdata++ = Y1[7] - T128;
			
			blockdata = block[1] + k;
			*blockdata++ = Y2[0] - T128;
			*blockdata++ = Y2[1] - T128;
			*blockdata++ = Y2[2] - T128;
			*blockdata++ = Y2[3] - T128;
			*blockdata++ = Y2[4] - T128;
			*blockdata++ = Y2[5] - T128;
			*blockdata++ = Y2[6] - T128;
			*blockdata++ = Y2[7] - T128;
			
			blockdata = block[2] + k;
			*blockdata++ = Y3[0] - T128;
			*blockdata++ = Y3[1] - T128;
			*blockdata++ = Y3[2] - T128;
			*blockdata++ = Y3[3] - T128;
			*blockdata++ = Y3[4] - T128;
			*blockdata++ = Y3[5] - T128;
			*blockdata++ = Y3[6] - T128;
			*blockdata++ = Y3[7] - T128;
			
			blockdata = block[3] + k;
			*blockdata++ = Y4[0] - T128;
			*blockdata++ = Y4[1] - T128;
			*blockdata++ = Y4[2] - T128;
			*blockdata++ = Y4[3] - T128;
			*blockdata++ = Y4[4] - T128;
			*blockdata++ = Y4[5] - T128;
			*blockdata++ = Y4[6] - T128;
			*blockdata++ = Y4[7] - T128;
			
			blockdata = block[4] + k;
			*blockdata++ = U[0] - T128;
			*blockdata++ = U[1] - T128;
			*blockdata++ = U[2] - T128;
			*blockdata++ = U[3] - T128;
			*blockdata++ = U[4] - T128;
			*blockdata++ = U[5] - T128;
			*blockdata++ = U[6] - T128;
			*blockdata++ = U[7] - T128;
			
			blockdata = block[5] + k;
			*blockdata++ = V[0] - T128;
			*blockdata++ = V[1] - T128;
			*blockdata++ = V[2] - T128;
			*blockdata++ = V[3] - T128;
			*blockdata++ = V[4] - T128;
			*blockdata++ = V[5] - T128;
			*blockdata++ = V[6] - T128;
			*blockdata++ = V[7] - T128;
			
			Y1 += indata->Stride[0];
			Y2 += indata->Stride[0];
			Y3 += indata->Stride[0];
			Y4 += indata->Stride[0];
			U  += indata->Stride[1];
			V  += indata->Stride[2];
		}
	#endif
#endif
	}
	else
	{	
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);
		
		WB = MIN(picture->width  - blockwidthY , 8);
		HB = MIN(picture->heigth - blockheightY, 8);
		blockdata = block[0];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				blockdata[k] = Y1[j] - T128;
				k++;
			}
			Y1  += indata->Stride[0];
		}

		WB = MIN(MAX(picture->width  - blockwidthY - 8, 0), 8);
		blockdata = block[1];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				blockdata[k] = Y2[j] - T128;
				k++;
			}
			Y2  += indata->Stride[0];
		}
		
		WB = MIN(picture->width  - blockwidthY , 8);
		HB = MIN(MAX(picture->heigth - blockheightY - 8, 0), 8);
		blockdata = block[2];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				blockdata[k] = Y3[j] - T128;
				k++;
			}
			Y3  += indata->Stride[0];
		}

		WB = MIN(MAX(picture->width  - blockwidthY - 8, 0), 8);
		blockdata = block[3];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				blockdata[k] = Y4[j] - T128;
				k++;
			}
			Y4  += indata->Stride[0];
		}

		WB = MIN((picture->width  - blockwidthY) >> 1 , 8);
		HB = MIN((picture->heigth - blockheightY) >> 1, 8);		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				block[4][k] = U [j] - T128;
				block[5][k] = V [j] - T128;
				k++;
			}
			U  += indata->Stride[1];
			V  += indata->Stride[2];
		}		
	}

	return 0;
}

 INT32	get420iblockdata(VO_VIDEO_BUFFER*	indata, 
						 JPEG_MCU_BLOCK*		outblock,
						 PICTURE_ATT*			picture,
						 UINT32					xi,
						 UINT32					yi)
 {
	register INT32 i, j, k;
	INT32 blockwidthY,  blockwidthUV;
	INT32 blockheightY, blockheightUV;
	INT32 WB;
	INT32 HB;
	INT16 **block;
	INT16 *blockdata;
	INT16 *blockdata0 = NULL;
	INT16 *blockdata1 = NULL;
	UINT8 *Y1;
	UINT8 *Y2;
	UINT8 *Y3;
	UINT8 *Y4;
	UINT8 *UV;
	INT16 T128 = 128;

	blockwidthY   = xi * picture->MCU_width;
	blockheightY  = yi * picture->MCU_heigth;
	blockwidthUV   = (xi << 3) << 1;
	blockheightUV  = yi << 3;
	
	block = outblock->WorkBlock;
	Y1 = indata->Buffer[0] + indata->Stride[0] * blockheightY + blockwidthY;
	Y2 = Y1 + 8;
	Y3 = Y1 + (indata->Stride[0] << 3);
	Y4 = Y3 + 8;

	UV = indata->Buffer[1] + indata->Stride[1] * blockheightUV + blockwidthUV;

	if(indata->ColorType == VO_COLOR_YUV_420_PACK)//VOYUV_420_PACK)
	{
		blockdata0 = block[4];
		blockdata1 = block[5];
	}
	else if(indata->ColorType == VO_COLOR_YUV_420_PACK_2)//VOYUV_420_PACK_2)
	{
		blockdata0 = block[5];
		blockdata1 = block[4];
	}
	
	if((xi < picture->MCU_XNum - 1) || (yi < picture->MCU_YNum - 1))
	{
		for(i = 0; i < 8; i++)
		{
			k = i << 3;
			
			blockdata = block[0] + k;
			*blockdata++ = Y1[0] - T128;
			*blockdata++ = Y1[1] - T128;
			*blockdata++ = Y1[2] - T128;
			*blockdata++ = Y1[3] - T128;
			*blockdata++ = Y1[4] - T128;
			*blockdata++ = Y1[5] - T128;
			*blockdata++ = Y1[6] - T128;
			*blockdata++ = Y1[7] - T128;
			
			blockdata = block[1] + k;
			*blockdata++ = Y2[0] - T128;
			*blockdata++ = Y2[1] - T128;
			*blockdata++ = Y2[2] - T128;
			*blockdata++ = Y2[3] - T128;
			*blockdata++ = Y2[4] - T128;
			*blockdata++ = Y2[5] - T128;
			*blockdata++ = Y2[6] - T128;
			*blockdata++ = Y2[7] - T128;
			
			blockdata = block[2] + k;
			*blockdata++ = Y3[0] - T128;
			*blockdata++ = Y3[1] - T128;
			*blockdata++ = Y3[2] - T128;
			*blockdata++ = Y3[3] - T128;
			*blockdata++ = Y3[4] - T128;
			*blockdata++ = Y3[5] - T128;
			*blockdata++ = Y3[6] - T128;
			*blockdata++ = Y3[7] - T128;
			
			blockdata = block[3] + k;
			*blockdata++ = Y4[0] - T128;
			*blockdata++ = Y4[1] - T128;
			*blockdata++ = Y4[2] - T128;
			*blockdata++ = Y4[3] - T128;
			*blockdata++ = Y4[4] - T128;
			*blockdata++ = Y4[5] - T128;
			*blockdata++ = Y4[6] - T128;
			*blockdata++ = Y4[7] - T128;
			
			blockdata = blockdata0 + k;
			*blockdata++ = UV[0] - T128;
			*blockdata++ = UV[2] - T128;
			*blockdata++ = UV[4] - T128;
			*blockdata++ = UV[6] - T128;
			*blockdata++ = UV[8] - T128;
			*blockdata++ = UV[10] - T128;
			*blockdata++ = UV[12] - T128;
			*blockdata++ = UV[14] - T128;
			
			blockdata = blockdata1 + k;
			*blockdata++ = UV[1] - T128;
			*blockdata++ = UV[3] - T128;
			*blockdata++ = UV[5] - T128;
			*blockdata++ = UV[7] - T128;
			*blockdata++ = UV[9] - T128;
			*blockdata++ = UV[11] - T128;
			*blockdata++ = UV[13] - T128;
			*blockdata++ = UV[15] - T128;
			
			Y1 += indata->Stride[0];
			Y2 += indata->Stride[0];
			Y3 += indata->Stride[0];
			Y4 += indata->Stride[0];
			UV += indata->Stride[1];
		}
	}
	else
	{	
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);
		
		WB = MIN(picture->width  - blockwidthY , 8);
		HB = MIN(picture->heigth - blockheightY, 8);
		blockdata = block[0];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				blockdata[k] = Y1[j] - T128;
				k++;
			}
			Y1  += indata->Stride[0];
		}

		WB = MIN(MAX(picture->width  - blockwidthY - 8, 0), 8);
		blockdata = block[1];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				blockdata[k] = Y2[j] - T128;
				k++;
			}
			Y2  += indata->Stride[0];
		}
		
		WB = MIN(picture->width  - blockwidthY , 8);
		HB = MIN(MAX(picture->heigth - blockheightY - 8, 0), 8);
		blockdata = block[2];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				blockdata[k] = Y3[j] - T128;
				k++;
			}
			Y3  += indata->Stride[0];
		}

		WB = MIN(MAX(picture->width  - blockwidthY - 8, 0), 8);
		blockdata = block[3];
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j++)
			{
				blockdata[k] = Y4[j] - T128;
				k++;
			}
			Y4  += indata->Stride[0];
		}

		WB = MIN(picture->width  - blockwidthUV , 16);
		HB = MIN((picture->heigth - blockheightY) >> 1, 8);		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j+=2)
			{
				blockdata0[k] = UV[j] - T128;
				blockdata1[k] = UV[j+1] - T128;
				k++;
			}
			UV  += indata->Stride[1];
		}		
	}

	return 0;
 }



INT32	getRGB888pblockdata(VO_VIDEO_BUFFER*		indata, 
						    JPEG_MCU_BLOCK*			outblock,
							PICTURE_ATT*			picture,
							UINT32					xi,
							UINT32					yi)
{
	register INT32 i, j, k;
	INT32 r, g, b, y, k2;
	INT32 blockwidthRGB;
	INT32 blockstep[3];
	INT32 blockheightRGB;
	INT32 WB;
	INT32 HB;
	INT16 **block;
	UINT8 *buf_R, *buf_G, *buf_B;
	UINT8 *buf_R1, *buf_G1, *buf_B1;

	blockwidthRGB   = xi * picture->MCU_width;
	blockheightRGB  = yi * picture->MCU_heigth;
	block = outblock->WorkBlock;

	buf_R = indata->Buffer[0] + indata->Stride[0] * blockheightRGB + blockwidthRGB;
	buf_G = indata->Buffer[1] + indata->Stride[1] * blockheightRGB + blockwidthRGB;
	buf_B = indata->Buffer[2] + indata->Stride[2] * blockheightRGB + blockwidthRGB;

	if(xi < picture->MCU_XNum - 1 && yi < picture->MCU_YNum - 1)
	{
		buf_R1 = buf_R + indata->Stride[0];
		buf_G1 = buf_G + indata->Stride[1];
		buf_B1 = buf_B + indata->Stride[2];
		blockstep[0] = (indata->Stride[0] << 1) - 16;
		blockstep[1] = (indata->Stride[1] << 1) - 16;
		blockstep[2] = (indata->Stride[2] << 1) - 16;
		
		for(i = 0; i < 4; i++)
		{
			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				r = *buf_R++;
				g = *buf_G++;
				b = *buf_B++;
				
				k2 = k << 1;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k ] = (INT16)(((b - y)*0x48)>>7);
				block[5][k ] = (INT16)(((r - y)*0x5b)>>7);
				block[0][k2] = (INT16)(y - 128);
				
				r = *buf_R++;
				g = *buf_G++;
				b = *buf_B++;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[0][k2 + 1] = (INT16)(y - 128);
				
				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[0][k2 + 8] = (INT16)(y - 128);	
				
				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[0][k2 + 9] = (INT16)(y - 128);
				
				k++;
			}

			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				k2 = k << 1;

				r = *buf_R++;
				g = *buf_G++;
				b = *buf_B++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k + 4] = (INT16)(((b - y)*0x48)>>7);
				block[5][k + 4] = (INT16)(((r - y)*0x5b)>>7);
				block[1][k2] = (INT16)(y - 128);
				
				r = *buf_R++;
				g = *buf_G++;
				b = *buf_B++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[1][k2 + 1] = (INT16)(y - 128);
				
				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[1][k2 + 8] = (INT16)(y - 128);	
				
				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[1][k2 + 9] = (INT16)(y - 128);
				
				k++;
			}

			buf_R  += blockstep[0];
			buf_G  += blockstep[1];
			buf_B  += blockstep[2];

			buf_R1 += blockstep[0];
			buf_G1 += blockstep[1];
			buf_B1 += blockstep[2];
		}
		
		for(i = 0; i < 4; i++)
		{
			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				k2 = k << 1;

				r = *buf_R++;
				g = *buf_G++;
				b = *buf_B++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k + 32] = (INT16)(((b - y)*0x48)>>7);
				block[5][k + 32] = (INT16)(((r - y)*0x5b)>>7);
				block[2][k2] = (INT16)(y - 128);
				
				r = *buf_R++;
				g = *buf_G++;
				b = *buf_B++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[2][k2 + 1] = (INT16)(y - 128);
				
				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[2][k2 + 8] = (INT16)(y - 128);	
				
				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[2][k2 + 9] = (INT16)(y - 128);
				
				k++;
			}

			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				r = *buf_R++;
				g = *buf_G++;
				b = *buf_B++;				
				k2 = k << 1;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k + 36] = (INT16)(((b - y)*0x48)>>7);
				block[5][k + 36] = (INT16)(((r - y)*0x5b)>>7);
				block[3][k2] = (INT16)(y - 128);
				
				r = *buf_R++;
				g = *buf_G++;
				b = *buf_B++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[3][k2 + 1] = (INT16)(y - 128);
				
				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[3][k2 + 8] = (INT16)(y - 128);	
				
				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[3][k2 + 9] = (INT16)(y - 128);
				
				k++;
			}

			buf_R  += blockstep[0];
			buf_G  += blockstep[1];
			buf_B  += blockstep[2];

			buf_R1 += blockstep[0];
			buf_G1 += blockstep[1];
			buf_B1 += blockstep[2];
		}
	}
	else
	{
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);
		
		WB = MIN(picture->width  - blockwidthRGB , 8);
		HB = MIN(picture->heigth - blockheightRGB, 8);
		blockstep[0] = indata->Stride[0] - WB;
		blockstep[1] = indata->Stride[1] - WB;
		blockstep[2] = indata->Stride[2] - WB;
		
		buf_R1 = buf_R;
		buf_G1 = buf_G;
		buf_B1 = buf_B;
		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				k2 = k >> 1;

				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i & 1)) && (!(j & 1)))
				{
					block[4][k2] = (INT16)(((b - y)*0x48)>>7);
					block[5][k2] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[0][k] = (INT16)(y - 128);
				
				k++;
			}
			buf_R1 += blockstep[0];
			buf_G1 += blockstep[1];
			buf_B1 += blockstep[2];
		}

		WB = MIN(MAX(picture->width  - blockwidthRGB - 8, 0) , 8);
		blockstep[0] = indata->Stride[0] - WB;
		blockstep[1] = indata->Stride[1] - WB;
		blockstep[2] = indata->Stride[2] - WB;
		buf_R1 = buf_R + 8;
		buf_G1 = buf_G + 8;
		buf_B1 = buf_B + 8;
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				k2 = k >> 1;

				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i & 1)) && (!(j & 1)))
				{
					block[4][k2 + 4] = (INT16)(((b - y)*0x48)>>7);
					block[5][k2 + 4] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[1][k] = (INT16)(y - 128);
				
				k++;
			}
			buf_R1 += blockstep[0];
			buf_G1 += blockstep[1];
			buf_B1 += blockstep[2];
		}

		WB = MIN(picture->width  - blockwidthRGB , 8);
		HB = MIN(MAX(picture->heigth - blockheightRGB - 8, 0), 8);
		blockstep[0] = indata->Stride[0] - WB;
		blockstep[1] = indata->Stride[1] - WB;
		blockstep[2] = indata->Stride[2] - WB;
		
		buf_R1 = buf_R + (indata->Stride[0] << 3);
		buf_G1 = buf_G + (indata->Stride[1] << 3);
		buf_B1 = buf_B + (indata->Stride[2] << 3);		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				k2 = k >> 1;

				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i & 1)) && (!(j & 1)))
				{
					block[4][k2 + 32] = (INT16)(((b - y)*0x48)>>7);
					block[5][k2 + 32] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[2][k] = (INT16)(y - 128);
				
				k++;
			}
			buf_R1 += blockstep[0];
			buf_G1 += blockstep[1];
			buf_B1 += blockstep[2];
		}

		WB = MIN(MAX(picture->width  - blockwidthRGB - 8, 0) , 8);
		blockstep[0] = indata->Stride[0] - WB;
		blockstep[1] = indata->Stride[1] - WB;
		blockstep[2] = indata->Stride[2] - WB;
		buf_R1 = buf_R + (indata->Stride[0] << 3) + 8;
		buf_G1 = buf_G + (indata->Stride[1] << 3) + 8;
		buf_B1 = buf_B + (indata->Stride[2] << 3) + 8;
		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				k2 = k >> 1;

				r = *buf_R1++;
				g = *buf_G1++;
				b = *buf_B1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i & 1)) && (!(j & 1)))
				{
					block[4][k2 + 36] = (INT16)(((b - y)*0x48)>>7);
					block[5][k2 + 36] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[3][k] = (INT16)(y - 128);
				
				k++;
			}
			buf_R1 += blockstep[0];
			buf_G1 += blockstep[1];
			buf_B1 += blockstep[2];
		}
	}	
	
	return 0;
}


INT32	getRGB888iblockdata(VO_VIDEO_BUFFER*		indata, 
						    JPEG_MCU_BLOCK*			outblock,
							PICTURE_ATT*			picture,
							UINT32					xi,
							UINT32					yi)
{
	register INT32 i, j, k;
	INT32 r, g, b, y, k2;
	INT32 blockwidthRGB;
	INT32 blockheightRGB;
	INT32 blockstep;
	INT32 WB;
	INT32 HB;
	INT16 **block;
	UINT8 *buf_RGB ;
	UINT8 *buf_RGB1;

	blockwidthRGB   = xi * picture->MCU_width;
	blockheightRGB  = yi * picture->MCU_heigth;

	block = outblock->WorkBlock;
	buf_RGB  = indata->Buffer[0] + indata->Stride[0] * blockheightRGB + blockwidthRGB * 3;

	if(xi < picture->MCU_XNum - 1 && yi < picture->MCU_YNum - 1)
	{
		buf_RGB1 = buf_RGB + indata->Stride[0];
		blockstep = (indata->Stride[0] << 1) - 48;
		
		for(i = 0; i < 4; i++)
		{
			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				k2 = k << 1;
				
				b = *buf_RGB++;
				g = *buf_RGB++;
				r = *buf_RGB++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k ] = (INT16)(((b - y)*0x48)>>7);
				block[5][k ] = (INT16)(((r - y)*0x5b)>>7);
				block[0][k2] = (INT16)(y - 128);
				
				b = *buf_RGB++;
				g = *buf_RGB++;
				r = *buf_RGB++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[0][k2 + 1] = (INT16)(y - 128);
				
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[0][k2 + 8] = (INT16)(y - 128);	
				
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[0][k2 + 9] = (INT16)(y - 128);			
				
				k++;
			}

			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				k2 = k << 1;
				
				b = *buf_RGB++;
				g = *buf_RGB++;
				r = *buf_RGB++;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k + 4] = (INT16)(((b - y)*0x48)>>7);
				block[5][k + 4] = (INT16)(((r - y)*0x5b)>>7);
				block[1][k2] = (INT16)(y - 128);
				
				b = *buf_RGB++;
				g = *buf_RGB++;
				r = *buf_RGB++;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[1][k2 + 1] = (INT16)(y - 128);
				
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[1][k2 + 8] = (INT16)(y - 128);	
				
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[1][k2 + 9] = (INT16)(y - 128);			
				
				k++;
			}
			
			buf_RGB += blockstep;
			buf_RGB1 += blockstep;
		}
		
		for(i = 0; i < 4; i++)
		{
			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				k2 = k << 1;
				
				b = *buf_RGB++;
				g = *buf_RGB++;
				r = *buf_RGB++;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k + 32] = (INT16)(((b - y)*0x48)>>7);
				block[5][k + 32] = (INT16)(((r - y)*0x5b)>>7);
				block[2][k2] = (INT16)(y - 128);
				
				b = *buf_RGB++;
				g = *buf_RGB++;
				r = *buf_RGB++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[2][k2 + 1] = (INT16)(y - 128);
				
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[2][k2 + 8] = (INT16)(y - 128);	
				
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[2][k2 + 9] = (INT16)(y - 128);			
				
				k++;
			}

			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				k2 = k << 1;
				
				b = *buf_RGB++;
				g = *buf_RGB++;
				r = *buf_RGB++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k + 36] = (INT16)(((b - y)*0x48)>>7);
				block[5][k + 36] = (INT16)(((r - y)*0x5b)>>7);
				block[3][k2] = (INT16)(y - 128);
				
				b = *buf_RGB++;
				g = *buf_RGB++;
				r = *buf_RGB++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[3][k2 + 1] = (INT16)(y - 128);
				
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[3][k2 + 8] = (INT16)(y - 128);	
				
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[3][k2 + 9] = (INT16)(y - 128);			
				
				k++;
			}
			
			buf_RGB += blockstep;
			buf_RGB1 += blockstep;
		}
	}
	else
	{
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);
		
		WB = MIN(picture->width  - blockwidthRGB , 8);
		HB = MIN(picture->heigth - blockheightRGB, 8);		
		blockstep = indata->Stride[0] - WB*3;

		buf_RGB1 = buf_RGB;		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i&1)) && (!(j&1)))
				{
					block[4][k>>1] = (INT16)(((b - y)*0x48)>>7);
					block[5][k>>1] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[0][k] = (INT16)(y - 128);
				
				k++;
			}
			
			buf_RGB1 += blockstep;
		}

		WB = MIN(MAX(picture->width  - blockwidthRGB - 8, 0) , 8);
		blockstep = indata->Stride[0] - WB*3;
		buf_RGB1 = buf_RGB + 24;	
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i&1)) && (!(j&1)))
				{
					block[4][(k>>1) + 4] = (INT16)(((b - y)*0x48)>>7);
					block[5][(k>>1) + 4] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[1][k] = (INT16)(y - 128);				
				k++;
			}
			
			buf_RGB1 += blockstep;
		}

		WB = MIN(picture->width  - blockwidthRGB , 8);
		HB = MIN(MAX(picture->heigth - blockheightRGB - 8, 0), 8);
		blockstep = indata->Stride[0] - WB*3;
		buf_RGB1 = buf_RGB + (indata->Stride[0] << 3);
	
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i&1)) && (!(j&1)))
				{
					block[4][(k>>1) + 32] = (INT16)(((b - y)*0x48)>>7);
					block[5][(k>>1) + 32] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[2][k] = (INT16)(y - 128);
				
				k++;
			}
			
			buf_RGB1 += blockstep;
		}

		WB = MIN(MAX(picture->width  - blockwidthRGB - 8, 0) , 8);
		blockstep = indata->Stride[0] - WB*3;
		buf_RGB1 = buf_RGB + (indata->Stride[0] << 3) + 24;		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				b = *buf_RGB1++;
				g = *buf_RGB1++;
				r = *buf_RGB1++;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i&1)) && (!(j&1)))
				{
					block[4][(k>>1) + 36] = (INT16)(((b - y)*0x48)>>7);
					block[5][(k>>1) + 36] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[3][k] = (INT16)(y - 128);
				
				k++;
			}
			
			buf_RGB1 += blockstep;
		}
	}

	return 0;
}

INT32	getRGB565iblockdata(VO_VIDEO_BUFFER*		indata, 
							JPEG_MCU_BLOCK*			outblock,
							PICTURE_ATT*			picture,
							UINT32					xi,
							UINT32					yi)
{
	register INT32 i, j, k;
	register UINT16 data;
	INT32 r, g, b, y, k2;
	INT32 blockwidthRGB;
	INT32 blockheightRGB;
	INT32 blockstep;
	INT32 WB;
	INT32 HB;
	INT16 **block;
	UINT16 *buf_RGB ;
	UINT16 *buf_RGB1;

	blockwidthRGB   = xi * picture->MCU_width;
	blockheightRGB  = yi * picture->MCU_heigth;

	buf_RGB  = (UINT16 *)(indata->Buffer[0] + indata->Stride[0] * blockheightRGB + (blockwidthRGB << 1));
	block = outblock->WorkBlock;
	blockstep = indata->Stride[0] >> 1;
	
	if(xi < picture->MCU_XNum - 1 && yi < picture->MCU_YNum - 1)
	{
		buf_RGB1 = buf_RGB + blockstep;
		blockstep = indata->Stride[0] - 16;
		
		for(i = 0; i < 4; i++)
		{
			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				k2 = k << 1;
				
				data = *buf_RGB++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k ] = (INT16)(((b - y)*0x48)>>7);
				block[5][k ] = (INT16)(((r - y)*0x5b)>>7);
				block[0][k2] = (INT16)(y - 128);

				data = *buf_RGB++;				
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[0][k2 + 1] = (INT16)(y - 128);
				
				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[0][k2 + 8] = (INT16)(y - 128);	
				
				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[0][k2 + 9] = (INT16)(y - 128);			
				
				k++;
			}

			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				k2 = k << 1;
				
				data = *buf_RGB++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k + 4] = (INT16)(((b - y)*0x48)>>7);
				block[5][k + 4] = (INT16)(((r - y)*0x5b)>>7);
				block[1][k2] = (INT16)(y - 128);
				
				data = *buf_RGB++;				
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[1][k2 + 1] = (INT16)(y - 128);
				
				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[1][k2 + 8] = (INT16)(y - 128);	
				
				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[1][k2 + 9] = (INT16)(y - 128);			
				
				k++;
			}
			
			buf_RGB += blockstep;
			buf_RGB1 += blockstep;
		}
		
		for(i = 0; i < 4; i++)
		{
			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				k2 = k << 1;
				
				data = *buf_RGB++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k + 32] = (INT16)(((b - y)*0x48)>>7);
				block[5][k + 32] = (INT16)(((r - y)*0x5b)>>7);
				block[2][k2] = (INT16)(y - 128);
				
				data = *buf_RGB++;				
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[2][k2 + 1] = (INT16)(y - 128);
				
				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[2][k2 + 8] = (INT16)(y - 128);	
				
				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[2][k2 + 9] = (INT16)(y - 128);			
				
				k++;
			}

			k = i << 3;
			for(j = 0; j < 4; j ++)
			{
				k2 = k << 1;
				
				data = *buf_RGB++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[4][k + 36] = (INT16)(((b - y)*0x48)>>7);
				block[5][k + 36] = (INT16)(((r - y)*0x5b)>>7);
				block[3][k2] = (INT16)(y - 128);
				
				data = *buf_RGB++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[3][k2 + 1] = (INT16)(y - 128);
				
				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[3][k2 + 8] = (INT16)(y - 128);	
				
				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;
				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				block[3][k2 + 9] = (INT16)(y - 128);			
				
				k++;
			}
			
			buf_RGB += blockstep;
			buf_RGB1 += blockstep;
		}
	}
	else
	{
		memset(block[0], 0, sizeof(INT16) * JPEG_BLOCK_SIZE * outblock->blockNum);
		WB = MIN(picture->width  - blockwidthRGB , 8);
		HB = MIN(picture->heigth - blockheightRGB, 8);
		buf_RGB1 = buf_RGB;
		blockstep = (indata->Stride[0] >> 1) - WB;
		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				k2 = k >> 1;

				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;

				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i & 1)) && (!(j & 1)))
				{
					block[4][k2] = (INT16)(((b - y)*0x48)>>7);
					block[5][k2] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[0][k] = (INT16)(y - 128);
				
				k++;
			}
			buf_RGB1 += blockstep;

		}

		WB = MIN(MAX(picture->width  - blockwidthRGB - 8, 0) , 8);
		blockstep = (indata->Stride[0] >> 1) - WB;
		buf_RGB1 = buf_RGB + 8;

		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				k2 = k >> 1;

				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;

				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i & 1)) && (!(j & 1)))
				{
					block[4][k2 + 4] = (INT16)(((b - y)*0x48)>>7);
					block[5][k2 + 4] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[1][k] = (INT16)(y - 128);
				
				k++;
			}
			buf_RGB1 += blockstep;
		}

		WB = MIN(picture->width  - blockwidthRGB , 8);
		HB = MIN(MAX(picture->heigth - blockheightRGB - 8, 0), 8);
		blockstep = (indata->Stride[0] >> 1) - WB;
		
		buf_RGB1 = buf_RGB + (indata->Stride[0] << 2);		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				k2 = k >> 1;

				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;

				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i & 1)) && (!(j & 1)))
				{
					block[4][k2 + 32] = (INT16)(((b - y)*0x48)>>7);
					block[5][k2 + 32] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[2][k] = (INT16)(y - 128);
				
				k++;
			}
			buf_RGB1 += blockstep;
		}

		WB = MIN(MAX(picture->width  - blockwidthRGB - 8, 0) , 8);
		blockstep = (indata->Stride[0] >> 1) - WB;
		
		buf_RGB1 = buf_RGB + (indata->Stride[0] << 2) + 8;		
		for(i = 0; i < HB; i++)
		{
			k = i << 3;
			for(j = 0; j < WB; j ++)
			{
				k2 = k >> 1;

				data = *buf_RGB1++;
				r = (data&0xf800)>>8;
				g = (data&0x07c0)>>3;
				b = (data&0x001f)<<3;

				y = (0x26*r + 0x4b*g + 0xf*b)>>7;
				if((!(i & 1)) && (!(j & 1)))
				{
					block[4][k2 + 36] = (INT16)(((b - y)*0x48)>>7);
					block[5][k2 + 36] = (INT16)(((r - y)*0x5b)>>7);
				}
				block[3][k] = (INT16)(y - 128);
				
				k++;
			}
			buf_RGB1 += blockstep;
		}
	}

	return 0;	
}