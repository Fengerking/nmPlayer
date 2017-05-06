/*
**
** File:        "vad.h"
**
** Description:     Function prototypes for "vad.c"
**  
*/

/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/
#ifndef __VAD_H__
#define __VAD_H__

void    Init_Vad(VADSTATDEF *VadStat);
Flag    Comp_Vad(G723EncState *Dpnt);

#endif  //__VAD_H__
