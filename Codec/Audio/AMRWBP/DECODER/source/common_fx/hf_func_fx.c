#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "oper_32b.h"
#include "log2.h"
#include "count.h"



/*---------------------------------------------------------------------*
 * routine match_gain_6k4()                                            *
 * ~~~~~~~~~~~~~~~~~~~~~~~~                                            *
 * Find the gain to match amplitude of both filters at 6.4kHz.         *
 * Gain is returned in log domain.                                     *
 *---------------------------------------------------------------------*/

Word16 Match_gain_6k4(  /* (o): Matching gain             Q8  */
  Word16 *AqLF,         /* (i): Low frequency lpc coeff   Q12 */
  Word16 *AqHF          /* (i): high frequency lpc coeff  Q12 */  
)
{
  Word32 Ltmp;
  Word16 buf[M+L_SUBFR], tmp16, max, scale;
  Word16 code[L_SUBFR], exp_g, frac;
  Word16 i, gain16;

  /* generate 1/(1+0.9 z^{-1}) over one subframe (HF signal close to a sinusoid at 6.4 kHz) */
  Set_zero(buf, M);
  tmp16 = 128;                 /* <7bits*/
 
  
  for (i=0; i<L_SUBFR; i++)
  {
    buf[i+M] = tmp16;         
    tmp16 = mult_r(tmp16, -29490);
  }

  /* apply A(z) from lower-band and 1/A(z) from upper-band */
  voAMRWBPDecResidu(AqLF, M, buf+M, code, L_SUBFR);
  voAMRWBPDecSynFilt(AqHF, MHF, code, code, L_SUBFR, buf, 0);

  /* compute energy and correction scale factor */

  max= 0; 
  for (i = 0;i < L_SUBFR; i++)
  { 
    tmp16= abs_s(code[i]);
    if(tmp16 > max)
    {
      max = tmp16;
    }
  }

  scale = norm_s(max) - 3; 
  voAMRWBPDecScale_sig(code, L_SUBFR, scale);

  Ltmp = voAMRWBPDecDot_product12(code,code, L_SUBFR, &exp_g);
  /*gain = 10.0f * log10(1.0f / tmp);*/
  voAMRWBPDecLog2_norm(Ltmp,exp_g, &tmp16, &frac);
  exp_g = sub(30-1, add(tmp16, shl(add(scale,7),1)));
    
  Ltmp    = Mpy_32_16(exp_g, frac, LG10);  
  gain16 = extract_l(L_shr(Ltmp, 14-8));  /* gain16 Q8 */

  return(negate(gain16));
}

/*---------------------------------------------------------------------*
 * routine int_gain()                                                  *
 * ~~~~~~~~~~~~~~~~~~                                                  *
 * Find the interpolated gain parameters in every subframes.           *
 *---------------------------------------------------------------------*/

void Int_gain(
  Word16 old_gain,    /* (i)  : Old gain              Q8 */
  Word16 new_gain,    /* (i)  : New gain              Q8 */
  const Word16 *Int_wind,   /* (i)  : Interpolation window  Q15*/
  Word16 *gain,       /* (o)  : Interpolated  gains   Q8 */
  Word16 nb_subfr     /* (i)  : Nb subfr                 */ 
)
{
  Word16 fold;
  Word16 k;
  const Word16 *fnew;
  Word32 Ltmp;
  
  fnew = Int_wind;          
									 
  for (k=0; k<nb_subfr; k++)
  {
    fold = sub(32767, *fnew);

    /* gain = (old_gain*fold) + (new_gain*fnew);*/
    Ltmp = L_mult(old_gain, fold);
    Ltmp = L_mac(Ltmp, new_gain, *fnew);
    *gain = round16(Ltmp); 

    fnew ++;
    gain++;                /* ptr*/
  }

  return;
}


/*---------------------------------------------------------------------*
 * routine soft_exc_hf()                                               *
 * ~~~~~~~~~~~~~~~~~~~~~                                               *
 * reduce buzziness from excitation.                                   *
 *---------------------------------------------------------------------*/

void Soft_exc_hf(
  Word16 *exc_hf,     /* (i/o)  : High frequency excitation     */
  Word32 *mem         /* (i/o)  : Memory                    Q16 */ 
)
{
#if (FUNC_SOFT_EXC_HF_OPT)

  Word32 lp_amp, Ltmp, Ltmp1;    /*Q16*/
  Word16 i, hi, lo;
  
  lp_amp = *mem;

  for (i=0; i<L_SUBFR; i++)
  {
    Ltmp = abs_s(exc_hf[i]) << 15;
    
    /*lp_amp = 0.98f*lp_amp + 0.02f*tmp;*/
    voAMRWBPDecL_Extract(Ltmp, &hi, &lo);
    Ltmp1 = Mpy_32_16(hi, lo, 655);   /*Q16*/
    
    voAMRWBPDecL_Extract(lp_amp, &hi, &lo);
    lp_amp = Ltmp1 + Mpy_32_16(hi, lo, 32112);

    /*tmp = tmp - 2.0f*lp_amp;      */
    Ltmp -= lp_amp << 1;

    if (Ltmp <= 0) 
    {
      Ltmp = 0;
    }
    else
    {
      lp_amp += Ltmp >> 1;
      if (exc_hf[i] >= 0)
      {
        exc_hf[i] -= ((Ltmp + 0x00008000L) >> 16) << 1;
      }
      else
      {
        exc_hf[i] += ((Ltmp + 0x00008000L) >> 16) << 1;
      }
    }
  }

  *mem = lp_amp;
  return;

#else

  Word32 lp_amp, Ltmp, Ltmp1;    /*Q16*/
  Word16 i, hi, lo;
  
  lp_amp = *mem;      move32();

  for (i=0; i<L_SUBFR; i++)
  {
    
    Ltmp = L_shl(abs_s(exc_hf[i]),16-1);/* clearing sign bit*/
    
    /*lp_amp = 0.98f*lp_amp + 0.02f*tmp;*/
    voAMRWBPDecL_Extract(Ltmp, &hi, &lo);
    Ltmp1 = Mpy_32_16(hi, lo, 655);   /*Q16*/
    
    voAMRWBPDecL_Extract(lp_amp, &hi, &lo);
    lp_amp = L_add(Ltmp1, Mpy_32_16(hi, lo, 32112));  /*Q16*/

    /*tmp = tmp - 2.0f*lp_amp;      */
    Ltmp = L_sub(Ltmp, L_shl(lp_amp,1));
    
    if (Ltmp <= 0) 
    {
      Ltmp = 0;         move32();
    }
    else
    {
      lp_amp = L_add(lp_amp, L_shr(Ltmp,1));
      
      if (exc_hf[i] >= 0)
      {
        exc_hf[i]= sub(exc_hf[i], shl(round16(Ltmp),1));    
      }
      else
      {
        exc_hf[i] = add(exc_hf[i], shl(round16(Ltmp), 1));   
      }
    }
  }

  *mem = lp_amp;    move32();
  return;
#endif
}

/*---------------------------------------------------------------------*
 * routine smooth_ener_hf()                                            *
 * ~~~~~~~~~~~~~~~~~~~~~~~~                                            *
 * smooth energy evolution of HF synthesis subframe.                   *
 *---------------------------------------------------------------------*/

void Smooth_ener_hf(
  Word16 *HF,         /* (i/o)  : HF synthesis    Qsyn+1  */
  Word32 *threshold   /* (i/o)  : Thresold memory         */
)
{
#if (FUNC_SMOOTH_ENER_HF_OPT)

  Word32 Ltmp, Lener;
  Word16 i, HF_tmp[L_SUBFR], hi, lo;
  Word16 exp_ener, exp_tmp, frac_ener, frac_tmp;
 
  voAMRWBPDecCopy(HF, HF_tmp, L_SUBFR);
  voAMRWBPDecScale_sig(HF_tmp, L_SUBFR, -2);     /* <14 bits -> <12 bits */

  Lener = 0;
  for (i=0; i<L_SUBFR; i++) 
  {
    Lener += HF_tmp[i] * HF_tmp[i];
  }
  Lener <<= 1;
  Lener++;
  
  /* if energy<threshold, add 1.5 dB and saturate to threshold
     else substract 1.5 dB and saturate to threshold */
  Ltmp = Lener;

  if (Ltmp < *threshold)
  {
    /*tmp = tmp*1.414f;*/
    voAMRWBPDecL_Extract(Ltmp, &hi, &lo);
    Ltmp = Mpy_32_16(hi, lo, 23169) << 1;
    if (Ltmp > *threshold)
    {
      Ltmp = *threshold;
    }
  }
  else
  {
    /*tmp = tmp/1.414f;*/
    voAMRWBPDecL_Extract(Ltmp, &hi, &lo);
    Ltmp = Mpy_32_16(hi, lo, 23174);
    if (Ltmp < *threshold)
    {
      Ltmp = *threshold;
    }
  }
  /* set the threshold for next subframer to the current modified energy */
  *threshold = Ltmp;

  if(Ltmp == 0)
  {
    Ltmp = 1;
  }

  /* apply correction scale factor to HF signal */
  exp_ener = norm_l(Lener);
  frac_ener = L_shl(Lener, exp_ener) >> 16;
  
  exp_tmp= norm_l(Ltmp);
  frac_tmp= L_shl(Ltmp, exp_tmp) >> 16;

  if(frac_ener > frac_tmp)
  {
    frac_ener >>= 1;
    exp_ener--;
  }

  frac_tmp = div_s(frac_ener, frac_tmp);
  exp_tmp = exp_ener - exp_tmp;

  /*rac_tmp = shr(frac_tmp, exp_tmp);    Q15*/
  exp_ener = exp_tmp - 1;             /* Output frac_tmp in Q14*/ 
  Ltmp = L_deposit_h(frac_tmp);
  voAMRWBPDecIsqrt_n(&Ltmp, &exp_tmp);

  frac_tmp = (Ltmp + 0x8000) >> 16;
  exp_tmp += exp_ener + 1;

  for (i=0; i<L_SUBFR; i++) 
  {
    Ltmp  = HF[i] * frac_tmp;
    HF[i] = (L_shl(Ltmp, (exp_tmp + 1)) + 0x8000) >> 16;
  }
  return;
  
#else

  Word32 Ltmp, Lener;
  Word16 i, HF_tmp[L_SUBFR], hi, lo;
  Word16 exp_ener, exp_tmp, frac_ener, frac_tmp;
 
  voAMRWBPDecCopy(HF, HF_tmp, L_SUBFR);
  voAMRWBPDecScale_sig(HF_tmp, L_SUBFR, -2);     /* <14 bits -> <12 bits */

  Lener = 1;        move32();
  for (i=0; i<L_SUBFR; i++) 
  {
    Lener = L_mac(Lener, HF_tmp[i], HF_tmp[i]);
  }
  
  /* if energy<threshold, add 1.5 dB and saturate to threshold
     else substract 1.5 dB and saturate to threshold */
  Ltmp = Lener;       move32();

  
  if (L_sub(Ltmp,*threshold) < 0) 
  {
    /*tmp = tmp*1.414f;*/
    voAMRWBPDecL_Extract(Ltmp, &hi, &lo);
    Ltmp = L_shl(Mpy_32_16(hi, lo, 23169),1);
    
    if (L_sub(Ltmp,*threshold) > 0)
    {
      Ltmp = *threshold;      move32();
    }
  }
  else
  {
    /*tmp = tmp/1.414f;*/
    voAMRWBPDecL_Extract(Ltmp, &hi, &lo);
    Ltmp = Mpy_32_16(hi, lo, 23174);
    
    if (L_sub(Ltmp,*threshold) < 0) 
    {
      Ltmp = *threshold;      move32();
    }
  }
  /* set the threshold for next subframer to the current modified energy */
  *threshold = Ltmp;      move32();

  
  if(Ltmp == 0)
  {
    Ltmp = 1;       move32();
  }

  /* apply correction scale factor to HF signal */
  exp_ener = norm_l(Lener);
  frac_ener = extract_h(L_shl(Lener, exp_ener));
  
  exp_tmp= norm_l(Ltmp);
  frac_tmp= extract_h(L_shl(Ltmp, exp_tmp));

  
  if(sub(frac_ener, frac_tmp)>0)
  {
    frac_ener =  shr(frac_ener,1);
    exp_ener = sub(exp_ener,1);
  }

  frac_tmp = div_s(frac_ener, frac_tmp);
  exp_tmp = sub(exp_ener, exp_tmp);

  /*rac_tmp = shr(frac_tmp, exp_tmp);    Q15*/
  exp_ener = sub(exp_tmp,1);             /* Output frac_tmp in Q14*/ 
  Ltmp = L_deposit_h(frac_tmp);
  voAMRWBPDecIsqrt_n(&Ltmp, &exp_tmp);

  frac_tmp = round16(Ltmp);
  exp_tmp = add(1,add(exp_tmp,exp_ener));

  for (i=0; i<L_SUBFR; i++) 
  {
    Ltmp  = L_mult(HF[i], frac_tmp);
    HF[i] = round16(L_shl(Ltmp, exp_tmp));        
  }
  return;
#endif
}
