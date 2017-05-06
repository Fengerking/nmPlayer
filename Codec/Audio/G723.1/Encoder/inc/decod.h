/*
**
** File:        "decod.h"
**
** Description:     Function prototypes and external declarations 
**          for "decod.c"
**  
*/


/*
    ITU-T AV.25Y Speech Coder   ANSI-C Source Code     Version 5.00
    copyright (c) 1995, AudioCodes, DSP Group, France Telecom,
    Universite de Sherbrooke.  All rights reserved.
*/
#ifndef  __DECOD_H__
#define  __DECOD_H__

void  	Init_Decod(void);
Flag    Decod( Word16 *DataBuff, char *Vinp, Word16 Crc );
extern 	DECSTATDEF  DecStat  ;

#endif //__DECOD_H__

