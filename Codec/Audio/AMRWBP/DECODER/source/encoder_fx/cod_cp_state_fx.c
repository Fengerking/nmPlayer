#include <stdlib.h>
#include <stdio.h>

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"

#include "cod_main_fx.h"
#include "mem_fx.h"
#include "proto_func_fx.h"
#include "acelp_fx.h"



#ifndef WB_enc_if_state
typedef struct
{
   Word16 sid_update_counter;   /* Number of frames since last SID */
   Word16 sid_handover_debt;    /* Number of extra SID_UPD frames to schedule */
   Word16 prev_ft;              /* Type of the previous frame */
   void *encoder_state;         /* Points encoder state structure */
} WB_enc_if_state;
#endif

#define Q_MAX        8        /* scaling max for signal                 */


void Reset_encoder(void *st, Word16 reset_all);


void Copy_coder_state(
  Coder_State_Plus_fx *wbP,	/* AMR-WB+ state struct */
  void   *st,				/* AMR-WB state struct  */
  Word16 sw,					/* sw=0 -> switch from WB to WB+, sw=1 -> switch from WB+ to WB */
  Word16 use_case_mode
)
{
	Word16 i;

	WB_enc_if_state *s1;
	Coder_State *wb;

	s1 = (WB_enc_if_state *)st;                 move16();
	wb = (Coder_State *)(s1->encoder_state);    move16();
	
	/* Copy data from WB to WB+ structure */
    test();
    if (sw == 0)
    {

        /* Full reset for WB+ encoder*/
        Init_coder_amrwb_plus(wbP, 1, 0, use_case_mode, 1);
        for (i = 0; i < M; i++)
        {
            wbP->isfold[i] = round(L_mult(shl(wb->isfold[i],2), 20972));         move16();
            wbP->past_isfq[i] = round(L_mult(shl(wb->past_isfq[i],2),20972));    move16();
        }

        Copy(wb->ispold, wbP->ispold, M);
        Copy(wb->ispold_q,wbP->ispold_q, M);

        for (i = 0; i < 5; i++)
        {
            wbP->old_ol_lag[i] = wb->old_ol_lag[i];		move32();
        } 

        Copy(wb->old_ol_lag, wbP->old_ol_lag, 5);
        wbP->old_T0_med = wb->old_T0_med;		    move16();


        Copy(wb->mem_decim, wbP->right.mem_decim, 30);
        Copy(wb->mem_sig_in, wbP->right.mem_sig_in, 6);

        Copy(wb->old_speech + 48, wbP->old_speech_pe, 16 + 512);
        Scale_sig(wbP->old_speech_pe, 16 + 512, -wb->Q_old);
        Copy(wb->old_wsp, wbP->old_d_wsp+PIT_MAX_MAX-PIT_MAX, PIT_MAX / OPL_DECIM);
        Copy(wb->old_hp_wsp, wbP->hp_old_wsp+PIT_MAX_MAX-PIT_MAX, PIT_MAX / OPL_DECIM);
        Scale_sig(wbP->old_d_wsp+PIT_MAX_MAX-PIT_MAX, PIT_MAX / OPL_DECIM, -wb->Q_old);
        Scale_sig(wbP->hp_old_wsp+PIT_MAX_MAX-PIT_MAX, PIT_MAX / OPL_DECIM, -wb->Q_old);

        wbP->mem_wsp = shr(wb->mem_wsp,wb->Q_old);   move16();
        Copy(wb->mem_decim2, wbP->mem_lp_decim2, 3);
        wbP->old_T0_med= wb->old_T0_med;    move16();
        wbP->ol_gain = wb->ol_gain;      move16();

        Copy(wb->hp_wsp_mem + 2, wbP->hp_ol_ltp_mem, 7);

        wbP->ada_w = wb->ada_w;      move16();
        wbP->ol_wght_flg = wb->ol_wght_flg;      move16();
        wbP->old_mem_w0 = wb->mem_w0 ;      move16();

        Copy(wb->mem_syn, wbP->old_synth, M);
        wbP->right.mem_preemph = shr(wb->mem_preemph, wb->Q_old);  ;      move16();

        Scale_sig(wbP->old_synth,M, -wb->Q_old );


        /*  Udpate adaptive codebook*/
        Copy(wb->old_exc, wbP->old_exc + PIT_MAX_MAX-PIT_MAX ,PIT_MAX + L_INTERPOL);
        Scale_sig(wbP->old_exc + PIT_MAX_MAX-PIT_MAX ,PIT_MAX + L_INTERPOL, -wb->Q_old );

        wbP->old_ovlp_size = 0;     move16();

        wbP->SwitchFlagPlusToWB = 1;    move16();
        test();  
        if (sub(use_case_mode,USE_CASE_B) == 0)
        {
            /* VAD states: wb->vadSt to wbP->vadSt */
            Copy(wb->vadSt->bckr_est, wbP->_vadSt->bckr_est, 12);
            Copy(wb->vadSt->ave_level, wbP->_vadSt->ave_level, 12);
            Copy(wb->vadSt->old_level, wbP->_vadSt->old_level, 12);
            Copy(wb->vadSt->sub_level, wbP->_vadSt->sub_level, 12);
            Copy(wb->vadSt->a_data5, wbP->_vadSt->a_data5, 10);
            Copy(wb->vadSt->a_data3, wbP->_vadSt->a_data3, 6);

            wbP->_vadSt->sp_max = wb->vadSt->sp_max;                move16();
            wbP->_vadSt->speech_level = wb->vadSt->speech_level;    move16();
            wbP->_vadSt->burst_count = wb->vadSt->burst_count;    move16();
            wbP->_vadSt->hang_count = wb->vadSt->hang_count;    move16();
            wbP->_vadSt->stat_count = wb->vadSt->stat_count;    move16();
            wbP->_vadSt->vadreg = wb->vadSt->vadreg;    move16();
            wbP->_vadSt->tone_flag = wb->vadSt->tone_flag;    move16();
            wbP->_vadSt->sp_est_cnt = wb->vadSt->sp_est_cnt;    move16();
            wbP->_vadSt->sp_max_cnt = wb->vadSt->sp_max_cnt;    move16();
            test();
            if (wbP->_stClass->StatClassCount == 0)
            {
                wbP->_stClass->StatClassCount = 15;        move16();
            }
        }

        for (i = 0; i < 2; i++)
        {
            wbP->Q_max[i] = wb->Q_max[i];     move16();
            wbP->OldQ_sp_deci[i] = 0;   move16();
        }

        wbP->Q_new = 0;          /* First scaling */   move16();

        wbP->Q_exc = 0;         move16();
        wbP->Old_Qexc = 0;      move16();
        wbP->LastQMode = 0;     move16();
        wbP->OldQ_sp = 0;       move16();

	} 
	/* This is WB mode: copy data from WB+ to WB */
	else if (sub(sw,1) == 0)
	{
        Word32 Ltmp;

        /* Full reset for AMR-WB encoder */
        Reset_encoder((void*)wb, 1);

        /* Copy ISF data */
        for (i = 0; i < M; i++)
        {
            wb->isfold[i] = mult(wbP->isfold[i],12800);    move16();
            wb->past_isfq[i] = mult(wbP->past_isfq[i],12800);   move16();     
        }
        Copy(wbP->ispold, wb->ispold, M);
        Copy(wbP->ispold_q, wb->ispold_q, M);
        Copy(wbP->old_ol_lag, wb->old_ol_lag, 5);

        wb->old_T0_med = (wbP->old_T0_med);   move16();

        Copy(wbP->right.mem_decim+L_MEM_DECIM_SPLIT-30, wb->mem_decim, 30);
        Scale_sig(wb->mem_decim, 30, -wbP->Q_new);

        Copy(wbP->right.mem_sig_in, wb->mem_sig_in, 6);
        wb->mem_preemph = shl(wbP->right.mem_preemph,-wbP->Q_new + wbP->Q_sp - 1);      move16();

        Copy(wbP->old_speech_pe, wb->old_speech + 48, 16 + 512);

        for (i = 0; i < 48; i++)	
        {
            wb->old_speech[i] = 0;	move16();
        }

        Copy(wbP->old_d_wsp+PIT_MAX_MAX-PIT_MAX, wb->old_wsp, PIT_MAX / OPL_DECIM);
        Copy(wbP->hp_old_wsp+PIT_MAX_MAX-PIT_MAX, wb->old_hp_wsp, PIT_MAX / OPL_DECIM);


        wb->mem_wsp = wbP->mem_wsp;                         move16();
        Copy(wbP->mem_lp_decim2, wb->mem_decim2, 3);

        wb->ol_gain = wbP->ol_gain;                         move16();
        Copy(wbP->hp_ol_ltp_mem, wb->hp_wsp_mem + 2, 7);	    
        wb->hp_wsp_mem[0] = 0;	wb->hp_wsp_mem[1] = 0;      move16();move16();

        wb->ada_w = wbP->ada_w;    move16();
        wb->ol_wght_flg = wbP->ol_wght_flg;    move16();

        wb->mem_w0 = wbP->old_mem_w0; move16();

        test();
        if (wbP->prev_mod == 0)
        {
            for (i = 0; i < 4; i++)
            {
                wb->qua_gain[i]  = wbP->mem_gain_code[3-i];   move16();
            }
        }

        Copy(wbP->old_synth, wb->mem_syn, M);

        for (i = 0; i< M; i++)
        {   
            Ltmp = L_mult(wb->mem_syn[i], 128);
            wb->mem_syn_hi[i] = extract_l(L_shr(Ltmp, 4096));   move16();
            wb->mem_syn_lo[i] = extract_l(Ltmp&0x00000FFF);   logic32();move16();
        }
        for (i = 0; i < (PIT_MAX + L_INTERPOL); i++) 
        {
            wb->old_exc[i] = wbP->old_exc[i+PIT_MAX_MAX-PIT_MAX];   move16();
        }
        Scale_sig(wb->old_exc, PIT_MAX+L_INTERPOL, -wbP->Old_Qexc+ wbP->Q_sp-1);
        wb->Q_old = add(1,wbP->Q_sp);           move16();
        for (i = 0; i < 2; i++)
        {
            wb->Q_max[i] = wbP->Q_max[i];     move16();
        }

        test();
        if (sub(use_case_mode,USE_CASE_B) == 0)
        {
            /* VAD states: wbP->vadSt to wb->vadSt*/
            Copy(wbP->_vadSt->bckr_est, wb->vadSt->bckr_est, 12);
            Copy(wbP->_vadSt->ave_level, wb->vadSt->ave_level, 12);
            Copy(wbP->_vadSt->old_level, wb->vadSt->old_level, 12);
            Copy(wbP->_vadSt->sub_level, wb->vadSt->sub_level, 12);
            Copy(wbP->_vadSt->a_data5, wb->vadSt->a_data5, 10);
            Copy(wbP->_vadSt->a_data3, wb->vadSt->a_data3, 6);

            wb->vadSt->sp_max = wbP->_vadSt->sp_max;
            wb->vadSt->speech_level = wbP->_vadSt->speech_level;
            wb->vadSt->burst_count = wbP->_vadSt->burst_count;
            wb->vadSt->hang_count = wbP->_vadSt->hang_count;
            wb->vadSt->stat_count = wbP->_vadSt->stat_count;
            wb->vadSt->vadreg = wbP->_vadSt->vadreg;
            wb->vadSt->tone_flag = wbP->_vadSt->tone_flag;
            wb->vadSt->sp_est_cnt = wbP->_vadSt->sp_est_cnt;
            wb->vadSt->sp_max_cnt = wbP->_vadSt->sp_max_cnt;
        }
    }
    else
    {
        AMRWBPLUS_FPRINTF(stderr, " Unknown mode switching paramer: %d \n", sw);
        exit(-1);
    }

    return;
}
