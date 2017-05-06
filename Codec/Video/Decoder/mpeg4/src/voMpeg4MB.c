/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#include "voMpeg4MB.h"
#include "voMpegReadbits.h"
#include "voMpeg4Haff.h"
#include "voMpeg4Parser.h"
#include "voMpeg4ACDC.h"
#include "voMpegIdct.h"
#include "voMpegMC.h"
#include "voMpeg4MCQpel.h"
#include "voMpeg4Gmc.h"
#include "voMpeg4ACDC.h"
#include "voMpegBuf.h"


VOCONST VO_U8 scanTab[3][64] = {
	{	
		0,  1,  8, 16,  9,  2,  3, 10, 
		17,	24, 32, 25, 18, 11,  4,  5,
		12, 19, 26, 33, 40, 48, 41, 34, 
		27,	20, 13,  6,  7, 14, 21, 28, 
		35, 42,	49, 56, 57, 50, 43, 36, 
		29, 22,	15, 23, 30, 37, 44, 51, 
		58, 59, 52, 45, 38, 31, 39, 46, 
		53, 60, 61,	54, 47, 55, 62, 63
	},
	{
		0,  1,  2,  3,  8,  9, 16, 17, 
		10, 11,  4,  5,  6,  7, 15, 14,
		13, 12, 19, 18, 24, 25, 32, 33, 
		26, 27, 20, 21, 22, 23, 28, 29,
		30, 31, 34, 35, 40, 41, 48, 49, 
		42, 43, 36, 37, 38, 39, 44, 45,
		46, 47, 50, 51, 56, 57, 58, 59, 
		52, 53, 54, 55, 60, 61, 62, 63
	},
	{
		0,  8, 16, 24,  1,  9,  2, 10, 
		17, 25, 32, 40, 48, 56, 57, 49,
		41, 33, 26, 18,  3, 11,  4, 12, 
		19, 27, 34, 42, 50, 58, 35, 43,
		51, 59, 20, 28,  5, 13,  6, 14, 
		21, 29, 36, 44, 52, 60, 37, 45,
		53, 61, 22, 30,  7, 15, 23, 31, 
		38, 46, 54, 62, 39, 47, 55, 63
	}
};


VOCONST VO_U8 MCBPCtabIntra[] = {
	0,0,	20,6,	36,6,	52,6,	4,4,	4,4,	4,4,	4,4,
	19,3,	19,3,	19,3,	19,3,	19,3,	19,3,	19,3,	19,3,
	35,3,	35,3,	35,3,	35,3,	35,3,	35,3,	35,3,	35,3,
	51,3,	51,3,	51,3,	51,3,	51,3,	51,3,	51,3, 	51,3
};

VOCONST VO_U8 MCBPCtabInter[] = {
	0,0,	0,9,	52,9,	36,9,	20,9,	49,9,	35,8,	35,8, 
	19,8,	19,8,	50,8,	50,8,	51,7,	51,7,	51,7,	51,7,
	34,7,	34,7,	34,7,	34,7,	18,7,	18,7,	18,7,	18,7,
	33,7,	33,7,	33,7,	33,7, 	17,7,	17,7,	17,7,	17,7, 
	4,6,	4,6,	4,6,	4,6,	4,6,	4,6,	4,6,	4,6,
	48,6,	48,6,	48,6,	48,6,	48,6,	48,6,	48,6,	48,6, 
	3,5,	3,5,	3,5,	3,5,	3,5,	3,5,	3,5,	3,5, 
	3,5,	3,5,	3,5,	3,5,	3,5,	3,5,	3,5,	3,5, 
	32,4,	32,4,	32,4,	32,4,	32,4,	32,4,	32,4,	32,4, 
	32,4, 	32,4,	32,4,	32,4,	32,4,	32,4,	32,4,	32,4, 
	32,4,	32,4, 	32,4,	32,4,	32,4,	32,4,	32,4,	32,4, 
	32,4,	32,4,	32,4, 	32,4,	32,4,	32,4,	32,4,	32,4,
	16,4,	16,4,	16,4,	16,4, 	16,4,	16,4,	16,4,	16,4,
	16,4,	16,4,	16,4,	16,4,	16,4, 	16,4,	16,4,	16,4,
	16,4,	16,4,	16,4,	16,4,	16,4,	16,4, 	16,4,	16,4,
	16,4,	16,4,	16,4,	16,4,	16,4,	16,4,	16,4, 	16,4, 
	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,	2,3, 
	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,
	2,3, 	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,
	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,
	2,3,	2,3,	2,3, 	2,3,	2,3,	2,3,	2,3,	2,3,
	2,3,	2,3,	2,3,	2,3, 	2,3,	2,3,	2,3,	2,3, 
	2,3,	2,3,	2,3,	2,3,	2,3, 	2,3,	2,3,	2,3,
	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,	2,3,	2,3, 
	1,3,	1,3,	1,3,	1,3,	1,3,	1,3,	1,3, 	1,3,
	1,3,	1,3,	1,3,	1,3,	1,3,	1,3,	1,3,	1,3, 
	1,3,	1,3,	1,3,	1,3,	1,3,	1,3,	1,3,	1,3,
	1,3, 	1,3,	1,3,	1,3,	1,3,	1,3,	1,3,	1,3, 
	1,3,	1,3, 	1,3,	1,3,	1,3,	1,3,	1,3,	1,3, 
	1,3,	1,3,	1,3, 	1,3,	1,3,	1,3,	1,3,	1,3, 
	1,3,	1,3,	1,3,	1,3, 	1,3,	1,3,	1,3,	1,3, 
	1,3,	1,3,	1,3,	1,3,	1,3,	1,3,	1,3,	1,3
};

/**/

static VOCONST VO_U8 CBPYtab[] = 
{ 
	0,0, 0,0, 6,6,  9,6,  
	8,5,  8,5,  4,5,  4,5,//-1,0
	2,5,  2,5,  1,5,  1,5, 
	0,4,  0,4,  0,4,  0,4, 
	12,4, 12,4, 12,4, 12,4,
	10,4, 10,4, 10,4, 10,4,
	14,4, 14,4, 14,4, 14,4, 
	5,4,  5,4,  5,4,  5,4,
	13,4, 13,4, 13,4, 13,4,
	3,4,  3,4,  3,4,  3,4, 
	11,4, 11,4, 11,4, 11,4,
	7,4,  7,4,  7,4,  7,4 
};

/**/
/* K = 4 */
static VOCONST VO_U32 roundtab_76[16] =
{ 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1 };

/* K = 2 */
//const VO_U32 roundtab_78[8] =
//{ 0, 0, 1, 1, 0, 0, 0, 1  };

/* K = 1 */
static VOCONST VO_U32 roundtab_79[4] =
{ 0, 1, 0, 0 };


static INLINE VO_S32 GetDCsizeLum( VO_MPEG4_DEC* pDec )
{
	VO_S32 i,code = SHOW_BITS(pDec,11);
	
	if (code == 0){
//		pDec->nMBError |= ERR_LDC_SIZE;
		code = 1;
		
	}
	for (i = 0; i < 8; i++){
		if ((code >> i)==1){
			FLUSH_BITS(pDec,11-i);
//			pDec->nMBError |= ERR_LDC_SIZE; 
			return 12-i;
		}
	}
	
	code >>= 8;
	
	if (code == 1) {
		FLUSH_BITS(pDec,3);
		return 4;
	}else if (code == 2) { 
		FLUSH_BITS(pDec,3);
		return 3;
	}else if (code == 3) { 
		FLUSH_BITS(pDec,3);
		return 0;
	}
	
	code >>=1;
	
	if (code == 2) {
		FLUSH_BITS(pDec,2);
		return 2;
	}else if (code == 3) {
		FLUSH_BITS(pDec,2);
		return 1;
	}   
	return 0;
}

static INLINE VO_S32 GetDCsizeChr( VO_MPEG4_DEC* pDec )
{
	VO_S32 i,code = SHOW_BITS(pDec,12);
	if (code == 0){
		code = 1;
//		pDec->nMBError |= ERR_CDC_SIZE; 
	}
	for (i=0; i<10; i++){
		if ((code >> i)==1){
			FLUSH_BITS(pDec,12-i);
//			pDec->nMBError |= ERR_CDC_SIZE; 
			return 12-i;
		}
	}
	
	return 3 - GetBits(pDec,2);
}

static INLINE VO_S32 GetDCdiff(VO_S32 DCSize,VO_MPEG4_DEC* pDec)
{
	VO_S32 code = SHOW_BITS(pDec,32); //we need only DCSize nBits (but in the higher nBits)
	VO_S32 adj = 0;
	FLUSH_BITS_LARGE(pDec,DCSize);
	if (code >= 0)
		adj = (-1 << DCSize) + 1;
	return adj + ((VO_U32)code >> (32-DCSize));
}

void GetDPDCDiff(VO_MPEG4_DEC *pDec, VO_U32 nMBPos)
{
	VO_U32 nBlkIdx;

	for (nBlkIdx = 0; nBlkIdx < 6; ++nBlkIdx){
		VO_S32 DCSize, nDctDCDiff = 0;

		UPDATE_CACHE(pDec);

		DCSize = nBlkIdx<4 ? GetDCsizeLum(pDec) : GetDCsizeChr(pDec); //max11bit : max12bit

		if (DCSize>0){
			nDctDCDiff = GetDCdiff(DCSize, pDec);
			if (DCSize > 8)				
				FLUSH_BITS(pDec,1); // marker bit	
		}
		pDec->DP_Buffer->nDctDCDiff[nMBPos][nBlkIdx] = nDctDCDiff;
	}
}

VO_U32 MPEG4DecIntraMB( VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos, VO_S32 dp_pos)
{
	VO_S32 j;
//	VO_S32 nMBIdx = POSY(nMBPos)*pDec->nMBWidth + POSX(nMBPos);
	VO_S8 *blockLen = vDecLocal->blockLen;
	VO_S8 *blockscanType = vDecLocal->scanType;
	idct_t *blockptr = vDecLocal->blockptr;

//////////////////////////////////////////////////////////////////////////
// if (nMBIdx != dp_pos && dp_pos != 0)
// {
// 	printf("!!!!!!!!!!!!!!!!!!!!!!!!!nMBIdx:%d dp_pos:%d\n", nMBIdx, dp_pos);
// }
//////////////////////////////////////////////////////////////////////////
// printf("MBpos:%d %d\n", POSY(nMBPos), POSX(nMBPos));
	for (j = 0; j < 6; ++j){
		idct_t *dc_addr;
		VO_S32 subpos;
		VO_S32 dc_scaler = pDec->nQuant;

		VO_S8 *len = blockLen+j, *scanType = blockscanType+j;
		idct_t *block = blockptr + (j<<6);
		
		
		UPDATE_CACHE(pDec); 
		
		//stream: 24bit available
		
		if (j < 4) {
			if (dc_scaler >24) dc_scaler = dc_scaler*2 - 16;
			else if (dc_scaler>8) dc_scaler = (dc_scaler + 8);
			else if (dc_scaler>4) dc_scaler <<= 1;
			else dc_scaler = 8;
			
			// convert 1x1 -> 2x2 (accodring to block number)
			subpos = 2*POSX(nMBPos) + (POSY(nMBPos) << (MB_X2+2));
			subpos += (j & 1) + ((j & 2) << MB_X2);
		}
		else {
			if (dc_scaler > 24) dc_scaler = (dc_scaler - 6);
			else if (dc_scaler > 4) dc_scaler = (dc_scaler + 13)>>1;
			else dc_scaler = 8;
			
			subpos = nMBPos;
		}
		
		if (pDec->nQuant < pDec->nIntraDCThreshold){
			VO_S32 nDctDCDiff = 0;
			if(!pDec->DP_Buffer){
				VO_S32 DCSize;

				DCSize = j<4 ? GetDCsizeLum(pDec) : GetDCsizeChr(pDec); //max11bit : max12bit

				if (DCSize){
					nDctDCDiff = GetDCdiff(DCSize,pDec);
					if (DCSize > 8)				
						FLUSH_BITS(pDec,1); // marker bit	
				}
			}else{
				nDctDCDiff = pDec->DP_Buffer->nDctDCDiff[dp_pos][j];
			}

			*block = (idct_t)(nDctDCDiff * dc_scaler);
			*len = 1;
		}
		else
			*len = 0;
		
		// dc reconstruction, prediction direction
		dc_addr = dc_recon(pDec, j, subpos, dc_scaler );
		
		if(pDec->alter_ver_scan){
			*scanType = IDCTSCAN_ALT_VERT;
		}else{
			if (pDec->bACPred){ 
				if (pDec->predict_dir == TOP)
					*scanType = IDCTSCAN_ALT_HORI;
				else
					*scanType = IDCTSCAN_ALT_VERT;
			}
			else
				*scanType = IDCTSCAN_ZIGZAG;
		}
		
		if ((pDec->nCBP << (26+j)) < 0)
			*len = (VO_S8)pDec->vld_block(pDec,vld_intra,scanTab[*scanType],*len, block, 1);

		if(*len){
			if (pDec->nQuant >= pDec->nIntraDCThreshold){
				if (pDec->nQuantType)
					*block = (idct_t)(((*block*8) / (pDec->quant[0][0] * pDec->nQuant)) * dc_scaler);
				else
					*block = (idct_t)((*block / (pDec->nQuant*2)) * dc_scaler);
			}
		}
			
		*block = *dc_addr = (idct_t)(*block + *dc_addr);

		// ac reconstruction
		ac_recon(pDec, vDecLocal, block, j, subpos);
		
// 		if (pDec->nFrame == 0 && nMBPos == 256 && j == 0)
// 		{
// 			int x,y;
// // 			printf("%d\n", j);
// 			for (x = 0; x < 8; x++)
// 			{
// 				for (y = 0; y < 8; y++)
// 				{
// 					printf("%5d", block[x*8+y]);
// 				}
// 				printf("\n");
// 			}
// 		}
	}

//	(pDec->fpReconMB[nMBIdx])(pDec, vDecLocal, nMBPos);

	return VO_ERR_NONE;
}


VO_S32 GetChromaMV(VO_S32 v, const VO_S32 bQpel)
{
	VO_S32 dx, dy;

// 	dx = MVX(v, bQpel);
// 	dy = MVY(v, bQpel);
	dx = MVX(v, 0);
	dy = MVY(v, 0);
	if (bQpel)
	{
		dx /= 2;
		dy /= 2;
	}

	dx = (dx >> 1) + roundtab_79[dx & 0x3];
	dy = (dy >> 1) + roundtab_79[dy & 0x3];
	
	
	return MAKEMV(dx,dy);
}

VO_S32 GetChromaMV4(const VO_S32* pMV, const VO_S32 bQpel)
{
	VO_S32 dx, dy;

// 	dx = MVX(pMV[0], bQpel)+MVX(pMV[1], bQpel)+MVX(pMV[2], bQpel)+MVX(pMV[3], bQpel);
// 	dy = MVY(pMV[0], bQpel)+MVY(pMV[1], bQpel)+MVY(pMV[2], bQpel)+MVY(pMV[3], bQpel);
	if (!bQpel)
	{
		dx = MVX(pMV[0], 0)+MVX(pMV[1], 0)+MVX(pMV[2], 0)+MVX(pMV[3], 0);
		dy = MVY(pMV[0], 0)+MVY(pMV[1], 0)+MVY(pMV[2], 0)+MVY(pMV[3], 0);
	}
	else
	{
		dx = MVX(pMV[0], 0)/2+MVX(pMV[1], 0)/2+MVX(pMV[2], 0)/2+MVX(pMV[3], 0)/2;
		dy = MVY(pMV[0], 0)/2+MVY(pMV[1], 0)/2+MVY(pMV[2], 0)/2+MVY(pMV[3], 0)/2;
	}

	dx = (dx >> 3) + roundtab_76[dx & 0xf];
	dy = (dy >> 3) + roundtab_76[dy & 0xf];
	
	
	return MAKEMV(dx,dy);
}

VO_U32 MPEG4DecInterMB( VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL *vDecLocal, VO_S32 nMBPos)
{
	VO_S32 j;
//	VO_S32 nMBIdx = POSY(nMBPos)*pDec->nMBWidth + POSX(nMBPos);
	VO_S8 *blockLen = vDecLocal->blockLen;
	idct_t *blockptr = vDecLocal->blockptr;
	
// printf("MBpos:%d %d\n", POSY(nMBPos), POSX(nMBPos));

	// texture decoding add
	for (j = 0; j < 6; j++) {
		VO_S8 *len = blockLen+j;
		idct_t *block = blockptr + (j<<6);
		*len = 0;
		if ((pDec->nCBP << (26+j)) < 0) { //if coded
			*len = (VO_S8)pDec->vld_block(pDec,vld_inter,scanTab[IDCTSCAN_ZIGZAG], *len, block, 0);
			
// 			if (pDec->nFrame == 894 && nMBPos == 4 && j == 3)
// 			{
// 				int x,y;
// // 				printf("%d\n", j);
// 				for (x = 0; x < 8; x++)
// 				{
// 					for (y = 0; y < 8; y++)
// 					{
// 						printf("%5d", block[x*8+y]);
// 					}
// 					printf("\n");
// 				}
// 			}
		}
	}
//	(pDec->fpReconMB[nMBIdx])(pDec, vDecLocal,nMBPos);

	return 0;
}


VO_S32 GetCBPY( VO_MPEG4_DEC* pDec ) //max 6 nBits
{
	VO_S32 code = SHOW_BITS(pDec,6);
	
	if (code < 2){
		code = 2;
		pDec->nMBError |= ERR_CBPY;
	}
//	if (code < 2) return -1;
	if (code >= 48) {
		FLUSH_BITS(pDec,2);
		code = 15;
	} 
	else {
		FLUSH_BITS(pDec,CBPYtab[(code<<1)+1]);
		code = CBPYtab[code<<1];
	}
	return code;
}

VO_S32 CheckResyncMarker(VO_MPEG4_DEC *pDec)
{
	VO_U32 nBits = BitsToNextByte(pDec);
	VO_U32 nCode;
	UPDATE_CACHE(pDec);
	nCode = SHOW_BITS(pDec, nBits);

	return (nCode == (1 << (nBits-1)) - 1) &&
		(SHOW_BITS(pDec,nBits+pDec->nResyncMarkerBits) & 
		((1 << pDec->nResyncMarkerBits)-1)) == 1;
}


void
MPEG4DecGmcMB( VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos, VO_S32* mv )
{
	VO_U32 stride = pDec->nLumEXWidth;
	VO_U32 stride2 = pDec->nChrEXWidth;
	VO_S32 x_pos = POSX(nMBPos), y_pos = POSY(nMBPos);
	VO_S32 mvx = 0, mvy = 0;
	VO_VIDEO_GMC_DATA * vGmcData = &(pDec->vGmcData);
	VO_S32 nOffset = (y_pos << 4) * pDec->nLumEXWidth + (x_pos << 4);
	VO_U8 *pCurY = pDec->pCurFrameMB->y + nOffset;
	VO_U8 *pCurUV[2];
	nOffset = (y_pos << 3) * pDec->nChrEXWidth + (x_pos << 3);
	pCurUV[0] = pDec->pCurFrameMB->u + nOffset; 
	pCurUV[1] = pDec->pCurFrameMB->v + nOffset; 

	vGmcData->GMCGetAvgMV(vGmcData, &mvx, &mvy, x_pos, y_pos, pDec->qpel);

	mvx = GMCSanitize(mvx, pDec->qpel, pDec->nFcode);
	mvy = GMCSanitize(mvy, pDec->qpel, pDec->nFcode);

	*(mv) = *(mv+1) = *(mv+2) = *(mv+3) = MAKEMV(mvx, mvy);

	// this is where the calculations are done /
	vGmcData->GMCLuma(vGmcData,
		pCurY, pDec->pRefFrame->y,
		stride, stride, x_pos, y_pos, pDec->rounding);

	vGmcData->GMCChroma(vGmcData,
		pCurUV[0], pDec->pRefFrame->u,
		pCurUV[1], pDec->pRefFrame->v,
		stride2, stride2, x_pos, y_pos, pDec->rounding);

}

void MPEG4DecDirectMB( VO_MPEG4_DEC *pDec, VO_S32 nMBPos, VO_S32 dmv, VO_MPEG4_DEC_LOCAL* vDecLocal)
{
	VO_S32 *BckMV = vDecLocal->MVBack;
	VO_S32 *FwdMV = vDecLocal->MVFwd;
	VO_S32 XHigh, XLow, YHigh, YLow, range;
	
	VO_S32* mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
	VO_S32 j;

	VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos); 
// 	VO_S32 nMBIdx = nYMBPos*pDec->nMBWidth+nXMBPos;
// 	BckMV = pDec->MVBack + 6*nMBIdx;
// 	FwdMV = pDec->MVFwd + 6*nMBIdx;
	*(BckMV++) = 1<<31;
	*(FwdMV++) = 1<<31;

	range = 5 + pDec->qpel;
	XHigh = (VO_S32)(pDec->nMBWidth - nXMBPos) << range;
	XLow  = (-(VO_S32)nXMBPos -1 ) << range;

	YHigh = (VO_S32)(pDec->nMBHeight - nYMBPos) << range;
	YLow	 = (-(VO_S32)nYMBPos -1 ) << range;

	for (j=0;j<4;++j,++mv) {
		
// 		VO_S64 v;
		VO_S32 dx, dy; 
		VO_S32 mvx = MVX(*mv, 0), mvy = MVY(*mv, 0);
		
// 		v = pDec->TRB * MVX(*mv, 0);
// 		if ((VO_S32)(v>>32)<0) //best code for (v<0)
// 			dx = (VO_S32)((v-1) >> 32);
// 		else
// 			dx = (VO_S32)(v >> 32);
// 		if (dx<0) ++dx;
// 		
// 		v = pDec->TRB * MVY(*mv, 0);
// 		if ((VO_S32)(v>>32)<0) //best code for (v<0)
// 			dy = (VO_S32)((v-1) >> 32);
// 		else
// 			dy = (VO_S32)(v >> 32);
// 		if (dy<0) ++dy;
		
		//to calc "*" in positive, by Harry, 2011.11.18
		if (mvx >= 0)
		{
			dx = (VO_S32)((pDec->TRB * mvx) >> 32);
		}
		else
		{
			dx = -(VO_S32)((pDec->TRB * (-mvx)) >> 32);
		}

		if (mvy >= 0)
		{
			dy = (VO_S32)((pDec->TRB * mvy) >> 32);
		}
		else
		{
			dy = -(VO_S32)((pDec->TRB * (-mvy)) >> 32);
		}

		dx += MVX(dmv, 0);
		dy += MVY(dmv, 0);



		if (dx > XHigh) {  
			dx = XHigh; 
		} else if (dx < XLow) {
			dx = XLow;
		} 

		if (dy > YHigh) {  
			dy = YHigh; 
		} else if (dy < YLow) {
			dy = YLow;
		} 
		
		FwdMV[j] = MAKEMV(dx,dy);
		
		if (dmv & 0xFFFF)
			dx -= mvx;
		else
		{
// 			v = pDec->TRB_TRD * MVX(*mv, 0);
// 			if ((VO_S32)(v>>32)<0) //best code for (v<0)
// 				dx = (VO_S32)((v-1) >> 32);
// 			else
// 				dx = (VO_S32)(v >> 32);
// 			if (dx<0) ++dx;

			//to calc "*" in positive, by Harry, 2011.11.18
			if (mvx >= 0)
			{
				dx = -(VO_S32)((pDec->TRB_TRD * mvx) >> 32);
			}
			else
			{
				dx = (VO_S32)((pDec->TRB_TRD * (-mvx)) >> 32);
			}
		}
		
		if (dmv>>16)
			dy -= mvy;
		else
		{
// 			v = pDec->TRB_TRD * MVY(*mv, 0);
// 			if ((VO_S32)(v>>32)<0) //best code for (v<0)
// 				dy = (VO_S32)((v-1) >> 32);
// 			else
// 				dy = (VO_S32)(v >> 32);
// 			if (dy<0) ++dy;

			//to calc "*" in positive, by Harry, 2011.11.18
			if (mvy >= 0)
			{
				dy = -(VO_S32)((pDec->TRB_TRD * mvy) >> 32);
			}
			else
			{
				dy = (VO_S32)((pDec->TRB_TRD * (-mvy)) >> 32);
			}
		}
		
		if (dx > XHigh) {  
			dx = XHigh; 
		} else if (dx < XLow) {
			dx = XLow;
		} 

		if (dy > YHigh) {  
			dy = YHigh; 
		} else if (dy < YLow) {
			dy = YLow;
		} 

		BckMV[j] = MAKEMV(dx,dy);
	}
	
	FwdMV[4]=GetChromaMV4(FwdMV, pDec->qpel);
	BckMV[4]=GetChromaMV4(BckMV, pDec->qpel);
	
}


void ReconIntraMB(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos)
{
	VO_U32 nBlkIdx;
	VO_S32 uiStride = pDec->nLumEXWidth;
	VO_S16 *pDct_dat;
	VO_U8 *pDst;
	VO_S32 next_block = uiStride * 8;
	VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
//	VO_S32 nMBIdx = nYMBPos*pDec->nMBWidth + nXMBPos;
	VO_S8 *blockLen = vDecLocal->blockLen;
	VO_S8 *blockscanType = vDecLocal->scanType;
	idct_t *blockptr = vDecLocal->blockptr;
	VO_S32 nOffset = (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4);
	VO_U8 *pCurY = pDec->pCurFrameMB->y + nOffset;
	VO_U8 *pCurUV[2];
	nOffset = (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3);
	pCurUV[0] = pDec->pCurFrameMB->u + nOffset; 
	pCurUV[1] = pDec->pCurFrameMB->v + nOffset; 

// 	if (pCurY != vDecLocal->pCurY || pCurUV[0] != vDecLocal->pCurUV[0] || pCurUV[1] != vDecLocal->pCurUV[1])
// 	{
// 
// 	}

	if(pDec->bInterlaced && vDecLocal->bFieldDct){
		next_block = uiStride;
		uiStride <<= 1;
	}
		
	for(nBlkIdx=0;nBlkIdx<4;nBlkIdx++)
	{
		VO_S32 iLen;
		VO_S32 iScanType;

		iScanType = blockscanType[nBlkIdx];
		pDct_dat = blockptr + (nBlkIdx << 6);

		pDst = pCurY + (nBlkIdx >> 1) * next_block + ((nBlkIdx&1)<<3);
		
		iLen = blockLen[nBlkIdx];
		if(iScanType == IDCTSCAN_ZIGZAG){
			if (iLen == 1){
				IDCT_Block1x1(pDct_dat,pDst,uiStride,NULL,0);
			}else if ( iLen < 15 && (pDct_dat[32]==0)){
				IDCTBlock4x4(pDct_dat,pDst,uiStride,NULL,0);
			}else if ((iLen<26 && (!(((VO_U32*)pDct_dat)[2] | ((VO_U32*)pDct_dat)[6])))){
				IDCTBlock4x8(pDct_dat,pDst,uiStride,NULL,0);
			}else{
				IDCTBlock8x8(pDct_dat,pDst,uiStride,NULL,0);
			}
		}else if(iScanType == IDCTSCAN_ALT_HORI){
				IDCTBlock8x8(pDct_dat,pDst,uiStride,NULL,0);	
		}else{	//IDCTSCAN_ALT_VERT
			if (iLen < 15){
				IDCTBlock4x8(pDct_dat,pDst,uiStride,NULL,0);
			}else{
				IDCTBlock8x8(pDct_dat,pDst,uiStride,NULL,0);
			}
		}

// 		if (pDec->nFrame == 0 && nMBPos == 256 && nBlkIdx == 0)
// 		{
// 			int x,y;
// // 			printf("%d\n", j);
// 			for (x = 0; x < 8; x++)
// 			{
// 				for (y = 0; y < 8; y++)
// 				{
// 					printf("%5d", pDst[x*uiStride+y]);
// 				}
// 				printf("\n");
// 			}
// 		}
	}

	uiStride = pDec->nChrEXWidth;
	for(nBlkIdx = 0; nBlkIdx < 2; nBlkIdx++){
		VO_S32 iLen;
		VO_S32 iScanType;

		pDst = pCurUV[nBlkIdx];
		iScanType = blockscanType[nBlkIdx + 4];
		iLen = blockLen[nBlkIdx + 4];

		pDct_dat = blockptr + ((nBlkIdx<<6) + 256);
		if(iScanType == IDCTSCAN_ZIGZAG){
			if (iLen == 1){
				IDCT_Block1x1(pDct_dat,pDst,uiStride,NULL, 0);
			}else if ( iLen < 15 && (pDct_dat[32]==0)){
				IDCTBlock4x4(pDct_dat,pDst,uiStride,NULL, 0);
			}else if ((iLen<26 && (!(((VO_U32*)pDct_dat)[2] | ((VO_U32*)pDct_dat)[6])))){
			
				IDCTBlock4x8(pDct_dat,pDst,uiStride,NULL, 0);
			}else{
				IDCTBlock8x8(pDct_dat,pDst,uiStride,NULL, 0);
			}
		}else if(iScanType == IDCTSCAN_ALT_HORI){
				IDCTBlock8x8(pDct_dat,pDst,uiStride,NULL, 0);	
		}else{	//IDCTSCAN_ALT_VERT
			if (iLen < 15){
				IDCTBlock4x8(pDct_dat,pDst,uiStride,NULL, 0);
			}else{
				IDCTBlock8x8(pDct_dat,pDst,uiStride,NULL, 0);
			}
		}
	}
	
}

void ReconCopyMB(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos)
{
	VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
	VO_S32 nOffset = (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4);
	VO_U8 *pCurY = pDec->pCurFrameMB->y + nOffset;
	VO_U8 *pRefY = pDec->pRefFrameMB->y + nOffset;
 	VO_U8 *pCurUV[2];
	VO_U8 *pRefUV[2];
	nOffset = (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3);
	pCurUV[0] = pDec->pCurFrameMB->u + nOffset; 
	pCurUV[1] = pDec->pCurFrameMB->v + nOffset; 
	pRefUV[0] = pDec->pRefFrameMB->u + nOffset; 
	pRefUV[1] = pDec->pRefFrameMB->v + nOffset; 

	CopyBlock16x16(pRefY, pCurY, pDec->nLumEXWidth, pDec->nLumEXWidth,16);
	CopyBlock8x8(pRefUV[0], pCurUV[0], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
	CopyBlock8x8(pRefUV[1], pCurUV[1], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
}

void ReconCopyMB_B(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos)
{
	VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
	VO_S32 nOffset = (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4);
	VO_U8 *pCurY = pDec->pCurFrameMB->y + nOffset;
	VO_U8 *pDstY = pDec->pDstFrameMB->y + nOffset;
	VO_U8 *pCurUV[2];
	VO_U8 *pDstUV[2];
	nOffset = (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3);
	pCurUV[0] = pDec->pCurFrameMB->u + nOffset; 
	pCurUV[1] = pDec->pCurFrameMB->v + nOffset; 
	pDstUV[0] = pDec->pDstFrameMB->u + nOffset; 
	pDstUV[1] = pDec->pDstFrameMB->v + nOffset; 

	CopyBlock16x16(pCurY,pDstY , pDec->nLumEXWidth, pDec->nLumEXWidth,16);
	CopyBlock8x8(pCurUV[0], pDstUV[0], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
	CopyBlock8x8(pCurUV[1], pDstUV[1], pDec->nChrEXWidth, pDec->nChrEXWidth,8);
}

void ReconInterMB(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos)
{
	VO_U32 nBlkIdx;
	VO_S32 uiStride = pDec->nLumEXWidth;
	VO_S32 iLen;
	VO_S16 *pDct_dat;
	VO_U8 *pDst,*pSrc;
	VO_U8 *pTmp_MC_dat = vDecLocal->mc_data;
	VO_S32 MV;
	VO_S32 next_block = uiStride * 8;
	VO_S32 qpel = pDec->qpel + 1;
	VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
// 	VO_S32 nMBIdx = nYMBPos*pDec->nMBWidth + nXMBPos;
	VO_S8 *blockLen = vDecLocal->blockLen;
	idct_t *blockptr = vDecLocal->blockptr;
	VO_S32 *MVBack = vDecLocal->MVBack;
	VO_S32 nOffset = (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4);
	VO_U8 *pCurY = pDec->pCurFrameMB->y + nOffset;
	VO_U8 *pRefY = pDec->pRefFrameMB->y + nOffset;
	VO_U8 *pCurUV[2];
	VO_U8 *pRefUV[2];
// 	VO_U8 mc_data[BLOCK_SIZE*BLOCK_SIZE];
	nOffset = (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3);
	pCurUV[0] = pDec->pCurFrameMB->u + nOffset; 
	pCurUV[1] = pDec->pCurFrameMB->v + nOffset; 
	pRefUV[0] = pDec->pRefFrameMB->u + nOffset; 
	pRefUV[1] = pDec->pRefFrameMB->v + nOffset; 

//	pTmp_MC_dat = pDec->mc_data;
// 	pTmp_MC_dat = mc_data;

	for(nBlkIdx = 0; nBlkIdx < 4; nBlkIdx++){
		VO_S32 iOffset, x, y;

		MV = MVBack[nBlkIdx+1];
		iOffset = (nBlkIdx >> 1)*next_block + ((nBlkIdx&1)<<3);
		pDst = pCurY + iOffset;
		pDct_dat = blockptr + (nBlkIdx << 6);
		iLen = blockLen[nBlkIdx];


		x = MVX(MV, qpel);
		y = MVY(MV, qpel);

		pSrc = pRefY + x + uiStride * y + iOffset;

		if (iLen){
			if(!vDecLocal->gmcmb){
				if(pDec->qpel){
					MCMpeg4BlockQpel(pSrc, pTmp_MC_dat, uiStride, 8, MV_SUB_QPEL(MV), pDec->rounding);
				}else{
					AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc,pTmp_MC_dat,uiStride,8,8);
				}

				if (iLen == 1){
					IDCT_Block1x1(pDct_dat,pDst,uiStride,pTmp_MC_dat,8);
				}
				else if ( iLen < 15 && (pDct_dat[32]==0)){
					IDCTBlock4x4(pDct_dat,pDst,uiStride,pTmp_MC_dat,8);
				}
				else if ((iLen<26 && (!(((VO_U32*)pDct_dat)[2] | ((VO_U32*)pDct_dat)[6]))))
				{
					IDCTBlock4x8(pDct_dat,pDst,uiStride,pTmp_MC_dat,8);
				}
				else{
					IDCTBlock8x8(pDct_dat,pDst,uiStride,pTmp_MC_dat,8);
				}

			}else{
				if (iLen == 1){
					IDCT_Block1x1(pDct_dat,pDst,uiStride,pDst,uiStride);
				}
				else if ( iLen < 15 && (pDct_dat[32]==0)){
					IDCTBlock4x4(pDct_dat,pDst,uiStride,pDst,uiStride);
				}
				else if ((iLen<26 && (!(((VO_U32*)pDct_dat)[2] | ((VO_U32*)pDct_dat)[6])))){				
					IDCTBlock4x8(pDct_dat,pDst,uiStride,pDst,uiStride);
				}else{
					IDCTBlock8x8(pDct_dat,pDst,uiStride,pDst,uiStride);
				}
			}
		}
		else
		{
			if(!vDecLocal->gmcmb){
				if(pDec->qpel){
					MCMpeg4BlockQpel(pSrc, pDst, uiStride, uiStride, MV_SUB_QPEL(MV), pDec->rounding);
				}else{
					AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc,pDst,uiStride,uiStride,8);
				}
			}
		}

// 		if (pDec->nFrame == 110 && nMBPos == 4522 && nBlkIdx == 1)
//  		{
//  			int x,y;
//  			printf("MC data\n");
// 			for (x = 0; x < 8; x++)
// 			{
// 				for (y = 0; y < 8; y++)
// 				{
// 					printf("%5d", pTmp_MC_dat[x*8+y]);
// 				}
// 				printf("\n");
// 			}
// 
// 			printf("Pixel data\n");
//  			for (x = 0; x < 8; x++)
//  			{
//  				for (y = 0; y < 8; y++)
//  				{
//  					printf("%5d", pDst[x*uiStride+y]);
//  				}
//  				printf("\n");
//  			}
//  		}

	}

	uiStride = pDec->nChrEXWidth;
	MV = MVBack[5];

	for(nBlkIdx = 0; nBlkIdx < 2; nBlkIdx++)	//U,V
	{
		pDst = pCurUV[nBlkIdx];
		
		pSrc = pRefUV[nBlkIdx];
		pSrc += MVX(MV, 1) + uiStride * MVY(MV, 1);

		pDct_dat = blockptr + ((nBlkIdx +4)<<6);
		iLen = blockLen[nBlkIdx + 4];
		if(iLen)
		{
			if(!vDecLocal->gmcmb){
				AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc,pTmp_MC_dat,uiStride,8,8);

				if (iLen == 1){
					IDCT_Block1x1(pDct_dat,pDst,uiStride,pTmp_MC_dat,8);
				}
				else if ( iLen < 15 && (pDct_dat[32]==0)){
					IDCTBlock4x4(pDct_dat,pDst,uiStride,pTmp_MC_dat,8);
				}
				else if ((iLen<26 && (!(((VO_U32*)pDct_dat)[2] | ((VO_U32*)pDct_dat)[6])))){				
					IDCTBlock4x8(pDct_dat,pDst,uiStride,pTmp_MC_dat,8);
				}else{
					IDCTBlock8x8(pDct_dat,pDst,uiStride,pTmp_MC_dat,8);
				}
			}else{
				if (iLen == 1){
					IDCT_Block1x1(pDct_dat,pDst,uiStride,pDst,uiStride);
				}
				else if ( iLen < 15 && (pDct_dat[32]==0)){
					IDCTBlock4x4(pDct_dat,pDst,uiStride,pDst,uiStride);
				}
				else if ((iLen<26 && (!(((VO_U32*)pDct_dat)[2] | ((VO_U32*)pDct_dat)[6])))){				
					IDCTBlock4x8(pDct_dat,pDst,uiStride,pDst,uiStride);
				}else{
					IDCTBlock8x8(pDct_dat,pDst,uiStride,pDst,uiStride);
				}

			}
		}else{
			if(!vDecLocal->gmcmb)
				AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc,pDst,uiStride,uiStride,8);
		}
	}
}

void ReconInterMB_B(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos)
{
	VO_U32 nBlkIdx;
	VO_S32 uiStride = pDec->nLumEXWidth;
	VO_S32 iLen, qpel;
	VO_S16 *pDct_dat;
	VO_U8 *pDst,*pSrc;
	VO_U8 *pTmp_MC_dat = vDecLocal->mc_data;
	VO_S32 MV;
	VO_S32 next_block = uiStride * 8;
	VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
// 	VO_S32 nMBIdx = nYMBPos*pDec->nMBWidth + nXMBPos;
	VO_S8 *blockLen = vDecLocal->blockLen;
	idct_t *blockptr = vDecLocal->blockptr;
	VO_S32 *MVBack = vDecLocal->MVBack;
	VO_S32 *MVFwd = vDecLocal->MVFwd;
	VO_S32 nOffset = (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4);
	VO_U8 *pCurY = pDec->pCurFrameMB->y + nOffset;
	VO_U8 *pRefY = pDec->pRefFrameMB->y + nOffset;
	VO_U8 *pDstY = pDec->pDstFrameMB->y + nOffset;
	VO_U8 *pCurUV[2];
	VO_U8 *pRefUV[2];
	VO_U8 *pDstUV[2];
// 	VO_U8 mc_data[BLOCK_SIZE*BLOCK_SIZE];
	nOffset = (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3);
	pCurUV[0] = pDec->pCurFrameMB->u + nOffset; 
	pCurUV[1] = pDec->pCurFrameMB->v + nOffset; 
	pRefUV[0] = pDec->pRefFrameMB->u + nOffset; 
	pRefUV[1] = pDec->pRefFrameMB->v + nOffset; 
	pDstUV[0] = pDec->pDstFrameMB->u + nOffset; 
	pDstUV[1] = pDec->pDstFrameMB->v + nOffset; 

	qpel = pDec->qpel + 1;

//	pTmp_MC_dat = pDec->mc_data;
// 	pTmp_MC_dat = mc_data;

	for(nBlkIdx = 0; nBlkIdx < 4; nBlkIdx++)
	{
		VO_S32 iOffset, x, y;

		iOffset = (nBlkIdx >> 1)*next_block + ((nBlkIdx&1)<<3);

		pDst = pDstY + iOffset;
		
		pDct_dat = blockptr + (nBlkIdx << 6);
		iLen = blockLen[nBlkIdx];


////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (iLen){
			if (MVBack[0]){
				MV = MVBack[nBlkIdx+1];

				x = MVX(MV, qpel);
				y = MVY(MV, qpel);
				pSrc = pRefY + x + uiStride * y + iOffset;

				if(pDec->qpel){
					MCMpeg4BlockQpel(pSrc, pTmp_MC_dat, uiStride, 8, MV_SUB_QPEL(MV), pDec->rounding);
				}else{
					AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pTmp_MC_dat, uiStride, 8,8);
				}
				
				if (MVFwd[0]){
					MV = MVFwd[nBlkIdx+1];
					x = MVX(MV, qpel);
					y = MVY(MV, qpel);
					pSrc = pCurY + x + uiStride * y + iOffset;

					if(pDec->qpel){
						MCMpeg4BlockQpelAdd(pSrc, pTmp_MC_dat, uiStride, 8, MV_SUB_QPEL(MV), pDec->rounding);
					}else{
						AllAddBlock[MV_SUB(MV)](pSrc, pTmp_MC_dat, uiStride,8, 8);
					}
				}
			}else if (MVFwd[0])	{
				MV = MVFwd[nBlkIdx+1];
				x = MVX(MV, qpel);
				y = MVY(MV, qpel);
				pSrc = pCurY + x + uiStride * y + iOffset;
			
				if(pDec->qpel){
					MCMpeg4BlockQpel(pSrc, pTmp_MC_dat, uiStride, 8, MV_SUB_QPEL(MV), pDec->rounding);
				}else{
					AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pTmp_MC_dat, uiStride, 8,8);
				}
			}
				
			if (iLen == 1){
				IDCT_Block1x1(pDct_dat,pDst,uiStride,pTmp_MC_dat,8);
			}
			else if ( iLen < 15 && (pDct_dat[32]==0))
			{
				IDCTBlock4x4(pDct_dat,pDst,uiStride,pTmp_MC_dat,8);
			}
			else if ((iLen<26 && (!(((VO_U32*)pDct_dat)[2] | ((VO_U32*)pDct_dat)[6])))){			
				IDCTBlock4x8(pDct_dat, pDst, uiStride,pTmp_MC_dat,8);			
			}else{
				IDCTBlock8x8(pDct_dat, pDst, uiStride, pTmp_MC_dat,8);
			}

		}else{
			// interpolate back and foward (using tmp buffer)
			
			if (MVBack[0] && MVFwd[0]){
				MV = MVBack[nBlkIdx+1];
				x = MVX(MV, qpel);
				y = MVY(MV, qpel);
				pSrc = pRefY + x + uiStride * y + iOffset;

				if(pDec->qpel){
					MCMpeg4BlockQpel(pSrc, pTmp_MC_dat, uiStride, 8, MV_SUB_QPEL(MV), pDec->rounding);
				}else{
					AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pTmp_MC_dat, uiStride, 8,8);
				}
				
				MV = MVFwd[nBlkIdx+1];
				x = MVX(MV, qpel);
				y = MVY(MV, qpel);
				pSrc = pCurY + x + uiStride * y + iOffset;

				if(pDec->qpel){
					MCMpeg4BlockQpelAdd(pSrc, pTmp_MC_dat, uiStride, 8, MV_SUB_QPEL(MV), pDec->rounding);
				}else{
					AllAddBlock[MV_SUB(MV)](pSrc, pTmp_MC_dat, uiStride,8,8);
				}
				// copy Tmp to Dst
				CopyBlock8x8(pTmp_MC_dat, pDst, 8, uiStride,8);
				//				}
			}else{
				if (MVBack[0]){
					MV = MVBack[nBlkIdx+1];
					x = MVX(MV, qpel);
					y = MVY(MV, qpel);
					pSrc = pRefY + x + uiStride * y + iOffset;

					if(pDec->qpel){
						MCMpeg4BlockQpel(pSrc, pDst, uiStride, uiStride, MV_SUB_QPEL(MV), pDec->rounding);
					}else{
						AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pDst, uiStride, uiStride,8);
					}
				}else{
					if (MVFwd[0]){
						MV = MVFwd[nBlkIdx+1];
						x = MVX(MV, qpel);
						y = MVY(MV, qpel);
						pSrc = pCurY + x + uiStride * y + iOffset;

						if(pDec->qpel){
							MCMpeg4BlockQpel(pSrc, pDst, uiStride, uiStride, MV_SUB_QPEL(MV), pDec->rounding);
						}else{
							AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pDst, uiStride, uiStride,8);
						}
					}
				}
			}
		}
	}


	uiStride = pDec->nChrEXWidth;
//	nWidth >>= 1;
//	MV = vDecLocal->MVBack[4];

	for(nBlkIdx = 0; nBlkIdx < 2; nBlkIdx++){//U,V
		pDst = pDstUV[nBlkIdx];
			
		pDct_dat = blockptr + ((nBlkIdx +4)<<6);

		iLen = blockLen[nBlkIdx + 4];

		if (iLen){
		
			// mcomp and idct (using tmp buffer)

			if (MVBack[0]){
				MV = MVBack[5];
				pSrc = pRefUV[nBlkIdx] + MVX(MV, 1) + uiStride * MVY(MV, 1);

				AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pTmp_MC_dat, uiStride, 8,8);
							
				if (MVFwd[0]){
					MV = MVFwd[5];
					pSrc = pCurUV[nBlkIdx] + MVX(MV, 1) + uiStride * MVY(MV, 1);

					AllAddBlock[MV_SUB(MV)](pSrc, pTmp_MC_dat, uiStride,8,8);
				}
			}else if (MVFwd[0]){
				MV = MVFwd[5];
				pSrc = pCurUV[nBlkIdx] + MVX(MV, 1) + uiStride * MVY(MV, 1);

				AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pTmp_MC_dat, uiStride, 8,8);
			}
				
			if (iLen == 1){
				IDCT_Block1x1(pDct_dat, pDst, uiStride, pTmp_MC_dat,8); 
			}
			else if ( iLen < 15 && (pDct_dat[32]==0))
			{
				IDCTBlock4x4(pDct_dat, pDst, uiStride, pTmp_MC_dat,8);
			}
			else if ((iLen<26 && (!(((VO_U32*)pDct_dat)[2] | ((VO_U32*)pDct_dat)[6])))){
				IDCTBlock4x8(pDct_dat, pDst, uiStride, pTmp_MC_dat,8);	
			}else{
				IDCTBlock8x8(pDct_dat, pDst, uiStride, pTmp_MC_dat,8);
			}

		}else{
			// interpolate back and foward (using tmp buffer)
			
			if (MVBack[0] && MVFwd[0]){
				MV = MVBack[5];
				pSrc = pRefUV[nBlkIdx] + MVX(MV, 1) + uiStride * MVY(MV, 1);


				AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pTmp_MC_dat, uiStride, 8,8);
				MV = MVFwd[5];
				pSrc = pCurUV[nBlkIdx] + MVX(MV, 1) + uiStride * MVY(MV, 1);

				AllAddBlock[MV_SUB(MV)](pSrc, pTmp_MC_dat, uiStride,8,8);
				// copy Tmp to Dst
				CopyBlock8x8(pTmp_MC_dat, pDst, 8, uiStride,8);
			}else{
				if (MVBack[0]){
					MV = MVBack[5];
					pSrc = pRefUV[nBlkIdx] + MVX(MV, 1) + uiStride * MVY(MV, 1);

					AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pDst, uiStride, uiStride,8);

				}else{
					if (MVFwd[0]){
						MV = MVFwd[5];
						pSrc = (VO_U8*)(pCurUV[nBlkIdx] + MVX(MV, 1) + uiStride * MVY(MV, 1));

						AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pDst, uiStride, uiStride,8);
					}
				}
			}
		}
	}
}



void ReconInterMBInterlace(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos)
{
	VO_U32 blk_idx, stride = pDec->nLumEXWidth;
	VO_S32 iLen, qpel ;
	VO_S16 *pDct_dat;
	VO_U8 *pDst,*pSrc;
	VO_S32 MV;
	VO_U32 next_block = stride * 8;
	VO_U32 dct_stride = stride;
	VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
// 	VO_S32 nMBIdx = nYMBPos*pDec->nMBWidth + nXMBPos;
	VO_S8 *blockLen = vDecLocal->blockLen;
	idct_t *blockptr = vDecLocal->blockptr;
	VO_S32 *MVBack = vDecLocal->MVBack;
	VO_S32 *MVFwd = vDecLocal->MVFwd;
	VO_S32 nOffset = (nYMBPos << 4) * pDec->nLumEXWidth + (nXMBPos << 4);
	VO_U8 *pCurY = pDec->pCurFrameMB->y + nOffset;
	VO_U8 *pRefY = pDec->pRefFrameMB->y + nOffset;
	VO_U8 *pDstY = pDec->pDstFrameMB->y + nOffset;
	VO_U8 *pCurUV[2];
	VO_U8 *pRefUV[2];
	VO_U8 *pDstUV[2];
	nOffset = (nYMBPos << 3) * pDec->nChrEXWidth + (nXMBPos << 3);
	pCurUV[0] = pDec->pCurFrameMB->u + nOffset; 
	pCurUV[1] = pDec->pCurFrameMB->v + nOffset; 
	pRefUV[0] = pDec->pRefFrameMB->u + nOffset; 
	pRefUV[1] = pDec->pRefFrameMB->v + nOffset; 
	pDstUV[0] = pDec->pDstFrameMB->u + nOffset; 
	pDstUV[1] = pDec->pDstFrameMB->v + nOffset; 

	qpel = pDec->qpel + 1;
		
	if (!vDecLocal->gmcmb)
	{
		if (!pDec->bFieldPred_buf[nMBPos])
		{
			for(blk_idx = 0; blk_idx < 4; blk_idx++){
				VO_S32 iOffset, x, y;

				iOffset = (blk_idx >> 1)*(stride * 8) + ((blk_idx&1)<<3);

				pDst = pDstY + iOffset;

				// mcomp and idct (using tmp buffer)
				if (MVBack[0]){
					MV = MVBack[blk_idx+1];
					x = MVX(MV, qpel);
					y = MVY(MV, qpel);
					pSrc = pRefY + x + stride * y + iOffset;

					if(pDec->qpel){
						MCMpeg4BlockQpel(pSrc, pDst, stride, stride, MV_SUB_QPEL(MV), pDec->rounding);
					}else{
						AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pDst, stride, stride, 8);
					}

					if (MVFwd[0]){
						MV = MVFwd[blk_idx+1];
						x = MVX(MV, qpel);
						y = MVY(MV, qpel);
						pSrc = pCurY + x + stride * y + iOffset;

						if(pDec->qpel){
							MCMpeg4BlockQpelAdd(pSrc, pDst, stride, stride, MV_SUB_QPEL(MV), pDec->rounding);
						}else{
							AllAddBlock[MV_SUB(MV)](pSrc, pDst, stride, stride, 8);
						}
					}
				}else if (MVFwd[0])	{
					MV = MVFwd[blk_idx+1];
					x = MVX(MV, qpel);
					y = MVY(MV, qpel);
					pSrc = pCurY + x + stride * y + iOffset;

					if(pDec->qpel){
						MCMpeg4BlockQpel(pSrc, pDst, stride, stride, MV_SUB_QPEL(MV), pDec->rounding);
					}else{
						AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pDst, stride, stride, 8);
					}
				}

			}
		}
		else
		{
			for(blk_idx = 0; blk_idx < 4; blk_idx++){
				VO_S32 iOffset, x, y;

				iOffset = (blk_idx >> 1)*stride + ((blk_idx&1)<<3);

				pDst = pDstY + iOffset;

				// mcomp and idct (using tmp buffer)
				if (MVBack[0]){
					MV = MVBack[blk_idx+1];
					x = MVX(MV, qpel);
					y = MVY(MV, qpel+1);
					pSrc = pRefY + x + stride*2 * y + ((blk_idx&1)<<3);
					if ((blk_idx<2 && (MVBack[0]&2)) ||
						(blk_idx>1 && (MVBack[0]&1)))
					{
						pSrc += stride;
					}
					
					if(pDec->qpel){
						MCMpeg4BlockQpel(pSrc, pDst, stride<<1, stride<<1, MV_SUB_QPEL_FIELD(MV), pDec->rounding);
					}else{
						AllCopyBlock[(pDec->rounding<<2) + MV_SUB_FIELD(MV)](pSrc, pDst, stride<<1, stride<<1, 8);
					}

					if (MVFwd[0]){
						MV = MVFwd[blk_idx+1];
						x = MVX(MV, qpel);
						y = MVY(MV, qpel+1);
						pSrc = pCurY + x + stride*2 * y + ((blk_idx&1)<<3);
						if ((blk_idx<2 && (MVFwd[0]&2)) ||
							(blk_idx>1 && (MVFwd[0]&1)))
						{
							pSrc += stride;
						}

						if(pDec->qpel){
							MCMpeg4BlockQpelAdd(pSrc, pDst, stride<<1, stride<<1, MV_SUB_QPEL_FIELD(MV), pDec->rounding);
						}else{
							AllAddBlock[MV_SUB_FIELD(MV)](pSrc, pDst, stride<<1, stride<<1, 8);
						}
					}
				}else if (MVFwd[0])	{
					MV = MVFwd[blk_idx+1];
					x = MVX(MV, qpel);
					y = MVY(MV, qpel+1);
					pSrc = pCurY + x + stride*2 * y + ((blk_idx&1)<<3);
					if ((blk_idx<2 && (MVFwd[0]&2)) ||
						(blk_idx>1 && (MVFwd[0]&1)))
					{
						pSrc += stride;
					}

					if(pDec->qpel){
						MCMpeg4BlockQpel(pSrc, pDst, stride<<1, stride<<1, MV_SUB_QPEL_FIELD(MV), pDec->rounding);
					}else{
						AllCopyBlock[(pDec->rounding<<2) + MV_SUB_FIELD(MV)](pSrc, pDst, stride<<1, stride<<1, 8);
					}
				}
			}		
		}
	}

	if(pDec->bInterlaced && vDecLocal->bFieldDct){
		next_block = stride;
		dct_stride <<= 1;
	}

	for(blk_idx = 0; blk_idx < 4; blk_idx++){
		iLen = blockLen[blk_idx];

		if (iLen){		
			pDct_dat = blockptr + (blk_idx << 6);
			pDst = pDstY + (blk_idx >> 1)*next_block + ((blk_idx&1)<<3);

			if (iLen == 1){
				IDCT_Block1x1(pDct_dat, pDst, dct_stride, pDst, dct_stride); 
			}else if ( iLen < 15 && (pDct_dat[32]==0)){
				IDCTBlock4x4(pDct_dat, pDst, dct_stride, pDst, dct_stride);
			}else if ((iLen<26 && (!(((VO_U32*)pDct_dat)[2] | ((VO_U32*)pDct_dat)[6])))){
				IDCTBlock4x8(pDct_dat, pDst, dct_stride, pDst, dct_stride);
			}else{
				IDCTBlock8x8(pDct_dat, pDst, dct_stride, pDst, dct_stride);
			}
		}
	}

	stride = pDec->nChrEXWidth;
	dct_stride = stride;

	if (!vDecLocal->gmcmb)
	{
		if (!pDec->bFieldPred_buf[nMBPos])
		{
			for(blk_idx = 0; blk_idx < 2; blk_idx++){//U,V
				pDst = pDstUV[blk_idx];

				// mcomp and idct (using tmp buffer)
				if (MVBack[0]){
					MV = MVBack[5];
					pSrc = pRefUV[blk_idx] + MVX(MV, 1) + stride * MVY(MV, 1);

					AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pDst, stride, stride, 8);

					if (MVFwd[0]){
						MV = MVFwd[5];
						pSrc = pCurUV[blk_idx] + MVX(MV, 1) + stride * MVY(MV, 1);

						AllAddBlock[MV_SUB(MV)](pSrc, pDst, stride, stride, 8);
					}
				}else if (MVFwd[0]){
					MV = MVFwd[5];
					pSrc = pCurUV[blk_idx] + MVX(MV, 1) + stride * MVY(MV, 1);
					AllCopyBlock[(pDec->rounding<<2) + MV_SUB(MV)](pSrc, pDst, stride, stride, 8);
				}
			}

		}
		else
		{
			VO_S32 b_dx[2]={0},b_dy[2]={0};
			VO_S32 f_dx[2]={0},f_dy[2]={0};
			//calc chroma mv in field
			if (MVBack[0])
			{
				b_dx[0] = MVX(MVBack[1],0);
				b_dy[0] = MVY(MVBack[1],1);
				b_dx[1] = MVX(MVBack[3],0);
				b_dy[1] = MVY(MVBack[3],1);

				if (pDec->qpel)
				{
					b_dx[0] /= 2;
	// 				b_dy[0] /= 2;
					b_dy[0] = DIV2ROUND(b_dy[0]);
					b_dx[1] /= 2;
	// 				b_dy[1] /= 2;
					b_dy[1] = DIV2ROUND(b_dy[1]);
				}
				b_dx[0] = DIV2ROUND(b_dx[0]);
				b_dy[0] = DIV2ROUND(b_dy[0]);
				b_dx[1] = DIV2ROUND(b_dx[1]);
				b_dy[1] = DIV2ROUND(b_dy[1]);
			}
			if (MVFwd[0])
			{
				f_dx[0] = MVX(MVFwd[1],0);
				f_dy[0] = MVY(MVFwd[1],1);
				f_dx[1] = MVX(MVFwd[3],0);
				f_dy[1] = MVY(MVFwd[3],1);

				if (pDec->qpel)
				{
					f_dx[0] /= 2;
	// 				f_dy[0] /= 2;
					f_dy[0] = DIV2ROUND(f_dy[0]);
					f_dx[1] /= 2;
	// 				f_dy[1] /= 2;
					f_dy[1] = DIV2ROUND(f_dy[1]);
				}
				f_dx[0] = DIV2ROUND(f_dx[0]);
				f_dy[0] = DIV2ROUND(f_dy[0]);
				f_dx[1] = DIV2ROUND(f_dx[1]);
				f_dy[1] = DIV2ROUND(f_dy[1]);
			}

			for(blk_idx = 0; blk_idx < 2; blk_idx++){//U,V
				pDst = pDstUV[blk_idx];

				// mcomp and idct (using tmp buffer)
				if (MVBack[0]){
					pSrc = pRefUV[blk_idx] + (b_dx[0]>>1) + stride*2 * (b_dy[0]>>1) + stride*((MVBack[0]&2)>>1);
					AllCopyBlock[(pDec->rounding<<2) + (b_dy[0]&1)*2 + (b_dx[0]&1)](pSrc, pDst, stride<<1, stride<<1, 4);

					pSrc = pRefUV[blk_idx] + (b_dx[1]>>1) + stride*2 * (b_dy[1]>>1) + stride*(MVBack[0]&1);
					AllCopyBlock[(pDec->rounding<<2) + (b_dy[1]&1)*2 + (b_dx[1]&1)](pSrc, pDst+stride, stride<<1, stride<<1, 4);

					if (MVFwd[0]){
						pSrc = pCurUV[blk_idx] + (f_dx[0]>>1) + stride*2 * (f_dy[0]>>1) + stride*((MVFwd[0]&2)>>1);
						AllAddBlock[(f_dy[0]&1)*2 + (f_dx[0]&1)](pSrc, pDst, stride<<1, stride<<1, 4);

						pSrc = pCurUV[blk_idx] + (f_dx[1]>>1) + stride*2 * (f_dy[1]>>1) + stride*(MVFwd[0]&1);
						AllAddBlock[(f_dy[1]&1)*2 + (f_dx[1]&1)](pSrc, pDst+stride, stride<<1, stride<<1, 4);
					}

				}else if (MVFwd[0]){
					pSrc = pCurUV[blk_idx] + (f_dx[0]>>1) + stride*2 * (f_dy[0]>>1) + stride*((MVFwd[0]&2)>>1);
					AllCopyBlock[(pDec->rounding<<2) + (f_dy[0]&1)*2 + (f_dx[0]&1)](pSrc, pDst, stride<<1, stride<<1, 4);

					pSrc = pCurUV[blk_idx] + (f_dx[1]>>1) + stride*2 * (f_dy[1]>>1) + stride*(MVFwd[0]&1);
					AllCopyBlock[(pDec->rounding<<2) + (f_dy[1]&1)*2 + (f_dx[1]&1)](pSrc, pDst+stride, stride<<1, stride<<1, 4);
				}
			}
		}
	}

	for(blk_idx = 0; blk_idx < 2; blk_idx++){//U,V

		iLen = blockLen[blk_idx + 4];

		if (iLen){			
			pDst = pDstUV[blk_idx];
			pDct_dat = blockptr + ((blk_idx +4)<<6);

			if (iLen == 1){
				IDCT_Block1x1(pDct_dat, pDst, dct_stride, pDst, dct_stride); 
			}else if ( iLen < 15 && (pDct_dat[32]==0)){
				IDCTBlock4x4(pDct_dat, pDst, dct_stride, pDst, dct_stride);
			}else if ((iLen<26 && (!(((VO_U32*)pDct_dat)[2] | ((VO_U32*)pDct_dat)[6])))){
				IDCTBlock4x8(pDct_dat, pDst, dct_stride, pDst, dct_stride);	
			}else{
				IDCTBlock8x8(pDct_dat, pDst, dct_stride, pDst, dct_stride);
			}

		}
	}
}


static void H263ReconDCAC(VO_MPEG4_DEC* pDec, idct_t *block, VO_S32 BlkNum, VO_S32 nMBPos)
{
	VO_S32 i;
	VO_S32 pred_dc;
	VO_S32	Fa,Fc;
	idct_t *ac_top;
	idct_t *ac_left;
	idct_t *dc;


	Fa = 1024;
	Fc = 1024;

	//get dc c
	if (BlkNum < 4) {
		dc = pDec->dc_lum;
		if (nMBPos >= MB_X*2){
			Fc=dc[(nMBPos-MB_X*2) % (MB_X*4)];
		}
		if((nMBPos%(MB_X*2)) & DC_LUM_MASK){
			Fa = dc[((nMBPos%(MB_X*4)) & DC_LUM_MASK) - 1];
		}
	}else {
		dc = pDec->dc_chr[BlkNum & 1];
		if (nMBPos >= MB_X){
			Fc=dc[(nMBPos-MB_X) % (MB_X*2)];
		}
		if((nMBPos%(MB_X)) & DC_CHR_MASK){
			Fa = dc[((nMBPos%(MB_X*2)) & DC_CHR_MASK) - 1];
		}
	}

	if (BlkNum < 4) {
		ac_top = pDec->ac_top_lum[nMBPos & (MB_X*2-1)];
		ac_left = pDec->ac_left_lum[(nMBPos >> (MB_X2+1)) & 1];
		i = MB_X*2-1; //stride-1
	}else {
		ac_top = pDec->ac_top_chr[nMBPos & (MB_X-1)][BlkNum & 1];
		ac_left = pDec->ac_left_chr[BlkNum & 1];
		i = MB_X-1; //stride-1
	}

	if (pDec->bACPred)	{
		pred_dc = 1024;
		if (pDec->predict_dir){/*left*/
			if (Fa != 1024){
				for (i = 1; i < 8; i++){
					block[i<<3] = (idct_t)(block[i<<3] + ac_left[i]);
				}
				 pred_dc = Fa;
			}
		}else{ /*top*/
			if (Fc != 1024){
				for (i = 1; i < 8; i++){
					block[i] = (idct_t)(block[i] + ac_top[i]);
				}
				 pred_dc = Fc;
			}
		}
	}else{
        if (Fa != 1024 && Fc != 1024)
            pred_dc = (Fa + Fc) >> 1;
        else if (Fa != 1024)
            pred_dc = Fa;
        else
            pred_dc = Fc;
	}

//	DPRINTF(ff_debug,"pred_dc: %d \n",pred_dc );//,FrameCount);
    block[0] = (idct_t)(block[0] + pred_dc);
    
    if (block[0] < 0){
        block[0] = 0;
	}else {
        block[0] |= 1;
	}

	if (BlkNum < 4) {
		dc[(nMBPos%(MB_X*4)) & DC_LUM_MASK] = block[0];
	}else{
		dc[(nMBPos%(MB_X*2)) & DC_CHR_MASK] = block[0];
	}

	for (i = 1; i < 8; i++){ 
		ac_top[i] = block[i];
		ac_left[i] = block[i<<3];
	}
}

///////////////////////////////////////////////////////////////////////
/* H.263 related*/
static void H263UpdateDCAC(VO_MPEG4_DEC* pDec, idct_t *block, VO_S32 BlkNum, VO_S32 nMBPos)
{
	VO_S32 i;

	idct_t *ac_top;
	idct_t *ac_left;
	idct_t *dc;

	if (BlkNum < 4) {
		dc = pDec->dc_lum;
		dc[(nMBPos%(MB_X*4)) & DC_LUM_MASK] = 1024;
	}else{
		dc = pDec->dc_chr[BlkNum & 1];
		dc[(nMBPos%(MB_X*2)) & DC_CHR_MASK] = 1024;
	}

	if (BlkNum < 4) {
		ac_top = pDec->ac_top_lum[nMBPos & (MB_X*2-1)];
		ac_left = pDec->ac_left_lum[(nMBPos >> (MB_X2+1)) & 1];
		i = MB_X*2-1; //stride-1
	}else {
		ac_top = pDec->ac_top_chr[nMBPos & (MB_X-1)][BlkNum & 1];
		ac_left = pDec->ac_left_chr[BlkNum & 1];
		i = MB_X-1; //stride-1
	}


	for (i = 1; i < 8; i++){ 
		ac_top[i] = 1024;
		ac_left[i] = 1024;
	}
}

void H263DecIntraMB( VO_MPEG4_DEC *pDec, VO_S32 nMBPos, VO_MPEG4_DEC_LOCAL *vDecLocal)
{
	VO_S32 j, nQuant = pDec->nQuant;
	VO_S8 *blockLen = vDecLocal->blockLen;
	VO_S8 *blockscanType = vDecLocal->scanType;
	idct_t *blockptr = vDecLocal->blockptr;
	idct_t *block;
	
	for (j = 0; j < 6; ++j){
		VO_S32 dc;
		VO_U16 *vld_tab;
		VO_S8 *len = blockLen+j, *scanType = blockscanType+j;
		VO_S32 subpos;

		block = blockptr + (j << 6);
		//clearblock(block);	

		UPDATE_CACHE(pDec);

		if (j < 4) {
			subpos = 2*POSX(nMBPos) + (POSY(nMBPos) << (MB_X2+2));
			subpos += (j & 1) + ((j & 2) << MB_X2);
		}else {
			subpos = nMBPos;
			nQuant = pDec->chroma_qscale_table[pDec->nQuant];
		}

		//stream: 24bit available
		if(pDec->aic){
			vld_tab = (VO_U16 *)vld_intra_aic;
			*len = 0;
		}else{
			dc = GetBits(pDec, 8);
			if (dc == 255)
				dc = 128;
			*block = (idct_t)(dc << 3);

			*len = 1;
			vld_tab = (VO_U16 *)vld_inter;
		}

		if (pDec->bACPred){
			if (pDec->predict_dir)/*left*/
				*scanType = IDCTSCAN_ALT_VERT;
			else
				*scanType = IDCTSCAN_ALT_HORI;/*top*/
		}else{
			*scanType = IDCTSCAN_ZIGZAG;
		}

		if ((pDec->nCBP << (26+j)) < 0){
			*len = (VO_S8)H263GetBlockVld(pDec,vld_tab,scanTab[*scanType],*len, block, nQuant);
		}

		if(pDec->aic){
			H263ReconDCAC(pDec, block, j,subpos);
		}
	}
//	ReconIntraMB(pDec, vDecLocal, nMBPos);
}


void H263DecInterMB( VO_MPEG4_DEC *pDec, VO_S32 nMBPos, VO_MPEG4_DEC_LOCAL *vDecLocal)
{
	VO_S32 j, nQuant = pDec->nQuant;
	VO_S32 subpos;
	VO_S8 *blockLen = vDecLocal->blockLen;
	idct_t *blockptr = vDecLocal->blockptr;

	// texture decoding add
	for (j = 0; j < 6; j++) {	
		VO_S8 *len = blockLen+j;
		idct_t* block = blockptr + (j<<6);
		*len = 0;

		if (j < 4) {
			subpos = 2*POSX(nMBPos) + (POSY(nMBPos) << (MB_X2+2));
			subpos += (j & 1) + ((j & 2) << MB_X2);
		}else {
			subpos = nMBPos;
			nQuant = pDec->chroma_qscale_table[pDec->nQuant];
		}

		if ((pDec->nCBP << (26+j)) < 0) { //if coded
			*len = (VO_S8)H263GetBlockVld(pDec,vld_inter,scanTab[IDCTSCAN_ZIGZAG], *len, block, nQuant);
		}
		if(pDec->aic)
			H263UpdateDCAC(pDec, block, j,subpos);
	}

// 	ReconInterMB(pDec, vDecLocal, nMBPos);
}

// #if ENABLE_MULTI_THREAD

// static MBRow_t *pDec->MBr = NULL;
// static pthread_mutex_t s_parserMutex;
// static pthread_mutex_t s_readyMutex;
// static pthread_mutex_t s_finishMutex;
// 
// static pthread_cond_t s_parserCond;
// static pthread_cond_t s_readyCond;
// static pthread_cond_t s_finishCond;

// pthread_mutex_t * VOCONST &(pDec->parserMutex) = &s_parserMutex;
// pthread_mutex_t * VOCONST &(pDec->readyMutex) = &s_readyMutex;
// pthread_mutex_t * VOCONST &(pDec->finishMutex) = &s_finishMutex;
// 
// pthread_cond_t * VOCONST &(pDec->parserCond) = &s_parserCond;
// pthread_cond_t * VOCONST &(pDec->readyCond) = &s_readyCond;
// pthread_cond_t * VOCONST &(pDec->finishCond) = &s_finishCond;

// volatile VO_U32 pDec->nParserErr = VO_ERR_NONE;
// volatile VO_S32 pDec->nParserMBPos = -1;
// volatile VO_S32 pDec->nWaiter = 0;
// volatile VO_S32 pDec->nParser = 0;
// volatile VO_S32 pDec->bEndVop = 0;
// volatile VO_S32 pDec->nFinish = 0;
// VO_S32 *volatile pDec->pParserMBQuant = NULL;
// volatile VO_S32 pDec->vfprev = 0;
// volatile VO_S32 pDec->vbprev = 0;

extern VOCONST VO_S8 DQtab[4];
VO_U32 MBrMPEG4VOP(MBRow_t *pMBr)
{
	VO_MPEG4_DEC *pDec = pMBr->pDec;
	VO_MPEG4_DEC_LOCAL vDecLocal;

	switch(pDec->nVOPType)
	{
	case VO_VIDEO_FRAME_I:
		vDecLocal.voERR = VO_ERR_DEC_MPEG4_I_FRAME;
		break;
#ifdef ASP_GMC
	case VO_VIDEO_FRAME_S:
		//no break, go on
#endif//ASP_GMC
	case VO_VIDEO_FRAME_P:
		vDecLocal.voERR = VO_ERR_DEC_MPEG4_P_FRAME;
		break;
	case VO_VIDEO_FRAME_B:
		vDecLocal.voERR = VO_ERR_DEC_MPEG4_B_FRAME;
		break;
	default:
		vDecLocal.voERR = VO_ERR_DEC_MPEG4_HEADER;
	}

	if(pDec->bInterlaced)
	{
		vDecLocal.fpReconMB_i = ReconInterMBInterlace;
	}
	else if(pDec->nVOPType != VO_VIDEO_FRAME_B)
		vDecLocal.fpReconMB_i = ReconInterMB;
	else
		vDecLocal.fpReconMB_i = ReconInterMB_B;

// 	if (pthread_mutex_trylock(&(pDec->parserMutex)))
// 	{
// 		VO_ATOMIC_Inc(&pDec->nWaiter);	//need atom-op because it's out lock
// 		pthread_mutex_lock(&(pDec->parserMutex));
// 		VO_ATOMIC_Dec(&pDec->nWaiter);	//need atom-op because it may be written by other threads out lock
// 	}
	
#if USE_WHILE_FORVER
	WaitForParser(pDec);
#else
	pthread_mutex_lock(&(pDec->parserMutex));
	if (pDec->nParser)
	{
		++pDec->nWaiter;
		do 
		{
			pthread_cond_wait(&(pDec->parserCond), &(pDec->parserMutex));
		} while (pDec->nParser);
		--pDec->nWaiter;
	}
	pDec->nParser = 1;
	pthread_mutex_unlock(&(pDec->parserMutex));
#endif

	if (pDec->nParserErr != VO_ERR_NONE || !pMBr->runCMD)
	{
#if USE_WHILE_FORVER
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		pthread_mutex_unlock(&(pDec->parserMutex));
#else
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		if (pDec->nWaiter)
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
			pthread_cond_signal(&(pDec->parserCond));
		}
		else
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
		}
#endif
		return pDec->nParserErr;
	}

	vDecLocal.nMBPos = pDec->nParserMBPos + 1;
	if (vDecLocal.nMBPos + MB_X-pDec->nMBWidth >= pDec->nPosEnd)
	{
#if USE_WHILE_FORVER
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		pthread_mutex_unlock(&(pDec->parserMutex));
#else
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		if (pDec->nWaiter)
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
			pthread_cond_signal(&(pDec->parserCond));
		}
		else
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
		}
#endif
		return VO_ERR_NONE;
	}
	vDecLocal.pMBQuant = pDec->pParserMBQuant;		
	if (pDec->nVOPType == VO_VIDEO_FRAME_B)
	{
		vDecLocal.fprev0 = pDec->vfprev0;
		vDecLocal.bprev0 = pDec->vbprev0;
		if (pDec->bInterlaced)
		{
			vDecLocal.fprev1 = pDec->vfprev1;
			vDecLocal.bprev1 = pDec->vbprev1;
		}
	}
	MPEG4DecMBRow(pDec, &vDecLocal, pMBr->vopCtxt);

	return VO_ERR_NONE;
}

VO_U32 MBrH263IVOP(MBRow_t *pMBr)
{
	VO_MPEG4_DEC *pDec = pMBr->pDec;
	VO_MPEG4_VOP_CONTEXT *vopCtxt = pMBr->vopCtxt;
	VO_U32 voRC = VO_ERR_NONE;
	VO_S32 mcbpc, nMBPos;
	//const VO_U32 nMBWidth = pDec->nMBWidth, nMBHeight = pDec->nMBHeight;
	FuncReconMB* fpReconMB = vopCtxt->fpReconMB;
	VO_S32 *pMBPos = vopCtxt->pMBPos;
	VO_MPEG4_DEC_LOCAL vDecLocal;

	vopCtxt->nMBCount = 0;
	vDecLocal.bFieldDct = 0;
	vDecLocal.scanType = vopCtxt->scanType;
	vDecLocal.blockptr = vopCtxt->blockptr;
	vDecLocal.blockLen = vopCtxt->blockLen;

	pthread_mutex_lock(&(pDec->parserMutex));
	if (pDec->nParser)
	{
		++pDec->nWaiter;
		do 
		{
			pthread_cond_wait(&(pDec->parserCond), &(pDec->parserMutex));
		} while (pDec->nParser);
		--pDec->nWaiter;
	}
	pDec->nParser = 1;
	pthread_mutex_unlock(&(pDec->parserMutex));

	if (pDec->nParserErr != VO_ERR_NONE || !pMBr->runCMD)
	{
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		if (pDec->nWaiter)
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
			pthread_cond_signal(&(pDec->parserCond));
		}
		else
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
		}
		return pDec->nParserErr;
	}

	nMBPos = pDec->nParserMBPos + 1;
	if (nMBPos + MB_X-pDec->nMBWidth >= pDec->nPosEnd)
	{
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		if (pDec->nWaiter)
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
			pthread_cond_signal(&(pDec->parserCond));
		}
		else
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
		}
		return voRC;
	}
	vDecLocal.pMBQuant = pDec->pParserMBQuant;		

	for (;nMBPos<pDec->nPosEnd;nMBPos+=MB_X-pDec->nMBWidth) {
		for (;POSX(nMBPos)<pDec->nMBWidth;++nMBPos) {
			VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);
			UPDATE_CACHE(pDec);

			if (EofBits(pDec))
			{
				pDec->nParserErr = VO_ERR_INPUT_BUFFER_SMALL;
				pthread_mutex_lock(&(pDec->parserMutex));
				pDec->nParser = 0;
				if (pDec->nWaiter)
				{
					pthread_mutex_unlock(&(pDec->parserMutex));
					pthread_cond_signal(&(pDec->parserCond));
				}
				else
				{
					pthread_mutex_unlock(&(pDec->parserMutex));
				}
				return pDec->nParserErr;
			}

			if (SHOW_BITS(pDec,16) == 0){ 
				if(VO_ERR_NONE != Resync(pDec, &nMBPos))
				{
					pDec->nParserErr = VO_ERR_DEC_MPEG4_I_FRAME;
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					if (pDec->nWaiter)
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
						pthread_cond_signal(&(pDec->parserCond));
					}
					else
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
					}
					return pDec->nParserErr;
				}

				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);
				//pMBQuant update, ----by harry, 2011,11,8
				vDecLocal.pMBQuant = pDec->quantrow + nXMBPos;
			}
			*pMBPos = nMBPos;

			pDec->nMBError = 0;

			//stream: 24bits available
			do{
				UPDATE_CACHE(pDec);
				mcbpc = GetMCBPC_I(pDec); // mcbpc
			}while(mcbpc < 0);

			//stream: 15bits available

			if (pDec->aic){
				pDec->bACPred = GetBits(pDec, 1); //todo: aic
				if(pDec->bACPred){
					pDec->predict_dir = GetBits(pDec, 1);
				}
			}else{
				pDec->bACPred = 0;
			}

			pDec->nCBP = (GetCBPY(pDec) << 2) | ((mcbpc >> 4) & 3);

			if ((mcbpc & 7) == INTRA_Q){ //mb_type
				VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
				pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
			}
			*fpReconMB = ReconIntraMB;
			H263DecIntraMB(pDec, nMBPos, &vDecLocal);
#if ENABLE_ERROR_CONCEAL
			if((pDec->nMBError & CONCEAL_ERROR)){
				pDec->nFrameError++;
//				*fpReconMB = ReconCopyMB;
			}
#endif

#if ENABLE_DEBLOCK
			pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
#endif
			*(vDecLocal.pMBQuant++) = pDec->nQuant;

			//condition of continue parser
			++vopCtxt->nMBCount;
			if (vopCtxt->nMBCount < pDec->nMpeg4ThdMinMB || (vopCtxt->nMBCount < pDec->nMpeg4ThdMaxMB && pDec->nWaiter == 0) )
			{
				//parser can go on
				++fpReconMB;
				++pMBPos;
				vDecLocal.scanType += 6;
				vDecLocal.blockptr += 6*64;
				vDecLocal.blockLen += 6;
			}
			else
			{
				pDec->nParserErr = VO_ERR_NONE;
				pDec->nParserMBPos = nMBPos;
				pDec->pParserMBQuant = vDecLocal.pMBQuant;
				pthread_mutex_lock(&(pDec->parserMutex));
				pDec->nParser = 0;
				if (pDec->nWaiter)
				{
					pthread_mutex_unlock(&(pDec->parserMutex));
					pthread_cond_signal(&(pDec->parserCond));
				}
				else
				{
					pthread_mutex_unlock(&(pDec->parserMutex));
				}

				fpReconMB = vopCtxt->fpReconMB;
				pMBPos = vopCtxt->pMBPos;
				vDecLocal.scanType = vopCtxt->scanType;
				vDecLocal.blockptr = vopCtxt->blockptr;
				vDecLocal.blockLen = vopCtxt->blockLen;
				do 
				{
					(*fpReconMB++)(pDec, &vDecLocal, *pMBPos++);
					vDecLocal.scanType += 6;
					vDecLocal.blockptr += 6*64;
					vDecLocal.blockLen += 6;
				} while (--vopCtxt->nMBCount);

				fpReconMB = vopCtxt->fpReconMB;
				pMBPos = vopCtxt->pMBPos;
				vDecLocal.scanType = vopCtxt->scanType;
				vDecLocal.blockptr = vopCtxt->blockptr;
				vDecLocal.blockLen = vopCtxt->blockLen;

				//wait & update (nMBPos, mv) pMBQuant?
				pthread_mutex_lock(&(pDec->parserMutex));
				if (pDec->nParser)
				{
					++pDec->nWaiter;
					do 
					{
						pthread_cond_wait(&(pDec->parserCond), &(pDec->parserMutex));
					} while (pDec->nParser);
					--pDec->nWaiter;
				}
				pDec->nParser = 1;
				pthread_mutex_unlock(&(pDec->parserMutex));

				if (pDec->nParserErr != VO_ERR_NONE)
				{
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					if (pDec->nWaiter)
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
						pthread_cond_signal(&(pDec->parserCond));
					}
					else
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
					}
					return pDec->nParserErr;
				}

				nMBPos = pDec->nParserMBPos;
				vDecLocal.pMBQuant = pDec->pParserMBQuant;
			}
		}
		vDecLocal.pMBQuant = pDec->quantrow;
	}

	pDec->nParserErr = VO_ERR_NONE;
	pDec->nParserMBPos = nMBPos - MB_X + pDec->nMBWidth - 1;	//let other threads finish the MB scanning
	pthread_mutex_lock(&(pDec->parserMutex));
	pDec->nParser = 0;
	if (pDec->nWaiter)
	{
		pthread_mutex_unlock(&(pDec->parserMutex));
		pthread_cond_signal(&(pDec->parserCond));
	}
	else
	{
		pthread_mutex_unlock(&(pDec->parserMutex));
	}

	if (vopCtxt->nMBCount > 0)
	{
		fpReconMB = vopCtxt->fpReconMB;
		pMBPos = vopCtxt->pMBPos;
		vDecLocal.scanType = vopCtxt->scanType;
		vDecLocal.blockptr = vopCtxt->blockptr;
		vDecLocal.blockLen = vopCtxt->blockLen;
		do
		{
			(*fpReconMB++)(pDec, &vDecLocal, *pMBPos++);
			vDecLocal.scanType += 6;
			vDecLocal.blockptr += 6*64;
			vDecLocal.blockLen += 6;
		}while (--vopCtxt->nMBCount);
	}

	return voRC;
}

VO_U32 MBrH263PVOP(MBRow_t *pMBr)
{
	VO_MPEG4_DEC *pDec = pMBr->pDec;
	VO_MPEG4_VOP_CONTEXT *vopCtxt = pMBr->vopCtxt;
	VO_U32 voRC = VO_ERR_NONE;
	VO_S32 *BckMV = vopCtxt->MVBack;
	VO_S32 *FwdMV = vopCtxt->MVFwd;
	FuncReconMB* fpReconMB = vopCtxt->fpReconMB;
	VO_S32 *pMBPos = vopCtxt->pMBPos;
	VO_S32 j, nMBPos=0, mcbpc, bNotCode;
	VO_MPEG4_DEC_LOCAL vDecLocal;

	vopCtxt->nMBCount = 0;
	vDecLocal.mc_data = vopCtxt->mc_data;
	vDecLocal.gmcmb = 0;
	vDecLocal.bFieldDct = 0;
	vDecLocal.scanType = vopCtxt->scanType;
	vDecLocal.blockptr = vopCtxt->blockptr;
	vDecLocal.blockLen = vopCtxt->blockLen;
	vDecLocal.MVBack = BckMV;
	vDecLocal.MVFwd = FwdMV;

	pthread_mutex_lock(&(pDec->parserMutex));
	if (pDec->nParser)
	{
		++pDec->nWaiter;
		do 
		{
			pthread_cond_wait(&(pDec->parserCond), &(pDec->parserMutex));
		} while (pDec->nParser);
		--pDec->nWaiter;
	}
	pDec->nParser = 1;
	pthread_mutex_unlock(&(pDec->parserMutex));

	if (pDec->nParserErr != VO_ERR_NONE || !pMBr->runCMD)
	{
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		if (pDec->nWaiter)
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
			pthread_cond_signal(&(pDec->parserCond));
		}
		else
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
		}
		return pDec->nParserErr;
	}

	nMBPos = pDec->nParserMBPos + 1;
	if (nMBPos + MB_X-pDec->nMBWidth >= pDec->nPosEnd)
	{
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 0;
		if (pDec->nWaiter)
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
			pthread_cond_signal(&(pDec->parserCond));
		}
		else
		{
			pthread_mutex_unlock(&(pDec->parserMutex));
		}
		return voRC;
	}
	vDecLocal.pMBQuant = pDec->pParserMBQuant;		

	for (;nMBPos<pDec->nPosEnd;nMBPos+=MB_X-pDec->nMBWidth) {
		VO_S32* mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
		for (;POSX(nMBPos)<pDec->nMBWidth;++nMBPos,mv+=4) {
			VO_S32 nYMBPos = POSY(nMBPos), nXMBPos = POSX(nMBPos);

			UPDATE_CACHE(pDec);

			if (EofBits(pDec))
			{
				pDec->nParserErr = VO_ERR_INPUT_BUFFER_SMALL;
				pthread_mutex_lock(&(pDec->parserMutex));
				pDec->nParser = 0;
				if (pDec->nWaiter)
				{
					pthread_mutex_unlock(&(pDec->parserMutex));
					pthread_cond_signal(&(pDec->parserCond));
				}
				else
				{
					pthread_mutex_unlock(&(pDec->parserMutex));
				}
				return pDec->nParserErr;
			}

			if (SHOW_BITS(pDec,16) == 0){
				if(VO_ERR_NONE != Resync(pDec, &nMBPos))
				{
					pDec->nParserErr = VO_ERR_DEC_MPEG4_P_FRAME;
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					if (pDec->nWaiter)
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
						pthread_cond_signal(&(pDec->parserCond));
					}
					else
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
					}
					return pDec->nParserErr;
				}

				nYMBPos = POSY(nMBPos);
				nXMBPos = POSX(nMBPos);
				//mv & pMBQuant update, ----by harry, 2011,11,8
				mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
				vDecLocal.pMBQuant = pDec->quantrow + nXMBPos;
			}
			*pMBPos = nMBPos;

			/*TBD package the initial process*/

			pDec->nMBError = 0;

			//stream: 24bits available
			do{// stuffing mb type
				UPDATE_CACHE(pDec);
				if((bNotCode = GetBits(pDec, 1))){
					break;
				}
				mcbpc = GetMCBPC_P(pDec); // mcbpc
			}while(mcbpc < 0);

			if (!bNotCode) {         //if coded

//				VO_S32 mcbpc = GetMCBPC_P(pDec); // mcbpc
				VO_S32 mb_type = mcbpc & 7;
				mcbpc = (mcbpc >> 4) & 3;

				//stream: 14bits available

				if (mb_type >= INTRA)
				{
					pDec->frameMap[nMBPos] = (VO_U8)pDec->nCurFrameMap;

					if (pDec->aic)
						pDec->bACPred = GetBits(pDec,1); //todo: aic
					else
						pDec->bACPred = 0;

					pDec->nCBP = (GetCBPY(pDec) << 2) | mcbpc;

					//stream: 7bits available
					//DPRINTF(ff_debug, "CBP = %d \n",pDec->nCBP);

					if (mb_type == INTRA_Q){
						VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
						pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
					}

					//stream: 5bits available

					mv[3] = mv[2] = mv[1] = mv[0] = 0;
					*fpReconMB = ReconIntraMB;
					H263DecIntraMB(pDec, nMBPos, &vDecLocal);
				}
				else
				{
					pDec->frameMap[nMBPos] = (VO_U8)(pDec->nCurFrameMap|RESCUE); // set rescue needed flag

					pDec->nCBP = ((15-GetCBPY(pDec)) << 2) | mcbpc;
					//DPRINTF(ff_debug, "CBP = %d \n",pDec->nCBP);

					//stream: 8bits available

					// we will use mv[4],mv[5] for temporary purposes
					// in the next macroblock it will be overwrite (mv incremented by 4)

					switch (mb_type) 
					{
					case INTER4V:
						BckMV[0] = 1;
						FwdMV[0] = 0;
						for (j = 0; j < 4; j++) {
							mv[j] = GetMV(pDec->nFcode,GetPMV(j,nMBPos,0,pDec),pDec, 0);
							BckMV[j+1] = CorrectMV(mv[j], pDec, nXMBPos, nYMBPos);

						}

						/*TBD Calculate chroma mv*/
						BckMV[5] = GetChromaMV4(BckMV+1, pDec->qpel);

						break;

					case INTER_Q:
						{
							VO_S32 q = pDec->nQuant + DQtab[GetBits(pDec,2)]; //DQtab[dquant]
							pDec->nQuant = q<1 ? 1: (q>31 ? 31:q);
						}

					default: //case INTER:

						BckMV[0] = 1;
						FwdMV[0] = 0;
						mv[3] = mv[2] = mv[1] = mv[0] = 
							GetMV(pDec->nFcode,GetPMV(0,nMBPos,0,pDec),pDec, 0);

						BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] = CorrectMV( mv[0], pDec, nXMBPos, nYMBPos);

						BckMV[5] = GetChromaMV(BckMV[1], pDec->qpel);

						break;
					}
					*fpReconMB = ReconInterMB;
					H263DecInterMB(pDec, nMBPos, &vDecLocal);
				}
#if ENABLE_ERROR_CONCEAL
				if(pDec->nMBError){
					pDec->nFrameError++;
//					if(pDec->nMBError& CONCEAL_ERROR){
// 						*fpReconMB = ReconCopyMB;					
//					}
				}
#endif
			}else {
				// not coded macroblock
				VO_S32 n = pDec->frameMap[nMBPos];
				pDec->frameMap[nMBPos] = (VO_U8)(n|RESCUE);
				mv[3] = mv[2] = mv[1] = mv[0] = 0;
				*fpReconMB = ReconCopyMB;
			}
#if ENABLE_DEBLOCK
			pDec->pCurFrame->pQuant[nYMBPos * pDec->nMBWidth + nXMBPos] = pDec->nQuant;
#endif
			*(vDecLocal.pMBQuant++) = pDec->nQuant;

			//condition of continue parser
			++vopCtxt->nMBCount;
			if (vopCtxt->nMBCount < pDec->nMpeg4ThdMinMB || (vopCtxt->nMBCount < pDec->nMpeg4ThdMaxMB && pDec->nWaiter == 0) )
			{
				//parser can go on
				++fpReconMB;
				++pMBPos;
				vDecLocal.scanType += 6;
				vDecLocal.blockptr += 6*64;
				vDecLocal.blockLen += 6;
				vDecLocal.MVBack = BckMV += 6;
				vDecLocal.MVFwd  = FwdMV += 6;
			}
			else
			{
				pDec->nParserErr = VO_ERR_NONE;
				pDec->nParserMBPos = nMBPos;
				pDec->pParserMBQuant = vDecLocal.pMBQuant;
				pthread_mutex_lock(&(pDec->parserMutex));
				pDec->nParser = 0;
				if (pDec->nWaiter)
				{
					pthread_mutex_unlock(&(pDec->parserMutex));
					pthread_cond_signal(&(pDec->parserCond));
				}
				else
				{
					pthread_mutex_unlock(&(pDec->parserMutex));
				}

				fpReconMB = vopCtxt->fpReconMB;
				pMBPos = vopCtxt->pMBPos;
				vDecLocal.scanType = vopCtxt->scanType;
				vDecLocal.blockptr = vopCtxt->blockptr;
				vDecLocal.blockLen = vopCtxt->blockLen;
				vDecLocal.MVBack = vopCtxt->MVBack;
				vDecLocal.MVFwd = vopCtxt->MVFwd;
				do 
				{
					(*fpReconMB++)(pDec, &vDecLocal, *pMBPos++);
					vDecLocal.scanType += 6;
					vDecLocal.blockptr += 6*64;
					vDecLocal.blockLen += 6;
					vDecLocal.MVBack += 6;
					vDecLocal.MVFwd  += 6;
				} while (--vopCtxt->nMBCount);

				fpReconMB = vopCtxt->fpReconMB;
				pMBPos = vopCtxt->pMBPos;
				vDecLocal.scanType = vopCtxt->scanType;
				vDecLocal.blockptr = vopCtxt->blockptr;
				vDecLocal.blockLen = vopCtxt->blockLen;
				BckMV = vDecLocal.MVBack = vopCtxt->MVBack;
				FwdMV = vDecLocal.MVFwd = vopCtxt->MVFwd;

				//wait & update (nMBPos, mv) pMBQuant?
				pthread_mutex_lock(&(pDec->parserMutex));
				if (pDec->nParser)
				{
					++pDec->nWaiter;
					do 
					{
						pthread_cond_wait(&(pDec->parserCond), &(pDec->parserMutex));
					} while (pDec->nParser);
					--pDec->nWaiter;
				}
				pDec->nParser = 1;
				pthread_mutex_unlock(&(pDec->parserMutex));

				if (pDec->nParserErr != VO_ERR_NONE)
				{
					pthread_mutex_lock(&(pDec->parserMutex));
					pDec->nParser = 0;
					if (pDec->nWaiter)
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
						pthread_cond_signal(&(pDec->parserCond));
					}
					else
					{
						pthread_mutex_unlock(&(pDec->parserMutex));
					}
					return pDec->nParserErr;
				}

				nMBPos = pDec->nParserMBPos;
				vDecLocal.pMBQuant = pDec->pParserMBQuant;
				mv = &pDec->mv_buf[(nMBPos & pDec->mv_bufmask)*4];
			}
		}
		vDecLocal.pMBQuant = pDec->quantrow;
	}

	pDec->nParserErr = VO_ERR_NONE;
	pDec->nParserMBPos = nMBPos - MB_X + pDec->nMBWidth - 1;	//let other threads finish the MB scanning
	pthread_mutex_lock(&(pDec->parserMutex));
	pDec->nParser = 0;
	if (pDec->nWaiter)
	{
		pthread_mutex_unlock(&(pDec->parserMutex));
		pthread_cond_signal(&(pDec->parserCond));
	}
	else
	{
		pthread_mutex_unlock(&(pDec->parserMutex));
	}

	if (vopCtxt->nMBCount > 0)
	{
		fpReconMB = vopCtxt->fpReconMB;
		pMBPos = vopCtxt->pMBPos;
		vDecLocal.scanType = vopCtxt->scanType;
		vDecLocal.blockptr = vopCtxt->blockptr;
		vDecLocal.blockLen = vopCtxt->blockLen;
		vDecLocal.MVBack = vopCtxt->MVBack;
		vDecLocal.MVFwd = vopCtxt->MVFwd;
		do
		{
			(*fpReconMB++)(pDec, &vDecLocal, *pMBPos++);
			vDecLocal.scanType += 6;
			vDecLocal.blockptr += 6*64;
			vDecLocal.blockLen += 6;
			vDecLocal.MVBack += 6;
			vDecLocal.MVFwd  += 6;
		}while (--vopCtxt->nMBCount);
	}

	return voRC;
}

static THREAD_FUNCTION MBrFunc(void *pArg)
{
	VO_MPEG4_VOP_CONTEXT vopCtxt = {0};
	THREAD_FUNCTION_RETURN exitCode = 0;
	MBRow_t *pMBr = (MBRow_t *)pArg;
	VO_MPEG4_DEC *pDec = pMBr->pDec;
	VO_U32 voRC = VO_ERR_NONE;
	pMBr->vopCtxt = &vopCtxt;

//	assert(pMBr!=NULL);
	if (pMBr == NULL)
	{
		pMBr->bRunning = 0;
		return (THREAD_FUNCTION_RETURN)(-1);
	}

	vopCtxt.bFieldDct = (VO_S8*) MallocMem(NULL, 0, pDec->nVopCtxtSize, CACHE_LINE);
	if(!vopCtxt.bFieldDct)
	{
		pMBr->bRunning = 0;
		return (THREAD_FUNCTION_RETURN)(-1);
	}

	SetMem(NULL, 0, (VO_U8*)vopCtxt.bFieldDct, 0, pDec->nVopCtxtSize);
	vopCtxt.gmcmb = vopCtxt.bFieldDct + pDec->nMpeg4ThdMaxMB;
	vopCtxt.MVBack = (VO_S32 *)(((VO_U32)(vopCtxt.gmcmb + pDec->nMpeg4ThdMaxMB + 3))&(~3));
	vopCtxt.MVFwd = vopCtxt.MVBack + pDec->nMpeg4ThdMaxMB*6;
	vopCtxt.fpReconMB = (FuncReconMB *)(vopCtxt.MVFwd + pDec->nMpeg4ThdMaxMB*6);
	vopCtxt.scanType = (VO_S8 *)(vopCtxt.fpReconMB + pDec->nMpeg4ThdMaxMB);
	vopCtxt.blockLen = vopCtxt.scanType + pDec->nMpeg4ThdMaxMB*6;
	vopCtxt.blockptr = (VO_S16 *)(vopCtxt.blockLen + pDec->nMpeg4ThdMaxMB*6);
	vopCtxt.pMBPos = (VO_S32 *)(vopCtxt.blockptr + pDec->nMpeg4ThdMaxMB*6*64);
	vopCtxt.mc_data = (VO_U8 *)(vopCtxt.pMBPos + pDec->nMpeg4ThdMaxMB);

//	pMBr->bRunning = 1;

// 	printf("MBr start\n");
	while (pMBr->runCMD)
	{
// 		printf("wait for ready\n");
		pthread_mutex_lock(&(pDec->readyMutex));
		while (!pMBr->fpVOP && pMBr->runCMD)
		{
			pthread_cond_wait(&(pDec->readyCond), &(pDec->readyMutex));
		}
		pthread_mutex_unlock(&(pDec->readyMutex));
// 		printf("ready\n");

		if (!pMBr->runCMD)
		{
			break;
		}
// 		printf("start vop\n");
		voRC = pMBr->fpVOP(pMBr);
		pMBr->fpVOP = NULL;

#if USE_WHILE_FORVER
		pthread_mutex_lock(&(pDec->finishMutex));
		++pDec->nFinish;
		pthread_mutex_unlock(&(pDec->finishMutex));
#else
		pthread_mutex_lock(&(pDec->finishMutex));
		++pDec->nFinish;
		if (pDec->bEndVop)
		{
			pthread_mutex_unlock(&(pDec->finishMutex));
			pthread_cond_signal(&(pDec->finishCond));
		}
		else
		{
			pthread_mutex_unlock(&(pDec->finishMutex));
		}
// 		VO_ATOMIC_Inc(&pDec->nFinish);
#endif
//  	printf("end vop\n");
	}
//ExitMBrFunc:

	FreeMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, vopCtxt.bFieldDct);

	pMBr->bRunning = 0;
// 	if (pMBr->nMBPos == -1)
// 	{
		exitCode = 0;
// 	}
// 	else
// 	{
// 		exitCode = -1;
// 	}
	return exitCode;
}

VO_U32 InitialMBThread(VO_MPEG4_DEC * VOCONST pDec)
{
	VOCONST VO_S32 nSubThd = pDec->nMpeg4Thd - 1;
	VO_S32 i;
	MBRow_t *pMBr = NULL;
	pDec->MBr = (MBRow_t *) MallocMem(NULL, 0, nSubThd*sizeof(MBRow_t), CACHE_LINE);

	if (pDec->MBr == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}
	pMBr = pDec->MBr;

	if (pthread_mutex_init(&(pDec->parserMutex), NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_mutex_init(&(pDec->readyMutex), NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_mutex_init(&(pDec->finishMutex), NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_cond_init(&(pDec->parserCond), NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_cond_init(&(pDec->readyCond), NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_cond_init(&(pDec->finishCond), NULL))
	{
		return VO_ERR_FAILED;
	}

	for (i = nSubThd; i != 0; --i,++pMBr)
	{
		pMBr->pDec = pDec;
		pMBr->fpVOP = NULL;
		pMBr->runCMD = 1;
		if (pthread_create(&(pMBr->thread_id), NULL, MBrFunc, pMBr))
		{
			return VO_ERR_FAILED;
		}
		pMBr->bRunning = 1;
	}

	return VO_ERR_NONE;
}

VO_U32 ReleaseMBThread(VO_MPEG4_DEC* pDec)
{
	MBRow_t *pMBr = pDec->MBr;
	VOCONST VO_S32 nSubThd = pDec->nMpeg4Thd - 1;
	VO_U32 voRC = VO_ERR_NONE;
	VO_S32 i;

	for (i = nSubThd; i != 0; --i,++pMBr)
	{
		pMBr->runCMD = 0;
	}

	pthread_mutex_lock(&pDec->readyMutex);
	pthread_cond_broadcast(&pDec->readyCond);
#if defined(VOWINCE) || defined(WIN32)
	{
		VO_U32 i;
		for (i = nSubThd; i != 0; --i)
		{
			pthread_cond_signal(&pDec->readyCond);
		}
	}
#endif
	pthread_mutex_unlock(&pDec->readyMutex);

	for (i = nSubThd, pMBr = pDec->MBr; i != 0; --i,++pMBr)
	{
		while (pMBr->bRunning)
		{
			thread_sleep(1);
		}
		pthread_join(pMBr->thread_id, NULL);
		pMBr->thread_id = (pthread_t)NULL;
	}

	if (pthread_cond_destroy(&(pDec->finishCond)))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_cond_destroy(&(pDec->readyCond)))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_cond_destroy(&(pDec->parserCond)))
	{
		voRC |= VO_ERR_FAILED;
	}

	if (pthread_mutex_destroy(&(pDec->finishMutex)))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_mutex_destroy(&(pDec->readyMutex)))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_mutex_destroy(&(pDec->parserMutex)))
	{
		voRC |= VO_ERR_FAILED;
	}

	FreeMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->MBr);
	pDec->MBr = NULL;

	return voRC;
}

VO_U32 ResetMBThread(VO_MPEG4_DEC* pDec)
{
	VO_S32 i;
	MBRow_t *pMBr = pDec->MBr;
	for (i = pDec->nMpeg4Thd-1; i != 0; --i,++pMBr)
	{
// 		pMBr->nMBPos = -1;
		pMBr->vopCtxt->nMBCount = 0;
	}
	return VO_ERR_NONE;
}

#if 0
VO_U32 SetMBThreadBuff(VOCONST VO_MPEG4_DEC *VOCONST pDec)
{
	VO_S32 i;
	FuncReconMB* fpReconMB = pDec->fpReconMB;
	VO_S32 *pMBPos = pDec->pMBPos;
	VO_U8* mc_data = pDec->mc_data;
	VO_S8 *gmcmb = pDec->gmcmb;
	VO_S8 *bFieldDct = pDec->bFieldDct;
	VO_S8 *scanType = pDec->scanType;
	VO_S16* blockptr = pDec->blockptr;
	VO_S8* blockLen = pDec->blockLen;
	VO_S32* MVBack = pDec->MVBack;
	VO_S32* MVFwd = pDec->MVFwd;
	MBRow_t *pMBr = pDec->MBr;

	for (i = SUB_THREAD_NUM; i != 0; --i,++pMBr)
	{
		VO_MPEG4_VOP_CONTEXT *vopCtxt = &(pMBr->vopCtxt);
		vopCtxt->nMBCount = 0;
		vopCtxt->fpReconMB = fpReconMB += MAX_MB_TASK_NUM;
		vopCtxt->pMBPos = pMBPos += MAX_MB_TASK_NUM;
		vopCtxt->mc_data = mc_data += BLOCK_SIZE*BLOCK_SIZE;
		vopCtxt->gmcmb = gmcmb += MAX_MB_TASK_NUM;
		vopCtxt->bFieldDct = bFieldDct += MAX_MB_TASK_NUM;
		vopCtxt->scanType = scanType += 6*MAX_MB_TASK_NUM;
		vopCtxt->blockptr = blockptr += 6*64*MAX_MB_TASK_NUM;
		vopCtxt->blockLen = blockLen += 6*MAX_MB_TASK_NUM;
		vopCtxt->MVBack = MVBack += 6*MAX_MB_TASK_NUM;
		vopCtxt->MVFwd = MVFwd += 6*MAX_MB_TASK_NUM;
	}
	return VO_ERR_NONE;
}
#endif
VO_U32 SetMBThreadVOPFunc(VO_MPEG4_DEC* pDec, VO_U32 (*fpVOP)(struct _MBRow_t *))
{
	VO_S32 i;
	MBRow_t *pMBr = pDec->MBr;
//	printf("before set ready\n");
	pthread_mutex_lock(&(pDec->readyMutex));
	for (i = pDec->nMpeg4Thd-1; i != 0; --i,++pMBr)
	{
		pMBr->fpVOP = fpVOP;
	}
	pthread_mutex_unlock(&(pDec->readyMutex));
//	printf("after set ready\n");

#ifdef WIN32
// #if (_WIN32_WINNT < 0x0600)	//only for WIN XP
	for (i = pDec->nMpeg4Thd-1; i != 0; --i)
	{
		pthread_cond_signal(&(pDec->readyCond));
	}	
// #endif
#endif
// 	printf("post ready\n");
	pthread_cond_broadcast(&(pDec->readyCond));	//WIN XP not work
	return VO_ERR_NONE;
}
//#endif

void WaitForParser(VO_MPEG4_DEC* pDec)
{
	pthread_mutex_t * pMutex = &(pDec->parserMutex);
waitForParser:
	while (pDec->nParser);
	pthread_mutex_lock(pMutex);
	if (pDec->nParser)
	{
		pthread_mutex_unlock(pMutex);
		goto waitForParser;
	}
	pDec->nParser = 1;
	pthread_mutex_unlock(pMutex);
}

void WaitForSubThdFinish(VO_MPEG4_DEC* pDec)
{
	const VO_S32 nSubThd = (VO_S32)pDec->nMpeg4Thd-1;
	pthread_mutex_t * const pMutex = &(pDec->finishMutex);
waitForSubThdFinish:
	while (pDec->nFinish < nSubThd);
	pthread_mutex_lock(pMutex);
	if (pDec->nFinish < nSubThd)
	{
		pthread_mutex_unlock(pMutex);
		goto waitForSubThdFinish;
	}
	pDec->nFinish = 0;
	pthread_mutex_unlock(pMutex);
}
