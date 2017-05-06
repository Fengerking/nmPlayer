/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : d3_14pf.c
*      Purpose          : Algebraic codebook decoder
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "d3_14pf.h"
const char d3_14pf_c_id[] = "@(#)$Id $" d3_14pf_h;

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
#define NB_PULSE 3           /* number of pulses  */

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:  decode_3i40_14bits (decod_ACELP())
*
*  PURPOSE:   Algebraic codebook decoder
*
*************************************************************************/

void decode_3i40_14bits(
						Word16 sign,   /* i : signs of 3 pulses.                       */
						Word16 index,  /* i : Positions of the 3 pulses.               */
						Word16 cod[]   /* o : algebraic (fixed) codebook excitation    */
)
{
	Word16 i, j;
	Word16 pos[NB_PULSE];
	int    ix,jx;
	/* Decode the positions */
	i = index & 7;                                       
	pos[0] = i + (i << 2);   /* pos0 =i*5 */       
	index = (index>> 3);
	j = index & 1;                                       
	index = (index>> 1);
	i = index & 7;                                       

	i = i + (i << 2);        /* pos1 =i*5+1+j*2 */
	i += 1;
	j += 1;     
	pos[1] = i + j;                                  

	index = (index>> 3);
	j = index & 1;                                       
	index = (index>> 1);
	i = index & 7;                                       

	i = i + (i << 2);        /* pos2 =i*5+2+j*2 */
	i = (i + 2); 
	j <<= 1; 
	pos[2] = i + j;                                  

	/* decode the signs  and build the codeword */
	for (ix = 0; ix < L_SUBFR; ix++) {
		cod[ix] = 0;                                     
	}
	for (jx = 0; jx < NB_PULSE; jx++) {
		i = sign & 1;                                    
		sign = (sign>> 1);
		if (i > 0) {
			cod[pos[jx]] = 8191;                         
		} else {
			cod[pos[jx]] = -8192;                        
		}
	}
	return;
}
