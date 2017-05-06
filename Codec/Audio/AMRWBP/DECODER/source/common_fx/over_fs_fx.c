
//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>

#include "amr_plus_fx.h"



#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"


#define L_FRAME_MAX   (L_FRAME48k)

static void mInterpol_mem(
  Word16 *signal,         /* (i)  : Signal    */
  Word16 *signal_int,     /* (o)  : interpolated signal           */
  Word16 L_frame_int,     /* (i)  : Length of interpolated signal */
  const Word16 *filter,         /* (i)  : Filter                    Q14 */ 
  Word16 nb_coef,         /* (i)  : Number of coeff               */
  Word16 fac_up,          /* (i)  : Upsampling factor             */
  Word16 fac_down,        /* (i)  : Denumerator on numerator      */
  Word16 gain,            /* (i)  : Gain to apply                 */  
  Word16 *mem_frac        /* (i/o): Fraction memory              */
);

Word16 Decim_fs(       /* number of sample decimated       */
  Word16 sig_in[],     /* input:  signal to decimate         */
  Word16 lg,           /* input:  length of input            */
  Word16 sig_out[],    /* output: signal decimated           */
  Word16 fac_up,         /* input:  44k/48k *fac_up/12 = fs    */
  Word16 mem[],        /* in/out: mem[2*L_FILT_DECIM_FS]     */
  Word16 *frac_mem     /* in/out: interpol fraction memory   */
)
{
  Word16 i, ncoef, L_frame, L_frame_int;
  Word16 signal[(2*L_FILT_DECIM_FS)+2*L_FRAME_MAX];
  Word16 *filter, gain;
  Word16 fac_down, tmp16, exp_u, m_fu;
  Word32 Ltmp;
  

  
  if(sub(fac_up,12)>=0) return(lg);

  fac_down = 12;      
  exp_u = norm_s(fac_up);
  m_fu = shl(fac_up, exp_u);


  /* ncoef = L_FILT_OVER_FS*fac_down/fac_up;*/
  ncoef = div_s(((12*12)<<6), m_fu);    
  ncoef = shr(ncoef, sub(15+6, exp_u));

  /* gain = ((float)fac_up) / ((float)fac_down);*/
  gain = div_s(m_fu, 24576);   /* gain in Q15 */
  gain = shl(gain, sub(11,exp_u));

  filter = (Word16*)Filter_LP12;

  L_frame = lg;
  /*L_frame_int = ((lg*fac_up)-(*frac_mem)+(fac_down-1))/fac_down;*/
  Ltmp = L_mult(L_frame, fac_up);
  Ltmp = L_mac(Ltmp, sub(fac_down, add(1, *frac_mem)), 1);

  i = 11;           /*norm_s(fac_down = 12); */

  L_frame_int = div_s(extract_h(L_shl(Ltmp, i)), 24576/*shl(fac_down, i)*/);

  tmp16 = add(L_frame_int, 1);
  Ltmp = L_msu(Ltmp, tmp16, fac_down);
  
  if (Ltmp >= 0)
  {
    L_frame_int = tmp16;            
  } 

  /* load buffer & update memory */

  for (i=0; i<(2*L_FILT_DECIM_FS); i++) 
  {
    signal[i] = mem[i];         
  }

  for (i=0; i<L_frame; i++) 
  {
    signal[i+(2*L_FILT_DECIM_FS)] = sig_in[i];      
  }

  for (i=0; i<(2*L_FILT_DECIM_FS); i++) 
  {
    mem[i] = signal[i+L_frame];         
  }

  /* decimation from 44.1/48khz to fs_output */

  mInterpol_mem(signal+ncoef-1, sig_out, L_frame_int, filter, ncoef, 
                  fac_up, fac_down, gain, frac_mem);


  return(L_frame_int);
}

Word16 Over_fs(        /* number of sample oversampled       */
  Word16 sig_in[],     /* input:  signal to oversample       */
  Word16 sig_out[],    /* output: signal oversampled         */
  Word16 lg,           /* input:  length of output           */
  Word16 fac_down,     /* input:  fs*12/fac_down = 44k/48k   */
  Word16 mem[],        /* in/out: mem[2*L_FILT_OVER_FS]      */
  Word16 *frac_mem     /* in/out: interpol fraction memory   */
)
{
  Word16  i, ncoef, L_frame, L_frame_int;
  Word16  signal[(2*L_FILT_OVER_FS)+2*L_FRAME_MAX];
  Word16 *filter, gain;
  Word16 fac_up, tmp16;
  Word32 Ltmp;

  
  if(sub(fac_down,12)>= 0) return (lg);
  
  fac_up = 12;      

  filter = (Word16*)Filter_LP12;        

  ncoef = L_FILT_OVER_FS;             
  gain = 32767;      

  L_frame_int = lg;     

  /*L_frame = ((lg*fac_down)+(*frac_mem)) / fac_up;*/
  Ltmp = L_mult(L_frame_int, fac_down);
  Ltmp = L_mac(Ltmp, *frac_mem, 1);

  L_frame = div_s(extract_h(L_shl(Ltmp, 11)), 24576);

  tmp16 = add(L_frame, 1);
  Ltmp = L_msu(Ltmp, tmp16, fac_up);
  
  if (Ltmp >= 0)
  {
    L_frame = tmp16;            
  } 

  /* load buffer & update memory */

  for (i=0; i<(2*L_FILT_OVER_FS); i++)
  {
    signal[i] = mem[i];         
  }
  for (i=0; i<L_frame; i++) 
  {
    signal[i+(2*L_FILT_OVER_FS)] = sig_in[i];           
  }

  for (i=0; i<(2*L_FILT_OVER_FS); i++) 
  {
    mem[i] = signal[i+L_frame];                         
  }

  /* oversample to 44.1/48 khz */

  mInterpol_mem(signal+ncoef-1, sig_out, L_frame_int, filter, ncoef, 
                  fac_up, fac_down, gain, frac_mem);

  return(L_frame);
}

static void mInterpol_mem(
  Word16 *signal,         /* (i)  : Signal    */
  Word16 *signal_int,     /* (o)  : interpolated signal           */
  Word16 L_frame_int,     /* (i)  : Length of interpolated signal */
  const Word16 *filter,         /* (i)  : Filter                    Q14 */ 
  Word16 nb_coef,         /* (i)  : Number of coeff               */
  Word16 fac_up,          /* (i)  : Upsampling factor             */
  Word16 fac_down,        /* (i)  : Denumerator on numerator      */
  Word16 gain,            /* (i)  : Gain to apply                 */  
  Word16 *mem_frac        /* (i/o): Fraction memory              */
)
{
  Word16 i, j, frac, frac_step, pos, pos_step;
  Word16 pos_step_plus_one, fac_up_minus_frac_step;
  Word16 *x1, *x2;
  const Word16 *c1, *c2;
  Word32 s;

#if (!FUNC_MINTERPOL_MEM_OPT)
  /* pos_step = fac_down / fac_up; */
  pos_step = 0;        
  i = fac_down;        
  
  while (sub(i, fac_up) >= 0)
  {
    
    i = sub(i, fac_up);
    pos_step = add(pos_step, 1);
  }

  pos_step_plus_one = add(pos_step, 1);

  frac_step = sub(fac_down, extract_l(L_shr(L_mult(pos_step,fac_up),1)));
  fac_up_minus_frac_step = sub(fac_up,frac_step);

  pos = 0;             
  frac = *mem_frac;    
  for(i=0; i<L_frame_int; i++) 
  {
    x1 = signal+pos;    
    x2 = x1+1;          
    c1 = &filter[frac]; 
    c2 = &filter[fac_up-frac];  
    s = 0;    move32();
    for(j=0; j<nb_coef; j++, c1+=fac_up, c2+=fac_up) 
    {
      /*s += (*x1--) * (*c1) + (*x2++) * (*c2);*/
      s = L_mac(s, *x1-- , *c1);
      s = L_mac(s, *x2++ , *c2);
    }
    /* in decim and imterpol split, gain is always <= 1*/
    signal_int[i] = shl(mult_r(round16(s),gain),1);     /* Apply gain before left shift to prevent overflow when used in decim_split*/

      
    if (sub(frac,fac_up_minus_frac_step) >= 0) 
    {
      pos = add(pos, pos_step_plus_one);
      frac = sub(frac, fac_up_minus_frac_step);
    }
    else 
    {
      pos = add(pos, pos_step);
      frac = add(frac, frac_step);
    }

  }

  *mem_frac = frac;   
  
#else

    const Word16 *temp1 = NULL;
    
  /* pos_step = fac_down / fac_up; */
//  pos_step = 0;
//  i = fac_down;
//  j = i - fac_up;
//  while (j >= 0)
//  {
//    pos_step++;
//    j -= fac_up;
//  }
  pos_step = fac_down / fac_up;

  pos_step_plus_one = pos_step + 1;

  frac_step = fac_down - pos_step * fac_up;
  fac_up_minus_frac_step = fac_up - frac_step;
  
  temp1 = filter + fac_up;
  
  pos = 0;
  frac = *mem_frac;
  for(i=0; i<L_frame_int; i++) 
  {
    x1 = signal+pos;
    x2 = x1+1;
    c1 = filter + frac;
    c2 = temp1 - frac;
    s = 0;
    
    j = nb_coef;
    while (j-- > 0)
    {
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up;
      c2 += fac_up;
    }
    
    /* in decim and imterpol split, gain is always <= 1*/
    signal_int[i] = ((((s + 0x00004000) >> 15) * gain + 0x00004000) >> 15) << 1;
    
    j = frac - fac_up_minus_frac_step;
    pos += (j >= 0) ? pos_step_plus_one : pos_step;
    frac = (j >= 0) ? (frac - fac_up_minus_frac_step) : (frac + frac_step);
  }

  *mem_frac = frac;   

#endif

  return;
}


