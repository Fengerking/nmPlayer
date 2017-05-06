/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/


#ifndef _MBPREDICTION_H_
#define _MBPREDICTION_H_
#include "../voMpegEnc.h"
#include "../voUtilits.h"


// very large value
#define MV_MAX_ERROR	(4096 * 256)

#define VOGETMEDIAN(median,a,b,c)\
    if((a) > (b)) {\
        if((b) > (c))\
            median = (b);\
        else {\
            if((a) > (c))\
                median = (c);\
            else\
                median = (a);\
            }\
    }\
    else {\
        if((a) >= (c))\
            median = (a);\
        else {\
            if((b) <= (c))\
                median = (b);\
            else\
                median = (c);\
            }\
    }


VO_VOID ACDCPrediction(ENCHND * enc_hnd,
			                MACROBLOCK *pMB,
			                const VO_U32 al_flag,
			                const VO_U32 x,
			                const VO_U32 y,
			                const VO_U32 mb_width,
			                VO_S16 * const qcoeff);

VECTOR GetBlockPredictMV(const MACROBLOCK * const mb,
		                            const VO_S32 mb_width,
		                            const VO_S32 x,
		                            const VO_S32 y,
		                            const VO_S32 block);

#endif							// _MBPREDICTION_H_ 
