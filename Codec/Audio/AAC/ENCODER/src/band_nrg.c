/*
   Band/Line energy calculations
 */
#include "basic_op.h"
#include "band_nrg.h"

#ifndef ARMV5E
void CalcBandEnergy(const Word32 *mdctSpectrum,
                    const Word16 *bandOffset,
                    const Word16  numBands,
                    Word32       *bandEnergy,
                    Word32       *bandEnergySum)
{
  Word32 i, j;
  Word32 accuSum = 0;                                            

  for (i=0; i<numBands; i++) {
    Word32 accu = 0;                                             
    for (j=bandOffset[i]; j<bandOffset[i+1]; j++)
      accu = L_add(accu, MULHIGH(mdctSpectrum[j], mdctSpectrum[j]));

	accu = L_add(accu, accu);
    accuSum = L_add(accuSum, accu);
    bandEnergy[i] = accu;                                        
  }
  *bandEnergySum = accuSum;                                      
}

void CalcBandEnergyMS(const Word32 *mdctSpectrumLeft,
                      const Word32 *mdctSpectrumRight,
                      const Word16 *bandOffset,
                      const Word16  numBands,
                      Word32       *bandEnergyMid,
                      Word32       *bandEnergyMidSum,
                      Word32       *bandEnergySide,
                      Word32       *bandEnergySideSum)
{

  Word32 i, j;
  Word32 accuMidSum = 0;        
  Word32 accuSideSum = 0;                                          
 

  for(i=0; i<numBands; i++) {
    Word32 accuMid = 0;
    Word32 accuSide = 0;                                           
    for (j=bandOffset[i]; j<bandOffset[i+1]; j++) {
      Word32 specm, specs; 
      Word32 l, r;

      l = mdctSpectrumLeft[j] >> 1;
      r = mdctSpectrumRight[j] >> 1;
      specm = l + r;
      specs = l - r;
      accuMid = L_add(accuMid, MULHIGH(specm, specm));
      accuSide = L_add(accuSide, MULHIGH(specs, specs));
    }
    
	accuMid = L_add(accuMid, accuMid);
	accuSide = L_add(accuSide, accuSide);
	bandEnergyMid[i] = accuMid;                                  
    accuMidSum = L_add(accuMidSum, accuMid);
    bandEnergySide[i] = accuSide;                                
    accuSideSum = L_add(accuSideSum, accuSide);
    
  }
  *bandEnergyMidSum = accuMidSum;                                
  *bandEnergySideSum = accuSideSum;                              
}

#endif