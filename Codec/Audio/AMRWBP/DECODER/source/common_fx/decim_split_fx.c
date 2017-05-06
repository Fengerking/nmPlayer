
//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>

#include "amr_plus_fx.h"



#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"


#define L_FRAME_MAX   (L_FRAME48k)

  
Word16 Decim_split_12k8(  /* number of sample decimated         */
  Word16 sig_fs[],        /* (i)  : signal to decimate         */
  Word16 lg_input,        /* input:  2*L_FRAME44k if 44kHz      */
  Word16 sig12k8_lf[],    /* (o)  : LF decimated signal        */
  Word16 sig12k8_hf[],    /* (o)  : HF decimated signal        */
  Word16 lg,              /* (i)  : length of LF and HF        */
  Word16 fac_fs,          /* (i)  : at 48kHz, scale fac = fac_fs/FSCALE_DENOM */
  Word16 mem[],           /* (i/o): mem[L_MEM_DECIM_SPLIT]     */
  Word16 *frac_mem        /* (i/o): interpol fraction memory   */
)
{
  Word16 i, j, ncoef, L_frame, L_frame_int;
  Word16 signal[(2*(L_FILT_SPLIT+L_FILT_DECIM))+2*L_FRAME_MAX], *sig;
  Word16 gain, *x1, *x2;
  Word16 fac_up = 0, fac_down, exp_u, m_fu, tmp16;
  const Word16 *filter;
  Word32 Ltmp, s;
  Word16 n_fac_down;  

#ifdef FILTER_44kHz
  if (L_sub(lg_input,(2*L_FRAME44k)) == 0)
  {
    fac_up = shl(fac_fs,3);
    fac_down = 3*441;
    n_fac_down = 17199;
    filter = (Word16 *)Filter_LP165;
  }
#endif
#ifdef FILTER_48kHz
  if (L_sub(lg_input,(2*L_FRAME48k)) == 0)
  {
    fac_up = shl(fac_fs,3);
    fac_down = 180*8; 
    n_fac_down = 18720;
    filter = (Word16*)Filter_LP180; 
  }
#endif

  exp_u = norm_s(fac_up);
  m_fu = shl(fac_up, exp_u);

  if(n_fac_down > m_fu)
  {
    n_fac_down = (n_fac_down >> 1);
    ncoef = div_s(n_fac_down, m_fu);    /* 18720 = (180*(L_FILT_OVER+1))<<1 */
    ncoef = shr(ncoef, sub(15-1, exp_u));
  }
  else
  {
    ncoef = div_s(n_fac_down, m_fu);    /* 18720 = (180*(L_FILT_OVER+1))<<1 */
    ncoef = shr(ncoef, sub(15, exp_u));
  }


  gain = div_s(shl(fac_fs,7), 23040);   /* gain in Q15 */

  /* frames length */
  L_frame_int = shl(lg,1);     /* 25k6 rate */

  /*L_frame = ((L_frame_int*fac_down)+(*frac_mem))/fac_up;*/
  Ltmp = L_mult(L_frame_int, fac_down);
  Ltmp = L_mac(Ltmp, *frac_mem, 1);

  L_frame = div_s(extract_h(L_shl(Ltmp, exp_u)), m_fu);

  tmp16 = add(L_frame, 1);
  Ltmp = L_msu(Ltmp, tmp16, fac_up);
  
  if (Ltmp >= 0)
  {
    L_frame = tmp16;            
  } 
  /* load buffer & update memory */

  for (i=0; i<(2*(L_FILT_SPLIT+L_FILT_DECIM)); i++) 
  {
    signal[i] = mem[i];       
  }

  for (i=0; i<L_frame; i++) 
  {
    signal[i+(2*(L_FILT_SPLIT+L_FILT_DECIM))] = sig_fs[i];      
  }

  sig = signal + (2*L_FILT_SPLIT);

  for (i=0; i<(2*L_FILT_DECIM); i++) 
  {
    mem[i+(2*L_FILT_SPLIT)] = sig[i+L_frame];     
  }

  /* decimation to 25.6kHz */
  Interpol_mem(sig+ncoef, sig, L_frame_int, filter, ncoef, 
               fac_up, fac_down, gain, frac_mem);

  /* update memory */

  for (i=0; i<(2*L_FILT_SPLIT); i++) 
  {
    mem[i] = signal[i+L_frame_int];     
  }

  /* band split (25.6kHz --> 2 x 12.8kHz) */

  sig = signal + L_FILT_SPLIT;

  for(i=0; i<lg; i++) 
  {
    s = L_mult(sig[i*2],16384);
    x1 = &sig[(i*2)-1];         
    x2 = &sig[(i*2)+1];                 
    for(j=0; j<L_FILT_JOIN; j++, x1-=2, x2+=2) 
    {
      /*s += (*x1 + *x2) * Inter2_coef[j];*/
      s = L_mac(s, *x1, Inter2_coef[j]);
      s = L_mac(s, *x2, Inter2_coef[j]);
    }
    sig12k8_lf[i] = round16(s);     
    sig12k8_hf[i] = sub(sig[i*2], sig12k8_lf[i]);    

  }

  return(L_frame);
}

