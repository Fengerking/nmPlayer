
/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.0
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/
#ifndef __TAME_H__
#define __TAME_H__

void   Update_Err(Word16 Olp, Word16 AcLg, Word16 AcGn, G723EncState *st);
Word16 Test_Err(Word16 Lag1, Word16 Lag2, CODSTATDEF *CodStat);

#endif //__TAME_H__
