#include    <math.h>
#include	"config.h"
#include	"global.h"
#include	"frame.h"

#define         PI                      3.14159265358979
#define SQRT1_2 0x5a82799a	/* sqrt(1/2) in Q31 */
#define SQRT2	

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

        swapcplx(part0[4*a+0], part0[4*b+0]);	
        swapcplx(part0[4*a+2], part1[4*b+0]);	
        swapcplx(part1[4*a+0], part0[4*b+2]);	
        swapcplx(part1[4*a+2], part1[4*b+2]);	
    }

    do {
        swapcplx(part0[4*a+2], part1[4*a+0]);	
    } while ((a = *tab++) != 0);
}

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

		wr = ar + cr;
		yr = ar - cr;
		wi = ai + ci;
		yi = ai - ci;

		x[ 0] = sr + wr;
		x[ 8] = sr - wr;
		x[ 1] = si + wi;
		x[ 9] = si - wi;
		x[ 4] = ur + yi;
		x[12] = ur - yi;
		x[ 5] = ui - yr;
		x[13] = ui + yr;

		ar = br - di;
		cr = br + di;
		ai = bi + dr;
		ci = bi - dr;

		xr = MUL_31(SQRT1_2, ar - ai);
		xi = MUL_31(SQRT1_2, ar + ai);
		zr = MUL_31(SQRT1_2, cr - ci);
		zi = MUL_31(SQRT1_2, cr + ci);

		x[ 6] = tr - xr;
		x[14] = tr + xr;
		x[ 7] = ti - xi;
		x[15] = ti + xi;
		x[ 2] = vr + zi;
		x[10] = vr - zi;
		x[ 3] = vi - zr;
		x[11] = vi + zr;

		x += 16;
	}
}

void R4Core(int *x, int bg, int gp, int *wtab)
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
				tr = MUL_32(wi, br + bi);
				br = MUL_32(wd, br) - tr;	/* cos*br + sin*bi */
				bi = MUL_32(ws, bi) + tr;	/* cos*bi - sin*br */
				xptr += step;
				
				ws = wptr[2];
				wi = wptr[3];
				cr = xptr[0];
				ci = xptr[1];
				wd = ws + 2*wi;
				tr = MUL_32(wi, cr + ci);
				cr = MUL_32(wd, cr) - tr;
				ci = MUL_32(ws, ci) + tr;
				xptr += step;
				
				ws = wptr[4];
				wi = wptr[5];
				dr = xptr[0];
				di = xptr[1];
				wd = ws + 2*wi;
				tr = MUL_32(wi, dr + di);
				dr = MUL_32(wd, dr) - tr;
				di = MUL_32(ws, di) + tr;
				wptr += 6;

				tr = ar;
				ti = ai;
				ar = (tr >> 2) - br;
				ai = (ti >> 2) - bi;
				br = (tr >> 2) + br;
				bi = (ti >> 2) + bi;

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

		t  = MUL_32(sin2a, ar1 + ai1);
		z2 = MUL_32(cps2a, ai1) - t;
		cms2 = cps2a - 2*sin2a;
		z1 = MUL_32(cms2, ar1) + t;
		*zbuf1++ = z1;	/* cos*ar1 + sin*ai1 */
		*zbuf1++ = z2;	/* cos*ai1 - sin*ar1 */

		t  = MUL_32(sin2b, ar2 + ai2);
		z2 = MUL_32(cps2b, ai2) - t;
		cms2 = cps2b - 2*sin2b;
		z1 = MUL_32(cms2, ar2) + t;
		*zbuf2-- = z2;	/* cos*ai2 - sin*ar2 */
		*zbuf2-- = z1;	/* cos*ar2 + sin*ai2 */
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

		t = MUL_32(sin2, ar1 + ai1);
		*fft2-- = t - MUL_32(cps2, ai1);	/* sin*ar1 - cos*ai1 */
		*fft1++ = t + MUL_32(cms2, ar1);	/* cos*ar1 + sin*ai1 */
		cps2 = *csptr++;
		sin2 = *csptr;
		csptr += skipFactor;

		ai2 = -ai2;
		t = MUL_32(sin2, ar2 + ai2);
		*fft2-- = t - MUL_32(cps2, ai2);	/* sin*ar1 - cos*ai1 */
		cms2 = cps2 - 2*sin2;
		*fft1++ = t + MUL_32(cms2, ar2);	/* cos*ar1 + sin*ai1 */
	}
}

void R4FFT(int tabidx, int *x)
{
	int order = nfftlog2Tab[tabidx];
	int nfft = nfftTab[tabidx];

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


void IMDCT(int tabidx, int *coef)
{
	PreMultiply(tabidx, coef);
	R4FFT(tabidx, coef);
	PostMultiply(tabidx, coef);	
}


static int WinLong(int *buf0, int *over0, int *out0, int curType)
{
	int in, w0, w1, f0, f1;
	int *buf1, *over1, *out1;
	const int *wndCurr;
	
	buf0 += (1024 >> 1);
	buf1  = buf0  - 1;
	out1  = out0 + 1024 - 1;
	over1 = over0 + 1024 - 1;
	wndCurr = nWinShape + sinWindowOffset[2];
	
	do {
		w0 = *wndCurr++;
		w1 = *wndCurr++;
		in = *buf0++;
		
		f0 = MUL_32(w0, in);
		f1 = MUL_32(w1, in);
		
		in = *over0;	
		*out0++ = in - f0;
		
		in = *over1;	
		*out1-- = in + f1;
		
		in = *buf1--;
		*over1-- = MUL_32(w0, in);
		*over0++ = MUL_32(w1, in);
	} while (over0 < over1);

	return 0;
}

static int WinLongStart(int *buf0, int *over0, int *out0, short curType)
{
	int i,  in, w0, w1, f0, f1;
	int *buf1, *over1, *out1;
	const int *wndPrev, *wndCurr;

	buf0 += (1024 >> 1);
	buf1  = buf0  - 1;
	out1  = out0 + 1024 - 1;
	over1 = over0 + 1024 - 1;

	wndPrev = nWinShape + sinWindowOffset[2];
	if(curType == WIN_LONG_LONG2SHORT)
	{		
		wndCurr = nWinShape + sinWindowOffset[1];
		i = 448;
	}
	else
	{
		wndCurr = nWinShape + sinWindowOffset[0];
		i = 496;		
	}

	do {
		w0 = *wndPrev++;
		w1 = *wndPrev++;
		in = *buf0++;
		
		f0 = MUL_32(w0, in);
		f1 = MUL_32(w1, in);
		
		in = *over0;	
		*out0++ = in - f0;
		
		in = *over1;	
		*out1-- = in + f1;
		
		in = *buf1--;
		
		*over1-- = 0;		
		*over0++ = in >> 1;	
	} while (--i);	
	
	do {
		w0 = *wndPrev++;
		w1 = *wndPrev++;
		in = *buf0++;
		
		f0 = MUL_32(w0, in);
		f1 = MUL_32(w1, in);
		
		in = *over0;	
		*out0++ = in - f0;
		
		in = *over1;	
		*out1-- = in + f1;
		
		w0 = *wndCurr++;	
		w1 = *wndCurr++;	
		in = *buf1--;
		
		*over1-- = MUL_32(w0, in);	
		*over0++ = MUL_32(w1, in);	
	} while (over0 < over1);

	return 0;
}

static int WinStopLong(int *buf0, int *over0, int *out0, short curType)
{
	int i, in, w0, w1, f0, f1;
	int *buf1, *over1, *out1;
	const int *wndPrev, *wndCurr;

	buf0 += (1024 >> 1);
	buf1  = buf0  - 1;
	out1  = out0 + 1024 - 1;
	over1 = over0 + 1024 - 1;

	if(curType == WIN_LONG_SHORT2LONG)
	{
		wndPrev = nWinShape + sinWindowOffset[1];
		wndCurr = nWinShape + sinWindowOffset[2];
		i = 448;	
	}
	else
	{
		wndPrev = nWinShape + sinWindowOffset[0];
		wndCurr = nWinShape + sinWindowOffset[2];
		i = 496;
	}

	do {
		in = *buf0++;
		f1 = in >> 1;	
		
		in = *over0;	
		*out0++ = in;
		*out1-- = f1;
		
		w0 = *wndCurr++;
		w1 = *wndCurr++;
		in = *buf1--;
		
		*over1-- = MUL_32(w0, in);
		*over0++ = MUL_32(w1, in);
	} while (--i);
	
	do {
		w0 = *wndPrev++;	
		w1 = *wndPrev++;	
		in = *buf0++;
		
		f0 = MUL_32(w0, in);
		f1 = MUL_32(w1, in);
		
		in = *over0;	
		*out0++ = in - f0;
		
		in = *over1;	
		*out1-- = in + f1;
		
		w0 = *wndCurr++;
		w1 = *wndCurr++;	
		in = *buf1--;
		
		*over1-- = MUL_32(w0, in);
		*over0++ = MUL_32(w1, in);
	} while (over0 < over1);		
	
	return 0;
}


static int WinStopLongStart(int *buf0, int *over0, int *out0, short curType)
{
	int i, in, w0, w1, f0, f1;
	int *buf1, *over1, *out1;
	const int *wndPrev;
	
	buf0 += (1024 >> 1);
	buf1  = buf0  - 1;
	out1  = out0 + 1024 - 1;
	over1 = over0 + 1024 - 1;
	
	i = 448;	
	
	do {
		in = *buf0++;
		f1 = in >> 1;	
		
		in = *over0;	
		*out0++ = in;
		
		*out1-- = f1;
		
		in = *buf1--;
		
		*over1-- = 0;
		*over0++ = in >> 1;
	} while (--i);
	
	if(curType == WIN_LONG_SHORT2SHORT || curType == WIN_LONG_SHORT2BRIEF)
	{
		wndPrev = nWinShape + sinWindowOffset[1];
	}
	else
	{
		wndPrev = nWinShape + sinWindowOffset[0]; 
		i = 48;	
		
		do {
			in = *buf0++;
			f1 = in >> 1;	
			
			in = *over0++;	
			*out0++ = in;
			
			*out1-- = f1;
		} while (--i);
		
		over1 -= 48;
	}
	
	do {
		w0 = *wndPrev++;	
		w1 = *wndPrev++;	
		in = *buf0++;
		
		f0 = MUL_32(w0, in);
		f1 = MUL_32(w1, in);
		
		in = *over0++;	
		*out0++ = in - f0;
		
		in = *over1--;	
		*out1-- = in + f1;
	} while (out0 < out1);
	
	over0 -= 64;
	over1 += 64;		
	
	if(curType == WIN_LONG_SHORT2SHORT || curType == WIN_LONG_BRIEF2SHORT)
	{
		wndPrev = nWinShape + sinWindowOffset[1]; 	
	}
	else
	{
		wndPrev = nWinShape + sinWindowOffset[0];
		
		i = 48;	
		do {
			in = *buf1--;	
			
			*over1-- = 0;
			*over0++ = in >> 1;
		} while (--i);
	}
		
	do {
		in = *buf1--;	
		
		w0 = *wndPrev++;	
		w1 = *wndPrev++;	
		
		*over1-- = MUL_32(w0, in);
		*over0++ = MUL_32(w1, in);
	} while (over0 < over1);
	
	return 0;
}

static int WinShort(int *buf0, int *over0, int *out0, short *pnWinTypeShort)
{
	int i, n, in, w0, w1, f0, f1;
	int *buf1, *over1, *out1, *overb;
	const int *wndPrev, *wndCurr;

	overb = over0;
	i = 448;
	do {
		f0 = *over0++;
		f1 = *over0++;
		*out0++ = f0;
		*out0++ = f1;
		i -= 2;
	} while (i);
	
	out1  = out0 + (128 - 1);
	over1 = over0 + 128 - 1;
	buf0 += 64;
	buf1  = buf0  - 1;

	if(pnWinTypeShort[0] == WIN_SHORT_SHORT2SHORT || pnWinTypeShort[0] == WIN_SHORT_SHORT2BRIEF)
	{
		wndPrev = nWinShape + sinWindowOffset[1]; 	 
	}
	else
	{
		wndPrev = nWinShape + sinWindowOffset[0];
		i = 48;
		do {
			f0 = *over0++;
			f1 = *over0++;
			*out0++ = f0;
			*out0++ = f1;
			
			f0 = *buf0++;
			f1 = *buf0++;
			*out1-- = f0 >> 1;	
			*out1-- = f1 >> 1;
			i -= 2;
		} while (i);

		over1 -= 48;
	}

	do {
		w0 = *wndPrev++;	
		w1 = *wndPrev++;	
		in = *buf0++;

		f0 = MUL_32(w0, in);
		f1 = MUL_32(w1, in);

		in = *over0++;	
		*out0++ = in - f0;

		in = *over1--;	
		*out1-- = in + f1;
	} while (over0 < over1);

	for (n = 0; n < 3; n++) {
		out0 += 64;
		out1 = out0 + 128 - 1;
		
		buf1  = buf0  - 64 - 1;
		buf0 += 64;

		if(pnWinTypeShort[n] == WIN_SHORT_SHORT2SHORT || pnWinTypeShort[n] == WIN_SHORT_BRIEF2SHORT)
		{
			wndCurr = nWinShape + sinWindowOffset[1]; 	 
		}
		else
		{
			wndCurr = nWinShape + sinWindowOffset[0];
			i = 48;
			do {
				f0 = *buf1--;
				f1 = *buf1--;
				*out0++ = f0 >> 1;
				*out0++ = f1 >> 1;
				
				f0 = *buf0++;
				f1 = *buf0++;
				*out1-- = f0 >> 1;	
				*out1-- = f1 >> 1;
				i -= 2;
			} while (i);
			
		}

		do {
			w0 = *wndCurr++;	
			w1 = *wndCurr++;	

			in = *buf0++;

			f0 = MUL_32(w0, in);
			f1 = MUL_32(w1, in);

			in = *buf1--;
			
			*out0++ = MUL_32(w1, in) - f0;
			*out1-- = MUL_32(w0, in) + f1;
		} while (out0 < out1);
	}
	
	out0 += 64;

	over0 = overb;				/* points at overlap[64] */
	over1 = over0 + 64 - 1;		/* points at overlap[191] */
	
	buf1  = buf0  - 64 - 1;
	buf0 += 64;
	
	if(pnWinTypeShort[n] == WIN_SHORT_SHORT2SHORT || pnWinTypeShort[n] == WIN_SHORT_BRIEF2SHORT)
	{
		wndCurr = nWinShape + sinWindowOffset[1];
		i = 64;
	}
	else
	{
		wndCurr = nWinShape + sinWindowOffset[0];
		i = 48;
		do {
			f0 = *buf1--;
			f1 = *buf1--;
			*out0++ = f0 >> 1;
			*out0++ = f1 >> 1;
			
			f0 = *buf0++;
			f1 = *buf0++;
			*over1-- = f0 >> 1;	
			*over1-- = f1 >> 1;
			i -= 2;
		} while (i);

		i = 16;	
	}
	
	do {
		w0 = *wndCurr++;	
		w1 = *wndCurr++;	
		
		in = *buf0++;
		
		f0 = MUL_32(w0, in);
		f1 = MUL_32(w1, in);
		
		in = *buf1--;
		
		*out0++ = MUL_32(w1, in) - f0;
		*over1-- = MUL_32(w0, in) + f1;
	} while (--i);

	for (n = 4; n < 7; n++) {
		over0 += 64;
		over1 = over0 + 128 - 1;
		
		buf1  = buf0  - 64 - 1;
		buf0 += 64;
		
		if(pnWinTypeShort[n] == WIN_SHORT_SHORT2SHORT || pnWinTypeShort[n] == WIN_SHORT_BRIEF2SHORT)
		{
			wndCurr = nWinShape + sinWindowOffset[1]; 	 
		}
		else
		{
			wndCurr = nWinShape + sinWindowOffset[0];
			i = 48;
			do {
				f0 = *buf1--;
				f1 = *buf1--;
				*over0++ = f0 >> 1;
				*over0++ = f1 >> 1;
				
				f0 = *buf0++;
				f1 = *buf0++;
				*over1-- = f0 >> 1;	
				*over1-- = f1 >> 1;
				i -= 2;
			} while (i);			
		}
		
		do {
			w0 = *wndCurr++;	
			w1 = *wndCurr++;	
			
			in = *buf0++;
			
			f0 = MUL_32(w0, in);
			f1 = MUL_32(w1, in);
			
			in = *buf1--;
			
			*over0++ = MUL_32(w1, in) - f0;
			*over1-- = MUL_32(w0, in) + f1;
		} while (over0 < over1);
	}

	over0 += 64;
	over1 = over0 + 128 - 1;
	buf1  = buf0  - 64 - 1;

	
	if(pnWinTypeShort[n] == WIN_SHORT_SHORT2SHORT || pnWinTypeShort[n] == WIN_SHORT_BRIEF2SHORT)
	{
		wndCurr = nWinShape + sinWindowOffset[1]; 	 
	}
	else
	{
		wndCurr = nWinShape + sinWindowOffset[0];
		i = 48;
		do {
			f0 = *buf1--;
			f1 = *buf1--;
			*over0++ = f0 >> 1;
			*over0++ = f1 >> 1;

			*over1-- = 0;
			*over1-- = 0;

			i -= 2;
		} while (i);
	}
	
	do {
		in = *buf1--;	

		w0 = *wndCurr++;	
		w1 = *wndCurr++;			
		*over0++ = MUL_32(w1, in);
		*over1-- = MUL_32(w0, in);
	} while (over0 < over1);


	i = 448;
	over0 += 64;
	do {
		*over0++ = 0;
		*over0++ = 0;
		*over0++ = 0;
		*over0++ = 0;
		i -= 4;
	} while (i);

	return 0;
}


int  FilterBand(DraDecInfo *decoder, short *outbuf, int nCh)
{
	int				i, chanNum;
	short			curType;
	FrameCHInfo		*chInfo = NULL;
	int				*indata, *outdata;
	int				*overlap;

	chInfo = decoder->chInfo[nCh];
	indata = chInfo->anQIndex;
	overlap = chInfo->overlap;
	chanNum = decoder->channelNum;
	outdata = decoder->afBinNatural;

	curType = chInfo->frame.nWinTypeCurrent;

	if(curType < WIN_SHORT_SHORT2SHORT) {
		/* 1 long block */
		IMDCT(1, indata);
	} else {
		/* 8 short blocks */
		for (i = 0; i < 8; i++)
			IMDCT(0, indata + i*128);
	}
	
	switch(curType) {
	case WIN_LONG_LONG2LONG:
		WinLong(indata, overlap, outdata, curType);
		break;
	case WIN_LONG_SHORT2SHORT:
	case WIN_LONG_BRIEF2BRIEF:
	case WIN_LONG_SHORT2BRIEF:
	case WIN_LONG_BRIEF2SHORT:
		WinStopLongStart(indata, overlap, outdata, curType);				
		break;
	case WIN_LONG_LONG2SHORT:
	case WIN_LONG_LONG2BRIEF:		
		WinLongStart(indata, overlap, outdata, curType);				
		break;
	case WIN_SHORT_SHORT2SHORT:
	case WIN_SHORT_SHORT2BRIEF:
	case WIN_SHORT_BRIEF2BRIEF:
	case WIN_SHORT_BRIEF2SHORT:
		WinShort(indata, overlap, outdata, chInfo->pnWinTypeShort);
		break;
	case WIN_LONG_SHORT2LONG:
	case WIN_LONG_BRIEF2LONG:		
		WinStopLong(indata, overlap, outdata, curType);			
		break;
	default:
		break;
	}	

	for(i = 0; i < 1024; i++)
	{
		int quant;
		quant = outdata[i];
		
		if(quant > 32767)
			quant = 32767;
		else if(quant < -32768)
			quant = -32768;

		outbuf[i*chanNum + nCh] = quant;
	}	

	return 0;
}