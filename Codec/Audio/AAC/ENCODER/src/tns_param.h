/*
   TNS parameters
 */
#ifndef _TNS_PARAM_H
#define _TNS_PARAM_H

#include "tns.h"

typedef struct{
  Word32 samplingRate;
  Word16 maxBandLong;
  Word16 maxBandShort;
}TNS_MAX_TAB_ENTRY;

typedef struct{
    Word32 bitRateFrom;
    Word32 bitRateTo;
    const TNS_CONFIG_TABULATED *paramMono_Long;  /* contains TNS parameters */
    const TNS_CONFIG_TABULATED *paramMono_Short;
    const TNS_CONFIG_TABULATED *paramStereo_Long;
    const TNS_CONFIG_TABULATED *paramStereo_Short;
}TNS_INFO_TAB;


void GetTnsParam(TNS_CONFIG_TABULATED *tnsConfigTab, 
                 Word32 bitRate, Word16 channels, Word16 blockType);

void GetTnsMaxBands(Word32 samplingRate, Word16 blockType, Word16* tnsMaxSfb);

#endif /* _TNS_PARAM_H */
