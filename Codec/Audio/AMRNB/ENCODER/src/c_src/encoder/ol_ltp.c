/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : ol_ltp.c
*      Purpose          : Compute the open loop pitch lag.
*
********************************************************************************
*/

/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "ol_ltp.h"
const char ol_ltp_id[] = "@(#)$Id $" ol_ltp_h;

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include "typedef.h"
#include "cnst.h"
#include "pitch_ol.h"
#include "p_ol_wgh.h"
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
int voAMRNBEnc_ol_ltp(
		   cod_amrState *amr_st,
		   enum Mode mode,       /* i   : coder mode                              */
		   Word16 wsp[],         /* i   : signal used to compute the OL pitch, Q0 */
		   /*  uses signal[-pit_max] to signal[-1] */
		   Word16 *T_op,         /* o   : open loop pitch lag,                 Q0 */
		   Word16 idx            /* i   : index                                   */
		   )
{ 
	pitchOLWghtState *st = amr_st->pitchOLWghtSt;
	vadState         *vadSt = amr_st->vadSt;
	Word16           *old_lags = amr_st->old_lags;
	Word16           *ol_gain_flg = amr_st->ol_gain_flg;
	Flag             dtx = amr_st->dtx;

	if (mode != MR102)
	{
		ol_gain_flg[0] = 0;                                       
		ol_gain_flg[1] = 0;                                       
	}

	switch(mode){
  case MR122:
	  *T_op = voAMRNBEnc_Pitch_ol(vadSt, mode, wsp, PIT_MIN_MR122, PIT_MAX, L_FRAME_BY2, idx, dtx);
	  break;
  case MR475:
  case MR515:
	  *T_op = voAMRNBEnc_Pitch_ol(vadSt, mode, wsp, PIT_MIN, PIT_MAX, L_FRAME, idx, dtx);
	  break;
  case MR59:
  case MR67:
  case MR74:
  case MR795:
	  *T_op = voAMRNBEnc_Pitch_ol(vadSt, mode, wsp, PIT_MIN, PIT_MAX, L_FRAME_BY2, idx, dtx);
	  break;
  case MR102:
	  *T_op = voAMRNBEnc_Pitch_ol_wgh(st, vadSt, wsp, PIT_MIN, PIT_MAX, L_FRAME_BY2, old_lags, ol_gain_flg, idx, dtx);
	  break;
  default:
	  break;
	} 
	return 0;
}
