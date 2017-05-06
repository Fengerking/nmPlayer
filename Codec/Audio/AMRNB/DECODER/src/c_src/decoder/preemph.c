/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : preemph.c
*      Purpose          : Preemphasis filtering
*      Description      : Filtering through 1 - g z^-1 
*
********************************************************************************
*/


/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "preemph.h"
const char preemph_id[] = "@(#)$Id $" preemph_h;

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
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   Post_Filter_init
*  Purpose:    Allocates memory for filter structure and initializes
*              state memory
*
**************************************************************************
*/
int preemphasis_init (preemphasisState **state, VO_MEM_OPERATOR *pMemOP)
{
	preemphasisState* s;
	if (state == (preemphasisState **) NULL){
		return -1;
	}
	*state = NULL;

	/* allocate memory */
	if ((s= (preemphasisState *) voAMRNBDecmem_malloc(pMemOP, sizeof(preemphasisState), 32)) == NULL){
		return -1;
	}
	preemphasis_reset(s);
	*state = s;
	return 0;
}

/*************************************************************************
*
*  Function:   preemphasis_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int preemphasis_reset (preemphasisState *state)
{
	if (state == (preemphasisState *) NULL){
		return -1;
	}
	state->mem_pre = 0;
	return 0;
}

/*************************************************************************
*
*  Function:   preemphasis_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void preemphasis_exit (preemphasisState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	/* deallocate memory */
	voAMRNBDecmem_free(pMemOP, *state);
	*state = NULL;
	return;
}

/*
**************************************************************************
*  Function:  preemphasis
*  Purpose:   Filtering through 1 - g z^-1 
*
**************************************************************************
*/
int preemphasis (
				 preemphasisState *st, /* (i/o)  : preemphasis filter state    */
				 Word16 *signal, /* (i/o)   : input signal overwritten by the output */
				 Word16 g,       /* (i)     : preemphasis coefficient                */
				 Word16 L        /* (i)     : size of filtering                      */
				 )
{
	Word16 *p1, *p2, temp;
	nativeInt i;
	p1 = signal + L - 1;                      
	p2 = p1 - 1;                              
	temp = *p1;                               
	for (i = 0; i <= L - 2; i++)
	{
		//*p1 = sub(*p1, mult(g, *p2--));
		*p1 = (*p1 - ((g * (*p2--))>>15));    
		p1--;
	}
	//*p1 = sub(*p1, mult(g, st->mem_pre));
	*p1 = (*p1 - ((g * st->mem_pre)>>15));    
	st->mem_pre = temp;                       
	return 0;
}
