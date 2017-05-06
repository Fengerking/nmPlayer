
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "math_op.h"

#include "amr_plus_fx.h"

/*---------------------------------------------------------------*
 * Adaptive low frequencies emphasis (for SV below 1600 hz).     *
 *                                                               *
 * To ensure that low frequencies is well quantized (otherwise   *
 * audible overlaps problem occurs), each subvector (RE8) is     *
 * is emphased relatively to the spectral peak under 1600Hz.     *
 *                                                               *
 * The subvector gain (dB) is:                                   *
 *    0.5 * min( spectral peak (dB) / local peak (dB), 40dB)     *
 * where                                                         *  
 *    spectral peak = peak between 0 Hz and 1600 Hz.             *
 *    local peak = peak between 0Hz to the subvector.            *
 *                                                               *
 * The gain is never over 20dB at the beginning and decrease to  * 
 * 0 dB somewhere between 0 and 1600 Hz.                         *
 *---------------------------------------------------------------*/

void Adap_low_freq_emph(Word16 xri[], Word16 lg)
{
  Word16 i, j,lg4, i8;
  Word16 m_max, fac, tmp16, e_max, e_tmp, e_maxtmp, max ;
  Word32 Ltmp;
 /*---------------------------------------------------------------*
  * Find spectral peak under 1600Hz (lg = 6400Hz)                 *
  * (find maximum of energy of all Re8 subvector under 1600Hz)    *
  *---------------------------------------------------------------*/

  lg4  = shr(lg,2);
  
  m_max = extract_h(SpPeak1k6(xri, &e_max, lg4));  
  
 /*---------------------------------------------------------------*
  * Emphasis of all subvector below 1600 Hz.                      *
  *---------------------------------------------------------------*/
  
  fac = 20480;       /*Q11 fac == 10 */
  for(i=0; i<lg4; i+=8)	
  {

    max = m_max; 
    e_maxtmp = e_max; 

    Ltmp = 1;     move32();
    i8 = add(i,8);
    for(j=i; j < i8; j++) 
      Ltmp = L_mac(Ltmp, xri[j], xri[j]);
    
    /*Ltmp = (float)sqrt(Ltmp)  -> (voAMRWBPDecIsqrt)*/
    e_tmp = norm_l(Ltmp);
    Ltmp= L_shl(Ltmp, e_tmp);
    e_tmp = sub(30,e_tmp);
    voAMRWBPDecIsqrt_n(&Ltmp, &e_tmp);

    tmp16 = extract_h(Ltmp);

    
    if(sub(max, tmp16)>0)
    {
      max = shr(max,1);
      e_maxtmp = add(e_maxtmp,1);
    }

    tmp16 = div_s(max, tmp16);
    e_tmp = sub(e_maxtmp, e_tmp);

    /* tmp = sqrt(max/tmp); -> tmp = voAMRWBPDecIsqrt(tmp/max)*/						
    Ltmp = L_deposit_h(tmp16);
    voAMRWBPDecIsqrt_n(&Ltmp,&e_tmp);  
  
    tmp16 = extract_h(L_shl(Ltmp, sub(e_tmp,4)));  /*Q11*/  /* Saturation can occu here*/
      
    if (sub(tmp16,fac) < 0) 
    {
      fac = tmp16;    
    }
    i8 = add(i,8);
    for(j=i; j<i8; j++) 
    {
      Ltmp = L_mult(xri[j], fac);
      xri[j] = round16(L_shl(Ltmp, 4));     
    }
  }

  return;
}

/*---------------------------------------------------------------*
 * Adaptive low frequencies weak deemphasis (for SV below 1600hz)*
 *                                                               *
 * To ensure that low frequencies is well quantized (otherwise   *
 * audible overlaps problem occurs), each subvector (RE8) was    *
 * emphased relatively to the spectral peak under 1600Hz.        *
 *                                                               *
 * This routine is the inverse operation of low_freq_emph().     *
 *---------------------------------------------------------------*/

void Adap_low_freq_deemph(
  Word16 xri[],     /*(i/o) : ftt coeff  Qifft  */
  Word16 lg         /*(i)   : Number of coeff   */
)
{
  Word16 exp_m, max, lg4;
  

  /*---------------------------------------------------------------*
   * Find spectral peak under 1600Hz (lg = 6400Hz)                 *
   * (find maximum of energy of all Re8 subvector under 1600Hz)    *
   *---------------------------------------------------------------*/
  lg4  = shr(lg,2);
  max = extract_h(SpPeak1k6(xri, &exp_m, lg4));  
  /*---------------------------------------------------------------*
   * Deemphasis of all subvector below 1600 Hz.                    *
   *---------------------------------------------------------------*/
  Deemph1k6(xri, exp_m, max, lg4);

  return;
}

/*---------------------------------------------------------------*
 * Find spectral peak under 1600Hz (lg = 6400Hz)                 *
 * (find maximum of energy of all Re8 subvector under 1600Hz)    *
 *---------------------------------------------------------------*/
 
Word32 SpPeak1k6( /* (o)  : Return maximum of energy    Q31 */
  Word16 *xri,    /* (i)  : Fft coeff                   Qfft*/ 
  Word16 *exp_m,  /* (o)  : exponent of maximum of energy   */
  Word16 lg4      /* (i)  : Length                          */
)
{
  Word32 Lmax, Ltmp;
  Word16 i, j, i8;

  Lmax = 1;  move32();
  for(i=0; i<lg4; i+=8)
  {
    Ltmp = 1;   move32();
    i8 = add(i,8);
    for(j=i; j<i8; j++) Ltmp = L_mac(Ltmp, xri[j],xri[j]);
    
    if (L_sub(Ltmp,Lmax) > 0)
    {
      Lmax = Ltmp; move32();
    }
  }

  *exp_m = norm_l(Lmax);      
  Lmax = L_shl(Lmax, *exp_m);
  *exp_m = sub(30,*exp_m);    
  /* voAMRWBPDecIsqrt of energy */
  voAMRWBPDecIsqrt_n(&Lmax, exp_m);
    
  return Lmax;
}

/*---------------------------------------------------------------*
 * Deemphasis of all subvector below 1600 Hz.                    *
 *---------------------------------------------------------------*/

void Deemph1k6(
  Word16 *xri,    /* (i/o)  : Fft Coeff       Qfft            */
  Word16 e_max,   /* (i)    : exponent of maximum of energy   */
  Word16 m_max,   /* (i)    : Mantise of  maximum of energy   */
  Word16 lg4      /* (i)    : Length                          */
)
{
#if (FUNC_DEEMPH1K6_OPT)
  Word32 Ltmp;
  Word16 i, j, i8;
  Word16 exp_tmp, tmp16,fac;
  
  fac = 3277;
  for(i=0; i<lg4; i+=8)
  {
    Ltmp = 0;
    i8 = i + 8;
    for(j=i; j<i8; j++)
    {
        Ltmp += xri[j] * xri[j];
    }
    Ltmp <<= 1;

    /*float ftmp = (float)sqrt(Ltmp);
      tmp16 = tmp16/max;*/
    exp_tmp = norm_l(Ltmp);
    Ltmp= L_shl(Ltmp, exp_tmp);
    exp_tmp = 30 - exp_tmp;
    voAMRWBPDecIsqrt_n(&Ltmp, &exp_tmp);

    tmp16 = (Word16)(Ltmp >> 16);
    if(m_max > tmp16)
    {
      m_max >>= 1;
      e_max++;

    }
    tmp16 = div_s(m_max, tmp16);
    exp_tmp = e_max - exp_tmp;
    
    tmp16 = shl(tmp16, exp_tmp);    /*Q15 : can up to 32767 (saturation is possible if tmp == max) */
    
    if (tmp16 > fac)
    {
      fac = tmp16;
    }

    for(j=i; j<i8; j++)
    {
      xri[j] = (xri[j] * fac) >> 15;
    }
  }
  return;
  
#else

  Word32 Ltmp;
  Word16 i, j, i8;
  Word16 exp_tmp, tmp16,fac;
  
  fac = 3277; 
  for(i=0; i<lg4; i+=8)
  {
    Ltmp = 0; move32();
    i8 = add(i,8);
    for(j=i; j<i8; j++) Ltmp = L_mac(Ltmp, xri[j], xri[j]);

    /*float ftmp = (float)sqrt(Ltmp);
      tmp16 = tmp16/max;*/
    exp_tmp = norm_l(Ltmp);
    Ltmp= L_shl(Ltmp, exp_tmp);
    exp_tmp = sub(30,exp_tmp);
    voAMRWBPDecIsqrt_n(&Ltmp, &exp_tmp);

    tmp16 = extract_h(Ltmp);
    
    if(sub(m_max, tmp16)>0)
    {
      m_max = shr(m_max,1);
      e_max = add(e_max,1);

    }
    tmp16 = div_s(m_max, tmp16);
    exp_tmp = sub(e_max, exp_tmp);
    
    tmp16 = shl(tmp16, exp_tmp);    /*Q15 : can up to 32767 (saturation is possible if tmp == max) */
    
    
    if (sub(tmp16,fac) > 0)
    {
      fac = tmp16;      
    }

    for(j=i; j<i8; j++)
    {
      xri[j] = mult(xri[j], fac);     
    }
  }
  return;
#endif
}
