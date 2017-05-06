#include "config.h"
#include "global.h"
#include "bit.h"
#include "frame.h"
#include "huffman.h"

int	 HeaderSync(unsigned char *buf, int nbytes)
{
	int i;
	
	if(nbytes < BUFFER_GUARD)
		return -1;

	for (i = 0; i < nbytes - BUFFER_GUARD; i++) {
		if ( buf[i+0] == 0x7f && buf[i+1] == 0xff)
			return i;
	}
	
	return -1;
}

static
int DecodeHeaderinfo(FrameHeader* header, Bitstream*	bitptr)
{
	int nHeaderType;
	
	SkipBits(bitptr, 16);

	header->nFrmHeaderType = nHeaderType = GetBits(bitptr, 1);
	if(!nHeaderType) 
	{
		header->nNumWord = GetBits(bitptr, 10) << 2;
	}
	else
	{
		header->nNumWord = GetBits(bitptr, 13) << 2;
	}

	header->nNumBlocksPerFrm = 1 << GetBits(bitptr, 2);
	header->nSamplerateRateIndex = GetBits(bitptr, 4);
	if(header->nSamplerateRateIndex > 12)
		return -1;

	if(!nHeaderType)
	{
		header->nNumNormalCh = GetBits(bitptr, 3) + 1;
		header->nNumLfeCh = GetBits(bitptr, 1);
	}
	else
	{
		header->nNumNormalCh = GetBits(bitptr, 6) + 1;
		header->nNumLfeCh = GetBits(bitptr, 2);
	}

	if(header->nNumNormalCh < 1)
		return -1;
	
	header->bAuxChCfg = GetBits(bitptr, 1);

	if(!nHeaderType)
	{
		if(header->nNumNormalCh > 1)
		{
			header->bUseSumdiff = GetBits(bitptr, 1);
			header->bUseJIC = GetBits(bitptr, 1);			
		}
		else
		{
			header->bUseSumdiff = 0;
			header->bUseJIC = 0;
		}

		if(header->bUseJIC)
		{
			header->nJicCb = GetBits(bitptr, 5) + 1;
		}
		else
		{
			header->nJicCb = 0;
		}
	}
	else
	{
		header->bUseSumdiff = 0;
		header->bUseJIC = 0;
		header->nJicCb = 0;
	}
	
	return 0;	
}

int HeaderDecode(FrameHeader *header, FrameStream *stream)
{
	unsigned char *ptr, *end;
	Bitstream*	bitptr;
	int len, hlen; 

	bitptr = &stream->bitptr;

SHEADER:	
	ptr = stream->this_frame;
	end = stream->this_frame + stream->length;
	hlen = 0;

	do{
		if(hlen < 0) ptr++;
		len  = 	HeaderSync(ptr, end - ptr);
		if(len < 0)
		{
			if(end - ptr >= BUFFER_GUARD)
			{
				stream->this_frame = end - BUFFER_GUARD;
				stream->length = BUFFER_GUARD;
			}
			return -2;
		}
		
		ptr += len;
		stream->this_frame = ptr;
		stream->length = end - ptr;
		
		InitBits(bitptr, ptr, end - ptr);
				
		hlen = DecodeHeaderinfo(header, bitptr);
	}while(hlen < 0);	
	
	if(end - stream->this_frame < header->nNumWord)
		return -1;

	stream->next_frame = stream->this_frame + header->nNumWord;	
	if(end - stream->this_frame -  header->nNumWord > 2)
	{
		if(!(stream->next_frame[0] == 0x7f && stream->next_frame[1] & 0xff))
		{
			stream->this_frame += 1;
			stream->length -= 1;
			goto SHEADER;
		}
	}

	return 0;
}


int	 UnpackWinSequence(DraDecInfo* decoder, int nch)
{
	int bUseJIC, bUseSumDiff;
	int nBlockPerFrm, nCluster;
	FrameCHInfo* pChInfo;
	FrameDataInfo *frame;
	FrameDataInfo *frame0;
	Bitstream*	bitptr;

	pChInfo = decoder->chInfo[nch];
	bUseJIC = decoder->header->bUseJIC;
	bUseSumDiff = decoder->header->bUseSumdiff;
	nBlockPerFrm = decoder->header->nNumBlocksPerFrm;
	frame = &pChInfo->frame;
	bitptr = &decoder->stream->bitptr;

	if(nch == 0 || ((!bUseJIC) && (!bUseSumDiff)))
	{
		frame->nWinTypeCurrent = GetBits(bitptr, 4);
		if(frame->nWinTypeCurrent > WIN_LONG_BRIEF2SHORT)
		{
			frame->nNumCluster = GetBits(bitptr, 2) + 1;
			if(frame->nNumCluster >= 2)
			{
				int nLast, k;

				nLast = 0;

				for(nCluster = 0; nCluster < frame->nNumCluster - 1; nCluster++)
				{
					k = HuffDec(bitptr, HuffDec1_7x1, NCLUSTER_HUF) + 1;
					frame->anNumBlocksPerFrmPerCluster[nCluster] = k;
					nLast += k;
				}

				frame->anNumBlocksPerFrmPerCluster[nCluster] = 
						nBlockPerFrm - nLast;
			}
			else
			{
				frame->anNumBlocksPerFrmPerCluster[0] = nBlockPerFrm;
			}

		}
		else
		{
			frame->nNumCluster = 1;
			frame->anNumBlocksPerFrmPerCluster[0] = 1;
		}
	}
	else
	{
		frame0 = &(decoder->chInfo[0]->frame);

		frame->nWinTypeCurrent = frame0->nWinTypeCurrent;
		frame->nNumCluster = frame0->nNumCluster;
		
		for(nCluster = 0; nCluster < frame0->nNumCluster; nCluster++)
		{
			frame->anNumBlocksPerFrmPerCluster[nCluster] = 
				frame0->anNumBlocksPerFrmPerCluster[nCluster];
		}		
	}

	pChInfo->anClusterBin0[0] = 0;
	for(nCluster = 1; nCluster < frame->nNumCluster; nCluster++)
	{
		pChInfo->anClusterBin0[nCluster] = pChInfo->anClusterBin0[nCluster - 1] + 
			frame->anNumBlocksPerFrmPerCluster[nCluster-1] * 128;
	}

	return 0;

}

int	 UnpackCodeBooks(FrameCHInfo* pChInfo, Bitstream *bitptr)
{
	int nCluster, nLast;
	int nBand, k, Booklen;
	const int *hufBook;
	FrameDataInfo *frame;

	frame = &pChInfo->frame;
	
	if(frame->nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT)
	{
		hufBook = HuffDec2_64x1;
		Booklen = 64;
	}
	else
	{
		hufBook = HuffDec3_32x1;
		Booklen = 32;
	}
	
	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++)
	{
		frame->anHSNumBands[nCluster] = GetBits(bitptr, 5);
		nLast = 0;

		for(nBand = 0; nBand < frame->anHSNumBands[nCluster]; nBand++)
		{
			k = HuffDecRecurive(bitptr, hufBook, Booklen) + nLast + 1;
			frame->mnHSBandEdge[nCluster][nBand] = k;
			nLast = k;
		}
	}


	if(frame->nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT)
	{
		hufBook = HuffDec4_18x1;
		Booklen = 18;
	}
	else
	{
		hufBook = HuffDec5_18x1;
		Booklen = 18;
	}
	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++)
	{
		if(frame->anHSNumBands[nCluster] > 0)
		{
			nLast = GetBits(bitptr, 4);
			if(nLast > 9) 
				nLast = 9;
			frame->mnHS[nCluster][0] = nLast;

			for(nBand = 1; nBand < frame->anHSNumBands[nCluster]; nBand++)
			{
				k = HuffDec(bitptr, hufBook, Booklen);

				if(k > 8)
				{
					k -= 8;
				}
				else
				{
					k -= 9;
				}

				k += nLast;

				if( k > 9 )
					k = 9;
				else if(k < 0)
					k = 0;					

				frame->mnHS[nCluster][nBand] = k;
				nLast = k;
			}
		}
	}

	return 0;
}

int	 UnpackQIndex(FrameCHInfo* pChInfo, Bitstream *bitptr)
{
	int nCluster, nCtr, k;
	int nBand, Booklen, nQuBooklen, nBin, nQIndex;
	int nStart, nEnd, nHSelect;
	int nQuotientWidth, nMaxIndex;
	const int *hufBook, *nQuhufBook;
	const QIndexHuf *pQindexBook;
	FrameDataInfo *frame;

	frame = &pChInfo->frame;
	nQuotientWidth = 0;
	
	if(frame->nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT)
	{
		nQuhufBook = HuffDec8_16x1;
		nQuBooklen = 16;
	}
	else
	{
		nQuhufBook = HuffDec9_16x1;
		nQuBooklen = 16;
	}
	
	for(k = 0; k < MAX_FRAMELENGTH; k += 4)
	{
		pChInfo->anQIndex[k  ] = 0;
		pChInfo->anQIndex[k+1] = 0;
		pChInfo->anQIndex[k+2] = 0;
		pChInfo->anQIndex[k+3] = 0;
	}	

	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++){
		nStart = pChInfo->anClusterBin0[nCluster];
		for(nBand = 0; nBand < frame->anHSNumBands[nCluster]; nBand++)
		{
			nEnd = pChInfo->anClusterBin0[nCluster] + 
				frame->mnHSBandEdge[nCluster][nBand] * 4;
			nHSelect = frame->mnHS[nCluster][nBand];
			
			if(nHSelect != 0)
			{
				pQindexBook = &nQindexHufTab[nHSelect];

				if(nHSelect == 9)
				{
					nMaxIndex = pQindexBook->nNumDimhuf - 1;
					nCtr = 0;

					if(frame->nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT)
					{
						hufBook = pQindexBook->nHufCode_l;
						Booklen = pQindexBook->nNumhufCodes;
					}
					else
					{
						hufBook = pQindexBook->nHufCode_s;
						Booklen = pQindexBook->nNumhufCodes;
					}

					for(nBin = nStart; nBin < nEnd; nBin++){
						nQIndex = HuffDec(bitptr, hufBook, Booklen);

						if(nQIndex == nMaxIndex) {
							nCtr++;
						}

						pChInfo->anQIndex[nBin] = nQIndex;
					}

					if(nCtr > 0)
					{
						nQuotientWidth = HuffDiff(bitptr, nQuhufBook, nQuotientWidth, nQuBooklen) + 1;

						for(nBin = nStart; nBin < nEnd; nBin++)
						{
							nQIndex = pChInfo->anQIndex[nBin];
							
							if(nQIndex == nMaxIndex)
							{
								nQIndex *= GetBits(bitptr, nQuotientWidth) + 1;
								nQIndex += HuffDec(bitptr, hufBook, Booklen);

								pChInfo->anQIndex[nBin] = nQIndex;
							}
						}
					}
				}
				else
				{
					int nDim = pQindexBook->nDim;

					if(frame->nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT)
					{
						hufBook = pQindexBook->nHufCode_l;
						Booklen = pQindexBook->nNumhufCodes;
					}
					else
					{
						hufBook = pQindexBook->nHufCode_s;
						Booklen = pQindexBook->nNumhufCodes;
					}

					if(nDim > 1)
					{
						int nNumCodes = pQindexBook->nNumDimhuf;

						for(nBin = nStart; nBin < nEnd; nBin += nDim)
						{
							nQIndex = HuffDec(bitptr, hufBook, Booklen);

							for(k = 0; k < nDim; k++){
								pChInfo->anQIndex[nBin + k] = nQIndex % nNumCodes;
								nQIndex /= nNumCodes;
							}
						}
					}
					else
					{
						for(nBin = nStart; nBin < nEnd; nBin++)
						{
							pChInfo->anQIndex[nBin] = HuffDec(bitptr, hufBook, Booklen);
						}
					}
				}
				
				if(nHSelect < 9){
					nMaxIndex = pQindexBook->nNumDimhuf >> 1;

					for(nBin = nStart; nBin < nEnd; nBin++){
						pChInfo->anQIndex[nBin] -= nMaxIndex;
					}
				}
				else
				{
					for(nBin = nStart; nBin < nEnd; nBin++) {

						nQIndex = pChInfo->anQIndex[nBin];

						if(nQIndex != 0)
						{
							int nSign = GetBits(bitptr, 1);
							if(nSign == 0)
							{
								nQIndex = -nQIndex;
							}
						}

						pChInfo->anQIndex[nBin] = nQIndex;						
					}
				}
			}

			nStart = nEnd;
		}
	}


	return 0;
}

int  UnpackStepIndex(FrameCHInfo* pChInfo, Bitstream *bitptr)
{
	int nCluster, nBand;
	int nQStep, Booklen;
	const int *hufBook;
	FrameDataInfo *frame;

	frame = &pChInfo->frame;

	if(frame->nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT)
	{
		hufBook = HuffDec6_116x1;
		Booklen = 116;
	}
	else
	{
		hufBook = HuffDec7_116x1;
		Booklen = 116;
	}

	nQStep = 0;
	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++){
		for(nBand = 0; nBand < pChInfo->anMaxActcb[nCluster]; nBand++){
			nQStep = HuffDiff(bitptr, hufBook, nQStep, Booklen);
			frame->mnQStepIndex[nCluster][nBand] = nQStep;
		}
	}

	return 0;
}

int  UnpackSumDff(FrameCHInfo* pChInfo, int nJicCb, short *pnMaxCb, Bitstream *bitptr)
{
	int nCluster, nMaxCb, nBand;
	FrameDataInfo *frame;
	FrameSumDff	  *fSumDff;

	frame = &pChInfo->frame;
	fSumDff = pChInfo->fSumDff;	

	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++){
		nMaxCb = MAX(pnMaxCb[nCluster], pChInfo->anMaxActcb[nCluster]);

		if(nJicCb > 0)
		{
			nMaxCb = MIN(nJicCb, nMaxCb);
		}

		if(nMaxCb > 0)
		{
			fSumDff->anSumDffAllOff[nCluster] = GetBits(bitptr, 1);

			if(fSumDff->anSumDffAllOff[nCluster] == 0)
			{
				for(nBand = 0; nBand < nMaxCb; nBand++) {
					fSumDff->mnSumDffOn[nCluster][nBand] = GetBits(bitptr, 1);
				}
			}
			else
			{
				for(nBand = 0; nBand < nMaxCb; nBand++) {
					fSumDff->mnSumDffOn[nCluster][nBand] = 0;
				}
			}
		}
	}

	return 0;
}

int  UnpackJicScale(FrameCHInfo* pChInfo, int nJicCb, short *pnMaxCb, Bitstream *bitptr)
{
	int nCluster, nBand;
	int nQStep, Booklen;
	const int *hufBook;
	FrameDataInfo *frame;
	FrameJicScale *fJicScale;

	frame = &pChInfo->frame;
	fJicScale = pChInfo->fJicScale;

	if(frame->nWinTypeCurrent <= WIN_LONG_BRIEF2SHORT)
	{
		hufBook = HuffDec6_116x1;
		Booklen = 116;
	}
	else
	{
		hufBook = HuffDec7_116x1;
		Booklen = 116;
	}

	nQStep = 57;
	for(nCluster = 0; nCluster < frame->nNumCluster; nCluster++){
		for(nBand = nJicCb; nBand < pnMaxCb[nCluster]; nBand++){
			nQStep = HuffDiff(bitptr, hufBook, nQStep, Booklen);
			fJicScale->mnQStepIndex[nCluster][nBand] = nQStep;
		}
	}

	return 0;
}

int  UnpackFrameData(DraDecInfo* decoder)
{
	FrameHeader		*header = NULL;
	FrameStream		*stream = NULL;
	Bitstream		*bitptr;	
	FrameCHInfo		*chInfo = NULL;
	FrameCHInfo		*chInfop = NULL;
	FrameCHInfo		*chInfo0 = NULL;
	VO_MEM_OPERATOR *pMemop;
	int				len, nch;

	header = decoder->header;
	stream = decoder->stream;
	pMemop = decoder->pvoMemop;
	bitptr = &stream->bitptr;
	
	chInfo0 =  decoder->chInfo[0];

	for(nch = 0; nch < header->nNumNormalCh; nch++) {
		chInfo = decoder->chInfo[nch];

		len  = UnpackWinSequence(decoder, nch);
		if(len < 0) return -1;

		len = UnpackCodeBooks(chInfo, bitptr);
		if(len < 0) return -1;

		if(MaxActCB(chInfo, decoder->sfbw) < 0)
			return -1;

		len = UnpackQIndex(chInfo, bitptr);
		if(len < 0) return -1;

		len = UnpackStepIndex(chInfo, bitptr);
		if(len < 0) return -1;
		
		if(header->bUseSumdiff && (nch & 1))
		{
			if(chInfo->fSumDff == NULL)
			{
				chInfo->fSumDff = (FrameSumDff *)mem_malloc(pMemop, sizeof(FrameSumDff), 32);
				if(!chInfo->fSumDff) return -1;
				pMemop->Set(VO_INDEX_DEC_DRA, chInfo->fSumDff, 0, sizeof(FrameSumDff));
				
			}
			
			len = UnpackSumDff(chInfo, header->nJicCb, chInfop->anMaxActcb, bitptr);
			if(len < 0) return -1;
		}

		if(header->bUseJIC && nch > 0)
		{
			if(chInfo->fJicScale == NULL)
			{
				chInfo->fJicScale = (FrameJicScale *)mem_malloc(pMemop, sizeof(FrameJicScale), 32);				
				if(!chInfo->fJicScale) return -1;
				pMemop->Set(VO_INDEX_DEC_DRA, chInfo->fJicScale, 0, sizeof(FrameJicScale));
			}

			len = UnpackJicScale(chInfo, header->nJicCb, chInfo0->anMaxActcb, bitptr);
			if(len < 0) return -1;
		}		

		chInfop = chInfo;
	}	

	for( ; nch < header->nNumNormalCh + header->nNumLfeCh; nch++) {
		chInfo = decoder->chInfo[nch];

		if(header->nNumBlocksPerFrm == 8)
		{
			chInfo->frame.nWinTypeCurrent = WIN_LONG_LONG2LONG;
			chInfo->frame.nNumCluster = 1;
			chInfo->frame.anNumBlocksPerFrmPerCluster[0] = 1;
		}
		else
		{
			chInfo->frame.nWinTypeCurrent = WIN_SHORT_SHORT2SHORT;
			chInfo->frame.nNumCluster = 1;
			chInfo->frame.anNumBlocksPerFrmPerCluster[0] = header->nNumBlocksPerFrm;
		}

		len = UnpackCodeBooks(chInfo, bitptr);
		if(len < 0) return -1;

		MaxActCB(chInfo, decoder->sfbw);

		len = UnpackQIndex(chInfo, bitptr);
		if(len < 0) return -1;

		len = UnpackStepIndex(chInfo, bitptr);
		if(len < 0) return -1;
	}	

	if(bitptr->bitsleft < 0)
		return -1;

	return 0;
}