/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : ton_stab.h
*      Purpose          : Tone stabilization routines 
*
********************************************************************************
*/
#ifndef ton_stab_h
#define ton_stab_h "$Id $"
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"
#include "cnst.h"

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

/* state variable */
typedef struct {

   /* counters */
   Word16 count;
   
   /* gain history Q11 */
   Word16 gp[N_FRAME];
   
} tonStabState;

/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
int voAMRNBEnc_ton_stab_init (tonStabState **st, VO_MEM_OPERATOR *pMemOP);
/* initialize one instance of the pre processing state.
   Stores pointer to filter status struct in *st. This pointer has to
   be passed to ton_stab in each call.
   returns 0 on success
 */
 
int voAMRNBEnc_ton_stab_reset (tonStabState *st);
/* reset of pre processing state (i.e. set state memory to zero)
   returns 0 on success
 */

void voAMRNBEnc_ton_stab_exit (tonStabState **st, VO_MEM_OPERATOR *pMemOP);
/* de-initialize pre processing state (i.e. free status struct)
   stores NULL in *st
 */

Word16 voAMRNBEnc_check_lsp(tonStabState *st, /* i/o : State struct            */
                 Word16 *lsp       /* i   : unquantized LSP's       */
);

Word16 voAMRNBEnc_check_gp_clipping(tonStabState *st, /* i/o : State struct            */
                         Word16 g_pitch    /* i   : pitch gain              */
);

void voAMRNBEnc_update_gp_clipping(tonStabState *st, /* i/o : State struct            */
                        Word16 g_pitch    /* i   : pitch gain              */
);
#endif
