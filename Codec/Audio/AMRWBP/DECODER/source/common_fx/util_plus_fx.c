/*-------------------------------------------------------------------*
 *                         UTIL_fx.C									 *
 *-------------------------------------------------------------------*/
 
#include "typedef.h"
#include "basic_op.h"
#include "count.h"


#include "amr_plus_fx.h"
//#include "stdio.h"
/*---------------------------------------------------------------*
 * Cos_windows                                                   
 *                                                               
 * To compute cosine window in tcx overlap
 * Based on a 128 cosine table length
 *---------------------------------------------------------------*/
#define DELAY_MAX (D_BPF +L_SUBFR+ L_BSP + 2*D_NC + L_FDEL + 32*D_STEREO_TCX/5)
void Cos_windowLP(Word16 *fh, Word16 n1)
{
  Word16 i, n1_3;
  Word32 Ltmp;
  const Word16 *pt_cosup, *pt_cosdwn;

  
  if(n1 == (L_WINDOW/2))    /* LOW_RATE WINDOW */
  {
    pt_cosup = Cos_WindLR ;        
    for(i = 0; i < n1; i++)
    {
      *fh = *pt_cosup;      
      pt_cosup ++;
      fh++;
    }
    pt_cosup--;
    for(i = 0; i < n1; i++)
    {
      *fh = *pt_cosup;      
      pt_cosup--;
      fh++;
    }  
  }
  else                            /* HIGH_RATE WINDOW */  
  {
    pt_cosup = Cos_wind;            
    *fh++ = shr_r(*pt_cosup,1);      
    n1_3 = sub(n1,3);
    for(i = 0; i < n1_3; i+= 2)
    {
      Ltmp = L_mult(*(pt_cosup + 1), 8192);     /* fh = 1(cos[0] + cos[1])/4 + cos[0] */
      Ltmp = L_msu(Ltmp, *pt_cosup, 8192);
      *fh++ = add(round16(Ltmp), *pt_cosup);     

      Ltmp = L_mult(*(pt_cosup + 1), 24576);    /* fh = 3(cos[0] + cos[1])/4 + cos[0] */
      Ltmp = L_msu(Ltmp, *pt_cosup, 24576);
      *fh++ = add(round16(Ltmp), *pt_cosup);     

      pt_cosup ++;
      
    }
    *fh++ = *pt_cosup;    

    pt_cosdwn = (fh-1);     
    for(i = 0; i < n1; i++)
    {
      *fh = *pt_cosdwn;      
      pt_cosdwn--;
      fh++;
    }  
  
  }
}

void Cos_window(Word16 *fh, Word16 n1, Word16 n2, Word16 inc2)
{ 
  Word16 i;
  Word16 inc, offset,offset2;
  Word32 Ltmp;
  const Word16 *pt_cosup, *pt_cosdwn;

  /* compute offset from beginning of table andjump size */
  offset2 = 1;  
  
  if(sub(n1, L_OVLP) == 0)
  {
     inc = 1;  
     offset2 = 0;  
  }
  else if(sub(n1, L_OVLP/2) == 0)
  {
     inc = 2;  
  }
  else if(sub(n1, L_OVLP/4) == 0)
  {
     inc = 4;  
  }
  else if(sub(n1, L_OVLP/8) == 0)
  {
     inc = 8;  
  }
  else
  {
     inc = 16;  
  }
  offset = sub(shr(inc,1),1);
  
  if(offset < 0)
  {
    offset = 0;   
  }
  
  /* Calculate first cosine part */
  pt_cosup = Cos_wind + offset;
  for(i = 0;i <n1; i++)
  {
    Ltmp = L_mult(*pt_cosup, 16384);
    Ltmp = L_mac(Ltmp, *(pt_cosup+offset2), 16384);
    *fh = round16(Ltmp);        
    fh++; pt_cosup += inc;
  }

  /* compute offset from beginning of table and jump size for second part*/
  offset = sub(shr(inc2,1),1);
  
  if(offset < 0)
  {
    offset = 0;   
  }
  
  if(sub(inc2,1) == 0)
  {
    offset2 = 0;      
  }
  else
  {
    offset2 = 1;      
  }

  pt_cosdwn = Cos_wind + L_OVLP - 1 - offset; 
  /* Calculate second cosine part */
  for(i = 0;i <n2; i++)
  {
    Ltmp = L_mult(*pt_cosdwn, 16384);
    Ltmp = L_mac(Ltmp, *(pt_cosdwn-offset2), 16384);
    *fh = round16(Ltmp);        
    fh++; pt_cosdwn -= inc2;
  }
  return;
}
void Scale_tcx_ifft( 
  Word16 exc[],         /* (i/o)  : excitation to rescale       Q_exc */
  Word16 lg,            /* (i)    : frame size                        */
  Word16 *Q_exc         /* (i/o)  : State structure                   */
)  
{
  Word16 i, tmp, rem_bit;
  Word32  Ltmp;

  Ltmp = 0;               move32();
  for(i=0; i<lg; i++) 
  {
    Ltmp = L_mac(Ltmp, exc[i], exc[i]);
  }
  tmp = 0;      
  /* Estimate appropriate scaling (energy of coef can be up to 2^25) */

  /* Note: ener signal = ener coef * lg/8 (using fixed-point fft)
     this mean:
      -> RMS signal = sqrt(ener coef / 8)
      -> ener coef = 8 * RMS^2
     with RMS < 3072, ener coef < 2^(23+3) (RMS < 2896) */

  rem_bit = 4;    
   
  if(sub(lg,1152) == 0)
  {
    rem_bit = 5;         /* Special case when lg = 1152 remove  one more bit*/
                                  /* To prevent Overflow in ifft9 (cast.wav) */
  }

  
  if (Ltmp != 0)
  {
    tmp = shr(sub(norm_l(Ltmp), /*(30-26)*/rem_bit), 1);  

    
    if (tmp < 0)
    {
      tmp = 0;             
    }
  }
  
  if(sub(tmp,10)>0)
  {
    tmp = 10;   
  }

  *Q_exc = tmp;            
  
  if (tmp!=0)
  {
    voAMRWBPDecScale_sig(exc, lg, tmp);
  }
  return; 
}
void Ifft_reorder(Word16 xri[], Word16 lg)
{
  Word16 i, tmp1, lg2;
  Word16 im[576], *pt_re, *pt_im, *pt_ims;

  tmp1 = xri[1]; 
  pt_re = &xri[2]; 
  pt_im = &xri[3]; 
  pt_ims = im;     

  lg2 = shr(lg,1);
  for(i = 1; i<lg2; i++)
  {
    xri[i] = *pt_re;   
    *pt_ims++ = *pt_im;    
    pt_im += 2;
    pt_re += 2;
  }
  xri[lg2] = tmp1;   
  pt_im = &im[0];   
  lg2 = add(lg2,1);
  for(i = lg2; i<lg; i++)
  {
    xri[i] = *pt_im++;   
  }
}

void FFT_reorder(Word16 Y[],Word16 lg)
{
  Word16 i, Z[1152], lg2; 
  for (i = 0; i < lg; i++) 
  {
    Z[i] = Y[i];    
  }
  lg2 = shr(lg,1);
  Y[1] = Z[lg2];     

  for(i = 1; i < lg2; i++)
  {
    Y[i*2] = Z[i];                
    Y[(i*2)+1] = Z[lg2+i];      
  }

}

void Delay(
  Word16 signal[],    /* (i/o): Signal to delay */
  Word16 lg,          /* (i)  : Length          */
  Word16 delay,       /* (i)  : Delay to apply  */
  Word16 mem[]        /* (i/o): Delay memory    */
)
{ 
  Word16 i;
  Word16 buf[DELAY_MAX];

  for (i=0; i<delay; i++) 
  {
    buf[i] = mem[i];                
  }
  for (i=0; i<delay; i++) 
  {
    mem[i] = signal[lg-delay+i];    
  }
  lg = sub(lg,1);
  for (i=lg; i>=delay; i--) 
  {
    signal[i] = signal[i-delay];    
  }
  for (i=0; i<delay; i++) 
  {
    signal[i] = buf[i];             
  }
  return;
}
Word16 get_nb_bits(Word16 extension, Word16 mode, Word16 st_mode)
{
  
  Word16 nb_bits;
    if (mode != 14 && mode != 15)
    {
        if(extension != 0)
        {

            nb_bits = add(NBITS_CORE_FX[mode] ,NBITS_BWE); 
            if (st_mode>=0) 
            {
                nb_bits = add(nb_bits, add(StereoNbits_FX[st_mode] , NBITS_BWE)); 
            }
        }
        else
        {
            nb_bits = NBITS_CORE_AMR_WB_FX[mode]; 
        }
    }
    else
    {
        nb_bits = 0;        
    }
  
  return nb_bits;
    
}
