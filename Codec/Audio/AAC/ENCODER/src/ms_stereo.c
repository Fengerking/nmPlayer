/*
   MS stereo processing
 */
#include "basic_op.h"
#include "oper_32b.h"
#include "psy_const.h"
#include "ms_stereo.h"


/**************************************************************************/
/*!
  \brief  calculates 100*atan(x/100)
          based on atan approx for x > 0

  \return    100*atan(x/100)

*/
/**************************************************************************/

static Word32 atan_100(Word32 val) 
{
  Word32 temp, sqrval, val100, y; 

   
  if (val > 0) {
    val = min(val, 10000);
    sqrval = val * val;
    val100 = 100 * val;
    temp = val - 100;
     
    if(temp < 0) {
      y = val100 / (100 + (sqrval / 356));
    }
    else {
      y = (157 - (val100 / (28 + (sqrval / 100)))); 
    }
  }
  else{
    val = max(val,-10000);
    sqrval = val * val;
    val100 = -100 * val;
    temp = val + 100;
     
    if(temp > 0) {
      y = - (val100 / (100 + (sqrval / 356)));
    }
    else {
      y =  (val100 / (28 + (sqrval/100))) - 157 ; 
    }
  }

  return y;
}


void MsStereoProcessing(Word32       *sfbEnergyLeft,
                        Word32       *sfbEnergyRight,
                        const Word32 *sfbEnergyMid,
                        const Word32 *sfbEnergySide,
                        Word32       *mdctSpectrumLeft,
                        Word32       *mdctSpectrumRight,
                        Word32       *sfbThresholdLeft,
                        Word32       *sfbThresholdRight,
                        Word32       *sfbSpreadedEnLeft,
                        Word32       *sfbSpreadedEnRight,
                        Word16       *msDigest,
                        Word16       *msMask,
                        const Word16  sfbCnt,
                        const Word16  sfbPerGroup,
                        const Word16  maxSfbPerGroup,
                        const Word16 *sfbOffset,
                        Word16       *weightMsLrPeRatio) {
  Word32 temp;
  Word32 sfb,sfboffs, j; 
  Word32 msMaskTrueSomewhere = 0;
  Word32 msMaskFalseSomewhere = 0;
  Word32 sumMsLrPeRatio = 0;
  Word32 cnt = 0;
                                                                                                          

  for (sfb=0; sfb<sfbCnt; sfb+=sfbPerGroup) {
    for (sfboffs=0;sfboffs<maxSfbPerGroup;sfboffs++) {

      Word32 temp;
      Word32 pnlr,pnms;
      Word32 minThreshold;
      Word32 thrL, thrR, nrgL, nrgR;
      Word32 idx, shift;

      idx = sfb + sfboffs;                                                                       

      thrL = sfbThresholdLeft[idx];                                                                 
      thrR = sfbThresholdRight[idx];                                                                
      nrgL = sfbEnergyLeft[idx];                                                                    
      nrgR = sfbEnergyRight[idx];                                                                   

      minThreshold = min(thrL, thrR);

      nrgL = max(nrgL,thrL) + 1;
      shift = norm_l(nrgL);
	  nrgL = Div_32(thrL << shift, nrgL << shift);
      nrgR = max(nrgR,thrR) + 1;
      shift = norm_l(nrgR);
	  nrgR = Div_32(thrR << shift, nrgR << shift);

      pnlr = fixmul(nrgL, nrgR);

      nrgL = sfbEnergyMid[idx];                                                                     
      nrgR = sfbEnergySide[idx];                                                                    

      nrgL = max(nrgL,minThreshold) + 1;
      shift = norm_l(nrgL);
	  nrgL = Div_32(minThreshold << shift, nrgL << shift);

      nrgR = max(nrgR,minThreshold) + 1;
      shift = norm_l(nrgR);
	  nrgR = Div_32(minThreshold << shift, nrgR << shift);

      pnms = fixmul(nrgL, nrgR);

      temp = (pnlr + 1) / ((pnms >> 8) + 1);
      sumMsLrPeRatio = sumMsLrPeRatio + temp;

      cnt += 1;

      temp = pnms - pnlr;                                                                     
      if( temp > 0 ){

        msMask[idx] = 1;                                                                            
        msMaskTrueSomewhere = 1;                                                                    

        for (j=sfbOffset[idx]; j<sfbOffset[idx+1]; j++) {
          Word32 left, right;
          left  = (mdctSpectrumLeft[j] >>  1);
          right = (mdctSpectrumRight[j] >> 1);
          mdctSpectrumLeft[j] =  left + right;                                               
          mdctSpectrumRight[j] =  left - right;                                              
        }
        
        sfbThresholdLeft[idx] = minThreshold;                                                       
        sfbThresholdRight[idx] = minThreshold;                                                      
        sfbEnergyLeft[idx] = sfbEnergyMid[idx];                                                     
        sfbEnergyRight[idx] = sfbEnergySide[idx];                                                   

        sfbSpreadedEnRight[idx] = min(sfbSpreadedEnLeft[idx],sfbSpreadedEnRight[idx]) >> 1;  
        sfbSpreadedEnLeft[idx] = sfbSpreadedEnRight[idx];                                           
        
      }
      else {
        msMask[idx]  = 0;                                                                           
        msMaskFalseSomewhere = 1;                                                                   
      }
    }                                                                                               
    if ( msMaskTrueSomewhere ) {                                                                    
      if(msMaskFalseSomewhere ) {
        *msDigest = SI_MS_MASK_SOME;                                                                
      } else {
        *msDigest = SI_MS_MASK_ALL;                                                                 
      }
    } else {
      *msDigest = SI_MS_MASK_NONE;                                                                  
    }

    cnt = max(1,cnt);
    /*
      *weightMsLrPeRatio = 28 *  atan( 0.37f*((float)sumMsLrPeRatio/(float)cnt-6.5f) ) + 125; 
    */
    
    temp = atan_100((37 * sumMsLrPeRatio) / (cnt << 8)  - 241);
     
    if (temp >= 0) {
      *weightMsLrPeRatio = (28 * temp) / 100;
      *weightMsLrPeRatio = 125 + *weightMsLrPeRatio;
    }
    else {
      *weightMsLrPeRatio = (-28 * temp) / 100;
      *weightMsLrPeRatio = 125 - *weightMsLrPeRatio;
    }
  }

}
