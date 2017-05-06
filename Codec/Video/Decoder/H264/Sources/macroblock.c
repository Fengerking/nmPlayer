/**
	read:
		macroblock.o
		cabac.o/biaridecod.o/cCabac.o
		vlc.o
	decode:
		nalu.o
		erc_do_i.o //GetLumaBlock,SILumaAvgBlock
		ARM_GB1.o
		ARM_BAVG.o
		erc_do_p.o//GetChromaBlock,SIChromaAvgBlock
		ARM_GB2.o
		itrans.o
		ARM_itrans.o
	deblock:
		InplaceDeblock.o
		ARM_DB1
		ARM_DB2
		
*/
#include "defines.h"
#if !BUILD_WITHOUT_C_LIB
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#endif

#include "loopFilter.h"
#include "global.h"
//#include "H264_C_Type.h"
#include "avd_neighbor.h"
#include "mbuffer.h"
#include "elements.h"
#include "errorconcealment.h"
#include "macroblock.h"
#include "fmo.h"
#include "cabac.h"
#include "vlc.h"
#include "image.h"
#include "mb_access.h"
#include "biaridecod.h"
#include "block.h"
#if CALC_THUMBNAIL
#ifdef LINUX
#include <stdio.h>
#include <sys/time.h>
#endif
#endif
//extern ColocatedParams *Co_located;
#define MOVE_P8x8_TO_READMOTION 0
#if (DUMP_VERSION & DUMP_SLICE)
StorablePicture* gRefFrame = NULL;
#endif//DUMP_VERSION & DUMP_SLICE)
#if FEATURE_INTERLACE
/* field scan pattern
// change from FIELD_SCAN[location][i/j] to FIELD_SCAN_TO_INDEX[location] = i*4 + j;
const avdUInt8 FIELD_SCAN[16][2] =
{
{0,0},{0,1},{1,0},{0,2},
{0,3},{1,1},{1,2},{1,3},
{2,0},{2,1},{2,2},{2,3},
{3,0},{3,1},{3,2},{3,3}
};
*/
const avdUInt8 FIELD_SCAN_TO_INDEX[16] = 
{0, 1, 4, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

#endif //FEATURE_INTERLACE


/* single scan pattern
// change from SNGL_SCAN[location][i/j] to SNGL_SCAN_TO_INDEX[location] = i*4 + j;
const avdUInt8 SNGL_SCAN[16][2] =
{
{0,0},{1,0},{0,1},{0,2},
{1,1},{2,0},{3,0},{2,1},
{1,2},{0,3},{1,3},{2,2},
{3,1},{3,2},{2,3},{3,3}
};
*/
const avdUInt8 SNGL_SCAN_TO_INDEX[16] = 
{0, 4, 1, 2, 5, 8, 12, 9, 6, 3, 7, 10, 13, 14, 11, 15};

//change from [j][i] to [i*4+j] of cofTypeIdx[4][4] = {{1,4,8,8},{2,4,8,8},{2,4,16,16},{2,4,16,16}};
//const avdUInt8 cofTypeIdx[16] = {1,2,2,2,4,4,4,4,8,8,16,16,8,8,16,16};

//! used to control block sizes : Not used/16x16/16x8/8x16/8x8/8x4/4x8/4x4
const avdUInt8 BLOCK_STEP[16]=
{
	4,4,4,2,2,2,1,1,
	4,4,2,4,2,1,2,1
};
#if FEATURE_T8x8
//! single scan pattern
const avdUInt8 SNGL_SCAN8x8[64][2] = {
	{0,0}, {1,0}, {0,1}, {0,2}, {1,1}, {2,0}, {3,0}, {2,1}, {1,2}, {0,3}, {0,4}, {1,3}, {2,2}, {3,1}, {4,0}, {5,0},
	{4,1}, {3,2}, {2,3}, {1,4}, {0,5}, {0,6}, {1,5}, {2,4}, {3,3}, {4,2}, {5,1}, {6,0}, {7,0}, {6,1}, {5,2}, {4,3},
	{3,4}, {2,5}, {1,6}, {0,7}, {1,7}, {2,6}, {3,5}, {4,4}, {5,3}, {6,2}, {7,1}, {7,2}, {6,3}, {5,4}, {4,5}, {3,6},
	{2,7}, {3,7}, {4,6}, {5,5}, {6,4}, {7,3}, {7,4}, {6,5}, {5,6}, {4,7}, {5,7}, {6,6}, {7,5}, {7,6}, {6,7}, {7,7}
};

#if 1//FEATURE_INTERLACE
//! field scan pattern
const avdUInt8 FIELD_SCAN8x8[64][2] = {   // 8x8
	{0,0}, {0,1}, {0,2}, {1,0}, {1,1}, {0,3}, {0,4}, {1,2}, {2,0}, {1,3}, {0,5}, {0,6}, {0,7}, {1,4}, {2,1}, {3,0},
	{2,2}, {1,5}, {1,6}, {1,7}, {2,3}, {3,1}, {4,0}, {3,2}, {2,4}, {2,5}, {2,6}, {2,7}, {3,3}, {4,1}, {5,0}, {4,2},
	{3,4}, {3,5}, {3,6}, {3,7}, {4,3}, {5,1}, {6,0}, {5,2}, {4,4}, {4,5}, {4,6}, {4,7}, {5,3}, {6,1}, {6,2}, {5,4},
	{5,5}, {5,6}, {5,7}, {6,3}, {7,0}, {7,1}, {6,4}, {6,5}, {6,6}, {6,7}, {7,2}, {7,3}, {7,4}, {7,5}, {7,6}, {7,7}
};
#endif// FEATURE_INTERLACE
#endif//FEATURE_T8x8
const  	int T8x8KPos[4]={0,2,8,10};
const	int T8x8Offset1[16]={0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3};
const avdUInt8 decode_scan[32] = {0,1,0,1,2,3,2,3,0,1,0,1,2,3,2,3,
0,0,1,1,0,0,1,1,2,2,3,3,2,2,3,3};
const avdUInt8 inv_decode_scan[16] = {0,1,4,5,2,3,6,7,8,9,12,13,10,11,14,15};
const avdUInt8 chromaScan[16] = {0,1,0,1,2,3,2,3,
4,4,5,5,4,4,5,5};

//! Dequantization coefficients
#if 1
const avdUInt8 dequant_coef[96] = {
	10, 13, 10, 13, 13, 16, 13, 16,10, 13, 10, 13, 13, 16, 13, 16,
	11, 14, 11, 14, 14, 18, 14, 18,11, 14, 11, 14, 14, 18, 14, 18,
	13, 16, 13, 16, 16, 20, 16, 20,13, 16, 13, 16, 16, 20, 16, 20,
	14, 18, 14, 18, 18, 23, 18, 23,14, 18, 14, 18, 18, 23, 18, 23,
	16, 20, 16, 20, 20, 25, 20, 25,16, 20, 16, 20, 20, 25, 20, 25,
	18, 23, 18, 23, 23, 29, 23, 29,18, 23, 18, 23, 23, 29, 23, 29
};
#if FEATURE_T8x8
const avdUInt8 dequant_coef8[384] =
{
	
		20,  19, 25, 19, 20, 19, 25, 19,
		19,  18, 24, 18, 19, 18, 24, 18,
		25,  24, 32, 24, 25, 24, 32, 24,
		19,  18, 24, 18, 19, 18, 24, 18,
		20,  19, 25, 19, 20, 19, 25, 19,
		19,  18, 24, 18, 19, 18, 24, 18,
		25,  24, 32, 24, 25, 24, 32, 24,
		19,  18, 24, 18, 19, 18, 24, 18
	,
	
		22,  21, 28, 21, 22, 21, 28, 21,
		21,  19, 26, 19, 21, 19, 26, 19,
		28,  26, 35, 26, 28, 26, 35, 26,
		21,  19, 26, 19, 21, 19, 26, 19,
		22,  21, 28, 21, 22, 21, 28, 21,
		21,  19, 26, 19, 21, 19, 26, 19,
		28,  26, 35, 26, 28, 26, 35, 26,
		21,  19, 26, 19, 21, 19, 26, 19
	,
	
		26,  24, 33, 24, 26, 24, 33, 24,
		24,  23, 31, 23, 24, 23, 31, 23,
		33,  31, 42, 31, 33, 31, 42, 31,
		24,  23, 31, 23, 24, 23, 31, 23,
		26,  24, 33, 24, 26, 24, 33, 24,
		24,  23, 31, 23, 24, 23, 31, 23,
		33,  31, 42, 31, 33, 31, 42, 31,
		24,  23, 31, 23, 24, 23, 31, 23
	,
	
		28,  26, 35, 26, 28, 26, 35, 26,
		26,  25, 33, 25, 26, 25, 33, 25,
		35,  33, 45, 33, 35, 33, 45, 33,
		26,  25, 33, 25, 26, 25, 33, 25,
		28,  26, 35, 26, 28, 26, 35, 26,
		26,  25, 33, 25, 26, 25, 33, 25,
		35,  33, 45, 33, 35, 33, 45, 33,
		26,  25, 33, 25, 26, 25, 33, 25
	,
	
		32,  30, 40, 30, 32, 30, 40, 30,
		30,  28, 38, 28, 30, 28, 38, 28,
		40,  38, 51, 38, 40, 38, 51, 38,
		30,  28, 38, 28, 30, 28, 38, 28,
		32,  30, 40, 30, 32, 30, 40, 30,
		30,  28, 38, 28, 30, 28, 38, 28,
		40,  38, 51, 38, 40, 38, 51, 38,
		30,  28, 38, 28, 30, 28, 38, 28
	,
	
		36,  34, 46, 34, 36, 34, 46, 34,
		34,  32, 43, 32, 34, 32, 43, 32,
		46,  43, 58, 43, 46, 43, 58, 43,
		34,  32, 43, 32, 34, 32, 43, 32,
		36,  34, 46, 34, 36, 34, 46, 34,
		34,  32, 43, 32, 34, 32, 43, 32,
		46,  43, 58, 43, 46, 43, 58, 43,
		34,  32, 43, 32, 34, 32, 43, 32
	
};
#endif//FEATURE_T8x8
#else
//delete 
#endif
void read_motion_info_of_BDirect(ImageParameters *img,TMBsProcessor *info);
void read_motion_info_of_BTempol(ImageParameters *img,TMBsProcessor *info);
const avdUInt8 QP_SCALE_CR[52]=
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,
	12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
	28,29,29,30,31,32,32,33,34,34,35,35,36,36,37,37,
	37,38,38,38,39,39,39,39
};

const avdUInt8 DIVIDE6[52]=
{
	0, 0, 0, 0, 0, 0, 1, 1, 1, 1, //0-9
	1, 1, 2, 2, 2, 2, 2, 2, 3, 3, //10-19
	3, 3, 3, 3, 4, 4, 4, 4, 4, 4, //20-29
	5, 5, 5, 5, 5, 5, 6, 6, 6, 6, //30-39
	6, 6, 7, 7, 7, 7, 7, 7, 8, 8, //40-49
	8, 8
};

//! gives CBP value from codeword number, both for intra and inter
const avdUInt8 avd_NCBP[2][48]=
{
	{47,31,15,0,23,27,29,30,7,11,13,14,39,43,45,46,16,3,5,10,12,19,21,26,
	28,35,37,42,44,1,2,4,8,17,18,20,24,6,9,22,25,32,33,34,36,40,38,41},
	{0,16,1,2,4,8,32,3,5,10,12,15,47,7,11,13,14,6,9,31,35,37,42,44,
	33,34,36,40,39,43,45,46,17,18,20,24,19,21,26,28,23,27,29,30,22,25,38,41}
};

//#define CHECK_MOTIONINFO(a,b,c)
/*!
************************************************************************
* \brief
*    initializes the current macroblock
************************************************************************
*/

#if CALC_THUMBNAIL
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
static unsigned long GetTickCount(){
#if 1
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
#else
	return clock();
#endif
}
#endif
#endif
/*!
************************************************************************
* \brief
*    set coordinates of the next macroblock
*    check end_of_slice condition 
************************************************************************
*/
avdNativeInt exit_macroblock(ImageParameters *img,TMBsProcessor *info,avdNativeInt eos_bit)
{
	Slice *currSlice = img->currentSlice;
	TMBBitStream* mbBits = GetMBBits(info->currMB);
	TPosInfo* posInfo = GetPosInfo(info);
	TSizeInfo	*sizeInfo = img->sizeInfo;
	int		num_dec_mb = GetCurPos(info,sizeInfo);//GetMBY(info)*sizeInfo->PicWidthInMbs+GetMBX(info);
	Finish_MB_Neighbor_Info(img,info);
#if FEATURE_INTERLACE//IL TBD, remove it?
	if(GetMbAffFrameFlag(img) && mbIsMbField(info->currMB))
	{
		img->num_ref_idx_l0_active >>= 1;
		img->num_ref_idx_l1_active >>= 1;
	}
#endif//VOI_H264D_NON_BASELINE

	
	if (num_dec_mb == sizeInfo->PicSizeInMbs-1)
	{
		
			 return VOI_TRUE;
	

	}

	//checkbits(15);
	if(nal_startcode_follows(img,info,eos_bit) == VOI_FALSE) 
		return VOI_FALSE;
	
	if(!IsVLCCoding(img->active_pps) || img->cod_counter <= 0
		|| img->type == I_SLICE  || img->type == SI_SLICE)
	{
		//
		//AVD_ERROR_CHECK(img,"exit_macroblock:cabac and img->cod_counter<=0\n",100);
		return VOI_TRUE;
	}
	return VOI_FALSE;
}
/*!
************************************************************************
* \brief
*    Set motion vector predictor
************************************************************************
*/
// blkIdx is defined as below to keep number of function arguments under 4;
#define		GET_MVP_BLOCKX_MASK						0xf
#define		GET_MVP_BLOCKX_SHAPE_MASK				0xf0
#define		GET_MVP_BLOCKY_MASK						0xf00
#define		GET_MVP_BLOCKY_SHAPE_MASK				0xf000
#define		GetMVPBlockX(blkIdx)					(blkIdx&GET_MVP_BLOCKX_MASK)
#define		GetMVPBlockXShape(blkIdx)				((blkIdx&GET_MVP_BLOCKX_SHAPE_MASK)>>4)
#define		GetMVPBlockY(blkIdx)					((blkIdx&GET_MVP_BLOCKY_MASK)>>8)
#define		GetMVPBlockYShape(blkIdx)				((blkIdx&GET_MVP_BLOCKY_SHAPE_MASK)>>12)
#if FEATURE_INTERLACE
//static const int YOffset[4][2][2]={//[][neighbor_is_top][curr_is_top]
//	{
//		0,//bottom,bottom
//		16//bottom,top
//	},
//	{
//		-16,//top,bottom
//		0//top,top
//	}
//};

int  GetAbsYPos(ImageParameters *img,TMBsProcessor *info,avdNativeInt nbIdx, avdNativeInt yOff,avdNativeInt maxH)
{ 
	int yW,posY,offset;
	TPosInfo*  posInfo  = GetPosInfo(info);
	if(nbIdx!=NEIGHBOR_A||GetLeftMB(info))
	{
		yW = GetYPosition(info->neighborABCDPos[nbIdx].yPosIdx,yOff,maxH);
		offset = info->neighborABCDPos[nbIdx].offset2CurMB;
	}
	else
	{
		TMBAddrNYPos	addrNYPos = info->getNeighborAFunc(info,info->neighborA,yOff,maxH);
		yW = GetYPosition(addrNYPos.yPosIdx,yOff,maxH);
		offset = addrNYPos.offset2CurMB;
	}
	posY = posInfo->pix_y + ((yW+maxH)&(maxH-1)) + offset;

	return posY;
}
void GetNeighborAB(ImageParameters *img,TMBsProcessor *info,avdNativeInt x4x4Offset, avdNativeInt y4x4Offset, MacroBlock *mbAB[], 
				   avdNativeInt y4x4AB[])
{
	
	MacroBlock   *currMBInfo = info->currMB;
	MacroBlock   *tmp;
	TPosInfo*  posInfo  = GetPosInfo(info);
	if (x4x4Offset > 0){
		mbAB[NEIGHBOR_A] = currMBInfo;
		y4x4AB[NEIGHBOR_A] = posInfo->block_y + y4x4Offset;
	}
	else {
		int					leftMBYForIntra = -1;
		mbAB[NEIGHBOR_A] = tmp = GetLeftMBAffMB2(img,info,y4x4Offset*4,16,&leftMBYForIntra);
		if(tmp&&!INSAMESLICE(tmp,currMBInfo))
			mbAB[NEIGHBOR_A] = NULL;

		if (mbAB[NEIGHBOR_A] != NULL)
		{
			y4x4AB[NEIGHBOR_A]  = GetAbsYPos(img,info,NEIGHBOR_A,y4x4Offset*4,MB_BLOCK_SIZE)>>2;
			
		}

	}
	// Get neighborB
	if (y4x4Offset > 0){
		mbAB[NEIGHBOR_B] = currMBInfo;
		y4x4AB[NEIGHBOR_B] = posInfo->block_y + y4x4Offset - 1;
	}
	else {
		mbAB[NEIGHBOR_B] = tmp = GetUpMB(info);
		if(tmp&&!INSAMESLICE(tmp,currMBInfo))//
			mbAB[NEIGHBOR_B] = NULL;
		if (mbAB[NEIGHBOR_B] != NULL)
			y4x4AB[NEIGHBOR_B] =  GetAbsYPos(img,info,NEIGHBOR_B,-1,MB_BLOCK_SIZE)>>2;
	}
}

void Get_Neighbor_ABCnRefIdx(ImageParameters *img,TMBsProcessor *info,avdNativeInt x4x4Offset, avdNativeInt y4x4Offset, 
							 avdNativeInt listIdx, avdNativeInt cxStep,
							 Macroblock *nMBs[], 
							 avdNativeInt *x4x4AB, avdNativeInt *y4x4AB, 
							 avdNativeInt refIdx[2][3])
{
	
	Macroblock   *currMBInfo = info->currMB;
	avdNativeInt		nAX, nAY, nBX, nBY, nCX, nCY, x4, list, startList, endList;
	StorablePicture	*dec_picture = img->dec_picture;
	AVDStoredPicMotionInfo *motionInfo = dec_picture->motionInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);

	GetNeighborAB(img,info,x4x4Offset, y4x4Offset, nMBs, y4x4AB);
	x4 = posInfo->block_x + x4x4Offset;
	x4x4AB[NEIGHBOR_A] = x4 - 1;
	x4x4AB[NEIGHBOR_B] = x4;
	nMBs[NEIGHBOR_C] = NULL;
	nCX = x4x4Offset + cxStep;
	nCY = 0;
	if (nCX < 4){
		if (!(nCX == 2 && (y4x4Offset&1))){
			nMBs[NEIGHBOR_C] = nMBs[NEIGHBOR_B];
			nCY = y4x4AB[NEIGHBOR_B];
			nCX = x4 + cxStep;
		}
	}
	else if (!y4x4Offset){	
		Macroblock* tmp = GetUpRightMB(info);
		nMBs[NEIGHBOR_C] = tmp = GetUpRightMB(info);
		if(tmp&&!INSAMESLICE(tmp,currMBInfo))
			nMBs[NEIGHBOR_C] = NULL;

		if (nMBs[NEIGHBOR_C]!= NULL){
			nCY = GetAbsYPos(img,info,NEIGHBOR_C,-1,MB_BLOCK_SIZE)>>2;
			nCX = x4 + cxStep;
		}
	}

	if (!nMBs[NEIGHBOR_C]){
		// use NEIGHBOR_D
		if (x4x4Offset > 0){
			nMBs[NEIGHBOR_C] = nMBs[NEIGHBOR_B];
			nCY = y4x4AB[NEIGHBOR_B];
		}
		else {
			Macroblock* tmp;// = GetUpLeftMB(info);
			int leftMBYForIntra = -1;
			nMBs[NEIGHBOR_C] = tmp = y4x4Offset==0?GetUpLeftMB(info):GetLeftMBAffMB2(img,info,y4x4Offset*4-1,16,&leftMBYForIntra);
			if(tmp&&!INSAMESLICE(tmp,currMBInfo))
				nMBs[NEIGHBOR_C] = NULL;
			if (nMBs[NEIGHBOR_C]!= NULL)
			{
				nCY =  y4x4Offset? GetAbsYPos(img,info,NEIGHBOR_A,y4x4Offset*4-1,MB_BLOCK_SIZE)>>2:GetAbsYPos(img,info,NEIGHBOR_D,-1,MB_BLOCK_SIZE)>>2;
			}
		}
		nCX = x4x4AB[NEIGHBOR_A];
	}

	x4x4AB[NEIGHBOR_C] = nCX;
	y4x4AB[NEIGHBOR_C] = nCY;

	// refPic is in 8x8 coordinate;
	nAY = (y4x4AB[NEIGHBOR_A]>>1);
	nAX = (x4x4AB[NEIGHBOR_A]>>1);
	nBY = (y4x4AB[NEIGHBOR_B]>>1);
	nBX = (x4x4AB[NEIGHBOR_B]>>1);
	nCY >>= 1;
	nCX >>= 1;
	if (listIdx == BOTH_LISTS){
		startList = LIST_0;
		endList = LIST_1;
	}
	else
		startList = endList = listIdx;
	for (list = startList; list <= endList; list++){
		avdUInt8 **refPicList = list == 0 ? motionInfo->ref_idx0 : motionInfo->ref_idx1;
		avdNativeInt *refIdxList = refIdx[list];
	
		if (!mbIsMbField(currMBInfo))
		{
			refIdxList[NEIGHBOR_A] = (nMBs[NEIGHBOR_A] &&
				refPicList[nAY][nAX] != AVD_INVALID_REF_IDX)
				? (!mbIsMbField(nMBs[NEIGHBOR_A]) ? 
				refPicList[nAY][nAX] : refPicList[nAY][nAX]>>1) : AVD_INVALID_REF_IDX;
			refIdxList[NEIGHBOR_B] = (nMBs[NEIGHBOR_B] && 
				refPicList[nBY][nBX] != AVD_INVALID_REF_IDX)
				? (!GetFieldByPos(img,info,y4x4AB[NEIGHBOR_B]>>2,x4x4AB[NEIGHBOR_B]>>2) ? 
				refPicList[nBY][nBX] : refPicList[nBY][nBX]>>1) : AVD_INVALID_REF_IDX;
			refIdxList[NEIGHBOR_C] = (nMBs[NEIGHBOR_C] && 
				refPicList[nCY][nCX] != AVD_INVALID_REF_IDX)
				? (!GetFieldByPos(img,info,y4x4AB[NEIGHBOR_C]>>2,x4x4AB[NEIGHBOR_C]>>2) ? 
				refPicList[nCY][nCX] : refPicList[nCY][nCX]>>1) : AVD_INVALID_REF_IDX;
		}
		else
		{
			refIdxList[NEIGHBOR_A] = (nMBs[NEIGHBOR_A] && 
				refPicList[nAY][nAX] != AVD_INVALID_REF_IDX)
				? (!mbIsMbField(nMBs[NEIGHBOR_A]) ? 
				refPicList[nAY][nAX] * 2 : refPicList[nAY][nAX]) : AVD_INVALID_REF_IDX;
			refIdxList[NEIGHBOR_B] = (nMBs[NEIGHBOR_B] && 
				refPicList[nBY][nBX] != AVD_INVALID_REF_IDX)
				? (!GetFieldByPos(img,info,y4x4AB[NEIGHBOR_B]>>2,x4x4AB[NEIGHBOR_B]>>2) ? 
				refPicList[nBY][nBX] * 2 : refPicList[nBY][nBX]) : AVD_INVALID_REF_IDX;
			refIdxList[NEIGHBOR_C] = (nMBs[NEIGHBOR_C] && 
				refPicList[nCY][nCX] != AVD_INVALID_REF_IDX)
				? (!GetFieldByPos(img,info,y4x4AB[NEIGHBOR_C]>>2,x4x4AB[NEIGHBOR_C]>>2)? 
				refPicList[nCY][nCX] * 2 : refPicList[nCY][nCX]) : AVD_INVALID_REF_IDX;
		}

	}

}

static void SetMotionVectorPredictorMBAff (ImageParameters *img,TMBsProcessor *info,
							   avdNativeUInt			blkIdx,
							   avdNativeInt				list,                               
							   avdNativeInt				ref_frame,
							   AVDMotionVector			*pmv)
{
	MacroBlock *nMBs[3];
	AVDMotionVector **mv2D;
	avdNativeInt mv_a, mv_b, mv_c;
	avdNativeInt mvPredType, rFrameL, rFrameU, rFrameUR;
	StorablePicture	*dec_picture = img->dec_picture;
	avdNativeUInt block_x, block_y, blkShape;
    Macroblock* currMB = info->currMB;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	////TMBsProcessor *info  = img->mbsProcessor;
	MacroBlock *currMBInfo = info->currMB;
	avdNativeInt x4;
	avdNativeInt y4;
	avdNativeInt nCX, mv_a1;

	AVDStoredPicMotionInfo *motionInfo;
	avdUInt8		**refList;
	TPosInfo*  posInfo  = GetPosInfo(info);

	avdNativeInt hv;
	avdNativeInt x4x4AB[3], y4x4AB[3], refIdx[2][3];

	block_x = GetMVPBlockX(blkIdx);
	block_y = GetMVPBlockY(blkIdx);
	Get_Neighbor_ABCnRefIdx(img,info,block_x, block_y, list, GetMVPBlockXShape(blkIdx), nMBs, 
		x4x4AB, y4x4AB, refIdx);	

	mv2D = list == 0 ? dec_picture->motionInfo->mv0 :
		dec_picture->motionInfo->mv1;

	rFrameL = refIdx[list][NEIGHBOR_A];
	rFrameU = refIdx[list][NEIGHBOR_B];
	rFrameUR = refIdx[list][NEIGHBOR_C];

	mvPredType = MVPRED_MEDIAN;
	if(rFrameL == ref_frame){
		if ((rFrameU != ref_frame) & (rFrameUR != ref_frame))       
			mvPredType = MVPRED_L;
	}
	else {
		if((rFrameU == ref_frame) & (rFrameUR != ref_frame))  
			mvPredType = MVPRED_U;
		else if((rFrameU != ref_frame) & (rFrameUR == ref_frame))  
			mvPredType = MVPRED_UR;
	}

	// Directional predictions 
	blkShape = (blkIdx & (GET_MVP_BLOCKX_SHAPE_MASK | GET_MVP_BLOCKY_SHAPE_MASK));
	if (blkShape == ((2<<4) | (4<<12))) //(blockshape_x == 2 & blockshape_y == 4)
	{
		if (block_x){
			if(rFrameUR == ref_frame)
				mvPredType = MVPRED_UR;
		}
		else if(rFrameL == ref_frame)
			mvPredType = MVPRED_L;
	}
	else if (blkShape == ((4<<4) | (2<<12))) //(blockshape_y == 2 & blockshape_x == 4)
	{
		if (block_y){
			if(rFrameL == ref_frame)
				mvPredType = MVPRED_L;
		}
		else if(rFrameU == ref_frame)
			mvPredType = MVPRED_U;
	}

	for (hv=0; hv < 2; hv++)
	{
		// TBD: cal mv_a, mv_b, mv_c only when it needed;
		if (hv==0)
		{
			mv_a = nMBs[NEIGHBOR_A] ? mv2D[y4x4AB[NEIGHBOR_A]][x4x4AB[NEIGHBOR_A]].x : 0;
			mv_b = nMBs[NEIGHBOR_B] ? mv2D[y4x4AB[NEIGHBOR_B]][x4x4AB[NEIGHBOR_B]].x : 0;
			mv_c = nMBs[NEIGHBOR_C] ? mv2D[y4x4AB[NEIGHBOR_C]][x4x4AB[NEIGHBOR_C]].x : 0;
		}
		else if (!mbIsMbField(currMB))
		{
			mv_a = nMBs[NEIGHBOR_A]  ? (!mbIsMbField(nMBs[NEIGHBOR_A]) ?
				mv2D[y4x4AB[NEIGHBOR_A]][x4x4AB[NEIGHBOR_A]].y : 
			mv2D[y4x4AB[NEIGHBOR_A]][x4x4AB[NEIGHBOR_A]].y * 2) : 0;
			mv_b = nMBs[NEIGHBOR_B]  ? (!GetFieldByPos(img,info,y4x4AB[NEIGHBOR_B]>>2,x4x4AB[NEIGHBOR_B]>>2) ?
				mv2D[y4x4AB[NEIGHBOR_B]][x4x4AB[NEIGHBOR_B]].y : 
			mv2D[y4x4AB[NEIGHBOR_B]][x4x4AB[NEIGHBOR_B]].y * 2) : 0;
			mv_c = nMBs[NEIGHBOR_C]  ? (!GetFieldByPos(img,info,y4x4AB[NEIGHBOR_C]>>2,x4x4AB[NEIGHBOR_C]>>2)?
				mv2D[y4x4AB[NEIGHBOR_C]][x4x4AB[NEIGHBOR_C]].y : 
			mv2D[y4x4AB[NEIGHBOR_C]][x4x4AB[NEIGHBOR_C]].y * 2) : 0;
		}
		else
		{
			mv_a = nMBs[NEIGHBOR_A]  ? (!mbIsMbField(nMBs[NEIGHBOR_A]) ?
				mv2D[y4x4AB[NEIGHBOR_A]][x4x4AB[NEIGHBOR_A]].y / 2 : 
			mv2D[y4x4AB[NEIGHBOR_A]][x4x4AB[NEIGHBOR_A]].y) : 0;
			mv_b = nMBs[NEIGHBOR_B]  ? (!GetFieldByPos(img,info,y4x4AB[NEIGHBOR_B]>>2,x4x4AB[NEIGHBOR_B]>>2) ?
				mv2D[y4x4AB[NEIGHBOR_B]][x4x4AB[NEIGHBOR_B]].y / 2 : 
			mv2D[y4x4AB[NEIGHBOR_B]][x4x4AB[NEIGHBOR_B]].y) : 0;
			mv_c = nMBs[NEIGHBOR_C]  ? (!GetFieldByPos(img,info,y4x4AB[NEIGHBOR_C]>>2,x4x4AB[NEIGHBOR_C]>>2) ?
				mv2D[y4x4AB[NEIGHBOR_C]][x4x4AB[NEIGHBOR_C]].y / 2 : 
			mv2D[y4x4AB[NEIGHBOR_C]][x4x4AB[NEIGHBOR_C]].y) : 0;
		}
		switch (mvPredType)
		{
		case MVPRED_MEDIAN:
			if(nMBs[NEIGHBOR_B] || nMBs[NEIGHBOR_C]){
				if (mv_a >= mv_b)
					rFrameL = (mv_c >= mv_a) ? mv_a : ((mv_c >= mv_b) ? mv_c : mv_b);
				else
					rFrameL = (mv_c >= mv_b) ? mv_b : ((mv_c >= mv_a) ? mv_c : mv_a);
			}
			else
				rFrameL = mv_a;
			break;
		case MVPRED_L:
			rFrameL = mv_a;
			break;
		case MVPRED_U:
			rFrameL = mv_b;
			break;
		case MVPRED_UR:
			rFrameL = mv_c;
			break;
		default:
			break;
		}
		if (hv==0) 
			pmv->x = rFrameL;
		else if(hv==1)
			pmv->y = rFrameL;
	}


}

#endif//FEATURE_INTERLACE
void SetMotionVectorPredictor (ImageParameters *img,TMBsProcessor *info,
							   avdNativeUInt			blkIdx,
							   avdNativeInt				list,                               
							   avdNativeInt				ref_frame,
							   AVDMotionVector			*pmv)
{
	MacroBlock *nMBs[3];
	AVDMotionVector **mv2D;
	avdNativeInt mv_a, mv_b, mv_c;
	avdNativeInt mvPredType, rFrameL, rFrameU, rFrameUR;
	StorablePicture	*dec_picture = img->dec_picture;
	avdNativeUInt block_x, block_y, blkShape;
    Macroblock* currMB = info->currMB;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	////TMBsProcessor *info  = img->mbsProcessor;
	MacroBlock *currMBInfo = info->currMB;
	avdNativeInt x4;
	avdNativeInt y4;
	avdNativeInt nCX, mv_a1;
	int					leftMBYForIntra = -1;
	AVDStoredPicMotionInfo *motionInfo;
	avdUInt8		**refList;
	TPosInfo*  posInfo  = GetPosInfo(info);
#if FEATURE_INTERLACE
	if(GetMbAffFrameFlag(img))
	{ 
		SetMotionVectorPredictorMBAff (img,info,
							   blkIdx,
							   list,                               
							   ref_frame,
							   pmv);
		return ;
	}
#endif//FEATURE_INTERLACE
	block_x = GetMVPBlockX(blkIdx);
	block_y = GetMVPBlockY(blkIdx);
	
	nMBs[NEIGHBOR_A] = block_x ? currMBInfo : GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,GetPosInfo(info)->subblock_y*4,16,&leftMBYForIntra):GetLeftMB(info);
	nMBs[NEIGHBOR_B] = block_y ? currMBInfo : GetUpMB(info);
	nMBs[NEIGHBOR_C] = NULL;

	nCX = block_x + GetMVPBlockXShape(blkIdx);
	if (nCX < 4){
		if (!(nCX == 2 && (block_y&1)))
			nMBs[NEIGHBOR_C] = nMBs[NEIGHBOR_B];
	}
	else if (!block_y)		
		nMBs[NEIGHBOR_C] = GetUpRightMB(info);

	x4 = posInfo->block_x + block_x;
	if (!nMBs[NEIGHBOR_C]){
		// use NEIGHBOR_D
		nMBs[NEIGHBOR_C] = block_x ? nMBs[NEIGHBOR_B]
		: block_y ? nMBs[NEIGHBOR_A] : GetUpLeftMB(info);
		nCX = x4 - 1;
	}
	else
		nCX += posInfo->block_x;

	if(img->notSaveAllMV)
		y4 = block_y;
	else //SAVE_ALL_MV
		y4 = posInfo->block_y + block_y;

	motionInfo = dec_picture->motionInfo;
	if (list == 0){
		refList = motionInfo->ref_idx0;
		mv2D = motionInfo->mv0;
	}
#if FEATURE_BFRAME
	else {
		refList = motionInfo->ref_idx1;
		mv2D = motionInfo->mv1;
	}
#endif

	
	rFrameL  = nMBs[NEIGHBOR_A]&&INSAMESLICE(currMB,nMBs[NEIGHBOR_A]) ? refList[y4>>1][(x4 - 1)>>1] : AVD_INVALID_REF_IDX;
	rFrameU  = nMBs[NEIGHBOR_B]&&INSAMESLICE(currMB,nMBs[NEIGHBOR_B]) ? refList[(y4 - 1)>>1][x4>>1] : AVD_INVALID_REF_IDX;
	rFrameUR = nMBs[NEIGHBOR_C]&&INSAMESLICE(currMB,nMBs[NEIGHBOR_C]) ? refList[(y4 - 1)>>1][nCX>>1] : AVD_INVALID_REF_IDX;
	
	

	// Prediction if only one of the neighbors uses the reference frame
	// we are checking
	mvPredType = MVPRED_MEDIAN;
	if(rFrameL == ref_frame){
		if ((rFrameU != ref_frame) & (rFrameUR != ref_frame))       
			mvPredType = MVPRED_L;
	}
	else {
		if((rFrameU == ref_frame) & (rFrameUR != ref_frame))  
			mvPredType = MVPRED_U;
		else if((rFrameU != ref_frame) & (rFrameUR == ref_frame))  
			mvPredType = MVPRED_UR;
	}

	// Directional predictions 
	blkShape = (blkIdx & (GET_MVP_BLOCKX_SHAPE_MASK | GET_MVP_BLOCKY_SHAPE_MASK));
	if (blkShape == ((2<<4) | (4<<12))) //(blockshape_x == 2 & blockshape_y == 4)
	{
		if (block_x){
			if(rFrameUR == ref_frame)
				mvPredType = MVPRED_UR;
		}
		else if(rFrameL == ref_frame)
			mvPredType = MVPRED_L;
	}
	else if (blkShape == ((4<<4) | (2<<12))) //(blockshape_y == 2 & blockshape_x == 4)
	{
		if (block_y){
			if(rFrameL == ref_frame)
				mvPredType = MVPRED_L;
		}
		else if(rFrameU == ref_frame)
			mvPredType = MVPRED_U;
	}

#define DUMP_MV2 0
	switch (mvPredType)
	{
	case MVPRED_MEDIAN:
		if (nMBs[NEIGHBOR_A]){
			mv_a  = mv2D[y4][x4-1].x;
			mv_a1 = mv2D[y4][x4-1].y;
		}
		else
			mv_a = mv_a1 = 0;

		if(nMBs[NEIGHBOR_B] || nMBs[NEIGHBOR_C]){
			mv_b = nMBs[NEIGHBOR_B] ? mv2D[y4-1][x4].x : 0;
			mv_c = nMBs[NEIGHBOR_C] ? mv2D[y4-1][nCX].x : 0;
#if DUMP_MV2//(DUMP_VERSION & DUMP_SLICE)
			AvdLog(DUMP_SLICE,DUMP_DCORE "\n(h8f_tmp4) nMBs[NEIGHBOR_C]=%d,mv2D[y4-1][nCX].y=%d,(%d,%d)\n", 
				nMBs[NEIGHBOR_C]!=0,
				mv2D[y4-1][nCX].x,
				posInfo->block_y + block_y-1,
				nCX
				);
#endif
			if (mv_a >= mv_b)
				pmv->x = (mv_c >= mv_a) ? mv_a : ((mv_c >= mv_b) ? mv_c : mv_b);
			else
				pmv->x = (mv_c >= mv_b) ? mv_b : ((mv_c >= mv_a) ? mv_c : mv_a);
#if DUMP_MV2//(DUMP_VERSION & DUMP_SLICE)
			AvdLog(DUMP_SLICE,DUMP_DCORE "\n(h8f_tmp3) mv_c=%d,mv_a=%d,mv_b=%d,nCX=%d\n", 
				mv_c,
				mv_a,
				mv_b,
				nCX
				);
#endif
			mv_b = nMBs[NEIGHBOR_B] ? mv2D[y4-1][x4].y : 0;
			mv_c = nMBs[NEIGHBOR_C] ? mv2D[y4-1][nCX].y : 0;

			if (mv_a1 >= mv_b)
				pmv->y = (mv_c >= mv_a1) ? mv_a1 : ((mv_c >= mv_b) ? mv_c : mv_b);
			else
				pmv->y = (mv_c >= mv_b) ? mv_b : ((mv_c >= mv_a1) ? mv_c : mv_a1);

		}
		else{
			*(avdUInt32 *)pmv = ((mv_a&0xffff) | (mv_a1<<16));
		}
		break;
	case MVPRED_L:
		*(avdUInt32 *)pmv = *(avdUInt32 *)&mv2D[y4][x4-1];
		break;
	case MVPRED_U:
		*(avdUInt32 *)pmv = *(avdUInt32 *)&mv2D[y4-1][x4];
		break;
	case MVPRED_UR:
		*(avdUInt32 *)pmv = *(avdUInt32 *)&mv2D[y4-1][nCX];
		break;
	default:
		break;
	}
#if DUMP_MV2//(DUMP_VERSION & DUMP_SLICE)
	AvdLog(DUMP_SLICE,DUMP_DCORE "\n(h8f_tmp2) mvPredType=%d,MV=(%d,%d),a=%d,b=%d,c=%d\n", 
		mvPredType,pmv->x,pmv->y,
		nMBs[NEIGHBOR_A]!=0,
		nMBs[NEIGHBOR_B]!=0,
		nMBs[NEIGHBOR_C]!=0
		);
#endif

}

/*!
************************************************************************
* \brief
*    Interpret the mb mode for P-Frames
************************************************************************
*/

const avdUInt8 ICBPTAB[6] = {0,16,32,15,31,ALLCBPNONZERO};
void interpret_mb_mode_P(ImageParameters *img,TMBsProcessor *info)
{
	Macroblock *currMB = info->currMB;
	avdNativeUInt newType, mbmode, b8P = B8FWPRED;
	TMBsParser  *parser = GetMBsParser(info);
	TMBBitStream	*mbBits = GetMBBits(currMB);
	int i16mode = 0;
	newType = mbmode = mbGetMbType(currMB);
	
#define ZERO_P8x8     (mbmode==5)
#define MODE_IS_P8x8  (mbmode==4 || mbmode==5)
#define MODE_IS_I4x4  (mbmode==6)
#define I16OFFSET     (mbmode-7)
#define MODE_IS_IPCM  (mbmode==31)

	if(mbmode<0||mbmode>31)
	{
		mbmode = ZERO_P8x8;
		AVD_ERROR_CHECK2(img,"interpret_mb_mode_P,mbmode<0||mbmode>31",ERROR_InvalidMBMode);
	}

	switch(mbmode){
	case 0:
	case 1:
	case 2:
	case 3:
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\n%s(%d):F", MBModeName[mbmode], mbmode-1);
		_dumpMbMode[mbmode]++;
#endif //(DUMP_VERSION & DUMP_SLICE)



		break;
	case 4:
	case 5:
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\nP8x8(%d):", mbmode-1);
#endif //(DUMP_VERSION & DUMP_SLICE)
		newType = P8x8;
		b8P = 0; // not to set;
		break;
	case 6:
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\nI4MB, i4mode(%d):", mbmode-1);
		_dumpMbMode[10]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
		newType = I4MB;
		b8P = B8NOPRED;
		mbmode = IBLOCK;
		break;
#ifndef VOI_H264D_BLOCK_IPCM
	case 31:
		newType = IPCM;
		parser->cbp = ALLCBPNONZERO;
		i16mode = 0;
		mbmode = 0;
		break;
#endif //VOI_H264D_BLOCK_IPCM
	default:
		newType = I16MB;
		parser->cbp = ICBPTAB[(I16OFFSET)>>2];
		i16mode = (I16OFFSET) & 0x03;
#if (DUMP_VERSION & DUMP_SLICE)
		//AvdLog(DUMP_SLICE, "\nI16MB: cbp = %d, i16mode = %s(%d)", parser->cbp,IPredName16x16[i16mode], mbmode-1);
		AvdLog(DUMP_SLICE,DUMP_DCORE "\nI16MB: cbp = %d, i16mode = (%d)", parser->cbp, mbmode-1);

		_dumpIP16[i16mode]++;
		_dumpMbMode[9]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
		b8P = B8NOPRED;
		mbmode = 0;
		break;
	}
	info->mbsParser->allrefzero = ZERO_P8x8;		

	MEMSET_B8PDIR(b8P);
	MEMSET_B8MODE(mbmode);
	mbSetMbType(currMB, newType);
	mbSetI16PredMode(mbBits,i16mode);
}

/*!
************************************************************************
* \brief
*    Interpret the mb mode for I-Frames
************************************************************************
*/
void interpret_mb_mode_I(ImageParameters *img,TMBsProcessor *info)
{
	Macroblock *currMB = info->currMB;
	avdNativeInt newType, mbmode;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	TMBsParser *parser = GetMBsParser(info);
	int i16mode = 0;
	newType = mbmode = mbGetMbType(currMB);
	if(mbmode<0||mbmode>25)
	{
		mbmode = 0;
		AVD_ERROR_CHECK2(img,"interpret_mb_mode_I(),mbmode<0||mbmode>25",ERROR_InvalidMBMode);
	}
	if (mbmode==0)
	{
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\nI4MB, i4mode:(%d)", mbmode);
		_dumpMbMode[10]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
		newType = I4MB;
		mbmode = IBLOCK;
	}
	else 
#ifndef VOI_H264D_BLOCK_IPCM
		if (mbmode != 25)
#endif  // VOI_H264D_BLOCK_IPCM
		{
			newType = I16MB;
			parser->cbp = ICBPTAB[(mbmode-1)>>2];
			i16mode = (mbmode-1) & 0x03;
#if (DUMP_VERSION & DUMP_SLICE)
			//AvdLog(DUMP_SLICE, "\nI16MB: cbp = %d, i16mode = %s(%d)", parser->cbp,IPredName16x16[i16mode], mbmode);
			AvdLog(DUMP_SLICE,DUMP_DCORE "\nI16MB: cbp = %d, i16mode = (%d)", parser->cbp, mbmode);		
			_dumpIP16[i16mode]++;
			_dumpMbMode[9]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
			mbmode = 0;
		}
#ifndef VOI_H264D_BLOCK_IPCM
		else
		{
			newType = IPCM;
			parser->cbp = ALLCBPNONZERO;
			i16mode = 0;
			mbmode = 0;
#if (DUMP_VERSION & DUMP_SLICE)
			AvdLog(DUMP_SLICE,DUMP_DCORE "\nIPCM: cbp = %d, i16mode =(%d)", 
				ALLCBPNONZERO, 25);

#endif //(DUMP_VERSION & DUMP_SLICE)
		}
#endif

		MEMSET_B8PDIR(B8NOPRED);
		MEMSET_B8MODE(mbmode);
		mbSetMbType(currMB, newType);
		mbSetI16PredMode(mbBits,i16mode);
}

/*!
************************************************************************
* \brief
*    Interpret the mb mode for B-Frames
************************************************************************
*/



/*!
************************************************************************
* \brief
*    Sets mode for 8x8 block
************************************************************************
*/
void SetB8Mode (ImageParameters *img,TMBsProcessor *info,Macroblock* currMB, avdNativeInt value, avdNativeInt i)
{
	//static const avdInt8 p_v2b8 [ 5] = {4, 5, 6, 7, IBLOCK};
	//static const avdInt8 p_v2pd [ 5] = {0, 0, 0, 0, -1};
	TMBBitStream	*mbBits = GetMBBits(currMB);
#if FEATURE_BFRAME
	
	if (img->type==B_SLICE)
	{
		static const avdUInt8 b_v2b8 [14] = {0, 4, 4, 4, 5, 6, 5, 6, 5, 6, 7, 7, 7, IBLOCK};
		static const avdUInt8 b_v2pd [14] = {2, 0, 1, 2, 0, 0, 1, 1, 2, 2, 0, 1, 2, -1};
		mbBits->uMBS.b8Info.b8Mode[i] = (avdUInt8)b_v2b8[value];
		mbBits->uMBS.b8Info.b8Pdir[i] = (avdUInt8)b_v2pd[value];
	}
	else
#endif //VOI_H264D_NON_BASELINE
	{
		if(value<0||value>=4)
		{
			value = 0;
			AVD_ERROR_CHECK2(img,"B8Mode is not correct",ERROR_InvalidB8Mode);
		}
		mbBits->uMBS.b8Info.b8Mode[i] = (avdUInt8)(value + 4);
		mbBits->uMBS.b8Info.b8Pdir[i] = 0;
	}
#if 0//(DUMP_VERSION & DUMP_SLICE)
	AvdLog(DUMP_SLICE,DUMP_DCORE "\n%s(%s), ", 
		mbBits->uMBS.b8Info.b8Mode[i]==IBLOCK?MBModeName[8]:MBModeName[mbBits->uMBS.b8Info.b8Mode[i]], 
		mbBits->uMBS.b8Info.b8Pdir[i]==B8NOPRED? "NA":MCDirName[mbBits->uMBS.b8Info.b8Pdir[i]]);
	mbBits->uMBS.b8Info.b8Mode[i]==IBLOCK?_dumpMbMode[8]++:
		_dumpMbMode[mbBits->uMBS.b8Info.b8Mode[i]]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
}

avdNativeInt GetPSliceSkippedMV(ImageParameters *img,TMBsProcessor *info)
{
	avdNativeInt zeroMotionAbove, zeroMotionLeft, i;
	StorablePicture	*dec_picture = img->dec_picture;
	avdNativeUInt mvWidth = (dec_picture->size_x>>2);
	AVDStoredPicMotionInfo *motionInfo = dec_picture->motionInfo;
	//TMBsProcessor *info  = img->mbsProcessor;
	avdUInt32 *tmp, *tmp2;
	AVDMotionVector pmv = {0, 0};
	Macroblock *currMB = info->currMB;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	avdNativeInt y4 ;
	int					leftMBYForIntra = -1;
	Macroblock *leftMB = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,0,16,&leftMBYForIntra):GetLeftMB(info);
	



	//MacroBlock **mbAB = &nbInfo->neighborABCD[0];
#if FEATURE_CABAC
	MacroBlock		*currMBInfo = info->currMB;
	avdNativeInt listIdx = (img->type == B_SLICE) ? 2 : 1; // for list 0 & 1;
	Slice *currSlice = img->currentSlice;
#endif//FEATURE_CABAC
	//TBD:CheckInSameSlice
	if(img->notSaveAllMV)
		y4 = 0;
	else
		y4 = posInfo->block_y;
#if FEATURE_INTERLACE	
	if(GetMbAffFrameFlag(img))
	{
		if(leftMB)
		{
			int y = GetAbsYPos(img,info,NEIGHBOR_A,0,MB_BLOCK_SIZE)>>2;
			AVDMotionVector* mv = &motionInfo->mv0[y][posInfo->block_x - 1];
			
			int mv_x = mv->x;
			int mv_y =  mbIsMbField(currMB)&&(!mbIsMbField(leftMB))? (mv->y==-1? 0 :mv->y>>1): mv->y;//note:-1>>1 == -1
			int refIdx = motionInfo->ref_idx0[y>>1][(posInfo->block_x - 1)>>1];
			if(!mbIsMbField(currMB)&&(mbIsMbField(leftMB)))//frame2field
			{
				refIdx>>=1;
			}
			
			zeroMotionLeft = mv_y==0&&mv_x==0&&refIdx==0?1:0;
			
		}
		else
			zeroMotionLeft = 1;
		

	}
	else
#endif//FEATURE_INTERLACE
	{
		zeroMotionLeft = (!leftMB ||!INSAMESLICE(currMB,leftMB)||
			(!*((avdInt32 *)&motionInfo->mv0[y4][posInfo->block_x - 1])
			&& !motionInfo->ref_idx0[y4>>1][(posInfo->block_x - 1)>>1])) 
			? 1 : 0;
	}
	

	if (!zeroMotionLeft)
	{
		MacroBlock* upMB = GetUpMB(info);
#if FEATURE_INTERLACE		
		if(GetMbAffFrameFlag(img))
		{
			if(upMB)
			{
				int y = GetAbsYPos(img,info,NEIGHBOR_B,-1,MB_BLOCK_SIZE)>>2;
				AVDMotionVector* mv = &motionInfo->mv0[y-1][posInfo->block_x];

				int mv_x = mv->x;
				int mv_y =  mbIsMbField(currMB)&&(!mbIsMbField(upMB))? (mv->y==-1? 0 :mv->y>>1): mv->y;
				int refIdx = motionInfo->ref_idx0[y>>1][(posInfo->block_x)>>1];
				if(!mbIsMbField(currMB)&&(mbIsMbField(upMB)))//frame2field
				{
					refIdx>>=1;
				}

				zeroMotionAbove = mv_y==0&&mv_x==0&&refIdx==0?1:0;
			}
			else
				zeroMotionAbove = 1;
		}
		else
#endif//FEATURE_INTERLACE
		{
			zeroMotionAbove = (!upMB ||!INSAMESLICE(currMB,upMB)||
				(!*((avdInt32 *)&motionInfo->mv0[y4-1][posInfo->block_x])
				&& !motionInfo->ref_idx0[(y4-1)>>1][posInfo->block_x>>1])) 
				? 1 : 0;
		}
	


		if (!zeroMotionAbove){
			int j;
			SetMotionVectorPredictor (img,info,(4<<4)|(4<<12), LIST_0, 0, &pmv);
			if(img->notSaveAllMV==0)
			{
				tmp = (avdUInt32 *)&motionInfo->mv0[y4][posInfo->block_x];
				tmp2 = (avdUInt32 *)&pmv;
				for(j=0;j<BLOCK_SIZE;j++){
					*tmp = *(tmp + 1) = *(tmp + 2) = *(tmp + 3) = *tmp2;
					tmp += mvWidth;
				}
			}
		}
	}

	if(img->notSaveAllMV)
	{
		//AvdLog(DUMP_SEQUENCE,"RIM2_GETPSKIPED\n");
		//CHECK_MOTIONINFO(img,motionInfo);
		// only update MB boundary;
		tmp = (avdUInt32 *)&motionInfo->mv0[0][posInfo->block_x];
		tmp2 = (avdUInt32 *)&pmv;
		//CHECK_POINTER(img,tmp,"motionInfo->mv0[0]==0\n");
		*tmp = *(tmp + 1) = *(tmp + 2) = *(tmp + 3) = *tmp2;
		tmp += mvWidth;
		*tmp = *(tmp + 3) = *tmp2;
		tmp += mvWidth;
		*tmp = *(tmp + 3) = *tmp2;
		//mv0[3] & mv0[-1] may exchage address;
		tmp = (avdUInt32 *)&motionInfo->mv0[3][posInfo->block_x];
		*tmp = *(tmp + 1) = *(tmp + 2) = *(tmp + 3) = *tmp2;
	}
	else
	{
		y4 >>= 1;
	}

	i = (posInfo->block_x>>1);
	motionInfo->ref_idx0[y4][i] = 
		motionInfo->ref_idx0[y4][i+1] = 
		motionInfo->ref_idx0[y4+1][i] = 
		motionInfo->ref_idx0[y4+1][i+1] = 0;
#if FEATURE_CABAC
	if (!IsVLCCoding(img->active_pps)){ 
		int					leftMBYForIntra = -1;
		MacroBlock   *leftMBInfo = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,0,16,&leftMBYForIntra):GetLeftMB(info);
		avdNativeInt size = 4;
		avdInt32 *tmp32Pt2 = (avdInt32 *)&niGetCabacNeighborStruct(currMBInfo)->mvd[0][0];
		avdInt32 *tmp32Pt = (avdInt32 *)&niGetCabacNeighborStruct(currMBInfo)->mvdLeft[0][0];
		do {
			*tmp32Pt++ = *tmp32Pt2++ = 0;
		} while (--size);
	}
#endif
	return DECODE_MB;
}




#if (DUMP_VERSION & DUMP_SLICE)
void DumpMBBeforeDeblock(ImageParameters *img,TMBsProcessor* info,int mbStart,int mbEnd)
{
 TSizeInfo	*sizeInfo = img->sizeInfo;
 if(_dmpSlcFpData[0])
 {
	int isCurrMB = mbEnd-mbStart==1;
	while (mbStart<mbEnd)
	{
		int j,i,mbx,mby,x,y;
		StorablePicture	*dec_picture = img->dec_picture;
		avdUInt8 *img1;
		if(isCurrMB)
		{
			mby = mbStart/sizeInfo->PicWidthInMbs;
			mbx = mbStart-mby*sizeInfo->PicWidthInMbs;
		}
		else
		{
			mby = info->mb_x;
			mbx = info->mb_y;
		}
		
		y = mby*16;
		x = mbx*16;
		img1 = dec_picture->plnY+(y*sizeInfo->yPlnPitch+x);
		AvdLog(DUMP_SLICEDATA,DUMP_DCORE"\n=======Dump one MB======poc=%d,MB=%d(%d,%d),frNum=%d\n",img->pocInfo->currPoc,mbStart++,x,y,
			dec_picture->pic_num
		);
		AvdLog(DUMP_SLICEDATA,DUMP_DCORE"====Luma====");
		for(j=0;j<16;j++)
		{
			AvdLog(DUMP_SLICEDATA,DUMP_DCORE "\nLine%d ",j);
			for(i=0;i<16;i++)
			{
				AvdLog(DUMP_SLICEDATA,DUMP_DCORE "%d ", img1[j*sizeInfo->yPlnPitch+i]);
			}
		}
		AvdLog(DUMP_SLICEDATA,DUMP_DCORE"\n====Cb====\n");
		y = mby*8;
		x = mbx*8;
		img1 = dec_picture->plnU+(y*(sizeInfo->yPlnPitch>>1)+x);
		for(j=0;j<8;j++)
		{
			AvdLog(DUMP_SLICEDATA,DUMP_DCORE "\nLine%d ",j);
			for(i=0;i<8;i++)
			{
				AvdLog(DUMP_SLICEDATA,DUMP_DCORE "%d ", img1[j*(sizeInfo->yPlnPitch>>1)+i]);
			}
		}
		AvdLog(DUMP_SLICEDATA,DUMP_DCORE"\n====Cr====\n");
		img1 = dec_picture->plnV+(y*(sizeInfo->yPlnPitch>>1)+x);
		for(j=0;j<8;j++)
		{
			AvdLog(DUMP_SLICEDATA,DUMP_DCORE "\nLine%d ",j);
			for(i=0;i<8;i++)
			{
				AvdLog(DUMP_SLICEDATA,DUMP_DCORE "%d ", img1[j*(sizeInfo->yPlnPitch>>1)+i]);
			}
		}

		if(isCurrMB)
			break;
	}
 }
}

void Dumploop(int dumpflag,TMBsProcessor* info,char* name,int count,void* src,DUMP_DataType type)
{
	if ((DUMP_VERSION & dumpflag)==0)
		return;
	switch(type) {
	case DDT_D8:
		{
			char* data = src;
			int i;
			if (name)
				AvdLog(dumpflag,DUMP_DCORE"\n%s\n",name);
			for(i=0;i<count;i++)
			{
				AvdLog(dumpflag,DUMP_DCORE"[%d]=%d,",i,data[i]);	
			}

		}
		break;
	case DDT_D16:
		{
			short* data = src;
			int i;
			if (name)
				AvdLog(dumpflag,DUMP_DCORE"\n%s\n",name);
			for(i=0;i<count;i++)
			{
				AvdLog(dumpflag,DUMP_DCORE"[%d]=%d,",i,data[i]);	
			}

		}
		break;
	case DDT_D32:
		{
			int* data = src;
			int i;
			if (name)
				AvdLog(dumpflag,DUMP_DCORE"\n%s\n",name);
			for(i=0;i<count;i++)
			{
				AvdLog(dumpflag,DUMP_DCORE"[%d]=%d,",i,data[i]);	
			}

		}
		break;
	default:
		break;
	}
	AvdLog(dumpflag,DUMP_DCORE"\n");	
}
void Dumploop2(int dumpflag,TMBsProcessor* info,char* name,int count1,int count2,void* src,DUMP_DataType type,int stride)
{
	if ((DUMP_VERSION & dumpflag)==0)
		return;
	switch(type) {
	case DDT_D8:
		{
			char* data = src;
			int i;
			if (name)
				AvdLog(dumpflag,DUMP_DCORE"\n%s\n",name);
			for(i=0;i<count1;i++)
			{
				Dumploop(dumpflag,info,NULL,count2,data,type);
				data+=stride;
			}

		}
		break;
	case DDT_D16:
		{
			short* data = src;
			int i;
			if (name)
				AvdLog(dumpflag,DUMP_DCORE"\n%s\n",name);
			for(i=0;i<count1;i++)
			{
				Dumploop(dumpflag,info,NULL,count2,data,type);
				data+=stride;
			}

		}
		break;
	case DDT_D32:
		{
			int* data = src;
			int i;
			if (name)
				AvdLog(dumpflag,DUMP_DCORE"\n%s\n",name);
			for(i=0;i<count1;i++)
			{
				Dumploop(dumpflag,info,NULL,count2,data,type);
				data+=stride;
			}

		}
		break;
	default:
		break;
	}
}
#if 0
void Dumploop3Int(int dumpflag,char* name,int count1,int count2,int count3,int* src,DUMP_DataType type)
{

	int* data = src;
	int i;
	if ((DUMP_VERSION & dumpflag)==0)
		return;
	if (name)
		AvdLog(dumpflag,DUMP_DCORE"\n%s\n",name);
	for(i=0;i<count1;i++)
	{
		Dumploop2(dumpflag,info,NULL,count2,count3,data,type);
		data+=count2*count3;
	}
}
#endif//0
#endif //(DUMP_VERSION & DUMP_SLICE)		


/*!
************************************************************************
* \brief
*    decodes one slice
************************************************************************
*/
//#define CHECK_ERR_RET_POS  if(img->error<0) goto RETPOS;
static int IsWorkThreadEOFSlice(ImageParameters *img)
{
	int i,coreNum = img->vdLibPar->multiCoreNum;
	for (i=1;i<coreNum;i++)
	{
		TMBsProcessor* info2 = img->mbsProcessor[i];
		if (info2->end_of_slice)
		{
			return 1;
		}
	}
	return 0;
}

#ifdef USE_JOBPOOL

int   parsembs(ImageParameters *img,TMBsProcessor* info)
{
	avdNativeUInt i,l, k;
	avdNativeInt read_flag, tmpInt, startMBX, endMBX, startMBNr, endMBNr;
	avdNativeInt end_of_slice = VOI_FALSE;

	TPosInfo*  posInfo  = GetPosInfo(info);
	Slice *slice = img->currentSlice;
	TFuncInfo	*funcInfo = img->funcInfo;
	TMBsParser*	 parser = info->mbsParser;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	int decNum=0;
	int coreNum = img->vdLibPar->multiCoreNum;
	TMBsProcessor* anotherProcessor = info->anotherProcessor;
	int exit = 0;
	int height = sizeInfo->FrameHeightInMbs;
	int width  = sizeInfo->PicWidthInMbs;
	int endOfRow = 0;
	info->flag = 0;
	
	if(info->mb_y == 16)
	{
	     exit = 0;
	}

	do 
	{
		//error check
		CHECK_ERR_BREAK	
	
		switch (info->status)
		{
		    case EMP_READING:
			CHECK_ERR_BREAK
				info->start_mb_nr = img->currentSlice->start_mb_nr;
				info->decNum = read_mbs(img,info,sizeInfo->PicWidthInMbs - 1);CHECK_ERR_BREAK
				SetProcessorStatus(img,info,EMP_DECODING);
				exit = 1;
				break;

		}
	} while (!exit);//end_of_slice = VOI_FALSE);

	AvdLog2(LL_INFO, "framepoc %d parse start_mb_nr %d,  decNum %d end_of_slice %d error %d\n", img->pocInfo->framepoc,  info->start_mb_nr, info->decNum, info->end_of_slice, img->error);

#ifdef USE_JOBPOOL1
    if(img->error < 0)
    {
        int i;
#if defined(WIN32) || defined(WINCE)
    WaitForSingleObject(img->JobMutex, INFINITE);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
     pthread_mutex_lock(&img->JobMutex);
#endif
    
    img->end_of_frame = 1;
#if defined(WIN32) || defined(WINCE)
	 for(i = 1; i < img->vdLibPar->multiCoreNum; i++)
            SetEvent(img->mcoreThreadParam[i]->JobEvent);
    ReleaseMutex(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
   for(i = 1; i < img->vdLibPar->multiCoreNum; i++)
            VOH264SetEvent(&img->mcoreThreadParam[i]->JobEvent);
     pthread_mutex_unlock(&img->JobMutex);
#endif

    }
#endif
	return info->end_of_slice;

}

int processmbs(ImageParameters *img,TMBsProcessor* info)
{
	avdNativeUInt i,l, k;
	avdNativeInt read_flag, tmpInt, startMBX, endMBX, startMBNr, endMBNr;
	avdNativeInt end_of_slice = VOI_FALSE;

	TPosInfo*  posInfo  = GetPosInfo(info);
	Slice *slice = img->currentSlice;
	TFuncInfo	*funcInfo = img->funcInfo;
	TMBsParser*	 parser = info->mbsParser;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	int decNum=0;
	int coreNum = img->vdLibPar->multiCoreNum;
	int exit = 0;
	int height = sizeInfo->FrameHeightInMbs;
	int width  = sizeInfo->PicWidthInMbs;
	int endOfRow = 0;
	TMCoreThreadParam* threadParam = info->mcoreThreadParam;	
	info->flag = 0;
	if(info->mb_y == 16)
	{
	     exit = 0;
	}

#if defined(WIN32) || defined(WINCE)
    AvdLog2(LL_INFO, "====>Enter Thread ID %p processmbs start_mb_nr %d,  decNum %d end_of_slice %d\n", GetCurrentThreadId(), info->start_mb_nr, info->decNum, info->end_of_slice);
#endif
	do 
	{
		//error check
		CHECK_ERR_BREAK	

		switch (info->status)
		{
		case EMP_DECODING:
#if USE_IMG_MIRROR			
			decode_mbs(img->curr_slice_img[info->current_slice_nr -1],info,info->decNum);CHECK_ERR_BREAK
			info->flag |= 4;//4 makes no sense but just for mul-thread debugging
			img->error |= img->curr_slice_img[info->current_slice_nr -1]->error;
#else
			decode_mbs(img, info,info->decNum);CHECK_ERR_BREAK
			info->flag |= 4;//4 makes no sense but just for mul-thread debugging
#endif
#if defined(WIN32) || defined(WINCE)
                     AvdLog2(LL_INFO, "Thread ID %p processmbs start_mb_nr %d,	decNum %d end_of_slice %d\n", GetCurrentThreadId(), info->start_mb_nr, info->decNum, info->end_of_slice);
#else			
	              AvdLog2(LL_INFO, "processmbs start_mb_nr %d,  decNum %d end_of_slice %d\n", info->start_mb_nr, info->decNum, info->end_of_slice);		
#endif
			exit = 1;
			break;

		}

	} while (!exit);//end_of_slice = VOI_FALSE);


#if defined(WIN32) || defined(WINCE)	 
    WaitForSingleObject(img->JobMutex, INFINITE);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)	 
    pthread_mutex_lock(&img->JobMutex);
#endif
	
	

#if defined(WIN32) || defined(WINCE)
	AvdLog2(LL_INFO, "<====Exit Thread ID %p processmbs start_mb_nr %d,  decNum %d end_of_slice %d img error %d\n", GetCurrentThreadId(), info->start_mb_nr, info->decNum, info->end_of_slice, img->error);
#endif

	if(img->error>=0)
	{
		  SetProcessorStatus(img,info,EMP_READING);
                info->job_state = EMP_JOB_FINISHED; 
	}
	else
	{
		  SetProcessorStatus(img,info,EMP_READING);
                info->job_state = EMP_JOB_ERROR; 	
	}

	if(threadParam != NULL)//work thread
	{
		threadParam->errorID = img->error;
	}     	
		
	return img->current_mb_nr;

}

#else
int processmbs(ImageParameters *img,TMBsProcessor* info)
{
	avdNativeUInt i,l, k;
	avdNativeInt read_flag, tmpInt, startMBX, endMBX, startMBNr, endMBNr;
	avdNativeInt end_of_slice = VOI_FALSE;

	TPosInfo*  posInfo  = GetPosInfo(info);
	Slice *slice = img->currentSlice;
	TFuncInfo	*funcInfo = img->funcInfo;
	TMBsParser*	 parser = info->mbsParser;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	int decNum=0;
	int coreNum = img->vdLibPar->multiCoreNum;
	TMBsProcessor* anotherProcessor = info->anotherProcessor;
	int exit = 0;
	int height = sizeInfo->FrameHeightInMbs;
	int width  = sizeInfo->PicWidthInMbs;
	int endOfRow = 0;
	TMCoreThreadParam* threadParam = info->mcoreThreadParam;
	info->flag = 0;
	if(info->mb_y == 16)
	{
	     exit = 0;
	}

	do 
	{
		//error check
		CHECK_ERR_BREAK	
		if(threadParam&&threadParam->status==ETSDB_STOP)
			return 0;
		switch (info->status)
		{
		case EMP_WAIT_DATA:
			CHECK_ERR_BREAK
				if(IsReadyRead(img,info,anotherProcessor))
					SetProcessorStatus(img,info,EMP_READING);
				else if(!(threadParam==NULL&&IsWorkThreadEOFSlice(img)))
					Wait2(img,info,0);
				break;
		case EMP_READING:
			CHECK_ERR_BREAK
				if(!IsReadyRead(img,info,anotherProcessor))//for thread safe check it again before reading
				{
					TAvdLog(LL_INFO,"!!somehow, the other is eos, but the signal is delayed,return to Wait status\n");
					SetProcessorStatus(img,info,EMP_WAIT_DATA);
					break;
				}
				info->start_mb_nr = img->currentSlice->start_mb_nr;
				decNum = read_mbs(img,info,sizeInfo->PicWidthInMbs - 1);CHECK_ERR_BREAK
				SetProcessorStatus(img,info,EMP_DECODING);
				//break;

		case EMP_DECODING:
			decode_mbs(img,info,decNum);CHECK_ERR_BREAK
			info->flag |= 4;//4 makes no sense but just for mul-thread debugging
			exit = 1;
			break;

		}

		if(threadParam==NULL&&IsWorkThreadEOFSlice(img))
		{
			info->end_of_slice = 2;//2 makes no sense but just for mul-thread debugging
			info->flag |= 2;
			exit = 1;
		}

		if(GetMBY(info)>=height)//for the safe quit of worker thread
		{
			info->flag |= 16;//3 makes no sense but just for mul-thread debugging
			exit = 1;
		}
	} while (!exit);//end_of_slice = VOI_FALSE);
	


	if(img->error>=0)
	{
		SetProcessorStatus(img,info,EMP_WAIT_DATA);
	}
	

	//for thread safe check it again before reading new slice
	if(threadParam==NULL)//current is main thread,sync up the workthread when eof or error
	{
		if(img->error<0)
		{
			//WaitWorkThreadRet()			
			for (i=1;i<coreNum;i++)
			{
				TMBsProcessor* info2 = img->mbsProcessor[i];
				while(info2&&info2->status!=EMP_WAIT_DATA_ERROR)
				{
					TAvdLog(LL_INFO,"WaitWorkThreadRet when error\n");
					Wait2(img,info,0);
				}
			}
		
		}
		else if(info->end_of_slice)//working thread is reading/decoding
		{		
			for (i=1;i<coreNum;i++)
			{
				TMBsProcessor* info2 = img->mbsProcessor[i];
				int waitNum = 0;
				while(info2->status>=EMP_READING)
				{
#define MAX_WAIT_TIME1 500				
					TAvdLog(LL_INFO,"WaitWorkThreadRet when end of slice of the main thread,mb_y (w=%d,m=%d)\n",info2->mb_y,info->mb_y);
					//in theory, the current mb_y should be less than the workthread,but it sometimes happens that the mb_y of main thread does not
					//jump to next two row,the reason is still not clear,so add the following check
					if((info->end_of_slice==1)&&(anotherProcessor->mb_y-info->mb_y==(1+GetMbAffFrameFlag(img)))&&(waitNum++>MAX_WAIT_TIME1))
					{
						GetMBY(info)  += (1 + GetMbAffFrameFlag(img))*coreNum;
						TAvdLog(LL_INFO,"WorkAround: enforce the current mby plus 2\n");
					}
					Wait2(img,info,0);
				}
			}
			
		}

	}
	else//work thread
	{
		threadParam->errorID = img->error;
	}
	
	return img->current_mb_nr;

}
#endif

int processmbsSingleCore(ImageParameters *img,TMBsProcessor* info)
{
	avdNativeUInt l, k;
	avdNativeInt read_flag, tmpInt, startMBX, endMBX, startMBNr, endMBNr;
	avdNativeInt end_of_slice = VOI_FALSE;

	TPosInfo*  posInfo  = GetPosInfo(info);
	Slice *slice = img->currentSlice;
	TFuncInfo	*funcInfo = img->funcInfo;
	TMBsParser*	 parser = info->mbsParser;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	int numRows ;
	int step = img->vdLibPar->rowNumOfMBs;
	int decNum,start;

	do 
	{
		if(step)
			numRows = GetMBX(info)+ step;
		else
			numRows = sizeInfo->PicWidthInMbs-1;
TIME_BEGIN(start)
#if        CALC_THUMBNAIL
             if(img->type == I_SLICE)
                 start = GetTickCount();
#endif
		decNum = read_mbs(img,info,numRows);
TIME_END(start,readSum)
#if        CALC_THUMBNAIL
             if(img->type == I_SLICE)
                 img->parse_ms += GetTickCount() - start;
#endif	
		CHECK_ERR_RET_INT
TIME_BEGIN(start)
#if        CALC_THUMBNAIL
             if(img->type == I_SLICE)
                 start = GetTickCount();
#endif
		decode_mbs(img,info,decNum);
TIME_END(start,decSum)
#if        CALC_THUMBNAIL
             if(img->type == I_SLICE)
                 img->decode_ms += GetTickCount() - start;
#endif		
		CHECK_ERR_RET_INT
		if(img->deblockType!=DEBLOCK_BY_MB&&GetMBX(info) == sizeInfo->PicWidthInMbs-1)
		{
			TIME_BEGIN(start)
			deblock_mbs(img,info,sizeInfo->PicWidthInMbs-1);
			TIME_END(start,deblockSum)
			CHECK_ERR_RET_INT
		}
			
		if (GetMBX(info) == sizeInfo->PicWidthInMbs-1){

			GetMBY(info) += 2  + GetMbAffFrameFlag(img)*2;//jump to next 
			GetMBX(info)  = -1;
			//info->dec_mbaff_pos =
			info->dec_mb_num  =
			info->read_mb_num = 0;
		}
	} while (!info->end_of_slice&&GetMBX(info)!=-1);

	return 0;

}




int read_mbs(ImageParameters *img,TMBsProcessor* info, int endMBX)
{
	avdNativeInt end_of_slice = VOI_FALSE;
	TFuncInfo	*funcInfo = img->funcInfo;
	TMBsParser*	 parser = info->mbsParser;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo	*posInfo  = GetPosInfo(info);
	TMBBitStream	*mbBits;
	MacroBlock* currMB;
	Slice *slice = img->currentSlice;
	
	DataPartition *dP = &(slice->partArr[0]);
	Bitstream *bs = dP->bitstream;
	int number = 0;
	int x_stride = 0;
	int exitFlag = 1;
	int enableReadMB = !(img->vdLibPar->optFlag&OPT_DISABLE_DECODE);
#if FEATURE_INTERLACE
	info->currMBisBottom = 0;//for error resilience, make sure the current MB is top
#endif//FEATURE_INTERLACE
#ifdef X86_TEST
	//TAvdLog(LL_INFO,"@!@sliceID=%d,read_mbs(%d,%d) ",img->current_slice_nr,GetMBX(info)+1,GetMBY(info));
#endif
	do {
			// Initializes the current macroblock
 #if FEATURE_INTERLACE
			if(!GetMbAffFrameFlag(img)||!info->currMBisBottom)
#endif// #if FEATURE_INTERLACE
				GetMBX(info)++;
			
			if(!GetMbAffFrameFlag(img))
				Init_MB_Neighbor_Info(img,info,1);
			else
			{
				SetActiveNeighborMBAff(img,info);
			}	
			//posInfo = GetPosInfo(info);
			//mbBits = GetMBBits(currMB);
			/* Define vertical positions */
			posInfo->block_y = (GetMBY(info)<<2);   /* luma block position */
			posInfo->pix_y   = (GetMBY(info)<<4);	/* luma macroblock position */
			posInfo->pix_c_y = (GetMBY(info)<<3);	/* chroma macroblock position */

			/* Define horizontal positions */
			posInfo->block_x = (GetMBX(info)<<2);      /* luma block position */
			posInfo->pix_x   = (GetMBX(info)<<4);	 /* luma pixel position */
			posInfo->pix_c_x = (GetMBX(info)<<3);	/* chroma pixel position */
			
			
			//if (enableReadMB)
			funcInfo->read_one_macroblock(img,info,bs);CHECK_ERR_RET_INT
			trace_overflow(img);
#if FEATURE_INTERLACE
			exitFlag = (!GetMbAffFrameFlag(img)||!CurrMBisTop(info));
#endif//FEATURE_INTERLACE
			end_of_slice=exit_macroblock(img,info,exitFlag);CHECK_ERR_RET_INT
			info->read_mb_num++;//place it before end_of_slice to guarantee the thread safe
			info->end_of_slice = end_of_slice;
#ifdef X86_TEST
			//TAvdLog(LL_INFO,"@!@:Red another(%d,%d),s=%d,curr(%d,%d),s=%d\n",GetMBX(info->anotherProcessor),GetMBY(info->anotherProcessor),info->anotherProcessor->status,GetMBX(info),GetMBY(info),info->status);
#endif//X86_TEST
			number++;
			
			if(GetMBX(info) == endMBX
#if FEATURE_INTERLACE
				&&(!GetMbAffFrameFlag(img)||
					!info->currMBisBottom//this means that the bottom is read just now, and current one points to the next top
				  )
#endif//#if FEATURE_INTERLACE
			  )
				break;

		} while (!info->end_of_slice);
		img->current_mb_nr+=number;
		x_stride = GetMbAffFrameFlag(img)?number>>1:number;
#ifdef USE_JOBPOOL
		if(((GetMBX(info) == endMBX) && (GetMBY(info) == ((img->field_pic_flag ? img->sizeInfo->PicHeightInMbs : img->sizeInfo->FrameHeightInMbs) - GetMbAffFrameFlag(img) -1))))
			img->end_of_frame = 1;
//		if(img->end_of_frame == 1)
//			AvdLog2(LL_ERROR, "x %d, y %d, h %d\n", GetMBX(info), GetMBY(info), img->sizeInfo->FrameHeightInMbs - GetMbAffFrameFlag(img) -1);
#endif		
		GetMBX(info)-=x_stride;//for thread safe,reset the mb_x here before changing status to decoding	
#ifdef X86_TEST
		//TAvdLog(LL_INFO," y=%d,readlast=%d,end_mb_nr=%d,eos=%d\n",GetMBY(info),info->read_mb_num,img->current_mb_nr,info->end_of_slice);
#endif//#ifdef X86_TEST
		
	return number;
}


/*!
************************************************************************
* \brief
*    Initialize decoding engine after decoding an IPCM macroblock
*    (for IPCM CABAC  28/11/2003)
*
* \author
*    Dong Wang <Dong.Wang@bristol.ac.uk>  
************************************************************************
*/


void Get_Neighbor_ABCnRefIdx(ImageParameters *img,TMBsProcessor *info,
							 avdNativeInt x4x4Offset, avdNativeInt y4x4Offset, 
							 avdNativeInt listIdx, avdNativeInt cxStep, MacroBlock *nMBs[],
							 avdNativeInt *x4x4AB, avdNativeInt *y4x4AB, avdNativeInt refIdx[2][3]);
void SetMotionVectorPredictor (ImageParameters *img,TMBsProcessor *info,
							   avdNativeUInt blkIdx, avdNativeInt list, 
							   avdNativeInt ref_frame, AVDMotionVector *pmv);
/*!
************************************************************************
* \brief
*    decode one macroblock
************************************************************************
*/

