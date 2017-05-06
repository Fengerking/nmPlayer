#include "amr_plus_fx.h"
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "oper_32b.h"
#include "log2.h"


/*----------- CODE AND SYNTHESIZE UPPER-BAND SIGNAL (synth_hf[])  ----------
    bitstream pointer = ptr

  coding modes (coder type=ACELP,TCX and frame length=20,40,80 ms) for upper-band are copied from lower-band
  HF coding mode depends on bit allocation to HFs (nbits_hf) :
  nbits_hf = 4*16=64
       the lower-band excitation is randomized, folded, and shaped first in time-domain
       with subframe gains then in frequency domain with a LP filter (plus energy smoothing)
  nbits_hf = 4*16+640=704
       the lower-band is quantized by ACELP/TCX model, using the same mode as lower band.
       
*/

 /*------------------------------------------------------*
  * HF analysis, quantization & synthesis                *
  *------------------------------------------------------*/
void Init_coder_hf(Coder_StState_fx *st)
{
  /* Static vectors to zero */
  Set_zero(st->mem_hf1, MHF);
  Set_zero(st->mem_hf2, MHF);
  Set_zero(st->mem_hf3, MHF);
  Set_zero(st->past_q_isf_hf, MHF);
  Set_zero(st->mem_lev_hf, 18);
  st->old_gain = 256;                   move16();

  /* isp initialization */
  Copy(Isp_init_HF, st->ispold_hf, MHF);
  Copy(st->ispold_hf, st->ispold_q_hf, MHF);

  return;
}



void Coder_hf(
  Word16 mod[],         /* (i) : mode for each 20ms frame (mode[4]      */
  Word16 AqLF[],        /* (i) : Q coeff in lower band (AdLF[16])       */ 
  Word16 speech[],      /* (i) : speech vector [-M..L_FRAME_PLUS]           */ 
  Word16 speech_hf[],   /* (i) : HF speech vec [-MHF..L_FRAME_PLUS+L_NEXT]  */ 
  Word16 window[],      /* (i) : window for LPC analysis                */
  Word16 param[],       /* (o) : parameters (NB_DIV*NPRM_HF_DIV)        */
  Word16 fscale,        /* (i) : Internal frequency scaling             */
  Coder_StState_fx *st, /* i/o : coder memory state                     */
  Word16 Q_new,         /* (i) : Speech scaling                         */
  Word16 Q_speech       /* (i) : Weighed speech scaling                 */
) 
{

  Word16 *prm;
  /* LPC coefficients */
  Word16 r_l[MHF+1], r_h[MHF+1];                 /* Autocorrelations of windowed speech  */
  Word16 A[(NB_SUBFR+1)*(MHF+1)];
  Word16 Aq[(NB_SUBFR+1)*(MHF+1)];
  Word16 *p_A, *p_Aq, Ap[MHF+1];

  Word16 ispnew[NB_DIV*MHF];                /* LSPs at 4nd subframe                 */
  Word16 ispnew_q[MHF];                     /* LSPs at 4nd subframe                 */
  Word16 isfnew[MHF];

  Word16 gain_hf[NB_SUBFR], gain4_[4];
  Word16 HF[L_SUBFR];
  Word16 buf[MHF+L_SUBFR];
  Word16 rc[M+2], gain, m_tmp, e_tmp, tmp16;
  const Word16 *interpol_wind;

  /* Scalars */
  Word16 i, j, k, i_subfr, sf, nsf, index, mode;
  Word32 Ltmp, Lener;

 /*---------------------------------------------------------------*
  *  Perform HF LP analysis four times (every 20 ms)              *
  *  - autocorrelation + lag windowing                            *
  *  - Levinson-Durbin algorithm to find a[]                      *
  *  - convert a[] to isp[]                                       *
  *  - interpol isp[]                                             *
  *---------------------------------------------------------------*/
  for (k=0; k<NB_DIV; k++)
  {
    test();
    if(sub(fscale, FSCALE_DENOM) <= 0) 
    {
     /* Autocorrelations of HF signal folded into 12.8kHz */
      Autocorr(&speech_hf[(k*L_DIV)+L_SUBFR], MHF, r_h, r_l, L_WINDOW, window);	  
    }
    else 
    {
     /* Autocorrelations of HF signal folded into 12.8kHz */
      Autocorr(&speech_hf[(k*L_DIV)+(L_SUBFR/2)], MHF, r_h, r_l, L_WINDOW_HIGH_RATE, window);	  
    }
    
    Lag_window(r_h, r_l, MHF);
    
    Levinson(r_h, r_l,Ap, rc, st->mem_lev_hf, MHF);
    Az_isp(Ap, &ispnew[k*MHF], st->ispold_hf, MHF);
    Int_lpc(st->ispold_hf, &ispnew[k*MHF], interpol_frac4, &A[k*4*(MHF+1)], 4, MHF);
    Copy(&ispnew[k*MHF], st->ispold_hf, MHF);
  }

  p_A = A;      move16();

  for (k=0; k<NB_DIV; k++)
  {
    mode = mod[k];        move16();
    test();test();test();
    if ((mode == 0) || (sub(mode,1) == 0)) 
    {
      nsf = 4;    move16();
      interpol_wind = interpol_frac4;

    } else if (sub(mode, 2) == 0)
    {
      nsf = 8;    move16();
      interpol_wind = interpol_frac8;
    }
    else
    {
      nsf = 16;    move16();
      interpol_wind = interpol_frac16;
    }

    /* set pointer to parameters */
    prm = param + (k*NPRM_BWE_DIV);         move16();

    /* Convert isps to frequency domain 0..6400 */
    Isp_isf(&ispnew[(k+sub(shr(nsf,2),1))*MHF], isfnew, MHF);

    /* quantize isf */
    Q_isf_hf(isfnew, isfnew, st->past_q_isf_hf, prm, st->mean_isf_hf, st->dico1_isf_hf);

    prm += 2;

    /* Convert isfs to the cosine domain */
    Isf_isp(isfnew, ispnew_q, MHF);

    /* interpol quantized isp for local synthesis */
    Int_lpc(st->ispold_q_hf, ispnew_q, interpol_wind, Aq, nsf, MHF);


    Copy(ispnew_q, st->ispold_q_hf, MHF);
    /* gain factor to match amplitude at 6.4kHz */
    gain = Match_gain_6k4(&AqLF[((k*4)+nsf)*(M+1)], &Aq[nsf*(MHF+1)]);

    Int_gain(st->old_gain, gain, interpol_wind, gain_hf, nsf);

    st->old_gain = gain;            move16();

   /*------------------------------------------------------*
    * find gain in weighted domain                         *
    *------------------------------------------------------*/
    
    p_Aq = Aq;                      move16();
    for (sf=0; sf<nsf; sf++)
    {
      /*i_subfr = ((k*4)+sf)*L_SUBFR;*/
      i_subfr = shl(add(shl(k,2),sf),6);
      Residu2(&AqLF[((k*4)+sf)*(M+1)], M, &speech[i_subfr], buf + MHF, L_SUBFR);  /*Q_speech */
     /* Residu may sature in special case here */
      Copy(st->mem_hf1, buf, MHF);
      Syn_filt_s(3, p_Aq, MHF, buf+MHF, buf+MHF, L_SUBFR, st->mem_hf1, 1);      /*Q_speech - 3*/
      Weight_a(p_A, Ap, GAMMA_HF_FX, MHF);

      Residu(p_A, MHF, buf + MHF, HF,  L_SUBFR);                     /* Q_speech -2*/
      Syn_filt_s(1, Ap, MHF, HF, HF, L_SUBFR, st->mem_hf2, 1);       /* Q_speech -3   */

      Residu2(p_A, MHF, &speech_hf[i_subfr], buf, L_SUBFR);        /* Q_sp_hf + Q_new  */
      Syn_filt_s(3, Ap, MHF, buf, buf, L_SUBFR, st->mem_hf3, 1);       /* Q_sp_hf + Q_new - 3 */
      Ltmp = 0;     move32();
      Lener = 0;    move32();
      for (i=0; i<L_SUBFR; i++) 
      { 
        Lener = L_mac(Lener, buf[i], buf[i]);
        Ltmp = L_mac(Ltmp, HF[i], HF[i]);
     }

      test();
      if(Ltmp == 0)
      {
        Ltmp = 1;     move32();
      }
      test();
      if(Lener== 0)
      {
        Lener= 1;     move32();
      }
      /*gain = 10.0f * (float)log10(ener/tmp);*/

      Log2(Lener,&e_tmp,&m_tmp);
      e_tmp = sub(e_tmp, shl(add(st->Q_sp_hf, Q_new),1));
      Lener = Mpy_32_16(e_tmp, m_tmp, LG10);  

      Log2(Ltmp,&e_tmp,&m_tmp);
      e_tmp = sub(e_tmp, shl(Q_speech,1));
      Ltmp = Mpy_32_16(e_tmp, m_tmp, LG10);  
      Lener = L_sub(Lener, Ltmp);   /*Q14*/

      gain = extract_h(L_shl(Lener, 10));          /* Q14 -> Q8 with saturation*/
      gain_hf[sf] = sub(gain,gain_hf[sf]);        move16();


      p_A += (MHF+1);
      p_Aq += (MHF+1);
    }


   /*------------------------------------------------------*
    * HF gains quantization                                *
    *------------------------------------------------------*/
    test();test();
    if (sub(mode,3) == 0)   /* TCX 80 */
    {
      for (i=0; i<4; i++)
      {
        Ltmp = 0;     move32();
        for (j=0; j<4; j++) 
        {
          Ltmp = L_mac(Ltmp, gain_hf[(4*i)+j], 8192);
        }
        gain4_[i] = round(Ltmp);     move16();
      }
      Q_gain_hf(gain4_, gain4_, prm);
      prm++;

      for (i=0; i<nsf; i++)
      {
        gain = gain4_[shr(i,2)];    move16();

        /* quantize gain at: -10.5, -7.5, -4.5, -1.5, +1.5, +4.5, +7.5, +10.5 dB */
        /*index = (int)floor(((10.5 + tmp) / 3.0) + 0.5);*/
        tmp16 = sub(gain_hf[i],gain);
        Ltmp  = L_mac(8388608, add(2688,tmp16), 10923);
        index = extract_h(L_shr(Ltmp,8));

        test();
        if (sub(index,7) > 0) 
        {
          index = 7;          move16();
        }
        test();
        if (index < 0) 
        {
          index = 0;        move16();
        }
        prm[i] = index;     move16();

        /*gain_hf[i] = gain + (3.0f*((float)index)) - 10.5f;*/
        gain_hf[i] = sub(add(gain, add(index, shl(index,1))),2688);     move16();

      }
      prm += nsf;
    }        
    else if (sub(mode,2) == 0)   /* TCX 40 */
    {

      for (i=0; i<4; i++)
      {
        Ltmp = 0;   move32();
        for (j=0; j<2; j++) 
        {
          Ltmp = L_mac(Ltmp, gain_hf[(2*i)+j], 16384);;
        }
        gain4_[i] = round(Ltmp);     move16();;
      }

      Q_gain_hf(gain4_, gain4_, prm);
      prm++;
      for (i=0; i<nsf; i++)
      {
        gain = gain4_[shr(i,1)];

        /* quantize gain at: -4.5, -1.5, +1.5, +4.5 dB */
        /*index = (int)floor(((4.5 + tmp) / 3.0) + 0.5);*/
        tmp16 = sub(gain_hf[i], gain);
        Ltmp  = L_mac(8388608, add(1152,tmp16), 10923);
        index = extract_h(L_shr(Ltmp,8));
        test();
        if (sub(index,3) > 0) 
        {
          index = 3;      move16();
        }
        test();
        if (index < 0) 
        {
          index = 0;        move16();
        }
        prm[i] = index;     move16();

        /*gain_hf[i] = gain + (3.0f*((float)index)) - 4.5f;*/
        gain_hf[i] = sub(add(gain, add(index, shl(index,1))),1152);       move16();
      }
      prm += nsf;
    }        
    else     /* ACELP/TCX 20 */
    {
      Q_gain_hf(gain_hf, gain_hf,prm);
      prm++;
    }
    test();test();
    if (sub(mode,2) == 0) 
    {
      k = add(k, 1);
    } 
    else if (sub(mode,3) == 0) 
    {
      k = add(k,3);
    }
  }
  return;
}
