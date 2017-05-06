/*
   Spreading of energy
 */
#include "basic_op.h"
#include "oper_32b.h"
#include "spreading.h"


void SpreadingMax(const Word16 pbCnt,
                  const Word16 *maskLowFactor,
                  const Word16 *maskHighFactor,
                  Word32       *pbSpreadedEnergy)
{
  Word32 i;

  /* slope to higher frequencies */
  for (i=1; i<pbCnt; i++) {
    pbSpreadedEnergy[i] = max(pbSpreadedEnergy[i],
                                L_mpy_ls(pbSpreadedEnergy[i-1], maskHighFactor[i]));
  }
  /* slope to lower frequencies */
  for (i=pbCnt - 2; i>=0; i--) {
    pbSpreadedEnergy[i] = max(pbSpreadedEnergy[i],
                                L_mpy_ls(pbSpreadedEnergy[i+1], maskLowFactor[i]));
  }
}
