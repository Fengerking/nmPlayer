#include <stdlib.h>
#include <stdio.h>

#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "oper_32b.h"

#include "amr_plus_fx.h"

#define REMOVED_BITS 1


/*-----------------------------------------------------------------*
 * Funtion  init_coder_amr_plus                                    *
 *          ~~~~~~~~~~~~~~~~~~~                                    *
 *   - Allocate memory for static variables.                       *	
 *   - Initialization of variables for the coder section.          *
 *   - compute lag window and LP analysis window (if not done)     *
 *-----------------------------------------------------------------*/
void Init_coder_amrwb_plus(Coder_State_Plus_fx *st, Word16 num_chan, Word16 fscale, Word16 use_case_mode, Word16 full_reset)
{
  /* initialize memories (stereo part) */
  test();test();
  if ((full_reset >0) && (sub(use_case_mode,1) == 0) ) 
  {
    /* initialize memories (ClassB part) */
    st->vad_hist = 0;	move16();
    Wb_vad_init (&(st->_vadSt));
    st->_stClass = malloc(sizeof(NCLASSDATA_FX));
    InitClassifyExcitation(st->_stClass);
  }

  /* initialize bwe codebooks */
  test();  
  if (fscale == 0) 
  {
    st->left.mean_isf_hf = (Word16*)Mean_isf_hf_low_rate;       move16();
    st->left.dico1_isf_hf =(Word16*)Dico1_isf_hf_low_rate;       move16();
    st->right.mean_isf_hf = (Word16*)Mean_isf_hf_low_rate;       move16();
    st->right.dico1_isf_hf =(Word16*)Dico1_isf_hf_low_rate;       move16();
  }
  else { 
    st->left.mean_isf_hf = (Word16*)Mean_isf_hf_12k8;       move16();
    st->left.dico1_isf_hf =(Word16*)Dico1_isf_hf_12k8;       move16();
    st->right.mean_isf_hf = (Word16*)Mean_isf_hf_12k8;       move16();
    st->right.dico1_isf_hf =(Word16*)Dico1_isf_hf_12k8;       move16();
  }

  /* initialize memories (stereo part) */
  
  st->right.decim_frac = 0;       move16();
  st->left.decim_frac = 0;       move16();

  test();
  if (full_reset > 0) 
  {
    Init_Levinson(st->lev_mem);

    /* initialize memories (stereo part) */
    Set_zero(st->right.mem_decim_hf, 2*L_FILT24k);
    Set_zero(st->right.old_speech_hf, L_TOTAL_HIGH_RATE-L_FRAME_PLUS);
    Set_zero(st->right.mem_decim, L_MEM_DECIM_SPLIT);
    st->right.mem_preemph = 0;       move16();
    Init_coder_hf(&(st->right));
	
    Set_zero(st->left.mem_decim, L_MEM_DECIM_SPLIT);
    Set_zero(st->left.mem_decim_hf, 2*L_FILT24k);
    Set_zero(st->left.old_speech_hf, L_TOTAL_HIGH_RATE-L_FRAME_PLUS);
    st->left.mem_preemph = 0;       move16();
    Init_coder_hf(&(st->left));
  
    /* initialize memories (mono part) */
  
    Set_zero(st->old_speech, L_OLD_SPEECH_HIGH_RATE);
    Set_zero(st->old_synth, M);
  
    Set_zero(st->left.mem_sig_in,6);
    Set_zero(st->right.mem_sig_in,6);
    /* init band splitting memories */
    Set_zero(st->old_chan, L_OLD_SPEECH_HIGH_RATE);
    Set_zero(st->old_chan_2k, L_OLD_SPEECH_2k);
    Set_zero(st->old_speech_2k, L_OLD_SPEECH_2k);
    Set_zero(st->old_chan_hi, L_OLD_SPEECH_hi);
    Set_zero(st->old_speech_hi, L_OLD_SPEECH_hi);
  
    Init_coder_lf(st);
    test();
    if(sub(num_chan,2) == 0)
    {
      Init_coder_stereo_x(st);
    }
    Set_zero(st->mem_gain_code, 4);
    /* init band splitting memories */
    st->SwitchFlagPlusToWB = 0;       move16();
    st->prev_mod = 0;                 move16();
    
    /* excitation scaling*/ 
    st->Q_exc = 0;         move16();
    st->Old_Qexc = 0;     move16();
    
    /* Speech scaling */
    st->Q_new = 0;         move16();
    st->OldQ_sp_deci[0] = 0;   move16();
    st->OldQ_sp_deci[1] = 0;   move16();
    st->Q_max[0] = 0;      move16();
    st->Q_max[1] = 0;      move16();

    /* LF scaling*/
    st->Q_sp = 0;         move16();
    st->OldQ_sp = 0;         move16();
    st->scale_fac = 0;          move16();

    /* HF scaling */
    st->right.Q_sp_hf = 0;         move16();
    st->right.OldQ_sp_hf[0] = 0;   move16();
    st->right.OldQ_sp_hf[1] = 0;   move16();
    st->left.Q_sp_hf = 0;         move16();
    st->left.OldQ_sp_hf[0] = 0;   move16();
    st->left.OldQ_sp_hf[1] = 0;   move16();

    st->LastQMode = 0;           move16();/* 0 -> Acelp ; 1 ->tcx */

  } 
  else 
  {
    /* There may be a few states that still need to be reset when fstate changes*/
  }
  
  /* Initialize the LP analysis window */
  if(fscale <= FSCALE_DENOM) 
  {
    Cos_windowLP(st->window, L_WINDOW/2);
  }
  else 
  {
    Cos_windowLP(st->window, L_WINDOW_HIGH_RATE/2);
  }


  return;
}
	
/*-----------------------------------------------------------------*
 * Funtion  coder_amrwb_plus_first                                 *
 *          ~~~~~~~~~~~~~~~~                                       *
 *   - Fill lookahead buffers                                      *
 *                                                                 *
 *-----------------------------------------------------------------*/

Word16 Coder_amrwb_plus_first(   /* output: number of sample processed */
  Word16 channel_right[], /* input: used on mono and stereo       */
  Word16 channel_left[],  /* input: used on stereo only           */
  Word16 n_channel,     /* input: 1 or 2 (mono/stereo)              */
  Word16 L_frame,       /* input: frame size                        */
  Word16 L_next,        /* input: lookahead                         */
  Word16 fscale,
  Coder_State_Plus_fx *st   /* i/o : coder memory state                 */
)
{
  Word16 nb_samp, fac_fs;
  Word16 wchannel_right[4*L_FRAME_FSMAX];
  Word16 wchannel_left[2*L_FRAME_FSMAX];
  Word16 buffer[L_FRAME_FSMAX];
  Word16 old_speech[L_TOTAL_ST];
  Word16 sig_right[L_FRAME_PLUS];
  Word16 old_mono_hi[L_TOTAL_ST_hi];
  Word16 old_mono_2k[L_TOTAL_ST_2k];

  Word16 *new_mono_2k = old_mono_2k + L_OLD_SPEECH_2k;
  Word16 *new_mono_hi = old_mono_hi + L_OLD_SPEECH_hi,BitToRemove,scale_fac;
  Word16 *new_speech  = old_speech + L_OLD_SPEECH_ST;

  move16();       move16();       move16();
  Copy(channel_right, wchannel_right, L_frame);      /* Copy into Working space or ewmove scaling at the end... */
  Copy(channel_left, wchannel_left, L_frame);

 /*-----------------------------------------------------------------*
  * MONO/STEREO signal downsampling (codec working at 6.4kHz)       *
  * - decimate signal to fs=12.8kHz                                 *
  * - Perform 50Hz HP filtering of signal at fs=12.8kHz.            *
  * - Perform fixed preemphasis through 1 - g z^-1                  *
  * - Mix left and right channels into sum and difference signals   *
  *-----------------------------------------------------------------*/

 
  Set_zero(buffer, L_FRAME_FSMAX);
  Set_zero(old_speech, L_OLD_SPEECH_ST);
   
  test();
  if (sub(n_channel,2) == 0)
  {

    Set_zero(old_mono_2k, L_OLD_SPEECH_2k);                                     
    Set_zero(old_mono_hi, L_OLD_SPEECH_hi);                                     

    test(); test(); move16();
    if(sub(fscale,72) < 0 && fscale != 0)
      BitToRemove  = 2;       /* if ncoeff > 32, an overflow may occure in interpol_mem */  
    else
      BitToRemove = 1;      

    scale_fac = Scale_speech_st(wchannel_right, wchannel_left,L_frame, &(st->Q_new), st->OldQ_sp_deci, BitToRemove); 
    Rescale_enc_mem_st(st, scale_fac, scale_fac);

    test();
    if (fscale == 0)
    {
      /* copy memory into working space */
      Copy(wchannel_left, buffer+L_FRAME_FSMAX-L_next, L_next);                   
      Decim_12k8_p(buffer+L_FRAME_FSMAX-L_frame, L_frame, new_speech, st->left.mem_decim, 0);   
      /* copy memory into working space */
      Copy(wchannel_right, buffer+L_FRAME_FSMAX-L_next, L_next);                  
      Decim_12k8_p(buffer+L_FRAME_FSMAX-L_frame, L_frame, sig_right, st->right.mem_decim, 0);   
      nb_samp = L_next;     move16();
    }
    else
    {
      Set_zero(new_speech, L_FRAME_PLUS-L_NEXT_ST);                                  
      Set_zero(sig_right, L_FRAME_PLUS-L_NEXT_ST);                                   

      /* decimation and band split (HF temporary into channel_left/right) */
      test();
      if (sub(L_frame,L_FRAME32k) == 0) 
      {
        fac_fs = FSCALE_DENOM*3/2;      move16();
      }
      else 
      {
        fac_fs = fscale;                move16();
      }
      Decim_split_12k8(wchannel_left, L_frame, new_speech+L_FRAME_PLUS-L_NEXT_ST, wchannel_left,
                      L_NEXT_ST, fac_fs, st->left.mem_decim, &(st->left.decim_frac));	

      nb_samp = Decim_split_12k8(wchannel_right, L_frame, sig_right+L_FRAME_PLUS-L_NEXT_ST, wchannel_right,
                       L_NEXT_ST, fac_fs, st->right.mem_decim, &(st->right.decim_frac));
    }

    HP50_12k8_p(new_speech, L_FRAME_PLUS, st->left.mem_sig_in, fscale);                   
    HP50_12k8_p(sig_right, L_FRAME_PLUS, st->right.mem_sig_in, fscale); 

    /* parametric stereo : mix left and right channels */
    Mix_ch(new_speech,sig_right,new_speech,L_FRAME_PLUS,16384,16384);			

    /* do the lo,hi band-splitting on the mono signal */
    Band_split_taligned_2k(new_speech-2*L_FDEL,new_mono_2k-2*L_FDEL_2k,new_mono_hi,L_FRAME_PLUS);		

    /* copy working space into memory */

    Copy(old_speech+L_FRAME_PLUS, st->old_speech, L_OLD_SPEECH_ST);			
    Copy(old_mono_2k+L_FRAME_2k, st->old_speech_2k, L_OLD_SPEECH_2k);          
    Copy(old_mono_hi+L_FRAME_PLUS, st->old_speech_hi, L_OLD_SPEECH_hi);             

  } 
  else 
  {
    test(); test(); move16();
    if(sub(fscale,72) < 0 && fscale != 0)
      BitToRemove  = 2;       /* if ncoeff > 32, an overflow may occure in interpol_mem */  
    else
      BitToRemove = 1;      

    scale_fac = Scale_speech(wchannel_right,L_frame, &(st->Q_new), st->OldQ_sp_deci, BitToRemove); 
    Rescale_enc_mem(st, scale_fac, scale_fac);

    /* Mono case */
    test();
    if (fscale == 0)
    {
      Copy(wchannel_right, buffer+L_FRAME_FSMAX-L_next, L_next);                  
      Decim_12k8_p(buffer+L_FRAME_FSMAX-L_frame, L_frame, new_speech, st->right.mem_decim, 0);    
      nb_samp = L_next;   move16();
    }
    else
    {
      Set_zero(new_speech, L_FRAME_PLUS-L_NEXT);                                     

      /* decimation and band split (HF temporary into channel_right) */
      test();move16();
      if (sub(L_frame,L_FRAME32k) == 0) 
      {
        fac_fs = FSCALE_DENOM*3/2;    
      }
      else
      {
        fac_fs = fscale;
      }
      nb_samp = Decim_split_12k8(wchannel_right, L_frame, new_speech+L_FRAME_PLUS-L_NEXT, wchannel_right,
                                 L_NEXT, fac_fs, st->right.mem_decim, &(st->right.decim_frac));
    }

    HP50_12k8_p(new_speech, L_FRAME_PLUS, st->right.mem_sig_in, fscale);
    /* copy working space into memory */
    Copy(old_speech+L_FRAME_PLUS, st->old_speech, L_OLD_SPEECH_ST); 
  }
  Preemph_scaled(new_speech, &(st->Q_sp), &(st->scale_fac),&(st->right.mem_preemph),st->Q_max, &(st->OldQ_sp),PREEMPH_FAC_FX, REMOVED_BITS, L_FRAME_PLUS); 
  st->Q_sp = sub(add(st->Q_sp,st->Q_new),REMOVED_BITS);     move16();
  Rescale_enc_mem(st, st->scale_fac, 0);
  st->scale_fac = add(st->scale_fac, scale_fac);            move16();
  /* update lower band memory for next frame */
  Copy(&old_speech[L_FRAME_PLUS], st->old_speech_pe, L_OLD_SPEECH_ST);

  test();                                                                              
  if (sub(n_channel,2) == 0)
  {
    Copy(sig_right, new_speech, L_FRAME_PLUS);                                      
 
    /* do the lo,hi band-splitting on the mono signal */
    Band_split_taligned_2k(&new_speech[-2*L_FDEL] , &new_mono_2k[-2*L_FDEL_2k], new_mono_hi, L_FRAME_PLUS);			

    /* copy working space into memory */
    Copy(&old_speech[L_FRAME_PLUS], st->old_chan, L_OLD_SPEECH_ST);			
    Copy(&old_mono_2k[L_FRAME_2k], st->old_chan_2k, L_OLD_SPEECH_2k);            
    Copy(&old_mono_hi[L_FRAME_PLUS], st->old_chan_hi, L_OLD_SPEECH_hi);               

  }
  test();                                                                              
  if (sub(L_frame,L_FRAME8k) > 0) 
  {
    /* prepare buffers for MONO/STEREO Bandwidth extension */
    test();
    if (fscale == 0)
    {
      /* copy memory into working space */
      Copy(wchannel_right, &buffer[L_FRAME_FSMAX-L_next], L_next);                   
      Decim_12k8_p(&buffer[L_FRAME_FSMAX-L_frame], L_frame, new_speech, st->right.mem_decim_hf, (fscale == 0)?1:2);  
    }
    else
    {
      test();
      if (sub(n_channel,2) == 0)
      { /* right HF was stored into channel_right */
        Copy(wchannel_right, &new_speech[L_FRAME_PLUS-L_NEXT_ST], L_NEXT_ST);             

      }
      else 
      { /* right HF was stored into channel_right */
        Copy(wchannel_right, &new_speech[L_FRAME_PLUS-L_NEXT], L_NEXT);                  
      }
    }

    Copy(&old_speech[L_FRAME_PLUS], st->right.old_speech_hf, L_OLD_SPEECH_ST);        
  
    test();                                                                                
    if (sub(n_channel,2) == 0)
    {
      test();
      if (fscale == 0)
      { /* copy memory into working space */
        Copy(wchannel_left, &buffer[L_FRAME_FSMAX-L_next], L_next);                  

        Decim_12k8_p(&buffer[L_FRAME_FSMAX-L_frame], L_frame, new_speech, 
                   st->left.mem_decim_hf, (fscale == 0)?1:2);	                
      }
      else
      { /* left HF was stored into channel_left */
        Copy(wchannel_left, &new_speech[L_FRAME_PLUS-L_NEXT_ST], L_NEXT_ST);              

      }
      Copy(&old_speech[L_FRAME_PLUS], st->left.old_speech_hf, L_OLD_SPEECH_ST);	
    }
  }


  return(nb_samp);
}


/*-----------------------------------------------------------------*
 * Funtion  coder_amrwb_plus_stereo                                *
 *          ~~~~~~~~~~~~~~~~                                       *
 *   - Main stereo coder routine.                                  *
 *                                                                 *
 *-----------------------------------------------------------------*/
Word16 Coder_amrwb_plus_stereo(  /* output: number of sample processed */
  Word16 channel_right[],        /* input: used on mono and stereo       */
  Word16 channel_left[],         /* input: used on stereo only           */
  Word16 codec_mode,             /* input: AMR-WB+ mode (see cnst.h)         */
  Word16 L_frame,                /* input: 80ms frame size                   */
  Word16 serial[],               /* output: serial parameters                */
  Coder_State_Plus_fx *st,       /* i/o : coder memory state                 */
  Word16 use_case_mode,
  Word16 fscale,
  Word16 StbrMode
)
{
  /* Scalars */
  Word16 i, k, nbits_pack;
  Word16 mod[NB_DIV];

  /* LTP parameters for high band */
  Word16 nb_samp, fac_fs;

  /*ClassB parameters*/
  Word16 excType[4];  

  /* LPC coefficients of lower frequency */
  Word16 wAqLF[(NB_SUBFR+1)*(M+1)];

  Word16 sparam[NB_DIV*NPRM_DIV];
  Word16 sprm_stereo[MAX_NPRM_STEREO_DIV*NB_DIV];   /* see cnst.h */
  Word16 sprm_hf_left[NB_DIV*NPRM_BWE_DIV];
  Word16 sprm_hf_right[NB_DIV*NPRM_BWE_DIV];

  /* vector working at fs=12.8kHz */
  Word16 wsig_left[L_FRAME_PLUS];
  Word16 sig_right[L_FRAME_PLUS];

  Word16 old_speech[L_TOTAL_ST];
  Word16 *speech, *new_speech;

  Word16 old_synth[M+L_FRAME_PLUS];
  Word16 *wsynth;

  /* LTP parameters for high band */
  Word16 wol_gain[NB_DIV];  
  Word16 wT_out[NB_DIV];    
  Word16 wp_out[NB_DIV],BitToRemove, scale_fac, scale_fac_hf;    


  Word16 wchannel_right[4*L_FRAME_FSMAX];
  Word16 wchannel_left[2*L_FRAME_FSMAX];


  Copy(channel_right, wchannel_right, L_frame);      /* Copy into Working space or ewmove scaling at the end... */
  Copy(channel_left, wchannel_left, L_frame);


 /*---------------------------------------------------------------------*
  * Initialize pointers to speech vector.                               *
  *                                                                     *
  *                     20ms     20ms     20ms     20ms    >=20ms       *
  *             |----|--------|--------|--------|--------|--------|     *
  *           past sp   div1     div2     div3     div4    L_NEXT       *
  *             <--------  Total speech buffer (L_TOTAL_PLUS)  -------->     *
  *        old_speech                                                   *
  *                  <----- present frame (L_FRAME_PLUS) ----->              *
  *                  |        <------ new speech (L_FRAME_PLUS) ------->     *
  *                  |        |                                         *
  *                speech     |                                         *
  *                         new_speech                                  *
  *---------------------------------------------------------------------*/

  new_speech = old_speech + L_OLD_SPEECH_ST;                                                move16();
  speech     = old_speech + L_TOTAL_ST - L_FRAME_PLUS - L_A_MAX - L_BSP;                    move16();
  wsynth = old_synth + M;                    move16();

 /*-----------------------------------------------------------------*
  * STEREO signal downsampling (codec working at 6.4kHz)            *
  * - decimate signal to fs=12.8kHz                                 *
  * - Perform 50Hz HP filtering of signal at fs=12.8kHz.            *
  *-----------------------------------------------------------------*/
  test(); test(); move16();
  if(sub(fscale,72) < 0 && fscale != 0)
    BitToRemove  = 2;       /* in that case ncoeff > 32  and overflow may occure in interpol_mem */  
  else
    BitToRemove = 1;      

  scale_fac = Scale_speech_st(wchannel_right,wchannel_left,L_frame, &(st->Q_new), st->OldQ_sp_deci, BitToRemove); 
  Rescale_enc_mem_st(st, scale_fac, scale_fac);


  test();
  if (fscale == 0)
  {
    Decim_12k8_p(wchannel_left, L_frame, wsig_left, st->left.mem_decim, 0);
    Decim_12k8_p(wchannel_right, L_frame, sig_right, st->right.mem_decim, 0);              
    nb_samp = L_frame;      move16();
  }
  else
  {
    /* decimation and band split (HF temporary into channel_left/right) */
    test();
    if (sub(L_frame,L_FRAME32k) == 0) 
    {
      fac_fs = FSCALE_DENOM*3/2;    move16();
    }
    else 
    {
      fac_fs = fscale;              move16();
    }
    Decim_split_12k8(wchannel_left, L_frame, wsig_left, wchannel_left,
                     L_FRAME_PLUS, fac_fs, st->left.mem_decim, &(st->left.decim_frac));

    nb_samp = Decim_split_12k8(wchannel_right, L_frame, sig_right, wchannel_right,
                     L_FRAME_PLUS, fac_fs, st->right.mem_decim, &(st->right.decim_frac));

  }

  HP50_12k8_p(wsig_left, L_FRAME_PLUS, st->left.mem_sig_in, fscale);
  HP50_12k8_p(sig_right, L_FRAME_PLUS, st->right.mem_sig_in, fscale);                     

 /*-----------------------------------------------------------------*
  * Encode MONO low frequency band.                                 *
  * - Mix left and right channels (mono signal)                     *
  * - Perform fixed preemphasis through 1 - g z^-1                  *
  * - Encode low frequency band using ACELP/TCX model               *
  *-----------------------------------------------------------------*/
  Mix_ch(wsig_left,sig_right,new_speech,L_FRAME_PLUS, 16384, 16384);

  Preemph_scaled(new_speech, &(st->Q_sp), &(st->scale_fac),&(st->right.mem_preemph),st->Q_max, &(st->OldQ_sp),PREEMPH_FAC_FX, REMOVED_BITS, L_FRAME_PLUS); 
  st->Q_sp = sub(add(st->Q_sp,st->Q_new),REMOVED_BITS);                    move16();
  Rescale_enc_mem_st(st, st->scale_fac, 0);
  st->scale_fac = add(st->scale_fac, scale_fac);                    move16();

 /* copy memory into working space */
  Copy(st->old_speech_pe, old_speech, L_OLD_SPEECH_ST);
  Copy(st->old_synth, old_synth, M);
                                    
  test();
  if (sub(use_case_mode,USE_CASE_B) == 0) 
  {
    for(i=0;i<4;i++)
    {
      {
        Word16 tmps[256];
        /* WB_vad work's on unscaled signal only */
        Copy(&new_speech[256*i],tmps, 256);
        Scale_sig(tmps, 256, negate(st->Q_sp));     /* to have better level estimation */

        st->_stClass->vadFlag[i] = Wb_vad(st->_vadSt, tmps);                     move16();
      }
      test();                                                                          
      if (st->_stClass->vadFlag[i] == 0)
      {
        st->vad_hist = add(st->vad_hist, 1);                       move16();
      } 
      else 
      {
        st->vad_hist = 0;                                                 move16();
      }

      excType[i] = ClassifyExcitation(st->_stClass, st->_vadSt->level, i);     move16();

    }
      Coder_lf_b(codec_mode, speech, wsynth, mod, wAqLF, st->window, sparam, 
                                wol_gain, wT_out, wp_out, excType, fscale, st); 

  } 
  else
  {
  
    for (i=0;i<4;i++) 
    {
      excType[i] = 0;    move16();            
    }
    /* encode mono lower band */
    Coder_lf(codec_mode, speech, wsynth, mod, wAqLF, st->window, sparam, 
             wol_gain, wT_out, wp_out, excType, fscale, st);                               
  }
                                                                                        
  for(i=0;i<4;i++)
  { 
    mod[i] = excType[i];   move16();                                                             
  }
  /* update lower band memory for next frame */
  Copy(&old_speech[L_FRAME_PLUS], st->old_speech_pe, L_OLD_SPEECH_ST);                 
  Copy(&old_synth[L_FRAME_PLUS], st->old_synth, M);                                    
 /*------------------------------------------------------------*
  * STEREO Bandwidth extension (2 channels used)               *
  * - fold and decimate higher band into new_speech_hf         *
  *   (2000Hz..6400Hz <-- 6400Hz..10800 Hz)                    *
  * - encode HF using 0.8kbps per channel.                     *
  *------------------------------------------------------------*/
  
  test();                                                                                            
  if (sub(L_frame,L_FRAME8k) > 0)
  {

    Word16 old_speech_hf[L_TOTAL_ST];
    Word16 *new_speech_hf, *speech_hf_;
    
    new_speech_hf = old_speech_hf + L_OLD_SPEECH_ST;                                                move16();
    speech_hf_     = old_speech_hf + L_TOTAL_ST - L_FRAME_PLUS - L_A_MAX - L_BSP;                    move16();
    /*left*/
    Copy(st->left.old_speech_hf, old_speech_hf, L_OLD_SPEECH_ST);             
    test(); 
    if (fscale == 0)
    {
      Decim_12k8_p(wchannel_left, L_frame, new_speech_hf, st->left.mem_decim_hf, 1);       
    }
    else
    {
      /* HF was stored into channel_right */
      Copy(wchannel_left, new_speech_hf, L_FRAME_PLUS);
    }
    Copy(&old_speech_hf[L_FRAME_PLUS], st->left.old_speech_hf, L_OLD_SPEECH_ST);   

    if (StbrMode < 0) 
    {
      Copy(speech_hf_, wchannel_left, L_FRAME_PLUS+L_A_MAX+L_BSP);
    }
    else 
    {
      scale_fac_hf = Scale_speech(old_speech_hf, L_TOTAL_ST, &(st->left.Q_sp_hf), st->left.OldQ_sp_hf, 1);
      Rescale_enc_mem_hf(&(st->left), scale_fac_hf);
      Coder_hf(mod, wAqLF, speech, speech_hf_, st->window, sprm_hf_left,fscale, &(st->left),st->Q_new, st->Q_sp);
    }
    /* right*/
    Copy(st->right.old_speech_hf, old_speech_hf, L_OLD_SPEECH_ST);             
    test(); 
    if (fscale == 0)
    {
      Decim_12k8_p(wchannel_right, L_frame, new_speech_hf, st->right.mem_decim_hf, 1);       
    }
    else
    {
      /* HF was stored into wchannel_right */
      Copy(wchannel_right, new_speech_hf, L_FRAME_PLUS);
    }
    test();
    if (StbrMode < 0)
    {
      for (i=0; i<L_FRAME_PLUS+L_A_MAX+L_BSP; i++) 
      {
        speech_hf_[i] = round(L_mac(L_mult(speech_hf_[i],16384), wchannel_left[i], 16384));    move16();
      }
    }
    Copy(&old_speech_hf[L_FRAME_PLUS], st->right.old_speech_hf, L_OLD_SPEECH_ST);   
    scale_fac_hf = Scale_speech(old_speech_hf, L_TOTAL_ST, &(st->right.Q_sp_hf), st->right.OldQ_sp_hf, 1); 
    Rescale_enc_mem_hf(&(st->right), scale_fac_hf);
    Coder_hf(mod, wAqLF, speech, speech_hf_, st->window, sprm_hf_right, fscale, &(st->right),st->Q_new, st->Q_sp);         

  } 
  else 
  {
    for (i=0; i<NB_DIV*NPRM_BWE_DIV; i++) 
    {
      sprm_hf_right[i] = 0;      move16();
      sprm_hf_left[i] = 0;       move16();
    }
  }
 /*------------------------------------------------------------*
  * STEREO low frequency band encoder                          *
  * - Mix left and right channels (mono signal)                *
  * - split mono and right signal (2k, hi).                    *
  * - perform parametric stereo encoding				   *
  *------------------------------------------------------------*/

{
  Word16 old_mono_2k[L_TOTAL_ST_2k];
  Word16 *new_mono_2k = old_mono_2k + L_OLD_SPEECH_2k;
  Word16 *wmono_2k = old_mono_2k+L_TOTAL_ST_2k-L_FRAME_2k-L_A_2k-L_FDEL_2k;

  Word16 old_chan_2k[L_TOTAL_ST_2k];
  Word16 *wnew_chan_2k = old_chan_2k + L_OLD_SPEECH_2k;
  Word16 *wchan_2k = old_chan_2k+L_TOTAL_ST_2k-L_FRAME_2k-L_A_2k-L_FDEL_2k;

  Word16 old_mono_hi[L_TOTAL_ST_hi];
  Word16 *new_mono_hi = old_mono_hi + L_OLD_SPEECH_hi;
  Word16 *wmono_hi = old_mono_hi + L_TOTAL_ST_hi - L_FRAME_PLUS-L_A_MAX;
 
  Word16 old_chan_hi[L_TOTAL_ST_hi];
  Word16 *wnew_chan_hi = old_chan_hi + L_OLD_SPEECH_hi;
  Word16 *wchan_hi = old_chan_hi + L_TOTAL_ST_hi - L_FRAME_PLUS-L_A_MAX;
  move16();move16();move16();move16();
  move16();move16();move16();move16();

  /* copy memory into working space */
  Copy(st->old_speech, old_speech, L_OLD_SPEECH_ST);						
  Copy(st->old_speech_2k, old_mono_2k, L_OLD_SPEECH_2k);                                       
  Copy(st->old_speech_hi, old_mono_hi, L_OLD_SPEECH_hi);  
  Mix_ch(wsig_left,sig_right,new_speech,L_FRAME_PLUS, 16384, 16384);					
  
  /* do the lo,hi band-splitting on the mono signal */
  Band_split_taligned_2k(&new_speech[-2*L_FDEL],&new_mono_2k[-2*L_FDEL_2k],new_mono_hi,L_FRAME_PLUS);					
  /* copy working space into memory */
  Copy(&old_speech[L_FRAME_PLUS], st->old_speech, L_OLD_SPEECH_ST);					
  Copy(&old_mono_2k[L_FRAME_2k], st->old_speech_2k, L_OLD_SPEECH_2k);				
  Copy(&old_mono_hi[L_FRAME_PLUS], st->old_speech_hi, L_OLD_SPEECH_hi);				

  /* copy memory into working space */
  Copy(st->old_chan, old_speech, L_OLD_SPEECH_ST);						
  Copy(st->old_chan_2k, old_chan_2k, L_OLD_SPEECH_2k);						
  Copy(st->old_chan_hi, old_chan_hi, L_OLD_SPEECH_hi);						

  Copy(sig_right, new_speech, L_FRAME_PLUS);							

  /* do the lo,hi band-splitting on the mono signal */
  Band_split_taligned_2k(&new_speech[-2*L_FDEL],&wnew_chan_2k[-2*L_FDEL_2k],wnew_chan_hi,L_FRAME_PLUS);				
  /* copy working space into memory */
  Copy(&old_speech[L_FRAME_PLUS], st->old_chan, L_OLD_SPEECH_ST);				
  Copy(&old_chan_2k[L_FRAME_2k], st->old_chan_2k, L_OLD_SPEECH_2k);				
  Copy(&old_chan_hi[L_FRAME_PLUS], st->old_chan_hi, L_OLD_SPEECH_hi);			


  test();
  if (StbrMode < 0)
  {
    Init_coder_stereo_x(st);
    st->mem_stereo_ovlp_size= L_OVLP_2k;    move16();
  }
  else
  {
    Coder_stereo_x(wAqLF, StbrMode, sprm_stereo, fscale, st, wmono_hi, wchan_hi, wmono_2k, wchan_2k);
  }
}


 /*--------------------------------------------------*
  * encode bits for serial stream                    *
  *--------------------------------------------------*/

  /* mode (0=ACELP 20ms, 1=TCX 20ms, 2=TCX 40ms, 3=TCX 80ms) */

  /* for 20-ms packetization, divide by 4 the 80-ms bitstream */
  nbits_pack = shr(add(NBITS_CORE_FX[codec_mode], NBITS_BWE),2);					
  test();
  if (StbrMode >= 0)
  {
    nbits_pack = add(nbits_pack, shr(add(StereoNbits_FX[StbrMode],NBITS_BWE),2));
  }

  for (k=0; k<NB_DIV; k++) 
  {
    Int2bin(mod[k], 2, &serial[k*nbits_pack]);                    
  }
  Enc_prm(mod, codec_mode, sparam, serial, nbits_pack);                                          
  test();
  if (StbrMode >= 0)
  {
    Enc_prm_stereo_x(sprm_stereo, serial, nbits_pack, NBITS_BWE, StbrMode);
    Enc_prm_hf(mod, sprm_hf_left, serial-NBITS_BWE/4, nbits_pack);
  }
  Enc_prm_hf(mod, sprm_hf_right, serial, nbits_pack);
  return(nb_samp);
}

/*-----------------------------------------------------------------*
 * Funtion  coder_amrwb_plus_mono                                  *
 *          ~~~~~~~~~~~~~~~~                                       *
 *   - Main mono coder routine.                                    *
 *                                                                 *
 *-----------------------------------------------------------------*/
Word16 Coder_amrwb_plus_mono(  /* output: number of sample processed */
  Word16 channel_right[], /* input: used on mono and stereo       */
  Word16 codec_mode,    /* input: AMR-WB+ mode (see cnst.h)         */
  Word16 L_frame,       /* input: 80ms frame size                   */
  Word16 serial[],    /* output: serial parameters                */
  Coder_State_Plus_fx *st,    /* i/o : coder memory state                 */
  Word16 use_case_mode,
  Word16 fscale
)
{
  /* LPC coefficients of lower frequency */
  Word16 AqLF[(NB_SUBFR+1)*(M+1)];

  Word16 param[NB_DIV*NPRM_DIV];

  Word16 prm_hf_right[NB_DIV*NPRM_BWE_DIV];

  /* Scalars */
  Word16 i, k, nbits_pack;
  Word16 mod[NB_DIV];

  /* LTP parameters for high band */
  Word16 ol_gain[NB_DIV];  
  Word16 T_out[NB_DIV];    
  Word16 p_out[NB_DIV];    
  Word16 nb_samp, fac_fs;

  /*ClassB parameters*/
  Word16 excType[4];  
  /* vector working at fs=12.8kHz */
  Word16 old_speech[L_TOTAL_ST];
  Word16 old_synth[M+L_FRAME_PLUS];

  Word16 *speech, *new_speech;
  Word16 *wsynth;

     /* LTP parameters for high band */
  Word16 scale_fac_hf;

  /*ClassB parameters*/
  Word16 BitToRemove; 
  Word16 scale_fac = 0;

  Word16 wchannel_right[4*L_FRAME_FSMAX];


  Copy(channel_right, wchannel_right, L_frame);

 /*---------------------------------------------------------------------*
  * Initialize pointers to speech vector.                               *
  *                                                                     *
  *                     20ms     20ms     20ms     20ms    >=20ms       *
  *             |----|--------|--------|--------|--------|--------|     *
  *           past sp   div1     div2     div3     div4    L_NEXT       *
  *             <--------  Total speech buffer (L_TOTAL_PLUS)  -------->*
  *        old_speech                                                   *
  *                  <----- present frame (L_FRAME_PLUS) ----->         *
  *                  |        <------ new speech (L_FRAME_PLUS) ------->*
  *                  |        |                                         *
  *                speech     |                                         *
  *                         new_speech                                  *
  *---------------------------------------------------------------------*/

  new_speech = old_speech + L_OLD_SPEECH_ST;                           move16();
  speech     = old_speech + L_TOTAL_ST - L_FRAME_PLUS - L_A_MAX - L_BSP;        move16();
  wsynth      = old_synth + M;        move16();


 /*-----------------------------------------------------------------*
  * MONO/STEREO signal downsampling (codec working at 6.4kHz)       *
  * - decimate signal to fs=12.8kHz                                 *
  * - Perform 50Hz HP filtering of signal at fs=12.8kHz.            *
  * - Perform fixed preemphasis through 1 - g z^-1                  *
  * - Mix left and right channels into sum and difference signals   *
  * - perform parametric stereo encoding                            *
  *-----------------------------------------------------------------*/

  test(); test(); move16();
  if(sub(fscale,72) < 0 && fscale != 0)
    BitToRemove  = 2;       /* in this case ncoeff > 32  and overflow may occure in interpol_mem */  
  else
    BitToRemove = 1;      

  scale_fac = Scale_speech(wchannel_right,L_frame, &(st->Q_new), st->OldQ_sp_deci, BitToRemove); 
  Rescale_enc_mem(st, scale_fac, scale_fac);
  test();
  if (fscale == 0)
  {
    Decim_12k8_p(wchannel_right, L_frame, new_speech, st->right.mem_decim, 0);
    nb_samp = L_frame;      move16();
  }
  else
  {
    /* decimation and band split (HF temporary into channel_right) */
    test();
    if (sub(L_frame,L_FRAME32k) == 0) 
    {
      fac_fs = FSCALE_DENOM*3/2;    move16();
    }
    else 
    {
      fac_fs = fscale;              move16();
    }
    nb_samp = Decim_split_12k8(wchannel_right, L_frame, new_speech, wchannel_right,
                      L_FRAME_PLUS, fac_fs, st->right.mem_decim, &(st->right.decim_frac)); 
  }

  HP50_12k8_p(new_speech, L_FRAME_PLUS, st->right.mem_sig_in, fscale);

 /*------------------------------------------------------------*
  * Encode MONO low frequency band using ACELP/TCX model       *
  *------------------------------------------------------------*/
  Preemph_scaled(new_speech, &(st->Q_sp), &(st->scale_fac),&(st->right.mem_preemph),st->Q_max, &(st->OldQ_sp),PREEMPH_FAC_FX, REMOVED_BITS, L_FRAME_PLUS); 

  st->Q_sp = sub(add(st->Q_sp,st->Q_new),REMOVED_BITS);
  Rescale_enc_mem(st, st->scale_fac, 0);
  st->scale_fac = add(st->scale_fac, scale_fac);
  
  /* copy memory into working space */
  Copy(st->old_speech_pe, old_speech, L_OLD_SPEECH_ST);
  Copy(st->old_synth, old_synth, M);

  test();
  if (sub(use_case_mode,USE_CASE_B) == 0) 
  {
    for(i=0;i<4;i++)
    {
      {
        Word16 tmps[256];
        /* WB_vad work's on unscaled signal only */
        Copy(&new_speech[256*i],tmps, 256);
        Scale_sig(tmps, 256, negate(st->Q_sp));     /* to have better level estimation */
        st->_stClass->vadFlag[i] = Wb_vad(st->_vadSt, tmps);         move16();

      }
      test();                                                                          
      if (st->_stClass->vadFlag[i] == 0)
      {
        st->vad_hist = add(st->vad_hist, 1);           move16();
      } 
      else 
      {
        st->vad_hist = 0;                                                 move16();
      }
      excType[i] = ClassifyExcitation(st->_stClass, st->_vadSt->level, i);     move16();
    }
  /* encode mono lower band */

    Coder_lf_b(codec_mode, speech, wsynth, mod, AqLF, st->window, param, 
                                ol_gain, T_out, p_out, excType, fscale, st); 
  } 
  else
  {
      for (i = 0; i < 4; i++)
      {
         excType[i] = 0;      move16();
      }
      /* encode mono lower band */
      Coder_lf(codec_mode, speech, wsynth, mod, AqLF, st->window, param,
               ol_gain, T_out, p_out, excType, fscale, st);

  }                                                                              

  for(i=0;i<4;i++)
  {
    mod[i] = excType[i];      move16();
  }
  st->prev_mod = mod[3];      move16();

  /* update lower band memory for next frame */
  Copy(&old_speech[L_FRAME_PLUS], st->old_speech_pe, L_OLD_SPEECH_ST);              
  Copy(&old_synth[L_FRAME_PLUS], st->old_synth, M);   

 /*------------------------------------------------------------*
  * MONO/STEREO Bandwidth extension (2 channels used in stereo)*
  * - fold and decimate higher band into new_speech_hf         *
  *   (2000Hz..6400Hz <-- 6400Hz..10800 Hz)                    *
  * - encode HF using 0.8kbps per channel.                     *
  *------------------------------------------------------------*/
  test();
  if (sub(L_frame,L_FRAME8k) > 0)
  {
    Word16 old_speech_hf[L_TOTAL_ST];
    Word16 *new_speech_hf, *speech_hf_;
    
    new_speech_hf = old_speech_hf + L_OLD_SPEECH_ST;                            move16();
    speech_hf_     = old_speech_hf + L_TOTAL_ST - L_FRAME_PLUS - L_A_MAX - L_BSP;    move16();

    Copy(st->right.old_speech_hf, old_speech_hf, L_OLD_SPEECH_ST);             

    test(); 
    if (fscale == 0)
    {
      Decim_12k8_p(wchannel_right, L_frame, new_speech_hf, st->right.mem_decim_hf, 1);       
    }
    else
    {
      /* HF was stored into channel_right */
      Copy(wchannel_right, new_speech_hf, L_FRAME_PLUS);
    }

    Copy(&old_speech_hf[L_FRAME_PLUS], st->right.old_speech_hf, L_OLD_SPEECH_ST);   
    scale_fac_hf = Scale_speech(old_speech_hf, L_TOTAL_ST, &(st->right.Q_sp_hf), st->right.OldQ_sp_hf, 1);
    Rescale_enc_mem_hf(&(st->right), scale_fac_hf);

    Coder_hf(mod, AqLF, speech, speech_hf_, st->window, prm_hf_right, fscale, &(st->right),st->Q_new, st->Q_sp);         
  } 
  else
  {
                                                                                
    for (k=0; k<NB_DIV*NPRM_BWE_DIV; k++) 
    {
      prm_hf_right[k] = 0;     move16();
    }
  }

 /*--------------------------------------------------*
  * encode bits for serial stream                    *
  *--------------------------------------------------*/

  /* mode (0=ACELP 20ms, 1=TCX 20ms, 2=TCX 40ms, 3=TCX 80ms) */

  /* for 20-ms packetization, divide by 4 the 80-ms bitstream */
  nbits_pack = shr(add(NBITS_CORE_FX[codec_mode], NBITS_BWE),2);
                                                                                
  for (k=0; k<NB_DIV; k++) 
  {
    Int2bin(mod[k], 2, &serial[k*nbits_pack]);                                  
  }
  Enc_prm(mod, codec_mode, param, serial, nbits_pack);                          
  Enc_prm_hf(mod, prm_hf_right, serial, nbits_pack);                            
  return(nb_samp);
}
