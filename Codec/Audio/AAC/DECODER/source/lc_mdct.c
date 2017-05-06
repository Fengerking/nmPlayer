        /************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "struct.h"
#include "decoder.h"

static void PreMDCTMultiply(int tabidx, int *zbuf1)
{
	int i, nmdct, ar1, ai1, ar2, ai2, z1, z2;
	int t, cms2, cps2a, sin2a;
	int *zbuf2, *zbuf3, *zout, *zout1;
	const int *csptr;

	csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];
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
		
		t  = MUL_30(sin2a, ar1 + ai1);
		z2 = MUL_30(cps2a, ai1) - t;
		cms2 = cps2a - 2*sin2a;
		z1 = MUL_30(cms2, ar1) + t;
		*zout++ = z1;
		*zout++ = z2;
		
		cps2a = *csptr++;	
		sin2a = *csptr++;

		t  = MUL_30(sin2a, ar2 + ai2);
		z2 = MUL_30(cps2a, ai2) - t;
		cms2 = cps2a - 2*sin2a;
		z1 = MUL_30(cms2, ar2) + t;
		*zout1-- = z2;
		*zout1-- = z1;
	}
}

static void PostMDCTMultiply(int tabidx, int *fft1)
{
	int i, nmdct, ar1, ai1, ar2, ai2, z1, z2;
	int t, cms2, cps2a, sin2a, cps2b, sin2b;
	int *fft2;
	const int *csptr;

	nmdct = nmdctTab[tabidx];		
	fft2 = fft1 + nmdct - 1;
	csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];
	
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

		t  = MUL_30(sin2a, ar1 + ai1);
		z2 = t - MUL_30(cps2a, ai1);
		cms2 = cps2a - 2*sin2a;
		z1 = t + MUL_30(cms2, ar1);
		*fft1++ = z1;	/* cos*ar2 + sin*ai2 */
		*fft2-- = z2;	/* cos*ai2 - sin*ar2 */	

		t  = MUL_30(sin2b, ar2 + ai2);
		z2 = t - MUL_30(cps2b, ai2);
		cms2 = cps2b - 2*sin2b;
		z1 = t + MUL_30(cms2, ar2);
		*fft1++ = z2;	/* cos*ar2 + sin*ai2 */
		*fft2-- = z1;	/* cos*ai2 - sin*ar2 */			
	}
}

void MDCT(int tabidx, int *coef)
{
	PreMDCTMultiply(tabidx, coef);
	voRadix4FFT(tabidx, coef);
	PostMDCTMultiply(tabidx, coef);	
}
