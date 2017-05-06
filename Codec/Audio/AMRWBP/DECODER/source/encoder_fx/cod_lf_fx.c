#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "count.h"


#include "amr_plus_fx.h"

extern const short NBITS_CORE_FX[];

/*-----------------------------------------------------------------*
 *   Funtion  coder_wb                                             *
 *            ~~~~~~~~                                             *
 *   ->Main coder routine.                                         *
 *                                                                 *
 *-----------------------------------------------------------------*/
	
void Coder_lf(  
  Word16 codec_mode,  /* (i) : AMR-WB+ mode (see cnst.h)             */ 
  Word16 speech[],    /* (i) : speech vector [-M..L_FRAME_PLUS+L_NEXT]    */ 
  Word16 synth[],     /* (o) : synthesis vector [-M..L_FRAME_PLUS]        */ 
  Word16 mod[],       /* (o) : mode for each 20ms frame (mode[4]     */
  Word16 AqLF[],      /* (o) : quantized coefficients (AdLF[16])     */ 
  Word16 window[],    /* (i) : window for LPC analysis               */
  Word16 param[],     /* (o) : parameters (NB_DIV*NPRM_DIV)          */
  Word16 ol_gain[],   /* (o) : open-loop LTP gain                    */  
  Word16 ave_T_out[], /* (o) : average LTP lag                       */ 
  Word16 ave_p_out[], /* (o) : average LTP gain                      */ 
  Word16 coding_mod[],/* (i) : selected mode for each 20ms           */
  Word16 pit_adj,     /* (i) : indicate pitch adjustment             */
  Coder_State_Plus_fx *st/* i/o : coder memory state               */
  ) 
{ 
  Word16 r_h[M+1],r_l[M+1];                   /* Autocorrelations of windowed speech  */
  Word16 A[(NB_SUBFR+1)*(M+1)];
  Word16 Aq[(NB_SUBFR+1)*(M+1)];
  Word16 rc[M];  
  Word16 ispnew[M];                /* LSPs at 4nd subframe                 */
  Word16 ispnew_q[M];              /* LSPs at 4nd subframe                 */
  Word16 isp[(NB_DIV+1)*M];
  Word16 isp_q[(NB_DIV+1)*M];
  Word16 isfnew[M];
  Word16 past_isfq_1[M];                  /* past isf quantizer */
  Word16 past_isfq_2[M];                  /* past isf quantizer */
  Word16 mem_w0[NB_DIV+1], mem_wsyn[NB_DIV+1];
  Word16 mem_xnq[NB_DIV+1];
  Word16 ovlp_size[NB_DIV+1];
  
  Word16 Ap[M+1];
  Word16 prm_tcx[NPRM_LPC+NPRM_TCX80];
  
  Word16 synth_tcx[M+L_TCX];
  Word16 exc_tcx[L_TCX], mem_w0_tcx, mem_xnq_tcx, mem_wsyn_tcx;
  Word16 wsp[L_FRAME_PLUS];
  Word16 wovlp[(NB_DIV+1)*128];
  Word16 wovlp_tcx[128];
  
  Word16 old_d_wsp[(PIT_MAX_MAX/OPL_DECIM)+L_DIV];          /* Weighted speech vector */
  Word16 old_exc[PIT_MAX_MAX+L_INTERPOL+L_FRAME_PLUS+1];    /* Excitation vector */
  Word16 *d_wsp, *exc;
		
  /* Scalars */
  Word16 i, j, k, i2, i1, nbits, *prm;
  Word16 tmp_max, i_fr, exp_wsp, exp_t0, exp_cm, exp_ener;
  Word16 snr, snr1, snr2;
  Word16 tmp;
		
  Word16 *p, *p1,exp_dwn;
  Word16 norm_corr[4], norm_corr2[4];
  Word16 T_op[NB_DIV], T_op2[NB_DIV];
  Word16 T_out[4];           /* integer pitch-lag */
  Word16 p_out[4];

  Word16 PIT_min;            /* Minimum pitch lag with resolution 1/4      */
  Word16 PIT_max;            /* Maximum pitch lag                          */
  
  Word32 Lener, Lcor_max, Lt0;
  Word32 Ltmp;
  Word16 tmp16;
  Word16 shift;
  Word16 max;

  test();
  if(pit_adj ==0)
  {
    PIT_min = PIT_MIN_12k8;   move16();
    PIT_max = PIT_MAX_12k8;   move16();
  }
  else 
  {
    /*i = (((pit_adj*PIT_MIN_12k8)+(FSCALE_DENOM/2))/FSCALE_DENOM)-PIT_MIN_12k8;	*/
    Ltmp = L_mac(FSCALE_DENOM, pit_adj, PIT_MIN_12k8);
  
    k = FSCALE_DENOM_NORMS;     move16();
    i = div_s(extract_h(L_shl(Ltmp, k)), shl(FSCALE_DENOM, k));
  
    tmp16 = add(i, 1);    
    Ltmp = L_msu(Ltmp, tmp16, FSCALE_DENOM);
  
    i = sub(i, PIT_MIN_12k8);
    test();
    if (Ltmp >= 0)
    {
      i = sub(tmp16,PIT_MIN_12k8);     
    } 

    st->i_offset = i; move16();     /* prevent recomputation */

    PIT_min = add(PIT_MIN_12k8 , i);           move16();
    PIT_max = add(PIT_MAX_12k8, extract_l(L_shr(L_mult(i, 6),1)));       move16();
    st->pit_max = PIT_max;      move16();
  }

  /* Initialize pointers */

  d_wsp = old_d_wsp + PIT_MAX_MAX/OPL_DECIM;            move16();
  exc   = old_exc   + PIT_MAX_MAX + L_INTERPOL;        move16();

  /* copy coder memory state into working space (dynamic memory) */

  Copy(st->old_d_wsp, old_d_wsp, PIT_MAX_MAX/OPL_DECIM);
  Copy(st->old_exc, old_exc, PIT_MAX_MAX+L_INTERPOL);

  /* number of bits per frame (80 ms) */
  nbits = NBITS_CORE_FX[codec_mode];        move16();

  /* remove bits for mode */
  nbits = sub(nbits, NBITS_MODE);

  /*---------------------------------------------------------------*
   *  Perform LP analysis four times (every 20 ms)                 *
   *  - autocorrelation + lag windowing                            *
   *  - Levinson-Durbin algorithm to find a[]                      *
   *  - convert a[] to isp[]                                       *
   *  - interpol isp[]                                             *
   *---------------------------------------------------------------*/
  /* read old isp for LPC interpolation */
  Copy(st->ispold, isp, M);
  for (i = 0; i < NB_DIV; i++)
  {
    test();
    if (sub(pit_adj,FSCALE_DENOM) <=0) 
    {
      /* Autocorrelations of signal at 12.8kHz */
      Autocorr(&speech[(i*L_DIV)+L_SUBFR],M, r_h, r_l, L_WINDOW, window);	  
    } 
    else 
    {
      /* Autocorrelations of signal at 12.8kHz */
      Autocorr(&speech[(i*L_DIV)+(L_SUBFR/2)],M, r_h, r_l, L_WINDOW_HIGH_RATE, window);	  
    }
    Lag_window(r_h,r_l, M);                            /* Lag windowing    */		
    Levinson(r_h, r_l, Ap, rc, st->lev_mem, M);                         /* Levinson Durbin  */	
    Az_isp(Ap, ispnew, st->ispold, M);          /* From A(z) to ISP */	
    
    Copy(ispnew, &isp[(i+1)*M], M);

    /* A(z) interpolation every 20 ms (used for weighted speech) */
    Int_lpc(st->ispold, ispnew, interpol_frac4, &A[i*4*(M+1)], 4, M);
    
    /* update ispold[] for the next LPC analysis */
    Copy(ispnew, st->ispold, M);
  }

  /*---------------------------------------------------------------*
   * Calculate open-loop LTP parameters                            *
   *---------------------------------------------------------------*/
  for (i = 0; i < NB_DIV; i++) 
  {
    /* weighted speech for SNR */
    Find_wsp(&A[i*(NB_SUBFR/4)*(M+1)], &speech[i*L_DIV], &wsp[i*L_DIV], &(st->mem_wsp), L_DIV); 	  
    mem_w0[i+1] = st->mem_wsp;     move16();     /* prevent to recompute wspeech in acelp */  
    
    Copy(&wsp[i*L_DIV], d_wsp, L_DIV);         
    
    /* find maximum value on wsp[] for 12 bits scaling */
    max = 0;                               move16();
    for (i_fr = 0; i_fr < L_DIV; i_fr++)
    {
        tmp16 = abs_s(d_wsp[i_fr]);
        test();
        if (sub(tmp16, max) > 0)
        {
            max = tmp16;                     move16();
        }
    }
    tmp_max = max;                                move16();
    for(i_fr = 0;i_fr < 3; i_fr++)
    {
      tmp16 = st->old_wsp_max[i_fr];                 move16();
      test();
      if (sub(tmp_max , tmp16) > 0)
      {
          tmp16 = tmp_max ;                         /* tmp16 = max(wsp_max, old_wsp_max) */
          move16();
      }
      else
      {
          tmp_max  = tmp16;     move16();
      }
      st->old_wsp_max[i_fr] = st->old_wsp_max[i_fr+1] ;                 move16();
    }
    tmp16 = st->old_wsp_max[3];                 move16();
    test();
    if (sub(tmp_max , tmp16) > 0)
    {
        tmp16 = tmp_max ;                         /* tmp16 = max(wsp_max, old_wsp_max) */
        move16();
    }

    st->old_wsp_max[3] = max;                 move16();
    shift = sub(norm_s(tmp16), 3);
    test();
    if (shift > 0)
    {
        shift = 0;                         /* shift = 0..-3 */
        move16();
    }
    
    LP_Decim2(d_wsp, L_DIV, st->mem_lp_decim2); 
    
   /* scale wsp[] in 12 bits to avoid overflow */
    Scale_sig(d_wsp, L_DIV / OPL_DECIM, shift);
   /* scale old_wsp (warning: exp must be Q_new-Q_old) */
    test();
    if (i == 0)
    {
      exp_wsp = add(st->scale_fac, sub(shift, st->old_wsp_shift));
    }
    else
    {
      exp_wsp = sub(shift, st->old_wsp_shift);
    }
    st->old_wsp_shift = shift;        move16();
    test();
    if(exp_wsp != 0)
    {
      Scale_sig(old_d_wsp, PIT_MAX_MAX / OPL_DECIM, exp_wsp);
      Scale_sig(st->hp_old_wsp, PIT_MAX_MAX / OPL_DECIM, exp_wsp);
      scale_mem_Hp_wsp(st->hp_ol_ltp_mem, exp_wsp);
    }
    /* Find open loop pitch lag for first 1/2 frame */
    T_op[i] = Pitch_med_ol(d_wsp, add(shr(PIT_min,1),1), shr(PIT_max,1),
                  (2*L_SUBFR)/OPL_DECIM, st->old_T0_med, &(st->ol_gain), 
                      st->hp_ol_ltp_mem, st->hp_old_wsp, st->ol_wght_flg);        move16();
    
    test();
    if (sub(st->ol_gain, 19661) > 0)       /*0.6 in Q15 */
    {
      st->old_T0_med = Med_olag(T_op[i], st->old_ol_lag);        move16();
      st->ada_w = 32767;      move16();
    }     
    else 
    {
      st->ada_w = mult(st->ada_w, 29491);       move16();
    }

    test();
    if (sub(st->ada_w, 26214) < 0)     /* 0.8 in Q15 */
    {
      st->ol_wght_flg = 0;       move16();
    }
    else 
    {
      st->ol_wght_flg = 1;       move16();
    }

    /* compute max */
    Lcor_max =0;      move32();
    
    /* compute energy */
    Lt0 = 0;              move32();

    /* normalized corr (0..1) */
    Lener = 0;        move32();
    
    p = &d_wsp[0];         move16();
    p1 = d_wsp - T_op[i];        move16();

    for(j=0; j<(2*L_SUBFR)/OPL_DECIM; j++, p++, p1++) 
    {
      Lcor_max  = L_mac(Lcor_max, *p, *p1);
      Lt0 = L_mac(Lt0, *p1, *p1);
      Lener = L_mac(Lener, *p, *p);
    }

    test();
    if (Lt0 == 0)
    {
       Lt0 = 1;  move32();
    }
    test();
    if (Lener == 0)
    {
       Lener = 1;  move32();
    }

    exp_cm = norm_l(Lcor_max);
    Lcor_max = L_shl(Lcor_max, exp_cm);

    exp_t0 = norm_l(Lt0);
    Lt0 = L_shl(Lt0, exp_t0);

    exp_ener = norm_l(Lener);
    Lener = L_shl(Lener, exp_ener);

    Lt0 = L_mult(round(Lt0), round(Lener));

    j = norm_l(Lt0);
    Lt0 = L_shl(Lt0, j);

    exp_t0 = add(exp_t0, exp_ener);
    exp_t0 = add(exp_t0, j);
    exp_t0 = sub(62, exp_t0);

    Isqrt_n(&Lt0, &exp_t0);

    Lcor_max = L_mult(round(Lcor_max), round(Lt0));
    exp_cm = sub(31, exp_cm);
    exp_cm = add(exp_cm, exp_t0);

    norm_corr[i]  = round(L_shl(Lcor_max, exp_cm));
    move16();

    /* Find open loop pitch lag for first 1/2 frame */
    T_op2[i] = Pitch_med_ol(d_wsp + ((2*L_SUBFR)/OPL_DECIM), add(shr(PIT_min,1),1), shr(PIT_max,1),
                  (2*L_SUBFR)/OPL_DECIM, st->old_T0_med, &(st->ol_gain), 
                      st->hp_ol_ltp_mem, st->hp_old_wsp, st->ol_wght_flg);        move16();

    test();
    if (sub(st->ol_gain, 19661) > 0)       /*0.6 in Q15 */
    {
      st->old_T0_med = Med_olag((Word16)T_op2[i], st->old_ol_lag);        move16();
      st->ada_w = 32767;      move16();
    }     
    else 
    {
      st->ada_w = mult(st->ada_w, 29491);       move16();
    }

    test();
    if (sub(st->ada_w, 26214) < 0)     /* 0.8 in Q15 */
    {
      st->ol_wght_flg = 0;       move16();
    }
    else 
    {
      st->ol_wght_flg = 1;       move16();
    }
    /* compute max */
    Lcor_max =0;      move32();
    
    /* compute energy */
    Lt0 = 0;              move32();

    /* normalized corr (0..1) */
    Lener = 0;        move32();
    
    p = d_wsp + (2*L_SUBFR)/OPL_DECIM;         move16();
    p1 = d_wsp + ((2*L_SUBFR)/OPL_DECIM) - T_op2[i];        move16();

    for(j=0; j<(2*L_SUBFR)/OPL_DECIM; j++, p++, p1++) 
    {
      Lcor_max  = L_mac(Lcor_max, *p, *p1);
      Lt0 = L_mac(Lt0, *p1, *p1);
      Lener = L_mac(Lener, *p, *p);
    }

    test();
    if (Lt0 == 0)
    {
       Lt0 = 1;  move32();
    }
    test();
    if (Lener == 0)
    {
       Lener = 1;  move32();
    }
    
    exp_cm = norm_l(Lcor_max);
    Lcor_max = L_shl(Lcor_max, exp_cm);

    exp_t0 = norm_l(Lt0);
    Lt0 = L_shl(Lt0, exp_t0);

    exp_ener = norm_l(Lener);
    Lener = L_shl(Lener, exp_ener);


    Lt0 = L_mult(round(Lt0), round(Lener));

    j = norm_l(Lt0);
    Lt0 = L_shl(Lt0, j);

    exp_t0 = add(exp_t0, exp_ener);
    exp_t0 = add(exp_t0, j);
    exp_t0 = sub(62, exp_t0);

    Isqrt_n(&Lt0, &exp_t0);

    Lcor_max = L_mult(round(Lcor_max), round(Lt0));
    exp_cm = sub(31, exp_cm);
    exp_cm = add(exp_cm, exp_t0);

    norm_corr2[i]  = round(L_shl(Lcor_max, exp_cm));
    move16();

    ol_gain[i] = st->ol_gain;         move16();

    Copy(&old_d_wsp[L_DIV/OPL_DECIM], old_d_wsp, PIT_MAX_MAX/OPL_DECIM);

  }

  Copy(old_d_wsp, st->old_d_wsp, PIT_MAX_MAX/OPL_DECIM);   /* d_wsp already shifted */


  /*---------------------------------------------------------------*
   *  Call ACELP and TCX codec                                     *
   *---------------------------------------------------------------*/
  ovlp_size[0] = st->old_ovlp_size;     move16();
  mem_w0[0]   = st->old_mem_w0;     move16();
  mem_xnq[0]   = st->old_mem_xnq;     move16();
  mem_wsyn[0] = st->old_mem_wsyn;     move16();

  Copy(st->old_wovlp, wovlp, 128);
  Copy(st->past_isfq, past_isfq_1, M);
  Copy(st->ispold_q, isp_q, M);

  snr2 = 0;   move16();

  for (i1=0; i1<2; i1++) 
  {
    Copy(past_isfq_1, past_isfq_2, M);
    snr1 = 0;        move16();

    for (i2=0; i2<2; i2++) 
    {
      k = add(shl(i1,1),i2);

      /* set pointer to parameters */
      prm = param + (k*NPRM_DIV);        move16();
      
      /*---------------------------------------------------------------*
       *  Quantize ISF parameters (46 bits) every 20 ms                *
       *---------------------------------------------------------------*/
      
      /* Convert isps to frequency domain 0..6400 */
      Isp_isf(&isp[(k+1)*M], isfnew, M);
      
      /* quantize 1st and 2nd LPC with 46 bits */
      Qpisf_2s_46b(isfnew, isfnew, past_isfq_1, prm, 4);
      
      prm += NPRM_LPC;
      
      /* Convert isfs to the cosine domain */
      Isf_isp(isfnew, &isp_q[(k+1)*M], M);
      
      /* interpol quantized lpc */
      Int_lpc(&isp_q[k*M], &isp_q[(k+1)*M], interpol_frac4, Aq, 4, M);
      
      /* lpc coefficient needed for HF extension */
      Copy(Aq, &AqLF[k*4*(M+1)], 5*(M+1));
      
      /*---------------------------------------------------------------*
       *  Call ACELP 4 subfr x 5ms = 20 ms frame                       *
       *---------------------------------------------------------------*/
      
      /* mem_w0[k+1] = mem_w0[k]  already computed in FIND WSP */

      mem_xnq[k+1] = mem_xnq[k];     move16();
      ovlp_size[k+1] = 0;            move16();
      
      Coder_acelp(&A[k*(NB_SUBFR/4)*(M+1)],
                  Aq,
                  &speech[k*L_DIV],
                 /* &mem_w0[k+1],*/
                  &mem_xnq[k+1],
                  &synth[k*L_DIV],
                  &exc[k*L_DIV],
                  &wovlp[(k+1)*128],
                  L_DIV,
                  codec_mode,
                  norm_corr[k],
                  norm_corr2[k],
                  T_op[k],
                  T_op2[k],
                  T_out,
                  p_out,
                  st->mem_gain_code,
                  pit_adj,
                  prm,
                  &wsp[k*L_DIV],
                  st);
      
      
      /* average integer pitch-lag for high band coding */
      ave_T_out[k] = add(T_op[k],T_op2[k]);     move16();
      /*ave_p_out[k] = (p_out[0]+p_out[1]+p_out[2]+p_out[3])/4.0f;*/
      Ltmp = L_mult(p_out[0],8192);
      Ltmp = L_mac(Ltmp, p_out[1],8192);
      Ltmp = L_mac(Ltmp, p_out[2],8192);
      ave_p_out[k] = round(L_mac(Ltmp, p_out[3],8192));   move16();
      mem_wsyn[k+1] = mem_wsyn[k];                        move16();
      {
          Word16 buf[L_FRAME_PLUS];

          Find_wsp(&A[k*(NB_SUBFR/4)*(M+1)], &synth[k*L_DIV], buf, &mem_wsyn[k+1], L_DIV);
          snr = Segsnr(&wsp[k*L_DIV], buf, L_DIV, L_SUBFR);

          st->LastQMode = 0; move16(); /* Acelp mode */
          test();
          if (st->SwitchFlagPlusToWB>0) 
          {
              snr = 32767;                      move16();
              st->SwitchFlagPlusToWB = 0;       move16();
          }

      }

      mod[k] = 0;                               move16();
      coding_mod[k] = 0;                        move16();

      /*--------------------------------------------------*
       * Call short TCX coder and find segmental SNR       *
       *--------------------------------------------------*/
      Copy(&synth[(k*L_DIV)-M], synth_tcx, M);
      mem_w0_tcx = mem_w0[k];         move16();
      mem_xnq_tcx = mem_xnq[k];       move16();
      
      Copy(&wovlp[k*128], wovlp_tcx, 128);
      
      exp_dwn = Coder_tcx(Aq,
                      &speech[k*L_DIV],
                      &mem_w0_tcx, 
                      &mem_xnq_tcx,
                      &synth_tcx[M],
                      exc_tcx,
                      wovlp_tcx,
                      ovlp_size[k],
                      L_DIV,
                      sub(shr(nbits,2),NBITS_LPC),
                      prm_tcx,
                      st);
      
      mem_wsyn_tcx = mem_wsyn[k];     move16();
      
      {
          Word16 buf[L_FRAME_PLUS];

          Find_wsp(&A[k*(NB_SUBFR/4)*(M+1)], &synth_tcx[M], buf, &mem_wsyn_tcx, L_DIV);
          tmp = Segsnr(&wsp[k*L_DIV], buf, L_DIV, L_SUBFR);

      }
      /*--------------------------------------------------------*
       * Save tcx parameters if tcx segmental SNR is better     *
       *--------------------------------------------------------*/
      test();
      if (sub(tmp,snr) > 0) 
      {
        st->Q_exc = add(st->Q_sp,exp_dwn);      move16();
        st->LastQMode = 1;                      move16();
        
        snr = tmp;            move16();
        mod[k] = 1;           move16();
        coding_mod[k] = 1;    move16();
        
        mem_w0[k+1] = mem_w0_tcx;           move16();
        mem_xnq[k+1] = mem_xnq_tcx;         move16();
        mem_wsyn[k+1] = mem_wsyn_tcx;       move16();
        
        ovlp_size[k+1] = 32;                    move16();
        
        Copy(wovlp_tcx, &wovlp[(k+1)*128], 128);

        Copy(&synth_tcx[M], &synth[k*L_DIV], L_DIV);
        Copy(exc_tcx, &exc[k*L_DIV], L_DIV);
        
        Copy(prm_tcx, prm, NPRM_TCX20);

      }
      snr1 = add(snr1, shr(snr,1));
    } /* end of coding mode ACELP or TCX_20MS */

    k = shl(i1,1);
    
    /* set pointer to parameters */
    prm = param + (k*NPRM_DIV);        move16();
    
    /*---------------------------------------------------------------*
     *  Quantize ISF parameters (46 bits) every 40 ms                *
     *---------------------------------------------------------------*/
    
    /* Convert isps to frequency domain 0..6400 */
    Isp_isf(&isp[(k+2)*M], isfnew, M);
    
    /* quantize 1st and 2nd LPC with 46 bits */
    Qpisf_2s_46b(isfnew, isfnew, past_isfq_2, prm_tcx, 4);
    
    /* Convert isfs to the cosine domain */
    Isf_isp(isfnew, ispnew_q, M);
    
    /* interpol quantized lpc */
    Int_lpc(&isp_q[k*M], ispnew_q,interpol_frac8, Aq, (NB_SUBFR/2), M);
    
    /*--------------------------------------------------*
     * Call medium TCX coder and find segmental SNR       *
     *--------------------------------------------------*/
    
    Copy(&synth[(k*L_DIV)-M], synth_tcx, M);
    mem_w0_tcx = mem_w0[k];       move16();
    mem_xnq_tcx = mem_xnq[k];     move16();  
    
    Copy(&wovlp[k*128], wovlp_tcx, 128);
    
    exp_dwn = Coder_tcx(Aq,
                &speech[k*L_DIV],
                &mem_w0_tcx, &mem_xnq_tcx,
                &synth_tcx[M],
                exc_tcx,
                wovlp_tcx,
                ovlp_size[k],
                2*L_DIV,
                sub(shr(nbits,1),NBITS_LPC),
                prm_tcx+NPRM_LPC,
                st);
    
    mem_wsyn_tcx = mem_wsyn[k];     move16();
    
    {
        Word16 buf[L_FRAME_PLUS];

        Find_wsp(&A[i1*(NB_SUBFR/2)*(M+1)], &synth_tcx[M], buf, &mem_wsyn_tcx, 2*L_DIV);
        tmp = Segsnr(&wsp[k*L_DIV], buf, 2*L_DIV, L_SUBFR);

    }
	
    /*--------------------------------------------------------*
     * Save tcx parameters if tcx segmental SNR is better     *
     *--------------------------------------------------------*/
    test();
    if (sub(tmp,snr1) > 0) 
    {
      st->Q_exc = add(st->Q_sp,exp_dwn);      move16();
      st->LastQMode = 1;                      move16();
      snr1 = tmp;             move16();

      for (i=0; i<2; i++) 
      {
        mod[k+i] = 2;           move16();
        coding_mod[k+i] = 2;    move16();
      }

      Copy(ispnew_q, &isp_q[(k+2)*M], M);

      mem_w0[k+2] = mem_w0_tcx;       move16();
      mem_xnq[k+2] = mem_xnq_tcx;     move16();
      mem_wsyn[k+2] = mem_wsyn_tcx;   move16();
      
      ovlp_size[k+2] = 64;            move16();
      
      Copy(past_isfq_2, past_isfq_1, M);
      
      Copy(wovlp_tcx, &wovlp[(k+2)*128], 128);

      Copy(&synth_tcx[M], &synth[k*L_DIV], 2*L_DIV);
      Copy(exc_tcx, &exc[k*L_DIV], 2*L_DIV);
      
      Copy(prm_tcx, prm, NPRM_LPC+NPRM_TCX40);
      
      /* lpc coefficient needed for HF extension */
      Copy(Aq, &AqLF[k*4*(M+1)], 9*(M+1));

    }
    
    snr2 = add(snr2, shr(snr1,1));

  } /* end of i1 */
  
  k = 0;      move16();
  
  /* set pointer to parameters */
  prm = param + (k*NPRM_DIV);        move16();
  
  /*---------------------------------------------------------------*
   *  Quantize ISF parameters (46 bits) every 80 ms                *
   *---------------------------------------------------------------*/
  
  Copy(st->past_isfq, past_isfq_2, M);
  
  /* Convert isps to frequency domain 0..6400 */
  Isp_isf(&isp[(k+4)*M], isfnew, M); 
  
  /* quantize 1st and 2nd LPC with 46 bits */
  Qpisf_2s_46b(isfnew, isfnew, past_isfq_2, prm_tcx, 4);
  
  /* Convert isfs to the cosine domain */
  Isf_isp(isfnew, ispnew_q, M);
  
  /* interpol quantized lpc */
  Int_lpc(&isp_q[k*M], ispnew_q, interpol_frac16, Aq, NB_SUBFR, M); 
  
  /*--------------------------------------------------*
   * Call long TCX coder and find segmental SNR       *
   *--------------------------------------------------*/
  
  Copy(&synth[(k*L_DIV)-M], synth_tcx, M);
  mem_w0_tcx = mem_w0[k];                   move16();
  mem_xnq_tcx = mem_xnq[k];                 move16();
  
  Copy(&wovlp[k*128], wovlp_tcx, 128);
  
  exp_dwn = Coder_tcx(Aq,
                  &speech[k*L_DIV],
                  &mem_w0_tcx, &mem_xnq_tcx,
                  &synth_tcx[M],
                  exc_tcx,
                  wovlp_tcx,
                  ovlp_size[k],
                  4*L_DIV,
                  sub(nbits,NBITS_LPC),
                  prm_tcx+NPRM_LPC,
                  st);
  
  mem_wsyn_tcx = mem_wsyn[k];       move16();
  
  {
      Word16 buf[L_FRAME_PLUS];

      Find_wsp(&A[0*(NB_SUBFR/2)*(M+1)], &synth_tcx[M], buf, &mem_wsyn_tcx, 4*L_DIV);
      tmp = Segsnr(&wsp[k*L_DIV], buf, 4*L_DIV, L_SUBFR);
  }
  
  /*--------------------------------------------------------*
   * Save tcx parameters if tcx segmental SNR is better     *
   *--------------------------------------------------------*/

  test();
  if (sub(tmp,snr2) > 0)
  {
    st->Q_exc = add(st->Q_sp,exp_dwn);      move16();
    st->LastQMode = 1;                      move16();
    snr2 = tmp;                 move16();

    for (i=0; i<4; i++) 
    {
      mod[k+i] = 3;             move16();
      coding_mod[k+i] = 3;      move16();
    }
    
    Copy(ispnew_q, &isp_q[(k+4)*M], M);
    
    mem_w0[k+4] = mem_w0_tcx;       move16();
    mem_xnq[k+4] = mem_xnq_tcx;     move16();
    mem_wsyn[k+4] = mem_wsyn_tcx;   move16();
    
    ovlp_size[k+4] = 128;           move16();
    
    Copy(past_isfq_2, past_isfq_1, M);
    
    Copy(wovlp_tcx, &wovlp[(k+4)*128], 128);

    Copy(&synth_tcx[M], &synth[k*L_DIV], 4*L_DIV);
    Copy(exc_tcx, &exc[k*L_DIV], 4*L_DIV);
    
    Copy(prm_tcx, prm, NPRM_LPC+NPRM_TCX80);
    
    /* lpc coefficient needed for HF extension */
    Copy(Aq, &AqLF[k*4*(M+1)], 17*(M+1));

  }
  /*--------------------------------------------------*
   * Update filter memory.                            *
   *--------------------------------------------------*/

  st->old_ovlp_size = ovlp_size[NB_DIV];      move16();
  st->old_mem_w0 = mem_w0[NB_DIV];           move16();  
  st->old_mem_xnq = mem_xnq[NB_DIV];         move16();
  st->old_mem_wsyn = mem_wsyn[NB_DIV];       move16();

  Copy(&wovlp[4*128], st->old_wovlp, 128);
  Copy(past_isfq_1, st->past_isfq, M);
  Copy(&isp_q[NB_DIV*M], st->ispold_q, M);
  
  /*--------------------------------------------------*
   * Update exc for next frame.                       *
   *--------------------------------------------------*/
  Copy(&old_exc[L_FRAME_PLUS], st->old_exc, PIT_MAX_MAX+L_INTERPOL);
  return;
}


