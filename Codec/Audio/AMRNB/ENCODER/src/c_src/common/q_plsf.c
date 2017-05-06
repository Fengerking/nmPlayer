/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : q_plsf.c
*      Purpose          : common part (init, exit, reset) of LSF quantization
*                         module (rest in voAMRNBEnc_Q_plsf_3.c and q_plsf_5.c)
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "q_plsf.h"
const char q_plsf_id[] = "@(#)$Id $" q_plsf_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/

/*
**************************************************************************
*
*  Function    : voAMRNBEnc_Q_plsf_init
*  Purpose     : Allocates memory and initializes state variables
*
**************************************************************************
*/
int voAMRNBEnc_Q_plsf_init (Q_plsfState **state, VO_MEM_OPERATOR *pMemOP)
{
	Q_plsfState* s;
	if (state == (Q_plsfState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (Q_plsfState *) voAMRNBEnc_mem_malloc(pMemOP, sizeof(Q_plsfState), 32)) == NULL){
		return -1;
	}
	voAMRNBEnc_Q_plsf_reset(s);
	*state = s;
	return 0;
}

/*
**************************************************************************
*
*  Function    : voAMRNBEnc_Q_plsf_reset
*  Purpose     : Resets state memory
*
**************************************************************************
*/
int voAMRNBEnc_Q_plsf_reset (Q_plsfState *state)
{
	Word16 i;
	if (state == (Q_plsfState *) NULL){
		return -1;
	}
	for ( i = 0; i < M; i++)
		state->past_rq[i] = 0;
	return 0;
}

/*
**************************************************************************
*
*  Function    : voAMRNBEnc_Q_plsf_exit
*  Purpose     : The memory used for state memory is freed
*
**************************************************************************
*/
void voAMRNBEnc_Q_plsf_exit (Q_plsfState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	/* deallocate memory */
	voAMRNBEnc_mem_free(pMemOP, *state);
	*state = NULL;
	return;
}
