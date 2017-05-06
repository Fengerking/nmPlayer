/*
   Short block grouping
 */
#ifndef __GRP_DATA_H__
#define __GRP_DATA_H__
#include "psy_data.h"
#include "typedefs.h"

void
groupShortData(Word32        *mdctSpectrum,
               Word32        *tmpSpectrum,
               SFB_THRESHOLD *sfbThreshold,
               SFB_ENERGY    *sfbEnergy,
               SFB_ENERGY    *sfbEnergyMS,
               SFB_ENERGY    *sfbSpreadedEnergy,
               const Word16   sfbCnt,
               const Word16  *sfbOffset,
               const Word16  *sfbMinSnr,
               Word16        *groupedSfbOffset,
               Word16        *maxSfbPerGroup,
               Word16        *groupedSfbMinSnr,
               const Word16   noOfGroups,
               const Word16  *groupLen);

#endif /* _INTERFACE_H */
