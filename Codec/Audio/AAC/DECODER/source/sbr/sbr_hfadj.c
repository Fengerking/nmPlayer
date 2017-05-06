	/************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "sbr_dec.h"
#include "vosbr_ghdr.h"

#ifdef SBR_DEC

/*
* Brief: High frequency Envelop estimate 
*/

static void voEstimateEnv(AACDecoder *decoder,          /* i/o:   AAC decoder global info structure */   
						  SBRHeader  *sbrHdr,           /* i:   SBR header information structure */
						  SBRGrid    *hGrid,			/* i:   SBRGrid structure for this channel */
						  SBRFreq    *hFreq,			/* i:   SBRFreq structure for this SCE/CPE block */
						  int        env				/* i:   the number of SBR envelopes */
						  )
{
	int i, j, k, m, n, re, im;
	int eScale, eMax, invFactor, tEcurr;
	int iStart, iEnd, jStart, jEnd;
	int *nBuf, kStart;
	unsigned char *freqTab;
	VO_U64 voEval;
	unsigned int hi, low;
	sbr_info *psi = (sbr_info *)decoder->sbr;

	iStart = hGrid->t_E[env] + SBR_HF_ADJ;
	iEnd =   hGrid->t_E[env+1] + SBR_HF_ADJ;

	kStart = hFreq->kStart;

	eMax = 0;
	if (sbrHdr->interpFreq) 
	{
		for (k = 0; k < hFreq->numQMFBands; k++) 
		{
			voEval = 0;
			nBuf = psi->XBuf[iStart][kStart + k];

			for (i = iStart; i < iEnd; i++) 
			{
				re = nBuf[0] >> SBR_OUT_QMFA;	
				im = nBuf[1] >> SBR_OUT_QMFA;

				nBuf += 128;
				voEval = MAC32(voEval, re, re);
				voEval = MAC32(voEval, im, im);
			}

			hi = (unsigned int)(voEval >> 32);
			low = (unsigned int)voEval;
			if (hi) 
			{
				eScale = (32 - CLZ(hi)) + 1;
				tEcurr  = low >> eScale;
				tEcurr |= hi << (32 - eScale);
			} 
			else if (low >> 31) 
			{
				eScale = 1;
				tEcurr  = low >> 1;
			} 
			else 
			{
				eScale = 0;
				tEcurr  = low;
			}

			eScale += 1;
			invFactor = vo_invBandTab[(iEnd - iStart)-1];
			psi->eCurrExp[k] = eScale;	
			psi->eCurr[k] = MULHIGH(tEcurr, invFactor);
			if (eScale > eMax)
				eMax = eScale;
		}
	} 
	else 
	{
		if (hGrid->freqRes[env]) {
			n = hFreq->nHigh;
			freqTab = hFreq->freqBandHigh;
		} else {
			n = hFreq->nLow;
			freqTab = hFreq->freqBandLow;
		}

		for (m = 0; m < n; m++) 
		{
			jStart = freqTab[m];
			jEnd =   freqTab[m+1];

			voEval = 0;
			for (i = iStart; i < iEnd; i++) 
			{
				nBuf = psi->XBuf[i][jStart];
				for (j = jStart; j < jEnd; j++) 
				{
					re = (*nBuf++) >> SBR_OUT_QMFA;
					im = (*nBuf++) >> SBR_OUT_QMFA;
					voEval = MAC32(voEval, re, re);
					voEval = MAC32(voEval, im, im);
				}
			}

			hi = (unsigned int)(voEval >> 32);
			low = (unsigned int)voEval;
			if (hi) 
			{
				eScale = (32 - CLZ(hi)) + 1;
				tEcurr = low >> eScale;		
				tEcurr |= hi << (32 - eScale);
			} 
			else if (low >> 31) 
			{
				eScale = 1;
				tEcurr  = low >> eScale;		
			} 
			else 
			{
				eScale = 0;
				tEcurr  = low;
			}

			invFactor = vo_invBandTab[(iEnd - iStart)-1];
			invFactor = vo_Multi32(vo_invBandTab[(jEnd - jStart)-1], invFactor);
			tEcurr = MULHIGH(tEcurr, invFactor);

			eScale +=  1;
			for (k = jStart; k < jEnd; k++) 
			{
				psi->eCurrExp[k - kStart] = eScale;	
				psi->eCurr[k - kStart] = tEcurr;				
			}

			if (eScale > eMax)
				eMax = eScale;
		}
	}
	psi->eCurrExpMax = eMax;
}

static int voGetSinuMapped(SBRGrid *hGrid,         /* i: SBRGrid structure for this channel */
			   SBRFreq *hFreq,                     /* i: SBRFreq structure for this SCE/CPE block */
			   SBRChan *hChan,                     /* i: SBRChan structure for this channel */
			   int     env,                        /* i: index of current envelope */
			   int     nband,                      /* i: index of current QMF band */
			   int     lA                           /* i: lA flag for this envelope */
			  )
{
	int n, nStart, nEnd;
	int odd, index;
	VO_U8 *addHarmonic0, *addHarmonic1;
	VO_U8 *freqBandHigh;

	if (hGrid->freqRes[env]) 
	{
		nStart = nband;
		nEnd = nband+1;
	} 
	else 
	{
		odd = hFreq->nHigh & 1;
		nStart = nband > 0 ? (2*nband - odd) : 0;		
		nEnd = 2*(nband+1) - odd;				
	}

	addHarmonic0 = hChan->addHarmonic[0];
	addHarmonic1 = hChan->addHarmonic[1];
	freqBandHigh = hFreq->freqBandHigh;
	for (n = nStart; n < nEnd; n++) 
	{
		if (addHarmonic1[n]) 
		{
			index = (freqBandHigh[n+1] + freqBandHigh[n]) >> 1;

			if (env >= lA || addHarmonic0[index] == 1)
				return 1;
		}
	}

	return 0;
}


static void voCalcMaxGain(sbr_info *psi,               /* i/o: AAC+ SBR info structure */
						SBRHeader *hInf,               /*   i: AAC+ SBR header structure */
						SBRGrid *hGrid,                /*   i: SBRGrid structure */
						SBRFreq *hFreq,                /*   i: SBR freq info structure */
						int nch,                       /*   i: index of current channel */
						int env,                       /*   i: index of current envelope */
						int limBand,                   /*   i: index of current limiter band */
						int fDQbits                    /*   i: number of fraction bits */
						) 
{
	int n, nStart, nEnd;
	int q, z, r;
	int sumEOrigMap, sumECurr;
	int Gainmax, teOMGainMax, tenvBand;
	int kStart;
	unsigned char *freqTab;
	unsigned char tCurrExpMax;
	unsigned char tlimGains;


	nStart = hFreq->freqBandLim[limBand];   
	nEnd =   hFreq->freqBandLim[limBand + 1];

	freqTab = hGrid->freqRes[env] ? hFreq->freqBandHigh : hFreq->freqBandLow;

	sumECurr = 0; sumEOrigMap = 0;
	tCurrExpMax = psi->eCurrExpMax;
	teOMGainMax = psi->eOMGainMax;
	tenvBand = psi->envBand;
	kStart  = hFreq->kStart;
	tlimGains = hInf->limiterGains;

	for (n = nStart; n < nEnd; n++) 
	{
		if (n == freqTab[tenvBand + 1] - kStart) 
		{
			tenvBand++;
			teOMGainMax = psi->envDataDequant[nch][env][tenvBand] >> SBR_ACC_SCALE;	
		}
		sumEOrigMap += teOMGainMax;

		sumECurr += (psi->eCurr[n] >> (tCurrExpMax - psi->eCurrExp[n]));
		if (sumECurr >> 30) 
		{
			sumECurr >>= 1;
			tCurrExpMax++;
		}
	}

	psi->envBand = tenvBand;
	psi->eOMGainMax = teOMGainMax;


	if (sumECurr == 0) 
	{
		Gainmax = (sumEOrigMap == 0 ? vo_limGainTab[tlimGains] : 0x80000000);
		psi->gainMaxFBits = 30;
	} 
	else if (sumEOrigMap == 0) 
	{
		Gainmax = 0;
		psi->gainMaxFBits = 30;
	} 
	else 
	{
		Gainmax = vo_limGainTab[tlimGains];
		if (tlimGains != 3) 
		{
			q = MULHIGH(sumEOrigMap,Gainmax);	
			z = vo_norm(sumECurr);         
			r = voInvRNormal(sumECurr << z);	
			Gainmax = MULHIGH(q, r);			
			psi->gainMaxFBits = 26 + tCurrExpMax + fDQbits - z - SBR_ACC_SCALE;
		}
	}

	psi->gainMax = Gainmax;
	psi->sumEOrigMapped = sumEOrigMap;
}

static void voCalcNoiseFactors(int Q,     /* i: dequantized noise floor scalefactor */
							   int *qp1Inv,            /* o: 1/(1 + Q) */
							   int *qqp1Inv            /* o: Q/(1 + Q) */
							   )
{
	int n, s;
	int qp1, tmp;

	qp1  = (Q >> 1);
	qp1 += (1 << (SBR_OUT_DQ_NOISE - 1));		              
	n = CLZ(qp1) - 1;				              
	qp1 <<= n;					              
	tmp = voInvRNormal(qp1) << 1;			             

	s = (31 - (SBR_OUT_DQ_NOISE - 1) - n - 1);	              
	*qp1Inv = (tmp >> s);					      
	*qqp1Inv = MULHIGH(tmp, Q) << (32 - SBR_OUT_DQ_NOISE - s);
}


static int voCalcSubbandGains(sbr_info   *psi,         /* i/o: AAC+ SBR info structure */
							  SBRGrid    *hGrid,                       /* SBRGrid structure  */
							  SBRFreq    *hFreq,                       /* SBRFreq structure  */
							  SBRChan    *hChan,                       /* SBRChan structure  */
							  int        nch,                          /* index of current channel */
							  int        env,                          /* index of current envelope */
							  int        limBand,                      /* index of current limiter band */
							  int        fDQbits                       /* number of fraction bits  */
							  )
{
	int n, s, qs;
	int nStart, nEnd;
	int Qm, Sm, tmp, dlA;
	int noiseIndex, sMapIndex;
	int shift, eCurrEnv, gFlag, highBand, sBand;
	int Gain, Gainml, fGmlbits, Gainmax, GainmaxBits;
	int kStart;
	unsigned char *freqTab, *freqHigh;
	unsigned char *addHarmonic0, *addHarmonic1;

	nStart = hFreq->freqBandLim[limBand];   
	nEnd =   hFreq->freqBandLim[limBand + 1];
	kStart = hFreq->kStart;

	addHarmonic0 = hChan->addHarmonic[0];
	addHarmonic1 = hChan->addHarmonic[1];
	Gainmax = psi->gainMax;
	GainmaxBits = psi->gainMaxFBits;

	dlA = (env == psi->la || env == hChan->laPrev ? 0 : 1);

	freqTab = (hGrid->freqRes[env] ? hFreq->freqBandHigh : hFreq->freqBandLow);
	freqHigh = hFreq->freqBandHigh;

	noiseIndex = 0;
	if (hGrid->L_Q == 2 && hGrid->t_Q[1] <= hGrid->t_E[env])
		noiseIndex++;

	psi->sumECurrGLim = 0;  psi->sumSM = 0;  psi->sumQM = 0;
	highBand = psi->highBand;  sBand = psi->sBand;

	for (n = nStart; n < nEnd; n++) 
	{
		if (n == hFreq->freqBandNoise[psi->noiseFloorBand + 1] - kStart) 
		{
			int qMapped;
			psi->noiseFloorBand++;
			qMapped = psi->noiseDataDequant[nch][noiseIndex][psi->noiseFloorBand];			

			voCalcNoiseFactors(qMapped, &(psi->qp1Inv), &(psi->qqp1Inv));
		}

		if (n == freqHigh[highBand + 1] - kStart)
			highBand++;

		if (n == freqTab[sBand + 1] - kStart) 
		{
			sBand++;
			psi->sMapped = voGetSinuMapped(hGrid, hFreq, hChan, env, sBand, psi->la);
		}

		sMapIndex = 0;
		tmp = (freqHigh[highBand+1] + freqHigh[highBand]) >> 1;

		if (n + kStart == tmp) 
		{
			if (env >= psi->la || addHarmonic0[tmp] == 1)
				sMapIndex = addHarmonic1[highBand];
		}

		if (env == hGrid->L_E - 1) 
		{
			if (n + kStart == tmp)
				addHarmonic0[n + kStart] = addHarmonic1[highBand];
			else
				addHarmonic0[n + kStart] = 0;
		}

		Gain = psi->envDataDequant[nch][env][sBand];
		Qm = vo_Multi32(Gain, psi->qqp1Inv);
		Sm = (sMapIndex ? vo_Multi32(Gain, psi->qp1Inv) : 0);
		if (dlA == 1 && psi->sMapped == 0)
			Gain = vo_Multi32(psi->qp1Inv, Gain);
		else if (psi->sMapped != 0)
			Gain = vo_Multi32(psi->qqp1Inv, Gain);

		eCurrEnv = psi->eCurr[n];
		if (eCurrEnv) 
		{
			s = vo_norm(eCurrEnv);
			tmp = voInvRNormal(eCurrEnv << s);		
			Gainml = MULHIGH(Gain, tmp);	
			fGmlbits = 28 + psi->eCurrExp[n] + fDQbits - s;
		} 
		else 
		{
			Gainml = Gain;
			fGmlbits = fDQbits;
		}

		gFlag = 0;
		if (Gainmax != (int)0x80000000)
		{
			if (fGmlbits >= GainmaxBits) 
			{
				shift = MIN(fGmlbits - GainmaxBits, 31);
				gFlag = ((Gainml >> shift) > Gainmax ? 1 : 0);
			} 
			else 
			{
				shift = MIN(GainmaxBits - fGmlbits, 31);
				gFlag = (Gainml > (Gainmax >> shift) ? 1 : 0);
			}
		}

		if (gFlag) 
		{
			qs = 0;
			tmp = Gainml;	
			if(Gainml<=0)
			{
				return -1;
			}

			s = CLZ(tmp);
			if (s < 16) 
			{
				qs = 16 - s; 	tmp >>= qs;	
			}

			s = vo_norm(Gainmax);
			tmp = (Gainmax << s) / tmp;		
			qs = (GainmaxBits + s) - (fGmlbits - qs);	
			if (qs > 30) 
			{
				tmp >>= MIN(qs - 30, 31);
			} 
			else 
			{
				s = MIN(30 - qs, 30);
				CLIP_2N_SHIFT(tmp, s);	
			}

			Qm = MULHIGH(Qm, tmp) << 2;
			Gain = MULHIGH(Gain, tmp) << 2;
			psi->G_LimBuf[n] = Gainmax;
			psi->G_LimFbits[n] = GainmaxBits;
		} 
		else 
		{
			psi->G_LimBuf[n] = Gainml;
			psi->G_LimFbits[n] = fGmlbits;
		}

		psi->Sm_Buf[n] = Sm;
		psi->sumSM += (Sm >> SBR_ACC_SCALE);

		psi->Qm_LimBuf[n] = Qm;

		if (env != psi->la && env != hChan->laPrev && Sm == 0)
			psi->sumQM += (Qm >> SBR_ACC_SCALE);

		if (eCurrEnv)
			psi->sumECurrGLim += (Gain >> SBR_ACC_SCALE);
	}

	psi->highBand = highBand;
	psi->sBand = sBand;

	return 0;
}

static int voCalcBoostGain(sbr_info *psi,               /* i/o: The SBR info structure */
						   SBRFreq  *hFreq,				/*   i: SBRFreq structure for this SCE/CPE block */
						   int      limBand,            /*   i: index of current limiter band */
						   int      fDQbits             /*   i: number of fraction bits in dequantized envelope */
						   )
{
	int n, q, s;
	int tmp;
	int nStart, nEnd;
	int sumEOrigMap, G_Boost;

	nStart = hFreq->freqBandLim[limBand];   
	nEnd =   hFreq->freqBandLim[limBand + 1];

	sumEOrigMap = psi->sumEOrigMapped >> 1;
	tmp = (psi->sumECurrGLim >> 1) + (psi->sumSM >> 1) + (psi->sumQM >> 1);	
	if (tmp < 8) 
	{
		G_Boost = (sumEOrigMap == 0 ? (1 << 28) : SBR_GBOOST_MAX);
		s = 0;
	} 
	else if (sumEOrigMap == 0) 
	{
		G_Boost = 0;
		s = 0;
	} 
	else 
	{
		s = CLZ(tmp) - 1;	
		tmp = voInvRNormal(tmp << s);
		G_Boost = MULHIGH(sumEOrigMap, tmp);
	}

	if (G_Boost > (SBR_GBOOST_MAX >> s)) 
	{
		G_Boost = SBR_GBOOST_MAX;
		s = 0;
	}

	G_Boost <<= s;	

	for (n = nStart; n < nEnd; n++) 
	{
		q = MULHIGH(psi->G_LimBuf[n], G_Boost) << 2;	
		tmp = voSqrtFix(q, psi->G_LimFbits[n] - 2, &s);

		s -= SBR_GLIM_BOOST;
		if (s >= 0) 
		{
			s = MIN(s, 31);
			psi->G_LimBoost[n] = tmp >> s;
		} 
		else 
		{
			s = MIN(30, -s);
			CLIP_2N_SHIFT(tmp, s);
			psi->G_LimBoost[n] = tmp;
		}

		//if want to perfectly support sine tone test clips, you have to comment these source code
		//such as fix ID 6939 bug.
		//comment these source code for support sine tone clips. #7947
		//if(psi->G_LimBoost[m] > 0x10000000) //avoid noise,just experience
		//{
		//	return -1;
		//}

		q = MULHIGH(psi->Qm_LimBuf[n], G_Boost) << 2;	
		tmp = voSqrtFix(q, fDQbits - 2, &s);

		s -= SBR_QLIM_BOOST;		
		if (s >= 0) 
		{
			s = MIN(s, 31);
			psi->Qm_LimBoost[n] = tmp >> s;
		} 
		else 
		{
			s = MIN(30, -s);
			CLIP_2N_SHIFT(tmp, s);
			psi->Qm_LimBoost[n] = tmp;
		}

		//if(psi->Qm_LimBoost[n] > 0x34567890)
		//{
		//	return -1;
		//}

		q = MULHIGH(psi->Sm_Buf[n], G_Boost) << 2;		
		tmp = voSqrtFix(q, fDQbits - 2, &s);

		s -= SBR_OUT_QMFA;		
		if (s >= 0) 
		{
			s = MIN(s, 31);
			psi->Sm_Boost[n] = tmp >> s;
		} 
		else 
		{
			s = MIN(30, -s);
			CLIP_2N_SHIFT(tmp, s);
			psi->Sm_Boost[n] = tmp;
		}
	}

	return 0;
}

/*
* Brief: Calculation Gain 
*/
static int voCalcGain(sbr_info *psi,           /* i/o: AAC+ SBR mode information */
					SBRHeader *sbrHdr, 
					SBRGrid *sbrGrid,          /*   i: SBRGrid structure for this channel */
					SBRFreq *sbrFreq,          /*   i: SBRFreq structure for thus SCE/CPE block */
					SBRChan *sbrChan,          /*   i: SBRChan structure for this channel */
					int ch,                    /*   i: index of current channel (0 for SCE, 0 or 1 for CPE)*/
					int env                    /*   i: index of current envelope */
					)
{
	int n, fDQbits, ret;

	psi->envBand        = -1; psi->noiseFloorBand = -1;
	psi->sBand          = -1; psi->highBand       = -1;

	fDQbits = (SBR_OUT_DQ_ENV - psi->envDataDequantScale[ch][env]);	/* Q(29 - optional scalefactor) */
	for (n = 0; n < sbrFreq->nLimiter; n++) {
		/* the QMF bands are divided into lim regions (consecutive, non-overlapping) */
		voCalcMaxGain(psi, 
			          sbrHdr, 
					  sbrGrid, 
					  sbrFreq, 
					  ch, 
					  env, 
					  n, 
					  fDQbits);
		ret = voCalcSubbandGains(psi, sbrGrid, sbrFreq, sbrChan, ch, env, n, fDQbits);
		if(ret < 0)
			return -1;
		ret = voCalcBoostGain(psi, sbrFreq, n, fDQbits);
		if(ret < 0)
			return -1;
	}

	return 0;
}

static int voAssemHF(sbr_info   *psi,               /*   i/o: AAC+ SBR mode information */
					 SBRHeader  *sbrHdr,            /*   i: SBR Header information structure */
					 SBRGrid    *hGrid,             /*   i: SBRGrid structure for this channel */
					 SBRFreq    *hFreq,             /*   i: SBRFreq structure for thus SCE/CPE block */
					 SBRChan    *hChan,             /*   i: SBRChan structure for this channel */
					 int        env,                /*   i: Index of SBR envelope */
					 int        reset
					 )
{
	int i, m, idx, j, s, n;
	int fIndexNoise, fIndexSine, gIndexRing;
	int hSL, iStart, iEnd;
	int G_Filt, Q_Filt, gbMaxBits, gbIndex;
	int X_re, X_im, Sm_re, Sm_im;
	int *XBuf;
	int *G_Temp, *Q_Temp;

	fIndexSine  =  hChan->fIndexSine;
	fIndexNoise =  hChan->fIndexNoise;
	gIndexRing  =  hChan->gIndexRing;	

	if (reset) fIndexNoise = 2;	

	hSL = (sbrHdr->smoothMode ? 0 : 4); 

	if (reset) 
	{
		for (i = 0; i < hSL; i++) 
		{
			G_Temp = hChan->G_Temp[gIndexRing];
			Q_Temp = hChan->Q_Temp[gIndexRing];
			for (m = 0; m < hFreq->numQMFBands; m++) 
			{
				G_Temp[m] = psi->G_LimBoost[m];
				Q_Temp[m] = psi->Qm_LimBoost[m];
			}

			gIndexRing++;
			if (gIndexRing == MAX_NUM_SMOOTH_COEFS)
				gIndexRing = 0;
		}
	}

	iStart = hGrid->t_E[env];
	iEnd =   hGrid->t_E[env+1];
	for (i = iStart; i < iEnd; i++) 
	{
		G_Temp = hChan->G_Temp[gIndexRing];
		Q_Temp = hChan->Q_Temp[gIndexRing];
		if (i - iStart < MAX_NUM_SMOOTH_COEFS) 
		{
			for (m = 0; m < hFreq->numQMFBands; m++) 
			{
				G_Temp[m] = psi->G_LimBoost[m];
				Q_Temp[m] = psi->Qm_LimBoost[m];
			}
		}

		gbMaxBits = 0;
		XBuf = psi->XBuf[i + SBR_HF_ADJ][hFreq->kStart];

		for (m = 0; m < hFreq->numQMFBands; m++) 
		{
			if (env == psi->la || env == hChan->laPrev) 
			{
				if (i == iStart) 
				{
					psi->G_FiltLast[m] = G_Temp[m];
#ifdef LITMITED
					if(psi->G_FiltLast[m] >= 0x10000000) 
						psi->G_FiltLast[m] = 0x1000000;
#endif
					psi->Q_FiltLast[m] = 0;
				}
			}
			else if(hSL)
			{
				if (i - iStart < MAX_NUM_SMOOTH_COEFS) 
				{
					G_Filt = 0;
					Q_Filt = 0;
					idx = gIndexRing;
					for (j = 0; j < MAX_NUM_SMOOTH_COEFS; j++) 
					{
						G_Filt += MULHIGH(hChan->G_Temp[idx][m], vo_hSmoothCoef[j]);
						Q_Filt += MULHIGH(hChan->Q_Temp[idx][m], vo_hSmoothCoef[j]);
						idx--;
						if (idx < 0)
							idx += MAX_NUM_SMOOTH_COEFS;
					}
#ifdef LITMITED
					if(G_Filt >= 0x8000000) 
						G_Filt = 0x800000;
					if(Q_Filt >= 0x8000)
						Q_Filt = 0x4000;
#endif
					psi->G_FiltLast[m] = G_Filt << 1;	
					psi->Q_FiltLast[m] = Q_Filt << 1;	
				}
			}
			else if (hSL == 0) 
			{
				if (i == iStart) 
				{
					psi->G_FiltLast[m] = G_Temp[m];
#ifdef LITMITED
					if(psi->G_FiltLast[m] >= 0x10000000) 
						psi->G_FiltLast[m] = 0x1000000;
#endif
					psi->Q_FiltLast[m] = Q_Temp[m];
#ifdef LITMITED
					if(psi->Q_FiltLast[m] >= 0x10000)
					{
						psi->Q_FiltLast[m] = 0x8000;
					}
#endif
				}
			} 

			if (psi->Sm_Boost[m] != 0) 
			{
				Sm_re = psi->Sm_Boost[m];
				Sm_im = Sm_re;

				s = (fIndexSine >> 1);		s <<= 31;
				Sm_re ^= (s >> 31);			Sm_re -= (s >> 31);
				s ^= ((m + hFreq->kStart) << 31);
				Sm_im ^= (s >> 31);			Sm_im -= (s >> 31);

				s = fIndexSine << 31;		Sm_im &= (s >> 31);
				s ^= 0x80000000;			Sm_re &= (s >> 31);

				fIndexNoise += 2;		
			} 
			else 
			{
				Q_Filt = psi->Q_FiltLast[m];	
				n = noiseTab[fIndexNoise++];
				Sm_re = MULHIGH(n, Q_Filt) >> (SBR_QLIM_BOOST - SBR_OUT_QMFA - 1 );

				n = noiseTab[fIndexNoise++];
				Sm_im = MULHIGH(n, Q_Filt) >> (SBR_QLIM_BOOST - SBR_OUT_QMFA - 1 );
			}

			fIndexNoise &= 1023;	

			G_Filt = psi->G_FiltLast[m];
			X_re = MULHIGH(G_Filt, XBuf[0]);
			X_im = MULHIGH(G_Filt, XBuf[1]);

			CLIP_2N_SHIFT(X_re, 32 - SBR_GLIM_BOOST);
			CLIP_2N_SHIFT(X_im, 32 - SBR_GLIM_BOOST);

			X_re += Sm_re;	
			*XBuf++ = X_re;
			X_im += Sm_im;	
			*XBuf++ = X_im;

			gbMaxBits |= ABS(X_re);
			gbMaxBits |= ABS(X_im);
		}

		gIndexRing++;
		if (gIndexRing == MAX_NUM_SMOOTH_COEFS)
			gIndexRing = 0;

		fIndexSine++;
		fIndexSine &= 3;

		if (gbMaxBits >> (31 - SBR_GBITS_IN_QMFS)) 
		{
			int maxBits = 31 - SBR_GBITS_IN_QMFS;
			XBuf = psi->XBuf[i + SBR_HF_ADJ][hFreq->kStart];

			for (m = 0; m < hFreq->numQMFBands; m++) 
			{
				X_re = XBuf[0];	
				X_im = XBuf[1];	

				CLIP_2N(X_re, maxBits);	
				CLIP_2N(X_im, maxBits);	

				*XBuf++ = X_re;	
				*XBuf++ = X_im;
			}	
			CLIP_2N(gbMaxBits, maxBits);	
		}

		gbIndex = ((i + SBR_HF_ADJ) >> 5) & 0x01;
		hChan->gbMask[gbIndex] |= gbMaxBits;
	}

	hChan->fIndexNoise =  fIndexNoise;
	hChan->fIndexSine  =  fIndexSine;
	hChan->gIndexRing  =  gIndexRing;

	return 0;
}

static int voResetHF(sbr_info *psi,              /* i/o: AAC+ SBR mode information */
					 SBRGrid  *hGrid,                    /*   i: SBRGrid structure for this channel */
					 SBRFreq  *hFreq,                    /*   i: SBRFreq structure for thus SCE/CPE block */
					 SBRChan  *hChan                     /*   i: SBRChan structure for this channel */
					 )
{
	int i, m;
	int env, iStart, iEnd;
	int *XBuf;
	int kStart, freqlim;

	kStart  = hFreq->kStart;
	freqlim = hFreq->freqBandLim[0];

	for (i = 0; i < freqlim + kStart; i++)
		hChan->addHarmonic[0][i] = 0;

	freqlim = hFreq->freqBandLim[hFreq->nLimiter];

	for (i = freqlim + kStart; i < 64; i++)
		hChan->addHarmonic[0][i] = 0;

	/* save lA for next frame */
	if (psi->la == hGrid->L_E)
		hChan->laPrev = 0;
	else
		hChan->laPrev = -1;

	for (env = 0; env < hGrid->L_E; env++) {
		iStart = hGrid->t_E[env];
		iEnd =   hGrid->t_E[env+1];
		for (i = iStart; i < iEnd; i++) {
			XBuf = psi->XBuf[i + SBR_HF_ADJ][hFreq->kStart];
			for (m = 0; m < hFreq->numQMFBands; m++) {
				*XBuf++ = 0;	
				*XBuf++ = 0;
			}	
		}
	}
	return 0;
}

int voHFAdj(AACDecoder *decoder,       /*   i/o: AAC+ decoder global structure */
		SBRHeader  *sbrHdr,            /*   i: SBR Header information structure */
	    SBRGrid    *hGrid,             /*   i: SBRGrid structure for this channel */
	    SBRFreq    *hFreq,             /*   i: SBRFreq structure for thus SCE/CPE block */
	    SBRChan    *hChan,             /*   i: SBRChan structure for this channel */
	    int        ch                  /*   i: index of current channle (0 for SCE, 0 or 1 for CPE)*/
	    )
{
	int i, n;
	int reset, kStart, freqLim;
	unsigned char FrameType, pointer;
	unsigned char *addHarmonic0;
	sbr_info *psi = (sbr_info *)decoder->sbr;

	FrameType = hGrid->FrameType;
	pointer  = hGrid->ptr;

	if ((FrameType == voFIXVAR || FrameType == voVARVAR) && pointer > 0)
		psi->la = hGrid->L_E + 1 - pointer;
	else if (FrameType == voVARFIX && pointer > 1)
		psi->la = pointer - 1;
	else
		psi->la = -1;

	reset = hChan->reset;
	for (n = 0; n < hGrid->L_E; n++) {
		voEstimateEnv(decoder,sbrHdr, hGrid, hFreq, n);

		if(voCalcGain(psi, sbrHdr, hGrid, hFreq, hChan, ch, n) < 0)
			goto END;

		if(voAssemHF(psi, sbrHdr, hGrid, hFreq, hChan, n, reset) < 0)
			goto END;
		reset = 0;	
	}

	kStart  = hFreq->kStart;
    freqLim = hFreq->freqBandLim[0];
	addHarmonic0 = hChan->addHarmonic[0];
	for (i = 0; i < freqLim + kStart; i++)
		addHarmonic0[i] = 0;

	freqLim = hFreq->freqBandLim[hFreq->nLimiter];
	for (i = freqLim + kStart; i < 64; i++)
		addHarmonic0[i] = 0;

	if (psi->la == hGrid->L_E)
		hChan->laPrev = 0;
	else
		hChan->laPrev = -1;

	return 0;

END:
	voResetHF(psi,hGrid, hFreq, hChan);
	return -1;
}
#endif