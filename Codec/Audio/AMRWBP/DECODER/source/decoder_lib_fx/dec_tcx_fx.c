
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "oper_32b.h"

#include "amr_plus_fx.h"

//#include "stdlib.h"

/* local function */
static Word16 Find_mpitch(Word16 xri[], Word16 lg);
static void NoiseFill(Word16 *xri, Word16 *buf, Word16 *seed_tcx, Word16 fac_ns, Word16 Q, Word16 lg);

void Decoder_tcx(
  Word16 prm[],         /* (i)  : parameters                  */
  Word16 nbits_AVQ[],   /* (i)  : nbits in parameters of AVQ  */
  Word16 A[],           /* (i)  : coefficients NxAz[M+1]      */
  Word16 L_frame,       /* (i)  : frame length                */
  Word16 bad_frame[],   /* (i)  : Bad frame indicator         */
  Word16  exc[],        /* (o)  : exc[-lg..lg]                */
  Word16 synth[],       /* (i/o): synth[-M..lg]               */
  Decoder_State_Plus_fx *st /* (i/o): coder memory state      */
)                       
{

  Word16 i, k, i_subfr, index, lg, lext, bfi, n_pack = 0;
  Word16 tmp16, fac_ns;
  Word16 *p_A, Ap[M+1];
  Word16 *xri;  
  Word16 *xnq;
  Word16 window[2*L_OVLP];
  Word16 buf[L_TCX];  
  Word16 any_loss, num_loss, tmp_buf[M];
  Word32 gain32;
  Word16 inc2 = 0, Q_ifft;
  
#if (!FUNC_DECODER_TCX_OPT)
  xri = exc;  
  xnq = synth;  

  lext = L_OVLP;  
  
  if (sub(L_frame,(L_FRAME_PLUS/4)) == 0)
  {
    n_pack = 1; 
    lext = L_OVLP/4;          
    inc2 = 4;   
  }
  else if (sub(L_frame,(L_FRAME_PLUS/2)) == 0)
  {
    n_pack = 2; 
    lext = L_OVLP/2;          
    inc2 = 2;   
  }
  else if(L_frame == (L_FRAME_PLUS))   /*L_FRAME_PLUS*/
  {
    n_pack = 4;     
    inc2 = 1;        
  }

  lg = add(L_frame,lext);

  any_loss = 0;                   
  num_loss = 0;                   

  for (i=0; i<n_pack; i++) 
  {
    any_loss |= bad_frame[i];             logic16();
    num_loss = add(num_loss, bad_frame[i]);
  }
  Q_ifft = st->Old_Qxri;  

  /*------ initialize window of TCX target :
    two halves of a squared root of a hanning window
    the first part is as long as the overlap length of past frame (ovlp_size)
    the second part is as long as lext -----*/
  
  if ((sub(n_pack, 1) ==0) && (bad_frame[0] != 0))
  {
    /* repeat past excitation */
    k = st->pitch_tcx;            

    for (i=0; i<L_frame; i++) 
    {
      exc[i] = mult_r(exc[i-k], 22938);   
    }
    voAMRWBPDecCopy(synth-M, synth, M);  

    /* Synth in Q_syn*/
    /* exc in  Q_exc */
    p_A = A;
    for (i_subfr=0; i_subfr<L_frame; i_subfr+=L_SUBFR)
    {
      voAMRWBPDecSyn_filt_s(sub(st->Q_exc, st->Q_syn), p_A, M, &exc[i_subfr], &synth[i_subfr+M], L_SUBFR, &synth[i_subfr], 0);  /* synth Q0*/
      voAMRWBPDecWeight_a(p_A, Ap, GAMMA1_FX, M);
      voAMRWBPDecResidu(Ap, M, &synth[i_subfr+M], &xnq[i_subfr], L_SUBFR);
      p_A += (M+1);
    }

    /* xnq  in Q_syn +1 */
    voAMRWBPDecScale_sig(&(st->wmem_wsyn), 1, sub(add(st->Q_syn,1), st->Old_Qxnq));/* not need to be scale cause scaling is idem*/
    tmp16 = st->wmem_wsyn;    
    voAMRWBPDecDeemph(xnq, TILT_FAC_FX, L_frame, &tmp16);  

    st->wwsyn_rms = mult_r(st->wwsyn_rms, 22938); 
    tmp16 = st->wwsyn_rms;                
    voAMRWBPDecScale_sig(&tmp16, 1, add(st->Q_syn,1));             /* Scale wsyn_rms to fit xnq scaling */
    for (i=0; i<L_frame; i++) 
    {
      
      if (sub(xnq[i],tmp16) > 0)
      {
        xnq[i] = tmp16;      
      }
      else if (add(xnq[i],tmp16) < 0)
      {
        xnq[i] = negate(tmp16); 
      }
    }

    voAMRWBPDecPreemph(xnq, TILT_FAC_FX, L_frame, &(st->wmem_wsyn));

    /*xnq -> Q_syn +1*/
    /*synth -> Q_syn */
    p_A = A;
    for (i_subfr=0; i_subfr<L_frame; i_subfr+=L_SUBFR)
    {
      voAMRWBPDecWeight_a(p_A, Ap, GAMMA1_FX, M);
      voAMRWBPDecSyn_filt_s(1, Ap, M, &xnq[i_subfr], &synth[i_subfr], L_SUBFR, &synth[i_subfr-M], 0);
      p_A += (M+1);
    }

    /* zero windowed overlap (wovlp) for next frame */ 
    Set_zero(st->wwovlp, lext);
    
    /* Window scaling update needed */
    voAMRWBPDecScale_sig(st->wwovlp + lext, sub(L_OVLP,lext), sub(st->Q_exc,st->Old_Qxnq));
    st->Old_Qxnq = add(st->Q_syn,1);   
  }
  else
  {
    /*----- synthesize windowed TCX target (inverse FFT of decoded spectrum + noise fill-in)
          using available info                                                      ------*/

    /* decode noise level (fac_ns) (stored in 2nd packet on TCX80) */
    index = *prm++;  
      
    if (sub(n_pack,4) == 0)
    {
      bfi = bad_frame[1];     
    }
    else
    {
      bfi = bad_frame[0];     
    }
    
    if (bfi != 0)
    {
      index = 0;              
    }

    /*fac_ns = 0.1f*(8.0f - ((float)index));*/
    fac_ns = extract_l(L_shr(L_mult(3277, sub(8,index)),1)); /* fac_ns Q15*/
    /* read index of global TCX gain : erased bits are set to 1 to minimize global gain
       if bad_frame set erased bits to default value (0 or 1 depending on parameter) */
    index = *prm++;     
    /* decode parameters of multi-rate lattice VQ */
    AVQ_Demuxdec(n_pack, prm, nbits_AVQ, xri, (Word16)(lg/8), bad_frame);

    /*----------------------------------------------*
     * noise fill-in on unquantized subvector       *
     * injected only from 1066Hz to 6400Hz.         *
     *----------------------------------------------*/
    Q_ifft = 0;     
    /* Scale with coefficent energy and index information */
    Scale_tcx_ifft(xri, lg, &Q_ifft); /* Output max value scaled*/


    
    if(any_loss) 
    {

      Adapt_low_freq_deemph_ecu(xri,lg,Q_ifft,st);
      Reconst_spect(xri,st->wold_xri,n_pack,bad_frame,lg,st->last_mode,Q_ifft);

      /* Pat*/
      /* generate random excitation buffer */
      /*----------------------------------------------*
       * noise fill-in on unquantized subvector       *
       * injected only from 1066Hz to 6400Hz.         *
       *----------------------------------------------*/
      NoiseFill(xri, buf, &(st->seed_tcx), fac_ns, Q_ifft, lg);      
    }
    else 
    {
      
      /*----------------------------------------------*
       * noise fill-in on unquantized subvector       *
       * injected only from 1066Hz to 6400Hz.         *
       *----------------------------------------------*/
      NoiseFill(xri, buf, &(st->seed_tcx), fac_ns, Q_ifft, lg);      
      /* adaptive low frequency deemphasis */
      Adap_low_freq_deemph(xri, lg);
    }
    st->Old_Qxri = Q_ifft;  
    voAMRWBPDecCopy(xri, st->wold_xri, lg);
    /* find pitch for bfi case */
    st->pitch_tcx = Find_mpitch(xri, lg);

    /*-----------------------------------------------------------*
     * Compute inverse FFT for obtaining xnq[] without noise.    *
     * Coefficients (xri[]) order are                            *
     *    re[0], re[n/2], re[1], re[2], ... re[n/2-1], im[n/2-1] *
     * Note that last FFT element (re[n/2]) is zeroed.           *

     *-----------------------------------------------------------*/
    
    /* Reorder coefficient to use fixed-point ifft */  
    xri[1] = 0;      /* freq bin at 6400 Hz zeroed */
    Ifft_reorder(xri, lg);
    ifft9_fx(xri, xnq, lg);

    /*-----------------------------------------------------------*
     * decode TCX global gain, multiply xnq[] by gain.           *
     * windowing of xnq[] for TCX overlap.                       *
     *-----------------------------------------------------------*/
    Cos_window(window, st->ovlp_size, lext, inc2);
    bfi = 0;  
    gain32 = D_gain_tcx(index, xnq, lg, bfi, &(st->wwsyn_rms));      

    /* adaptive windowing on overlap (beginning and end of frame) */
    for (i=0; i<st->ovlp_size; i++)
    {
      xnq[i] = mult_r(xnq[i], window[i]);   
    }
    for (i=0; i<lext; i++)
    {
      xnq[i+L_frame] = mult_r(xnq[i+L_frame], window[st->ovlp_size+i]);   
    }
    voAMRWBPDecCopy(synth-M, tmp_buf, M);     /* tempo save*/ 
    Scale_mem_tcx(xnq, lg, gain32, synth-M, st);


    /*-----------------------------------------------------------*
     * TCX overlap and add.  Update memory for next overlap.     *
     *-----------------------------------------------------------*/
    for (i=0; i<L_OVLP; i++)
    {
      xnq[i] = add(xnq[i],st->wwovlp[i]);     
    }

    /* save overlap for next frame */
    for (i=0; i<lext; i++)
    {
      st->wwovlp[i] = xnq[i+L_frame];   
    }
    for (i=lext; i<L_OVLP; i++)
    {
      st->wwovlp[i] = 0;   
    }

    st->ovlp_size = lext;  
    /*-----------------------------------------------------------*
     * find excitation and synthesis                             *
     *-----------------------------------------------------------*/
    voAMRWBPDecPreemph(xnq, TILT_FAC_FX, L_frame, &(st->wmem_wsyn));
    /* Do filtering in 2 times to avoid modifiy synth mem*/
    p_A = A;   
        
    for (i_subfr=0; i_subfr<L_frame; i_subfr+=L_SUBFR)
    {
      voAMRWBPDecWeight_a(p_A, Ap, GAMMA1_FX, M);

      voAMRWBPDecSyn_filt_s(sub(st->Q_exc,st->Q_syn), Ap, M, &xnq[i_subfr], &synth[i_subfr], L_SUBFR, &synth[i_subfr-M], 0);
      voAMRWBPDecResidu2(p_A, M, &synth[i_subfr], &exc[i_subfr], L_SUBFR);
      p_A += (M+1);
    }

    voAMRWBPDecCopy(tmp_buf,synth-M, M);      /*restore*/
    st->Q_exc = st->Q_syn;          
  }

#else

  xri = exc;
  xnq = synth;

  lext = L_OVLP;
  if (L_frame == (L_FRAME_PLUS >> 2))
  {
    n_pack = 1;
    lext = L_OVLP >> 2;
    inc2 = 4;
  }
  else if (L_frame == (L_FRAME_PLUS >> 1))
  {
    n_pack = 2;
    lext = L_OVLP >> 1;
    inc2 = 2;
  }
  else if (L_frame == L_FRAME_PLUS)   /*L_FRAME_PLUS*/
  {
    n_pack = 4;
    inc2 = 1;
  }

  //lg = add(L_frame,lext);
  lg = L_frame + lext;

  any_loss = 0;
  num_loss = 0;

  for (i=0; i<n_pack; i++) 
  {
    any_loss |= bad_frame[i];
    num_loss += bad_frame[i];
  }
  Q_ifft = st->Old_Qxri;

  /*------ initialize window of TCX target :
    two halves of a squared root of a hanning window
    the first part is as long as the overlap length of past frame (ovlp_size)
    the second part is as long as lext -----*/
  if ((n_pack == 1) && (bad_frame[0] != 0))
  {
    /* repeat past excitation */
    k = st->pitch_tcx;

    for (i=0; i<L_frame; i++) 
    {
      //exc[i] = mult_r(exc[i-k], 22938);
      exc[i] = ((exc[i-k] * 22938) + 0x00004000L) >> 15;
    }
    voAMRWBPDecCopy(synth-M, synth, M);
    //memcpy(synth, synth-M, M * sizeof(Word16));

    /* Synth in Q_syn*/
    /* exc in  Q_exc */
    p_A = A;
    for (i_subfr=0; i_subfr<L_frame; i_subfr+=L_SUBFR)
    {
      voAMRWBPDecSyn_filt_s(sub(st->Q_exc, st->Q_syn), p_A, M, &exc[i_subfr], &synth[i_subfr+M], L_SUBFR, &synth[i_subfr], 0);  /* synth Q0*/
      voAMRWBPDecWeight_a(p_A, Ap, GAMMA1_FX, M);
      voAMRWBPDecResidu(Ap, M, &synth[i_subfr+M], &xnq[i_subfr], L_SUBFR);
      p_A += (M+1);
    }

    /* xnq  in Q_syn +1 */
    voAMRWBPDecScale_sig(&(st->wmem_wsyn), 1, sub(add(st->Q_syn,1), st->Old_Qxnq));/* not need to be scale cause scaling is idem*/
    tmp16 = st->wmem_wsyn;
    voAMRWBPDecDeemph(xnq, TILT_FAC_FX, L_frame, &tmp16);  

    //st->wwsyn_rms = mult_r(st->wwsyn_rms, 22938);
    st->wwsyn_rms = ((st->wwsyn_rms * 22938) + 0x4000) >> 15;
    tmp16 = st->wwsyn_rms;
    voAMRWBPDecScale_sig(&tmp16, 1, add(st->Q_syn,1));             /* Scale wsyn_rms to fit xnq scaling */
    for (i=0; i<L_frame; i++) 
    {
      if (xnq[i] > tmp16)
      {
        xnq[i] = tmp16;
      }
      else if ((xnq[i] + tmp16) < 0)
      {
        //xnq[i] = negate(tmp16); 
        xnq[i] = -tmp16;
      }
    }

    voAMRWBPDecPreemph(xnq, TILT_FAC_FX, L_frame, &(st->wmem_wsyn));


    /*xnq -> Q_syn +1*/
    /*synth -> Q_syn */
    p_A = A;
    for (i_subfr=0; i_subfr<L_frame; i_subfr+=L_SUBFR)
    {
      voAMRWBPDecWeight_a(p_A, Ap, GAMMA1_FX, M);
      voAMRWBPDecSyn_filt_s(1, Ap, M, &xnq[i_subfr], &synth[i_subfr], L_SUBFR, &synth[i_subfr-M], 0);
      p_A += (M+1);
    }

    /* zero windowed overlap (wovlp) for next frame */ 
    Set_zero(st->wwovlp, lext);
    //memset(st->wwovlp, 0, lext * sizeof(Word16));
    
    /* Window scaling update needed */
    voAMRWBPDecScale_sig(st->wwovlp + lext, sub(L_OVLP,lext), sub(st->Q_exc,st->Old_Qxnq));
    //st->Old_Qxnq = add(st->Q_syn,1);   
    st->Old_Qxnq = st->Q_syn + 1;
  }
  else
  {
    /*----- synthesize windowed TCX target (inverse FFT of decoded spectrum + noise fill-in)
          using available info                                                      ------*/

    /* decode noise level (fac_ns) (stored in 2nd packet on TCX80) */
    index = *prm++;
      
    if (n_pack == 4)
    {
      bfi = bad_frame[1];
    }
    else
    {
      bfi = bad_frame[0];
    }
    if (bfi != 0)
    {
      index = 0;
    }

    /*fac_ns = 0.1f*(8.0f - ((float)index));*/
    //fac_ns = extract_l(L_shr(L_mult(3277, sub(8,index)),1)); /* fac_ns Q15*/
    fac_ns = extract_l(3277 * (8-index)); /* fac_ns Q15*/
    /* read index of global TCX gain : erased bits are set to 1 to minimize global gain
       if bad_frame set erased bits to default value (0 or 1 depending on parameter) */
    index = *prm++;
    /* decode parameters of multi-rate lattice VQ */
    //AVQ_Demuxdec(n_pack, prm, nbits_AVQ, xri, (Word16)(lg/8), bad_frame);
    AVQ_Demuxdec(n_pack, prm, nbits_AVQ, xri, (Word16)(lg>>3), bad_frame);

    /*----------------------------------------------*
     * noise fill-in on unquantized subvector       *
     * injected only from 1066Hz to 6400Hz.         *
     *----------------------------------------------*/
    Q_ifft = 0;
    /* Scale with coefficent energy and index information */
    Scale_tcx_ifft(xri, lg, &Q_ifft); /* Output max value scaled*/


    if(any_loss) 
    {

      Adapt_low_freq_deemph_ecu(xri,lg,Q_ifft,st);
      Reconst_spect(xri,st->wold_xri,n_pack,bad_frame,lg,st->last_mode,Q_ifft);

      /* Pat*/
      /* generate random excitation buffer */
      /*----------------------------------------------*
       * noise fill-in on unquantized subvector       *
       * injected only from 1066Hz to 6400Hz.         *
       *----------------------------------------------*/
      NoiseFill(xri, buf, &(st->seed_tcx), fac_ns, Q_ifft, lg);      
    }
    else 
    {
      
      /*----------------------------------------------*
       * noise fill-in on unquantized subvector       *
       * injected only from 1066Hz to 6400Hz.         *
       *----------------------------------------------*/
      NoiseFill(xri, buf, &(st->seed_tcx), fac_ns, Q_ifft, lg);      
      /* adaptive low frequency deemphasis */
      Adap_low_freq_deemph(xri, lg);
    }
    st->Old_Qxri = Q_ifft;
    voAMRWBPDecCopy(xri, st->wold_xri, lg);
    //memcpy(st->wold_xri, xri, lg * sizeof(Word16));
    /* find pitch for bfi case */
    st->pitch_tcx = Find_mpitch(xri, lg);

    /*-----------------------------------------------------------*
     * Compute inverse FFT for obtaining xnq[] without noise.    *
     * Coefficients (xri[]) order are                            *
     *    re[0], re[n/2], re[1], re[2], ... re[n/2-1], im[n/2-1] *
     * Note that last FFT element (re[n/2]) is zeroed.           *

     *-----------------------------------------------------------*/
    
    /* Reorder coefficient to use fixed-point ifft */  
    xri[1] = 0; /* freq bin at 6400 Hz zeroed */
    Ifft_reorder(xri, lg);
    ifft9_fx(xri, xnq, lg);

    /*-----------------------------------------------------------*
     * decode TCX global gain, multiply xnq[] by gain.           *
     * windowing of xnq[] for TCX overlap.                       *
     *-----------------------------------------------------------*/
    Cos_window(window, st->ovlp_size, lext, inc2);
    bfi = 0;
    gain32 = D_gain_tcx(index, xnq, lg, bfi, &(st->wwsyn_rms));      

    /* adaptive windowing on overlap (beginning and end of frame) */
    for (i=0; i<st->ovlp_size; i++)
    {
      //xnq[i] = mult_r(xnq[i], window[i]);
      xnq[i] = ((xnq[i] * window[i]) + 0x4000) >> 15;
    }
    for (i=0; i<lext; i++)
    {
      //xnq[i+L_frame] = mult_r(xnq[i+L_frame], window[st->ovlp_size+i]);   
      xnq[i+L_frame] = ((xnq[i+L_frame] * window[st->ovlp_size+i]) + 0x4000) >> 15;
    }
    voAMRWBPDecCopy(synth-M, tmp_buf, M);     /* tempo save*/ 
    //memcpy(tmp_buf, synth-M, M * sizeof(Word16));     /* tempo save*/ 
    Scale_mem_tcx(xnq, lg, gain32, synth-M, st);


    /*-----------------------------------------------------------*
     * TCX overlap and add.  Update memory for next overlap.     *
     *-----------------------------------------------------------*/
    for (i=0; i<L_OVLP; i++)
    {
      //xnq[i] = add(xnq[i],st->wwovlp[i]);
      xnq[i] += st->wwovlp[i];
    }

    /* save overlap for next frame */
    for (i=0; i<lext; i++)
    {
      st->wwovlp[i] = xnq[i+L_frame];
    }
    for (i=lext; i<L_OVLP; i++)
    {
      st->wwovlp[i] = 0;
    }

    st->ovlp_size = lext;
    /*-----------------------------------------------------------*
     * find excitation and synthesis                             *
     *-----------------------------------------------------------*/
    voAMRWBPDecPreemph(xnq, TILT_FAC_FX, L_frame, &(st->wmem_wsyn));
    /* Do filtering in 2 times to avoid modifiy synth mem*/
    p_A = A;
        
    for (i_subfr=0; i_subfr<L_frame; i_subfr+=L_SUBFR)
    {
      voAMRWBPDecWeight_a(p_A, Ap, GAMMA1_FX, M);
      voAMRWBPDecSyn_filt_s(sub(st->Q_exc,st->Q_syn), Ap, M, &xnq[i_subfr], &synth[i_subfr], L_SUBFR, &synth[i_subfr-M], 0);
      voAMRWBPDecResidu2(p_A, M, &synth[i_subfr], &exc[i_subfr], L_SUBFR);
      p_A += (M+1);
    }

    voAMRWBPDecCopy(tmp_buf,synth-M, M);      /*restore*/
    //memcpy(synth-M, tmp_buf, M * sizeof(Word16));      /*restore*/
    st->Q_exc = st->Q_syn;
  }
#endif

  return;
}

static Word16 Find_mpitch(
  Word16 xri[],             /* (i): Fft coeff         */
  Word16 lg                 /* (i): Number of coeff   */  
)
{
  Word32 Ltmp, Lmax;
  Word16 pitch, mpitch, exp_tmp, lg4;
  Word16 i, n, tmp16;

  
  Lmax = 0;   move32();
  n = 2;      
  /* find maximum below 400Hz */
  lg4 = shr(lg,4);
  for (i=2; i<lg4; i+=2) 
  {
    Ltmp = L_mult(xri[i], xri[i]);
    Ltmp = L_mac(Ltmp, xri[i+1], xri[i+1]);
    
    if (L_sub(Ltmp, Lmax) > 0)
    {
      Lmax = Ltmp; move32();
      n = i;       
    }
  }
  /* tmp = n/lg */
  /* pitch = 12800.0f / (tmp*6400.0f);*/
  /* pitch = (2*lg)/n */
  
  exp_tmp  = norm_s(n);
  tmp16 = shl(n, exp_tmp);
  exp_tmp = sub(15, exp_tmp);
  tmp16 = div_s(16384, tmp16);    /* 1/n */
  exp_tmp = sub(1+1, exp_tmp);
  pitch = round16(L_mult(lg, shl(tmp16, exp_tmp)));  /*Q0*/


  /* find pitch multiple under 20ms */
  
  if (sub(pitch,256) >= 0)
  {
    n = 256;      
  }
  else
  {
    mpitch = pitch;      /*Q6*/
    
    while (sub(mpitch,256) < 0)
    {
      mpitch = add(pitch,mpitch);
      
    }
    n = sub(mpitch,pitch);     /* return n in Q0 */  
  }
  return(n);
}
static void NoiseFill(
  Word16 *xri,          /* (i/o) : Fft coeff          */
  Word16 *buf,          /* (i/o) : Tmp buffer         */
  Word16 *seed_tcx,     /* (i/o) : Seed generator     */
  Word16 fac_ns,        /* (i)   ; Noise gain      Q15*/
  Word16 Q_ifft,        /* (i)   : Fft coef scaling   */
  Word16 lg             /* (i)   : Number of coeff    */
)
{
  Word16 lg6, i, k;
  Word32 Ltmp;
  
  lg6 = mult_r(lg, 5461);

  Set_zero(buf, lg);
  Rnd_ph16(seed_tcx, &buf[lg6], sub(lg, lg6), Q_ifft);

  for(k=lg6; k<lg; k+=8)
  {
    Ltmp = 0;   move32();
    for(i=k; i<k+8; i++) Ltmp = L_mac(Ltmp, xri[i], xri[i]);
      
    if (Ltmp == 0)
    { 
      for(i=k; i<k+8; i++)
      {
        xri[i] = mult_r(fac_ns, buf[i]);    
      }
    }
  }
}
