#include <stdlib.h>
#include <stdio.h>
#include "amr_plus_fx.h"


#include "typedef.h"
#include "oper_32b.h"
#include "log2.h"
#include "math_op.h"
#include "basic_op.h"
#include "count.h"

/* prototypes*/

void Init_tcx_stereo_encoder(Coder_State_Plus_fx *st)
{
    st->mem_stereo_ovlp_size = 0;       move16();
    Set_zero(st->mem_stereo_ovlp,L_OVLP_2k);
}


static void Comp_gain_shap_cod(
  Word16 *wm,               /* i:     Qwm */
  Word16 *gain_shap_cod,    /* o:     Q14 */
  Word16 *gain_shap_dec,    /* o:     Q14 */
  Word16 lg,                /* i:         */
  Word16 Qwm                /* i:         */
)
{
  /* gain_shap_cod sometimes overflows */
  Word16 i,j,k;
  Word16 tmp, expt, expt2, frac, log_sum, inv_lg16;
  Word32 L_tmp;

  log_sum = 0;        move16();

  test();test();move16();
  if (sub(lg,192) == 0)
  {
    inv_lg16 = 1365;    
  }
  else if (sub(lg, 96) ==0)
  {
    inv_lg16 = 2731;   
  }
  else  /* 48*/
  {
    inv_lg16 = 5461; 
  }


  j = 0;   move16(); /*ptr*/
  for(i=0;i<lg;i+=16)
  {      
    L_tmp = 1;      move32();
    for(k=0;k<16;k++) 
    {
      tmp = shr(wm[i+k],2);      /* 13+13+4+1 = 31 */
      L_tmp = L_mac(L_tmp,tmp,tmp);	
    }

    Log2(L_tmp,&expt,&frac);
    expt = sub(expt, add(shl(Qwm,1),1-4));
    gain_shap_dec[j] = add(shl(expt,8),shr(frac,7));             move16();
    log_sum = add(log_sum,gain_shap_dec[j]);  /* Q8 */
    gain_shap_dec[j] = mult_r(gain_shap_dec[j],16384);      move16(); /* 0.5 = sqrt in log domain  */
    j++;    /*ptr*/
  }
  log_sum = mult_r(log_sum,inv_lg16); /* 0.33333*0.5 in Q15  */
                               

  expt = shr(log_sum,8);  /* extract exponent */
  frac = shl(sub(log_sum,shl(expt,8)),7); /* extract fraction   */

  L_tmp = Pow2(expt,frac);
  log_sum = extract_h(Pow2(30,frac));
  log_sum = div_s(16384,log_sum);
  j = 0;     move16(); /*ptr*/
  for(i=0;i<lg;i+=16) 
  {
    tmp = gain_shap_dec[j]; move16();
    expt2 = shr(tmp,8);  /* extract exponent */
    frac = shl(sub(tmp,shl(expt2,8)),7);  /* extract fraction  */ 
    tmp = extract_h(Pow2(30,frac));
    tmp = mult(tmp,log_sum);
    gain_shap_dec[j] = shl(tmp,sub(expt2,expt));    move16();

    test();
    if (sub(gain_shap_dec[j],8192) < 0)
    {
      gain_shap_dec[j] = 8192;  move16();
    }
    tmp = div_s(8192,tmp);
    gain_shap_cod[j] = shr(tmp,sub(expt2,expt));      move16();

    j++; /* ptr*/

  }

}

static Word16 Compute_xn_target(Word16 *xn, Word16 *wm, Word16 gain_pan, Word16 lg)
{
  Word32 Ltmp,Lmax;
  Word32 Lxn[L_TCX_LB];
  Word16 tmp;
  Word16 i ;

  Lmax = 1; move32();
  for(i = 0;i < lg; i++)
  { 
    Lxn[i] = L_msu(L_mult(xn[i], 16384), gain_pan, wm[i]);  move32();
    Ltmp = L_abs(Lxn[i]);
    test();
    if (L_sub(Ltmp,Lmax) > 0) 
    {
       Lmax = Ltmp;         move32();
    }
  }
  tmp = sub(norm_l(Lmax),1); move16();
  for(i = 0;i < lg; i++)
  {
    xn[i] = round(L_shl(Lxn[i],tmp));  move16();
  }
  tmp = sub(tmp,1); 
  return tmp;
}


static Word16 Q_gain_pan(    /*  output: return quantization index */
  Word16 *gain      /*  in/out: quantized gain            */
  )
{
  Word16 index, tmp16;
  
  tmp16 = shr(*gain,2);
  tmp16 = add(tmp16,8192);
  tmp16 = add(tmp16,64); /*  +0.5 */
  index = shr(tmp16,7);
  test();
  if(sub(index,127)>0)
  {
    index = 127;    move16();
  }
  tmp16 = shl(index,8);
  *gain = shl(sub(tmp16,16384),1);      move16();
  return index;
}

static void Ctcx_stereo(
  Word16 wside[],       /* i   : speech side            */
  Word16 wmono[],       /* i   : mono speech            */
  Word16 wsynth[],      /* i/o : synth[-M..lg]          */
  Word16 wwovlp[],      /* i/o : wovlp[0..127]          */
  Word16 ovlp_size,     /* i   : 0, 64 or 128 (0=acelp) */
  Word16 L_frame,       /* i   : frame length           */
  Word16 nb_bits,       /* i   : number of bits allowed */
  Word16 sprm[],        /* o   : tcx parameters         */
  Word16 pre_echo,      /* i   : Presence of pre echo ? */    
  Word16 Q_in           /* i   : Input scaling          */        
)
{
  Word16 i, i_subfr, lg;
  Word16 lext;

  Word32 Ltmp, Lgain;
  Word16 tmp16, fac_ns;
  Word16 gain_pan, inc2,Q_tcx, e_tmp, mant_ener, exp_ener;

  Word16  xri[L_TCX_LB];

  Word16 xn[L_TCX_LB];
  Word16 wm[L_TCX_LB];
  Word16 xnq[L_TCX_LB];
  Word16 window[L_TCX_LB];
  Word16 gain_shap_cod[8];
  Word16 gain_shap_dec[8];
  Word16 xn_scl;

  lext = 32;     move16();
  inc2 = 4;      move16();
  /*------ set length of overlap (lext) and length of encoded frame (lg) -----*/
  test();test();test();
  switch (L_frame) 
  {
    case 40:
      lext = 8;     move16();
      inc2 = 16;  move16();
      break;
    case 80:
      lext = 16;    move16();
      inc2 = 8;  move16();
      break;
    case 160:
      lext = 32;    move16();
      inc2 = 4;  move16();
      break;
  };
  lg = add(L_frame, lext);

  /* built window for overlaps section */
  Cos_window(window, ovlp_size, lext, inc2);
  /* reduce by the correlation with the mono  */
  for (i=0; i<lg; i++)
  {
    xn[i] = wside[i];      move16();
    wm[i] = wmono[i];      move16();
  }

  /* xn[] windowing for TCX overlap and correlation */
  Windowing(ovlp_size, xn, window, 0, 0);
  Windowing(ovlp_size, wm, window, 0, 0);

  Windowing(lext, xn, window, L_frame, ovlp_size);
  Windowing(lext, wm, window, L_frame, ovlp_size);

  test();
  if(pre_echo>0)
  {
    /* Anisse: This mode replaces somehow the ACELP mode */
    /* compensate for the gain */
    Comp_gain_shap_cod(wm,gain_shap_cod,gain_shap_dec ,lg, Q_in);
    Apply_gain_shap(lg, xn, gain_shap_cod);
    Apply_gain_shap(lg, wm, gain_shap_cod);
  }


  /* compute the optimal panning gain */
  gain_pan = extract_h(L_shl(Get_gain(xn, wm, lg, 1, &mant_ener, &exp_ener),15));

  /* do not amplify the mono */
  /* if(gain_pan > 1.0) gain_pan =1.0;*/
  /* if(gain_pan < -1.0) gain_pan =-1.0;*/
  /* quantize the panning gain (would be better if predictive scalar quantizer is used)*/

  nb_bits = sub(nb_bits,7);
  sprm[0] = Q_gain_pan(&gain_pan);  move16();

  xn_scl = Compute_xn_target(xn,wm,gain_pan,lg);
  /*-----------------------------------------------------------*
  * Compute the FFT of xn[].                                  *
  * Coefficients (xri[]) order are                            *
  *    re[0], re[n/2], re[1], re[2], ... re[n/2-1], im[n/2-1] *
  * Note that last FFT element (re[n/2]) is zeroed.           *
  *-----------------------------------------------------------*/
  Fft3(xn, xri, lg);
  FFT_reorder(xri, lg);
  xri[1] = 0;    move16();      /* freq bin at 1000 Hz zeroed */
  /*-----------------------------------------------------------*
  * Spectral algebraic quantization                           *
  * with adaptive low frequency emphasis/deemphasis.          *
  * Noise factor is the average level of unquantized freq.    * 
  *-----------------------------------------------------------*/

  Adap_low_freq_emph(xri, shl(lg,2));

  nb_bits = sub(nb_bits,7);        /* gain = 7 bits */
  fac_ns = AVQ_Cod(xri, sprm+2, nb_bits, shr(lg,3));

  for(i=0; i<lg; i++) 
  {
    xri[i] = sprm[i+2];     move16();
  }
  /*-----------------------------------------------------------*
  * Compute inverse FFT for obtaining xnq[] without noise.    *
  * Coefficients (xri[]) order are                            *
  *    re[0], re[n/2], re[1], re[2], ... re[n/2-1], im[n/2-1] *
  * Note that last FFT element (re[n/2]) is zeroed.           *
  *-----------------------------------------------------------*/
  Scale_tcx_ifft(xri, lg ,&Q_tcx);
  Adap_low_freq_deemph(xri, shl(lg,2));

  xri[1] = 0;        move16();      /* freq bin at 6400 Hz zeroed */
  Ifft_reorder(xri, lg);
  Ifft3(xri, xnq, lg);
  /*-----------------------------------------------------------*
  * find and quantize gain, multiply xnq[] by gain.           *
  * windowing of xnq[] for TCX overlap.                       *
  *-----------------------------------------------------------*/
  /* scale to normal scale for gain computation */
  Scale_sig(xnq, lg, negate(xn_scl));
  Scale_sig(xn, lg, negate(xn_scl));

  if (pre_echo>0) 
  {
    Apply_gain_shap(lg, xnq, gain_shap_dec );
    Apply_gain_shap(lg, xn, gain_shap_dec );
  }
  
  Lgain = Get_gain(xn, xnq, lg, Q_in, &mant_ener, &exp_ener);
  sprm[1] = Q_gain_tcx(lg, &Lgain, mant_ener, exp_ener);     move16();

  e_tmp = norm_l(Lgain);
  tmp16 = extract_h(L_shl(Lgain,e_tmp));
  e_tmp = sub(e_tmp, add(Q_in,14));  /* To scale xnq with Q_in -1 to fit wm *gain_pan  */
  for (i=0; i<lg; i++) 
  {
    /* xnq[i] = gain *xnq[i] + gain_pan * wm[i];*/
    Ltmp = L_mult(xnq[i], tmp16);
    Ltmp = L_mac(L_shr(Ltmp, e_tmp), wm[i], gain_pan);
    xnq[i] = round(L_shl(Ltmp,1));      move16();     
  }
  /* adaptive windowing on overlap (beginning and end of frame) */
  Windowing(ovlp_size, xnq, window, 0, 0);
  Windowing(lext, xnq, window, L_frame, ovlp_size);
  for (i=L_frame+lext; i<lg; i++) 
  {
    xnq[i] = 0;       move16();
  }

  /*-----------------------------------------------------------*
  * TCX overlap and add.  Update memory for next overlap.     *
  *-----------------------------------------------------------*/
  Apply_tcx_overlap(xnq,wwovlp,lext,L_frame);
 /*-----------------------------------------------------------*
  * find excitation and synthesis                             *
  *-----------------------------------------------------------*/
  for (i_subfr=0; i_subfr<L_frame; i_subfr++)
  {
    wsynth[i_subfr] = xnq[i_subfr];      move16();
  }

  return;
}

/*-----------------------------------------------------------------*
* Funtion c_stereo                                             *
* ~~~~~~~~~~~~~~~~~~~~                                            *
*-----------------------------------------------------------------*/

void Cod_tcx_stereo(
  Word16 wmono_2k[],        /* i   : mono speech                */
  Word16 wright_2k[],       /* i   : speech side                */
  Word16 param[],           /* o   : tcx parameters             */
  Word16 brMode,            /* i   : Stereo bit rate index      */
  Word16 mod[],             /* i   : Mono modes used            */
  Word16 fscale,            /* i   : Internal frequency scaling */
  Coder_State_Plus_fx *st   /* i/o : Encoder states	            */
  )
{
  /* Scalars */
  Word16 i, k, i2, i1, nbits;
  Word16 ovlp_size[4+1];
  Word16 prm_tcx[NPRM_TCX80_D];
  Word16  *prm;
  Word16 sig_buf_mono[TCX_L_FFT_2k];
  Word16 sig_buf_right[TCX_L_FFT_2k];
  Word16 sig_buf_side[TCX_L_FFT_2k];

  Word16 synth_tcx[L_FRAME_2k];
  Word16 synth[L_FRAME_2k];


  Word16 ovlp[L_OVLP_2k*5];
  Word16 ovlp_tcx[L_OVLP_2k];
  /* Scalars */

  Word16 snr, snr1, snr2, tmp16;
  
  /* number of bits per frame (80 ms) */
  nbits = sub(StereoNbits_FX[brMode], 24+4);
  test();
  if (sub(StereoNbits_FX[brMode],300+4) > 0)
  {
    nbits = sub(nbits, 24);
  }

  /* the following assumes that we have enough look back for the */
  /* wiener stereo that we can dirctly copy the buffer into the working*/
  /* buffer without memory saving.*/

  /* compute the left signal buffer*/
  Set_zero(sig_buf_mono,TCX_L_FFT_2k);

  test();
  if(fscale == 0) 
  {
    Copy(&wmono_2k[-TCX_STEREO_DELAY_2k],sig_buf_mono,L_FRAME_2k+L_OVLP_2k);
  }
  else
  {
    Copy(&wmono_2k[-TCX_STEREO_DELAY_2k-L_SUBFR_2k],sig_buf_mono,L_FRAME_2k+L_OVLP_2k);
  }

  /* compute the right signal buffer*/
  Set_zero(sig_buf_right,TCX_L_FFT_2k);

  test();
  if(fscale == 0) 
  {
    Copy(&wright_2k[-TCX_STEREO_DELAY_2k],sig_buf_right,L_FRAME_2k+L_OVLP_2k);
  }
  else 
  {
    Copy(&wright_2k[-TCX_STEREO_DELAY_2k-L_SUBFR_2k],sig_buf_right,L_FRAME_2k+L_OVLP_2k);
  }

  /* compute the side signal buffer*/
  Set_zero(sig_buf_side,TCX_L_FFT_2k);

  for(i=0;i<TCX_L_FFT_2k;i++) 
  {
    sig_buf_side[i] = sub(sig_buf_mono[i],sig_buf_right[i]);   move16();
  }

  /*---------------------------------------------------------------*
  *  Call TCX codec
  *---------------------------------------------------------------*/

  ovlp_size[0] = st->mem_stereo_ovlp_size;      move16();
  Copy(st->mem_stereo_ovlp, ovlp, L_OVLP_2k);
  
  snr2 = 0;      move16();
  for (i1=0; i1<2; i1++)
  {
    snr1 = 0;      move16();
    for (i2=0; i2<2; i2++)
    {
      k = add(shl(i1,1),i2);

      /* set pointer to parameters */
      prm = param + (k*NPRM_DIV_TCX_STEREO);        move16();


      /*--------------------------------------------------*
      * Call 20MS TCX with pre-echo coder and find segmental SNR *
      *--------------------------------------------------*/

      Copy(&ovlp[k*L_OVLP_2k], ovlp_tcx, L_OVLP_2k);
      Ctcx_stereo(&sig_buf_side[k*L_DIV_2k], &sig_buf_mono[k*L_DIV_2k], &synth_tcx[k*L_DIV_2k], 
                    ovlp_tcx, ovlp_size[k], L_FRAME_2k/4, sub(shr(nbits,2),2), prm_tcx, 1, st->Q_new);

      tmp16 = Segsnr(&sig_buf_side[k*L_DIV_2k], &synth_tcx[k*L_DIV_2k], L_FRAME_2k/4,L_DIV_2k);
      snr = tmp16;       move16();
      
      mod[k] = 0;       move16();

      ovlp_size[k+1] = L_OVLP_2k/4;     move16();

      /*--------------------------------------------------*
      * Call 20MS TCX coder and find segmental SNR       *
      *--------------------------------------------------*/


      Copy(ovlp_tcx, &ovlp[(k+1)*L_OVLP_2k], L_OVLP_2k);
      Copy(&synth_tcx[k*L_DIV_2k], &synth[k*L_DIV_2k], L_FRAME_2k/4);

      Copy(prm_tcx, prm, NPRM_TCX20_D);

      /*--------------------------------------------------*
      * Call 20MS TCX coder and find segmental SNR       *
      *--------------------------------------------------*/

      Copy(&ovlp[k*L_OVLP_2k], ovlp_tcx, L_OVLP_2k);

      Ctcx_stereo(&sig_buf_side[k*L_DIV_2k], &sig_buf_mono[k*L_DIV_2k], &synth_tcx[k*L_DIV_2k], 
                    ovlp_tcx, ovlp_size[k], L_FRAME_2k/4, sub(shr(nbits,2),2), prm_tcx, 0, st->Q_new);

      tmp16 = Segsnr(&sig_buf_side[k*L_DIV_2k], &synth_tcx[k*L_DIV_2k], L_FRAME_2k/4,L_DIV_2k);
      test();
      if(sub(tmp16, snr) > 0) 
      {
        snr = tmp16;        move16();

        mod[k] = 1;       move16();

        ovlp_size[k+1] = L_OVLP_2k/4;     move16();

        Copy(ovlp_tcx, &ovlp[(k+1)*L_OVLP_2k], L_OVLP_2k);
        Copy(&synth_tcx[k*L_DIV_2k], &synth[k*L_DIV_2k], L_FRAME_2k/4);

        Copy(prm_tcx, prm, NPRM_TCX20_D);
      }
      snr1  = add(snr1, mult_r(snr,16384));

    } /* end of i2 */


    k = shl(i1,1);

    /* set pointer to parameters */
    prm = param + (k*NPRM_DIV_TCX_STEREO);      move16();

    /*--------------------------------------------------*
    * Call 40MS TCX coder and find segmental SNR       *
    *--------------------------------------------------*/
    Copy(&ovlp[k*L_OVLP_2k], ovlp_tcx, L_OVLP_2k);


    Ctcx_stereo(&sig_buf_side[k*L_DIV_2k], &sig_buf_mono[k*L_DIV_2k], &synth_tcx[k*L_DIV_2k], ovlp_tcx,
            ovlp_size[k], L_FRAME_2k/2, sub(shr(nbits,1), 4), prm_tcx, 0, st->Q_new);
    tmp16 = Segsnr(&sig_buf_side[k*L_DIV_2k], &synth_tcx[k*L_DIV_2k], L_FRAME_2k/2, L_DIV_2k);				

    /*--------------------------------------------------------*
    * Save tcx parameters if tcx segmental SNR is better     *
    *--------------------------------------------------------*/
    test();
    if (sub(tmp16, snr1) > 0)
    {
      snr1 = tmp16;         move16();
      for (i=0; i<2; i++) 
      {
        mod[k+i] = 2;       move16();
      }
      ovlp_size[k+2] = L_OVLP_2k/2;     move16();

      Copy(ovlp_tcx, &ovlp[(k+2)*L_OVLP_2k], L_OVLP_2k);
      Copy(&synth_tcx[k*L_DIV_2k], &synth[k*L_DIV_2k], L_FRAME_2k/2);

      Copy(prm_tcx, prm, NPRM_TCX40_D);
    }

    snr2 =add(snr2, mult_r(snr1,16384));
  }/* end of i1 */

  k = 0;      move16();

  /* set pointer to parameters */
  prm = param + (k*NPRM_DIV_TCX_STEREO);        move16();

  /*--------------------------------------------------*
  * Call 80MS TCX coder and find segmental SNR       *
  *--------------------------------------------------*/

  Copy(&ovlp[k*L_OVLP_2k], ovlp_tcx, L_OVLP_2k);

  Ctcx_stereo(sig_buf_side, sig_buf_mono, synth_tcx, ovlp_tcx, ovlp_size[k], L_FRAME_2k,
          sub(nbits,8), prm_tcx, 0, st->Q_new);

  tmp16 = Segsnr(sig_buf_side, synth_tcx, L_FRAME_2k, L_DIV_2k);
  /*--------------------------------------------------------*
  * Save tcx parameters if tcx segmental SNR is better     *
  *--------------------------------------------------------*/
  test();
  if (sub(tmp16, snr2) > 0)
  {
    snr2 = tmp16;           move16();
    for (i=0; i<4; i++) 
    {
      mod[k+i] = 3;       move16();
    }

    ovlp_size[k+4] = L_OVLP_2k;     move16();

    Copy(ovlp_tcx, &ovlp[(k+4)*L_OVLP_2k], L_OVLP_2k);
    Copy(synth_tcx, &synth[k*L_DIV_2k], L_FRAME_2k);

    Copy(prm_tcx, prm, NPRM_TCX80_D);
  }

  /*--------------------------------------------------*
  * Update memory.		                            *
  *--------------------------------------------------*/
  st->mem_stereo_ovlp_size = ovlp_size[4];      move16();

  Copy(&ovlp[4*L_OVLP_2k], st->mem_stereo_ovlp, L_OVLP_2k);

}

