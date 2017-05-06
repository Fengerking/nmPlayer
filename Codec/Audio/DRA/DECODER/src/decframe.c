#include	"config.h"
#include	"global.h"
#include	"frame.h"


int MaxActCB(FrameCHInfo *chInfo, Sfbwidth*	 sfbw)
{
	int nCluster, nIndex, nMaxLen;
	int nMaxBand, nMaxBin, nCb;
	FrameDataInfo *frame;
	const short *pnCBEdge;

	frame = &chInfo->frame;
	
	if(frame->nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT){
		pnCBEdge = sfbw->l;
		nMaxLen = 1024;
	}
	else
	{
		pnCBEdge = sfbw->s;
		nMaxLen = 128;
	}

	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++)
	{
		nMaxBand = frame->anHSNumBands[nCluster];
		if(nMaxBand == 0)
		{
			chInfo->anMaxActcb[nCluster] = 0;
			continue;
		}
		
		if(frame->anNumBlocksPerFrmPerCluster[nCluster] == 0)
			return -1;

		nMaxBin = frame->mnHSBandEdge[nCluster][nMaxBand-1] * 4;
		nMaxBin  += frame->anNumBlocksPerFrmPerCluster[nCluster] - 1;
		nMaxBin /= frame->anNumBlocksPerFrmPerCluster[nCluster];

		if(nMaxBin > nMaxLen)
			nMaxBin = nMaxLen;
		
		nCb = 0;

		nIndex = pnCBEdge[nCb];

		while(nIndex < nMaxBin) {
			nCb++;
			nIndex += pnCBEdge[nCb];
		}

		chInfo->anMaxActcb[nCluster] = nCb + 1;
	}

	return 0;	
}

int  InterToNatural(FrameCHInfo *chInfo, int *buf)
{
	int nCluster, nBlock;
	int n ,p, q;
	int nBin0, nNumBlocksPerFrm;
	int *afBinInter, *afBinNatural;
	FrameDataInfo *frame;

	frame = &chInfo->frame;

	if(frame->nNumCluster == 1) 
		return 0;
	
	afBinInter = chInfo->anQIndex;
	afBinNatural = buf;

	p = 0;
	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++)
	{
		nBin0 = chInfo->anClusterBin0[nCluster];
		nNumBlocksPerFrm = frame->anNumBlocksPerFrmPerCluster[nCluster];
		
		for(nBlock = 0; nBlock < nNumBlocksPerFrm; nBlock++)
		{			
			q = nBin0;
			for(n = 0; n < 128; n++)
			{
				afBinNatural[p] = afBinInter[q];
				q += nNumBlocksPerFrm;
				p++;
			}
			nBin0++;
		}
	}
	
	for(n = 0; n < MAX_FRAMELENGTH; n+= 4)
	{
		chInfo->anQIndex[n  ] = buf[n  ];
		chInfo->anQIndex[n+1] = buf[n+1];
		chInfo->anQIndex[n+2] = buf[n+2];
		chInfo->anQIndex[n+3] = buf[n+3];
	}

	return 0;	
}

int  DeQuant(FrameCHInfo *chInfo, short *pnCBEdge)
{
	int nCluster, nBand;
	int nStart, nEnd, nQStepSelect;
	int nStepSize;
	int nBin0, nBin, nNumBlocksPerFrm;
	FrameDataInfo *frame;
	int	*afBinReconst;

	frame = &chInfo->frame;
	afBinReconst = chInfo->anQIndex;

	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++)
	{
		nBin0 = chInfo->anClusterBin0[nCluster];
		nNumBlocksPerFrm = frame->anNumBlocksPerFrmPerCluster[nCluster];

		nStart = nBin0;
		nEnd = nBin0;
		for(nBand = 0; nBand < chInfo->anMaxActcb[nCluster]; nBand++)
		{
			nEnd = nEnd + nNumBlocksPerFrm * pnCBEdge[nBand];
			nQStepSelect = frame->mnQStepIndex[nCluster][nBand];
			nStepSize = nQStepSize[nQStepSelect];

			for(nBin = nStart; nBin < nEnd; nBin++)
			{
				afBinReconst[nBin] = afBinReconst[nBin] * nStepSize; 
			}

			nStart = nEnd;
		}	
	}

	return 0;	
}

int  MSdecode(FrameCHInfo *chInfo, FrameCHInfo *chInfosrc, short *pnCBEdge, int nJicCB)
{
	int nCluster, nBand, nMaxCb;
	int nStart, nEnd;
	int rSum, rDff;
	int nBin0, nBin, nNumBlocksPerFrm;
	FrameDataInfo *frame;
	FrameSumDff *fSumDff;
	int	*afBinReconst, *afBinReconst1;

	frame = &chInfo->frame;
	afBinReconst = chInfo->anQIndex;
	afBinReconst1 = chInfosrc->anQIndex;
	fSumDff = chInfo->fSumDff;

	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++)
	{
		if(fSumDff->anSumDffAllOff[nCluster])
			continue;
		
		nBin0 = chInfo->anClusterBin0[nCluster];
		nNumBlocksPerFrm = frame->anNumBlocksPerFrmPerCluster[nCluster];
		nMaxCb = MAX(chInfo->anMaxActcb[nCluster], chInfosrc->anMaxActcb[nCluster]);
		
		if(nJicCB > 0)
		{
			nMaxCb = MIN(nMaxCb, nJicCB);
		}

		nStart = nBin0;
		nEnd = nBin0;
		for(nBand = 0; nBand < nMaxCb; nBand++)
		{
			nStart = nEnd;
			nEnd = nEnd + nNumBlocksPerFrm * pnCBEdge[nBand];
			
			if(fSumDff->mnSumDffOn[nCluster][nBand] == 0)				
				continue;
			
			for(nBin = nStart; nBin < nEnd; nBin++)
			{
				rSum = afBinReconst1[nBin] + afBinReconst[nBin];
				rDff = afBinReconst1[nBin] - afBinReconst[nBin];

				afBinReconst1[nBin] = rSum;
				afBinReconst[nBin] = rDff;
			}
		}
	}

	return 0;		
}

int  ISdeocde(FrameCHInfo *chInfo, FrameCHInfo *chInfosrc, short *pnCBEdge, int nJicCB)
{
	int nCluster, nBand, ntCBEdge;
	int nStart, nEnd, nQStepSelect;
	int nStepSize;
	int nBin0, nBin, nNumBlocksPerFrm;
	FrameDataInfo *frame;
	FrameJicScale *fJicScale;
	int	*afBinReconst,*afBinReconst1;

	frame = &chInfo->frame;
	afBinReconst = chInfo->anQIndex;
	afBinReconst1 = chInfosrc->anQIndex;
	fJicScale = chInfo->fJicScale;

	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++)
	{
		nBin0 = chInfo->anClusterBin0[nCluster];
		nNumBlocksPerFrm = frame->anNumBlocksPerFrmPerCluster[nCluster];

		if(nJicCB == 0)
			nStart = nBin0;
		else
		{
			ntCBEdge = 0;
			nBand= 0;
			while(nBand < nJicCB)
			{
				ntCBEdge += pnCBEdge[nBand];
				nBand++;
			}
			nStart = nBin0 + nNumBlocksPerFrm * ntCBEdge;
		}

		nEnd = nStart;
		for(nBand = nJicCB; nBand < chInfosrc->anMaxActcb[nCluster]; nBand++)
		{
			nEnd = nEnd + nNumBlocksPerFrm * pnCBEdge[nBand];
			nQStepSelect = fJicScale->mnQStepIndex[nCluster][nBand];
			nStepSize = nJicStepSize[nQStepSelect];

			for(nBin = nStart; nBin < nEnd; nBin++)
			{
				afBinReconst[nBin] = MUL_18(afBinReconst1[nBin], nStepSize); 
			}
			
			nStart = nEnd;
		}
	}

	return 0;		
}

int ReConstructWinType(FrameCHInfo *chInfo, int nNumBlockPerFrm)
{
	short nBlock, nCluster, nNumCluster;
	short nWinTypeCurrent, nWinTypeLast;
	FrameDataInfo *frame;
	short *pnWinTypeShort;

	frame = &chInfo->frame;
	nWinTypeCurrent = chInfo->frame.nWinTypeCurrent;
		
	if(nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT)
	{
		chInfo->preWinType = nWinTypeCurrent;
		return 0;
	}

	nNumCluster = chInfo->frame.nNumCluster;
	pnWinTypeShort = chInfo->pnWinTypeShort;
	nWinTypeLast = chInfo->preWinType;

	if(nWinTypeCurrent == WIN_SHORT_SHORT2SHORT || 
		nWinTypeCurrent == WIN_SHORT_SHORT2BRIEF)
	{
		pnWinTypeShort[0] = WIN_SHORT_SHORT2SHORT;

		switch(nWinTypeLast)
		{
		case WIN_SHORT_BRIEF2BRIEF:
			pnWinTypeShort[0] = WIN_SHORT_BRIEF2SHORT;
			break;
		case WIN_LONG_LONG2SHORT:
		case WIN_LONG_SHORT2SHORT:
		case WIN_LONG_BRIEF2SHORT:
		case WIN_SHORT_SHORT2SHORT:
		case WIN_SHORT_BRIEF2SHORT:
			break;
		default:
			return -1;
		}
	}
	else
	{
		pnWinTypeShort[0] = WIN_SHORT_BRIEF2BRIEF;
		switch(nWinTypeLast)
		{
		case WIN_SHORT_BRIEF2BRIEF:
		case WIN_SHORT_SHORT2BRIEF:
		case WIN_LONG_LONG2BRIEF:
		case WIN_LONG_BRIEF2BRIEF:
		case WIN_LONG_SHORT2BRIEF:
			break;
		default:
			return -1;
		}
	}

	for(nBlock = 1; nBlock < nNumBlockPerFrm; nBlock++)
	{
		pnWinTypeShort[nBlock] = WIN_SHORT_SHORT2SHORT;
	}
	
	nBlock = 0;	
	for(nCluster = 0; nCluster < nNumCluster - 1; nCluster++)
	{
		nBlock += frame->anNumBlocksPerFrmPerCluster[nCluster];
		pnWinTypeShort[nBlock] = WIN_SHORT_BRIEF2BRIEF;
	}

	if(pnWinTypeShort[0] == WIN_SHORT_BRIEF2BRIEF)
	{
		if( pnWinTypeShort[1] == WIN_SHORT_SHORT2SHORT)
		{
			pnWinTypeShort[1] = WIN_SHORT_BRIEF2SHORT;
		}
	}

	for(nBlock = 1; nBlock < nNumBlockPerFrm - 1; nBlock++)
	{
		if( pnWinTypeShort[nBlock] == WIN_SHORT_BRIEF2BRIEF)
		{
			if(pnWinTypeShort[nBlock-1] == WIN_SHORT_SHORT2SHORT)
			{
				pnWinTypeShort[nBlock - 1] = WIN_SHORT_SHORT2BRIEF;
			}

			if(pnWinTypeShort[nBlock-1] == WIN_SHORT_BRIEF2SHORT)
			{
				pnWinTypeShort[nBlock - 1] = WIN_SHORT_BRIEF2BRIEF;
			}

			if(pnWinTypeShort[nBlock + 1] == WIN_SHORT_SHORT2SHORT)
			{
				pnWinTypeShort[nBlock + 1] = WIN_SHORT_BRIEF2SHORT;
			}
		}
	}

	switch(pnWinTypeShort[nBlock])
	{
	case WIN_SHORT_BRIEF2BRIEF:
		if(pnWinTypeShort[nBlock - 1] == WIN_SHORT_SHORT2SHORT)
		{
			pnWinTypeShort[nBlock - 1] = WIN_SHORT_SHORT2BRIEF;
		}

		if(pnWinTypeShort[nBlock - 1] == WIN_SHORT_BRIEF2SHORT)
		{
			pnWinTypeShort[nBlock - 1] = WIN_SHORT_BRIEF2BRIEF;
		}
		break;
	case WIN_SHORT_SHORT2SHORT:
		if(nWinTypeCurrent == WIN_SHORT_SHORT2BRIEF || 
			nWinTypeCurrent == WIN_SHORT_BRIEF2BRIEF)
		{
			pnWinTypeShort[nBlock] = WIN_SHORT_SHORT2BRIEF;
		}
		break;
	case WIN_SHORT_BRIEF2SHORT:
		if(nWinTypeCurrent == WIN_SHORT_SHORT2BRIEF ||
			nWinTypeCurrent == WIN_SHORT_BRIEF2BRIEF)
		{
			pnWinTypeShort[nBlock] = WIN_SHORT_BRIEF2BRIEF;
		}
		break;
	default:
		return -1;
	}

	chInfo->preWinType = pnWinTypeShort[nBlock];

	return 0;
}

int	 DecoderFrame(DraDecInfo* decoder, short* outbuf)
{
	FrameHeader		*header = NULL;
	FrameCHInfo		*chInfo = NULL;
	FrameCHInfo		*chInfop = NULL;
	FrameCHInfo		*chInfo0 = NULL;
	short			*pnCBEdge = 0;

	int				len, nch;

	header = decoder->header;
	
	len = UnpackFrameData(decoder);	
	if(len < 0) return -1;
	
	chInfo0 = decoder->chInfo[0];

	for(nch = 0; nch < header->nNumNormalCh; nch++) {
		chInfo = decoder->chInfo[nch];

		if(chInfo->frame.nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT)
		{
			pnCBEdge = decoder->sfbw->l;
		}
		else
		{
			pnCBEdge = decoder->sfbw->s;
		}
		
		len  = DeQuant(chInfo, pnCBEdge);
		if(len < 0) return len;

		if(header->bUseJIC && nch > 0)
		{
			len = ISdeocde(chInfo, chInfo0, pnCBEdge, header->nJicCb);
			if(len < 0) return len;
		}

		if(header->bUseSumdiff && (nch & 1))
		{
			len  = MSdecode(chInfo, chInfop, pnCBEdge, header->nJicCb);
		}

		chInfop = chInfo;
	}
	
	for(nch = 0; nch < header->nNumNormalCh; nch++) {
		chInfo = decoder->chInfo[nch];
		
		len = InterToNatural(chInfo, decoder->afBinNatural);
		if(len < 0) return len;

		len = ReConstructWinType(chInfo, header->nNumBlocksPerFrm);
		if(len < 0) return len;
		
		len = FilterBand(decoder, outbuf, nch);
		if(len < 0) return len;
	}

	for( ; nch < header->nNumNormalCh + header->nNumLfeCh; nch++) {
		chInfo = decoder->chInfo[nch];

		if(chInfo->frame.nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT)
		{
			pnCBEdge = decoder->sfbw->l;
		}
		else
		{
			pnCBEdge = decoder->sfbw->s;
		}
		
		len  = DeQuant(chInfo, pnCBEdge);
		if(len < 0) return len;

		len = InterToNatural(chInfo, decoder->afBinNatural);
		if(len < 0) return len;

		len = ReConstructWinType(chInfo, header->nNumBlocksPerFrm);
		if(len < 0) return len;
		
		len = FilterBand(decoder, outbuf, nch);
		if(len < 0) return len;
	}	

	return 0;	
}