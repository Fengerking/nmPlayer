/*
**
** File:        "lsp.h"
**
** Description:     Function prototypes for "lpc.c"
**  
*/

/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/
#ifndef  __LSP_H__
#define  __LSP_H__

void     AtoLsp( Word16 *LspVect, Word16 *Lpc, Word16 *PrevLsp );
Word32   Lsp_Qnt( Word16 *CurrLsp, Word16 *PrevLsp );
Word32   Lsp_Svq( Word16 *Tv, Word16 *Wvect );
void     Lsp_Inq( Word16 *Lsp, Word16 *PrevLsp, Word32 LspId, Word16 Crc );
void     Lsp_Int( Word16 *QntLpc, Word16 *CurrLsp, Word16 *PrevLsp );
void     LsptoA( Word16 *Lsp );

#endif //__LSP_H__
