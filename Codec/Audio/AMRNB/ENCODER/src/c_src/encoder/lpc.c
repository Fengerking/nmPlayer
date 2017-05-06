/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : lpc.c
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "lpc.h"
const char lpc_id[] = "@(#)$Id $" lpc_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "autocorr.h"
#include "lag_wind.h"
#include "levinson.h"
#include "cnst.h"
#include "mode.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#include "window.tab"
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   voAMRNBEnc_lpc_init
*
**************************************************************************
*/
int voAMRNBEnc_lpc_init (lpcState **state, VO_MEM_OPERATOR *pMemOP)
{
	lpcState* s;
	if (state == (lpcState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (lpcState *)voAMRNBEnc_mem_malloc(pMemOP, sizeof(lpcState), 32)) == NULL){
		return -1;
	}
	s->levinsonSt = NULL;
	/* Init sub states */
	if (voAMRNBEnc_Levinson_init(&s->levinsonSt, pMemOP)) {
		voAMRNBEnc_lpc_exit(&s, pMemOP);
		return -1;
	}
	voAMRNBEnc_lpc_reset(s);
	*state = s;
	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_lpc_reset
*
**************************************************************************
*/
int voAMRNBEnc_lpc_reset (lpcState *state)
{
	if (state == (lpcState *) NULL){
		fprintf(stderr, "voAMRNBEnc_lpc_reset: invalid parameter\n");
		return -1;
	}
	voAMRNBEnc_Levinson_reset(state->levinsonSt);
	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_lpc_exit
*
**************************************************************************
*/
void voAMRNBEnc_lpc_exit (lpcState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	voAMRNBEnc_Levinson_exit(&(*state)->levinsonSt, pMemOP);
	/* deallocate memory */
	voAMRNBEnc_mem_free(pMemOP, *state);
	*state = NULL;
	return;
}

int voAMRNBEnc_lpc(
		cod_amrState *amr_st,
		enum Mode    mode,
		Word16       a[])
{
    Word16   rLow[MP1];                                    /* Autocorrelations low and hi      */
	Word16   rHigh[MP1];  
	Word16   rc[4];                                        /* First 4 reflection coefficients Q15 */
	Word16   *x = amr_st->p_window;
	Word16   *x_12k2 = amr_st->p_window_12k2;
    lpcState *st = amr_st->lpcSt;

	/* No fixed Q value but normalized  */
	/* so that overflow is avoided      */
	if ( (mode == MR122))
	{
		/* Autocorrelations */
#ifdef ARMv6_OPT  //have error
		Autocorr_asm(x_12k2,rHigh, rLow, window_160_80);   
#else
		voAMRNBEncAutocorr(x_12k2, M, rHigh, rLow, window_160_80);  
#endif
		/* Lag windowing    */
		voAMRNBEnc_Lag_window(M, rHigh, rLow);    //baseline verion discard the function                                 
		/* Levinson Durbin  */
		voAMRNBEnc_Levinson(st->levinsonSt, rHigh, rLow, &a[MP1], rc);     

		/* Autocorrelations */
#ifdef ARMv6_OPT  //have error
        Autocorr_asm(x_12k2,rHigh, rLow, window_232_8);  
#else
		voAMRNBEncAutocorr(x_12k2, M, rHigh, rLow, window_232_8);   
#endif
		/* Lag windowing    */
		voAMRNBEnc_Lag_window(M, rHigh, rLow);                                  
		/* Levinson Durbin  */
		voAMRNBEnc_Levinson(st->levinsonSt, rHigh, rLow, &a[33], rc); 
	}
	else
	{
		/* Autocorrelations */
#ifdef ARMv6_OPT //have error
		Autocorr_asm(x,rHigh, rLow, window_200_40);
#else
        voAMRNBEncAutocorr(x, M, rHigh, rLow, window_200_40);
#endif
		/* Lag windowing    */
		voAMRNBEnc_Lag_window(M, rHigh, rLow);                                   
		/* Levinson Durbin  */
		voAMRNBEnc_Levinson(st->levinsonSt, rHigh, rLow, &a[33], rc); 
	}
	return 0;
}
