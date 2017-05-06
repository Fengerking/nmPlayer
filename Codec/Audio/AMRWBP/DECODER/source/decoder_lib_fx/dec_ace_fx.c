#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "math_op.h"
#include "oper_32b.h"

//#include "stdlib.h"


void Decoder_acelp(
  Word16 prm[],         /* (i): parameters                   */
  Word16 lg,            /* (i): frame length                 */
  Word16 codec_mode,    /* (i): AMR-WB+ mode (see cnst.h)    */
  Word16 bfi,           /* (i): 1=bad frame                  */
  Word16 *pT,           /* (o): pitch for all subframe       */
  Word16 *pgainT,       /* (o): pitch gain for all subfr     */
  Word16 Az[],          /* (i): LPC parameters           Q12 */ 
  Word16 exc[],        /* (i): Excitation               Qexc*/
  Word16 synth[],
  Word16 pit_adj,
  Word16 len,
  Word16 stab_fac,
  Decoder_State_Plus_fx *st /* i/o :  coder memory state       */
)                       
{
#if (!FUNC_DECODER_ACELP_OPT)
  Word16 i, i_subfr, select, tmp16_2;
  Word16 T0, T0_frac, index, pit_flag, T0_min, T0_max;
  Word16 code[L_SUBFR];
  Word16 tmp16  =0;
  Word16 mean_ener_code;        /* Q8 ener in dB*/
  Word16 ener_e, ener_m, lg_m, lg_e;
  Word16 gain_pit16, *p_Az, Azp[1+M];
  Word32 Lgain_code;
  Word32 Lenerwsyn, L_tmp;
  Word16 gain_code16;
  Word16 max = 0;
  Word16 Scaling_update;
  Word16 buf[M+L_OVLP]; 
  Word16 tmp_buf[M];

  Word16 PIT_min;       /* Minimum pitch lag with resolution 1/4      */
  Word16 PIT_fr2;       /* Minimum pitch lag with resolution 1/2      */
  Word16 PIT_fr1;       /* Minimum pitch lag with resolution 1        */
  Word16 PIT_max;       /* Maximum pitch lag                          */
  Word16 exc2[L_DIV], code2[L_SUBFR], voice_fac;
  Word16 gain_code, gain_code_lo, fac;

  mean_ener_code = 0;   
  lg_e = 31-22;        
  lg_m = 16384;     
  
  
  if(pit_adj ==0) 
  {
    PIT_min = PIT_MIN_12k8;			  
    PIT_fr2 = PIT_FR2_12k8;			  
    PIT_fr1 = PIT_FR1_12k8;			  
    PIT_max = PIT_MAX_12k8;			  
  }
  else 
  {
      /*i = (((pit_adj*PIT_MIN_12k8)+(FSCALE_DENOM/2))/FSCALE_DENOM)-PIT_MIN_12k8;*/
    i  = st->i_offset;       /*already compute in dec_lf*/

    PIT_min = PIT_MIN_12k8 + i;			  
    PIT_fr2 = PIT_FR2_12k8 - i;			  
    PIT_fr1 = PIT_FR1_12k8;			      
    PIT_max = PIT_MAX_12k8 + (6*i);		
  }
  T0 = st->wold_T0;              
  T0_frac = st->wold_T0_frac;    
  T0_min = sub(T0, 8);

  /*------------------------------------------------------------------------*
   * - decode mean_ener_code for gain decoder (d_gain2.c)                   *
   *------------------------------------------------------------------------*/
  index = *prm++;   

  /* decode mean energy with 2 bits : 18, 30, 42 or 54 dB */
  if (!bfi)
  { /*index * 12.0f + 18.0f;*/
    L_tmp = L_mac(36,index, 12);
    mean_ener_code = extract_l(L_shl(L_tmp,8-1));  /*mean_ener_code Q8*/
  }



 /*------------------------------------------------------------------------*
  *          Loop for every subframe in the analysis frame                 *
  *------------------------------------------------------------------------*
  *  To find the pitch and innovation parameters. The subframe size is     *
  *  L_SUBFR and the loop is repeated L_ACELP/L_SUBFR times.               *
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

  Lenerwsyn = 1;    move32();

  p_Az = Az;  
  for (i_subfr = 0; i_subfr < lg; i_subfr += L_SUBFR)
  {
    pit_flag = i_subfr;     
    
    if (sub(i_subfr,2*L_SUBFR)  == 0)
    {
      pit_flag = 0;   
    }

    index = *prm++;     

    /*-------------------------------------------------*
     * - Decode pitch lag                              *
     *-------------------------------------------------*/
          
    if (bfi)                     /* if frame erasure */
    {
      /* Lag indices received also in case of BFI, so that 
         the parameter pointer stays in sync. */
                  
      st->wold_T0_frac = add(st->wold_T0_frac,1);          /* use last delay incremented by 1/4 */
      
      if (sub(st->wold_T0_frac,3) > 0)
      {
        st->wold_T0_frac = sub(st->wold_T0_frac,4);  
        st->wold_T0 = add(st->wold_T0,1);  
      }
                  
      if (sub(st->wold_T0, PIT_max) >= 0)
      {
        st->wold_T0 = sub(PIT_max,5);     
      }
      T0 = st->wold_T0;              
      T0_frac = st->wold_T0_frac;    
    }
    else
    {
       
      if (pit_flag == 0)
      {
          
        if (sub(index,shl(sub(PIT_fr2 ,PIT_min) ,2)) < 0)
        {
          T0 = add(PIT_min ,shr(index,2));      
          T0_frac = sub(index, shl(sub(T0 , PIT_min),2));   
        }
        else if (sub(index,add(shl(sub(PIT_fr2,PIT_min),2) , shl(sub(PIT_fr1,PIT_fr2),1))) < 0 )
        {
          index = sub(index, shl(sub(PIT_fr2,PIT_min),2));      
          T0 = add(PIT_fr2,shr(index,1));               
          T0_frac = sub(index, shl(sub(T0, PIT_fr2),1));    
          T0_frac = shl(T0_frac,1);                     
        }
        else 
        {
          T0 = add(index, sub(sub(PIT_fr1, shl(sub(PIT_fr2,PIT_min),2)), shl(sub(PIT_fr1,PIT_fr2),1)) );   
          T0_frac = 0;                                                                      
        }
                          
        /* find T0_min and T0_max for subframe 2 or 4 */
                          
        T0_min = sub(T0, 8);
        
        if (sub(T0_min,PIT_min) < 0)
        {
          T0_min = PIT_min;           
        }
        T0_max = add( T0_min, 15);
        
        if (sub(T0_max,PIT_max) > 0)
        {
          T0_max = PIT_max;             
          T0_min = sub(T0_max, 15);    
        }
      }
      else      /* if subframe 2 or 4 */
      {
        T0 = add(T0_min, shr(index,2));                
        T0_frac = sub(index, shl(sub(T0, T0_min),2));  
      }
    }

    /*-------------------------------------------------*
     * - Find the pitch gain, the interpolation filter *
     *   and the adaptive codebook vector.             *
     *-------------------------------------------------*/

    voAMRWBPDecPredlt4(&exc[i_subfr], T0, T0_frac, L_SUBFR+1);
    select = *prm++; 
    if (bfi)
    {
      select = 1;
    }     
    if (select == 0)
    {
      /* find pitch excitation with lp filter */
      /* Pitch fixed point */
      for(i = 0; i < L_SUBFR; i++)
      {
        L_tmp = L_mult(5898,exc[i - 1 + i_subfr]);
        L_tmp = L_mac(L_tmp, 20972, exc[i + i_subfr]);
        L_tmp = L_mac(L_tmp, 5898, exc[i + 1+ i_subfr]);
        code[i] = round16(L_tmp);
      }
      voAMRWBPDecCopy(code, &exc[i_subfr], L_SUBFR);
    }

    /*-------------------------------------------------------*
     * - Decode innovative codebook.                         *
     * - Add the fixed-gain pitch contribution to code[].    *
     *-------------------------------------------------------*/
    if(codec_mode == MODE_9k6)     
    {
      if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 20, code);
      prm+=4;
    }
    else if(codec_mode == MODE_11k2)
    {
      if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 28, code);
      prm+=4;
    }
    else if (sub(codec_mode,MODE_12k8) == 0)
    {
      
      if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 36, code);
      prm+=4;
    }
    else if (sub(codec_mode,MODE_14k4) == 0)
    {
      
      if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 44, code);
      prm+=4;
    }
    else if (sub(codec_mode,MODE_16k) == 0)
    {
      
      if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 52, code);
      prm+=4;
    }
    else if (sub(codec_mode ,MODE_18k4) == 0)   
    {
      
      if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 64, code);
      prm+=8;
    }
    else if (sub(codec_mode, MODE_20k) ==0)     
    {
      
      if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 72, code);
      prm+=8;
    }
    else if (sub(codec_mode,MODE_23k2) == 0)
    {
      
      if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 88, code);
      prm+=8;
    }

    if (bfi)
    {
      /* the innovative code doesn't need to be scaled (see D_gain2) */ 
      for (i=0; i<L_SUBFR; i++)
      {
// @shanrong modified
#ifdef MSVC_AMRWBPLUS
        code[i] = shr(Random(&(st->seed_ace)),3);   
#elif EVC_AMRWBPLUS
        code[i] = shr(Random_evc(&(st->seed_ace)),3);   
#endif
// end
      }
    }

    /*-------------------------------------------------------*
     * - Add the fixed-gain pitch contribution to code[].    *
     *-------------------------------------------------------*/
    tmp16 = 0;                                         
    voAMRWBPDecPreemph(code, TILT_CODE_FX, L_SUBFR, &tmp16);
    
    i = T0;                                            
    
    if (sub(T0_frac,2) > 0)
    {
      i  = add(i,1);                                    
    }

    voAMRWBPDecPit_shrp(code, i, PIT_SHARP_FX, L_SUBFR);
    /*-------------------------------------------------*
     * - Decode codebooks gains.                       *
     *-------------------------------------------------*/
    index = *prm++;                                     
    st->wmem_gain_code[shr(i_subfr,6)] = D_gain2_plus((Word16)index, code, L_SUBFR, &gain_pit16, &Lgain_code, bfi,
                  mean_ener_code, &(st->wpast_gpit), &(st->Lpast_gcode));    
    /*Scaling excitation */
    /* find best scaling to perform on excitation (Q_new) */
    Scaling_update = Scale_exc(&exc[i_subfr], L_SUBFR, Lgain_code, &(st->Q_exc),st->mem_subfr_q, len);

    gain_code16 = round16(L_shl(Lgain_code, st->Q_exc));
    /* End of scaling*/


    voAMRWBPDecCopy(&exc[i_subfr], &exc2[i_subfr], L_SUBFR);
    voAMRWBPDecScale_sig(&exc2[i_subfr], L_SUBFR, -3);
    voice_fac = voAMRWBPDecVfactor(&exc2[i_subfr], -3, gain_pit16, code, gain_code16, L_SUBFR);

    voAMRWBPDecCopy(&exc[i_subfr], &exc2[i_subfr], L_SUBFR);

    /*-------------------------------------------------------*
     * - Find the total excitation.                          *
     *   find maximum value of excitation for next scaling   *
     *-------------------------------------------------------*/
    max = 1;                           
    for(i = 0; i < L_SUBFR; i++)
    {
      L_tmp = L_shl(L_mult(code[i], gain_code16), 5);
      L_tmp = L_mac(L_tmp, exc[i + i_subfr], gain_pit16);
      exc[i + i_subfr] = round16(L_shl(L_tmp,1));      
      tmp16 = abs_s(exc[i + i_subfr]);
      
      if (sub(tmp16, max) > 0)
      {
        max = tmp16;                 
      }
    }
        
    /*-------------------------------------------------------*
     * - Output pitch parameters for bass post-filter        *
     *-------------------------------------------------------*/
    i = T0;                                              
    
    if (sub(T0_frac,2) > 0)
    {
      i = add(i,1);  
    }
    
    if (sub(i,PIT_max) > 0)
    {
      i = PIT_max;                                       
    }
    *pT = i;                                             
    *pgainT = gain_pit16 ;                               

    pT++;
    pgainT++;


    /*----------------------------------------------------------*
     * - compute the synthesis speech                           *
     *----------------------------------------------------------*/
    
    if(pit_adj>0)
    {
      /*------------------------------------------------------------*
       * noise enhancer                                             *
       * ~~~~~~~~~~~~~~                                             *
       * - Enhance excitation on noise. (modify gain of code)       *
       *   If signal is noisy and LPC filter is stable, move gain   *
       *   of code 1.5 dB toward gain of code threshold.            *
       *   This decrease by 3 dB noise energy variation.            *
       *------------------------------------------------------------*/
      voAMRWBPDecL_Extract(Lgain_code, &gain_code, &gain_code_lo);

      tmp16 = sub(16384, shr(voice_fac, 1));    /* 1=unvoiced, 0=voiced */
      fac = mult(stab_fac, tmp16);

      L_tmp = Lgain_code;               move32();
      
      if (L_sub(L_tmp, st->L_gc_thres) < 0)
      {
        L_tmp = L_add(L_tmp, Mpy_32_16(gain_code, gain_code_lo, 6226));
        
        if (L_sub(L_tmp, st->L_gc_thres) > 0)
        {
          L_tmp = st->L_gc_thres;    move32();
        }
      }
      else
      {
        L_tmp = Mpy_32_16(gain_code, gain_code_lo, 27536);
        
        if (L_sub(L_tmp, st->L_gc_thres) < 0)
        {
          L_tmp = st->L_gc_thres;    move32();
        }
      }
      st->L_gc_thres = L_tmp;            move32();

      Lgain_code = Mpy_32_16(gain_code, gain_code_lo, sub(32767, fac));
      voAMRWBPDecL_Extract(L_tmp, &gain_code, &gain_code_lo);
      Lgain_code = L_add(Lgain_code, Mpy_32_16(gain_code, gain_code_lo, fac));

      /*------------------------------------------------------------*
       * pitch enhancer                                             *
       * ~~~~~~~~~~~~~~                                             *
       * - Enhance excitation on voice. (HP filtering of code)      *
       *   On voiced signal, filtering of code by a smooth fir HP   *
       *   filter to decrease energy of code in low frequency.      *
       *------------------------------------------------------------*/

      tmp16 = add(shr(voice_fac, 3), 4096);/* 0.25=voiced, 0=unvoiced */

      L_tmp = L_deposit_h(code[0]);
      L_tmp = L_msu(L_tmp, code[1], tmp16);
      code2[0] = round16(L_tmp);
      

      for (i = 1; i < L_SUBFR - 1; i++)
      {
        L_tmp = L_deposit_h(code[i]);
        L_tmp = L_msu(L_tmp, code[i + 1], tmp16);
        L_tmp = L_msu(L_tmp, code[i - 1], tmp16);
        code2[i] = round16(L_tmp);
        
      }

      L_tmp = L_deposit_h(code[L_SUBFR - 1]);
      L_tmp = L_msu(L_tmp, code[L_SUBFR - 2], tmp16);
      code2[L_SUBFR - 1] = round16(L_tmp);
      

      /* build excitation */

      gain_code = round16(L_shl(Lgain_code, st->Q_exc));

      for (i = 0; i < L_SUBFR; i++)
      {
        L_tmp = L_mult(code2[i], gain_code);
        L_tmp = L_shl(L_tmp, 5);
        L_tmp = L_mac(L_tmp, exc2[i+i_subfr], gain_pit16);
        L_tmp = L_shl(L_tmp, 1);       /* saturation can occur here */
        exc2[i + i_subfr] = round16(L_tmp);

        
      }
      voAMRWBPDecScale_sig(exc2, i_subfr, Scaling_update);
    }
    p_Az += (M+1);
  }
  /* end of subframe loop */
    

  voAMRWBPDecCopy(synth - M, tmp_buf, M);
  Rescale_mem(tmp_buf, st);
  voAMRWBPDecScale_sig(&(st->wmem_wsyn), 1, sub(st->Q_syn, st->Old_Qxnq));
  p_Az = Az;
  for(i_subfr = 0; i_subfr < L_DIV; i_subfr+=L_SUBFR)
  {
    if (pit_adj == 0)
    {
      voAMRWBPDecSyn_filt_s(sub(st->Q_exc,st->Q_syn) ,p_Az, M, &exc[i_subfr], &synth[i_subfr], L_SUBFR, tmp_buf, 0);
      voAMRWBPDecCopy(synth + i_subfr + L_SUBFR - M, tmp_buf, M);
    }
    else
    {
      voAMRWBPDecSyn_filt_s(sub(st->Q_exc,st->Q_syn) ,p_Az, M, &exc2[i_subfr], &synth[i_subfr], L_SUBFR, tmp_buf, 0);
      voAMRWBPDecCopy(synth + i_subfr + L_SUBFR  - M, tmp_buf, M);

    }

    voAMRWBPDecWeight_a(p_Az, Azp, GAMMA1_FX, M);
    voAMRWBPDecResidu(Azp,M, &synth[i_subfr], code, L_SUBFR);
    voAMRWBPDecDeemph2(code, TILT_FAC_FX, L_SUBFR, &(st->wmem_wsyn)); /*if synth is in Q-1, code, and wmem_wsyn are in Q-2 */
    L_tmp = 0;   move32();
    tmp16 = shl(1,sub(12, st->Q_syn));
    for (i=0; i<L_SUBFR; i++) 
    {
      tmp16_2 = mult(code[i], tmp16);
      L_tmp  = L_mac(L_tmp, tmp16_2, tmp16_2);
    }
    Lenerwsyn = L_add(Lenerwsyn,L_tmp);

    p_Az += (M+1);
  }

  /* RMS of weighted synthesis (for frame recovery on TCX mode) */
  ener_e = norm_l(Lenerwsyn);
  ener_m = extract_h(L_shl(Lenerwsyn, ener_e));
  ener_e = sub(ener_e,(2*3)-1);
  ener_e = sub(31, ener_e);
  if(sub(lg_m,ener_m)>0)
  {
    lg_m = shr(lg_m,1);
    lg_e = sub(lg_e,1);
  }
  ener_m = div_s(lg_m, ener_m);
  ener_e = sub(lg_e, ener_e);
  Lenerwsyn = L_deposit_h(ener_m);
  voAMRWBPDecIsqrt_n(&Lenerwsyn, &ener_e);
  st->wwsyn_rms = extract_h(L_shr(Lenerwsyn, sub(31-16, ener_e)));  /* Q3 */
  st->Q_old = st->Q_exc;      
  st->Old_Qxnq = st->Q_syn;   




  /*----------------------------------------------------------*
   * find 10ms ZIR in weighted domain for next tcx frame      *
   *----------------------------------------------------------*/

  voAMRWBPDecCopy(&synth[lg-M], buf, M);
  Set_zero(buf+M, L_OVLP);
  voAMRWBPDecSynFilt(p_Az,M, buf+M, buf+M, L_OVLP, buf, 0);

  voAMRWBPDecWeight_a(p_Az, Azp, GAMMA1_FX, M);      /* wAi of tcx is quantized */
  voAMRWBPDecResidu(Azp,M, buf+M, st->wwovlp, L_OVLP);
  tmp16 = st->wmem_wsyn;        
  voAMRWBPDecDeemph2(st->wwovlp, TILT_FAC_FX, L_OVLP, &tmp16);

  for (i=1; i<(L_OVLP/2); i++) 
  {
    st->wwovlp[L_OVLP-i] = mult_r(st->wwovlp[L_OVLP-i], Ovelap_wind[i-1]);
  }
  st->ovlp_size = 0;       /* indicate ACELP frame to TCX */
  /* update pitch value for bfi procedure */
  st->wold_T0_frac = T0_frac;               
  st->wold_T0 = T0;                         

#else

  Word16 i, i_subfr, select;
  //Word16 tmp16_2;
  Word16 T0, T0_frac, index, pit_flag, T0_min, T0_max;
  Word16 code[L_SUBFR];
  Word16 tmp16  =0;
  Word16 mean_ener_code;        /* Q8 ener in dB*/
  Word16 ener_e, ener_m, lg_m, lg_e;
  Word16 gain_pit16, *p_Az, Azp[1+M];
  Word32 Lgain_code;
  Word32 Lenerwsyn, L_tmp;
  Word16 gain_code16;
  Word16 max = 0;
  Word16 Scaling_update;
  Word16 buf[M+L_OVLP]; 
  Word16 tmp_buf[M];

  Word16 PIT_min;       /* Minimum pitch lag with resolution 1/4      */
  Word16 PIT_fr2;       /* Minimum pitch lag with resolution 1/2      */
  Word16 PIT_fr1;       /* Minimum pitch lag with resolution 1        */
  Word16 PIT_max;       /* Maximum pitch lag                          */
  Word16 exc2[L_DIV], code2[L_SUBFR], voice_fac;
  Word16 gain_code, gain_code_lo, fac;

  mean_ener_code = 0;
  lg_e = 31-22;
  lg_m = 16384;
  
  if(pit_adj ==0) 
  {
    PIT_min = PIT_MIN_12k8;
    PIT_fr2 = PIT_FR2_12k8;
    PIT_fr1 = PIT_FR1_12k8;
    PIT_max = PIT_MAX_12k8;
  }
  else 
  {
      /*i = (((pit_adj*PIT_MIN_12k8)+(FSCALE_DENOM/2))/FSCALE_DENOM)-PIT_MIN_12k8;*/
    i  = st->i_offset;       /*already compute in dec_lf*/

    PIT_min = PIT_MIN_12k8 + i;
    PIT_fr2 = PIT_FR2_12k8 - i;
    PIT_fr1 = PIT_FR1_12k8;
    PIT_max = PIT_MAX_12k8 + (6*i);
  }
  T0 = st->wold_T0;
  T0_frac = st->wold_T0_frac;
  T0_min = T0 - 8;

  /*------------------------------------------------------------------------*
   * - decode mean_ener_code for gain decoder (d_gain2.c)                   *
   *------------------------------------------------------------------------*/
  index = *prm++;

  /* decode mean energy with 2 bits : 18, 30, 42 or 54 dB */
  if (!bfi)
  { /*index * 12.0f + 18.0f;*/
    L_tmp = 36 + ((index * 12) << 1);
    mean_ener_code = L_tmp << 7;  /*mean_ener_code Q8*/
  }



 /*------------------------------------------------------------------------*
  *          Loop for every subframe in the analysis frame                 *
  *------------------------------------------------------------------------*
  *  To find the pitch and innovation parameters. The subframe size is     *
  *  L_SUBFR and the loop is repeated L_ACELP/L_SUBFR times.               *
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

  Lenerwsyn = 1;

  p_Az = Az;
  for (i_subfr = 0; i_subfr < lg; i_subfr += L_SUBFR)
  {
    pit_flag = i_subfr;
    if (i_subfr == 2*L_SUBFR)
    {
      pit_flag = 0;
    }

    index = *prm++;

    /*-------------------------------------------------*
     * - Decode pitch lag                              *
     *-------------------------------------------------*/
    if (bfi)                     /* if frame erasure */
    {
      /* Lag indices received also in case of BFI, so that 
         the parameter pointer stays in sync. */
                  
      st->wold_T0_frac += 1;  /* use last delay incremented by 1/4 */
      if (st->wold_T0_frac > 3)
      {
        st->wold_T0_frac -= 4;
        st->wold_T0 += 1;
      }
      if (st->wold_T0 >= PIT_max)
      {
        st->wold_T0 = PIT_max - 5;
      }
      T0 = st->wold_T0;
      T0_frac = st->wold_T0_frac;
    }
    else
    {
      if (pit_flag == 0)
      {
        if (index < shl((PIT_fr2 - PIT_min) ,2))
        {
          T0 = PIT_min + (index >> 2);
          T0_frac = index - shl((T0 - PIT_min),2);
        }
        else if ((index - (((PIT_fr2 - PIT_min) << 2) + ((PIT_fr1 - PIT_fr2) << 1))) < 0 )
        {
          index -= (PIT_fr2 - PIT_min) << 2;
          T0 = PIT_fr2 + (index >> 1);               
          T0_frac = index - ((T0 - PIT_fr2) << 1);
          T0_frac <<= 1;
        }
        else 
        {
          T0 = index + ((PIT_fr1 - ((PIT_fr2 - PIT_min) << 2)) - ((PIT_fr1 - PIT_fr2) << 1));
          T0_frac = 0;
        }
                          
        /* find T0_min and T0_max for subframe 2 or 4 */
                          
        T0_min = T0 - 8;
        T0_min = (T0_min < PIT_min) ? PIT_min : T0_min;
        T0_max = T0_min + 15;
        if (T0_max > PIT_max)
        {
          T0_max = PIT_max;
          T0_min = T0_max - 15;
        }
      }
      else      /* if subframe 2 or 4 */
      {
        T0 = T0_min + (index >> 2);
        T0_frac = index - ((T0 - T0_min) << 2);  
      }
    }

    /*-------------------------------------------------*
     * - Find the pitch gain, the interpolation filter *
     *   and the adaptive codebook vector.             *
     *-------------------------------------------------*/
    voAMRWBPDecPredlt4(&exc[i_subfr], T0, T0_frac, L_SUBFR+1);
    select = *prm++;
    if (bfi)
    {
      select = 1;
    }
    if (select == 0)
    {
      /* find pitch excitation with lp filter */

      /* Pitch fixed point */
      for(i = 0; i < L_SUBFR; i++)
      {
        L_tmp = 5898 * exc[i - 1 + i_subfr];
        L_tmp += 20972 * exc[i + i_subfr];
        L_tmp += 5898 * exc[i + 1+ i_subfr];
        code[i] = (L_tmp + 0x4000L) >> 15;
      }
      voAMRWBPDecCopy(code, &exc[i_subfr], L_SUBFR);
    }

    /*-------------------------------------------------------*
     * - Decode innovative codebook.                         *
     * - Add the fixed-gain pitch contribution to code[].    *
     *-------------------------------------------------------*/
    
    switch(codec_mode)
    {
        case MODE_9k6:
            if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 20, code);
            prm+=4;
            break;

        case MODE_11k2:
            if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 28, code);
            prm+=4;            
            break;
            
        case MODE_12k8:
            if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 36, code);
            prm+=4;            
            break;
            
        case MODE_14k4:
            if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 44, code);
            prm+=4;
            break;

        case MODE_16k:
            if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 52, code);
            prm+=4;
            break;

        case MODE_18k4:
            if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 64, code);
            prm+=8;
            break;

        case MODE_20k:
            if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 72, code);
            prm+=8;
            break;

        case MODE_23k2:
            if (!bfi) voAMRWBPDecACELP_4t64_fx(prm, 88, code);
            prm+=8;
            break;

        default:
            //exit(102);
            break;
    }
    
    if (bfi)
    {
      /* the innovative code doesn't need to be scaled (see D_gain2) */ 
      for (i=0; i<L_SUBFR; i++)
      {
// @shanrong modified
#ifdef MSVC_AMRWBPLUS
        code[i] = Random(&(st->seed_ace)) >> 3;
#elif EVC_AMRWBPLUS
        code[i] = Random_evc(&(st->seed_ace)) >> 3;
#endif
// end
      }
    }

    /*-------------------------------------------------------*
     * - Add the fixed-gain pitch contribution to code[].    *
     *-------------------------------------------------------*/
    tmp16 = 0;
    voAMRWBPDecPreemph(code, TILT_CODE_FX, L_SUBFR, &tmp16);
    
    i = T0;
    if (T0_frac > 2)
    {
      i  += 1;
    }

    voAMRWBPDecPit_shrp(code, i, PIT_SHARP_FX, L_SUBFR);
    /*-------------------------------------------------*
     * - Decode codebooks gains.                       *
     *-------------------------------------------------*/
    index = *prm++;
    st->wmem_gain_code[(i_subfr >> 6)] = D_gain2_plus((Word16)index, code, L_SUBFR, &gain_pit16, &Lgain_code, bfi,
                  mean_ener_code, &(st->wpast_gpit), &(st->Lpast_gcode)); 
    /*Scaling excitation */
    /* find best scaling to perform on excitation (Q_new) */
    Scaling_update = Scale_exc(&exc[i_subfr], L_SUBFR, Lgain_code, &(st->Q_exc),st->mem_subfr_q, len);

    gain_code16 = (L_shl(Lgain_code, st->Q_exc) + 0x8000) >> 16;
    /* End of scaling*/


    voAMRWBPDecCopy(&exc[i_subfr], &exc2[i_subfr], L_SUBFR);
    //memcpy(&exc2[i_subfr], &exc[i_subfr], L_SUBFR * sizeof(Word16));
    voAMRWBPDecScale_sig(&exc2[i_subfr], L_SUBFR, -3);
    voice_fac = voAMRWBPDecVfactor(&exc2[i_subfr], -3, gain_pit16, code, gain_code16, L_SUBFR);

    voAMRWBPDecCopy(&exc[i_subfr], &exc2[i_subfr], L_SUBFR);
    //memcpy(&exc2[i_subfr], &exc[i_subfr], L_SUBFR*sizeof(Word16));

    /*-------------------------------------------------------*
     * - Find the total excitation.                          *
     *   find maximum value of excitation for next scaling   *
     *-------------------------------------------------------*/
    max = 1;
    for(i = 0; i < L_SUBFR; i++)
    {
      L_tmp = (code[i] * gain_code16) << 5;
      L_tmp += exc[i + i_subfr] * gain_pit16;
      exc[i + i_subfr] = ((L_tmp << 1) + 0x4000) >> 15;
      tmp16 = abs_s(exc[i + i_subfr]);
      max = (tmp16 > max) ? tmp16 : max;
    }
        
    /*-------------------------------------------------------*
     * - Output pitch parameters for bass post-filter        *
     *-------------------------------------------------------*/
    i = T0;
    if (T0_frac > 2)
    {
      i++;
    }
    if (i > PIT_max)
    {
      i = PIT_max;
    }
    *pT = i;
    *pgainT = gain_pit16 ;

    pT++;
    pgainT++;


    /*----------------------------------------------------------*
     * - compute the synthesis speech                           *
     *----------------------------------------------------------*/
    if(pit_adj>0)
    {
      /*------------------------------------------------------------*
       * noise enhancer                                             *
       * ~~~~~~~~~~~~~~                                             *
       * - Enhance excitation on noise. (modify gain of code)       *
       *   If signal is noisy and LPC filter is stable, move gain   *
       *   of code 1.5 dB toward gain of code threshold.            *
       *   This decrease by 3 dB noise energy variation.            *
       *------------------------------------------------------------*/
      voAMRWBPDecL_Extract(Lgain_code, &gain_code, &gain_code_lo);

      tmp16 = 16384 - (voice_fac >> 1);    /* 1=unvoiced, 0=voiced */
      fac = (stab_fac * tmp16) >> 15;

      L_tmp = Lgain_code;
      if (L_tmp < st->L_gc_thres)
      {
        L_tmp += Mpy_32_16(gain_code, gain_code_lo, 6226);
        if (L_tmp > st->L_gc_thres)
        {
          L_tmp = st->L_gc_thres;
        }
      }
      else
      {
        L_tmp = Mpy_32_16(gain_code, gain_code_lo, 27536);
        if (L_tmp < st->L_gc_thres)
        {
          L_tmp = st->L_gc_thres;
        }
      }
      st->L_gc_thres = L_tmp;

      Lgain_code = Mpy_32_16(gain_code, gain_code_lo, sub(32767, fac));
      voAMRWBPDecL_Extract(L_tmp, &gain_code, &gain_code_lo);
      Lgain_code += Mpy_32_16(gain_code, gain_code_lo, fac);

      /*------------------------------------------------------------*
       * pitch enhancer                                             *
       * ~~~~~~~~~~~~~~                                             *
       * - Enhance excitation on voice. (HP filtering of code)      *
       *   On voiced signal, filtering of code by a smooth fir HP   *
       *   filter to decrease energy of code in low frequency.      *
       *------------------------------------------------------------*/

      tmp16 = (voice_fac >> 3) + 4096;/* 0.25=voiced, 0=unvoiced */

      L_tmp = L_deposit_h(code[0]);
      L_tmp -= (code[1] * tmp16) << 1;
      code2[0] = (L_tmp + 0x8000) >> 16;
      
      for (i = 1; i < L_SUBFR - 1; i++)
      {
        L_tmp = L_deposit_h(code[i]);
        L_tmp -= (code[i + 1] * tmp16) << 1;
        L_tmp -= (code[i - 1] * tmp16) << 1;
        code2[i] = (L_tmp + 0x8000) >> 16;
      }

      L_tmp = L_deposit_h(code[L_SUBFR - 1]);
      L_tmp -= (code[L_SUBFR - 2] * tmp16) << 1;
      code2[L_SUBFR - 1] = (L_tmp + 0x8000) >> 16;

      /* build excitation */

      gain_code = (L_shl(Lgain_code, st->Q_exc) + 0x8000) >> 16;

      for (i = 0; i < L_SUBFR; i++)
      {
        L_tmp = (code2[i] * gain_code) << 7;
        L_tmp += (exc2[i+i_subfr] * gain_pit16) << 2;
        exc2[i + i_subfr] = (L_tmp + 0x8000) >> 16;
      }
      voAMRWBPDecScale_sig(exc2, i_subfr, Scaling_update);
    }
    p_Az += (M+1);
  }
  /* end of subframe loop */
    

  voAMRWBPDecCopy(synth - M, tmp_buf, M);
  Rescale_mem(tmp_buf, st);
  voAMRWBPDecScale_sig(&(st->wmem_wsyn), 1, sub(st->Q_syn, st->Old_Qxnq));
  p_Az = Az;
  for(i_subfr = 0; i_subfr < L_DIV; i_subfr+=L_SUBFR)
  {
    if (pit_adj == 0)
    {
      voAMRWBPDecSyn_filt_s(sub(st->Q_exc,st->Q_syn) ,p_Az, M, &exc[i_subfr], &synth[i_subfr], L_SUBFR, tmp_buf, 0);
      voAMRWBPDecCopy(synth + i_subfr + L_SUBFR - M, tmp_buf, M);
    }
    else
    {
      voAMRWBPDecSyn_filt_s(sub(st->Q_exc,st->Q_syn) ,p_Az, M, &exc2[i_subfr], &synth[i_subfr], L_SUBFR, tmp_buf, 0);
      voAMRWBPDecCopy(synth + i_subfr + L_SUBFR  - M, tmp_buf, M);

    }

    voAMRWBPDecWeight_a(p_Az, Azp, GAMMA1_FX, M);
    voAMRWBPDecResidu(Azp,M, &synth[i_subfr], code, L_SUBFR);
    voAMRWBPDecDeemph2(code, TILT_FAC_FX, L_SUBFR, &(st->wmem_wsyn)); //if synth is in Q-1, code, and wmem_wsyn are in Q-2
  /*
    L_tmp = 0;
    tmp16 = shl(1,(12 - st->Q_syn));
    for (i=0; i<L_SUBFR; i++) 
    {
      tmp16_2 = (code[i] * tmp16) >> 15;
      L_tmp  += tmp16_2 * tmp16_2;
    }
    Lenerwsyn += L_tmp << 1;
  */
  //shanrong ??? why it can be removed?
    
    p_Az += (M+1);
  }

  /* RMS of weighted synthesis (for frame recovery on TCX mode) */
  ener_e = norm_l(Lenerwsyn);
  ener_m = extract_h(L_shl(Lenerwsyn, ener_e));
  ener_e -= 5;
  ener_e = 31 - ener_e;
  if(lg_m > ener_m)
  {
    lg_m >>= 1;
    lg_e--;
  }
  ener_m = div_s(lg_m, ener_m);
  ener_e = lg_e - ener_e;
  Lenerwsyn = L_deposit_h(ener_m);
  voAMRWBPDecIsqrt_n(&Lenerwsyn, &ener_e);
  st->wwsyn_rms = extract_h(L_shr(Lenerwsyn, sub(31-16, ener_e)));  /* Q3 */
  st->Q_old = st->Q_exc;
  st->Old_Qxnq = st->Q_syn;




  /*----------------------------------------------------------*
   * find 10ms ZIR in weighted domain for next tcx frame      *
   *----------------------------------------------------------*/

  voAMRWBPDecCopy(&synth[lg-M], buf, M);
  Set_zero(buf+M, L_OVLP);
  voAMRWBPDecSynFilt(p_Az,M, buf+M, buf+M, L_OVLP, buf, 0);

  voAMRWBPDecWeight_a(p_Az, Azp, GAMMA1_FX, M);      /* wAi of tcx is quantized */
  voAMRWBPDecResidu(Azp,M, buf+M, st->wwovlp, L_OVLP);
  tmp16 = st->wmem_wsyn;
  voAMRWBPDecDeemph2(st->wwovlp, TILT_FAC_FX, L_OVLP, &tmp16);

  for (i=1; i<(L_OVLP/2); i++) 
  {
    st->wwovlp[L_OVLP-i] = (st->wwovlp[L_OVLP-i] * Ovelap_wind[i-1] + 0x4000) >> 15;
  }
  st->ovlp_size = 0;/* indicate ACELP frame to TCX */
  /* update pitch value for bfi procedure */
  st->wold_T0_frac = T0_frac;
  st->wold_T0 = T0;
#endif

  return;
}



