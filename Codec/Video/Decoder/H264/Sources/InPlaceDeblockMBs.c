#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#include <string.h>
#endif
#include "global.h"




#include "image.h"
//#include "mb_access.h"
#include "h264VdDump.h"
#include "loopFilter.h"
#include "avd_neighbor.h"
void StoreInPlaceDeblockTopLine(ImageParameters *img,TMBsProcessor *info)
{
	AVDIPDeblocker *dbk = GetIPDeblocker(info);
	StorablePicture	*dec_picture = img->dec_picture;
	avdUInt8	*srcY, *srcU, *srcV;
	avdNativeInt i, offset;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	H264VdLibParam *params = img->vdLibPar;
#if FEATURE_INTERLACE
	int isTop = CurrMBisTop(info);
#endif//#if FEATURE_INTERLACE
	// save neighbor D before overwritten;
	if(img->deblockType!=DEBLOCK_BY_MB)
	{
		AVDIPDeblocker *dbk2 = GetIPDeblocker(info->anotherProcessor);
		dbk2->yuvD[0] = dbk2->topY[posInfo->pix_x+15]; //Y
		dbk2->yuvD[1] = dbk2->topUV[0][posInfo->pix_c_x+7]; //U
		dbk2->yuvD[2] = dbk2->topUV[1][posInfo->pix_c_x+7]; //V
	}


	
	if (img->deblockType==DEBLOCK_BY_MB&&GetMBX(info)!=sizeInfo->PicWidthInMbs-1)
	{
		avdUInt8	*leftY, *leftU, *leftV;
		int num	=	16;
		int YStride  = sizeInfo->yPlnPitch;
		int UVStride = YStride/2;
		int needSetLeft = 0;
		if(!GetMbAffFrameFlag(img))
		{
			if(params->rowNumOfMBs==1||IS_INTRA(info->currMB+1))
			{
				leftY  = GetLeftMBY(dbk);
				leftU  = GetLeftMBUV(dbk,0);
				leftV  = GetLeftMBUV(dbk,1);
				//needSetLeft = 1;
				srcY   = dec_picture->plnY + posInfo->pix_y* YStride + posInfo->pix_x+15;
				srcU   = dec_picture->plnU + posInfo->pix_c_y* UVStride + posInfo->pix_c_x + 7;
				srcV   = dec_picture->plnV + posInfo->pix_c_y* UVStride + posInfo->pix_c_x + 7;

				do{
					*leftY = *srcY;leftY++;srcY+=YStride;
				}while(--num);

				num = 8;

				do{
					*leftU = *srcU;leftU++;srcU+=UVStride;
					*leftV = *srcV;leftV++;srcV+=UVStride;
				}while(--num);
			}
		}
#if FEATURE_INTERLACE
		else
		{
			
			srcY   = dec_picture->plnY + posInfo->pix_y* YStride + posInfo->pix_x+15;
			srcU   = dec_picture->plnU + posInfo->pix_c_y* UVStride + posInfo->pix_c_x + 7;
			srcV   = dec_picture->plnV + posInfo->pix_c_y* UVStride + posInfo->pix_c_x + 7;
			
			if (isTop)
			{
				leftY  = GetMBAffLeftYTopBackup(dbk,isTop);
				leftU  = GetMBAffLeftUVTopBackup(dbk,isTop,0);
				leftV  = GetMBAffLeftUVTopBackup(dbk,isTop,1);
				do{
					*leftY = *srcY;leftY++;srcY+=YStride;
				}while(--num);

				num = 8;

				do{
					*leftU = *srcU;leftU++;srcU+=UVStride;
					*leftV = *srcV;leftV++;srcV+=UVStride;
				}while(--num);
			} 
			else
			{
				avdUInt8	*leftYTop  = GetMBAffLeftY(dbk,1);
				avdUInt8	*leftUTop  = GetMBAffLeftUV(dbk,1,0);
				avdUInt8	*leftVTop  = GetMBAffLeftUV(dbk,1,1);
				avdUInt8	*leftYTopB  = GetMBAffLeftYTopBackup(dbk,1);
				avdUInt8	*leftUTopB  = GetMBAffLeftUVTopBackup(dbk,1,0);
				avdUInt8	*leftVTopB  = GetMBAffLeftUVTopBackup(dbk,1,1);
				avdUInt8	*leftYBottom  = GetMBAffLeftY(dbk,0);
				avdUInt8	*leftUBottom  = GetMBAffLeftUV(dbk,0,0);
				avdUInt8	*leftVBottom  = GetMBAffLeftUV(dbk,0,1);
				do{
					*leftYBottom++ = *srcY;srcY+=YStride;
					*leftYTop++	   = *leftYTopB++;
					
				}while(--num);

				num = 8;

				do{
					*leftUBottom++ = *srcU;srcU+=UVStride;
					*leftUTop++	   = *leftUTopB++;
					*leftVBottom++ = *srcV;srcV+=UVStride;
					*leftVTop++	   = *leftVTopB++;
				}while(--num);
			}
			
		}
#endif//#if FEATURE_INTERLACE		
	
		
	}

	if (GetMBY(info) != sizeInfo->PicHeightInMbs - 1){

		avdUInt32	*tmp32Pt1, *tmp32Pt2;
		int			row = 0;
		dec_picture = img->dec_picture;
		
		do 
		{
			tmp32Pt2 = (avdUInt32 *)(dec_picture->plnY + (posInfo->pix_y + MB_BLOCK_SIZE - 1-(row)) 
				* sizeInfo->yPlnPitch + posInfo->pix_x);
#if FEATURE_INTERLACE
			if(GetMbAffFrameFlag(img))
			{
				tmp32Pt1 = (avdUInt32 *)(GetMBAffTopY(dbk,isTop,!row)+posInfo->pix_x);
			}
			else
#endif//#if FEATURE_INTERLACE
				tmp32Pt1 = (avdUInt32 *)&dbk->topY[posInfo->pix_x];
			
			*tmp32Pt1       = *tmp32Pt2;
			*(tmp32Pt1 + 1) = *(tmp32Pt2 + 1);
			*(tmp32Pt1 + 2) = *(tmp32Pt2 + 2);
			*(tmp32Pt1 + 3) = *(tmp32Pt2 + 3);



			i = ((posInfo->pix_c_y + (MB_BLOCK_SIZE>>1) - 1-row))* (sizeInfo->yPlnPitch>>1) 
				+ posInfo->pix_c_x;
			tmp32Pt2 = (avdUInt32 *)(dec_picture->plnU + i);
#if FEATURE_INTERLACE
			if(GetMbAffFrameFlag(img))
			{
				tmp32Pt1 = (avdUInt32 *)(GetMBAffTopUV(dbk,isTop,!row,0)+posInfo->pix_c_x);
			}
			else
#endif//#if FEATURE_INTERLACE
				tmp32Pt1 = (avdUInt32 *)&dbk->topUV[0][posInfo->pix_c_x];
			//tmp32Pt1 = !GetMbAffFrameFlag(img)?(avdUInt32 *)&dbk->topUV[0][posInfo->pix_c_x]:(avdUInt32 *)(GetMBAffTopUV(dbk,isTop,!row,0)+posInfo->pix_c_x);
			*tmp32Pt1       = *tmp32Pt2;
			*(tmp32Pt1 + 1) = *(tmp32Pt2 + 1);



			tmp32Pt2 = (avdUInt32 *)(dec_picture->plnV + i);
#if FEATURE_INTERLACE
			if(GetMbAffFrameFlag(img))
			{
				tmp32Pt1 = (avdUInt32 *)(GetMBAffTopUV(dbk,isTop,!row,1)+posInfo->pix_c_x);
			}
			else
#endif//#if FEATURE_INTERLACE
				tmp32Pt1 = (avdUInt32 *)&dbk->topUV[1][posInfo->pix_c_x];
			//tmp32Pt1 = !GetMbAffFrameFlag(img)?(avdUInt32 *)&dbk->topUV[1][posInfo->pix_c_x]:(avdUInt32 *)(GetMBAffTopUV(dbk,isTop,!row,1)+posInfo->pix_c_x);
			*tmp32Pt1       = *tmp32Pt2;
			*(tmp32Pt1 + 1) = *(tmp32Pt2 + 1);
		} while (GetMbAffFrameFlag(img)&&row++<1);//MBAFF needs to store two row,the lastRow first, then the last but one
		

	}

}


extern const avdUInt8 QP_SCALE_CR[52] ;

/*********************************************************************************************************/

#define  IClip( Min, Max, Val) (((Val)<(Min))? (Min):(((Val)>(Max))? (Max):(Val)))

// NOTE: to change the tables below for instance when the QP doubling is changed from 6 to 8 values 
//       send an e-mail to Peter.List@t-systems.com to get a little programm that calculates them automatically 

const avdUInt8 ALPHA_TABLE[52]  = {0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,4,4,5,6,  7,8,9,10,12,13,15,17,  20,22,25,28,32,36,40,45,  50,56,63,71,80,90,101,113,  127,144,162,182,203,226,255,255} ;
const avdUInt8  BETA_TABLE[52]  = {0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,2,2,2,3,  3,3,3, 4, 4, 4, 6, 6,   7, 7, 8, 8, 9, 9,10,10,  11,11,12,12,13,13, 14, 14,   15, 15, 16, 16, 17, 17, 18, 18} ;


const avdUInt8 CLIP_TAB[52][4]  =
{
	{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},
// Line 1		
	{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},{ 0, 0, 0, 0},
// Line 2
	{ 0, 0, 0, 0},{ 0, 0, 1, 1},{ 0, 0, 1, 1},{ 0, 0, 1, 1},{ 0, 0, 1, 1},{ 0, 1, 1, 1},{ 0, 1, 1, 1},{ 1, 1, 1, 1},
// Line 3
	{ 1, 1, 1, 1},{ 1, 1, 1, 1},{ 1, 1, 1, 1},{ 1, 1, 2, 2},{ 1, 1, 2, 2},{ 1, 1, 2, 2},{ 1, 1, 2, 2},{ 1, 2, 3, 3},
	{ 1, 2, 3, 3},{ 2, 2, 3, 3},{ 2, 2, 4, 4},{ 2, 3, 4, 4},{ 2, 3, 4, 4},{ 3, 3, 5, 5},{ 3, 4, 6, 6},{ 3, 4, 6, 6},
	{ 4, 5, 7, 7},{ 4, 5, 8, 8},{ 4, 6, 9, 9},{ 5, 7,10,10},{ 6, 8,11,11},{ 6, 8,13,13},{ 7,10,14,14},{ 8,11,16,16},
	{ 9,12,18,18},{ 10,13,20,20},{11,15,23,23},{ 13,17,25,25}
} ;



void SetInPlaceIntraMBStrength(ImageParameters *img,TMBsProcessor *info,avdInt8 * strength)
{
	// for I frame deblocking strength will be all the same;
	avdUInt32 *str32 = (avdUInt32 *)strength;
	*str32 = 0x04040404;
#if FEATURE_INTERLACE
	
	if((GetMbAffFrameFlag(img)&&(!niIsFrameMb2(info, GetMBX(info))||mbIsMbField(info->currMB)))
		||img->dec_picture->structure!=FRAME)
	{
		*(str32 + 4) = 0x03030303;
	}
	else
#endif//FEATURE_INTERLACE
		*(str32 + 4) = 0x04040404;
	*(str32 + 1) = *(str32 + 2) = *(str32 + 3) = 
		*(str32 + 5) = *(str32 + 6) = *(str32 + 7) = (img->vdLibPar->deblockFlag&EDF_ONLYMBEDGE)? 0: 0x03030303;

}
#if DUMP_VERSION
static int LumafilterIdx=0;
static int ChromafilterIdx=0;
#endif
//static int __inline iabs(int a)
//{
//	return a<0?-a:a;
//}
#define iABS(a) if((a)<0) (a) = -(a);
static int __inline CheckBSliceRefAndMV(ImageParameters* img,MacroBlock* mb,int ref_p0,int ref_p1,int ref_q0,int ref_q1,
										AVDMotionVector* mv_p0,AVDMotionVector* mv_p1,AVDMotionVector* mv_q0,AVDMotionVector* mv_q1,int mvlimit)
{
	int tmp1,tmp2,tmp3,tmp4,list_offset = 0;
	avdUInt32* refPicNum0 = img->dec_picture->ref_pic_num[LIST_0+ list_offset];
	avdUInt32* refPicNum1 = img->dec_picture->ref_pic_num[LIST_1+ list_offset];
	int StrValue = 0;
#define INVALID_REF_NUM 0x12344567					
	ref_p0 = ref_p0==AVD_INVALID_REF_IDX?INVALID_REF_NUM:refPicNum0[ref_p0];
	ref_q0 = ref_q0==AVD_INVALID_REF_IDX?INVALID_REF_NUM:refPicNum0[ref_q0];
	ref_p1 = ref_p1==AVD_INVALID_REF_IDX?INVALID_REF_NUM:refPicNum1[ref_p1];
	ref_q1 = ref_q1==AVD_INVALID_REF_IDX?INVALID_REF_NUM:refPicNum1[ref_q1];
	


	if ( ((ref_p0==ref_q0) & (ref_p1==ref_q1)) || ((ref_p0==ref_q1) & (ref_p1==ref_q0)))
	{
		// L0 and L1 reference pictures of p0 are different; q0 as well
		if (ref_p0 != ref_p1)
		{
			// compare MV for the same reference picture
			if (ref_p0 == ref_q0)
			{

				//if (ref_p0 == INVALID_REF_NUM)
				//{
				//	/*StrValue =  (
				//	(iabs( list1_mv[blk_y][blk_x][0] - list1_mv[blk_y2][blk_x2][0]) >= 4) |
				//	(iabs( list1_mv[blk_y][blk_x][1] - list1_mv[blk_y2][blk_x2][1]) >= mvlimit));*/
				//	tmp1 = mv_p1->x - mv_q1->x;iABS(tmp1);
				//	tmp2 = mv_p1->y - mv_q1->y;iABS(tmp2);
				//	StrValue =  (tmp1>=4||tmp2>=mvlimit);
				//		
				//}
				//else if (ref_p1 == INVALID_REF_NUM)
				//{
				//	tmp1 = mv_p0->x - mv_q0->x;iABS(tmp1);
				//	tmp2 = mv_p0->y - mv_q0->y;iABS(tmp2);
				//	StrValue =  (tmp1>=4||tmp2>=mvlimit);
				//}
				//else
				{
					tmp1 = mv_p0->x - mv_q0->x;iABS(tmp1);
					tmp2 = mv_p0->y - mv_q0->y;iABS(tmp2);
					tmp3 = mv_p1->x - mv_q1->x;iABS(tmp3);
					tmp4 = mv_p1->y - mv_q1->y;iABS(tmp4);
					StrValue =  (tmp1>=4||tmp2>=mvlimit||tmp3>=4||tmp4>=mvlimit);						
				}
			}
			else
			{
				tmp1 = mv_p0->x - mv_q1->x;iABS(tmp1);
				tmp2 = mv_p0->y - mv_q1->y;iABS(tmp2);
				tmp3 = mv_p1->x - mv_q0->x;iABS(tmp3);
				tmp4 = mv_p1->y - mv_q0->y;iABS(tmp4);
				StrValue =  (tmp1>=4||tmp2>=mvlimit||tmp3>=4||tmp4>=mvlimit);		
			}
		}
		else
		{
			tmp1 = mv_p0->x - mv_q1->x;iABS(tmp1);
			tmp2 = mv_p0->y - mv_q1->y;iABS(tmp2);
			tmp3 = mv_p1->x - mv_q0->x;iABS(tmp3);
			tmp4 = mv_p1->y - mv_q0->y;iABS(tmp4);
			StrValue = (tmp1>=4||tmp2>=mvlimit||tmp3>=4||tmp4>=mvlimit);
			if(StrValue)
			{
				tmp1 = mv_p0->x - mv_q0->x;iABS(tmp1);
				tmp2 = mv_p0->y - mv_q0->y;iABS(tmp2);
				tmp3 = mv_p1->x - mv_q1->x;iABS(tmp3);
				tmp4 = mv_p1->y - mv_q1->y;iABS(tmp4);
				StrValue =  (tmp1>=4||tmp2>=mvlimit||tmp3>=4||tmp4>=mvlimit);	
			}
				
		}
	}
	else
	{
		StrValue = 1;
	}

	return StrValue;
}
#define CHECK_P_REF(refp,refq){\
	ref_p0 = (refp);\
	ref_q0 = (refq);\
	if(ref_p0!=AVD_INVALID_REF_IDX&&list0[ref_p0])\
		ref_p0 = list0[ref_p0]->poc;\
	if(ref_q0!=AVD_INVALID_REF_IDX&&list0[ref_q0])\
		ref_q0 = list0[ref_q0]->poc;}
void SetInPlaceInterMBStrength(ImageParameters *img,TMBsProcessor *info,avdInt8 * strength,Macroblock *mb)
{
	Macroblock *neighborMB;
	avdInt8 *tmp8Pt;
	avdNativeUInt cbpBlk;
	avdNativeInt j, i, k, x4x4, x8x8, mbtype,picA,picB;
	AVDStoredPicMotionInfo *mInfo;
	AVDMotionVector **mv0, *mv0A[4],*mv_p0,*mv_q0,*mv_p1,*mv_q1;
	avdUInt8		**ref0, *ref0A, *ref0B;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	struct storable_picture			**list0 = img->listX[LIST_0];
#if FEATURE_BFRAME
	AVDMotionVector **mv1=NULL, *mv1A[4];
	avdUInt8		**ref1=NULL, *ref1A, *ref1B;
	int bframe;
#else//FEATURE_BFRAME
	const int bframe = 0;
#endif //FEATURE_BFRAME
	//TMBsProcessor *niInfo;
	avdUInt32 *tmp32Pt;
	int j8,j4,up8,up4;
	int    ref_p0,ref_p1,ref_q0,ref_q1,StrValue;
	
#if FEATURE_INTERLACE
	int fieldModeMbFlag = (img->dec_picture->structure!=FRAME) || (GetMbAffFrameFlag(img) && mbIsMbField(mb));
	int mvlimit = !fieldModeMbFlag?4:2;
#else//FEATURE_INTERLACE
	const int mvlimit = 4;
#endif//FEATURE_INTERLACE
	if (IS_INTRA(mb)){
		SetInPlaceIntraMBStrength(img,info,strength);
		return;
	}

	// init to 0;
	//strength = &img->ipDeblocker->strength[0];
#ifdef XSCALE //when using Intel XSCALE compiler,the  tmp32Pt will cause alias issue
	AVD_ALIGNED32_MEMSET(strength, 0, 32 / sizeof(avdInt32));
#else//XSCALE
	tmp32Pt = (avdUInt32 *)(strength); 
	tmp32Pt[0] = tmp32Pt[1] = tmp32Pt[2] = tmp32Pt[3] = 
		tmp32Pt[4] = tmp32Pt[5] = tmp32Pt[6] = tmp32Pt[7] =  0;

#endif//XSCALE
	// check internal cbp_blk first;
	if ((cbpBlk = mbGetCBPBlk(mb)) != 0){
		i = 15;
		do {
			if (cbpBlk&(1<<i)){
				k = (i>>2) | ((i&3)<<2);
				strength[k] = strength[i+16] = 2;
				if (k < 12)
					strength[k+4] = 2;
				if (i < 12)
					strength[i+20] = 2;
			}
		} while (--i >= 0);
	}

	// check outside boundary to overwrite cpb_blk at boundary;
	if(img->deblockType==DEBLOCK_BY_MB)
		x4x4 = posInfo->block_x;
	else
		x4x4 = ((sizeInfo->PicWidthInMbs - (info->currMB - mb) - 1)<<2);

	x8x8 = (x4x4>>1);
	mInfo = img->dec_picture->motionInfo;
	if(img->notSaveAllMV)
	{
		j8 = j4 = 0;
		up8 = up4 = -1;
	}
	else //SAVE_ALL_MV
	{
		j4 = posInfo->block_y;
		//up4 = j4 - 1;
		j8 = (j4>>1);
		//up8 = up8
	}
	mv0  = &mInfo->mv0[j4];
	ref0 = &mInfo->ref_idx0[j8];
#if FEATURE_BFRAME
	bframe = (img->type == B_SLICE);
	if (bframe){
		mv1  = &mInfo->mv1[j4];
		ref1 = &mInfo->ref_idx1[j8];
	}
#endif //FEATURE_BFRAME
	neighborMB = mb - 1;
	
	// vertical 16x16 boundary;
	if (!x4x4 || IS_INTRA(neighborMB))
		// check intra boundary;
		*(avdUInt32 *)strength = 0x04040404;
	else{
		// check neighbor cbp_blk; internal cbp_blk checking is done in readCBPandCoeffsFromNAL();
		cbpBlk = mbGetCBPBlk(neighborMB);
		i = 3;
		if(GetMbAffFrameFlag(img)&&(mbIsMbField(mb)!=mbIsMbField(mb-1)))//          if (p_Vid->mixedModeEdgeFlag)
		{
			*(avdUInt32 *)strength = 0x01010101;
		}
		else
		{
			do {
				if (strength[i])
					continue;

				if (cbpBlk&(0x8<<(i<<2))) // check 3, 7, 11, 15 block;
					strength[i] = 2; // mark 0, 1, 2, 3 strength;
				else if(!bframe) 
				{	
					CHECK_P_REF(ref0[(i>>1)][x8x8],ref0[(i>>1)][x8x8-1])

					if ((ref_p0 != ref_q0)
						||
						((k = mv0[i][x4x4].x - mv0[i][x4x4-1].x) >= 4 ||
						-k >= 4) ||
						((k = mv0[i][x4x4].y - mv0[i][x4x4-1].y) >= mvlimit ||
						-k >= mvlimit))
						strength[i] = 1;
				}
#if FEATURE_BFRAME
				else{
					// bframe; TBD: need to cross check if ref0A = ref1B && ref1A = ref0B;

					ref_p0 = ref0[(i>>1)][x8x8];
					ref_q0 = ref0[(i>>1)][x8x8-1];
					mv_p0   = &mv0[i][x4x4];
					mv_q0   = &mv0[i][x4x4-1];

					ref_p1 = ref1[(i>>1)][x8x8];
					ref_q1 = ref1[(i>>1)][x8x8-1];
					mv_p1	= &mv1[i][x4x4];
					mv_q1	= &mv1[i][x4x4-1];
					strength[i] = CheckBSliceRefAndMV(img,mb,ref_p0,ref_p1,ref_q0,ref_q1,
						mv_p0,mv_p1,mv_q0,mv_q1,mvlimit);
				}
#endif //FEATURE_BFRAME
			} while (--i >= 0);
		}
		
	}
	// horizontal 16x16 boundary;
	//niInfo = img->mbsProcessor;
	tmp8Pt = strength + 16;
	if (!posInfo->block_y || niIsIntraMb2(info, x4x4>>2))
	{
#if FEATURE_INTERLACE
		if((GetMbAffFrameFlag(img)&&(!niIsFrameMb2(info, (x4x4>>2))||mbIsMbField(mb)))
			||img->dec_picture->structure!=FRAME)
		{
			*(avdUInt32 *)tmp8Pt = 0x03030303;
		}
		else
#endif//FEATURE_INTERLACE
		*(avdUInt32 *)tmp8Pt = 0x04040404;
	}
	else{

		if(GetMbAffFrameFlag(img)&&(mbIsMbField(mb)!=(!niIsFrameMb2(info, (x4x4>>2)))))//          if (p_Vid->mixedModeEdgeFlag)
		{
			*(avdUInt32 *)tmp8Pt = 0x01010101;
		}
		else
		{
			i = 3;
			do {
				if (tmp8Pt[i])
					continue;

				if (niIsTopCbpBlkNonZero2(info, (x4x4>>2), i))// check 12, 13, 14, 15 block;
					tmp8Pt[i] = 2; // mark 16, 17, 18, 19 strength;
				else if(!bframe) 
				{	
					CHECK_P_REF(ref0[0][x8x8+(i>>1)],ref0[-1][x8x8+(i>>1)]);
					if ((ref_p0 != ref_q0) ||
						((k = mv0[0][x4x4+i].x - mv0[-1][x4x4+i].x) >= 4 ||
						-k >= 4) ||
						((k = mv0[0][x4x4+i].y - mv0[-1][x4x4+i].y) >= mvlimit ||
						-k >= mvlimit ))
						tmp8Pt[i] = 1;
				}
#if FEATURE_BFRAME
				else {
					// bframe; TBD: need to cross check if ref0A = ref1B && ref1A = ref0B;
					/*	if ((ref0[0][x8x8+(i>>1)] != ref0[-1][x8x8+(i>>1)] ||
					(ref0[0][x8x8+(i>>1)] != AVD_INVALID_REF_IDX &&
					((k = mv0[0][x4x4+i].x - mv0[-1][x4x4+i].x) >= 4 || -k >= 4) ||
					((k = mv0[0][x4x4+i].y - mv0[-1][x4x4+i].y) >= mvlimit || -k >= mvlimit))) ||
					(bframe && (ref1[0][x8x8+(i>>1)] != ref1[-1][x8x8+(i>>1)] ||
					(ref1[0][x8x8+(i>>1)] != AVD_INVALID_REF_IDX &&
					((k = mv1[0][x4x4+i].x - mv1[-1][x4x4+i].x) >= 4 || -k >= 4) ||
					((k = mv1[0][x4x4+i].y - mv1[-1][x4x4+i].y) >= mvlimit || -k >= mvlimit)))))
					tmp8Pt[i] = 1;*/

					ref_p0 = ref0[0][x8x8+(i>>1)];
					ref_q0 = ref0[-1][x8x8+(i>>1)];
					mv_p0   = &mv0[0][x4x4+i];
					mv_q0   = &mv0[-1][x4x4+i];

					ref_p1 = ref1[0][x8x8+(i>>1)];
					ref_q1 = ref1[-1][x8x8+(i>>1)];
					mv_p1	= &mv1[0][x4x4+i];
					mv_q1	= &mv1[-1][x4x4+i];
					tmp8Pt[i] = CheckBSliceRefAndMV(img,mb,ref_p0,ref_p1,ref_q0,ref_q1,
						mv_p0,mv_p1,mv_q0,mv_q1,mvlimit);
				}
#endif //FEATURE_BFRAME				

			} while (--i >= 0);
		}
		
	}
	
	if(img->vdLibPar->deblockFlag&EDF_ONLYMBEDGE)
		return;

	// overwrite it for next row;
	//niSetTopCbpBlkFromBottomOfFullCbpBlk(info, mbGetCBPBlk(mb), (x4x4>>2));
#if FEATURE_BFRAME
	if (((mbtype = mbGetMbType(mb)) ==1)
		|| (!bframe && mbtype == 0))
		// bframe skip, mv may different 4x4;
#else //FEATURE_BFRAME
	if ((mbtype = mbGetMbType(mb)) < 2)
		// if P skip or 16x16 done;
#endif //FEATURE_BFRAME
		return;

	mv0A[0] = &mv0[0][x4x4];
	ref0A = &ref0[0][x8x8];
	ref0B = &ref0[1][x8x8];
#if FEATURE_BFRAME
	if (bframe){
		mv1A[0] = &mv1[0][x4x4];
		ref1A = &ref1[0][x8x8];
		ref1B = &ref1[1][x8x8];
	}
#endif //FEATURE_BFRAME
	switch(mbtype){
	case 2:
		// 16x8
		if(!bframe)
		{

			CHECK_P_REF(*ref0A,*ref0B);
			if (ref_p0 != ref_q0 ||
				((k = mv0A[0]->x - mv0[2][x4x4].x) >= 4 ||
				-k >= 4) ||
				((k = mv0A[0]->y - mv0[2][x4x4].y) >= mvlimit ||
				-k >= mvlimit)){
					// if strength[24-27] != 2, strength[24-27] = 1;
					*(avdUInt32 *)(strength + 24) 
						= 0x01010101 + ((*(avdUInt32 *)(strength + 24))>>1);
			}
		}
	
#if FEATURE_BFRAME
		else
		{
			// bframe; TBD: need to cross check if ref0A = ref1B && ref1A = ref0B;
		/*	if ((*ref0A != *ref0B || 
				(*ref0A != AVD_INVALID_REF_IDX &&
				((k = mv0A[0]->x - mv0[2][x4x4].x) >= 4 || -k >= 4) ||
				((k = mv0A[0]->y - mv0[2][x4x4].y) >= mvlimit || -k >= mvlimit))) ||
				(bframe && (*ref1A != *ref1B ||  
				(*ref1A != AVD_INVALID_REF_IDX &&
				((k = mv1A[0]->x - mv1[2][x4x4].x) >= 4 || -k >= 4) ||
				((k = mv1A[0]->y - mv1[2][x4x4].y) >= mvlimit || -k >= mvlimit))))){
					// if strength[24-27] != 2, strength[24-27] = 1;
					*(avdUInt32 *)(strength + 24) 
						= 0x01010101 + ((*(avdUInt32 *)(strength + 24))>>1);*/

			StrValue = CheckBSliceRefAndMV(img,mb,*ref0A,*ref1A,*ref0B,*ref1B,
				mv0A[0],mv1A[0],&mv0[2][x4x4],&mv1[2][x4x4],mvlimit);
			if(StrValue)
				*(avdUInt32 *)(strength + 24) 
					= 0x01010101 + ((*(avdUInt32 *)(strength + 24))>>1);
		}
#endif //FEATURE_BFRAME
		break;
	case 3:
		// 8x16

		if(!bframe)
		{
			CHECK_P_REF(*ref0A,*(ref0A + 1));
			if (ref_q0 != ref_p0 ||
				((k = mv0A[0]->x - (mv0A[0] + 2)->x) >= 4 ||
				-k >= 4) ||
				((k = mv0A[0]->y - (mv0A[0] + 2)->y) >= mvlimit ||
				-k >= mvlimit)){
					// if strength[8-11] != 2, strength[8-11] = 1;
					*(avdUInt32 *)(strength + 8) 
						= 0x01010101 + ((*(avdUInt32 *)(strength + 8))>>1);
			}
		}
#if FEATURE_BFRAME
		else
		{
			//if ((*ref0A != *(ref0A + 1) ||
			//	(*ref0A != AVD_INVALID_REF_IDX &&
			//	((k = mv0A[0]->x - (mv0A[0] + 2)->x) >= 4 || -k >= 4) ||
			//	((k = mv0A[0]->y - (mv0A[0] + 2)->y) >= mvlimit || -k >= mvlimit))) ||
			//	(bframe && (*ref1A != *(ref1A + 1) ||
			//	(*ref1A != AVD_INVALID_REF_IDX &&
			//	((k = mv1A[0]->x - (mv1A[0] + 2)->x) >= 4 || -k >= 4) ||
			//	((k = mv1A[0]->y - (mv1A[0] + 2)->y) >= mvlimit || -k >= mvlimit))))){
			//		// if strength[8-11] != 2, strength[8-11] = 1;
			//		*(avdUInt32 *)(strength + 8) 
			//			= 0x01010101 + ((*(avdUInt32 *)(strength + 8))>>1);
			//}
			StrValue = CheckBSliceRefAndMV(img,mb,*ref0A,*ref1A,*(ref0A + 1),*(ref1A + 1),
				mv0A[0],mv1A[0],(mv0A[0] + 2),(mv1A[0] + 2),mvlimit);
			if(StrValue)
				*(avdUInt32 *)(strength + 8) 
						= 0x01010101 + ((*(avdUInt32 *)(strength + 8))>>1);
		}

		
#endif //FEATURE_BFRAME

		break;
	default:
		// vertical internal edges;
		mv0A[1] = &mv0[1][x4x4];
		mv0A[2] = &mv0[2][x4x4];
		mv0A[3] = &mv0[3][x4x4];
		tmp8Pt = strength + 12;
		i = 3;
		do {
			j = 3;
			do {
				AVDMotionVector *mvAij;
				if (tmp8Pt[j]){
					continue;
				}

				if(!bframe)
				{
				
					if (i == 2)
					{
						CHECK_P_REF(ref0[(j>>1)][x8x8],ref0[(j>>1)][x8x8+1]);
						if(ref_p0!=ref_q0)
						{
							tmp8Pt[j] = 1;
							continue;
						}
					
					}

					mvAij = mv0A[j]+i;
					if (((k = mvAij->x - (mvAij-1)->x) >= 4 ||
						-k >= 4) ||
						((k = mvAij->y - (mvAij-1)->y) >= mvlimit ||
						-k >= mvlimit))
						tmp8Pt[j] = 1;
				}
#if FEATURE_BFRAME
				else
				{
#if 0
					if (((i == 2 && ref0[(j>>1)][x8x8] != ref0[(j>>1)][x8x8+1]) ||
						(ref0[(j>>1)][x8x8] != AVD_INVALID_REF_IDX &&
						((k = mv0[j][x4x4+i].x - mv0[j][x4x4+i-1].x) >= 4 || -k >= 4) ||
						((k = mv0[j][x4x4+i].y - mv0[j][x4x4+i-1].y) >= mvlimit || -k >= mvlimit))) ||
						(bframe && ((i == 2 && ref1[(j>>1)][x8x8] != ref1[(j>>1)][x8x8+1]) ||
						(ref1[(j>>1)][x8x8] != AVD_INVALID_REF_IDX &&
						((k = mv1[j][x4x4+i].x - mv1[j][x4x4+i-1].x) >= 4 || -k >= 4) ||
						((k = mv1[j][x4x4+i].y - mv1[j][x4x4+i-1].y) >= mvlimit || -k >= mvlimit))))) 
									tmp8Pt[j] = 1;
#else
			
					{
						int y = (j>>1);
						int xp = (x4x4+i)>>1;
						int xq = (x4x4+i-1)>>1;
						ref_p0 = ref0[y][xp];
						ref_q0 = ref0[y][xq];
						ref_p1 = ref1[y][xp];
						ref_q1 = ref1[y][xq];
						tmp8Pt[j] = CheckBSliceRefAndMV(img,mb,ref_p0,ref_p1,ref_q0,ref_q1,
							&mv0[j][x4x4+i],&mv1[j][x4x4+i],&mv0[j][x4x4+i-1],&mv1[j][x4x4+i-1],mvlimit);
					}
#endif				
					
					
				}

				
#endif //FEATURE_BFRAME
			} while (--j >= 0);
			tmp8Pt -= 4;
		} while (--i); 

		// horizontal internal edges;
		tmp8Pt = strength + 28;
		j = 3;
		do {
			i = 3;
			do {
				AVDMotionVector *mvAij1, *mvAij2;
				if (tmp8Pt[i]){
					continue;
				}

				if(!bframe)
				{
					if (j == 2)
					{
						CHECK_P_REF(*(ref0A + (i>>1)),*(ref0B + (i>>1)));
						if(ref_p0!=ref_q0)
						{
							tmp8Pt[i] = 1;
							continue;
						}

					}
					mvAij1 = mv0A[j]+i;
					mvAij2 = mv0A[j-1]+i;
					if (((k = mvAij1->x - mvAij2->x) >= 4 ||
						-k >= 4) ||
						((k = mvAij1->y - mvAij2->y) >= mvlimit ||
						-k >= mvlimit))
						tmp8Pt[i] = 1;
				}
#if FEATURE_BFRAME
				else
				{
#if 0				
					if (((j == 2 && *(ref0A + (i>>1)) != *(ref0B + (i>>1))) ||
						(*(ref0A + (i>>1)) != AVD_INVALID_REF_IDX &&
						((k = mv0[j][x4x4+i].x - mv0[j-1][x4x4+i].x) >= 4 || -k >= 4) ||
						((k = mv0[j][x4x4+i].y - mv0[j-1][x4x4+i].y) >= mvlimit || -k >= mvlimit))) ||
						(bframe && ((j == 2 && *(ref1A + (i>>1)) != *(ref1B + (i>>1))) ||
						(*(ref1A + (i>>1)) != AVD_INVALID_REF_IDX &&
						((k = mv1[j][x4x4+i].x - mv1[j-1][x4x4+i].x) >= 4 || -k >= 4) ||
						((k = mv1[j][x4x4+i].y - mv1[j-1][x4x4+i].y) >= mvlimit || -k >= mvlimit))))) 
						tmp8Pt[i] = 1;
#else

					int yp = (j>>1);
					int yq = ((j-1)>>1);
					int x = (x4x4+i)>>1;
					ref_p0 = ref0[yp][x];
					ref_q0 = ref0[yq][x];
					ref_p1 = ref1[yp][x];
					ref_q1 = ref1[yq][x];
					tmp8Pt[i] = CheckBSliceRefAndMV(img,mb,ref_p0,ref_p1,ref_q0,ref_q1,
						&mv0[j][x4x4+i],&mv1[j][x4x4+i],&mv0[j-1][x4x4+i],&mv1[j-1][x4x4+i],mvlimit);
#endif
				}

			
#endif //FEATURE_BFRAME
			} while (--i >= 0);
			tmp8Pt -= 4;
		} while (--j);
		break;
	}
}
#if FEATURE_INTERLACE
void Copy2Bottom(ImageParameters *img,TMBsProcessor* info)
{
	StorablePicture	*dec_picture = img->dec_picture;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	int YStride  = sizeInfo->yPlnPitch;
	int y,UVStride = YStride/2;
	avdUInt8* srcY   = dec_picture->plnY + (posInfo->pix_y-MB_BLOCK_SIZE)* YStride + posInfo->pix_x;
	avdUInt8* srcU   = dec_picture->plnU + (posInfo->pix_c_y-BLOCK_SIZE_8x8)* UVStride + posInfo->pix_c_x;
	avdUInt8* srcV   = dec_picture->plnV + (posInfo->pix_c_y-BLOCK_SIZE_8x8)* UVStride + posInfo->pix_c_x;
	avdUInt8* dstY   = dec_picture->plnY + (posInfo->pix_y)* YStride + posInfo->pix_x;
	avdUInt8* dstU   = dec_picture->plnU + (posInfo->pix_c_y)* UVStride + posInfo->pix_c_x;
	avdUInt8* dstV   = dec_picture->plnV + (posInfo->pix_c_y)* UVStride + posInfo->pix_c_x;
	for (y = 0; y < BLOCK_SIZE_8x8; y++){
		AVD_ALIGNED32_MEMCPY4(dstY,srcY);srcY+=YStride;dstY+=YStride;
		AVD_ALIGNED32_MEMCPY4(dstY,srcY);srcY+=YStride;dstY+=YStride;
		AVD_ALIGNED32_MEMCPY2(dstU,srcU);srcU+=UVStride;dstU+=UVStride;
		AVD_ALIGNED32_MEMCPY2(dstV,srcV);srcV+=UVStride;dstV+=UVStride;
	}
}
static void MBAFFRevert2Frame(ImageParameters *img,TMBsProcessor* info)
{

	MacroBlock* currMB = GetCurrMB(info,GetMBX(info));
	if(mbIsMbField(currMB)&&!CurrMBisTop(info))
	{
		StorablePicture	*dec_picture = img->dec_picture;
		TSizeInfo	*sizeInfo = img->sizeInfo;
		TPosInfo*  posInfo  = GetPosInfo(info);
		int YStride  = sizeInfo->yPlnPitch;
		int UVStride = YStride/2;
		avdUInt8* srcY   = dec_picture->plnY + (posInfo->pix_y-MB_BLOCK_SIZE)* YStride + posInfo->pix_x;
		avdUInt8* srcU   = dec_picture->plnU + (posInfo->pix_c_y-BLOCK_SIZE_8x8)* UVStride + posInfo->pix_c_x;
		avdUInt8* srcV   = dec_picture->plnV + (posInfo->pix_c_y-BLOCK_SIZE_8x8)* UVStride + posInfo->pix_c_x;
		avdUInt8* temp2	 = info->mbsParser->m7;
		avdUInt8* temp	 = temp2;
		avdUInt8* evenRow  = temp2;
		avdUInt8* oddRow   = temp2 + 256;
		int i,y;
		int deInterlaced; 
		avdUInt8* plnY	 = srcY;
		avdUInt8* plnChroma[2];
		plnChroma[0] = srcU;
		plnChroma[1] = srcV;
		//avdUInt8* plnV	 = srcV;
		
		for (y = 0; y < (MB_BLOCK_SIZE<<1); y++,plnY+=YStride,temp+=MB_BLOCK_SIZE)
			AVD_ALIGNED32_MEMCPY4(temp, plnY);
		plnY = srcY;

		{
			for (y = 0; y < MB_BLOCK_SIZE; y++){
				AVD_ALIGNED32_MEMCPY4(plnY,evenRow);plnY+=YStride;evenRow+=MB_BLOCK_SIZE;
				AVD_ALIGNED32_MEMCPY4(plnY,oddRow);plnY+=YStride; oddRow+=MB_BLOCK_SIZE;
			}
		}
		
		YStride = UVStride;
		//temp	 = temp2;
		evenRow  = temp2;
		oddRow   = temp2 + 64;
		for (i=0;i<2;i++)
		{
			plnY = plnChroma[i];
			temp = evenRow;
			for (y = 0; y < (MB_BLOCK_SIZE); y++,plnY+=YStride,temp+=BLOCK_SIZE_8x8)
				AVD_ALIGNED32_MEMCPY2(temp, plnY);
			plnY = plnChroma[i];

			{
				for (y = 0; y < BLOCK_SIZE_8x8; y++){
					AVD_ALIGNED32_MEMCPY2(plnY,evenRow);plnY+=YStride;evenRow+=BLOCK_SIZE_8x8;
					AVD_ALIGNED32_MEMCPY2(plnY,oddRow);plnY+=YStride; oddRow+=BLOCK_SIZE_8x8;
				}
			}
			
			evenRow  = temp2;
			oddRow   = temp2 + 64;
		}

	}
}
#endif//#if FEATURE_INTERLACE
#define IsValidMB(mb) ((mb)!=NULL)
const static avdUInt8 BLK_NUM[2][4][4]  = {{{0,4,8,12},{1,5,9,13},{2,6,10,14},{3,7,11,15}},{{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}}} ;

int deblock_mbs(ImageParameters *img,TMBsProcessor* info,int endMBX)
{
#ifdef USE_JOBPOOL
    Slice			*slice =img->vdLibPar->multiCoreNum < 2 ?  img->currentSlice : img->SliceList[info->current_slice_nr-1];
#else
	Slice *slice = img->currentSlice;
#endif
	TSizeInfo	*sizeInfo = img->sizeInfo;
	if (img->vdLibPar->optFlag&OPT_DISABLE_DECODE)
		return;
#if FEATURE_INTERLACE
	
	if (GetMbAffFrameFlag(img))//MBAff is enforced to deblock_by_frame 
	{
		MacroBlock* currMB = GetCurrMB(info,GetMBX(info));
		
		MBAFFRevert2Frame(img,info);
		if (slice->LFDisableIdc == 1 || img->vdLibPar->disableDeblock)
			return 0;
		if (1)//!mbIsMbField(currMB))
		{
			InPlaceDeblockMBs(img,info);
		}
		else
		{
			if(!CurrMBisTop(info))//deblock the combined fields
			{
				InPlaceDeblockMBs(img,info->anotherProcessor);
				InPlaceDeblockMBs(img,info);
			}
		}
	}
	else 
#endif//FEATURE_INTERLACE
	if (GetMBX(info) == endMBX)
	{
		
		info->currMB =  GetCurrMB(info,GetMBX(info));
#if FEATURE_INTERLACE1//not used any more
		if(((img->vdLibPar->deblockFlag & EDF_ONLYTOPFIELD)&&img->dec_picture->structure==BOTTOM_FIELD))//for SE de-interlace, damn it
		{
			AvdLog2(LL_INFO,"damn SE\n");
		}
		else
#endif//FEATURE_INTERLACE
		{
			if (!(slice->LFDisableIdc == 1 || img->vdLibPar->disableDeblock))
				InPlaceDeblockMBs(img,info);
		}
			

		CHECK_ERR_RET_INT

			if(img->notSaveAllMV)
				if (img->type != I_SLICE
					&&GetMBX(info) == sizeInfo->PicWidthInMbs - 1

					)
				{


#if FEATURE_BFRAME

					DpbFrPostProc2(img,info);
#endif//FEATURE_BFRAME
					if(1)//GetMBY(info) == sizeInfo->PicHeightInMbs - 1)
					{
						StorablePicture *dec_picture = img->dec_picture;
						AVDStoredPicMotionInfo *motionInfo = dec_picture->motionInfo;
						AVDMotionVector *tmpMV;
						avdUInt8 *tmpRef;
						tmpMV = motionInfo->mv0[3];
						motionInfo->mv0[3] = motionInfo->mv0[-1];
						motionInfo->mv0[-1] = tmpMV;
						tmpRef = motionInfo->ref_idx0[1];
						motionInfo->ref_idx0[1]  = motionInfo->ref_idx0[-1];
						motionInfo->ref_idx0[-1] = tmpRef;


						if (img->type == B_SLICE){
							tmpMV = motionInfo->mv1[3];
							motionInfo->mv1[3] = motionInfo->mv1[-1];
							motionInfo->mv1[-1] = tmpMV;
							tmpRef = motionInfo->ref_idx1[1];
							motionInfo->ref_idx1[1]  = motionInfo->ref_idx1[-1];
							motionInfo->ref_idx1[-1] = tmpRef;
							// since ref_idx0/1 is not updated in skipped MB;
						}
#if FEATURE_BFRAME
						ResetMVRefBuf(img,motionInfo);
#endif//FEATURE_BFRAME
					}

				}


	}


	return 0;
}
#if defined(NEON)&&defined(ARM_ASM)
static void InPlaceDeblockMBs_NEON(ImageParameters *img,TMBsProcessor *info)
{
	// this is a trim down version from DeblockFrameAllCases to speed up nonMbAff case;
	StorablePicture	*dec_picture = img->dec_picture;
	avdUInt8 *plnY = dec_picture->plnY;
	avdUInt8 *plnU = dec_picture->plnU;
	avdUInt8 *plnV = dec_picture->plnV;
	TSizeInfo *sizeInfo = img->sizeInfo;
	TCLIPInfo *clipInfo = img->clipInfo;
	TPosInfo  *posInfo  = GetPosInfo(info);
	avdNativeInt uvPitch = (sizeInfo->yPlnPitch>>1);
	Macroblock		 *MbQ;//, *MbP,*mbNeighbor[2]; //mbNeighbor[left/top]
	Slice			*sliceQ;
	//SliceType		typeIPB = dec_picture->typeIPB;
	avdNativeInt    dir, edge,*boundary,tempTop,tempLeft;
	//avdNativeInt	tmpAlpha[2],tmpBeta[2];//store dir=1 value
	avdNativeUInt	mb_x, mb_y, mbIdx;
	avdUInt8		*clipPt, *Strength, *SrcY, *SrcCr[2];
	avdNativeInt	i,indexA1, indexA0, idx, QP,QP2,QP0,lfAlphaC0Offset, lfBetaOffset,lfDisableIDC;
#ifdef USE_JOBPOOL	
	avdUInt8		tmpAlpha[2],tmpBeta[2], strengthArray[32];
       Slice			*slice =img->vdLibPar->multiCoreNum < 2 ?  img->currentSlice : img->SliceList[info->current_slice_nr-1];
#else
	avdUInt8		tmpAlpha[2],tmpBeta[2], *strengthArray;
	Slice		    *slice = img->currentSlice;
#endif
	int start;
	avdUInt32* tmpStr;
	avdUInt8 alpha2[2];
	avdUInt8 beta2[2];
	avdUInt8 tc0[16];
	Macroblock	    *mb;
	AVDIPDeblocker *dbk;
	int stepEdge;
	Macroblock* leftMB=NULL,*upMB=NULL;
	lfAlphaC0Offset = slice->LFAlphaC0Offset;
	lfBetaOffset    = slice->LFBetaOffset;
if(img->deblockType==DEBLOCK_BY_MB)
{
	mb = info->currMB;
	i = posInfo->pix_y * sizeInfo->yPlnPitch;
	SrcY = dec_picture->plnY + i + posInfo->pix_x;
	i >>= 2;
	SrcCr[0] = dec_picture->plnU + i+ posInfo->pix_c_x;
	SrcCr[1] = dec_picture->plnV + i+ posInfo->pix_c_x;
	mb_x  = GetMBX(info);
	mb_y  = GetMBY(info);
	leftMB = GetLeftMB(info);
	upMB   = GetUpMB(info);

	if(slice->LFDisableIdc != 2)
	{
		if(leftMB==NULL&&mb_x)
			leftMB = mb - 1;

		if(upMB==NULL&&mb_y>=1+GetMbAffFrameFlag(img))
			upMB = GetMB2(info,mb_x);
	}
}
else
{
	mb = info->currMB - sizeInfo->PicWidthInMbs + 1;
	i = posInfo->pix_y * sizeInfo->yPlnPitch;
	SrcY = dec_picture->plnY + i;
	i >>= 2;
	SrcCr[0] = dec_picture->plnU + i;
	SrcCr[1] = dec_picture->plnV + i;
	mb_x  = 0;
	mb_y  = GetMBY(info);
	
}
	dbk  = GetIPDeblocker(info);
#ifndef USE_JOBPOOL
	strengthArray = dbk->strength;
#endif
	tmpStr=(avdUInt32*)strengthArray;

	for (mbIdx = 0; mbIdx < sizeInfo->PicWidthInMbs; mbIdx++, mb++,mb_x++)
	{
#ifdef USE_JOBPOOL
        memcpy(strengthArray, dbk->strength, sizeof(dbk->strength));
#endif
#if FEATURE_T8x8
		stepEdge=1;
		if (mbIsMbT8x8Flag(mb))
		{
			stepEdge=2;
		}
#else
		stepEdge=1;
#endif
		MbQ =  mb;
#if (DUMP_VERSION & DUMP_DEBLOCK)
		{
			int mbID = mb_y*sizeInfo->PicWidthInMbs + mb_x;
			AvdLog(DUMP_DEBLOCK,DUMP_DCORE "\n=======nonAff MB=%d=============", mbID);
		}
		
#endif //(DUMP_VERSION & DUMP_DEBLOCK)

		if (img->type != I_SLICE||GetMbAffFrameFlag(img))
		{
			
			TIME_BEGIN(start)
			SetInPlaceInterMBStrength(img,info,strengthArray,mb);
			TIME_END(start,strcalSum)
		}
		
		TIME_BEGIN(start)
		if(img->deblockType!=DEBLOCK_BY_MB)
		{
			leftMB = mb_x? mb-1:NULL;
			upMB   = mb_y? GetMB2(info,mb_x):NULL;
			if(slice->LFDisableIdc == 2)
			{
				if(!INSAMESLICE(mb,leftMB))
					leftMB = NULL;
				if(!INSAMESLICE(mb,upMB))
					upMB = NULL;
			}
		}
		if(!leftMB)
			*tmpStr=0;
		
		if(!upMB)
			*(tmpStr+4)=0;
			
		
		Strength = strengthArray;

		//other three edges
		QP  = mbGetQP(MbQ);// + MbP->flags.qp + 1) >> 1 ; // Average QP of the two blocks
		indexA1 = clipInfo->clipQP51[QP + lfAlphaC0Offset];
		alpha2[1]   = ALPHA_TABLE[indexA1];
		beta2[1]    = BETA_TABLE[clipInfo->clipQP51[QP + lfBetaOffset]]; 
		for(dir=0; dir<2; dir++,Strength+=16)
		{			
			if((!dir && leftMB) || (dir && upMB))
			{
				QP0  = (QP + (dir==0 ? mbGetQP(leftMB) : mbGetQP2(upMB)) + 1)>>1;
				indexA0 = clipInfo->clipQP51[QP0 + lfAlphaC0Offset];
				alpha2[0] = ALPHA_TABLE[indexA0];
				beta2[0]  = BETA_TABLE[clipInfo->clipQP51[QP0 + lfBetaOffset]];
				clipPt = (avdUInt8 *)(&CLIP_TAB[indexA0][0] - 1);
				for(idx=0;idx<4;idx++)
					tc0[idx] = clipPt[Strength[idx]];			
			}

			clipPt = (avdUInt8 *)(&CLIP_TAB[indexA1][0] - 1);
			for(idx=4;idx<16;idx++)
				tc0[idx] = clipPt[Strength[idx]];
		

			if(!dir)
				NeonDeblockingLumaV_ASM(SrcY,sizeInfo->yPlnPitch,alpha2,beta2,tc0,Strength);						
			else
				NeonDeblockingLumaH_ASM(SrcY,sizeInfo->yPlnPitch,alpha2,beta2,tc0,Strength);	

		}

		//Chroma,unroll the loop and reduce cache miss        
		// for internal edges
		QP = QP_SCALE_CR[clipInfo->clipQP51[mbGetQP(MbQ) + img->active_pps->chroma_qp_index_offset]];
		indexA1 = clipInfo->clipQP51[QP + lfAlphaC0Offset];
		tmpAlpha[1] = alpha2[1]  = ALPHA_TABLE[indexA1];
		tmpBeta[1]  =  beta2[1]  = BETA_TABLE[clipInfo->clipQP51[QP + lfBetaOffset]]; 
		Strength = strengthArray;

		//dir 0
		if(leftMB)
		{
			//MbP = mbNeighbor[0];
			QP2 = ((QP_SCALE_CR[clipInfo->clipQP51[mbGetQP(leftMB) +  img->active_pps->chroma_qp_index_offset]] + 
				QP + 1)>>1);
			indexA0 = clipInfo->clipQP51[QP2 + lfAlphaC0Offset];
			alpha2[0] = ALPHA_TABLE[indexA0];
			beta2[0]  = BETA_TABLE[clipInfo->clipQP51[QP2 + lfBetaOffset]];
			clipPt = (avdUInt8 *)(&CLIP_TAB[indexA0][0] - 1);

			tc0[0] = clipPt[Strength[0]];
			tc0[1] = clipPt[Strength[1]];
			tc0[2] = clipPt[Strength[2]];
			tc0[3] = clipPt[Strength[3]];
		}


		clipPt = (avdUInt8 *)(&CLIP_TAB[indexA1][0] - 1);

		tc0[4] = clipPt[Strength[8]];
		tc0[5] = clipPt[Strength[9]];
		tc0[6] = clipPt[Strength[10]];
		tc0[7] = clipPt[Strength[11]];	
		//dir 1
		Strength+=16;
		if(upMB)
		{
			//MbP = mbNeighbor[1];
			QP2 = ((QP_SCALE_CR[clipInfo->clipQP51[mbGetQP2(upMB) +img->active_pps->chroma_qp_index_offset]]
			+ QP + 1)>>1);

			indexA0 = clipInfo->clipQP51[QP2 + lfAlphaC0Offset];
			tmpAlpha[0] = ALPHA_TABLE[indexA0];
			tmpBeta[0]  = BETA_TABLE[clipInfo->clipQP51[QP2 + lfBetaOffset]];
			clipPt = (avdUInt8 *)(&CLIP_TAB[indexA0][0] - 1);

			tc0[8] = clipPt[Strength[0]];
			tc0[9] = clipPt[Strength[1]];
			tc0[10] = clipPt[Strength[2]];
			tc0[11] = clipPt[Strength[3]];
		}

		clipPt = (avdUInt8 *)(&CLIP_TAB[indexA1][0] - 1);

		tc0[12] = clipPt[Strength[8]];
		tc0[13] = clipPt[Strength[9]];
		tc0[14] = clipPt[Strength[10]];
		tc0[15] = clipPt[Strength[11]];

		//Cb
		NeonDeblockingChromaV_ASM(SrcCr,uvPitch,alpha2,beta2,tc0,strengthArray);
		//NeonDeblockingChromaV_ASM(SrcCr[1],uvPitch,alpha2,beta2,tc0,strengthArray);	
		NeonDeblockingChromaH_ASM(SrcCr,uvPitch,tmpAlpha,tmpBeta,tc0+8,strengthArray+16);
		//NeonDeblockingChromaH_ASM(SrcCr[1],uvPitch,tmpAlpha,tmpBeta,tc0+8,strengthArray+16);

		if(img->deblockType==DEBLOCK_BY_MB)
			break;
		else
		{
			SrcY     += 16;
			SrcCr[0] += 8;
			SrcCr[1] += 8;
		}
	}
	TIME_END(start,filterSum)
}
#endif//NEON


void InPlaceDeblockMBs(ImageParameters *img,TMBsProcessor *info)
{
#if defined(NEON)&& DB_ASM
	H264VdLibParam* params = img->vdLibPar;
	if(params->disableDeblock)
		return;
	return InPlaceDeblockMBs_NEON(img,info);
#else//NEON
{

	
	avdNativeInt i, j, idx, edge, C0, c0, qp,  mb_x, mb_y,indexA1,qp0=0;
	avdNativeInt L2, L1, L0, R0, R1, R2, RL0, ap, aq, Delta, AbsDelta;
	avdNativeInt Alpha, Beta, str, srcInc, inc, inc2, inc3, small_gap, tmp1, tmp2;
	AVDIPDeblocker *dbk;
	StorablePicture	*dec_picture;
	avdUInt8	*srcY, *srcU, *srcV, *strClipPt, *inStrClipPt, *pt1, *pt2;
	avdUInt8	*alpha2, *beta2, *clip255;
	Macroblock	*mb;
	int mbID;
#ifdef USE_JOBPOOL
    Slice			*slice =img->vdLibPar->multiCoreNum < 2 ?  img->currentSlice : img->SliceList[info->current_slice_nr-1];
#else
	Slice		*slice = img->currentSlice;
#endif
	//TMBsProcessor *niInfo;
	H264VdLibParam* params = img->vdLibPar;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TCLIPInfo	*clipInfo = img->clipInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	Macroblock* leftMB=NULL,*upMB=NULL;
	int start;
#if defined(NEON) && TEST_QCORE
	if (CHECK_THREAD_NEON)
		return InPlaceDeblockMBs_NEON(img,info);
#endif
	if(params->disableDeblock)
		return;
	dec_picture = img->dec_picture;

	i = posInfo->pix_y * sizeInfo->yPlnPitch;
	mb_y = GetMBY(info);
if(img->deblockType==DEBLOCK_BY_MB)
{
	srcY = dec_picture->plnY + i + posInfo->pix_x;
	i >>= 2;
	srcU = dec_picture->plnU + i + posInfo->pix_c_x;
	srcV = dec_picture->plnV + i + posInfo->pix_c_x;
	mb = info->currMB;// - sizeInfo->PicWidthInMbs + 1;
	mb_x = GetMBX(info);
	
	leftMB = GetLeftMB(info);
	upMB   = GetUpMB(info);
	
	if(slice->LFDisableIdc != 2)
	{
		if(leftMB==NULL&&mb_x)
			leftMB = mb - 1;

		if(upMB==NULL&&mb_y>=1+GetMbAffFrameFlag(img))
			upMB = GetMB2(info,mb_x);
	}

}
else
{
	srcY = dec_picture->plnY + i;
	i >>= 2;
	srcU = dec_picture->plnU + i;
	srcV = dec_picture->plnV + i;
	mb = info->currMB - sizeInfo->PicWidthInMbs + 1;
	mb_x = 0;
}
	dbk  = GetIPDeblocker(info);
	// scratch buffer;
	alpha2 = (avdUInt8 *)&info->mbsParser->m7[0][0];
	beta2  = alpha2 + 2;
	clip255 = clipInfo->clip255;
	//slice   = img->currentSlice;
	//niInfo  = img->mbsProcessor;
	

	do {
#if FEATURE_T8x8
		int stepEdge=1;
		if (mbIsMbT8x8Flag(mb))
		{
			stepEdge=2;
		}
#else
		const int stepEdge=1;
#endif
#if (DUMP_VERSION & DUMP_DEBLOCK)
		
		{
			mbID = GetMBY(info)*sizeInfo->PicWidthInMbs+ mb_x;
			AvdLog(DUMP_DEBLOCK,DUMP_DCORE "\n=======nonAff MB=%d=============", mbID);
		}
		
#endif //(DUMP_VERSION & DUMP_DEBLOCK)

		if(img->deblockType!=DEBLOCK_BY_MB)
		{
			leftMB = mb_x? mb-1:NULL;
			upMB   = mb_y? GetMB2(info,mb_x):NULL;
			if(slice->LFDisableIdc == 2)
			{
				if(!INSAMESLICE(mb,leftMB))
					leftMB = NULL;
				if(!INSAMESLICE(mb,upMB))
					upMB = NULL;
			}
		}

		if (img->type != I_SLICE||GetMbAffFrameFlag(img))
		{
			
			TIME_BEGIN(start)
			SetInPlaceInterMBStrength(img,info,GetIPDeblocker(info)->strength,mb);
			TIME_END(start,strcalSum)
		}
		TIME_BEGIN(start)
		// edges 1 to 3;
		qp      = mbGetQP(mb);
		indexA1 = clipInfo->clipQP51[qp + slice->LFAlphaC0Offset];
		inStrClipPt = (avdUInt8 *)(&CLIP_TAB[indexA1][0] - 1);
		alpha2[1] = ALPHA_TABLE[indexA1];
		beta2[1]  = BETA_TABLE[clipInfo->clipQP51[qp + slice->LFBetaOffset]]; 
		pt1 = &dbk->strength[0]; 
		i = 0;
		do { // i = 0, vertical, then i = 1, horizotal;
			if((!i && leftMB) || (i && upMB))
			{
				//qp0 = (qp + mbGetQP(mb - (!i ? 1 : sizeInfo->PicWidthInMbs)) + 1)>>1;
				qp0 = (qp + (!i ? mbGetQP(leftMB) : mbGetQP2(upMB)) + 1)>>1;
				j   = clipInfo->clipQP51[qp0 + slice->LFAlphaC0Offset];
				alpha2[0] = ALPHA_TABLE[j];
				beta2[0]  = BETA_TABLE[clipInfo->clipQP51[qp0 + slice->LFBetaOffset]];
				strClipPt = (avdUInt8 *)(&CLIP_TAB[j][0] - 1);
				edge = 0;
			}
			else{
				edge = stepEdge; // starting edge;
				pt1 += 4*stepEdge;
			}

			if (!i){
				srcInc = sizeInfo->yPlnPitch;
				inc = 1;
			}
			else{
				srcInc = 1;
				inc = sizeInfo->yPlnPitch;
			}
			inc2 = (inc<<1);
			inc3 = inc2 + inc;
			do { // edges;


#if (DUMP_VERSION & DUMP_DEBLOCK)
				//AvdLog(DUMP_DEBLOCK,DUMP_DCORE "\ndir=%d edge=%d, strength=(%d,%d,%d,%d)", 
				//	i, edge, pt1[0], pt1[1], pt1[2], pt1[3]); 
#endif //(DUMP_VERSION & DUMP_DEBLOCK)
				if(*((avdInt32 *)pt1) == 0) // only if one of the 4 Strength bytes is != 0
					goto VOI_SKIP_LUMA_EDGE;
				if (!edge){
					Alpha = alpha2[0];
					Beta = beta2[0];
				}
				else {
					Alpha = alpha2[1];
					Beta = beta2[1];
					strClipPt = inStrClipPt;
				}
				pt2 = srcY + (edge<<2) * inc; 
#if LUMA_DB_ASM
				Luma_InPlaceDeblock(pt2,
					pt1,
					inc,
					srcInc,
					Alpha,
					Beta,
					strClipPt,
					clip255
					);
#else
				for(j = 0; j < 16; j++, pt2 += srcInc)
				{
					if(!(str = pt1[j>>2])){
						j += 3; // jump 4 in all cases;
						pt2 += srcInc + (srcInc<<1);
						continue;
					}

					L1  = pt2[-inc2] ;
					L0  = pt2 [-inc];
					R0  = pt2 [   0];
					R1  = pt2[ inc ] ;
					tmp1 = R0 - R1;
					if (tmp1 < 0)
						tmp1 = -tmp1;
					tmp2 = L0 - L1;
					if (tmp2 < 0)
						tmp2 = -tmp2;
					if((tmp1 >= Beta) || (tmp2 >= Beta))
						continue; 

					Delta = R0 - L0;
					AbsDelta  = Delta > 0 ? Delta : -Delta;
					if(AbsDelta >= Alpha)
						continue; 

					L2   = pt2[-inc3] ;
					// use pt2[ inc<<1] instead of pt2[ inc2], XScale compiler generate
					// much less code; (but only change here)
					R2   = pt2[ inc<<1] ;
					tmp1 = R0 - R2;
					if (tmp1 < 0)
						tmp1 = -tmp1;
					tmp2 = L0 - L2;
					if (tmp2 < 0)
						tmp2 = -tmp2;
					aq   = tmp1 < Beta;//((tmp1 = R0 - R2) < Beta) && (tmp1 > -Beta);
					ap   = tmp2 < Beta;//((tmp1 = L0 - L2) < Beta) && (tmp1 > -Beta);
					RL0  = L0 + R0 ;
#if (DUMP_VERSION & DUMP_DEBLOCK)
					AvdLog(DUMP_DEBLOCK,DUMP_DCORE "\n\t%d dir=%d,edge=%d,str=%d,a=%d,b=%d,pelY=%d, L(%d,%d,%d,%d), R(%d,%d,%d,%d)", LumafilterIdx++,i,edge,str,Alpha,Beta,
						j, L0,L1,L2,pt2[-(inc2<<1)],R0,R1,R2,pt2[inc3]); 
#endif
					if(str != 4)    
					{
						C0			  = strClipPt[str] ;
						tmp1		  = ((Delta << 2) + (L1 - R1) + 4)>>3;
						if (tmp1){
							c0 = (C0 + ap + aq) ;
							if (tmp1 > c0)
								tmp1 = c0;
							if (tmp1 < -c0)
								tmp1 = -c0;
							pt2[-inc]  = iClip255((L0 + tmp1));
							pt2[   0]  = iClip255((R0 - tmp1));
						}
						if(ap && (tmp1 = ((L2 + ((RL0 + 1) >> 1))>>1) - L1) != 0){
							if (tmp1 > C0)
								tmp1 = C0;
							if (tmp1 < -C0)
								tmp1 = -C0;
							pt2[-inc2] += tmp1;
						}
						if(aq && (tmp1 = ((R2 + ((RL0 + 1) >> 1))>>1) - R1) != 0){
							if (tmp1 > C0)
								tmp1 = C0;
							if (tmp1 < -C0)
								tmp1 = -C0;
							pt2[inc] += tmp1;
						}
					}
					else  // INTRA strong filtering                                                                                 // normal filtering
					{
						small_gap = (AbsDelta < ((Alpha >> 2) + 2));
						if ((aq & small_gap)){
							pt2[  0 ] = (L1 + ((tmp1 = RL0 + R1)<<1) +  R2 + 4)>>3;
							pt2[inc ] = ((tmp1 += R2) + 2)>>2;
							pt2[inc2] = (((pt2[inc3] + R2)<<1) + tmp1 + 4)>>3;
						}
						else{
							pt2[  0 ] = ((R1 << 1) + R0 + L1 + 2)>>2;
						}

						if ((ap & small_gap)){
							pt2[-inc ] = (R1 + ((RL0 += L1)<<1) + L2 + 4)>>3;
							pt2[-inc2] = ((RL0 += L2) + 2)>>2;
							pt2[-inc3] = (((pt2[-(inc2<<1)] + L2)<<1) + RL0 + 4)>>3;
						}
						else{
							pt2[-inc ] = ((L1 << 1) + L0 + R1 + 2)>>2 ;
						}
					}
#if (DUMP_VERSION & DUMP_DEBLOCK)
					AvdLog(DUMP_DEBLOCK,DUMP_DCORE "\n\tY(%d,%d,%d,%d,%d,%d)", 
						pt2[-inc3],pt2[-inc2],pt2[-inc],pt2[0],pt2[inc],pt2[inc2]); 
#endif
				}
#endif//LUMA_DB_ASM
VOI_SKIP_LUMA_EDGE:
				pt1 += 4*stepEdge; // strength;
			} while ((edge+=stepEdge) < 4);
		} while (++i < 2);

		// for uv;
		// for internal edges
		qp = QP_SCALE_CR[clipInfo->clipQP51[qp + img->active_pps->chroma_qp_index_offset]];
		indexA1 = clipInfo->clipQP51[qp + slice->LFAlphaC0Offset];
		alpha2[1] = ALPHA_TABLE[indexA1];
		beta2[1]  = BETA_TABLE[clipInfo->clipQP51[qp + slice->LFBetaOffset]]; 
		inStrClipPt = (avdUInt8 *)(&CLIP_TAB[indexA1][0] - 1);
		pt1 = &dbk->strength[0]; 
		i   = 0;
		do { // i = 0, vertical, then i = 1, horizotal;
			if((!i && leftMB) || (i && upMB))
			{
				//qp0 = ((QP_SCALE_CR[clipInfo->clipQP51[mbGetQP(mb - (!i ? 1 : sizeInfo->PicWidthInMbs)) + 
				qp0 = ((QP_SCALE_CR[clipInfo->clipQP51[(!i ? mbGetQP(leftMB) : 
					mbGetQP2(upMB)) + 
					img->active_pps->chroma_qp_index_offset]]
			+ qp + 1)>>1);
			j = clipInfo->clipQP51[qp0 + slice->LFAlphaC0Offset];
			alpha2[0] = ALPHA_TABLE[j];
			beta2[0]  = BETA_TABLE[clipInfo->clipQP51[qp0 + slice->LFBetaOffset]];
			strClipPt = (avdUInt8 *)(&CLIP_TAB[j][0] - 1);
			edge = 0;
			}
			else{
				edge = 1; // starting edge;
				pt1 += 8;
			}

			if (!i){
				srcInc = (sizeInfo->yPlnPitch>>1);
				inc = 1;
			}
			else{
				srcInc = 1;
				inc = (sizeInfo->yPlnPitch>>1);
			}
			do { // edges;
#if (DUMP_VERSION & DUMP_DEBLOCK)
				//AvdLog(DUMP_DEBLOCK,DUMP_DCORE "\ndir=%d edge=%d, strength=(%d,%d,%d,%d)", 
				//	i, edge, pt1[0], pt1[1], pt1[2], pt1[3]); 
#endif //(DUMP_VERSION & DUMP_DEBLOCK)


				if(*((avdInt32 *)pt1) == 0) // only if one of the 4 Strength bytes is != 0

					goto VOI_SKIP_CHROMA_EDGE;

				if (!edge){
					Alpha = alpha2[0];
					Beta = beta2[0];
				}
				else {
					Alpha = alpha2[1];
					Beta = beta2[1];
					strClipPt = inStrClipPt;
				}
				idx = 1;
				do {
					pt2 = (idx ? srcU : srcV) + (edge<<2) * inc; 
#if CHROMA_DB_ASM
					Chroma_InPlaceDeblock(pt2,
						pt1,
						inc,

						srcInc,
						Alpha,
						Beta,
						strClipPt,
						clip255
						);
#else
					for(j = 0; j < 8; j++, pt2 += srcInc)
					{
						if(!(str = pt1[j>>1])){
							j++; // jump 4 in all cases;
							pt2 += srcInc;
							continue;
						}

						L0  = pt2 [-inc];
						R0  = pt2 [   0];
						L1  = pt2[-(inc<<1)] ;
						R1  = pt2[ inc ] ;
						tmp1 = R0 - R1;
						if (tmp1 < 0)
							tmp1 = -tmp1;
						tmp2 = L0 - L1;
						if (tmp2 < 0)
							tmp2 = -tmp2;
						if((tmp1 >= Beta) || (tmp2 >= Beta))
							continue; 

						Delta = R0 - L0;
						if(Delta >= Alpha || Delta <= -Alpha)
							continue;
#if (DUMP_VERSION & DUMP_DEBLOCK)						
						//AvdLog(DUMP_DEBLOCK, "\n\tpel%s=%d, L(%d,%d), R(%d,%d)", !idx?"V":"U",j,L0,L1,R0,R1); 
						//AvdLog(DUMP_DEBLOCK, "\n\tpel%s=%d, {%d,%d,%d,%d},", !idx?"V":"U",j,L0,L1,R0,R1); 	
						AvdLog(DUMP_DEBLOCK,DUMP_DCORE "\n\t%d dir=%d,edge=%d,pel%s=%d, L(%d,%d), R(%d,%d)", ChromafilterIdx++,i,edge,
							!idx?"V":"U",j,L0,L1,R0,R1); 
#endif
						if(str != 4)    
						{
							tmp1 = ((Delta << 2) + (L1 - R1) + 4)>>3;
							if (tmp1){
								c0 = (strClipPt[str] + 1);
								if (tmp1 > c0)
									tmp1 = c0;
								if (tmp1 < -c0)
									tmp1 = -c0;
								pt2[-inc]  = iClip255(L0 + tmp1);
								pt2[   0]  = iClip255(R0 - tmp1);
							}
						}
						else  // INTRA strong filtering                                                                                 // normal filtering
						{
							pt2[   0] = ((R1 << 1) + R0 + L1 + 2) >> 2; 
							pt2[-inc] = ((L1 << 1) + L0 + R1 + 2) >> 2;                                           
						}
#if (DUMP_VERSION & DUMP_DEBLOCK)
						AvdLog(DUMP_DEBLOCK,DUMP_DCORE "\n\t%s-1To0(%d,%d)", 
							!idx?"V":"U",pt2[-inc],pt2[0]); 
#endif
					}
#endif//CHROMA_DB_ASM
				} while (--idx >= 0);// end idx for chroma;

VOI_SKIP_CHROMA_EDGE:
				pt1 += 8; // strength jumps 2 edges;
			} while (++edge < 2);
		} while (++i < 2);

		srcY += MB_BLOCK_SIZE;
		srcU += (MB_BLOCK_SIZE>>1);
		srcV += (MB_BLOCK_SIZE>>1);
		++mb;
		TIME_END(start,filterSum)
		if(img->deblockType==DEBLOCK_BY_MB)
			break;
	} while (++mb_x < sizeInfo->PicWidthInMbs);
	
}
#endif//NEON
}





