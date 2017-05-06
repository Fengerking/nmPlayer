/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : log2.h
*      Purpose          : Computes log2(L_x)
*
********************************************************************************
*/
#ifndef log2_h
#define log2_h "$Id $"

/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
 
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/

void voAMRNBDecLog2 (
    Word32 L_x,        /* (i) : input value                                 */
    Word16 *exponent,  /* (o) : Integer part of voAMRNBDecLog2.   (range: 0<=val<=30) */
    Word16 *fraction   /* (o) : Fractional part of voAMRNBDecLog2. (range: 0<=val<1)*/
);

void voAMRNBDecLog2_norm (
    Word32 L_x,         /* (i) : input value (normalized)                    */
    Word16 exp,         /* (i) : norm_l (L_x)                                */
    Word16 *exponent,   /* (o) : Integer part of voAMRNBDecLog2.   (range: 0<=val<=30) */
    Word16 *fraction    /* (o) : Fractional part of voAMRNBDecLog2. (range: 0<=val<1)  */
);

#endif
