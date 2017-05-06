/*-------------------------------------------------------------------*
 * procedure d_gain2_plus                                            *
 * ~~~~~~~~~~~~~~~~~~~~~~                                            *
 * Decoding of pitch and codebook gains  (see q_gain2_plus.c)        *
 *-------------------------------------------------------------------*
 * input arguments:                                                  *
 *                                                                   *
 *   indice     :Quantization index                                  *
 *   code[]     :Innovative code vector                              *
 *   lcode      :Subframe size                                       *
 *   bfi        :Bad frame indicator                                 *
 *                                                                   *
 * output arguments:                                                 *
 *                                                                   *
 *   gain_pit   :Quantized pitch gain                                *
 *   gain_code  :Quantized codeebook gain                            *
 *                                                                   *
 * Global variables defining quantizer (in qua_gns.h)                *
 *                                                                   *
 *   t_qua_gain[]    :Table of gain quantizers                       *
 *   nb_qua_gain     :Nombre de quantization levels                  *
 *                                                                   *
 *-------------------------------------------------------------------*/


#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "math_op.h"
#include "count.h"
#include "log2.h"




Word16 D_gain2_plus(    /* (o)  : 'correction factor' */
  Word16 index,         /* (i)  : index of quantizer                      Q0  */ 
  Word16  code[],       /* (i)  : Innovative code vector                  Q9  */
  Word16  lcode,        /* (i)  : Subframe size                           Q0  */
  Word16  *gain_pit,    /* (o)  : Quantized pitch gain                    Q14 */
  Word32  *gain_code,   /* (o)  : Quantized codebook gain                 Q16 */
  Word16  bfi,          /* (i)  : Bad frame indicato                      Q0  */
  Word16  mean_ener,    /* (i)  : mean_ener defined in open-loop (2 bits) Q8  */
  Word16  *past_gpit,   /* (i)  : past gain of pitch                      Q14 */
  Word32  *past_gcode
)                       /* (i/o): past gain of code                       Q16 */
{
  
  Word32  Ltmp;
  Word16  tmp16, exp, frac_enc, hi, lo, ind2, ind2p1;
  Word16  gcode0, exp_enc, exp_inov;
  Word16  exp_gcode, mant_code, mant_inov;
  Word32  ener_code;
  Word16 gcode_inov;
  const Word16  *t_qua_gain;
  t_qua_gain = t_qua_gain7b;

  
  /*----------------- Test erasure ---------------*/
  Ltmp = voAMRWBPDecDot_product12(code, code, lcode, &exp);   
  
  ener_code = Ltmp; move32(); /* To save further dot product*/
  exp_enc = exp;        

  exp = sub(exp, 6+18);       /* exp: -18 (code in Q9), -6 (/L_subfr) */
  voAMRWBPDecIsqrt_n(&Ltmp, &exp);
  gcode_inov = extract_h(L_shl(Ltmp, sub(exp, 3)));   /* g_code_inov in Q12*/
  
  
  if (bfi != 0)
  {                  
        
    if ( sub(*past_gpit,15565) > 0 )
    {
      *past_gpit = 15565;    /* 0.95 Q14*/
      
    }
    
    if (sub(*past_gpit,8192) < 0)
    {
      *past_gpit = 8192;      /* 0.5 Q14*/
      
    }
    
    *gain_pit = *past_gpit;  
    /**past_gpit *= 0.95f;*/  
    *past_gpit  = mult_r(*past_gpit, 31130); 

    /*past_gcode *= (1.4f - *past_gpit);*/
    tmp16 = sub(22938, *past_gpit);
    voAMRWBPDecL_Extract(*past_gcode, &hi, &lo);
    *past_gcode = L_shl(Mpy_32_16(hi, lo, tmp16),1);   move32();

    /*gain_code = (Word32)(*past_gcode * gcode_inov);      */
    voAMRWBPDecL_Extract(*past_gcode, &hi, &lo);
    *gain_code= L_shl(Mpy_32_16(hi, lo, gcode_inov),3);  move32();
    
    return 0;
  }

  /*-------------- Decode gains ---------------*/

  /*--------------------------------------------
  ener_code = (10.0 * log10(ener_code/lcode));
  ----------------------------------------------*/
  voAMRWBPDecLog2(ener_code, &exp, &frac_enc);
  hi =  sub(add(exp,exp_enc),18+6+31);  
  ener_code = Mpy_32_16(hi, frac_enc, LG10);       /*Ener_code in Q14*/
  
  /* predicted codebook gain */
  /* mean energy quantized with 2 bits : 18, 30, 42 or 54 dB */
  
  /*--------------------------------------------
  gcode0 = mean_ener - ener_code;
  gcode0 = (Float32)pow(10.0,gcode0/20.0);   
  ----------------------------------------------*/
  gcode0 = extract_l(L_sub(mean_ener, L_shr(ener_code,6)));      /* Q8->Q10-Q10 gcode Q10*/
 
  Ltmp = L_mult(gcode0, 5443);           /* *0.166096 in Q15 -> Q24     */
  Ltmp = L_shr(Ltmp, 8);                 /* From Q24 to Q16             */
  voAMRWBPDecL_Extract(Ltmp, &exp_enc, &frac_enc);  /* Extract exponant of gcode0  */

  gcode0 = extract_l(voAMRWBPDecPow2(14, frac_enc));    /* Put 14 as exponant so that  */
  /* output of voAMRWBPDecPow2() will be:   */
  /* 16384 < voAMRWBPDecPow2() <= 32767     */
  exp_enc = sub(exp_enc, 14);
    
  ind2 = shl(index,1);
  ind2p1 = add(ind2,1);
  *gain_pit = t_qua_gain[ind2];  
  Ltmp  = L_mult(t_qua_gain[ind2p1], gcode0);  /* Q11*Q0 -> Q12 */
  
  *gain_code = L_shl(Ltmp, add(exp_enc, 4));    move32();           /* Q12 -> Q16 */
    
  *past_gpit = *gain_pit;       

  /*-------------------------------------------------------
   past_gcode  = gain_code/gcode_inov
  --------------------------------------------------------*/  
  exp_gcode = norm_l(*gain_code);
  mant_code = extract_h(L_shl(*gain_code, exp_gcode));     
  
  exp_inov = norm_l(gcode_inov);        
  mant_inov = extract_h(L_shl(gcode_inov, exp_inov));           
  
  
  if (sub(mant_code, mant_inov)>0)
  {
    exp_gcode = sub(exp_gcode,1);
    mant_code = shr(mant_code,1);
  }
  exp_gcode = sub(exp_gcode, exp_inov);             
  mant_code = div_s(mant_code, mant_inov);                   
      
  *past_gcode = L_shr(mant_code, add(3,exp_gcode)); move32();        

  return t_qua_gain[ind2p1];
}







