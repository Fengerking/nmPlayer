/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : ph_disp.h
*      Purpose          : Phase dispersion of excitation signal
*
********************************************************************************
*/

#ifndef ph_disp_h
#define ph_disp_h "$Id $"
#ifdef VOI_OPT
#define VOI_OPT_PH_DISP 1
#endif
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "mode.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define PHDGAINMEMSIZE 5
#define PHDTHR1LTP     9830  /* 0.6 in Q14 */
#define PHDTHR2LTP     14746 /* 0.9 in Q14 */
#define ONFACTPLUS1    16384 /* 2.0 in Q13   */
#define ONLENGTH 2
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
typedef struct {
  Word16 gainMem[PHDGAINMEMSIZE];
  Word16 prevState;
  Word16 prevCbGain;
  Word16 lockFull;
  Word16 onset;
} ph_dispState;
#if !VOI_OPT_PH_DISP
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
/*************************************************************************
*
*  Function:   ph_disp_init
*  Purpose:    Allocates state memory and initializes state memory
*
**************************************************************************
*/
int voAMRNBEnc_ph_disp_init (ph_dispState **state);

/*************************************************************************
*
*  Function:   ph_disp_reset
*  Purpose:    Initializes state memory
*
**************************************************************************
*/
int voAMRNBEnc_ph_disp_reset (ph_dispState *state);

/*************************************************************************
*
*  Function:   ph_disp_exit
*  Purpose:    The memory used for state memory is freed
*
**************************************************************************
*/
void voAMRNBEnc_ph_disp_exit (ph_dispState **state);

/*************************************************************************
*
*  Function:   ph_disp_lock
*  Purpose:    mark phase dispersion as locked in state struct
*
**************************************************************************
*/
void voAMRNBEnc_ph_disp_lock (ph_dispState *state);

/*************************************************************************
*
*  Function:   ph_disp_release
*  Purpose:    mark phase dispersion as unlocked in state struct
*
**************************************************************************
*/
void voAMRNBEnc_ph_disp_release (ph_dispState *state);

/*************************************************************************
*
*  Function:   ph_disp
*  Purpose:    perform phase dispersion according to the specified codec
*              mode and computes total excitation for synthesis part
*              if decoder
*
**************************************************************************
*/
void voAMRNBEnc_ph_disp (
      ph_dispState *state, /* i/o     : State struct                     */
      enum Mode mode,      /* i       : codec mode                       */
      Word16 x[],          /* i/o Q0  : in:  LTP excitation signal       */
                           /*           out: total excitation signal     */
      Word16 cbGain,       /* i   Q1  : Codebook gain                    */
      Word16 ltpGain,      /* i   Q14 : LTP gain                         */
      Word16 inno[],       /* i   Q13 : Innovation vector (Q12 for 12.2) */
      Word16 pitch_fac,    /* i   Q14 : pitch factor used to scale the
                                        LTP excitation (Q13 for 12.2)    */
      Word16 tmp_shift     /* i   Q0  : shift factor applied to sum of   
                                        scaled LTP ex & innov. before
                                        rounding                         */
);
#else//VOI_OPT_PH_DISP
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "basic_op.h"
#include "count.h"
#include "cnst.h"
#include "copy.h"

/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/

#include "ph_disp.tab"

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  Function:   ph_disp_init
*
**************************************************************************
*/
__inline int ph_disp_init (ph_dispState **state)
{
  ph_dispState *s;
 
#if OUTPUT_ERROE
  if (state == (ph_dispState **) NULL){
      fprintf(stderr, "ph_disp_init: invalid parameter\n");
      return -1;
  }

  *state = NULL;

  /* allocate memory */
  if ((s= (ph_dispState *) AlignedMalloc(sizeof(ph_dispState))) == NULL){
      fprintf(stderr, "ph_disp_init: can not malloc state structure\n");
      return -1;
  }
#else
  s= (ph_dispState *)  AlignedMalloc(sizeof(ph_dispState));
#endif
  ph_disp_reset(s);
  *state = s;

  return 0;
  
}

/*************************************************************************
*
*  Function:   ph_disp_reset
*
**************************************************************************
*/
__inline int ph_disp_reset (ph_dispState *state)
{
  Word16 i;
#if OUTPUT_ERROE
   if (state == (ph_dispState *) NULL){
      fprintf(stderr, "ph_disp_reset: invalid parameter\n");
      return -1;
   }
#endif
   for (i=0; i<PHDGAINMEMSIZE; i++)
   {
       state->gainMem[i] = 0;
   }
   state->prevState = 0;
   state->prevCbGain = 0;
   state->lockFull = 0;
   state->onset = 0;          /* assume no onset in start */ 

   return 0;
}

/*************************************************************************
*
*  Function:   ph_disp_exit
*
**************************************************************************
*/
__inline void ph_disp_exit (ph_dispState **state)
{
  if ((state == NULL) || (*state == NULL))
      return;
  
  /* deallocate memory */
  AlignedFree(*state);
  *state = NULL;
  
  return;
}
/*************************************************************************
*
*  Function:   ph_disp_lock
*
**************************************************************************
*/
__inline void ph_disp_lock (ph_dispState *state)
{
  state->lockFull = 1;
  return;
}

/*************************************************************************
*
*  Function:   ph_disp_release
*
**************************************************************************
*/
__inline void ph_disp_release (ph_dispState *state)
{
  state->lockFull = 0;
  return;
}



#endif//VOI_OPT_PH_DISP


#endif
