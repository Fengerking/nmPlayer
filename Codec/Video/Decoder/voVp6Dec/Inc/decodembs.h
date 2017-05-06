/****************************************************************************
*
*   Module Title :     decodembs.h
*
*   Description  :     platform specific decode mbs header file
*
****************************************************************************/
#ifndef __INC_DECODEMBS_H 
#define __INC_DECODEMBS_H

extern void  (*VP6_D_ReadTokens_MB)(PB_INSTANCE *pbi, int *EOBPos);
extern void (*VP6_D_ResetAboveContext)(PB_INSTANCE *pbi);
extern void (*VP6_D_ResetLeftContext)(PB_INSTANCE *pbi);

#endif
