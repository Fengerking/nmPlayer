#include "global.h"
#include "intra16x16_pred.h"
#include "mb_access.h"
#include "image.h"


extern int intrapred_16x16_normal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int predmode);

int intrapred16x16(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,  //!< Current Macroblock
                         //!< Current colorplane (for 4:4:4)                         
                   int predmode)        //!< prediction mode
{

    return intrapred_16x16_normal(pDecGlobal,pDecLocal, predmode);

}

