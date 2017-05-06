
//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>

#include "amr_plus_fx.h"



#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "count.h"

#define L_FRAME_MAX   (L_FRAME48k)

#if (FUNC_INTERPOL_MEM_FOR12K8)
void Interpol_mem_for12k8(
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
#endif

Word16 Join_over_12k8(/* (o)   : number of sample oversampled       */
  Word16 sig12k8_lf[],/* (i)   : LF signal (fs=12k8)        */
  Word16 sig12k8_hf[],/* (i)   : HF signal (fs=12k8)        */
  Word16 lg,          /* (i)   : length of LF and HF        */
  Word16 sig_fs[],    /* (o)   : oversampled signal         */
  Word16 lg_output,   /* (i)   : L_FRAME44k if 44kHz        */
  Word16 fac_fs,      /* (i)   : >= 32 (32 = base fs)       */
  Word16 mem[],       /* (i/o) : mem[L_MEM_JOIN_OVER]       */
  Word16 *frac_mem    /* (i/o) : interpol fraction memory   */
)
{
#if (FUNC_JOIN_OVER_12K8_OPT)
  Word16 i, j, ncoef, L_frame, L_frame_int;
  Word16 signal[(2*L_FILT_JOIN)+L_FRAME_PLUS];
  Word16 signal2[(2*L_FILT_OVER)+(2*L_FRAME_PLUS)], *sig;
  Word16 gain, *x1, *x2;
  Word32 s, Ltmp;
  Word16 fac_up, fac_down = 0, tmp16;
  const Word16 *filter;

  Word16 *addr_for_opt_1 = NULL;
  const Word32 var_for_opt_1 = L_FILT_JOIN << 1;
  const Word32 var_for_opt_2 = L_FILT_OVER << 1;
  Word32 var_for_opt_3;

#ifdef FILTER_44kHz
  if (lg_output == L_FRAME44k)
  {
    fac_up = 1323;
    fac_down = fac_fs << 3;
    filter = (Word16*)Filter_LP165;
  }
#endif
#ifdef FILTER_48kHz
  if (lg_output == L_FRAME48k)
  {
    fac_up = 1440;
    fac_down = fac_fs << 3;
    filter = (Word16 *)Filter_LP180;
  }
#endif

  ncoef = L_FILT_OVER;
  gain = 32767;

  /* frames length */

  L_frame = lg << 1;

  /* L_frame = ((L_frame_int*fac_down)+frac) / fac_up */
  /* L_frame_int = ((L_frame*fac_up)-(*frac_mem)+(fac_down-1))/fac_down; */

  Ltmp = ((L_frame * fac_up) << 1) + ((fac_down - *frac_mem - 1) << 1);

  i = norm_s(fac_down);

  //L_frame_int = div_s(extract_h(L_shl(Ltmp, i)), shl(fac_down, i));
  L_frame_int = div_s((((Word32)L_shl(Ltmp, i)) >> 16), shl(fac_down, i));

  tmp16 = L_frame_int + 1;
  Ltmp -= (tmp16 * fac_down) << 1;
  if (Ltmp >= 0)
  {
    L_frame_int = tmp16;
  }

  /* band join (2 x 12.8kHz --> 25.6kHz) */

  /* load buffer (LF-HF) & update memory */
  for (i = 0; i < var_for_opt_1; i++)
  {
  	*(signal + i) = *(mem + var_for_opt_2 + i);
  }
//  memcpy(signal, (mem + var_for_opt_2), var_for_opt_1 * sizeof(Word16));
  
  for (i=0; i<lg; i++)
  {
    Ltmp = ((sig12k8_lf[i] << 15) - sig12k8_lf[i]) - (sig12k8_hf[i] << 14);
    /*
    //signal[i+(2*L_FILT_JOIN)] = sub(sig12k8_lf[i], sig12k8_hf[i]);    
    Ltmp = ((sig12k8_lf[i] << 15) - sig12k8_lf[i]) << 1;    //Ltmp = (sig12k8_lf[i] * 32767) << 1;
    Ltmp -= sig12k8_hf[i] << 15;                            //Ltmp -= (sig12k8_hf[i] * 16384) << 1;
    */
    signal[i+var_for_opt_1]  = (Ltmp + 0x00004000) >> 15;
  }
  for (i = 0; i < var_for_opt_1; i++)
  {
  	*(mem + var_for_opt_2 + i) = *(signal + lg + i);
  }
  //memcpy((mem + var_for_opt_2), (signal + lg), var_for_opt_1 * sizeof(Word16));

  /* odd samples = LF-HF interpolated */
  sig = signal2 + var_for_opt_2;
  addr_for_opt_1 = signal + L_FILT_JOIN;
  for(i=0; i<lg; i++) 
  {
    x1 = addr_for_opt_1 + i;
    x2 = x1 + 1;
    s = 0;
    j = 0;
    while (j < L_FILT_JOIN)
    {
        var_for_opt_3 = Inter2_coef[j++];
        s += ((*x1--) * var_for_opt_3) + ((*x2++) * var_for_opt_3);
    }
    sig[(i << 1) + 1] = ((s << 2) + 0x00008000) >> 16;
  }

  /* synchronise LF+HF with interpolated LF-HF */
  for (i = 0; i < var_for_opt_1; i++)
  {
  	*(signal + i) = *(mem + var_for_opt_1 + var_for_opt_2 + i);
  }
//  memcpy(signal, (mem + var_for_opt_1 + var_for_opt_2), var_for_opt_1 * sizeof(Word16));
  for (i=0; i<lg; i++) 
  {
    /*
    //signal[i+(2*L_FILT_JOIN)] = add(sig12k8_lf[i], sig12k8_hf[i]);  
    Ltmp = ((sig12k8_lf[i] << 15) - sig12k8_lf[i]) << 1;    //(sig12k8_lf[i] * 32767) << 1;
    Ltmp += sig12k8_hf[i] << 15;            // Hy synth is always Q_syn +1
    */
    Ltmp = ((sig12k8_lf[i] << 15) - sig12k8_lf[i]) + (sig12k8_hf[i] << 14);
    signal[i+(L_FILT_JOIN << 1)] = (Ltmp + 0x00004000) >> 15;
  }
  for (i = 0; i < var_for_opt_1; i++)
  {
  	*(mem + var_for_opt_1 + var_for_opt_2 + i) = *(signal + lg + i);
  }
//  memcpy((mem + var_for_opt_1 + var_for_opt_2), (signal + lg), var_for_opt_1 * sizeof(Word16));  

  /* even samples = LF+HF */
  for (i=0; i<lg; i++)
  {
    sig[i<<1] = signal[i+L_FILT_JOIN];
  }

  /* load buffer & update memory (for oversampling) */
//  memcpy(signal2, mem, var_for_opt_2 * sizeof(Word16));
//  memcpy(mem, (signal2 + L_frame), var_for_opt_2 * sizeof(Word16));
  for (i = 0; i < var_for_opt_2; i++)
  {
    *(signal2 + i) = *(mem + i);
  }
  for (i = 0; i < var_for_opt_2; i++) 
  {
    *(mem + i) = *(signal2 + L_frame + i);
  }

  /* oversample from 25.6kHz to fs */
  Interpol_mem_for12k8(signal2+ncoef, sig_fs, L_frame_int, filter, ncoef, 
  fac_up, fac_down, gain, frac_mem);

  return(L_frame_int);
  
#else

  Word16 i, j, ncoef, L_frame, L_frame_int;
  Word16 signal[(2*L_FILT_JOIN)+L_FRAME_PLUS];
  Word16 signal2[(2*L_FILT_OVER)+(2*L_FRAME_PLUS)], *sig;
  Word16 gain, *x1, *x2;
  Word32 s, Ltmp;
  Word16 fac_up, fac_down, tmp16;
  const Word16 *filter;

#ifdef FILTER_44kHz
  
  if (L_sub(lg_output,L_FRAME44k) == 0)
  {
    fac_up = 3*441;             
    fac_down = shl(fac_fs,3);        
    filter = (Word16*)Filter_LP165;        
  }
#endif
#ifdef FILTER_48kHz
  
  if (L_sub(lg_output,(L_FRAME48k)) == 0)
  {
    fac_up = 180*8;        
    fac_down = shl(fac_fs,3);        
    filter = (Word16 *)Filter_LP180;        
  }
#endif

  ncoef = L_FILT_OVER;                    
  gain = 32767;                              

  /* frames length */

  L_frame = shl(lg,1);     /* 25k6 rate */     

  /* L_frame = ((L_frame_int*fac_down)+frac) / fac_up */
  /* L_frame_int = ((L_frame*fac_up)-(*frac_mem)+(fac_down-1))/fac_down; */

  Ltmp = L_mult(L_frame, fac_up);
  Ltmp = L_mac(Ltmp, sub(fac_down, add(1, *frac_mem)), 1);

  i = norm_s(fac_down);

  L_frame_int = div_s(extract_h(L_shl(Ltmp, i)), shl(fac_down, i));

  tmp16 = add(L_frame_int, 1);
  Ltmp = L_msu(Ltmp, tmp16, fac_down);
  
  if (Ltmp >= 0)
  {
    L_frame_int = tmp16;            
  } 

  /* band join (2 x 12.8kHz --> 25.6kHz) */

  /* load buffer (LF-HF) & update memory */
  for (i=0; i<(2*L_FILT_JOIN); i++) 
  {
    signal[i] = mem[i+(2*L_FILT_OVER)];   
  }
  for (i=0; i<lg; i++) 
  {
    /*signal[i+(2*L_FILT_JOIN)] = sub(sig12k8_lf[i], sig12k8_hf[i]);    */
    Ltmp = L_mult(sig12k8_lf[i], 32767);
    Ltmp = L_msu(Ltmp, sig12k8_hf[i], 16384);     /* Hy synth is always Q_syn +1*/
    signal[i+(2*L_FILT_JOIN)]  = round16(Ltmp);   
  }
  for (i=0; i<(2*L_FILT_JOIN); i++) 
  {
    mem[i+(2*L_FILT_OVER)] = signal[i+lg];    
  }

  /* odd samples = LF-HF interpolated */
  sig = signal2 + (2*L_FILT_OVER);    
  for(i=0; i<lg; i++) 
  {
    x1 = &signal[i+L_FILT_JOIN];    
    x2 = &signal[i+1+L_FILT_JOIN];    
    s = 0;    move32();
    for(j=0; j<L_FILT_JOIN; j++, x1--, x2++) 
    {
      /*s += (*x1 + *x2) * inter2_coef[j];*/
      s = L_mac(s, *x1, Inter2_coef[j]);
      s = L_mac(s, *x2, Inter2_coef[j]);
    }
    sig[(i*2)+1] = round16(L_shl(s,1));
  }

  /* synchronise LF+HF with interpolated LF-HF */
  for (i=0; i<(2*L_FILT_JOIN); i++) 
  {
    signal[i] = mem[i+(2*L_FILT_OVER)+(2*L_FILT_JOIN)];   
  }
  for (i=0; i<lg; i++) 
  {
    /*signal[i+(2*L_FILT_JOIN)] = add(sig12k8_lf[i], sig12k8_hf[i]);  */
    Ltmp = L_mult(sig12k8_lf[i], 32767);
    Ltmp = L_mac(Ltmp, sig12k8_hf[i], 16384);            /* Hy synth is always Q_syn +1*/
    signal[i+(2*L_FILT_JOIN)] = round16(Ltmp); 

  }
  for (i=0; i<(2*L_FILT_JOIN); i++) 
  {
    mem[i+(2*L_FILT_OVER)+(2*L_FILT_JOIN)] = signal[i+lg];    
  }

  /* even samples = LF+HF */
  for (i=0; i<lg; i++)
  {
    sig[i*2] = signal[i+L_FILT_JOIN];   
  }

  /* load buffer & update memory (for oversampling) */

  for (i=0; i<(2*L_FILT_OVER); i++) 
  {
    signal2[i] = mem[i];            
  }
  for (i=0; i<(2*L_FILT_OVER); i++) 
  {
    mem[i] = signal2[i+L_frame];    
  }

  /* oversample from 25.6kHz to fs */
  Interpol_mem(signal2+ncoef, sig_fs, L_frame_int, filter, ncoef, 
  fac_up, fac_down, gain, frac_mem);
  
  return(L_frame_int);
#endif
}



// @shanrong modified
#if (!FUNC_INTERPOL_MEM_ASM)
void Interpol_mem(
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
  Word16 frac_d, fac_up_d;
  Word16 *x1, *x2;
  const Word16 *c1, *c2;
  Word32 s;


#if (!FUNC_INTERPOL_MEM_OPT)
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

  fac_up_d = shr(fac_up, 3);

  pos = 0;             
  frac = *mem_frac;    
  for(i=0; i<L_frame_int; i++) 
  {
    x1 = signal+pos;    
    x2 = x1+1;          
    frac_d = shr(frac, 3);
    c1 = &filter[frac_d];           
    c2 = &filter[fac_up_d-frac_d];  
    s = 0;    move32();

    for(j=0; j<nb_coef; j++, c1+=fac_up_d, c2+=fac_up_d) 
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

//  pos_step = 0;
//  i = fac_down;
//  j = i - fac_up;
//  while (j > 0)
//  {
//    pos_step++;
//    j -= fac_up;
//  }
  //pos_step = (fac_down + 1) / fac_up;
  pos_step = 0;
  
  pos_step_plus_one = pos_step + 1;

  frac_step = fac_down - pos_step * fac_up;
  fac_up_minus_frac_step = fac_up - frac_step;
  fac_up_d = fac_up >> 3;
  temp1 = filter + fac_up_d;

  pos = 0;
  frac = *mem_frac;
  for(i=0; i<L_frame_int; i++) 
  {
    x1 = signal+pos;
    x2 = x1+1;
    frac_d = frac >> 3;
    c1 = filter + frac_d;
    c2 = temp1 - frac_d;
    s = 0;

    j = nb_coef;
    while (j-- > 0)
    {
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
    }
    
    //in decim and imterpol split, gain is always <= 1
    signal_int[i] = ((((s + 0x00004000) >> 15) * gain + 0x00004000) >> 15) << 1;

    j = frac - fac_up_minus_frac_step;
    pos += (j >= 0) ? pos_step_plus_one : pos_step;
    frac = (j >= 0) ? j : (frac + frac_step);
  }

  *mem_frac = frac;
#endif

  return;
}


#if (FUNC_INTERPOL_MEM_FOR12K8)
void Interpol_mem_for12k8(
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
  Word16 frac_d, fac_up_d;
  Word16 *x1, *x2;
  const Word16 *c1, *c2;
  Word32 s;


#if (!FUNC_INTERPOL_MEM_OPT)
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

  fac_up_d = shr(fac_up, 3);

  pos = 0;             
  frac = *mem_frac;    
  for(i=0; i<L_frame_int; i++) 
  {
    x1 = signal+pos;    
    x2 = x1+1;          
    frac_d = shr(frac, 3);
    c1 = &filter[frac_d];           
    c2 = &filter[fac_up_d-frac_d];  
    s = 0;    move32();

    for(j=0; j<nb_coef; j++, c1+=fac_up_d, c2+=fac_up_d) 
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
  fac_up_d = fac_up >> 3;
  temp1 = filter + fac_up_d;

  pos = 0;
  frac = *mem_frac;
  for(i=0; i<L_frame_int; i++) 
  {
    x1 = signal+pos;
    x2 = x1+1;
    frac_d = frac >> 3;
    c1 = filter + frac_d;
    c2 = temp1 - frac_d;
    s = 0;

    {
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
      c1 += fac_up_d;
      c2 += fac_up_d;
      s += ((*x1--) * (*c1)) +  ((*x2++) * (*c2));
//      c1 += fac_up_d;
//      c2 += fac_up_d;
//      s += ((*x1) * (*c1)) +  ((*x2) * (*c2));
    }
    
    //in decim and imterpol split, gain is always <= 1
    signal_int[i] = ((((s + 0x00004000) >> 15) * gain + 0x00004000) >> 15) << 1;

    j = frac - fac_up_minus_frac_step;
    pos += (j >= 0) ? pos_step_plus_one : pos_step;
    frac = (j >= 0) ? j : (frac + frac_step);
  }

  *mem_frac = frac;
#endif

  return;
}
#endif
#endif
// end

