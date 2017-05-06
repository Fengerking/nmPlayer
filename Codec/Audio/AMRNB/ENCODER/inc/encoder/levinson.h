/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : levinson.h
*      Purpose          : Levinson-Durbin algorithm in double precision.
*                       : To compute the LP filter parameters from the
*                       : speech autocorrelations.
*
********************************************************************************
*/
#ifndef levinson_h
#define levinson_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "cnst.h"
#include "cod_amr.h"
 
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
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
 
int voAMRNBEnc_Levinson_init (LevinsonState **st, VO_MEM_OPERATOR *pMemOP);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to Levinson in each call.
   returns 0 on success
 */
 
int voAMRNBEnc_Levinson_reset (LevinsonState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */
void voAMRNBEnc_Levinson_exit (LevinsonState **st, VO_MEM_OPERATOR *pMemOP);
/* de-initialize pre processing state (i.e. free status struct)
   stores NULL in *st
 */

int voAMRNBEnc_Levinson (
    LevinsonState *st,
    Word16 Rh[],       /* i : Rh[m+1] Vector of autocorrelations (msb) */
    Word16 Rl[],       /* i : Rl[m+1] Vector of autocorrelations (lsb) */
    Word16 A[],        /* o : A[m]    LPC coefficients  (m = 10)       */
    Word16 rc[]        /* o : rc[4]   First 4 reflection coefficients  */
);
 
#endif
