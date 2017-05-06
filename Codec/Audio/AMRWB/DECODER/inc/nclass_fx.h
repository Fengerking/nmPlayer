#ifndef _NCLASS_FX_H_
#define _NCLASS_FX_H_

#include "typedef.h"
#include "cnst_fx.h"

#define TWINLEN (24-8)
#define LPHLEN 4
#define LPHAVELEN 8

#define TWINLENSHORT (4)
#define COMPLEN 12

// For debugging
#define COMPLEN2 12
#define CS 0

#define ACELP_MODE 0
#define TCX_MODE 1
#define TCX_OR_ACELP 2

typedef struct
{
  Word16 levelHist[TWINLEN][COMPLEN];
  Word16 averageHistTime[COMPLEN];
  Word16 stdDevHistTime[COMPLEN];
  Word16 averageHistTimeShort[COMPLEN];
  Word16 stdDevHistTimeShort[COMPLEN];
  Word16 lphBuf[LPHLEN];
  Word16 lphAveBuf[LPHAVELEN];
  Word16 prevModes[4];
  Word16 vadFlag[4];
  Word16 vadFlag_old[4];
  Word16 LTPLag[10];
  Word16 NormCorr[10];
  Word16 LTPGain[10];
  Word32 TotalEnergy[5];
  Word16 NoMtcx[2];
  Word16 NbOfAcelps;
  Word16 ApBuf[4 * M];
  Word16 lph[4];
  Word16 StatClassCount;

  Word16 LTPlagV[8];

} NCLASSDATA_FX;

Word16 ClassifyExcitation(
    NCLASSDATA_FX *stClass,         /* i/o : Static variables  */
    Word16 levelNew[],              /* i/o : Energy level      */
    Word16 sfIndex                  /* i   : Subfr index       */
);
void ClassifyExcitationRef(
    NCLASSDATA_FX *stClass,     /* i/o : Static variables   */
    Word16 *ISPs_,              /* i   : Isp coefficients   */
    Word16 *headMode            /* i/o : Mode choosen       */
);
void InitClassifyExcitation(NCLASSDATA_FX * stClass);

#endif /*_NCLASS_H_*/
