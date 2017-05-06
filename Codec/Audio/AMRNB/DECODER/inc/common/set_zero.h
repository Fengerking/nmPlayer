/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : set_zero.h
*      Description      : Set vector x[] to zero
*
*
********************************************************************************
*/
#ifndef set_zero_h
#define set_zero_h "$Id $"
#ifdef VOI_OPT
#define VOI_OPT_SET_ZERO 1
#endif
#include "typedef.h"
#if !VOI_OPT_SET_ZERO
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/

 
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
 
void voAMRNBDecSetZero (
    Word16 x[],        /* (o)  : vector to clear                            */
    Word16 L           /* (i)  : length of vector                           */
);
#else//VOI_OPT_SET_ZERO
#include "basic_op.h"
#include "count.h"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
extern __inline void voAMRNBDecSetZero (
    Word16 x[],         /* (o)    : vector to clear     */
    Word16 L            /* (i)    : length of vector    */
)
{
    memset(x,0,L<<1);
}
#endif//VOI_OPT_SET_ZERO
#endif
