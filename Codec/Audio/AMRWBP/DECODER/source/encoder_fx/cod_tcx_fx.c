#include "amr_plus_fx.h"
#include "basic_op.h"
#include "count.h"

Word16 Coder_tcx(   
  Word16 A[],         /* i  :  coefficients NxAz[M+1]  */
  Word16 speech[],    /* i  :  speech[-M..lg]          */
  Word16 *mem_wsp,    /* i/o: memory of target        */
  Word16 *mem_wsyn,   /* i/o: memory of quantized xn  */
  Word16 synth[],     /* i/o: synth[-M..lg]           */
  Word16 exc_[],      /* o  : exc[0..lg]              */
  Word16 wovlp[],     /* i/o:    wovlp[0..127]           */
  Word16 ovlp_size,   /* i  :  0, 64 or 128 (0=acelp)  */
  Word16 L_frame,     /* i  :  frame length            */
  Word16 nb_bits,     /* i  :  number of bits allowed  */
  Word16 prm[],       /* o  : tcx parameters          */  
  Coder_State_Plus_fx *st /* i/o : coder memory state   */        
  )
{


  Word16 i, i_subfr, lext, lg, index;
  Word16 tmp16, fac_ns_, inc2;

  Word16 *p_A, Ap[M+1];
  Word16 *xri, *xn, *xnq;
  Word16 window[256],mant_ener, exp_ener;              
  Word16 Q_tcx, e_tmp, temp[M] , max, max1;
  Word32 Ltmp, Lgain;


  xn = synth;    move16();
  xnq = xri = exc_;    move16();

  /*------ set length of overlap (lext) and length of encoded frame (lg) -----*/
  lext = L_OVLP;        move16();
  inc2 = 1;   move16();

  test();
  if (sub(L_frame,(L_FRAME_PLUS/2)) == 0) 
  {
    lext = L_OVLP/2;        move16();
    inc2 = 2;   move16();

  }
  test();
  if (sub(L_frame,(L_FRAME_PLUS/4)) == 0) 
  {
    lext = L_OVLP/4;        move16();
    inc2 = 4;   move16();

  }

  lg = add(L_frame, lext);

  /* built window for overlaps section */
  Cos_window(window, ovlp_size, lext, inc2);
  /*-----------------------------------------------------------*
   * Find target xn[] (weighted speech when prev mode is TCX)  *
   * Note that memory isn't updated in the overlap area.       *
   *-----------------------------------------------------------*/

  p_A = A;    move16();
  for (i_subfr=0; i_subfr<L_frame; i_subfr+=L_SUBFR) 
  {
    Weight_a(p_A, Ap, GAMMA1_FX, M);
    Residu2(Ap, M,  &speech[i_subfr], &xn[i_subfr], L_SUBFR);
    p_A += (M+1);
  }
  Deemph(xn, TILT_FAC_FX, L_frame, mem_wsp);

  /* overlap area (xn[L_frame..L_frame+lext]) */
  Weight_a(p_A, Ap, GAMMA1_FX, M);
  Residu2(Ap, M, &speech[L_frame], &xn[L_frame], lext);
  tmp16 = *mem_wsp;       move16();
  Deemph(&xn[L_frame], TILT_FAC_FX, lext, &tmp16);

  /* remove weighted ZIR when previous frame is ACELP */
  test();
  if (ovlp_size == 0) 
  {
    for (i=0; i<(2*L_SUBFR); i++) 
    {
      xn[i] = sub(xn[i], wovlp[i]);    move16();
    }
  }

  /* xn[] windowing for TCX overlap */

  for (i=0; i<ovlp_size; i++) 
  {
    xn[i] = mult_r(xn[i], window[i]);  move16();
  }

  for (i=0; i<lext; i++) 
  {
    xn[L_frame+i] = mult_r(xn[L_frame + i], window[ovlp_size+i]);    move16();
  }
 /*-----------------------------------------------------------*
  * Compute the FFT of xn[].                                  *
  * Coefficients (xri[]) order are                            *
  *    re[0], re[n/2], re[1], re[2], ... re[n/2-1], im[n/2-1] *
  * Note that last FFT element (re[n/2]) is zeroed.           *
  *-----------------------------------------------------------*/
  fft9_fx(xn, xri, lg);
  FFT_reorder(xri,lg);

  xri[1] = 0;      move16();       /* freq bin at 6400 Hz zeroed */

 /*-----------------------------------------------------------*
  * Spectral algebraic quantization                           *
  * with adaptive low frequency emphasis/deemphasis.          *
  * Noise factor is the average level of unquantized freq.    * 
  *-----------------------------------------------------------*/
  Adap_low_freq_emph(xri, lg);
  nb_bits = sub(nb_bits,(3+7));        /* fac_ns = 3 bits, gain = 7 bits */

  /* remove also the redundancy bits of the TCX gain
     TCX-40 -> 6 bits (in 2nd packet)
     TCX-80 -> 9 bits (3 bits in 2nd, 3rd and 4th packet) */
  test();
  if (sub(L_frame,(L_FRAME_PLUS/2)) == 0) 
  {
    nb_bits = sub(nb_bits, 6);
  }
  test();
  if (sub(L_frame,L_FRAME_PLUS) == 0) 
  {
    nb_bits = sub(nb_bits, 9);
  }

  fac_ns_ = AVQ_Cod(xri, &prm[2], nb_bits, shr(lg,3));

  for(i=0; i<lg; i++) 
  {
    xri[i] = prm[i+2];      move16();
  }
  Scale_tcx_ifft(xri, lg ,&Q_tcx);
  Adap_low_freq_deemph(xri, lg);

  /* quantize noise factor (noise factor = 0.1 to 0.8) */
  
  Ltmp = L_msu(524288, 10, fac_ns_);
  index = round(Ltmp);
  test();
  if (index < 0) 
  {
    index = 0;      move16();
  }
  test();
  if (sub(index,7) > 0) 
  {
    index = 7;      move16();
  }

  prm[0] = index;      move16();  /* fac_ns : 3 bits */

  /*-----------------------------------------------------------*
  * Compute inverse FFT for obtaining xnq[] without noise.    *
  * Coefficients (xri[]) order are                            *
  *    re[0], re[n/2], re[1], re[2], ... re[n/2-1], im[n/2-1] *
  * Note that last FFT element (re[n/2]) is zeroed.           *
  *-----------------------------------------------------------*/
  xri[1] = 0;       move16();      /* freq bin at 6400 Hz zeroed */
  Ifft_reorder(xri, lg);
  ifft9_fx(xri, xnq, lg);

  /*-----------------------------------------------------------*
  * find and quantize gain, multiply xnq[] by gain.           *
  * windowing of xnq[] for TCX overlap.                       *
  *-----------------------------------------------------------*/
  Lgain = Get_gain(xn, xnq, lg, st->Q_sp, &mant_ener, &exp_ener/*&Lener_xnq*/);
  prm[1] = Q_gain_tcx(lg, &Lgain, mant_ener, exp_ener/*, Lener_xnq*/);    move16();
 

  e_tmp = norm_l(Lgain);
  tmp16 = extract_h(L_shl(Lgain,e_tmp));
  e_tmp = sub(e_tmp, add(st->Q_sp,15));  /* To scale xnq with Q_sp  */
  for (i=0; i<lg; i++) 
  {
    /* xnq[i] *= gain;*/
    Ltmp = L_mult(xnq[i], tmp16);
    xnq[i] = round(L_shr(Ltmp, e_tmp));  move16();  /* xnq scale with Q_sp  */
  }

  /* adaptive windowing on overlap (beginning and end of frame) */

  for (i=0; i<ovlp_size; i++) 
  {
    /*xnq[i] *= window[i];*/
    xnq[i] = mult_r(xnq[i], window[i]);      move16();
  }

  for (i=0; i<lext; i++) 
  {
    /*xnq[i+L_frame] *= window[ovlp_size+i];*/
    xnq[i+L_frame] = mult_r(xnq[i+L_frame], window[ovlp_size+i]);   move16();
  }

 /*-----------------------------------------------------------*
  * TCX overlap and add.  Update memory for next overlap.     *
  *-----------------------------------------------------------*/
  for (i=0; i<L_OVLP; i++) 
  {
    /*xnq[i] += wovlp[i];*/
    xnq[i] = add(xnq[i], wovlp[i]);    move16();   /* wolvp must be Q_SP scaled */
  }

  /* save overlap for next frame */
  for (i=0; i<lext; i++) 
  {
    /*wovlp[i] = xnq[i+L_frame];*/
    wovlp[i] = xnq[i+L_frame];      move16();
  }

  for (i=lext; i<L_OVLP; i++) 
  {
    /*wovlp[i] = 0.0;*/
    wovlp[i] = 0;      move16();
  }
 /*-----------------------------------------------------------*
  * find excitation and synthesis                             *
  *-----------------------------------------------------------*/
  Preemph(xnq, TILT_FAC_FX, L_frame, mem_wsyn);

  /* Ensure exc < 4096 */
  Copy(&synth[-M], temp, M);
  max = 0;    move16();
  for(i = 0;i < L_frame; i++)
  {
    test();
    if(sub(abs_s(xnq[i]),max) > 0)
    {
      max = abs_s(xnq[i]);
    }
  }
  max = sub(norm_s(max),2);
  test();
  if (max  > 0)
  {
    max = 0;      move16();
  }
  max1 = sub(max,1);


  Scale_sig(xnq, L_frame, max);
  Scale_sig(&synth[-M], M, max1);

  p_A = A;    move16();
  for (i_subfr=0; i_subfr<L_frame; i_subfr+=L_SUBFR) 
  {
    Weight_a(p_A, Ap, GAMMA1_FX, M);
    Syn_filt_s(1, Ap, M, &xnq[i_subfr], &synth[i_subfr], L_SUBFR, &synth[i_subfr-M], 0);
    Residu(p_A, M, &synth[i_subfr], &exc_[i_subfr], L_SUBFR);

    p_A += (M+1);
  }

  Scale_sig(synth, L_frame, negate(max1));
  Copy(temp, &synth[-M], M);

  return max1;

}

