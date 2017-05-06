/****************************************************************************
*
*   Module Title :     idctpart.c
*
*   Description  :     IDCT with multiple versions based on # of non 0 coeffs
*
*   Copyright (c) 1999 - 2005  On2 Technologies Inc. All Rights Reserved.
*
****************************************************************************/

/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"
#include "on2_mem.h"

//#define INT32 int
//#define INT16 short
//#define UINT32 unsigned int

//#include "dct.h"

/****************************************************************************
*  Macros
****************************************************************************/
#define IdctAdjustBeforeShift 8

#define xC1S7 64277
#define xC2S6 60547
#define xC3S5 54491
#define xC4S4 46341
#define xC5S3 36410
#define xC6S2 25080
#define xC7S1 12785



/****************************************************************************
*  
****************************************************************************/
//static void (*idct_dx[65])( INT16 *InputData, INT16 *QuantMatrix, INT16 *OutputData );



/****************************************************************************
*  Forward reference
****************************************************************************/

void 
IDct1_G ( INT16 *InputData, INT16 *QuantMatrix, INT16 *OutputData );

void 
IDct10_G ( INT16 *InputData, INT16 *QuantMatrix, INT16 *OutputData );

void 
IDctSlow_G ( INT16 *InputData, INT16 *QuantMatrix, INT16 *OutputData );


/****************************************************************************
 * 
 *  ROUTINE       : dequant_slow
 *
 *  INPUTS        : INT16 *dequant_coeffs : Pointer to dequantization step sizes.
 *                  INT16 *quantized_list : Pointer to quantized DCT coeffs
 *                                          (in zig-zag order).
 *
 *  OUTPUTS       : INT32 *DCT_block      : Pointer to 8x8 de-quantized block
 *                                          (in 2-D raster order).
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : De-quantizes an 8x8 block of quantized DCT coeffs.
 *
 *  SPECIAL NOTES : Uses dequant_index to invert zig-zag ordering. 
 *
 ****************************************************************************/
static FORCEINLINE void 
//dequant_slow ( INT16 *dequant_coeffs, INT16 *quantized_list, INT32 *DCT_block )
dequant_slow ( INT16 *dequant_coeffs, INT16 *quantized_list, INT16 *DCT_block )
{
    int i;

    DCT_block[0]  = quantized_list[0]  * dequant_coeffs[0];

    for(i = 1; i < 64; i++)
    {
//        DCT_block[dequant_index[i]]  =  quantized_list[i] * dequant_coeffs[1];
        DCT_block[i]  =  quantized_list[i] * dequant_coeffs[1];
    }
}

/****************************************************************************
 * 
 *  ROUTINE       : IDctSlow
 *
 *  INPUTS        : INT16 *InputData   : Pointer to 8x8 quantized DCT coefficients.
 *                  INT16 *QuantMatrix : Pointer to 8x8 quantization matrix.
 *
 *  OUTPUTS       : INT16 *OutputData  : Pointer to 8x8 block to hold output.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Inverse quantizes and inverse DCT's input 8x8 block
 *                  to reproduce prediction error.
 *
 *  SPECIAL NOTES : None. 
 *
 ****************************************************************************/
void 
IDctSlow_G ( INT16 *InputData, INT16 *QuantMatrix, INT16 *OutputData )
{
	int   loop;
	INT32 t1, t2;
//    INT32 IntermediateData[64];
    INT16 IntermediateData[64];
	INT32 _A, _B, _C, _D, _Ad, _Bd, _Cd, _Dd, _Ex, _F, _G, _H;
	INT32 _Ed, _Gd, _Add, _Bdd, _Fd, _Hd;
	
//    INT32 *ip = IntermediateData;
    INT16 *ip = IntermediateData;
	INT16 *op = OutputData;
	
	// dequantize the input 
	dequant_slow ( QuantMatrix, InputData, IntermediateData );

	// Inverse DCT on the rows now
	for ( loop=0; loop<8; loop++ )
	{
		// Check for non-zero values
		if ( ip[0] | ip[1] | ip[2] | ip[3] | ip[4] | ip[5] | ip[6] | ip[7] )
		{
			t1 = (INT32)(xC1S7 * ip[1]);
            t2 = (INT32)(xC7S1 * ip[7]);
            t1 >>= 16;
            t2 >>= 16;
			_A = t1 + t2;

			t1 = (INT32)(xC7S1 * ip[1]);
			t2 = (INT32)(xC1S7 * ip[7]);
            t1 >>= 16;
            t2 >>= 16;
			_B = t1 - t2;

			t1 = (INT32)(xC3S5 * ip[3]);
			t2 = (INT32)(xC5S3 * ip[5]);
            t1 >>= 16;
            t2 >>= 16;
			_C = t1 + t2;

			t1 = (INT32)(xC3S5 * ip[5]);
			t2 = (INT32)(xC5S3 * ip[3]);
            t1 >>= 16;
            t2 >>= 16;
			_D = t1 - t2;

			t1 = (INT32)(xC4S4 * (_A - _C));
            t1 >>= 16;
			_Ad = t1;

			t1 = (INT32)(xC4S4 * (_B - _D));
            t1 >>= 16;
			_Bd = t1;
			
			_Cd = _A + _C;
			_Dd = _B + _D;

			t1 = (INT32)(xC4S4 * (ip[0] + ip[4]));
            t1 >>= 16;
			_Ex = t1;

			t1 = (INT32)(xC4S4 * (ip[0] - ip[4]));
            t1 >>= 16;
			_F = t1;
			
			t1 = (INT32)(xC2S6 * ip[2]);
			t2 = (INT32)(xC6S2 * ip[6]);
            t1 >>= 16;
            t2 >>= 16;
			_G = t1 + t2;

			t1 = (INT32)(xC6S2 * ip[2]);
			t2 = (INT32)(xC2S6 * ip[6]);
            t1 >>= 16;
            t2 >>= 16;
			_H = t1 - t2;

			_Ed = _Ex - _G;
			_Gd = _Ex + _G;

			_Add = _F + _Ad;
			_Bdd = _Bd - _H;
			
			_Fd = _F - _Ad;
			_Hd = _Bd + _H;
	
			// Final sequence of operations over-write original inputs.
			ip[0] = (INT16)((_Gd + _Cd )  >> 0);
			ip[7] = (INT16)((_Gd - _Cd )  >> 0);

			ip[1] = (INT16)((_Add + _Hd ) >> 0);
			ip[2] = (INT16)((_Add - _Hd ) >> 0);

			ip[3] = (INT16)((_Ed + _Dd )  >> 0);
			ip[4] = (INT16)((_Ed - _Dd )  >> 0);

			ip[5] = (INT16)((_Fd + _Bdd ) >> 0);
			ip[6] = (INT16)((_Fd - _Bdd ) >> 0);
		}

		ip += 8;			/* next row */
	}

	ip = IntermediateData;

	for ( loop=0; loop<8; loop++ )
	{
		// Check for non-zero values (bitwise | faster than logical ||)
		if ( ip[0 * 8] | ip[1 * 8] | ip[2 * 8] | ip[3 * 8] |
			 ip[4 * 8] | ip[5 * 8] | ip[6 * 8] | ip[7 * 8] )
		{

			t1 = (INT32)(xC1S7 * ip[1*8]);
            t2 = (INT32)(xC7S1 * ip[7*8]);
            t1 >>= 16;
            t2 >>= 16;
			_A = t1 + t2;

			t1 = (INT32)(xC7S1 * ip[1*8]);
			t2 = (INT32)(xC1S7 * ip[7*8]);
            t1 >>= 16;
            t2 >>= 16;
			_B = t1 - t2;

			t1 = (INT32)(xC3S5 * ip[3*8]);
			t2 = (INT32)(xC5S3 * ip[5*8]);
            t1 >>= 16;
            t2 >>= 16;
			_C = t1 + t2;

			t1 = (INT32)(xC3S5 * ip[5*8]);
			t2 = (INT32)(xC5S3 * ip[3*8]);
            t1 >>= 16;
            t2 >>= 16;
			_D = t1 - t2;

			t1 = (INT32)(xC4S4 * (_A - _C));
            t1 >>= 16;
			_Ad = t1;

			t1 = (INT32)(xC4S4 * (_B - _D));
            t1 >>= 16;
			_Bd = t1;

			_Cd = _A + _C;
			_Dd = _B + _D;

			t1 = (INT32)(xC4S4 * (ip[0*8] + ip[4*8]));
            t1 >>= 16;
			_Ex = t1;

			t1 = (INT32)(xC4S4 * (ip[0*8] - ip[4*8]));
            t1 >>= 16;
			_F = t1;
			
			t1 = (INT32)(xC2S6 * ip[2*8]);
			t2 = (INT32)(xC6S2 * ip[6*8]);
            t1 >>= 16;
            t2 >>= 16;
			_G = t1 + t2;

			t1 = (INT32)(xC6S2 * ip[2*8]);
			t2 = (INT32)(xC2S6 * ip[6*8]);
            t1 >>= 16;
            t2 >>= 16;
			_H = t1 - t2;
			
			_Ed = _Ex - _G;
			_Gd = _Ex + _G;

			_Add = _F + _Ad;
			_Bdd = _Bd - _H;
			
			_Fd = _F - _Ad;
			_Hd = _Bd + _H;
	
			_Gd += IdctAdjustBeforeShift;
			_Add += IdctAdjustBeforeShift;
			_Ed += IdctAdjustBeforeShift;
			_Fd += IdctAdjustBeforeShift;

			// Final sequence of operations over-write original inputs.
			op[0*8] = (INT16)((_Gd + _Cd )  >> 4);
			op[7*8] = (INT16)((_Gd - _Cd )  >> 4);

			op[1*8] = (INT16)((_Add + _Hd ) >> 4);
			op[2*8] = (INT16)((_Add - _Hd ) >> 4);

			op[3*8] = (INT16)((_Ed + _Dd )  >> 4);
			op[4*8] = (INT16)((_Ed - _Dd )  >> 4);

			op[5*8] = (INT16)((_Fd + _Bdd ) >> 4);
			op[6*8] = (INT16)((_Fd - _Bdd ) >> 4);
		}
		else
		{
			op[0*8] = 0;
			op[7*8] = 0;
			op[1*8] = 0;
			op[2*8] = 0;
			op[3*8] = 0;
			op[4*8] = 0;
			op[5*8] = 0;
			op[6*8] = 0;
		}

		ip++;			// next column
        op++;
	}
}

/****************************************************************************
 * 
 *  ROUTINE       : dequant_slow10
 *
 *  INPUTS        : INT16 *dequant_coeffs : Pointer to dequantization step sizes.
 *                  INT16 *quantized_list : Pointer to quantized DCT coeffs
 *                                          (in zig-zag order).
 *
 *  OUTPUTS       : INT32 *DCT_block      : Pointer to 8x8 de-quantized block
 *                                          (in 2-D raster order).
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : De-quantizes an 8x8 block of quantized DCT coeffs that
 *                  only has non-zero coefficients in the first 10, i.e.
 *                  only DC & AC1-9 are non-zero, AC10-63 __MUST_BE_ zero.
 *
 *  SPECIAL NOTES : Uses dequant_index to invert zig-zag ordering. 
 *
 ****************************************************************************/
static FORCEINLINE void 
dequant_slow10 ( INT16 *dequant_coeffs, INT16 *quantized_list, INT16 *DCT_block )
{
//	memset(DCT_block,0, 64);

    DCT_block[0] = quantized_list[0]  * dequant_coeffs[0];;
	DCT_block[1] = quantized_list[1] * dequant_coeffs[1];
	DCT_block[2] = quantized_list[2] * dequant_coeffs[1];
	DCT_block[3] = quantized_list[3] * dequant_coeffs[1];

    DCT_block[4] = 0;
    DCT_block[5] = 0;
    DCT_block[6] = 0;
    DCT_block[7] = 0;

	DCT_block[8] = quantized_list[8] * dequant_coeffs[1];
	DCT_block[9] = quantized_list[9] * dequant_coeffs[1];
	DCT_block[10] = quantized_list[10] * dequant_coeffs[1];

    DCT_block[11] = 0;
    DCT_block[12] = 0;
    DCT_block[13] = 0;
    DCT_block[14] = 0;
    DCT_block[15] = 0;

	DCT_block[16] = quantized_list[16] * dequant_coeffs[1];
	DCT_block[17] = quantized_list[17] * dequant_coeffs[1];

    DCT_block[18] = 0;
    DCT_block[19] = 0;
    DCT_block[20] = 0;
    DCT_block[21] = 0;
    DCT_block[22] = 0;
    DCT_block[23] = 0;

	DCT_block[24] = quantized_list[24] * dequant_coeffs[1];

    DCT_block[25] = 0;
    DCT_block[26] = 0;
    DCT_block[27] = 0;
    DCT_block[28] = 0;
    DCT_block[29] = 0;
    DCT_block[30] = 0;
    DCT_block[31] = 0;


}

/****************************************************************************
 * 
 *  ROUTINE       : IDctSlow10
 *
 *  INPUTS        : INT16 *InputData   : Pointer to 8x8 quantized DCT coefficients.
 *                  INT16 *QuantMatrix : Pointer to 8x8 quantization matrix.
 *
 *  OUTPUTS       : INT16 *OutputData  : Pointer to 8x8 block to hold output.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Inverse quantizes and inverse DCT's input 8x8 block
 *                  with non-zero coeffs only in DC & the first 9 AC coeffs.
 *                  i.e. non-zeros ONLY in the following 10 positions:
 *                  
 *                          x  x  x  x  0  0  0  0
 *                          x  x  x  0  0  0  0  0
 *                          x  x  0  0  0  0  0  0
 *                          x  0  0  0  0  0  0  0
 *                          0  0  0  0  0  0  0  0
 *                          0  0  0  0  0  0  0  0
 *                          0  0  0  0  0  0  0  0
 *                          0  0  0  0  0  0  0  0
 *
 *  SPECIAL NOTES : Output data is in raster, not zig-zag, order.
 *
 ****************************************************************************/
void 
IDct10_G ( INT16 *InputData, INT16 *QuantMatrix, INT16 *OutputData )
{
	int   loop;
	INT32 t1, t2;
	INT16 IntermediateData[64];
	INT32 _A, _B, _C, _D, _Ad, _Bd, _Cd, _Dd, _Ex, _F, _G, _H;
	INT32 _Ed, _Gd, _Add, _Bdd, _Fd, _Hd;

    INT16 *ip = IntermediateData;
	INT16 *op = OutputData;
	
	// dequantize the input 
	dequant_slow10 ( QuantMatrix, InputData, IntermediateData );

	// Inverse DCT on the rows now
	for ( loop=0; loop<4; loop++ )
	{
		// Check for non-zero values
		if ( ip[0] | ip[1] | ip[2] | ip[3] )
		{
			t1 = (INT32)(xC1S7 * ip[1]);
            t1 >>= 16;
			_A = t1; 

			t1 = (INT32)(xC7S1 * ip[1]);
            t1 >>= 16;
			_B = t1 ;

			t1 = (INT32)(xC3S5 * ip[3]);
            t1 >>= 16;
			_C = t1; 

			t2 = (INT32)(xC5S3 * ip[3]);
            t2 >>= 16;
			_D = -t2; 

			t1 = (INT32)(xC4S4 * (_A - _C));
            t1 >>= 16;
			_Ad = t1;

			t1 = (INT32)(xC4S4 * (_B - _D));
            t1 >>= 16;
			_Bd = t1;
			
			_Cd = _A + _C;
			_Dd = _B + _D;

			t1 = (INT32)(xC4S4 * ip[0] );
            t1 >>= 16;
			_Ex = t1;

			_F = t1;
			
			t1 = (INT32)(xC2S6 * ip[2]);
            t1 >>= 16;
			_G = t1; 

			t1 = (INT32)(xC6S2 * ip[2]);
            t1 >>= 16;
			_H = t1 ;
			
			_Ed = _Ex - _G;
			_Gd = _Ex + _G;

			_Add = _F + _Ad;
			_Bdd = _Bd - _H;
			
			_Fd = _F - _Ad;
			_Hd = _Bd + _H;
	
			// Final sequence of operations over-write original inputs.
			ip[0] = (INT16)((_Gd + _Cd )   >> 0);
			ip[7] = (INT16)((_Gd - _Cd )   >> 0);

			ip[1] = (INT16)((_Add + _Hd )  >> 0);
			ip[2] = (INT16)((_Add - _Hd )  >> 0);

			ip[3] = (INT16)((_Ed + _Dd )   >> 0);
			ip[4] = (INT16)((_Ed - _Dd )   >> 0);

			ip[5] = (INT16)((_Fd + _Bdd )  >> 0);
			ip[6] = (INT16)((_Fd - _Bdd )  >> 0);
		}

		ip += 8;			/* next row */
	}

	ip = IntermediateData;

	for ( loop=0; loop<8; loop++ )
	{	
		// Check for non-zero values (bitwise or faster than ||)
		if ( ip[0 * 8] | ip[1 * 8] | ip[2 * 8] | ip[3 * 8] )
		{
			t1 = (INT32)(xC1S7 * ip[1*8]);
            t1 >>= 16;
			_A = t1 ;

			t1 = (INT32)(xC7S1 * ip[1*8]);
            t1 >>= 16;
			_B = t1 ;

			t1 = (INT32)(xC3S5 * ip[3*8]);
            t1 >>= 16;
			_C = t1 ;

			t2 = (INT32)(xC5S3 * ip[3*8]);
            t2 >>= 16;
			_D = - t2;

			t1 = (INT32)(xC4S4 * (_A - _C));
            t1 >>= 16;
			_Ad = t1;

			t1 = (INT32)(xC4S4 * (_B - _D));
            t1 >>= 16;
			_Bd = t1;

			_Cd = _A + _C;
			_Dd = _B + _D;

			t1 = (INT32)(xC4S4 * ip[0*8]);
            t1 >>= 16;
			_Ex = t1;
			_F = t1;
			
			t1 = (INT32)(xC2S6 * ip[2*8]);
            t1 >>= 16;
			_G = t1;

			t1 = (INT32)(xC6S2 * ip[2*8]);
            t1 >>= 16;
			_H = t1;
			
			_Ed = _Ex - _G;
			_Gd = _Ex + _G;

			_Add = _F + _Ad;
			_Bdd = _Bd - _H;
			
			_Fd = _F - _Ad;
			_Hd = _Bd + _H;
	
			_Gd += IdctAdjustBeforeShift;
			_Add += IdctAdjustBeforeShift;
			_Ed += IdctAdjustBeforeShift;
			_Fd += IdctAdjustBeforeShift;

			// Final sequence of operations over-write original inputs.
			op[0*8] = (INT16)((_Gd + _Cd )  >> 4);
			op[7*8] = (INT16)((_Gd - _Cd )  >> 4);

			op[1*8] = (INT16)((_Add + _Hd ) >> 4);
			op[2*8] = (INT16)((_Add - _Hd ) >> 4);

			op[3*8] = (INT16)((_Ed + _Dd )  >> 4);
			op[4*8] = (INT16)((_Ed - _Dd )  >> 4);

			op[5*8] = (INT16)((_Fd + _Bdd ) >> 4);
			op[6*8] = (INT16)((_Fd - _Bdd ) >> 4);
		}
		else
		{
			op[0*8] = 0;
			op[7*8] = 0;
			op[1*8] = 0;
			op[2*8] = 0;
			op[3*8] = 0;
			op[4*8] = 0;
			op[5*8] = 0;
			op[6*8] = 0;
		}

		ip++;	// next column
        op++;
	}
}

/****************************************************************************
 * 
 *  ROUTINE       : IDct1
 *
 *  INPUTS        : INT16 *InputData   : Pointer to 8x8 quantized DCT coefficients.
 *                  INT16 *QuantMatrix : Pointer to 8x8 quantization matrix.
 *
 *  OUTPUTS       : INT16 *OutputData  : Pointer to 8x8 block to hold output.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Inverse DCT's input 8x8 block with only one non-zero
 *                  coeff in the DC position:
 *                  
 *                          x   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *                          0   0   0  0  0  0  0  0
 *
 *  SPECIAL NOTES : Output data is in raster, not zig-zag, order.
 *
 ****************************************************************************/
void 
IDct1_G ( INT16 *InputData, INT16 *QuantMatrix, INT16 *OutputData )
{
    INT32 loop;
	INT16 OutD;
	
	OutD = (INT16)((INT32)(InputData[0]*QuantMatrix[0]+15)>>5);

	loop = 0;
    do
    {
		OutputData[loop] = OutD;
    }while(++loop < 64);
}

