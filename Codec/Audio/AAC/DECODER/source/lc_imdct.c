#include "struct.h"
#include "decoder.h"
#include "sbr_dec.h"
#include "portab.h"

#ifdef ARMV6
void R8FirstPass(int *x, int bg);
void R4FirstPass(int *x, int bg);
void PreMultiplyRescale(int tabidx, int *zbuf1, int es, const int *tab1, const int *tab2, const int *tab3);
void PostMultiplyRescale(int tabidx, int *fft1, int es, const int *tab1, const int *tab2, const int *tab3);
void PreMultiply(int tabidx, int *zbuf1, const int *tab1, const int *tab2, const int *tab3);
void PostMultiply(int tabidx, int *fft1, const int *tab1, const int *tab2, const int *tab3);
void writePCM_V6(int *tmpBuffer, short *outbuf, int channel_stride);
#endif

#ifdef ARMV7
void WinLong(int *buf0, int *over0, int *out0, int winTypeCurr, int winTypePrev, const int *tab1, const int *tab2);
void WinLongStart(int *buf0, int *over0, int *out0, int winTypeCurr, int winTypePrev, const int *tab1, const int *tab2);
void WinShort(int *buf0, int *over0, int *out0, int winTypeCurr, int winTypePrev, const int *tab1, const int *tab2);
void WinLongStop(int *buf0, int *over0, int *out0, int winTypeCurr, int winTypePrev, const int *tab1, const int *tab2);
void writePCM_V7(int *tmp1, int *tmp2, short *outbuffer, int channel_stride);
#endif

#define swap2(p0,p1) \
	t = p0; t1 = *(&(p0)+1);	\
	p0 = p1; *(&(p0)+1) = *(&(p1)+1); \
	p1 = t; *(&(p1)+1) = t1

void Shuffle(int *x, int bLongWin)
{
	int *LowPart, *HighPart;
	const unsigned char* pTab;
	int nFFT;
	int l,h, t,t1;

	if(bLongWin)
	{
		pTab = BitRevTab + 17;
		nFFT = 512;
	}
	else
	{
		pTab = BitRevTab;
		nFFT = 64;
	}

	LowPart = x;
	HighPart = x + nFFT;

	while ((l = *pTab++) != 0) {
		h = *pTab++;
		swap2(LowPart[4*l+0], LowPart[4*h+0]);	swap2(LowPart[4*l+2], HighPart[4*h+0]);	
		swap2(HighPart[4*l+0], LowPart[4*h+2]);	swap2(HighPart[4*l+2], HighPart[4*h+2]);	
	}
	do {
		swap2(LowPart[4*l+2], HighPart[4*l+0]);	
	} while ((l = *pTab++) != 0);
}

#ifndef ARMV6
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

static void R8FirstPass(int *xbuf, int step)
{
	int ar, ai, br, bi, cr, ci, dr, di;
	int sr, si, tr, ti, ur, ui, vr, vi;
	int wr, wi, xr, xi, yr, yi, zr, zi;

	for (; step != 0; step--) {

		ar = xbuf[0] + xbuf[2];
		ai = xbuf[1] + xbuf[3];
		br = xbuf[0] - xbuf[2];
		bi = xbuf[1] - xbuf[3];

		cr = xbuf[4] + xbuf[6];
		ci = xbuf[5] + xbuf[7];
		dr = xbuf[4] - xbuf[6];
		di = xbuf[5] - xbuf[7];

		sr = (ar + cr) >> 1;
		ur = (ar - cr) >> 1;
		si = (ai + ci) >> 1;
		ui = (ai - ci) >> 1;
		tr = (br - di) >> 1;
		vr = (br + di) >> 1;
		ti = (bi + dr) >> 1;
		vi = (bi - dr) >> 1;

		ar = xbuf[ 8] + xbuf[10];
		ai = xbuf[ 9] + xbuf[11];
		br = xbuf[ 8] - xbuf[10];
		bi = xbuf[ 9] - xbuf[11];
		cr = xbuf[12] + xbuf[14];
		ci = xbuf[13] + xbuf[15];
		dr = xbuf[12] - xbuf[14];
		di = xbuf[13] - xbuf[15];

		wr = (ar + cr) >> 1;
		yr = (ar - cr) >> 1;
		wi = (ai + ci) >> 1;
		yi = (ai - ci) >> 1;

		xbuf[ 0] = sr + wr;
		xbuf[ 1] = si + wi;
		xbuf[ 8] = sr - wr;
		xbuf[ 9] = si - wi;
		xbuf[ 4] = ur + yi;
		xbuf[ 5] = ui - yr;
		xbuf[12] = ur - yi;
		xbuf[13] = ui + yr;

		ar = br - di;
		cr = br + di;
		ai = bi + dr;
		ci = bi - dr;

		xr = MULHIGH(SQRT1_2, ar - ai);
		xi = MULHIGH(SQRT1_2, ar + ai);
		zr = MULHIGH(SQRT1_2, cr - ci);
		zi = MULHIGH(SQRT1_2, cr + ci);

		xbuf[ 6] = tr - xr;
		xbuf[ 7] = ti - xi;
		xbuf[14] = tr + xr;
		xbuf[15] = ti + xi;
		xbuf[ 2] = vr + zi;
		xbuf[ 3] = vi - zr;
		xbuf[10] = vr - zi;
		xbuf[11] = vi + zr;

		xbuf += 16;
	}
}

void R4Core(int *xbuf, int step, int loop, int *wtab)
{
	int ar, ai, br, bi, cr, ci, dr, di;
	int tr, ti;
	int wd, ws, wi;
	int i, j, skip;
	int *xptr, *wptr;

	for (; step != 0; loop <<= 2, step >>= 2) {

		skip = 2*loop;
		xptr = xbuf;

		for (i = step; i != 0; i--) {

			wptr = wtab;

			for (j = loop; j != 0; j--) {

				ar = xptr[0];
				ai = xptr[1];
				xptr += skip;

				ws = wptr[0];
				wi = wptr[1];
				br = xptr[0];
				bi = xptr[1];
				wd = ws + 2*wi;
				tr = MULHIGH(wi, br + bi);
				br = MULHIGH(wd, br) - tr;	
				bi = MULHIGH(ws, bi) + tr;	
				xptr += skip;

				ws = wptr[2];
				wi = wptr[3];
				cr = xptr[0];
				ci = xptr[1];
				wd = ws + 2*wi;
				tr = MULHIGH(wi, cr + ci);
				cr = MULHIGH(wd, cr) - tr;
				ci = MULHIGH(ws, ci) + tr;
				xptr += skip;

				ws = wptr[4];
				wi = wptr[5];
				dr = xptr[0];
				di = xptr[1];
				wd = ws + 2*wi;
				tr = MULHIGH(wi, dr + di);
				dr = MULHIGH(wd, dr) - tr;
				di = MULHIGH(ws, di) + tr;
				wptr += 6;

				tr = ar >> 2;
				ti = ai >> 2;
				ar = tr - br;
				ai = ti - bi;
				br = tr + br;
				bi = ti + bi;

				tr = cr;
				ti = ci;
				cr = tr + dr;
				ci = di - ti;
				dr = tr - dr;
				di = di + ti;

				xptr[0] = ar + ci;
				xptr[1] = ai + dr;
				xptr -= skip;
				xptr[0] = br - cr;
				xptr[1] = bi - di;
				xptr -= skip;
				xptr[0] = ar - ci;
				xptr[1] = ai - dr;
				xptr -= skip;
				xptr[0] = br + cr;
				xptr[1] = bi + di;
				xptr += 2;
			}
			xptr += 3*skip;
		}
		wtab += 3*skip;
	}
}

static void PreMultiply(int tabidx, int *zbuf1)
{
	int i, nmdct, ar1, ai1, ar2, ai2, z1, z2;
	int t, cms2, cps2a, sin2a, cps2b, sin2b;
	int *zbuf2;
	const int *csptr;

	nmdct = nmdctTab[tabidx];		
	zbuf2 = zbuf1 + nmdct - 1;
	csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];

	for (i = nmdct >> 2; i != 0; i--) {
		cps2a = *csptr++;
		sin2a = *csptr++;
		cps2b = *csptr++;
		sin2b = *csptr++;

		ar1 = *(zbuf1 + 0);
		ai2 = *(zbuf1 + 1);
		ai1 = *(zbuf2 + 0);
		ar2 = *(zbuf2 - 1);

		t  = MULHIGH(sin2a, ar1 + ai1);
		z2 = MULHIGH(cps2a, ai1) - t;
		cms2 = cps2a - 2*sin2a;
		z1 = MULHIGH(cms2, ar1) + t;
		*zbuf1++ = z1;	
		*zbuf1++ = z2;	

		t  = MULHIGH(sin2b, ar2 + ai2);
		z2 = MULHIGH(cps2b, ai2) - t;
		cms2 = cps2b - 2*sin2b;
		z1 = MULHIGH(cms2, ar2) + t;
		*zbuf2-- = z2;	
		*zbuf2-- = z1;	
	}
}

static void PostMultiply(int tabidx, int *fft1)
{
	int i, nmdct, ar1, ai1, ar2, ai2, skipFactor;
	int t, cms2, cps2, sin2;
	int *fft2;
	const int *csptr;

	nmdct = nmdctTab[tabidx];		
	csptr = cos1sin1tab;
	skipFactor = postSkip[tabidx];
	fft2 = fft1 + nmdct - 1;

	cps2 = *csptr++;
	sin2 = *csptr;
	csptr += skipFactor;
	cms2 = cps2 - 2*sin2;

	for (i = nmdct >> 2; i != 0; i--) {
		ar1 = *(fft1 + 0);
		ai1 = *(fft1 + 1);
		ar2 = *(fft2 - 1);
		ai2 = *(fft2 + 0);

		t = MULHIGH(sin2, ar1 + ai1);
		*fft2-- = t - MULHIGH(cps2, ai1);	
		*fft1++ = t + MULHIGH(cms2, ar1);	
		cps2 = *csptr++;
		sin2 = *csptr;
		csptr += skipFactor;

		ai2 = -ai2;
		t = MULHIGH(sin2, ar2 + ai2);
		*fft2-- = t - MULHIGH(cps2, ai2);	
		cms2 = cps2 - 2*sin2;
		*fft1++ = t + MULHIGH(cms2, ar2);	
	}
}

static void PreMultiplyRescale(int tabidx, int *zbuf1, int es)
{
	int i, nmdct, ar1, ai1, ar2, ai2, z1, z2;
	int t, cms2, cps2a, sin2a, cps2b, sin2b;
	int *zbuf2;
	const int *csptr;

	nmdct = nmdctTab[tabidx];		
	zbuf2 = zbuf1 + nmdct - 1;
	csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];

	for (i = nmdct >> 2; i != 0; i--) {
		cps2a = *csptr++;	
		sin2a = *csptr++;
		cps2b = *csptr++;	
		sin2b = *csptr++;
		
		ar1 = *(zbuf1 + 0) >> es;
		ai1 = *(zbuf2 + 0) >> es;
		ai2 = *(zbuf1 + 1) >> es;

		t  = MUL_32(sin2a, ar1 + ai1);
		z2 = MUL_32(cps2a, ai1) - t;
		cms2 = cps2a - 2*sin2a;
		z1 = MUL_32(cms2, ar1) + t;
		*zbuf1++ = z1;
		*zbuf1++ = z2;

		ar2 = *(zbuf2 - 1)>> es;	

		t  = MUL_32(sin2b, ar2 + ai2);
		z2 = MUL_32(cps2b, ai2) - t;
		cms2 = cps2b - 2*sin2b;
		z1 = MUL_32(cms2, ar2) + t;
		*zbuf2-- = z2;
		*zbuf2-- = z1;
	}
}

static void PostMultiplyRescale(int tabidx, int *fft1, int es)
{
	int i, nmdct, ar1, ai1, ar2, ai2, skipFactor, z;
	int t, cs2, sin2;
	int *fft2;
	const int *csptr;

	nmdct = nmdctTab[tabidx];		
	csptr = cos1sin1tab;
	skipFactor = postSkip[tabidx];
	fft2 = fft1 + nmdct - 1;

	cs2 = *csptr++;
	sin2 = *csptr;
	csptr += skipFactor;

	for (i = nmdct >> 2; i != 0; i--) {
		ar1 = *(fft1 + 0);
		ai1 = *(fft1 + 1);
		ai2 = *(fft2 + 0);

		t = MULHIGH(sin2, ar1 + ai1);
		z = t - MULHIGH(cs2, ai1);	
		CLIP_2N_SHIFT(z, es);	 
		*fft2-- = z;
		cs2 -= 2*sin2;
		z = t + MULHIGH(cs2, ar1);	
		CLIP_2N_SHIFT(z, es);	 
		*fft1++ = z;

		cs2 = *csptr++;
		sin2 = *csptr;
		csptr += skipFactor;

		ar2 = *fft2;
		ai2 = -ai2;
		t = MULHIGH(sin2, ar2 + ai2);
		z = t - MULHIGH(cs2, ai2);	
		CLIP_2N_SHIFT(z, es);	 
		*fft2-- = z;
		cs2 -= 2*sin2;
		z = t + MULHIGH(cs2, ar2);	
		CLIP_2N_SHIFT(z, es);	 
		*fft1++ = z;
		cs2 += 2*sin2;
	}
}
#endif

void voRadix4FFT(int tabidx, int *x)
{
	int order = nfftlog2Tab[tabidx];
	int nfft = nfftTab[tabidx];

	Shuffle(x, tabidx);

	if (order & 0x1) {
		R8FirstPass(x, nfft >> 3);								
		R4Core(x, nfft >> 5, 8, (int *)twidTabOdd);		
	} else {
		R4FirstPass(x, nfft >> 2);						
		R4Core(x, nfft >> 4, 4, (int *)twidTabEven);	
	}
}


void voIMDCT(int tabidx, int *coef, int gb)
{
	int es;

	if (gb < GBITS_IN_DCT4) 
	{
		es = GBITS_IN_DCT4 - gb;
#ifdef ARMV6
		PreMultiplyRescale(tabidx, coef, es, &nmdctTab[0], &cos4sin4tab[0], &cos4sin4tabOffset[0]);		
		voRadix4FFT(tabidx, coef);		
		PostMultiplyRescale(tabidx, coef, es, &nmdctTab[0], &cos1sin1tab[0], &postSkip[0]);
#else
		PreMultiplyRescale(tabidx, coef, es);		
		voRadix4FFT(tabidx, coef);		
		PostMultiplyRescale(tabidx, coef, es);
#endif
	} else {
#ifdef ARMV6
		PreMultiply(tabidx, coef, &nmdctTab[0], &cos4sin4tab[0], &cos4sin4tabOffset[0]);		
		voRadix4FFT(tabidx, coef);		
		PostMultiply(tabidx, coef, &nmdctTab[0], &cos1sin1tab[0], &postSkip[0]);
#else
		PreMultiply(tabidx, coef);		
		voRadix4FFT(tabidx, coef);		
		PostMultiply(tabidx, coef);
#endif
	}
}

#ifndef ARMV7
void WinLong(int *inBuf, int *overlap, int *out, int winTypeCurr, int winTypePrev)
{
	int win0, win1;
	int tmp0, tmp1;
	int inData;
	int *inBuf1, *overlap1, *out1;
	const int *wndPrev, *wndCurr;

	inBuf += 512;
	inBuf1 = inBuf  - 1;
	out1  = out + 1023;
	overlap1 = overlap + 1023;

	wndPrev = (winTypePrev == 1 ? kbdWindow + 128 : sinWindow + 128);

	if (winTypeCurr == winTypePrev) {
		do {
			win0 = *wndPrev++; 	 win1 = *wndPrev++;

			inData = *inBuf++;
			tmp0 = MULHIGH(win0, inData);
			tmp1 = MULHIGH(win1, inData);

			inData = *overlap;	 *out++  = inData - tmp0;
			inData = *overlap1;  *out1-- = inData + tmp1;

			inData = *inBuf1--;
			*overlap1-- = MULHIGH(win0, inData);
			*overlap++  = MULHIGH(win1, inData);
		} while (overlap < overlap1);
	} else {
		wndCurr = (winTypeCurr == 1 ? kbdWindow + 128 : sinWindow + 128);
		do {
			win0 = *wndPrev++;    win1 = *wndPrev++;

			inData = *inBuf++;
			tmp0 = MULHIGH(win0, inData);
			tmp1 = MULHIGH(win1, inData);

			inData = *overlap;	  *out++  = inData - tmp0;
			inData = *overlap1;	  *out1-- = inData + tmp1;

			win0 = *wndCurr++;    win1 = *wndCurr++;

			inData = *inBuf1--;
			*overlap1-- = MULHIGH(win0, inData);
			*overlap++  = MULHIGH(win1, inData);
		} while (overlap < overlap1);
	}
}

void WinLongStart(int *inBuf, int *overlap, int *out, int winTypeCurr, int winTypePrev)
{
	int i;
	int win0, win1;
	int tmp0, tmp1;
	int inData;
	int *inBuf1, *overlap1, *out1;
	const int *wndPrev, *wndCurr;

	inBuf += 512;
	inBuf1 = inBuf  - 1;
	out1  = out + 1023;
	overlap1 = overlap + 1023;

	wndPrev = (winTypePrev == 1 ? kbdWindow + 128 : sinWindow + 128);
	for(i = 0; i < 448; i++)
	{
		win0 = *wndPrev++;		win1 = *wndPrev++;

		inData = *inBuf++;
		tmp0 = MULHIGH(win0, inData);
		tmp1 = MULHIGH(win1, inData);

		inData = *overlap;		*out++  = inData - tmp0;
		inData = *overlap1;		*out1-- = inData + tmp1;

		inData = *inBuf1--;
		*overlap1-- = 0;		
		*overlap++ = inData >> 1;	
	}

	wndCurr = (winTypeCurr == 1 ? kbdWindow : sinWindow);

	do {
		win0 = *wndPrev++;		win1 = *wndPrev++;

		inData = *inBuf++;
		tmp0 = MULHIGH(win0, inData);
		tmp1 = MULHIGH(win1, inData);

		inData = *overlap;		*out++  = inData - tmp0;
		inData = *overlap1;		*out1-- = inData + tmp1;

		win0 = *wndCurr++;		win1 = *wndCurr++;	

		inData = *inBuf1--;
		*overlap1-- = MULHIGH(win0, inData);	
		*overlap++  = MULHIGH(win1, inData);	
	} while (overlap < overlap1);
}

void WinLongStop(int *inBuf, int *overlap, int *out, int winTypeCurr, int winTypePrev)
{
	int i;
	int win0, win1;
	int tmp0, tmp1;
	int inData;
	int *inBuf1, *overlap1, *out1;
	const int *wndPrev, *wndCurr;

	inBuf += 512;
	inBuf1 = inBuf  - 1;
	out1  = out + 1023;
	overlap1 = overlap + 1023;

	wndPrev = (winTypePrev == 1 ? kbdWindow : sinWindow);
	wndCurr = (winTypeCurr == 1 ? kbdWindow + 128 : sinWindow + 128);

	for(i = 0; i < 448; i++)	
	{
		inData = *inBuf++;
		tmp1 = inData >> 1;	

		inData = *overlap;		*out++  = inData;
		inData = *overlap1;		*out1-- = inData + tmp1;

		win0 = *wndCurr++;		win1 = *wndCurr++;

		inData = *inBuf1--;
		*overlap1-- = MULHIGH(win0, inData);
		*overlap++  = MULHIGH(win1, inData);
	}

	do {
		win0 = *wndPrev++;	win1 = *wndPrev++;	

		inData = *inBuf++;
		tmp0 = MULHIGH(win0, inData);
		tmp1 = MULHIGH(win1, inData);

		inData = *overlap;		*out++  = inData - tmp0;
		inData = *overlap1;		*out1-- = inData + tmp1;

		win0 = *wndCurr++;		win1 = *wndCurr++;	

		inData = *inBuf1--;
		*overlap1-- = MULHIGH(win0, inData);
		*overlap++  = MULHIGH(win1, inData);
	} while (overlap < overlap1);
}

void WinShort(int *inBuf, int *overlap, int *out0, int winTypeCurr, int winTypePrev)
{
	int i;
	int win0, win1;
	int tmp0, tmp1;
	int inData;
	int *inBuf1, *overlap1, *out1;
	const int *wndPrev, *wndCurr;

	wndPrev = (winTypePrev == 1 ? kbdWindow : sinWindow);
	wndCurr = (winTypeCurr == 1 ? kbdWindow : sinWindow);

	for(i = 0; i < 448; i+= 4)	
	{
		*out0++ = *overlap++; 
		*out0++ = *overlap++;
		*out0++ = *overlap++; 
		*out0++ = *overlap++;
	}

	out1  = out0 + 127;
	overlap1 = overlap + 127;
	inBuf += 64;
	inBuf1  = inBuf  - 1;
	do {
		win0 = *wndPrev++;		win1 = *wndPrev++;	
		
		inData = *inBuf++;		
		tmp0 = MULHIGH(win0, inData);
		tmp1 = MULHIGH(win1, inData);

		inData = *overlap;		*out0++ = inData - tmp0;
		inData = *overlap1;		*out1-- = inData + tmp1;

		win0 = *wndCurr++;		win1 = *wndCurr++;

		inData = *inBuf1--;
		*overlap1-- = MULHIGH(win0, inData);
		*overlap++  = MULHIGH(win1, inData);
	} while (overlap < overlap1);

	for (i = 0; i < 3; i++) {
		out0 += 64;
		out1 = out0 + 127;
		overlap += 64;
		overlap1 = overlap + 127;
		inBuf += 64;
		inBuf1 = inBuf - 1;
		wndCurr -= 128;

		do {
			win0 = *wndCurr++;		win1 = *wndCurr++;	

			inData = *inBuf++;
			tmp0 = MULHIGH(win0, inData);
			tmp1 = MULHIGH(win1, inData);

			inData  = *(overlap - 128);	
			inData += *(overlap + 0);		
			*out0++ = inData - tmp0;

			inData  = *(overlap1 - 128);	
			inData += *(overlap1 + 0);		
			*out1-- = inData + tmp1;

			inData = *inBuf1--;
			*overlap1-- = MULHIGH(win0, inData);
			*overlap++ = MULHIGH(win1, inData);
		} while (overlap < overlap1);
	}

	out0 += 64;
	overlap -= 832;				
	overlap1 = overlap + 127;	
	inBuf += 64;
	inBuf1 = inBuf - 1;
	wndCurr -= 128;
	do {
		win0 = *wndCurr++;		win1 = *wndCurr++;	

		inData = *inBuf++;
		tmp0 = MULHIGH(win0, inData);
		tmp1 = MULHIGH(win1, inData);

		inData  = *(overlap + 768);	
		inData += *(overlap + 896);	
		*out0++ = inData - tmp0;

		inData  = *(overlap1 + 768);	
		*(overlap1 - 128) = inData + tmp1;

		inData = *inBuf1--;
		*overlap1-- = MULHIGH(win0, inData);	
		*overlap++ = MULHIGH(win1, inData);	
	} while (overlap < overlap1);
	
	for (i = 0; i < 3; i++) {
		overlap += 64;
		overlap1 = overlap + 127;
		inBuf += 64;
		inBuf1 = inBuf - 1;
		wndCurr -= 128;
		do {
			win0 = *wndCurr++;		win1 = *wndCurr++;	

			inData = *inBuf++;
			tmp0 = MULHIGH(win0, inData);
			tmp1 = MULHIGH(win1, inData);

			*(overlap - 128) -= tmp0;
			*(overlap1 - 128) += tmp1;

			inData = *inBuf1--;
			*overlap1-- = MULHIGH(win0, inData);
			*overlap++ = MULHIGH(win1, inData);
		} while (overlap < overlap1);
	}

	overlap += 64;
	for(i = 0; i < 448; i+= 4)
	{
		*overlap++ = 0;  *overlap++ = 0;
		*overlap++ = 0;  *overlap++ = 0;
	}
}
#endif


int filter_bank(AACDecoder* decoder,short *outbuf)
{
	int i,ch,chOut;
	ICS_Data *ics;
	short *outbuffer = outbuf;
	int channel_stride = decoder->channelNum;
	int *outCh = decoder->channel_offsize;
	chOut = decoder->decodedChans;// decodedChannels;

	if(decoder->chSpec==VO_AUDIO_CHAN_DUALONE&&decoder->channelNum==1)
		channel_stride = 2;
	
	for(ch=0;ch<decoder->elementChans;ch++,chOut++)
	{
		if(!EnableDecodeCurrChannel(decoder,ch))
			continue;
		if(decoder->common_window)
			ics = &(decoder->ICS_Data[0]);
		else
			ics = &(decoder->ICS_Data[ch]);
#if SUPPORT_MUL_CHANNEL
		if(decoder->channelNum > 2)//multi channel
		{
			if(decoder->seletedChs==VO_CHANNEL_ALL)
				outbuf = outbuffer + outCh[chOut];
			else
			{
				chOut  = decoder->seletedChDecoded+ch;
				outbuf = outbuffer + chOut;
				channel_stride = 2; // for just 2 ch
			}
		}
		else
#endif//SUPPORT_MUL_CHANNEL
		{
			outbuf = outbuffer + chOut;
		}

		if (ics->window_sequence == 2) {
			for (i = 0; i < 8; i++)
				voIMDCT(0, decoder->coef[ch] + i*128, decoder->gbCurrent[ch]);
		} else {
			voIMDCT(1, decoder->coef[ch], decoder->gbCurrent[ch]);
		}

		{
			int* tmpBuffer = (int*)(decoder->tmpBuffer+MAX_SAMPLES*ch);
#ifndef _SYMBIAN_
			if(decoder->overlap[chOut]==0)
			{
				decoder->overlap[chOut] = (int*)voAACDecAlignedMalloc(decoder->voMemop, MAX_SAMPLES*sizeof(int));
				if(decoder->overlap[chOut] == NULL)
					return VO_ERR_OUTOF_MEMORY;
			}
#else
			if(chOut >= 2)
				return VO_ERR_OUTOF_MEMORY;
#endif			
			switch(ics->window_sequence) {
			case ONLY_LONG_SEQUENCE:
#ifdef ARMV7
				WinLong(decoder->coef[ch], decoder->overlap[chOut], tmpBuffer, 
					ics->window_shape, decoder->prevWinShape[chOut], &sinWindow[0], &kbdWindow[0]);
#else
				WinLong(decoder->coef[ch], decoder->overlap[chOut], tmpBuffer, ics->window_shape, decoder->prevWinShape[chOut]);
#endif

				break;
			case LONG_START_SEQUENCE:
#ifdef ARMV7
				WinLongStart(decoder->coef[ch], decoder->overlap[chOut], tmpBuffer, 
					     ics->window_shape, decoder->prevWinShape[chOut], &sinWindow[0], &kbdWindow[0]);
#else
				WinLongStart(decoder->coef[ch], decoder->overlap[chOut], tmpBuffer, ics->window_shape, decoder->prevWinShape[chOut]);
#endif
				
				break;
			case EIGHT_SHORT_SEQUENCE:
#ifdef ARMV7
				WinShort(decoder->coef[ch], decoder->overlap[chOut], tmpBuffer, 
					 ics->window_shape, decoder->prevWinShape[chOut], &sinWindow[0], &kbdWindow[0]);
#else
				WinShort(decoder->coef[ch], decoder->overlap[chOut], tmpBuffer, ics->window_shape, decoder->prevWinShape[chOut]);
#endif
				
				break;
			case LONG_STOP_SEQUENCE:
#ifdef ARMV7
				WinLongStop(decoder->coef[ch], decoder->overlap[chOut], tmpBuffer, 
					    ics->window_shape, decoder->prevWinShape[chOut], &sinWindow[0], &kbdWindow[0]);	
#else
				WinLongStop(decoder->coef[ch], decoder->overlap[chOut], tmpBuffer, ics->window_shape, decoder->prevWinShape[chOut]);	
#endif			
				
				break;
			default:
				break;
			}
#ifdef ARMV7
			if(channel_stride != decoder->elementChans)
#endif
#ifdef ARMV6
			writePCM_V6(tmpBuffer, outbuf, channel_stride);
#else
			for (i = 0; i < MAX_SAMPLES; ) {
				*outbuf = CLIPTOSHORT((tmpBuffer[i] + RND_VAL) >> SCLAE_IMDCT);
				i++;
				outbuf += channel_stride;
				*outbuf = CLIPTOSHORT((tmpBuffer[i] + RND_VAL) >> SCLAE_IMDCT);
				i++;
				outbuf += channel_stride;
				*outbuf = CLIPTOSHORT((tmpBuffer[i] + RND_VAL) >> SCLAE_IMDCT);
				i++;
				outbuf += channel_stride;
				*outbuf = CLIPTOSHORT((tmpBuffer[i] + RND_VAL) >> SCLAE_IMDCT);
				i++;
				outbuf += channel_stride;
			}
#endif			
			decoder->rawSampleBuf[ch] = tmpBuffer;
			decoder->rawSampleBytes = sizeof(int);
			decoder->rawSampleFBits = SCLAE_IMDCT;	
		}

		decoder->prevWinShape[chOut] = ics->window_shape;
	}
	
#ifdef ARMV7		
	if(channel_stride == decoder->elementChans)
	{
		int *tmp1, *tmp2;
		tmp1 = (int*)decoder->tmpBuffer;
		tmp2 = (int*)(decoder->tmpBuffer + MAX_SAMPLES);

		writePCM_V7(tmp1, tmp2, outbuffer, channel_stride);
	}
#endif

	return VO_ERR_NONE;
}

