/*****************************************************************
*
* FFT9_FX.C : Fixed-point implementation of FFT_9.C
*
* Fixed point transform coefficients factor (compared to floating point):
*   - in floating point, energy of coeff. is "lg/2" time energy of the signal.
*   - in fixed-point, energy of coeff. is "8/lg" time energy of the signal.
*   this difference give the following factor : coeff fix = coeff float * 4/lg.
*
* The fixed-point scaling in Transform domain is optimized to avoid overflow on
* sinusoidal signal.  During the test, the following setup was used:
*   - frame of 80ms with 10ms overlap = 90ms for fft9 (1152 samples)
*   - window used in overlap was hamming.
*   - input signal was sinusoidal wave (sweep from fs/64 to fs/2) with amplitude of 24000.
* with this setup, no saturation was noticed.
* in short ==> Before using fft9_fx/ifft9_fx, the signal should be scaled up to 14 bits
*              to maximise the precision.
*
* For the decoder side (when signal is unknown) :
* The following scale is recommended on coefficients before calling ifft9_fx(): 
*   - scale up by decoded gain/2.
*   - if gain is low, coefficients RMS value should be limited to 256.
*     In time domain, rms will be limited to 3072 for lg=1152 and 1536 for lg=288.
*
*****************************************************************/
//#include <stdlib.h>

#include "typedef.h"
#include "basic_op.h"
#include "count.h"

#include "amr_plus_fx.h"


#define N_MAX       1152
#define ORDER_MAX   7
#define SCALE1      14      /* Scaling factor for FFT9_Fx  (/4) */
#define SCALE1T     8192      /* Scaling factor for FFT9_Fx  (/4) */

#define ISCALE1     1       /* Scaling factor for IFFT9_Fx (/2) */

#define COSOFFSET   288



/*_____________________________________________________________________
 |                                                                     |
 |  FUNCTION NAME fft9_fx                                              |
 |      Radix-9 FFT fixed-point for real-valued sequences of length 
 |              576 or 1152.
 |      The  input  sequence  is  first decimated by five,  and  the
 |      split-radix  algorithm described in [1]  are applied to  the
 |      decimated sequences.  The resulting nine separate transforms
 |      are then combined.
 |
 |      The function requires sine and cosine tables t_sin and t_cos,
 |      and constants N_MAX = 1152 and ORDER_MAX = log2(N_MAX/9). The
 |      table entries are  defined as sin(2*pi*i) and cos(2*pi*i) for
 |      i = 0, 1, ..., N_MAX.
 |
 |  INPUT
 |      X[0:n-1]  Input sequence.
 |      n         Number of samples in the sequence, must be 288,
 |                576, or 1152.
 |
 |  OUTPUT
 |      Y[0:n-1]  Transform coeffients in the order re[0], re[1], ...
 |                re[n/2], im[1], im[2], ..., im[n/2-1].
 |_____________________________________________________________________|
*/

void fft9_fx(
  Word16 X[],
  Word16 Y[],
  Word16 n
)
{
  Word16 Z[N_MAX];
  Word16 *Z0, *Z1, *Z2, *Z3, *Z4, *Z5, *Z6, *Z7, *Z8;
  Word16 *z0, *z1, *z2, *z3, *z4, *z5, *z6, *z7, *z8;
  Word16 wtmp1, wtmp2;
  Word16 ns2, ms2;
  Word16 *yre, *yim, *zre, *zim, *wre, *wim;
  Word16 *x;
  Word16 m = 0, step = 0, sign, order = 0;
  Word16 i, j, k;
  Word32 Lyre, Lyim;
  Word16 wSizeByTwo = 0, wNumberStage = 0;
  Word16 phs_tbl[voSIZE];
  Word16 ii_table[NUM_STAGE];

  Word16 inc = 0, pos = 0 ;
  
  
  /* Determine the order of the transform, the length of decimated  */
  /* transforms m, and the step for the sine and cosine tables.     */
  
  switch(n) {
  case 288:
    order = 5;    
    m     = 32;   
    step  = 2;       /*4*/
    wSizeByTwo   = voSIZE/8;          
    wNumberStage = NUM_STAGE-2;     

    break;
  case 576:
    order = 6; 
    m     = 64;
    step  = 1;       /*2*/
    wSizeByTwo   = voSIZE/4;          
    wNumberStage = NUM_STAGE-1;     

    break;
  case 1152:
    order = 7;
    m     = 128; 
    step  = 0;       /*1*/
    wSizeByTwo   = voSIZE/2;          
    wNumberStage = NUM_STAGE;     

    break;
  default:
    //AMRWBPLUS_PRINT(" invalid fft9 size!\n");
    //exit(0);
	  break;
  }

  /* Compose decimated sequences X[9i], X[9i+1], ..., X[9i+4] and   */
  /* compute their FFT of length m.                                 */

  Z0 = &Z[0];   z0 = &Z0[0];        
  Z1 = &Z0[m];  z1 = &Z1[0];        
  Z2 = &Z1[m];  z2 = &Z2[0];          
  Z3 = &Z2[m];  z3 = &Z3[0];        
  Z4 = &Z3[m];  z4 = &Z4[0];        
  Z5 = &Z4[m];  z5 = &Z5[0];        
  Z6 = &Z5[m];  z6 = &Z6[0];        
  Z7 = &Z6[m];  z7 = &Z7[0];        
  Z8 = &Z7[m];  z8 = &Z8[0];        
  x  =  &X[0];                      

  for (i = 0; i < m; i++)
  {
    *z0++ = *x++;           
    *z1++ = *x++;           
    *z2++ = *x++;           
    *z3++ = *x++;           
    *z4++ = *x++;           
    *z5++ = *x++;           
    *z6++ = *x++;           
    *z7++ = *x++;           
    *z8++ = *x++;           
  }

  for (i=0; i<n; i++) 
  {
      Z[i] = mult_r(Z[i], 29127);         /* multiply by 8/9 */
      
  }

  init_r_fft_fx(m, wSizeByTwo, wNumberStage, ii_table, phs_tbl);
    
  r_fft_fx(Z0, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, 1);
  r_fft_fx(Z1, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, 1);
  r_fft_fx(Z2, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, 1);
  r_fft_fx(Z3, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, 1);
  r_fft_fx(Z4, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, 1);
  r_fft_fx(Z5, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, 1);
  r_fft_fx(Z6, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, 1);
  r_fft_fx(Z7, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, 1);
  r_fft_fx(Z8, m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, 1);
        
  /* Compute the DC coefficient and store it into Y[0]. Note that   */
  /* the variables Z0, ..., Z8, z0, ..., z8 are not needed after    */
  /* this.                                                          */

  Lyre = L_mult(*Z0, 1);
  Lyre = L_mac(Lyre, *Z1, 1);
  Lyre = L_mac(Lyre, *Z2, 1);
  Lyre = L_mac(Lyre, *Z3, 1);
  Lyre = L_mac(Lyre, *Z4, 1);
  Lyre = L_mac(Lyre, *Z5, 1);
  Lyre = L_mac(Lyre, *Z6, 1);
  Lyre = L_mac(Lyre, *Z7, 1);
  Lyre = L_mac(Lyre, *Z8, 1);
  *Y = round16(L_shl(Lyre, SCALE1-1));

  /* Butterflies of order 9. */

  /* EXAMPLE RADIX5:                                                */
  /* ~~~~~~~~~~~~~~~                                                */
  /* Transform coefficients in Y are computed so that the pointer   */
  /* yre goes over Y[1:n/2] = Y[1:5m],  and the pointer yim  over   */
  /* Y[n/2+1:n-1] = Y[5m+1:10m-1].  The pointers  zre and zim run   */
  /* over the butterflies in Z according to the following table.    */
  /*                                                                */
  /* ===================================================            */
  /*   i     yre           yim       zre         zim                */
  /* ===================================================            */
  /*   0   Y[   1: m]  Y[10m-1:9m]  Z[1:m]    Z[2m-1:m]             */
  /*   1   Y[ m+1:2m]  Y[ 9m-1:8m]  Z[m-1:0]  Z[m+1:2m]             */
  /*   2   Y[2m+1:3m]  Y[ 8m-1:7m]  Z[1:m]    Z[2m-1:m]             */
  /*   3   Y[3m+1:4m]  Y[ 7m-1:6m]  Z[m-1:0]  Z[m+1:2m]             */
  /*   4   Y[4m+1:5m]  Y[ 6m-1:5m]  Z[1:m]    Z[2m-1:m]             */
  /* ===================================================            */

  sign = 32767;                /* 1 in Q15*/
  ns2 = shr(n,1);
  ms2 = shr(m,1);

  wtmp1 = sub(m,1);
  wtmp2 = add(ns2,1);
  zre = &Z[1];   zim = &Z[wtmp1];       
  yre = &Y[1];   yim = &Y[wtmp2];       
        
  for (i = 0; i < 9; i++) 
  {
    for (j = 1; j < ms2; j++) 
    {
      wre = &zre[0];    
      wim = &zim[0];
         
      /* to reduce complexity, the loop is separate into 2 parts. */

      inc = add(inc, shl(1,step));
      pos = 0;      
      
      if (sign > 0)
      {
        Lyre = L_mult(*wre,SCALE1T);        /* *wre/4 stored in high part*/
        Lyim = L_mult(*wim,SCALE1T);
        for (k = 1; k < 9; k++)
        {
          wre  += m;             
          wim  += m;
          pos += inc;
          
          if (sub(pos,N_MAX) >= 0)
          {
              pos = sub(pos,N_MAX);      /* circular addressing*/
          }
                              
          /*yre +=  (*wre)*t_cos[pos] + sign*(*wim)*t_sin[pos];*/
          Lyre = L_mac(Lyre,*wim,t_sinFxS4[pos]);
          Lyre = L_mac(Lyre,*wre,t_sinFxS4[pos+COSOFFSET]);
                                                                        
          /*yim += -(*wre)*t_sin[pos] + sign*(*wim)*t_cos[pos];*/
          Lyim = L_msu(Lyim,*wre,t_sinFxS4[pos]);
          Lyim = L_mac(Lyim,*wim,t_sinFxS4[pos+COSOFFSET]);
        }
        zre++;
        zim--;
      }
      else
      {
        Lyre = L_mult(*wre,SCALE1T);
//        Lyim = L_negate(L_mult(*wim,SCALE1T));      /*yim = sign*(*wim);*/
        Lyim = L_mult(*wim,SCALE1T);
        Lyim = L_negate(Lyim);      /*yim = sign*(*wim);*/
		
        for (k = 1; k < 9; k++) 
        {
          wre  += m;
          wim  += m;
          pos += inc;
          
          if (sub(pos,N_MAX) >= 0)
          {
              pos = sub(pos,N_MAX);      /* circular addressing*/
          }

          /*yre +=  (*wre)*t_cos[pos] + sign*(*wim)*t_sin[pos];*/
          Lyre = L_mac(Lyre,*wre,t_sinFxS4[pos+COSOFFSET]);
          Lyre = L_msu(Lyre,*wim,t_sinFxS4[pos]);         
                                        
          /*yim += -(*wre)*t_sin[pos] + sign*(*wim)*t_cos[pos];*/
          Lyim = L_msu(Lyim,*wre,t_sinFxS4[pos]);
          Lyim = L_msu(Lyim,*wim,t_sinFxS4[pos+COSOFFSET]);
        }
        zre--;
        zim++;
      }
      *yre++  = round16(Lyre);
      *yim++  = round16(Lyim);
    }

    wre  = &zre[0];     
           
    Lyre = L_mult(*wre,SCALE1T);
    Lyim = 0;
    pos = 0;             
    inc = add(inc, shl(1,step));

    for (k = 1; k < 9; k++)
    {
      wre += m; 
      pos += inc;
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }

      /*yre +=  (*wre)*t_cos[pos];*/
      Lyre = L_mac(Lyre,*wre,t_sinFxS4[pos+COSOFFSET]);
                        
      /*yim += -(*wre)*t_sin[pos];*/
      Lyim = L_msu(Lyim,*wre,t_sinFxS4[pos]);
    }
       
    sign = negate(sign);
    
    if (sign > 0)
    {
      zre++;
      zim--;
    }
    else
    {
      zre--;
      zim++;
    }

    *yre++ = round16(Lyre);
    if (i<8) *yim++ = round16(Lyim);
  }

  return;
}


/*_____________________________________________________________________
 |                                                                     |
 |  FUNCTION NAME ifft9_fx                                             |
 |      Inverse Radix-9 FFT for real-valued sequences of length 288,   |
 |      576 or 1152. The function computes first inverse butterflies   |
 |      for obtaining transform coefficients of sequencies decimated
 |      by 9.  The inverse split-radix FFT [1] is applied separately
 |      to these nine coefficient blocks and the outcomes are
 |      combined.
 |
 |      The function requires sine and cosine tables t_sin and t_cos,
 |      and constants N_MAX = 1152 and ORDER_MAX = log2(N_MAX/9). The
 |      table entries are  defined as sin(2*pi*i) and cos(2*pi*i) for
 |      i = 0, 1, ..., N_MAX-1.
 |
 |  INPUT
 |      Y[0:n-1]  Transform coeffients in the order re[0], re[1],
 |                ..., re[n/2], im[1], im[2], ..., im[n/2-1].
 |      n         Number of transform coefficients, must be 288, 576,
 |                or 1152.
 |
 |  OUTPUT
 |      X[0:n-1]  Output sequence.
 |_____________________________________________________________________|
*/

void ifft9_fx(
  Word16 Y[],
  Word16 X[],
  Word16 n
)
{
#if (FUNC_IFFT9_FX_OPT)

  Word16 i, k, m =0, step = 0, order =0;
  Word16 Z[N_MAX];
  Word16 *z, *zre, *zim, *x;
  Word16 *z0,   *z1,   *z2,   *z3,   *z4, *z5, *z6, *z7, *z8;
  Word16 ns2, ms2,m3, m2, m4, ns2pm4,ns2pm, m4min1, ns2pm4min1, ns2p1;
  Word16 *yref, *yreb, *yimf, *yimb, *yre, *yim, inc = 0 ,pos = 0;
  Word16 wSizeByTwo = 0, wNumberStage = 0;
  Word16 phs_tbl[voSIZE];
  Word16 ii_table[NUM_STAGE];
    
  Word32 LtpA, LtpB, L_wtmp1;
  /* Determine the order of the transform, the length of decimated  */
  /* transforms m, and the step for the sine and cosine tables.     */
  switch(n) 
  {
    case 288:
      order = 5;
      m     = 32;
      step  = 4;
      wSizeByTwo   = voSIZE >> 3;
      wNumberStage = NUM_STAGE - 2;
      break;
    case 576:
      order = 6;
      m     = 64;
      step  = 2;
      wSizeByTwo   = voSIZE >> 2;
      wNumberStage = NUM_STAGE-1;
      break;
    case 1152:
      order = 7;
      m     = 128;
      step  = 1;
      wSizeByTwo   = voSIZE >> 1;
      wNumberStage = NUM_STAGE;  

      break;
    default:
      //AMRWBPLUS_PRINT(" invalid fft9 size!\n");
      //exit(0); 
		break;
  }

  /* EXAMPLE RADIX5:                                                */
  /* ~~~~~~~~~~~~~~~                                                */
  /* The following table depicts indexing and locations of pointers */
  /* in an illustrative example case as n = 20 and m = n/5 = 4.     */
  /* The pointers yr0, yr1, yr2, yi0, yi1, yi2 are anchored to the  */
  /* beginning of the blocks in the coefficient vector. They will   */
  /* not be changed during the computation. The coefficient vector  */
  /* and the fixed pointers are shown in the second and third       */
  /* column of the table. The floating pointers yr0f, yr1f, yr2f,   */
  /* yi0f, yi1f, yi2f go forward over the corresponding blocks.     */
  /* Correspondingly yr0b, yr1b, yi0b, yi1b run backwards. These    */
  /* pointers will be repositioned during the algorithm.            */
  /*                                                                */
  /*   ==================================                           */
  /*   index  coeff    block     pointers                           */
  /*   ==================================                           */
  /*     0    re[0]    &yr0[0]    yr0f->                            */
  /*     1    re[1]                                                 */
  /*     2    re[2]                                                 */
  /*     3    re[3]               <-yr0b                            */
  /*     4    re[4]    &yr1[0]    yr1f->                            */
  /*     5    re[5]                                                 */
  /*     6    re[6]                                                 */
  /*     7    re[7]               <-yr1b                            */
  /*     8    re[8]    &yr2[0]    yr2f->                            */
  /*     9    re[9]                                                 */
  /*     10   re[10]                                                */
  /*     11   im[1]    &yi0[0]    yi0f->                            */
  /*     12   im[2]                                                 */
  /*     13   im[3]               <-yi0b                            */
  /*     14   im[4]    &yi1[0]                                      */
  /*     15   im[5]               yi1f->                            */
  /*     16   im[6]                                                 */
  /*     17   im[7]               <-yi1b                            */
  /*     18   im[8]    &yi2[0]                                      */
  /*     19   im[9]               yi2f->                            */
  /*   ==================================                           */

  /* Initialize the fixed and the floating pointers. */

  ns2 = n >> 1;
  ms2 = m >> 1;
  m2 = m << 1;
  m3 = m + m2;
  m4 = m << 2;
  ns2pm4 = ns2 + m4;
  ns2pm = ns2 + m;
  ns2p1 = ns2 + 1;
  m4min1 = m4 - 1;
  ns2pm4min1 = ns2pm4 - 1;

  zre  = Z;
  zim  = Z + m - 1;
  yref = Y;

  /* Compute the inverse butterflies. */

  /*zre++ = *yr0f++ + 2*(*yr1f++) + 2*(*yr2f++) + 2*(*yr3f++) + 2*(*yr4f++) ;*/

  L_wtmp1 = *yref << 1;
  yref+=m;
  L_wtmp1 += *yref << 2;
  yref+=m;
  L_wtmp1 += *yref << 2;
  yref+=m;
  L_wtmp1 += *yref << 2;
  yref+=m;
  L_wtmp1 += *yref << 2;
  *zre++ = ((L_wtmp1 << (15-ISCALE1)) + 0x00008000) >> 16;
        
  for (i = 1; i < ms2; i++) 
  {
    yref = Y + i;
    yreb = Y + m4 - i;
    yimf = Y + ns2 + i;
    yimb = Y + ns2pm4 - i;

    /*zre++ = *yr0f++ + *yr1f++ + *yr2f++ + *yr3f++ + *yr4f++ + *yr0b-- + *yr1b-- + *yr2b-- + *yr3b--;*/
    L_wtmp1 = *yref << 1;
    yref += m;
    L_wtmp1 += *yref << 1;
    yref += m;
    L_wtmp1 += *yref << 1;
    yref += m;
    L_wtmp1 += *yref << 1;
    yref += m;
    L_wtmp1 += *yref << 1;
    L_wtmp1 += *yreb << 1;
    yreb -= m;
    L_wtmp1 += *yreb << 1;
    yreb -= m;
    L_wtmp1 += *yreb << 1;
    yreb -= m;
    L_wtmp1 += *yreb << 1;
    *zre++ = ((L_wtmp1 << (15-ISCALE1)) + 0x00008000) >> 16;
               
    /*zim-- = *yi0f++ + *yi1f++ + *yi2f++ + *yi3f++ + *yi4f++ - *yi0b-- - *yi1b-- - *yi2b-- - *yi3b--;*/
    L_wtmp1 = *yimf << 1;
    yimf += m;
    L_wtmp1 += *yimf << 1;
    yimf += m;
    L_wtmp1 += *yimf << 1;
    yimf += m;
    L_wtmp1 += *yimf << 1;
    yimf += m;
    L_wtmp1 += *yimf << 1;
    L_wtmp1 -= *yimb << 1;
    yimb -= m;
    L_wtmp1 -= *yimb << 1;
    yimb -= m;
    L_wtmp1 -= *yimb << 1;
    yimb -= m;
    L_wtmp1 -= *yimb << 1;
    *zim-- = ((L_wtmp1 << (15-ISCALE1)) + 0x00008000) >> 16;
  }
  yref = Y + ms2;
       
  /*zre = 2*(*yr0f) + 2*(*yr1f) + 2*(*yr2f) + 2*(*yr3f) + *yr4f;*/

  L_wtmp1 = *yref << 2;
  yref+=m;
  L_wtmp1 += *yref << 2;
  yref+=m;
  L_wtmp1 += *yref << 2;
  yref+=m;
  L_wtmp1 += *yref << 2;
  yref+=m;
  L_wtmp1 += *yref << 1;
  *zre = ((L_wtmp1 << (15-ISCALE1)) + 0x00008000) >> 16;
        
  for (k = 1; k < 9; k++)
  {
    inc = inc + (N_MAX/9);
    pos = inc;

    yre = Y + m;
    yim = Y + ns2pm;

    z  = Z + (k << order);

    /*z  = *yr0;*/ 
    LtpA = Y[0] << 14;

    /*z += *yr1*t_cosFx[pos] - *yi1*t_sinFxS4[pos];  ind++;*/
    LtpA += ((*yre * t_sinFxS4[pos+COSOFFSET]) << 1) - ((*yim * t_sinFxS4[pos]) << 1);
    yre += m;               /* MAR arx*/
    yim += m;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }
    
    /*z += *yr2*t_cosFx[pos] - *yi2*t_sinFxS4[pos];  ind++;*/
    LtpA += ((*yre * t_sinFxS4[pos+COSOFFSET]) - (*yim * t_sinFxS4[pos])) << 1;
    yre += m;
    yim += m;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }

    /*z += *yr3*t_cosFx[pos] - *yi3*t_sinFxS4[pos];  ind++;*/
    LtpA += ((*yre * t_sinFxS4[pos+COSOFFSET]) - (*yim * t_sinFxS4[pos])) << 1;
    yre += m;
    yim += m;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }

    /*z += *yr4*t_cosFx[pos] - *yi4*t_sinFxS4[pos];  ind++;*/
    LtpA += ((*yre * t_sinFxS4[pos+COSOFFSET]) - (*yim * t_sinFxS4[pos])) << 1;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }

    /*z += *yr4*t_cosFx[pos] + *yi4*t_sinFxS4[pos];  ind++;*/
    LtpA += ((*yre * t_sinFxS4[pos+COSOFFSET]) + (*yim * t_sinFxS4[pos])) << 1;
    yre -= m;
    yim -= m;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }

    /*z += *yr3*t_cosFx[pos] + *yi3*t_sinFxS4[pos];  ind++;*/
    LtpA += ((*yre * t_sinFxS4[pos+COSOFFSET]) + (*yim * t_sinFxS4[pos])) << 1;
    yre -= m;
    yim -= m;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }
    
    /*z += *yr2*t_cosFx[pos] + *yi2*t_sinFxS4[pos];  ind++;*/
    LtpA += ((*yre * t_sinFxS4[pos+COSOFFSET]) + (*yim * t_sinFxS4[pos])) << 1;
    yre -= m;
    yim -= m;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }

    /*z += *yr1*t_cosFx[pos] + *yi1*t_sinFxS4[pos];*/
    LtpA += ((*yre * t_sinFxS4[pos+COSOFFSET]) + (*yim * t_sinFxS4[pos])) << 1;
    
    *z = ((LtpA << 1) + 0x00008000) >> 16;

    zre  = z + 1;     zim  = z + m - 1;

    pos = 0;

                
    for (i = 1; i < ms2; i++) 
    {
      
      pos += step;
      if (pos >= N_MAX)
      {
        pos -= N_MAX;      /* circular addressing*/
      }

      yref = Y + i;
      yreb = Y + m4 - i;
      yimf = Y + ns2 + i;
      yimb = Y + ns2pm4 - i;

      LtpA = (*yref * t_sinFxS4[pos+COSOFFSET]) << 1;
      LtpA -= (*yimf * t_sinFxS4[pos]) << 1;

      /*zim  = *yr0f*t_sinFxS4[pos] + *yi0f*t_cosFx[pos];  ind++;*/
      LtpB = ((*yref * t_sinFxS4[pos]) + (*yimf * t_sinFxS4[pos + COSOFFSET])) << 1;
      yref += m;
      yimf += m;
      pos += inc;
      if (pos >= N_MAX)
      {
        pos -= N_MAX;      /* circular addressing*/
      }

      /*zre += *yr1f*t_cosFx[pos] - *yi1f*t_sinFxS4[pos];*/
      LtpA += ((*yref * t_sinFxS4[pos+COSOFFSET]) - (*yimf * t_sinFxS4[pos])) << 1;
      
      /*zim += *yr1f*t_sinFxS4[pos] + *yi1f*t_cosFx[pos];  ind++;*/
      LtpB += ((*yref * t_sinFxS4[pos]) + (*yimf * t_sinFxS4[pos + COSOFFSET])) << 1;
      pos += inc;
      if (pos >= N_MAX)
      {
        pos -= N_MAX;      /* circular addressing*/
      }

      yref += m;
      yimf += m;
                        
      /*zre += *yr2f*t_cosFx[pos] - *yi2f*t_sinFxS4[pos];*/
      LtpA += ((*yref * t_sinFxS4[pos+COSOFFSET]) - (*yimf * t_sinFxS4[pos])) << 1;
      
      /*zim += *yr2f*t_sinFxS4[pos] + *yi2f*t_cosFx[pos];  ind++;*/
      LtpB += ((*yref * t_sinFxS4[pos]) + (*yimf * t_sinFxS4[pos + COSOFFSET])) << 1;
      pos += inc;
      if (pos >= N_MAX)
      {
        pos -= N_MAX;      /* circular addressing*/
      }
      yref += m;
      yimf += m;
      
      /*zre += *yr3f*t_sinFxS4[pos+COSOFFSET]) - *yi3f*t_sinFxS4[pos];*/
      LtpA += ((*yref * t_sinFxS4[pos+COSOFFSET]) - (*yimf * t_sinFxS4[pos])) << 1;

      /*zim += *yr3f*t_sinFxS4[pos] + *yi3f*t_cosFx[pos];  ind++;*/
      LtpB += ((*yref * t_sinFxS4[pos]) + (*yimf * t_sinFxS4[pos + COSOFFSET])) << 1;
      pos += inc;
      if (pos >= N_MAX)
      {
        pos -= N_MAX;      /* circular addressing*/
      }
      yref += m;
      yimf += m;

      /*zre += *yr4f*t_cosFx[pos] - *yi4f*t_sinFxS4[pos];*/
      LtpA += ((*yref * t_sinFxS4[pos+COSOFFSET]) - (*yimf * t_sinFxS4[pos])) << 1;
        
      /*zim += *yr4f*t_sinFxS4[pos] + *yi4f*t_cosFx[pos];  ind++;*/
      LtpB += ((*yref * t_sinFxS4[pos]) + (*yimf * t_sinFxS4[pos + COSOFFSET])) << 1;
      pos += inc;
      if (pos >= N_MAX)
      {
        pos -= N_MAX;      /* circular addressing*/
      }

      /*zre += *yr3b*t_cosFx[pos] + *yi3b*t_sinFxS4[pos];*/
      LtpA += ((*yreb * t_sinFxS4[pos+COSOFFSET]) + (*yimb * t_sinFxS4[pos])) << 1;

      /*zim += *yr3b*t_sinFxS4[pos] - *yi3b*t_cosFx[pos];  ind++;*/
      LtpB += ((*yreb * t_sinFxS4[pos]) - (*yimb * t_sinFxS4[pos + COSOFFSET])) << 1;
      pos += inc;
      if (pos >= N_MAX)
      {
        pos -= N_MAX;      /* circular addressing*/
      }
      yreb -=m;
      yimb -=m;
      
      /*zre += *yr2b*t_cosFx[pos] + *yi2b*t_sinFxS4[pos];*/
      LtpA += ((*yreb * t_sinFxS4[pos+COSOFFSET]) + (*yimb * t_sinFxS4[pos])) << 1;

      /*zim += *yr2b*t_sinFxS4[pos] - *yi2b*t_cosFx[pos];  ind++;*/
      LtpB += ((*yreb * t_sinFxS4[pos]) - (*yimb * t_sinFxS4[pos + COSOFFSET])) << 1;
      pos += inc;
      if (pos >= N_MAX)
      {
        pos -= N_MAX;      /* circular addressing*/
      }
      yreb -=m;
      yimb -=m;

      /*zre += *yr1b*t_cosFx[pos] + *yi1b*t_sinFxS4[pos];*/
      LtpA += ((*yreb * t_sinFxS4[pos+COSOFFSET]) + (*yimb * t_sinFxS4[pos])) << 1;

      /*zim += *yr1b*t_sinFxS4[pos] - *yi1b*t_cosFx[pos];  ind++;*/
      LtpB += ((*yreb * t_sinFxS4[pos]) - (*yimb * t_sinFxS4[pos + COSOFFSET])) << 1;
      pos += inc;
      if (pos >= N_MAX)
      {
        pos -= N_MAX;      /* circular addressing*/
      }
      yreb -=m;
      yimb -=m;

      /*zre += *yr0b*t_cosFx[pos] + *yi0b*t_sinFxS4[pos];*/
      LtpA += ((*yreb * t_sinFxS4[pos+COSOFFSET]) + (*yimb * t_sinFxS4[pos])) << 1;

      /*zim += *yr0b*t_sinFxS4[pos] - *yi0b*t_cosFx[pos];*/
      LtpB += ((*yreb * t_sinFxS4[pos]) - (*yimb * t_sinFxS4[pos+COSOFFSET])) << 1;
      pos += inc;
      if (pos >= N_MAX)
      {
        pos -= N_MAX;      /* circular addressing*/
      }

      *zre++ = ((LtpA << 1) + 0x00008000) >> 16;
      *zim-- = ((LtpB << 1) + 0x00008000) >> 16;
    }
    
    pos += step;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }

    yref = Y  + ms2;
    yreb = Y + ns2 - m;
    yimf = Y + ns2 + ms2;
    yimb = Y + n - m;

    /*zre  = *yr0f*t_cosFx[pos] - *yi0f*t_sinFxS4[pos];   ind++;*/
    LtpA = ((*yref * t_sinFxS4[pos+COSOFFSET]) -(*yimf * t_sinFxS4[pos])) << 1;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }
    yref +=m;
    yimf +=m;

    /*zre += *yr1f*t_cosFx[pos] - *yi1f*t_sinFxS4[pos];   ind++;*/
    LtpA += ((*yref * t_sinFxS4[pos+COSOFFSET]) - (*yimf * t_sinFxS4[pos])) << 1;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }
    yref +=m;
    yimf +=m;

    /*zre += *yr2f*t_cosFx[pos] - *yi2f*t_sinFxS4[pos];   ind++;*/
    LtpA += ((*yref * t_sinFxS4[pos+COSOFFSET]) - (*yimf * t_sinFxS4[pos])) << 1;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }
    yref +=m;
    yimf +=m;

    /*zre += *yr3f*t_cosFx[pos] - *yi3f*t_sinFxS4[pos];   ind++;*/
    LtpA += ((*yref * t_sinFxS4[pos+COSOFFSET]) - (*yimf * t_sinFxS4[pos])) << 1;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }
    yref +=m;
    yimf +=m;
                
    /*zre += *yr4f*(t_cosFx[pos] - t_sinFxS4[pos]);       ind++;*/
    LtpA += ((*yref * t_sinFxS4[pos+COSOFFSET]) - ((t_sinFxS4[pos] << 14) - t_sinFxS4[pos])) << 1;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }

    /*zre += *yr3b*t_cosFx[pos] + *yi3b*t_sinFxS4[pos];   ind++;*/
    LtpA += ((*yreb * t_sinFxS4[pos+COSOFFSET]) + (*yimb * t_sinFxS4[pos])) << 1;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }
    yreb -=m;
    yimb -=m;

    /*zre += *yr2b*t_cosFx[pos] + *yi2b*t_sinFxS4[pos];   ind++;*/
    LtpA += ((*yreb * t_sinFxS4[pos+COSOFFSET]) + (*yimb * t_sinFxS4[pos])) << 1;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }
    yreb -=m;
    yimb -=m;

    /*zre += *yr1b*t_cosFx[pos] + *yi1b*t_sinFxS4[pos];   ind++;*/
    LtpA += ((*yreb * t_sinFxS4[pos+COSOFFSET]) + (*yimb * t_sinFxS4[pos])) << 1;
    pos += inc;
    if (pos >= N_MAX)
    {
        pos -= N_MAX;      /* circular addressing*/
    }
    yreb -=m;
    yimb -=m;

    /*zre += *yr0b*t_cosFx[pos] + *yi0b*t_sinFxS4[pos];*/
    LtpA += ((*yreb * t_sinFxS4[pos+COSOFFSET]) + (*yimb * t_sinFxS4[pos])) << 1;

    *zre = ((LtpA << 1) + 0x00008000) >> 16;

    step += 1 << (ORDER_MAX - order);
  }

  /* Compute the inverse FFT for all nine blocks. */

  z0 = Z;
  z1 = z0 + m;     /* z1 = &Z[ m];     */
  z2 = z1 + m;     /* z2 = &Z[2m];     */
  z3 = z2 + m;    /* z3 = &Z[3m];     */
  z4 = z3 + m;    /* z4 = &Z[4m];     */
  z5 = z4 + m;    /* z5 = &Z[5m];     */
  z6 = z5 + m;    /* z6 = &Z[6m];     */
  z7 = z6 + m;    /* z7 = &Z[7m];     */
  z8 = z7 + m;    /* z8 = &Z[8m];     */

  init_r_fft_fx(m, wSizeByTwo, wNumberStage, ii_table, phs_tbl);
        
  r_fft_fx(&z0[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z1[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z2[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z3[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z4[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z5[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z6[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z7[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z8[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);

  /* signal reconstitution */
  x = X; 
  for (i = 0; i < m; i++)
  {
    *x++ = *z0++;
    *x++ = *z1++;
    *x++ = *z2++;
    *x++ = *z3++;
    *x++ = *z4++;
    *x++ = *z5++;
    *x++ = *z6++;
    *x++ = *z7++;
    *x++ = *z8++;
  }

  return;

#else

  Word16 i, k, m, step, order;
  Word16 Z[N_MAX];
  Word16 *z, *zre, *zim, *x;
  Word16 *z0,   *z1,   *z2,   *z3,   *z4, *z5, *z6, *z7, *z8;
  Word16 wtmp1, wtmp2;
  Word16 ns2, ms2,m3, m2, m4, ns2pm4,ns2pm, m4min1, ns2pm4min1, ns2p1;
  Word16 *yref, *yreb, *yimf, *yimb, *yre, *yim, inc = 0 ,pos = 0;
  Word16 wSizeByTwo, wNumberStage;
  Word16 phs_tbl[SIZE];
  Word16 ii_table[NUM_STAGE];
    
  Word32 LtpA, LtpB, L_wtmp1;
              
  /* Determine the order of the transform, the length of decimated  */
  /* transforms m, and the step for the sine and cosine tables.     */
    
  switch(n) 
  {
    case 288:
      order = 5; 
      m     = 32;
      step  = 4; 
      wSizeByTwo   = voSIZE/8;          
      wNumberStage = NUM_STAGE-2;     
      break;
    case 576:
      order = 6; 
      m     = 64;
      step  = 2;
      wSizeByTwo   = voSIZE/4;          
      wNumberStage = NUM_STAGE-1;     
      break;
    case 1152:
      order = 7;
      m     = 128; 
      step  = 1;
      wSizeByTwo   = voSIZE/2;          
      wNumberStage = NUM_STAGE;     

      break;
    default:
      printf(" invalid fft9 size!\n");
      //exit(0);
	  break;
  }

  /* EXAMPLE RADIX5:                                                */
  /* ~~~~~~~~~~~~~~~                                                */
  /* The following table depicts indexing and locations of pointers */
  /* in an illustrative example case as n = 20 and m = n/5 = 4.     */
  /* The pointers yr0, yr1, yr2, yi0, yi1, yi2 are anchored to the  */
  /* beginning of the blocks in the coefficient vector. They will   */
  /* not be changed during the computation. The coefficient vector  */
  /* and the fixed pointers are shown in the second and third       */
  /* column of the table. The floating pointers yr0f, yr1f, yr2f,   */
  /* yi0f, yi1f, yi2f go forward over the corresponding blocks.     */
  /* Correspondingly yr0b, yr1b, yi0b, yi1b run backwards. These    */
  /* pointers will be repositioned during the algorithm.            */
  /*                                                                */
  /*   ==================================                           */
  /*   index  coeff    block     pointers                           */
  /*   ==================================                           */
  /*     0    re[0]    &yr0[0]    yr0f->                            */
  /*     1    re[1]                                                 */
  /*     2    re[2]                                                 */
  /*     3    re[3]               <-yr0b                            */
  /*     4    re[4]    &yr1[0]    yr1f->                            */
  /*     5    re[5]                                                 */
  /*     6    re[6]                                                 */
  /*     7    re[7]               <-yr1b                            */
  /*     8    re[8]    &yr2[0]    yr2f->                            */
  /*     9    re[9]                                                 */
  /*     10   re[10]                                                */
  /*     11   im[1]    &yi0[0]    yi0f->                            */
  /*     12   im[2]                                                 */
  /*     13   im[3]               <-yi0b                            */
  /*     14   im[4]    &yi1[0]                                      */
  /*     15   im[5]               yi1f->                            */
  /*     16   im[6]                                                 */
  /*     17   im[7]               <-yi1b                            */
  /*     18   im[8]    &yi2[0]                                      */
  /*     19   im[9]               yi2f->                            */
  /*   ==================================                           */

  /* Initialize the fixed and the floating pointers. */

  ns2 = shr(n,1);
  ms2 = shr(m,1);
  m2 = shl(m,1);
  m3  = add(m,shl(m,1));
  m4 = shl(m,2);
  ns2pm4 = add(ns2,m4);
  ns2pm = add(ns2,m);
  ns2p1 = add(ns2,1);
  m4min1 = sub(m4,1);
  ns2pm4min1 = sub(ns2pm4,1);

  zre  = &Z[0]; 
  zim  = &Z[m-1]; 
   
  yref = &Y[0]; 

  /* Compute the inverse butterflies. */

  /*zre++ = *yr0f++ + 2*(*yr1f++) + 2*(*yr2f++) + 2*(*yr3f++) + 2*(*yr4f++) ;*/

  L_wtmp1 = L_mult(*yref, 1);        
  yref+=m;
  L_wtmp1 = L_mac(L_wtmp1, *yref, 2);
  yref+=m;
  L_wtmp1 = L_mac(L_wtmp1, *yref, 2);
  yref+=m;
  L_wtmp1 = L_mac(L_wtmp1, *yref, 2);
  yref+=m;
  L_wtmp1 = L_mac(L_wtmp1, *yref, 2);
  *zre++ = round16(L_shl(L_wtmp1, 15-ISCALE1));
        
  for (i = 1; i < ms2; i++) 
  {
    yref = &Y[i];                                                    
    yreb = &Y[m4-i];                            
    yimf = &Y[ns2+i];                       
    yimb = &Y[ns2pm4-i];                    

    /*zre++ = *yr0f++ + *yr1f++ + *yr2f++ + *yr3f++ + *yr4f++ + *yr0b-- + *yr1b-- + *yr2b-- + *yr3b--;*/
    L_wtmp1 = L_mult(*yref, 1);
    yref += m;
    L_wtmp1 = L_mac(L_wtmp1, *yref, 1);
    yref += m;
    L_wtmp1 = L_mac(L_wtmp1, *yref, 1);
    yref += m;
    L_wtmp1 = L_mac(L_wtmp1, *yref, 1);
    yref += m;
    L_wtmp1 = L_mac(L_wtmp1, *yref, 1);
    L_wtmp1 = L_mac(L_wtmp1, *yreb, 1);
    yreb -= m;
    L_wtmp1 = L_mac(L_wtmp1, *yreb, 1);
    yreb -= m;
    L_wtmp1 = L_mac(L_wtmp1, *yreb, 1);
    yreb -= m;
    L_wtmp1 = L_mac(L_wtmp1, *yreb, 1);
    *zre++ = round16(L_shl(L_wtmp1, 15-ISCALE1));      
               
    /*zim-- = *yi0f++ + *yi1f++ + *yi2f++ + *yi3f++ + *yi4f++ - *yi0b-- - *yi1b-- - *yi2b-- - *yi3b--;*/
    L_wtmp1 = L_mult(*yimf, 1);
    yimf += m;
    L_wtmp1 = L_mac(L_wtmp1, *yimf, 1);
    yimf += m;
    L_wtmp1 = L_mac(L_wtmp1, *yimf, 1);
    yimf += m;
    L_wtmp1 = L_mac(L_wtmp1, *yimf, 1);
    yimf += m;
    L_wtmp1 = L_mac(L_wtmp1, *yimf, 1);
    L_wtmp1 = L_msu(L_wtmp1, *yimb, 1);
    yimb -= m;
    L_wtmp1 = L_msu(L_wtmp1, *yimb, 1);
    yimb -= m;
    L_wtmp1 = L_msu(L_wtmp1, *yimb, 1);
    yimb -= m;
    L_wtmp1 = L_msu(L_wtmp1, *yimb, 1);
    *zim-- = round16(L_shl(L_wtmp1, 15-ISCALE1));      
  }
  yref = &Y[ms2]; 
       
  /*zre = 2*(*yr0f) + 2*(*yr1f) + 2*(*yr2f) + 2*(*yr3f) + *yr4f;*/

  L_wtmp1 = L_mult(*yref, 2);
  yref+=m;
  L_wtmp1 = L_mac(L_wtmp1, *yref, 2);
  yref+=m;
  L_wtmp1 = L_mac(L_wtmp1, *yref, 2);
  yref+=m;
  L_wtmp1 = L_mac(L_wtmp1, *yref, 2);
  yref+=m;
  L_wtmp1 = L_mac(L_wtmp1, *yref, 1);
  *zre = round16(L_shl(L_wtmp1, 15-ISCALE1));
        
  for (k = 1; k < 9; k++)
  {
    inc = add(inc, (N_MAX/9));       
    pos = inc; 
              
    yre = &Y[m]; 
    yim = &Y[ns2pm]; 

    z  = &Z[shl(k,order)]; 

    /*z  = *yr0;*/ 
    LtpA = L_shl(Y[0],14);  

    /*z += *yr1*t_cosFx[pos] - *yi1*t_sinFxS4[pos];  ind++;*/
    LtpA = L_mac(LtpA,*yre,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_msu(LtpA,*yim,t_sinFxS4[pos]);
    yre += m;               /* MAR arx*/
    yim += m;
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }

    /*z += *yr2*t_cosFx[pos] - *yi2*t_sinFxS4[pos];  ind++;*/
    LtpA = L_mac(LtpA,*yre,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_msu(LtpA, *yim,t_sinFxS4[pos]);
    yre += m;
    yim += m;
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }

    /*z += *yr3*t_cosFx[pos] - *yi3*t_sinFxS4[pos];  ind++;*/
    LtpA = L_mac(LtpA,*yre,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_msu(LtpA,*yim,t_sinFxS4[pos]);
    yre += m;
    yim += m;
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }

    /*z += *yr4*t_cosFx[pos] - *yi4*t_sinFxS4[pos];  ind++;*/
    LtpA = L_mac(LtpA,*yre,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_msu(LtpA,*yim,t_sinFxS4[pos]);         
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }

    /*z += *yr4*t_cosFx[pos] + *yi4*t_sinFxS4[pos];  ind++;*/
    LtpA = L_mac(LtpA,*yre,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_mac(LtpA, *yim,t_sinFxS4[pos]);
    yre -= m;
    yim -= m;
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }

    /*z += *yr3*t_cosFx[pos] + *yi3*t_sinFxS4[pos];  ind++;*/
    LtpA = L_mac(LtpA, *yre,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_mac(LtpA,*yim,t_sinFxS4[pos]);
    yre -= m;
    yim -= m;
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }

    /*z += *yr2*t_cosFx[pos] + *yi2*t_sinFxS4[pos];  ind++;*/
    LtpA = L_mac(LtpA,*yre,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_mac(LtpA,*yim,t_sinFxS4[pos]);
    yre -= m;
    yim -= m;
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }

    /*z += *yr1*t_cosFx[pos] + *yi1*t_sinFxS4[pos];*/
    LtpA = L_mac(LtpA,*yre,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_mac(LtpA,*yim,t_sinFxS4[pos]);
                                
    *z = round16(L_shl(LtpA,1));

    zre  = &z[1];     zim  = &z[sub(m,1)];  

    pos = 0;             
                
    for (i = 1; i < ms2; i++) 
    {
      pos = add(pos,step);  
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }

      yref = &Y[i];  
      yreb = &Y[m4-i]; 
      yimf = &Y[ns2+i]; 
      yimb = &Y[ns2pm4 - i]; 
                
      LtpA = L_mult(*yref,t_sinFxS4[pos+COSOFFSET]);
      LtpA = L_msu(LtpA,*yimf,t_sinFxS4[pos]);
        
      /*zim  = *yr0f*t_sinFxS4[pos] + *yi0f*t_cosFx[pos];  ind++;*/
      LtpB = L_mult(*yref,t_sinFxS4[pos]);
      LtpB = L_mac(LtpB,*yimf,t_sinFxS4[pos + COSOFFSET]);
      yref += m;
      yimf += m;
      pos += inc;
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }

      /*zre += *yr1f*t_cosFx[pos] - *yi1f*t_sinFxS4[pos];*/
      LtpA = L_mac(LtpA,*yref,t_sinFxS4[pos+COSOFFSET]);
      LtpA = L_msu(LtpA,*yimf,t_sinFxS4[pos]);
        
      /*zim += *yr1f*t_sinFxS4[pos] + *yi1f*t_cosFx[pos];  ind++;*/
      LtpB = L_mac(LtpB,*yref,t_sinFxS4[pos]);
      LtpB = L_mac(LtpB,*yimf,t_sinFxS4[pos + COSOFFSET]);
      pos += inc;
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }

      yref += m;
      yimf += m;
                        
      /*zre += *yr2f*t_cosFx[pos] - *yi2f*t_sinFxS4[pos];*/
      LtpA = L_mac(LtpA,*yref,t_sinFxS4[pos+COSOFFSET]);
      LtpA = L_msu(LtpA,*yimf,t_sinFxS4[pos]);
        
      /*zim += *yr2f*t_sinFxS4[pos] + *yi2f*t_cosFx[pos];  ind++;*/
      LtpB = L_mac(LtpB,*yref,t_sinFxS4[pos]);
      LtpB = L_mac(LtpB,*yimf,t_sinFxS4[pos + COSOFFSET]);
      pos += inc;
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }
      yref += m;
      yimf += m;
                        
      /*zre += *yr3f*t_sinFxS4[pos+COSOFFSET]) - *yi3f*t_sinFxS4[pos];*/
      LtpA = L_mac(LtpA,*yref,t_sinFxS4[pos+COSOFFSET]);
      LtpA = L_msu(LtpA,*yimf,t_sinFxS4[pos]);
        
      /*zim += *yr3f*t_sinFxS4[pos] + *yi3f*t_cosFx[pos];  ind++;*/
      LtpB = L_mac(LtpB,*yref,t_sinFxS4[pos]);
      LtpB = L_mac(LtpB,*yimf,t_sinFxS4[pos + COSOFFSET]);
      pos += inc;
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }
      yref += m;
      yimf += m;
                        
      /*zre += *yr4f*t_cosFx[pos] - *yi4f*t_sinFxS4[pos];*/
      LtpA = L_mac(LtpA,*yref,t_sinFxS4[pos+COSOFFSET]);
      LtpA = L_msu(LtpA,*yimf,t_sinFxS4[pos]);
        
      /*zim += *yr4f*t_sinFxS4[pos] + *yi4f*t_cosFx[pos];  ind++;*/
      LtpB = L_mac(LtpB,*yref,t_sinFxS4[pos]);
      LtpB = L_mac(LtpB,*yimf,t_sinFxS4[pos + COSOFFSET]);
      pos += inc;
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }
                        
      /*zre += *yr3b*t_cosFx[pos] + *yi3b*t_sinFxS4[pos];*/
      LtpA = L_mac(LtpA,*yreb,t_sinFxS4[pos+COSOFFSET]);
      LtpA = L_mac(LtpA,*yimb,t_sinFxS4[pos]);
                
        
      /*zim += *yr3b*t_sinFxS4[pos] - *yi3b*t_cosFx[pos];  ind++;*/
      LtpB = L_mac(LtpB,*yreb,t_sinFxS4[pos]);
      LtpB = L_msu(LtpB,*yimb,t_sinFxS4[pos + COSOFFSET]);
      pos += inc;
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }
      yreb -=m;
      yimb -=m;
                        
      /*zre += *yr2b*t_cosFx[pos] + *yi2b*t_sinFxS4[pos];*/
      LtpA = L_mac(LtpA, *yreb,t_sinFxS4[pos+COSOFFSET]);
      LtpA = L_mac(LtpA,*yimb,t_sinFxS4[pos]);
        
      /*zim += *yr2b*t_sinFxS4[pos] - *yi2b*t_cosFx[pos];  ind++;*/
      LtpB = L_mac(LtpB,*yreb,t_sinFxS4[pos]);
      LtpB = L_msu(LtpB,*yimb,t_sinFxS4[pos + COSOFFSET]);
      pos += inc;
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }
      yreb -=m;
      yimb -=m;
                                                
      /*zre += *yr1b*t_cosFx[pos] + *yi1b*t_sinFxS4[pos];*/
      LtpA = L_mac(LtpA,*yreb,t_sinFxS4[pos+COSOFFSET]);
      LtpA = L_mac(LtpA,*yimb,t_sinFxS4[pos]);
        
      /*zim += *yr1b*t_sinFxS4[pos] - *yi1b*t_cosFx[pos];  ind++;*/
      LtpB = L_mac(LtpB,*yreb,t_sinFxS4[pos]);
      LtpB = L_msu(LtpB,*yimb,t_sinFxS4[pos + COSOFFSET]);
      pos += inc;
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }
      yreb -=m;
      yimb -=m;
                            
      /*zre += *yr0b*t_cosFx[pos] + *yi0b*t_sinFxS4[pos];*/
      LtpA = L_mac(LtpA,*yreb,t_sinFxS4[pos+COSOFFSET]);
      LtpA = L_mac(LtpA,*yimb,t_sinFxS4[pos]);
                
      /*zim += *yr0b*t_sinFxS4[pos] - *yi0b*t_cosFx[pos];*/
      LtpB = L_mac(LtpB,*yreb,t_sinFxS4[pos]);
      LtpB = L_msu(LtpB,*yimb,t_sinFxS4[pos+COSOFFSET]);
      pos += inc;
      
      if (sub(pos,N_MAX) >= 0)
      {
        pos = sub(pos,N_MAX);      /* circular addressing*/
      }

      *zre++ = round16(L_shl(LtpA,1));
      *zim-- = round16(L_shl(LtpB,1));
    }
                
    pos = add(pos,step);
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }
        
    yref = &Y[ms2]; 
    yreb = &Y[ns2-m]; 
    yimf = &Y[ns2+ms2]; 
    yimb = &Y[n-m]; 

    /*zre  = *yr0f*t_cosFx[pos] - *yi0f*t_sinFxS4[pos];   ind++;*/
    LtpA = L_mult(*yref,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_msu(LtpA,*yimf,t_sinFxS4[pos]);
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }
    yref +=m;
    yimf +=m;

    /*zre += *yr1f*t_cosFx[pos] - *yi1f*t_sinFxS4[pos];   ind++;*/
    LtpA = L_mac(LtpA,*yref,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_msu(LtpA,*yimf,t_sinFxS4[pos]);
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }
    yref +=m;
    yimf +=m;

    /*zre += *yr2f*t_cosFx[pos] - *yi2f*t_sinFxS4[pos];   ind++;*/
    LtpA = L_mac(LtpA,*yref,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_msu(LtpA,*yimf,t_sinFxS4[pos]);
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }
    yref +=m;
    yimf +=m;

    /*zre += *yr3f*t_cosFx[pos] - *yi3f*t_sinFxS4[pos];   ind++;*/
    LtpA = L_mac(LtpA,*yref,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_msu(LtpA,*yimf,t_sinFxS4[pos]);
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }
    yref +=m;
    yimf +=m;
                
    /*zre += *yr4f*(t_cosFx[pos] - t_sinFxS4[pos]);       ind++;*/
    LtpA = L_mac(LtpA,*yref,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_msu(LtpA,32767,t_sinFxS4[pos]);

    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }

    /*zre += *yr3b*t_cosFx[pos] + *yi3b*t_sinFxS4[pos];   ind++;*/
    LtpA = L_mac(LtpA,*yreb,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_mac(LtpA,*yimb,t_sinFxS4[pos]);
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }
    yreb -=m;
    yimb -=m;

    /*zre += *yr2b*t_cosFx[pos] + *yi2b*t_sinFxS4[pos];   ind++;*/
    LtpA = L_mac(LtpA,*yreb,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_mac(LtpA,*yimb,t_sinFxS4[pos]);
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }
    yreb -=m;
    yimb -=m;

    /*zre += *yr1b*t_cosFx[pos] + *yi1b*t_sinFxS4[pos];   ind++;*/
    LtpA = L_mac(LtpA,*yreb,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_mac(LtpA,*yimb,t_sinFxS4[pos]);
    pos += inc;
    
    if (sub(pos,N_MAX) >= 0)
    {
        pos = sub(pos,N_MAX);      /* circular addressing*/
    }
    yreb -=m;
    yimb -=m;

    /*zre += *yr0b*t_cosFx[pos] + *yi0b*t_sinFxS4[pos];*/
    LtpA = L_mac(LtpA,*yreb,t_sinFxS4[pos+COSOFFSET]);
    LtpA = L_mac(LtpA,*yimb,t_sinFxS4[pos]);
               
    *zre = round16(L_shl(LtpA,1));
        
    wtmp1 = sub(ORDER_MAX,order);
    wtmp2 = shl(1,wtmp1);        step = add(step,wtmp2);
  }

  /* Compute the inverse FFT for all nine blocks. */

  z0 = &Z[0]; 
  z1 = &z0[m];    /* z1 = &Z[ m];     */
  z2 = &z1[m];    /* z2 = &Z[2m];     */
  z3 = &z2[m];    /* z3 = &Z[3m];     */
  z4 = &z3[m];    /* z4 = &Z[4m];     */
  z5 = &z4[m];    /* z5 = &Z[5m];     */
  z6 = &z5[m];    /* z6 = &Z[6m];     */
  z7 = &z6[m];    /* z7 = &Z[7m];     */
  z8 = &z7[m];    /* z8 = &Z[8m];     */

  init_r_fft_fx(m, wSizeByTwo, wNumberStage, ii_table, phs_tbl);
        
  r_fft_fx(&z0[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z1[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z2[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z3[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z4[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z5[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z6[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z7[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);
  r_fft_fx(&z8[0], m, wSizeByTwo, wNumberStage, ii_table, phs_tbl, -1);

  /* signal reconstitution */
  x = X; 
  for (i = 0; i < m; i++)
  {
    *x++ = *z0++;           
    *x++ = *z1++;           
    *x++ = *z2++;           
    *x++ = *z3++;           
    *x++ = *z4++;           
    *x++ = *z5++;           
    *x++ = *z6++;           
    *x++ = *z7++;           
    *x++ = *z8++;           
  }

  return;
#endif
}


