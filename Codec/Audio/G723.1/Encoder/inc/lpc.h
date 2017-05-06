/*
**
** File:        "lpc.h"
**
** Description:     Function prototypes for "lpc.c"
**  
*/

/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/
#ifndef __LPC_H__
#define __LPC_H__

void    Comp_Lpc( Word16 *UnqLpc, G723EncState *st);
Word16  Durbin( Word16 *Lpc, Word16 *Corr, Word16 Err, Word16 *Pk2 );
void    Wght_Lpc( Word16 *PerLpc, Word16 *UnqLpc );
void    Error_Wght( G723EncState *st, Word16 *PerLpc );
void    Comp_Ir( Word16 *ImpResp, Word16 *QntLpc, Word16 *PerLpc, PWDEF Pw );
void    Sub_Ring( Word16 *Dpnt, Word16 *QntLpc, Word16 *PerLpc, CODSTATDEF *CodStat, PWDEF Pw );
void    Upd_Ring( Word16 *Dpnt, Word16 *QntLpc, Word16 *PerLpc, CODSTATDEF *CodStat);
void    Synt( Word16 *Dpnt, Word16 *Lpc );
Word32  Spf( Word16 *Tv, Word16 *Lpc );

#endif //__LPC_H__
