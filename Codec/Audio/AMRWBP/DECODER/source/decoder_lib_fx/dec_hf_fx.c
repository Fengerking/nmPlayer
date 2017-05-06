
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "oper_32b.h"
#include "math_op.h"


#include "amr_plus_fx.h"

/*-----------------------------------------------------------------*
 * Funtion  init_decoder_hf                                        *
 * ~~~~~~~~~~~~~~~~~~~~~~~~                                        *
 *                                                                 *
 *   ->Initialization of variables for the decoder section.        *
 *     (band 6400Hz to 10800Hz).                                   *
 *                                                                 *
 *-----------------------------------------------------------------*/

void Init_decoder_hf(Decoder_StState_fx *st)
{

  /* Static vectors to zero */

  Set_zero(st->wmem_syn_hf, MHF);
  Set_zero(st->wpast_q_isf_hf, MHF);
  Set_zero(st->wpast_q_isf_hf_other, MHF);
  st->wpast_q_gain_hf = 0;
  st->wpast_q_gain_hf_other = 0;

  st->wold_gain = 256;    
  st->Lp_amp = 0;         move32();
  st->Lthreshold = 0;     move32();
  
  
  st->Q_synHF = 6;       
  
   
  /* Initialize the ISPs */
  voAMRWBPDecCopy(Isp_init_HF, st->wispold_hf, MHF);

  return;
}

/*-----------------------------------------------------------------*
 * Funtion decoder_lf                                              *
 * ~~~~~~~~~~~~~~~~~~                                              *
 *   ->Principle decoder routine (band 6400Hz to 10800Hz).         *
 *                                                                 *
 *-----------------------------------------------------------------*/
static Word16 D_gain_chan(Word16 * gain_hf, Word16 *mem_gain, Word16 *prm, Word16 bfi, Word16 mode, Word16 *bad_frame, Word32 *Lgain, Word16 mono_dec_stereo );

void Decoder_hf(
  Word16 mod[],            /* (i)  : mode for each 20ms frame (mode[4]     */
  Word16 param[],          /* (i)  : parameters                            */
  Word16 param_other[],    /* (i)  : parameters for the right channel in case of mono decoding with stereo bitstream */
  Word16 mono_dec_stereo,  /* (i)  : 1=Mono decoding with stereo bitstream */
  Word16 bad_frame[],      /* (i)  : for each frame (bad_frame[4])         */
  Word16 AqLF[],           /* (i)  : decoded coefficients (AdLF[16])  Q12  */ 
  Word16 exc[],            /* (i)  : decoded excitation                    */
  Word16 synth_hf[],       /* (o)  : decoded synthesis                     */
  Word16 mem_lpc_hf[],     /* (i/o): lpc mem                               */
  Word16 *mem_gain_hf,     /* (i/o): gain hf mem                      Q8   */
  Word16 *ramp_state,      /* (i/o): Ramp to switch wb <-> wb+             */ 
  Decoder_StState_fx *st,  /* (i/o): decoder memory state                  */
  Word16 QexcHF            /* (i)  : Excitation scaling                    */ 
)
{
#if (FUNC_Decoder_HF_OPT)
  /* LPC coefficients */
  Word16 ispnew[MHF];     
  Word16 isfnew_other[MHF];
  Word16 isfnew[MHF];     
  Word16 Aq[(NB_SUBFR+1)*(MHF+1)]; /* A(z) for all subframes     */
  Word16 *p_Aq, tmp16, exp_g, frac_g;
  
  Word16 gain_hf[NB_SUBFR], gain_hf_tmp[NB_SUBFR], gain_hf_other[NB_SUBFR], gain16;  /* in log domain*/
  Word16 HF[L_SUBFR], *prm_in_other;

  /* Scalars */
  Word16 i, k, mode, bfi, sf, nsf, *prm, *prm_other;
  Word16 i_subfr, thr_lo, thr_hi;
  const Word16 *interpol_wind;
  Word32 Ltmp,Lgain_sum,Lgain_sum_other, gdiff;




  /*----------- DECODE AND SYNTHESIZE UPPER-BAND SIGNAL (synth_hf[])
                AND ADD IT TO UPSAMPLED LOWER-BAND SIGNAL (synthesis) ----------
    bitstream pointer = ptr

    coding modes (coder type=ACELP,TCX and frame length=20,40,80 ms) for upper-band are copied from lower-band
    HF decoding depends on bit allocation to HFs (nbits_hf) :
    nbits_hf=0:
    the lower-band signal post-filtered and upsampled is the actual synthesis
    (no HF decoding and synthesis)
    nbits_hf=4*16:
    the lower-band excitation is randomized, folded, and shaped first in time-domain
    with subframe gains then in frequency domain with a LP filter (plus energy smoothing)
   */
  /* compute Ai (Aq) and correction scale factor (gain) for each subframe */
  prm_other = param_other;
  prm_in_other = prm_other;

  for (k=0; k<NB_DIV; k++) 
  {
    bfi = bad_frame[k];
    mode = mod[k];
    if (mode == 0 || (mode == 1))
    {
      nsf = 4;
      interpol_wind = interpol_frac4;
    }
    else if (mode == 2)
    {
      nsf = 8;
      interpol_wind = interpol_frac8;
    }
    else
    {
      nsf = 16;
      interpol_wind = interpol_frac16;
    }
  

    /* set pointer to parameters */
    prm = param + (k*NPRM_BWE_DIV);
    if (mono_dec_stereo == 1)
    {
      prm_other = param_other + (k*NPRM_BWE_DIV);/*ptr*/
      prm_in_other = prm_other;

      Lgain_sum = 0;
      Lgain_sum_other = 0;
    }
    /* decode ISFs and convert ISFs to cosine domain */

    D_isf_hf(prm, isfnew, st->wpast_q_isf_hf, bfi, st->Mean_isf_hf, st->Dico1_isf_hf);

    if(mono_dec_stereo == 0)
    {
      voAMRWBPDecIsf_isp(isfnew, ispnew, MHF);
      /* interpolate Ai in ISP domain (Aq) */
      voAMRWBPDecIntLpc(st->wispold_hf, ispnew, interpol_wind, Aq, nsf, MHF);
      voAMRWBPDecCopy(ispnew, st->wispold_hf, MHF);

      /* compute gain correction factor to match amplitude at 6.4kHz */
      gain16 = Match_gain_6k4(&AqLF[((k*4)+nsf)*(M+1)], &Aq[nsf*(MHF+1)]);
      /* interpolate per subframe */
      Int_gain(st->wold_gain, gain16, interpol_wind, gain_hf, nsf);
      st->wold_gain = gain16;
    }
 
    /* decode 4 gains */
    prm += D_gain_chan(gain_hf, &(st->wpast_q_gain_hf),prm, bfi, mode, &bad_frame[k], &Lgain_sum, mono_dec_stereo ); 
    if(mono_dec_stereo == 1)
    {
      /* Need to decode the gain for the other channel also*/
      /* decode 4 gains */
      prm_other += D_gain_chan(gain_hf_other, &(st->wpast_q_gain_hf_other), prm_other, bfi, mode, &bad_frame[k], &Lgain_sum_other, mono_dec_stereo); 
    
      /* Decode the other channel's parameters in case of mono decoding when stereo*/
      /* decode ISFs and convert ISFs to cosine domain */
      thr_lo = (Word16)(((-10) * nsf) >> 16);
      thr_hi = -thr_lo;

      /* Difference in average gain offsets (in dB)*/
      gdiff = Lgain_sum - Lgain_sum_other;

      D_isf_hf(prm_in_other, isfnew_other, st->wpast_q_isf_hf_other, bfi, st->Mean_isf_hf, st->Dico1_isf_hf);
      /* Take the average between the both channels' isp's*/
      if(gdiff < (thr_lo << 8))
      {
        for (i = 0; i < MHF; i++)
        {
          isfnew[i] = isfnew_other[i];
        }
      }
      else  if(gdiff < (thr_hi <<8))
      {
        for(i=0;i<MHF;i++)
        {
          isfnew[i] = (isfnew[i] + isfnew_other[i] + 1) >> 1;
        }
      }

      voAMRWBPDecIsf_isp(isfnew, ispnew, MHF);
      /* interpolate Ai in ISP domain (Aq) */
      voAMRWBPDecIntLpc(st->wispold_hf, ispnew, interpol_wind, Aq, nsf, MHF);
      voAMRWBPDecCopy(ispnew, st->wispold_hf, MHF);
      /* compute gain correction factor to match amplitude at 6.4kHz */
      gain16 = Match_gain_6k4(&AqLF[((k*4)+nsf)*(M+1)], &Aq[nsf*(MHF+1)]);
      /* interpolate per subframe */
      Int_gain(st->wold_gain, gain16, interpol_wind, gain_hf_tmp, nsf);
      for(i=0;i<nsf;i++)
      {
        gain_hf[i] += gain_hf_tmp[i];
        gain_hf_other[i] += gain_hf_tmp[i];
      }
      st->wold_gain = gain16;
    }

    /*
     * AriL: We could also select overall gain based on channel balance, but since
     *       the low-band remains as average of both channels it could make the
     *       hihg-band unnecessarily loud in case of silent other channel
     *
     */

    /*Store latest LPC filter coeffs for possible WB+ -> WB switching*/
    voAMRWBPDecCopy(&Aq[nsf*(MHF+1)],mem_lpc_hf, MHF+1);
    
    p_Aq = Aq;
    for (sf=0; sf<nsf; sf++) 
    {
      //i_subfr = shl(add(shl(k,2),sf),6);
      i_subfr = ((k << 2) + sf) << 6;
        
      /*fgain = (float)pow(10.0, fgain_hf[sf]/20.0);*/

      Ltmp = gain_hf[sf] * 21771;                     /*0.166096 in Q17 -> Q25        */
      Ltmp = (Ltmp >> 9);                         /* From Q28 to Q16              */
      exp_g = Ltmp >> 16;
      frac_g = (Ltmp & 0xffff) >> 1;
      tmp16 = (Word16)voAMRWBPDecPow2(11, frac_g);            /* Put 11 as exponant so that   */
                                                      /* output of voAMRWBPDecPow2() will be:    */
                                                      /* 16384 < voAMRWBPDecPow2() <= 32767      */
      gain16 = shl(tmp16, exp_g);                     /* gain16 in Q11 */  
      if(mono_dec_stereo == 1)
      {
        /* Average the gain between the two channels*/
        /*fgain += (float)pow(10.0, fgain_hf_other[sf]/20.0);*/
        Ltmp = (gain_hf_other[sf] * 21771) >> 9;        /*0.166096 in Q17 -> Q25        */
                                                        /* From Q28 to Q16              */
//        voAMRWBPDecL_Extract(Ltmp, &exp_g, &frac_g);               /* Extract exponant of wgain_hf */
        exp_g = (Word16)(Ltmp >> 16);
        frac_g = (Word16)((Ltmp & 0xffff) >> 1);
        tmp16 = (Word16)voAMRWBPDecPow2(11, frac_g);            /* Put 11 as exponant so that   */
                                                        /* output of voAMRWBPDecPow2() will be:    */
                                                        /* 16384 < voAMRWBPDecPow2() <= 32767      */
        tmp16 = shl(tmp16, exp_g);                      /* gain16 in Q11 */  
        /*fgain *= 0.5;*/
        gain16 = (tmp16 + gain16 + 1) >> 1;
      }

      /* Scale the gain towards full value gradually after a switch from AMR-WB mode*/
      if ((*ramp_state) < 64)
      {
        gain16 = (gain16 * Gain_hf_ramp[*ramp_state] + 0x4000) >> 15;            
        (*ramp_state)++;
      }

      /* Memorize the latest gain for possible WB+ -> WB switching*/
      *mem_gain_hf = gain16;
      for (i=0; i<L_SUBFR; i++) 
      {
        /*fHF[i] = fgain*fexc[i+i_subfr];*/
        Ltmp = gain16 * exc[i+i_subfr];
        HF[i] = (Ltmp + 0x400) >> 11;
      }
      
      Soft_exc_hf(HF, &(st->Lp_amp));

      /* filtering through HF LP filter */
      voAMRWBPDecSyn_filt_s(sub(QexcHF,st->Q_synHF), p_Aq, MHF, HF, HF, L_SUBFR, st->wmem_syn_hf, 1);
      /* smooth energy of HF signal on subframe (HF[]) */
      Smooth_ener_hf(HF, &(st->Lthreshold));
      for (i=0; i<L_SUBFR; i++) 
      {
        synth_hf[i+i_subfr] = HF[i];
      }

      p_Aq += (MHF+1);
    }

    if (mode == 2)
    {
      k++;
    }
    else if (mode == 3)
    {
      k += 3;
    }
  }

  return;
  
#else

  /* LPC coefficients */
  Word16 ispnew[MHF];     
  Word16 isfnew_other[MHF];
  Word16 isfnew[MHF];     
  Word16 Aq[(NB_SUBFR+1)*(MHF+1)]; /* A(z) for all subframes     */
  Word16 *p_Aq, tmp16, exp_g, frac_g;
  
  Word16 gain_hf[NB_SUBFR], gain_hf_tmp[NB_SUBFR], gain_hf_other[NB_SUBFR], gain16;  /* in log domain*/
  Word16 HF[L_SUBFR], *prm_in_other;

  /* Scalars */
  Word16 i, k, mode, bfi, sf, nsf, *prm, *prm_other;
  Word16 i_subfr, thr_lo, thr_hi;
  const Word16 *interpol_wind;
  Word32 Ltmp,Lgain_sum,Lgain_sum_other, gdiff;




  /*----------- DECODE AND SYNTHESIZE UPPER-BAND SIGNAL (synth_hf[])
                AND ADD IT TO UPSAMPLED LOWER-BAND SIGNAL (synthesis) ----------
    bitstream pointer = ptr

    coding modes (coder type=ACELP,TCX and frame length=20,40,80 ms) for upper-band are copied from lower-band
    HF decoding depends on bit allocation to HFs (nbits_hf) :
    nbits_hf=0:
    the lower-band signal post-filtered and upsampled is the actual synthesis
    (no HF decoding and synthesis)
    nbits_hf=4*16:
    the lower-band excitation is randomized, folded, and shaped first in time-domain
    with subframe gains then in frequency domain with a LP filter (plus energy smoothing)
   */
  /* compute Ai (Aq) and correction scale factor (gain) for each subframe */
  prm_other = param_other;		
  prm_in_other = prm_other;		

  for (k=0; k<NB_DIV; k++) 
  {
    bfi = bad_frame[k];     
    mode = mod[k];          
    
    if (mode == 0 || (sub(mode,1) == 0))
    {
      nsf = 4;                          
      interpol_wind = interpol_frac4;   
    }
    else if (sub(mode, 2) == 0)
    {
      nsf = 8;                          
      interpol_wind = interpol_frac8;   
    }
    else
    {
      nsf = 16;                         
      interpol_wind = interpol_frac16;  
    }
  

    /* set pointer to parameters */
    prm = param + (k*NPRM_BWE_DIV);    
    
    if(sub(mono_dec_stereo,1) == 0)
    {
      prm_other = param_other + (k*NPRM_BWE_DIV);    /*ptr*/
    }
    
    if (sub(mono_dec_stereo,1) == 0)
    {
      prm_in_other = prm_other;		  

      Lgain_sum = 0;                  move32();
      Lgain_sum_other = 0;            move32();
    }

    /* decode ISFs and convert ISFs to cosine domain */

    D_isf_hf(prm, isfnew, st->wpast_q_isf_hf, bfi, st->Mean_isf_hf, st->Dico1_isf_hf);
    if(mono_dec_stereo == 0)
    {
      voAMRWBPDecIsf_isp(isfnew, ispnew, MHF);
      /* interpolate Ai in ISP domain (Aq) */
      voAMRWBPDecIntLpc(st->wispold_hf, ispnew, interpol_wind, Aq, nsf, MHF);
      voAMRWBPDecCopy(ispnew, st->wispold_hf, MHF);

      /* compute gain correction factor to match amplitude at 6.4kHz */
      gain16 = Match_gain_6k4(&AqLF[((k*4)+nsf)*(M+1)], &Aq[nsf*(MHF+1)]);
      /* interpolate per subframe */
      Int_gain(st->wold_gain, gain16, interpol_wind, gain_hf, nsf);
      st->wold_gain = gain16;     
    }
 
    /* decode 4 gains */
    prm += D_gain_chan(gain_hf, &(st->wpast_q_gain_hf),prm, bfi, mode, &bad_frame[k], &Lgain_sum, mono_dec_stereo ); 
    
    if(sub(mono_dec_stereo,1) == 0)
    {
      /* Need to decode the gain for the other channel also*/
      /* decode 4 gains */
      prm_other += D_gain_chan(gain_hf_other, &(st->wpast_q_gain_hf_other), prm_other, bfi, mode, &bad_frame[k], &Lgain_sum_other, mono_dec_stereo); 
    
      /* Decode the other channel's parameters in case of mono decoding when stereo*/
      /* decode ISFs and convert ISFs to cosine domain */
      thr_lo = extract_l(L_mult(-10,nsf));
      thr_hi = negate(thr_lo);

      /* Difference in average gain offsets (in dB)*/
      gdiff = L_sub(Lgain_sum , Lgain_sum_other);

      D_isf_hf(prm_in_other, isfnew_other, st->wpast_q_isf_hf_other, bfi, st->Mean_isf_hf, st->Dico1_isf_hf);
      /* Take the average between the both channels' isp's*/
      
      if(L_sub(gdiff, L_shl(thr_lo,8)) < 0)
      {
        for (i = 0; i < MHF; i++)
        {
          isfnew[i] = isfnew_other[i];        
        }
      }
      else  if(L_sub(gdiff, L_shl(thr_hi,8)) < 0)
      {
        for(i=0;i<MHF;i++)
        {
          Ltmp = L_mult(isfnew[i], 16384);
          Ltmp = L_mac(Ltmp, isfnew_other[i], 16384);
          isfnew[i] = round16(Ltmp);            
        }
      }

      voAMRWBPDecIsf_isp(isfnew, ispnew, MHF);
      
      /* interpolate Ai in ISP domain (Aq) */
      voAMRWBPDecIntLpc(st->wispold_hf, ispnew, interpol_wind, Aq, nsf, MHF);
      voAMRWBPDecCopy(ispnew, st->wispold_hf, MHF);
      /* compute gain correction factor to match amplitude at 6.4kHz */
      gain16 = Match_gain_6k4(&AqLF[((k*4)+nsf)*(M+1)], &Aq[nsf*(MHF+1)]);
      /* interpolate per subframe */
      Int_gain(st->wold_gain, gain16, interpol_wind, gain_hf_tmp, nsf);
      for(i=0;i<nsf;i++)
      {
        gain_hf[i] = add(gain_hf[i], gain_hf_tmp[i]);               
        gain_hf_other[i] = add(gain_hf_other[i], gain_hf_tmp[i]);   
      }
      st->wold_gain = gain16;     
    }

    
    /*
     * AriL: We could also select overall gain based on channel balance, but since
     *       the low-band remains as average of both channels it could make the
     *       hihg-band unnecessarily loud in case of silent other channel
     *
     */


    /*Store latest LPC filter coeffs for possible WB+ -> WB switching*/
    voAMRWBPDecCopy(&Aq[nsf*(MHF+1)],mem_lpc_hf, MHF+1); 
    
    p_Aq = Aq;
    for (sf=0; sf<nsf; sf++) 
    {
      i_subfr = shl(add(shl(k,2),sf),6);
        
      /*fgain = (float)pow(10.0, fgain_hf[sf]/20.0);*/


      Ltmp = L_mult(gain_hf[sf], 21771);              /*0.166096 in Q17 -> Q25        */
      Ltmp = L_shr(Ltmp, 10);                         /* From Q28 to Q16              */
      voAMRWBPDecL_Extract(Ltmp, &exp_g, &frac_g);               /* Extract exponant of wgain_hf */
      tmp16 = extract_l(voAMRWBPDecPow2(11, frac_g));            /* Put 11 as exponant so that   */
                                                      /* output of voAMRWBPDecPow2() will be:    */
                                                      /* 16384 < voAMRWBPDecPow2() <= 32767      */
      gain16 = shl(tmp16, exp_g);                     /* gain16 in Q11 */  
      
      if(sub(mono_dec_stereo,1) == 0)
      {
        /* Average the gain between the two channels*/
        /*fgain += (float)pow(10.0, fgain_hf_other[sf]/20.0);*/
        Ltmp = L_mult(gain_hf_other[sf], 21771);              /*0.166096 in Q17 -> Q25        */
        Ltmp = L_shr(Ltmp, 10);                         /* From Q28 to Q16              */
        voAMRWBPDecL_Extract(Ltmp, &exp_g, &frac_g);               /* Extract exponant of wgain_hf */
        tmp16 = extract_l(voAMRWBPDecPow2(11, frac_g));            /* Put 11 as exponant so that   */
                                                        /* output of voAMRWBPDecPow2() will be:    */
                                                        /* 16384 < voAMRWBPDecPow2() <= 32767      */
        tmp16 = shl(tmp16, exp_g);                      /* gain16 in Q11 */  
        /*fgain *= 0.5;*/                                        
        Ltmp = L_mult(tmp16, 16384);
        Ltmp = L_mac(Ltmp, gain16, 16384);
          
        gain16 = round16(Ltmp);
      }

      /* Scale the gain towards full value gradually after a switch from AMR-WB mode*/
        
      if (sub(*ramp_state,64) < 0)
      {
        gain16 = mult_r(gain16, Gain_hf_ramp[*ramp_state]);            
        *ramp_state = add(*ramp_state, 1);    
      }

      /* Memorize the latest gain for possible WB+ -> WB switching*/
      *mem_gain_hf = gain16;      
      for (i=0; i<L_SUBFR; i++) 
      {
        /*fHF[i] = fgain*fexc[i+i_subfr];*/
        Ltmp = L_mult(gain16, exc[i+i_subfr]);
        HF[i] = round16(L_shl(Ltmp,15-11));       
      }
      
      Soft_exc_hf(HF, &(st->Lp_amp));

      /* filtering through HF LP filter */
      voAMRWBPDecSyn_filt_s(sub(QexcHF,st->Q_synHF), p_Aq, MHF, HF, HF, L_SUBFR, st->wmem_syn_hf, 1);
      /* smooth energy of HF signal on subframe (HF[]) */
      Smooth_ener_hf(HF, &(st->Lthreshold));
      for (i=0; i<L_SUBFR; i++) 
      {
        synth_hf[i+i_subfr] = HF[i];      
      }

      p_Aq += (MHF+1);
    }

    
    if (sub(mode,2) == 0)
    {
      k = add(k,1);
    }
    else if (sub(mode,3) == 0)
    {
      k =add(k,3);
    }
  }

  return;
#endif
}

static Word16 D_gain_chan(
  Word16 * gain_hf,           /* (i/o): gain hf                       Q8   */
  Word16 *mem_gain,           /* (i/o): gain hf mem                   Q8   */  
  Word16 *prm,                /* (i)  : parameters                         */ 
  Word16 bfi,                 /* (i)  : Bad frame indicator                */
  Word16 mode,                /* (i)  : Mode type                          */
  Word16 *bad_frame,          /* (i)  : for each frame (bad_frame[4])      */
  Word32 *Lgain,              /* (i)  : Gain sum                      Q8   */
  Word16 mono_dec_stereo      /* (i)  : 1=Mono decoding with stereo bitstream */
)
{

  Word16 index, gain4[4], prm_adv, *pt_prm, tmp16, i;

  pt_prm = prm;  


  D_gain_hf(pt_prm[2], gain4, mem_gain, bfi);

  pt_prm += 3;

  /* decode gain correction offsets (in dB) for each subframe */
  
  if (sub(mode,3) == 0)
  {
    /* 80+10-ms TCX */
    /* decode gain offset: -10.5, -7.5, -4.5, -1.5, +1.5, +4.5, +7.5, +10.5 dB */
    prm_adv = 3+16;   
    for (i=0; i<16; i++)
    {
      index = *pt_prm++;     
      
      /*ftmp = (3.0f*((float)index)) - 10.5f;*/
      tmp16 = sub(shl(add(shl(index,1),index),8), 2688);
      
      
      if (sub(bfi,1) == 0)
      {
        tmp16 = 0;      
      }
      
      if ((sub(bad_frame[1],1)==0) && (sub(i,8) < 0)) 
      {
        tmp16 = 0;      
      }
      
      if ((sub(bad_frame[2],1) == 0) && (sub(i,8)>=0)) 
      {
        tmp16 = 0;        
      }
      
      if (sub(mono_dec_stereo,1)==0)
      {
        gain_hf[i] = add(tmp16, gain4[shr(i,2)]);        
        *Lgain = L_add(gain_hf[i], *Lgain);              move32();
      }
      else
      {
        gain_hf[i] = add(add(tmp16,gain_hf[i]), gain4[shr(i,2)]);      
      }
    }
  } 
  else if (sub(mode,2) == 0) 
  {
    /* 40+5-ms TCX */
    /* decode gain offset: -4.5, -1.5, +1.5, +4.5 dB */
    prm_adv = 3+8;      
    for (i=0; i<8; i++) 
    {
      index = *pt_prm++;     

      /*ftmp = (3.0f*((float)index)) - 4.5f;*/
      tmp16 = sub(shl(add(shl(index,1),index),8), 1152);

      
      if ((sub(bfi,1)==0) || (sub(bad_frame[1],1) == 0)) 
      {
        tmp16 = 0;        
      }
      
      if(sub(mono_dec_stereo, 1) == 0)
      {
        gain_hf[i] = add(tmp16, gain4[shr(i,1)]);      
        *Lgain = L_add(gain_hf[i], *Lgain);              move32();
      }
      else
      {
        gain_hf[i] = add(add(tmp16,gain_hf[i]), gain4[shr(i,1)]);       
      }
    }
  } 
  else
  {
    /* ACELP or 20+2.5-ms TCX */
    prm_adv = 3;      
    
    if(sub(mono_dec_stereo,1) == 0)
    {
      for (i=0; i<4; i++)
      {
        gain_hf[i] = gain4[i];      
        *Lgain = L_add(gain_hf[i], *Lgain);     move32();    
      }

    }
    else
    {
      for (i=0; i<4; i++)
      {
        gain_hf[i] = add(gain_hf[i], gain4[i]);      
      }
    }
  }

  return prm_adv;
}
