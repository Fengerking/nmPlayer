/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : lsp_lsf.c
*      Purpose          : Lsp_lsf:   Transformation lsp to lsf
*                       : Lsf_lsp:   Transformation lsf to lsp
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "lsp_lsf.h"
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "lsp_lsf.tab"          /* Look-up table for transformations */

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*   FUNCTIONS:  Lsp_lsf and Lsf_lsp
*
*   PURPOSE:
*      Lsp_lsf:   Transformation lsp to lsf
*      Lsf_lsp:   Transformation lsf to lsp
*
*   DESCRIPTION:
*         lsp[i] = cos(2*pi*lsf[i]) and lsf[i] = arccos(lsp[i])/(2*pi)
*
*   The transformation from lsp[i] to lsf[i] and lsf[i] to lsp[i] are
*   approximated by a look-up table and interpolation.
*
*************************************************************************/
void voAMRNBEnc_Lsf_lsp (
			  Word16 lsf[],       /* (i) : lsf[m] normalized (range: 0.0<=val<=0.5) */
			  Word16 lsp[],       /* (o) : lsp[m] (range: -1<=val<1)                */
			  Word16 m            /* (i) : LPC order                                */
			  )
{
	int  i, ind, offset;
	Word32 L_tmp;

	for (i = 0; i < 10; i++)
	{
		ind =(lsf[i]>> 8);                         /* ind = b8-b15 of lsf[i] */
		offset = lsf[i] & 0x00ff;                  /* offset = b0-b7  of lsf[i] */
		/* lsp[i] = table[ind]+ ((table[ind+1]-table[ind])*offset) / 256 */
		L_tmp = ((table[ind + 1] - table[ind])* offset)<<1;
		lsp[i] = add (table[ind], extract_l((L_tmp>> 9)));
	}
	return;
}
void voAMRNBEnc_Lsp_lsf(
			  Word16 lsp[],       /* (i)  : lsp[m] (range: -1<=val<1)                */
			  Word16 lsf[],       /* (o)  : lsf[m] normalized (range: 0.0<=val<=0.5) */
			  Word16 m            /* (i)  : LPC order                                */
			  )
{
	Word32  ind = 63;    /* begin at end of table -1 */
	Word32  i, L_tmp;

	for (i = 9; i >= 0; i--)
	{
		/* find value in table that is just greater than lsp[i] */
		while (table[ind] < lsp[i])
		{
			ind--;
		}
		/* acos(lsp[i])= ind*256 + ( ( lsp[i]-table[ind] ) * slope[ind] )/4096 */
		L_tmp = ((lsp[i]- table[ind]) * slope[ind])<<1;         /*(lsp[i]-table[ind])*slope[ind])>>12*/
		lsf[i] = (L_tmp << 3)>>16;                              // lsf[i] = (lsf[i], + (ind<< 8));
		lsf[i] = lsf[i] + (ind<< 8);  
	}
	return;
}
