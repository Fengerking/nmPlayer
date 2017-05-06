/*
**
** File:        "coder.h"
**
** Description:     Function prototypes and external declarations 
**          for "coder.c"
**  
*/
/*
    ITU-T G.723 Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/
#ifndef __CODER_H__
#define __CODER_H__

void    Init_Coder(CODSTATDEF *CodStat);
int     G723_Encoder(G723EncState *st);
//Flag    Coder( Word16 *DataBuff, char *Vout );
#endif //__CODER_H__
