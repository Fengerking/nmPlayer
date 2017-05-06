/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : d_plsf.c
*      Purpose          : common part (init, exit, reset) of LSF decoder
*                         module (rest in d_plsf_3.c and d_plsf_5.c)
*
*****************************************************************************
*/


/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "d_plsf.h"
const char d_plsf_id[] = "@(#)$Id $" d_plsf_h; 
/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "copy.h"
#include "q_plsf_5.tab"
/*
*--------------------------------------------------*
* Constants (defined in cnst.h)                    *
*--------------------------------------------------*
*  M                    : LPC order
*--------------------------------------------------*
*/

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*
**************************************************************************
*
*  Function    : D_plsf_init
*  Purpose     : Allocates and initializes state memory
*
**************************************************************************
*/
int D_plsf_init (D_plsfState **state,VO_MEM_OPERATOR *pMemOP)
{
	D_plsfState* s;
	if (state == (D_plsfState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (D_plsfState *) voAMRNBDecmem_malloc(pMemOP, sizeof(D_plsfState), 32)) == NULL){
		return -1;
	}
	D_plsf_reset(s);
	*state = s;
	return 0;
}
/*
**************************************************************************
*
*  Function    : D_plsf_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
int D_plsf_reset (D_plsfState *state)
{
	Word16 i;
	if (state == (D_plsfState *) NULL){
		return -1;
	}
	for (i = 0; i < M; i++){
		state->past_r_q[i] = 0;             /* Past quantized prediction error */
	}
	/* Past dequantized lsfs */
	voAMRNBDecCopy(voAMRNBDecmean_lsf, &state->past_lsf_q[0], M);

	return 0;
}
/*
**************************************************************************
*
*  Function    : D_plsf_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void D_plsf_exit (D_plsfState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return; 
	voAMRNBDecmem_free(pMemOP, *state);	/* deallocate memory */
	*state = NULL;
	return;
}
