/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : prm2bits.c
*      Purpose          : Converts the encoder parameter vector into a
*                       : vector of serial bits.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "prm2bits.h"

const char prm2bits_id[] = "@(#)$Id $" prm2bits_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "mode.h"
#include <stdlib.h>
#include <stdio.h>
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "bitno.tab"
#define MASK      0x0001
/*
********************************************************************************
*                         LOCAL PROGRAM CODE
********************************************************************************
*/

/*************************************************************************
*
*  FUNCTION:  Int2bin
*
*  PURPOSE:  convert integer to binary and write the bits to the array
*            bitstream[]. The most significant bits are written first.
*
*************************************************************************/

__voinline void Int2bin (
							  Word16 value,       /* input : value to be converted to binary      */
							  Word16 no_of_bits,  /* input : number of bits associated with value */
							  Word16 *bitstream   /* output: address where bits are written       */
							  )
{
	Word16 *pt_bitstream, bit;
	nativeInt i;

	pt_bitstream = &bitstream[no_of_bits];      

	for (i = 0; i < no_of_bits; i++)
	{
		bit = value & MASK; 
		*--pt_bitstream = bit;
		/*
		if (bit == 0)
		{
		*--pt_bitstream = BIT_0;            
		}
		else
		{
		*--pt_bitstream = BIT_1;            
		}*/
		value = (value>> 1);
	}
}

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:    Prm2bits
*
*  PURPOSE:     converts the encoder parameter vector into a vector of serial
*               bits.
*
*  DESCRIPTION: depending on the mode, different numbers of parameters
*               (with differing numbers of bits) are processed. Details
*               are found in bitno.tab
*
*************************************************************************/
void voAMRNBEnc_Prm2bits (
			   enum Mode mode,    /* i : AMR mode                                      */
			   Word16 prm[],      /* i : analysis parameters (size <= MAX_PRM_SIZE)    */
			   Word16 bits[]      /* o : serial bits         (size <= MAX_SERIAL_SIZE) */
)
{
	Word32  i;
	Word32  j;
	Word32  no_of_bits;
	Word32  value;
	Word16 *pt_bitstream;
#if ONLY_ENCODE_122
	nativeInt size =  prmno[mode];
	const Word8* BITS = bitno_MR122;
#else//ONLY_ENCODE_122
	Word32 size =  prmno[mode];
	const Word8* BITS = bitno[mode];
#endif//ONLY_ENCODE_122

	for (i = 0; i <size; i++)
	{
		//Int2bin (prm[i], BITS[i], bits);
		no_of_bits = BITS[i];
		pt_bitstream = &bits[no_of_bits]; 
		value = prm[i];
		for (j = 0; j < no_of_bits; j++)
		{
			*--pt_bitstream = value & MASK;
			value = (value>> 1);
		}
		bits += no_of_bits;
	}      
	return;
}
