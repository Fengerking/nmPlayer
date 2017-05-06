/*
   Spreading of energy
 */
#ifndef _SPREADING_H
#define _SPREADING_H
#include "typedefs.h"


void SpreadingMax(const Word16 pbCnt,
                  const Word16 *maskLowFactor,
                  const Word16 *maskHighFactor,
                  Word32       *pbSpreadedEnergy);

#endif /* #ifndef _SPREADING_H */
