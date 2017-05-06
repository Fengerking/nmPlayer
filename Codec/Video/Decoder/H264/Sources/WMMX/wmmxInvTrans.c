#include "global.h"
void WMMXItrans(avdUInt8 *outStart, 
							avdInt16  *coef, 
							avdNativeInt outLnLength,
							avdInt16*	quotiety);
void WMMXitrans_LeftHalf(avdUInt8 *outStart, avdInt16 *coef, 
						 avdNativeInt outLnLength,
						 avdInt16*	quotiety);


/*
static avdInt16 quotiety2[6][4]=
{
	{2,2,2,1},f
	{2,1,-2,-2},
	{2,-1,-2,2},
	{2,-2,2,-1},
	{2, 2,2,2},
	{2,1,-1,-2},
};*/



//#define HALFITRANS
//#define ITRANS



//#define BLOCKITRANS
//#define BLOCKHALFITRANS
/*!
 ***********************************************************************
 * \brief
 *    Inverse 4x4 transformation, transforms cof to imgY directly;
 ***********************************************************************
 */
void itrans(ImageParameters *img,avdUInt8 *outStart, avdInt16 *coef, avdNativeInt outLnLength)
{
	//avdNativeInt i,j;
	//avdNativeInt m6[4];
#ifdef WMMX
#ifdef BLOCKITRANS
	return;
#endif
	avdInt16* temp = img->itransTemCoeff
	
	StartRunTimeClock(DT_ITRANS);
	WMMXItrans(outStart,coef,outLnLength,temp);
	EndRunTimeClock(DT_ITRANS);	
#else//ITRANS
	avdNativeInt i,j,a,b,c,d;
	StartRunTimeClock(DT_ITRANS);	
	// horizontal, notice that coef[i][j], i & j reversed; 
	for (i=0;i<BLOCK_SIZE;i++)
	{
		a = coef[0] + coef[2];
		b = coef[0] - coef[2];
		c = (coef[1]>>1) - coef[3];
		d = coef[1] + (coef[3]>>1);		
		img->m7[0][i] = a + d;
		img->m7[1][i] = b + c;
		img->m7[2][i] = b - c;
		img->m7[3][i] = a - d;
		coef += 4;
	}
	// vertical
	for (j=0;j<BLOCK_SIZE;j++)
	{
		a = (img->m7[j][0]     +  img->m7[j][2]);
		b = (img->m7[j][0]     -  img->m7[j][2]);
		c = (img->m7[j][1]>>1) -  img->m7[j][3];
		d = img->m7[j][1]      + (img->m7[j][3]>>1);
		outStart[0] = img->clip255[((a+d+32)>>6)+outStart[0]];
		outStart[1] = img->clip255[((b+c+32)>>6)+outStart[1]];
		outStart[2] = img->clip255[((b-c+32)>>6)+outStart[2]];
		outStart[3] = img->clip255[((a-d+32)>>6)+outStart[3]];
		outStart += outLnLength;
	}
	EndRunTimeClock(DT_ITRANS);	
#endif//ITRANS
}

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
void itrans_LeftHalf(ImageParameters *img,avdUInt8 *outStart, avdInt16 *coef, avdNativeInt outLnLength)
{
	//avdNativeInt m6[4], i;
#ifdef WMMX
#ifdef BLOCKHALFITRANS
	return;
#endif	
	avdInt16* temp = img->itransTemCoeff
	StartRunTimeClock(DT_ITRANSLEFTHALF);	
	WMMXitrans_LeftHalf(outStart,coef,outLnLength,temp);
	EndRunTimeClock(DT_ITRANSLEFTHALF);	
#else//HALFITRANS
	avdNativeInt i, a, b, c, d;
	avdNativeInt *m7 = &img->m7[0][0];
	StartRunTimeClock(DT_ITRANSLEFTHALF);	
	a = coef[0] + coef[2];
	b = coef[0] - coef[2];
	c = (coef[1]>>1) - coef[3];
	d = coef[1] + (coef[3]>>1);		
	m7[0] = a+d; // use m7[j][i]=m7[row][col];
	m7[1] = b+c;
	m7[2] = b-c;
	m7[3] = a-d;
	a = coef[4] + coef[6]; 
	b = coef[4] - coef[6]; 
	c = (coef[5]>>1) - coef[7];
	d = coef[5] + (coef[7]>>1);		
	m7[4] = a+d;
	m7[5] = b+c;
	m7[6] = b-c;
	m7[7] = a-d;
	
	// A * m7; m7 only have upper half (two rows);
	for (i = 0; i < BLOCK_SIZE; i++)
	{
		a = i + 4;
		outStart[0] = img->clip255[((m7[i]+m7[a]+32)>>6) 
			+ outStart[0]];
		outStart[1] = img->clip255[((m7[i]+(m7[a]>>1)+32)>>6) 
			+ outStart[1]];
		outStart[2] = img->clip255[((m7[i]-(m7[a]>>1)+32)>>6) 
			+ outStart[2]];
		outStart[3] = img->clip255[((m7[i]-m7[a]+32)>>6) 
			+ outStart[3]];
		outStart += outLnLength;
	}
	EndRunTimeClock(DT_ITRANSLEFTHALF);	
#endif//HALFITRANS
}





















