/*
**
** File:        "dec_cng.h"
**
** Description:     Function prototypes for "dec_cng.c"
**
*/
/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/
#ifndef __DEC_CNG_H__
#define __DEC_CNG_H__

void Init_Dec_Cng(DECCNGDEF *DecCng);
void Dec_Cng(Word16 Ftyp, LINEDEF *Line, G723DncState *st, Word16 *QntLpc);

#endif  //__DEC_CNG_H__
