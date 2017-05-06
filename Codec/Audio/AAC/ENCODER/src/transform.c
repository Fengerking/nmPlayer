/*
   MDCT Transform
 */
#include "basic_op.h"
#include "psy_const.h"
#include "transform.h"
#include "aac_rom.h"


#define LS_TRANS ((FRAME_LEN_LONG-FRAME_LEN_SHORT)/2) /* 448 */
#define SQRT1_2 0x5a82799a	/* sqrt(1/2) in Q31 */
#define swapcplx(p0,p1) \
	t = p0; t1 = *(&(p0)+1); p0 = p1; *(&(p0)+1) = *(&(p1)+1); p1 = t; *(&(p1)+1) = t1

void BitReverse(int *inout, int tabidx)
{
    int *part0, *part1;
	int a,b, t,t1;
	const unsigned char* tab = bitrevtab + bitrevtabOffset[tabidx];
	int nbits = nfftlog2Tab[tabidx];

	part0 = inout;
    part1 = inout + (1 << nbits);
	
	while ((a = *tab++) != 0) {
        b = *tab++;

        swapcplx(part0[4*a+0], part0[4*b+0]);	/* 0xxx0 <-> 0yyy0 */
        swapcplx(part0[4*a+2], part1[4*b+0]);	/* 0xxx1 <-> 1yyy0 */
        swapcplx(part1[4*a+0], part0[4*b+2]);	/* 1xxx0 <-> 0yyy1 */
        swapcplx(part1[4*a+2], part1[4*b+2]);	/* 1xxx1 <-> 1yyy1 */
    }

    do {
        swapcplx(part0[4*a+2], part1[4*a+0]);	/* 0xxx1 <-> 1xxx0 */
    } while ((a = *tab++) != 0);
}

#ifndef ARMV7Neon
static void R4FirstPass(int *x, int bg)
{
    int ar, ai, br, bi, cr, ci, dr, di;
	
	for (; bg != 0; bg--) {

		ar = x[0] + x[2];
		br = x[0] - x[2];
		ai = x[1] + x[3];
		bi = x[1] - x[3];
		cr = x[4] + x[6];
		dr = x[4] - x[6];
		ci = x[5] + x[7];
		di = x[5] - x[7];

		/* max per-sample gain = 4.0 (adding 4 inputs together) */
		x[0] = ar + cr;
		x[4] = ar - cr;
		x[1] = ai + ci;
		x[5] = ai - ci;
		x[2] = br + di;
		x[6] = br - di;
		x[3] = bi - dr;
		x[7] = bi + dr;

		x += 8;
	}
}

static void R8FirstPass(int *x, int bg)
{
    int ar, ai, br, bi, cr, ci, dr, di;
	int sr, si, tr, ti, ur, ui, vr, vi;
	int wr, wi, xr, xi, yr, yi, zr, zi;

	for (; bg != 0; bg--) {

		ar = x[0] + x[2];
		br = x[0] - x[2];
		ai = x[1] + x[3];
		bi = x[1] - x[3];
		cr = x[4] + x[6];
		dr = x[4] - x[6];
		ci = x[5] + x[7];
		di = x[5] - x[7];

		sr = ar + cr;
		ur = ar - cr;
		si = ai + ci;
		ui = ai - ci;
		tr = br - di;
		vr = br + di;
		ti = bi + dr;
		vi = bi - dr;

		ar = x[ 8] + x[10];
		br = x[ 8] - x[10];
		ai = x[ 9] + x[11];
		bi = x[ 9] - x[11];
		cr = x[12] + x[14];
		dr = x[12] - x[14];
		ci = x[13] + x[15];
		di = x[13] - x[15];

		wr = (ar + cr) >> 1;
		yr = (ar - cr) >> 1;
		wi = (ai + ci) >> 1;
		yi = (ai - ci) >> 1;

		x[ 0] = (sr >> 1) + wr;
		x[ 8] = (sr >> 1) - wr;
		x[ 1] = (si >> 1) + wi;
		x[ 9] = (si >> 1) - wi;
		x[ 4] = (ur >> 1) + yi;
		x[12] = (ur >> 1) - yi;
		x[ 5] = (ui >> 1) - yr;
		x[13] = (ui >> 1) + yr;

		ar = br - di;
		cr = br + di;
		ai = bi + dr;
		ci = bi - dr;

		xr = MULHIGH(SQRT1_2, ar - ai);
		xi = MULHIGH(SQRT1_2, ar + ai);
		zr = MULHIGH(SQRT1_2, cr - ci);
		zi = MULHIGH(SQRT1_2, cr + ci);

		x[ 6] = (tr >> 1) - xr;
		x[14] = (tr >> 1) + xr;
		x[ 7] = (ti >> 1) - xi;
		x[15] = (ti >> 1) + xi;
		x[ 2] = (vr >> 1) + zi;
		x[10] = (vr >> 1) - zi;
		x[ 3] = (vi >> 1) - zr;
		x[11] = (vi >> 1) + zr;

		x += 16;
	}
}

static void R4Core(int *x, int bg, int gp, int *wtab)
{
	int ar, ai, br, bi, cr, ci, dr, di, tr, ti;
	int wd, ws, wi;
	int i, j, step;
	int *xptr, *wptr;

	for (; bg != 0; gp <<= 2, bg >>= 2) {

		step = 2*gp;
		xptr = x;

		for (i = bg; i != 0; i--) {

			wptr = wtab;

			for (j = gp; j != 0; j--) {

				ar = xptr[0];
				ai = xptr[1];
				xptr += step;
				
				ws = wptr[0];
				wi = wptr[1];
				br = xptr[0];
				bi = xptr[1];
				wd = ws + 2*wi;
				tr = MULHIGH(wi, br + bi);
				br = MULHIGH(wd, br) - tr;	
				bi = MULHIGH(ws, bi) + tr;	
				xptr += step;

				tr = ar >> 2;
				ti = ai >> 2;
				ar = tr - br;
				ai = ti - bi;
				br = tr + br;
				bi = ti + bi;

				ws = wptr[2];
				wi = wptr[3];
				cr = xptr[0];
				ci = xptr[1];
				wd = ws + 2*wi;
				tr = MULHIGH(wi, cr + ci);
				cr = MULHIGH(wd, cr) - tr;
				ci = MULHIGH(ws, ci) + tr;
				xptr += step;				
				
				ws = wptr[4];
				wi = wptr[5];
				dr = xptr[0];
				di = xptr[1];
				wd = ws + 2*wi;
				tr = MULHIGH(wi, dr + di);
				dr = MULHIGH(wd, dr) - tr;
				di = MULHIGH(ws, di) + tr;
				wptr += 6;

				tr = cr;
				ti = ci;
				cr = tr + dr;
				ci = di - ti;
				dr = tr - dr;
				di = di + ti;

				xptr[0] = ar + ci;
				xptr[1] = ai + dr;
				xptr -= step;
				xptr[0] = br - cr;
				xptr[1] = bi - di;
				xptr -= step;
				xptr[0] = ar - ci;
				xptr[1] = ai - dr;
				xptr -= step;
				xptr[0] = br + cr;
				xptr[1] = bi + di;
				xptr += 2;
			}
			xptr += 3*step;
		}
		wtab += 3*step;
	}
}

static void PreMultiply(int tabidx, int *zbuf1)
{
	int i, nmdct, ar1, ai1, ar2, ai2, z1, z2;
	int t, cms2, cps2a, sin2a;
	int *zbuf2, *zbuf3, *zout, *zout1;
	const int *csptr;

	csptr = cos1sin1tab1024 + cos1sin1tabOffset[tabidx];
	nmdct = nmdctTab[tabidx];		

	zbuf2 = zbuf1;
	zbuf3 = zbuf1 + nmdct - 1;
	zout = zbuf1;
	zout1 = zout + nmdct - 1;

	for(i = nmdct >> 2; i != 0; i--)
	{
		cps2a = *csptr++;	
		sin2a = *csptr++;		

		ar1 = *zbuf2++;
		ai2 = *zbuf2++;
		ai1 = *zbuf3--;
		ar2 = *zbuf3--;
		
		cms2 = cps2a - 2*sin2a;
		t  = MULHIGH(sin2a, ar1 + ai1);
		z2 = MULHIGH(cps2a, ai1) - t;
		z1 = MULHIGH(cms2, ar1) + t;
		*zout++ = z1;
		*zout++ = z2;
		
		cps2a = *csptr++;	
		sin2a = *csptr++;

		cms2 = cps2a - 2*sin2a;
		t  = MULHIGH(sin2a, ar2 + ai2);
		z2 = MULHIGH(cps2a, ai2) - t;
		z1 = MULHIGH(cms2, ar2) + t;
		*zout1-- = z2;
		*zout1-- = z1;
	}
}

static void PostMultiply(int tabidx, int *fft1)
{
	int i, nmdct, ar1, ai1, ar2, ai2, z1, z2;
	int t, cms2, cps2a, sin2a, cps2b, sin2b;
	int *fft2;
	const int *csptr;

	nmdct = nmdctTab[tabidx];		
	fft2 = fft1 + nmdct - 1;
	csptr = cos1sin1tab1024 + cos1sin1tabOffset[tabidx];
	

	for(i = nmdct >> 2; i != 0; i--)
	{
		/* cps2 = (cos+sin), sin2 = sin, cms2 = (cos-sin) */
		cps2a = *csptr++;
		sin2a = *csptr++;
		cps2b = *csptr++;
		sin2b = *csptr++;

		ar1 = *(fft1 + 0);
		ai1 = *(fft1 + 1);
		ai2 = *(fft2 + 0);
		ar2 = *(fft2 - 1);

		cms2 = cps2a - 2*sin2a;
		t  = MULHIGH(sin2a, ar1 + ai1);
		z2 = t - MULHIGH(cps2a, ai1);
		z1 = t + MULHIGH(cms2, ar1);
		*fft1++ = z1;	
		*fft2-- = z2;	

		cms2 = cps2b - 2*sin2b;
		t  = MULHIGH(sin2b, ar2 + ai2);
		z2 = t - MULHIGH(cps2b, ai2);
		z1 = t + MULHIGH(cms2, ar2);
		*fft1++ = z2;	
		*fft2-- = z1;		
	}
}

#else
void PostMultiply(int tabidx, int *fft1);
void PreMultiply(int tabidx, int *zbuf1);
void R4Core(int *x, int bg, int gp, int *wtab);
void R4FirstPass(int *x, int bg);
void R8FirstPass(int *x, int bg);
#endif

void R4FFT(int tabidx, int *x)
{
	int order = nfftlog2Tab[tabidx];
	int nfft = nfftTab[tabidx];

	/* decimation in time */
	BitReverse(x, tabidx);

	if (order & 0x1) {
		/* long block: order = 9, nfft = 512 */
		R8FirstPass(x, nfft >> 3);						/* gain 1 int bit,  lose 2 GB */
		R4Core(x, nfft >> 5, 8, (int *)twidTabOdd);		/* gain 6 int bits, lose 2 GB */
	} else {
		/* short block: order = 6, nfft = 64 */
		R4FirstPass(x, nfft >> 2);						/* gain 0 int bits, lose 2 GB */
		R4Core(x, nfft >> 4, 4, (int *)twidTabEven);	/* gain 4 int bits, lose 1 GB */
	}
}

void MDCT(int tabidx, int *coef)
{
	/* fast in-place DCT-IV - adds guard bits if necessary */
	PreMultiply(tabidx, coef);
	R4FFT(tabidx, coef);
	PostMultiply(tabidx, coef);
}

static void shiftMdctDelayBuffer(Word16 *mdctDelayBuffer, /*! start of mdct delay buffer */
                                 Word16 *timeSignal,      /*! pointer to new time signal samples, interleaved */
                                 Word16 chIncrement       /*! number of channels */
                                 )
{
  Word32 i;
  Word16 *srBuf = mdctDelayBuffer;
  Word16 *dsBuf = mdctDelayBuffer+FRAME_LEN_LONG;

  for(i = 0; i < BLOCK_SWITCHING_OFFSET-FRAME_LEN_LONG; i+= 8)
  {
	 *srBuf++ = *dsBuf++;	 *srBuf++ = *dsBuf++;
	 *srBuf++ = *dsBuf++;	 *srBuf++ = *dsBuf++;
	 *srBuf++ = *dsBuf++;	 *srBuf++ = *dsBuf++;
	 *srBuf++ = *dsBuf++;	 *srBuf++ = *dsBuf++;
  }

  srBuf = mdctDelayBuffer + BLOCK_SWITCHING_OFFSET-FRAME_LEN_LONG;
  dsBuf = timeSignal;

  //memmove(mdctDelayBuffer,mdctDelayBuffer+FRAME_LEN_LONG,(BLOCK_SWITCHING_OFFSET-FRAME_LEN_LONG)*sizeof(Word16));
  for(i=0; i<FRAME_LEN_LONG; i+=8)
  {  //mdctDelayBuffer[BLOCK_SWITCHING_OFFSET-FRAME_LEN_LONG+i] = timeSignal[i*chIncrement];  
		*srBuf++ = *dsBuf; dsBuf += chIncrement;
		*srBuf++ = *dsBuf; dsBuf += chIncrement;
		*srBuf++ = *dsBuf; dsBuf += chIncrement;
		*srBuf++ = *dsBuf; dsBuf += chIncrement;
		*srBuf++ = *dsBuf; dsBuf += chIncrement;
		*srBuf++ = *dsBuf; dsBuf += chIncrement;
		*srBuf++ = *dsBuf; dsBuf += chIncrement;
		*srBuf++ = *dsBuf; dsBuf += chIncrement;
  }
}

static Word16 getScalefactorOfShortVectorStride(const Word16 *vector, /*!< Pointer to input vector */
                                                Word16 len,           /*!< Length of input vector */
                                                Word16 stride)        /*!< Stride of input vector */
{
  Word16 maxVal = 0;
  Word16 absVal;
  Word16 i;

  for(i=0; i<len; i++){
    absVal = abs_s(vector[i*stride]);
    maxVal |= absVal;
  }
 
  return( maxVal ? norm_s(maxVal) : 15);
}


void Transform_Real(Word16 *mdctDelayBuffer,
                    Word16 *timeSignal,
                    Word16 chIncrement,
                    Word32 *realOut,
                    Word16 *mdctScale,
                    Word16 blockType
                    )
{
	Word32 i,w;
	Word32 timeSignalSample;
	Word32 ws1,ws2;
	Word32 *dctIn;
	
	Word32 delayBufferSf,timeSignalSf,minSf;
	
	switch(blockType){
		
		
	case LONG_WINDOW:
		dctIn = realOut;
		
		/*
		we access BLOCK_SWITCHING_OFFSET (1600 ) delay buffer samples + 448 new timeSignal samples
		*/
		delayBufferSf = getScalefactorOfShortVectorStride(mdctDelayBuffer,BLOCK_SWITCHING_OFFSET,1);
		timeSignalSf  = getScalefactorOfShortVectorStride(timeSignal,2*FRAME_LEN_LONG-BLOCK_SWITCHING_OFFSET,chIncrement);
		minSf = min(delayBufferSf,timeSignalSf);
		minSf = min(minSf,12);
		for(i=0;i<FRAME_LEN_LONG/2;i++){
			timeSignalSample = (mdctDelayBuffer[i] << minSf); 
			ws1 = ( timeSignalSample * LongWindowKBD[i] );
			timeSignalSample = (mdctDelayBuffer[(FRAME_LEN_LONG-i-1)] << minSf);
			ws2 = ( timeSignalSample * LongWindowKBD[FRAME_LEN_LONG-i-1] );
			dctIn[FRAME_LEN_LONG/2+i] = (ws1 >> 2) - (ws2 >> 2);    
		}
		
		shiftMdctDelayBuffer(mdctDelayBuffer,timeSignal,chIncrement);
		
		for(i=0;i<FRAME_LEN_LONG/2;i++){    
			timeSignalSample = (mdctDelayBuffer[i] << minSf);
			ws1 = ( timeSignalSample * LongWindowKBD[FRAME_LEN_LONG-i-1] );
			timeSignalSample = (mdctDelayBuffer[(FRAME_LEN_LONG-i-1)] << minSf);
			ws2 = ( timeSignalSample * LongWindowKBD[i] );
			dctIn[FRAME_LEN_LONG/2-i-1] = -((ws1 >> 2) + (ws2 >> 2));  
		}
		
		MDCT(1, (int*)dctIn);
		minSf = (14 - minSf);
		*mdctScale=minSf; 
		break;
		
	case START_WINDOW:
		dctIn = realOut;
		
		/*
		we access BLOCK_SWITCHING_OFFSET (1600 ) delay buffer samples + no timeSignal samples
		*/
		minSf = getScalefactorOfShortVectorStride(mdctDelayBuffer,BLOCK_SWITCHING_OFFSET,1);
		minSf = min(minSf,12);
		
		
		for(i=0;i<FRAME_LEN_LONG/2;i++){
			timeSignalSample = (mdctDelayBuffer[i] << minSf);
			ws1 = timeSignalSample * LongWindowKBD[i];
			timeSignalSample = (mdctDelayBuffer[(FRAME_LEN_LONG-i-1)] << minSf);
			ws2 = timeSignalSample * LongWindowKBD[FRAME_LEN_LONG-i-1];
			dctIn[FRAME_LEN_LONG/2+i] = (ws1 >> 2) - (ws2 >> 2);
		}
		
		shiftMdctDelayBuffer(mdctDelayBuffer,timeSignal,chIncrement);
		
		for(i=0;i<LS_TRANS;i++){
			timeSignalSample = (mdctDelayBuffer[i] << minSf);
			dctIn[FRAME_LEN_LONG/2-i-1] = (-timeSignalSample) << (15 - 2);  
		}
		
		for(i=0;i<FRAME_LEN_SHORT/2;i++){
			timeSignalSample= (mdctDelayBuffer[i+LS_TRANS] << minSf);
			ws1 = timeSignalSample * ShortWindowSine[FRAME_LEN_SHORT-i-1];
			timeSignalSample= (mdctDelayBuffer[(FRAME_LEN_LONG-i-1-LS_TRANS)] << minSf);
			ws2 = timeSignalSample * ShortWindowSine[i];
			dctIn[FRAME_LEN_LONG/2-i-1-LS_TRANS] =  -((ws1 >> 2) + (ws2 >> 2));
		}
		
		MDCT(1, (int*)dctIn);
		minSf = (14 - minSf);
		*mdctScale= minSf;
		break;
		
	case STOP_WINDOW:
		dctIn = realOut;
		
		/*
		we access BLOCK_SWITCHING_OFFSET-LS_TRANS (1600-448 ) delay buffer samples + 448 new timeSignal samples
		*/
		delayBufferSf = getScalefactorOfShortVectorStride(mdctDelayBuffer+LS_TRANS,BLOCK_SWITCHING_OFFSET-LS_TRANS,1);
		timeSignalSf  = getScalefactorOfShortVectorStride(timeSignal,2*FRAME_LEN_LONG-BLOCK_SWITCHING_OFFSET,chIncrement);
		minSf = min(delayBufferSf,timeSignalSf);    
		minSf = min(minSf,12);
		
		for(i=0;i<LS_TRANS;i++){
			timeSignalSample= mdctDelayBuffer[(FRAME_LEN_LONG-i-1)] << minSf;
			dctIn[FRAME_LEN_LONG/2+i] = (-timeSignalSample) << (15 - 2);    
		}
		
		for(i=0;i<FRAME_LEN_SHORT/2;i++){
			timeSignalSample = (mdctDelayBuffer[(i+LS_TRANS)] << minSf);
			ws1 = timeSignalSample * ShortWindowSine[i];
			timeSignalSample= (mdctDelayBuffer[(FRAME_LEN_LONG-LS_TRANS-i-1)] << minSf);
			ws2 = timeSignalSample * ShortWindowSine[FRAME_LEN_SHORT-i-1];
			dctIn[FRAME_LEN_LONG/2+i+LS_TRANS] = (ws1 >> 2) - (ws2 >> 2);
		}
		
		shiftMdctDelayBuffer(mdctDelayBuffer,timeSignal,chIncrement);
		
		for(i=0;i<FRAME_LEN_LONG/2;i++){
			timeSignalSample= (mdctDelayBuffer[i] << minSf);
			ws1 = timeSignalSample * LongWindowKBD[FRAME_LEN_LONG-i-1];
			timeSignalSample= (mdctDelayBuffer[(FRAME_LEN_LONG-i-1)] << minSf);
			ws2 = timeSignalSample * LongWindowKBD[i];
			dctIn[FRAME_LEN_LONG/2-i-1] =  -((ws1 >> 2) + (ws2 >> 2));
		}
		
		MDCT(1, (int*)dctIn);
		minSf = (14 - minSf);
		*mdctScale= minSf; 
		break;
		
	case SHORT_WINDOW:
    /*
	we access BLOCK_SWITCHING_OFFSET (1600 ) delay buffer samples + no new timeSignal samples
		*/
		
		minSf = getScalefactorOfShortVectorStride(mdctDelayBuffer+TRANSFORM_OFFSET_SHORT,9*FRAME_LEN_SHORT,1);
		minSf = min(minSf,9);
		
		
		for(w=0;w<TRANS_FAC;w++){
			dctIn = realOut+w*FRAME_LEN_SHORT;
			
			for(i=0;i<FRAME_LEN_SHORT/2;i++){
				timeSignalSample= (mdctDelayBuffer[TRANSFORM_OFFSET_SHORT+w*FRAME_LEN_SHORT+i] << minSf);
				ws1 = timeSignalSample * ShortWindowSine[i];
				timeSignalSample= (mdctDelayBuffer[TRANSFORM_OFFSET_SHORT+w*FRAME_LEN_SHORT+FRAME_LEN_SHORT-i-1] << minSf);
				ws2 = timeSignalSample * ShortWindowSine[FRAME_LEN_SHORT-i-1];
				dctIn[FRAME_LEN_SHORT/2+i] = (ws1 >> 2) - (ws2 >> 2);
				
				timeSignalSample= (mdctDelayBuffer[TRANSFORM_OFFSET_SHORT+w*FRAME_LEN_SHORT+FRAME_LEN_SHORT+i] << minSf);
				ws1 = timeSignalSample * ShortWindowSine[FRAME_LEN_SHORT-i-1];
				timeSignalSample= (mdctDelayBuffer[TRANSFORM_OFFSET_SHORT+w*FRAME_LEN_SHORT+FRAME_LEN_SHORT*2-i-1] << minSf);
				ws2 = timeSignalSample * ShortWindowSine[i];
				dctIn[FRAME_LEN_SHORT/2-i-1] =  -((ws1 >> 2) + (ws2 >> 2));
			}
			
			MDCT(0, (int*)dctIn);
		}
		
		minSf = (11 - minSf);
		*mdctScale = minSf; 
		
		shiftMdctDelayBuffer(mdctDelayBuffer,timeSignal,chIncrement);
		break;

  }
}

