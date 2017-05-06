/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
 
#ifndef __DIVX3_VLD_H
#define __DIVX3_VLD_H
#include "voMpeg4DecGlobal.h"
#include "voMpeg4Dec.h"

#define ESCAPE 0xFF0

VO_S32 GetDivx3VLC( VO_MPEG4_DEC* pDec, const VO_U16* table );

#endif
