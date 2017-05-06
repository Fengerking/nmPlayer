#include "typedef.h"


#include <stdlib.h>
#include "basic_op.h"
#include "count.h"
#include "math_op.h"
#include "oper_32b.h"
#include "log2.h"

#include "amr_plus_fx.h"

void Coder_acelp(
  Word16 Az[],          /* i  : coefficients 4xAz[M+1]    */
  Word16 Azq[],         /* i  : coefficients 4xAz_q[M+1]  */
  Word16 speech[],      /* i  : speech[-M..lg]            */
  /*Word16 *mem_wsp,    i/o: wsp memory , computed in Find_wsp()   */
  Word16 *mem_wsyn_,    /* i/o: wsyn memory               */
  Word16 synth_[],      /* i/o: synth[-M..lg]             */
  Word16 exc[],         /* i/o: exc[-(PIT_max+L_INTERPOL)..lg+1] */
  Word16 wovlp_[],      /* o  : wovlp[0..128]             */
  Word16 lg,            /* i  : frame length              */
  Word16 codec_mode,    /* i  : AMR_WB+ mode (see cnst.h) */
  Word16 norm_corr,     /* i  : Normalized correlation    */
  Word16 norm_corr2,    /* i  : Normalized correlation    */
  Word16 T_op,          /* i  : open-loop LTP             */  
  Word16 T_op2,         /* i  : open-loop LTP             */  
  Word16 T_out[],       /* o  : integer pitch-lag         */  
  Word16 p_out[],       /* o  : pitch gain                */ 
  Word16 c_out_[],	    /* o  : fixed codebook gain       */
  Word16 pit_adj,       /* i  : indicate pitch adjustment */ 
  Word16 *sprm,         /* o  : acelp parameters          */
  Word16 xn_in[],       /* i  : Target                    */
  Coder_State_Plus_fx* st /* i/o : encoder static memory    */
  )                     
{
  Word16 i, i_subfr, select, j;
  Word16 T0, T0_min, T0_max, index, pit_flag;
  Word16 T0_frac;

  Word16 PIT_min;       /* Minimum pitch lag with resolution 1/4      */
  Word16 PIT_fr2;       /* Minimum pitch lag with resolution 1/2      */
  Word16 PIT_fr1;       /* Minimum pitch lag with resolution 1        */
  Word16 PIT_max;       /* Maximum pitch lag                          */

  /* pitch variables assignement */
  Word32 ener, mean_ener_code, gain_code;
  Word16 gain_pit, max, gain1, gain2;
  Word16 g_corr[10], g_corr2[4];            /*norm_corr, norm_corr2*/
  Word16 *p_A, *p_Aq, Ap[M+1];

  Word16 h1[L_SUBFR+M+1]; 
  Word16 h2[L_SUBFR]; 

  Word16 code[L_SUBFR];
  
  Word16 error[M+L_SUBFR];
  Word16 cn[L_SUBFR];
  Word16 xn[L_SUBFR];
  Word16 xn2[L_SUBFR];
  Word16 dn[L_SUBFR];        /* Correlation between xn and h1      */
  Word16 y1[L_SUBFR];        /* Filtered adaptive excitation       */
  Word16 y2[L_SUBFR];        /* Filtered adaptive excitation       */

  Word16 exp1, m1, exp2;
  Word16 ener_dB, mean_ener_code_dB, tmp16;
  Word32 Ltmp;  
  Word16 g_code;

  test();
  if(pit_adj ==0) 
  {
    PIT_min = PIT_MIN_12k8;     move16();
    PIT_fr2 = PIT_FR2_12k8;     move16();
    PIT_fr1 = PIT_FR1_12k8;     move16();
    PIT_max = PIT_MAX_12k8;     move16();
  }
  else 
  {
   /*  (((pit_adj*PIT_MIN_12k8)+(FSCALE_DENOM/2))/FSCALE_DENOM)-PIT_MIN_12k8; */
    i = st->i_offset ;                  move16();
    PIT_min = add(PIT_MIN_12k8, i);
    PIT_fr2 = sub(PIT_FR2_12k8, i);
    PIT_fr1 = PIT_FR1_12k8;             move16();
    PIT_max = st->pit_max;              move16();
  }

  T_op = shl(T_op,1)            /*OPL_DECIM == 2 */;
  T_op2 = shl(T_op2,1);
  
  /* range for closed loop pitch search in 1st subframe */
  T0_min = sub(T_op, 8);
  
  test();
  if (sub(T0_min,PIT_min) < 0) 
  {
    T0_min = PIT_min;     move16();
  }
  T0_max = add(T0_min, 15);
  test();
  if (sub(T0_max,PIT_max) > 0) 
  {
    T0_max = PIT_max;             move16();
    T0_min = sub(T0_max, 15);
  }
  	  
 /*------------------------------------------------------------------------*
  * Find and quantize mean_ener_code for gain quantizer (q_gain2_live.c)   *
  * This absolute reference replace the gains prediction.                  *
  * This modification for AMR_WB+ have the following advantage:            *
  * - better quantization on attacks (onset, drum impulse, etc.)           *
  * - robust on packet lost.                                               *
  * - independent of previous mode (can be TCX with alg. 7 bits quantizer).*
  *------------------------------------------------------------------------*/

  max =0;                move16();
  mean_ener_code = 0;    move32();

  p_Aq = Azq;            move16();

  /* update scaling of exc memory when past mode was tcx */
  tmp16 = sub(st->Q_sp, st->Q_exc);

  test();
  if(tmp16 != 0)
    Scale_sig(&exc[-(PIT_MAX_MAX+L_INTERPOL)], PIT_MAX_MAX+L_INTERPOL,  tmp16);
  for (i_subfr=0; i_subfr<lg; i_subfr+=L_SUBFR) 
  {

    Residu(p_Aq, M, &speech[i_subfr], &exc[i_subfr], L_SUBFR);

    ener = 1;     move32();
    for (i=0; i<L_SUBFR; i++) 
    {
      tmp16 = mult(exc[i+i_subfr], 8192);
      ener = L_mac(ener, tmp16, tmp16);
    }

    /*ener = 10.0f*(float)log10(ener/((float)L_SUBFR));*/
    exp2 = norm_l(ener);
    ener = L_shl(ener,exp2);
    exp2 = sub(30,exp2);

    Log2_norm(ener, exp2, &exp1, &m1);
    exp2 = sub(31-6, add(exp1, shl(st->Q_sp,1)));

    Ltmp    = Mpy_32_16(exp2, m1, LG10);  
    ener_dB = extract_l(L_shr(Ltmp, 14-7));  /* ener_dB Q7 */

    test();
    if (ener_dB < 0) 
    {
      ener_dB = 0;       move16();        /* ener in log (0..90dB) */
    }
    test();
    if (sub(ener_dB,max) > 0)
    {
      max = ener_dB;    move16();
    }
    mean_ener_code = L_mac(mean_ener_code, ener_dB, 8192);  /*Q7*/    
    p_Aq += (M+1);
  } 
  mean_ener_code_dB = round(mean_ener_code);   /*Q7*/

  
  /* reduce mean energy on voiced signal */
  /*mean_ener_code -= 5.0f*norm_corr;*/
  mean_ener_code = L_msu(mean_ener_code, norm_corr, 5*128);
  /*mean_ener_code -= 5.0f*norm_corr2;*/
  mean_ener_code = L_msu(mean_ener_code, norm_corr2, 5*128);


  /* quantize mean energy with 2 bits : 18, 30, 42 or 54 dB */
  /*tmp = (mean_ener_code-18.0f) / 12.0f;*/

  mean_ener_code = L_sub(mean_ener_code, 150994944);  /*18*128*65365*/
  mean_ener_code_dB = round(mean_ener_code);
  Ltmp = L_msu(mean_ener_code, mean_ener_code_dB, 30037);  /*  x 1/12 */ 
  
  /*index = (int)floor(tmp + 0.5);*/
  Ltmp = L_add(Ltmp, 4194304);
  Ltmp = L_shr(Ltmp, 7);
  index = extract_h(Ltmp);           /*Q0 */

  test();
  if (index < 0) 
  {
    index = 0;      move16();
  }
  test();
  if (sub(index,3) > 0)
  {
    index = 3;      move16();
  }

  /*mean_ener_code = (((float)index) * 12.0f) + 18.0f;*/
  mean_ener_code = L_mac(18, index, 12/2);
  mean_ener_code_dB= shl(extract_l(mean_ener_code),7);
  tmp16 = sub(max,27*128);
  /* limit mean energy to be able to quantize attack (table limited to +24dB) */
  test();test();
  while ((sub(mean_ener_code_dB,tmp16)<0) && (sub(index,3) < 0)) 
  {
    index = add(index,1);
    mean_ener_code_dB = add(mean_ener_code_dB, 12*128);
    test();test();
  }
  *sprm = index;   move16();   sprm++;

 /*------------------------------------------------------------------------*
  *          Loop for every subframe in the analysis frame                 *
  *------------------------------------------------------------------------*
  *  To find the pitch and innovation parameters. The subframe size is     *
  *  L_SUBFR and the loop is repeated L_FRAME_PLUS/L_SUBFR times.          *
  *     - compute impulse response of weighted synthesis filter (h1[])     *
  *     - compute the target signal for pitch search                       *
  *     - find the closed-loop pitch parameters                            *
  *     - encode the pitch dealy                                           *
  *     - update the impulse response h1[] by including fixed-gain pitch   *
  *     - find target vector for codebook search                           *
  *     - correlation between target vector and impulse response           *
  *     - codebook search                                                  *
  *     - encode codebook address                                          *
  *     - VQ of pitch and codebook gains                                   *
  *     - find synthesis speech                                            *
  *     - update states of weighting filter                                *
  *------------------------------------------------------------------------*/

  p_A = Az;     move16();
  p_Aq = Azq;   move16();

  for (i_subfr=0; i_subfr<lg; i_subfr+=L_SUBFR) 
  {
    pit_flag = i_subfr;               move16();
    test();
    if (sub(i_subfr,(2*L_SUBFR)) == 0) 
    {
      pit_flag = 0;                     move16();
      /* range for closed loop pitch search in 3rd subframe */
      T0_min = sub(T_op2, 8);
      test();
      if (sub(T0_min, PIT_min) < 0)
      {
        T0_min = PIT_min;           move16();
      }
      T0_max = add(T0_min, 15);
      test();
      if (sub(T0_max,PIT_max) > 0)
      {
        T0_max = PIT_max;             move16();
        T0_min = sub(T0_max, 15);
      }
    }

    /*-----------------------------------------------------------------------*

             Find the target vector for pitch search:
             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

                 |------|  res[n]
     speech[n]---| A(z) |--------
                 |------|       |   |-------------|
                       zero -- (-)--| 1/A(z/gamma)|----- target xn[]
                       exc          |-------------|

    Instead of subtracting the zero-input response of filters from
    the weighted input speech, the above configuration is used to
    compute the target vector.

    *-----------------------------------------------------------------------*/
    /* find WSP (done for pitch ol, just copy into xn) */
    Weight_a(p_A, Ap, GAMMA1_FX, M);
    Copy(&xn_in[i_subfr], xn, L_SUBFR);     

    /* find ZIR in weighted domain */

    Copy(&synth_[i_subfr-M], error, M);
    Set_zero(&error[M], L_SUBFR);

    Syn_filt(p_Aq,M, &error[M], &error[M], L_SUBFR, error, 0);

    /*Weight_a(p_A, Ap, GAMMA1_FX, M); already done */
    Residu(Ap, M,  &error[M], xn2, L_SUBFR);

    Deemph2(xn2, TILT_FAC_FX, L_SUBFR, mem_wsyn_);

    for (i=0; i<L_SUBFR; i++) 
    {
      xn[i] = sub(xn[i], xn2[i]);     move16();
    }

    /* fill current exc with residu */

    Residu(p_Aq, M, &speech[i_subfr], &exc[i_subfr], L_SUBFR);

    /*--------------------------------------------------------------*
    * Find target in residual domain (cn[]) for innovation search. *
    *--------------------------------------------------------------*/

    /* first half: xn[] --> cn[] */
    Set_zero(code, M);
    Copy(xn, &code[M], L_SUBFR/2);

    tmp16 = 0;      move16();
    Preemph(&code[M], TILT_FAC_FX, L_SUBFR/2, &tmp16);
    /* Weight_a(p_A, Ap, GAMMA1_FX, M);  already done */
    Syn_filt_s(0, Ap, M, &code[M], &code[M], L_SUBFR/2, code, 0);
    Residu(p_Aq, M, &code[M], cn, L_SUBFR/2);

    /* second half: res[] --> cn[] (approximated and faster) */
    Copy(&exc[i_subfr+(L_SUBFR/2)], cn+(L_SUBFR/2), L_SUBFR/2);

    /*---------------------------------------------------------------*
     * Compute impulse response, h1[], of weighted synthesis filter  *
     *---------------------------------------------------------------*/

    /*Weight_a(p_A, Ap, GAMMA1, M);  already done */
    Set_zero(h1, L_SUBFR+M+1);
    Copy(Ap, &h1[M], M+1);

    for (i = 0; i < L_SUBFR; i++)
    {
        Ltmp = L_mult(h1[i + M], 16384);        /* x4 (Q12 to Q14) */
        for (j = 1; j <= M; j++)
            Ltmp = L_msu(Ltmp, p_Aq[j], h1[i + M - j]);
        
        h1[i + M] = round(L_shl(Ltmp, 3));
        h1[i] = h1[i + M];      move16();move16();
    }

    /* deemph without division by 2 -> Q14 to Q15 */
    tmp16 = 0;                           move16();
    Deemph2(h1, TILT_FAC_FX, L_SUBFR, &tmp16);   /* h1 in Q14 */

    /* h2 in Q12 for codebook search */
    Copy(h1, h2, L_SUBFR);
    Scale_sig(h2, L_SUBFR, -2);

    /*---------------------------------------------------------------*
     * scale xn[] and h1[] to avoid overflow in dot_product12()      *
     *---------------------------------------------------------------*/

    test();
    if(st->old_wsp_shift != 0)
      Scale_sig(xn, L_SUBFR, st->old_wsp_shift);     /* scaling of xn[] to limit dynamic at 12 bits */
    test();
    if(sub(st->old_wsp_shift,1) != 0)
      Scale_sig(h1, L_SUBFR, add(1, st->old_wsp_shift));  /* set h1[] in Q15 with scaling for convolution */

    /*----------------------------------------------------------------------*
     *                 Closed-loop fractional pitch search                  *
     *----------------------------------------------------------------------*/

    /* find closed loop fractional pitch  lag */

    T0 = Pitch_fr4(&exc[i_subfr], xn, h1, T0_min, T0_max, &T0_frac,
                  pit_flag, PIT_fr2 , PIT_fr1, L_SUBFR);
    
      /* encode pitch lag */
    test();
    if (pit_flag == 0) 
    {  /* if 1st/3rd subframe */
       /*--------------------------------------------------------------*
        * The pitch range for the 1st/3rd subframe is encoded with     *
        * 9 bits and is divided as follows:                            *
        *   PIT_min to PIT_fr2-1  resolution 1/4 (frac = 0,1,2 or 3)   *
        *   PIT_fr2 to PIT_fr1-1  resolution 1/2 (frac = 0 or 2)       *
        *   PIT_fr1 to PIT_max    resolution 1   (frac = 0)            *
        *--------------------------------------------------------------*/
        test();test();
        if (sub(T0,PIT_fr2) < 0) 
        {
          index = sub(add(shl(T0,2) , T0_frac) , shl(PIT_min,2));					
        }
        else if (sub(T0,PIT_fr1) < 0) 
        {
          index = add(sub(add(shl(T0,1) , shr(T0_frac,1)) , shl(PIT_fr2,1)) , shl(sub(PIT_fr2,PIT_min),2));	
        }
        else 
        {
          index = add(add(sub(T0, PIT_fr1) , shl(sub(PIT_fr2,PIT_min),2)) , shl(sub(PIT_fr1,PIT_fr2),1));	
        }

        /* find T0_min and T0_max for subframe 2 and 4 */
        T0_min = sub(T0, 8);
        test();
        if (sub(T0_min,PIT_min) < 0) 
        {
          T0_min = PIT_min;     move16();
        }
        T0_max = add(T0_min, 15);
        test();
        if (sub(T0_max,PIT_max) > 0) 
        {
          T0_max = PIT_max;         move16();
          T0_min = sub(T0_max, 15);
        }
    }
    else 
    {     /* if subframe 2 or 4 */
       /*--------------------------------------------------------------*
        * The pitch range for subframe 2 or 4 is encoded with 6 bits:  *
        *   T0_min  to T0_max     resolution 1/4 (frac = 0,1,2 or 3)   *
        *--------------------------------------------------------------*/
        i = sub(T0, T0_min);
        index = add(shl(i,2), T0_frac);
    }

    /* store pitch-lag for high band coding */
    T_out[i_subfr/L_SUBFR] = T0;        move16();

    *sprm = index;  sprm++;             move16();
   /*-----------------------------------------------------------------*
    * - find unity gain pitch excitation (adaptive codebook entry)    *
    *   with fractional interpolation.                                *
    * - find filtered pitch exc. y1[]=exc[] convolved with h1[])      *
    * - compute pitch gain1                                           *
    *-----------------------------------------------------------------*/
    Pred_lt4(&exc[i_subfr], (Word16)T0, T0_frac, L_SUBFR + 1);
    Convolve(&exc[i_subfr], h1, y1, L_SUBFR);
    gain1 = G_pitch(xn, y1, g_corr, L_SUBFR);
    
    /* find energy of new target xn2[] */
    Updt_tar(xn, dn, y1, gain1, L_SUBFR);       /* dn used temporary */
  
    /*-----------------------------------------------------------------*
     * - find pitch excitation filtered by 1st order LP filter.        *
     * - find filtered pitch exc. y2[]=exc[] convolved with h1[])      *
     * - compute pitch gain2                                           *
     *-----------------------------------------------------------------*/

    /* find pitch excitation with lp filter */
    for (i = 0; i < L_SUBFR; i++)
    {
        Ltmp = L_mult(5898, exc[i - 1 + i_subfr]);
        Ltmp = L_mac(Ltmp, 20972, exc[i + i_subfr]);
        Ltmp = L_mac(Ltmp, 5898, exc[i + 1 + i_subfr]);
        code[i] = round(Ltmp);        move16();
    }

    Convolve(code, h1, y2, L_SUBFR);
    gain2 = G_pitch(xn, y2, g_corr2, L_SUBFR);
    
    /* find energy of new target xn2[] */
    Updt_tar(xn, xn2, y2, gain2, L_SUBFR);
  
    Ltmp = 0;      move32();
    for (i=0; i<L_SUBFR; i++)
    {
      Ltmp = L_mac(Ltmp, dn[i],dn[i]);
    }
    for (i=0; i<L_SUBFR; i++)
    {
      Ltmp = L_msu(Ltmp, xn2[i],xn2[i]);
    }

    test();
    if(Ltmp > 0)
    {
      /* use the lp filter for pitch excitation prediction */
      select = 0;               move16();
      Copy(code, &exc[i_subfr], L_SUBFR);
      Copy(y2, y1, L_SUBFR);
      gain_pit = gain2;         move16();
      g_corr[0] = g_corr2[0];   move16();
      g_corr[1] = g_corr2[1];   move16();
      g_corr[2] = g_corr2[2];   move16();
      g_corr[3] = g_corr2[3];   move16();
    }
    else
    {
      /* no filter used for pitch excitation prediction */
      select = 1;           move16();
      gain_pit = gain1;     move16();
      Copy(dn, xn2, L_SUBFR);        /* target vector for codebook search */
    }
    *sprm = select;  sprm++;    move16();

    Updt_tar(xn, xn2, y1, gain_pit, L_SUBFR);
    Updt_tar(cn, cn, &exc[i_subfr], gain_pit, L_SUBFR);
    Scale_sig(cn, L_SUBFR, st->old_wsp_shift);     /* scaling of cn[] to limit dynamic at 12 bits */

    /*-----------------------------------------------------------------*
    * - include fixed-gain pitch contribution into impulse resp. h1[] *
    *-----------------------------------------------------------------*/

    tmp16 = 0;                           move16();
    Preemph(h2, TILT_CODE_FX, L_SUBFR, &tmp16);

    test();
    if (sub(T0_frac,2) > 0)
      T0 = add(T0, 1);
    Pit_shrp(h2, T0, PIT_SHARP_FX, L_SUBFR);

    /*-----------------------------------------------------------------*
    * - Correlation between target xn2[] and impulse response h1[]    *
    * - Innovative codebook search                                    *
    *-----------------------------------------------------------------*/

    cor_h_x(h2, xn2, dn);

    test();test();test();test();
    test();test();test();test();
    if (sub(codec_mode,MODE_9k6) == 0) 
    {
      ACELP_4t64_fx(dn, cn, h2, code, y2, 20, 0, sprm);
      sprm += 4;
    } 
    else if (sub(codec_mode,MODE_11k2) == 0) 
    {
      ACELP_4t64_fx(dn, cn, h2, code, y2, 28, 0, sprm);
      sprm += 4;
    } 
    else if (sub(codec_mode,MODE_12k8) == 0) 
    {
      ACELP_4t64_fx(dn, cn, h2, code, y2, 36, 0, sprm);
      sprm += 4;
    } 
    else if (sub(codec_mode,MODE_14k4) == 0) 
    {
      ACELP_4t64_fx(dn, cn, h2, code, y2, 44, 0, sprm);
      sprm += 4;
    } 
    else if (sub(codec_mode,MODE_16k) == 0) 
    {
      ACELP_4t64_fx(dn, cn, h2, code, y2, 52, 0, sprm);
      sprm += 4;
    } 
    else if (sub(codec_mode,MODE_18k4) == 0) 
    {
      ACELP_4t64_fx(dn, cn, h2, code, y2, 64, 0, sprm);
      sprm += 8;
    } 
    else if (sub(codec_mode,MODE_20k) == 0) 
    {
      ACELP_4t64_fx(dn, cn, h2, code, y2, 72, 0, sprm);
      sprm += 8;
    } 
    else if (sub(codec_mode,MODE_23k2) == 0) 
    {
      ACELP_4t64_fx(dn, cn, h2, code, y2, 88, 0, sprm);
      sprm += 8;
    } 
    else 
    {
      AMRWBPLUS_PRINT("invalid mode for acelp frame!\n");
      exit(0);
    }

    /*-------------------------------------------------------*
     * - Add the fixed-gain pitch contribution to code[].    *
     *-------------------------------------------------------*/

    tmp16 = 0;                           move16();
    Preemph(code, TILT_CODE_FX, L_SUBFR, &tmp16);
    Pit_shrp(code, T0, PIT_SHARP_FX, L_SUBFR);

   /*----------------------------------------------------------*
    *  - Compute the fixed codebook gain                       *
    *  - quantize fixed codebook gain                          *
    *----------------------------------------------------------*/
    index = Q_gain2_plus(code, xn, y1, y2,  L_SUBFR, &gain_pit, &gain_code, g_corr, mean_ener_code_dB, &c_out_[i_subfr/L_SUBFR], add(add(st->Q_sp,st->old_wsp_shift),1));
    /* store pitch-lag for high band coding */
    p_out[i_subfr/L_SUBFR] = gain_pit;	move16();

    *sprm = index; sprm++;              move16();

    Ltmp = L_shl(gain_code, st->Q_sp); /* saturation can occur here */
    g_code = round(Ltmp);          /* scaled gain_code with Qnew */

    /*----------------------------------------------------------*
     * Update parameters for the next subframe.                 *
     * - tilt of code: 0.0 (unvoiced) to 0.5 (voiced)           *
     *----------------------------------------------------------*/

    /*VOICE_FAC NOT NEEDED */
        /* find voice factor in Q15 (1=voiced, -1=unvoiced) 
          
            Copy(&exc[i_subfr], exc2, L_SUBFR);
            Scale_sig(exc2, L_SUBFR, st->old_wsp_shift);
            voice_fac = voice_factor(exc2, st->old_wsp_shift, gain_pit, code, g_code, L_SUBFR);
        */

   /*------------------------------------------------------*
    * - Update filter's memory "mem_w0" for finding the    *
    *   target vector in the next subframe.                *
    * - Find the total excitation                          *
    * - Find synthesis speech to update mem_syn[].         *
    *------------------------------------------------------*/
    /* y2 in Q9, gain_pit in Q14 */
    Ltmp = L_mult(g_code, y2[L_SUBFR - 1]);
    Ltmp = L_shl(Ltmp, add(5, st->old_wsp_shift));
    Ltmp = L_mac(Ltmp, y1[L_SUBFR - 1], gain_pit);
    Ltmp = L_shl(Ltmp, sub(1, st->old_wsp_shift));
    *mem_wsyn_ = round(Ltmp);         move16();

    for (i = 0; i < L_SUBFR; i++)
    {
        /* code in Q9, gain_pit in Q14; exc Q_new+1 */
        Ltmp = L_mult(g_code, code[i]);
        Ltmp = L_shl(Ltmp, 6);
        Ltmp = L_mac(Ltmp, exc[i + i_subfr], gain_pit);
        Ltmp = L_shl(Ltmp, 1);       /* saturation can occur here */
        exc[i + i_subfr] = round(Ltmp);    move16();
    }
    Syn_filt(p_Aq, M, &exc[i_subfr], &synth_[i_subfr], L_SUBFR, &synth_[i_subfr-M], 0);
    p_A += (M+1);
    p_Aq += (M+1);

  } /* end of subframe loop */

  /*----------------------------------------------------------*
  * find 10ms ZIR in weighted domain for next tcx frame      *
  *----------------------------------------------------------*/

  Weight_a(p_Aq, Ap, GAMMA1_FX, M);      /* wAi of tcx is quantized */	

  Copy(&synth_[lg-M], error, M);
  tmp16 = *mem_wsyn_;       move16();

  for (i_subfr=0; i_subfr<(2*L_SUBFR); i_subfr+=L_SUBFR) 
  {
    Set_zero(&error[M], L_SUBFR);
    Syn_filt(p_Aq, M, &error[M], &error[M], L_SUBFR, error, 0);
    Residu(Ap, M, &error[M], &wovlp_[i_subfr], L_SUBFR);
    Deemph2(&wovlp_[i_subfr], TILT_FAC_FX, L_SUBFR, &tmp16);
    Copy(&error[L_SUBFR], error, M);
  }
  for (i=1; i<L_SUBFR; i++) 
  {
    wovlp_[L_SUBFR+i] = mult_r(wovlp_[L_SUBFR+i], Ovelap_wind[L_SUBFR-1-i]);
  }

  st->Q_exc = st->Q_sp;   move16();
  return;
}


