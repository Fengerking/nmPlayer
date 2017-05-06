#include "amr_plus_fx.h"


#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "count.h"


Word16 Scale_exc( 
  Word16 exc[],         /* (i/o)  : excitation to rescale       Q_exc */
  Word16 lg,            /* (i)    : frame size                        */
  Word32 L_gain_code,   /* (i)    : decoded codebook gain       Q16   */      
  Word16 *Q_exc,        /* (i/o)  : State structure                   */
  Word16 *Q_exsub,      /*                                            */
  Word16 len
)
{
  Word16 i, tmp16, max, new_Q;
  max = 1;                           
  for (i = 0; i < lg; i++)
  {
    tmp16 = abs_s(exc[i]);
    
    if (sub(tmp16, max) > 0)
    {
      max = tmp16;                 
    }
  }

  /* find scaling (tmp) to set max = [2048..4096[ */
  tmp16 = sub(add(norm_s(max), *Q_exc), 3);
  
  if (sub(tmp16, 12) > 0) 
  {
    tmp16 = 12;             
  }

  /* find scaling (new_Q) to keep gain_code < 2048 */

  new_Q = -1;          
  
  while ((L_sub(L_gain_code, 0x08000000L) < 0) && (sub(new_Q, tmp16) < 0))
  {
    new_Q = add(new_Q, 1);
    L_gain_code = L_shl(L_gain_code, 1);
    
  }
  /* new_Q = smallest Q since 4 subframes (20ms) */
  tmp16 = new_Q;            
  for (i = 0; i < 7; i++)
  {
    
    if (sub(Q_exsub[i], new_Q) < 0)
    {
      new_Q = Q_exsub[i];           
    }
  }
  Q_exsub[6] = Q_exsub[5];     
  Q_exsub[5] = Q_exsub[4];     
  Q_exsub[4] = Q_exsub[3];     
  Q_exsub[3] = Q_exsub[2];     
  Q_exsub[2] = Q_exsub[1];     
  Q_exsub[1] = Q_exsub[0];     
  Q_exsub[0] = tmp16;            
    
  /* rescale excitation and associated memories */
  tmp16 = sub(new_Q, *Q_exc);
  
  if (tmp16 != 0)
  {
    voAMRWBPDecScale_sig(&exc[-len], add(len, lg), tmp16);
  }

  /* scaling factor of excitation (-1..12) */
  *Q_exc = new_Q;                                      

  return tmp16;
}

/* this function should be called after excitation update (4 subfr) and before frame synthesis */

void Rescale_mem(
  Word16 *mem_syn2,
  Decoder_State_Plus_fx *st
)
{
  Word16 expn, new_Q, tmp;

  /* find scaling of synthesis (based on min of current frame and last frame) */
  /* scaling factor of synthesis (-1..6) */

  new_Q = sub(st->Q_exc, 6);
  if (sub(new_Q, -1) < 0)
  {
    new_Q = -1;              
  }
  tmp = new_Q;                   
  
  if(sub(st->prev_Q_syn, tmp) < 0)
  {
    tmp = st->prev_Q_syn;        
  }
  st->prev_Q_syn = new_Q;        

  /* rescale synthesis memory (mem_syn2) */

  expn = sub(tmp, st->Q_syn);
  st->Q_syn = tmp;               
  
  if(expn!= 0)
  {
    voAMRWBPDecScale_sig(mem_syn2, M, expn);
  }
  return;
}

void Scale_mem2(
  Word16 Signal[],
  Word16* Old_q,
  Decoder_State_Plus_fx *st,
  Word16 Syn
)
{
  Word16 i, j, tmp, exp_e, exp_s;
  Word16 *pt_scale;
  Word32 L_tmp;

  tmp = 16;   /* take minimum of last L_FRAM_PLUS + 4 presents L_DIV*/
              /* Old_q[0], Old_q[1] represents 2 last L_FRAME_PLUS */
  for(i = 1;i < 6; i++)
  {
    
    if(sub(Old_q[i],tmp)<0)
    {
      tmp = Old_q[i];       
    }
  }

  pt_scale = &Old_q[2];  

  /* rescale all Signal to same level */
  for (j = 0;j < L_FRAME_PLUS; j+=L_DIV)
  {
    voAMRWBPDecScale_sig(&Signal[j], L_DIV, sub(tmp, *pt_scale));
    pt_scale++;    
  }
  Old_q[2] = tmp;    /* last frame update memory*/
 


  
  if (Syn > 0 )
  {
    /* rescale all memories related to synthesis */
    exp_s = sub(tmp, st->Old_Q_syn);
    st->Old_Q_syn = tmp;             move16() ;
    st->left.Q_synHF = add(st->Old_Q_syn,1);
    st->right.Q_synHF = add(st->Old_Q_syn,1);

    
    if(exp_s!=0)
    {
      voAMRWBPDecScale_sig(&(st->wmem_deemph), 1, exp_s);
      voAMRWBPDecScale_sig(st->wold_synth_pf, PIT_MAX_MAX+2*L_SUBFR, exp_s);
      voAMRWBPDecScale_sig(st->wold_noise_pf, 2*L_FILT, exp_s);



      /* rescale double-precision HP_50 memories (y2, y1, x2, x1) */
      /* mono case (LF) */
      L_tmp = L_deposit_h(st->wmem_sig_out[4]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &st->wmem_sig_out[4], &tmp);

      L_tmp = voAMRWBPDecL_Comp(st->wmem_sig_out[2], st->wmem_sig_out[3]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &st->wmem_sig_out[2], &st->wmem_sig_out[3]);

      st->wmem_sig_out[3] = sub(st->wmem_sig_out[3], shr(tmp, 1));      

      L_tmp = L_deposit_h(st->wmem_sig_out[5]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &(st->wmem_sig_out[5]), &tmp);

      L_tmp = voAMRWBPDecL_Comp(st->wmem_sig_out[0], st->wmem_sig_out[1]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &st->wmem_sig_out[0], &st->wmem_sig_out[1]);

      st->wmem_sig_out[1] = sub(st->wmem_sig_out[1], shr(tmp, 1));      

      /* Stereo case */
      /* rescale double-precision HP_50 memories (y2, y1, x2, x1) */
      /* left part */
      L_tmp = L_deposit_h(st->left.wmem_sig_out[4]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &st->left.wmem_sig_out[4], &tmp);

      L_tmp = voAMRWBPDecL_Comp(st->left.wmem_sig_out[2], st->left.wmem_sig_out[3]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &st->left.wmem_sig_out[2], &st->left.wmem_sig_out[3]);

      st->left.wmem_sig_out[3] = sub(st->left.wmem_sig_out[3], shr(tmp, 1));      

      L_tmp = L_deposit_h(st->left.wmem_sig_out[5]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &(st->left.wmem_sig_out[5]), &tmp);

      L_tmp = voAMRWBPDecL_Comp(st->left.wmem_sig_out[0], st->left.wmem_sig_out[1]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &st->left.wmem_sig_out[0], &st->left.wmem_sig_out[1]);

      st->left.wmem_sig_out[1] = sub(st->left.wmem_sig_out[1], shr(tmp, 1));      

      /* Stereo case */
      /* rescale double-precision HP_50 memories (y2, y1, x2, x1) */
      /* right part */
      L_tmp = L_deposit_h(st->right.wmem_sig_out[4]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &st->right.wmem_sig_out[4], &tmp);

      L_tmp = voAMRWBPDecL_Comp(st->right.wmem_sig_out[2], st->right.wmem_sig_out[3]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &st->right.wmem_sig_out[2], &st->right.wmem_sig_out[3]);

      st->right.wmem_sig_out[3] = sub(st->right.wmem_sig_out[3], shr(tmp, 1));      

      L_tmp = L_deposit_h(st->right.wmem_sig_out[5]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &(st->right.wmem_sig_out[5]), &tmp);

      L_tmp = voAMRWBPDecL_Comp(st->right.wmem_sig_out[0], st->right.wmem_sig_out[1]);
      L_tmp = L_shl(L_tmp, exp_s);
      voAMRWBPDecL_Extract(L_tmp, &st->right.wmem_sig_out[0], &st->right.wmem_sig_out[1]);

      st->right.wmem_sig_out[1] = sub(st->right.wmem_sig_out[1], shr(tmp, 1));      


      /* Give same scaling on HF part */
      /* Oversampling */
      voAMRWBPDecScale_sig(st->left.wmem_oversamp, L_MEM_JOIN_OVER, exp_s);
      voAMRWBPDecScale_sig(st->right.wmem_oversamp, L_MEM_JOIN_OVER, exp_s);
      voAMRWBPDecScale_sig(st->left.wmem_syn_hf, MHF, exp_s);
      voAMRWBPDecScale_sig(st->left.wold_synth_hf, D_BPF +L_SUBFR+ L_BSP + 2*D_NC + L_FDEL + 32*D_STEREO_TCX/5, exp_s);
      st->left.Lthreshold = L_shl(st->left.Lthreshold, shl(exp_s,1));      move32();   /* energy*/
      voAMRWBPDecScale_sig(st->left.wmem_oversamp_hf, 2*L_FILT, exp_s);

      voAMRWBPDecScale_sig(st->right.wmem_syn_hf, MHF, exp_s);
      voAMRWBPDecScale_sig(st->right.wold_synth_hf, D_BPF +L_SUBFR+ L_BSP + 2*D_NC + L_FDEL + 32*D_STEREO_TCX/5, exp_s);
      st->right.Lthreshold = L_shl(st->right.Lthreshold, shl(exp_s,1));        /* energy */
      voAMRWBPDecScale_sig(st->right.wmem_oversamp_hf, 2*L_FILT, exp_s);

      /* stereo part */
      voAMRWBPDecScale_sig(st->my_old_synth_fx, 2*L_FDEL+20, exp_s);
      voAMRWBPDecScale_sig(st->my_old_synth_2k_fx, L_FDEL_2k + D_STEREO_TCX + 2*(D_NC*5)/32, exp_s);
      voAMRWBPDecScale_sig(st->my_old_synth_hi_fx, 2*L_FDEL, exp_s);
      voAMRWBPDecScale_sig(st->mem_left_2k_fx, 2*L_FDEL_2k, exp_s);
      voAMRWBPDecScale_sig(st->mem_right_2k_fx, 2*L_FDEL_2k, exp_s);
      voAMRWBPDecScale_sig(st->mem_left_hi_fx, L_FDEL, exp_s);
      voAMRWBPDecScale_sig(st->mem_right_hi_fx, L_FDEL, exp_s);
      voAMRWBPDecScale_sig(st->mem_stereo_ovlp_fx, L_OVLP_2k, exp_s);
      voAMRWBPDecScale_sig(st->left.mem_d_tcx_fx,D_NC + (D_STEREO_TCX*32/5), exp_s);
      voAMRWBPDecScale_sig(st->right.mem_d_tcx_fx,D_NC + (D_STEREO_TCX*32/5), exp_s);

      voAMRWBPDecScale_sig(st->old_exc_mono_fx, HI_FILT_ORDER, exp_s);
      voAMRWBPDecScale_sig(st->left.mem_synth_hi_fx, M, exp_s);
      voAMRWBPDecScale_sig(st->right.mem_synth_hi_fx, M, exp_s);

      voAMRWBPDecScale_sig(st->right.wmem_d_nonc, D_NC, exp_s);
      voAMRWBPDecScale_sig(st->left.wmem_d_nonc, D_NC, exp_s);
    }
    return ;

  }
  else
  {
    /* rescale all memories related to excitation */    
    exp_e = sub(tmp, st->Old_Q_exc);
    st->Old_Q_exc = tmp;        
    
    if(exp_e!= 0)
    {
      st->left.Lp_amp = L_shl(st->left.Lp_amp, exp_e);          move32();
      st->right.Lp_amp = L_shl(st->right.Lp_amp, exp_e);        move32();
    }
    return ;
  }

}


void Scale_mem_tcx(
  Word16 xnq[],         /* (i/o)  : xnq to rescale       Q_exc */
  Word16 lg,            /* (i)    : frame size                        */
  Word32 Lgain,         /* (i)    : Q16 */ 
  Word16 mem_syn[], 
  Decoder_State_Plus_fx *st
)
{
#if (FUNC_SCALE_MEM_TCX_OPT)
  Word16 i, max, tmp, expn, tmpgain, tmp16, new_Q, tmpQ;


  tmp16 = norm_l(Lgain);
  tmpgain = (Word16)(L_shl(Lgain, tmp16) >> 16);

  tmp16 -= 15;

  /* Apply gain */
  for(i = 0; i < lg; i++)
  {
    xnq[i] = (xnq[i] * tmpgain) >> 15;
  }
  
  /* Find maximum scaling*/
  max = 0;
  for(i = 0;i < lg; i++)
  {
    if (abs_s(xnq[i]) > max)
    {
      max = abs_s(xnq[i]);
    }
  }

  /* find scaling (tmp) to set max = [4096..8191[ */
  tmp = norm_s(max) - 2;
  if(tmp + tmp16 > Q_MAX)
  {
    tmp = Q_MAX - tmp16;
  }
  tmpQ = tmp + tmp16;

  new_Q = tmpQ;       /* smooth scaling */
  for (i = 0; i < 7; i++)
  {
    if (st->mem_subfr_q[i] < new_Q)
    {
      new_Q = st->mem_subfr_q[i];
    }
  }
  tmp = new_Q - tmp16;
    
  if (tmp != 0)
  {
    voAMRWBPDecScale_sig(xnq, lg, tmp);
  }

  st->Q_exc = new_Q;
  
  if(lg == 288)
  {
    st->mem_subfr_q[6] = st->mem_subfr_q[5];
    st->mem_subfr_q[5] = st->mem_subfr_q[4];
    st->mem_subfr_q[4] = st->mem_subfr_q[3];
    st->mem_subfr_q[3] = tmpQ;
    st->mem_subfr_q[2] = tmpQ;
    st->mem_subfr_q[1] = tmpQ;
    st->mem_subfr_q[0] = tmpQ;
  }
  else
  {
    for(i = 0;i < 7; i++)
    {
      st->mem_subfr_q[i] = tmpQ;
    }
  }

  /* rescale synthesis memory (mem_syn) */
  new_Q -= 4;
  if (sub(new_Q, -1) < 0)
  {
    if(st->Q_exc < 0) /* if Qexc < 0 -> Hi-level excitation so play safe and allow Qsyn < -1 ref:diva48m fr63*/
    {
      new_Q = st->Q_exc;
    }
    else
    {
      new_Q = -1;
    }
  }

  tmp = new_Q;

  if(st->prev_Q_syn < tmp)
  {
    tmp = st->prev_Q_syn;
  }
  st->prev_Q_syn = new_Q;

  

  expn = tmp - st->Q_syn;
  st->Q_syn = tmp;

  if(expn!= 0)
  {
    voAMRWBPDecScale_sig(mem_syn, M, expn);
  }
  if(st->Q_exc != st->Old_Qxnq)
  {
    /* rescale others memories */
    voAMRWBPDecScale_sig(st->wwovlp, L_OVLP, sub(st->Q_exc, st->Old_Qxnq));   
    voAMRWBPDecScale_sig(&(st->wmem_wsyn), 1, sub(st->Q_exc, st->Old_Qxnq));
  }
  st->Old_Qxnq  = st->Q_exc;

  return;
  
#else

  Word16 i, max, tmp, expn, tmpgain, tmp16, new_Q, tmpQ;


  tmp16 = norm_l(Lgain);
  tmpgain = extract_h(L_shl(Lgain, tmp16));

  tmp16 = sub(tmp16,15);

  /* Apply gain */
  for(i = 0; i < lg; i++)
  {
    xnq[i] = mult(xnq[i], tmpgain);        
  }
  
  /* Find maximum scaling*/
  max = 0;  
  for(i = 0;i < lg; i++)
  {
    
    if (sub(abs_s(xnq[i]),max) > 0)
    {
      max = abs_s(xnq[i]);
    }
  }

  /* find scaling (tmp) to set max = [4096..8191[ */
  tmp = sub(norm_s(max), 2);
  
  if(sub(add(tmp,tmp16), Q_MAX)>0)
  {
    tmp = sub(Q_MAX,tmp16);    
  }
  tmpQ = add(tmp, tmp16);

  new_Q = tmpQ;       /* smooth scaling */
  for (i = 0; i < 7; i++)
  {
    
    if (sub(st->mem_subfr_q[i], new_Q) < 0)
    {
      new_Q = st->mem_subfr_q[i];           
    }
  }
  tmp = sub(new_Q, tmp16);
    
  
  if (tmp != 0)
  {
    voAMRWBPDecScale_sig(xnq, lg, tmp);
  }

  st->Q_exc = new_Q;    
  
  
  if(sub(lg,288)==0)
  {
    st->mem_subfr_q[6] = st->mem_subfr_q[5];     
    st->mem_subfr_q[5] = st->mem_subfr_q[4];     
    st->mem_subfr_q[4] = st->mem_subfr_q[3];     
    st->mem_subfr_q[3] = tmpQ;            
    st->mem_subfr_q[2] = tmpQ;            
    st->mem_subfr_q[1] = tmpQ;            
    st->mem_subfr_q[0] = tmpQ;            
  }
  else
  {
    for(i = 0;i < 7; i++)
    {
      st->mem_subfr_q[i] = tmpQ;            
    }
  }

  /* rescale synthesis memory (mem_syn) */
  new_Q = sub(new_Q, 4);
  if (sub(new_Q, -1) < 0)
  {
    
    if(st->Q_exc < 0) /* if Qexc < 0 -> Hi-level excitation so play safe and allow Qsyn < -1 ref:diva48m fr63*/
    {
      new_Q = st->Q_exc;        
    }
    else
    {
      new_Q = -1;          
    }
  }

  tmp = new_Q;                   

  
  if(sub(st->prev_Q_syn, tmp) < 0)
  {
    tmp = st->prev_Q_syn;        
  }
  st->prev_Q_syn = new_Q;        

  

  expn = sub(tmp, st->Q_syn);
  st->Q_syn = tmp;               

  
  if(expn!= 0)
  {
    voAMRWBPDecScale_sig(mem_syn, M, expn);
  }
  
  if(sub(st->Q_exc, st->Old_Qxnq) != 0)
  {
    /* rescale others memories */
    voAMRWBPDecScale_sig(st->wwovlp, L_OVLP, sub(st->Q_exc, st->Old_Qxnq));   
    voAMRWBPDecScale_sig(&(st->wmem_wsyn), 1, sub(st->Q_exc, st->Old_Qxnq));
  }
  st->Old_Qxnq  = st->Q_exc;               

  return;
#endif
}

void Updt_mem_q(
  Word16* old_sub_q,
  Word16 n,
  Word16 new_Q
)
{

  Word16 i, j;  

  
  for(j = 0; j <n; j++)
  {
    for(i = 0;i < 5; i++)
    {
      old_sub_q[i] = old_sub_q[i+1];     
    }
    old_sub_q[i] = new_Q;            
  }
}
