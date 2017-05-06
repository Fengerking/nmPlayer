/*
**
** File:        "cod_cng.h"
**
** Description:     Function prototypes for "cod_cng.c"
**
*/
/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/
#ifndef __COD_CNG_H__
#define __COD_CNG_H__

void 	Init_Cod_Cng(CODCNGDEF *CodCng);
void 	Cod_Cng(G723EncState *st, Word16 *Ftyp, LINEDEF *Line, Word16 *QntLpc);
void 	Update_Acf(Word16 *Acfsf, Word16 *Shsf, CODCNGDEF *CodCng);
#endif //__COD_CNG_H__

