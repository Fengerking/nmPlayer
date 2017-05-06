/*
   Psychoaccoustic configuration
 */
#ifndef _PSY_CONFIGURATION_H
#define _PSY_CONFIGURATION_H

#include "typedefs.h"
#include "psy_const.h"
#include "tns.h"

typedef struct{

  Word16 sfbCnt;
  Word16 sfbActive;   /* number of sf bands containing energy after lowpass */
  const Word16 *sfbOffset;

  Word32 sfbThresholdQuiet[MAX_SFB_LONG];

  Word16 maxAllowedIncreaseFactor;   /* preecho control */
  Word16 minRemainingThresholdFactor;

  Word16 lowpassLine;
  Word16 sampRateIdx;
  Word32 clipEnergy;                 /* for level dependend tmn */

  Word16 ratio;
  Word16 sfbMaskLowFactor[MAX_SFB_LONG];
  Word16 sfbMaskHighFactor[MAX_SFB_LONG];

  Word16 sfbMaskLowFactorSprEn[MAX_SFB_LONG];
  Word16 sfbMaskHighFactorSprEn[MAX_SFB_LONG];


  Word16 sfbMinSnr[MAX_SFB_LONG];       /* minimum snr (formerly known as bmax) */

  TNS_CONFIG tnsConf;

}PSY_CONFIGURATION_LONG; /*Word16 size: 8 + 52 + 102 + 51 + 51 + 51 + 51 + 47 = 515 */


typedef struct{

  Word16 sfbCnt;
  Word16 sfbActive;   /* number of sf bands containing energy after lowpass */
  const Word16 *sfbOffset;

  Word32 sfbThresholdQuiet[MAX_SFB_SHORT];

  Word16 maxAllowedIncreaseFactor;   /* preecho control */
  Word16 minRemainingThresholdFactor;

  Word16 lowpassLine;
  Word16 sampRateIdx;
  Word32 clipEnergy;                 /* for level dependend tmn */

  Word16 ratio;
  Word16 sfbMaskLowFactor[MAX_SFB_SHORT];
  Word16 sfbMaskHighFactor[MAX_SFB_SHORT];

  Word16 sfbMaskLowFactorSprEn[MAX_SFB_SHORT];
  Word16 sfbMaskHighFactorSprEn[MAX_SFB_SHORT];


  Word16 sfbMinSnr[MAX_SFB_SHORT];       /* minimum snr (formerly known as bmax) */

  TNS_CONFIG tnsConf;

}PSY_CONFIGURATION_SHORT; /*Word16 size: 8 + 16 + 16 + 16 + 16 + 16 + 16 + 16 + 47 = 167 */


/* Returns the sample rate index */
Word32 GetSRIndex(Word32 sampleRate);


Word16 InitPsyConfigurationLong(Word32 bitrate,
                                Word32 samplerate,
                                Word16 bandwidth,
                                PSY_CONFIGURATION_LONG *psyConf);

Word16 InitPsyConfigurationShort(Word32 bitrate,
                                 Word32 samplerate,
                                 Word16 bandwidth,
                                 PSY_CONFIGURATION_SHORT *psyConf);

#endif /* _PSY_CONFIGURATION_H */



