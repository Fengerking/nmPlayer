
#include "amr_plus_fx.h"

#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "math_op.h"
#define LOWLIMIT 8
#define LOWOMIT 1
#define DFTN 64
#define DFTNx2 128
#define LPC_N 16


void InitClassifyExcitation(NCLASSDATA_FX *stClass)
{

  Word16 i,j;

  stClass->prevModes[0] = ACELP_MODE;     move16();
  stClass->prevModes[1] = ACELP_MODE;     move16();
  stClass->prevModes[2] = ACELP_MODE;     move16();
  stClass->prevModes[3] = ACELP_MODE;     move16();

  stClass->vadFlag[0] = 1;     move16();
  stClass->vadFlag[1] = 1;     move16();
  stClass->vadFlag[2] = 1;     move16();
  stClass->vadFlag[3] = 1;     move16();
  stClass->vadFlag_old[0] = 0;     move16();
  stClass->vadFlag_old[1] = 0;     move16();
  stClass->vadFlag_old[2] = 0;     move16();
  stClass->vadFlag_old[3] = 0;     move16();
  for(i=0;i<10;i++)
  {
    stClass->LTPGain[i]= 0;     move16();
    stClass->LTPLag[i]= 0;     move16();
    stClass->NormCorr[i]= 0;     move16();
  }
  stClass->TotalEnergy[0]= 0;     move16();
  stClass->TotalEnergy[1]= 0;     move16();
  stClass->TotalEnergy[2]= 0;     move16();
  stClass->TotalEnergy[3]= 0;     move16();
  stClass->TotalEnergy[4]= 0;     move16();
  stClass->NoMtcx[0]= 0;     move16();
  stClass->NoMtcx[1]= 0;     move16();
  stClass->NbOfAcelps = 0;     move16();
  stClass->lph[0] = 0;     move16();
  stClass->lph[1] = 0;     move16();
  stClass->lph[2] = 0;     move16();
  stClass->lph[3] = 0;     move16();
  for (j=0;j<4*M;j++)
  {
    stClass->ApBuf[j] = 0;     move16();
  }

  for(j=0;j<TWINLEN;j++)
  {
    for(i=0;i<COMPLEN;i++)
    {
      stClass->levelHist[j][i] = 0;     move16();
    }
  }

  for(i=0;i<COMPLEN;i++)
  {
    stClass->averageHistTime[i] = (Word16)QS;         move16();
    stClass->stdDevHistTime[i] = (Word16)QAVE;     move16();

    stClass->averageHistTimeShort[i] = (Word16)QS;     move16();
    stClass->stdDevHistTimeShort[i] = (Word16)QAVE;       move16();
  }
  for(i=0;i<LPHLEN;i++)  stClass->lphBuf[i] = (Word16)(6*QLPH);     move16();
  for(i=0;i<LPHAVELEN;i++)  stClass->lphAveBuf[i] = (Word16)(6*QLPH);     move16();
  stClass->StatClassCount = 0;     move16();

}

Word16 ClassifyExcitation(
    NCLASSDATA_FX *stClass,         /* i/o : Static variables  */
    Word16 levelNew[],              /* i/o : Energy level      */
    Word16 sfIndex                  /* i   : Subfr index       */
)
{

  Word32 levH , levL, Ltmp1;
  Word16 tmp1, stdAve, stdAveShort, lph, exp_tmp, m_tmp, exp_hist, m_hist;
  Word16 exp_tmp1, m_tmp1, tmp16;
  Word16 i, j;
  Word16 headMode;
  Word16 NormalizedBands[COMPLEN2], bckr_est[COMPLEN2];
  Word16 AverFreqFiltBand, FiltBand_Thres,levRatio;
  Word32 Ltmp;

  levH=0; levL = 0;   move32();move32();
  
  Set_zero(bckr_est,COMPLEN2);  
  FiltBand_Thres = 2000;    move16();
  for(i=CS;i<COMPLEN2;i++)
  {
    /* Take filter bank energy levels from VAD */
    /* done directly in wb_vad to prevent overflow*/

    /* Normalised energy to dB/Hz */
    /*levelNew[i] = levelNew[i] / bw[i];  bw in Q22       Q7*/
    /*levelNew[i] = shr(mult_r(levelNew[i], Bw_inv[i]),0);   move16();*/


    /* Store the latest levels to level buffer only if active speech, during silence buffer is frozen.*/
    test();
    if(stClass->vadFlag[sfIndex]>0) 
    {
      stClass->levelHist[0][i] = levelNew[i];   move16();
    }
  }

  test();
  if(stClass->vadFlag[sfIndex]==0)
  {
    /* During silence, do not update buffers */
  }
  else
  {
    /* Calculate reqular standard deviation for short and long window */
    /* These calculations can be optmised some unnecessary calculations are performed still */
    
    levH=0; levL = 0;   move32();move32();
    
    for(i=CS;i<COMPLEN2;i++)
    {
      tmp16 = sub(stClass->averageHistTime[i], shr_r(stClass->levelHist[TWINLEN - 1][i],4));
      stClass->averageHistTime[i] = add(tmp16, shr_r(stClass->levelHist[0][i],4));    move16();

      tmp16 = sub(stClass->averageHistTimeShort[i], shr_r(stClass->levelHist[TWINLENSHORT - 1][i],2));
      stClass->averageHistTimeShort[i] = add(tmp16, shr_r(stClass->levelHist[0][i] ,2));   move16();

      Ltmp = 0;      move32();

     for(j=0;j<TWINLEN;j++)
      { 
        tmp16 = sub(stClass->averageHistTime[i], stClass->levelHist[j][i]);
        Ltmp = L_mac(Ltmp, tmp16,tmp16);
      }
      exp_tmp = norm_l(Ltmp);
      Ltmp = L_shl(Ltmp, exp_tmp);
      exp_tmp = sub(30-4+2*5, exp_tmp);   
      Isqrt_n(&Ltmp, &exp_tmp);
      m_tmp = extract_h(Ltmp);

      
      Ltmp = L_mult(stClass->averageHistTime[i], m_tmp);
      exp_hist = norm_l(Ltmp);
      m_hist = extract_h(L_shl(Ltmp,exp_hist));
      exp_hist = add(sub(15+5, exp_hist),exp_tmp);

      m_hist = div_s(16384, m_hist);
      exp_hist = sub(1,exp_hist);
      stClass->stdDevHistTime[i] = shr(m_hist, sub(4,exp_hist));          /*QAVE*/

      /* SHORT WIN start ************************** */
      
      Ltmp = 0;      move32();
      for(j=0;j<TWINLENSHORT;j++)
      {
        /* tmp1 = sub(stClass->averageHistTimeShort[i],stClass->levelHist[j][i]);
        Ltmp = L_mac(Ltmp, tmp1, tmp1);	cause overflow... */
        tmp16 = sub(stClass->averageHistTimeShort[i], stClass->levelHist[j][i]);
        Ltmp = L_mac(Ltmp, tmp16,tmp16);
      }

      exp_tmp = norm_l(Ltmp);
      Ltmp = L_shl(Ltmp, exp_tmp);
      exp_tmp = sub(30-2+2*5, exp_tmp);   
      Isqrt_n(&Ltmp, &exp_tmp);
      m_tmp = extract_h(Ltmp);
      
      Ltmp = L_mult(stClass->averageHistTimeShort[i], m_tmp);
      exp_hist = norm_l(Ltmp);
      m_hist = extract_h(L_shl(Ltmp,exp_hist));
      exp_hist = add(sub(15+5, exp_hist),exp_tmp);

      m_hist = div_s(16384, m_hist);
      exp_hist = sub(1,exp_hist);
      stClass->stdDevHistTimeShort[i] = shr(m_hist, sub(4,exp_hist));          /*QAVE*/

      /* SHORT WIN end ************************** */
      test();test();
      if(sub(i,LOWOMIT)<0);
      else if(sub(i,LOWLIMIT)<0)
      {
        levL = L_add(levL, stClass->levelHist[0][i]);  /*Qs*/
      }
      else
      {
        levH = L_add(levH, stClass->levelHist[0][i]);
      }
    }
  }
  /*Analysis of energy & spectral variation*/
  
  for(i=4;i>0;i--)
  {
    stClass->TotalEnergy[i] = stClass->TotalEnergy[i-1];     /*Qs*/  move32();
  }

  stClass->TotalEnergy[0] = 0;          move32();
  for(i=CS;i<COMPLEN2;i++)
  { test();
    if (sub(levelNew[i],bckr_est[i]) < 0)
    {
      bckr_est[i] = levelNew[i];      move16();         /*Qs*/
    }
    stClass->TotalEnergy[0] = L_add(stClass->TotalEnergy[0], sub(levelNew[i],bckr_est[i]));   /*QS*/  move32();
  }

  for(i=CS;i<COMPLEN2;i++)
  {
    /*NormalizedBands[i]=(float)((levelNew[i]-bckr_est[i])/(stClass->TotalEnergy[0]+0.0001));*/
    tmp16 = sub(levelNew[i],bckr_est[i]);
    exp_tmp = sub(norm_s(tmp16),1);
    m_tmp = shl(tmp16, exp_tmp);
    exp_tmp = sub(15, exp_tmp);

    test();
    if(stClass->TotalEnergy[0]== 0)
    {
      stClass->TotalEnergy[0] = 1;  move32();         
    }
    exp_tmp1 = norm_l(stClass->TotalEnergy[0]);
    m_tmp1  = extract_h(L_shl(stClass->TotalEnergy[0], exp_tmp1));
    exp_tmp1 = sub(31,exp_tmp1);

    m_tmp = div_s(m_tmp, m_tmp1);
    exp_tmp = sub(exp_tmp, exp_tmp1);
    NormalizedBands[i] = shl(m_tmp, exp_tmp);   move16();  /* Q15*/
  }
  
  /*the mean frequency along filter bands */

  Ltmp = 0; move32();
  for(i=CS;i<COMPLEN2;i++)
  {
    Ltmp =L_mac(Ltmp, NormalizedBands[i], VADFiltBandFreqs[i]);   /*VadFiltBandFreqs Q0*/
  }
  AverFreqFiltBand = round(Ltmp);  /*Q0*/ /* can saturate to 2000*/
  /* Worst case levL and levH is sum over 0 to 12 levelHist
      levelHist is 16 bits, so maximum value of levL or levH < 2^(16+4) */
  /* Perform division here */
  /* (levL/220) / (levH/4000)  -> levL/levH * 1.818  */
  test();
  if(levL == 0)
  {
    levRatio = 0;   move16();
  }
  else
  {
    if (levH == 0)
    {
      levH = 1; move32();
    }
    exp_tmp1 = norm_l(levL);
    tmp1 = extract_h(L_shl(levL, exp_tmp1));
    exp_tmp1 = sub(31, exp_tmp1);
    
    exp_tmp = norm_l(levH);
    tmp16 = extract_h(L_shl(levH, exp_tmp));
    exp_tmp = sub(31, exp_tmp);

    test();
    if(sub(tmp1, tmp16)>0)
    {
      tmp1 = shr(tmp1, 1);
      exp_tmp1 = add(exp_tmp1,1);
    }
    tmp1 = div_s(tmp1, tmp16);
    exp_tmp1 = sub(exp_tmp1, exp_tmp);
    levRatio = mult(tmp1, 29789);     /*1.818181 Q14 */
    levRatio = shr(levRatio, sub(9 - 1,exp_tmp1));  /* QLPH*/
  }
 
  
  Ltmp = 0;         move32();
  Ltmp1 = 0;    move32();

  for(j=CS;j<COMPLEN2;j++)
  {
    /*stdAve += stClass->stdDevHistTime[j];*/
    Ltmp = L_mac(Ltmp, stClass->stdDevHistTime[j], 2731); /* mult by 1/12 */
    /*stdAveShort += stClass->stdDevHistTimeShort[j];*/
    Ltmp1 = L_mac(Ltmp1, stClass->stdDevHistTimeShort[j], 2731); /* mult by 1/12 */
  }
  stdAve = round(Ltmp);
  stdAveShort = round(Ltmp1);



  test();
  if(stClass->vadFlag[sfIndex]>0)
  {
    for(i=CS;i<COMPLEN2;i++)
    {
      for(j=TWINLEN-1;j>0;j--)
      {
        stClass->levelHist[j][i] = stClass->levelHist[j-1][i];    move16();
      }
    }
  }
  test();
  if(stClass->vadFlag[sfIndex] > 0)
  {
    for(j=LPHLEN-1;j>0;j--) 
    {
      stClass->lphBuf[j] =  stClass->lphBuf[j-1];     move16();
    }
    /*stClass->lphBuf[0] = levL / levH;*/
    stClass->lphBuf[0] = levRatio;  move16();   /*QLPH -> L/H > 1024*/
  }
  Ltmp = 0; move32();
  for(j=0;j<LPHLEN;j++) 
  {
    Ltmp = L_mac(Ltmp, stClass->lphBuf[j], 8192);
  }
  lph = round(Ltmp);  /*QLPH*/
  test();test();
  if( (stClass->vadFlag[sfIndex]>0) && (sub(sfIndex,3) == 0) )
  {
    for(j=LPHAVELEN-1;j>0;j--) 
    {
      stClass->lphAveBuf[j] =	stClass->lphAveBuf[j-1];    move16();
    }
  }
  stClass->lphAveBuf[0] = lph;     /*QLPH*/ move16();

  Ltmp = 0;   move32();

  for(j=0;j<LPHAVELEN;j++) 
  {
    Ltmp = L_mac(Ltmp, stClass->lphAveBuf[j], Lwg[j]);
  }
  lph = round(Ltmp);

  /*Classifies the signal into three categories
  based on the standard deviation and tilt of the signal*/
  /* Encode with ACELP = ACELP_MODE*/
  /* Encode with TCX   = TCX_MODE*/
  /* Encode with ACELP or TCX = TCX_OR_ACELP (decided in ClassB refinement)*/
  /*TCX*/
  test();
  if (stClass->StatClassCount == 0) 
  {
    /* Default mode */
    headMode = TCX_MODE;      move16();
    test();test();test();
    if (sub(stdAve, 819) <= 0) 
    {
      headMode = TCX_MODE;        move16();
    }
    else if (sub(lph, 280*QLPH) > 0) 
    {
      headMode = TCX_MODE;        move16();
    }
    else if (sub(stdAve, 819) > 0)
    {
     
      /* TCX : ((5+(1/(stdAve/QAVE-0.4))) > lph/QLPH) */
      tmp16 = sub(stdAve,819);
      exp_tmp = norm_s(tmp16);
      tmp16 = shl(tmp16, exp_tmp);
      exp_tmp = sub(15,exp_tmp);
      tmp16 = div_s(16384, tmp16);
      tmp1 = sub(add(shl(tmp16, sub(1+11-9,exp_tmp)), 5*QLPH), lph);      /* QLPH */ 
      

      /* ACELP : (-90*stdAve/QAVE+120) < lph/QLPH) */
      Ltmp1 = L_mac(491520, stdAve, -90);
      Ltmp1 = L_sub(Ltmp1, L_shl(lph, 11+1-6));

      /* TCX */
      test();test();
      if (tmp1>0)
      {
        headMode = TCX_MODE;          move16();
      }
      /* ACELP */
      else if (Ltmp1 < 0) 
      {
        headMode = ACELP_MODE;        move16();
      }
      /* TCX_OR_ACELP */
      else  
      {
        headMode = TCX_OR_ACELP;        move16();
      }
    }
  }
  else 
  {
    headMode = TCX_OR_ACELP;
  }

  /*Noise like signal with TCX*/
  test();test();test();
  if ((sub(headMode,ACELP_MODE ) == 0|| sub(headMode,TCX_OR_ACELP) == 0) &&
        sub(AverFreqFiltBand,FiltBand_Thres) > 0) 
  {
    headMode = TCX_MODE;      move16();
  }
  test();
  if (sub(stClass->StatClassCount,5) < 0) 
  {
    test();
    if (sub(headMode,TCX_OR_ACELP) == 0)
    {
      test();test();
      if (sub(stdAveShort,410) <= 0)         /*0.2*QAVE  = Q11*/
      {
        headMode = TCX_MODE;	    move16();
      }
      else if (sub(stdAveShort, 410) > 0) /*0.2*QAVE  = Q11*/
      {
        /* TCX : (2.5+(1/(stdAveShort/QAVE-0.2))) > lph/QLPH) */
        tmp16 = sub(stdAveShort,410);
        exp_tmp = norm_s(tmp16);
        tmp16 = shl(tmp16, exp_tmp);
        exp_tmp = sub(15,exp_tmp);
        tmp16 = div_s(16384, tmp16);
      
        tmp1 = sub(add(shl(tmp16, sub(1+11-9,exp_tmp)), 2.5*QLPH), lph);      /* QLPH */ 

        /*ACELP  :  (-90*stdAveShort/QAVE+140) < lph/QLPH) */
        Ltmp1 = L_mac(573440, stdAveShort, -90);
        Ltmp1 = L_sub(Ltmp1, L_shl(lph, 11+1-6));

        /* TCX */
        test();test();
        if (tmp1>0) 
        {
          headMode = TCX_MODE;		move16();
        }
        /*ACELP*/
        else if (Ltmp1 < 0) 
        {
          headMode = ACELP_MODE;		move16();
        }
        /*TCX_OR_ACELP*/
        else
        {
          headMode = TCX_OR_ACELP;	move16();
        }
      }
    }
  }
  test();
  if (headMode == TCX_OR_ACELP) 
  {
    test();

    if (sub(stClass->StatClassCount,15) < 0) 
    {
      /*(stClass->TotalEnergy[0]/(stClass->TotalEnergy[1]+0.00001))>25)*/
      exp_tmp = sub(norm_l(stClass->TotalEnergy[0]),1);
      tmp16 = extract_h(L_shl(stClass->TotalEnergy[0],exp_tmp));
      exp_tmp = sub(31-5,exp_tmp);      /* 31 -5*/
      tmp16 = div_s(tmp16, 25600);
      exp_tmp = sub(exp_tmp,5); 
      Ltmp1 = L_shr(tmp16, sub(15-5,exp_tmp)); /* 15 -5*/      /*QS = 5*/ 

      Ltmp = L_sub(Ltmp1,stClass->TotalEnergy[1]);
      test();
      if (Ltmp > 0)
      {
        headMode = ACELP_MODE;      move16();
      }
    }
  }
  test();test();
  if ((sub(headMode,TCX_MODE) == 0|| sub(headMode,TCX_OR_ACELP) == 0)) 
  {
    test();test();
    if (sub(AverFreqFiltBand,FiltBand_Thres) > 0 &&
          L_sub(stClass->TotalEnergy[0],1920) < 0) /* 60*QS*/
    {
      headMode = ACELP_MODE;      move16();
    }
  }
  for(i=2;i>=0;i--) 
  {
    stClass->lph[i+1] = stClass->lph[i];      move16();
  }
  stClass->lph[0] = lph;      move16();
  
  /* Always use TCX for non-speech, better because short DTX bursts do not affect speech if misclassification with speech/nonspeech happens */
  /* during music */
  test();
  if(stClass->vadFlag[sfIndex]==0) 
  {
    headMode = TCX_MODE;      move16();
  }

  return(headMode);

}

void ClassifyExcitationRef(
    NCLASSDATA_FX *stClass,     /* i/o : Static variables   */
    Word16 *ISPs_,              /* i   : Isp coefficients   */
    Word16 *headMode            /* i/o : Mode choosen       */
)
{

  Word16 sfIndex, i, j;
  Word16 SDminInd = 0, SDmaxInd = 0;
  Word16 tmp1 = 0, tmp2 = 0, e_tmp;
  Word16 SD_[4] = {0}, tmp16, sf2, mag_[DFTN];
  Word32 Ltmp1;

  Word16 ip_[LPC_N]={0};
  Word16 cos_t_[DFTNx2], sin_t_[DFTNx2];
  Word32 x_ = 0, y_ = 0;

  for(sfIndex=0; sfIndex<4; sfIndex++)
  {
    tmp16 = 0;      move16();
    for(i=0; i<4; i++)
    {
      /*SD[sfIndex] += (float)(fabs(ISPs[(sfIndex+1)*M+i]-ISPs[sfIndex*M+i]));*/
      tmp16 = add(tmp16, abs_s(sub(ISPs_[(sfIndex+1)*M+i], ISPs_[sfIndex*M+i])));     /* can sature because only check if > 0.2 */
    }
    SD_[sfIndex] =  tmp16;   move16();
    
    test();test();
    if (sub(SD_[sfIndex],SD_[SDminInd]) < 0) 
    {
      SDminInd = sfIndex;       move16();
    }
    else if (sub(SD_[sfIndex],SD_[SDmaxInd]) > 0)
    {
      SDmaxInd = sfIndex;       move16();
    }
  }
  /*In the case of switching, the history of buffers are updated with the values of current frame*/
  test();
  if (sub(stClass->StatClassCount,15) == 0) 
  {
    sfIndex=0;                                                            move16();
    sf2 = add(shl(sfIndex,1),2);    
    stClass->LTPLag[sf2-2] = stClass->LTPLag[sf2];                        move16();
    stClass->LTPLag[sf2-1] = stClass->LTPLag[sf2+1];                      move16();
    stClass->LTPGain[sf2-2] = stClass->LTPGain[sf2];                      move16();
    stClass->LTPGain[sf2-1] = stClass->LTPGain[sf2+1];                    move16();
    stClass->NormCorr[sf2-2] = stClass->NormCorr[sf2];                    move16();
    stClass->NormCorr[sf2-1] = stClass->NormCorr[sf2+1];                  move16();
  }

  for (sfIndex=0; sfIndex<4; sfIndex++) 
  {
    sf2 = add(shl(sfIndex,1),2);
    test();test();test();test();
    if( (stClass->vadFlag[sfIndex]!=0) && (sub(headMode[sfIndex],TCX_OR_ACELP) == 0) ) 
    {
      test();
      if (sub(SD_[sfIndex],6554) > 0) 
      {
        headMode[sfIndex] = ACELP_MODE;       move16();
      }
      else
      {
        tmp1 = 0;     move16();
        
        tmp1 = abs_s(sub(stClass->LTPLag[sf2],stClass->LTPLag[sf2+1]));
        tmp2 = abs_s(sub(stClass->LTPLag[sf2-2],stClass->LTPLag[sf2-1]));
        
        test();test();test();
        if ( (sub(tmp1,2) < 0) &&  
              (sub(tmp2,2) < 0)  &&
              (sub(abs_s(sub(stClass->LTPLag[sf2-1],stClass->LTPLag[(sfIndex*2+2)])),2) < 0) ) 
        {
          test();test();test();test();test();test();test();test();test();test();    

          if ( ( (sub(stClass->LTPLag[sf2-2],18) == 0) && (sub(stClass->LTPLag[sf2+1],18) == 0) ) ||
                  ( (sub(stClass->LTPLag[sf2-2],115) == 0) && (sub(stClass->LTPLag[sf2+1],115) == 0) ) ) 
          {
        
            if ( (sub(abs_s(sub(stClass->LTPGain[sf2],stClass->NormCorr[sf2])),3277) < 0) &&
                  (sub(abs_s(sub(stClass->LTPGain[sf2+1],stClass->NormCorr[sf2+1])),3277) < 0) &&
                      (sub(stClass->NormCorr[sf2], 29491) > 0) &&
                      (sub(stClass->NormCorr[sf2+1], 29491) > 0) &&
                      (sub(stClass->NormCorr[sf2-1], 29491) > 0) &&
                      (sub(stClass->NormCorr[sf2-2], 29491) > 0)) 
            {
              headMode[sfIndex] = ACELP_MODE;         move16();
            }
            else 
            {
              headMode[sfIndex] = TCX_MODE;           move16();
            }
          }
          else if ( (sub(abs_s(sub(stClass->LTPGain[sf2],stClass->NormCorr[sf2])),3277) < 0) &&
                      (sub(abs_s(sub(stClass->LTPGain[sf2+1],stClass->NormCorr[sf2+1])),3277) < 0) &&
                      (sub(stClass->NormCorr[sf2],28836) > 0) && (sub(stClass->NormCorr[sf2+1],28836) > 0)) 
          {
            headMode[sfIndex] = ACELP_MODE;                   move16();
          }
          else if ( (sub(abs_s(sub(stClass->LTPGain[sf2],stClass->NormCorr[sf2])),6554) > 0) &&
                      (sub(abs_s(sub(stClass->LTPGain[sf2+1],stClass->NormCorr[sf2+1])),6554) > 0) ) 
          {
            headMode[sfIndex] = TCX_MODE;             move16();
          }
          else 
          {
            test();
            if (sub(sfIndex,2) < 0) 
            {
              stClass->NoMtcx[0] = add(stClass->NoMtcx[0],1);   move16();
            }
            else
            {
              stClass->NoMtcx[1] = add(stClass->NoMtcx[1],1);   move16();
            }
          }
        }
        test();
        if (sub(headMode[sfIndex],TCX_OR_ACELP) == 0) 
        {
          Ltmp1 = stClass->TotalEnergy[4];
  
          for(i=0;i<4;i++) 
          {
            test();
            if(L_sub(stClass->TotalEnergy[i],Ltmp1) > 0)
            {
              Ltmp1=stClass->TotalEnergy[i];      move32();
            }
          }
          test();
          if (L_sub(Ltmp1,60*QS) < 0 ) 
          {
            test();
            if (sub(SD_[sfIndex],4915) > 0)     /*0.15*/
            {
              headMode[sfIndex] = ACELP_MODE;                 move16();
            }
            else
            { 
              test();
              if (sub(sfIndex,2) < 0) 
              {
                stClass->NoMtcx[0] = add(stClass->NoMtcx[0],1);   move16();
              }
              else
              {
                  stClass->NoMtcx[1] = add(stClass->NoMtcx[1],1);   move16();
              }
            }
          }
        }
      }
    }
    else if( (stClass->vadFlag[sfIndex]!=0)  && (sub(headMode[sfIndex],ACELP_MODE) == 0) ) 
    {
      test();test();
      if ( (sub(abs_s(sub(stClass->LTPLag[sf2],stClass->LTPLag[sf2+1])),2) < 0) &&
            (sub(abs_s(sub(stClass->LTPLag[sf2-2],stClass->LTPLag[sf2-1])),2) < 0)) 
      {
        test();test();test();
        if ( (sub(stClass->NormCorr[sf2],26214) < 0) && (sub(stClass->NormCorr[sf2+1],26214) < 0) &&
            ( sub(SD_[SDmaxInd],3277) < 0)) 
        {
          headMode[sfIndex] = TCX_MODE;       move16();
        }
      }
      if ( (sub(stClass->lph[sfIndex],200*QLPH) > 0) && (sub(SD_[SDmaxInd],3277) < 0)) 
      {
        headMode[sfIndex] = TCX_MODE;         move16();
      }
    }

  }


  for(sfIndex=0;sfIndex<4;sfIndex++)
  {
  
    sf2 = add(shl(sfIndex,1),2);
    /*80ms TCX is disabled if VAD is set to zero in last 80ms */
    test();test();test();
    if ( (stClass->vadFlag_old[sfIndex] == 0) &&
          (sub(stClass->vadFlag[sfIndex],1) == 0) &&
          (sub(headMode[sfIndex],TCX_MODE) == 0)) 
    {
      test();  
      if (sub(sfIndex,2) < 0) 
      {
        stClass->NoMtcx[0] = add(stClass->NoMtcx[0],1);   move16();
      }
      else 
      {
        stClass->NoMtcx[1] = add(stClass->NoMtcx[1],1);   move16();
      }
    }
    test();
    if (sub(headMode[sfIndex],ACELP_MODE) != 0) 
    {
      Ltmp1 = L_abs(L_sub(stClass->LTPGain[sf2-2], stClass->NormCorr[sf2-2]));    /*4*/
      Ltmp1 = L_add(Ltmp1, L_abs(L_sub(stClass->LTPGain[sf2-1], stClass->NormCorr[sf2-1])));/*6*/
      Ltmp1 = L_add(Ltmp1, L_abs(L_sub(stClass->LTPGain[sf2], stClass->NormCorr[sf2])));
      Ltmp1 = L_add(Ltmp1,L_abs(L_sub(stClass->LTPGain[sf2+1], stClass->NormCorr[sf2+1])));
     
      test();test();test();test();test();
      if ( (L_sub(Ltmp1, 786) < 0) &&                   /*0.006*32768*4*/
            (sub(stClass->NormCorr[sf2],30147) > 0) && 
            (sub(stClass->NormCorr[sf2+1],30147) > 0) &&
            (sub(stClass->LTPLag[sf2],21) > 0) && 
            (sub(stClass->LTPLag[sf2+1],21) > 0) ) 
      {
        for (i=0; i<DFTNx2; i++)
        {
          cos_t_[i] = t_sinFxS4[i*N_MAX/DFTNx2 + COSOFFSET];   move16();/* Q13  */
          sin_t_[i] = t_sinFxS4[i*N_MAX/DFTNx2];               move16();/* Q13  */
        }
        for (i=0; i<LPC_N; i++) 
        {
          ip_[i] = stClass->ApBuf[sfIndex*M+i]; move16();       /* Q12*/
        }

        mag_[0] = 0;      move16();
        for (i=0; i<DFTN; i++)
        {/* calc DFT */
          x_ = y_ = 0;     move32();move32();
          for (j=0; j<LPC_N; j++)
          {
            /*x += ip[j]*(cos_t[(i*j)&(DFTNx2-1)]);*/
            /*y += ip[j]*(sin_t[(i*j)&(DFTNx2-1)]);*/
            x_ = L_mac(x_, ip_[j],cos_t_[(i*j)&(DFTNx2-1)]);
            y_ = L_mac(y_, ip_[j], sin_t_[(i*j)&(DFTNx2-1)]);
          }
          /*mag[i] = (float)(1/sqrt(x*x+y*y));*/          
          tmp16= round(x_);
          Ltmp1 = L_mult(tmp16, tmp16);
          tmp16 = round(y_);
          Ltmp1 = L_mac(Ltmp1, tmp16, tmp16);
          e_tmp = norm_l(Ltmp1);
          Ltmp1 = L_shl(Ltmp1, e_tmp);
          e_tmp = sub(30-20, e_tmp);
          
          Isqrt_n(&Ltmp1, &e_tmp);
          mag_[i] = extract_l(L_shr(Ltmp1, sub(25, e_tmp)));     move16();      /*Q6*/
          
        }
        Ltmp1 = 0;      move32();
        for (i=1; i<40; i++) 
        { /*First element left out*/
          Ltmp1 = L_add(mag_[i], Ltmp1);
        }
        test();
        if ((L_sub(Ltmp1, 6080)> 0) && (sub(mag_[0], 320) < 0) )    /*95 and 5*/
        {
          headMode[sfIndex] = TCX_MODE;     move16();
        }
        else
        {
          headMode[sfIndex] = ACELP_MODE;     move16();
          test();
          if (sub(sfIndex,2) < 0) 
          {
            stClass->NoMtcx[0] = add(stClass->NoMtcx[0],1);   move16();
          }
          else 
          {
            stClass->NoMtcx[1] = add(stClass->NoMtcx[1],1);   move16();
          }
        }
      }
    }
    test();
    if (sub(stClass->StatClassCount,12) < 0) 
    {
      test();
      if (sub(headMode[sfIndex],TCX_OR_ACELP) == 0) 
      {
        tmp1 = 0;   move16();
        tmp2 = 0;   move16();
        for(i=0;i<4;i++) 
        {
          test();test();test();test();
          if ( ( (sub(stClass->prevModes[i],3) == 0) || (sub(stClass->prevModes[i],2) == 0) ) &&
                  sub(stClass->vadFlag_old[i],1) == 0 && L_sub(stClass->TotalEnergy[i],60*QS) > 0) 
          {
            tmp1 = add(tmp1,1);
          }
          test();
          if (sub(stClass->prevModes[i],ACELP_MODE) == 0) 
          {
            tmp2 = add(tmp2,1);;
          }
          test();
          if (sub(sfIndex,i) != 0) 
          { test();
            if (sub(headMode[i],ACELP_MODE) == 0)
            {
              tmp2 = add(tmp2,1);;
            }
          }
        }
        test();test();
        if (sub(tmp1,3) > 0) 
        {
          headMode[sfIndex] = TCX_MODE;       move16();
        }
        else if (sub(tmp2,1) > 0) 
        { 
          headMode[sfIndex] = ACELP_MODE;     move16();
        }
        else 
        {
          headMode[sfIndex] = TCX_MODE;       move16();
        }
      }
    }
    else
    {
      test();
      if (sub(headMode[sfIndex],TCX_OR_ACELP) == 0) 
      {
        headMode[sfIndex] = TCX_MODE;     move16();
      }
    }
  }
  stClass->NbOfAcelps = 0;      move16();
  
  for(sfIndex=0;sfIndex<4;sfIndex++)
  {
    test();
    if (sub(headMode[sfIndex],ACELP_MODE) == 0) 
    {
      stClass->NbOfAcelps  = add(stClass->NbOfAcelps,1);
    }
  }

  /*Buffer updates*/
  
  stClass->LTPGain[0] = stClass->LTPGain[8];        move16();
  stClass->LTPGain[1] = stClass->LTPGain[9];        move16();
  stClass->LTPLag[0] = stClass->LTPLag[8];          move16();
  stClass->LTPLag[1] = stClass->LTPLag[9];          move16();
  stClass->NormCorr[0] = stClass->NormCorr[8];      move16();
  stClass->NormCorr[1] = stClass->NormCorr[9];      move16();
  
  for(i=0;i<4;i++) 
  {
    stClass->vadFlag_old[i] = stClass->vadFlag[i];  move16();
    test();
    if (stClass->StatClassCount > 0) 
    {
      stClass->StatClassCount = sub(stClass->StatClassCount,1);
    }
  }
  return;
}
