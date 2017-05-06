/*
   Perceptual entropie module
 */
#include "basic_op.h"
#include "oper_32b.h"
#include "typedef.h"
#include "line_pe.h"


static const Word16  C1_I = 12;    /* log(8.0)/log(2) *4         */
static const Word32  C2_I = 10830; /* log(2.5)/log(2) * 1024 * 4 * 2 */
static const Word16  C3_I = 573;   /* (1-C2/C1) *1024            */



/* constants that do not change during successive pe calculations */

void prepareSfbPe(PE_DATA *peData,
                  PSY_OUT_CHANNEL  psyOutChannel[MAX_CHANNELS],
                  Word16 logSfbEnergy[MAX_CHANNELS][MAX_GROUPED_SFB],
                  Word16 sfbNRelevantLines[MAX_CHANNELS][MAX_GROUPED_SFB],
                  const Word16 nChannels,
                  const Word16 peOffset)
{
  Word32 sfbGrp, sfb;
  Word32 ch;     

  for(ch=0; ch<nChannels; ch++) {
    PSY_OUT_CHANNEL *psyOutChan = &psyOutChannel[ch];
    PE_CHANNEL_DATA *peChanData=&peData->peChannelData[ch];
    for(sfbGrp=0;sfbGrp<psyOutChan->sfbCnt; sfbGrp+=psyOutChan->sfbPerGroup){
      for (sfb=0; sfb<psyOutChan->maxSfbPerGroup; sfb++) {
	    peChanData->sfbNLines4[sfbGrp+sfb] = sfbNRelevantLines[ch][sfbGrp+sfb];          
        sfbNRelevantLines[ch][sfbGrp+sfb] = sfbNRelevantLines[ch][sfbGrp+sfb] >> 2;    
	    peChanData->sfbLdEnergy[sfbGrp+sfb] = logSfbEnergy[ch][sfbGrp+sfb];              
      }
    }
  }
  peData->offset = peOffset;                                                             
}

/*
   constPart is sfbPe without the threshold part n*ld(thr) or n*C3*ld(thr)
*/

void calcSfbPe(PE_DATA *peData,
               PSY_OUT_CHANNEL psyOutChannel[MAX_CHANNELS],
               const Word16 nChannels)
{
  Word32 ch;
  Word32 sfbGrp, sfb;
  Word32 nLines4;
  Word32 ldThr, ldRatio;
  Word32 pe, constPart, nActiveLines;

  peData->pe = peData->offset;                                           
  peData->constPart = 0;                                                 
  peData->nActiveLines = 0;                                              
  for(ch=0; ch<nChannels; ch++) {
    PSY_OUT_CHANNEL *psyOutChan = &psyOutChannel[ch];
    PE_CHANNEL_DATA *peChanData = &peData->peChannelData[ch];
    const Word32 *sfbEnergy = psyOutChan->sfbEnergy;
    const Word32 *sfbThreshold = psyOutChan->sfbThreshold;

    pe = 0;                                                  
    constPart = 0;                                           
    nActiveLines = 0;                                        

    for(sfbGrp=0; sfbGrp<psyOutChan->sfbCnt; sfbGrp+=psyOutChan->sfbPerGroup) {
      for (sfb=0; sfb<psyOutChan->maxSfbPerGroup; sfb++) {
        Word32 nrg = sfbEnergy[sfbGrp+sfb];                             
        Word32 thres = sfbThreshold[sfbGrp+sfb];                           
        Word32 sfbLDEn = peChanData->sfbLdEnergy[sfbGrp+sfb];

        if (nrg > thres) {
          ldThr = iLog4(thres);

          ldRatio = sfbLDEn - ldThr;

          nLines4 = peChanData->sfbNLines4[sfbGrp+sfb];                    
           
          if (ldRatio >= C1_I) {
            peChanData->sfbPe[sfbGrp+sfb] = (nLines4*ldRatio + 8) >> 4;
            peChanData->sfbConstPart[sfbGrp+sfb] = ((nLines4*sfbLDEn)) >> 4;
          }
          else {
            peChanData->sfbPe[sfbGrp+sfb] = extract_l((L_mpy_ls(
                    (C2_I + C3_I * ldRatio * 2) << 4, nLines4) + 8) >> 4);
            peChanData->sfbConstPart[sfbGrp+sfb] = extract_l(( L_mpy_ls(
                    (C2_I + C3_I * sfbLDEn * 2) << 4, nLines4) + 8) >> 4);
            nLines4 = (nLines4 * C3_I + (1024<<1)) >> 10;
          }
          peChanData->sfbNActiveLines[sfbGrp+sfb] = nLines4 >> 2;
        }
        else {
          peChanData->sfbPe[sfbGrp+sfb] = 0;                             
          peChanData->sfbConstPart[sfbGrp+sfb] = 0;                      
          peChanData->sfbNActiveLines[sfbGrp+sfb] = 0;                   
        }
        pe = pe + peChanData->sfbPe[sfbGrp+sfb];
        constPart = constPart + peChanData->sfbConstPart[sfbGrp+sfb];
        nActiveLines = nActiveLines + peChanData->sfbNActiveLines[sfbGrp+sfb];
      }
    }
	
	peChanData->pe = pe;                                                  
    peChanData->constPart = constPart;                                           
    peChanData->nActiveLines = nActiveLines;                                        

    peData->pe = peData->pe + pe;
    peData->constPart = peData->constPart + constPart;
    peData->nActiveLines = peData->nActiveLines + nActiveLines;
  } 
}
