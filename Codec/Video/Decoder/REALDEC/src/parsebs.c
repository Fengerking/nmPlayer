/******************************************** BEGIN LICENSE BLOCK ******************************************************* 
* Version: RCSL 1.0 and Exhibits. 
* REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
* Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
* All Rights Reserved. 
* 
* The contents of this file, and the files included with this file, are 
* subject to the current version of the RealNetworks Community Source 
* License Version 1.0 (the "RCSL"), including Attachments A though H, 
* all available at http://www.helixcommunity.org/content/rcsl. 
* You may also obtain the license terms directly from RealNetworks. 
* You may not use this file except in compliance with the RCSL and 
* its Attachments. There are no redistribution rights for the source 
* code of this file. Please see the applicable RCSL for the rights, 
* obligations and limitations governing use of the contents of the file. 
* 
* This file is part of the Helix DNA Technology. RealNetworks is the 
* developer of the Original Code and owns the copyrights in the portions 
* it created. 
* 
* This file, and the files included with this file, is distributed and made 
* available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
* INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
* FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
* 
* Technology Compatibility Kit Test Suite(s) Location: 
* https://rarvcode-tck.helixcommunity.org 
* 
* Contributor(s): 
* 
******************************************************** END LICENSE BLOCK *************************************************/ 

/*************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute.
*************************************************************************************
*    Main decoding stage. *
*************************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1995 - 2000 Intel Corporation.
*    All Rights Reserved. 
***************************************************************************************
************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/

/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/parsebs.c,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp $ */
/* This file implements the main decoding stage of the rv89combo algorithm. */

/********************************************************************************
Emuzed India Private Limited
**********************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2nd May, 2005	Real Networks			Initial Code
16th May, 2005	M. Anjaneya Prasad		Optimized INTRA type and MB type
decoding functios
Modified RV9 Sub2FullPels functions and
Decoder_decodeLumaAndChromaCoefficients_SVLC
function
********************************************************************************/
//nav
#include <string.h>

#include "decoder.h"
#include "parsebs.h"
#include "bsx.h"
#include "tables.h"
#include "recon.h"
#include "idct.h"
#include  "basic.h"

#define MEDIAN_OF_3(a, b, c) \
	(((a) > (b)) \
	? (((b) > (c)) \
	? (b) \
	: (((a) > (c)) ? (c) : (a))) \
	: (((a) > (c)) \
	? (a) \
	: (((b) > (c)) ? (c) : (b))))

//#define MV_TEST

static const I32 LeftSubBlockEdge[2]		   = {-1,3};
static const U8  number_mv[NUMBER_OF_MBTYPES]  = { 0, 0, 1, 4, 1, 1, 0, 0, 2, 2, 2, 1 };
static const I32 mv_diff[4]					   = {1,3,1,3};
static const I32 mv_left[4]					   = {3,1,3,1};

#if defined(VODEBUG)||defined(_DEBUG)
int g_inputFrameNum;
#endif//_DEBUG


//Convert Length and Info to VLC code number 
#define LENGTH_INFO_TO_N(length, info) \
	((1 << ((length)>>1)) + (info) - 1)


RV_Status DecodeIntraTypes_RV9(SLICE_DEC *slice_di, const struct DecoderMBInfo *pMB, I8 *pAboveIntraTypes)
{
	U32			ulength, info = 0;
	U32			block;
	U32			lBitBuffer;
	I32			iLeftMode, iAboveLeftMode, iAboveRightMode;
	I32			A;
	I32			table_number;
	U32         table_index;
	U8			*u8TabPtr;
	U16			*u16TabPtr;
	DecoderIntraType    *pDstType = slice_di->m_pMBIntraTypes;
	const Bool32        isTopMB  = !(slice_di->uEdgeAvail&0x2);
	I32			bits,bitpos;
	I32         j,idx;
	I32         left_idx[4] = {3,0,1,2};

	loadbits(slice_di);

	/* Loop over 8 pairs of subblocks. One pair is */
	/* decoded in each iteration. */
	bits   = slice_di->m_bits;
	bitpos = slice_di->m_bitpos;

	for (block = 0; block < 4; block ++,pDstType += 4){
		if (!block && isTopMB){
			/* decode 4 modes */
			u8TabPtr = (PU8)aic_top_vlc;
			loadbits_check(slice_di,bits, bitpos,32 - 8);
			lBitBuffer = showbits_pos(bits,bitpos,8);

			for (A = 0; A < 16; A++){
				info = *u8TabPtr++;
				GET_VLC_LENGTH_INFO_CHAR(info, ulength);
				if (info == (lBitBuffer >> ( 8 - ulength)))
				{
					flushbits_pos(bitpos, ulength);
					break;
				}
			}

			pDstType[0] = (DecoderIntraType)((A >> 2) & 2);
			pDstType[1] = (DecoderIntraType)((A >> 1) & 2);
			pDstType[2] = (DecoderIntraType)((A) & 2);
			pDstType[3] = (DecoderIntraType)((A << 1) & 2);

			continue;
		}


		for (j = 0; j < 4; j++){
			idx = left_idx[j];

			iLeftMode =  pDstType[idx];
			iAboveLeftMode  = block ? pDstType[-4 + j] : pAboveIntraTypes[0 + j];
			iAboveRightMode = block ? pDstType[-3 + j] : pAboveIntraTypes[1 + j];
			table_index    = 21;

			if(j < 3){
				table_number = (iLeftMode << 8) + (iAboveLeftMode << 4) +iAboveRightMode;  

				for (table_index = 0; table_index < 20; table_index++){
					if (aic_table_index[table_index] == table_number)
						break;
				}
			}


			loadbits_check(slice_di,bits,bitpos,32 - 16);

			if (table_index < 20){
				u16TabPtr = (PU16)(aic_2mode_vlc + table_index*81);
				lBitBuffer = showbits_pos(bits, bitpos,16);

				for (table_number = 0, A = 0;  table_number < 81; table_number++){
					info = *u16TabPtr++;
					GET_VLC_LENGTH_INFO_SHORT(info,ulength);
					if (info == (lBitBuffer >> ( 16 - ulength))){
						flushbits_pos(bitpos,ulength);
						break;
					}					
				}

				pDstType[0 + j] = (DecoderIntraType)(table_number/9);
				pDstType[1 + j] = (DecoderIntraType)(table_number%9);

				j++;
			}else{
				/* decode 1 mode */
				if (iLeftMode != -1 && iAboveLeftMode != -1){
					if (((10*iLeftMode + iAboveLeftMode + 11)*9)>= 900){
						return RV_S_ERROR;
					}
					u8TabPtr = (PU8)(aic_1mode_vlc + (10*iLeftMode + iAboveLeftMode + 11)*9);
					lBitBuffer = showbits_pos(bits,bitpos,8);

					for (A = 0; A < 9; A ++){
						info = *u8TabPtr++;
						GET_VLC_LENGTH_INFO_CHAR(info,ulength);
						if ((ulength) && (info == (lBitBuffer >> (8 - ulength)))){
							flushbits_pos(bitpos, ulength);
							break;
						}
					}
				} else{
					A = 0;
					lBitBuffer = showbits_pos(bits,bitpos,1);
					flushbits_pos(bitpos, 1);
					switch(iLeftMode){
					case -1: // code 0 -> 1, 1 -> 0
						if(iAboveLeftMode == -1 || iAboveLeftMode == 0 || iAboveLeftMode == 1)
							A = lBitBuffer ^ 1;
						break;
					case  0:
					case  2: // code 0 -> 2, 1 -> 0
						A = (lBitBuffer ^ 1) << 1;
						break;
					}
				}

				pDstType[0 + j] = (DecoderIntraType)A;

			}
		}
	}


	slice_di->m_bits   = bits;
	slice_di->m_bitpos = bitpos;

	/* Copy the intra-coding types for the bottom row of subblocks */
	/* to m_pAboveSubBlockIntraTypes, so it can be used when decoding */
	((PU32)pAboveIntraTypes)[0] = ((PU32)slice_di->m_pMBIntraTypes)[3];
	return RV_S_OK;
}


RV_Status DecodePMBType_RV8(SLICE_DEC *slice_di, struct DecoderMBInfo *pMBInfo, U8 quant_prev,U32 * ulSkipModesLeft, I32 uWidthInMBs)
{
	U32 length, info;
	U32 mode;
	RV_Status status = RV_S_OK;

	static const I32 MBTypes_P[6] = { MBTYPE_SKIPPED, MBTYPE_INTER, MBTYPE_INTER_4V, -1, MBTYPE_INTRA, MBTYPE_INTRA_16x16 };

	length = CB_GetVLCBits(slice_di, &info);

	mode = LENGTH_INFO_TO_N(length,info);

	if (mode > 5){
		/* dquant */
		mode -= 5;
		CB_GetDQUANT(slice_di, &pMBInfo->QP, quant_prev);
	}else{
		pMBInfo->QP = quant_prev;
	}

	if(mode > 5){
		status = RV_S_ERROR;
	}else{
	if(-1 == MBTypes_P[mode]){
		pMBInfo->mbtype = MBTYPE_SKIPPED;
		status = RV_S_ERROR;
	}else{
		pMBInfo->mbtype = (U8)MBTypes_P[mode];//TBD, make sure mode will not exceed 5
	}
	}

	return status;
}  /* decodemacroblocktype_rv8 */

RV_Status DecodeBMBType_RV8(SLICE_DEC *slice_di, struct DecoderMBInfo *pMBInfo, U8 quant_prev,U32 * ulSkipModesLeft, I32 uWidthInMBs)
{
	U32 length, info;
	U32 mode;
	RV_Status status = RV_S_OK;
	static const I32 MBTypes_B[6] ={MBTYPE_SKIPPED,MBTYPE_DIRECT,MBTYPE_FORWARD,MBTYPE_BACKWARD,MBTYPE_INTRA,MBTYPE_INTRA_16x16};

	length = CB_GetVLCBits(slice_di, &info);

	mode = LENGTH_INFO_TO_N(length,info);

	if (mode > 5){
		/* dquant */
		mode -= 5;
		CB_GetDQUANT(slice_di, &pMBInfo->QP, quant_prev);
	}else{
		pMBInfo->QP = quant_prev;
	}

	if(mode > 5){
		return RV_S_ERROR;
	}

	pMBInfo->mbtype = (U8)MBTypes_B[mode];

	return status;

}  /* decodemacroblocktype_rv8 */

static INLINE I32 mvd_value(struct DecoderPackedMotionVector *pMV, I32 step)
{
	I32 d;
	d = pMV->mvx - (pMV - step)->mvx;
	if(d < -3 || d > 3)
		return 1;
	d = pMV->mvy - (pMV - step)->mvy;
	if(d < -3 || d > 3)
		return 1;
	return 0;
}

static void SetDeblockCoefs(struct Decoder *t, struct DecoderMBInfo  *pMB, struct DecoderPackedMotionVector *pMV, I32 slice_index)
{
	U32 i,j;
	U32 hmvd_mask = 0,vmvd_mask = 0, mvd_mask;
	SLICE_DEC *slice_di = &t->m_slice_di[slice_index];
	U32 top_avail = (slice_di->uEdgeAvail & 0x2) >> 1;
	U32 left_avail = (slice_di->uEdgeAvail&0x10) >> 4;
	I32 top_step;


	if (!t->m_deblocking_filter_passthrough){

		top_step = (t->m_mbX << 2) - 2;

		for(j = 0; j < 16; j += 8){
			for(i = 0; i < 2; i++){
				if((i || left_avail) && mvd_value(pMV + i,mv_left[i]))
					vmvd_mask |= 0x11 << (j + i*2);
				if((j||top_avail) && mvd_value(pMV + i, top_step))
					hmvd_mask |= 0x03 << (j + i*2);
			}
			pMV     += 2;
			top_step = 2;
		}

		if(t->m_bIsRV8){ //RV8 marks both subblocks on the edge for filtering
			vmvd_mask |= (vmvd_mask & 0x4444) >> 1;
			hmvd_mask |= (hmvd_mask & 0x0F00) >> 4;
			if(top_avail)
				(pMB - t->m_mbX)->deblock_cbpcoef |= (hmvd_mask & 0xF) << 12;
			if(left_avail)
				(pMB - 1)->deblock_cbpcoef |= (vmvd_mask & 0x1111) << 3;

			mvd_mask = hmvd_mask | vmvd_mask;
			pMB->deblock_cbpcoef |= mvd_mask;
		}else{
			mvd_mask = hmvd_mask | vmvd_mask;
			pMB->mvd = (pMB->deblock_cbpcoef & 0xffff) | mvd_mask;
		}
	}
}

RV_Status DecodeIntraTypes_RV8(SLICE_DEC *slice_di, const struct DecoderMBInfo *pMB, DecoderIntraType *pAboveIntraTypes)
{
	U32                 length, info, blockpair, N, A, B, prob0, prob1;
	Bool32              isLeftEdgeSubBlock, isTopEdgeSubBlock;
	DecoderIntraType    *pDstType = slice_di->m_pMBIntraTypes;
	U32                 idx;
	I32                 left_idx;

	loadbits(slice_di);

	for (blockpair = 0; blockpair < 8; blockpair++){
		length = CB_GetVLCBits(slice_di, &info);
		N = LENGTH_INFO_TO_N(length,info);

		isLeftEdgeSubBlock = ((blockpair & 1) == 0);
		isTopEdgeSubBlock  = (blockpair < 2);

		left_idx = LeftSubBlockEdge[isLeftEdgeSubBlock];

		B =  1 + pDstType[left_idx];
		A = isTopEdgeSubBlock ? (1 + pAboveIntraTypes[blockpair << 1]) : (1 + pDstType[-4]);

		if (N > 83){
			N = 83;
		}

		idx = dec_iprob_rv8[N];
		prob0 = idx >> 4;
		prob1 = idx & 15;

		idx   = 90*A + 9*B + prob0;

		if((idx>>1)>= 450 ){
			return RV_S_ERROR;
		}

		*pDstType = (dec_aic_prob_rv8[idx>>1]>>(!(idx&1)<<2)) & 15;

		if (*pDstType >= NUM_AI_MODES){
			return RV_S_OUT_OF_RANGE;
		}

		pDstType++;

		B = 1 + pDstType[-1];
		A = isTopEdgeSubBlock ? (1 + pAboveIntraTypes[(blockpair << 1) + 1]) : (1 + pDstType[-4]);

		idx = 90*A + 9*B + prob1;
		if(idx > 899){//huwei 20110304 stability
			idx = 899;
		}
		*pDstType = (dec_aic_prob_rv8[idx>>1]>>(!(idx&1)<<2)) & 15;

		if (*pDstType >= NUM_AI_MODES){
			return RV_S_OUT_OF_RANGE;
		}

		pDstType++;
	}

	/* Copy the intra-coding types for the bottom row of subblocks */
	/* to m_pAboveSubBlockIntraTypes, so it can be used when decoding */
	((PU32)pAboveIntraTypes)[0] = ((PU32)slice_di->m_pMBIntraTypes)[3];
	return RV_S_OK;
}  /* decodeintratypes_rv8 */

RV_Status DecodePMBType_RV9(SLICE_DEC *slice_di, struct DecoderMBInfo *pMBInfo, U8 quant_prev,U32 * ulSkipModesLeft, I32 uWidthInMBs)
{
	U32 M, N, length, info;
	U32 lBitBuffer;
	U32 thisMBType;
	U32 bestMBType;
	U32 edge_avail = slice_di->uEdgeAvail;
	U16 vlc_buffer;

	I32 bits,bitpos;
#define  LAST_HIST_INDEX NUMBER_OF_MBTYPES+3
	U32 uNeighborMBHist[LAST_HIST_INDEX] = 
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; /* Hist array 15 */

	RV_Status status = RV_S_OK;

	if (*ulSkipModesLeft == 0)
	{
		/* Read the number of skipped MBs */
		length = CB_GetVLCBits(slice_di, &info);
		*ulSkipModesLeft = (U32) LENGTH_INFO_TO_N(length,info);
	}
	else
	{
		(*ulSkipModesLeft)--;
	}

	/* If there's still more MBs to skip, then skip them. */
	if (*ulSkipModesLeft > 0)
	{
		pMBInfo->mbtype = MBTYPE_SKIPPED;
		pMBInfo->QP = quant_prev;

		return status;
	}

	/* left */
	if (edge_avail & 0x10){
		if (pMBInfo[-1].mbtype > 14){
			return RV_S_ERROR;
		}
		uNeighborMBHist[pMBInfo[-1].mbtype]++;
	}
	/* above */
	if (edge_avail & 0x2){
		if (pMBInfo[-uWidthInMBs].mbtype >= 14){
			return RV_S_ERROR;
		}
		uNeighborMBHist[pMBInfo[-uWidthInMBs].mbtype]++;
		if (edge_avail & 0x1){/* left-above */
			if (pMBInfo[-1 - uWidthInMBs].mbtype > 14){
				return RV_S_ERROR;
			}
			uNeighborMBHist[pMBInfo[-1 - uWidthInMBs].mbtype]++;
		}
		if (edge_avail & 0x8){/* right-above */
			if (pMBInfo[1 - uWidthInMBs].mbtype > 14){
				return RV_S_ERROR;
			}
			uNeighborMBHist[pMBInfo[1 - uWidthInMBs].mbtype]++;
		}
	}

	/* make all MBTYPE_SKIPPED be MBTYPE_INTER or MBTYPE_DIRECT */

	uNeighborMBHist[MBTYPE_INTER] += uNeighborMBHist[MBTYPE_SKIPPED];
	uNeighborMBHist[MBTYPE_SKIPPED] = 0;


	for (M = 0, N = 0, bestMBType = MBTYPE_SKIPPED; M < LAST_HIST_INDEX; M++)
	{
		if (uNeighborMBHist[M] > N)
		{
			N = uNeighborMBHist[M];
			bestMBType = M;
		}
	}

#define P_MBTYPE_DQUANT 7
#define PI_MBTYPE_DQUANT 10

	bits   = slice_di->m_bits;
	bitpos = slice_di->m_bitpos;
	loadbits_check(slice_di,bits,bitpos,32-16);
	/* Construct and write codeword */

	if (bestMBType >= 12){
		return RV_S_ERROR;
	}

	M = mbtype_p_enc_tab[bestMBType];
	lBitBuffer = showbits_pos(bits,bitpos, 8);


	for (thisMBType = 0; thisMBType <= P_MBTYPE_DQUANT; thisMBType++)
	{
		vlc_buffer = mbtype_pframe_vlc[(M << 3) + thisMBType];
		info       = vlc_buffer >> 8;
		length     = vlc_buffer & 0xff;
		if (info == (lBitBuffer >> (8 - length))){
			flushbits_pos(bitpos,length);
			break;
		}
	}

	RVAssert(thisMBType <= P_MBTYPE_DQUANT);

	if (thisMBType == P_MBTYPE_DQUANT){
		/* DQUANT escape code */
		lBitBuffer = showbits_pos(bits,bitpos,8);
		for (thisMBType = 0; thisMBType <= P_MBTYPE_DQUANT; thisMBType++){
			vlc_buffer = mbtype_pframe_vlc[(M << 3) + thisMBType];
			info       = vlc_buffer >> 8;
			length     = vlc_buffer & 0xff;
			if (info == (lBitBuffer >> (8 - length))){
				flushbits_pos(bitpos,length);
				break;
			}
		}

		if (thisMBType >= P_MBTYPE_DQUANT){
			return RV_S_ERROR;
		}

		/* Get DQUANT */
		CB_GetDQUANT(slice_di, &(pMBInfo->QP), quant_prev);
	}else{
		pMBInfo->QP = quant_prev;
	}

	pMBInfo->mbtype = mbtype_p_dec_tab[thisMBType];
	slice_di->m_bitpos = bitpos;
	slice_di->m_bits   = bits;

	RVAssert(pMBInfo->mbtype != 255);
	return status;

}

RV_Status DecodeBMBType_RV9(SLICE_DEC *slice_di, struct DecoderMBInfo *pMBInfo, U8 quant_prev,U32 * ulSkipModesLeft, I32 uWidthInMBs)
{
	U32 M, N, length, info;
	U32 lBitBuffer;
	U32 thisMBType;
	U32 bestMBType;
	U32 edge_avail = slice_di->uEdgeAvail;
	U8  vlc_buffer;

	I32 bits,bitpos;

#define  LAST_HIST_INDEX NUMBER_OF_MBTYPES+3
	U32 uNeighborMBHist[LAST_HIST_INDEX] = 
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; /* Hist array 15 */

	RV_Status status = RV_S_OK;


	if (*ulSkipModesLeft == 0)
	{
		/* Read the number of skipped MBs */
		length = CB_GetVLCBits(slice_di, &info);
		*ulSkipModesLeft = (U32) LENGTH_INFO_TO_N(length,info);
	}
	else
	{
		(*ulSkipModesLeft)--;
	}

	/* If there's still more MBs to skip, then skip them. */
	if (*ulSkipModesLeft > 0)
	{
		pMBInfo->mbtype = MBTYPE_SKIPPED;
		pMBInfo->QP = quant_prev;

		return status;
	}

	/* left */
	if (edge_avail & 0x10){
		if (pMBInfo[-1].mbtype > 14){
			return RV_S_ERROR;
		}
		uNeighborMBHist[pMBInfo[-1].mbtype]++;
	}
	/* above */
	if (edge_avail & 0x2){
		if (pMBInfo[-uWidthInMBs].mbtype > 14){
			return RV_S_ERROR;
		}
		uNeighborMBHist[pMBInfo[-uWidthInMBs].mbtype]++;
		if (edge_avail & 0x1){/* left-above */
			if (pMBInfo[-1 - uWidthInMBs].mbtype > 14){
				return RV_S_ERROR;
			}
			uNeighborMBHist[pMBInfo[-1 - uWidthInMBs].mbtype]++;
		}
		if (edge_avail & 0x8){/* right-above */
			if (pMBInfo[1 - uWidthInMBs].mbtype > 14){
				return RV_S_ERROR;
			}
			uNeighborMBHist[pMBInfo[1 - uWidthInMBs].mbtype]++;
		}
	}

	/* make all MBTYPE_SKIPPED be MBTYPE_INTER or MBTYPE_DIRECT */
	uNeighborMBHist[MBTYPE_DIRECT] += uNeighborMBHist[MBTYPE_SKIPPED];
	uNeighborMBHist[MBTYPE_SKIPPED] = 0;


	for (M = 0, N = 0, bestMBType = MBTYPE_SKIPPED; M < LAST_HIST_INDEX; M++)
	{
		if (uNeighborMBHist[M] > N)
		{
			N = uNeighborMBHist[M];
			bestMBType = M;
		}
	}

#define P_MBTYPE_DQUANT 7
#define PI_MBTYPE_DQUANT 10

	bits   = slice_di->m_bits;
	bitpos = slice_di->m_bitpos;
	loadbits_check(slice_di,bits,bitpos,32-16);

	/* Construct and write codeword */
	/*  BiFrame Mb Types
	MBTYPE_INTRA,			// 0 , mbtype=0
	MBTYPE_INTRA_16x16,		// 1 , mbtype=1
	MBTYPE_FORWARD,			// 2 , mbtype=4
	MBTYPE_BACKWARD,		// 3 , mbtype=5       
	MBTYPE_DIRECT,			// 4 , mbtype=7
	MBTYPE_BIDIR,			// 5  B_DQUANT 6 , mbtype=10
	MBTYPE_FORWARD_16x16i,  // 6 , mbtype=15
	MBTYPE_BACKWARD_16x16i, // 7 , mbtype=16
	MBTYPE_FORWARD_16x8i,   // 8 , mbtype=17
	MBTYPE_BACKWARD_16x8i,  // 9 , mbtype=18
	BI_DQUANT               //10 */
#define B_MBTYPE_DQUANT 6
#define BI_MBTYPE_DQUANT 10


	M = mbtype_b_enc_tab[bestMBType];
	lBitBuffer = showbits_pos(bits,bitpos,8);

	for (thisMBType = 0; thisMBType <= B_MBTYPE_DQUANT; thisMBType++)
	{
		vlc_buffer = mbtype_bframe_vlc[(M << 3) + thisMBType];
		info       = vlc_buffer >> 3;
		length     = vlc_buffer & 7;
		if (info == (lBitBuffer >> (8 - length)))
		{
			flushbits_pos(bitpos,length);
			break;
		}
	}

	RVAssert(thisMBType <= B_MBTYPE_DQUANT);

	if (thisMBType == B_MBTYPE_DQUANT){
		/* DQUANT escape code */
		lBitBuffer = showbits_pos(bits,bitpos,8);
		for (thisMBType = 0; thisMBType <= B_MBTYPE_DQUANT; thisMBType++)
		{
			vlc_buffer = mbtype_bframe_vlc[(M << 3) + thisMBType];
			info       = vlc_buffer >> 3;
			length     = vlc_buffer & 7;
			if (info == (lBitBuffer >> (8 - length)))
			{
				flushbits_pos(bitpos,length);
				break;
			}
		}

		if (thisMBType >= B_MBTYPE_DQUANT){
			return RV_S_ERROR;
		}

		/* Get DQUANT */
		CB_GetDQUANT(slice_di, &(pMBInfo->QP), quant_prev);
	}
	else
	{
		pMBInfo->QP = quant_prev;
	}

	pMBInfo->mbtype = mbtype_b_dec_tab[thisMBType];

	slice_di->m_bitpos = bitpos;
	slice_di->m_bits   = bits;

	RVAssert(pMBInfo->mbtype != 255);

	RVDebug((RV_TL_MBLK,"QP: %d", pMBInfo->QP));

	return status;
}

/***********************************************************************/
/* Calculate motion vector predictor for a specific block. 				*/
/* B frame backward and forward MB types 								*/
/**********************************************************************/
static INLINE void rv89_pred_b_vector(I32 A[2], I32 B[2], I32 C[2], I32 no_A, I32 no_B, I32 no_C, I32 *mx, I32 *my)
{
	if(no_A + no_B + no_C){
		*mx = A[0] + B[0] + C[0];
		*my = A[1] + B[1] + C[1];
		if(no_A + no_B + no_C == 1){
			*mx /= 2;
			*my /= 2;
		}
	}else{
		*mx = MEDIAN_OF_3(A[0], B[0], C[0]);
		*my = MEDIAN_OF_3(A[1], B[1], C[1]);
	}
}

RV_Status GetPredictorsMVBFrame(SLICE_DEC *slice_di,const struct DecoderMBInfo *pMBInfo,
								const struct DecoderPackedMotionVector *pMV,I32 *pMVx,I32 *pMVy,const U32 uMB_X, const I32 mbX)
{
	Bool32  isRightUnavailable ,isTopUnavailable, isLeftUnavailable;
	U32 mbtype;

	I32 c_mv_pos, c_mv_pos_backward;
	struct DecoderPackedMotionVector *pMVb,*pMVbTop,*pMVTop;

	I32 mbA[2][2], mbB[2][2], mbC[2][2];
	I32 no_mbA[2], no_mbB[2], no_mbC[2];

	mbA[0][0] = 0;//left
	mbA[0][1] = 0;
	mbA[1][0] = 0;
	mbA[1][1] = 0;

	mbB[0][0] = 0;//top
	mbB[0][1] = 0;
	mbB[1][0] = 0;
	mbB[1][1] = 0;

	mbC[0][0] = 0;//right
	mbC[0][1] = 0;
	mbC[1][0] = 0;
	mbC[1][1] = 0;

	pMVTop  = slice_di->m_pTopMotionVectors + uMB_X * 4;
	pMVb    = slice_di->m_pCurBidirMotionVectors + uMB_X * 4;
	pMVbTop = slice_di->m_pTopBidirMotionVectors + uMB_X * 4; 
	isLeftUnavailable  = !(slice_di->uEdgeAvail&0x10);
	isTopUnavailable   = !(slice_di->uEdgeAvail&0x2);
	isRightUnavailable = !(slice_di->uEdgeAvail&0x8);//((uMB_X + 1)== t->m_mbX) ? 1 : 0;

	mbtype = pMBInfo->mbtype;

	if(isLeftUnavailable)
		no_mbA[0] = no_mbA[1] = 1;
	else{
		no_mbA[0] = no_mbA[1] = 0;
		if((pMBInfo - 1)->mbtype  != MBTYPE_FORWARD  && (pMBInfo - 1)->mbtype  != MBTYPE_BIDIR)
			no_mbA[0] = 1;
		if((pMBInfo - 1)->mbtype  != MBTYPE_BACKWARD && (pMBInfo - 1)->mbtype  != MBTYPE_BIDIR)
			no_mbA[1] = 1;
		if(!no_mbA[0]){
			mbA[0][0] = (pMV - 3)->mvx ;
			mbA[0][1] = (pMV - 3)->mvy ;
		}
		if(!no_mbA[1]){
			mbA[1][0] = (pMVb - 4)->mvx;
			mbA[1][1] = (pMVb - 4)->mvy;
		}
	}

	if(isTopUnavailable){
		no_mbB[0] = no_mbB[1] = 1;
		no_mbC[0] = no_mbC[1] = 1;
		c_mv_pos  = 0;
	}else{
		no_mbB[0] = no_mbB[1] = 0;
		if((pMBInfo - mbX)->mbtype != MBTYPE_FORWARD  && (pMBInfo - mbX)->mbtype != MBTYPE_BIDIR)
			no_mbB[0] = 1;
		if((pMBInfo - mbX)->mbtype != MBTYPE_BACKWARD && (pMBInfo - mbX)->mbtype != MBTYPE_BIDIR)
			no_mbB[1] = 1;
		if(!no_mbB[0]){
			if (isTopUnavailable){
				mbB[0][0] = mbA[0][0];
				mbB[0][1] = mbA[0][1];
			} else{
				mbB[0][0] = (pMVTop + 2)->mvx;
				mbB[0][1] = (pMVTop + 2)->mvy;
			}
		}
		if(!no_mbB[1]){
			if (isTopUnavailable){
				mbB[1][0] = mbA[1][0];
				mbB[1][1] = mbA[1][1];
			} else{
				mbB[1][0] = pMVbTop->mvx;
				mbB[1][1] = pMVbTop->mvy;
			}

		}
	}

	if(isRightUnavailable){
		no_mbC[0] = no_mbC[1] = 0;
		if((pMBInfo - mbX - 1)->mbtype != MBTYPE_FORWARD  && (pMBInfo - mbX - 1)->mbtype != MBTYPE_BIDIR)
			no_mbC[0] = 1;
		if((pMBInfo - mbX - 1)->mbtype != MBTYPE_BACKWARD && (pMBInfo - mbX - 1)->mbtype != MBTYPE_BIDIR)
			no_mbC[1] = 1;
		c_mv_pos = - 1;
		c_mv_pos_backward = -4;
	}else if(!isTopUnavailable){
		no_mbC[0] = no_mbC[1] = 0;
		if((pMBInfo - mbX + 1)->mbtype != MBTYPE_FORWARD  && (pMBInfo - mbX + 1)->mbtype != MBTYPE_BIDIR)
			no_mbC[0] = 1;
		if((pMBInfo - mbX + 1)->mbtype != MBTYPE_BACKWARD && (pMBInfo - mbX + 1)->mbtype != MBTYPE_BIDIR)
			no_mbC[1] = 1;
		c_mv_pos =  6;
		c_mv_pos_backward = 4;
	}

	if(!no_mbC[0]){
		if (isTopUnavailable){
			mbC[0][0] = mbA[0][0];
			mbC[0][1] = mbA[0][1];
		} else{
			mbC[0][0] = (pMVTop + c_mv_pos)->mvx;
			mbC[0][1] = (pMVTop + c_mv_pos)->mvy;
		}
	}

	if(!no_mbC[1]){
		if (isTopUnavailable){
			mbC[1][0] = mbA[1][0];
			mbC[1][1] = mbA[1][1];
		} else{
			mbC[1][0] = (pMVbTop + c_mv_pos_backward)->mvx;
			mbC[1][1] = (pMVbTop + c_mv_pos_backward)->mvy;
		}
	}

	switch(mbtype){
	case MBTYPE_FORWARD:
		rv89_pred_b_vector(mbA[0], mbB[0], mbC[0], no_mbA[0], no_mbB[0], no_mbC[0], pMVx, pMVy);
		break;
	case MBTYPE_BACKWARD:
		rv89_pred_b_vector(mbA[1], mbB[1], mbC[1], no_mbA[1], no_mbB[1], no_mbC[1], pMVx, pMVy);
		break;
	case MBTYPE_BIDIR:
		rv89_pred_b_vector(mbA[0], mbB[0], mbC[0], no_mbA[0], no_mbB[0], no_mbC[0], pMVx, pMVy);
		rv89_pred_b_vector(mbA[1], mbB[1], mbC[1], no_mbA[1], no_mbB[1], no_mbC[1], pMVx + 1, pMVy + 1);
		break;
	default:
		no_mbA[0] = no_mbA[1] = no_mbB[0] = no_mbB[1] = no_mbC[0] = no_mbC[1] = 1;
	}

	return RV_S_OK;

}

#ifdef MV_TEST 
I32 getPMV(struct Decoder *t,I32 Block_Num,I32 pos, I32 *pMVx,I32 *pMVy,struct DecoderPackedMotionVector *pMV,const U32 uMB_X, const U32 is16)
{
	I32 resyncpos = t->m_mba;

	I32 MB_X = t->m_mbX;
	I32     mbA[2], mbB[2], mbC[2];
	struct DecoderPackedMotionVector *pMVTop;
	Bool32  left_avail,top_avail,right_avail;
	pMVTop = t->m_pTopMotionVectors + uMB_X * 4;//YU TBD

	memset(mbA, 0, sizeof(mbA));//left
	memset(mbB, 0, sizeof(mbB));//top
	memset(mbC, 0, sizeof(mbC));//right
	left_avail  = t->uEdgeAvail&0x10;
	top_avail   = t->uEdgeAvail&0x2;
	right_avail = t->uEdgeAvail&0x8;
	switch (Block_Num){
	case 0:
		if (pos == resyncpos){
			*pMVx = 0; //YU TBD
			*pMVy = 0;
			return 0;//y
		}

		if (pos < resyncpos+MB_X){ 
			if (left_avail){
				*pMVx = pMV[-4+1].mvx;
				*pMVy = pMV[-4+1].mvy;
			}else{
				*pMVx = 0; //YU TBD
				*pMVy = 0;
			}
			return 0;
		}
		if(left_avail){
			mbA[0] = pMV[-4+1].mvx;
			mbA[1] = pMV[-4+1].mvy;
		}
		if(top_avail){
			mbB[0] = pMVTop[2].mvx;
			mbB[1] = pMVTop[2].mvy;
		}else{
			mbB[0] = mbA[0];
			mbB[1] = mbA[1];
		}
		if(right_avail || is16 == 3){
			mbC[0] = pMVTop[4+2-is16].mvx;
			mbC[1] = pMVTop[4+2-is16].mvy;	
		}else{
			if(!left_avail || !top_avail){
				mbC[0] = mbA[0];
				mbC[1] = mbA[1];					
			}else{
				mbC[0] = pMVTop[-1].mvx ;
				mbC[1] = pMVTop[-1].mvy ;
			}
		}
		break;
	case 1:
		if (pos < resyncpos+MB_X){ 
			*pMVx = pMV[0].mvx;
			*pMVy = pMV[0].mvy;
			return 0;
		}

		mbA[0] = pMV[0].mvx;
		mbA[1] = pMV[0].mvy;

		if(top_avail){
			mbB[0] = pMVTop[3].mvx;
			mbB[1] = pMVTop[3].mvy;
		}else{
			mbB[0] = mbA[0];
			mbB[1] = mbA[1];
		}
		if(right_avail){
			mbC[0] = pMVTop[4+2].mvx;
			mbC[1] = pMVTop[4+2].mvy;	
		}else{
			if(!top_avail){
				mbC[0] = mbA[0];
				mbC[1] = mbA[1];					
			}else{
				mbC[0] = pMVTop[2].mvx ;
				mbC[1] = pMVTop[2].mvy ;
			}	
		}
		break;	
	case 2:
		if(left_avail){
			mbA[0] = pMV[-4+3].mvx;
			mbA[1] = pMV[-4+3].mvy;
		}
		mbB[0] = pMV[0].mvx;
		mbB[1] = pMV[0].mvy;
		if(is16==0){
			if(!left_avail){
				mbC[0] = mbA[0];
				mbC[1] = mbA[1];
			}else{
				mbC[0] = pMV[-3].mvx;
				mbC[1] = pMV[-3].mvy;
			}	
		}else{
			mbC[0] = pMV[1].mvx;
			mbC[1] = pMV[1].mvy;	
		}

		break;
	default: // case 3
		mbA[0] = pMV[2].mvx;
		mbA[1] = pMV[2].mvy;
		mbB[0] = pMV[1].mvx;
		mbB[1] = pMV[1].mvy;
		mbC[0] = pMV[0].mvx;
		mbC[1] = pMV[0].mvy;
		break;
	}

	*pMVx = MEDIAN_OF_3(mbA[0], mbB[0], mbC[0]);
	*pMVy = MEDIAN_OF_3(mbA[1], mbB[1], mbC[1]);
}
#endif //MV_TEST
/*************************************************************************/
/* Calculate motion vector predictor for a specific block. 					*/
/*************************************************************************/
RV_Status GetPredictorsMV(SLICE_DEC *slice_di,const struct DecoderMBInfo *pMBInfo,
						  struct DecoderPackedMotionVector *pMV,I32 block,
						  I32 *pMVx,I32 *pMVy,const U32 uMB_X, I32 isRV8)
{	
	I32     diff;
	I32     mbA[2], mbB[2], mbC[2];

	Bool32  isRightAvailable,isTopAvailable,isLeftAvailable;
	struct DecoderPackedMotionVector *pMVTop,*pMVTemp;

	mbA[0] = 0;//left
	mbA[1] = 0;

	mbB[0] = 0;//top
	mbB[1] = 0;

	mbC[0] = 0;//right
	mbC[1] = 0;

	pMVTop = slice_di->m_pTopMotionVectors + uMB_X * 4 + block;

	isLeftAvailable  = (slice_di->uEdgeAvail&0x10)>>4; 
	isTopAvailable   = (slice_di->uEdgeAvail&0x2)>>1;
	isRightAvailable = (slice_di->uEdgeAvail&0x8)>>3;

	pMVTemp = pMVTop + 2;

	switch(pMBInfo->mbtype){
	case MBTYPE_INTER:
	case MBTYPE_INTER_16x16:
	case MBTYPE_FORWARD:
	case MBTYPE_BACKWARD:
		if(isRightAvailable){
			mbC[0] = (pMVTop + 6)->mvx;
			mbC[1] = (pMVTop + 6)->mvy;
		}
		break;
	case MBTYPE_INTER_4V:
		diff = mv_diff[block];
		if (block >= 2) pMVTemp = pMV - 2;
		if(block & 1) isLeftAvailable = 1;
		if(block & 2) isTopAvailable = 1;
		if(block == 2) isRightAvailable = 1;
		if(!block) isRightAvailable = isTopAvailable;

		if(isRightAvailable){
			mbC[0] = (pMVTemp + diff)->mvx;
			mbC[1] = (pMVTemp + diff)->mvy;
		}

		if(block == 3){
			isRightAvailable = 1;
			mbC[0] = (pMV - 3)->mvx;
			mbC[1] = (pMV - 3)->mvy;
		}
		break;
	case MBTYPE_INTER_16x8V:
		isTopAvailable |= block;
		isRightAvailable &= ~block;

		if (block)
		{
			pMVTemp = pMV - 2;
			block   = 2;
		}

		if(isRightAvailable){
			mbC[0] = (pMVTop + 6)->mvx;
			mbC[1] = (pMVTop + 6)->mvy;
		}
		break;
	case MBTYPE_INTER_8x16V:
		isLeftAvailable |= block;
		if(!block) isRightAvailable = isTopAvailable;
		if(isRightAvailable){
			mbC[0] = (pMVTop + 2 + mv_diff[block])->mvx;
			mbC[1] = (pMVTop + 2 + mv_diff[block])->mvy;
		}
		break;
	default:
		isLeftAvailable = isTopAvailable = isRightAvailable = 0;
	}

	if(isLeftAvailable){
		mbA[0] = (pMV - mv_left[block])->mvx;
		mbA[1] = (pMV - mv_left[block])->mvy;
	}
	if(isTopAvailable){
		mbB[0] = pMVTemp->mvx;
		mbB[1] = pMVTemp->mvy;
	}else{
		mbB[0] = mbA[0];
		mbB[1] = mbA[1];
	}
	if(!isRightAvailable){
		if(!isTopAvailable || (!isLeftAvailable && !isRV8)){
			mbC[0] = mbA[0];
			mbC[1] = mbA[1];
		}else{
			mbC[0] = (pMVTemp - mv_left[block])->mvx;
			mbC[1] = (pMVTemp - mv_left[block])->mvy;
		}
	}

	*pMVx = MEDIAN_OF_3(mbA[0], mbB[0], mbC[0]);
	*pMVy = MEDIAN_OF_3(mbA[1], mbB[1], mbC[1]);

	return RV_S_OK;
}

/*************************************************************************/
/* Calculate motion vector predictor for a specific block. 					*/
/*************************************************************************/
RV_Status DecodeMotionVectors(struct Decoder *t, struct DecoderMBInfo *pMBInfo, struct DecoderPackedMotionVector *pMV,
							  const U32 uMB_X, const U32 uMBNum, I32 slice_index)
{
	I32 num_vectors, vector;
	I32 mvdx[4],mvdy[4], pmvx[4], pmvy[4];
	MVComponent  mvx, mvy;
	U32 length, info, N;
	RV_Status status = RV_S_OK;
	struct DecoderPackedMotionVector *pMVb;
	SLICE_DEC *slice_di = &t->m_slice_di[slice_index];

	pMVb = slice_di->m_pCurBidirMotionVectors + uMB_X * 4;
	num_vectors = number_mv[pMBInfo->mbtype];

	for (vector = 0; vector < num_vectors; vector++){
		//YU TBD VLC
		length = CB_GetVLCBits(slice_di, &info);
		N = LENGTH_INFO_TO_N(length,info);
		mvdx[vector] = (N+1)>>1;
		if (!(N & 1))
			mvdx[vector] = -mvdx[vector];

		/* Read vertical component */
		length = CB_GetVLCBits(slice_di, &info);
		N = LENGTH_INFO_TO_N(length,info);
		mvdy[vector] = (N+1)>>1;
		if (!(N & 1))
			mvdy[vector] = -mvdy[vector];
	}

	switch (pMBInfo->mbtype)
	{
	case MBTYPE_INTER:
	case MBTYPE_INTER_16x16:
		//YU TBD check rc

#ifndef MV_TEST
		GetPredictorsMV(slice_di,pMBInfo, pMV, 0, &pmvx[0], &pmvy[0], uMB_X, t->m_bIsRV8);
#else
		getPMV(t, 0,uMBNum, &pmvx[0],&pmvy[0],pMV,uMB_X, 0);
#endif
		//voVLog("\n NBNum = %d, mvx = %d, mvy = %d", uMBNum, pmvx[0], pmvy[0]);

		mvx =  (MVComponent)(mvdx[0] + pmvx[0]);
		mvy =  (MVComponent)(mvdy[0] + pmvy[0]);
		pMV[0].mvx = mvx;
		pMV[0].mvy = mvy;
		pMV[1].mvx = mvx;
		pMV[1].mvy = mvy;
		pMV[2].mvx = mvx;
		pMV[2].mvy = mvy;
		pMV[3].mvx = mvx;
		pMV[3].mvy = mvy;

		break;

	case MBTYPE_FORWARD:
	case MBTYPE_BACKWARD:
		if (t->m_bIsRV8){
#ifdef MV_TEST
			getPMV(t, 0,uMBNum, &pmvx[0],&pmvy[0],pMV,uMB_X, 0);
#else
			GetPredictorsMV(slice_di,pMBInfo, pMV, 0, &pmvx[0], &pmvy[0], uMB_X, t->m_bIsRV8);
#endif
		}else{
			status = GetPredictorsMVBFrame(slice_di, pMBInfo, pMV, &pmvx[0], &pmvy[0], uMB_X, t->m_mbX);
		}

		mvx =  (MVComponent)(mvdx[0] + pmvx[0]);
		mvy =  (MVComponent)(mvdy[0] + pmvy[0]);
		pMV[0].mvx = mvx;
		pMV[0].mvy = mvy;
		pMV[1].mvx = mvx;
		pMV[1].mvy = mvy;
		pMV[2].mvx = mvx;
		pMV[2].mvy = mvy;
		pMV[3].mvx = mvx;
		pMV[3].mvy = mvy;

		if (pMBInfo->mbtype == MBTYPE_BACKWARD){
			pMVb[0].mvx = mvx;//YU actually, only has one mv
			pMVb[0].mvy = mvy;
			pMVb[1].mvx = mvx;
			pMVb[1].mvy = mvy;
			pMVb[2].mvx = mvx;
			pMVb[2].mvy = mvy;
			pMVb[3].mvx = mvx;
			pMVb[3].mvy = mvy;
		}
		break;

	case MBTYPE_INTER_4V:

		for (vector = 0; vector < num_vectors; vector++)
		{
#ifndef MV_TEST
			GetPredictorsMV(slice_di,pMBInfo, pMV, vector, &pmvx[vector], &pmvy[vector], uMB_X, t->m_bIsRV8);
			mvx =  (MVComponent)(mvdx[vector] + pmvx[vector]);
			mvy =  (MVComponent)(mvdy[vector] + pmvy[vector]);
			pMV[0].mvx = mvx;
			pMV[0].mvy = mvy;

			pMV += 1;
#else

			getPMV(t, vector,uMBNum, &pmvx[vector],&pmvy[vector],pMV,uMB_X, 3);
			mvx =  (MVComponent)(mvdx[vector] + pmvx[vector]);
			mvy =  (MVComponent)(mvdy[vector] + pmvy[vector]);
			pMV[vector].mvx = mvx;
			pMV[vector].mvy = mvy;
#endif

		}

		break;

	case MBTYPE_INTER_16x8V:
		//	if(uMBNum==2){
		//		int aa = 0;
		//	}
#ifndef MV_TEST
		for (vector = 0; vector < num_vectors; vector++)
		{
			status = GetPredictorsMV(slice_di, pMBInfo, pMV, vector, &pmvx[vector], &pmvy[vector], uMB_X, t->m_bIsRV8);
			mvx =  (MVComponent)(mvdx[vector] + pmvx[vector]);
			mvy =  (MVComponent)(mvdy[vector] + pmvy[vector]);
			pMV[0].mvx = mvx;
			pMV[0].mvy = mvy;
			pMV[1].mvx = mvx;
			pMV[1].mvy = mvy;

			pMV += 2;
			//voVLog("\n NBNum = %d, block = %d, mvx = %d, mvy = %d", uMBNum,vector, pmvx[vector], pmvy[vector]);

		}
#else
		status = getPMV(t, 0,uMBNum, &pmvx[0],&pmvy[0],pMV,uMB_X, 0);
		mvx =  (MVComponent)(mvdx[0] + pmvx[0]);
		mvy =  (MVComponent)(mvdy[0] + pmvy[0]);
		pMV[0].mvx = mvx;
		pMV[0].mvy = mvy;
		pMV[1].mvx = mvx;
		pMV[1].mvy = mvy;
		voVLog("\n NBNum = %d, block = %d, mvx = %d, mvy = %d", uMBNum,0, pmvx[0], pmvy[0]);
		getPMV(t, 2,uMBNum, &pmvx[0],&pmvy[0],pMV,uMB_X, 0);
		mvx =  (MVComponent)(mvdx[1] + pmvx[0]);
		mvy =  (MVComponent)(mvdy[1] + pmvy[0]);
		pMV[2].mvx = mvx;
		pMV[2].mvy = mvy;
		pMV[3].mvx = mvx;
		pMV[3].mvy = mvy;
		voVLog("\n NBNum = %d, block = %d, mvx = %d, mvy = %d", uMBNum,1, pmvx[0], pmvy[0]);
#endif

		break;

	case MBTYPE_INTER_8x16V:
#ifndef MV_TEST
		for (vector = 0; vector < num_vectors; vector++)
		{
			status = GetPredictorsMV(slice_di, pMBInfo, pMV, vector, &pmvx[vector], &pmvy[vector], uMB_X, t->m_bIsRV8);
			mvx =  (MVComponent)(mvdx[vector] + pmvx[vector]);
			mvy =  (MVComponent)(mvdy[vector] + pmvy[vector]);
			pMV[0].mvx = mvx;
			pMV[0].mvy = mvy;
			pMV[2].mvx = mvx;
			pMV[2].mvy = mvy;

			pMV += 1;
			//voVLog("\n NBNum = %d, block = %d, mvx = %d, mvy = %d", uMBNum,vector, pmvx[vector], pmvy[vector]);
		}
#else

		status = getPMV(t, 0,uMBNum, &pmvx[0],&pmvy[0],pMV,uMB_X, 3);
		mvx =  (MVComponent)(mvdx[0] + pmvx[0]);
		mvy =  (MVComponent)(mvdy[0] + pmvy[0]);
		pMV[0].mvx = mvx;
		pMV[0].mvy = mvy;
		pMV[2].mvx = mvx;
		pMV[2].mvy = mvy;
		//	voVLog("\n NBNum = %d, block = %d, mvx = %d, mvy = %d", uMBNum,0, pmvx[0], pmvy[0]);

		getPMV(t, 1,uMBNum, &pmvx[0],&pmvy[0],pMV,uMB_X, 3);
		mvx =  (MVComponent)(mvdx[1] + pmvx[0]);
		mvy =  (MVComponent)(mvdy[1] + pmvy[0]);
		pMV[1].mvx = mvx;
		pMV[1].mvy = mvy;
		pMV[3].mvx = mvx;
		pMV[3].mvy = mvy;
		//	voVLog("\n NBNum = %d, block = %d, mvx = %d, mvy = %d", uMBNum,1, pmvx[0], pmvy[0]);
#endif

		break;
	case MBTYPE_BIDIR:
		status = GetPredictorsMVBFrame(slice_di, pMBInfo, pMV, &pmvx[0], &pmvy[0], uMB_X, t->m_mbX);

		mvx =  (MVComponent)(mvdx[0] + pmvx[0]);
		mvy =  (MVComponent)(mvdy[0] + pmvy[0]);
		pMV[0].mvx = mvx;
		pMV[0].mvy = mvy;
		pMV[1].mvx = mvx;
		pMV[1].mvy = mvy;
		pMV[2].mvx = mvx;
		pMV[2].mvy = mvy;
		pMV[3].mvx = mvx;
		pMV[3].mvy = mvy;

		mvx =  (MVComponent)(mvdx[1] + pmvx[1]);
		mvy =  (MVComponent)(mvdy[1] + pmvy[1]);
		pMVb[0].mvx = mvx;//YU actually, only has one mv
		pMVb[0].mvy = mvy;
		pMVb[1].mvx = mvx;
		pMVb[1].mvy = mvy;
		pMVb[2].mvx = mvx;
		pMVb[2].mvy = mvy;
		pMVb[3].mvx = mvx;
		pMVb[3].mvy = mvy;

		break;
	}

	return status;
}
/****************************************************************************************************
* Decode the luma and chroma coefficients for a macroblock, placing them 
* in pQuantBuf and performing the inverse transform on them. 
* pIsSubBlockEmpty is initialized set to false for any 
* luma subblock with coefficients. pQuantBuf is left uninitialized for 
* empty subblocks. 
***************************************************************************************************/
static void DecMB_SVLC(SLICE_DEC *slice_di, RV89_DEC_LOCAL *rv89_local, U32 cbp,U32 QP, I32 isRV8, EnumRVPicCodType pictureType)
{
	I32 *buf;
	U32 block, startblock, stopblock;
	U32 qpc, qdc0, qpc1_2;/*qpc1_2: qp c1 & c2*/
	I32 *pQuantBuf = slice_di->m_pQuantBuf;

	rv89_local->uDCOnly = 0;

	if (rv89_local->m_is16){
		if (isRV8){
			qdc0 = qpc1_2 = qpc = ISINTRAPIC(pictureType)? luma_quant_DC_RV8[QP] : QP;
		}else{
			qpc = QP;
			/*The second transform adopt diffent QO for the three lowest frequency coeff*/  
			qdc0 = qpc1_2 = rv89_local->isIntra ? luma_intra_quant_DC[QP]:luma_inter_quant_DC[QP];		
		}

		buf = pQuantBuf + LUMA_SIZE;

		/* inverse tranform to obtain the luma DC transform coeffs, only */
		/* when there is a nonzero coefficient */
		if (decode_4x4_block(slice_di,(I32 *)buf, qdc0, qpc1_2, qpc,rv89_local->pDC4x4DscLuma,
			rv89_local->pDC2x2DscLuma, rv89_local->pLevelDsc)){
				Intra16x16ITransform4x4(buf);
				rv89_local->uDCOnly = 1;/* for Intra16x16 always 1 */
		}
	}


	/* decode 4x4 blocks, luma */	
	buf = pQuantBuf;

	for (block = 0; block < 16; block++, buf += 16){
		if (cbp & (1U << block)){
			/*Combined coefficient decoding and dequantization*/
			decode_4x4_block(slice_di,(I32 *)buf, QP, QP, QP,
				rv89_local->p4x4DscLuma, rv89_local->p2x2DscLuma, rv89_local->pLevelDsc);
		}
		/* dequant DC coefficent */
		/* (stored in chroma coeff space) */
		if(rv89_local->m_is16)
			buf[0] = *(pQuantBuf + LUMA_SIZE + block);
	} /* for block */

	/* Now decode the SUPER_VLC chroma coefficients */
	startblock = (cbp & 0xf0000 ) ? 16: 20;
	stopblock  = (cbp & 0xf00000) ? 24: 20;
	/* decode 4x4 blocks, chroma */
	buf = pQuantBuf + (startblock<<4);
	for (block = startblock; block < stopblock; block++, buf += 16){
		if (cbp & (1U << block)){
			/*Combined coefficient decoding and dequantization*/
			decode_4x4_block(slice_di, (I32 *)buf, chroma_quant_DC[QP], chroma_quant[QP], chroma_quant[QP],
				rv89_local->p4x4DscChroma, rv89_local->p2x2DscChroma, rv89_local->pLevelDscChroma);
		}
	}
}	/* decodeLumaCoefficients_SVLC */

IMAGE*  buf_seq_ctl(FIFOTYPE *priv, IMAGE* img , const U32 flag)
{
	IMAGE *temp = NULL;
	U32 w_idx = 0;

	w_idx = priv->w_idx;    /* Save the old index before proceeding */
	if (FIFO_WRITE == flag){ /*write data*/

		if(NULL != img){
			/* Save it to buffer */
#ifdef FBM_INTERNAL_CACHE_SUPPORT
      if (((priv->w_idx + 1) % (priv->buf_num + 1)) == priv->r_idx) {
#else
			if (((priv->w_idx + 1) % FIFO_NUM) == priv->r_idx) {
#endif
				/* Adjust read index since buffer is full */
				/* Keep the latest one and drop the oldest one */
#ifdef FBM_INTERNAL_CACHE_SUPPORT
        priv->r_idx = (priv->r_idx + 1) % (priv->buf_num + 1);
#else
				priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
#endif
			}
			priv->img_seq[priv->w_idx] = img;
#ifdef FBM_INTERNAL_CACHE_SUPPORT
      priv->w_idx = (priv->w_idx + 1) % (priv->buf_num + 1);
#else
			priv->w_idx = (priv->w_idx + 1) % FIFO_NUM;
#endif
		}

	}else{/*read data*/
		if ((priv->r_idx == w_idx)){
			return NULL;/*there is no free buffer*/
		}else{
			temp = priv->img_seq[priv->r_idx];
#ifdef FBM_INTERNAL_CACHE_SUPPORT
      priv->r_idx = (priv->r_idx + 1) % (priv->buf_num + 1);
#else
			priv->r_idx = (priv->r_idx + 1) % FIFO_NUM;
#endif
		}
	}
	return temp;
}

static RV_Status DecodeMBHeader(struct Decoder *t, RV89_DEC_LOCAL *rv89_local, struct DecoderPackedMotionVector *pMV, 
								  U32 *ulSkipModesLeft, struct DecoderMBInfo  *pMB, U32 uMB_X, U32 uMB_Y, I32 slice_index)
{
	RV_Status rc = RV_S_OK;/* decoding status, if !RV_S_OK, can not continue, return error */
	I32 info;
	U32 uLumaVlc = 2;
	SLICE_DEC *slice_di = &t->m_slice_di[slice_index];
	DecoderIntraType *pMBIntraTypes = slice_di->m_pMBIntraTypes;
	const U32 uMBWidth  = t->m_mbX;
	const U32 uMBNum = uMBWidth*uMB_Y+uMB_X;//YU TBD

	slice_di->uEdgeAvail = 0x0660;
	if(uMB_X && !(uMBNum == slice_di->m_mba))
		slice_di->uEdgeAvail |= 0x0110;
	if((uMBNum - slice_di->m_mba) >= uMBWidth){
		slice_di->uEdgeAvail |= 0x0006;
		if((uMB_X+1) < t->m_mbX)
			slice_di->uEdgeAvail |= 0x0008;
		if(uMB_X&&(uMBNum - slice_di->m_mba != uMBWidth))
			slice_di->uEdgeAvail |= 0x0001;
	}

	if (slice_di->m_pBitend < slice_di->m_pBitptr){
		return RV_S_ERROR;
	}

	if(ISINTRAPIC(t->m_ptype)){
		pMB->QP   = slice_di->m_pquant;
		rv89_local->m_is16 = getbits1(slice_di);
		if(!rv89_local->m_is16 && !t->m_bIsRV8){
			if(!getbits1(slice_di)){
				CB_GetDQUANT(slice_di, &pMB->QP, slice_di->m_pquant);
				slice_di->m_entropyqp = get_entropyqp(slice_di->m_pquant, slice_di->m_OSVQUANT);
				rv89_local->m_is16 = getbits1(slice_di);
				slice_di->m_pquant = pMB->QP;
			}
		}
		pMB->mbtype = rv89_local->m_is16 ? MBTYPE_INTRA_16x16 : MBTYPE_INTRA;
	}else{
		rc = t->GetInterMBType(slice_di, pMB, slice_di->m_pquant, ulSkipModesLeft, t->m_mbX);/* ini in get pic header*/

#if defined(VODEBUG)
		if(RV_S_OK != rc){
			sprintf(aa, "\n GetInterMBType err !!!");
			NU_SIO_Puts(aa);
		}
#endif

		rv89_local->m_is16 = (MBTYPE_INTRA_16x16 == pMB->mbtype);

		if(slice_di->m_pquant != pMB->QP)
			slice_di->m_entropyqp = get_entropyqp(pMB->QP, slice_di->m_OSVQUANT);

		slice_di->m_pquant = pMB->QP;

		if ((IS_INTRA_MBTYPE(pMB->mbtype))||(MBTYPE_SKIPPED == pMB->mbtype) || (MBTYPE_DIRECT == pMB->mbtype)){
			pMV[0].mvx = pMV[0].mvy = 0;
			pMV[1].mvx = pMV[1].mvy = 0;
			pMV[2].mvx = pMV[2].mvy = 0;
			pMV[3].mvx = pMV[3].mvy = 0;
		}else{
#if defined (MULTI_THREADED_DECODER_SUPPORT_BETA_BITSTREAMS)
			if(!(slice_di->m_first_flage) && (t->m_decoding_thread_count > 1)){//huwei 20110311 bug fixed
#else
			if(t->m_bIsRV8 && !(slice_di->m_first_flage) && (t->m_decoding_thread_count > 1)){//huwei 20110311 bug fixed
#endif
				if ((slice_index > 0) && (uMB_X + uMB_Y*uMBWidth == slice_di->m_mba + uMBWidth)){
					U32  isRefPelUsed = 0;
					if(uMB_X == (uMBWidth - 1)){
						U32 isRightAvailable = (slice_di->uEdgeAvail&0x8)>>3;
						if (!isRightAvailable){
							isRefPelUsed = 1;
						}							
					}

					if (isRefPelUsed){
						while (!t->m_main_threaded_finish){
							x86_pause_hint();
							thread_sleep(0);
						}
					}

					if ((RV_TRUEBPIC == t->m_ptype) && isRefPelUsed){
						struct DecoderPackedMotionVector *pMVTemp0,*pMVTemp1;
						pMVTemp0 = t->m_slice_di[0].m_pCurMotionVectors + (uMB_X - 1) * 4;
						pMVTemp1 = t->m_slice_di[1].m_pTopMotionVectors + (uMB_X - 1) * 4;

						pMVTemp1[0].mvx = pMVTemp0[0].mvx;
						pMVTemp1[0].mvy = pMVTemp0[0].mvy;
						pMVTemp1[1].mvx = pMVTemp0[1].mvx;
						pMVTemp1[1].mvy = pMVTemp0[1].mvy;
						pMVTemp1[2].mvx = pMVTemp0[2].mvx;
						pMVTemp1[2].mvy = pMVTemp0[2].mvy;
						pMVTemp1[3].mvx = pMVTemp0[3].mvx;
						pMVTemp1[3].mvy = pMVTemp0[3].mvy;

#if defined (MULTI_THREADED_DECODER_SUPPORT_BETA_BITSTREAMS)
						if (!t->m_bIsRV8){
							pMVTemp0 = t->m_slice_di[0].m_pCurBidirMotionVectors + (uMB_X - 1) * 4;
							pMVTemp1 = t->m_slice_di[1].m_pTopBidirMotionVectors + (uMB_X - 1) * 4;

							pMVTemp1[0].mvx = pMVTemp0[0].mvx;
							pMVTemp1[0].mvy = pMVTemp0[0].mvy;
							pMVTemp1[1].mvx = pMVTemp0[1].mvx;
							pMVTemp1[1].mvy = pMVTemp0[1].mvy;
							pMVTemp1[2].mvx = pMVTemp0[2].mvx;
							pMVTemp1[2].mvy = pMVTemp0[2].mvy;
							pMVTemp1[3].mvx = pMVTemp0[3].mvx;
							pMVTemp1[3].mvy = pMVTemp0[3].mvy;
						}
#endif
					}
				}
			}
			rc = DecodeMotionVectors(t,pMB, pMV, uMB_X, uMBNum, slice_index);
		}

		if(pMB->mbtype == MBTYPE_SKIPPED){

			((U32*)pMBIntraTypes)[0] = 0;
			((U32*)pMBIntraTypes)[1] = 0;
			((U32*)pMBIntraTypes)[2] = 0;
			((U32*)pMBIntraTypes)[3] = 0;

			((PU32)(slice_di->m_pAboveSubBlockIntraTypes + uMB_X * 4))[0] = 0;

			pMB->cbp = pMB->deblock_cbpcoef = 0;

			return rc;
		}

		uLumaVlc = 0;
	}


	if(IS_INTRA_MBTYPE(pMB->mbtype)){

		if(!rv89_local->m_is16){
			if(t->GetIntraMBType(slice_di, pMB, slice_di->m_pAboveSubBlockIntraTypes + uMB_X * 4) != RV_S_OK){

#if defined(VODEBUG)
				sprintf(aa, "\n GetIntraMBType err !!!");
				NU_SIO_Puts(aa);
#endif
				return -1;
			}

			uLumaVlc   = 1;
		}else{
			loadbits(slice_di);
			info = getbits(slice_di, 2);        
			info *= 0x01010101;
			uLumaVlc   = 2;

			((U32*)pMBIntraTypes)[0] = info;
			((U32*)pMBIntraTypes)[1] = info;
			((U32*)pMBIntraTypes)[2] = info;
			((U32*)pMBIntraTypes)[3] = info;

			((PU32)(slice_di->m_pAboveSubBlockIntraTypes + uMB_X * 4))[0] = info;
		}
		//-------------------YU TBD Deblock
		pMB->deblock_cbpcoef = 0xffffff;
		if (slice_di->m_entropyqp > 31){
			slice_di->m_entropyqp = 31;
			return RV_S_ERROR;
		}
		pMB->cbp = decode_cbp(t, rv89_local->m_is16 ? 2 : 1, slice_di->m_entropyqp, slice_index);
		//-------------------------------------
		rv89_local->isIntra = TRUE;
	}else{

		((U32*)pMBIntraTypes)[0] = 0;
		((U32*)pMBIntraTypes)[1] = 0;
		((U32*)pMBIntraTypes)[2] = 0;
		((U32*)pMBIntraTypes)[3] = 0;

		((PU32)(slice_di->m_pAboveSubBlockIntraTypes + uMB_X * 4))[0] = 0;

		//-------------------------------YU TBD Deblock
		if(MBTYPE_INTER_16x16 == pMB->mbtype){
			rv89_local->m_is16       = 1;
			uLumaVlc   = 2;
		}

		if (slice_di->m_entropyqp > 31){
			slice_di->m_entropyqp = 31;
			return RV_S_ERROR;
		}

		pMB->deblock_cbpcoef = pMB->cbp = decode_cbp(t, rv89_local->m_is16 ? 2 : 0, slice_di->m_entropyqp, slice_index);

		if(MBTYPE_INTER_16x16 == pMB->mbtype)
			pMB->deblock_cbpcoef |= 0xffff;
		//--------------------------------------
		rv89_local->isIntra = FALSE;
	}


	//YU TBD add cbp table
	if(uLumaVlc == 2){/*second DC transfor*/
		INTRA_DECODE_TABLES *ct = t->m_pTables->intra_decode_tables + intra_qp_to_idx [slice_di->m_entropyqp];
		rv89_local->pDC4x4DscLuma = &(ct->luma._4x4_dsc[2]); /* !!! */
		rv89_local->pDC2x2DscLuma = &(ct->luma._2x2_dsc [0]);
	}

	if (uLumaVlc > 2){
		uLumaVlc = 2;
	}

	if(uLumaVlc){/*intra and inter 16x16*/
		INTRA_DECODE_TABLES *ct = t->m_pTables->intra_decode_tables + intra_qp_to_idx [slice_di->m_entropyqp];
		rv89_local->p4x4DscLuma = &(ct->luma._4x4_dsc [uLumaVlc-1]); /* !!! */
		rv89_local->p2x2DscLuma = &(ct->luma._2x2_dsc [0]);
		rv89_local->p4x4DscChroma = &(ct->chroma._4x4_dsc);
		rv89_local->p2x2DscChroma = &(ct->chroma._2x2_dsc [0]);
		rv89_local->pLevelDsc = &(ct->level_dsc);
	}else{
		INTER_DECODE_TABLES *ct = t->m_pTables->inter_decode_tables + inter_qp_to_idx [slice_di->m_entropyqp];
		rv89_local->p4x4DscLuma = &(ct->luma._4x4_dsc); /* !!! */
		rv89_local->p2x2DscLuma = &(ct->luma._2x2_dsc [0]);

		rv89_local->p4x4DscChroma = &(ct->chroma._4x4_dsc);
		rv89_local->p2x2DscChroma = &(ct->chroma._2x2_dsc [0]);
		rv89_local->pLevelDsc = &(ct->level_dsc);
	}

	if(IS_INTRA_MBTYPE(pMB->mbtype)){
		INTRA_DECODE_TABLES *ct = t->m_pTables->intra_decode_tables + intra_qp_to_idx [slice_di->m_entropyqp];
		rv89_local->p4x4DscChroma = &(ct->chroma._4x4_dsc);
		rv89_local->p2x2DscChroma = &(ct->chroma._2x2_dsc [0]);
		rv89_local->pLevelDscChroma = &(ct->level_dsc);	
	}else{
		INTER_DECODE_TABLES *ct = t->m_pTables->inter_decode_tables + inter_qp_to_idx [slice_di->m_entropyqp];
		rv89_local->p4x4DscChroma = &(ct->chroma._4x4_dsc);
		rv89_local->p2x2DscChroma = &(ct->chroma._2x2_dsc [0]);
		rv89_local->pLevelDscChroma = &(ct->level_dsc);
	}

	return rc;

}

RV_Status Decoder_Slice(struct Decoder *t, I32 slice_number,  I32 slice_index)
{
	RV_Status	rc = RV_S_OK;
	U32         ulSkipModesLeft = 0;
	I32		    iRatio0=0, iRatio1=0, trb, trd;
	U32         uMB_X, uMB_Y;
	U32			offsetY, offsetUV;

	struct DecoderMBInfo  *pMB;
	DecoderIntraType *pMBIntraTypes;
	struct DecoderPackedMotionVector *mv_temp;
	struct DecoderPackedMotionVector *pMVR;
	struct DecoderPackedMotionVector *pMV=NULL;
	SLICE_DEC *slice_di = &t->m_slice_di[slice_index];

	const U32 uMBWidth  = t->m_mbX;
	const U32 uMBHeight = t->m_mbY;
	RV89_DEC_LOCAL rv89_local;

	if(RV_TRUEBPIC == t->m_ptype){
		/* Compute ratios needed for direct mode */
		trb = (I32) t->m_tr - (I32) t->m_refTR;
		trd = (I32) t->m_curTR - (I32) t->m_refTR;

		if (trb < 0)
			trb += (I32) t->m_uTRWrap;

		if (trd < 0)
			trd += (I32) t->m_uTRWrap;

		if (trb > trd)		/* just in case TR's are incorrect */
			trb=0;

		if (trd>0) {
			iRatio0 = (trb << TR_SHIFT) / trd;
			iRatio1 = ((trd-trb) << TR_SHIFT) / trd;
		}else{
			iRatio0 = iRatio1 = 0;
		}

	}

	/* pel position of current macroblock in the luma plane */
	uMB_X = slice_di->m_resync_mb_x;
	uMB_Y = slice_di->m_resync_mb_y;

	if((uMB_X >= uMBWidth) || (uMB_Y >= uMBHeight)){
		return RV_S_ERROR;
	}

	if(!slice_di->m_first_flage){
		if(RV_INTERPIC == t->m_ptype){
			slice_di->m_pCurMotionVectors = slice_di->m_pReferenceMotionVectors + (t->m_img_width >> 2) * uMB_Y;
			pMV  = slice_di->m_pCurMotionVectors + uMB_X * 4;		
		}else if(RV_TRUEBPIC == t->m_ptype){
			slice_di->m_pCurMotionVectors = slice_di->m_pBMotionVectors;
			pMV  = slice_di->m_pCurMotionVectors + uMB_X * 4;		
		}

		slice_di->m_pTopMotionVectors = slice_di->m_pCurMotionVectors + (t->m_img_width >> 2);
	}else{
		if(RV_INTERPIC == t->m_ptype){//huwei 20110315 stability
			slice_di->m_pCurMotionVectors = slice_di->m_pReferenceMotionVectors + (t->m_img_width >> 2) * uMB_Y;
			pMV  = slice_di->m_pCurMotionVectors + uMB_X * 4;
		}else if(RV_TRUEBPIC == t->m_ptype){
			pMV = slice_di->m_pCurMotionVectors + uMB_X * 4;
		}
		
	}	

	slice_di->m_pMBInfo[uMB_Y * uMBWidth + uMB_X].QP = slice_di->m_pquant;	
	pMBIntraTypes = slice_di->m_pMBIntraTypes;

	for(; uMB_Y < uMBHeight; uMB_Y++){
		for(; uMB_X < uMBWidth; uMB_X++){

			if (ulSkipModesLeft <= 1){
				if (CB_GetSSC(t, slice_number, slice_di)){
					slice_di->m_first_flage = 1;
					return rc;
				}
			}

			slice_di->m_xmb_pos = uMB_X;
			slice_di->m_ymb_pos = uMB_Y;

			pMB = &slice_di->m_pMBInfo[uMB_X + uMBWidth * uMB_Y];
			if(DecodeMBHeader(t, &rv89_local, pMV, &ulSkipModesLeft, pMB, uMB_X, uMB_Y, slice_index) != RV_S_OK){
				return RV_S_ERROR;
			}
			/* Finally, perform motion compensation to reconstruct the YUV data */

			//TBD
			offsetY  = (uMB_Y<<4) * t->m_img_stride + (uMB_X<<4);
			offsetUV = (uMB_Y<<3) * t->m_img_UVstride + (uMB_X<<3);

			if (ISINTRAPIC(t->m_ptype)){
				rv89_local.dst[0] = t->m_pCurrentFrame->m_pYPlane + offsetY;
				rv89_local.dst[1] = t->m_pCurrentFrame->m_pUPlane + offsetUV;
				rv89_local.dst[2] = t->m_pCurrentFrame->m_pVPlane + offsetUV;
			}else if(RV_INTERPIC == t->m_ptype){
				rv89_local.dst[0] = rv89_local.cur[0] = t->m_pCurrentFrame->m_pYPlane + offsetY;
				rv89_local.ref[0] = t->m_pRefFrame->m_pYPlane + offsetY;
				rv89_local.dst[1] = rv89_local.cur[1] = t->m_pCurrentFrame->m_pUPlane + offsetUV;
				rv89_local.dst[2] = rv89_local.cur[2] = t->m_pCurrentFrame->m_pVPlane + offsetUV;
				rv89_local.ref[1] = t->m_pRefFrame->m_pUPlane + offsetUV;
				rv89_local.ref[2] = t->m_pRefFrame->m_pVPlane + offsetUV;
			}else {
				rv89_local.cur[0] = t->m_pCurrentFrame->m_pYPlane + offsetY;
				rv89_local.dst[0] = t->m_pBFrame->m_pYPlane + offsetY;
				rv89_local.ref[0] = t->m_pRefFrame->m_pYPlane + offsetY;

				rv89_local.cur[1] = t->m_pCurrentFrame->m_pUPlane + offsetUV;
				rv89_local.cur[2] = t->m_pCurrentFrame->m_pVPlane + offsetUV;
				rv89_local.dst[1] = t->m_pBFrame->m_pUPlane + offsetUV;
				rv89_local.dst[2] = t->m_pBFrame->m_pVPlane + offsetUV;
				rv89_local.ref[1] = t->m_pRefFrame->m_pUPlane + offsetUV;
				rv89_local.ref[2] = t->m_pRefFrame->m_pVPlane + offsetUV;
			}

			/* Depending on MB type set up pointers to reference planes */
			if(pMB->mbtype != MBTYPE_SKIPPED){
				DecMB_SVLC(slice_di, &rv89_local, pMB->cbp, pMB->QP, t->m_bIsRV8, t->m_ptype);
			}

			if(RV_INTERPIC == t->m_ptype){
				SetDeblockCoefs(t, pMB, pMV, slice_index);
			}

#if defined (MULTI_THREADED_DECODER_SUPPORT_BETA_BITSTREAMS)
			if(!(slice_di->m_first_flage) && (t->m_decoding_thread_count > 1)){
#else
			if(t->m_bIsRV8 && !(slice_di->m_first_flage) && (t->m_decoding_thread_count > 1)){
#endif
				if ((slice_index > 0) && (uMB_X + uMB_Y*uMBWidth == slice_di->m_mba + uMBWidth) && (slice_di->m_mba > 0)){
						U32  isRefPelUsed = 0;

						if (IS_INTRA_MBTYPE(pMB->mbtype)){
							if (MBTYPE_INTRA == pMB->mbtype){
								if (pMBIntraTypes[0] == 3 || pMBIntraTypes[0] == 5 || pMBIntraTypes[0] == 8){
									isRefPelUsed = 1;
								}
							}else{// INTRA16x16
								if (pMBIntraTypes[0] == 3){
									isRefPelUsed = 1;
								}
							}
						}

						if (isRefPelUsed){
							while (!t->m_main_threaded_finish){
								x86_pause_hint();
								thread_sleep(0);
							}
						}
				}
			}

			/* Reconstruct luma */
			if(pMB->mbtype == MBTYPE_SKIPPED || pMB->mbtype == MBTYPE_DIRECT){
				/* Just copy the reference plane data. */
				U32  uRefMBType = slice_di->m_pReferenceMBInfo[uMB_X + uMBWidth * uMB_Y].mbtype;

				if (RV_INTERPIC == t->m_ptype){
					/* Just copy the reference plane data. */
					CopyBlock16x16(rv89_local.ref[0], rv89_local.dst[0], t->m_img_stride, t->m_img_stride);
					CopyBlock8x8(rv89_local.ref[1], rv89_local.dst[1], t->m_img_UVstride, t->m_img_UVstride);
					CopyBlock8x8(rv89_local.ref[2], rv89_local.dst[2], t->m_img_UVstride, t->m_img_UVstride);
				}else if(uRefMBType == MBTYPE_SKIPPED){//YU TBD remove m_pReferenceMBInfo
					CopyBlock16x16(rv89_local.cur[0], rv89_local.dst[0], t->m_img_stride, t->m_img_stride);
					CopyBlock8x8(rv89_local.cur[1], rv89_local.dst[1], t->m_img_UVstride, t->m_img_UVstride);
					CopyBlock8x8(rv89_local.cur[2], rv89_local.dst[2], t->m_img_UVstride, t->m_img_UVstride);
				}else{
					if(IS_INTRA_MBTYPE(uRefMBType)){

						if(iRatio0 == iRatio1){
							AddBlock16x16(rv89_local.cur[0], rv89_local.ref[0], rv89_local.dst[0], t->m_img_stride);
							AddBlock8x8(rv89_local.cur[1], rv89_local.ref[1], rv89_local.dst[1], t->m_img_UVstride);
							AddBlock8x8(rv89_local.cur[2], rv89_local.ref[2], rv89_local.dst[2], t->m_img_UVstride);
						}else{
							struct DecoderPackedMotionVector *pMVf;
							pMVR = slice_di->m_pReferenceMotionVectors + (uMB_X + uMBWidth * uMB_Y) * 4;
							pMVf = slice_di->m_pCurBidirMotionVectors + uMB_X * 4;

							pMV[0].mvx = pMV[0].mvy = 0;
							pMV[1].mvx = pMV[1].mvy = 0;
							pMV[2].mvx = pMV[2].mvy = 0;
							pMV[3].mvx = pMV[3].mvy = 0;

							pMVf[0].mvx = pMVf[0].mvy = 0;
							pMVf[1].mvx = pMVf[1].mvy = 0;
							pMVf[2].mvx = pMVf[2].mvy = 0;
							pMVf[3].mvx = pMVf[3].mvy = 0;

							InterpolateMBWeight(t, &rv89_local, t->m_img_stride,pMVf, pMV, slice_di->m_pPredBuf, 1, t->m_bIsRV8,iRatio0, iRatio1, slice_index);
						}
					}else{

						struct DecoderPackedMotionVector *pMVf;
						pMVR = slice_di->m_pReferenceMotionVectors + (uMB_X + uMBWidth * uMB_Y) * 4;
						pMVf = slice_di->m_pCurBidirMotionVectors + uMB_X * 4;

						DecodeDirectMV(t, uRefMBType, pMVR, pMVf, pMV, uMB_X<<4, uMB_Y<<4, iRatio0, iRatio1);

						if(iRatio0 == iRatio1){
							InterpolateMB(t, &rv89_local, t->m_img_stride,pMVf, pMV, slice_di->m_pPredBuf, 1, t->m_bIsRV8, slice_index);
						}else{
							InterpolateMBWeight(t, &rv89_local, t->m_img_stride,pMVf, pMV, slice_di->m_pPredBuf, 1, t->m_bIsRV8,iRatio0, iRatio1, slice_index);
						}												

						//TBD BUG FIX
						pMV[0].mvx = pMV[0].mvy = 0;
						pMV[1].mvx = pMV[1].mvy = 0;
						pMV[2].mvx = pMV[2].mvy = 0;
						pMV[3].mvx = pMV[3].mvy = 0;
					}
				}
				if(pMB->cbp)
					IdctAddInterMB(t, pMB, &rv89_local, slice_index);

			}else if (pMB->mbtype == MBTYPE_BIDIR ){
				struct DecoderPackedMotionVector *pMVp, *pMVf;
				pMVp = pMV;
				pMVf = slice_di->m_pCurBidirMotionVectors + uMB_X * 4;
				//	voVLog("\n NBNum = %d", uMB_X + uMBWidth*uMB_Y);

				InterpolateMB(t, &rv89_local, t->m_img_stride,pMVf, pMVp,slice_di->m_pPredBuf, 1, t->m_bIsRV8, slice_index);
				IdctAddInterMB(t,pMB,&rv89_local, slice_index);

			}else if (MBTYPE_INTRA_16x16 == pMB->mbtype){
				rc = ReconIntraMB16x16(t,pMB, &rv89_local, slice_index);
				Decoder_transferMB(t,pMB, &rv89_local, slice_index);
			}else if (IS_INTRA_MBTYPE(pMB->mbtype)){
				rc = ReconIntraMB(t, pMB, &rv89_local, slice_index);
			}else{

				if ((pMB->mbtype != MBTYPE_BACKWARD) && (RV_TRUEBPIC == t->m_ptype)){
					rv89_local.ref[0] = rv89_local.cur[0];
					rv89_local.ref[1] = rv89_local.cur[1];
					rv89_local.ref[2] = rv89_local.cur[2];
				}

				if (RV_TRUEBPIC == t->m_ptype){
					InterpolateMB(t, &rv89_local, t->m_img_stride,pMV, 0, slice_di->m_pPredBuf, 1, t->m_bIsRV8, slice_index);
				}else{
					InterpolateMB(t, &rv89_local, t->m_img_stride,pMV, 0, slice_di->m_pPredBuf, 0, t->m_bIsRV8, slice_index);
				}

				IdctAddInterMB(t, pMB, &rv89_local, slice_index);
			}

			pMV += 4;
		}
		pMBIntraTypes[3] = pMBIntraTypes[7] = 
			pMBIntraTypes[11] = pMBIntraTypes[15] = -1;

		uMB_X = 0;

		if (RV_INTERPIC == t->m_ptype){
			slice_di->m_pTopMotionVectors = slice_di->m_pCurMotionVectors;
			slice_di->m_pCurMotionVectors = slice_di->m_pCurMotionVectors + (t->m_img_width >> 2);
			pMV = slice_di->m_pCurMotionVectors;
		}else if (RV_TRUEBPIC == t->m_ptype){
			mv_temp                       = slice_di->m_pCurMotionVectors;
			slice_di->m_pCurMotionVectors = slice_di->m_pTopMotionVectors;
			slice_di->m_pTopMotionVectors = mv_temp;
			mv_temp                            = slice_di->m_pCurBidirMotionVectors;
			slice_di->m_pCurBidirMotionVectors = slice_di->m_pTopBidirMotionVectors;
			slice_di->m_pTopBidirMotionVectors = mv_temp;
			pMV                                = slice_di->m_pCurMotionVectors;
		}
	}

	slice_di->m_first_flage = 1;

	return rc;

}

static RV_Status GetSliceHeader(struct Decoder *t, U32* uMB_X, U32* uMB_Y, SLICE_DEC *slice_di)
{
	RV_Status rc = RV_S_OK;
	U32 uMBA;
	const U32 uMBWidth	= t->m_mbX;
	const U32 uMBHeight = t->m_mbY;

	rc = t->GetSliceHeader(t, (I32*)&uMBA, slice_di);
	if(rc != RV_S_OK || (uMBA > uMBWidth*uMBHeight))//TBD
		return RV_S_ERROR;

	*uMB_Y = uMBA / uMBWidth;//TBD TODO_DIV
	*uMB_X = uMBA % uMBWidth;//TBD TODO_MOD

	SetMem((VO_U8 *)slice_di->m_pAboveSubBlockIntraTypes,  -1, uMBWidth << 2, t->m_pMemOP);

	slice_di->m_pMBIntraTypes[3] =  \
		slice_di->m_pMBIntraTypes[7] =  \
		slice_di->m_pMBIntraTypes[11] = \
		slice_di->m_pMBIntraTypes[15] = -1;

	slice_di->m_pMBInfo[(*uMB_Y)*uMBWidth+(*uMB_X)].QP = slice_di->m_pquant;

	slice_di->m_resync_mb_x      = *uMB_X;
	slice_di->m_resync_mb_y      = *uMB_Y;

	return rc;

} /* CB_GetSliceHeader() */

RV_Status Decoder_IPicture(struct Decoder *t)
{
	/* decoding status, if !RV_S_OK, can not continue, return error */
	RV_Status rc = RV_S_OK;

	struct DecoderMBInfo  *pMB;
	RV89_DEC_LOCAL rv89_local;

	U32 slice_number = 0;
	U32 uMB_X, uMB_Y;
	const U32 uMBWidth  = t->m_mbX;
	const U32 uMBHeight = t->m_mbY;
	SLICE_DEC *slice_di = &t->m_slice_di[0];

	t->m_pCurrentFrame = buf_seq_ctl(&t->m_priv,NULL,FIFO_READ);

	if(NULL == t->m_pCurrentFrame){
		return VO_MEMRC_NO_YUV_BUFFER;
	}

	t->m_pCurrentFrame->Time = t->m_timeStamp;
	t->m_pCurrentFrame->UserData = t->UserData;
	for(uMB_Y = 0; uMB_Y < uMBHeight; uMB_Y++){
		for(uMB_X = 0; uMB_X < uMBWidth; uMB_X++){
			U32 offset;

			//TBD ??can we move this check to Y boundary??
			//uMBWidth*uMB_Y+uMB_X TBD clean up
			if (CB_GetSSC(t, slice_number + 1, slice_di)){
				if(GetSliceHeader(t, &uMB_X, &uMB_Y, slice_di)){
#if defined(VODEBUG)
					sprintf(aa, "\n  GetSliceHeader err !!!");
					NU_SIO_Puts(aa);
#endif
					return RV_S_ERROR;
				}
				slice_number++;
			}

			slice_di->m_xmb_pos = uMB_X;
			slice_di->m_ymb_pos = uMB_Y;
			pMB = &slice_di->m_pMBInfo[uMB_X + uMBWidth*uMB_Y];

			offset = (uMB_Y<<4) * t->m_img_stride + (uMB_X<<4);
			rv89_local.dst[0] = t->m_pCurrentFrame->m_pYPlane + offset;
			offset = (uMB_Y<<3) * t->m_img_UVstride + (uMB_X<<3);
			rv89_local.dst[1] = t->m_pCurrentFrame->m_pUPlane + offset;
			rv89_local.dst[2] = t->m_pCurrentFrame->m_pVPlane + offset;

			if(DecodeMBHeader(t, &rv89_local, NULL, NULL,pMB, uMB_X, uMB_Y, 0) != RV_S_OK){
				return RV_S_ERROR;
			}
			DecMB_SVLC(slice_di, &rv89_local,pMB->cbp,pMB->QP, t->m_bIsRV8, t->m_ptype);

			/* Finally, perform motion compensation to reconstruct the YUV data */
			/* Reconstruct luma */
			if (rv89_local.m_is16){
				rc = ReconIntraMB16x16(t, pMB, &rv89_local, 0);
				Decoder_transferMB(t,pMB, &rv89_local, 0);
			}else{/*intra*/
				rc = ReconIntraMB(t,pMB, &rv89_local, 0);	
			}
		}

		slice_di->m_pMBIntraTypes[3] = slice_di->m_pMBIntraTypes[7] = 
			slice_di->m_pMBIntraTypes[11] = slice_di->m_pMBIntraTypes[15] = -1;
	}

	//if ((!t->m_deblocking_filter_passthrough) && (t->m_enable_deblocking_flage)){
	//	for(uMB_Y = 0; uMB_Y < uMBHeight; uMB_Y++){
	//		t->InLoopFilter(t, uMB_Y);
	//	}
	//}

	return rc;
}


RV_Status Decoder_PPicture(struct Decoder *t)
{
	RV_Status	rc = RV_S_OK;
	U32  ulSkipModesLeft = 0;

	struct DecoderMBInfo  *pMB;
	DecoderIntraType *pMBIntraTypes;
	struct DecoderPackedMotionVector *pMV;
	U32 slice_number = 0;
	U32 uMB_X, uMB_Y;

	const U32 uMBWidth  = t->m_mbX;
	const U32 uMBHeight = t->m_mbY;
	SLICE_DEC *slice_di = &t->m_slice_di[0];

	RV89_DEC_LOCAL rv89_local;

	t->m_pCurrentFrame = buf_seq_ctl(&t->m_priv, NULL, FIFO_READ);
	if(NULL == t->m_pCurrentFrame){
		return VO_MEMRC_NO_YUV_BUFFER;
	}
	t->m_pCurrentFrame->Time = t->m_timeStamp;
	t->m_pCurrentFrame->UserData = t->UserData;

	pMV                        = slice_di->m_pCurMotionVectors;
	slice_di->m_pMBInfo[0].QP  = slice_di->m_pquant;
	pMBIntraTypes              = slice_di->m_pMBIntraTypes;

	for(uMB_Y = 0; uMB_Y < uMBHeight; uMB_Y++){
		for(uMB_X = 0; uMB_X < uMBWidth; uMB_X++){
			U32 offset;

			if (ulSkipModesLeft <= 1){
				if (CB_GetSSC(t,slice_number + 1, slice_di)){
					if(GetSliceHeader(t, &uMB_X, &uMB_Y, slice_di)){
#if defined(VODEBUG)
						sprintf(aa, "\n  GetSliceHeader err !!!");
						NU_SIO_Puts(aa);
#endif
						return RV_S_ERROR;
					}

					ulSkipModesLeft = 0;
					slice_number++;
					pMV = slice_di->m_pCurMotionVectors + uMB_X * 4;
				}
			}

			slice_di->m_xmb_pos = uMB_X;
			slice_di->m_ymb_pos = uMB_Y;

			pMB = &slice_di->m_pMBInfo[uMB_X + uMBWidth*uMB_Y];
			//TBD
			offset = (uMB_Y<<4) * t->m_img_stride + (uMB_X<<4);
			rv89_local.dst[0] = rv89_local.cur[0] = t->m_pCurrentFrame->m_pYPlane + offset;
			rv89_local.ref[0] = t->m_pRefFrame->m_pYPlane + offset;
			offset = (uMB_Y<<3) * t->m_img_UVstride + (uMB_X<<3);
			rv89_local.dst[1] = rv89_local.cur[1] = t->m_pCurrentFrame->m_pUPlane + offset;
			rv89_local.dst[2] = rv89_local.cur[2] = t->m_pCurrentFrame->m_pVPlane + offset;
			rv89_local.ref[1] = t->m_pRefFrame->m_pUPlane + offset;
			rv89_local.ref[2] = t->m_pRefFrame->m_pVPlane + offset;

			if(DecodeMBHeader(t, &rv89_local, pMV, &ulSkipModesLeft, pMB, uMB_X, uMB_Y, 0) != RV_S_OK){
#if defined(VODEBUG)
				sprintf(aa, "\n  DecodeMBHeader err !!!");
				NU_SIO_Puts(aa);
#endif
				return RV_S_ERROR;
			}

			SetDeblockCoefs(t, pMB, pMV,0);

			if ((pMB->mbtype == MBTYPE_SKIPPED)){
				/* Just copy the reference plane data. */
				CopyBlock16x16(rv89_local.ref[0], rv89_local.dst[0], t->m_img_stride, t->m_img_stride);
				CopyBlock8x8(rv89_local.ref[1], rv89_local.dst[1], t->m_img_UVstride, t->m_img_UVstride);
				CopyBlock8x8(rv89_local.ref[2], rv89_local.dst[2], t->m_img_UVstride, t->m_img_UVstride);
			}else{
				DecMB_SVLC(slice_di, &rv89_local,pMB->cbp, pMB->QP, t->m_bIsRV8, t->m_ptype);
				/* Reconstruct luma */
				if (MBTYPE_INTRA_16x16 == pMB->mbtype){
					rc = ReconIntraMB16x16(t,pMB, &rv89_local,0);
					Decoder_transferMB(t,pMB, &rv89_local,0);	
				}else if (IS_INTRA_MBTYPE(pMB->mbtype)){
					rc = ReconIntraMB(t,pMB, &rv89_local,0);
				}else{
					InterpolateMB(t, &rv89_local, t->m_img_stride,pMV, 0, slice_di->m_pPredBuf, 0, t->m_bIsRV8,0);
					IdctAddInterMB(t,pMB, &rv89_local,0);
				}
			}
			pMV += 4;
		} /* for mbnum */
		pMBIntraTypes[3] = pMBIntraTypes[7] = 
			pMBIntraTypes[11] = pMBIntraTypes[15] = -1;

		slice_di->m_pTopMotionVectors = slice_di->m_pCurMotionVectors;
		slice_di->m_pCurMotionVectors = slice_di->m_pCurMotionVectors + (t->m_img_width >> 2);
		pMV = slice_di->m_pCurMotionVectors;
	}

	//if ((!t->m_deblocking_filter_passthrough) && (t->m_enable_deblocking_flage)){
	//	for(uMB_Y = 0; uMB_Y < uMBHeight; uMB_Y++){
	//		t->InLoopFilter(t, uMB_Y);
	//	}
	//}

	return rc;
}


RV_Status Decoder_BPicture(struct Decoder *t)
{
	RV_Status	rc = RV_S_OK;
	U32         ulSkipModesLeft = 0;
	I32		    iRatio0, iRatio1, trb, trd;
	U32         uMB_X, uMB_Y;
	U32         slice_number = 0;

	struct DecoderMBInfo  *pMB;
	DecoderIntraType *pMBIntraTypes;
	struct DecoderPackedMotionVector *mv_temp;
	struct DecoderPackedMotionVector *pMVR;
	struct DecoderPackedMotionVector *pMV;


	const U32 uMBWidth  = t->m_mbX;
	const U32 uMBHeight = t->m_mbY;
	RV89_DEC_LOCAL rv89_local;
	SLICE_DEC *slice_di = &t->m_slice_di[0];

	t->m_pBFrame =  buf_seq_ctl(&t->m_priv, NULL, FIFO_READ);
	if(NULL == t->m_pBFrame){
		return VO_MEMRC_NO_YUV_BUFFER;
	}
	t->m_pBFrame->Time = t->m_timeStamp;
	t->m_pBFrame->UserData = t->UserData;

	/* Compute ratios needed for direct mode */
	trb = (I32) t->m_tr - (I32) t->m_refTR;
	trd = (I32) t->m_curTR - (I32) t->m_refTR;

	if (trb < 0)
		trb += (I32) t->m_uTRWrap;

	if (trd < 0)
		trd += (I32) t->m_uTRWrap;

	if (trb > trd)		/* just in case TR's are incorrect */
		trb=0;

	if (trd>0) {
		iRatio0 = (trb << TR_SHIFT) / trd;
		iRatio1 = ((trd-trb) << TR_SHIFT) / trd;
	}else{
		iRatio0 = iRatio1 = 0;
	}

	/* pel position of current macroblock in the luma plane */
	pMV					       = slice_di->m_pCurMotionVectors;
	pMBIntraTypes		       = slice_di->m_pMBIntraTypes;
	slice_di->m_pMBInfo[0].QP  = slice_di->m_pquant;

	loadbits(slice_di);


	for(uMB_Y = 0; uMB_Y < uMBHeight; uMB_Y++){
		for(uMB_X = 0; uMB_X < uMBWidth; uMB_X++){
			U32 offset;

			//no skip MB left then checking SSC
			if ((ulSkipModesLeft <= 1)&&CB_GetSSC(t, slice_number+1, slice_di)){
				if(GetSliceHeader(t, &uMB_X, &uMB_Y,slice_di)){
#if defined(VODEBUG)
					sprintf(aa, "\n  GetSliceHeader err !!!");
					NU_SIO_Puts(aa);
#endif
					return RV_S_ERROR;
				}
				slice_number++;
				ulSkipModesLeft = 0;
				pMV = slice_di->m_pCurMotionVectors + uMB_X * 4;
			}

			slice_di->m_xmb_pos = uMB_X;
			slice_di->m_ymb_pos = uMB_Y;

			pMB = &slice_di->m_pMBInfo[uMB_X + uMBWidth * uMB_Y];
			if(DecodeMBHeader(t, &rv89_local, pMV, &ulSkipModesLeft, pMB, uMB_X, uMB_Y,0) != RV_S_OK){
#if defined(VODEBUG)
				sprintf(aa, "\n  DecodeMBHeader err !!!");
				NU_SIO_Puts(aa);
#endif
				return RV_S_ERROR;
			}
			/* Finally, perform motion compensation to reconstruct the YUV data */

			//TBD
			offset = (uMB_Y<<4) * t->m_img_stride + (uMB_X<<4);
			rv89_local.cur[0] = t->m_pCurrentFrame->m_pYPlane + offset;
			rv89_local.dst[0] = t->m_pBFrame->m_pYPlane + offset;
			rv89_local.ref[0] = t->m_pRefFrame->m_pYPlane + offset;
			offset = (uMB_Y<<3) * t->m_img_UVstride + (uMB_X<<3);
			rv89_local.cur[1] = t->m_pCurrentFrame->m_pUPlane + offset;
			rv89_local.cur[2] = t->m_pCurrentFrame->m_pVPlane + offset;
			rv89_local.dst[1] = t->m_pBFrame->m_pUPlane + offset;
			rv89_local.dst[2] = t->m_pBFrame->m_pVPlane + offset;
			rv89_local.ref[1] = t->m_pRefFrame->m_pUPlane + offset;
			rv89_local.ref[2] = t->m_pRefFrame->m_pVPlane + offset;

			/* Depending on MB type set up pointers to reference planes */
			rc = RV_S_OK; 

			if(pMB->mbtype != MBTYPE_SKIPPED){
				DecMB_SVLC(slice_di, &rv89_local, pMB->cbp, pMB->QP, t->m_bIsRV8, t->m_ptype);
			}

			/* Reconstruct luma */
			if(pMB->mbtype == MBTYPE_SKIPPED || pMB->mbtype == MBTYPE_DIRECT){
				/* Just copy the reference plane data. */
				U32  uRefMBType = slice_di->m_pReferenceMBInfo[uMB_X + uMBWidth * uMB_Y].mbtype;

				if(uRefMBType == MBTYPE_SKIPPED){//YU TBD remove m_pReferenceMBInfo
					CopyBlock16x16(rv89_local.cur[0], rv89_local.dst[0], t->m_img_stride, t->m_img_stride);
					CopyBlock8x8(rv89_local.cur[1], rv89_local.dst[1], t->m_img_UVstride, t->m_img_UVstride);
					CopyBlock8x8(rv89_local.cur[2], rv89_local.dst[2], t->m_img_UVstride, t->m_img_UVstride);
				}else{
					if(IS_INTRA_MBTYPE(uRefMBType)){

						if(iRatio0 == iRatio1){
							AddBlock16x16(rv89_local.cur[0], rv89_local.ref[0], rv89_local.dst[0], t->m_img_stride);
							AddBlock8x8(rv89_local.cur[1], rv89_local.ref[1], rv89_local.dst[1], t->m_img_UVstride);
							AddBlock8x8(rv89_local.cur[2], rv89_local.ref[2], rv89_local.dst[2], t->m_img_UVstride);
						}else{
							struct DecoderPackedMotionVector *pMVf;
							pMVR = slice_di->m_pReferenceMotionVectors + (uMB_X + uMBWidth * uMB_Y) * 4;
							pMVf = slice_di->m_pCurBidirMotionVectors + uMB_X * 4;

							pMV[0].mvx = pMV[0].mvy = 0;
							pMV[1].mvx = pMV[1].mvy = 0;
							pMV[2].mvx = pMV[2].mvy = 0;
							pMV[3].mvx = pMV[3].mvy = 0;

							pMVf[0].mvx = pMVf[0].mvy = 0;
							pMVf[1].mvx = pMVf[1].mvy = 0;
							pMVf[2].mvx = pMVf[2].mvy = 0;
							pMVf[3].mvx = pMVf[3].mvy = 0;

							InterpolateMBWeight(t, &rv89_local, t->m_img_stride,pMVf, pMV, slice_di->m_pPredBuf, 1, t->m_bIsRV8,iRatio0, iRatio1,0);
						}
					}else{

						struct DecoderPackedMotionVector *pMVf;
						pMVR = slice_di->m_pReferenceMotionVectors + (uMB_X + uMBWidth * uMB_Y) * 4;
						pMVf = slice_di->m_pCurBidirMotionVectors + uMB_X * 4;

						DecodeDirectMV(t, uRefMBType, pMVR, pMVf, pMV, uMB_X<<4, uMB_Y<<4, iRatio0, iRatio1);

						if(iRatio0 == iRatio1){
							InterpolateMB(t, &rv89_local, t->m_img_stride,pMVf, pMV, slice_di->m_pPredBuf, 1, t->m_bIsRV8,0);
						}else{
							InterpolateMBWeight(t, &rv89_local, t->m_img_stride,pMVf, pMV, slice_di->m_pPredBuf, 1, t->m_bIsRV8,iRatio0, iRatio1,0);
						}												

						//TBD BUG FIX
						pMV[0].mvx = pMV[0].mvy = 0;
						pMV[1].mvx = pMV[1].mvy = 0;
						pMV[2].mvx = pMV[2].mvy = 0;
						pMV[3].mvx = pMV[3].mvy = 0;
					}
				}
				if(pMB->cbp)
					IdctAddInterMB(t, pMB, &rv89_local,0);

			}else if (pMB->mbtype == MBTYPE_BIDIR ){
				struct DecoderPackedMotionVector *pMVp, *pMVf;
				pMVp = pMV;
				pMVf = slice_di->m_pCurBidirMotionVectors + uMB_X * 4;

				InterpolateMB(t, &rv89_local, t->m_img_stride,pMVf, pMVp,slice_di->m_pPredBuf, 1, t->m_bIsRV8,0);
				IdctAddInterMB(t,pMB,&rv89_local,0);

			}else if (MBTYPE_INTRA_16x16 == pMB->mbtype){
				rc = ReconIntraMB16x16(t,pMB, &rv89_local,0);
				Decoder_transferMB(t,pMB, &rv89_local,0);
			}else if (IS_INTRA_MBTYPE(pMB->mbtype)){
				rc = ReconIntraMB(t, pMB, &rv89_local,0);
			}else{

				if (pMB->mbtype != MBTYPE_BACKWARD){
					rv89_local.ref[0] = rv89_local.cur[0];
					rv89_local.ref[1] = rv89_local.cur[1];
					rv89_local.ref[2] = rv89_local.cur[2];
				}

				InterpolateMB(t, &rv89_local, t->m_img_stride,pMV, 0, slice_di->m_pPredBuf, 1, t->m_bIsRV8,0);
				IdctAddInterMB(t, pMB, &rv89_local,0);
			}

			pMV += 4;
		}
		pMBIntraTypes[3] = pMBIntraTypes[7] = 
			pMBIntraTypes[11] = pMBIntraTypes[15] = -1;

		mv_temp                        = slice_di->m_pCurMotionVectors;
		slice_di->m_pCurMotionVectors  = slice_di->m_pTopMotionVectors;
		slice_di->m_pTopMotionVectors  = mv_temp;

		mv_temp                            = slice_di->m_pCurBidirMotionVectors;
		slice_di->m_pCurBidirMotionVectors = slice_di->m_pTopBidirMotionVectors;
		slice_di->m_pTopBidirMotionVectors = mv_temp;

		pMV = slice_di->m_pCurMotionVectors;
	}

	return rc;
}
