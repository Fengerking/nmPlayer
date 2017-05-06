
#include "typedef.h"
#include "amr_plus_fx.h"



#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "count.h"

void Preemph_scaled( 
    Word16 new_speech[],     /* i:   Speech to scale                 */
    Word16 *Q_new,           /* o:   Scaling factor                  */
    Word16 *exp,             /* o:   Diff scaling factor             */ 
    Word16 *mem_preemph,     /* i/o: Preemph memory                  */
    Word16 *Q_max,           /* i/o: Q_new limitation                */  
    Word16 *Q_old,           /* i/o: Q_new memory                    */  
    Word16 Preemph_factor,   /* i:   Preemphasis factor         Q15  */
    Word16 bits,             /* i:   Limit output to (15-bits) bits  */  
    Word16 Lframe            /* i:   Frame length                    */
)
{
    Word16 i, tmp_fixed;
    Word16 mu, shift, QVal; 
    Word32 L_tmp, L_max;

   /*---------------------------------------------------------------*
    * Perform fixed preemphasis through 1 - g z^-1                  *
    * Scale signal to get maximum of precision in filtering         *
    *---------------------------------------------------------------*/

    QVal = shl(1, sub(15,bits)); 
    mu = shr(Preemph_factor, bits);              /* Q15 --> Q(15-bits) */

   /* get max of new preemphased samples (L_FRAME+L_FILT) */

    L_tmp = L_mult(new_speech[0], QVal);
    L_tmp = L_msu(L_tmp, *mem_preemph, mu);
    L_max = L_abs(L_tmp);

    for (i = 1; i < Lframe; i++)
    {
       /* Equivalent to tmp = max((abs(x[i] - mu*x[i-1]),tmp)
        * finds the max of preemphasized signal               */
        L_tmp = L_mult(new_speech[i], QVal);
        L_tmp = L_msu(L_tmp, new_speech[i - 1], mu);
        L_tmp = L_abs(L_tmp);
        test();
        if (L_sub(L_tmp, L_max) > (Word32) 0)
        {
            L_max = L_tmp;                 move32();
        }
    }

   /* get scaling factor for new and previous samples */
   /* limit scaling to Q_MAX to keep dynamic for ringing in low signal */
   /* limit scaling to Q_MAX also to avoid a[0]<1 in syn_filt_32 */
    tmp_fixed = extract_h(L_max);
    test();
    if (tmp_fixed == 0) 
    {
        shift = Q_MAX3;                     move16();
    }
    else 
    {
        shift = sub(norm_s(tmp_fixed), bits);
        test();
        if (shift < 0)
        {
            shift = 0;                     move16();
        }
        test();
        if (sub(shift, Q_MAX3) > 0)
        {
            shift = Q_MAX3;                 move16();
        }
    }
    *Q_new = shift;                        move16();
    test();
    if (sub(*Q_new, Q_max[0]) > 0) 
    {
        *Q_new = Q_max[0];                 move16();
    }
    test();
    if (sub(*Q_new, Q_max[1]) > 0) 
    {
        *Q_new = Q_max[1];                 move16();
    }

    *exp     = sub(*Q_new, *Q_old);         move16();
    *Q_old   = *Q_new;                     move16();
    Q_max[1] = Q_max[0];                   move16();
    Q_max[0] = shift;                      move16();

   /* preemphasis with scaling (L_FRAME+L_FILT) */

   /* now do the actual preemphasis, since we have the
    * proper scaling factor.
    * Done backwards to save storage space             */

    tmp_fixed = new_speech[Lframe - 1];         move16();

    for (i = sub(Lframe,1); i > 0; i--)
    {
        L_tmp = L_mult(new_speech[i], QVal);
        L_tmp = L_msu(L_tmp, new_speech[i - 1], mu);
        L_tmp = L_shl(L_tmp, *Q_new);
        new_speech[i] = round(L_tmp);      move16();
    }

    L_tmp = L_mult(new_speech[0], QVal);
    L_tmp = L_msu(L_tmp, *mem_preemph, mu);
    L_tmp = L_shl(L_tmp, *Q_new);
    new_speech[0] = round(L_tmp);          move16();

    *mem_preemph = tmp_fixed;              move16();
}


Word16 Scale_speech( 
  Word16 speech[],      /* (i/o)  : excitation to rescale     Q0/Qnew */
  Word16 lg,            /* (i)    : frame size                        */
  Word16 *Q_speech,     /* (i/o)  : State structure                   */
  Word16 *Old_Q,        /* (i/o)  : Last frame Scaling                */
  Word16 BitToRemove
)  
{
  Word16 i, tmp16, max, new_Q;
  max = 1;                           move16();
  for (i = 0; i < lg; i++)
  {
    tmp16 = abs_s(speech[i]);
    test();
    if (sub(tmp16, max) > 0)
    {
      max = tmp16;                 move16();
    }
  }
  /* find scaling (tmp) to set max = [1024..(8192 or 16384)[ */
  new_Q = sub(norm_s(max),BitToRemove);
  test();
  if (sub(new_Q, Q_MAX2) > 0) 
  {
    new_Q = Q_MAX2;             move16();
  }

  /* new_Q = smallest Q since 2 lastframe (160 ms) */
  tmp16 = new_Q;        move16();
  if (sub(new_Q, Old_Q[0]) > 0) 
  {
      new_Q = Old_Q[0];                 move16();
  }
  test();
  if (sub(new_Q, Old_Q[1]) > 0) 
  {
      new_Q = Old_Q[1];                 move16();
  }
  Old_Q[1] = Old_Q[0];                   move16();
  Old_Q[0] = tmp16;                      move16();

  /* rescale excitation and associated memories */
  tmp16 = sub(new_Q, *Q_speech);
  Scale_sig(speech, lg, new_Q);
    /* scaling factor of excitation (-1..12) */
  *Q_speech = new_Q;                                      move16();

  return tmp16;
}
Word16 Scale_speech_st( 
  Word16 sp_right[],      /* (i/o)  : excitation to rescale     Q0/Qnew */
  Word16 sp_left[],      /* (i/o)  : excitation to rescale     Q0/Qnew */
  Word16 lg,            /* (i)    : frame size                        */
  Word16 *Q_speech,     /* (i/o)  : State structure                   */
  Word16 *Old_Q,        /* (i/o)  : Last frame Scaling                */
  Word16 BitToRemove
)  
{
  Word16 i, tmp16, max_r, max_l, new_Q, max;
  max_l = max_r = 1;               move16();move16();
  for (i = 0; i < lg; i++)
  {
    tmp16 = abs_s(sp_right[i]);
    test();
    if (sub(tmp16, max_r) > 0)
    {
      max_r = tmp16;                 move16();
    }
    tmp16 = abs_s(sp_left[i]);
    if (sub(tmp16, max_l) > 0)
    {
      max_l = tmp16;                 move16();
    }
  }

  test();move16();        /* Keep max of 2 channels */
  if(sub(max_r,max_l)>0)
    max = max_r;
  else
    max = max_l;
  /* find scaling (tmp) to set max = [1024..(8192 or 16384)[ */
  new_Q = sub(norm_s(max),BitToRemove);
  test();
  if (sub(new_Q, Q_MAX2) > 0) 
  {
    new_Q = Q_MAX2;             move16();
  }

  /* new_Q = smallest Q since 2 lastframe (160 ms) */
  tmp16 = new_Q;        move16();
  if (sub(new_Q, Old_Q[0]) > 0) 
  {
      new_Q = Old_Q[0];                 move16();
  }
  test();
  if (sub(new_Q, Old_Q[1]) > 0) 
  {
      new_Q = Old_Q[1];                 move16();
  }
  Old_Q[1] = Old_Q[0];                   move16();
  Old_Q[0] = tmp16;                      move16();

  /* rescale excitation and associated memories */
  tmp16 = sub(new_Q, *Q_speech);
  Scale_sig(sp_right, lg, new_Q);
  Scale_sig(sp_left, lg, new_Q);

    /* scaling factor of excitation (-1..12) */
  *Q_speech = new_Q;                                      move16();

  return tmp16;
}
void Rescale_enc_mem_hf(Coder_StState_fx *st, Word16 scale_speech_hf)
{
    Scale_sig(st->mem_hf3, MHF, scale_speech_hf);
}

void Rescale_enc_mem(Coder_State_Plus_fx *st, Word16 scale_preem, Word16 scale_speech)
{
  Word32 L_tmp;
  Word16 tmp;
  test();
  if (scale_speech != 0)
  {

    Scale_sig(st->right.mem_decim,L_MEM_DECIM_SPLIT, scale_speech);
    Scale_sig(&(st->right.mem_preemph),1, scale_speech);

    L_tmp = L_deposit_h(st->right.mem_sig_in[4]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &st->right.mem_sig_in[4], &tmp);

    L_tmp = L_Comp(st->right.mem_sig_in[2], st->right.mem_sig_in[3]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &st->right.mem_sig_in[2], &st->right.mem_sig_in[3]);

    st->right.mem_sig_in[3] = sub(st->right.mem_sig_in[3], shr(tmp, 1));      move16();

    L_tmp = L_deposit_h(st->right.mem_sig_in[5]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &(st->right.mem_sig_in[5]), &tmp);

    L_tmp = L_Comp(st->right.mem_sig_in[0], st->right.mem_sig_in[1]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &st->right.mem_sig_in[0], &st->right.mem_sig_in[1]);

    st->right.mem_sig_in[1] = sub(st->right.mem_sig_in[1], shr(tmp, 1));      move16();

/*HF*/
    Scale_sig(st->right.mem_decim_hf, 2*L_FILT24k, scale_speech);

    Scale_sig(st->right.old_speech_hf, L_OLD_SPEECH_ST, scale_speech);
    Scale_sig(st->right.mem_hf3, MHF, scale_speech);
    Scale_sig(st->old_exc_mono, HI_FILT_ORDER, scale_speech);
  }
  test();  
  if(scale_preem != 0)
  {
    Scale_sig(st->right.mem_hf1, MHF, scale_preem);
    Scale_sig(st->right.mem_hf2, MHF, scale_preem);
    Scale_sig(st->old_speech_pe, L_OLD_SPEECH_ST, scale_preem);
    Scale_sig(st->old_synth, M, scale_preem);
    Scale_sig(&(st->mem_wsp), 1, scale_preem);
    Scale_sig(st->mem_lp_decim2, 3, scale_preem);

   /*Scale_sig(st->wold_exc, PIT_MAX_MAX+L_INTERPOL, scale_preem);    Done in acelp if necessary */
    Scale_sig(st->old_wovlp, 128, scale_preem);
  }


}
void Rescale_enc_mem_st(Coder_State_Plus_fx *st, Word16 scale_preem, Word16 scale_speech)
{
  Word32 L_tmp;
  Word16 tmp;
  test();
  if (scale_speech != 0)
  {
    Scale_sig(st->right.mem_decim,L_MEM_DECIM_SPLIT, scale_speech);
    Scale_sig(st->left.mem_decim,L_MEM_DECIM_SPLIT, scale_speech);
    Scale_sig(&(st->right.mem_preemph),1, scale_speech);
    Scale_sig(&(st->left.mem_preemph),1, scale_speech);

    L_tmp = L_deposit_h(st->right.mem_sig_in[4]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &st->right.mem_sig_in[4], &tmp);

    L_tmp = L_Comp(st->right.mem_sig_in[2], st->right.mem_sig_in[3]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &st->right.mem_sig_in[2], &st->right.mem_sig_in[3]);

    st->right.mem_sig_in[3] = sub(st->right.mem_sig_in[3], shr(tmp, 1));      move16();

    L_tmp = L_deposit_h(st->right.mem_sig_in[5]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &(st->right.mem_sig_in[5]), &tmp);

    L_tmp = L_Comp(st->right.mem_sig_in[0], st->right.mem_sig_in[1]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &st->right.mem_sig_in[0], &st->right.mem_sig_in[1]);

    st->right.mem_sig_in[1] = sub(st->right.mem_sig_in[1], shr(tmp, 1));      move16();


    
    L_tmp = L_deposit_h(st->left.mem_sig_in[4]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &st->left.mem_sig_in[4], &tmp);

    L_tmp = L_Comp(st->left.mem_sig_in[2], st->left.mem_sig_in[3]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &st->left.mem_sig_in[2], &st->left.mem_sig_in[3]);

    st->left.mem_sig_in[3] = sub(st->left.mem_sig_in[3], shr(tmp, 1));      move16();

    L_tmp = L_deposit_h(st->left.mem_sig_in[5]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &(st->left.mem_sig_in[5]), &tmp);

    L_tmp = L_Comp(st->left.mem_sig_in[0], st->left.mem_sig_in[1]);
    L_tmp = L_shl(L_tmp, scale_speech);
    L_Extract(L_tmp, &st->left.mem_sig_in[0], &st->left.mem_sig_in[1]);

    st->left.mem_sig_in[1] = sub(st->left.mem_sig_in[1], shr(tmp, 1));      move16();

    
    /*HF*/
    Scale_sig(st->right.mem_decim_hf, 2*L_FILT24k, scale_speech);
    Scale_sig(st->left.mem_decim_hf, 2*L_FILT24k, scale_speech);

    Scale_sig(st->right.old_speech_hf, L_OLD_SPEECH_ST, scale_speech);
    Scale_sig(st->left.old_speech_hf, L_OLD_SPEECH_ST, scale_speech);
    Scale_sig(st->right.mem_hf3, MHF, scale_speech);
    Scale_sig(st->left.mem_hf3, MHF, scale_speech);

    Scale_sig(st->old_chan, L_OLD_SPEECH_ST, scale_speech);
    Scale_sig(st->old_chan_2k, L_OLD_SPEECH_2k, scale_speech);
    Scale_sig(st->old_speech_hi, L_OLD_SPEECH_hi, scale_speech);

    Scale_sig(st->old_speech, L_OLD_SPEECH_ST, scale_speech);
    Scale_sig(st->old_speech_2k, L_OLD_SPEECH_2k, scale_speech);
    Scale_sig(st->old_chan_hi, L_OLD_SPEECH_hi, scale_speech);

    Scale_sig(st->mem_stereo_ovlp, L_OVLP_2k, scale_speech);
  }
  test();  
  if(scale_preem != 0)
  {
    Scale_sig(st->right.mem_hf1, MHF, scale_preem);
    Scale_sig(st->left.mem_hf1, MHF, scale_preem);
    Scale_sig(st->right.mem_hf2, MHF, scale_preem);
    Scale_sig(st->left.mem_hf2, MHF, scale_preem);
    Scale_sig(st->old_speech_pe, L_OLD_SPEECH_ST, scale_preem);
    Scale_sig(st->old_synth, M, scale_preem);
    Scale_sig(&(st->mem_wsp), 1, scale_preem);
    Scale_sig(st->mem_lp_decim2, 3, scale_preem);

    /*Scale_sig(st->wold_exc, PIT_MAX_MAX+L_INTERPOL, scale_preem);    Done in acelp if necessary */
    Scale_sig(st->old_wovlp, 128, scale_preem);
  }
}
