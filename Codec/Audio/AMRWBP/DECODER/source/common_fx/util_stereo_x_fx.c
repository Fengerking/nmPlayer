
#include "amr_plus_fx.h"
#include "basic_op.h"
#include "count.h"
#include "oper_32b.h"
#include "log2.h"
#include "math_op.h"

#define MAX_VECT_DIM  16

#define INV_FAC32 1024 /* 1/32 in Q15 */

void Fir_filt(
     Word16 a[],      /* (i) Q14 : filter coefficients                     */
     Word16 m,        /* (i)     : order of filter                          */
     Word16 x[],      /* (i)     : speech (values x[-m..-1] are needed         */
     Word16 y[],      /* (o) x2  : residual signal                             */
     Word16 lg        /* (i)     : size of filtering                           */
)
{
  Word16 i, j;
  Word32 L_s;

// @shanrong modified
#if (!FUNC_FIR_FILT_OPT)
  for (i = 0; i < lg; i++)
  {
    L_s = L_mult(x[i], a[0]);
    for (j = 1; j < m; j++)
      L_s = L_mac(L_s, a[j], x[i - j]);
    y[i] = round16(L_shl(L_s,2));                   
  }
#else
  for (i = 0; i < lg; i++)
  {
    //L_s = L_mult(x[i], a[0]);
    L_s = 0;
    for (j = 0; j < m; j++)
    {
        L_s += a[j] * x[i - j];
    }

    y[i] = ((L_s << 3) + 0x00008000) >> 16;
    //y[i] = round16(L_shl(L_s,2));                   
  }
#endif
// end

  return;
}


void Band_join_2k(Word16 sig_fx[],
  Word16 sig_2k_fx[],
  Word16 sig_hi_fx[],
  Word16 lg)
{
  Word16 i;

  Oversamp_2k(sig_2k_fx-L_FDEL_2k,sig_fx,lg,Filter_2k_fxQ14_32);

  for(i=0;i<lg;i++)
  {
    sig_fx[i]=add(sig_fx[i],sig_hi_fx[i-L_FDEL]);    
  }

  return;
}


void Band_split_taligned_2k(Word16 sig_fx[],
  Word16 sig_2k_fx[],
  Word16 sig_hi_fx[],
  Word16 lg)
{
  Word16 i, L_frame,tmp;

  tmp=shr(lg,5);
  L_frame=add(shl(tmp,2),tmp);

  /* decimate to 2k fs */

  Decim_2k(sig_fx+L_FDEL,sig_2k_fx+2*L_FDEL_2k,L_frame,Filter_2k_fxQ14_5);	

  /* interpolate to 12.8k fs */

  Oversamp_2k(sig_2k_fx+L_FDEL_2k,sig_hi_fx,lg,Filter_2k_fxQ14_32);

  for(i=0;i<lg;i++)
  {
    sig_hi_fx[i]=sub(sig_fx[i],sig_hi_fx[i]);   
  }


  return;
}



void Decim_2k(Word16 *in, Word16 *out, Word16 L_frame, const Word16 *filter)
{
  Word16 pos,j,frac,pos_step,pos_step_plus_one;
  Word16 frac_step,fac_up_minus_frac_step;

  pos_step=mult(32,6554); /*32*1/5 ;*/
  pos_step_plus_one = add(pos_step,1);

  frac_step = sub(32,add(shl(pos_step,2),pos_step));
  fac_up_minus_frac_step = sub(5,frac_step);


  pos = 0;      
  frac = 0;     
  for (j = 0; j < L_frame; j++)
  {
    out[j] = Interpol_st(in+pos, filter, frac, 5, L_FDEL);       
    
    if (sub(frac, fac_up_minus_frac_step) > 0) 
    {
      pos=add(pos,pos_step_plus_one);
      frac=sub(frac,fac_up_minus_frac_step);
    }
    else 
    {
      pos=add(pos,pos_step);
      frac=add(frac,frac_step);
    }
  }

    return;
}
void Oversamp_2k(Word16 *in, Word16 *out, Word16 L_frame, const Word16 *filter)
{
  Word16 pos,frac,i,j;


  pos = 0;                                 /* Q5, position with 1/32 resolution */

  for (j = 0; j < L_frame; j++)
  {
    i = mult(pos, INV_FAC32);           /* integer part = pos * 1/32 */
    frac = sub(pos, shl(i, 5));         /* frac = pos - (pos/32)*32   */

    out[j] = Interpol_st(&in[i], filter, frac, 32, L_FDEL_2k);      

    pos = add(pos, 5);                  /* position + 5/32 */
  }

  return;

}

Word16 Interpol_st(      /* o:   interpolated value               Qx  */
  const Word16 *x,       /* i:   input vector                     Q0  */
  const Word16 *win,     /* i:   interpolation window             Q14 */
  const Word16 frac,     /* i:   fraction (0..up_samp)            Q0  */
  const Word16 up_samp,  /* i:   upsampling factor                Q0  */
  const Word16 nb_coef   /* i:   number of coefficients           Q0  */
)
{
#if (FUNC_INTERPOL_ST_OPT)
  Word16 i;
  const Word16 *c1, *c2, *x1, *x2;
  Word32 L_sum;

  x1 = x;
  x2 = x + 1;
  c1 = win + frac;
  c2 = win + up_samp - frac;

  L_sum = 0L;
  i = 0;
  while (i++ < nb_coef)
  {
    L_sum += ((*x1--) * (*c1)) + ((*x2++) * (*c2));
    c1+=up_samp;
    c2+=up_samp;
  }
  
  return ((L_sum + 0x00002000) >> 14);
  
#else

  Word16 i;
  const Word16 *c1, *c2, *x1, *x2;
  Word32 L_sum;

  x1 = &x[0];                 
  x2 = &x[1];                  
  c1 = &win[frac];            
  c2 = &win[up_samp-frac];    

  L_sum = 0L;                 move32();
  for (i=0; i<nb_coef; i++, c1+=up_samp, c2+=up_samp)
  {
      L_sum = L_mac(L_sum, *x1--, *c1);
      L_sum = L_mac(L_sum, *x2++, *c2);
  }
  L_sum = L_shl(L_sum, 1);      /* saturation can occur here */

  return (round16(L_sum));

#endif
}


/* Crosscorr FIP with Word32 precision*/
void Crosscorr_2(
  Word16* vec1_fx,      /* (i)  : Input vector 1  */
  Word16* vec2_fx,      /* (i)  : Input vector 2  */
  Word16 *result_fx_h,  /* (o)  : Output normalized result vector msb	*/
  Word16 *result_fx_l,  /* (o)  : Output normalized result vector lsb	*/
  Word16 length,        /* (i)  : Length of input vectors */
  Word16 minlag,        /* (i)  : Minimum lag     */
  Word16 maxlag,        /* (i)  : Maximum lag     */
  Word16 *shift_fx,     /* The right_shifts, needed in glev_s()*/
  Word16 crosscorr_type /* 0=autocorrelation, 1=crosscorrelation*/
)
{
#if (FUNC_CROSSCORR_2_OPT)

  Word16 i,j,k;
  Word32 t_L, Ltmp, t_L_max=0, res_tmp[ECU_WIEN_ORD+1];
  Word16 vec1_fx_max=0;
  Word16 vec2_fx_max=0;
  Word16 tmp_16, shf, Li;
  Word16 norm;

  /* Find the maximum values of vec1_fx and vec2_fx in order to find the optimum shift*/
  for(i=0;i<length;i++)
  {
    tmp_16 = abs_s(vec1_fx[i]);
    if(tmp_16 > vec1_fx_max)
    {
      vec1_fx_max = tmp_16;
    }
    tmp_16 = abs_s(vec2_fx[i]);
    if(tmp_16 > vec2_fx_max)
    {
      vec2_fx_max = tmp_16;
    }
  }
  
  Ltmp = (vec1_fx_max * vec2_fx_max) << 1;
  norm = norm_l(Ltmp);
  /* The "optimal" shift*/
  *shift_fx = 6 - norm;     //6 is for avoiding overflow when adding 48 Word32:s, 2^6 = 64
//  shf = shr_r(*shift_fx, 1);    //saturation may occur here--shanrong
  shf = (*shift_fx + 1) >> 1;
  /* The crosscorr function*/
  k = 0;
  for (i=minlag; i < 0; i++)
  {
    t_L = 0;
    Li = length + i;
    for (j = Li - 1; j >= 0; j--)
    {
      t_L += (vec1_fx[j] >> shf) * (vec2_fx[j-i] >> shf);
    }
    t_L <<= 1;
    Ltmp = L_abs(t_L);
    if(Ltmp > t_L_max)
    {
      t_L_max = Ltmp;
    }
    res_tmp[k] = t_L;
    k++;
  }

  for (i=0; i < maxlag; i++)  
  {
    t_L = 0;
    Li = length - i;
    for (j = Li - 1; j >= 0; j--)
    {
      t_L += shr(vec1_fx[j+i],shf) * shr(vec2_fx[j],shf);
    }
    t_L <<= 1;
    Ltmp = L_abs(t_L);
    if(Ltmp > t_L_max)
    {
      t_L_max = Ltmp;
    }
    res_tmp[k] = t_L;
    k++;
  }

  /* Normalize the result*/
  norm = norm_l(t_L_max);
  /* Shift one step less if it is a crosscorrelation calculation. This is due to that z[0] must be smaller than r[0] in glev_s()*/
  norm -= crosscorr_type;
  for(i=0;i<maxlag-minlag;i++)
  {
    t_L = L_shl(res_tmp[i],norm);
//    voAMRWBPDecL_Extract(t_L,&result_fx_h[i],&result_fx_l[i]);
	result_fx_h[i] = t_L >> 16;
	result_fx_l[i] = (t_L & 0xffff) >> 1;
  }
  
  /* Adjust shift_fx */
  *shift_fx -= norm;

  return;

#else

  Word16 i,j,k;
  Word32 t_L, Ltmp, t_L_max=0, res_tmp[ECU_WIEN_ORD+1];
  Word16 vec1_fx_max=0;
  Word16 vec2_fx_max=0;
  Word16 tmp_16, shf, Li;
  Word16 norm;

  move32();
  /* Find the maximum values of vec1_fx and vec2_fx in order to find the optimum shift*/
  for(i=0;i<length;i++)
  {
    tmp_16 = abs_s(vec1_fx[i]);
    
    if(sub(tmp_16,vec1_fx_max) > 0)
    {
      vec1_fx_max = tmp_16;     
    }
    tmp_16 = abs_s(vec2_fx[i]);
    if(sub(tmp_16,vec2_fx_max) > 0)
    {
      vec2_fx_max = tmp_16;   
    }
  }
  Ltmp = L_mult(vec1_fx_max,vec2_fx_max);
  norm = norm_l(Ltmp);
  /* The "optimal" shift*/
  *shift_fx = sub(6,norm);    /* 6 is for avoiding overflow when adding 48 Word32:s, 2^6 = 64*/
  shf = shr_r(*shift_fx, 1);
  /* The crosscorr function*/
  k = 0;  
  for (i=minlag; i < 0; i++)
  {
    t_L = 0;      move32();
    Li = add(length,i);
    for (j = 0; j < Li; j++)
    {
      t_L = L_mac(t_L, shr(vec1_fx[j],shf), shr(vec2_fx[j-i],shf));
    }
    Ltmp = L_abs(t_L);
    
    if(L_sub(Ltmp,t_L_max) > 0)
    {
      t_L_max = Ltmp;   move32();
    }
    res_tmp[k] = t_L;   move32();
    k++;
  }

  for (i=0; i < maxlag; i++)  
  {
    t_L = 0;        move32();
    Li = sub(length,i);
    for (j = 0; j < Li; j++)	
    {
      t_L = L_mac(t_L, shr(vec1_fx[j+i],shf), shr(vec2_fx[j],shf));
    }
    Ltmp = L_abs(t_L);
    
    if(L_sub(Ltmp,t_L_max) > 0)
    {
      t_L_max = Ltmp;   move32();
    }
    res_tmp[k] = t_L;   move32();
    k++;
  }

  /* Normalize the result*/
  norm = norm_l(t_L_max);
  /* Shift one step less if it is a crosscorrelation calculation. This is due to that z[0] must be smaller than r[0] in glev_s()*/
  norm = sub(norm,crosscorr_type);
  for(i=0;i<maxlag-minlag;i++)
  {
    t_L = L_shl(res_tmp[i],norm);
    voAMRWBPDecL_Extract(t_L,&result_fx_h[i],&result_fx_l[i]);
  }
  /* Adjust shift_fx */
  *shift_fx = sub(*shift_fx,norm);      
  return;
#endif
}


Word16 Glev_s(Word16 *b_fx,     /* output: filter coefficients */
  Word16 *Rh,     /* input : normalized vector of autocorrelations msb  */
  Word16 *Rl,     /* input : normalized vector of autocorrelations lsb  */
  Word16 *Zh,     /* input: normalized vector of cross correlations msb */
  Word16 *Zl,     /* input: normalized vector of cross correlations msb */
  Word16   m,     /* input : order of filter            */
  Word16 shift    /* The shift needed to correct the output due to that R and Z may have been shifted differently*/
)
{
  Word16 i,j;
  Word32 t0,t1,t2;
  Word16 Ah[ECU_WIEN_ORD+1], Al[ECU_WIEN_ORD+1];
  Word16 Bh[ECU_WIEN_ORD+1], Bl[ECU_WIEN_ORD+1];
  Word16 Anh[ECU_WIEN_ORD+1], Anl[ECU_WIEN_ORD+1];
  Word16 Kh, Kl;
  Word16 hi,lo, i1;
  Word16 alp_h, alp_l, alp_exp;
  Word16 rc_fx[ECU_WIEN_ORD+1];



  /* K = A[1] = -R[1] / R[0] */

  t1 = voAMRWBPDecL_Comp(Rh[1], Rl[1]);             /* R[1] in Q31      */
  t2 = L_abs(t1);                        /* abs R[1]         */
  
  if(Rh[0] == 0 && Rl[0] == 0)
  {
    Rh[0] = 32767;    
  }
  t0 = voAMRWBPDecDiv_32(t2, Rh[0], Rl[0]);         /* R[1]/R[0] in Q31 */
  
  if (t1 > 0)
  {
      t0 = L_negate(t0);                /* -R[1]/R[0]       */
  }
  voAMRWBPDecL_Extract(t0, &Kh, &Kl);               /* K in DPF         */
  rc_fx[0] = Kh;                            
  t0 = L_shr(t0, 4);                     /* A[1] in Q27      */
  voAMRWBPDecL_Extract(t0, &Ah[1], &Al[1]);         /* A[1] in DPF      */

  /* Alpha = R[0] * (1-K**2) */

  t0 = Mpy_32(Kh, Kl, Kh, Kl);           /* K*K      in Q31 */
  t0 = L_abs(t0);                        /* Some case <0 !! */
  t0 = L_sub((Word32) 0x7fffffffL, t0);  /* 1 - K*K  in Q31 */
  voAMRWBPDecL_Extract(t0, &hi, &lo);               /* DPF format      */
  t0 = Mpy_32(Rh[0], Rl[0], hi, lo);     /* Alpha in Q31    */

  /* Normalize Alpha */

  alp_exp = norm_l(t0);
  t0 = L_shl(t0, alp_exp);
  voAMRWBPDecL_Extract(t0, &alp_h, &alp_l);
  /* DPF format    */



  /* b[0]=z[0]/r[0];*/

  t1 = voAMRWBPDecL_Comp(Zh[0], Zl[0]);             /* Z[0] in Q31      */
  t2 = L_abs(t1);                        /* abs Z[0]         */
  t0 = voAMRWBPDecDiv_32(t2, Rh[0], Rl[0]);         /* Z[0]/R[0] in Q31 */
  
  if (t1 < 0)
  {
      t0 = L_negate(t0);
  }
  t0 = L_shr(t0, 4);                     /* B[0] in Q27      */
  voAMRWBPDecL_Extract(t0, &Bh[0], &Bl[0]);         /* B[0] in DPF      */

  for (i = 2; i <= m; i++)
  {
  
    t0 = 0;                            move32();
    i1 = sub(i,1);
    for (j = 0; j < i1; j++)
    {
        t0 = L_add(t0, Mpy_32(Rh[i1-j], Rl[i1-j], Bh[j], Bl[j]));
    }
    t0 = L_shl(t0, 4);                 /* result in Q27 -> convert to Q31 */

    /*b[i-1]=(z[i-1]-t)/err;*/
    
    t1 = voAMRWBPDecL_Comp(Zh[i1],Zl[i1]);
    t1 = L_sub(t1,t0);            /* z[i-1]-t in Q31 */
    t2 = L_abs(t1);               /* abs(z[i-1]-t) */
    t0 = voAMRWBPDecDiv_32(t2, alp_h, alp_l);     /* abs(z[i-1]-t)/Alpha                   */
    
    if (t1 < 0)
    {
        t0 = L_negate(t0);             
    }
    t0 = L_shl(t0, sub(alp_exp,4));           /* denormalize; compare to Alpha and adjust to Q27  */
    voAMRWBPDecL_Extract(t0, &Bh[i-1], &Bl[i-1]);         /* B[i-1] in DPF      */

    for(j=0;j<i1;j++)
    {
      t0 = voAMRWBPDecL_Comp(Bh[j],Bl[j]);
      t1 = Mpy_32(Bh[i1],Bl[i1],Ah[i1-j],Al[i1-j]); /*Q27*Q27->Q23 */
      t1 = L_shl(t1,4); /* Q23->Q27 */
      t0 = L_add(t0,t1); /* Q27 + Q27 */
      voAMRWBPDecL_Extract(t0,&Bh[j],&Bl[j]);
    }
    
    if(sub(i,m) == 0)
    {
      break; /* Break here since we do not need more A values */
    } 
    
    /* t0 = SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i] */
    t0 = 0;                            move32();
    for (j = 1; j < i; j++)
    {
        t0 = L_add(t0, Mpy_32(Rh[j], Rl[j], Ah[i - j], Al[i - j]));
    }
    t0 = L_shl(t0, 4);                 /* result in Q27 -> convert to Q31 */
    /* No overflow possible */
    t1 = voAMRWBPDecL_Comp(Rh[i], Rl[i]);
    t0 = L_add(t0, t1);                /* add R[i] in Q31                 */

    /* K = -t0 / Alpha */

    t1 = L_abs(t0);
    t2 = voAMRWBPDecDiv_32(t1, alp_h, alp_l);     /* abs(t0)/Alpha                   */
    
    if (t0 > 0)
    {
        t2 = L_negate(t2);             /* K =-t0/Alpha                    */
    }
    t2 = L_shl(t2, alp_exp);           /* denormalize; compare to Alpha   */
    voAMRWBPDecL_Extract(t2, &Kh, &Kl);           /* K in DPF                        */
    rc_fx[i - 1] = Kh;                    

    /*------------------------------------------
      Compute new LPC coeff. -> An[i]         
      An[j]= A[j] + K*A[i-j]     , j=1 to i-1 
      An[i]= K                                
      ------------------------------------------*/

    for (j = 1; j < i; j++)
    {
        t0 = Mpy_32(Kh, Kl, Ah[i - j], Al[i - j]);
        t0 = L_add(t0, voAMRWBPDecL_Comp(Ah[j], Al[j]));
        voAMRWBPDecL_Extract(t0, &Anh[j], &Anl[j]);
    }
    t2 = L_shr(t2, 4);                 /* t2 = K in Q31 ->convert to Q27  */
    voAMRWBPDecL_Extract(t2, &Anh[i], &Anl[i]);   /* An[i] in Q27                    */

    /* Alpha = Alpha * (1-K**2) */
    t0 = Mpy_32(Kh, Kl, Kh, Kl);       /* K*K      in Q31 */
    t0 = L_abs(t0);                    /* Some case <0 !! */
    t0 = L_sub((Word32) 0x7fffffffL, t0);   /* 1 - K*K  in Q31 */
    voAMRWBPDecL_Extract(t0, &hi, &lo);           /* DPF format      */
    t0 = Mpy_32(alp_h, alp_l, hi, lo); /* Alpha in Q31    */
    
    if(t0 <= 0)
    {
      t0 = 21474836; /* ~0.01 in Q31*/
    }

    /* Normalize Alpha */
    j = norm_l(t0);
    t0 = L_shl(t0, j);
    voAMRWBPDecL_Extract(t0, &alp_h, &alp_l);     /* DPF format    */
    alp_exp = add(alp_exp, j);         /* Add normalization to alp_exp */

    /* A[j] = An[j] */
    for (j = 1; j <= i; j++)
    {
        Ah[j] = Anh[j];                
        Al[j] = Anl[j];                
    }
  }

  for(j=0;j<ECU_WIEN_ORD+1;j++)
  {
    t0 = voAMRWBPDecL_Comp(Bh[j],Bl[j]); /* Q27*/
    /* Shift to Q15 by considering that R and Z may have been shifted differently*/
    b_fx[j] = round16(L_shl(t0,sub(4,shift))); /* Q27->Q15*/
  }
  return 0;
}

/* this supposes time alignement between encoder and decoder */
void Dec_filt(
  Word16  *prm,
  Word16 wh[],        /* Q15*/
  Word16 old_wh[],    /* Q15*/
  Word16 bfi,
  const PMSVQ_fx*	filt_hi_pmsvq)       
{
  /* A concatenation of pmsvq_inv and mspq_inv*/
  const Word16 **cb = filt_hi_pmsvq->msvq.cbs;
  const Word16 *cbm = filt_hi_pmsvq->mean;
  const Word16 a =filt_hi_pmsvq->a;
  const Word16 vdim =filt_hi_pmsvq->msvq.vdim;
  const Word16 stages = filt_hi_pmsvq->msvq.nstages;
  Word16 eq[HI_FILT_ORDER];
  Word16 i,l;
  
  Word32 L_tmp;
  
  
  if (!bfi) 
  {
    for (i=0;i<vdim;i++) 
    {
      eq[i] = cb[0][prm[0] * vdim + i];   

      for (l=1;l<stages;l++)  
      {
        eq[i] = add(eq[i],cb[l][extract_l(L_shr(L_mult(prm[l], vdim),1)) + i]);    
      }
    }
  }
  else
  {
    for (i=0;i<vdim;i++) 
    {
      eq[i] = 0;    
    }
  }


  for(i=0;i<vdim;i++)
  {
    L_tmp = L_mac(L_mult(eq[i], 16384),old_wh[i],a); 
    old_wh[i] = round16(L_tmp);                 
    L_tmp = L_mac(L_tmp,cbm[i],16384);    
    wh[i] = round16(L_tmp);                 

  }
}


void Dec_gain(
  Word16  *prm,
  Word16 *gain_left,  /* Q10*/
  Word16 *gain_right, /* Q10*/
  Word16 *old_gain,   /* Q10*/
  Word16  bfi,
  const PMSVQ_fx*	Gain_hi_pmsvq_fx)
{
  Word16 tmp[2];
  /* A concatenation of pmsvq_inv and mspq_inv*/
  const Word16 **cb = Gain_hi_pmsvq_fx->msvq.cbs;
  const Word16 *cbm = Gain_hi_pmsvq_fx->mean;
  const Word16 a =Gain_hi_pmsvq_fx->a;
  const Word16 vdim =Gain_hi_pmsvq_fx->msvq.vdim;
  Word16 eq[HI_FILT_ORDER];
  Word16 i;

  
  if (!bfi) 
  {
    for (i=0;i<vdim;i++) 
    {
      eq[i] = cb[0][prm[0] * vdim + i];   
    }
  }
  else
  {
    for (i=0;i<vdim;i++) 
    {
      eq[i] = 0;    
    }
  }

  for(i=0;i<vdim;i++)
  {
    old_gain[i] = round16(L_mac(L_mult(eq[i], 32767),old_gain[i],a));     
    tmp[i] = add(old_gain[i],cbm[i]);                

  }

  *gain_left  = tmp[0];   
  *gain_right = tmp[1];   
}


void Get_exc_win(
  Word16 *exc_ch,    /* out*/
  Word16 *buf,       /* in*/
  Word16 *exc_mono,  /* in */
  Word16 *side_buf,  /* in*/
  Word16 *win,       /* in*/
  Word16 gain[],     /* gain*/
  Word16 N,          /* N samples*/
  Word16 doSum)      /* 1 = sum, 0 = subtract*/
{
#if (FUNC_GET_EXC_WIN_OPT)
  Word16 n;
  
  Word16 tmp1;
  Word32 L_tmp;
  
  
  if (doSum>0) 
  {
    for (n=0; n < N; n++)
    {
      tmp1 = (gain[0] * win[n]) >> 15;
      L_tmp = tmp1 * exc_mono[n];
      L_tmp += tmp1 * buf[n];
      tmp1 = 32767 - win[n];                /*1-win*/
      tmp1 = mult(tmp1, gain[1]);               /* G(1-win)*/
      L_tmp += tmp1 * exc_mono[n];
      L_tmp += tmp1 * side_buf[n];
      exc_ch[n] = round16(L_shl(L_tmp,4));
    }
  } 
  else 
  {
    for (n=0; n < N; n++) 
    {
      tmp1 = (win[n] * gain[0]) >> 15;
      L_tmp = tmp1 * exc_mono[n];
      L_tmp -= tmp1 * buf[n];
      tmp1 = 32767 - win[n];                /*1-win*/
      tmp1 = (tmp1 * gain[1]) >> 15;               /* G(1-win)*/
      L_tmp += tmp1 * exc_mono[n];
      L_tmp -= tmp1 * side_buf[n];
      exc_ch[n] = round16(L_shl(L_tmp, 4));
    }
  }
  
#else

  Word16 n;
  
  Word16 tmp1;
  Word32 L_tmp;
  
  
  if (doSum>0) 
  {
    for (n=0; n < N; n++)
    {
      tmp1 = mult(gain[0], win[n]);
      L_tmp = L_mult(tmp1, exc_mono[n]);
      L_tmp = L_mac(L_tmp, tmp1, buf[n]);
      tmp1 = sub(32767, win[n]);                /*1-win*/
      tmp1 = mult(tmp1, gain[1]);               /* G(1-win)*/
      L_tmp = L_mac(L_tmp, tmp1, exc_mono[n]);
      L_tmp = L_mac(L_tmp, tmp1, side_buf[n]);
      exc_ch[n] = round16(L_shl(L_tmp,3));    
    }
  } 
  else 
  {
    for (n=0; n < N; n++) 
    {
      tmp1 = mult(win[n], gain[0]);
      L_tmp = L_mult(tmp1, exc_mono[n]);
      L_tmp = L_msu(L_tmp, tmp1, buf[n]);
      tmp1 = sub(32767, win[n]);                /*1-win*/
      tmp1 = mult(tmp1, gain[1]);               /* G(1-win)*/
      L_tmp = L_mac(L_tmp,tmp1, exc_mono[n]);
      L_tmp = L_msu(L_tmp, tmp1, side_buf[n]);
      exc_ch[n] = round16(L_shl(L_tmp,3));    
    }
  }
#endif
}



void Get_exc(
  Word16 *exc_ch,        /* out*/
  Word16 *exc_mono,      /* in*/
  Word16 *side_buf,      /* in*/
  Word16 gain,           /* gain*/
  Word16 N,              /* N samples*/
  Word16 doSum)          /* 1 = sum, 0 = subtract*/
{
#if (FUNC_GET_EXC_OPT)

  Word16 n;

  Word32 L_tmp;

  if (doSum>0) 
  {  
    for(n=0; n < N; n++) 
    {
      L_tmp = gain * exc_mono[n] + side_buf[n] * gain;
      exc_ch[n] = (L_shl(L_tmp,4) + 0x8000) >> 16;
    }
  }
  else 
  {
    for(n=0; n < N; n++) 
    {
      L_tmp = gain * exc_mono[n] - side_buf[n] * gain;
      exc_ch[n] = round16(L_shl(L_tmp,4));
    }
  }

#else

  Word16 n;

  Word32 L_tmp;

  
  if (doSum>0) 
  {  
    for(n=0; n < N; n++) 
    {
      L_tmp = L_mult(gain ,exc_mono[n]);
      L_tmp = L_mac(L_tmp, side_buf[n], gain);
      exc_ch[n] = round16(L_shl(L_tmp,3));    
    }
  } 
  else 
  {
    for(n=0; n < N; n++) 
    {
      L_tmp = L_mult(gain ,exc_mono[n]);
      L_tmp = L_msu(L_tmp, side_buf[n], gain);
      exc_ch[n] = round16(L_shl(L_tmp,3));    
    }
  }
#endif

}


Word16 Comp_hi_gain(Word16 gcode0)   /* Input in Q10, output Q13 */
{ 
  /*-----------------------------------------------------------------*
   * gcode0 = pow(10.0, gcode0/20)                                   *
   *        = pow(2, 3.321928*gcode0/20)                             *
   *        = pow(2, 0.166096*gcode0)                                *
   *-----------------------------------------------------------------*/
  Word32 L_tmp;
  Word16 exp_gcode0,frac;

  L_tmp = L_mult(gcode0, 5443);             /* *0.166096 in Q15 -> Q24     */
  L_tmp = L_shr(L_tmp, 10);                 /* From Q24 to Q16             */
  voAMRWBPDecL_Extract(L_tmp, &exp_gcode0, &frac);     /* Extract exponant of gcode0  */

  gcode0 = extract_l(voAMRWBPDecPow2(12, frac));       /* Put 12 as exponant so that  Saturate if gain over 8 */
  gcode0 = round16(L_shl(gcode0, add(exp_gcode0, 16)));   

  return gcode0;
};


static Word16 D_Balance(     /* output: gain                    */
  Word16 index      /* (i)  : index                    */
)
{
  Word16 balance_fx, tmp;

  /*float gain = (float) index / 32.0f  - 2.0f; */
  tmp = shl(index,8);
  balance_fx = shl(sub(tmp,16384),1);

  return balance_fx; /* Q14 */
}

void Comp_gain_shap(
  Word16 *wm, 
  Word16 *gain_shap, 
  Word16 lg, 
  Word16 Qwm
)
{
  Word16 i,j,k, inv_lg16;
  Word16 tmp, expt, expt2, frac, log_sum;
  Word32 L_tmp;
  log_sum = 0;
  
  /* 48*/
  {
    inv_lg16 = 5461; 
  }

  j = 0;    
  for(i=0;i<lg;i+=16)
  {      
    L_tmp = 0;    move32();
    for(k=0;k<16;k++) 
    {
      L_tmp = L_mac(L_tmp,wm[i+k],wm[i+k]);	
    }
    
    if(L_tmp ==0)
    {
      L_tmp = 1; move32();
    }

    voAMRWBPDecLog2(L_tmp,&expt,&frac);
    expt = sub(expt, add(shl(Qwm,1),1));
    gain_shap[j] = add(shl(expt,8),shr(frac,7));  
    log_sum = add(log_sum,gain_shap[j]);  /* Q8*/
    gain_shap[j] = mult_r(gain_shap[j],16384);    /* 0.5 = sqrt in log domain */
    j++;/*ptr*/
  }
  log_sum = mult_r(log_sum,inv_lg16); /* 0.33333*0.5 in Q15, */
                               /* Result in Q8, 0.5 = sqrt op*/

  expt = shr(log_sum,8);  /* extract exponent*/
  frac = shl(sub(log_sum,shl(expt,8)),7); /* extract fraction  */
  L_tmp = voAMRWBPDecPow2(expt,frac);
  log_sum = extract_h(voAMRWBPDecPow2(30,frac));
  log_sum = div_s(16384,log_sum);
  j = 0;        
  for(i=0;i<lg;i+=16) 
  {
    tmp = gain_shap[j];
    expt2 = shr(tmp,8);  /* extract exponent*/
    frac = shl(sub(tmp,shl(expt2,8)),7); /* extract fraction  */
    tmp = extract_h(voAMRWBPDecPow2(30,frac));
    gain_shap[j] = mult(tmp,log_sum);     
    
    if (sub(expt2,expt) > 0) 
    {
      gain_shap[j] = shl(gain_shap[j],sub(expt2,expt));   
    } 
    else 
    {
      gain_shap[j] = shr(gain_shap[j],sub(expt,expt2));   
    }
    j ++; /*ptr*/

  }
}


void Windowing(Word16 length, Word16 *vec, Word16 *window, Word16 offset_vec, Word16 offset_win)
{
  Word16 i;
  for (i=0; i<length; i++) 
  {
    vec[i+offset_vec] = mult_r(vec[i+offset_vec],window[i+offset_win]);    
  }
  return;
}

void Apply_xnq_gain2(Word16 lg, Word16 wbalance, Word32 Lgain, Word16 *xnq_fx, Word16 *wm_fx, Word16 Q_syn)
{

  Word16 i, tmp16,tmpgain, tmp_scale;
  Word32 L_tmp1,L_tmp2;

  tmp16 = norm_l(Lgain);
  tmpgain = extract_h(L_shl(Lgain, tmp16));
  tmp16 = sub(tmp16, 16);
  tmp_scale = sub(sub(Q_syn, tmp16),1);
  for(i=0; i<lg; i++) 
  {
    L_tmp1 = L_mult(wbalance,wm_fx[i]); /*  Q14+Qsyn */
    L_tmp2 = L_mult(tmpgain, xnq_fx[i]);  
    xnq_fx[i]= round16(L_add(L_shl(L_tmp2, tmp_scale), L_shl(L_tmp1,1)));    /* Q_syn */
  }
  return;
}


void Apply_tcx_overlap(Word16 *xnq_fx, Word16 *wovlp_fx, Word16 lext, Word16 L_frame)
{
  Word16 i;
  Word32 Ltmp;
  for (i=0; i<L_OVLP_2k; i++) 
  {
    Ltmp = L_mult(xnq_fx[i], 32767);
    Ltmp = L_mac(Ltmp,  wovlp_fx[i], 32767);
    xnq_fx[i] = round16(Ltmp);   
  }

  /* save overlap for next frame */
  for (i=0; i<lext; i++) 
  {
    wovlp_fx[i] = xnq_fx[i+L_frame];      
  }

  for (i=lext; i<L_OVLP_2k; i++) 
  {
    wovlp_fx[i] = 0;    
  }

  return;
}


void Apply_gain_shap(
  Word16 lg, 
  Word16 *xnq_fx,     /*Qsyn*/
  Word16 *gain_shap_fx /* Q14*/
)
{
  Word16 i,k, is4;
  Word32 Ltmp;

  for(i=0;i<lg;i+=16)
  {
    is4 = shr(i,4);
    for(k=0;k<16;k++) 
    {
      Ltmp = L_mult(gain_shap_fx[is4], xnq_fx[i+k]);
      Ltmp = L_shl(Ltmp,1);
      xnq_fx[i+k]= round16(Ltmp);
    }
  }

  return;
}

void Apply_wien_filt(
  Word16 lg,  
  Word16 *xnq_fx,   /* Qsyn*/
  Word16 *h_fx,     /*Q15*/
  Word16 *wm_fx     /*Qsyn*/
)
{
  Word16 i,k;
  Word32 L_tmp;

  for(i=0;i<lg;i++) 
  {
    L_tmp=0;

    for (k=0;k<ECU_WIEN_ORD+1;k++) 
    {
      L_tmp = L_mac(L_tmp,h_fx[k],wm_fx[i-k]); 
    }
    xnq_fx[i] = round16(L_tmp);
  }

return;
}



void Ch_sep(Word16 *synth_side_fx, Word16 alpha_fx,Word16 *synth_2k_fx, Word16 *left_2k_fx, Word16 *right_2k_fx, Word16 start, Word16 end)
{

  Word16 i;
  for(i=start;i<end;i++)
  {
    left_2k_fx[i] = add(synth_2k_fx[i], synth_side_fx[i]); 
    right_2k_fx[i] = sub(synth_2k_fx[i], synth_side_fx[i]); 
  }

  return;
}

Word16 Balance(Word16 bad_frame,Decoder_State_Plus_fx *st,Word16 prm)
{
  Word16 result_fx;
  
  
  if (bad_frame > 0)
  {
    result_fx = mult_r(st->mem_balance_fx , 29491); /*0.9 in Q15 */
    st->mem_balance_fx = result_fx;      
  }
  else 
  {
    /* panning is safe */
    result_fx = D_Balance(prm);
    st->mem_balance_fx  = result_fx;    
  }

  return result_fx;

}
void Mix_ch(
  Word16 *ch_left,	/* input: samples from left channel */
  Word16 *ch_right,	/* input: samples from right channel */
  Word16 *ch_sum,		/* output: mixed mono signal */
  Word16 n,				  /* input: length of frame */	
  Word16 gA,			  /* input: weight factor for left channel Q14 */
  Word16 gB			    /* input: weight factor for right channel Q14 (avoid multiply by 0.5) */
)
{
  Word16 i;
  Word32 Ltmp;

  for (i=0;i<n;i++)
  { 
    Ltmp  = L_mult(gA, ch_left[i]);
    ch_sum[i] = round16(L_mac(Ltmp, gB, ch_right[i]));    
  }
}
