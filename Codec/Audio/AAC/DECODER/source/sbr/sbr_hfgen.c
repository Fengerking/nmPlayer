	/************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "sbr_dec.h"
#include "vosbr_ghdr.h"

#ifdef SBR_DEC


#if (!(defined(_IOS))) && ((defined (__arm) && defined (__ARMCC_VERSION)) || (defined (_WIN32_WCE) && defined (ARM)) || (defined(__GNUC__) && defined(__arm__)))
#ifdef __cplusplus
extern "C"
#endif
void CVKernel1(int *pIn, int *pSum);
#else
void CVKernel1(int *pIn, int *pSum)
{
	U64 nre1, nim1, nre12, nim12, nre11, nre22;
	int n, nrex0, nimx0, nrex1, nimx1;

	nrex0 = pIn[0];
	nimx0 = pIn[1];
	pIn += 128;
	nrex1 = pIn[0];
	nimx1 = pIn[1];
	pIn += 128;

	nre1.w64 = nim1.w64 = 0;
	nre12.w64 = nim12.w64 = 0;
	nre11.w64 = 0;
	nre22.w64 = 0;

	nre12.w64 = MAC32(nre12.w64,  nrex1, nrex0);
	nre12.w64 = MAC32(nre12.w64,  nimx1, nimx0);
	nim12.w64 = MAC32(nim12.w64,  nrex0, nimx1);
	nim12.w64 = MAC32(nim12.w64, -nimx0, nrex1);
	nre22.w64 = MAC32(nre22.w64,  nrex0, nrex0);
	nre22.w64 = MAC32(nre22.w64,  nimx0, nimx0);
	for (n = (NUM_TIME_SLOTS*voSAMP_PER_SLOT + 6); n != 0; n--) {
		nrex0 = nrex1;
		nimx0 = nimx1;
		nrex1 = pIn[0];
		nimx1 = pIn[1];

		nre1.w64 = MAC32(nre1.w64,  nrex1, nrex0);
		nre1.w64 = MAC32(nre1.w64,  nimx1, nimx0);
		nim1.w64 = MAC32(nim1.w64,  nrex0, nimx1);
		nim1.w64 = MAC32(nim1.w64, -nimx0, nrex1);
		nre11.w64 = MAC32(nre11.w64,  nrex0, nrex0);
		nre11.w64 = MAC32(nre11.w64,  nimx0, nimx0);

		pIn += 128;
	}
	nre12.w64 += nre1.w64;
	nre12.w64 = MAC32(nre12.w64, nrex1, -nrex0);
	nre12.w64 = MAC32(nre12.w64, nimx1, -nimx0);
	nim12.w64 += nim1.w64;
	nim12.w64 = MAC32(nim12.w64, nrex0, -nimx1);
	nim12.w64 = MAC32(nim12.w64, nimx0,  nrex1);
	nre22.w64 += nre11.w64;
	nre22.w64 = MAC32(nre22.w64, nrex0, -nrex0);
	nre22.w64 = MAC32(nre22.w64, nimx0, -nimx0);

	pSum[0]  = nre1.r.lo32;	
	pSum[1]  = nre1.r.hi32;
	pSum[2]  = nim1.r.lo32;	
	pSum[3]  = nim1.r.hi32;
	pSum[4]  = nre11.r.lo32;	
	pSum[5]  = nre11.r.hi32;
	pSum[6]  = nre12.r.lo32;
	pSum[7]  = nre12.r.hi32;
	pSum[8]  = nim12.r.lo32;	
	pSum[9]  = nim12.r.hi32;
	pSum[10] = nre22.r.lo32;
	pSum[11] = nre22.r.hi32;
}
#endif

static int voCalcCov1(int *pIn, 
		      int *nre1N, 
		      int *nim1N, 
		      int *nre12N, 
		      int *nim12N, 
		      int *nre11N, 
		      int *nre22N
		      )
{
	int pSum[2*6];
	int n, z, s, loShift, hiShift, gbMask;
	U64 nre1, nim1, nre12, nim12, nre11, nre22;

	CVKernel1(pIn, pSum);
	nre1.r.lo32 = pSum[0];
	nre1.r.hi32 = pSum[1];
	nim1.r.lo32 = pSum[2];	
	nim1.r.hi32 = pSum[3];
	nre11.r.lo32 = pSum[4];	
	nre11.r.hi32 = pSum[5];
	nre12.r.lo32 = pSum[6];	
	nre12.r.hi32 = pSum[7];
	nim12.r.lo32 = pSum[8];	
	nim12.r.hi32 = pSum[9];
	nre22.r.lo32 = pSum[10];	
	nre22.r.hi32 = pSum[11];


	gbMask  = ((nre1.r.hi32) ^ (nre1.r.hi32 >> 31)) | ((nim1.r.hi32) ^ (nim1.r.hi32 >> 31));
	gbMask |= ((nre12.r.hi32) ^ (nre12.r.hi32 >> 31)) | ((nim12.r.hi32) ^ (nim12.r.hi32 >> 31));
	gbMask |= ((nre11.r.hi32) ^ (nre11.r.hi32 >> 31)) | ((nre22.r.hi32) ^ (nre22.r.hi32 >> 31));
	if (gbMask == 0) {
		s = nre1.r.hi32 >> 31; gbMask  = (nre1.r.lo32 ^ s) - s;
		s = nim1.r.hi32 >> 31; gbMask |= (nim1.r.lo32 ^ s) - s;
		s = nre12.r.hi32 >> 31; gbMask |= (nre12.r.lo32 ^ s) - s;
		s = nim12.r.hi32 >> 31; gbMask |= (nim12.r.lo32 ^ s) - s;
		s = nre11.r.hi32 >> 31; gbMask |= (nre11.r.lo32 ^ s) - s;
		s = nre22.r.hi32 >> 31; gbMask |= (nre22.r.lo32 ^ s) - s;
		z = 32 + CLZ(gbMask);
	} else {
		gbMask  = ABS(nre1.r.hi32) | ABS(nim1.r.hi32);
		gbMask |= ABS(nre12.r.hi32) | ABS(nim12.r.hi32);
		gbMask |= ABS(nre11.r.hi32) | ABS(nre22.r.hi32);
		z = CLZ(gbMask);
	}

	n = 64 - z;	/* number of non-zero bits in bottom of 64-bit word */
	if (n <= 30) {
		loShift = (30 - n);
		*nre1N = nre1.r.lo32 << loShift;	*nim1N = nim1.r.lo32 << loShift;
		*nre12N = nre12.r.lo32 << loShift;	*nim12N = nim12.r.lo32 << loShift;
		*nre11N = nre11.r.lo32 << loShift;	*nre22N = nre22.r.lo32 << loShift;
		return -(loShift + 2 * SBR_OUT_QMFA);
	} else if (n < 32 + 30) {
		loShift = (n - 30);
		hiShift = 32 - loShift;
		*nre1N = (nre1.r.hi32 << hiShift) | (nre1.r.lo32 >> loShift);
		*nim1N = (nim1.r.hi32 << hiShift) | (nim1.r.lo32 >> loShift);
		*nre12N = (nre12.r.hi32 << hiShift) | (nre12.r.lo32 >> loShift);
		*nim12N = (nim12.r.hi32 << hiShift) | (nim12.r.lo32 >> loShift);
		*nre11N = (nre11.r.hi32 << hiShift) | (nre11.r.lo32 >> loShift);
		*nre22N = (nre22.r.hi32 << hiShift) | (nre22.r.lo32 >> loShift);
		return (loShift - 2*SBR_OUT_QMFA);
	} else {
		hiShift = n - (32 + 30);
		*nre1N = nre1.r.hi32 >> hiShift;	*nim1N = nim1.r.hi32 >> hiShift;
		*nre12N = nre12.r.hi32 >> hiShift;	*nim12N = nim12.r.hi32 >> hiShift;
		*nre11N = nre11.r.hi32 >> hiShift;	*nre22N = nre22.r.hi32 >> hiShift;
		return (32 - 2*SBR_OUT_QMFA - hiShift);
	}

	return 0;
}

#if (!(defined(_IOS))) && ((defined (__arm) && defined (__ARMCC_VERSION)) || (defined (_WIN32) && defined (_WIN32_WCE) && defined (ARM)) || (defined(__GNUC__) && defined(__arm__)))
#ifdef __cplusplus
extern "C"
#endif
void CVKernel2(int *pIn, int *pSum);
#else
void CVKernel2(int *pIn, int *pSum)
{
	U64 nre2, nim2;
	int n, nrex0, nimx0, nrex1, nimx1, x2re, x2im;

	nre2.w64 = nim2.w64 = 0;

	nrex0 = pIn[0];
	nimx0 = pIn[1];
	pIn += 128;
	nrex1 = pIn[0];
	nimx1 = pIn[1];
	pIn += 128;

	for (n = (NUM_TIME_SLOTS*voSAMP_PER_SLOT + 6); n != 0; n--) {
		/* 6 input, 2*2 acc, 1 ptr, 1 loop counter = 12 registers (use same for nimx0, -nimx0) */
		x2re = pIn[0];
		x2im = pIn[1];

		nre2.w64 = MAC32(nre2.w64,  x2re, nrex0);
		nre2.w64 = MAC32(nre2.w64,  x2im, nimx0);
		nim2.w64 = MAC32(nim2.w64,  nrex0, x2im);
		nim2.w64 = MAC32(nim2.w64, -nimx0, x2re);

		nrex0 = nrex1;
		nimx0 = nimx1;
		nrex1 = x2re;
		nimx1 = x2im;
		pIn += 128;
	}

	pSum[0] = nre2.r.lo32;
	pSum[1] = nre2.r.hi32;
	pSum[2] = nim2.r.lo32;
	pSum[3] = nim2.r.hi32;
}
#endif

static int voCalcCov2(int *pIn, 
		      int *nre2N, 
		      int *nim2N)
{
	U64 nre2, nim2;
	int n, z, s, loShift, hiShift, gbMask;
	int pSum[2*2];

	CVKernel2(pIn, pSum);
	nre2.r.lo32 = pSum[0];
	nre2.r.hi32 = pSum[1];
	nim2.r.lo32 = pSum[2];
	nim2.r.hi32 = pSum[3];


	gbMask  = ((nre2.r.hi32) ^ (nre2.r.hi32 >> 31)) | ((nim2.r.hi32) ^ (nim2.r.hi32 >> 31));
	if (gbMask == 0) {
		s = nre2.r.hi32 >> 31; gbMask  = (nre2.r.lo32 ^ s) - s;
		s = nim2.r.hi32 >> 31; gbMask |= (nim2.r.lo32 ^ s) - s;
		z = 32 + CLZ(gbMask);
	} else {
		gbMask  = ABS(nre2.r.hi32) | ABS(nim2.r.hi32);
		z = CLZ(gbMask);
	}
	n = 64 - z;	/* number of non-zero bits in bottom of 64-bit word */

	if (n <= 30) {
		loShift = (30 - n);
		*nre2N = nre2.r.lo32 << loShift;	
		*nim2N = nim2.r.lo32 << loShift;
		return -(loShift + 2*SBR_OUT_QMFA);
	} else if (n < 32 + 30) {
		loShift = (n - 30);
		hiShift = 32 - loShift;
		*nre2N = (nre2.r.hi32 << hiShift) | (nre2.r.lo32 >> loShift);
		*nim2N = (nim2.r.hi32 << hiShift) | (nim2.r.lo32 >> loShift);
		return (loShift - 2*SBR_OUT_QMFA);
	} else {
		hiShift = n - (32 + 30);
		*nre2N = nre2.r.hi32 >> hiShift;	
		*nim2N = nim2.r.hi32 >> hiShift;
		return (32 - 2*SBR_OUT_QMFA - hiShift);
	}

	return 0;
}

/* 
 * Function: voCalcLPC
 * Description: calculate linear prediction coefficients for one subband (4.6.18.6.2)
 *               coefficients clipped to (-4, 4), so format = Q29
 */

static int voCalcLPC(int *pIn, 
					 int *a0re, 
		             int *a0im, 
		             int *a1re, 
		             int *a1im, 
		             int gb
					 )
{
	int zFlag, n1, n2, nd, d, dInv, tre, tim;
	int nre1, nim1, nre2, nim2, nre12, nim12, nre11, nre22;

	if (gb < 3) {
		nd = 3 - gb;
		for (n1 = (40); n1 != 0; n1--) {
			pIn[0] >>= nd;	pIn[1] >>= nd;
			pIn += (128);
		}
		pIn -= (5120);
	}
	
	/* calculate covariance elements */
	n1 = voCalcCov1(pIn, &nre1, &nim1, &nre12, &nim12, &nre11, &nre22);
	n2 = voCalcCov2(pIn, &nre2, &nim2);

	/* normalize everything to larger power of 2 scalefactor, call it n1 */
	if (n1 < n2) {
		nd = MIN(n2 - n1, 31);
		nre1 >>= nd;	nim1 >>= nd;
		nre12 >>= nd;	nim12 >>= nd;
		nre11 >>= nd;	nre22 >>= nd;
		n1 = n2;
	} else if (n1 > n2) {
		nd = MIN(n1 - n2, 31);
		nre2 >>= nd;	nim2 >>= nd;
	}

	/* calculate determinant of covariance matrix (at least 1 GB in pXX) */
	d = MULHIGH(nre12, nre12) + MULHIGH(nim12, nim12);
	d = MULHIGH(d, RELAX_COEF) << 1;
	d = MULHIGH(nre11, nre22) - d;
#if ERROR_CHECK
	if(!(d >= 0))
	{
		return -1;		
	}
#endif
	zFlag = 0;
	*a0re = *a0im = 0;
	*a1re = *a1im = 0;
	if (d > 0) {
		nd = CLZ(d) - 1;
		d <<= nd;
		dInv = voInvRNormal(d);

		/* 1 GB in pXX */
		tre = MULHIGH(nre1, nre12) - MULHIGH(nim1, nim12) - MULHIGH(nre2, nre11);
		tre = MULHIGH(tre, dInv);
		tim = MULHIGH(nre1, nim12) + MULHIGH(nim1, nre12) - MULHIGH(nim2, nre11);
		tim = MULHIGH(tim, dInv);

		/* if d is extremely small, just set coefs to 0 (would have poor precision anyway) */
		if (nd > 28 || (ABS(tre) >> (28 - nd)) >= 4 || (ABS(tim) >> (28 - nd)) >= 4) {
			zFlag = 1;
		} else {
			*a1re = tre << (FBITS_LPCOEFS - 28 + nd);	/* i.e. convert Q(28 - nd) to Q(29) */
			*a1im = tim << (FBITS_LPCOEFS - 28 + nd);
		}
	}

	if (nre11) {
		nd = CLZ(nre11) - 1;	/* assume positive */
		nre11 <<= nd;
		dInv = voInvRNormal(nre11);

		/* a1re, a1im = Q29, so scaled by (n1 + 3) */
		tre = (nre1 >> 3) + MULHIGH(nre12, *a1re) + MULHIGH(nim12, *a1im);
		tre = -MULHIGH(tre, dInv);
		tim = (nim1 >> 3) - MULHIGH(nim12, *a1re) + MULHIGH(nre12, *a1im);
		tim = -MULHIGH(tim, dInv);

		if (nd > 25 || (ABS(tre) >> (25 - nd)) >= 4 || (ABS(tim) >> (25 - nd)) >= 4) {
			zFlag = 1;
		} else {
			*a0re = tre << (FBITS_LPCOEFS - 25 + nd);	/* i.e. convert Q(25 - nd) to Q(29) */
			*a0im = tim << (FBITS_LPCOEFS - 25 + nd);
		}
	} 

	/* see 4.6.18.6.2 - if magnitude of a0 or a1 >= 4 then a0 = a1 = 0 
	 * i.e. a0re < 4, a0im < 4, a1re < 4, a1im < 4
	 * Q29*Q29 = Q26
	 */
	if (zFlag || MULHIGH(*a0re, *a0re) + MULHIGH(*a0im, *a0im) >= MAG_16 || MULHIGH(*a1re, *a1re) + MULHIGH(*a1im, *a1im) >= MAG_16) {
		*a0re = *a0im = 0;
		*a1re = *a1im = 0;
	}

	/* no need to clip - we never changed the pIn data, just used it to calculate a0 and a1 */
	if (gb < 3) {
		nd = 3 - gb;
		for (n1 = (NUM_TIME_SLOTS*voSAMP_PER_SLOT + 6 + 2); n1 != 0; n1--) {
			pIn[0] <<= nd;	pIn[1] <<= nd;
			pIn += (2*64);
		}
	}

	return 0;
}

/*
* Function:    voHFGen
* Description: generate high frequencies with SBR (4.6.18.6)
*/

int voHFGen(sbr_info   *psi,              /*  i/o: sbr_info sbr_info struct ptr */
			 SBRGrid    *pGrid,            /*  i: SBRGrid struct for this channel */
			 SBRFreq    *pFreq,            /*  i: SBRFreq struct for this SCE/CPE block */
			 SBRChan    *pChan             /*  i: SBRChan struct for this channel */
			 )
{
	int oldBW, newBW, tmpBW, tmp1, tmp2; 
	int band, gb, gbMask, gbIdx;
	int currPatch, p, x, k, g, i, iStart, iEnd, bwA, bwAsq;
	int ap0re, ap0im, ap1re, ap1im;
	int nrex1, nimx1, x2re, x2im;
	int XH_re, XH_im;
	int *XBufLo, *XBufHi;

	for (band = 0; band < pFreq->NQ; band++) 
	{
		tmp1  = pChan->invfMode[0][band];
		tmp2  = pChan->invfMode[1][band];
		oldBW = pChan->chirpFact[band]; 
		newBW = vo_newBWTab[tmp1][tmp2];

		if (newBW < oldBW)
			tmpBW = Vo_Multi32(0x20000000, oldBW) + Vo_Multi32(newBW, 0x60000000);	
		else
			tmpBW = Vo_Multi32(0x0c000000, oldBW) + Vo_Multi32(newBW, 0x74000000);	
		tmpBW <<= 1;

		if (tmpBW < 0x02000000)	
			tmpBW = 0;
		if (tmpBW > 0x7f800000)	
			tmpBW = 0x7f800000;

		pChan->chirpFact[band] = tmpBW;
		pChan->invfMode[0][band] = pChan->invfMode[1][band];
	}

	tmp1 = pGrid->L_E;
	iStart = pGrid->t_E[0] + SBR_HF_ADJ;
	iEnd   = pGrid->t_E[tmp1] + SBR_HF_ADJ;

	k = pFreq->kStart;
	g = 0;
	bwA = pChan->chirpFact[g];
	bwAsq = Vo_Multi32(bwA, bwA) << 1;
	
	gbMask = (pChan->gbMask[0] | pChan->gbMask[1]);	/* older 32 | newer 8 */
	//gb = CLZ(gbMask) - 1;
	gb = vo_norm(gbMask);

	for (currPatch = 0; currPatch < pFreq->numPatches; currPatch++) 
	{
		for (x = 0; x < pFreq->patchNumSubbands[currPatch]; x++) {
			/* map k to corresponding noise floor band */
			if (k >= pFreq->freqBandNoise[g+1]) {
				g++;
				bwA = pChan->chirpFact[g];		/* Q31 */
				bwAsq = Vo_Multi32(bwA, bwA) << 1;	/* Q31 */
			}

			if(k >= 64)
				break;
		
			p = pFreq->patchStartSubband[currPatch] + x;
			XBufHi = psi->XBuf[iStart][k];
			if (bwA) {
				if(voCalcLPC(psi->XBuf[0][p], &ap0re, &ap0im, &ap1re, &ap1im, gb) < 0)
					return -1;

				ap0re = Vo_Multi32(bwA, ap0re);
				ap0im = Vo_Multi32(bwA, ap0im);
				ap1re = Vo_Multi32(bwAsq, ap1re);
				ap1im = Vo_Multi32(bwAsq, ap1im);

				XBufLo = psi->XBuf[iStart-2][p];

				x2re = XBufLo[0];	
				x2im = XBufLo[1];	
				XBufLo += 128;

				nrex1 = XBufLo[0];	
				nimx1 = XBufLo[1];	
				XBufLo += 128;

				for (i = iStart; i < iEnd; i++) {

					XH_re = Vo_Multi32(x2re, ap1re) - Vo_Multi32(x2im, ap1im);
					XH_im = Vo_Multi32(x2re, ap1im) + Vo_Multi32(x2im, ap1re);
					x2re = nrex1;
					x2im = nimx1;
					
					XH_re += Vo_Multi32(nrex1, ap0re) - Vo_Multi32(nimx1, ap0im);
					XH_im += Vo_Multi32(nrex1, ap0im) + Vo_Multi32(nimx1, ap0re);
					nrex1 = XBufLo[0];
					nimx1 = XBufLo[1];
					XBufLo += 128;


					XH_re = L_shl(XH_re, 4);
					XH_im = L_shl(XH_im, 4);

					XH_re += nrex1;
					XH_im += nimx1;

					XBufHi[0] = XH_re;
					XBufHi[1] = XH_im;
					XBufHi += 128;

					/* update guard bit masks */
					gbMask  = ABS(XH_re);
					gbMask |= ABS(XH_im);
					gbIdx = (i >> 5) & 0x01;	/* 0 if i < 32, 1 if i >= 32 */
					pChan->gbMask[gbIdx] |= gbMask;
				}
			} 
			else
		       	{
				XBufLo = (int *)psi->XBuf[iStart][p];
				for (i = iStart; i < iEnd; i++) 
				{
					XBufHi[0] = XBufLo[0];
					XBufHi[1] = XBufLo[1];
					XBufLo += 128; 
					XBufHi += 128;
				}
			}
			k++;	/* high QMF band */
		}
	}

	return 0;
}

#endif
