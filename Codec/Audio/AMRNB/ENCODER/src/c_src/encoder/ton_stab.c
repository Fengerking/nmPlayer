/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : ton_stab.c
*
*****************************************************************************
*/

/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "ton_stab.h"
const char ton_stab_id[] = "@(#)$Id $" ton_stab_h;

/*
*****************************************************************************
*                         INCLUDE FILES
*****************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "cnst.h"
#include "set_zero.h"
#include "copy.h"

/*
*****************************************************************************
*                         LOCAL VARIABLES AND TABLES
*****************************************************************************
*/

/*
*****************************************************************************
*                         PUBLIC PROGRAM CODE
*****************************************************************************
*/
/*************************************************************************
*
*  Function:   voAMRNBEnc_ton_stab_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int voAMRNBEnc_ton_stab_init (tonStabState **state, VO_MEM_OPERATOR *pMemOP)
{
	tonStabState* s;
	if (state == (tonStabState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (tonStabState *)voAMRNBEnc_mem_malloc(pMemOP, sizeof(tonStabState), 32)) == NULL){
		return -1;
	}
	voAMRNBEnc_ton_stab_reset(s);
	*state = s;

	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_ton_stab_reset
*  Purpose:    Initializes state memory to zero
*
**************************************************************************
*/
int voAMRNBEnc_ton_stab_reset (tonStabState *st)
{
	if (st == (tonStabState *) NULL){
		return -1;
	}
	/* initialize tone stabilizer state */ 
	st->count = 0;
	Set_zero(st->gp, N_FRAME);    /* Init Gp_Clipping */

	return 0;
}

/*************************************************************************
*
*  Function:   voAMRNBEnc_ton_stab_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void voAMRNBEnc_ton_stab_exit (tonStabState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	/* deallocate memory */
	voAMRNBEnc_mem_free(pMemOP, *state);
	*state = NULL;

	return;
}

/***************************************************************************
*                                                                          *
*  Function:  voAMRNBEnc_check_lsp()                                                  *
*  Purpose:   Check the LSP's to detect resonances                         *
*                                                                          *
****************************************************************************
*/
Word16 voAMRNBEnc_check_lsp(tonStabState *st, /* i/o : State struct            */
				 Word16 *lsp       /* i   : unquantized LSP's       */
				 )
{
	int i, dist, dist_min1, dist_min2, dist_th;
	/* Check for a resonance:                             */
	/* Find minimum distance between lsp[i] and lsp[i+1]  */
#ifdef C_OPT
	dist_min1 = MAX_16;                      
	for (i = 3; i < 8; i++)
	{
		dist = (lsp[i] - lsp[i+1]);
		if (dist < dist_min1)
		{
			dist_min1 = dist;                 
		}
	}
	dist_min2 = MAX_16;   
	for (i = 1; i < 3; i++)
	{
		dist = (lsp[i] - lsp[i+1]);
		if(dist < dist_min2)
		{
			dist_min2 = dist;                   
		}
	}
	dist_th = 1100;

	if (lsp[1] > 32000)
	{
		dist_th = 600;                         
	}

	else if (lsp[1] > 30500 )
	{
		dist_th = 800;                         
	}

	if ((dist_min1 < 1500) || (dist_min2 < dist_th))
	{
		st->count++;
	}
	else
	{
		st->count = 0;                         
	} 
	/* Need 12 consecutive frames to set the flag */
	if (st->count >= 12)
	{
		st->count = 12;                      
		return 1;
	}
	return 0;  
#else
	dist_min1 = MAX_16;                      
	for (i = 3; i < M-2; i++)
	{
		dist = sub(lsp[i], lsp[i+1]);

		if (sub(dist, dist_min1) < 0)
		{
			dist_min1 = dist;                  
		}
	}
	dist_min2 = MAX_16;                      
	for (i = 1; i < 3; i++)
	{
		dist = sub(lsp[i], lsp[i+1]);

		if (sub(dist, dist_min2) < 0)
		{
			dist_min2 = dist;                   
		}
	}

	if (sub(lsp[1], 32000) > 0)
	{
		dist_th = 600;                         
	}
	else if (sub(lsp[1], 30500) > 0)
	{
		dist_th = 800;                         
	}
	else
	{
		dist_th = 1100;                       
	}
	if (sub(dist_min1, 1500) < 0 ||
		sub(dist_min2, dist_th) < 0)
	{
		st->count = add(st->count, 1);
	}
	else
	{
		st->count = 0;                      
	} 
	/* Need 12 consecutive frames to set the flag */
	if (sub(st->count, 12) >= 0)
	{
		st->count = 12;                  
		return 1;
	}
	else
	{
		return 0;
	}
#endif
}

/***************************************************************************
*
*  Function:   voAMRNBEnc_check_gp_clipping()                                          
*  Purpose:    Verify that the sum of the last (N_FRAME+1) pitch  
*              gains is under a certain threshold.              
*                                                                         
***************************************************************************
*/ 
Word16 voAMRNBEnc_check_gp_clipping(tonStabState *st, /* i/o : State struct            */
						 Word16 g_pitch    /* i   : pitch gain              */
						 )
{
	Word32  i, sum; 
	sum = (g_pitch>> 3);          /* Division by 8 */
	for (i = 0; i < N_FRAME; i++)
	{
		sum += st->gp[i];
	}
	return ((sum- GP_CLIP) > 0);
}

/***************************************************************************
*
*  Function:  voAMRNBEnc_update_gp_clipping()                                          
*  Purpose:   Update past pitch gain memory
*                                                                         
***************************************************************************
*/
void voAMRNBEnc_update_gp_clipping(tonStabState *st, /* i/o : State struct            */
						Word16 g_pitch    /* i   : pitch gain              */
						)
{
	Copy(&st->gp[1], &st->gp[0], N_FRAME-1);
	st->gp[N_FRAME-1] = (g_pitch>> 3);
}
