/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : d4_17pf.c
*      Purpose          : Algebraic codebook decoder
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "d4_17pf.h"
const char d4_17pf_c_id[] = "@(#)$Id $" d4_17pf_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define NB_PULSE 4
#include "gray.tab"

/*********************************************************************************
*                         PUBLIC PROGRAM CODE
*********************************************************************************/
/*************************************************************************
*
*  FUNCTION:  decod_ACELP()
*
*  PURPOSE:   Algebraic codebook decoder
*
*************************************************************************/
void decode_4i40_17bits(
						Word16 sign,   /* i : signs of 4 pulses.                       */
						Word16 index,  /* i : Positions of the 4 pulses.               */
						Word16 cod[]   /* o : algebraic (fixed) codebook excitation    */
)
{
	int    ix,jx;
	Word16 i, j;
	Word16 pos[NB_PULSE];
	/* Decode the positions */
	i = index & 7;                                       
	i = voAMRNBDecdgray[i];                                        
	pos[0] = i + (i << 2);   /* pos0 =i*5 */       

	index = (index>> 3);
	i = index & 7;                                       
	i = voAMRNBDecdgray[i];                                        

	i = i + (i << 2);        /* pos1 =i*5+1 */
	pos[1] = i + 1;                                  

	index = (index>> 3);
	i = index & 7;                                       
	i = voAMRNBDecdgray[i];                                        

	i = (i + (i << 2));        /* pos2 =i*5+1 */
	pos[2] = i + 2;                                  

	index = (index>> 3);
	j = index & 1;                                       
	index = (index>> 1);
	i = index & 7;                                       
	i = voAMRNBDecdgray[i];                                        

	i = i + (i << 2);        /* pos3 =i*5+3+j */
	i += 3;
	pos[3] = i + j;                                  
	/* decode the signs  and build the codeword */
	for (ix = 0; ix< L_SUBFR; ix++) {
		cod[ix] = 0;                                     
	}

	for (jx = 0; jx < NB_PULSE; jx++) {
		i = sign & 1;                                    
		sign = (sign>> 1);
		if (i != 0) {
			cod[pos[jx]] = 8191;                         
		} else {
			cod[pos[jx]] = -8192;                        
		}
	}
	return;
}
