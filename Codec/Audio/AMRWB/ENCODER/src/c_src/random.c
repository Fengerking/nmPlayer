/***********************************************************************
*                                                                      *
*        VisualOn, Inc. Confidential and Proprietary, 2003-2010        *
*                                                                      *
************************************************************************/
/***********************************************************************
*       File: random.c                                                 *
*                                                                      *
*       Description: Signed 16 bits random generator                   *
*                                                                      *
************************************************************************/

#include "typedef.h"
#include "basic_op.h"

Word16 Random(Word16 * seed)
{
	/* static Word16 seed = 21845; */
	*seed = (Word16)(vo_L_add(((*seed * 31821)), 13849L));
	return (*seed);
}

