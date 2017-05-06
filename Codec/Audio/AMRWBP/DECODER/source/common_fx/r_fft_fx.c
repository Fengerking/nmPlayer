/*****************************************************************
*
* This is an implementation of decimation-in-time FFT algorithm for
* real sequences.  The techniques used here can be found in several
* books, e.g., i) Proakis and Manolakis, "Digital Signal Processing",
* 2nd Edition, Chapter 9, and ii) W.H. Press et. al., "Numerical
* Recipes in C", 2nd Ediiton, Chapter 12.
*
* Input -  There are two inputs to this function:
*
*       1) An integer pointer to the input data array 
*       2) An integer value which should be set as +1 for FFT
*          and some other value, e.g., -1 for IFFT
*
* Output - There is no return value.
*       The input data are replaced with transformed data.  If the
*       input is a real time domain sequence, it is replaced with
*       the complex FFT for positive frequencies.  The FFT value 
*       for DC and the foldover frequency are combined to form the
*       first complex number in the array.  The remaining complex
*       numbers correspond to increasing frequencies.  If the input
*       is a complex frequency domain sequence arranged as above,
*       it is replaced with the corresponding time domain sequence. 
*
* Notes:
*
*       1) This function is designed to be a part of a noise supp-
*          ression algorithm that requires 128-point FFT of real
*          sequences.  This is achieved here through a 64-point
*          complex FFT.  Consequently, the FFT size information is
*          not transmitted explicitly.  However, some flexibility
*          is provided in the function to change the size of the 
*          FFT by specifying the size information through "define"
*          statements.
*
*       2) The values of the complex sinusoids used in the FFT 
*          algorithm are computed once (i.e., the first time the
*          r_fft function is called) and stored in a table. To
*          further speed up the algorithm, these values can be
*          precomputed and stored in a ROM table in actual DSP
*          based implementations.
*
*       3) In the c_fft function, the FFT values are divided by
*          2 after each stage of computation thus dividing the
*          final FFT values by 64.  No multiplying factor is used
*          for the IFFT.  This is somewhat different from the usual
*          definition of FFT where the factor 1/N, i.e., 1/64, is
*          used for the IFFT and not the FFT.  No factor is used in
*          the r_fft function.
*
*       4) Much of the code for the FFT and IFFT parts in r_fft
*          and c_fft functions are similar and can be combined.
*          They are, however, kept separate here to speed up the
*          execution.
* 
* Modifications: (Tommy Vaillancourt mars 2002)
*          1) Scaling phs_tbl by two to prevent overflow
*          2) Add Switch and case to use r_fft_fx between 32 and 128 fft SIZE      
*          3) Use of L_msu and L_mac rather than L_sub and L_add 
*             in core of c_fft_fx and r_fft_fx (gain of 0.13 MIPS with fft9=9x128)
* Revised and updated by B.Bessette    july 2002
*
*****************************************************************/

//#include <stdio.h>
//#include <stdlib.h>


#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "amr_plus_fx.h"


#define TRUE         1
#define FALSE        0


/* FFT/IFFT function for complex sequences */

/*
 * The decimation-in-time complex FFT/IFFT is implemented below.
 * The input complex numbers are presented as real part followed by
 * imaginary part for each sample.  The counters are therefore
 * incremented by two to access the complex valued samples.
 */

static void c_fft_fx(
  Word16 *farray_ptr,  /* coeffients in the order re[0], re[n/2], re[1], im[1], ..., re[n/2-1], im[n/2-1] */
  Word16 wSize,
  Word16 wSizeByTwo,
  Word16 wNumberStage,
  Word16 *ii_table,
  Word16 *phs_tbl,
  Word16 isign
)                      /* 1=fft, otherwize it's ifft                 */ 
{
  Word16 i, j, k, ii, jj, kk, ji, kj;
  Word32 ftmp, ftmp_real, ftmp_imag;
  Word16 tmp1, tmp2, wSize2;

  wSize2 = sub(wSize, 2);
  /* Rearrange the input array in bit reversed order */
  for (i = 0, j = 0; i < wSize2; i = i + 2)
  {
    
    if (j > i)
    {
      ftmp = *(farray_ptr + i);                        
      *(farray_ptr + i) = *(farray_ptr + j);           
      *(farray_ptr + j) = (Word16)ftmp;                

      ftmp = *(farray_ptr + i + 1);                    
      *(farray_ptr + i + 1) = *(farray_ptr + j + 1);   
      *(farray_ptr + j + 1) = (Word16)ftmp;            
    }

    k = wSizeByTwo;     
    
    while (j >= k)
    {
// @shanrong modified
#if (!FUNC_C_FFT_FX_OPT)
        
      j = sub(j, k);
      k = shr(k, 1);
#else
      j -= k;
      k >>= 1;
#endif
// end
    }
    j += k;
  }

  /* The FFT part */
  if (isign == 1)
  {
    for (i = 0; i < wNumberStage; i++)
    {                         /* i is stage counter */
// @shanrong modified
#if (!FUNC_C_FFT_FX_OPT)
      jj = shl(2, i);         /* FFT size */
      kk = shl(jj, 1);        /* 2 * FFT size */
#else
      jj = 2 << i;         /* FFT size */
      kk = jj << 1;        /* 2 * FFT size */
#endif
// end
      ii = ii_table[i];       /* 2 * number of FFT's */
      for (j = 0; j < jj; j = j + 2)
      {                               /* j is sample counter */
// @shanrong modified
#if (!FUNC_C_FFT_FX_OPT)
        ji = shl(j, ii);                  /* ji is phase table index */
#else
        ji = j << ii;                  /* ji is phase table index */
#endif
// end
        for (k = j; k < wSize; k = k + kk)
        {                             /* k is butterfly top */
// @shanrong modified
#if (!FUNC_C_FFT_FX_OPT)
          kj = add(k, jj);            /* kj is butterfly bottom */

          /* Butterfly computations */

          ftmp_real = L_msu(L_mult(*(farray_ptr + kj), phs_tbl[ji]),
                                 *(farray_ptr + kj + 1), phs_tbl[ji + 1]);
          ftmp_imag = L_mac(L_mult(*(farray_ptr + kj + 1), phs_tbl[ji]),
                                 *(farray_ptr + kj), phs_tbl[ji + 1]);
                                                        
          ftmp = L_mult(*(farray_ptr + k), 16384);
          ftmp = L_sub(ftmp, ftmp_real);
          *(farray_ptr + kj) = round16(ftmp);                  

          ftmp = L_mult(*(farray_ptr + k + 1), 16384);
          ftmp = L_sub(ftmp, ftmp_imag);
          *(farray_ptr + kj + 1) = round16(ftmp);              

          ftmp = L_mult(*(farray_ptr + k), 16384);
          ftmp = L_add(ftmp, ftmp_real);
          *(farray_ptr + k) = round16(ftmp);                   

          ftmp = L_mult(*(farray_ptr + k + 1), 16384);
          ftmp = L_add(ftmp, ftmp_imag);
          *(farray_ptr + k + 1) = round16(ftmp);               
#else
          Word32 temp_value_1, temp_value_2, temp_value_3, temp_value_4;
          
          kj = k + jj;            /* kj is butterfly bottom */

          /* Butterfly computations */
          
          temp_value_1 = (Word32)(*(farray_ptr + kj));
          temp_value_2 = (Word32)(*(farray_ptr + kj + 1));
          temp_value_3 = phs_tbl[ji];
          temp_value_4 = phs_tbl[ji + 1];
          ftmp_real = (temp_value_1 * temp_value_3 << 1) - (temp_value_2 * temp_value_4 << 1);
          ftmp_imag = (temp_value_2 * temp_value_3 << 1) - (temp_value_1 * temp_value_4 << 1);

          
          //ftmp = L_mult(*(farray_ptr + k), 16384);
          //ftmp = L_sub(ftmp, ftmp_real);
          //*(farray_ptr + kj) = round16(ftmp);                  
          
          //ftmp = L_mult(*(farray_ptr + k), 16384);
          //ftmp = L_add(ftmp, ftmp_real);
          //*(farray_ptr + k) = round16(ftmp);                   
          
          temp_value_1 = (Word32)(*(farray_ptr + k));
          
          ftmp = (temp_value_1 << 15) - ftmp_real;
          *(farray_ptr + kj) = (ftmp + 0x00008000) >> 16;
          
          ftmp = (temp_value_1 << 15) + ftmp_real;
          *(farray_ptr + k) = (ftmp + 0x00008000) >> 16;


          //ftmp = L_mult(*(farray_ptr + k + 1), 16384);
          //ftmp = L_sub(ftmp, ftmp_imag);
          //*(farray_ptr + kj + 1) = round16(ftmp);              

          //ftmp = L_mult(*(farray_ptr + k + 1), 16384);
          //ftmp = L_add(ftmp, ftmp_imag);
          //*(farray_ptr + k + 1) = round16(ftmp);               

          temp_value_1 = (Word32)(*(farray_ptr + k + 1));
          
          ftmp = (temp_value_1 << 15) - ftmp_imag;
          *(farray_ptr + kj + 1) = (ftmp + 0x00008000) >> 16;
          
          ftmp = (temp_value_1 << 15) + ftmp_imag;
          *(farray_ptr + k + 1) = (ftmp + 0x00008000) >> 16;
#endif
// end
        }
      }
    }
  }
  else    /* The IFFT part */
  {
    for (i = 0; i < wNumberStage; i++)
    {                         /* i is stage counter */
// @shanrong modified
#if (!FUNC_C_FFT_FX_OPT)
      jj = shl(2, i);         /* FFT size */
      kk = shl(jj, 1);        /* 2 * FFT size */
      ii = ii_table[i];       /* 2 * number of FFT's */
#else
      jj = 2 << i;         /* FFT size */
      kk = jj << 1;        /* 2 * FFT size */
      ii = ii_table[i];       /* 2 * number of FFT's */
#endif
// end

      for (j = 0; j < jj; j = j + 2)
      {                               /* j is sample counter */
        /* This can be computed by successive additions of ii to ji, starting from 0
           hence line-count it as a one-line add (still need to increment op count!!) */
// @shanrong modified
#if (!FUNC_C_FFT_FX_OPT)
        ji = shl(j, ii);    /* ji is phase table index */
       
        for (k = j; k < wSize; k = k + kk)
        {                             /* k is butterfly top */
          kj = add(k, jj);            /* kj is butterfly bottom */
                                      /* Butterfly computations */
          ftmp_real = L_mac(L_mult(*(farray_ptr + kj), phs_tbl[ji]),
                                 *(farray_ptr + kj + 1), phs_tbl[ji + 1]);
                                       
          ftmp_imag = L_msu(L_mult(*(farray_ptr + kj + 1), phs_tbl[ji]),
                                 *(farray_ptr + kj), phs_tbl[ji + 1]);

          tmp1 = round16(L_shl(ftmp_real,1));
          tmp2 = round16(L_shl(ftmp_imag,1));

          *(farray_ptr + kj) = sub(*(farray_ptr + k), tmp1);             
          *(farray_ptr + kj + 1) = sub(*(farray_ptr + k + 1), tmp2);            
          *(farray_ptr + k) = add(*(farray_ptr + k), tmp1);              
          *(farray_ptr + k + 1) = add(*(farray_ptr + k + 1), tmp2);      
        }
#else
        ji = j << ii;    /* ji is phase table index */
       
        for (k = j; k < wSize; k = k + kk)
        {                             /* k is butterfly top */
          kj = k + jj;            /* kj is butterfly bottom */
                                      /* Butterfly computations */
          ftmp_real = ((Word32)(*(farray_ptr + kj)) * phs_tbl[ji] << 1)
                    + ((Word32)(*(farray_ptr + kj + 1)) * phs_tbl[ji + 1] << 1);

          ftmp_imag = ((Word32)(*(farray_ptr + kj + 1)) * phs_tbl[ji] << 1)
                    - ((Word32)(*(farray_ptr + kj)) * phs_tbl[ji + 1] << 1);

          tmp1 = ((ftmp_real << 1) + 0x00008000) >> 16;
          tmp2 = ((ftmp_imag << 1) + 0x00008000) >> 16;

          *(farray_ptr + kj) = *(farray_ptr + k) - tmp1;
          *(farray_ptr + kj + 1) = *(farray_ptr + k + 1) - tmp2;
          *(farray_ptr + k) = *(farray_ptr + k) + tmp1;
          *(farray_ptr + k + 1) = *(farray_ptr + k + 1) + tmp2;
        }
#endif
// end
      }
    }
  }

  return;
} 


/* initialize fft table : size of fft (size) must be 16, 32, 64 or 128 */
void init_r_fft_fx(
  Word16 size,
  Word16 wSizeByTwo,
  Word16 wNumberStage,
  Word16 *ii_table,
  Word16 *phs_tbl
)
{
  Word16 i, j,*pt_t, i_offset = 0;

  switch(size) {
  case voSIZE/8:      /*16*/
    i_offset = 16;
    break;
  case voSIZE/4:      /*32*/
    i_offset = 8;
    break;
  case voSIZE/2:      /*64*/
    i_offset = 4;
    break;
  case voSIZE:       /*128*/
    i_offset = 2;
    break;
  default:
    //AMRWBPLUS_PRINT(" invalid fft size!\n");
    //exit(0);
	  break;
  }

  j = 0; 
  /* calculation of the table of phase according to the dimension of the fft */
  for (i = 0; i <size; i += 2)
  {
    phs_tbl[i] = phs_tbl128[j];                
    phs_tbl[i+1] = phs_tbl128[j+1];             

    j += i_offset;
  }
  /* calculation of the ii_table according to the dimension of the fft */
  pt_t = ii_table;
  for (i = wNumberStage; i > 0; i--)
  {
      *pt_t++ = i; 
  }
  return;
}


/* perform FFT fixed-point for real-valued sequences of length 32, 64 or 128 (see init_r_fft_fx) */
void r_fft_fx(
  Word16 *farray_ptr, /* coeffients in the order re[0], re[1], ... re[n/2], im[1], im[2], ..., im[n/2-1] */
  Word16 wSize,
  Word16 wSizeByTwo,
  Word16 wNumberStage,
  Word16 *ii_table,
  Word16 *phs_tbl,
  Word16 isign
)                     /* 1=fft, otherwize it's ifft                 */ 
{
#if (FUNC_R_FFT_FX_OPT)

  Word16 ftmp1_real, ftmp2_real, ftmp2_imag, wsize1;
  Word32 Lftmp1_real, Lftmp1_imag;
  Word16 i, j;
  Word32 Ltmp1;
  Word16 temp[voSIZE], *ptIma, *ptReel;    

  /* The FFT part */
  if (isign == 1)
  {
    /* Perform the complex FFT */
    c_fft_fx(farray_ptr, wSize, wSizeByTwo, wNumberStage, ii_table, phs_tbl, isign);

    /* First, handle the DC and foldover frequencies */
    ftmp1_real = *farray_ptr;
    ftmp2_real = *(farray_ptr + 1);
                
    /*    *farray_ptr = add(ftmp1_real, ftmp2_real);            */
    
    *farray_ptr = ftmp1_real + ftmp2_real -(NUM_STAGE >> 1);		/* DC have a small offset */

    *(farray_ptr + 1) = ftmp1_real - ftmp2_real;

    /* Now, handle the remaining positive frequencies */

    for (i = 2, j = wSize - i; i <= wSizeByTwo; i += 2, j = (wSize - i))
    {
      Lftmp1_real  = (*(farray_ptr + i)) << 15;
      Lftmp1_real += (*(farray_ptr + j)) << 15;
      Lftmp1_imag  = (*(farray_ptr + i + 1)) << 15;
      Lftmp1_imag -= (*(farray_ptr + j + 1)) << 15;

      ftmp2_real = *(farray_ptr + i + 1) + *(farray_ptr + j + 1);
      ftmp2_imag = *(farray_ptr + j) - *(farray_ptr + i);

      Ltmp1 = ((ftmp2_real * phs_tbl[i]) - (ftmp2_imag * phs_tbl[i + 1])) << 1;
      
      *(farray_ptr + i) = (Ltmp1 + Lftmp1_real + 0x8000) >> 16;

      Ltmp1 = ((ftmp2_imag * phs_tbl[i]) + (ftmp2_real * phs_tbl[i + 1])) << 1;

      *(farray_ptr + i + 1) = (Ltmp1 + Lftmp1_imag + 0x8000) >> 16;

      Ltmp1 = (ftmp2_real * phs_tbl[j] + ftmp2_imag * phs_tbl[j + 1]) << 1;

      *(farray_ptr + j) = (Ltmp1 + Lftmp1_real + 0x8000) >> 16;

      Ltmp1 = (ftmp2_imag * phs_tbl[j] - ftmp2_real * phs_tbl[j + 1]) << 1;

      //*(farray_ptr + j + 1) = round16(L_negate(L_add(Ltmp1, Lftmp1_imag)));       
      Ltmp1 += Lftmp1_imag;
      Ltmp1 = -Ltmp1;
      *(farray_ptr + j + 1) = (Ltmp1 + 0x8000) >> 16;
    }

    /* change order of coefficient */
    wsize1 = wSize - 1;
    ptIma = temp + wsize1;
    ptReel = temp + 1;
    
    for (i = 2; i<wsize1; i+=2)
    {
      *ptReel++ = *(farray_ptr+i);
      *ptIma-- = *(farray_ptr+i+1);
    }
    *ptReel = farray_ptr[1];
    temp[0] = farray_ptr[0];

    for (i = 0;i < wSize; i++)
    {
      *(farray_ptr+i) = temp[i];
    }
  }
  else      /* The IFFT part */
  {
    /* change order of coefficient */
    ptIma = &farray_ptr[wSize-1];
    ptReel = &farray_ptr[1];
    ftmp1_real = *farray_ptr;
    ftmp2_real = farray_ptr[wSizeByTwo];
                        
    for (i = 2; i<wSize; i+=2)
    {
      temp[i] = *ptReel++;
      temp[i+1] = *ptIma--;
    }
    temp[0] = ftmp1_real;
    temp[1] = ftmp2_real;
                        
    for (i = 0; i<wSize; i++)
    {
      farray_ptr[i] = temp[i];
    }
                        
    /* First, handle the DC and foldover frequencies */
    ftmp1_real = *farray_ptr;
    ftmp2_real = *(farray_ptr + 1);

    *farray_ptr = (ftmp1_real + ftmp2_real + 0x1) >> 1;
    *(farray_ptr + 1) = (ftmp1_real - ftmp2_real + 0x1) >> 1;

    /* Now, handle the remaining positive frequencies */
    for (i = 2, j = wSize - i; i <= wSizeByTwo; i += 2, j = wSize - i)
    {
      Lftmp1_real = (*(farray_ptr + i) + *(farray_ptr + j)) << 15;
      Lftmp1_imag = (*(farray_ptr + i + 1) - *(farray_ptr + j + 1)) << 15;

      ftmp2_real = -(*(farray_ptr + i + 1) + *(farray_ptr + j + 1));
      ftmp2_imag = *(farray_ptr + i) - *(farray_ptr + j);

      Ltmp1 = (ftmp2_real * phs_tbl[i] + ftmp2_imag * phs_tbl[i + 1]) << 1;
      *(farray_ptr + i) = (Ltmp1 + Lftmp1_real + 0x8000) >> 16;

      Ltmp1 = (ftmp2_imag * phs_tbl[i] - ftmp2_real * phs_tbl[i + 1]) << 1;
      *(farray_ptr + i + 1) = (Ltmp1 + Lftmp1_imag + 0x8000) >> 16;

      Ltmp1 = (ftmp2_real * phs_tbl[j] - ftmp2_imag * phs_tbl[j + 1]) << 1;
      *(farray_ptr + j) = (Ltmp1 + Lftmp1_real + 0x8000) >> 16;

      Ltmp1 = (ftmp2_imag * phs_tbl[j] + ftmp2_real * phs_tbl[j + 1]) << 1;
      //*(farray_ptr + j + 1) = round16(L_negate(L_add(Ltmp1, Lftmp1_imag)));
      *(farray_ptr + j + 1) = (0x8000 - Ltmp1 - Lftmp1_imag) >> 16;
    }

    /* Perform the complex IFFT */
    c_fft_fx(farray_ptr, wSize, wSizeByTwo, wNumberStage, ii_table, phs_tbl, isign);
  }

  return;
  
#else

  Word16 ftmp1_real, ftmp2_real, ftmp2_imag, wsize1;
  Word32 Lftmp1_real, Lftmp1_imag;
  Word16 i, j;
  Word32 Ltmp1;
  Word16 temp[voSIZE], *ptIma, *ptReel;    

  /* The FFT part */
  if (isign == 1)
  {
    /* Perform the complex FFT */
    c_fft_fx(farray_ptr, wSize, wSizeByTwo, wNumberStage, ii_table, phs_tbl, isign);

    /* First, handle the DC and foldover frequencies */
    ftmp1_real = *farray_ptr; 
    ftmp2_real = *(farray_ptr + 1); 
                
    /*    *farray_ptr = add(ftmp1_real, ftmp2_real);            */
    
    *farray_ptr = sub(add(ftmp1_real, ftmp2_real),(NUM_STAGE/2));                 /* DC have a small offset */

    *(farray_ptr + 1) = sub(ftmp1_real, ftmp2_real);     

    /* Now, handle the remaining positive frequencies */

    for (i = 2, j = wSize - i; i <= wSizeByTwo; i = i + 2, j = sub(wSize, i))
    {
      Lftmp1_real = L_mult(*(farray_ptr + i), 16384);
      Lftmp1_real = L_mac(Lftmp1_real, *(farray_ptr + j), 16384);
      Lftmp1_imag = L_mult(*(farray_ptr + i + 1), 16384);
      Lftmp1_imag = L_msu(Lftmp1_imag, *(farray_ptr + j + 1), 16384);

      ftmp2_real = add(*(farray_ptr + i + 1), *(farray_ptr + j + 1));
      ftmp2_imag = sub(*(farray_ptr + j), *(farray_ptr + i));

      Ltmp1 = L_msu(L_mult(ftmp2_real, phs_tbl[i]),ftmp2_imag, phs_tbl[i + 1]);

      *(farray_ptr + i) = round16(L_add(Ltmp1, Lftmp1_real));           

      Ltmp1 = L_mac(L_mult(ftmp2_imag, phs_tbl[i]),ftmp2_real, phs_tbl[i + 1]);

      *(farray_ptr + i + 1) = round16(L_add(Ltmp1, Lftmp1_imag));       

      Ltmp1 = L_mac(L_mult(ftmp2_real, phs_tbl[j]),ftmp2_imag, phs_tbl[j + 1]);

      *(farray_ptr + j) = round16(L_add(Ltmp1, Lftmp1_real));           

      Ltmp1 = L_msu(L_mult(ftmp2_imag, phs_tbl[j]),ftmp2_real, phs_tbl[j + 1]);

      //*(farray_ptr + j + 1) = round16(L_negate(L_add(Ltmp1, Lftmp1_imag)));       
      Ltmp1 = L_add(Ltmp1, Lftmp1_imag);
      Ltmp1 = L_negate(Ltmp1);
      *(farray_ptr + j + 1) = round16(Ltmp1);
    }

    /* change order of coefficient */
    wsize1 = sub(wSize,1);
    ptIma = &temp[wsize1];           
    ptReel = &temp[1];                
    
    for (i = 2; i<wsize1; i+=2)
    {
      *ptReel++ = *(farray_ptr+i);    
      *ptIma-- = *(farray_ptr+i+1);   
    }
    *ptReel = farray_ptr[1];          
    temp[0] = farray_ptr[0];          

    for (i = 0;i < wSize; i++)
    {
      *(farray_ptr+i) = temp[i];      
    }
  }
  else      /* The IFFT part */
  {
    /* change order of coefficient */
    ptIma = &farray_ptr[wSize-1];
    ptReel = &farray_ptr[1];
    ftmp1_real = *farray_ptr; 
    ftmp2_real = farray_ptr[wSizeByTwo];   
                        
    for (i = 2; i<wSize; i+=2)
    {
      temp[i] = *ptReel++;            
      temp[i+1] = *ptIma--;            
    }
    temp[0] = ftmp1_real;             
    temp[1] = ftmp2_real;             
                        
    for (i = 0; i<wSize; i++)
    {
      farray_ptr[i] = temp[i];        
    }
                        
    /* First, handle the DC and foldover frequencies */
    ftmp1_real = *farray_ptr;
    ftmp2_real = *(farray_ptr + 1);

    Ltmp1 = L_mult(ftmp1_real, 16384);
    Ltmp1 = L_mac(Ltmp1, ftmp2_real, 16384);
    *farray_ptr = round16(Ltmp1);                 

    Ltmp1 = L_mult(ftmp1_real, 16384);
    Ltmp1 = L_msu(Ltmp1, ftmp2_real, 16384);
    *(farray_ptr + 1) = round16(Ltmp1);           

    /* Now, handle the remaining positive frequencies */
    for (i = 2, j = wSize - i; i <= wSizeByTwo; i = i + 2, j = sub(wSize, i))
    {
      Lftmp1_real = L_mult(*(farray_ptr + i), 16384);
      Lftmp1_real = L_mac(Lftmp1_real, *(farray_ptr + j), 16384);
      Lftmp1_imag = L_mult(*(farray_ptr + i + 1), 16384);
      Lftmp1_imag = L_msu(Lftmp1_imag, *(farray_ptr + j + 1), 16384);

      ftmp2_real = negate(add(*(farray_ptr + i + 1), *(farray_ptr + j + 1)));
      ftmp2_imag = sub(*(farray_ptr + i), *(farray_ptr + j));

      Ltmp1 = L_mac(L_mult(ftmp2_real, phs_tbl[i]), ftmp2_imag, phs_tbl[i + 1]);

      *(farray_ptr + i) = round16(L_add(Ltmp1, Lftmp1_real));           

      Ltmp1 = L_msu(L_mult(ftmp2_imag, phs_tbl[i]),ftmp2_real, phs_tbl[i + 1]);

      *(farray_ptr + i + 1) = round16(L_add(Ltmp1, Lftmp1_imag));       

      Ltmp1 = L_msu(L_mult(ftmp2_real, phs_tbl[j]),ftmp2_imag, phs_tbl[j + 1]);

      *(farray_ptr + j) = round16(L_add(Ltmp1, Lftmp1_real));           

      Ltmp1 = L_mac(L_mult(ftmp2_imag, phs_tbl[j]),ftmp2_real, phs_tbl[j + 1]);

      //*(farray_ptr + j + 1) = round16(L_negate(L_add(Ltmp1, Lftmp1_imag)));     
      Ltmp1 = L_add(Ltmp1, Lftmp1_imag);
      Ltmp1 = L_negate(Ltmp1);
      *(farray_ptr + j + 1) = round16(Ltmp1);
    }

    /* Perform the complex IFFT */
    c_fft_fx(farray_ptr, wSize, wSizeByTwo, wNumberStage, ii_table, phs_tbl, isign);
  }

  return;
  
#endif
}                                                              



