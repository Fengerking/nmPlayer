/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : spreproc.h
*      Purpose          : Subframe preprocessing
*
********************************************************************************
*/
#ifndef spreproc_h
#define spreproc_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "cnst.h"
#include "mode.h"
#include "typedef.h"
#include "cod_amr.h"

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int voAMRNBEnc_subframePreProc(
	cod_amrState *amr_st,
    enum Mode mode,            /* i  : coder mode                            */
    Word16 *A,                 /* i  : A(z) unquantized for the 4 subframes  */
    Word16 *Aq,                /* i  : A(z)   quantized for the 4 subframes  */
    Word16 i_subfr,
    Word16 xn[],               /* o  : target vector for pitch search        */
    Word16 res2[]              /* o  : long term prediction residual         */
);
#endif
