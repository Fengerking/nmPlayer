/**********************************************************************
*
* VisualOn voplayer
*
* CopyRight (C) VisualOn SoftWare Co., Ltd. All rights are reserved.
*
***********************************************************************/

/*********************************************************************
*
* Filename:	
*		commontables.c
* 
* Abstact:	
*
*		Channel up and down mix functions and definations.
*
* Author:
*
*		Witten Wen 2010-7-27
*
* Revision History:
*
************************************************************/
#ifndef __COMMON_TABLES_H_
#define __COMMON_TABLES_H_
#include "voResample.h"

#define MULT_HI32_SHIFT(a, b, bits)	(((VO_S64)(a) * (b)) >> (bits))
extern VO_U16 sine_table[];
extern int lookup_sine(int x);
extern VO_S16 CosTab[];

#endif	//__COMMON_TABLES_H_