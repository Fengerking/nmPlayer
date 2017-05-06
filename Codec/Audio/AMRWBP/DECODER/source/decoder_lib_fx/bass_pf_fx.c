
#include "typedef.h"
#include "count.h"
#include "basic_op.h"
#include "math_op.h"
#include "oper_32b.h"

#include "amr_plus_fx.h"


#define L_EXTRA    96

#if (LARGE_VAR_OPT)  
static Word16 syn_buf[PIT_MAX_MAX+(2*L_SUBFR)+L_FRAME_PLUS];
static Word16 noise_buf[(2*L_FILT)+L_SUBFR];
#endif

static Word16 Short_pitch_tracker(Word16 syn[],Word16 T);                 


void Init_bass_postfilter(Decoder_State_Plus_fx *st)
{
  Set_zero(st->wold_synth_pf, PIT_MAX_MAX+(2*L_SUBFR));
  Set_zero(st->wold_noise_pf, 2*L_FILT);

  st->wold_gain_pf[0] = 0;      
  st->wold_T_pf[0] = 64;        
  st->wold_gain_pf[1] = 0;      
  st->wold_T_pf[1] = 64;        
  
  return;
}


void Bass_postfilter(
  Word16 *synth_in,           /* (i) : 12.8kHz synthesis to postfilter                */
  Word16 *T_sf,               /* (i) : Pitch period for all subframe (T_sf[16])       */
  Word16 *gainT_sf,           /* (i) : Pitch gain for all subframe (gainT_sf[16]) Q14 */ 
  Word16 *synth_out,          /* (o) : filtered synthesis (with delay=L_SUBFR+L_FILT) */
                              /*       delay = (2*L_SUBFR)+L_FILT                     */
  Word16 pit_adj,             
  Decoder_State_Plus_fx *st   /* i/o): decoder memory state                           */
)
{
#if (FUNC_BASS_POSTFILTER_OPT)

  Word16 i, j, i_subfr, T, lg, gain, e_tmp, e_ener, m_tmp, m_ener, tmp16, tmp2;
  Word32 Ltmp, Lener;
  Word16 *syn;
  Word16 *noise, max, scale, length;

#if (!LARGE_VAR_OPT)  
  Word16 syn_buf[PIT_MAX_MAX+(2*L_SUBFR)+L_FRAME_PLUS];  
  Word16 noise_buf[(2*L_FILT)+L_SUBFR];
#endif

  syn = syn_buf + PIT_MAX_MAX;
  noise = noise_buf + L_FILT;

  T = st->wold_T_pf[0];
  gain = st->wold_gain_pf[0];

  if (pit_adj == 0) 
  {
  	for (i = 0; i < PIT_MAX_MAX+L_SUBFR; i++)
  	{
  		*(syn_buf + i) = *(st->wold_synth_pf + i);
  	}
  	for (i = 0; i < L_FRAME_PLUS; i++)
  	{
  		*(syn_buf+PIT_MAX_MAX+L_SUBFR + i) = *(synth_in + i);
  	}
  	for (i = 0; i < PIT_MAX_MAX+L_SUBFR; i++)
  	{
  		*(st->wold_synth_pf + i) = *(syn_buf+L_FRAME_PLUS + i);
  	}
  	for (i = 0; i < L_FRAME_PLUS; i++)
  	{
  		*(synth_out + i) = *(syn - L_FILT + i);
  	}
  	length = PIT_MAX_MAX + L_SUBFR + L_FRAME_PLUS;
  }
  else
  {
  	for (i = 0; i < PIT_MAX_MAX+(L_SUBFR << 1); i++)
  	{
  		*(syn_buf + i) = *(st->wold_synth_pf + i);
  	}
  	for (i = 0; i < L_FRAME_PLUS; i++)
  	{
  		*(syn_buf+PIT_MAX_MAX+ (L_SUBFR << 1) + i) = *(synth_in + i);
  	}
  	for (i = 0; i < PIT_MAX_MAX + (L_SUBFR << 1); i++)
  	{
  		*(st->wold_synth_pf + i) = *(syn_buf+L_FRAME_PLUS + i);
  	}
  	for (i = 0; i < L_FRAME_PLUS; i++)
  	{
  		*(synth_out + i) = *(syn - L_FILT + i);
  	}
  	length = PIT_MAX_MAX + (L_SUBFR << 1) + L_FRAME_PLUS;
  }
  max = 0;
  for(i = 0;i < length; i++)
  {
    if(abs_s(syn_buf[i]) > max)
    {
      max = abs_s(syn_buf[i]);
    }
  }
  scale = norm_s(max) - 3;  /* 12 bit scaling */
  voAMRWBPDecScale_sig(syn_buf, length, scale);

  for (i_subfr=0; i_subfr<L_FRAME_PLUS; i_subfr+=L_SUBFR)
  {
    if (pit_adj != 0) 
    {
      i = (i_subfr >> 6) - 2;
      if (i < 0)
      {
        T = st->wold_T_pf[i+2];
        gain = st->wold_gain_pf[i+2];
      }
      else 
      {
        T = T_sf[i];
        gain = gainT_sf[i];
      }
    }
   
    if (gain > 16384)
    {
      gain = 16384;
    }
    if (gain < 0) 
    {
      gain = 0;
    }
    T = Short_pitch_tracker(&syn[i_subfr], T);
    if (pit_adj == 0)
    {
      lg = L_SUBFR + L_FRAME_PLUS - T - i_subfr;
    }
    else
    {
      lg = 2*L_SUBFR+L_FRAME_PLUS - T - i_subfr;
    }

    if (lg < 0)
    {
      lg = 0;
    }
    if (lg > L_SUBFR)
    {
      lg = L_SUBFR;
    }

    /* limit gain to avoid problem on burst */
    if (lg > 0)
    {
      Ltmp = 0;
      for (i=0; i<lg; i++) 
      {
        Ltmp += syn[i+i_subfr] *  syn[i+i_subfr];
      }
      Ltmp <<= 1;
      Lener = 0;
      for (i=0; i<lg; i++) 
      {
        Lener += syn[i+i_subfr+T] * syn[i+i_subfr+T];

      }
      Lener <<= 1;
      if (Ltmp == 0)
        Ltmp  = 1;

      /*tmp = (float)sqrt(tmp / ener);     */
      e_tmp   = norm_l(Ltmp);
      e_ener  = norm_l(Lener);
      m_tmp   = (Ltmp << e_tmp) >> 16;
      m_ener  = (Lener << e_ener) >> 16;
      
      if (m_ener > m_tmp)
      {
        m_ener >>= 1;
        e_ener -= 1;
      }
      
      m_ener = div_s(m_ener, m_tmp);
      e_ener -= e_tmp;
      /* Q15 -> Q31*/
      Ltmp = L_deposit_h(m_ener);
      tmp16 = e_ener + 16;
      voAMRWBPDecIsqrt_n(&Ltmp, &tmp16);
      /* Output in Q14  (2 bits reserve to gain)*/  
      /* if tmp > gain we don't use */
      /* it and gain <= 1 in Q14 ->(16384)*/
      tmp16 = (Word16)(L_shl(Ltmp, (tmp16 + e_ener +7)) >> 16);
      
      if (tmp16 < gain)
      {
        gain = tmp16;
      }
    }
    /* calculate noise based on voiced pitch */
    tmp16 = gain;			/*don't need to right shift in fixed-point because gain in Q14*/
    tmp2 = tmp16 >> 1;
    for (i=0; i<lg; i++)
    {
      /*noise_buf[i+(2*L_FILT)] =
        tmp * (syn[i+i_subfr] - 0.5f*syn[i+i_subfr-T] - 0.5f*syn[i+i_subfr+T]);	 */
      Ltmp = (syn[i+i_subfr] * tmp16) - (syn[i+i_subfr-T] * tmp2) - (syn[i+i_subfr+T] * tmp2);
      noise_buf[i+(L_FILT << 1)] = (Ltmp + 0x00004000L) >> 15;
    }
    for (i=lg; i<L_SUBFR; i++) 
    {
      /*noise_buf[i+(2*L_FILT)] = tmp * (syn[i+i_subfr] - syn[i+i_subfr-T]);	 */
      Ltmp = tmp16 * (syn[i+i_subfr] - syn[i+i_subfr-T]);
      noise_buf[i+(L_FILT << 1)] = (Ltmp + 0x00004000) >> 15;
    }

	for (i = 0; i < (L_FILT << 1); i++)
	{
		*(noise_buf + i) = *(st->wold_noise_pf + i);
	}
    voAMRWBPDecScale_sig(noise_buf, 2*L_FILT, sub(scale, st->Old_bpf_scale));
	for (i = 0; i < (L_FILT << 1); i++)
	{
		*(st->wold_noise_pf + i) = *(noise_buf + L_SUBFR + i);
	}

    /* substract from voiced speech low-pass filtered noise */
    for (i=0; i<L_SUBFR; i++)
    {
      Ltmp = Filt_lp[0] * noise[i];

      for(j=1; j<=L_FILT; j++)
      {
        Ltmp  += Filt_lp[j] * (noise[i-j] + noise[i+j]);
      }

      Ltmp = (scale >= 0) ? (Ltmp >> scale) : (Ltmp << (-scale));
      synth_out[i+i_subfr] = synth_out[i+i_subfr] - ((Ltmp + 0x00004000L) >> 15);
    }

    if(pit_adj == 0) 
    {
      i = i_subfr >> 6; 
      T = T_sf[i];
      gain = gainT_sf[i];
    }
    /* update last scaling of noise buffer */
    st->Old_bpf_scale = scale;
  }
  
  if (pit_adj == 0)
  {
    st->wold_T_pf[0] = T;
    st->wold_gain_pf[0] = gain;
  }
  else 
  {
    st->wold_T_pf[0]    = T_sf[NB_SUBFR-2];
    st->wold_gain_pf[0] = gainT_sf[NB_SUBFR-2];
    st->wold_T_pf[1]    = T_sf[NB_SUBFR-1];
    st->wold_gain_pf[1] = gainT_sf[NB_SUBFR-1];
  }
  return;

#else

  Word16 i, j, i_subfr, T, lg, gain, e_tmp, e_ener, m_tmp, m_ener, tmp16, tmp2;
  Word32 Ltmp, Lener;
  Word16 syn_buf[PIT_MAX_MAX+(2*L_SUBFR)+L_FRAME_PLUS], *syn;
  
  Word16 noise_buf[(2*L_FILT)+L_SUBFR], *noise, max, scale, length;

  syn = syn_buf + PIT_MAX_MAX;							
  noise = noise_buf + L_FILT;                           

  T = st->wold_T_pf[0];        
  gain = st->wold_gain_pf[0];  

  
  if (pit_adj == 0) 
  {
    voAMRWBPDecCopy(st->wold_synth_pf, syn_buf, PIT_MAX_MAX+L_SUBFR);				
    voAMRWBPDecCopy(synth_in, syn_buf+PIT_MAX_MAX+L_SUBFR, L_FRAME_PLUS);				
    voAMRWBPDecCopy(syn_buf+L_FRAME_PLUS, st->wold_synth_pf, PIT_MAX_MAX+L_SUBFR);		

    voAMRWBPDecCopy(&syn[-L_FILT], synth_out, L_FRAME_PLUS);					
    length = PIT_MAX_MAX + L_SUBFR + L_FRAME_PLUS;    
  }
  else
  {
    voAMRWBPDecCopy(st->wold_synth_pf, syn_buf, PIT_MAX_MAX+(2*L_SUBFR));			
    voAMRWBPDecCopy(synth_in, syn_buf+PIT_MAX_MAX+(2*L_SUBFR), L_FRAME_PLUS);			
    voAMRWBPDecCopy(syn_buf+L_FRAME_PLUS, st->wold_synth_pf, PIT_MAX_MAX+(2*L_SUBFR));		

    voAMRWBPDecCopy(&syn[-L_FILT], synth_out, L_FRAME_PLUS);					
    length = PIT_MAX_MAX + (2*L_SUBFR) + L_FRAME_PLUS;    
  }
  max = 0; 
  for(i = 0;i < length; i++)
  {
    
    if(sub(abs_s(syn_buf[i]),max)>0)
    {
      max = abs_s(syn_buf[i]);
    }
  }
  scale = sub(norm_s(max),3);  /* 12 bit scaling */
  voAMRWBPDecScale_sig(syn_buf, length, scale);

  for (i_subfr=0; i_subfr<L_FRAME_PLUS; i_subfr+=L_SUBFR)
  {
    
    if (pit_adj != 0) 
    {
      i = sub(shr(i_subfr,6), 2);
      
      if (i < 0)
      {
        T = st->wold_T_pf[i+2];       
        gain = st->wold_gain_pf[i+2]; 
      }
      else 
      {
        T = T_sf[i];            
        gain = gainT_sf[i];     
      }
    }
   
    
    if (sub(gain,16384) > 0) 
    {
      gain = 16384; 
    }
    
    if (gain < 0) 
    {
      gain = 0;      
    }
    T = Short_pitch_tracker(&syn[i_subfr], T);
    
    if (pit_adj == 0)
    {
      lg = sub(L_SUBFR+L_FRAME_PLUS, add(T,i_subfr)); 
    }
    else
    {
      lg = sub(2*L_SUBFR+L_FRAME_PLUS, add(T,i_subfr)); 
    }

    
    if (lg < 0)
    {
      lg = 0;  
    }
    
    if (sub(lg, L_SUBFR)> 0)
    {
      lg = L_SUBFR; 
    }

    /* limit gain to avoid problem on burst */
    
    if (lg > 0)
    {
      Ltmp = 0;   move32();
      for (i=0; i<lg; i++) 
      {
        Ltmp = L_mac(Ltmp, syn[i+i_subfr],  syn[i+i_subfr]);
      }
      Lener = 0;     move32();

      for (i=0; i<lg; i++) 
      {
        Lener  = L_mac(Lener, syn[i+i_subfr+T] ,syn[i+i_subfr+T]);

      }
        /* test to avoid division by 0 */
      if (Ltmp ==0)
        Ltmp  = 1;

      /*tmp = (float)sqrt(tmp / ener);     */
      e_tmp   = norm_l(Ltmp);
      e_ener  = norm_l(Lener);
      m_tmp   = extract_h(L_shl(Ltmp,e_tmp));
      m_ener  = extract_h(L_shl(Lener,e_ener));
      
      
      if (sub(m_ener, m_tmp)>0)
      {
        m_ener = shr (m_ener, 1);
        e_ener = sub(e_ener,1);
      }
      
      m_ener = div_s(m_ener, m_tmp);
      e_ener = sub(e_ener, e_tmp);
      /* Q15 -> Q31*/
      Ltmp = L_deposit_h(m_ener);
      tmp16 = add(e_ener,16);
      voAMRWBPDecIsqrt_n(&Ltmp, &tmp16);
      /* Output in Q14  (2 bits reserve to gain)*/  
      /* if tmp > gain we don't use */
      /* it and gain <= 1 in Q14 ->(16384)*/
      tmp16 = extract_h(L_shl(Ltmp, add(add(tmp16,e_ener),7)));   
      
      
      if (sub(tmp16,gain) < 0)
      {
        gain = tmp16; 
      }
    }
    /* calculate noise based on voiced pitch */
    tmp16 = gain;  /*don't need to right shift in fixed-point because gain in Q14*/
    tmp2 = shr(tmp16,1);
    for (i=0; i<lg; i++)
    {
      /*noise_buf[i+(2*L_FILT)] =
        tmp * (syn[i+i_subfr] - 0.5f*syn[i+i_subfr-T] - 0.5f*syn[i+i_subfr+T]);	 */

      Ltmp = L_mult(syn[i+i_subfr], tmp16);
      Ltmp = L_msu(Ltmp, syn[i+i_subfr-T], tmp2);
      Ltmp = L_msu(Ltmp, syn[i+i_subfr+T], tmp2);
      noise_buf[i+(2*L_FILT)] = round16(Ltmp);   
      
    }
    for (i=lg; i<L_SUBFR; i++) 
    {
      /*noise_buf[i+(2*L_FILT)] = tmp * (syn[i+i_subfr] - syn[i+i_subfr-T]);	 */
      noise_buf[i+(2*L_FILT)] = round16(L_mult(tmp16, sub(syn[i+i_subfr], syn[i+i_subfr-T]))); 
    }

    voAMRWBPDecCopy(st->wold_noise_pf, noise_buf, 2*L_FILT);
    voAMRWBPDecScale_sig(noise_buf, 2*L_FILT, sub(scale, st->Old_bpf_scale));
    
    voAMRWBPDecCopy(noise_buf+L_SUBFR, st->wold_noise_pf, 2*L_FILT);


    /* substract from voiced speech low-pass filtered noise */
    for (i=0; i<L_SUBFR; i++)
    {
      Ltmp = L_mult(Filt_lp[0] ,noise[i]);

      for(j=1; j<=L_FILT; j++)
      {
        Ltmp  = L_mac(Ltmp, Filt_lp[j],  add(noise[i-j] ,noise[i+j]));
      }

      synth_out[i+i_subfr] = sub(synth_out[i+i_subfr], round16(L_shr(Ltmp,scale)));	 
    }

    
    if(pit_adj == 0) 
    {
      i = shr(i_subfr,6); 
      T = T_sf[i];          
      gain = gainT_sf[i];   
    }
    /* update last scaling of noise buffer */
    st->Old_bpf_scale = scale;    

  }
  
  if (pit_adj == 0)
  {
    st->wold_T_pf[0] = T;        
    st->wold_gain_pf[0] = gain;    
  }
  else 
  {
    st->wold_T_pf[0]    = T_sf[NB_SUBFR-2];       
    st->wold_gain_pf[0] = gainT_sf[NB_SUBFR-2];   
    st->wold_T_pf[1]    = T_sf[NB_SUBFR-1];        
    st->wold_gain_pf[1] = gainT_sf[NB_SUBFR-1];   
  }
  return;
#endif

}



static Word16 Short_pitch_tracker(
  Word16 syn[],   /* (i):  synthesis [-PIT_MAX..L_SUBFR] */
  Word16 T        /* (i):  pitch period (>= PIT_MIN)     */
)          
{
  Word16 i,j, T2, ex1, ex2, fr1, fr2;
  Word32 Ltmp, Lcorr, Lener;
  Word32 Ltmp0, Lcorr0, Lener0;
  Word16 *v1, *v2;
// @shanrong modified
#if (FUNC_SHORT_PITCH_TRACKER_OPT)
  Word16 temp_1, temp_2;
#endif
// end

  /*----------------------------------------------------------------*
   * Test pitch/2 to avoid continuous pitch doubling                *
   * (short pitch is limited to PIT_MIN (34 = 376Hz) by the encoder *
   *----------------------------------------------------------------*/

  T2 = shr(T, 1);

  v1 = &syn[-L_EXTRA];  
  v2 = &syn[-T2-L_EXTRA];
      

  Lener = 1;   move32();
  Lcorr = 1;   move32();
  Ltmp  = 1;   move32();
// @shanrong modified
#if (!FUNC_SHORT_PITCH_TRACKER_OPT)
  for(j = 0;j < L_SUBFR+L_EXTRA; j+= (L_SUBFR+L_EXTRA)/4)
  {
    Lener0 = 0; move32();
    for (i=0; i<(L_SUBFR+L_EXTRA)/4; i++)
    {
      Lener0  = L_mac(Lener0, v1[i+j],v1[i+j]);
    }
    Lcorr0 = 0; move32();
    for (i=0; i<(L_SUBFR+L_EXTRA)/4; i++) 
    {
      Lcorr0 = L_mac(Lcorr0, v1[i+j], v2[i+j]);
    }
    Ltmp0 = 0; move32();
    for (i=0; i<(L_SUBFR+L_EXTRA)/4; i++)
    {
      Ltmp0 = L_mac(Ltmp0 ,v2[i+j] ,v2[i+j]);
    }
    Lener = L_add(Lener, L_shr(Lener0,1));
    Lcorr= L_add(Lcorr, L_shr(Lcorr0,1));
    Ltmp = L_add(Ltmp, L_shr(Ltmp0,1));
  }
#else
  temp_1 = L_SUBFR+L_EXTRA;
  temp_2 = (L_SUBFR+L_EXTRA)/4;
  
  for(j = 0;j < temp_1; j += temp_2)
  {
    Lener0 = 0;
    for (i=0; i< temp_2; i++)
    {
      Lener0  += v1[i+j] * v1[i+j];
    }
    Lcorr0 = 0;
    for (i=0; i<temp_2; i++) 
    {
      Lcorr0 += v1[i+j] * v2[i+j];
    }
    Ltmp0 = 0;
    for (i=0; i<temp_2; i++)
    {
      Ltmp0 += v2[i+j] * v2[i+j];
    }
    Lener += Lener0;
    Lcorr += Lcorr0;
    Ltmp += Ltmp0;
  }
#endif
// end

// @shanrong modified
#if (!FUNC_SHORT_PITCH_TRACKER_OPT)
  voAMRWBPDecL_Extract(Lener, &ex1, &fr1);
  voAMRWBPDecL_Extract(Ltmp, &ex2, &fr2);
  Ltmp = Mpy_32(ex1, fr1, ex2, fr2);
  
  Ltmp = L_shr(Ltmp,1);
  Ltmp = voAMRWBPDecIsqrt(Ltmp);
  Lcorr = L_shr(Lcorr,1);
  voAMRWBPDecL_Extract(Ltmp, &ex2, &fr2);
  voAMRWBPDecL_Extract(Lcorr, &ex1, &fr1);
#else
  ex1 = Lener >> 16;
  fr1 = (Word16)(L_shr(Lener, 1) - (ex1 << 15));
  ex2 = Ltmp >> 16;
  fr2 = (Word16)(L_shr(Ltmp, 1) - (ex2 << 15));

  Ltmp = (((Word32)ex1 * ex2) << 1) + ((ex1 * fr2) >> 14) + ((fr1 * ex2) >> 14);
  
  Ltmp = L_shr(Ltmp,1);
  Ltmp = voAMRWBPDecIsqrt(Ltmp);
  Lcorr = L_shr(Lcorr,1);
  
  ex2 = Ltmp >> 16;
  fr2 = (Word16)(L_shr(Ltmp, 1) - (ex2 << 15));
  ex1 = Lcorr >> 16;
  fr1 = (Word16)(L_shr(Lcorr, 1) - (ex1 << 15));
#endif
// end
  
  /* cn = normalized correlation of pitch/2 */
  
// @shanrong modified
#if (!FUNC_SHORT_PITCH_TRACKER_OPT)
  Ltmp = Mpy_32(ex1, fr1, ex2, fr2);

  
  if (L_sub(Ltmp, 31130)> 0) 
  {
    T = T2;     
  }
#else
  Ltmp = (((Word32)ex1 * ex2) << 1) + ((ex1 * fr2) >> 14) + ((fr1 * ex2) >> 14);
  if (Ltmp > 31130)
  {
    T = T2;     
  }
#endif
// end

  return(T);
}


