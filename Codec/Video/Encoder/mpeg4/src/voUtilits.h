/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _VOUTILIT_H_
#define _VOUTILIT_H_

#include "stdlib.h"
#include "math.h"

#ifdef ARCH_IS_BIG_ENDIAN
#define VOSWAPTOBIGENDIAN(X)
#else
#define VOSWAPTOBIGENDIAN(X)   X = ((X<<8)&0xFF00FF00) | ((X>>8)&0x00FF00FF); \
                                                X = (X>>16) | (X<<16);
#endif

//#define VOGETABS(a)      ((a)-(2*(a)&((a)>>31)))
#define VOGETABS(a)     abs(a)
//#define VOGETABS(a)    ( (a)<0? -(a):(a) )

#define VOH263POS(level,run)    (((level)*64)+(run))

#define VOMPEG4POS(intra,last,level,run)    (intra)*(2*64*64)+(last)*64*64+(level)*64+(run)

#define MIN(X, Y) ((X)<(Y)?(X):(Y))
#define MAX(X, Y) ((X)>(Y)?(X):(Y))
#define SIGN(X)   (((X)>0)?1:-1)

#define VOCLAMP(Value,Min,Max) \
    if ((Value) < (Min)) { \
		(Value) = (Min);\
	} else if ((Value) > (Max)) {\
		(Value) = (Max);\
	}

#define VOCLAMP_UP(Value,Max) \
    if ((Value) > (Max)) {\
		(Value) = (Max);\
	}

#define VOCLAMP_DOWN(Value,Min) \
    if ((Value) < (Min)) { \
		(Value) = (Min);\
    }

#endif//_PROTECT_H_
