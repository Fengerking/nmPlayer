#include "global.h"
#include "h264VdDump.h"
#include "mbuffer.h"
/*!
 ***********************************************************************
 * \brief
 *    Inverse 4x4 for matrix only having left half elements
 *
 *
 *   {A * coef * B} ^ T   (T: transpose)
 *
 *     | 1   1   1  .5 |          | 1   1   1   1 |
 * = { | 1  .5  -1  -1 | * coef * | 1  .5 -.5  -1 | } ^ T
 *     | 1 -.5  -1   1 |          | 1  -1  -1   1 |
 *     | 1  -1   1 -.5 |          |.5  -1   1 -.5 |
 *
 * -- since index of coeff[i][j](=coef[col][row]) is reversed, so 
 *    left half means upper half of coef; 
 * -- so we do coef * B first;
 ***********************************************************************
 */
#ifndef ARM_ASM
#define USE_REF_IDCT 1
#endif//X86_TEST
#define DUMP_T8x8 1
#if (DUMP_VERSION & DUMP_SLICE)
//#define DEBUG_IDCT
#ifdef DEBUG_IDCT


void DumpPred(ImageParameters *img,TMBsProcessor* info,avdUInt8 *pred,int stride)
{
		
		Dumploop2(DUMP_SLICEDATA,info,"Pred",BLOCK_SIZE,BLOCK_SIZE,pred,DDT_D8,stride);
}
void DumpResidual(ImageParameters *img,TMBsProcessor* info,avdUInt8 *data,int stride)
{
		Dumploop2(DUMP_SLICEDATA,info,"Residual", BLOCK_SIZE,BLOCK_SIZE,data,DDT_D16,stride);
}
void DumpIDCT(ImageParameters *img,TMBsProcessor* info,avdUInt8 *data,int stride)
{
		static int idctNum = 0;
		AvdLog(DUMP_SLICEDATA,info->mb_y&1,"idctNum=%d\n",idctNum++);
		Dumploop2(DUMP_SLICEDATA,info,"IDCT",BLOCK_SIZE,BLOCK_SIZE,data,DDT_D8,stride);
}
#endif//DEBUG_IDCT

#endif//(DUMP_VERSION & DUMP_SLICE)	
#ifndef DEBUG_IDCT
#define  DumpPred(a,b,c,d)
#define  DumpResidual(a,b,c,d)
#define  DumpIDCT(a,b,c,d)
#endif//DEBUG_IDCT
#ifdef ARM_ASM
#if !defined(ARMv4) //&& !defined(DISABLE_ITRANS_ASM)
#define ARM_ASM_ITRANS ITRANS_ASM
#endif//ARMv4
#endif//ARM_ASM
void itrans(ImageParameters *img,TMBsProcessor *info,avdUInt8 *outStart, avdInt16 *coef, avdNativeInt outLnLength)
{
	
#if ARM_ASM_ITRANS//NEON
	int start;
	TIME_BEGIN(start)
	itrans_asm(outStart, coef, outLnLength);
#else//NEON	
	
	avdNativeInt i, a, b, c, d;
	avdNativeInt *m7 = &info->mbsParser->m7[0][0];
	TCLIPInfo	*clipInfo = img->clipInfo;
	avdUInt8 *clip255 = clipInfo->clip255;
#if USE_REF_IDCT
	avdNativeInt idctout[4][4];
	avdInt16 idctcoef[4][4];
#endif
#ifdef DEBUG_IDCT
	avdUInt8* outStart2 = outStart;
	AvdLog(DUMP_SLICE,DUMP_DCORE"\nMB=%d ",GetCurPos(info,img->sizeInfo));
#endif
	int start;
	TIME_BEGIN(start)
#ifdef DEBUG_IDCT
	DumpPred(img,info,outStart,outLnLength);
	DumpResidual(img,info,coef,4);
#endif//
#if !USE_REF_IDCT
	b = coef[0] + 32;
	a = b + coef[2];
	b -= coef[2];
	c = (coef[1]>>1) - coef[3];
	d = coef[1] + (coef[3]>>1);		
	m7[0]  = a + d; 
	m7[4]  = b + c;
	m7[8]  = b - c;
	m7[12] = a - d;
	a = coef[4] + coef[6]; 
	b = coef[4] - coef[6]; 
	c = (coef[5]>>1) - coef[7];
	d = coef[5] + (coef[7]>>1);		
	m7[1]  = a + d;
	m7[5]  = b + c;
	m7[9]  = b - c;
	m7[13] = a - d;

	a = coef[8] + coef[10]; 
	b = coef[8] - coef[10]; 
	c = (coef[9]>>1) - coef[11];
	d = coef[9] + (coef[11]>>1);		
	m7[2]  = a + d;
	m7[6]  = b + c;
	m7[10] = b - c;
	m7[14] = a - d;

	a = coef[12] + coef[14]; 
	b = coef[12] - coef[14]; 
	c = (coef[13]>>1) - coef[15];
	d = coef[13] + (coef[15]>>1);		
	m7[3]  = a + d;
	m7[7]  = b + c;
	m7[11] = b - c;
	m7[15] = a - d;
	i = BLOCK_SIZE;
	do {
		a = (m7[0]     +  m7[2]);
		b = (m7[0]     -  m7[2]);
		c = (m7[1]>>1) -  m7[3];
		d = m7[1]      + (m7[3]>>1);
		// only good for little endian;
		*(avdUInt32 *)outStart = 
			(iClip255(((a+d)>>6)+outStart[0])) |
			(iClip255(((b+c)>>6)+outStart[1])<<8) |
			(iClip255(((b-c)>>6)+outStart[2])<<16) |
			(iClip255(((a-d)>>6)+outStart[3])<<24);
		outStart += outLnLength;
		m7 += 4;
#if 0//(DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "Itrans:(%d,%d,%d,%d)(%d,%d,%d,%d)\n",a,b,c,d,outStart[0],outStart[1],outStart[2],outStart[3]);
#endif//(DUMP_VERSION & DUMP_SLICE)
	} while (--i);
#else//USE_REF_IDCT
	

	for(i = 0; i<BLOCK_SIZE; i++)
	{
		idctcoef[0][i] = *coef++;
		idctcoef[1][i] = *coef++;
		idctcoef[2][i] = *coef++;
		idctcoef[3][i] = *coef++;
	}
	coef-=16;

	{
		int i, ii;  
		int tmp[16];
		int *pTmp = tmp; 
		avdInt16 *pblock;
		int p0,p1,p2,p3;
		int t0,t1,t2,t3;

		// Horizontal
		for (i = 0; i < 0 + BLOCK_SIZE; i++)
		{
			pblock = &idctcoef[i][0];
			t0 = *(pblock++);
			t1 = *(pblock++);
			t2 = *(pblock++);
			t3 = *(pblock  );

			p0 =  t0 + t2;
			p1 =  t0 - t2;
			p2 = (t1 >> 1) - t3;
			p3 =  t1 + (t3 >> 1);

			*(pTmp++) = p0 + p3;
			*(pTmp++) = p1 + p2;
			*(pTmp++) = p1 - p2;
			*(pTmp++) = p0 - p3;
		}

		//  Vertical 
		for (i = 0; i < BLOCK_SIZE; i++)
		{
			pTmp = tmp + i;
			t0 = *pTmp;
			t1 = *(pTmp += BLOCK_SIZE);
			t2 = *(pTmp += BLOCK_SIZE);
			t3 = *(pTmp += BLOCK_SIZE);

			p0 = t0 + t2;
			p1 = t0 - t2;
			p2 =(t1 >> 1) - t3;
			p3 = t1 + (t3 >> 1);

			idctout[0    ][i] = p0 + p3;
			idctout[0 + 1][i] = p1 + p2;
			idctout[0 + 2][i] = p1 - p2;
			idctout[0 + 3][i] = p0 - p3;
		}
	}

	i = 0;
	do {
		*(avdUInt32 *)outStart = 
			(iClip255(((idctout[i][0]+32)>>6)+outStart[0])) |
			(iClip255(((idctout[i][1]+32)>>6)+outStart[1])<<8) |
			(iClip255(((idctout[i][2]+32)>>6)+outStart[2])<<16) |
			(iClip255(((idctout[i][3]+32)>>6)+outStart[3])<<24);
		outStart += outLnLength;
		i++;
	} while (i<4);

#endif//USE_REF_IDCT

	

#ifdef DEBUG_IDCT
	DumpIDCT(img,info,outStart2,outLnLength);
#endif
	
#endif//NEON
	TIME_END(start,idctSum)
}


#if FEATURE_T8x8
#if (DUMP_VERSION & DUMP_SLICE)
void DUMP_T8x8_NO_T(ImageParameters *img,TMBsProcessor* info,avdUInt8 *outStart, avdInt16 *coef, avdNativeInt outLnLength)
{
	
	int i,j,k;
	if(img->type!=I_SLICE||!DUMP_T8x8)
		return;
	AvdLog(DUMP_SLICE,DUMP_DCORE"\nBEFORE_T8x8_COEF\n");
	for (i=0;i<BLOCK_SIZE_8x8;i++)
	{
		AvdLog(DUMP_SLICE,DUMP_DCORE"[0]=%d,[1]=%d,[2]=%d,[3]=%d,[4]=%d,[5]=%d,[6]=%d,[7]=%d\n",0,0,0,0,0,0,0,0);
	}
	Dumploop2(DUMP_SLICE,info,"T8x8_COEF_AFTER_TRANSFORM",8,8,outStart,DDT_D8,outLnLength);
}
#endif//#if (DUMP_VERSION & DUMP_SLICE)
void itrans8x8(ImageParameters *img,TMBsProcessor* info,avdUInt8 *outStart, avdInt16 *coef, avdNativeInt outLnLength)//int **tblock, int **block, int pos_y, int pos_x)
{
	int i, ii,j;
	//avdInt16 tmp[64];
	avdNativeInt *pTmp = info->mbsParser->m7;//, *pblock0,*pblock1,*pblock2,*pblock3,*tblock;
	avdNativeInt *tmp=pTmp;
	int a0, a1, a2, a3;
	int p0, p1, p2, p3, p4, p5 ,p6, p7;  
	int b0, b1, b2, b3, b4, b5, b6, b7;
	avdUInt8* block=outStart;
	TCLIPInfo	*clipInfo = img->clipInfo;
	avdUInt8 *clip255 = clipInfo->clip255;
	int start;
	TIME_BEGIN(start)
	//tblock=coef;
	//pblock0=tblock;
	//pblock1=pblock0+16;
	//pblock2=pblock0+32;
	//pblock3=pblock0+48;
	// Horizontal  
#if (DUMP_VERSION & DUMP_SLICE)
	if(img->type==I_SLICE&&DUMP_T8x8)
		AvdLog(DUMP_SLICE,DUMP_DCORE"\nBEFORE_T8x8_COEF\n");
#endif//(DUMP_VERSION & DUMP_SLICE)
	//for (j=0;j<2;j++)
	{
		avdInt16 *pblock0=coef;
		
		for (i=0; i <BLOCK_SIZE_8x8; i++)
		{
			p0 = *(pblock0++);
			p1 = *(pblock0++);
			p2 = *(pblock0++);
			p3 = *(pblock0++);
			p4 = *(pblock0++);
			p5 = *(pblock0++);
			p6 = *(pblock0++);
			p7 = *(pblock0++);
#if (DUMP_VERSION & DUMP_SLICE)
		if(img->type==I_SLICE&&DUMP_T8x8)
			AvdLog(DUMP_SLICE,DUMP_DCORE"[0]=%d,[1]=%d,[2]=%d,[3]=%d,[4]=%d,[5]=%d,[6]=%d,[7]=%d\n",p0,p1,p2,p3,p4,p5,p6,p7);
#endif//(DUMP_VERSION & DUMP_SLICE)
			a0 = p0 + p4;
			a1 = p0 - p4;
			a2 = p6 - (p2 >> 1);
			a3 = p2 + (p6 >> 1);

			b0 =  a0 + a3;
			b2 =  a1 - a2;
			b4 =  a1 + a2;
			b6 =  a0 - a3;

			a0 = -p3 + p5 - p7 - (p7 >> 1);    
			a1 =  p1 + p7 - p3 - (p3 >> 1);    
			a2 = -p1 + p7 + p5 + (p5 >> 1);    
			a3 =  p3 + p5 + p1 + (p1 >> 1);


			b1 =  a0 + (a3>>2);    
			b3 =  a1 + (a2>>2);    
			b5 =  a2 - (a1>>2);
			b7 =  a3 - (a0>>2);                

			*(pTmp++) = b0 + b7;
			*(pTmp++) = b2 - b5;
			*(pTmp++) = b4 + b3;
			*(pTmp++) = b6 + b1;
			*(pTmp++) = b6 - b1;
			*(pTmp++) = b4 - b3;
			*(pTmp++) = b2 + b5;
			*(pTmp++) = b0 - b7;
		}
	}
	

	//  Calc Vertical 
	for (i=0; i < BLOCK_SIZE_8x8; i++)
	{
		avdUInt8* tmp2;
		pTmp = tmp + i;
		p0 = *pTmp;
		p1 = *(pTmp += BLOCK_SIZE_8x8);
		p2 = *(pTmp += BLOCK_SIZE_8x8);
		p3 = *(pTmp += BLOCK_SIZE_8x8);
		p4 = *(pTmp += BLOCK_SIZE_8x8);
		p5 = *(pTmp += BLOCK_SIZE_8x8);
		p6 = *(pTmp += BLOCK_SIZE_8x8);
		p7 = *(pTmp += BLOCK_SIZE_8x8);

		a0 =  p0 + p4;
		a1 =  p0 - p4;
		a2 =  p6 - (p2>>1);
		a3 =  p2 + (p6>>1);

		b0 = a0 + a3;
		b2 = a1 - a2;
		b4 = a1 + a2;
		b6 = a0 - a3;

		a0 = -p3 + p5 - p7 - (p7 >> 1);
		a1 =  p1 + p7 - p3 - (p3 >> 1);
		a2 = -p1 + p7 + p5 + (p5 >> 1);
		a3 =  p3 + p5 + p1 + (p1 >> 1);


		b1 =  a0 + (a3 >> 2);
		b7 =  a3 - (a0 >> 2);
		b3 =  a1 + (a2 >> 2);
		b5 =  a2 - (a1 >> 2);

		tmp2=block;
#define SHIFT_T8x8_ADD(a,b) (((a)+(b)+32)>>6)
#define SHIFT_T8x8_SUB(a,b) (((a)-(b)+32)>>6)
		block[0] = iClip255(SHIFT_T8x8_ADD(b0,b7)+block[0]);block+=outLnLength;
		block[0] = iClip255(SHIFT_T8x8_SUB(b2,b5)+block[0]);block+=outLnLength;
		block[0] = iClip255(SHIFT_T8x8_ADD(b4, b3)+block[0]);block+=outLnLength;
		block[0] = iClip255(SHIFT_T8x8_ADD(b6, b1)+block[0]);block+=outLnLength;
		block[0] = iClip255(SHIFT_T8x8_SUB(b6, b1)+block[0]);block+=outLnLength;
		block[0] = iClip255(SHIFT_T8x8_SUB(b4,b3)+block[0]);block+=outLnLength;
		block[0] = iClip255(SHIFT_T8x8_ADD(b2 , b5)+block[0]);block+=outLnLength;
		block[0] = iClip255(SHIFT_T8x8_SUB(b0,b7)+block[0]);
		block=tmp2+1;
	}
	TIME_END(start,idctSum)
#if (DUMP_VERSION & DUMP_SLICE)
	if(img->type==I_SLICE&&DUMP_T8x8)
	Dumploop2(DUMP_SLICE,info,"T8x8_COEF_AFTER_TRANSFORM",8,8,outStart,DDT_D8,outLnLength);
#endif//(DUMP_VERSION & DUMP_SLICE)
}
#endif//#if FEATURE_T8x8