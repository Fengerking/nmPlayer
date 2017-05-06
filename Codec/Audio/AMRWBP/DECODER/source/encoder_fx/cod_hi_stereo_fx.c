#include <stdlib.h>
#include <stdio.h>
#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"
#include "math_op.h"
#include "log2.h"

#define L2SCL 10
#define SCL3 0
#define MAX_VECT_DIM	16


void Init_cod_hi_stereo(Coder_State_Plus_fx *st)
{
  Set_zero(st->old_exc_mono,HI_FILT_ORDER);
  Set_zero(st->old_wh,HI_FILT_ORDER);		
  Set_zero(st->old_wh_q,HI_FILT_ORDER);	
  Set_zero(st->old_gm_gain,2);			
  st->filt_energy_threshold = 0;   move16();

}


static void Compute_exc_side(Word16 exc_side[] ,Word16 exc_mono[])
{
   Word16 i;
   for(i=0;i<L_FRAME_PLUS+L_SUBFR;i++)
   {
      exc_side[i] = sub(exc_mono[i],exc_side[i]);   move16();
   }
}

static void Compute_cross_corr_vector(Word16 x[],Word16 y[], Word16 c[], Word16 *shift_fx)
{
   Word16 k,t,i;
   Word32 Lc[HI_FILT_ORDER];
   Word32 Lc_max;
   Word16 tmp;
   Word32 Ltmp;
   Word16 x_max = 0, y_max = 0;
   Word16 norm_x,norm_y, norm;
   Word16 shf_x = 0, shf_y = 0;

   /* Initialisation */
   move16();move16();move16();move16();

   for(i=-HI_FILT_ORDER+1;i<L_DIV+L_SUBFR;i++)
   {
      tmp = abs_s(x[i]);
      test();
      if(sub(tmp,x_max) > 0)
      { 
        x_max = tmp;     move16();
      }
   }
   norm_x = norm_s(x_max);
   shf_x  = shr(sub(4,norm_x),1);
   for(i=0;i<L_DIV+L_SUBFR;i++)
   {
      tmp = abs_s(y[i]);
      test();
      if(sub(tmp,y_max) > 0)
      { 
        y_max = tmp;     move16();
      }
   }
   norm_y = norm_s(y_max);
   shf_y = shr(sub(4,norm_y),1);

   Lc_max = 0;      move32();
   /* compute cross-correlation terms */
   for (k=0;k<HI_FILT_ORDER;k++)
   {
      Ltmp = 0;      move32();
      for(t=0;t<L_DIV+L_SUBFR;t++)
      {
        Ltmp = L_mac(Ltmp,shr(y[t],shf_y),shr(x[t-k],shf_x));
      }
      Lc[k] = Ltmp; move32();
      Ltmp = L_abs(Ltmp);
      test();
      if (L_sub(Ltmp,Lc_max) > 0) 
      {
         Lc_max = Ltmp; move32();
      }
   }
   norm = norm_l(Lc_max);
   for (k=0;k<HI_FILT_ORDER;k++)
   {
      c[k] = round(L_shl(Lc[k],norm));      move16();
   }
   *shift_fx = sub(norm,add(shf_x,shf_y));      move16();
}

static void Compute_auto_corr_matrix(Word16 x[],Word16 r[HI_FILT_ORDER][HI_FILT_ORDER], Word16 r_shift[HI_FILT_ORDER])
{
   Word16 i,j,k,t;
   Word32 Lc[HI_FILT_ORDER];
   Word32 Lc_max;
   Word16 tmp;
   Word32 Ltmp;
   Word16 x_max = 0;
   Word16 norm_x,norm;
   Word16 shf_x;


   for(j=-HI_FILT_ORDER+1;j<L_DIV+L_SUBFR;j++)
   {
      tmp = abs_s(x[j]);
      test();
      if(sub(tmp,x_max) > 0)
      { 
        x_max = tmp;     move16();
      }
   }
   norm_x = norm_s(x_max);
   shf_x = shr(sub(4,norm_x),1);
   /* Compute matrix diagonal wise with separate scaling depending */
   /* on distance from diagonal, might be easier with 32 bit numbers completely but one need to*/
   /* pick a solution and live with it*/
   for (k=0;k<HI_FILT_ORDER;k++) 
   { /* distance from diagonal*/
      Lc_max = 0;       move32();
      for (i=k,j=0;j<HI_FILT_ORDER-k;i++,j++) /* Column*/
      {
         Ltmp = 0;     move32();
         for(t=0;t<L_DIV+L_SUBFR;t++) 
         {
            Ltmp = L_mac(Ltmp,shr(x[t-i],shf_x),shr(x[t-j],shf_x));
         }
         Lc[j] = Ltmp; move32();
         Ltmp = L_abs(Ltmp);
         test();
         if (L_sub(Ltmp,Lc_max) > 0) 
         {
            Lc_max = Ltmp; move32();
         }
      }
      norm = norm_l(Lc_max);
      r_shift[k] = sub(norm,shl(shf_x,1));    move16();
      for (i=k,j=0;j<HI_FILT_ORDER-k;i++,j++) /* Column */
      {
         r[i][j] = round(L_shl(Lc[j],norm));     move16();
         r[j][i] = r[i][j];     move16();

      }
   }
}



static Word16 Cholsolc(
    Word16 r[HI_FILT_ORDER][HI_FILT_ORDER],
    Word16 r_scl[HI_FILT_ORDER], /* scaling for diagonal elements positive->actually lower value*/
    Word16 c[HI_FILT_ORDER],
    Word16 h[HI_FILT_ORDER],
    Word16 scale_fx
)
{

    Word16 i,j,k;
    Word16 tmp,tmp1,tmp2,sgn,norm_sum,norm_lambda,sum,lambda;
    Word32 Lsum;
    Word32 Llambda;
    Word32 Lsum2;
    Word32 Llambda2;
    Word16 p_inv[HI_FILT_ORDER];
    Word16 p_inv_e[HI_FILT_ORDER];
    Word16 v[HI_FILT_ORDER];
    Word16 exp;
    Word32 Ltmp;
    /* cholesky decomposition */
    for (i=0;i<HI_FILT_ORDER;i++)
    {
      for (j=i;j<HI_FILT_ORDER;j++)
      {	
        tmp = sub(6-2,r_scl[abs_s(sub(i,j))]);
        Lsum = L_shl(r[i][j],tmp); 
        for (k=i-1; k>=0; k--)
        { 
          tmp1 = sub(add(r_scl[abs_s(sub(i,k))],r_scl[abs_s(sub(j,k))]),2); /* distance from diagonal, determines scaling*/
          Ltmp = L_shr(L_mult(r[i][k],r[j][k]),sub(tmp1,5-4)); /* must be brought to same exp as diagonal elements*/
          Lsum = L_sub(Lsum,Ltmp);		
        }
        test();
        if (sub(i,j)==0)
        {
          test();
          if (L_sub(Lsum,10) < 0) 
          {
            return 1;
          }
          Lsum = Isqrt(Lsum);
          exp = norm_l(Lsum);
          Lsum = L_shl(Lsum,exp);
          p_inv_e[i] = add(exp,1);      move16();
          p_inv[i] = round(Lsum);       move16();
        }
        else
        {
          L_Extract(Lsum,&tmp1,&tmp2);
          tmp = r_scl[abs_s(sub(j,i))]; /* distance from diagonal determines scaling*/
          Ltmp = L_shr(Mpy_32_16(tmp1,tmp2,p_inv[i]),-SCL3+sub(p_inv_e[i],add(13+2,tmp))); 

          r[j][i] = round(Ltmp);    move16(); 
        }
      }
    }
    /* scaling of off diaonal elements is modified above */
    for (i=1;i<HI_FILT_ORDER;i++) 
    {
      r_scl[i] = r_scl[i];      move16();
    }
    /* linear system solving */
    for (i=0;i<HI_FILT_ORDER;i++)
    {
      Lsum = L_mult(c[i],2);
      Llambda = 0x0fffff;                 move32();
      for (k=i-1;k>=0;k--)
      {
        tmp = r_scl[abs_s(sub(i,k))]; move16();/* distance from diagonal determines scaling*/
        Lsum = L_sub(Lsum,L_shr(L_mult(r[i][k],h[k]),add(tmp,3)));
        Llambda = L_sub(Llambda,L_shr(L_mult(r[i][k],v[k]),sub(tmp,2)));
      }
      L_Extract(Lsum,&tmp1,&tmp2);
      h[i] = round(L_shr(Mpy_32_16(tmp1,tmp2,p_inv[i]),sub(p_inv_e[i],21)));      move16();/* Q5*/

      L_Extract(Llambda,&tmp1,&tmp2);
      v[i] = round(L_shr(Mpy_32_16(tmp1,tmp2,p_inv[i]),sub(p_inv_e[i],16)));        move16();/*lambda/p[i];*/
      
    }
    Llambda2 = 0;       move32();
    Lsum2 = 0;          move32();
    for (i=HI_FILT_ORDER-1; i >= 0; i--)
    {
      Lsum = L_mult(h[i],1024);
      Llambda = L_mult(v[i], 512); 
      for (k=i+1;k<HI_FILT_ORDER;k++)
      {
        tmp = r_scl[abs_s(sub(i,k))];    /* distance from diagonal determines scaling*/
        Lsum = L_sub(Lsum,L_shr(L_mult(r[k][i],h[k]),sub(tmp,3)));
        Llambda = L_sub(Llambda,L_shr(L_mult(r[k][i],v[k]),sub(tmp,6)));
      }
      L_Extract(Lsum,&tmp1,&tmp2);
      Lsum = L_shr(Mpy_32_16(tmp1,tmp2,p_inv[i]),sub(p_inv_e[i],15));
      h[i] = round(Lsum);                                       move16(); /* Q13*/
      Lsum2 = L_add(Lsum2,L_shr(Lsum,3));
      L_Extract(Llambda,&tmp1,&tmp2);
      Llambda = L_shr(Mpy_32_16(tmp1,tmp2,p_inv[i]),sub(p_inv_e[i],12));
      v[i] = round(Llambda);                                    move16();  /*lambda/p[i]*/
      Llambda2 = L_add(Llambda2,L_shr(Llambda,3));
      
    }
    norm_sum = sub(norm_l(Lsum2),1);
    norm_lambda = norm_l(Llambda2);

    sum = round(L_shl(Lsum2,norm_sum));
    lambda = round(L_shl(Llambda2,norm_lambda));
    sgn = mult(sum,lambda);
    sum = abs_s(sum);
    lambda = abs_s(lambda);   
    lambda = div_s(sum,lambda);  /* sum/lambda*/
    test();
    if (sgn > 0) /* switch sign here */ 
    {
      lambda = negate(lambda);
    }  
    sum = 0;      move16();
    for(i=0;i<HI_FILT_ORDER;i++)
    {
      sum = add(sum,shr_r(h[i],scale_fx));
      tmp1 = mult(lambda,v[i]);
      h[i] = shr_r(add(h[i],shl(tmp1,sub(norm_lambda,norm_sum))),sub(scale_fx,2));   move16();
    }
    sum = abs_s(sum);
    test();
    if (sub(sum,2048) > 0) 
    {

      sum = div_s(2048,sum);
      for(i=0;i<HI_FILT_ORDER;i++) 
      {
        h[i] = mult_r(h[i],sum);        move16();
      }
    }
    return 0;

}

static void Cholsolc_repair(Word16 r_0,Word16 c_0,Word16 h[HI_FILT_ORDER],Word16 scale_fx)
{  
    Word16 k;
    Word16 tmp_r,tmp_c;
    Word16 norm_r,norm_c;

    for(k=1;k<HI_FILT_ORDER;k++)
    {
        h[k] = 0;     move16();
    }
    test();
    if(r_0 == 0)
      r_0 = add(r_0,1);

    tmp_c = abs_s(c_0);
    norm_c = sub(norm_s(tmp_c),1);

    norm_r = norm_s(r_0);
    tmp_c = abs_s(shl(tmp_c,norm_c));
    tmp_r = abs_s(shl(r_0,norm_r));
    
    tmp_c = div_s(tmp_c,tmp_r);  
    test();
    if (c_0 < 0) 
    {
        tmp_c = negate(tmp_c);
    }
    h[0] = shl(tmp_c,sub(norm_r,add(norm_c,add(scale_fx,2))));      move16();
}


/* filter smoother */
static void Smooth_ener_filter(
    Word16 filter[],
    Word16 *threshold)
{
  Word16 tmp; 
  Word32 Lener;
  Word16 ener,old_ener;
  Word16 i;
  Word16 norm_tmp, norm_old_ener;
  Word32 Ltmp;
  
  /* compute energy over subframe */
  Lener = 0;      move32();
  for (i=0; i<HI_FILT_ORDER; i++) 
  { 
      tmp = mult_r(filter[i],16384);  /* before :Q13 but <= 14bits  (14+14+4 = 32) */   
                                      /* now remove 1 bit to prevent overflow  (13+13+4 = 30) */
      Lener = L_mac(Lener,tmp,tmp);   /*Q12*Q12 = Q23  */
  }
  ener = round(L_shl(Lener,2+2));   /* Q23->Q11 */
  /* 
      FLP has a saturation level of 16, FIP has a saturation of 4 for
      acceptable performance
  */ 
  
  old_ener = ener;      move16();
  tmp = ener;           move16();
  test();
  if (sub(tmp,*threshold) < 0) 
  {
     tmp = shl(mult(tmp,23170),1);  /* *1.414 */
     if (sub(tmp,*threshold) > 0) 
     {
         tmp = *threshold;      move16();
     }
  }
  else 
  {
     tmp = mult(tmp,23170);         /* *0.707 */
     if (sub(tmp,*threshold) < 0) 
     {
          tmp = *threshold;     move16();
     }
  }

  test();  
  if (tmp == 0)
  {
    tmp = 1;        move16();
  }
  *threshold = tmp;   move16();

  norm_tmp = norm_s(tmp);
  norm_old_ener = sub(norm_s(old_ener),1);
  tmp = shl(tmp,norm_tmp);
  old_ener = shl(old_ener,norm_old_ener);
  tmp = div_s(old_ener,tmp);
  Ltmp = L_shl(L_deposit_l(tmp),add(sub(norm_tmp,norm_old_ener),1));
  Ltmp = Isqrt(Ltmp);
  tmp = round(L_shl(Ltmp,7));

  for (i=0; i<HI_FILT_ORDER; i++) 
  {
      filter[i] = shl(mult_r(tmp,filter[i]),1);     move16();
  }
  return;
}

static void M_cbcod(
    Word16 mpfade[INTENS_MAX][MAX_NUMSTAGES],/* TBD	*/	
    Word32 *dist,                         /* TBD	*/	
    Word16 *x,                            /* TBD	*/
    Word16 *pfad,                            /* TBD*/
    Word16 stage,                            /* TBD*/
    const Word16 *cb,                     /* TBD*/
    Word16 cbsize,                           /* TBD*/
    Word16 m,                                /* TBD*/
    Word16 *m_best,                          /* TBD*/
    Word32 *m_dist,                       /* TBD*/
    Word16 vdim                              /*  -> vdim		: vector dimension */
)

{
    Word16 i,j;
    const Word16 *y;
    Word32 Ldist;
    Word16 d, endloop;
    endloop = 0;          move16();

    for (i=0;i<cbsize;i++)
    {
      y = &cb[i*vdim];      move16();
      Ldist = 0;            move32();
      endloop = 0;          move16();
      for (j=0;j<vdim;j++)
      {
          d = round(L_msu(L_mult(x[j],16384),y[j],8192));
          Ldist = L_mac(Ldist,d,d);
          test();
          if (L_sub(Ldist, *m_dist) >= 0)
          {
              endloop = 1;     move16();
          }
      }

      test();
      if(endloop == 0)
      {
          *m_dist = Ldist;                  move32();
          dist[*m_best] = Ldist;            move32();

          for (j=0;j<stage;j++) 
          {
              mpfade[*m_best][j] = pfad[j];     move16();
          }
          mpfade[*m_best][stage] = i;           move16();

          for (j=0;j<m;j++)
          {
              test();
              if (L_sub(dist[j],*m_dist) > 0)
              {
                  *m_dist = dist[j];        move32();
                  *m_best = j;              move16();
              }
          }
      }
    }
    
}
static void Msvq2(Word16 *y, Word16 *inds, Word16 *x, const MSVQ_fx * msvq_tab_fx )
  /*  <- y                      : reconstruction vector */
  /*  <- inds           : the best path through the msvq trellis */
  /*  -> x                      : vector to be encoded */
  /*  -> msvq_tab   : all required parameters and tables */
{
   Word16 i, j, k, l, pfadanz_max, arg;
   Word16 m_best, tmp1;
   Word32 m_dist;
   Word16 e[MAX_VECT_DIM];
   Word16 mpfade_mem1[INTENS_MAX][MAX_NUMSTAGES];
   Word16 mpfade_mem2[INTENS_MAX][MAX_NUMSTAGES];
   Word16 (*mpfade)[INTENS_MAX][MAX_NUMSTAGES];
   Word16 (*alt_mpfade)[INTENS_MAX][MAX_NUMSTAGES];
   Word16 (*tmp_pfad)[INTENS_MAX][MAX_NUMSTAGES];
   Word32 dist[MAX_NUMSTAGES][INTENS_MAX];
   const Word16 *cbsize = msvq_tab_fx->cbsizes;
   const Word16  vdim = msvq_tab_fx->vdim;
   const Word16 stages = msvq_tab_fx->nstages;
   const Word16 m = msvq_tab_fx->intens;
   const Word16 **wcb = msvq_tab_fx->cbs;

   move16();move16();move16();move16();move16();

   mpfade = &mpfade_mem1;           move16();
   alt_mpfade = &mpfade_mem2;       move16();
   for (j = 0; j < m; j++)
   {
      dist[0][j] = MAX_32;        move32();
   }
   m_best = 0;                    move16();
   m_dist = MAX_32;               move32();
   
   pfadanz_max = cbsize[0];       move16();
   M_cbcod(*mpfade, dist[0], x, NULL, 0, wcb[0], cbsize[0], m, &m_best, &m_dist,
           vdim);
   for (i = 1; i < stages; i++)
   {
      for (j = 0; j < m; j++)
      {
         dist[i][j] = MAX_32;     move32();
      }
      m_best = 0;                 move16();
      m_dist = MAX_32;            move32();

      tmp_pfad = mpfade;        move16();
      mpfade = alt_mpfade;      move16();
      alt_mpfade = tmp_pfad;    move16();
      test();
      if (sub(m,pfadanz_max) < 0)
      {
         arg = m;                 move16();
      }
      else
      {
         arg = pfadanz_max;       move16();
      }
      tmp1 = m;   move16();
      test();
      if (sub(tmp1,pfadanz_max) > 0)
      {
         tmp1 = pfadanz_max;      move16();
      }

      for (k = 0; k < tmp1; k++)
      {
         for (j = 0; j < vdim; j++)
         {
            e[j] = x[j];          move16();
            for (l = 0; l < i; l++)
            {
               e[j] = sub(e[j], shr(wcb[l][(*alt_mpfade)[k][l] * vdim + j],1));     move16();
            }
         }
         M_cbcod(*mpfade, dist[i], e, (*alt_mpfade)[k], i, wcb[i], cbsize[i],
                 (i == stages - 1 ? 1 : m), &m_best, &m_dist, vdim);
      }
      pfadanz_max = extract_l(L_shr(L_mult(pfadanz_max, cbsize[i]),1));
   }
   for (j = 0; j < stages; j++)
   {
      inds[j] = (*mpfade)[0][j];        move16();
   }

   for (j = 0; j < vdim; j++)
   {
      y[j] = shr(wcb[0][inds[0] * vdim + j],1);    move16();
      for (l = 1; l < stages; l++)
      {
          y[j] = add(y[j], shr(wcb[l][inds[l] * vdim + j],1));      move16();
      }
   }
}
static void Pmsvq2(Word16 *y, Word16 **prm, Word16 *x, Word16 *old_x,const PMSVQ_fx * filt_hi_pmsvq_fx )
{
  Word16 e[MAX_VECT_DIM];
  Word16 eq[MAX_VECT_DIM];
  Word32 Ltmp;
  Word16 i;
  const  Word16 *cbm = filt_hi_pmsvq_fx->mean;
  Word16 a = filt_hi_pmsvq_fx->a;
  Word16 n = filt_hi_pmsvq_fx->msvq.vdim;
  Word16 *inds = *prm;

  *prm += filt_hi_pmsvq_fx->msvq.nstages;
  /* compute the predictor error */
  move16();move16();move16();move16();

  for (i = 0; i < n; i++)
  {
    /*e[i] = (x[i] - cbm[i]) - a * old_x[i];*/
    Ltmp = L_msu(L_mult(x[i], 32767), cbm[i], 16384);
    e[i] = round(L_msu(Ltmp, a, old_x[i]));        move16();
  }


  /* quantize the prediction error */
  Msvq2(eq, inds, e, &filt_hi_pmsvq_fx->msvq );

  /* save for next frame */
  for (i = 0; i < n; i++)
  {
    /*old_x[i] = eq[i] + a * old_x[i];*/
    Ltmp =  L_mac(L_mult(eq[i], 32767), a, old_x[i]);
    old_x[i] = round(Ltmp);     move16();
    /*y[i] = old_x[i] + cbm[i];*/
    y[i] = round(L_mac(Ltmp, cbm[i], 16384));      move16();
  }


}
static void Quant_filt(
    Word16 h[],       /* i/o */
    Word16 old_h[],  /* i/o */
    Word16 **prm,
    const PMSVQ_fx *filt_hi_pmsvq
)
{
   Pmsvq2(h, prm, h, old_h, filt_hi_pmsvq);
}

static void  Compute_gain_match(
    Word16 *gain_left,
    Word16 *gain_right,
    Word16 x[], 
    Word16 y[], 
    Word16 buf[]
) 
{
    Word16 t, tmp16;
    /* energies */
    Word32 Lenergy_right;
    Word32 Lenergy_right_q;
    Word32 Lenergy_left;
    Word32 Lenergy_mono, Ltmp;
    Word32 Lenergy_left_q;
    Word32 Lcorr_left_right;

    Word32 Lright;
    Word32 Lright_q;
    Word32 Lleft;
    Word32 Lmono;
    Word32 Left_q;
    Word32 Lcorr_lr;


    Word16 gain_fact, j;
    Word16 x_p_y;
    Word16 x_m_y;
    Word16 x_p_b;
    Word16 x_m_b;
    Word16 tmp,tmp_fraq, tmp_exp;


    Lenergy_left     = 1;           move32();
    Lenergy_right    = 1;           move32();

    Lenergy_left_q   = 1;           move32();
    Lenergy_right_q  = 1;           move32();
    Lenergy_mono     = 1;           move32();
    Lcorr_left_right = 1;           move32();

    for (t=0;t<L_DIV+L_SUBFR;t+= L_SUBFR)
    {
        Lright      = 0;       move32();
        Lright_q    = 0;       move32();
        Lleft       = 0;       move32();
        Lmono       = 0;       move32();
        Left_q      = 0;       move32();
        Lcorr_lr    = 0;       move32();

        for(j = 0;j < L_SUBFR; j++)
        {
            /*      x_p_y = add(x[t],y[t]);
            x_m_y = sub(x[t],y[t]);
            x_p_b = add(x[t],buf[t]);
            x_m_b = sub(x[t],buf[t]);*/
            Ltmp  = L_mult(x[j+t], 8192);                 /* remove 1 (2) bit to prevent overflow when input signal is scaled*/
            x_p_y = round(L_mac(Ltmp, y[j+t], 8192));
            x_m_y = round(L_msu(Ltmp, y[j+t], 8192));
            x_p_b = round(L_mac(Ltmp, buf[j+t], 8192));
            x_m_b = round(L_msu(Ltmp, buf[j+t], 8192));
            /* mono + side */
            Lleft     = L_mac(Lleft,  x_p_y,x_p_y);
            Left_q    = L_mac(Left_q, x_p_b,x_p_b);
            /* mono */
            tmp16     = round(Ltmp);
            Lmono     = L_mac(Lmono,  tmp16, tmp16);
            /* mono - side */
            Lright    = L_mac(Lright,   x_m_y,x_m_y);
            Lright_q  = L_mac(Lright_q, x_m_b,x_m_b);
            Lcorr_lr  = L_mac(Lcorr_lr, x_m_y,x_m_y);

        }
        Lenergy_right    = L_add(Lenergy_right, L_shr(Lright,1));
        Lenergy_left     = L_add(Lenergy_left, L_shr(Lleft,1));
        Lenergy_mono     = L_add(Lenergy_mono, L_shr(Lmono,1));
        Lenergy_right_q  = L_add(Lenergy_right_q, L_shr(Lright_q,1));
        Lenergy_left_q   = L_add(Lenergy_left_q, L_shr(Left_q,1));
        Lcorr_left_right = L_add(Lcorr_left_right, L_shr(Lcorr_lr,1));
    }

    Log2(Lenergy_left,&tmp_exp,&tmp_fraq);
    tmp = shl(tmp_exp,L2SCL);
    *gain_left = add(tmp,shr(tmp_fraq,15-L2SCL));                     move16();
    Log2(Lenergy_left_q,&tmp_exp,&tmp_fraq);
    tmp = shl(tmp_exp,L2SCL);
    *gain_left = sub(*gain_left,add(tmp,shr(tmp_fraq,15-L2SCL)));     move16();

    Log2(Lenergy_right,&tmp_exp,&tmp_fraq);
    tmp = shl(tmp_exp,L2SCL);
    *gain_right = add(tmp,shr(tmp_fraq,15-L2SCL));                    move16();
    Log2(Lenergy_right_q,&tmp_exp,&tmp_fraq);
    tmp = shl(tmp_exp,L2SCL);
    *gain_right = sub(*gain_right,add(tmp,shr(tmp_fraq,15-L2SCL)));   move16();

    Log2(Lenergy_mono,&tmp_exp,&tmp_fraq);
    tmp = shl(add(tmp_exp,2),L2SCL);
    gain_fact = add(tmp,shr(tmp_fraq,15-L2SCL));
    Log2(L_add(Lenergy_left,Lenergy_right),&tmp_exp,&tmp_fraq);
    tmp = shl(tmp_exp,L2SCL);
    gain_fact = sub(gain_fact,add(tmp,shr(tmp_fraq,15-L2SCL)));
    test();
    if (gain_fact < 0)
    {
        /* we have signal cancelation, take no risks */
        test();
        if (*gain_right >0)
        {
          *gain_right = add(*gain_right,gain_fact);         move16();
          test();
          if (*gain_right < 0)
          {
              *gain_right = 0;          move16();
          }
        }
        test();
        if (*gain_left > 0)
        {
            *gain_left = add(*gain_left,gain_fact);       move16();
            test();
            if (*gain_left < 0)
            {
                *gain_left = 0;     move16();
            }
        }
    }
    *gain_left = mult(shl(*gain_left,2),24717);     move16(); /*Q10 output*/    
    *gain_right = mult(shl(*gain_right,2),24717);   move16();
   
}
static void Quant_gain(
    Word16 gain_left,		/* i/o */
    Word16 gain_right,  /* i/o */
    Word16 old_gain[],
    Word16 **prm,
    const PMSVQ_fx *gain_hi_pmsvq
)
{
    Word16 tmp[2];
    Word16 e[2];
    Word16 i,j,bestI,d;
    Word32 Ldist,Lbest, Ltmp;
    
    Word16 a = gain_hi_pmsvq->a;
    const Word16 *cbsize = gain_hi_pmsvq->msvq.cbsizes;
    const Word16 **cb = gain_hi_pmsvq->msvq.cbs;
    const Word16 *cbm = gain_hi_pmsvq->mean;
    const Word16 *y;
        
    Word16 *inds = *prm;
    move16();move16();    
    
    tmp[0] = gain_left;         move16();
    tmp[1] = gain_right ;       move16();

    *prm += gain_hi_pmsvq->msvq.nstages;
    move16();
    /* compute the predictor error */
    for(i=0;i<2;i++)
    {  
      /* e[i] = (x[i] - cbm[i]) - a *old_x[i]; */
      Ltmp = L_mult(sub(tmp[i], cbm[i]),32767);
      e[i] = round(L_msu(Ltmp, a, old_gain[i]));    move16();
    }


    bestI = 0;                            move16();
    Lbest = 0x7FFFFFFF;                   move32();

    for (j = 0; j < cbsize[0]; j++) 
    {
      y = &cb[0][j*2];      /*ptr*/     move16();
      Ldist = 0;                          move32();
      for(i=0;i<2;i++)
      {
        d = sub(e[i], y[i]);
        Ldist = L_mac(Ldist, d, d);
      }
      test();
      if ( L_sub(Ldist, Lbest) < 0) 
      {
        Lbest = Ldist;                  move32();
        bestI = j;                      move16();
      }

    }
    y = &cb[0][bestI*2];                         move16(); /*ptr*/
    *inds = bestI;                                move16(); /*Save parameters*/ 
    tmp[0] = y[0];                                move16();                 
    tmp[1] = y[1];                                move16();
    
    /* save for next frame */
    for(i=0;i<2;i++)
    { /* old_gain in Q10 */
      Ltmp = L_mult(tmp[i], 32767);
      Ltmp = L_mac(Ltmp , a, old_gain[i]);
      old_gain[i] = round(Ltmp);                  move16();
      tmp[i] = round(L_mac(Ltmp, cbm[i], 32767)); move16();
    }
}

void Cod_hi_stereo(
    Word16 wAqLF[],             /* (i)  : Mono frequencies coefficient  */
    Word16 param[],             /* (o)  : Parameters output             */
    Coder_State_Plus_fx *st,    /* i/o  : Encoder states	            */
    Word16 speech_hi_[],        /* (i)  : Mono speech                   */
    Word16 right_hi_[]          /* (i)  : Side speech                   */
)
{
    Word16 *prm, i_subfr, i, j;

    Word16 *wexc_mono = speech_hi_-M;
    Word16 *wexc_side = right_hi_-M;
    Word16 *wp_Aq, scl;
     /* covariance matrix */
    Word16 wr[HI_FILT_ORDER][HI_FILT_ORDER];
    Word16 wc[HI_FILT_ORDER];
    /* estimated LMS filters */
    Word16 wwh[NB_DIV*HI_FILT_ORDER];
    Word16 *wp_h;
    /* signal big subframe pointers */
    Word16 *wx,*wy;
    Word16 wbuf[L_DIV+L_SUBFR];
    Word16 wgain_left[NB_SUBFR], shift_fx;
    Word16 wgain_right[NB_SUBFR];
    Word16 wr_shift[HI_FILT_ORDER];

    move16();move16();
    Copy(st->old_exc_mono,wexc_mono-HI_FILT_ORDER,HI_FILT_ORDER);

   /* compute the residual of the hi mono and right */
    wp_Aq = wAqLF;      move16();
    for (i_subfr=0; i_subfr<L_FRAME_PLUS; i_subfr+=L_SUBFR)
    {
        Residu2(wp_Aq, M, &speech_hi_[i_subfr], &wexc_mono[i_subfr], L_SUBFR);	
        Residu2(wp_Aq, M, &right_hi_[i_subfr], &wexc_side[i_subfr], L_SUBFR);
        wp_Aq += (M+1);
    }
    Residu2(wp_Aq, M, &speech_hi_[i_subfr], &wexc_mono[i_subfr], L_SUBFR);
    Residu2(wp_Aq, M, &right_hi_[i_subfr], &wexc_side[i_subfr], L_SUBFR);
   /* compute real side signal */
    Compute_exc_side(wexc_side,wexc_mono);

   /* save fir state for next frame */
    Copy(wexc_mono+L_FRAME_PLUS-HI_FILT_ORDER,st->old_exc_mono,HI_FILT_ORDER);

   /* compute the wiener filters, raw on each frame with covariance method*/
    wp_h = wwh;     move16();
    /* prm, wx, wy init */
    move16();move16();move16();
    for(i=0;i<NB_DIV;i++)
    { 
        /* set the pointer to parameters */	
        prm = param + i*NPRM_STEREO_HI_X;


        /* set signal pointers */
        wx = wexc_mono + i*L_DIV;
        wy = wexc_side + i*L_DIV;

        Compute_cross_corr_vector(wx, wy, wc, &scl);

        Compute_auto_corr_matrix(wx, wr, wr_shift);

        /* === This code must be present in integrated code === */
        shift_fx = sub(scl,wr_shift[0]);
        for (j=1; j< HI_FILT_ORDER; j++)
        {
           wr_shift[j] = sub(wr_shift[j], wr_shift[0]);      move16();
        }
        wr_shift[0] = 0;                                     move16(); 

        /* ==================================================== */
        if (Cholsolc(wr, wr_shift, wc, wp_h, shift_fx)) 
        {
          Cholsolc_repair(wr[0][0],wc[0],wp_h,shift_fx);
        }

        Smooth_ener_filter(wp_h,&(st->filt_energy_threshold));

        Quant_filt(wp_h,st->old_wh_q,&prm, st->filt_hi_pmsvq);
        Fir_filt(wp_h, HI_FILT_ORDER, wx, wbuf, L_DIV+L_SUBFR);
  
        Compute_gain_match(&wgain_left[i], &wgain_right[i], wx, wy, wbuf);

        /* quantize the gains */
        Quant_gain(wgain_left[i], wgain_right[i], st->old_gm_gain, &prm, st->gain_hi_pmsvq);

        /* next frame */
        wp_h += HI_FILT_ORDER;
    }
   /* save last filter*/
    Copy(&wwh[(NB_DIV-1)*HI_FILT_ORDER],st->old_wh,HI_FILT_ORDER);

}


