
#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "count.h"


#include "amr_plus_fx.h"

static void Try_tcx(Word16 k, Word16 mode, Word16 *snr, Word16 A[], Word16 wsp[], Word16 mod[], Word16 coding_mod[],
                      Word16 isp[], Word16 isp_q[], Word16 AqLF[], Word16 speech[], Word16 mem_w0[], Word16 mem_xnq[],
                      Word16 mem_wsyn[], Word16 old_exc[], Word16 mem_syn[], Word16 wovlp[], Word16 ovlp_size[], Word16 past_isfq[],
                      Word16 pit_adj, Word16 nbits, Word16 nprm_tcx, Word16 prm[], Coder_State_Plus_fx *st);


extern const short NBITS_CORE_FX[];
/*-----------------------------------------------------------------*
 *   Funtion init_coder_wb 
 *   ~~~~~~~~~~~~~ 
 * ->Initialization of variables for the coder section.  
 * - initilize pointers to speech buffer 
 * - initialize static pointers 
 * - set static vectors to zero 
 * - compute lag window and LP analysis window
 *  
 *-----------------------------------------------------------------*/

void Init_coder_lf(Coder_State_Plus_fx *st)
{
  Word16 i;
  
  /* Static vectors to zero */
  Set_zero(st->old_exc, PIT_MAX_MAX+L_INTERPOL);
  Set_zero(st->old_d_wsp, PIT_MAX_MAX/OPL_DECIM);
  Set_zero(st->mem_lp_decim2, 3);
  Set_zero(st->past_isfq, M);
  Set_zero(st->old_wovlp, 128);
  Set_zero(st->hp_old_wsp, L_FRAME_PLUS/OPL_DECIM+(PIT_MAX_MAX/OPL_DECIM));
  Set_zero(st->hp_ol_ltp_mem, 3*2+1);
  for (i=0;i<5;i++)
  {
    st->old_ol_lag[i] = 40;      move16();
  }
  
  st->old_mem_wsyn = 0;      move16();
  st->old_mem_w0   = 0;      move16();
  st->old_mem_xnq  = 0;      move16();
  st->mem_wsp      = 0;      move16();
  
  st->old_T0_med = 0;      move16();
  st->ol_wght_flg = 0;      move16();
  st->ada_w = 0;      move16();
  
  st->old_wsp_max[0] = Q_MAX2;   move16();
  st->old_wsp_max[1] = Q_MAX2;   move16();
  st->old_wsp_max[2] = Q_MAX2;   move16();   
  st->old_wsp_max[3] = Q_MAX2;  move16();
  st->old_ovlp_size = 0;    move16();
  st->old_wsp_shift = 0;    move16();
  /* isf and isp initialization */
  
  Copy(Isf_init, st->isfold, M);
  Copy(Isp_init, st->ispold, M);
  Copy(st->ispold, st->ispold_q, M);
  Init_Hp_wsp(st->hp_ol_ltp_mem);
  return;
}
void Coder_lf_b(
  Word16 codec_mode,    /* (i) : AMR-WB+ mode (see cnst.h)             */ 
  Word16 speech[],      /* (i) : speech vector [-M..L_FRAME_PLUS+L_NEXT]    */ 
  Word16 synth[],       /* (o) : synthesis vector [-M..L_FRAME_PLUS]        */ 
  Word16 mod[],         /* (o) : mode for each 20ms frame (mode[4]     */
  Word16 AqLF[],        /* (o) : quantized coefficients (AdLF[16])     */ 
  Word16 wwindow[],     /* (i) : window for LPC analysis               */
  Word16 param[],       /* (o) : parameters (NB_DIV*NPRM_DIV)          */
  Word16 ol_gain[],     /* (o) : open-loop LTP gain                    */  
  Word16 ave_T_out[],   /* (o) : average LTP lag                       */ 
  Word16 ave_p_out[],   /* (o) : average LTP gain                      */ 
  Word16 coding_mod[],  /* (i) : selected mode for each 20ms           */
  Word16 pit_adj,       /* (i) : indicate pitch adjustment             */
  Coder_State_Plus_fx *st /* i/o : coder memory state                  */
) 
{ 
  Word16 *prm;
  /* LPC coefficients */
  Word16 ovlp_size[NB_DIV+1];

  /* Scalars */
  Word16 i, j, k, i2, i1, nbits;

  Word16 r_h[M+1];                   /* Autocorrelations of windowed speech  */
  Word16 r_l[M+1];                   /* Autocorrelations of windowed speech  */
  Word16 rc[M];
  Word16 A[(NB_SUBFR+1)*(M+1)];
  Word16 Ap_[M+1];

  Word16 ispnew[M];                /* LSPs at 4nd subframe                 */
  Word16 isfnew[M];
  Word16 isp[(NB_DIV+1)*M];
  Word16 isp_q[(NB_DIV+1)*M];
  Word16 past_isfq[3*M];                  /* past isf quantizer */

  Word16 mem_w0[NB_DIV+1], mem_wsyn[NB_DIV+1];
  Word16 mem_xnq[NB_DIV+1];

  Word16 *wsp;
  Word16 mem_syn[(NB_DIV+1)*M];

  Word16 wovlp[(NB_DIV+1)*128];

  /* Scalars */
  Word16 snr, snr1, snr2;

  Word32 Lener, Lcor_max, Lt0;
  Word16 *p, *p1;
  Word16 norm_corr[4], norm_corr2[4];
  Word16 T_op[NB_DIV], T_op2[NB_DIV];
  Word16 T_out[4]; /* integer pitch-lag */
  Word16 p_out[4];

  Word16 LTPGain[2];

  Word16 PIT_min;    /* Minimum pitch lag with resolution 1/4      */
  Word16 PIT_max;  /* Maximum pitch lag                          */

  Word16 tmp16;
  Word32 Ltmp;
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


    PIT_min = add(PIT_MIN_12k8 , i);      
    PIT_max = add(PIT_MAX_12k8, extract_l(L_shr(L_mult(i, 6),1)));       
    st->pit_max = PIT_max;      move16();
  }

  /* number of bits per frame (80 ms) */
  nbits = NBITS_CORE_FX[codec_mode];      move16();

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
    if (sub(pit_adj,FSCALE_DENOM) <= 0) 
    {
      /* Autocorrelations of signal at 12.8kHz */
      Autocorr(&speech[(i*L_DIV)+L_SUBFR], M, r_h, r_l, L_WINDOW, wwindow);	  
    }
    else 
    {
      /* Autocorrelations of signal at 12.8kHz */
      Autocorr(&speech[(i*L_DIV)+(L_SUBFR/2)], M, r_h, r_l, L_WINDOW_HIGH_RATE, wwindow);	  
    } 

    /* Lag windowing    */
    Lag_window(r_h, r_l, M);
    /* Levinson Durbin  */
    Levinson(r_h, r_l, Ap_, rc, st->lev_mem, M);
    /* From A(z) to ISP */
    Az_isp(Ap_, ispnew, st->ispold, M);

    for (j = 0; j < M; j++) 
    {
      st->_stClass->ApBuf[i*M+j] = Ap_[j];        move16();
    }

    Copy(ispnew, &isp[(i+1)*M], M);

    /* A(z) interpolation every 20 ms (used for weighted speech) */
    Int_lpc(st->ispold, ispnew, interpol_frac4, &A[i*4*(M+1)], 4, M);

    /* update ispold[] for the next LPC analysis */
    Copy(ispnew, st->ispold, M);
  }

  Copy(&synth[-M], mem_syn, M);
  wsp = synth;  move16();      /* wsp[] use synth[] as tmp buffer */
  {
    Word16 tmp_max, i_fr, exp_wsp, exp_t0, exp_cm, exp_ener;
    Word16 old_d_wsp[(PIT_MAX_MAX/OPL_DECIM)+L_DIV], *d_wsp_;   /* Weighted speech vector and pointer*/

    d_wsp_ = old_d_wsp + PIT_MAX_MAX/OPL_DECIM;     move16();
    Copy(st->old_d_wsp, old_d_wsp, PIT_MAX_MAX/OPL_DECIM);
    
    /* Calculate open-loop LTP parameters */
    for (i = 0; i < NB_DIV; i++) 
    {
      Find_wsp(&A[i*(NB_SUBFR/4)*(M+1)], &speech[i*L_DIV], &wsp[i*L_DIV], &(st->mem_wsp), L_DIV); 
      
      mem_w0[i+1] = st->mem_wsp;     move16();     /* prevent to recompute wspeech in acelp */  
      Copy(&wsp[i*L_DIV], d_wsp_, L_DIV); 

      /* find maximum value on wsp[] for 12 bits scaling */
      max = 0;                               move16();
      for (i_fr = 0; i_fr < L_DIV; i_fr++)
      {
          tmp16 = abs_s(d_wsp_[i_fr]);
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
      /* decimation of wsp[] to search pitch in LF and to reduce complexity */
      LP_Decim2(d_wsp_, L_DIV, st->mem_lp_decim2);
      /* scale wsp[] in 12 bits to avoid overflow */
      Scale_sig(d_wsp_, L_DIV / OPL_DECIM, shift);
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
      T_op[i] = Pitch_med_ol(d_wsp_, (PIT_min/OPL_DECIM)+1, PIT_max/OPL_DECIM,
                    (2*L_SUBFR)/OPL_DECIM, st->old_T0_med, &(st->ol_gain), 
                        st->hp_ol_ltp_mem, st->hp_old_wsp, st->ol_wght_flg);

      LTPGain[1] = st->ol_gain;    move16(); move16();

      test();
      if (sub(st->ol_gain, 19661) > 0)       /*0.6 in Q15 */
      {
        st->old_T0_med = Med_olag(T_op[i], st->old_ol_lag);
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
      
      p = &d_wsp_[0];           move16();
      p1 = d_wsp_ - T_op[i];    move16();

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
      T_op2[i] = Pitch_med_ol(d_wsp_ + ((2*L_SUBFR)/OPL_DECIM), (PIT_min/OPL_DECIM)+1, PIT_max/OPL_DECIM,
                    (2*L_SUBFR)/OPL_DECIM, st->old_T0_med, &(st->ol_gain), 
                        st->hp_ol_ltp_mem, st->hp_old_wsp, st->ol_wght_flg);

      LTPGain[0] = st->ol_gain;    move16();move16();

      test();
      if (sub(st->ol_gain, 19661) > 0)       /*0.6 in Q15 */
      {
        st->old_T0_med = Med_olag((Word16)T_op2[i], st->old_ol_lag);
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
      
      p = d_wsp_ + (2*L_SUBFR)/OPL_DECIM;                   move16();
      p1 = d_wsp_ + ((2*L_SUBFR)/OPL_DECIM) - T_op2[i];     move16();

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


      st->_stClass->LTPGain[(2*i+2)] = LTPGain[1];        move16();
      st->_stClass->LTPGain[(2*i+2)+1] = LTPGain[0];      move16();
      st->_stClass->LTPLag[(2*i)+2] = T_op[i];            move16();
      st->_stClass->LTPLag[(2*i+2)+1] = T_op2[i];         move16();
      st->_stClass->NormCorr[(2*i+2)] = norm_corr[i];     move16();
      st->_stClass->NormCorr[(2*i+2)+1] = norm_corr2[i];  move16();
    }

    Copy(old_d_wsp, st->old_d_wsp, PIT_MAX_MAX/OPL_DECIM);   /* d_wsp already shifted */

  }


  /*---------------------------------------------------------------*
  *  Call ACELP and TCX codec                                     *
  *---------------------------------------------------------------*/
  ovlp_size[0] = st->old_ovlp_size;     move16();    
  /*Classification refinement */
  ClassifyExcitationRef(st->_stClass, isp, coding_mod);

  test();  
  if (st->SwitchFlagPlusToWB && coding_mod[0] != 0) 
  {
    coding_mod[0] = 0;                  move16();
    st->_stClass->NbOfAcelps = add(st->_stClass->NbOfAcelps,1); move16();
    st->SwitchFlagPlusToWB = 0;         move16();
  }

  ovlp_size[1] = st->old_ovlp_size;     move16();
  ovlp_size[2] = st->old_ovlp_size;     move16();
  ovlp_size[3] = st->old_ovlp_size;     move16();
  ovlp_size[4] = st->old_ovlp_size;     move16();


  mem_w0[0]   = st->old_mem_w0;       move16();
  mem_xnq[0]   = st->old_mem_xnq;     move16();
  mem_wsyn[0] = st->old_mem_wsyn;     move16();


  Copy(st->old_wovlp, wovlp, 128);
  Copy(st->past_isfq, &past_isfq[0], M);
  Copy(st->ispold_q, isp_q, M);

  snr2 = 0;                  move16();
  for (i1=0; i1<2; i1++)
  {
    Copy(&past_isfq[i1*M], &past_isfq[(i1+1)*M], M);

    snr1 = 0;                move16();
    for (i2=0; i2<2; i2++)
    {

     /*k = (i1*2) + i2;*/
      k = add(shl(i1,1),i2);
      test(); test();test(); test();test();
      if (coding_mod[k] == 0 || (sub(coding_mod[k],1) == 0 && st->_stClass->NbOfAcelps != 0)
			  || (st->_stClass->NoMtcx[i1] != 0 &&  st->_stClass->NbOfAcelps == 0)
        ) 
      { /* ACELP or TCX_20MS*/

        /* set pointer to parameters */
        prm = param + (k*NPRM_DIV);     move16();

        /*---------------------------------------------------------------*
        *  Quantize ISF parameters (46 bits) every 20 ms                *
        *---------------------------------------------------------------*/
        /* Convert isps to frequency domain 0..6400 */
        Isp_isf(&isp[(k+1)*M], isfnew, M);
        /* quantize 1st and 2nd LPC with 46 bits */
        Qpisf_2s_46b(isfnew, isfnew, &past_isfq[(i1+1)*M], prm, 4);

        prm += NPRM_LPC;

        /* Convert isfs to the cosine domain */
        Isf_isp(isfnew, &isp_q[(k+1)*M], M);

        /* interpol quantized lpc */
        Int_lpc(&isp_q[k*M], &isp_q[(k+1)*M], interpol_frac4, &AqLF[k*4*(M+1)], 4, M);
       /*---------------------------------------------------------------*
        *  Call ACELP 4 subfr x 5ms = 20 ms frame                       *
        *---------------------------------------------------------------*/
        test();
        if (coding_mod[k] == 0) 
        {
          mem_xnq[k+1] = mem_xnq[k];      move16();
          ovlp_size[k+1] = 0;               move16();

          {

            Word16 old_exc[PIT_MAX_MAX+L_INTERPOL+L_DIV+1];    /* Excitation vector */
            Word16 old_syn[M+L_DIV];
            Word16 buf[L_DIV];

            Copy(st->old_exc, old_exc, PIT_MAX_MAX+L_INTERPOL);			
            Copy(&mem_syn[k*M], old_syn, M);   

            Coder_acelp(
              &A[k*(NB_SUBFR/4)*(M+1)], 
              &AqLF[k*(NB_SUBFR/4)*(M+1)], 
              &speech[k*L_DIV]/*,&mem_w0[k+1] TV*/,
              &mem_xnq[k+1],
              old_syn+M,
              old_exc+PIT_MAX_MAX+L_INTERPOL,
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

            Copy(&old_exc[L_DIV], st->old_exc, PIT_MAX_MAX+L_INTERPOL);			
            Copy(&old_syn[L_DIV], &mem_syn[(k+1)*M], M);                    
            

            /* average integer pitch-lag for high band coding */
            ave_T_out[k] = add(T_op[k],T_op2[k]);                    move16();

            /*ave_p_out[k] = (p_out[0]+p_out[1]+p_out[2]+p_out[3])/4.0f;*/
            Ltmp = L_mult(p_out[0],8192);
            Ltmp = L_mac(Ltmp, p_out[1],8192);
            Ltmp = L_mac(Ltmp, p_out[2],8192);
            ave_p_out[k] = round(L_mac(Ltmp, p_out[3],8192));        move16();
            mem_wsyn[k+1] = mem_wsyn[k];                             move16();
            Find_wsp(&A[k*(NB_SUBFR/4)*(M+1)], &old_syn[M], buf, &mem_wsyn[k+1], L_DIV);
            st->LastQMode = 0; move16(); /* Acelp mode */

          }

          mod[k] = 0;           move16();
          coding_mod[k] = 0;    move16();

        } /* end ACELP */
        /*--------------------------------------------------*
        * Call 20MS TCX coder and find segmental SNR       *
        *--------------------------------------------------*/
        test();
        if (coding_mod[k] != 0 ) 
        {
          snr = -32760;        move16();

          Try_tcx(k, 1, &snr, A, wsp, mod, coding_mod, isp, isp_q,
              AqLF, speech, mem_w0, mem_xnq, mem_wsyn, st->old_exc,
              mem_syn, wovlp, ovlp_size, past_isfq, pit_adj,
              sub(shr(nbits,2),NBITS_LPC), NPRM_TCX20, prm, st);					

          snr1 = add(snr1, shr(snr,1));					
        } /* end of TCX_20MS */

      } /* end of i2 */
    } /* end of coding mode ACELP or TCX_20MS */
    test();test();
    if (coding_mod[i1*2] != 0 && coding_mod[i1*2+1] != 0) 
    { /* TCX_40MS*/
      test();
      if (st->_stClass->NbOfAcelps == 0) 
      {
        snr1 = -32760;                   move16();    /* TCX20 off*/
      }
      k = shl(i1,1);

      /* set pointer to parameters */
      prm = param + (k*NPRM_DIV);           move16();

      Try_tcx(k, 2, &snr1, A, wsp, mod, coding_mod, isp, isp_q,
                AqLF, speech, mem_w0, mem_xnq, mem_wsyn, st->old_exc,
                  mem_syn, wovlp, ovlp_size, past_isfq, pit_adj,
                    sub(shr(nbits,1),NBITS_LPC), NPRM_LPC+NPRM_TCX40, prm, st);

      snr2 = add(snr2, shr(snr1,1));

    } /* end of coding mode TCX_40MS */
  } /* end of i1 */
  test();test();test();test();test();test();
  if (coding_mod[0] != 0 &&  coding_mod[1] != 0 &&  coding_mod[2] != 0 &&  coding_mod[3] != 0 &&
          st->_stClass->NoMtcx[0] == 0 &&  st->_stClass->NoMtcx[1] == 0) 
  { 
    k = 0;                                  move16();
    /* set pointer to parameters */
    prm = param + (k*NPRM_DIV);             move16();
    Try_tcx(k, 3, &snr2, A, wsp, mod, coding_mod, isp, isp_q,
                AqLF, speech, mem_w0, mem_xnq, mem_wsyn, st->old_exc,
                    mem_syn, wovlp, ovlp_size, past_isfq, pit_adj,
                         sub(nbits,NBITS_LPC), NPRM_LPC+NPRM_TCX80, prm, st);

  } /* end of coding mode 3 */
  for(i=0;i<4;i++)
  {
    st->_stClass->prevModes[i] = coding_mod[i];          move16();
  }

  if (st->_stClass->NoMtcx[0] != 0) 
  {

    st->_stClass->NoMtcx[0] = 0;               move16();

  }
  if (st->_stClass->NoMtcx[1] != 0) 
  {
      st->_stClass->NoMtcx[1] = 0;                 move16();

  }

  /*--------------------------------------------------*
  * Update filter memory.                            *
  *--------------------------------------------------*/
  st->old_ovlp_size = ovlp_size[NB_DIV];            move16();

  st->old_mem_w0 = mem_w0[NB_DIV];                move16();
  st->old_mem_xnq = mem_xnq[NB_DIV];              move16();
  st->old_mem_wsyn = mem_wsyn[NB_DIV];            move16();

  Copy(&wovlp[NB_DIV*128], st->old_wovlp, 128);   
  Copy(&past_isfq[2*M], st->past_isfq, M);
  Copy(&isp_q[NB_DIV*M], st->ispold_q, M);
  Copy(&mem_syn[NB_DIV*M], synth+L_FRAME_PLUS-M, M);

  return;
}

static void Try_tcx(
  Word16 k,             /* i  : subfr value                 */
  Word16 mode,          /* i  :mode 1=TCX20,2=TCX40 3=TCX80 */
  Word16 *snr,          /*i/o : Snr value                   */
  Word16 A[],           /* i  : coefficients Az             */
  Word16 wsp[],         /* i  : Weighted speech             */
  Word16 mod[],         /* i/o: Coding mode                 */ 
  Word16 coding_mod[],  /* i  : selected mode for each 20ms */
  Word16 isp[],         /* i  : Isp coefficient             */
  Word16 isp_q[],       /* i/o: Quantized isp coef          */
  Word16 AqLF[],        /* i/o: Low frequencies Az coeff    */
  Word16 speech[],      /* i  : Input speech                */
  Word16 mem_w0[],      /* i/o: memory of target            */
  Word16 mem_xnq[],     /* i/o: memory of quantized xn      */
  Word16 mem_wsyn[],    /* i/o: Weighted synthesis memory   */
  Word16 old_exc[],     /* i/o: Old excitation vector       */
  Word16 mem_syn[],     /* i/o: Synthesis memory            */
  Word16 wovlp[],       /* i/o: Overlap window              */
  Word16 ovlp_size[],   /* i/o: Overlap size vector         */
  Word16 past_isfq[],   /* i/o: Old quantized isf vector    */
  Word16 pit_adj,       /* i  : indicate pitch adjustment   */ 
  Word16 nbits,         /* i  : Number of bits to use       */
  Word16 nprm_tcx,      /* i  : Number of parameters        */
  Word16 prm[],         /* i/o: Parameters vector           */
  Coder_State_Plus_fx *st /* i/o : encoder static memory    */
)
{

  /* Scalars */
  Word16 i, ndiv;
  Word16 tmp_snr;
  Word16 PIT_max, ndiv_l;

  Word16 Aq[(NB_SUBFR+1)*(M+1)], *p_Aq;
  Word16 synth_tcx[M+L_TCX];
  Word16 exc_tcx[L_TCX];
  Word16 mem_w0_tcx, mem_xnq_tcx, mem_wsyn_tcx;
  Word16 wovlp_tcx[128];
  Word16 past_isfq_tcx[M];
  Word16 ispnew_q[M];              /* LSPs at 4nd subframe                 */
  Word16 isfnew[M], exp_dwn;
  const Word16 *wind_frac;

  Word16 prm_tcx_buf[NPRM_LPC+NPRM_TCX80], *prm_tcx;

  test();  
  if(pit_adj ==0) 
  {
    PIT_max = PIT_MAX_12k8;       move16();
  }
  else 
  {
    PIT_max = st->pit_max;      move16();
  }

  prm_tcx = prm_tcx_buf;        move16();


  ndiv = mode;        move16();
  test();test();
  if(sub(ndiv,1)== 0)
  {
    wind_frac = interpol_frac4;     move16();
  }
  else if (sub(ndiv,2) == 0)
  {
    wind_frac = interpol_frac8;      move16();
  }
  else/* if (sub(ndiv,3) == 0)*/
  {
    ndiv = add(ndiv,1);    /* 4 divisions in mode 3 (TCX80) */
    wind_frac = interpol_frac16;     move16();
  }
  ndiv_l = extract_l(L_mult(ndiv, L_DIV/2));

  test();  
  if (sub(mode,1) > 0)
  {

   /*---------------------------------------------------------------*
    *  Quantize ISF parameters (46 bits) every 40/80 ms             *
    *---------------------------------------------------------------*/

    /* Convert isps to frequency domain 0..6400 */
    Isp_isf(&isp[(k+ndiv)*M], isfnew, M);

    Copy(&past_isfq[(k/2)*M], past_isfq_tcx, M);

    /* quantize 1st and 2nd LPC with 46 bits */
    Qpisf_2s_46b(isfnew, isfnew, past_isfq_tcx, prm_tcx, 4);			

    prm_tcx += NPRM_LPC;

    /* Convert isfs to the cosine domain */
    Isf_isp(isfnew, ispnew_q, M);

    /* interpol quantized lpc */
    Int_lpc(&isp_q[k*M], ispnew_q, wind_frac , Aq, shl(ndiv,2), M);

    p_Aq = Aq;   move16();
  }
  else 
  {
    p_Aq = &AqLF[k*4*(M+1)];    move16();
  }



 /*--------------------------------------------------------*
  * Call 20/40/80MS TCX coder and find segmental SNR       *
  *--------------------------------------------------------*/

  Copy(&mem_syn[k*M], synth_tcx, M);

  mem_w0_tcx = mem_w0[k];               move16();
  mem_xnq_tcx = mem_xnq[k];             move16();
  Copy(&wovlp[k*128], wovlp_tcx, 128);
  exp_dwn = Coder_tcx(
                p_Aq, 
                &speech[k*L_DIV], 
                &mem_w0_tcx, 
                &mem_xnq_tcx, 
                &synth_tcx[M], 
                exc_tcx, 
                wovlp_tcx, 
                ovlp_size[k], 
                ndiv_l, 
                nbits, 
                prm_tcx, 
                st);

  mem_wsyn_tcx = mem_wsyn[k];         move16();


  {
    Word16 buf[L_FRAME_PLUS];
   
    Find_wsp(&A[k*4*(M+1)], &synth_tcx[M], buf, &mem_wsyn_tcx, ndiv_l);    

    tmp_snr = Segsnr(&wsp[k*L_DIV], buf, ndiv_l, L_SUBFR);
 
  }
  
 /*--------------------------------------------------------*
  * Save tcx parameters if tcx segmental SNR is better     *
  *--------------------------------------------------------*/
  
  st->Q_exc = add(st->Q_sp,exp_dwn);      move16();

  test();
  if ( sub(tmp_snr,*snr) > 0 )
  {
    st->LastQMode = 1;                      move16();

    *snr = tmp_snr;         move16();
    for (i=0; i<ndiv; i++) 
    {
      mod[k+i] = mode;          move16();
      coding_mod[k+i] = mode;   move16();

    }

    mem_w0[k+ndiv] = mem_w0_tcx;      move16();
    mem_xnq[k+ndiv] = mem_xnq_tcx;    move16();
    mem_wsyn[k+ndiv] = mem_wsyn_tcx;  move16();


    ovlp_size[k+ndiv] = shl(ndiv,5);   move16(); 

    test();
    if (sub(mode,1) > 0) 
    {
      Copy(ispnew_q, &isp_q[(k+ndiv)*M], M);

      Copy(past_isfq_tcx, &past_isfq[((k+ndiv)/2)*M], M);

      /* lpc coefficient needed for HF extension */
      Copy(Aq, &AqLF[k*4*(M+1)], ((ndiv*4)+1)*(M+1));
    }
    test();
    if (sub(ndiv,1)==0)
    {
      Copy(&old_exc[L_DIV], old_exc, (PIT_MAX_MAX + L_INTERPOL)- L_DIV );   
  
      Copy(exc_tcx, old_exc+PIT_MAX_MAX+L_INTERPOL - L_DIV , L_DIV);
    }
    else
    {
      Copy(&exc_tcx[(ndiv_l)-(PIT_MAX_MAX+L_INTERPOL)], old_exc, PIT_MAX_MAX+L_INTERPOL);
    }

    Copy(prm_tcx_buf, prm, nprm_tcx);
    Copy(wovlp_tcx, &wovlp[(k+ndiv)*128], 128);
    Copy(&synth_tcx[ndiv_l], &mem_syn[(k+ndiv)*M], M);
  }


  return;
}

void Find_wsp(
    const Word16 *Az,           /* i:   A(z) filter coefficients                Q12 */
    const Word16 *speech_ns,    /* i:   pointer to the denoised speech frame        */
    Word16       *wsp,          /* o:   poitnter to the weighted speech frame       */
    Word16       *mem_wsp,       /* i/o: W(z) denominator memory                     */
    Word16       lg
)
{    
    Word16 i_subfr, Azp[M+1];
    const Word16 *p_Az;


   /*----------------------------------------------------------------------*
    *  Compute weighted speech for all subframes        
    *----------------------------------------------------------------------*/
    p_Az = Az;      move16();
    for ( i_subfr = 0;  i_subfr < lg; i_subfr += L_SUBFR )
    {
        Weight_a(p_Az, Azp, GAMMA1_FX, M); 
        /* use residu2 to prevent saturation */
        /* because the first scaling is done before decimation filtering */
        Residu2(Azp, M, &speech_ns[i_subfr], &wsp[i_subfr], L_SUBFR);
        p_Az += (M+1);
    }
   /*----------------------------------------------------------------------*
    *  Weighted speech computation is extended on look-ahead
    *----------------------------------------------------------------------*/

    Deemph(wsp, TILT_FAC_FX, lg, mem_wsp);        

    return;
}

















