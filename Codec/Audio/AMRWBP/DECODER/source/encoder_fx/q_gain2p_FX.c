/*-------------------------------------------------------------------------*
 * procedure q_gain2_plus                                                  *
 * ~~~~~~~~~~~~~~~~~~~~~~                                                  *
 * Quantization of pitch and codebook gains.                               *
 * The following routines is Q_gains updated for AMR_WB_PLUS.              *
 * MA prediction is removed and MEAN_ENER is now quantized with 2 bits and *
 * transmitted once every ACELP frame to the gains decoder.                *
 * The pitch gain and the code gain are vector quantized and the           *
 * mean-squared weighted error criterion is used in the quantizer search.  *
 *-------------------------------------------------------------------------*/

#include "amr_plus_fx.h"

#include "basic_op.h"
#include "count.h"
#include "math_op.h"
#include "oper_32b.h"
#include "log2.h"

#define RANGE         64


Word16 Q_gain2_plus(    /* (o)  : index of quantizer                            */
  Word16 code[],        /* (i)  : Innovative code vector                        */
  Word16 xn[],          /* (i)  : target signal                                 */
  Word16 y1[],          /* (i)  : filtered adaptive codebook excitation         */
  Word16 y2[],          /* (i)  : filtered fixed codebook excitation            */
  Word16 lcode,         /* (i)  : Subframe size                                 */
  Word16 *gain_pit,     /* (i/o): Pitch gain / Quantized pitch gain             */
  Word32 *gain_code,    /* (i/o): code gain / Quantized codebook gain           */
  Word16 *g_coeff,      /* (i)  : correlations <y1,y1>, -2<xn,y1>,              */
                        /*                <y2,y2>, -2<xn,y2> and 2<y1,y2>       */
  Word16 mean_ener,     /* (i)  : mean_ener defined in open-loop (2 bits)  Q7   */
  Word16 *g0,           /* (o)  : 'correction factor'                           */
  Word16 Q_xn           /* (i)  : Xn scaling                                    */  
)
{
  Word16   i, indice=0, min_ind, size, gcode0, g_pitch, coeff[5], exp_coeff[5];
  Word32 L_tmp;
  Word32 dist_min;
  const Word16 *t_qua_gain, *p;
  Word16 coeff_lo[5], exp1, exp_code, frac, exp_gcode0, g_code;
  Word16  exp_max[5], g2_pitch, g_pit_cod, g2_code_lo, g2_code, e_max, j, index ;


/*-----------------------------------------------------------------*
 * - Find the initial quantization pitch index                     *
 * - Set gains search range                                        *
 *-----------------------------------------------------------------*/

  t_qua_gain = t_qua_gain7b;        move16();

  p = (const Word16 *) (t_qua_gain7b + RANGE);  move16();     /* pt at 1/4th of table */ 
    
  min_ind = 0;          move16();
  g_pitch = *gain_pit;  move16();  

  for (i=0; i<(NB_QUA_GAIN7B-RANGE); i++, p+=2) 
  {
    test();
    if (sub(g_pitch,*p) > 0) 
    {
      min_ind = add(min_ind ,1);
    }
  }
  size = RANGE;     move16();

  min_ind = 0;        move16();
  size = 128;         move16();      


 /*------------------------------------------------------------------*
  *  Compute coefficient need for the quantization.                  
  *                                                                  
  *  coeff[0] =    y1 y1                                             
  *  coeff[1] = -2 xn y1                                             
  *  coeff[2] =    y2 y2                                             
  *  coeff[3] = -2 xn y2                                             
  *  coeff[4] =  2 y1 y2                                             
  *                                                                  
  *------------------------------------------------------------------*/
  coeff[0] = g_coeff[0];                 move16();
  exp_coeff[0] = g_coeff[1];             move16();
  coeff[1] = negate(g_coeff[2]);         move16();  /* coeff[1] = -2 xn y1 */
  exp_coeff[1] = add(g_coeff[3], 1);     move16();

 /* Compute scalar product <y2[],y2[]> */
  move16();move16();
  coeff[2] = extract_h(Dot_product12(y2, y2, lcode, &exp1));
  exp_coeff[2] = add(sub(exp1, 18), shl(Q_xn, 1));     /* -18 (y2 Q9) */

 /* Compute scalar product -2*<xn[],y2[]> */
  move16();move16();
  coeff[3] = extract_h(L_negate(Dot_product12(xn, y2, lcode, &exp1)));
  exp_coeff[3] = add(sub(exp1, 9 - 1), Q_xn);  /* -9 (y2 Q9), +1 (2 xn y2) */

 /* Compute scalar product 2*<y1[],y2[]> */
  move16();move16();
  coeff[4] = extract_h(Dot_product12(y1, y2, lcode, &exp1));
  exp_coeff[4] = add(sub(exp1, 9 - 1), Q_xn);  /* -9 (y2 Q9), +1 (2 y1 y2) */
 /*-----------------------------------------------------------------*
  *  Find energy of code and compute:                               
  *                                                                 
  *    L_tmp = MEAN_ENER - 10log10(energy of code/ lcode)         
  *          = MEAN_ENER - 3.0103*log2(energy of code/ lcode)     
  *-----------------------------------------------------------------*/

  L_tmp = Dot_product12(code, code, lcode, &exp_code);  
  exp_code = sub(exp_code, 18 + 6 + 31);

  Log2(L_tmp, &exp1, &frac);
  exp1 = add(exp1, exp_code);
  L_tmp = Mpy_32_16(exp1, frac, -24660);  /* x -3.0103(Q13) -> Q14 */

  /* predicted codebook gain */
  /* mean energy quantized with 2 bits : 18, 30, 42 or 54 dB */
  /*gcode0 = mean_ener - ener_code;*/
  L_tmp = L_mac(L_tmp, mean_ener, 64); /* + MEAN_ENER in Q7  (g_code0)*/

  /*  gcode0 = (float)pow(10.0,gcode0/20.0);    predicted gain */
  gcode0 = extract_h(L_shl(L_tmp,24-14));      /* From Q24 to Q8  */

  /*-----------------------------------------------------------------*
  * gcode0 = pow(10.0, gcode0/20)                                   
  *        = pow(2, 3.321928*gcode0/20)                             
  *        = pow(2, 0.166096*gcode0)                                
  *-----------------------------------------------------------------*/

  L_tmp = L_mult(gcode0, 5443);          /* *0.166096 in Q15 -> Q24     */
  L_tmp = L_shr(L_tmp, 8);               /* From Q24 to Q16             */
  L_Extract(L_tmp, &exp_gcode0, &frac);  /* Extract exponent of gcode0  */

  gcode0 = extract_l(Pow2(14, frac));    /* Put 14 as exponent so that  */
  /* output of Pow2() will be:   */
  /* 16384 < Pow2() <= 32767     */
  exp_gcode0 = sub(exp_gcode0, 14);

   /*-------------------------------------------------------------------------*
    * Find the best quantizer                                                 
    * ~~~~~~~~~~~~~~~~~~~~~~~                                                 
    * Before doing the computation we need to aling exponents of coeff[]      
    * to be sure to have the maximum precision.                               
    *                                                                         
    * In the table the pitch gains are in Q14, the code gains are in Q11 and  
    * are multiply by gcode0 which have been multiply by 2^exp_gcode0.        
    * Also when we compute g_pitch*g_pitch, g_code*g_code and g_pitch*g_code  
    * we divide by 2^15.                                                      
    * Considering all the scaling above we have:                              
    *                                                                         
    *   exp_code = exp_gcode0-11+15 = exp_gcode0+4                            
    *                                                                         
    *   g_pitch*g_pitch  = -14-14+15                                          
    *   g_pitch          = -14                                                
    *   g_code*g_code    = (2*exp_code)+15                                    
    *   g_code           = exp_code                                           
    *   g_pitch*g_code   = -14 + exp_code +15                                 
    *                                                                         
    *   g_pitch*g_pitch * coeff[0]  ;exp_max0 = exp_coeff[0] - 13             
    *   g_pitch         * coeff[1]  ;exp_max1 = exp_coeff[1] - 14             
    *   g_code*g_code   * coeff[2]  ;exp_max2 = exp_coeff[2] +15+(2*exp_code) 
    *   g_code          * coeff[3]  ;exp_max3 = exp_coeff[3] + exp_code       
    *   g_pitch*g_code  * coeff[4]  ;exp_max4 = exp_coeff[4] + 1 + exp_code   
    *-------------------------------------------------------------------------*/

    exp_code = add(exp_gcode0, 4);

    exp_max[0] = sub(exp_coeff[0], 13);    move16();
    exp_max[1] = sub(exp_coeff[1], 14);    move16();
    exp_max[2] = add(exp_coeff[2], add(15, shl(exp_code, 1)));  move16();
    exp_max[3] = add(exp_coeff[3], exp_code);   move16();
    exp_max[4] = add(exp_coeff[4], add(1, exp_code));   move16();

   /* Find maximum exponant */

    e_max = exp_max[0];                    move16();
    for (i = 1; i < 5; i++)
    {
        test();
        if (sub(exp_max[i], e_max) > 0)
        {
            e_max = exp_max[i];            move16();
        }
    }

   /* align coeff[] and save in special 32 bit double precision */

    for (i = 0; i < 5; i++)
    {
        j = add(sub(e_max, exp_max[i]), 2);/* /4 to avoid overflow */
        L_tmp = L_deposit_h(coeff[i]);
        L_tmp = L_shr(L_tmp, j);
        L_Extract(L_tmp, &coeff[i], &coeff_lo[i]);
        coeff_lo[i] = shr(coeff_lo[i], 3); move16();  /* lo >> 3 */
    }

   /* Codebook search */

    dist_min = MAX_32;                     move32();
    p = &t_qua_gain[shl(min_ind, 1)];      move16();


    index = 0;                             move16();
    for (i = 0; i < size; i++)
    {
        g_pitch = *p++;                    move16();
        g_code = *p++;                     move16();

        g_code = mult_r(g_code, gcode0);
        g2_pitch = mult_r(g_pitch, g_pitch);
        g_pit_cod = mult_r(g_code, g_pitch);
        L_tmp = L_mult(g_code, g_code);
        L_Extract(L_tmp, &g2_code, &g2_code_lo);

        L_tmp = L_mult(coeff[2], g2_code_lo);
        L_tmp = L_shr(L_tmp, 3);
        L_tmp = L_mac(L_tmp, coeff_lo[0], g2_pitch);
        L_tmp = L_mac(L_tmp, coeff_lo[1], g_pitch);
        L_tmp = L_mac(L_tmp, coeff_lo[2], g2_code);
        L_tmp = L_mac(L_tmp, coeff_lo[3], g_code);
        L_tmp = L_mac(L_tmp, coeff_lo[4], g_pit_cod);
        L_tmp = L_shr(L_tmp, 12);
        L_tmp = L_mac(L_tmp, coeff[0], g2_pitch);
        L_tmp = L_mac(L_tmp, coeff[1], g_pitch);
        L_tmp = L_mac(L_tmp, coeff[2], g2_code);
        L_tmp = L_mac(L_tmp, coeff[3], g_code);
        L_tmp = L_mac(L_tmp, coeff[4], g_pit_cod);

        test();
        if (L_sub(L_tmp, dist_min) < (Word32) 0)
        {
            dist_min = L_tmp;              move32();
            index = i;                     move16();
        }

    }
   /* Read the quantized gains */
    index = add(index, min_ind);
    indice = index;                                     move16();
    index = add(index,index);
    *gain_pit  = t_qua_gain[index];                     move16();
    
    index = add(index,1);
    
    L_tmp = L_mult(t_qua_gain[index], gcode0);             /* Q11*Q0 -> Q12 */
    *gain_code = L_shl(L_tmp, add(exp_gcode0, 4));        move32();/* Q12 -> Q16 */
    
    *g0 = t_qua_gain[index];                              move16();

  return indice;
}




