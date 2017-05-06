/*-------------------------------------------------------------------*
 * Function decim_12k8() and oversamp_12k8()                         *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                         *
 * decim_12k8    : decimation from fs to 12.8kHz.                    *
 * oversamp_12k8 : oversampling from 12.8kHz to fs.                  *
 *                                                                   *
 * fs = 16/22/24/28.8/32/44/48 kHz.                                  *
 *-------------------------------------------------------------------*/

//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>

#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"


#define L_FRAME_MAX   (L_FRAME24k)
#define L_FILT_MAX    (L_FILT24k)

#define L_SUBFR16k   80       /* Subframe size at 16kHz           */


void voAMRWBPDecOversamp_12k8(
  Word16 sig12k8[],    /* (i) :  signal to oversampling  */
  Word16 sig_fs[],     /* (o) : oversampled signal      */
  Word16 lg,           /* (i) :  length of output        */
  Word16 mem[],        /* (i/o): memory (2*L_FILT)       */
  Word16 band,         /* (i) :  0/2/3=0..6.4k, 1=6.4..10.8k */
  Word16 add          /* (i) :  1 if result is added to  */
                         /*         output vector, 0 if not */
)
{
  Word16 i, L_frame;
  Word16 signal[(2*L_FILT)+L_FRAME_PLUS];
  const Word16 *filter = NULL;
  Word16 gain, gain1, den_num;
  Word16 fac_up, fac_down;

  const int ncoef = NCOEF_12k8;

  gain = 2;                                                     
  switch (lg) 
  {

    case L_FRAME16kPLUS:
    case L_SUBFR16k:
      fac_up = FAC2_32k; 
      fac_down = FAC1_32k; 
      gain1 = 20480; 
      den_num = 0;  

      if (band == 0) 
      {
        filter = Filter_32k;
      }
      else
      {
        if(lg == L_FRAME16kPLUS) 
        {
          filter = Filter_32k_7k;
        }
        else
        {
          filter = Filter_32k_hf;
        }
      }    
   
    break;

    case L_FRAME24k:
      fac_up = FAC2_48k;        
      fac_down = FAC1_48k;
      gain1 = 30720; 
      den_num = 0;  

      if (band == 0) 
      {
        filter = Filter_48k;
      }
      else
      {
        filter = Filter_48k_hf; 
      }
    break;

    /* this mode is special: it's actually downsampling, thus needs other filter */
    case L_FRAME8k:
      fac_up = FAC2_32k;                                                  
      fac_down = FAC1_32k*2;                 /* note: will be multiplied by 2 again below */ 
      filter = Filter_8k;                                                             
      gain1 = 20480;                                                     
      den_num = 1;    
    break;

    default:
		break;

      //exit(0);
  }      
  if(lg <= L_FRAME24k) 
  {
    fac_down =shl(fac_down,1);
  }
   
  
  
  if(sub(lg,L_SUBFR16k) == 0)
  {
    L_frame = L_SUBFR;
  }
  else
  {
    L_frame = L_FRAME_PLUS;                                
  }

  for (i=0; i<(2*ncoef); i++) 
  {
    signal[i] = mem[i];     
  }

  for (i=0; i<L_frame; i++) 
  {
    signal[i+(2*ncoef)] = mult_r(sig12k8[i],gain1);           
  }
  
  
  
  voAMRWBPDecInterpol(signal+ncoef, sig_fs, lg, filter, ncoef, 
             fac_up, den_num, fac_down, gain, add);                                
  

  for (i=0; i<(2*ncoef); i++) 
  {
    mem[i] = signal[i+L_frame];   
  }
  return;
}



void voAMRWBPDecInterpol(
  Word16 *signal,         /* (i) : signal to oversampling        */
  Word16 *signal_int,     /* (o) : interpolated signal           */
  Word16 L_frame_int,     /* (i) : Length of interpolated signal */
  const Word16 *filter,   /* (i) : Filter                    Q14 */ 
  Word16 nb_coef,         /* (i) : Number of coeff               */
  Word16 fac_up,          /* (i) : Upsampling factor             */
  Word16 den_num,         /* (i) : Denumerator on numerator      */
  Word16 fac_down,        /* (i) : Downsampling factor           */
  Word16 gain,            /* (i) : Gain to apply                 */
  Word16 Add2            /* (i) ; Add result or not             */
)
{
  Word16 i,ii, frac, frac_step, pos;
  Word16 pos_step_plus_one, pos_step, fac_up_minus_frac_step;
  Word16 *x1, *x2, tmp16;
  Word32 L_sum, Ltmp;
  const Word16 *c1, *c2;

#if (!FUNC_INTERPOL_OPT)
  pos_step = den_num;   
  pos_step_plus_one = add(pos_step , 1);

  frac_step = sub(fac_down, extract_l(L_shr(L_mult(pos_step,fac_up),1)));
  fac_up_minus_frac_step = sub(fac_up,frac_step);

  pos = 0;            
  frac = 0;           

  for(i=0; i<L_frame_int; i++) 
  {
    x1 = signal+pos;                 
    x2 = &x1[1];                  
    c1 = &filter[frac];            
    c2 = &filter[sub(fac_up,frac)];    

    L_sum = 0L;                 move32();
    for (ii=0; ii<nb_coef; ii++, c1+=fac_up, c2+=fac_up)
    {
      L_sum = L_mac(L_sum, *x1--, *c1);
      L_sum = L_mac(L_sum, *x2++, *c2);
    }
    L_sum = L_shl(L_sum, add(1,gain));      /* saturation can occur here */
    tmp16 = round16(L_sum);

    
    if(Add2>0)
    {
      Ltmp = L_mult(tmp16, 32767);
      Ltmp = L_mac(Ltmp, signal_int[i], 16384);      /* Hf synt is always in Q_syn +1*/
      signal_int[i] = round16(Ltmp);      
    }
    else
    {
      signal_int[i] = tmp16;                          
    }

    
    if (sub(frac,fac_up_minus_frac_step) > 0) 
    {
      pos = add(pos, pos_step_plus_one);
      frac = sub(frac, fac_up_minus_frac_step);
    }
    else 
    {
      pos = add(pos, pos_step);
      frac = add(frac_step, frac);
    }
  }
  
#else
  Word16 j;
  const Word16 *temp_1 = NULL;
  
  pos_step = den_num;
  pos_step_plus_one = pos_step + 1;

  frac_step = fac_down - pos_step * fac_up;
  fac_up_minus_frac_step = fac_up - frac_step;

  temp_1 = filter + fac_up;
  pos = 0;
  frac = 0;

  for(i=0; i<L_frame_int; i++) 
  {
    x1 = signal+pos;
    x2 = x1 + 1;
    c1 = filter + frac;
    c2 = temp_1 - frac;
    
    L_sum = 0L;
    ii = nb_coef;
    while (ii-- > 0)
    {
        L_sum += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
        c1 += fac_up;
        c2 += fac_up;
    }
    
    L_sum = L_shl(L_sum, (2 + gain));      /* saturation can occur here */
    tmp16 = (L_sum + 0x8000) >> 16;

    if(Add2>0)
    {
      Ltmp = ((Word32)tmp16 << 15) - (Word32)tmp16;
      //Ltmp = L_mac(Ltmp, signal_int[i], 16384);      /* Hf synt is always in Q_syn +1*/
      Ltmp += signal_int[i] << 14;      /* Hf synt is always in Q_syn +1*/
      signal_int[i] = (Ltmp + 0x4000) >> 15;
    }
    else
    {
      signal_int[i] = tmp16;
    }

    j = frac - fac_up_minus_frac_step;
    pos += (j >= 0) ? pos_step_plus_one : pos_step;
    frac = (j >= 0) ? (frac - fac_up_minus_frac_step) : (frac + frac_step);
  }
  
#endif

  return;
}

