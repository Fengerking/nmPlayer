/*
*****************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
*****************************************************************************
*
*      File             : sid_sync.c
*
*****************************************************************************
*/
/*
*****************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
*****************************************************************************
*/
#include "sid_sync.h"
const char sid_sync_id[] = "@(#)$Id $" sid_sync_h;

/******************************************************************************
*                         INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "mode.h"

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
int voAMRNBEnc_sid_sync_init (sid_syncState **state, VO_MEM_OPERATOR *pMemOP)
{
	sid_syncState* s;
	if (state == (sid_syncState **) NULL){
		return -1;
	}
	*state = NULL;
	/* allocate memory */
	if ((s= (sid_syncState *)voAMRNBEnc_mem_malloc(pMemOP, sizeof(sid_syncState), 32)) == NULL){
			return -1;
	}
	s->sid_update_rate = 8;
	*state = s;
	return voAMRNBEnc_sid_sync_reset(s);
}

int voAMRNBEnc_sid_sync_reset (sid_syncState *st)
{
	st->sid_update_counter = 3;
	st->sid_handover_debt = 0;
	st->prev_ft = TX_SPEECH_GOOD;
	return 0;
}

void voAMRNBEnc_sid_sync_exit(sid_syncState **state, VO_MEM_OPERATOR *pMemOP)
{
	if (state == NULL || *state == NULL)
		return;
	/* deallocate memory */
	voAMRNBEnc_mem_free(pMemOP, *state);
	*state = NULL;
	return;

}

int voAMRNBEnc_sid_sync_set_handover_debt (sid_syncState *st, Word16 debtFrames) 
{
	/* debtFrames >= 0 */ 
	st->sid_handover_debt = debtFrames;
	return 0;
}

void voAMRNBEnc_sid_sync (sid_syncState *st,
			   enum Mode mode,
			   enum TXFrameType *tx_frame_type)
{

	if ( mode == MRDTX){
		st->sid_update_counter--;
		if (st->prev_ft == TX_SPEECH_GOOD) 
		{
			*tx_frame_type = TX_SID_FIRST;
			st->sid_update_counter = 3;
		} 
		else 
		{
			/* TX_SID_UPDATE or TX_NO_DATA */
			if( (st->sid_handover_debt > 0) && (st->sid_update_counter > 2) )
			{
				/* ensure extra updates are  properly delayed after 
				a possible SID_FIRST */
				*tx_frame_type = TX_SID_UPDATE;
				st->sid_handover_debt--;
			}
			else 
			{
				if (st->sid_update_counter == 0)
				{
					*tx_frame_type = TX_SID_UPDATE;
					st->sid_update_counter = st->sid_update_rate;
				} else {
					*tx_frame_type = TX_NO_DATA;
				}
			}
		}
	}
	else
	{
		st->sid_update_counter = st->sid_update_rate ;
		*tx_frame_type = TX_SPEECH_GOOD;
	}
	st->prev_ft = *tx_frame_type;
}

