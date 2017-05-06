#include "global.h"
/*!
 ***********************************************************************
 * \brief
 *    Inverse 4x4 transformation, transforms cof to imgY directly;
 ***********************************************************************
 */
void itrans(avdUInt8 *outStart, avdInt16 *coef, avdNativeInt outLnLength)
{
	// TBD: 1. try roll out loops, 2. try to classify sparseness;
	avdNativeInt i,j;
	avdNativeInt m6[4];
	StartRunTimeClock(DT_ITRANS);	
	// horizontal, notice that coef[i][j], i & j reversed; 
	for (i=0;i<BLOCK_SIZE;i++)
	{
		m6[0] = coef[0] + coef[2];
		m6[1] = coef[0] - coef[2];
		m6[2] = (coef[1]>>1) - coef[3];
		m6[3] = coef[1] + (coef[3]>>1);		
		img->m7[0][i]=m6[0]+m6[3];
		img->m7[1][i]=m6[1]+m6[2];
		img->m7[2][i]=m6[1]-m6[2];
		img->m7[3][i]=m6[0]-m6[3];
		coef += 4;
	}
	// vertical
	for (j=0;j<BLOCK_SIZE;j++)
	{
		m6[0]=(img->m7[j][0]     +  img->m7[j][2]);
		m6[1]=(img->m7[j][0]     -  img->m7[j][2]);
		m6[2]=(img->m7[j][1]>>1) -  img->m7[j][3];
		m6[3]= img->m7[j][1]     + (img->m7[j][3]>>1);
		outStart[0] = img->clip255[((m6[0]+m6[3]+32)>>6)+outStart[0]];
		outStart[1] = img->clip255[((m6[1]+m6[2]+32)>>6)+outStart[1]];
		outStart[2] = img->clip255[((m6[1]-m6[2]+32)>>6)+outStart[2]];
		outStart[3] = img->clip255[((m6[0]-m6[3]+32)>>6)+outStart[3]];
		outStart += outLnLength;
	}
	EndRunTimeClock(DT_ITRANS);	
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
void itrans_LeftHalf(avdUInt8 *outStart, avdInt16 *coef, avdNativeInt outLnLength)
{
	avdNativeInt m6[4], i;
	StartRunTimeClock(DT_ITRANSLEFTHALF);	
	m6[0] = coef[0] + coef[2];
	m6[1] = coef[0] - coef[2];
	m6[2] = (coef[1]>>1) - coef[3];
	m6[3] = coef[1] + (coef[3]>>1);		
	img->m7[0][0] = m6[0]+m6[3]; // use m7[j][i]=m7[row][col];
	img->m7[0][1] = m6[1]+m6[2];
	img->m7[0][2] = m6[1]-m6[2];
	img->m7[0][3] = m6[0]-m6[3];
	m6[0] = coef[4] + coef[6]; 
	m6[1] = coef[4] - coef[6]; 
	m6[2] = (coef[5]>>1) - coef[7];
	m6[3] = coef[5] + (coef[7]>>1);		
	img->m7[1][0] = m6[0]+m6[3];
	img->m7[1][1] = m6[1]+m6[2];
	img->m7[1][2] = m6[1]-m6[2];
	img->m7[1][3] = m6[0]-m6[3];

	// A * m7; m7 only have upper half (two rows);
	for (i = 0; i < BLOCK_SIZE; i++)
	{
		outStart[0] = img->clip255[((img->m7[0][i]+img->m7[1][i]+32)>>6) 
			+ outStart[0]];
		outStart[1] = img->clip255[((img->m7[0][i]+(img->m7[1][i]>>1)+32)>>6) 
			+ outStart[1]];
		outStart[2] = img->clip255[((img->m7[0][i]-(img->m7[1][i]>>1)+32)>>6) 
			+ outStart[2]];
		outStart[3] = img->clip255[((img->m7[0][i]-img->m7[1][i]+32)>>6) 
			+ outStart[3]];
		outStart += outLnLength;
	}
	EndRunTimeClock(DT_ITRANSLEFTHALF);	
}
