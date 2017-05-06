/****************************************************************************
*
*   Module Title :     context.c
*
*   Description  :     
*
****************************************************************************/

/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"

/****************************************************************************
* 
*  ROUTINE       :     BoolTreeToHuffCodes
*
*  INPUTS        :     UINT8  *BoolTreeProbs    -- Dct coeff tree node probs
*
*  OUTPUTS       :     UINT32 *HuffProbs        -- Dct coeff prob distribution
*
*  RETURNS       :     None.
*
*  FUNCTION      :     Convert zero run-length tree node probs to set 
*                      of run-length probs (run lengths 1--8, and >8
*                      are the tokens).
*
*  SPECIAL NOTES :     None. 
*
*
*  ERRORS        :     None.
*
****************************************************************************/
void 
BoolTreeToHuffCodes ( UINT8 *BoolTreeProbs, UINT32 *HuffProbs )
{
    UINT32 Prob;
    UINT32 Prob1;

    HuffProbs[DCT_EOB_TOKEN]       = ((UINT32)BoolTreeProbs[0] * (UINT32)BoolTreeProbs[1]) >> 8;
    HuffProbs[ZERO_TOKEN]          = ((UINT32)BoolTreeProbs[0] * (255 - (UINT32)BoolTreeProbs[1])) >> 8;

    Prob = (255 - (UINT32)BoolTreeProbs[0]);
    HuffProbs[ONE_TOKEN]           = (Prob * (UINT32)BoolTreeProbs[2]) >> 8;

    Prob = (Prob*(255 - (UINT32)BoolTreeProbs[2])) >> 8;
    Prob1 = (Prob * (UINT32)BoolTreeProbs[3]) >> 8;
    HuffProbs[TWO_TOKEN]           = (Prob1 * (UINT32)BoolTreeProbs[4]) >> 8; 
    Prob1 = (Prob1 * (255 - (UINT32)BoolTreeProbs[4])) >> 8;
    HuffProbs[THREE_TOKEN]         = (Prob1 * (UINT32)BoolTreeProbs[5]) >> 8;
    HuffProbs[FOUR_TOKEN]          = (Prob1 * (255 - (UINT32)BoolTreeProbs[5])) >> 8;

    Prob = (Prob * (255 - (UINT32)BoolTreeProbs[3])) >> 8;
    Prob1 = (Prob * (UINT32)BoolTreeProbs[6]) >> 8;
    HuffProbs[DCT_VAL_CATEGORY1]   = (Prob1 * (UINT32)BoolTreeProbs[7]) >> 8;
    HuffProbs[DCT_VAL_CATEGORY2]   = (Prob1 * (255 - (UINT32)BoolTreeProbs[7])) >> 8;
    
    Prob = (Prob * (255 - (UINT32)BoolTreeProbs[6])) >> 8;
    Prob1 = (Prob * (UINT32)BoolTreeProbs[8]) >> 8; 
    HuffProbs[DCT_VAL_CATEGORY3]   = (Prob1 * (UINT32)BoolTreeProbs[9]) >> 8;
    HuffProbs[DCT_VAL_CATEGORY4]   = (Prob1 * (255 - (UINT32)BoolTreeProbs[9])) >> 8;

    Prob = (Prob * (255 - (UINT32)BoolTreeProbs[8])) >> 8;
    HuffProbs[DCT_VAL_CATEGORY5]   = (Prob * (UINT32)BoolTreeProbs[10]) >> 8;
    HuffProbs[DCT_VAL_CATEGORY6]   = (Prob * (255 - (UINT32)BoolTreeProbs[10])) >> 8;
}

/****************************************************************************
* 
*  ROUTINE       :     ZerosBoolTreeToHuffCodes
*
*  INPUTS        :     UINT8  *BoolTreeProbs : Zrl tree node probabilities
*
*  OUTPUTS       :     UINT32 *HuffProbs     : Zrl run-length distribution
*
*  RETURNS       :     void
*
*  FUNCTION      :     Convert zero run-length tree node probs to set 
*                      of run-length probs (run lengths 1--8, and >8
*                      are the tokens).
*
*  SPECIAL NOTES :     None. 
*
****************************************************************************/
void 
ZerosBoolTreeToHuffCodes ( UINT8 *BoolTreeProbs, UINT32 *HuffProbs )
{
    UINT32 Prob;

    Prob  = ((UINT32)BoolTreeProbs[0] * (UINT32)BoolTreeProbs[1]) >> 8;
    HuffProbs[0] = (Prob * (UINT32)BoolTreeProbs[2]) >> 8;
    HuffProbs[1] = (Prob * (UINT32)(255 - BoolTreeProbs[2])) >> 8;

    Prob = ((UINT32)BoolTreeProbs[0] * (UINT32)(255 - BoolTreeProbs[1])) >> 8;
    HuffProbs[2] = (Prob * (UINT32)BoolTreeProbs[3]) >> 8;
    HuffProbs[3] = (Prob * (UINT32)(255 - BoolTreeProbs[3])) >> 8;

    Prob = ((UINT32)(255 - BoolTreeProbs[0]) * (UINT32)BoolTreeProbs[4]) >> 8;
    Prob = (Prob * (UINT32)BoolTreeProbs[5]) >> 8;
    HuffProbs[4] = (Prob * (UINT32)BoolTreeProbs[6]) >> 8;
    HuffProbs[5] = (Prob * (UINT32)(255 - BoolTreeProbs[6])) >> 8;

    Prob = ((UINT32)(255 - BoolTreeProbs[0]) * (UINT32)BoolTreeProbs[4]) >> 8;
    Prob = (Prob * (UINT32)(255 - BoolTreeProbs[5])) >> 8;
    HuffProbs[6] = (Prob * (UINT32)BoolTreeProbs[7]) >> 8;
    HuffProbs[7] = (Prob * (UINT32)(255 - BoolTreeProbs[7])) >> 8;

    Prob = ((UINT32)(255 - BoolTreeProbs[0]) * (UINT32)(255 - BoolTreeProbs[4])) >> 8;
    HuffProbs[8] = Prob;
}




