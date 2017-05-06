#ifndef  ___CONFIG_H___
#define  ___CONFIG_H___

#define SAMP_PER_FRAME       1152
#define SAMPL2                576
#define SBLIMIT                32
#define HTN                    16

#define MAX_NGRAN				2	
#define MAX_NCHAN				2

#define shft4(x)    ((x +     8) >>  4)
#define shft9(x)    ((x +   256) >>  9)
#define shft13(x)   ((x +  4096) >> 13)
#define shft15(x)   ((x + 16384) >> 15)
#define shft16(x)   ((x + 32768) >> 16)
#define shft_n(x,n) ((x) >> n)
#define SQRT        724 

#ifndef ABS
#define ABS(a)	((a) >= 0 ? (a) : -(a))
#endif

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))
#endif

#define BUFFER_GUARD	8
#define BUFFER_MDLEN	(512 + 2048 + BUFFER_GUARD)
#define BUFFER_DATA		MAX_NCHAN*(SAMP_PER_FRAME + 512)
#define BUFFER_HEAD		128

#endif