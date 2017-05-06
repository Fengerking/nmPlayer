/*
   Psychoacoustic data
 */
#ifndef _PSY_DATA_H
#define _PSY_DATA_H

#include "block_switch.h"
#include "tns.h"

/*
  the structs can be implemented as unions
*/

typedef struct{
  Word32 sfbLong[MAX_GROUPED_SFB];
  Word32 sfbShort[TRANS_FAC][MAX_SFB_SHORT];
}SFB_THRESHOLD; /* Word16 size: 260 */

typedef struct{
  Word32 sfbLong[MAX_GROUPED_SFB];
  Word32 sfbShort[TRANS_FAC][MAX_SFB_SHORT];
}SFB_ENERGY; /* Word16 size: 260 */

typedef struct{
  Word32 sfbLong;
  Word32 sfbShort[TRANS_FAC];
}SFB_ENERGY_SUM; /* Word16 size: 18 */


typedef struct{
  BLOCK_SWITCHING_CONTROL   blockSwitchingControl;          /* block switching */
  Word16                    *mdctDelayBuffer;               /* mdct delay buffer [BLOCK_SWITCHING_OFFSET]*/
  Word32                    sfbThresholdnm1[MAX_SFB];       /* PreEchoControl */
  Word16                    mdctScalenm1;                   /* scale of last block's mdct (PreEchoControl) */

  SFB_THRESHOLD             sfbThreshold;                   /* adapt           */
  SFB_ENERGY                sfbEnergy;                      /* sfb Energy      */
  SFB_ENERGY                sfbEnergyMS;
  SFB_ENERGY_SUM            sfbEnergySum;
  SFB_ENERGY_SUM            sfbEnergySumMS;
  SFB_ENERGY                sfbSpreadedEnergy;

  Word32                    *mdctSpectrum;                  /* mdct spectrum [FRAME_LEN_LONG] */
  Word16                    mdctScale;                      /* scale of mdct   */
}PSY_DATA; /* Word16 size: 4 + 87 + 102 + 360 + 360 + 360 + 18 + 18 + 360 = 1669 */

#endif /* _PSY_DATA_H */
