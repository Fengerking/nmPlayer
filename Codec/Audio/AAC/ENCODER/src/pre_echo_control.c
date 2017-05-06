/*
   Pre echo control
 */
#include "basic_op.h"
#include "oper_32b.h"

#include "oper_32b.h"
#include "pre_echo_control.h"

void InitPreEchoControl(Word32 *pbThresholdNm1,
                        Word16  numPb,
                        Word32 *pbThresholdQuiet)
{
  Word16 pb;

  for(pb=0; pb<numPb; pb++) {
    pbThresholdNm1[pb] = pbThresholdQuiet[pb];                                   
  }
}


void PreEchoControl(Word32 *pbThresholdNm1,
                    Word16  numPb,
                    Word32  maxAllowedIncreaseFactor,
                    Word16  minRemainingThresholdFactor,
                    Word32 *pbThreshold,
                    Word16  mdctScale,
                    Word16  mdctScalenm1)
{
  Word32 i;
  Word32 tmpThreshold1, tmpThreshold2;
  Word32 scaling;

  /* maxAllowedIncreaseFactor is hard coded to 2 */
  (void)maxAllowedIncreaseFactor;

  scaling = ((mdctScale - mdctScalenm1) << 1);
   
  if ( scaling > 0 ) {
    for(i = 0; i < numPb; i++) {
      tmpThreshold1 = pbThresholdNm1[i] >> (scaling-1);
      tmpThreshold2 = L_mpy_ls(pbThreshold[i], minRemainingThresholdFactor);

      /* copy thresholds to internal memory */
      pbThresholdNm1[i] = pbThreshold[i];                                        

       
      if(pbThreshold[i] > tmpThreshold1) {
        pbThreshold[i] = tmpThreshold1;                                          
      }
       
      if(tmpThreshold2 > pbThreshold[i]) {
        pbThreshold[i] = tmpThreshold2;                                          
      }

    }
  }
  else {
    scaling = -scaling;
    for(i = 0; i < numPb; i++) {

      tmpThreshold1 = pbThresholdNm1[i] << 1;
      tmpThreshold2 = L_mpy_ls(pbThreshold[i], minRemainingThresholdFactor);

      /* copy thresholds to internal memory */
      pbThresholdNm1[i] = pbThreshold[i];                                        

       
      if(((pbThreshold[i] >> scaling) > tmpThreshold1)) {
        pbThreshold[i] = tmpThreshold1 << scaling;
      }
       
      if(tmpThreshold2 > pbThreshold[i]) {
        pbThreshold[i] = tmpThreshold2;                                          
      }

    }
  }
}

