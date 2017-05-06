/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : pre_proc.c
*      Purpose          : Preprocessing of input speech.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "pre_proc.h"
const char pre_proc_id[] = "@(#)$Id $" pre_proc_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
/* filter coefficients (fc = 80 Hz, coeff. b[] is divided by 2) */
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   voAMRNBEnc_Pre_Process_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int voAMRNBEnc_Pre_Process_init(Pre_ProcessState **state, VO_MEM_OPERATOR *pMemOP)
{
	Pre_ProcessState* s;
	if (state == (Pre_ProcessState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (Pre_ProcessState *)voAMRNBEnc_mem_malloc(pMemOP, sizeof(Pre_ProcessState), 32)) == NULL){
		return -1;
	}
	voAMRNBEnc_Pre_Process_reset(s);
	*state = s;

	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_Pre_Process_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int voAMRNBEnc_Pre_Process_reset (Pre_ProcessState *state)
{
	if (state == (Pre_ProcessState *) NULL){
		return -1;
	}
	state->y2_hi = 0;
	state->y2_lo = 0;
	state->y1_hi = 0;
	state->y1_lo = 0;
	state->x0 = 0;
	state->x1 = 0;
	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_Pre_Process_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void voAMRNBEnc_Pre_Process_exit(Pre_ProcessState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	/* deallocate memory */
	voAMRNBEnc_mem_free(pMemOP, *state);
	*state = NULL;
	return;
}

/*************************************************************************
*
*  FUNCTION:  Pre_Process()
*
*  PURPOSE: Preprocessing of input speech.
*
*  DESCRIPTION:
*     - 2nd order high pass filtering with cut off frequency at 80 Hz.
*     - Divide input by two.                                                                      
* Algorithm:                                                             
*                                                                        
*  y[i] = b[0]*x[i]/2 + b[1]*x[i-1]/2 + b[2]*x[i-2]/2                    
*                     + a[1]*y[i-1]   + a[2]*y[i-2];                     
*                                                                        
*                                                                        
*  Input is divided by two in the filtering process.
*
*************************************************************************/
static const Word16 b[3] = {1899, -3798, 1899};
static const Word16 a[3] = {4096, 7807, -3733};
int voAMRNBEnc_Pre_Process (
				 Pre_ProcessState *st,
				 Word16 signal[], /* input/output signal */
				 Word16 lg)       /* lenght of signal    */
#ifdef C_OPT     //bit match
{
	Word32   L_tmp, x2, i;
	Word16   a_x0 = st->x0;
	Word16   a_x1 = st->x1;
	Word16   by1_hi = st->y1_hi;
	Word16   by1_lo = st->y1_lo;
	Word16   by2_hi = st->y2_hi;
	Word16   by2_lo = st->y2_lo;

	for (i = 0; i < lg; i++)
	{
		x2 = a_x1;                   
		a_x1 = a_x0;               
		a_x0 = signal[i];            
		L_tmp =  (by1_hi*7807 + (by1_lo*7807>>15));
		L_tmp += (by2_hi*(-3733) + (by2_lo*(-3733)>>15));
		L_tmp += (a_x0 + x2)*1899;
		L_tmp += a_x1*(-3798);

		L_tmp = L_shl2(L_tmp, 4);
		signal[i] = (Word16)((L_tmp + 0x8000L)>>16);

	    by2_hi = by1_hi;            
		by2_lo = by1_lo;   
		by1_hi  = L_tmp >> 16;
		by1_lo = (L_tmp - (by1_hi<<16))>>1;
	}
	st->x0 = a_x0;
	st->x1 = a_x1;
	st->y1_hi = by1_hi;
	st->y1_lo = by1_lo;
	st->y2_hi = by2_hi;
	st->y2_lo = by2_lo;

	return 0;
}
#else
{
	Word16 i, x2;
	Word32 L_tmp;
	for (i = 0; i < lg; i++)
	{
		x2 = st->x1;                  
		st->x1 = st->x0;              
		st->x0 = signal[i];           

		/*  y[i] = b[0]*x[i]/2 + b[1]*x[i-1]/2 + b140[2]*x[i-2]/2  */
		/*                     + a[1]*y[i-1] + a[2] * y[i-2];      */

		L_tmp = Mpy_32_16 (st->y1_hi, st->y1_lo, a[1]);
		L_tmp = L_add (L_tmp, Mpy_32_16 (st->y2_hi, st->y2_lo, a[2]));
		L_tmp = L_mac (L_tmp, st->x0, b[0]);
		L_tmp = L_mac (L_tmp, st->x1, b[1]);
		L_tmp = L_mac (L_tmp, x2, b[2]);
		L_tmp = L_shl (L_tmp, 3);
		signal[i] = vo_round (L_tmp); 
		st->y2_hi = st->y1_hi;     
		st->y2_lo = st->y1_lo;     
		L_Extract (L_tmp, &st->y1_hi, &st->y1_lo);
	}
	return 0;
}
#endif
