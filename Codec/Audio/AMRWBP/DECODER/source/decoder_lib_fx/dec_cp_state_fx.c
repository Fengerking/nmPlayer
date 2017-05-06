#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "log2.h"
#include "oper_32b.h"

#include "amr_plus_fx.h"
#include "dec_main_fx.h"
#include "mem_fx.h"
#include "proto_func_fx.h"
 




#ifndef WB_dec_if_state
typedef struct
{
  Word16 reset_flag_old;     /* previous was homing frame  */
  Word16 prev_ft;            /* previous frame type        */
  Word16 prev_mode;          /* previous mode              */
  void *decoder_state;       /* Points decoder state       */
}
  WB_dec_if_state;
#endif

#define Q_MAX        8        /* scaling max for signal                 */
#define LG10         24660     /*  10*log10(2)  in Q13                  */


void voAMRWBPDecReset_decoder(void *st, Word16 reset_all);


void Copy_decoder_state(
  Decoder_State_Plus_fx *wbP,
  void *st,
  Word16 sw
)
{
  Word16 i;
  Word32 Ltmp;
  

  WB_dec_if_state *s1;
  Decoder_State *wb;

  s1 = (WB_dec_if_state *)st;
  wb = (Decoder_State *)(s1->decoder_state);        


  /* Copy data from WB to WB+ structure*/
  
  if (sw == 0)
  {

    /* Full reset for WB+ decoder*/
    Init_decoder_amrwb_plus(wbP, 1, 0, 1);


    voAMRWBPDecCopy(wb->ispold, wbP->wispold, M);

    
    for (i = 0 ; i < M; i++)
    {
      wbP->wisfold[i] = round16(L_mult(shl(wb->isfold[i],2), 20972));         
      wbP->wpast_isfq[i] = round16(L_mult(shl(wb->past_isfq[i],2),20972));    
    } 

    for (i = 0; i < L_MEANBUF; i++)
    {
      voAMRWBPDecCopy(wbP->wisfold, &(wbP->wisf_buf[i*M]), M);
    }


    wbP->wold_T0 = wb->old_T0;            
    wbP->wold_T0_frac = wb->old_T0_frac;  


    wbP->wold_T_pf[0] = (wbP->wold_T_pf[1] = wbP->wold_T0);    
    wbP->wold_gain_pf[0] = (wbP->wold_gain_pf[1] = 0);         


    for (i = 0; i < M; i++)
    {
      Ltmp = L_mult(wb->mem_syn_lo[i],1);
      Ltmp = L_mac(Ltmp, wb->mem_syn_hi[i], 4096);
      Ltmp = L_shl(Ltmp,6);
      wbP->wold_synth[i] = round16(Ltmp);    
    }

    for(i = 0; i < PIT_MAX + L_INTERPOL; i++)
    {
      wbP->wold_exc[i + PIT_MAX_MAX - PIT_MAX] = wb->old_exc[i];      
    }


    wbP->wmem_deemph = wb->mem_deemph;      

    voAMRWBPDecCopy(wb->mem_sig_out, wbP->wmem_sig_out, 6);

    for (i = 0; i < 2 * L_FILT; i++)
    {
      wbP->right.wmem_oversamp[i] = mult(wb->mem_oversamp[i],20480);     /* To fit decim12k8 gain1 */
    }


    wbP->L_gc_thres = wb->L_gc_thres;     move32();
    for (i = 0; i < PIT_MAX + L_SUBFR; i++)
    {
      wbP->wold_synth_pf[PIT_MAX_MAX - PIT_MAX+i] = wb->mem_syn_out[i];      
    }

    /* trigger HF ramp-up */
    wbP->wramp_state = 0;                    
    for (i = 0; i < 6; i++)
    {
      wbP->mem_subfr_q[i] = wb->Q_old;      
      wbP->old_subfr_q[i] = wb->Q_old;      
      wbP->old_syn_q[i] = -1;               
    }
    wbP->mem_subfr_q[i] = wb->Q_old;        

    wbP->Q_exc = wb->Q_old;                 
    wbP->Old_Q_syn = 0;                     
    wbP->right.Q_synHF = add(wbP->Old_Q_syn, 1);    

  }
  /* Copy data from WB+ to WB structure*/
  else
  {
    /* Full reset for AMR-WB decoder*/
    voAMRWBPDecReset_decoder((void*)wb, 1);
    voAMRWBPDecCopy(wbP->wispold, wb->ispold, M);

    for (i = 0; i < M; i++)
    {
      wb->isfold[i] = mult(wbP->wisfold[i],12800);    
      wb->past_isfq[i] = mult(wbP->wpast_isfq[i],12800);        
    }
    wb->old_T0 = wbP->wold_T0;   
    wb->old_T0_frac = wbP->wold_T0_frac;   


    for (i = 0; i < M; i++)
    {
      Ltmp = L_shl(wbP->cp_old_synth[i],sub(8, wbP->Old_Q_syn));   /* Q0*/
      wb->mem_syn_hi[i] = extract_l(L_shr(Ltmp, 12)); 
      wb->mem_syn_lo[i] = extract_l((Word16)(Ltmp & 0x00000FFF));   logic32();
    }

      
    if (wbP->last_mode == 0)
    {
      Word16 frac, exp1;
      for (i = 0; i < 4; i++)
      {

        /*ftemp = 20.0f * (float)log10((float)wbP->wmem_gain_code[3-i]/2048.0) * 1024.0f;*/

        voAMRWBPDecLog2((wbP->wmem_gain_code[3-i]), &exp1, &frac);
        exp1 = sub(exp1,11);                            /* wmem_gain_code in Q11*/
        Ltmp = Mpy_32_16(exp1, frac, LG10);             /* Ltmp in Q13*/
        wb->dec_gain[i] = extract_l(L_shr(Ltmp, 13-10));      /* Output in Q10 */

      }

    }



    for (i = 0; i < (PIT_MAX + L_INTERPOL); i++)
    {
      wb->old_exc[i] = wbP->wold_exc[i + PIT_MAX_MAX- PIT_MAX];    
    }
    wb->Q_old = wbP->Q_exc;  


    for (i = 0; i < 4; i++)
    {
      wb->Qsubfr[i] = wbP->mem_subfr_q[i];      
    }



    wb->mem_deemph = shr(wbP->wmem_deemph,wbP->Old_Q_syn);   

    wb->mem_sig_out[0] = shr(wbP->wmem_sig_out[0],wbP->Old_Q_syn);  
    wb->mem_sig_out[1] = shr(wbP->wmem_sig_out[1],wbP->Old_Q_syn);  
    wb->mem_sig_out[2] = shr(wbP->wmem_sig_out[2],wbP->Old_Q_syn);  
    wb->mem_sig_out[3] = shr(wbP->wmem_sig_out[3],wbP->Old_Q_syn);  
    wb->mem_sig_out[4] = shr(wbP->wmem_sig_out[4],wbP->Old_Q_syn);  
    wb->mem_sig_out[5] = shr(wbP->wmem_sig_out[5],wbP->Old_Q_syn);  


    for (i = 0; i < 2 * L_FILT; i++)
    {
      Ltmp  = L_mult(shr(wbP->right.wmem_oversamp[i],sub(wbP->Old_Q_syn,1)), 26214);   /* x by 1.6 see decim12k8 */
      wb->mem_oversamp[i] = round16(Ltmp);   
    }



    wb->L_gc_thres = wbP->L_gc_thres;       move32();

    for (i = 0; i < PIT_MAX + L_SUBFR; i++)
    {
      wb->mem_syn_out[i] = round16(L_shr(L_deposit_h(wbP->wold_synth_pf[PIT_MAX_MAX-PIT_MAX+i]),wbP->Old_Q_syn));	/*To Q0 (with saturation)*/   
    }

    /* LPC filter for HF from WB+*/
    voAMRWBPDecCopy(wbP->wmem_lpc_hf,wb->lpc_hf_plus, 9);

    /* HF gain from WB+*/
    wb->gain_hf_plus = wbP->wmem_gain_hf;      

    /* Up-sampling filterbank memory for HF signal*/
    for (i = 0; i < 2 * L_FILT; i++)
    {
      wb->mem_oversamp_hf_plus[i] = shr(wbP->right.wmem_oversamp_hf[i],wbP->right.Q_synHF);   
    }

    /* HF synthesis filter memory*/
    for (i = 0; i < 8; i++)
    {
      wb->mem_syn_hf_plus[i] = shr(wbP->right.wmem_syn_hf[i], wbP->right.Q_synHF);    
    }

    /* HF smoothing*/
    wb->threshold_hf = wbP->right.Lthreshold;     move32();
    wb->lp_amp_hf = wbP->right.Lp_amp;            move32();

    /* Trigger HF ramp-down*/
    wb->ramp_state = 64;                          
  }

  return;
}
