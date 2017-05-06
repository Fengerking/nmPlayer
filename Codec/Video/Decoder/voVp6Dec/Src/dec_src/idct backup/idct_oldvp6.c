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

#if 0
#define LIMIT_x(x) 	(unsigned char) ( (x) < 0 ? 0 : ( (x) <= 255 ? (x) : 255 ) )
//#define LIMIT_x(x) 	x
#else

    static FORCEINLINE
    UINT8 LIMIT_x(INT16 _x)
    {
        if( _x & 0xff00 )
        {
    #if 0
            if( _x & 0x8000 )
                _x = 0;
            else
                _x = 255;
    #else
            /* if negative, 0xff + 1 = 0x100 */
            /* the lower 8bits will then become zero */
            _x = 255 + ( (_x >> 15) & 0x1 );
    #endif
        }
        return (UINT8)_x;
    }
#endif
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
void  IDCTvp6_Block8x8_c(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 SrcStride )
{
	int   loop;
	INT32 t1, t2;
	INT32 _A, _B, _C, _D, _Ad, _Bd, _Cd, _Dd, _Ex, _F, _G, _H;
	INT32 _Ed, _Gd, _Add, _Bdd, _Fd, _Hd;
    INT16 *ip = Block;
	UINT8 *op = Dest;
	UINT8 *ipsrc = Src;	

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

	ip = Block;

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
			ip[0*8] = (INT16)((_Gd + _Cd )  >> 4);
			ip[7*8] = (INT16)((_Gd - _Cd )  >> 4);

			ip[1*8] = (INT16)((_Add + _Hd ) >> 4);
			ip[2*8] = (INT16)((_Add - _Hd ) >> 4);

			ip[3*8] = (INT16)((_Ed + _Dd )  >> 4);
			ip[4*8] = (INT16)((_Ed - _Dd )  >> 4);

			ip[5*8] = (INT16)((_Fd + _Bdd ) >> 4);
			ip[6*8] = (INT16)((_Fd - _Bdd ) >> 4);
		}
		else
		{
			ip[0*8] = 0;
			ip[7*8] = 0;
			ip[1*8] = 0;
			ip[2*8] = 0;
			ip[3*8] = 0;
			ip[4*8] = 0;
			ip[5*8] = 0;
			ip[6*8] = 0;					
		}

		ip++;			// next column
	}
	ip = Block;	

	for ( loop=0; loop<8; loop++ )
	{	
		if (!Src)
		{
        		Dest[0] = (char) LIMIT_x( (INT16) ( ip[0] + 128 ) );
       			Dest[1] = (char) LIMIT_x( (INT16) ( ip[1] + 128 ) );
        		Dest[2] = (char) LIMIT_x( (INT16) ( ip[2] + 128 ) );
        		Dest[3] = (char) LIMIT_x( (INT16) ( ip[3] + 128 ) );

        		Dest[4] = (char) LIMIT_x( (INT16) ( ip[4] + 128 ) );
        		Dest[5] = (char) LIMIT_x( (INT16) ( ip[5] + 128 ) );
        		Dest[6] = (char) LIMIT_x( (INT16) ( ip[6] + 128 ) );
        		Dest[7] = (char) LIMIT_x( (INT16) ( ip[7] + 128 ) );
		}
		else
		{
        		Dest[0] = (char) LIMIT_x( (INT16) ( ip[0] + ipsrc[0] ) );
       			Dest[1] = (char) LIMIT_x( (INT16) ( ip[1] + ipsrc[1] ) );
        		Dest[2] = (char) LIMIT_x( (INT16) ( ip[2] + ipsrc[2] ) );
        		Dest[3] = (char) LIMIT_x( (INT16) ( ip[3] + ipsrc[3] ) );

        		Dest[4] = (char) LIMIT_x( (INT16) ( ip[4] + ipsrc[4] ) );
        		Dest[5] = (char) LIMIT_x( (INT16) ( ip[5] + ipsrc[5] ) );
        		Dest[6] = (char) LIMIT_x( (INT16) ( ip[6] + ipsrc[6] ) );
        		Dest[7] = (char) LIMIT_x( (INT16) ( ip[7] + ipsrc[7] ) );
		}
		ip+= 8;			
		Dest+= DestStride;
		ipsrc+= SrcStride;					
	}	

	memset(Block, 0, 64*sizeof(Q_LIST_ENTRY));
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
void  IDCTvp6_Block4x4_c(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 SrcStride )
{
	int   loop;
	INT32 t1, t2;
	INT32 _A, _B, _C, _D, _Ad, _Bd, _Cd, _Dd, _Ex, _F, _G, _H;
	INT32 _Ed, _Gd, _Add, _Bdd, _Fd, _Hd;
    INT16 *ip = Block;
	UINT8 *op = Dest;
	UINT8 *ipsrc = Src;	
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

	ip = Block;

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
			ip[0*8] = (INT16)((_Gd + _Cd )  >> 4);
			ip[7*8] = (INT16)((_Gd - _Cd )  >> 4);

			ip[1*8] = (INT16)((_Add + _Hd ) >> 4);
			ip[2*8] = (INT16)((_Add - _Hd ) >> 4);

			ip[3*8] = (INT16)((_Ed + _Dd )  >> 4);
			ip[4*8] = (INT16)((_Ed - _Dd )  >> 4);

			ip[5*8] = (INT16)((_Fd + _Bdd ) >> 4);
			ip[6*8] = (INT16)((_Fd - _Bdd ) >> 4);
		}
		else
		{
			ip[0*8] = 0;
			ip[7*8] = 0;
			ip[1*8] = 0;
			ip[2*8] = 0;
			ip[3*8] = 0;
			ip[4*8] = 0;
			ip[5*8] = 0;
			ip[6*8] = 0;
		}

		ip++;	// next column
	}

	ip = Block;	

	for ( loop=0; loop<8; loop++ )
	{	
		if (!Src)
		{
        		Dest[0] = (char) LIMIT_x( (INT16) ( ip[0] + 128 ) );
       			Dest[1] = (char) LIMIT_x( (INT16) ( ip[1] + 128 ) );
        		Dest[2] = (char) LIMIT_x( (INT16) ( ip[2] + 128 ) );
        		Dest[3] = (char) LIMIT_x( (INT16) ( ip[3] + 128 ) );

        		Dest[4] = (char) LIMIT_x( (INT16) ( ip[4] + 128 ) );
        		Dest[5] = (char) LIMIT_x( (INT16) ( ip[5] + 128 ) );
        		Dest[6] = (char) LIMIT_x( (INT16) ( ip[6] + 128 ) );
        		Dest[7] = (char) LIMIT_x( (INT16) ( ip[7] + 128 ) );
		}
		else
		{
        		Dest[0] = (char) LIMIT_x( (INT16) ( ip[0] + ipsrc[0] ) );
       			Dest[1] = (char) LIMIT_x( (INT16) ( ip[1] + ipsrc[1] ) );
        		Dest[2] = (char) LIMIT_x( (INT16) ( ip[2] + ipsrc[2] ) );
        		Dest[3] = (char) LIMIT_x( (INT16) ( ip[3] + ipsrc[3] ) );

        		Dest[4] = (char) LIMIT_x( (INT16) ( ip[4] + ipsrc[4] ) );
        		Dest[5] = (char) LIMIT_x( (INT16) ( ip[5] + ipsrc[5] ) );
        		Dest[6] = (char) LIMIT_x( (INT16) ( ip[6] + ipsrc[6] ) );
        		Dest[7] = (char) LIMIT_x( (INT16) ( ip[7] + ipsrc[7] ) );
		}
		ip+= 8;			
		Dest+= DestStride;
		ipsrc+= SrcStride;					
	}	

	memset(Block, 0, 64*sizeof(Q_LIST_ENTRY));
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
void  IDCTvp6_Block1x1_c(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 SrcStride )
{
    INT32 loop;
	INT16 OutD;
    INT16 *ip = Block;
	UINT8 *op = Dest;
	UINT8 *ipsrc = Src;	

	OutD = (INT16)((INT32)(Block[0]+15)>>5);


	loop = 0;
    do
    {
		ip[loop] = OutD;
    }while(++loop < 64);

	ip = Block;	

	for ( loop=0; loop<8; loop++ )
	{	
		if (!Src)
		{
        		Dest[0] = (char) LIMIT_x( (INT16) ( ip[0] + 128 ) );
       			Dest[1] = (char) LIMIT_x( (INT16) ( ip[1] + 128 ) );
        		Dest[2] = (char) LIMIT_x( (INT16) ( ip[2] + 128 ) );
        		Dest[3] = (char) LIMIT_x( (INT16) ( ip[3] + 128 ) );

        		Dest[4] = (char) LIMIT_x( (INT16) ( ip[4] + 128 ) );
        		Dest[5] = (char) LIMIT_x( (INT16) ( ip[5] + 128 ) );
        		Dest[6] = (char) LIMIT_x( (INT16) ( ip[6] + 128 ) );
        		Dest[7] = (char) LIMIT_x( (INT16) ( ip[7] + 128 ) );
		}
		else
		{
        		Dest[0] = (char) LIMIT_x( (INT16) ( ip[0] + ipsrc[0] ) );
       			Dest[1] = (char) LIMIT_x( (INT16) ( ip[1] + ipsrc[1] ) );
        		Dest[2] = (char) LIMIT_x( (INT16) ( ip[2] + ipsrc[2] ) );
        		Dest[3] = (char) LIMIT_x( (INT16) ( ip[3] + ipsrc[3] ) );

        		Dest[4] = (char) LIMIT_x( (INT16) ( ip[4] + ipsrc[4] ) );
        		Dest[5] = (char) LIMIT_x( (INT16) ( ip[5] + ipsrc[5] ) );
        		Dest[6] = (char) LIMIT_x( (INT16) ( ip[6] + ipsrc[6] ) );
        		Dest[7] = (char) LIMIT_x( (INT16) ( ip[7] + ipsrc[7] ) );
		}
		ip+= 8;			
		Dest+= DestStride;
		ipsrc+= SrcStride;					
	}	
	memset(Block, 0, 64*sizeof(Q_LIST_ENTRY));
}

