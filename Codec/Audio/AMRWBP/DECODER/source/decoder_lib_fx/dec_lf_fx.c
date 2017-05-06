#include "typedef.h"
#include "basic_op.h"
#include "count.h"


#include "amr_plus_fx.h"
//#include "stdlib.h"


/*-----------------------------------------------------------------*
 * Funtion  init_decoder_lf                                        *
 * ~~~~~~~~~~~~~~~~~~~~~~~~                                        *
 *                                                                 *
 *   ->Initialization of variables for the decoder section.        *
 *                                                                 *
 *-----------------------------------------------------------------*/

/* isp tables for initialization */

void Init_decoder_lf(Decoder_State_Plus_fx *st)
{
  Word32   i;
  /* Static vectors to zero */
  Set_zero(st->wold_exc, PIT_MAX_MAX+L_INTERPOL);

  Set_zero(st->wold_synth, M);
  Set_zero(st->cp_old_synth, M);


  Set_zero(st->wpast_isfq, M);
  Set_zero(st->wwovlp, L_OVLP);

  /* Initialize the ISFs */
  voAMRWBPDecCopy(Isf_init, st->wisfold, M);

  for (i = 0; i < L_MEANBUF; i++)
  {
    voAMRWBPDecCopy(Isf_init, &(st->wisf_buf[i*M]), M);
  }
  voAMRWBPDecCopy(Isp_init, st->wispold, M);
       

  /* scaling memories for excitation */
  st->mem_subfr_q[6] = Q_MAX;  
  st->mem_subfr_q[5] = Q_MAX;  
  st->mem_subfr_q[4] = Q_MAX;  
  st->mem_subfr_q[3] = Q_MAX;  
  st->mem_subfr_q[2] = Q_MAX;  
  st->mem_subfr_q[1] = Q_MAX;  
  st->mem_subfr_q[0] = Q_MAX;  
  
  Set_zero(st->old_subfr_q, 4*NB_DIV);
  Set_zero(st->old_syn_q, 4*NB_DIV);
  
  /* Initialize the ISPs */
  st->wold_T0 = 64;             
  st->wold_T0_frac = 0;         
  st->prev_lpc_lost = 0;        
  st->seed_ace = 0;             
  st->seed_tcx = 21845;         
  st->pitch_tcx = L_DIV;         

  st->wmem_wsyn = 0;            
  st->wwsyn_rms = 0;            
  st->wpast_gpit = 0;           
  st->Lpast_gcode = 0L;         move32();

  st->Q_exc = 0;                
  st->Q_syn = -1;               
  st->Old_Q_syn = -1;           


  st->prev_Q_syn = 6;           
  st->Old_Q_exc = Q_MAX;        
  st->Old_Qxri = 6;             
  st->Old_Qxnq = Q_MAX;         
  st->last_mode = 0;                   /* set last mode to acelp */
  st->Old_QxnqMax = Q_MAX;      
    
  st->ovlp_size = 0;            
  st->wold_T0 = 64;             
  st->wold_T0_frac = 0;         
  st->i_offset = 0;             

  Set_zero(st->mem_syn2,M);
  
  return;
}



/*-----------------------------------------------------------------*
 * Funtion decoder_lf                                              *
 * ~~~~~~~~~~~~~~~~~~                                              *
 *   ->Principle decoder routine (band 0 to 6400Hz).               *
 *                                                                 *
 *-----------------------------------------------------------------*/
void Decoder_lf(
  Word16 mod[],         /* (i)  : mode for each 20ms frame (mode[4]     */
  Word16 param[],       /* (i)  : parameters                            */
  Word16 nbits_AVQ[],   /* (i)  : for each frame (nbits_AVQ[4])         */
  Word16 codec_mode,    /* (i)  : AMR-WB+ mode (see cnst.h)             */ 
  Word16 bad_frame[],   /* (i)  : for each frame (bad_frame[4])         */
  Word16 AzLF[],        /* (o)  : decoded coefficients (AdLF[16])       */
  Word16 exc_[],        /* (o)  : decoded excitation                    */
  Word16 syn[],         /* (o)  : decoded synthesis                     */
  Word16 pitch[],       /* (o)  : decoded pitch (pitch[16])             */
  Word16 pit_gain[],    /* (o)  : decoded pitch gain (pit_gain[16])     */
  Word16 pit_adj,
  Decoder_State_Plus_fx *st /* (i/o) : coder memory state               */
)                       
{
#if (FUNC_DECODER_LF_OPT)

  Word16 i, k, mode, bfi, bfi_2nd_st, *prm, tmp, tmp16;
  Word16 ispnew_[M], isfnew_[M], Az[(NB_SUBFR+1)*(M+1)];
  Word16 len;
  Word16 *synth_;
  Word16 stab_fac;
  Word32 Ltmp;
  
  //Word16 *temp_p_1, *temp_p_2;
  //Word16 *temp_p_3, *temp_p_4;

  if(pit_adj == 0) 
  {
    len = L_DIV;
  }
  else 
  {
    
   /*i = (((pit_adj*PIT_MIN_12k8)+(FSCALE_DENOM/2))/FSCALE_DENOM)-PIT_MIN_12k8;*/

    //Ltmp = L_mac(FSCALE_DENOM, pit_adj, PIT_MIN_12k8);
    Ltmp = FSCALE_DENOM + ((pit_adj * PIT_MIN_12k8) << 1);
  
    //i = div_s(extract_h(L_shl(Ltmp, k)), shl(FSCALE_DENOM, k));
    i = div_s((Ltmp >> (16 - FSCALE_DENOM_NORMS)), (FSCALE_DENOM << FSCALE_DENOM_NORMS));
  
    tmp16 = i + 1;
    //Ltmp = L_msu(Ltmp, tmp16, FSCALE_DENOM);
    Ltmp -= (tmp16 * FSCALE_DENOM) << 1;
  
    //i = sub(i, PIT_MIN_12k8);
    i -= PIT_MIN_12k8;
    if (Ltmp >= 0)
    {
      //i = sub(tmp16,PIT_MIN_12k8);
      i = tmp16 - PIT_MIN_12k8;
    } 

    st->i_offset = i;

    //len = add(shr(extract_l(L_mult(6,i)),1),PIT_MAX_12k8+L_INTERPOL);    
    len = (extract_l((6 * i) << 1) >> 1) + PIT_MAX_12k8 + L_INTERPOL;
    len = (len < L_DIV) ? L_DIV : len;
  }

  /* initialize bass postfilter */
  for (i=0; i<NB_SUBFR; i++) 
  {
    pitch[i] = L_SUBFR;
    pit_gain[i] = 0;
  }

  /* Initialize pointers */
  synth_ = syn;

  voAMRWBPDecCopy(st->wold_synth, syn-M, M);
//  temp_p_1 = st->wold_synth;
//  temp_p_2 = syn-M;
//  i = M-1;
//  while (i >= 0)
//  {
//    *(temp_p_2 + i) = *(temp_p_1 + i);
//    i--;
//  }

  k = 0;

  while (k < NB_DIV)
  {
    mode = mod[k];

    bfi = bad_frame[k];

    bfi_2nd_st = 0;
    if ((2 == mode) && (bad_frame[k+1] != 0))
    {
      bfi_2nd_st = 31;//(1+2+4+8+16);
    }
    if (mode == 3)
    {
      if (bad_frame[k+1] != 0) 
      {
        bfi_2nd_st = 1;
      }
      if (bad_frame[k+2] != 0) 
      {
        bfi_2nd_st += 10;
      }
      if (bad_frame[k+3] != 0) 
      {
        bfi_2nd_st += 20;
      }
    }
    /* set pointer to parameters */
    prm = param + (k * DEC_NPRM_DIV);

    /* decode ISFs and convert ISFs to cosine domain */
    Dpisf_2s_46b_2(prm, isfnew_,st->wpast_isfq, st->wisfold, st->wisf_buf, bfi, bfi_2nd_st, 1);

    if(bfi == 0 && bfi_2nd_st == 0)
    {
        //st->seed_tcx = extract_l(L_shl(L_mac(L_mult(prm[0],prm[1]), prm[4],prm[5]),7));  
        st->seed_tcx = extract_l((((prm[0] * prm[1]) << 1) + ((prm[4] * prm[5]) << 1)) << 7);
    }

    prm += 7;

    voAMRWBPDecIsf_isp(isfnew_, ispnew_, M);

    /* Check stability on isf : distance between old isf and current isf */

    Ltmp = 0;
    for (i = 0; i < M - 1; i++)
    {
      tmp = isfnew_[i] - st->wisfold[i];
      Ltmp += (tmp * tmp) << 1;
    }
    tmp = extract_h(L_shl(Ltmp, 8));
    tmp = mult(tmp, 26214);                /* tmp = Ltmp*0.8/256 */

    //tmp = sub(20480, tmp);                 /* 1.25 - tmp */
    tmp = 20480 - tmp;                 /* 1.25 - tmp */
    stab_fac = shl(tmp, 1);                /* Q14 -> Q15 with saturation */
    if (stab_fac < 0)
    {
      stab_fac = 0;                      
    }

    /* ajust old isp[] following to a bad frame (to avoid overshoot) */
    if ((st->prev_lpc_lost != 0) && (bfi == 0)) 
    {
      for (i=M-2; i>=0; i--)
      {
        st->wispold[i] = (ispnew_[i] < st->wispold[i]) ? ispnew_[i] : st->wispold[i];
      }
    }
    /* - interpolate Ai in ISP domain (Aq) and save values for upper-band (Aq_lpc)
       - decode other parameters according to mode
       - set overlap size for next decoded frame (ovlp_size)
       - set mode for upper-band decoder (mod[])*/

    switch (mode) 
    {
 
      case 0:
      case 1:
      voAMRWBPDecIntLpc(st->wispold, ispnew_, interpol_frac4, Az, 4, M);
      voAMRWBPDecCopy(Az, &AzLF[(k*4)*(M+1)], 5*(M+1));
      //memcpy(&AzLF[(k*4)*(M+1)], Az, 5*(M+1) * sizeof(Word16));
//      temp_p_1 = Az;
//      temp_p_2 = AzLF + k * 4 * (M + 1);
//      i = 5*(M+1);
//      while (i-- > 0)
//      {
//        *(temp_p_2 + i) = *(temp_p_1 + i);
//      }
      
      if (mode == 0) /* 20-ms ACELP */
      { 
        /* set global variables used in dec_ace() even for bfi=1*/
        Decoder_acelp(prm, L_DIV, codec_mode, bad_frame[k],&pitch[k*4], 
                      &pit_gain[k*4], Az, &exc_[k*L_DIV], &synth_[k*L_DIV],pit_adj, len, stab_fac,st);


        /* average integer pitch-lag for high band coding */
        st->last_mode = 0;
        voAMRWBPDecCopy(&exc_[k*L_DIV],&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], L_DIV);
        //memcpy(&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], &exc_[k*L_DIV], L_DIV * sizeof(Word16));
//        temp_p_1 = exc_ + k * L_DIV;
//        temp_p_2 = exc_ + k * L_DIV - PIT_MAX_MAX - L_INTERPOL;
//        i = L_DIV;
//        while (i-- > 0)
//        {
//            *(temp_p_2 + i) = *(temp_p_1 + i);
//        }
      }
      else
      {
        /* 20+2.5-ms TCX */
        tmp16 = st->Q_exc;
        Decoder_tcx(prm, &nbits_AVQ[k], Az, L_DIV, bad_frame+k,
                    &exc_[k*L_DIV], &synth_[k*L_DIV], st);
        st->last_mode =1;

        voAMRWBPDecCopy(&exc_[k*L_DIV],&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], L_DIV);
        //memcpy(&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], &exc_[k*L_DIV], L_DIV * sizeof(Word16));
//        temp_p_1 = exc_ + k * L_DIV;
//        temp_p_2 = exc_ + k * L_DIV - PIT_MAX_MAX - L_INTERPOL;
//        i = L_DIV;
//        while (i-- > 0)
//        {
//            *(temp_p_2 + i) = *(temp_p_1 + i);
//        }
        /* update scaling on PIT_MAX_MAX - PIT_MAX samples (need in predlt4) */
        voAMRWBPDecScale_sig(&exc_[k*L_DIV+L_DIV-(PIT_MAX_MAX + L_INTERPOL)], (PIT_MAX_MAX + L_INTERPOL)-L_DIV, sub(st->Q_exc, tmp16));  /* Need to have an uniforme scaling over PIT_MAX_MAX */
      }
      Updt_mem_q(st->old_syn_q, 1, st->Q_syn);  
      Updt_mem_q(st->old_subfr_q, 1, st->Q_exc);

      k++;  /*ptr*/
      break;

    case 2: /* 40+5-ms TCX */

      voAMRWBPDecIntLpc(st->wispold, ispnew_, interpol_frac8, Az, 8, M);
      voAMRWBPDecCopy(Az, &AzLF[(k*4)*(M+1)], 9*(M+1));
      //memcpy(&AzLF[(k*4)*(M+1)], Az, 9*(M+1) * sizeof(Word16));
//      temp_p_1 = Az;
//      temp_p_2 = AzLF + k * 4 * (M + 1);
//      i = 9 * (M + 1);
//      while (i-- > 0)
//      {
//          *(temp_p_2 + i) = *(temp_p_1 + i);
//      }

      Decoder_tcx(prm, &nbits_AVQ[k], Az, 2*L_DIV, bad_frame+k,
                    &exc_[k*L_DIV], &synth_[k*L_DIV], st);
      st->last_mode =2;                 
      /* Update last subfr scaling, take in account the frame length*/
      Updt_mem_q(st->old_subfr_q, 2, st->Q_exc);
      Updt_mem_q(st->old_syn_q, 2, st->Q_syn);
      voAMRWBPDecCopy(&exc_[k*L_DIV],&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], 2*L_DIV);
      //memcpy(&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], &exc_[k*L_DIV], 2*L_DIV*sizeof(Word16));
//      temp_p_1 = exc_ + k * L_DIV;
//      temp_p_2 = exc_ + k * L_DIV - PIT_MAX_MAX - L_INTERPOL;
//      i = 0;
//      while (i < (L_DIV << 1))
//      {
//          *(temp_p_2 + i) = *(temp_p_1 + i);
//          i++;
//      }

      k+=2;         /*ptr*/
      break;

    case 3: /* 80+10-ms TCX */
      voAMRWBPDecIntLpc(st->wispold, ispnew_, interpol_frac16, Az, 16, M);
      voAMRWBPDecCopy(Az, &AzLF[(k*4)*(M+1)], 17*(M+1));
      //memcpy(&AzLF[(k*4)*(M+1)], Az, 17*(M+1)*sizeof(Word16));
//      temp_p_1 = Az;
//      temp_p_2 = AzLF + k * 4 * (M + 1);
//      i = 17 * (M + 1);
//      while (i-- > 0)
//      {
//          *(temp_p_2 + i) = *(temp_p_1 + i);
//      }

      Decoder_tcx(prm, &nbits_AVQ[k], Az, 4*L_DIV, bad_frame+k,
                    &exc_[k*L_DIV], &synth_[k*L_DIV], st);
      st->last_mode =3;                    

      /* Update last subfr scaling, take in account the frame length*/
      Updt_mem_q(st->old_subfr_q, 4, st->Q_exc);
      Updt_mem_q(st->old_syn_q, 4, st->Q_syn);

      voAMRWBPDecCopy(&exc_[k*L_DIV],&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], 4*L_DIV);
      //memcpy(&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], &exc_[k*L_DIV], 4*L_DIV*sizeof(Word16));
//      temp_p_1 = exc_ + k * L_DIV;
//      temp_p_2 = exc_ + k * L_DIV - PIT_MAX_MAX - L_INTERPOL;
//      i = 0;
//      while (i < (L_DIV << 2))
//      {
//          *(temp_p_2 + i) = *(temp_p_1 + i);
//          i++;
//      }


      k+=4;       /*ptr*/
      break;

    default:
      //AMRWBPLUS_PRINT("decoder error: mode > 3!\n");
      //exit(0);
		break;
    }

    st->prev_lpc_lost = bfi;           

    /* update ispold[] and isfold[] for the next frame */
    voAMRWBPDecCopy(ispnew_, st->wispold, M);
    voAMRWBPDecCopy(isfnew_, st->wisfold, M);
    //memcpy(st->wispold, ispnew_, M*sizeof(Word16));
    //memcpy(st->wisfold, isfnew_, M*sizeof(Word16));
//    temp_p_1 = ispnew_;
//    temp_p_2 = st->wispold;
//    temp_p_3 = isfnew_;
//    temp_p_4 = st->wisfold;
//    i = M;
//    while (i-- > 0)
//    {
//      *(temp_p_2 + i) = *(temp_p_1 + i);
//      *(temp_p_4 + i) = *(temp_p_3 + i);
//    }
  }//end of while

  /*------ update signal for next superframe :
   shift exc[], synth[] and synth_hf[] to the left by L_FRAME_PLUS -----*/

  voAMRWBPDecCopy(&syn[L_FRAME_PLUS-M], st->wold_synth, M);
//  temp_p_1 = syn + L_FRAME_PLUS - M;
//  temp_p_2 = st->wold_synth;
//  i = M;
//  while (i-- > 0)
//  {
//      *(temp_p_2 + i) = *(temp_p_1 + i);
//  }

  /* output exc and synth */
  /* Use no memorie excitation */
  voAMRWBPDecCopy(synth_, syn, L_FRAME_PLUS);
//  temp_p_1 = synth_;
//  temp_p_2 = syn;
//  i = L_FRAME_PLUS;
//  while (i-- > 0)
//  {
//      *(temp_p_2 + i) = *(temp_p_1 + i);
//  }

  return;

#else

  Word16 i, k, mode, bfi, bfi_2nd_st, *prm, tmp, tmp16;
  Word16 ispnew_[M], isfnew_[M], Az[(NB_SUBFR+1)*(M+1)];
  Word16 len;
  Word16 *synth_;
  Word16 stab_fac;
  Word32 Ltmp;

  
  if(pit_adj == 0) 
  {
    len = L_DIV;      
  }
  else 
  {
    
   /*i = (((pit_adj*PIT_MIN_12k8)+(FSCALE_DENOM/2))/FSCALE_DENOM)-PIT_MIN_12k8;*/

    Ltmp = L_mac(FSCALE_DENOM, pit_adj, PIT_MIN_12k8);
  
    k = FSCALE_DENOM_NORMS;     
 
    i = div_s(extract_h(L_shl(Ltmp, k)), shl(FSCALE_DENOM, k));
  
    tmp16 = add(i, 1);    
    Ltmp = L_msu(Ltmp, tmp16, FSCALE_DENOM);
  
    i = sub(i, PIT_MIN_12k8);
    
    if (Ltmp >= 0)
    {
      i = sub(tmp16,PIT_MIN_12k8);     
    } 

    st->i_offset = i; 

    len = add(shr(extract_l(L_mult(6,i)),1),PIT_MAX_12k8+L_INTERPOL);    
    
    if (sub(len,L_DIV) < 0)
    {
      len = L_DIV;      
    }
  }

  /* initialize bass postfilter */
  for (i=0; i<NB_SUBFR; i++) 
  {
    pitch[i] = L_SUBFR;            
    pit_gain[i] = 0;               
  }

  /* Initialize pointers */
  synth_ = syn;         

  voAMRWBPDecCopy(st->wold_synth, syn-M, M); 


  k = 0;            
  
   /* initialize the pointer prm = param + (k*DEC_NPRM_DIV);  */
   /* initialize the counter k*(4*(M+1))  */
   /* initialize the counter k*L_DIV  */
   /* initialize the counter k*4  */

  while (sub(k,NB_DIV) < 0)
  {
      
    mode = mod[k];              

    bfi = bad_frame[k];          

    bfi_2nd_st = 0;             
    
    if ((sub(mode,2) == 0) && (bad_frame[k+1] != 0)) 
    {
      bfi_2nd_st = (1+2+4+8+16);        
    }
     
    if (sub(mode,3) == 0) 
    {    
      
      if (bad_frame[k+1] != 0) 
      {
        bfi_2nd_st = 1;          
      }
      
      if (bad_frame[k+2] != 0) 
      {
        bfi_2nd_st = add(bfi_2nd_st,(2+8));
      }
      
      if (bad_frame[k+3] != 0) 
      {
        bfi_2nd_st = add(bfi_2nd_st,(4+16));
      }
    }
    /* set pointer to parameters */
    prm = param + (k*DEC_NPRM_DIV); 


    /* decode ISFs and convert ISFs to cosine domain */
    Dpisf_2s_46b_2(prm, isfnew_,st->wpast_isfq, st->wisfold, st->wisf_buf, bfi, bfi_2nd_st, 1);

    
    if(bfi == 0 && bfi_2nd_st == 0)
    {
        st->seed_tcx = extract_l(L_shl(L_mac(L_mult(prm[0],prm[1]), prm[4],prm[5]),7));  
    }
    
    prm += 7;

    voAMRWBPDecIsf_isp(isfnew_, ispnew_, M);

    /* Check stability on isf : distance between old isf and current isf */

    Ltmp = 0;                             move32();
    for (i = 0; i < M - 1; i++)
    {
      tmp = sub(isfnew_[i], st->wisfold[i]);
      Ltmp = L_mac(Ltmp, tmp, tmp);
    }
    tmp = extract_h(L_shl(Ltmp, 8));
    tmp = mult(tmp, 26214);                /* tmp = Ltmp*0.8/256 */

    tmp = sub(20480, tmp);                 /* 1.25 - tmp */
    stab_fac = shl(tmp, 1);                /* Q14 -> Q15 with saturation */
    
    if (stab_fac < 0)
    {
      stab_fac = 0;                      
    }

    /* ajust old isp[] following to a bad frame (to avoid overshoot) */
    
    if ((st->prev_lpc_lost != 0) && (bfi == 0)) 
    {
      for (i=0; i<M-1; i++) 
      {
        
        if (sub(ispnew_[i],st->wispold[i]) < 0) 
        {
          st->wispold[i] = ispnew_[i];            
        }
      }
    }
    /* - interpolate Ai in ISP domain (Aq) and save values for upper-band (Aq_lpc)
       - decode other parameters according to mode
       - set overlap size for next decoded frame (ovlp_size)
       - set mode for upper-band decoder (mod[])*/

    
    switch (mode) 
    {
 
      case 0:
      case 1:

      voAMRWBPDecIntLpc(st->wispold, ispnew_, interpol_frac4, Az, 4, M);
      voAMRWBPDecCopy(Az, &AzLF[(k*4)*(M+1)], 5*(M+1));
        
      if (mode == 0) /* 20-ms ACELP */
      { 
        /* set global variables used in dec_ace() even for bfi=1*/
        Decoder_acelp(prm, L_DIV, codec_mode, bad_frame[k],&pitch[k*4], 
                      &pit_gain[k*4], Az, &exc_[k*L_DIV], &synth_[k*L_DIV],pit_adj, len, stab_fac,st);


        /* average integer pitch-lag for high band coding */
        st->last_mode = 0;        move16() ; 
        voAMRWBPDecCopy(&exc_[k*L_DIV],&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], L_DIV);	
      }
      else
      {
        /* 20+2.5-ms TCX */
        tmp16 = st->Q_exc;    
        Decoder_tcx(prm, &nbits_AVQ[k], Az, L_DIV, bad_frame+k,
                    &exc_[k*L_DIV], &synth_[k*L_DIV], st);
        st->last_mode =1;             

        voAMRWBPDecCopy(&exc_[k*L_DIV],&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], L_DIV);
        /* update scaling on PIT_MAX_MAX - PIT_MAX samples (need in predlt4) */
        voAMRWBPDecScale_sig(&exc_[k*L_DIV+L_DIV-(PIT_MAX_MAX + L_INTERPOL)], (PIT_MAX_MAX + L_INTERPOL)-L_DIV, sub(st->Q_exc, tmp16));  /* Need to have an uniforme scaling over PIT_MAX_MAX */
      }
      Updt_mem_q(st->old_syn_q, 1, st->Q_syn);  
      Updt_mem_q(st->old_subfr_q, 1, st->Q_exc);

      k++;  /*ptr*/
      break;

    case 2: /* 40+5-ms TCX */

      voAMRWBPDecIntLpc(st->wispold, ispnew_, interpol_frac8, Az, 8, M);
      voAMRWBPDecCopy(Az, &AzLF[(k*4)*(M+1)], 9*(M+1));

      Decoder_tcx(prm, &nbits_AVQ[k], Az, 2*L_DIV, bad_frame+k,
                    &exc_[k*L_DIV], &synth_[k*L_DIV], st);
      st->last_mode =2;                 
      /* Update last subfr scaling, take in account the frame length*/
      Updt_mem_q(st->old_subfr_q, 2, st->Q_exc);
      Updt_mem_q(st->old_syn_q, 2, st->Q_syn);
      voAMRWBPDecCopy(&exc_[k*L_DIV],&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], 2*L_DIV);

      k+=2;         /*ptr*/
      break;

    case 3: /* 80+10-ms TCX */
      voAMRWBPDecIntLpc(st->wispold, ispnew_, interpol_frac16, Az, 16, M);
      voAMRWBPDecCopy(Az, &AzLF[(k*4)*(M+1)], 17*(M+1));

      Decoder_tcx(prm, &nbits_AVQ[k], Az, 4*L_DIV, bad_frame+k,
                    &exc_[k*L_DIV], &synth_[k*L_DIV], st);
      st->last_mode =3;                    

      /* Update last subfr scaling, take in account the frame length*/
      Updt_mem_q(st->old_subfr_q, 4, st->Q_exc);
      Updt_mem_q(st->old_syn_q, 4, st->Q_syn);

      voAMRWBPDecCopy(&exc_[k*L_DIV],&exc_[k*L_DIV-(PIT_MAX_MAX + L_INTERPOL)], 4*L_DIV);

      k+=4;       /*ptr*/
      break;

    default:
		break;
      //printf("decoder error: mode > 3!\n");
      //exit(0);
    }

 

    st->prev_lpc_lost = bfi;           

    /* update ispold[] and isfold[] for the next frame */
    voAMRWBPDecCopy(ispnew_, st->wispold, M);
    voAMRWBPDecCopy(isfnew_, st->wisfold, M);
              /* end of while*/
  }

  /*------ update signal for next superframe :
   shift exc[], synth[] and synth_hf[] to the left by L_FRAME_PLUS -----*/

  voAMRWBPDecCopy(&syn[L_FRAME_PLUS-M], st->wold_synth, M);

  /* output exc and synth */
  /* Use no memorie excitation */
  voAMRWBPDecCopy(synth_, syn, L_FRAME_PLUS);

  return;

#endif
}
