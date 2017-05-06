/*
   Block switching
 */
#ifndef _BLOCK_SWITCH_H
#define _BLOCK_SWITCH_H

#include "typedef.h"


/****************** Defines ******************************/
#define BLOCK_SWITCHING_IIR_LEN 2                                           /* Length of HighPass-FIR-Filter for Attack-Detection */
#define BLOCK_SWITCH_WINDOWS TRANS_FAC                                      /* number of windows for energy calculation */
#define BLOCK_SWITCH_WINDOW_LEN FRAME_LEN_SHORT                             /* minimal granularity of energy calculation */



/****************** Structures ***************************/
typedef struct{
  Word32 invAttackRatio;
  Word16 windowSequence;
  Word16 nextwindowSequence;
  Flag attack;
  Flag lastattack;
  Word16 attackIndex;
  Word16 lastAttackIndex;
  Word16 noOfGroups;
  Word16 groupLen[TRANS_FAC];
  Word32 windowNrg[2][BLOCK_SWITCH_WINDOWS];     /* time signal energy in Subwindows (last and current) */
  Word32 windowNrgF[2][BLOCK_SWITCH_WINDOWS];    /* filtered time signal energy in segments (last and current) */
  Word32 iirStates[BLOCK_SWITCHING_IIR_LEN];     /* filter delay-line */
  Word32 maxWindowNrg;                           /* max energy in subwindows */
  Word32 accWindowNrg;                           /* recursively accumulated windowNrgF */
}BLOCK_SWITCHING_CONTROL;





Word16 InitBlockSwitching(BLOCK_SWITCHING_CONTROL *blockSwitchingControl,
                          const Word32 bitRate, const Word16 nChannels);

Word16 BlockSwitching(BLOCK_SWITCHING_CONTROL *blockSwitchingControl,
                      Word16 *timeSignal,
                      Word16 chIncrement);

Word16 SyncBlockSwitching(BLOCK_SWITCHING_CONTROL *blockSwitchingControlLeft,
                          BLOCK_SWITCHING_CONTROL *blockSwitchingControlRight,
                          const Word16 noOfChannels);



#endif  /* #ifndef _BLOCK_SWITCH_H */
