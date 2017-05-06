#ifndef __UTILS_H__
#define __UTILS_H__

#include "voType.h"
#include "h265dec_porting.h"

static VO_S32 xLog2(VO_U32 x)
{
    VO_U32 log2Size = 0;
    while(x > 0) {
        x >>= 1;
        log2Size++;
    }
    return(log2Size);
}

static VOINLINE VO_S32 Clip3( VO_S32 minVal, VO_S32 maxVal, VO_S32 a )
{
// 	if ( a < minVal )
// 		a = minVal;
// 	else if ( a > maxVal )
// 		a = maxVal;
// 	return a;
	return  ((a) >= (minVal) ? ((a) <= (maxVal) ? (a) : (maxVal)): (minVal));
}
// #define Clip3(a,b,x)   ((x) < (a) ? (a) : ((x) > (b) ? (  b ) : (x)))

#define Clip(x)         Clip3( 0, 255, (x))
#define xSHR(x, n)      ( (n)>=32 ? 0 : ((x)>>(n)) )
#define xSHL(x, n)      ( (n)>=32 ? 0 : ((x)<<(n)) )
#define MAX(a, b)       ( (a) > (b) ? (a) : (b) )
#define MIN(a, b)       ( (a) < (b) ? (a) : (b) )
#define MALLOC(n)       malloc(n)
#define FREE(p)         free(p)
#define ASIZE(x)        ( sizeof(x)/sizeof((x)[0]) )

#define ABS(X)    (((X)>0)?(X):-(X)) 

#endif /* __UTILS_H__ */
