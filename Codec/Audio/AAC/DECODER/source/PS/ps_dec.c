/************************************************************************
*									*									*
*		VisualOn, Inc. Confidential and Proprietary, 2004	*
*									*									*
************************************************************************/

//#include "common.h"
#include "decoder.h"
#include "sbr_dec.h"
#ifdef PS_DEC


#include "ps_dec.h"
#include "ps_tables.h"
/* constants */
#define NEGATE_IPD_MASK            (0x1000)
#define DECAY_SLOPE                Q31(0.05)
#define COEF_SQRT2                 Q30(1.4142135623731)

static const int p8_13_20[7] =
{
    0x00f479f9,
	0x02e7f8b7,
	0x05d1eac2,
	0x094cf5d0,
	0x0ca72702,
	0x0f189026,
	0x10000000
};

static const int p2_13_20[7] =
{
    0,
	0x026e6c90,
	0,
	0xf6aa2f25,
	0,
	0x2729e766,
	0x40000000
};

static const int filter_a[NO_ALLPASS_LINKS] = { /* a(m) = exp(-d_48kHz(m)/7) */
    0x53625ae4,
	0x4848aef5,
	0x3ea94d15
};

static const unsigned char groupBorder20[10+12 + 1] =
{
    6, 7, 0, 1, 2, 3, /* 6 subqmf subbands */
	9, 8,             /* 2 subqmf subbands */
	10, 11,           /* 2 subqmf subbands */
	3, 4, 5, 6, 7, 8, 9, 11, 14, 18, 23, 35, 64
};


static const unsigned short mapGroupbk20[10+12] =
{
    (NEGATE_IPD_MASK | 1), (NEGATE_IPD_MASK | 0),
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
};

#define gamma  Q30(1.5)
const  int gamma2 = Q14(1.0);

/*
* hybrid filter init
*/
hyb_info *hybrid_init(VO_MEM_OPERATOR *voMemop)
{
    hyb_info *hyb = (hyb_info*)voAACDecAlignedMalloc(voMemop, sizeof(hyb_info));
	
    hyb->resolution20[0] = 8;
    hyb->resolution20[1] = 2;
    hyb->resolution20[2] = 2;
	
    hyb->framelen = 32;

    return hyb;
}

void hybrid_free(VO_MEM_OPERATOR *voMemop, 
				 hyb_info *hyb)
{
	SafeAlignedFree(hyb);
}

void ChannelFilter2(unsigned char frame_len, 
					const int *filter,
                    int *buffer, 
					int *XHybrid, 
					int *tmpBuf)
{
    int i;
	int *tXHybrid;
	int *top;
	int *bottom;
	int re0;
	int re1;
	int im0; 
	int im1;
	int rTmp;
	int iTmp;
    
	tXHybrid = XHybrid;
	for (i = 0; i < frame_len; i++)
    {
        bottom = buffer + i*2;
		top    = buffer + (7+i)*2;
		rTmp = MUL_32(filter[0],(bottom[0] + top[10]));
        iTmp = MUL_32(filter[0],(bottom[1] + top[11]));

		re0 = re1 = rTmp;
		im0 = im1 = iTmp;

        rTmp = MUL_32(filter[1],(bottom[2] + top[8]));
        iTmp = MUL_32(filter[1],(bottom[3] + top[9]));

		re0 += rTmp;
		re1 -= rTmp;
		im0 += iTmp;
		im1 -= iTmp;

        rTmp = MUL_32(filter[2],(bottom[4] + top[6]));
        iTmp = MUL_32(filter[2],(bottom[5] + top[7]));

		re0 += rTmp;
		re1 += rTmp;
		im0 += iTmp;
		im1 += iTmp;

        rTmp = MUL_32(filter[3],(bottom[6] + top[4]));
        iTmp = MUL_32(filter[3],(bottom[7] + top[5]));

		re0 += rTmp;
		re1 -= rTmp;
		im0 += iTmp;
		im1 -= iTmp;

        rTmp = MUL_32(filter[4],(bottom[8] + top[2]));
        iTmp = MUL_32(filter[4],(bottom[9] + top[3]));

		re0 += rTmp;
		re1 += rTmp;
		im0 += iTmp;
		im1 += iTmp;

        rTmp = MUL_32(filter[5],(bottom[10] + top[0]));
        iTmp = MUL_32(filter[5],(bottom[11] + top[1]));

		re0 += rTmp;
		re1 -= rTmp;
		im0 += iTmp;
		im1 -= iTmp;

        rTmp = MUL_32(filter[6],(bottom[12]));
        iTmp = MUL_32(filter[6],(bottom[13]));

		re0 += rTmp;
		re1 += rTmp;
		im0 += iTmp;
		im1 += iTmp;
		
        tXHybrid[0] = re0 << 1;
        tXHybrid[1] = im0 << 1;
		
        tXHybrid[2] = re1 << 1;
        tXHybrid[3] = im1 << 1;
		
		tXHybrid += 24;
    }
}

static void INLINE DCT3_4_unscaled(int *y, int *x)
{
    int f0, f1, f2, f3, f4, f5, f6, f7, f8;
	
    f0 = MUL_31(x[2], 0x5a82799a);
    f1 = x[0] - f0;
    f2 = x[0] + f0;
    f3 = x[1] + x[3];
    f4 = MUL_30(x[1], 0x14e7ae91*4);
    f5 = MUL_31(f3, 0x89be50c3);
    f6 = MUL_31(x[3], 0xbaba1611);
    f7 = f4 + f5;
    f8 = f6 - f5;
    y[3] = f2 - f8;
    y[0] = f2 + f8;
    y[2] = f1 - f7;
    y[1] = f1 + f7;
}

void ChannelFilter8(unsigned char frame_len, 
					const int *filter,
                    int *buffer,
					int *XHybrid, 
					int *tmpBuf)
{
    int i, n;
    int *tBuffer, *tXHybrid;
	int *re1, *re2, *im1, *im2;
	int *x;
	
	re1 = tmpBuf; 
	re2 = tmpBuf + 4; 
	im1 = tmpBuf + 8;
	im2 = tmpBuf + 12;
	x = tmpBuf + 16;

	tXHybrid = XHybrid;

    for (i = 0; i < frame_len; i++)
    {
        tBuffer = buffer + 2*i;

		re1[0] =  MUL_32(filter[6],tBuffer[12]);
		im2[0] =  MUL_32(filter[6],tBuffer[13]);

        re1[1] =  MUL_32(filter[5],(tBuffer[10] + tBuffer[14]));
		im2[1] =  MUL_32(filter[5],(tBuffer[11] + tBuffer[15]));

        re1[2] = -MUL_32(filter[0],(tBuffer[0] + tBuffer[24])) + 
				  MUL_32(filter[4],(tBuffer[8] + tBuffer[16]));
        im2[2] = -MUL_32(filter[0],(tBuffer[1] + tBuffer[25])) + 
				  MUL_32(filter[4],(tBuffer[9] + tBuffer[17]));

        re1[3] = -MUL_32(filter[1],(tBuffer[2] + tBuffer[22])) + 
				  MUL_32(filter[3],(tBuffer[6] + tBuffer[18]));
		im2[3] = -MUL_32(filter[1],(tBuffer[3] + tBuffer[23])) + 
			      MUL_32(filter[3],(tBuffer[7] + tBuffer[19]));
		
		re2[0] = MUL_32(filter[5],(tBuffer[14] - tBuffer[10]));
		im1[0] = MUL_32(filter[5],(tBuffer[15] - tBuffer[11]));

		re2[1] = MUL_32(filter[0],(tBuffer[24] - tBuffer[0])) + 
				 MUL_32(filter[4],(tBuffer[16] - tBuffer[8]));
		im1[1] = MUL_32(filter[0],(tBuffer[25] - tBuffer[1])) + 
				 MUL_32(filter[4],(tBuffer[17] - tBuffer[9]));

        re2[2] = MUL_32(filter[1],(tBuffer[22] - tBuffer[2])) + 
				 MUL_32(filter[3],(tBuffer[18] - tBuffer[6]));
		im1[2] = MUL_32(filter[1],(tBuffer[23] - tBuffer[3])) + 
				 MUL_32(filter[3],(tBuffer[19] - tBuffer[7]));

        re2[3] = MUL_32(filter[2],(tBuffer[20] - tBuffer[4]));
		im1[3] = MUL_32(filter[2],(tBuffer[21] - tBuffer[5]));
		

        for (n = 0; n < 4; n++)
        {
            x[n] = (re1[n] - im1[3-n]) << 1;
        }
        DCT3_4_unscaled(x, x);

		tXHybrid[14] = x[0];
        tXHybrid[10] = x[2];
        tXHybrid[6] = x[3];
        tXHybrid[2] = x[1];
        
        for (n = 0; n < 4; n++)
        {
            x[n] = (re1[n] + im1[3-n]) << 1;
        }
        DCT3_4_unscaled(x, x);
		tXHybrid[12] = x[1];
        tXHybrid[8] = x[3];
        tXHybrid[4] = x[2];
        tXHybrid[0] = x[0];

        for (n = 0; n < 4; n++)
        {
            x[n] = (im2[n] + re2[3-n]) << 1;
        }
        DCT3_4_unscaled(x, x);

		tXHybrid[15] = x[0];
        tXHybrid[11] = x[2];
        tXHybrid[ 7] = x[3];
        tXHybrid[ 3] = x[1];

        for (n = 0; n < 4; n++)
        {
            x[n] = (im2[n] - re2[3-n]) << 1;
        }
        DCT3_4_unscaled(x, x);

        tXHybrid[13] = x[1];
        tXHybrid[ 9] = x[3];
        tXHybrid[ 5] = x[2];
        tXHybrid[ 1] = x[0];

		tXHybrid += 24;
    }
}

/*
* Hybrid Analysis function 
*/
void HybridAnalysis(VO_MEM_OPERATOR *voMemop, 
					       hyb_info *hyb, 
					       int *XBuf, 
					       int *XHybrid)
{
    int k, n, band;
    int offset = 0;
    int qmf_bands = 3;
    unsigned char *resolution = hyb->resolution20;
	int *tXBuf, *WorkBuf, *Tmp, *tXHybrid;
	
    for (band = 0; band < qmf_bands; band++)
    {
		voMemop->Copy(VO_INDEX_DEC_AAC, hyb->WorkBuffer[0], hyb->SubQMFBuffer[band], 12*sizeof(int)*2);
		
		WorkBuf = hyb->WorkBuffer[12];
		tXBuf = XBuf + 6*128 + band*2;
		for (n = 0; n < hyb->framelen; n++)
        {
            *WorkBuf++ = tXBuf[0]; 
            *WorkBuf++ = tXBuf[1]; 

			tXBuf += 128;
        }
		
		WorkBuf = hyb->WorkBuffer[hyb->framelen];

        voMemop->Copy(VO_INDEX_DEC_AAC, hyb->SubQMFBuffer[band], WorkBuf, 12*sizeof(int)*2);

		WorkBuf = hyb->WorkBuffer[0];
		Tmp = hyb->TmpBuffer[0][0];
        switch(resolution[band])
        {
        case 2:
             /* Type B */
			ChannelFilter2(hyb->framelen, p2_13_20, WorkBuf, Tmp, hyb->uBuffer);
            break;
        case 8:
            /* Type A */
            ChannelFilter8(hyb->framelen, p8_13_20, WorkBuf, Tmp, hyb->uBuffer);
            break;
        }
		
		for (n = 0; n < hyb->framelen; n++)
        {
            tXHybrid = XHybrid + n*64 + offset*2;
			Tmp = hyb->TmpBuffer[n][0];
			for (k = 0; k < resolution[band]; k++)
            {
                *tXHybrid++  = *Tmp++;
                *tXHybrid++  = *Tmp++;
            }
        }

        offset += resolution[band];
    }
	
	tXHybrid = XHybrid;
	for (n = 0; n < 32 ; n++)
	{
		*(tXHybrid + 6) += *(tXHybrid + 8);
		*(tXHybrid + 7) += *(tXHybrid + 9);
		*(tXHybrid + 8) = 0;
		*(tXHybrid + 9) = 0;

		*(tXHybrid + 4) += *(tXHybrid + 10);
		*(tXHybrid + 5) += *(tXHybrid + 11);
		*(tXHybrid + 10) = 0;
		*(tXHybrid + 11) = 0;

		tXHybrid += 64;
	}

}

void HybridSynthesis(hyb_info *hyb, 
							int *XBuf, 
							int *XHybrid)
{
    int k, n, band;
    int offset = 0;
    int qmf_bands = 3;
    unsigned char *resolution = hyb->resolution20;
	int iReal, iImage;
	int *tXHybrid, *tXBuf;
	
    for(band = 0; band < qmf_bands; band++)
    {
        tXBuf = XBuf + band*2;
		for (n = 0; n < hyb->framelen; n++)
        {
            iReal = 0;
            iImage = 0;
			tXHybrid = XHybrid + n*64 + offset*2;

            for (k = 0; k < resolution[band]; k++)
            {
                iReal  += *tXHybrid++;
                iImage += *tXHybrid++;
            }

			tXBuf[0] = iReal;
			tXBuf[1] = iImage;

			tXBuf += 128;
		}

        offset += resolution[band];
    }
}

static char limitMinMax(char i, char min, char max)
{
    if (i < min)
        return min;
    else if (i > max)
        return max;
    else
        return i;
}

void DeltaDecArray(unsigned char enable, 
				   char *index, 
				   char *indexPrev,
                   unsigned char dtFlag, 
				   unsigned char nrPara, 
				   unsigned char stride,
                   char minIdx, 
				   char maxIdx)
{
    int i;
	
    if (enable == 1)
    {
        if (dtFlag == 0)
        {
            index[0] = 0 + index[0];
            index[0] = limitMinMax(index[0], minIdx, maxIdx);
			
            for (i = 1; i < nrPara; i++)
            {
                index[i] = index[i-1] + index[i];
                index[i] = limitMinMax(index[i], minIdx, maxIdx);
            }
        } else {
            for (i = 0; i < nrPara; i++)
            {
                index[i] = indexPrev[i*stride] + index[i];
                index[i] = limitMinMax(index[i], minIdx, maxIdx);
            }
        }
    } else {
        for (i = 0; i < nrPara; i++)
        {
            index[i] = 0;
        }
    }
	
    if (stride == 2)
    {
        for (i = (nrPara<<1)-1; i > 0; i--)
        {
            index[i] = index[i>>1];
        }
    }
}

static void map34indexto20(char *pIn)
{
    pIn[0] = (2*pIn[0]+pIn[1])/3;
    pIn[1] = (pIn[1]+2*pIn[2])/3;
    pIn[2] = (2*pIn[3]+pIn[4])/3;
    pIn[3] = (pIn[4]+2*pIn[5])/3;
    pIn[4] = (pIn[6]+pIn[7])/2;
    pIn[5] = (pIn[8]+pIn[9])/2;
    pIn[6] = pIn[10];
    pIn[7] = pIn[11];
    pIn[8] = (pIn[12]+pIn[13])/2;
    pIn[9] = (pIn[14]+pIn[15])/2;
    pIn[10] = pIn[16];
	pIn[11] = pIn[17];
	pIn[12] = pIn[18];
	pIn[13] = pIn[19];
	pIn[14] = (pIn[20]+pIn[21])/2;
	pIn[15] = (pIn[22]+pIn[23])/2;
	pIn[16] = (pIn[24]+pIn[25])/2;
	pIn[17] = (pIn[26]+pIn[27])/2;
	pIn[18] = (pIn[28]+pIn[29]+pIn[30]+pIn[31])/4;
	pIn[19] = (pIn[32]+pIn[33])/2;

}

void psDataInit(ps_info *ps)
{
    int env;
	int bin;
	int num_iid_steps = (ps->IidMode < 3) ? 7 : 15; 
	
    if (ps->bAvailblePsData == 0)
    {
        ps->NumEnv = 0;
    }
	
    for (env = 0; env < ps->NumEnv; env++)
    {
        char *iid_index_prev;
        char *icc_index_prev;
		
        if (env == 0)
        {
            iid_index_prev = ps->IidIndexPrev;
            icc_index_prev = ps->IccIndexPrev;
        } else {
            iid_index_prev = ps->IidIndex[env - 1];
            icc_index_prev = ps->IccIndex[env - 1];
        }
		
        DeltaDecArray(ps->bEnableIid, ps->IidIndex[env], iid_index_prev,
            ps->IidDtFlag[env], ps->NrIidNum,
            (ps->IidMode == 0 || ps->IidMode == 3) ? 2 : 1,
            -num_iid_steps, num_iid_steps);
		
        DeltaDecArray(ps->bEnableIcc, ps->IccIndex[env], icc_index_prev,
            ps->IccDtFlag[env], ps->NrIccNum,
            (ps->IccMode == 0 || ps->IccMode == 3) ? 2 : 1,
            0, 7);
	}
	
    if (ps->NumEnv == 0)
    {
        ps->NumEnv = 1;
		
        if (ps->bEnableIid)
        {
            for (bin = 0; bin < 34; bin++)
                ps->IidIndex[0][bin] = ps->IidIndexPrev[bin];
        } else {
            for (bin = 0; bin < 34; bin++)
                ps->IidIndex[0][bin] = 0;
        }
		
        if (ps->bEnableIcc)
        {
            for (bin = 0; bin < 34; bin++)
                ps->IccIndex[0][bin] = ps->IccIndexPrev[bin];
        } else {
            for (bin = 0; bin < 34; bin++)
                ps->IccIndex[0][bin] = 0;
        }
    }
	
    for (bin = 0; bin < 34; bin++)
        ps->IidIndexPrev[bin] = ps->IidIndex[ps->NumEnv-1][bin];
    for (bin = 0; bin < 34; bin++)
        ps->IccIndexPrev[bin] = ps->IccIndex[ps->NumEnv-1][bin];
	
    ps->bAvailblePsData = 0;
	
    if (ps->bFrameClass == 0)
    {
		int shift = 0;

		switch (ps->NumEnv){
		case 1:
			shift = 0;              
			break;
		case 2:
			shift = 1;              
			break;
		case 4:
			shift = 2;              
			break;
		}
		ps->aEnvStartStop[0] = 0;
		for (env = 1; env < ps->NumEnv; env++)
        {
            ps->aEnvStartStop[env] = (env * 32 ) >> shift;
        }
        ps->aEnvStartStop[ps->NumEnv] = 32;
    } else {
        ps->aEnvStartStop[0] = 0;
		
        if (ps->aEnvStartStop[ps->NumEnv] < 32)
        {
            for (bin = 0; bin < 34; bin++)
            {
                ps->IidIndex[ps->NumEnv][bin] = ps->IidIndex[ps->NumEnv-1][bin];
                ps->IccIndex[ps->NumEnv][bin] = ps->IccIndex[ps->NumEnv-1][bin];
            }

			ps->NumEnv++;
            ps->aEnvStartStop[ps->NumEnv] = 32;
        }
		
        for (env = 1; env < ps->NumEnv; env++)
        {
            char thr = 32 - (ps->NumEnv - env);
			
            if (ps->aEnvStartStop[env] > thr)
            {
                ps->aEnvStartStop[env] = thr;
            } else {
                thr = ps->aEnvStartStop[env-1]+1;
                if (ps->aEnvStartStop[env] < thr)
                {
                    ps->aEnvStartStop[env] = thr;
                }
            }
        }
    }
	
    for (env = 0; env < ps->NumEnv; env++)
    {
        if (ps->IidMode == 2 || ps->IidMode == 5)
            map34indexto20(ps->IidIndex[env]);
        if (ps->IccMode == 2 || ps->IccMode == 5)
            map34indexto20(ps->IccIndex[env]);
    }
}

#define SHORT_BITS 14 

static void CalculateEnergy(ps_info *ps, 
						 int *XLeft,
						 int *XHybridleft,
						 int *PEnergy)
{
	int gr, bk, n;
	int border0, bordern;
	int *txLeft;
	
	border0 = ps->aEnvStartStop[0];
    bordern = ps->aEnvStartStop[ps->NumEnv];

	for (gr = 0; gr < ps->NumGroups; gr++)
    {
		int step,sbSize;
		if (gr < ps->NumHybridGroups)
		{
			txLeft = XHybridleft + 64*border0 + 2*ps->GroupBorder[gr];;
			step  = 31;
			sbSize = 1;
		}
		else
		{
			sbSize = ps->GroupBorder[gr+1] - ps->GroupBorder[gr];
			txLeft = XLeft + 128*border0 + ps->GroupBorder[gr]*2;
			step  = 64-sbSize;			
		}

        bk = (~NEGATE_IPD_MASK) & ps->MapGroup2bk[gr];
		
        for (n = border0; n < bordern; n++)
        {			
			int sb, total = 0;
			sb = sbSize;
			do
            {
                int re, im;
				
				re = (txLeft[0] + (1<<(SHORT_BITS-1))) >> SHORT_BITS;
				im = (txLeft[1] + (1<<(SHORT_BITS-1))) >> SHORT_BITS;

                total += re*re + im*im;	

				txLeft += 2;				
            }while(--sb!=0);
			
			*(PEnergy + n*36 + bk) += total; 

			txLeft += step*2;
        }
    }
}

static void CalculateTransient(ps_info *ps, 
							   int *PBuf)
{
	int Nrg, peakDiffNrg;
	int n, bk, tmp0, tmp1;
	int *PrevNrg = ps->PrevNrg;
	int *PeakDecayFast = ps->PeakDecayFast;
	int *PrevPeakNrgDiff = ps->PrevPeakDiff;

	tmp0 = ps->alphaDecay;
	tmp1 = ps->alphaSmooth;

	for (n = ps->aEnvStartStop[0]; n < ps->aEnvStartStop[ps->NumEnv]; n++)
	{
		for (bk = 0; bk < ps->NrParBands; bk++)
		{
			int t1, *tmpValue;

			tmpValue = PBuf + 36*n + bk;

			PeakDecayFast[bk] = MUL_31(PeakDecayFast[bk], tmp0);
			if (PeakDecayFast[bk] < *tmpValue)
				PeakDecayFast[bk] = *tmpValue;

			peakDiffNrg = PrevPeakNrgDiff[bk];
			peakDiffNrg += MUL_31((PeakDecayFast[bk] - *tmpValue - PrevPeakNrgDiff[bk]), tmp1);
			PrevPeakNrgDiff[bk] = peakDiffNrg;

			Nrg = PrevNrg[bk];
			Nrg += MUL_31((*tmpValue - PrevNrg[bk]), tmp1);
			PrevNrg[bk] = Nrg;

			t1 = MUL_30(peakDiffNrg, gamma);

			/* calculate transient */
			if (t1 <= Nrg)
			{
				*tmpValue = gamma2;
			} else {
				*tmpValue = (int)(((VO_S64)Nrg << SHORT_BITS)/t1) ;
			}
		}
	}

}

void deCorrelate(AACDecoder* decoder,
				 ps_info *ps, 
				 int *XLeft, 
				 int *XRight,                           
				 int *XHybridleft, 
				 int *XHybridright)
{
    int gr, n, bk, m;
    int sb, maxsb;
    int *PhiFractSubQmf, *tPhiFractSubQmf;
	int *tPhiFractQmf;
	unsigned char tDelayBufSer[NO_ALLPASS_LINKS] = {0};
    int tDelay = 0,border0,bordern;
    int  *PtmpBuf;   
    int input1,input2;
    int *txLeft, *txRight;
	sbr_info* psi2 = (sbr_info*)decoder->sbr;

    PhiFractSubQmf = (int *)PhiFractSubQmf20[0];
	
    PtmpBuf = psi2->XBuf[0][0];
	input1 = 32*36*sizeof(int);
	decoder->voMemop->Set(VO_INDEX_DEC_AAC, PtmpBuf, 0, input1);
    border0 = 	ps->aEnvStartStop[0];
    bordern =   ps->aEnvStartStop[ps->NumEnv];
	
	/* Calculate energies */
	CalculateEnergy(ps, XLeft, XHybridleft, PtmpBuf);

	/* Transient detector */
	CalculateTransient(ps, PtmpBuf);

	tPhiFractSubQmf = (int *)PhiFractSubQmf20[0];
	tPhiFractQmf    = (int *)PhiFractQmf[0];
		
    /* apply stereo decorrelation filter to the signal */
    for (gr = 0; gr < ps->NumGroups; gr++)
    {
		int condition1,step;

        if (gr < ps->NumHybridGroups)
		{
            maxsb = ps->GroupBorder[gr] + 1;			
			condition1 = 1;
		}
        else
		{
            maxsb = ps->GroupBorder[gr + 1];
			condition1 = 0;
		}

		bk = (~NEGATE_IPD_MASK) & ps->MapGroup2bk[gr];

        for (sb = ps->GroupBorder[gr]; sb < maxsb; sb++)
        {
            int gDecaySlope;
            int gDecaySlopeFilter[NO_ALLPASS_LINKS];
			int *DelayBuffer;
			int *DelayBufferSer;
			unsigned char *delayIdx = 0;
			int condition2 = 0;

			if (condition1 || sb <= ps->DecayCutoff)
            {
                gDecaySlope = VO_INT_MAX;
            } 
			else 
			{
                int decay = ps->DecayCutoff - sb;
                if (decay <= -20)
                {
                    gDecaySlope = 0;
                } else {
                    gDecaySlope = VO_INT_MAX + DECAY_SLOPE * decay;
                }
            }
			
            for (m = 0; m < NO_ALLPASS_LINKS; m++)
            {
                gDecaySlopeFilter[m] = MUL_31(gDecaySlope, filter_a[m]);
			}		
			
            /* set delay indices */
            tDelay = ps->SavedDelay;
            for (n = 0; n < NO_ALLPASS_LINKS; n++)
				tDelayBufSer[n] = ps->DelayBufIndexSer[n];

			if (sb > ps->NrAllpassBands && !condition1)
			{
				delayIdx = &(ps->DelayBufIndex[sb]);
				condition2 = 1;
			}

			if (condition1)
			{
				txLeft = XHybridleft + 64*border0 + 2*sb;
				txRight = XHybridright + 64*border0 + 2*sb;
				step  = 32;
			}
			else
			{
				txLeft = XLeft + 128*border0 + 2*sb;
				txRight = XRight + 128*border0 + 2*sb;
				step  = 64;
			}
			
            for (n = border0; n < bordern; n++)
            {
                int tmp0,tmp1, tmp2, tmp3;
				int R0, R1;				
				
				input1 = txLeft[0];
				input2 = txLeft[1];	
				txLeft+=step*2;				
				
				if (condition2)
				{
					DelayBuffer = ps->DelayBufferQmf[*delayIdx][sb];
					tmp0 = DelayBuffer[0];
					tmp1 = DelayBuffer[1];
					R0 = tmp0;
					R1 = tmp1;
					DelayBuffer[0] = input1;
					DelayBuffer[1] = input2;					
					
				} else {
					int rPhiFract, iPhiFract;
					
					if (condition1)
					{
						/* select data from the hybrid subbands */
						DelayBuffer = &(ps->DelayBufferSubQmf[tDelay][sb][0]);
						rPhiFract = tPhiFractSubQmf[sb*2];
						iPhiFract = tPhiFractSubQmf[sb*2+1];
						
						tmp2 = DelayBuffer[0];
						tmp3 = DelayBuffer[1];						
						
						DelayBuffer[0] = input1;
						DelayBuffer[1] = input2;			  
					} 
					else 
					{
						/* select data from the QMF subbands */
						DelayBuffer = &(ps->DelayBufferQmf[tDelay][sb][0]);
						rPhiFract = tPhiFractQmf[sb*2];
						iPhiFract = tPhiFractQmf[sb*2+1];
						
						tmp2 = DelayBuffer[0];
						tmp3 = DelayBuffer[1];
						
						DelayBuffer[0] = input1;
						DelayBuffer[1] = input2;					
						
					}
					
					tmp0 = (MULHIGH(tmp2, rPhiFract) + MULHIGH(tmp3, iPhiFract))<<1;
					tmp1 = (MULHIGH(tmp3, rPhiFract) - MULHIGH(tmp2, iPhiFract))<<1;

					R0 = tmp0;
					R1 = tmp1;
					for (m = 0; m < NO_ALLPASS_LINKS; m++)
					{
						int rQFract,iQFract;
						int tmp4, tmp5;
						
						if (condition1)
						{
							DelayBufferSer = ps->DelayBufferSubQmfSer[m][tDelayBufSer[m]][sb];
							tmp2 = DelayBufferSer[0];
							tmp3 = DelayBufferSer[1];
							
							rQFract = QFractSubQmf20[sb][m][0];
							iQFract = QFractSubQmf20[sb][m][1];
						} else {
							DelayBufferSer = ps->DelayBufferQmfSer[m][tDelayBufSer[m]][sb];
							tmp2 = DelayBufferSer[0];
							tmp3 = DelayBufferSer[1];
							
							rQFract = QFractQmf[sb][m][0];
							iQFract = QFractQmf[sb][m][1];
						}
						

						tmp0 = (MULHIGH(tmp2, rQFract) + MULHIGH(tmp3, iQFract))<<(1);
						tmp1 = (MULHIGH(tmp3, rQFract) - MULHIGH(tmp2, iQFract))<<(1);

						tmp0 += -MUL_31(gDecaySlopeFilter[m], R0);
						tmp1 += -MUL_31(gDecaySlopeFilter[m], R1);
						
						tmp4 = R0 + MUL_31(gDecaySlopeFilter[m], tmp0);
						tmp5 = R1 + MUL_31(gDecaySlopeFilter[m], tmp1);
						
						DelayBufferSer[0] = tmp4;
						DelayBufferSer[1] = tmp5;  						

						R0 = tmp0;
						R1 = tmp1;
					}
				}                
				
                R0 = MUL_14(PtmpBuf[n*36 + bk], R0);
                R1 = MUL_14(PtmpBuf[n*36 + bk], R1);				
				
				txRight[0] = R0;
				txRight[1] = R1;
				txRight+= step*2;				
				
                if (++tDelay >= 2)
                {
                    tDelay = 0;
                }
				
                if (condition2)
                {
                    if (++(delayIdx[0]) >= ps->DelayD[sb])
                    {
                        delayIdx[0] = 0;
                    }
                }

                if (++tDelayBufSer[0] >= 3)
                {
                    tDelayBufSer[0] = 0;
                }

				if (++tDelayBufSer[1] >= 4)
                {
                    tDelayBufSer[1] = 0;
                }

				if (++tDelayBufSer[2] >= 5)
                {
                    tDelayBufSer[2] = 0;
                }
            }
        }
    }
	
    /* update delay indices */
    ps->SavedDelay = tDelay;
    
	for (m = 0; m < NO_ALLPASS_LINKS; m++)
		ps->DelayBufIndexSer[m] = tDelayBufSer[m];
}

static void InitEnvelope(ps_info *ps, 
						 int env,        
						 int bk,
						 int *hx) 
{
	const int *pScalefactorIid;
	int noIidSteps;
	int IID = ps->IidIndex[env][bk];
	int ICC = ps->IccIndex[env][bk];

	if (ps->IidMode >= 3)
    {
        noIidSteps = 15;
        pScalefactorIid = scalefactorIidFine;
    } else {
        noIidSteps = 7;
        pScalefactorIid = scalefactorIid;
    }

	if (ps->IccMode < 3)
	{
		int c_1b, c_2b;
		int cosa, sina;
		int cosb, sinb;
		int cosacosb, sinasinb;
		int cosasinb, sinacosb;

		c_1b = pScalefactorIid[noIidSteps + IID];
		c_2b = pScalefactorIid[noIidSteps - IID];

		cosa = CosAlphab[ICC];
		sina = SinAlphab[ICC];

		if (ps->IidMode >= 3)
		{
			if (IID < 0)
			{
				cosb =  CosBetabFine[-IID][ICC];
				sinb = -SinBetabFine[-IID][ICC];
			} else {
				cosb = CosBetabFine[IID][ICC];
				sinb = SinBetabFine[IID][ICC];
			}
		} else {
			if (IID < 0)
			{
				cosb =  CosBetab[-IID][ICC];
				sinb = -SinBetab[-IID][ICC];
			} else {
				cosb = CosBetab[IID][ICC];
				sinb = SinBetab[IID][ICC];
			}
		}

		cosacosb = MUL_30(cosa, cosb);
		sinasinb = MUL_30(sina, sinb);
		cosasinb = MUL_30(cosa, sinb);
		sinacosb = MUL_30(sina, cosb);

		hx[0] = MUL_30(c_2b, (cosacosb - sinasinb));
		hx[2] = MUL_30(c_1b, (cosacosb + sinasinb));
		hx[4] = MUL_30(c_2b, (cosasinb + sinacosb));
		hx[6] = MUL_30(c_1b, (cosasinb - sinacosb));
	} else {
		/* type 'B' */
		int sina, cosa;
		int cosg, sing;

		int AIID = ABS(IID);

		if (ps->IidMode >= 3)
		{				
			cosa = SinCosAlphabFineB[noIidSteps + IID][ICC];
			sina = SinCosAlphabFineB[30 - (noIidSteps + IID)][ICC];
			cosg = CosGammabFine[AIID][ICC];
			sing = SinGammabFine[AIID][ICC];
		} else {
			cosa = SinCosAlphabB[noIidSteps + IID][ICC];
			sina = SinCosAlphabB[14 - (noIidSteps + IID)][ICC];
			cosg = CosGammab[AIID][ICC];
			sing = SinGammab[AIID][ICC];
		}

		hx[0] = MUL_30(COEF_SQRT2,  MUL_30(cosa, cosg));
		hx[2] = MUL_30(COEF_SQRT2,  MUL_30(sina, cosg));
		hx[4] = MUL_30(COEF_SQRT2, -MUL_30(cosa, sing));
		hx[6] = MUL_30(COEF_SQRT2,  MUL_30(sina, sing));
	}

	return;
}

void applyRotation(AACDecoder* decoder,
				   ps_info *ps, 
				   int *XLeft, 
				   int *XRight,
                   int *XHybridleft, 
				   int *XHybridright)
{
    int n, gr, env, sb, maxsb;
    int bk = 0;
	int	*tmpBuf;
	int *hx, *Hx, *deltaH;
	int *tTmp, *inData;
    int L, invL;
	int InhybridGroups;
	hyb_info *hyb;
	hyb = ps->pHybrid;
	tmpBuf = hyb->uBuffer;
	
	hx = tmpBuf;
	Hx = tmpBuf + 8;
	deltaH = tmpBuf + 16;
	tTmp = tmpBuf + 20;
	inData = tmpBuf + 24;	

    for (gr = 0; gr < ps->NumGroups; gr++)
    {
        bk = (~NEGATE_IPD_MASK) & ps->MapGroup2bk[gr];

		InhybridGroups = 0;
		if(gr < ps->NumHybridGroups)
			InhybridGroups = 1;
		
        maxsb = InhybridGroups ? ps->GroupBorder[gr] + 1 : ps->GroupBorder[gr + 1];
		
        for (env = 0; env < ps->NumEnv; env++)
        {
            InitEnvelope(ps, env, bk, hx);		

            L = (int)(ps->aEnvStartStop[env + 1] - ps->aEnvStartStop[env]);
			invL = voSignedDivide(decoder, (int)(1 << 30), L);

			deltaH[0] = MUL_30((hx[0] - ps->h11Prev[gr]), invL);
			deltaH[2] = MUL_30((hx[2] - ps->h12Prev[gr]), invL);
			deltaH[4] = MUL_30((hx[4] - ps->h21Prev[gr]), invL);
			deltaH[6] = MUL_30((hx[6] - ps->h22Prev[gr]), invL);
			
            Hx[0] = ps->h11Prev[gr];
            Hx[2] = ps->h12Prev[gr];
            Hx[4] = ps->h21Prev[gr];
            Hx[6] = ps->h22Prev[gr];
			
            ps->h11Prev[gr] = hx[0];
            ps->h12Prev[gr] = hx[2];
            ps->h21Prev[gr] = hx[4];
            ps->h22Prev[gr] = hx[6];
			
            for (n = ps->aEnvStartStop[env]; n < ps->aEnvStartStop[env + 1]; n++)
            {
                int *tXLeft, *tXRight, *tXHybridLeft, *tXHybridRight;

				Hx[0] += deltaH[0];
                Hx[2] += deltaH[2];
                Hx[4] += deltaH[4];
                Hx[6] += deltaH[6];

                tXHybridLeft = XHybridleft + n*64 + ps->GroupBorder[gr]*2;
				tXHybridRight = XHybridright + n*64 + ps->GroupBorder[gr]*2;
				tXLeft = XLeft + 128 * n + ps->GroupBorder[gr]*2;
				tXRight = XRight + 128 * n + ps->GroupBorder[gr]*2;
				for (sb = ps->GroupBorder[gr]; sb < maxsb; sb++)
                {
                    if (InhybridGroups)
                    {
                        inData[0] =  tXHybridLeft[0];
                        inData[1] =  tXHybridLeft[1];
                        inData[2] =  tXHybridRight[0];
                        inData[3] =  tXHybridRight[1];
                    } else {
                        inData[0] =  tXLeft[0];
                        inData[1] =  tXLeft[1];
                        inData[2] =  tXRight[0];
                        inData[3] =  tXRight[1];
                    }
					
                    tTmp[0] = MUL_30(Hx[0], inData[0]) + MUL_30(Hx[4], inData[2]);
                    tTmp[1] = MUL_30(Hx[0], inData[1]) + MUL_30(Hx[4], inData[3]);
                    tTmp[2] = MUL_30(Hx[2], inData[0]) + MUL_30(Hx[6], inData[2]);
                    tTmp[3] = MUL_30(Hx[2], inData[1]) + MUL_30(Hx[6], inData[3]);
					
                    if (InhybridGroups)
                    {
                        tXHybridLeft[0]  = tTmp[0];
                        tXHybridLeft[1]  = tTmp[1];
                        tXHybridRight[0] = tTmp[2];
                        tXHybridRight[1] = tTmp[3];
                    } else {
                        tXLeft[0]  = tTmp[0];
                        tXLeft[1]  = tTmp[1];
                        tXRight[0] = tTmp[2];
                        tXRight[1] = tTmp[3];
                    }

					tXLeft += 2;
					tXRight += 2;
					tXHybridLeft  += 2;
					tXHybridRight += 2;
                }
            }			
        }
    }
}

void ps_free(VO_MEM_OPERATOR *voMemop, 
			 ps_info *ps)
{
	SafeAlignedFree(ps->pHybrid);
	
    SafeAlignedFree(ps);
}

ps_info *ps_init(VO_MEM_OPERATOR *voMemop, 
				 unsigned char sr_index)
{
    int i;
	
    ps_info *ps = (ps_info*)voAACDecAlignedMalloc(voMemop, sizeof(ps_info));
	
    ps->pHybrid = hybrid_init(voMemop);
	
    ps->bAvailblePsData = 0;
	
    ps->SavedDelay = 0;
	
    for (i = 0; i < 64; i++)
    {
        ps->DelayBufIndex[i] = 0;
    }
	
    for (i = 0; i < NO_ALLPASS_LINKS; i++)
    {
        ps->DelayBufIndexSer[i] = 0;
    }
	
    ps->NrAllpassBands = 22;
    ps->alphaDecay = 0x6209f096;
    ps->alphaSmooth = 0x20000000;
	
    for (i = 0; i < 35; i++)
    {
        ps->DelayD[i] = 14;
    }
    for (i = 35; i < 64; i++)
    {
        ps->DelayD[i] = 1;
    }
	
    for (i = 0; i < 50; i++)
    {
        ps->h11Prev[i] = 1;
        ps->h12Prev[i] = 1;
        ps->h21Prev[i] = 1;
        ps->h22Prev[i] = 1;
    }
	
	ps->GroupBorder = (unsigned char*)groupBorder20;
	ps->MapGroup2bk = (unsigned short*)mapGroupbk20;
	ps->NumGroups = 10+12;
	ps->NumHybridGroups = 10;
	ps->NrParBands = 20;
	ps->DecayCutoff = 3;

    return ps;
}

int voAACDecodePS(AACDecoder* decoder,
			      sbr_info* psi,
			      SBRGrid *sbrGrid,
			      SBRFreq *sbrFreq)
{
	
	int k,l;
	ps_info *inps = psi->ps;
	VO_MEM_OPERATOR *voMemop;
	int *LBuf, *XBuf;

	voMemop = decoder->voMemop;

	LBuf = inps->LBuf[0][0];
	XBuf = psi->XBuf[2][0];

	voMemop->Copy(VO_INDEX_DEC_AAC, LBuf, XBuf, 64*32*2*sizeof(int));

    for (l = 32; l < 38; l++)
    {
        LBuf = inps->LBuf[l][0];
		XBuf = psi->XBuf[2+l][0];
		for (k = 0; k < 5; k++)
        {
            *LBuf++ = *XBuf++;
			*LBuf++ = *XBuf++;
        }
    }

	ps_decode(decoder, inps);				

	return 0;
}

/*
* parametric stereo decoder process
* reference ISO_IEC_14496-3-2005.pdf(Figure 8.19)
*/
unsigned char ps_decode(AACDecoder* decoder,
						ps_info *ps)
{	

    VO_MEM_OPERATOR *voMemop;
	int *LBuf;
	int *RBuf;
	int *LHybrid;
	int *RHybrid;

	voMemop = decoder->voMemop;

	LBuf = ps->LBuf[0][0];
	RBuf = ps->RBuf[0][0];

	LHybrid = ps->LHybrid[0][0];
	RHybrid = ps->RHybrid[0][0];

    psDataInit(ps);	

    /*  Analysis the hybrid bank */
    HybridAnalysis(voMemop, (hyb_info*)ps->pHybrid, LBuf, LHybrid);

    /* decorrelate mono signal */
    deCorrelate(decoder,ps, LBuf, RBuf, LHybrid, RHybrid);
	
    /* apply mixing and phase parameters */
    applyRotation(decoder, ps, LBuf, RBuf, LHybrid, RHybrid);

    /* hybrid synthesis L */
    HybridSynthesis(ps->pHybrid, LBuf, LHybrid);
	 
	/* hybrid synthesis R */
    HybridSynthesis(ps->pHybrid, RBuf, RHybrid);
	
    return 0;
}

#endif   //PS_DEC

