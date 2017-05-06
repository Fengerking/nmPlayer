	/************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "sbr_dec.h"
#include "decoder.h"

#ifdef SBR_DEC

#define Q28_2	0x20000000	/* Q28: 2.0 */
#define Q28_15	0x30000000	/* Q28: 1.5 */
#define NUM_ITER_IRN		5

/* NINT(2.048E6 / Fs) (figure 4.47) 
* downsampled (single-rate) SBR not currently supported
*/
const unsigned char voGoalSBTab[VOSAMP_RATES_NUM ] = { 
	21, 23, 32, 43, 46, 64, 85, 93, 128
};

int voInvRNormal(int r)
{
	int i, xn, t;

	/* r =   [0.5, 1.0) 
	 * 1/r = (1.0, 2.0] 
	 *   so use 1.5 as initial guess 
	 */
	xn = Q28_15;

	/* xn = xn*(2.0 - r*xn) */
	for (i = NUM_ITER_IRN; i != 0; i--) {
		t = MULHIGH(r, xn);			/* Q31*Q29 = Q28 */
		t = Q28_2 - t;					/* Q28 */
		xn = MULHIGH(xn, t) << 4;	/* Q29*Q28 << 4 = Q29 */
	}

	return xn;
}

#define NUM_TERMS_RPI	5
#define LOG2_EXP_INV	0x58b90bfc	/* 1/log2(e), Q31 */

/* invTab[x] = 1/(x+1), format = Q30 */
static const int invTab[NUM_TERMS_RPI] = {
	0x40000000, 
    0x20000000, 
	0x15555555, 
	0x10000000, 
	0x0ccccccd
};

static int voRatioPowInv(AACDecoder* decoder, int a, int b, int c)
{
	int lna, lnb, i, p, t, y;

	if (a < 1 || b < 1 || c < 1 || a > 64 || b > 64 || c > 64 || a < b)
		return 0;

	lna = MULHIGH(log2Tab[a], LOG2_EXP_INV) << 1;	/* ln(a), Q28 */
	lnb = MULHIGH(log2Tab[b], LOG2_EXP_INV) << 1;	/* ln(b), Q28 */
#if !USE_DIVIDE_FUNC
	if(c==0)
	{
		c = 1;
	}
	p = (lna - lnb) / c;	/* Q28 */
#else//ARMv4Comipler
	p = voUnsignedDivide(decoder,lna - lnb,c);
#endif//ARMv4Comipler

	/* sum in Q24 */
	y = (1 << 24);
	t = p >> 4;		/* t = p^1 * 1/1! (Q24)*/
	y += t;

	for (i = 2; i <= NUM_TERMS_RPI; i++) {
		t = MULHIGH(invTab[i-1], t) << 2;
		t = MULHIGH(p, t) << 4;	/* t = p^i * 1/i! (Q24) */
		y += t;
	}

	return y;
}

int voSqrtFix(int q, int fBitsIn, int *fBitsOut)
{
	int z, lo, hi, mid;

	if (q <= 0) {
		*fBitsOut = fBitsIn;
		return 0;
	}

	z = fBitsIn & 0x01;
	q >>= z;
	fBitsIn -= z;

	z = (CLZ(q) - 1);
	z >>= 1;
	q <<= (2*z);

	lo = 1;
	if (q >= 0x10000000)
		lo = 16384;	
	hi = 46340;		

	do {
		mid = (lo + hi) >> 1;
		if (mid*mid > q)
			hi = mid - 1;
		else
			lo = mid + 1;
	} while (hi >= lo);
	lo--;

	*fBitsOut = ((fBitsIn + 2*z) >> 1);
	return lo;
}

#define SQRT1_2	0x5a82799a

#define sbr_swap(p0,p1) \
	t = p0; t1 = *(&(p0)+1); \
	p0 = p1; *(&(p0)+1) = *(&(p1)+1); \
	p1 = t; *(&(p1)+1) = t1

static const int Radix4SinCosTabSBR[48] = {
	1073741824,          0, 1073741824,          0, 1073741824,          0, 1402911301, -410903207, 
	1262586814, -209476638, 1489322693, -596538995, 1518500250, -759250125, 1402911301, -410903207, 
	1402911301, -992008094, 1402911301, -992008094, 1489322693, -596538995,  843633538, -1053110176, 
	1073741824, -1073741824, 1518500250, -759250125,          0, -759250125,  581104888, -992008094, 
	1489322693, -892783698, -843633538, -209476638,          0, -759250125, 1402911301, -992008094, 
	-1402911301,  410903207, -581104888, -410903207, 1262586814, -1053110176, -1489322693,  892783698, 
};


static void voRadix8Pre32(int *xbuf, int step)
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

		xr = Vo_Multi32(SQRT1_2, ar - ai);
		xi = Vo_Multi32(SQRT1_2, ar + ai);
		zr = Vo_Multi32(SQRT1_2, cr - ci);
		zi = Vo_Multi32(SQRT1_2, cr + ci);

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

static void voRadix4Core32(int *inbuf, int step, int *Tab_Ptr)
{
	int Tab1, Tab2;
	int ar, ai, br, bi, cr, ci, dr, di;
	int re2, re3, re1;

	do {
		Tab1 = Tab_Ptr[0];
		Tab2 = Tab_Ptr[1];
		ar = inbuf[16];
		ai = inbuf[17];
		re1 = ar + ai;
		re1 = Vo_Multi32(Tab2, re1);
		bi  = Vo_Multi32(Tab1, ai) + re1;
		Tab1 += 2*Tab2;
		br  = Vo_Multi32(Tab1, ar) - re1;	

		Tab1 = Tab_Ptr[2];
		Tab2 = Tab_Ptr[3];
		ar = inbuf[32];
		ai = inbuf[33];
		re1 = ar + ai;
		re1 = Vo_Multi32(Tab2, re1);
		ci  = Vo_Multi32(Tab1, ai) + re1;
		Tab1 += 2*Tab2;
		cr  = Vo_Multi32(Tab1, ar) - re1;

		Tab1 = Tab_Ptr[4];
		Tab2 = Tab_Ptr[5];
		ar = inbuf[48];
		ai = inbuf[49];
		re1 = ar + ai;
		re1 = Vo_Multi32(Tab2, re1);
		di  = Vo_Multi32(Tab1, ai) + re1;
		Tab1 += 2*Tab2;
		dr  = Vo_Multi32(Tab1, ar) - re1;

		ar = inbuf[0];
		ai = inbuf[1];

		re2 = cr + dr;
		dr  = cr - dr;
		re3 = di - ci;
		di  = di + ci;

		cr = (ar >> 2) - br;
		ci = (ai >> 2) - bi;
		br += (ar >> 2);
		bi += (ai >> 2);

		inbuf[0] = br + re2;
		inbuf[1] = bi + di;
		inbuf[16] = cr - re3;
		inbuf[17] = ci - dr;
		inbuf[32] = br - re2;
		inbuf[33] = bi - di;
		inbuf[48] = cr + re3;
		inbuf[49] = ci + dr;

		inbuf   += 2;
		Tab_Ptr += 6;
		step--;
	} while (step != 0);
}

void Radix4_FFT(int *x)
{
	int t, t1;
	int *Tab_ptr;
	Tab_ptr = (int *)Radix4SinCosTabSBR;

	/* Bit Inverse */
	sbr_swap(x[2],  x[32]);
	sbr_swap(x[4],  x[16]);
	sbr_swap(x[6],  x[48]);
	sbr_swap(x[10], x[40]);
	sbr_swap(x[12], x[24]);
	sbr_swap(x[14], x[56]);
	sbr_swap(x[18], x[36]);
	sbr_swap(x[22], x[52]);
	sbr_swap(x[26], x[44]);
	sbr_swap(x[30], x[60]);
	sbr_swap(x[38], x[50]);
	sbr_swap(x[46], x[58]);

	voRadix8Pre32(x, 4);
	voRadix4Core32(x, 8, Tab_ptr);		
}

static void BubbleSort(unsigned char *v, int nItems)
{
	int i;
	unsigned char t;

	while (nItems >= 2) {
		for (i = 0; i < nItems-1; i++) {
			if (v[i+1] < v[i]) {
				t = v[i+1];	
				v[i+1] = v[i];	
				v[i] = t;
			}
		}
		nItems--;
	}
}

static int voGetFreqMap_0(unsigned char *freqBandMaster, 
						  int alterScale, 
						  int k0, 
						  int k2)
{
	int nMaster, k, nBands, k2Achieved, dk, vDk[64], k2Diff;

	if (alterScale) {
		dk = 2;
		nBands = 2 * ((k2 - k0 + 2) >> 2);
	} else {
		dk = 1;
		nBands = 2 * ((k2 - k0) >> 1);
	}

	nBands = (nBands < 63) ? nBands : 63;

	if (nBands <= 0)
		return 0;

	k2Achieved = k0 + nBands * dk;
	k2Diff = k2 - k2Achieved;
	for (k = 0; k < nBands; k++)
		vDk[k] = dk;

	if (k2Diff > 0) {
		k = nBands - 1;
		while (k2Diff) {
			vDk[k]++;
			k--;
			k2Diff--;
		}
	} else if (k2Diff < 0) {
		k = 0;
		while (k2Diff) {
			vDk[k]--;
			k++;
			k2Diff++;
		}
	}

	nMaster = nBands;
	freqBandMaster[0] = k0;
	for (k = 1; k <= nBands; k++)
		freqBandMaster[k] = freqBandMaster[k-1] + vDk[k-1];

	return nMaster;
}

/* mBandTab[i] = temp1[i] / 2 */
static const int vomBandTab[3] = {6, 5, 4};

static int voGetFreqMap(AACDecoder* decoder,
						unsigned char *freqBandMaster, 
						int freqScale, 
						int alterScale, 
						int k0, 
						int k2)
{
	int bands, twoRegions, k, k1, t, vLast, vCurr, pCurr;
	int invWarp, nBands0, nBands1, change;
	unsigned char vDk1Min, vDk0Max;
	unsigned char *vDelta;

	if (freqScale < 1 || freqScale > 3)
		return -1;

	bands = vomBandTab[freqScale - 1];

	if(alterScale == 0)
		invWarp = 0x40000000;
	else if(alterScale == 1)
		invWarp = 0x313b13b1;

	/* tested for all k0 = [5, 64], k2 = [k0, 64] */
	if (k2*10000 > 22449*k0) {
		twoRegions = 1;
		k1 = 2*k0;
	} else {
		twoRegions = 0;
		k1 = k2;
	}


	t = (log2Tab[k1] - log2Tab[k0]) >> 3;				
	nBands0 = 2 * (((bands * t) + (1 << 24)) >> 25);	

	t = voRatioPowInv(decoder,k1, k0, nBands0);
	pCurr = k0 << 24;
	vLast = k0;
	vDelta = freqBandMaster + 1;	
	for (k = 0; k < nBands0; k++) {
		pCurr = MULHIGH(pCurr, t) << 8;	
		vCurr = (pCurr + (1 << 23)) >> 24;
		vDelta[k] = (vCurr - vLast);
		vLast = vCurr;
	}

	BubbleSort(vDelta, nBands0);
	vDk0Max = vDelta[nBands0 -1];

	freqBandMaster[0] = k0;
	for (k = 1; k <= nBands0; k++)
		freqBandMaster[k] += freqBandMaster[k-1];

	if (!twoRegions)
		return nBands0;


	t = (log2Tab[k2] - log2Tab[k1]) >> 3;		
	t = MULHIGH(bands * t, invWarp) << 2;	
	nBands1 = 2 * ((t + (1 << 24)) >> 25);		
	nBands1 = MIN(nBands1, 64);
	if(nBands1 < 0)
		nBands1 = 0;

	t = voRatioPowInv(decoder,k2, k1, nBands1);
	pCurr = k1 << 24;
	vLast = k1;
	vDelta = freqBandMaster + nBands0 + 1;	

	pCurr = MULHIGH(pCurr, t) << 8;
	vCurr = (pCurr + (1 << 23)) >> 24;
	vDelta[0] = (vCurr - vLast);
	vDk1Min = vDelta[0];
	vLast = vCurr;

	for (k = 1; k < nBands1; k++) {
		pCurr = MULHIGH(pCurr, t) << 8;	/* keep in Q24 */
		vCurr = (pCurr + (1 << 23)) >> 24;
		vDelta[k] = (vCurr - vLast);
		if(vDelta[k] < vDk1Min)
			vDk1Min = vDelta[k];
		vLast = vCurr;
	}

	if (vDk1Min < vDk0Max) {
		BubbleSort(vDelta, nBands1);
		change = vDk0Max - vDelta[0];
		if (change > ((vDelta[nBands1 - 1] - vDelta[0]) >> 1))
			change = ((vDelta[nBands1 - 1] - vDelta[0]) >> 1);
		vDelta[0] += change;
		vDelta[nBands1-1] -= change;
	}
	BubbleSort(vDelta, nBands1);

	for (k = 1; k <= nBands1; k++)
		freqBandMaster[k + nBands0] += freqBandMaster[k + nBands0 - 1];

	return (nBands0 + nBands1);
}


static int voSBRdecUpdateHiRes(SBRFreq *hFreq,               
		               int crossOverBand                /* i: crossover band from header */
		              )
{
	int k, nHigh;
	unsigned char *freqBandHigh = hFreq->freqBandHigh;               /* o: high resolution freuency table */
	unsigned char *freqBandMaster = hFreq->freqBandMaster;           /* i: master frequency table */
	int nMaster = hFreq->nMaster;                            /* i: number of bands in master frequency table */

	nHigh = nMaster - crossOverBand;
	if(nHigh > VOMQ_BANDS)
		nHigh = VOMQ_BANDS;

	for (k = 0; k <= nHigh; k++)
		freqBandHigh[k] = freqBandMaster[k + crossOverBand];

	return nHigh;
}

static int voSBRdecUpdateLoRes(SBRFreq *hFreq)
{
	int k, j, nLow;
	unsigned char *freqBandLow  = hFreq->freqBandLow;          /* o: low resolution frequency table */
	unsigned char *freqBandHigh = hFreq->freqBandHigh;        /* i: high resolution frequency table */
	int  nHigh = hFreq->nHigh;                        /* i: the number of bands in high resolution frequency table*/

	if(nHigh & 0x01)
	{
		nLow = (nHigh + 1) >> 1;
		freqBandLow[0] = freqBandHigh[0];
		for(k=1, j=1; k<= nLow; k++, j+=2)
			freqBandLow[k] = freqBandHigh[j];
	}
	else
	{
		nLow = nHigh>>1;
		for(k=0, j=0; k<= nLow; k++, j+=2)
			freqBandLow[k] = freqBandHigh[j];
	}
	return nLow;
}

static int voSBRCalcFreqNoise(SBRFreq *hFreq,
			      int k2,                       /* i: index of last QMF subband */
			      int noiseBands                /* i: number of noise bands */
		)
{
	unsigned char *freqBandNoise = hFreq->freqBandNoise;     /* o: noise floor frequency table */
	unsigned char *freqBandLow   = hFreq->freqBandLow;       /* i: low resolution frequency table */
	int nLow = hFreq->nLow;                          /* i: number of bands in low resolution frequency table */
	int kStart = hFreq->kStart;                      /* i: index of starting QMF subbands for SBR */

	int i, iprev, k, nQ, kend, kstart;

	kend = log2Tab[k2];
	kstart = log2Tab[kStart];
	nQ = noiseBands*((kend - kstart) >> 2);	/* Q28 to Q26, noiseBands = [0,3] */
	nQ = (nQ + (1 << 25)) >> 26;
	if (nQ < 1)
		nQ = 1;

	nQ = MIN(nQ, MAX_NUM_NOISE_FLOOR_BANDS);

	iprev = 0;
	freqBandNoise[0] = freqBandLow[0];
	for (k = 1; k <= nQ; k++) {
		int c = (nQ + 1 - k);
		if(nLow - iprev < 0)
			i = 0;
		else
			i = iprev + (nLow - iprev)/c;

		freqBandNoise[k] = freqBandLow[i];
		iprev = i;
	}

	return nQ;
}


static int voHF_buildpatches(SBRFreq *hFreq,
		             int k0, 
			     int sampRateIdx
		            )
{
	unsigned char *pTemp1 = hFreq->patchNumSubbands;        /*o: starting subband for each patch */
	unsigned char *pTemp2 = hFreq->patchStartSubband;      /*o: number of subbands in each patch */
	unsigned char *pTemp3 = hFreq->freqBandMaster;
	int nMst  = hFreq->nMaster;
	int nKst  = hFreq->kStart;
	int nQmfB = hFreq->numQMFBands;
	int nMaxSb = k0;
	int nSb = nKst;
	int nPatIndex = 0;
	int sbValue = voGoalSBTab[sampRateIdx];
	int nIndex1, nIndex2;
	int voSuband, nTemp;

	if (nMst == 0) {
		pTemp1[0] = 0;
		pTemp2[0] = 0;
		return 0;
	}

	if (sbValue < nKst + nQmfB) 
	{
		nIndex2 = 0;
		for (nIndex1 = 0; pTemp3[nIndex1] < sbValue; nIndex1++)
			nIndex2 = nIndex1 + 1;
	} 
	else 
	{
		nIndex2 = nMst;
	}

	do {
		nIndex1 = nIndex2 + 1;
		do {
			nIndex1--;
			voSuband = pTemp3[nIndex1];
			nTemp = (voSuband - 2 + k0) & 0x01;
		} while (voSuband > k0 - 1 + nMaxSb - nTemp);

		pTemp1[nPatIndex] = MAX(voSuband - nSb, 0);
		pTemp2[nPatIndex] = k0 - nTemp - pTemp1[nPatIndex];

		if ((pTemp1[nPatIndex] < 3) && (nPatIndex > 0))
			break;

		if (pTemp1[nPatIndex] > 0) 
		{
			nSb = voSuband;
			nMaxSb = voSuband;
			nPatIndex++;
		} 
		else 
		{
			nMaxSb = nKst;
		}

		if (pTemp3[nIndex2] - voSuband < 3)
			nIndex2 = nMst;

	} while (voSuband != (nKst + nQmfB) && nPatIndex <= MAX_NUM_PATCHES);

	return nPatIndex;
}

static int voCalcFreqLim(SBRFreq *hFreq,            /* i/o: SBRFreq structure */
		           int limiterBands             /*   o: number of limiter bands */
			  )
{
	unsigned char *freqBandLim = hFreq->freqBandLim;
	unsigned char *patchNumSubbands = hFreq->patchNumSubbands;
	unsigned char *freqBandLow = hFreq->freqBandLow;
	unsigned char patchBorders[MAX_NUM_PATCHES + 1];

	int nLow = hFreq->nLow;                       /* number of bands in low resolution frequency table */
	int kStart = hFreq->kStart;                   /* index of starting QMF subband for SBR */                  
	int numPatches = hFreq->numPatches;           /* number of patches */
	int k, i, j, bands, nLimiter, nOctaves;
	int nBandLim1, nBandLim2;


	if(limiterBands == 1)
		bands = 120;
	else if(limiterBands == 2)
		bands = 200;
	else
		bands = 300;

	patchBorders[0] = kStart;

	for (k = 1; k < numPatches; k++)
		patchBorders[k] = patchBorders[k-1] + patchNumSubbands[k-1];
	patchBorders[k] = freqBandLow[nLow];

	for (k = 0; k <= nLow; k++)
		freqBandLim[k] = freqBandLow[k];

	for (k = 1; k < numPatches; k++)
		freqBandLim[k+nLow] = patchBorders[k];

	k = 1;
	nLimiter = nLow + numPatches - 1;
	BubbleSort(freqBandLim, nLimiter + 1);

	while (k <= nLimiter) {
		nOctaves = log2Tab[freqBandLim[k]] - log2Tab[freqBandLim[k-1]];	
		nOctaves = (nOctaves >> 9) * bands;	
		if (nOctaves < (49 << 19)) {

			nBandLim1 = 0;
			for (j = 0; j < (numPatches + 1); j ++)
			{
				if (patchBorders[j] == freqBandLim[k])
				{
					nBandLim1 = 1;
				}
			}

            nBandLim2 = 0;
			for (j = 0; j < (numPatches + 1); j++)
			{
				if (patchBorders[j] == freqBandLim[k-1])
				{
					nBandLim2 = 1;
				}
			}

			if (freqBandLim[k] == freqBandLim[k-1] || nBandLim1 == 0) {
				for(i = k; i < nLimiter; i++)
					freqBandLim[i] = freqBandLim[i+1];
				nLimiter--;
			} else if (nBandLim2 == 0) {
				for(i = k-1; i < nLimiter; i++)
					freqBandLim[i] = freqBandLim[i+1];
				nLimiter--;
			} else {
				k++;
			}
		} else {
			k++;
		}
	}

	for (k = 0; k <= nLimiter; k++)
		freqBandLim[k] -= kStart;

	return nLimiter;
}


int voSBRDecUpdateFreqTables(AACDecoder* decoder,    /*  i: AAC+ SBR decoder global struture */   
							 SBRHeader *sbrHdr, 
							 SBRFreq *hFreq,          /*i/o: SBRFreq structure for this SCE/CPE block */
							 int sampRateIdx         /*  o: sample rate index of output sample rate */
							 )
{
	int k0, k2;

	k0 = k0Tab[sampRateIdx][sbrHdr->startFreq];

	if (sbrHdr->stopFreq == 14)
		k2 = 2*k0;
	else if (sbrHdr->stopFreq == 15)
		k2 = 2*k0+k0;
	else
		k2 = k2Tab[sampRateIdx][sbrHdr->stopFreq];
	if (k2 > 64)
		k2 = 64;

	/* calculate master frequency table */
	if (sbrHdr->freqScale == 0)
		hFreq->nMaster = voGetFreqMap_0(hFreq->freqBandMaster, 
		                                  sbrHdr->alterScale, 
										  k0, 
										  k2);
	else
		hFreq->nMaster = voGetFreqMap(decoder,
		                                hFreq->freqBandMaster, 
										sbrHdr->freqScale, 
										sbrHdr->alterScale, 
										k0, 
										k2);

	if(hFreq->nMaster < sbrHdr->crossOverBand) 
		return -1;

	if(hFreq->nMaster > VOMQ_BANDS)
		return -1;

	/* calculate high frequency table and related parameters */
	hFreq->nHigh = voSBRdecUpdateHiRes(hFreq,sbrHdr->crossOverBand);

	hFreq->numQMFBands = hFreq->freqBandHigh[hFreq->nHigh] - hFreq->freqBandHigh[0];
	if(hFreq->numQMFBands >= VOMQ_BANDS) 
		return -1;
	else if(hFreq->numQMFBands < 0)
		return -1;		
	
	hFreq->kStart = hFreq->freqBandHigh[0];

	/* calculate low frequency table */
	hFreq->nLow = voSBRdecUpdateLoRes(hFreq);

	/* calculate noise floor frequency table */
	hFreq->NQ = voSBRCalcFreqNoise(hFreq, 
								k2, 
								sbrHdr->noiseBands);

	/* calculate limiter table */
	hFreq->numPatches = voHF_buildpatches(hFreq,
									        k0, 
									        sampRateIdx);
	if(hFreq->numPatches >= MAX_NUM_PATCHES)
		return -1;

	hFreq->nLimiter = voCalcFreqLim(hFreq, sbrHdr->limiterBands);

	return 0;
}

#endif