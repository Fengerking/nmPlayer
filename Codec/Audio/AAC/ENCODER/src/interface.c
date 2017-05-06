/*
   Interface psychoaccoustic/quantizer
 */
#include "basic_op.h"
#include "oper_32b.h"
#include "psy_const.h"
#include "interface.h"




void BuildInterface(Word32                  *groupedMdctSpectrum,
                    const Word16             mdctScale,
                    SFB_THRESHOLD           *groupedSfbThreshold,
                    SFB_ENERGY              *groupedSfbEnergy,
                    SFB_ENERGY              *groupedSfbSpreadedEnergy,
                    const SFB_ENERGY_SUM     sfbEnergySumLR,
                    const SFB_ENERGY_SUM     sfbEnergySumMS,
                    const Word16             windowSequence,
                    const Word16             windowShape,
                    const Word16             groupedSfbCnt,
                    const Word16            *groupedSfbOffset,
                    const Word16             maxSfbPerGroup,
                    const Word16            *groupedSfbMinSnr,
                    const Word16             noOfGroups,
                    const Word16            *groupLen,
                    PSY_OUT_CHANNEL         *psyOutCh)
{
  Word32 j;
  Word32 grp; 
  Word32 mask;
  Word16 *tmpV;
  const Word32 *pSumMS,*pSumLR;


  /*
  copy values to psyOut
  */
  psyOutCh->maxSfbPerGroup    = maxSfbPerGroup;                             
  psyOutCh->sfbCnt            = groupedSfbCnt;                              
  psyOutCh->sfbPerGroup       = groupedSfbCnt/ noOfGroups;//ffr_Short_Div(groupedSfbCnt, noOfGroups);
  psyOutCh->windowSequence    = windowSequence;                             
  psyOutCh->windowShape       = windowShape;                                
  psyOutCh->mdctScale         = mdctScale;                                  
  psyOutCh->mdctSpectrum      = groupedMdctSpectrum;
  psyOutCh->sfbEnergy         = groupedSfbEnergy->sfbLong;
  psyOutCh->sfbThreshold      = groupedSfbThreshold->sfbLong;
  psyOutCh->sfbSpreadedEnergy = groupedSfbSpreadedEnergy->sfbLong;
  
  tmpV = psyOutCh->sfbOffsets;
  for(j=0; j<groupedSfbCnt + 1; j++) {
    //psyOutCh->sfbOffsets[j] = groupedSfbOffset[j];                               
	  *tmpV++ = groupedSfbOffset[j];
  }
  
  tmpV = psyOutCh->sfbMinSnr;
  for(j=0;j<groupedSfbCnt; j++) {
    //psyOutCh->sfbMinSnr[j] = groupedSfbMinSnr[j];  
	*tmpV++ =   groupedSfbMinSnr[j];
  }
  
  /* generate grouping mask */
  mask = 0;                                                                      
  for (grp = 0; grp < noOfGroups; grp++) {
    mask = mask << 1;
    for (j=1; j<groupLen[grp]; j++) {
      mask = mask << 1;
      mask |= 1;                                                                 
    }
  }
  psyOutCh->groupingMask = mask; 
  
  if (windowSequence != SHORT_WINDOW) {
    psyOutCh->sfbEnSumLR =  sfbEnergySumLR.sfbLong;                              
    psyOutCh->sfbEnSumMS =  sfbEnergySumMS.sfbLong;                              
  }
  else {
    Word32 i;
    Word32 accuSumMS=0;
    Word32 accuSumLR=0;        
	pSumMS = &sfbEnergySumMS.sfbShort[0];
	pSumLR = &sfbEnergySumLR.sfbShort[0];

    for (i=TRANS_FAC; i; i--) {
      accuSumLR = L_add(accuSumLR, *pSumLR); pSumLR++;
      accuSumMS = L_add(accuSumMS, *pSumMS); pSumMS++;
    }
    psyOutCh->sfbEnSumMS = accuSumMS;                                            
    psyOutCh->sfbEnSumLR = accuSumLR;                                            
  }
}
