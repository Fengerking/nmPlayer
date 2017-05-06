#ifndef __RESAMPLE_H__
#define __RESAMPLE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voResample.h"
#include "voMalloc.h"
#include "commontables.h"

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUG_printf(x) printf(x)
#else
#define DEBUG_printf(x)
#endif	//defined(DEBUG) || defined(_DEBUG)

#define MAXFILLEN	128
#define PI10		3217 /* 3.1415926*1024 */
#define MAX_CHANNEL	8
#define MAXMULTIPLE	6

#define CLIPTOSHORT(x)  ((((x) >> 31) == (x >> 15))?(x):((x) >> 31) ^ 0x7fff)

#define MUL_10(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 <<  9)) >> 10)
#define MUL_20(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 19)) >> 20)
#define MUL_28(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 27)) >> 28)
#define MUL_29(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 28)) >> 29)
#define MUL_30(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 29)) >> 30)
#define MUL_31(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)+(1 << 30)) >> 31)
#define MUL_32(A,B) (VO_S32)(((VO_S64)(A)*(VO_S64)(B)) >> 32)

typedef struct _FIRFilter{
	int		length;
	int     Scale;
	int		*Coef;
	short	*RemainBuf;
	int		nRemain;
}FIRFilter;



typedef struct _ResInfo{
	//FirterInfo 
	FIRFilter *filter;
	int		  level;
	int		  Filtlen;

	int		  init;

	//samplerate
	int		  UplastSimple[MAX_CHANNEL][3];
	int		  InRate;
	int		  OutRate;
	int		  Scale;
	int		  inv_Scale;
	int		  RateUp2Down;		// down -1, up 1, mutiple2 2, mutiplex 3, nothing 0
	int		  lastpos;
	int		  UplastScale;
	int		  UpInterScale[MAXMULTIPLE];
	int		  UpMulti;
	//channel
	int			m_cDstChannelInChDnMix;
	int			**m_rgrgfltChDnMix;

//	int		  ChanMap[MAX_CHANNEL];
//	int		  ChanPos[MAX_CHANNEL];
//	int		  ChanMapSet;
	int		  ChanUpDown;	// need mix 1, nothing 0 
//	int		  ChanIndex;
	int		  ChanIn;
	int		  ChanOut;
	VO_U32		ChanInMask;
	VO_U32		ChanOutMask;
	VO_S32		*m_pChTemp;
	int		  ChanSel;

	//buffer
	short	  *input;
	int		  inlen;
	short	  *tmpbuf;
	int		  tmplen;
	short	  *tmpbuf1;
	short     *pTemp;
	short     *pTemp1;
	short     *pLtemp;
	short     *pLtemp1;
	int		  tmplen1;
	int       *nLstate1;
	int       *nRstate1;
	int       *nLstate2;
	int       *nRstate2;
	int       *nLstate3;
	int       *nRstate3;

	int (*Interpolation)(int, int, int);
	VO_MEM_OPERATOR *pvoMemop;
}ResInfo;

extern const int FilterLength[12];
extern const int sampRateTab[12];
//extern const int ChanMapTab[8][8];
extern const int SinTab[1024];
extern const int HammTab[127];
//extern const VO_AUDIO_CHANNELTYPE ChanIndex[10];



int InitFilter(FIRFilter *filter, int FilterLen, int scale);
int FilterProcess(FIRFilter *filter, short *input, int pos, int nch, int InStride);

int InitResample(ResInfo *pRes);
int ResampleCore(ResInfo *pRes, short* outbuffer);
int ChanCore(ResInfo *pRes, short* inbuffer, int length, short* outbuffer);

int LinerInterpolation(int val1, int val2, int scale);
int CosInterpolation(int val1, int val2, int scale);
int RateCore(ResInfo *pRes, short* inbuffer, int length, int InStride, short* outbuffer);

VO_S32 InitChanDnUpMix(ResInfo *pRes);
VO_S32	ChannelDownMixType(ResInfo *pRes, VO_U8 *pSrc, VO_U32 nBlocks, VO_U8 *pDst);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif