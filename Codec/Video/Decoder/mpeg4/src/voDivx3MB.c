/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
 
//#include "stdafx.h"
#include "voDivx3Haff.h"
#include "voMpeg4MB.h"
#include "voMpegReadbits.h"

#include "voMpeg4Dec.h"
#include "voMpegMC.h"
#include "voMpegIdct.h"

#ifdef VODIVX3

#include "voMpeg4ACDC.h"
#include "voMpegBuf.h"

static INLINE VO_S32 mvchroma3(VO_S32 v)
{
	v |= ((v & 0x3)!=0) << 1;		//adjust dx
	v |= ((v & 0x30000)!=0) << 17;	//adjust dy

	v >>= 1; //shift

	//replace dx sign bit with old signed bit
	v &= ~0x8000;			
	v |= (v & 0x4000) << 1;
	
	return v;
}

// code = 1:last|6:run|5:level

static VO_S32 vld_block2_inter( VO_MPEG4_DEC* pDec, VO_S32 n, const VO_U8 *scan, VO_S32 len, VO_S32 rundiff,idct_t* block)
{
	const VO_U16 *table = pDec->rl_vlc[n];
	const VO_U8 *maxtable = pDec->rlmax_vlc[n];
	VO_S32 q_scale = (pDec->nQuant) << 1;
	VO_S32 q_add = (pDec->nQuant-1)|1;

    do
	{
		VO_S32 code, level;
		UPDATE_CACHE(pDec); 

        code = GetDivx3VLC(pDec, table); // max 15bits

		if (code != ESCAPE) {
			level = code & 31;
			len += code >> 5; // last|run
		} else {
			UPDATE_CACHE(pDec); 

			if (GetBits(pDec, 1)) {
				// first escape
				code = GetDivx3VLC(pDec, table); // max 15bits
				level = code & 31;
				code >>= 5;
				len += code; // last|run
				level += maxtable[code]; //maxlevel
			} else if (GetBits(pDec, 1)) {
				// second escape
				code = GetDivx3VLC(pDec, table); // max 15bits
				len += code >> 5;
				code &= 31;
				level = code;
				code += (len >> 6) << 5; // add (last bit << 5)
				len += maxtable[128+code] + rundiff; //maxrun
			} else {
				// third escape
				code = GetBits(pDec,7+8);
				len += code >> 8; //last|run
				level = (code << 24) >> 24;
				level *= q_scale;
				if (level > 0)
					level += q_add;
				else
					level -= q_add;

				block[scan[len & 63]] = (idct_t)level;
				continue;
			}
		}

		level *= q_scale;
		level += q_add;

		if (GetBits(pDec, 1)) 
			level = -level;

		block[scan[len & 63]] = (idct_t)level;

	} while (++len < 64);

	return len - 64;
}
static VO_S32 vld_block2( VO_MPEG4_DEC* pDec, VO_S32 n, const VO_U8 *scan, VO_S32 len, VO_S32 rundiff,idct_t* block)
{
	const VO_U16 *table = pDec->rl_vlc[n];
	const VO_U8 *maxtable = pDec->rlmax_vlc[n];
	VO_S32 q_scale = (pDec->nQuant) << 1;
	//VO_S32 q_add = (pDec->nQuant-1)|1;
//	idct_t* block = pDec->blockptr;

    do
	{
		VO_S32 code, level;

		UPDATE_CACHE(pDec); 

        code = GetDivx3VLC(pDec, table); // max 15bits

        if (code == ESCAPE) {

			UPDATE_CACHE(pDec); 

			if (!GetBits(pDec, 1)) {
				if (!GetBits(pDec, 1)) {
                    // third escape
					code = GetBits(pDec,7+8);
                    len += code >> 8; //last|run
                    level = (code << 24) >> 24;
					level *= q_scale;
/*
					if (level > 0)
						level += q_add;
					else
						level -= q_add;
*/
					block[scan[len & 63]] = (idct_t)level;
					continue;

                } else {
                    // second escape
                    code = GetDivx3VLC(pDec, table); // max 15bits
					len += code >> 5;
					code &= 31;
					level = code;
					code += (len >> 6) << 5; // add (last bit << 5)
					len += maxtable[128+code] + rundiff; //maxrun
                }
            } else {
                // first escape
                code = GetDivx3VLC(pDec, table); // max 15bits
				level = code & 31;
				code >>= 5;
				len += code; // last|run
                level += maxtable[code]; //maxlevel
            }

        } else {
			level = code & 31;
			len += code >> 5; // last|run
        }

		level *= q_scale;
//		level += q_add;

		if (GetBits(pDec, 1)) 
			level = -level;

		block[scan[len & 63]] = (idct_t)level;

	} while (++len < 64);

	return len - 64;
}

static void DIVX3DecIntraMB( VO_MPEG4_DEC *pDec, VO_S32 pos , VO_MPEG4_DEC_LOCAL *vDecLocal)
{
	VO_S32 j,i,q_add;
	VO_S8 *blockLen = vDecLocal->blockLen;
	VO_S8 *blockscanType = vDecLocal->scanType;
	idct_t *blockptr = vDecLocal->blockptr; 

	for (j = 0; j < 6; j++) {
		
		idct_t *dc_addr;
		VO_S32 subpos;
		VO_S32 dc_scaler,nDctDCDiff;
//		VO_S32 len,scanType;
		VO_S8 *len = blockLen+j, *scanType = blockscanType+j;
		idct_t *block = blockptr + (j<<6);

		//clearblock(block);	

		UPDATE_CACHE(pDec); 

		//stream: 24bit available

		if (j < 4) {
			nDctDCDiff = GetDivx3VLC(pDec, pDec->dc_lum_vlc); // max 26bits
			dc_scaler = pDec->dc_lum_scaler;
			// convert 1x1 -> 2x2 (accodring to block number)
			subpos = pos - pDec->slice_pos;
			subpos = 2*POSX(subpos) + (POSY(subpos) << (MB_X2+2));
			subpos += (j & 1) + ((j & 2) << MB_X2);
		}
		else {
			nDctDCDiff = GetDivx3VLC(pDec, pDec->dc_chr_vlc); // max 25bits
			dc_scaler = pDec->dc_chr_scaler;
			subpos = pos - pDec->slice_pos;
		}

		UPDATE_CACHE(pDec); 

		if (nDctDCDiff == 119) { // dcmax
			nDctDCDiff = GetBits(pDec,8);
			if (GetBits(pDec, 1))
				nDctDCDiff = -nDctDCDiff;
		} else 
			if (nDctDCDiff && GetBits(pDec, 1))
				nDctDCDiff = -nDctDCDiff;

		//DEBUG_MSG1(DEBUG_VCODEC,T("block[0] %i"), nDctDCDiff);
//		DPRINTF(FF_DUMP_ALL,"dc_diff=%d\n",nDctDCDiff);

		// dc reconstruction, prediction direction
		dc_addr = dc_recon(pDec, j, subpos, dc_scaler );
		// dc add
		*block = *dc_addr = (idct_t)(*dc_addr + nDctDCDiff * dc_scaler);
//		DPRINTF(FF_DUMP_ALL,"block[0]=%d\n",block[0]);
		if (pDec->bACPred) {

			if (pDec->predict_dir == TOP)
				*scanType = IDCTSCAN_ALT_HORI;
			else
				*scanType = IDCTSCAN_ALT_VERT;
		}
		else
			*scanType = IDCTSCAN_ZIGZAG;

		*len = 1;
		if ((pDec->nCBP << (26+j)) < 0) 
			*len = (VO_S8)vld_block2(pDec,j>=4,scanTab[*scanType],*len,0, block);
		
		// ac reconstruction
		ac_recon(pDec, vDecLocal, block, j, subpos);
/**/
		q_add = (pDec->nQuant-1)|1;

		for (i = 1; i < 64; i++) 
		{
			if(block[i])
			{
				if(block[i] > 0)
					block[i] = block[i] + (idct_t)q_add;
				else
					block[i] = block[i] - (idct_t)q_add;
			}
		}
	}
}

static void DIVX3DecInterMB( VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos )
{
	VO_S32 j;
	VO_S8 *blockLen = vDecLocal->blockLen;
	idct_t *blockptr = vDecLocal->blockptr; 

	// texture decoding add
	for (j = 0; j < 6; j++) {
		VO_S8 *len = blockLen+j;
		idct_t *block = blockptr + (j<<6);
		*len = 0;
		if ((pDec->nCBP << (26+j)) < 0) { //if coded

			//clearblock(block);	
			*len = (VO_S8)vld_block2_inter(pDec,1,scanTab[IDCTSCAN_ZIGZAG],*len,1, block);
		}
	}
}

static VOCONST VO_U8 y_dc_scale_table[32]={
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
    0, 8, 8, 8, 8,10,12,14,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39
};
static VOCONST VO_U8 c_dc_scale_table[32]={
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
    0, 8, 8, 8, 8, 9, 9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,18,19,19,20,20,21,21,22
};

static void dc_scaler( VO_MPEG4_DEC *pDec )
{
    VO_S32 scale = pDec->nQuant;

    if (scale < 1)
        scale = 1;
    else if (scale > 31)
        scale = 31;

	pDec->nQuant = scale;

    pDec->dc_lum_scaler = y_dc_scale_table[scale];
    pDec->dc_chr_scaler = c_dc_scale_table[scale];
}

static VO_S32 getMV2(VO_S32 prev, VO_MPEG4_DEC* pDec)
{
    VO_S32 code, mx, my;

	UPDATE_CACHE(pDec);

    code = GetDivx3VLC(pDec,pDec->mv_vlc); // max 17bits

    if (code == ESCAPE) {
		UPDATE_CACHE(pDec);
        mx = GetBits(pDec,6);
        my = GetBits(pDec,6);
    } else {
        mx = code & 63;
        my = code >> 6;
    }

    mx += MVX(prev, 0) - 32;
    my += MVY(prev, 0) - 32;

    if (mx <= -64)
        mx += 64;
    else 
	if (mx >= 64)
        mx -= 64;

    if (my <= -64)
        my += 64;
    else 
	if (my >= 64)
        my -= 64;

	return MAKEMV(mx,my);
}

static void ext_header( VO_MPEG4_DEC* pDec )
{
	pDec->nFlipRounding = 0;
	if (!EofBits(pDec))
	{
		UPDATE_CACHE(pDec);
		GetBits(pDec,5); //fps
		//TBD
		pDec->ByteRate = GetBits(pDec,11)*128;
		pDec->nFlipRounding = GetBits(pDec, 1);
	}
}

static void DIVX3RescuePredict(VO_MPEG4_DEC* pDec, VO_S32 pos, VO_S32 nMBPosA) 
{
	VO_S32 lumpos = 2*POSX(pos) + (POSY(pos) << (MB_X2+2));

	if (nMBPosA>=MB_X+1 && (pDec->frameMap[nMBPosA-MB_X-1] & RESCUE)) {
		// rescue -A- DC value
		pDec->dc_lum[(lumpos-MB_X*2) & DC_LUM_MASK] = 1024;
		pDec->dc_chr[0][(pos-MB_X) & DC_CHR_MASK] = 1024;
		pDec->dc_chr[1][(pos-MB_X) & DC_CHR_MASK] = 1024;
	}

	// left
	if (nMBPosA>=1 && (pDec->frameMap[nMBPosA-1] & RESCUE)) {
		// rescue -B- DC values
		pDec->dc_lum[(lumpos) & DC_LUM_MASK] = 1024;
		pDec->dc_lum[(lumpos+MB_X*2) & DC_LUM_MASK] = 1024;
		pDec->dc_chr[0][pos & DC_CHR_MASK] = 1024;
		pDec->dc_chr[1][pos & DC_CHR_MASK] = 1024;

		if (pDec->bACPred) {
			// rescue -B- AC values
			clear32(pDec->ac_left_lum);
			clear32(pDec->ac_left_chr);
		}
	}

	// top
	if (nMBPosA>=MB_X && (pDec->frameMap[nMBPosA-MB_X] & RESCUE)) {

		// rescue -C- DC values
		pDec->dc_lum[(lumpos+1-MB_X*2) & DC_LUM_MASK] = 1024;
		pDec->dc_lum[(lumpos+2-MB_X*2) & DC_LUM_MASK] = 1024;
		pDec->dc_chr[0][(pos+1-MB_X) & DC_CHR_MASK] = 1024;
		pDec->dc_chr[1][(pos+1-MB_X) & DC_CHR_MASK] = 1024;

		if (pDec->bACPred) {
			// rescue -C- AC values
			clear32(pDec->ac_top_lum[lumpos & (MB_X*2-1)]);
			clear32(pDec->ac_top_chr[pos & (MB_X-1)]);
		}
	}
}

// #if ENABLE_MULTI_THREAD
static VO_U32 MBrDIVX3IVOP(MBRow_t *pMBr)
{
	VO_MPEG4_DEC *pDec = pMBr->pDec;
	VO_MPEG4_VOP_CONTEXT *vopCtxt = pMBr->vopCtxt;
	VO_S32 a,b,c,code,pos;
	VO_S8* p;
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
		return VO_ERR_NONE;
	}

	pos = pDec->nParserMBPos + 1;
	if (pos + MB_X-pDec->nMBWidth >= pDec->nPosEnd)
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
		return VO_ERR_NONE;
	}
	vDecLocal.pMBQuant = pDec->pParserMBQuant;		

	for (;pos<pDec->nPosEnd;pos+=MB_X-pDec->nMBWidth) {

		if (pos == pDec->slice_pos + pDec->slice_height*MB_X)
			pDec->slice_pos = pos;

		for (;POSX(pos)<pDec->nMBWidth;++pos) {
			//VO_S32 nYMBPos = POSY(pos), nXMBPos = POSX(pos);

#if  ENABLE_MBINFO
			pDec->mb_info[nYMBPos*pDec->nMBWidth+nXMBPos] = 0;
#endif

			UPDATE_CACHE(pDec);

			if (EofBits(pDec))
			{
				pDec->nFlipRounding = 0;
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
				return VO_ERR_NONE;
			}

			*pMBPos = pos;

			//stream: 24bits available

			code = GetDivx3VLC(pDec, pDec->mb_intra_vlc); // max 13bits

			//predict coded block pattern
			p = &pDec->codedmap[pos+1+MB_X];

			//(0:0)
			a = (p[-1] << 1) & 32;
			b = (p[-1-MB_X] << 3) & 32;
			c = (p[-MB_X] << 2) & 32;

			if (b==c) c=a; 
			code ^= c;

			//(0:1)
			a = (code >> 1) & 16;
			b = (p[-MB_X] << 1) & 16;
			c = (p[-MB_X] << 2) & 16;

			if (b==c) c=a; 
			code ^= c;

			//(1:0)
			a = (p[-1] << 1) & 8;
			b = (p[-1] >> 1) & 8;
			c = (code >> 2) & 8;

			if (b==c) c=a; 
			code ^= c;

			//(1:1)
			a = (code >> 1) & 4;
			b = (code >> 3) & 4;
			c = (code >> 2) & 4;

			if (b==c) c=a; 
			code ^= c;

			*p = (VO_S8)code;
			pDec->nCBP = code;

			pDec->bACPred = GetBits(pDec,1);

			DIVX3DecIntraMB(pDec, pos, &vDecLocal);

			//condition of continue parser
			++vopCtxt->nMBCount;
			if (vopCtxt->nMBCount < pDec->nMpeg4ThdMinMB || (vopCtxt->nMBCount < pDec->nMpeg4ThdMaxMB && pDec->nWaiter == 0) )
			{
				//parser can go on
				++pMBPos;
				vDecLocal.scanType += 6;
				vDecLocal.blockptr += 6*64;
				vDecLocal.blockLen += 6;
			}
			else
			{
				pDec->nParserErr = VO_ERR_NONE;
				pDec->nParserMBPos = pos;
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

				pMBPos = vopCtxt->pMBPos;
				vDecLocal.scanType = vopCtxt->scanType;
				vDecLocal.blockptr = vopCtxt->blockptr;
				vDecLocal.blockLen = vopCtxt->blockLen;
				do 
				{
					ReconIntraMB(pDec, &vDecLocal, *pMBPos++);
					vDecLocal.scanType += 6;
					vDecLocal.blockptr += 6*64;
					vDecLocal.blockLen += 6;
				} while (--vopCtxt->nMBCount);

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
					return VO_ERR_NONE;
				}

				pos = pDec->nParserMBPos;
			}
		}
	}

	pDec->nParserErr = VO_ERR_NONE;
	pDec->nParserMBPos = pos - MB_X + pDec->nMBWidth - 1;	//let other threads finish the MB scanning
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
		pMBPos = vopCtxt->pMBPos;
		vDecLocal.scanType = vopCtxt->scanType;
		vDecLocal.blockptr = vopCtxt->blockptr;
		vDecLocal.blockLen = vopCtxt->blockLen;
		do
		{
			ReconIntraMB(pDec, &vDecLocal, *pMBPos++);
			vDecLocal.scanType += 6;
			vDecLocal.blockptr += 6*64;
			vDecLocal.blockLen += 6;
		}while (--vopCtxt->nMBCount);
	}

	return VO_ERR_NONE;
}

static VO_U32 MBrDIVX3PVOP(MBRow_t *pMBr)
{
	VO_MPEG4_DEC *pDec = pMBr->pDec;
	VO_MPEG4_VOP_CONTEXT *vopCtxt = pMBr->vopCtxt;
	VO_S32 *BckMV = vopCtxt->MVBack;
	VO_S32 *FwdMV = vopCtxt->MVFwd;
	FuncReconMB* fpReconMB = vopCtxt->fpReconMB;
	VO_S32 *pMBPos = vopCtxt->pMBPos;
	VO_S32 pos;
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
		return VO_ERR_NONE;
	}

	pos = pDec->nParserMBPos + 1;
	if (pos + MB_X-pDec->nMBWidth >= pDec->nPosEnd)
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
		return VO_ERR_NONE;
	}
	vDecLocal.pMBQuant = pDec->pParserMBQuant;		

	for (;pos<pDec->nPosEnd;pos+=MB_X-pDec->nMBWidth) {

		VO_S32* mv = &pDec->mv_buf[(pos & pDec->mv_bufmask)*4];

		if (pos == pDec->slice_pos + pDec->slice_height*MB_X)
		{
			pDec->slice_pos = pos;
		}

		for (;POSX(pos)<pDec->nMBWidth;++pos,mv+=4) {
			VO_S32 nYMBPos = POSY(pos), nXMBPos = POSX(pos);

#if  ENABLE_MBINFO
			pDec->mb_info[nYMBPos*pDec->nMBWidth+nXMBPos] = 0;
#endif
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
				return VO_ERR_NONE;
			}

			*pMBPos = pos;

			//stream: 24bits available

			//DEBUG_MSG3(-1,T("%d %d %02X"),POSX(pos),POSY(pos),ShowBits(pDec,8));

			if (pDec->use_coded_bit && GetBits(pDec, 1)) {

				// not coded macroblock
				VO_S32 n = pDec->frameMap[pos];
				pDec->frameMap[pos] = (VO_U8)(n|1); // set rescue needed flag
				mv[3] = mv[2] = mv[1] = mv[0] = 0;
				*fpReconMB = ReconCopyMB;
			}
			else {

				VO_S32 code = GetDivx3VLC(pDec, pDec->mb_inter_vlc); // max 21bits
				pDec->nCBP = code & 0x3F;

				if (code & 0x40) { //inter

					pDec->frameMap[pos] = (VO_U8)(pDec->nCurFrameMap|1); // set rescue needed flag
					mv[3] = mv[2] = mv[1] = mv[0] = 
						getMV2(GetPMV(0,pos,pDec->slice_pos,pDec),pDec);
					BckMV[0] = 1;
					FwdMV[0] = 0;
					BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] =  CorrectMV( mv[0], pDec, nXMBPos, nYMBPos);
					BckMV[5] = mvchroma3(BckMV[1]);
					*fpReconMB = ReconInterMB;

					DIVX3DecInterMB( pDec, &vDecLocal, pos );
				}
				else { //intra

					pDec->frameMap[pos] = (VO_U8)pDec->nCurFrameMap;
					pDec->bACPred = GetBits(pDec,1);

					mv[3] = mv[2] = mv[1] = mv[0] = 0;

					DIVX3RescuePredict(pDec, pos - pDec->slice_pos, pos);  //restore AC_DC values

					*fpReconMB = ReconIntraMB;
					DIVX3DecIntraMB(pDec, pos, &vDecLocal );
				}
			}
			//			DPRINTF(FF_DUMP_ALL,"MBY\n",vDecLocal.pCurY[0]);

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
				pDec->nParserMBPos = pos;
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
					return VO_ERR_NONE;
				}

				pos = pDec->nParserMBPos;
				mv = &pDec->mv_buf[(pos & pDec->mv_bufmask)*4];
			}
		}
	}

	pDec->nParserErr = VO_ERR_NONE;
	pDec->nParserMBPos = pos - MB_X + pDec->nMBWidth - 1;	//let other threads finish the MB scanning
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

	return VO_ERR_NONE;
}

// #endif

//VO_S32 dpos;
VO_U32 DIVX3DecIVOP( VO_MPEG4_DEC *pDec )
{
	VO_MPEG4_VOP_CONTEXT vopCtxt = pDec->vopCtxt;
	VO_S32 a,b,c,code,pos;
	VO_S8* p;
	VO_S32 *pMBPos = vopCtxt.pMBPos;
	VO_MPEG4_DEC_LOCAL vDecLocal;

	vDecLocal.bFieldDct = 0;
	vDecLocal.scanType = vopCtxt.scanType;
	vDecLocal.blockptr = vopCtxt.blockptr;
	vDecLocal.blockLen = vopCtxt.blockLen;

	pDec->nLastRefFrame = pDec->nFrame;
	pDec->nMapOfs = pDec->nFrame;
	SetMem(pDec->vMemSever.pMemOp, pDec->vMemSever.nCodecIdx, pDec->frameMap,0,pDec->nPosEnd); // set all block to current frame
	pDec->pCurFrameMB = pDec->pCurFrame;

	dc_scaler(pDec);

	pDec->slice_pos = 0;

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
		//start MB serials, init/reset all sub-threads if needed
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 1;
		pthread_mutex_unlock(&(pDec->parserMutex));
		pDec->nParserErr = VO_ERR_NONE;
		SetMBThreadVOPFunc(pDec, MBrDIVX3IVOP);
	}
// #endif

//	DPRINTF(ff_debug,"FRAME_%i, @@@@@@@@@@@@@@@@@@@@@\n",pDec->frame);//,FrameCount);
	for (pos=0;pos<pDec->nPosEnd;pos+=MB_X-pDec->nMBWidth) {

		if (pos == pDec->slice_pos + pDec->slice_height*MB_X)
			pDec->slice_pos = pos;

		for (;POSX(pos)<pDec->nMBWidth;++pos) {
			//VO_S32 nYMBPos = POSY(pos), nXMBPos = POSX(pos);
//			DPRINTF(FF_DUMP_ALL,"x = %d y = %d\n",nXMBPos, nYMBPos);

#if  ENABLE_MBINFO
			pDec->mb_info[nYMBPos*pDec->nMBWidth+nXMBPos] = 0;
#endif

			UPDATE_CACHE(pDec);

			if (EofBits(pDec))
			{
				pDec->nFlipRounding = 0;
// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
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
					goto EndOfDIVX3IVOP;
				}
				else
// #else
				{
//					return VO_ERR_INPUT_BUFFER_SMALL;
					return VO_ERR_NONE;	
				}
// #endif
			}
			*pMBPos = pos;

			//DEBUG_MSG3(-1,T("%d %d %02X"),POSX(pos),POSY(pos),ShowBits(pDec,8));

			//stream: 24bits available

			code = GetDivx3VLC(pDec, pDec->mb_intra_vlc); // max 13bits
//			printf("pos = %d, code = %d\n", dpos, code);

	        //predict coded block pattern
			p = &pDec->codedmap[pos+1+MB_X];

			//(0:0)
			a = (p[-1] << 1) & 32;
			b = (p[-1-MB_X] << 3) & 32;
			c = (p[-MB_X] << 2) & 32;

			if (b==c) c=a; 
			code ^= c;
			
			//(0:1)
			a = (code >> 1) & 16;
			b = (p[-MB_X] << 1) & 16;
			c = (p[-MB_X] << 2) & 16;

			if (b==c) c=a; 
			code ^= c;

			//(1:0)
			a = (p[-1] << 1) & 8;
			b = (p[-1] >> 1) & 8;
			c = (code >> 2) & 8;

			if (b==c) c=a; 
			code ^= c;

			//(1:1)
			a = (code >> 1) & 4;
			b = (code >> 3) & 4;
			c = (code >> 2) & 4;

			if (b==c) c=a; 
			code ^= c;

			*p = (VO_S8)code;
			pDec->nCBP = code;

			pDec->bACPred = GetBits(pDec,1);

			DIVX3DecIntraMB(pDec, pos, &vDecLocal);

// #if ENABLE_MULTI_THREAD
			//condition of continue parser
			++vopCtxt.nMBCount;
			if ((vopCtxt.nMBCount < pDec->nMpeg4ThdMaxMB && pDec->nWaiter == 0) || (pDec->nMpeg4Thd > 1 && vopCtxt.nMBCount < pDec->nMpeg4ThdMinMB) )
// #else
// 			if (++vopCtxt.nMBCount < MAX_MB_TASK_NUM)
// #endif
			{
				//parser can go on
				++pMBPos;
				vDecLocal.scanType += 6;
				vDecLocal.blockptr += 6*64;
				vDecLocal.blockLen += 6;
			}
			else
			{
// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
				{
					pDec->nParserErr = VO_ERR_NONE;
					pDec->nParserMBPos = pos;
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
				}
// #endif
				pMBPos = vopCtxt.pMBPos;
				vDecLocal.scanType = vopCtxt.scanType;
				vDecLocal.blockptr = vopCtxt.blockptr;
				vDecLocal.blockLen = vopCtxt.blockLen;
				do 
				{
					ReconIntraMB(pDec, &vDecLocal, *pMBPos++);
					vDecLocal.scanType += 6;
					vDecLocal.blockptr += 6*64;
					vDecLocal.blockLen += 6;
				} while (--vopCtxt.nMBCount);

				pMBPos = vopCtxt.pMBPos;
				vDecLocal.scanType = vopCtxt.scanType;
				vDecLocal.blockptr = vopCtxt.blockptr;
				vDecLocal.blockLen = vopCtxt.blockLen;

// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
				{
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
						goto EndOfDIVX3IVOP;
					}

					pos = pDec->nParserMBPos;
				}
// #endif
			}
		}
	}

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
		pDec->nParserErr = VO_ERR_NONE;
		pDec->nParserMBPos = pos - MB_X + pDec->nMBWidth - 1;	//let other threads finish the MB scanning
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
	}
// #endif
	if (vopCtxt.nMBCount > 0)
	{
		pMBPos = vopCtxt.pMBPos;
		vDecLocal.scanType = vopCtxt.scanType;
		vDecLocal.blockptr = vopCtxt.blockptr;
		vDecLocal.blockLen = vopCtxt.blockLen;
		do
		{
			ReconIntraMB(pDec, &vDecLocal, *pMBPos++);
			vDecLocal.scanType += 6;
			vDecLocal.blockptr += 6*64;
			vDecLocal.blockLen += 6;
		}while (--vopCtxt.nMBCount);
	}

	ext_header(pDec);

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
EndOfDIVX3IVOP:
		//wait all sub-threads finish
#if USE_WHILE_FORVER
		WaitForSubThdFinish(pDec);
#else
		pthread_mutex_lock(&(pDec->finishMutex));
		pDec->bEndVop = 1;
		while (pDec->nFinish < (VO_S32)pDec->nMpeg4Thd - 1)
		{
//			thread_sleep(0);
			pthread_cond_wait(&(pDec->finishCond), &(pDec->finishMutex));
		}
		pDec->nFinish = 0;
		pDec->bEndVop = 0;
		pthread_mutex_unlock(&(pDec->finishMutex));
#endif
// 		printf("end frame\n");
//		return pDec->nParserErr;
	}
// #endif
	return VO_ERR_NONE;
}

VO_U32 DIVX3DecPVOP( VO_MPEG4_DEC *pDec )
{
	VO_MPEG4_VOP_CONTEXT vopCtxt = pDec->vopCtxt;
	VO_S32 *BckMV = vopCtxt.MVBack;
	VO_S32 *FwdMV = vopCtxt.MVFwd;
	FuncReconMB* fpReconMB = vopCtxt.fpReconMB;
	VO_S32 *pMBPos = vopCtxt.pMBPos;
	VO_S32 pos;
	VO_MPEG4_DEC_LOCAL vDecLocal;

	vDecLocal.mc_data = vopCtxt.mc_data;
	vDecLocal.gmcmb = 0;
	vDecLocal.bFieldDct = 0;
	vDecLocal.scanType = vopCtxt.scanType;
	vDecLocal.blockptr = vopCtxt.blockptr;
	vDecLocal.blockLen = vopCtxt.blockLen;
	vDecLocal.MVBack = BckMV;
	vDecLocal.MVFwd = FwdMV;

#if  ENABLE_MBINFO
	pDec->mb_info_s = 1 - pDec->mb_info_s;
	pDec->mb_info = pDec->mbinfo[pDec->mb_info_s];
#endif

	pDec->nCurFrameMap = (pDec->nFrame - pDec->nMapOfs) << 1;
	pDec->nLastRefFrame = pDec->nFrame;
	pDec->pCurFrameMB = pDec->pCurFrame;
	pDec->pRefFrameMB = pDec->pRefFrame;

	dc_scaler(pDec);

	pDec->slice_pos = 0;

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
		//start MB serials, init/reset all sub-threads if needed
		pthread_mutex_lock(&(pDec->parserMutex));
		pDec->nParser = 1;
		pthread_mutex_unlock(&(pDec->parserMutex));
		pDec->nParserErr = VO_ERR_NONE;
		SetMBThreadVOPFunc(pDec, MBrDIVX3PVOP);
	}
// #endif

//	DPRINTF(ff_debug,"FRAME_%i, @@@@@@@@@@@@@@@@@@@@@\n",pDec->frame);//,FrameCount);
	for (pos=0;pos<pDec->nPosEnd;pos+=MB_X-pDec->nMBWidth) {

		VO_S32* mv = &pDec->mv_buf[(pos & pDec->mv_bufmask)*4];

		if (pos == pDec->slice_pos + pDec->slice_height*MB_X)
		{
			pDec->slice_pos = pos;
		}

		for (;POSX(pos)<pDec->nMBWidth;++pos,mv+=4) {
			VO_S32 nYMBPos = POSY(pos), nXMBPos = POSX(pos);	
//			DPRINTF(FF_DUMP_ALL,"x = %d y = %d\n",nXMBPos, nYMBPos);
#if  ENABLE_MBINFO
			pDec->mb_info[nYMBPos*pDec->nMBWidth+nXMBPos] = 0;
#endif
			/*TBD package the initial process*/
			UPDATE_CACHE(pDec);

			if (EofBits(pDec))
			{
// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
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
					goto EndOfDIVX3PVOP;
				}
				else
// #else
					return VO_ERR_NONE;
// #endif
			}
			*pMBPos = pos;

			//stream: 24bits available

			//DEBUG_MSG3(-1,T("%d %d %02X"),POSX(pos),POSY(pos),ShowBits(pDec,8));

			if (pDec->use_coded_bit && GetBits(pDec, 1)) {

				// not coded macroblock
				VO_S32 n = pDec->frameMap[pos];
				pDec->frameMap[pos] = (VO_U8)(n|1); // set rescue needed flag
				mv[3] = mv[2] = mv[1] = mv[0] = 0;
				*fpReconMB = ReconCopyMB;
			}
			else {
        
				VO_S32 code = GetDivx3VLC(pDec, pDec->mb_inter_vlc); // max 21bits
				pDec->nCBP = code & 0x3F;

				if (code & 0x40) { //inter

					pDec->frameMap[pos] = (VO_U8)(pDec->nCurFrameMap|1); // set rescue needed flag
					mv[3] = mv[2] = mv[1] = mv[0] = 
						getMV2(GetPMV(0,pos,pDec->slice_pos,pDec),pDec);
					BckMV[0] = 1;
					FwdMV[0] = 0;
					BckMV[4] = BckMV[3] = BckMV[2] = BckMV[1] =  CorrectMV( mv[0], pDec, nXMBPos, nYMBPos);
					BckMV[5] = mvchroma3(BckMV[1]);
					*fpReconMB = ReconInterMB;

					DIVX3DecInterMB( pDec, &vDecLocal, pos );
				}
				else { //intra

					pDec->frameMap[pos] = (VO_U8)pDec->nCurFrameMap;
					pDec->bACPred = GetBits(pDec,1);

					mv[3] = mv[2] = mv[1] = mv[0] = 0;

					DIVX3RescuePredict(pDec, pos - pDec->slice_pos, pos);  //restore AC_DC values

					*fpReconMB = ReconIntraMB;
					DIVX3DecIntraMB(pDec, pos, &vDecLocal );
				}
			}
//			DPRINTF(FF_DUMP_ALL,"MBY\n",vDecLocal.pCurY[0]);

// #if ENABLE_MULTI_THREAD
			//condition of continue parser
			++vopCtxt.nMBCount;
			if ((vopCtxt.nMBCount < pDec->nMpeg4ThdMaxMB && pDec->nWaiter == 0) || (pDec->nMpeg4Thd > 1 && vopCtxt.nMBCount < pDec->nMpeg4ThdMinMB) )
// #else
// 			if (++vopCtxt.nMBCount < MAX_MB_TASK_NUM)
// #endif
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
// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
				{
					pDec->nParserErr = VO_ERR_NONE;
					pDec->nParserMBPos = pos;
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
				}
// #endif
				fpReconMB = vopCtxt.fpReconMB;
				pMBPos = vopCtxt.pMBPos;
				vDecLocal.scanType = vopCtxt.scanType;
				vDecLocal.blockptr = vopCtxt.blockptr;
				vDecLocal.blockLen = vopCtxt.blockLen;
				vDecLocal.MVBack = vopCtxt.MVBack;
				vDecLocal.MVFwd = vopCtxt.MVFwd;
				do 
				{
					(*fpReconMB++)(pDec, &vDecLocal, *pMBPos++);
					vDecLocal.scanType += 6;
					vDecLocal.blockptr += 6*64;
					vDecLocal.blockLen += 6;
					vDecLocal.MVBack += 6;
					vDecLocal.MVFwd  += 6;
				} while (--vopCtxt.nMBCount);

				fpReconMB = vopCtxt.fpReconMB;
				pMBPos = vopCtxt.pMBPos;
				vDecLocal.scanType = vopCtxt.scanType;
				vDecLocal.blockptr = vopCtxt.blockptr;
				vDecLocal.blockLen = vopCtxt.blockLen;
				BckMV = vDecLocal.MVBack = vopCtxt.MVBack;
				FwdMV = vDecLocal.MVFwd = vopCtxt.MVFwd;

// #if ENABLE_MULTI_THREAD
				if (pDec->nMpeg4Thd > 1)
				{
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
						goto EndOfDIVX3PVOP;
					}

					pos = pDec->nParserMBPos;
					mv = &pDec->mv_buf[(pos & pDec->mv_bufmask)*4];
				}
// #endif
			}
		}
	}

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
		pDec->nParserErr = VO_ERR_NONE;
		pDec->nParserMBPos = pos - MB_X + pDec->nMBWidth - 1;	//let other threads finish the MB scanning
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
	}
// #endif
	if (vopCtxt.nMBCount > 0)
	{
		fpReconMB = vopCtxt.fpReconMB;
		pMBPos = vopCtxt.pMBPos;
		vDecLocal.scanType = vopCtxt.scanType;
		vDecLocal.blockptr = vopCtxt.blockptr;
		vDecLocal.blockLen = vopCtxt.blockLen;
		vDecLocal.MVBack = vopCtxt.MVBack;
		vDecLocal.MVFwd = vopCtxt.MVFwd;
		do
		{
			(*fpReconMB++)(pDec, &vDecLocal, *pMBPos++);
			vDecLocal.scanType += 6;
			vDecLocal.blockptr += 6*64;
			vDecLocal.blockLen += 6;
			vDecLocal.MVBack += 6;
			vDecLocal.MVFwd  += 6;
		}while (--vopCtxt.nMBCount);
	}

// #if ENABLE_MULTI_THREAD
	if (pDec->nMpeg4Thd > 1)
	{
EndOfDIVX3PVOP:
		//wait all sub-threads finish
#if USE_WHILE_FORVER
		WaitForSubThdFinish(pDec);
#else
		pthread_mutex_lock(&(pDec->finishMutex));
		pDec->bEndVop = 1;
		while (pDec->nFinish < (VO_S32)pDec->nMpeg4Thd-1)
		{
//			thread_sleep(0);
			pthread_cond_wait(&(pDec->finishCond), &(pDec->finishMutex));
		}
		pDec->nFinish = 0;
		pDec->bEndVop = 0;
		pthread_mutex_unlock(&(pDec->finishMutex));
#endif
// 		printf("end frame\n");
//		return pDec->nParserErr;
	}
// #endif
	return VO_ERR_NONE;
}

VO_U32 DIVX3DecBVOP( VO_MPEG4_DEC *pDec )
{
	// bLowDelay always 1 -> no B-frame
	return VO_ERR_NOT_IMPLEMENT;
}

#endif
