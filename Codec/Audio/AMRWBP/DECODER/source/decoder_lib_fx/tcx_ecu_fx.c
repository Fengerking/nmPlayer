
#include "typedef.h"
#include "count.h"
#include "basic_op.h"
#include "math_op.h"
#include "oper_32b.h"

#include <float.h>
#include "amr_plus_fx.h"

#define NB_INDEX  12

/*
  phasor computations
*/

static void Get_phasor(
  Word16 xri[],
  Word16 ph[]
)
{
  Word16 e_tmp, gain;
  Word32 Ltmp;

  /*tmp = xri[0]*xri[0] + xri[1]*xri[1];*/
  Ltmp  = L_mult(xri[0],xri[0]);
  Ltmp = L_mac(Ltmp, xri[1],xri[1]);
  
  if(Ltmp == 0)
  {
    ph[0] = 32767;    
    ph[1] = 0;    
  }
  else 
  {

    /*tmp = 1.0f/(float) sqrt(tmp);*/
    e_tmp = norm_l(Ltmp);
    Ltmp = L_shl(Ltmp, e_tmp);
    e_tmp = sub(30,e_tmp);
    
    voAMRWBPDecIsqrt_n(&Ltmp, &e_tmp);

    gain = round16(Ltmp);   
    /* Output in Q15 */
    ph[0] = mult_r(xri[0],gain);
    ph[1] = mult_r(xri[1],gain);

    voAMRWBPDecScale_sig(ph, 2, add(15,e_tmp));
   
  }

}

static void Mult_phasor(
  Word16 ph1[],
  Word16 ph2[],
  Word16 res[]
)
{

  Word16 re, im;
  Word32 Ltmp;


  /*ph_re = ph1[0]*ph2[0] - ph1[1]* ph2[1];*/
  Ltmp = L_mult(ph1[0], ph2[0]);
  Ltmp = L_msu(Ltmp, ph1[1], ph2[1]);
  re = round16(Ltmp);  

  /*ph_im = ph1[1]*ph2[0] + ph1[0]* ph2[1];*/
  Ltmp = L_mult(ph1[1], ph2[0]);
  Ltmp = L_mac(Ltmp, ph1[0], ph2[1]);
  im = round16(Ltmp);  

  res[0] = re;    
  res[1] = im;    


}

static void Div_phasor(
  Word16 ph1[],
  Word16 ph2[],
  Word16 res[]
)
{

  Word16 re, im;
  Word32 Ltmp;

  /* Output in Q15*/
  
  /*ph_re = ph1[0]*ph2[0] + ph1[1]* ph2[1];*/
  Ltmp = L_mult(ph1[0], ph2[0]);
  Ltmp = L_mac(Ltmp, ph1[1], ph2[1]);
  re = round16(Ltmp);

  /*ph_im = ph1[1]*ph2[0] - ph1[0]* ph2[1];*/
  Ltmp = L_mult(ph1[1], ph2[0]);
  Ltmp = L_msu(Ltmp, ph1[0], ph2[1]);
  im = round16(Ltmp);
  
  res[0] = re;    
  res[1] = im;    


}


/*---------------------------------------------------------------*/
/* Adaptive low frequency de-emphasis in case of packet loss     */
/*                                                               */
/*                                                               */
/*---------------------------------------------------------------*/
void Adapt_low_freq_deemph_ecu(
  Word16 xri[],
  Word16 lg,  
  Word16 Q_ifft,
  Decoder_State_Plus_fx *st
)
{

  Word16 buf[L_TCX];
  Word32 pred_max, max;
  Word16 m_max, e_max;
  Word16 curr_mode, e_pred, m_pred;
  Word16 lg4;

  lg4 = shr(lg,2);
  e_pred = 0; 
  m_pred = 0; 

  
  /*---------------------------------------------------------------*
   * Set correct buffer lengths                 *
   *---------------------------------------------------------------*/
  
  if(sub(lg,1152) == 0)
  {
    curr_mode =3;       
  }
  else if(sub(lg,576) == 0)
  {
    curr_mode =2;   
  }
  else
  {
    curr_mode =1;   
  }
  
  if((sub(st->last_mode,curr_mode) != 0) || (sub(curr_mode,2) <= 0)) 
  {
    pred_max = 0;     
  }
  else 
  {
    /*---------------------------------------------------------------*
     * Temporary working buffer                 *
     *---------------------------------------------------------------*/
    
    if(sub(Q_ifft, st->Old_Qxri) != 0)
    {
      voAMRWBPDecScale_sig(st->wold_xri, L_TCX, sub(Q_ifft, st->Old_Qxri));    /* Reconst_spect need it too*/
    }
    voAMRWBPDecCopy(st->wold_xri,buf,lg4);
    /*---------------------------------------------------------------*
     * Find spectral peak under 1600Hz (lg = 6400Hz)                 *
     * (find maximum of energy of all Re8 subvector under 1600Hz)    *
     *---------------------------------------------------------------*/
    pred_max = SpPeak1k6(buf, &e_pred, lg4);  
    m_pred = extract_h(pred_max);
    pred_max = L_shl(pred_max, e_pred);    /*Q31 voAMRWBPDecIsqrt!!!*/

  }

  /*---------------------------------------------------------------*
   * Find spectral peak under 1600Hz (lg = 6400Hz)                 *
   * (find maximum of energy of all Re8 subvector under 1600Hz)    *
   *---------------------------------------------------------------*/
  max = SpPeak1k6(xri, &e_max, lg4);  
  m_max = extract_h(max);
  max = L_shl(max, e_max);    /*Q31   voAMRWBPDecIsqrt!!!*/
  

  /*---------------------------------------------------------------*/
  /* Set the new max                                               */
  /*---------------------------------------------------------------*/
    
  if(L_sub(max, pred_max) > 0 && pred_max != 0) 
  {
    m_max = m_pred;
    e_max = e_pred;   
  }
  /*---------------------------------------------------------------*
   * Deemphasis of all subvector below 1600 Hz.                    *
   *---------------------------------------------------------------*/
  Deemph1k6(xri, e_max, m_max, lg4);

  return;
}


/*---------------------------------------------------------------*/
/* Spectral reconstruction in case of packet loss                */
/*                                                               */
/*                                                               */
/*---------------------------------------------------------------*/
void Reconst_spect(
  Word16 xri[],
  Word16 old_xri[],
  Word16 n_pack,
  Word16 bfi[],
  Word16 lg,
  Word16 last_mode,
  Word16 Q_xri
)   
{

  Word32 Lbuf[L_TCX],*sp = Lbuf, *old_sp = Lbuf+L_TCX/2; 
  Word16 lost[L_TCX/2];
  Word16 e_oenr, m_oenr, e_enr, m_enr;
  Word16 i,l,p, lg2, lg8, hi ,lo, e_sp, tmp16;
  Word16 gain_sq,curr_mode, gain16;
  Word32 energy,old_energy;
  Word32 Ltmp, sp_tmp;
  Word16 angle, ph_c[2],ph_tmp[2],ph_d[2],start, end;
  Word16 bin,bin_start,bin_end;

  /*---------------------------------------------------------------*
   * Set correct buffer lengths                                    *
   *---------------------------------------------------------------*/
  
  if(sub(lg,1152) == 0)
  {
    curr_mode =3;   
  }
  else if(sub(lg,576) == 0)
  {
    curr_mode =2;   
  }
  else
  {
    curr_mode =1;   
  }

  /*---------------------------------------------------------------*
   * Can't do anything with ACELP as previous mode           *
   *---------------------------------------------------------------*/ 
  
  if((sub(last_mode,curr_mode) != 0) || (sub(curr_mode,2) <= 0)) 
  {
    return;
  }
  /*---------------------------------------------------------------*
   * Fill up the lost frequency bins                               *
   *---------------------------------------------------------------*/
  lg2 = shr(lg,1);
  lg8 = shr(lg,3);
  for(i=0;i<lg2;i++) 
  {
    lost[i] = 0;      
  }
  for(p=0;p<n_pack;p++) 
  {
    
    if (bfi[p]>0)
    {
      for (l=p; l<lg8; l+=n_pack) 
      {
        /* subvector l is lost */

        for (i=0; i<4; i++) 
        {
          lost[4*l+i] = 1;    
        }
      }
    }
  }

  /*---------------------------------------------------------------*
   * Compute the old spectrum                                      *
   *---------------------------------------------------------------*/
  old_sp[0] = L_mult(old_xri[0],old_xri[0]); move32();

  for(i=1;i<lg2;i++)
  {
    Ltmp = L_mult(old_xri[2*i],old_xri[2*i]);
    old_sp[i] =  L_mac(Ltmp, old_xri[2*i+1], old_xri[2*i+1]);   move32();
  }

  /*---------------------------------------------------------------*
   * Compute the new spectrum                                      *
   *---------------------------------------------------------------*/
  sp[0] = L_mult(xri[0], xri[0]);

  for(i=1;i<lg2;i++)
  {
    Ltmp = L_mult(xri[2*i],xri[2*i]); 
    sp[i] = L_mac(Ltmp, xri[2*i+1], xri[2*i+1]);      move32();
  }
  /*---------------------------------------------------------------*
   * Compute the overall gain difference on non-zero spectral		*
   *---------------------------------------------------------------*/
  energy = 0;         move32();
  old_energy = 0;     move32();
  for(i=0;i<lg2;i++) 
  {
    
    if(sp[i]>0)
    {
      energy = L_add(energy, sp[i]);
      old_energy = L_add(old_energy, old_sp[i]);
    }
  }
  
  if (energy == 0)
  {
    gain16 = 0; 
  }
  else
  {
    /*gain_sq = energy / old_energy;
      gain = (float)sqrt(gain_sq);*/
    
    if (old_energy>0)
    {
      e_oenr = norm_l(old_energy);
      m_oenr = extract_h(L_shl(old_energy,e_oenr));
      e_oenr = sub(30, e_oenr);

      e_enr = norm_l(energy);
      m_enr = extract_h(L_shl(energy,e_enr));
      e_enr = sub(30, e_enr);
      if(sub(m_oenr,m_enr)>0)
      {
        m_oenr = shr(m_oenr,1);
        e_oenr = add(e_oenr,1);
      }
      m_enr = div_s(m_oenr, m_enr);
      e_enr =sub(e_oenr, e_enr);

      energy = L_deposit_h(m_enr);
  
      voAMRWBPDecIsqrt_n(&energy, &e_enr);
                                                                    /*saturation can occure here*/
      gain16 = extract_h(L_shl(energy, sub(e_enr,31-14-16)));  /* Q14 */ 

      /* limit the gain */
      
      if(sub(gain16,23170) >0) 
      {
        gain16 = 23170;       
      }
    }
    else
    {
      gain16 = 23170;       
    }

  }
  Ltmp = L_mult(gain16,gain16);
  gain_sq = round16(L_shl(Ltmp,1));  /*Q14*/

  /*---------------------------------------------------------------*
   * merge with the new spectrum                                   *
   *---------------------------------------------------------------*/

  for(i=0;i<lg;i++) 
  {
    
    if(lost[shr(i,1)]>0)
    {
      Ltmp = L_mult(gain16, old_xri[i]);
      xri[i] = round16(L_shl(Ltmp,1));      
    }
  }
  /*---------------------------------------------------------------*
   * compensate for the  spectrum                                  *
   *---------------------------------------------------------------*/
  for(i=0;i<lg2;i++)
  {
    
    if(lost[i]>0) 
    {
      voAMRWBPDecL_Extract(old_sp[i], &hi, &lo);
      Ltmp = Mpy_32_16(hi,lo, gain_sq);
      sp[i]= L_shl(Ltmp,1);               move32();
    }
  }
  /*---------------------------------------------------------------*
   * apply the group delay conservation                            *
   *---------------------------------------------------------------*/
  /* start phase compensation */
  bin = 0;    

  do 
  {
    
    if(lost[bin]>0) 
    {
      bin_start = bin;              
      
      while((lost[bin]>0) && (sub(bin,lg2)<0))
      {
        bin = add(bin, 4);
      }
      bin_end = bin;        
      
      if(bin_start==0) 
      {
        bin_start = 1;     
      }
      /* compute the phase correction factor */
      
      if(sub(bin_end,lg2) != 0)
      {
        start = shl(sub(bin_start,1),1);
        end = shl(bin_end,1);
        Get_phasor(&xri[start],ph_tmp);
        Get_phasor(&xri[end],ph_c);
        Div_phasor(ph_c,ph_tmp,ph_c);

        Get_phasor(&old_xri[start],ph_tmp);
        Mult_phasor(ph_c,ph_tmp,ph_c);
        Get_phasor(&old_xri[end],ph_tmp);
        Div_phasor(ph_c,ph_tmp,ph_c);

        /* get the angle on 4 quad and divide by the length */
        angle = Atan2(ph_c[1],ph_c[0]);   /*Q12*/

        angle = mult_r(angle, Len_tbl[add(sub(bin_end, bin_start),1-4)]);
        Find_x_y(angle, ph_c);
      }
      else 
      {
        ph_c[0] = 32767;        
        ph_c[1] = 0;        
      }


      /* loop in the missing bins */
      for(i=bin_start;i<bin_end;i++) 
      {
        /* compute old difference */
        Get_phasor(&old_xri[2*(i-1)],ph_tmp);
        Get_phasor(&old_xri[2*i],ph_d);

        
        Div_phasor(ph_d,ph_tmp,ph_d);  

        /* compute new phase*/
        Get_phasor(&xri[2*(i-1)],ph_tmp);
        Mult_phasor(ph_tmp,ph_d,ph_tmp);
        /* apply phase compensation */
        Mult_phasor(ph_tmp,ph_c,ph_tmp);

        /* Find sqroot*/
        
        if (sp[i] == 0)
        {
          sp_tmp = 1;      move32();
        }
        else
        {
          sp_tmp = sp[i];   move32();
        }

        e_sp = norm_l(sp_tmp);
        Ltmp = L_shl(sp_tmp,e_sp);
        e_sp = sub(30, e_sp);
        e_sp = sub(e_sp, shl(Q_xri,1));
        voAMRWBPDecIsqrt_n(&Ltmp, &e_sp);
        tmp16 = round16(Ltmp);
        tmp16 = div_s(16384,tmp16);

        e_sp = sub(1+1, e_sp);       
        e_enr = mult_r(tmp16,ph_tmp[0]);
        xri[2*i] = round16(L_shl(e_enr,add(Q_xri,e_sp)));       

        e_enr= mult_r(tmp16,ph_tmp[1]);
        xri[2*i+1] = round16(L_shl(e_enr,add(Q_xri,e_sp)));     

      }
    }
    bin  = add(bin,4);
  } while(sub(bin,lg2)<0);

}
/* Atant 2 Interpolation per  section */
Word16 Atan2(
  Word16 ph_y,        /*Q15 */
  Word16 ph_x         /*Q15 */
)
{
  Word16 ent_frac, alpha, alpham1;
  Word16 Num, angle, Den, e_num, m_num, e_den, m_den,  sign_x = 1, sign_y = 1, index;
  Word32 frac, Ltmp;


  Den = ph_x;   
  Num = ph_y;   
  
  

  
  if(Num < 0)
  {
    Num = negate(Num);
    sign_y = -1;        
  }
  
  if(Den < 0)
  {
    Den = negate(Den);
    sign_x = -1;        
  }

  
  if (Den == 0)
  {
    angle = 6434;   /* pi/2 */
  }
  else if(Num == 0)
  {
    angle = 0;     /* 0 */ 
  }
  else
  {

    e_num = norm_s(Num);
    m_num = shl(Num, e_num);
    e_num = sub(15,e_num);

    e_den = norm_s(Den);
    m_den = shl(Den, e_den);
    e_den = sub(15,e_den);

  
    
    if(sub(m_num, m_den)>0)
    {
      m_num = shr(m_num,1);
      e_num = add(e_num,1);
    }

    m_num = div_s(m_num,m_den);
    e_num = sub(e_num, e_den);
    frac = L_shl(m_num, e_num);

    /* Worst case = 20 op + 50 memory Word16*/
    ent_frac = extract_h((L_shl(frac,1)));
    
    if (sub(ent_frac, 88) >= 0)  /* y/x > 88 */     /*  Saturation*/
    {
      /* pi/2 in Q12*/
      alpha = shl(sub(ent_frac, 88),15-6);   /* x2^15 / 64 */

      alpham1 = sub(32767, alpha);                            
      /* Find x */
      Ltmp = L_mult(alpham1, TXV[30]);
      Ltmp = L_mac(Ltmp, alpha, 25736);
      angle = round16(L_shr(Ltmp,2));    
    }
    else if (sub(ent_frac, 24)>= 0) /* 24<= y/x < 88*/
    {
      /* Use lineaire interpolation (last index) error max = 0.02 rad */
                                                /* 12 op*/
      alpha = shl(sub(ent_frac, 24),15-6);   /* x2^15 / 64 */

      alpham1 = sub(32767, alpha);                            

      /* Find x */
      Ltmp = L_mult(alpham1, TXV[29]);
      Ltmp = L_mac(Ltmp, alpha, TXV[30]);
      angle = round16(L_shr(Ltmp,2));    
    }
    else if (sub(ent_frac, 12)> 0) /* 12< y/x < 24*/
    {
      index = add(extract_h(frac),18);
  
      alpha = extract_l(L_shr(frac,1)&0x7fff);    logic16();

      alpham1 = sub(32767, alpha);                            

      /* Find x */
      Ltmp = L_mult(alpham1, TXV[ index]);
      Ltmp = L_mac(Ltmp, alpha, TXV[ add(index,1)]);
      angle = round16(L_shr(Ltmp,2));    
    }
    else if (sub(ent_frac, 4)>= 0) /* 4<= y/x < 12*/
    {
      index = add(ent_frac, 16-4);   
  
      alpha = extract_l(frac&0x7fff);    logic16();

      alpham1 = sub(32767, alpha);                            

      /* Find x */
      Ltmp = L_mult(alpham1, TXV[ index]);
      Ltmp = L_mac(Ltmp, alpha, TXV[ add(index,1)]);
      angle = round16(L_shr(Ltmp,2));    
    }
    else                          /*  y/x < 4 13 op*/
    {
      index = extract_h((L_shl(frac,3)));
  
      alpha = extract_l(L_shl(frac,2)&0x7fff);    logic16();

      alpham1 = sub(32767, alpha);                            

      /* Find x */
      Ltmp = L_mult(alpham1, TXV[ index]);
      Ltmp = L_mac(Ltmp, alpha, TXV[ add(index,1)]);
      angle = round16(L_shr(Ltmp,2));    

    }

  }

  
  if(mult(sign_x, sign_y)>=0)  /* Quadrant 1 or 3*/
  { 
    
    if(sign_x>0)
    {
      return angle;
    }
    else
    {
      return sub(angle, 12868);/* return pi+ angle*/      
    }
  }
  else /* Quadrant 2 or 4*/
  {
    
    if(sign_x <0)   /*Quad 2*/
    {
      
      if(angle == 0)
        return 12868; /* return pi*/
      else
        return sub(12868, angle); /* return pi-angle*/
    }
    else            /*Quad 4*/
    {
      return negate(angle); /* return 2pi - angle */
    }
  }
}

/* To find value of x and y with sin and cos table of fft9 */
/* Wordt case: 29 op                                       */
void Find_x_y(
  Word16 angle,     /* Q12 */
  Word16 *ph        /* x, y -> Q15 */
)
{
  Word16 index, fr_ind, hi, lo, alpha, alpham1;
  Word32 Ltmp;

  Ltmp = L_shl(angle, 9);
  voAMRWBPDecL_Extract(Ltmp, &hi, &lo);
  Ltmp = Mpy_32_16(hi, lo, 23468);                        /* Q13 */
  index = round16(L_shl(Ltmp,31-13-15));                    /* Q0  */
  fr_ind = abs_s(extract_l(L_msu(Ltmp, index, 4096)));    logic16();

  alpha = shl(fr_ind,2);                                  /* Coefficient interpolation */
  alpham1 = sub(32767, alpha);                            

  /* Find x */
  Ltmp = L_mult(alpham1, t_sinFxS4[ index + COSOFFSET]);
  Ltmp = L_mac(Ltmp, alpha, t_sinFxS4[ add(index,1) + COSOFFSET]);
  ph[0] = round16(L_shl(Ltmp,2));    

  /* Find y */
  
  if(angle>0)
  {
    Ltmp = L_mult(alpham1, t_sinFxS4[ index]);
    Ltmp = L_mac(Ltmp, alpha, t_sinFxS4[ add(index,1)]);
    ph[1] = round16(L_shl(Ltmp,2));    
  }
  else                                          /* if angle < 0; begin at the end of sine */
  {
    Ltmp = L_mult(alpham1, t_sinFxS4[ index + 4*COSOFFSET]);
    Ltmp = L_mac(Ltmp, alpha, t_sinFxS4[ add(index,1) + 4*COSOFFSET]);
    ph[1] = round16(L_shl(Ltmp,2));    
  }

}
