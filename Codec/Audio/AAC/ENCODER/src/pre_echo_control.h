/*
   Pre echo control
 */
#ifndef __PRE_ECHO_CONTROL_H
#define __PRE_ECHO_CONTROL_H

#include "typedefs.h"

void InitPreEchoControl(Word32 *pbThresholdnm1,
                        Word16  numPb,
                        Word32 *pbThresholdQuiet);


void PreEchoControl(Word32 *pbThresholdNm1,
                    Word16  numPb,
                    Word32  maxAllowedIncreaseFactor,
                    Word16  minRemainingThresholdFactor,
                    Word32 *pbThreshold,
                    Word16  mdctScale,
                    Word16  mdctScalenm1);

#endif

