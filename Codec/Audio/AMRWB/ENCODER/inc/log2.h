
/***********************************************************************
*                                                                      *
*    VisualOn, Inc. Confidential and Proprietary, 2003-2010            *
*                                                                      *
************************************************************************/

/********************************************************************************
*
*      File             : log2.h
*      Purpose          : Computes log2(L_x)
*
********************************************************************************
*/
#ifndef __LOG2_H__
#define __LOG2_H__
 
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
 
/*
********************************************************************************
*                         DEFINITION OF DATA TYPES
********************************************************************************
*/
 
/*
********************************************************************************
*                         DECLARATION OF PROTOTYPES
********************************************************************************
*/
void Log2 (
		Word32 L_x,        /* (i) : input value                                 */
		Word16 *exponent,  /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
		Word16 *fraction   /* (o) : Fractional part of Log2. (range: 0<=val<1)*/
	  );

void Log2_norm (
		Word32 L_x,         /* (i) : input value (normalized)                    */
		Word16 exp,         /* (i) : norm_l (L_x)                                */
		Word16 *exponent,   /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
		Word16 *fraction    /* (o) : Fractional part of Log2. (range: 0<=val<1)  */
	       );

#endif  //__LOG2_H__


