/*
   Threshold compensation
 */
#ifndef __ADJ_THR_H
#define __ADJ_THR_H

#include "adj_thr_data.h"
#include "qc_data.h"
#include "interface.h"

Word16 bits2pe(const Word16 bits);

Word32 AdjThrNew(ADJ_THR_STATE** phAdjThr,
                 Word32 nElements);

void AdjThrDelete(ADJ_THR_STATE *hAdjThr);

void AdjThrInit(ADJ_THR_STATE *hAdjThr,
                const Word32 peMean,
                Word32 chBitrate);

void AdjustThresholds(ADJ_THR_STATE *adjThrState,
                      ATS_ELEMENT* AdjThrStateElement,
                      PSY_OUT_CHANNEL psyOutChannel[MAX_CHANNELS],
                      PSY_OUT_ELEMENT *psyOutElement,
                      Word16 *chBitDistribution,
                      Word16 logSfbEnergy[MAX_CHANNELS][MAX_GROUPED_SFB],
                      Word16 sfbNRelevantLines[MAX_CHANNELS][MAX_GROUPED_SFB],                      
                      QC_OUT_ELEMENT* qcOE,
					  ELEMENT_BITS* elBits,
					  const Word16 nChannels,
                      const Word16 maxBitFac);

void AdjThrUpdate(ATS_ELEMENT *AdjThrStateElement,
                  const Word16 dynBitsUsed);


#endif
