#ifndef __SF_ESTIM_H__
#define __SF_ESTIM_H__
/*
   Scale factor estimation 
 */
#include "psy_const.h"
#include "interface.h"
#include "qc_data.h"

void
CalcFormFactor(Word16          logSfbFormFactor[MAX_CHANNELS][MAX_GROUPED_SFB],
               Word16          sfbNRelevantLines[MAX_CHANNELS][MAX_GROUPED_SFB],
               Word16          logSfbEnergy[MAX_CHANNELS][MAX_GROUPED_SFB],
               PSY_OUT_CHANNEL psyOutChannel[MAX_CHANNELS],
               const Word16    nChannels);

void
EstimateScaleFactors(PSY_OUT_CHANNEL psyOutChannel[MAX_CHANNELS],
                     QC_OUT_CHANNEL  qcOutChannel[MAX_CHANNELS],
                     Word16          logSfbEnergy[MAX_CHANNELS][MAX_GROUPED_SFB],
                     Word16          logSfbFormFactor[MAX_CHANNELS][MAX_GROUPED_SFB],
                     Word16          sfbNRelevantLines[MAX_CHANNELS][MAX_GROUPED_SFB],
                     const Word16    nChannels);
#endif