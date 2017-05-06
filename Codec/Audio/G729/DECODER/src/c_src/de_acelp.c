/* ITU-T G.729 Software Package Release 2 (November 2006) */
/*
ITU-T G.729A Speech Coder    ANSI-C Source Code
Version 1.1    Last modified: September 1996

Copyright (c) 1996,
AT&T, France Telecom, NTT, Universite de Sherbrooke, Lucent Technologies
All rights reserved.
*/

/*-----------------------------------------------------------*
*  Function  Decod_ACELP()                                  *
*  ~~~~~~~~~~~~~~~~~~~~~~~                                  *
*   Algebraic codebook decoder.                             *
*----------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"

void Decod_ACELP(
				 Word16 sign,      /* (i)     : signs of 4 pulses.                       */
				 Word16 index,     /* (i)     : Positions of the 4 pulses.               */
				 Word16 cod[]      /* (o) Q13 : algebraic (fixed) codebook excitation    */
)
{
	Word16 i, j, tmp;
	Word16 pos[4];
	/* Decode the positions */
	i = index & (Word16)7;
	tmp = i << 2;
	pos[0] = i + tmp;                       /* pos0 =i*5 */

	index >>= 3;
	i      = index & (Word16)7;
	tmp = i << 2;
	pos[1] = tmp + i +1;                    /* pos1 =i*5+1 */

	index >>= 3;
	i      = index & (Word16)7;
	tmp = i << 2;
	pos[2] = tmp + i +2;                    /* pos2 =i*5+2 */

	index >>= 3;
	j      = index & (Word16)1;
	index >>= 1;
	i      = index & (Word16)7;
	tmp = i << 2;
	pos[3] = tmp + i + 3 + j;               /* pos3 =i*5+3+j */
	/* decode the signs  and build the codeword */
	for (i=0; i<L_SUBFR; i++) {
		cod[i] = 0;
	}
	for (j=0; j<4; j++)
	{
		i = sign & (Word16)1;
		sign >>= 1;
		if (i != 0) {
			cod[pos[j]] = 8191;      /* Q13 +1.0 */
		}
		else {
			cod[pos[j]] = -8192;     /* Q13 -1.0 */
		}
	}
	return;
}