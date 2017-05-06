#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "math_op.h"
#include "oper_32b.h"
#include "log2.h"

#include "amr_plus_fx.h"



Word16 Q_gain_tcx(  /* output: return quantization index */
  Word16 lcode,     /* (i)  : frame size                 */
  Word32 *gain,      /* in/out: quantized gain            */
  /*Word32 Lener*/
  Word16 m_tmp,
  Word16 e_tmp
)
{
  Word16 index, tmp16;
  Word16 e_g, g_frac;
  Word32 Ltmp, gcode0, gcode;

 
  /* energy already computed in Get_gain */
  /* fgcode0 = 4.0f * (float)sqrt(ftmp/((float)lcode));	*/
  
  e_g = norm_s(lcode);
  g_frac = shl(lcode, e_g);
  e_g = sub(15,e_g);

  
  if(sub(g_frac,m_tmp)>0)
  {
    g_frac = shr(g_frac,1);
    e_g = add(e_g,1);
  } 
  
  g_frac = div_s(g_frac, m_tmp);
  e_g = sub(e_g, e_tmp);

  gcode0 = voAMRWBPDecIsqrt(L_shl(g_frac, add(e_g,13/* came from get_gain*/)));      /*Q15 -> Q13 when multply by 4*/

  /* quantize gain of code */
  /*fgcode = *gain/65536.0 * fgcode0;*/      						

  voAMRWBPDecL_Extract(gcode0, &e_tmp, &m_tmp);
  e_g = norm_l(*gain);
  g_frac = extract_h(L_shl(*gain,e_g));
      
  gcode = Mpy_32_16(e_tmp, m_tmp, g_frac);    /*scaling of gcode is Q13 + (15-e_g) */
  
  gcode = L_shl(gcode, sub(15+2,e_g));     /* gcode in Q15 */

  
  if (gcode == 0) 
  {
    gcode = 1;	    move32();
  }

  /*ftmp = 28.0f * (float)log10(fgcode);*/       /* step of 0.714 dB */	
  voAMRWBPDecLog2(gcode, &e_tmp, &m_tmp);
  e_tmp = sub(e_tmp, 15);
  Ltmp = Mpy_32_16(e_tmp, m_tmp, LG10_28);  /* Q12 */
  

  /*index = (int)floor(ftmp + 0.5);*/

  index = extract_h(L_shl(L_add(Ltmp, 2048),4));
  
  if (index < 0) 
  {
    index = 0;    
  }
  
  if (sub(index, 127) > 0)
  {
    index = 127;        
  }

  /*fgcode = (float)pow(10.0, ((float)index)/28.0) / fgcode0; */

  Ltmp = L_mult(index, 3888); /* 3888 = ( 1 / (28*log10(2)) ) *Q15 -> Ltmp in Q16*/
  voAMRWBPDecL_Extract(Ltmp, &e_tmp, &m_tmp);
  tmp16 = extract_l(voAMRWBPDecPow2(14, m_tmp)); /* Put 14 as exponant -> result between 16384 and 32768*/
  
  e_g = norm_l(gcode0);
  g_frac = extract_h(L_shl(gcode0, e_g));
  e_g = sub(31, e_g);

  
  if(sub(tmp16,g_frac)>0)
  {
    tmp16 = shr(tmp16,1);
    e_tmp  = add(e_tmp,1);
  }
  tmp16 = div_s(tmp16, g_frac);
  e_tmp = sub(e_tmp, e_g);

  *gain = L_shl(tmp16, add(15,e_tmp));     /*Q16*/  move32();  

  return(index);
}



Word32 D_gain_tcx(    /* (o)  : gain                  Q16*/
  Word16 index,       /* (i)  : index                    */
  Word16 code[],      /* (i)  : quantized vector         */
  Word16 lcode,       /* (i)  : Length                   */
  Word16 bfi,         /* (i)  : Bad frame indicator      */
  Word16 *oldrms     /* (i/o): Old rms               Q0 */
)       
{
  Word16 i, ent, frac, exp_g0, rms, gcode0;
  Word16 e_lg,lg_frac, e_tmp, rms_e, tmp16;
  Word16 tmp_frac, *pt_y, j; 
  Word32 Ltmp,Ltmp1, gain, ent32, Lrms;

  
  
  e_lg = norm_s(lcode);
  lg_frac = shl(lcode, e_lg);
  e_lg = sub(15,e_lg);

  pt_y = code;
  Ltmp = 0;  move32();


  for (j = 0;j< lcode; j += 16)
  { 
    Ltmp1 = 0; move32();
    for (i = 0; i < L_SUBFR/4; i++) 
    {
      tmp16 = mult(*pt_y,8192);
      Ltmp1 = L_mac(Ltmp1, tmp16, tmp16);
      pt_y++; 
    }
    Ltmp = L_add(Ltmp, L_shr(Ltmp1, 5));
  }

  
  if (Ltmp== 0)
  {
    Ltmp= 1; move32();
  }
  e_tmp = norm_l(Ltmp);
  tmp_frac = extract_h(L_shl(Ltmp, e_tmp));
  e_tmp  = sub(e_tmp, 9);
  e_tmp = sub(30, e_tmp);

  
  if(sub(lg_frac, tmp_frac)>0)
  {
    lg_frac = shr(lg_frac,1);
    e_lg = add(e_lg,1);
  }

  /*rms = (float)sqrt(tmp/(float)lcode);*/ 
  rms = div_s(lg_frac, tmp_frac);
  rms_e = sub(e_lg, e_tmp);

  e_tmp = norm_s(rms);
  Lrms = L_shl(rms, add(e_tmp,16));
  rms_e = add(rms_e, e_tmp);
  voAMRWBPDecIsqrt_n(&Lrms, &rms_e);

  
  if(bfi == 0 )
  {
    /*float fgain = (float)pow(10.0, ((float)index)/28.0) / (frms*4);*/
    exp_g0 = add(rms_e, 2);  
    gcode0 = round16(Lrms);

    ent32 = L_mult(index, 3888); /* 3888 = ( 1 / (28*(1/log10(2))) ) *Q15 */
    voAMRWBPDecL_Extract(ent32, &ent, &frac);

    Ltmp = voAMRWBPDecPow2(14, frac); /* Put 14 as exponant so that */
    /*
     * output of voAMRWBPDecPow2() will be:
     * 16384 < voAMRWBPDecPow2() <= 32767
     */
    e_tmp = norm_l(Ltmp);
    frac = extract_h(L_shl(Ltmp,e_tmp));
    e_tmp  = add(e_tmp, sub(ent,14));
    ent  = sub(31, e_tmp);
    
   
    if(sub(frac,gcode0)>0)
    {
      frac = shr(frac,1);
      ent = sub(ent,1);
    }
    e_tmp  = div_s(frac,gcode0);
    exp_g0 = add(ent, exp_g0);

    exp_g0 = sub(31,exp_g0);
    gain = L_shl(e_tmp ,exp_g0);    /* gain in Q16 */
    /**old_rms = gain*rms;         rms of gain*code[] */

    rms = extract_h(L_shr(Lrms, sub(31-15, rms_e)));    /*Q0*/
    *oldrms = shl(mult(e_tmp, rms),exp_g0);             
  }
  else
  {
    /* This part is not tested 29/03/04 */
    *oldrms = mult_r(*oldrms, 22938);  /* Q0 */     
    
    if(*oldrms == 0)
    {
      *oldrms = 1;   
    }

    e_tmp   = norm_s(*oldrms);
    frac = shl(*oldrms, e_tmp );
    e_tmp  = sub(15, e_tmp );

    /*gain = *old_rms / rms;*/
    rms = round16(Lrms);
    
    if(sub(frac,rms)>0)
    {
      frac = shr(frac,1);
      e_tmp  = add(e_tmp ,1);
    }
    rms_e = sub(e_tmp , rms_e);
    e_tmp  = div_s(frac,rms);
    gain = L_shl(e_tmp ,sub(rms_e,-1));    /* gain in Q16*/
  }
  return(gain);
}



