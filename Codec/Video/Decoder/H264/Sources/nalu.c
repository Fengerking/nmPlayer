
#if !BUILD_WITHOUT_C_LIB
#include <assert.h>
#endif//BUILD_WITHOUT_C_LIB
#include "global.h"
#include "defines.h"
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
void SIAvgBlock_C(int blockSizeY, int blockSizeX,avdUInt8 * src, avdUInt8 *mpr, int Pitch);
void StoreInPlaceDeblockTopLine(ImageParameters *img,TMBsProcessor *info);
extern const  int T8x8KPos[4];//={0,2,8,10};
void CheckRef(ImageParameters *img,int listoffset,int idx)
{
#define CHECK_REF 1
#if CHECK_REF 
	StorablePicture* refFrame=img->listX[listoffset][idx];
	if (idx>=img->listXsize[listoffset]||refFrame==NULL||refFrame->valid!=BIT_PROTECT_CHECK)
	{
		AVD_ERROR_CHECK2(img,"core:refFrame==NULL",ERROR_REFFrameIsNULL);
	}
	else if (refFrame->error==OUTPUT_ERROR_FLAG&&img->vdLibPar->enableJump2I)
	{
		AVD_ERROR_CHECK2(img,"core:refFrame is not well decoded frame",ERROR_REFFrameIsNULL);
	}
#endif

}


avdNativeInt decode_one_inter_macroblock(ImageParameters *img,TMBsProcessor *info)
{
	Macroblock *currMB   = info->currMB;
	StorablePicture	*dec_picture = img->dec_picture;
	avdUInt8 *srcUV, *srcY;
	avdNativeUInt i, j, k, cacheMBIdx, xTmp, uv, uvPitch, yPitch, intra4x4,intra8x8,mbType;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	avdInt16 *cof = GetMBCofBuf(info);
	avdUInt32 iTransFlags = mbBits->iTransFlags;
	Slice *currSlice = img->currentSlice;
	TWeightInfo* weight = img->weightInfo;
	TWeightArray* wp_weight =  weight->wp_weight;
	TWeightArray* wp_offset =  weight->wp_offset;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	int weightShift = 0;
#if FEATURE_INTERLACE
	avdNativeInt curr_mb_field = (img->MbaffFrameFlag && mbIsMbField(currMB));
	avdNativeInt botMb = !CurrMBisTop(info);
#else//FEATURE_INTERLACE
#define botMb				0
#define curr_mb_field		0
#endif//FEATURE_INTERLACE
	if (img->vdLibPar->optFlag&OPT_DISABLE_DECODE)
		return;
	yPitch = sizeInfo->yPlnPitch;
	uvPitch = (yPitch>>1);
	if(img->apply_weights&&curr_mb_field)
	{
		pic_parameter_set_rbsp_t *active_pps = img->active_pps;
		int applyW2P	= active_pps->weighted_pred_flag && currSlice->picture_type == P_SLICE;
		int applyW2B    = active_pps->weighted_bipred_idc==1  && currSlice->picture_type == B_SLICE;
		weightShift = 	applyW2P|applyW2B;
	}
	{
		// inter;
		StorablePicture **list;
		AVDStoredPicMotionInfo	*motionInfo = dec_picture->motionInfo;
		AVDMotionVector  *fwMV, *bwMV, *tmpMV;
		avdInt32	 tmp32;
		avdInt16	 *clpHtCr;
		AVDMotionVector pmvfw, pmvbw;
		avdNativeUInt step_h, step_v, l,ii,jj,j4, mvJ4, i4, j8, i8, idx8x8, i1,j1;
		avdNativeInt ioff,joff;
		avdNativeInt vec1_x,vec1_y,vec2_x,vec2_y;
		avdNativeInt mv_mode; 
		avdNativeInt iTRb, iTRp, mv_scale, wt_list_offset;
		avdNativeInt max_y_cr;
		avdNativeUInt ref_idx, fw_ref_idx, bw_ref_idx;
		avdNativeUInt fw_rFrame, bw_rFrame, fw_refframe, bw_refframe;
		avdNativeInt direct_pdir;
		avdNativeInt yTmp;
		avdNativeInt xTmp2,yTmp2;
		TSizeInfo	*sizeInfo = img->sizeInfo;
		avdNativeInt width = sizeInfo->width;
		TCLIPInfo	*clipInfo = img->clipInfo;
		int gbSizeX ,gbSizeY,list_offset=0,pred_dir=0;
		int chroma_vector_adjustment = 0;
		int weightFlag1 = img->apply_weights?((img->active_pps->weighted_pred_flag&&(img->type==P_SLICE|| img->type == SP_SLICE))
												||
												(img->active_pps->weighted_bipred_idc==1 && (img->type==B_SLICE)))
											:0;
#if FEATURE_BFRAME
		avdNativeInt bDirect = 0;
		
		//list_offset = 0;  // no mb aff or frame mb
		if (!curr_mb_field) {
			list_offset = 0;  // no mb aff or frame mb
			max_y_cr = sizeInfo->height_cr-1;
			clpHtCr = clipInfo->clipHeightCr;
		}
		else {
			list_offset = botMb ? 4 : 2; 
			max_y_cr = (sizeInfo->height_cr>>1)-1;
			clpHtCr = clipInfo->clipHalfHeightCr;
		}



#else //VOI_H264D_NON_BASELINE
		//		max_y_cr = sizeInfo->height_cr-1;
		clpHtCr = clipInfo->clipHeightCr;
		list    = img->listX[0];
#endif //VOI_H264D_NON_BASELINE

		for (k = 0; k < 4; k++){ // each 8x8 block;
			mv_mode  = mbBits->uMBS.b8Info.b8Mode[k];
#if FEATURE_BFRAME
			if(img->type==B_SLICE&&!mv_mode)
			{
				step_h   = 
				step_v   = mbBits->uMBS.b8Info.b8Step[k];
				if(currSlice->direct_type!=B_DIRECT_TEMPORAL)
					pred_dir = mbBits->uMBS.b8Info.b8DirectDir[k];
				else
					pred_dir = mbBits->uMBS.b8Info.b8Pdir[k];
			}
			else
#endif
			{
				step_h   = BLOCK_STEP[mv_mode];
				step_v   = BLOCK_STEP[8+mv_mode]; 
#if FEATURE_BFRAME
				pred_dir = mbBits->uMBS.b8Info.b8Pdir[k];
#endif //VOI_H264D_NON_BASELINE
			}

			gbSizeX = (step_h<<2);
			gbSizeY = (step_v<<2);

			if(info->mb_y == 0xd && info->mb_x == 0x35 && info->start_mb_nr == 699)
			{
                j4 = 0;
			}

			for (j = 0; j < 2; j += step_v){
				joff = (k&0x2) | j;
				j4 = posInfo->block_y | joff;
				if(img->notSaveAllMV)
					mvJ4 = joff;
				else //!SAVE_ALL_MV
					mvJ4 = j4;

				j8 = (mvJ4>>1);
				joff <<= 2;
				for (i = 0; i < 2; i += step_h){
					i4 = posInfo->block_x | ((k&0x1)<<1) | i;
					i8 = (i4>>1);
					srcY = dec_picture->plnY + ((yPitch * j4 + i4)<<2);


#if FEATURE_BFRAME
					if(pred_dir>2||pred_dir<0)
						AVD_ERROR_CHECK2(img,"@!pred_dir>2||pred_dir<0\n",ERROR_InvalidBitstream);
					if (pred_dir != 2)
					{
						fwMV = pred_dir == 0 ? &motionInfo->mv0[mvJ4][i4] : 
							&motionInfo->mv1[mvJ4][i4];
#else //FEATURE_BFRAME
					{
						fwMV = &motionInfo->mv0[mvJ4][i4];
#endif //FEATURE_BFRAME

						//===== FORWARD/BACKWARD PREDICTION =====
#if FEATURE_BFRAME
						fw_refframe = ref_idx = (pred_dir == 0) ?
							motionInfo->ref_idx0[j8][i8] :
						motionInfo->ref_idx1[j8][i8];
						list    = img->listX[list_offset + pred_dir];
#if FEATURE_INTERLACE
						if (!curr_mb_field)
							vec1_y = (j4<<4) + fwMV->y;
						else if (!botMb) 
							vec1_y = (((posInfo->block_y<<1) + joff)<<2) + fwMV->y;
						else
							vec1_y = ((((posInfo->block_y-4)<<1) + joff)<<2) + fwMV->y;

					
#else//FEATURE_INTERLACE
						vec1_y = (j4<<4) + fwMV->y;
#endif//FEATURE_INTERLACE
#else //FEATURE_BFRAME
						fw_refframe = ref_idx = motionInfo->ref_idx0[j8][i8];
						vec1_y = (j4<<4) + fwMV->y;
#endif //FEATURE_BFRAME

						vec1_x  = (i4<<4) + fwMV->x;

						CHECK_REF2(img,list_offset + pred_dir,ref_idx);
						STORE_REF_FRAME(list[ref_idx])
							GetBlockLumaNxN(img,info, vec1_x, vec1_y, list[ref_idx],
							srcY, yPitch,gbSizeX ,gbSizeY);

						// assume 420???
#if FEATURE_INTERLACE
						if (GetMbAffFrameFlag(img)){
								chroma_vector_adjustment = 0;
								if(curr_mb_field)
								{
									if(!botMb && list[ref_idx]->structure == BOTTOM_FIELD)
										chroma_vector_adjustment = -2;
									else if(botMb && list[ref_idx]->structure == TOP_FIELD)
										chroma_vector_adjustment = 2;
								}
						}
						else
							chroma_vector_adjustment = list[ref_idx]->chroma_vector_adjustment;
						vec1_y += chroma_vector_adjustment;
						
#endif //FEATURE_INTERLACE

						//AvdLog(DUMP_SLICE,DUMP_DCORE "\nGBLCvec1:vec(%d + %d),list=%d",vec1_y, chroma_vector_adjustment,list_offset);

						jj = (j4<<1) * uvPitch + (i4<<1);
						srcUV = dec_picture->plnU + jj;

						GetBlockChromaNxN(img,info, vec1_x, vec1_y, list[ref_idx],0, 
							srcUV, uvPitch, clpHtCr,gbSizeX ,gbSizeY);


						srcUV = dec_picture->plnV + jj;

						GetBlockChromaNxN(img,info, vec1_x, vec1_y, list[ref_idx],1,
							srcUV, uvPitch, clpHtCr,gbSizeX ,gbSizeY);

#if FEATURE_WEIGHTEDPRED
						if (img->apply_weights)
						{
							if (weightFlag1)
								ref_idx >>=curr_mb_field;
							
							applyWeights(img,info,pred_dir,ref_idx,fw_refframe>>curr_mb_field,
								j4,
								i4,gbSizeX ,gbSizeY);

						}
#endif //FEATURE_WEIGHTEDPRED
					}
#if FEATURE_BFRAME
	else //if (pred_dir != 2)
	{
		avdNativeInt tmp;
		avdUInt8* tmpMPR = info->mbsParser->mpr;
			//GetMbAffFrameFlag(img)?GetRowCof(currMB):info->mbsParser->mpr;

		fwMV = &motionInfo->mv0[mvJ4][i4];
		if(motionInfo->mv1==NULL)
			AVD_ERROR_CHECK2(img,"motionInfo->mv1 is null",ERROR_NULLPOINT);
		bwMV = &motionInfo->mv1[mvJ4][i4];

		fw_ref_idx = motionInfo->ref_idx0[j8][i8];
		bw_ref_idx = motionInfo->ref_idx1[j8][i8];



		{
			struct storable_picture* refFrame;
			// TBC: 11.28.03;
			vec1_x = (i4<<4) + fwMV->x;
			vec2_x = (i4<<4) + bwMV->x;
#if FEATURE_INTERLACE
			if (!curr_mb_field)
			{
				tmp	 = (j4<<4);
				vec1_y =  tmp + fwMV->y;
				vec2_y =  tmp + bwMV->y;
			}
			else if (!botMb)
			{
				tmp = (((posInfo->block_y<<1) + joff)<<2);
				vec1_y =  tmp + fwMV->y;
				vec2_y =  tmp + bwMV->y;
			}
			else
			{
				tmp = ((((posInfo->block_y-4)<<1) + joff)<<2);
				vec1_y =  tmp + fwMV->y;
				vec2_y =  tmp + bwMV->y;
			}
#else//FEATURE_INTERLACE
			vec1_y = (j4<<4) + fwMV->y;
			vec2_y = (j4<<4) + bwMV->y;
#endif//FEATURE_INTERLACE
			refFrame=img->listX[list_offset][fw_ref_idx];
			CHECK_REF2(img,list_offset,fw_ref_idx);
			STORE_REF_FRAME(refFrame)
				GetBlockLumaNxN(img,info, vec1_x, vec1_y, refFrame, srcY, yPitch,gbSizeX ,gbSizeY);
			refFrame=img->listX[1+list_offset][bw_ref_idx];
			CHECK_REF2(img,1+list_offset,bw_ref_idx);
			STORE_REF_FRAME(refFrame)
				GetBlockLumaNxN(img,info, vec2_x, vec2_y, refFrame, tmpMPR, 16,gbSizeX ,gbSizeY);
		} //if (mv_mode==0 && currSlice->direct_type)

		if(!img->apply_weights) {
			//if (mv_mode || !currSlice->direct_type || direct_pdir==2)
			SILumaAvgBlock(img,info, (i4<<2), (j4<<2), tmpMPR,gbSizeX ,gbSizeY);
		}
		else{ //if(!img->apply_weights)
		
			applyWeights2(img,info,fw_ref_idx,bw_ref_idx,
				srcY,
				0,list_offset,
				gbSizeX ,gbSizeY,weightShift);
		} //if(!img->apply_weights)

		vec1_x = (i4<<4);
#if FEATURE_INTERLACE
		if (!curr_mb_field)
			vec1_y= ((posInfo->pix_c_y+(joff>>1))<<3); // joff is 2 * offset of chroma;
		else if (!botMb) 
			vec1_y=((posInfo->pix_c_y)<<2) + (joff<<2);
		else
			vec1_y=((posInfo->pix_c_y-8)<<2) + (joff<<2);
#else//FEATURE_INTERLACE
		vec1_y= ((posInfo->pix_c_y+(joff>>1))<<3); // joff is 2 * offset of chroma;
#endif//FEATURE_INTERLACE
		yTmp = (j4<<1);
		xTmp = (i4<<1);
		xTmp2 = vec1_x;
		yTmp2 = vec1_y;
		for (uv = 0; uv < 2; uv++){
			srcUV = uv == 0 ? dec_picture->plnU : dec_picture->plnV;
			srcUV += yTmp * uvPitch + xTmp;
			vec1_x = xTmp2; 
			vec1_y = yTmp2;


			vec2_x = vec1_x + fwMV->x;
			vec2_y = vec1_y + fwMV->y; 
#if FEATURE_INTERLACE
			if (GetMbAffFrameFlag(img)){
				chroma_vector_adjustment = 0;
				if(curr_mb_field)
				{
					if(!botMb && img->listX[0+list_offset][fw_ref_idx]->structure == BOTTOM_FIELD)
						chroma_vector_adjustment = -2;
					else if(botMb && img->listX[0+list_offset][fw_ref_idx]->structure == TOP_FIELD)
						chroma_vector_adjustment = 2;
				}
			}
			else
				chroma_vector_adjustment = img->listX[0+list_offset][fw_ref_idx]->chroma_vector_adjustment;
			vec2_y += chroma_vector_adjustment;
#endif//FEATURE_INTERLACE
			;
			GetBlockChromaNxN(img,info, vec2_x, vec2_y, img->listX[0+list_offset][fw_ref_idx],uv,
				srcUV, uvPitch, clpHtCr,gbSizeX ,gbSizeY);
			vec2_x = vec1_x + bwMV->x;
			vec2_y = vec1_y + bwMV->y; 
#if FEATURE_INTERLACE

			if (GetMbAffFrameFlag(img)){
				chroma_vector_adjustment = 0;
				if(curr_mb_field)
				{
					if(!botMb && img->listX[1+list_offset][bw_ref_idx]->structure == BOTTOM_FIELD)
						chroma_vector_adjustment = -2;
					else if(botMb && img->listX[1+list_offset][bw_ref_idx]->structure == TOP_FIELD)
						chroma_vector_adjustment = 2;
				}
			}
			else
				chroma_vector_adjustment = img->listX[1+list_offset][bw_ref_idx]->chroma_vector_adjustment;
			vec2_y += chroma_vector_adjustment;
			
			//img->listX[1+list_offset][bw_ref_idx]->chroma_vector_adjustment;
#endif//#if FEATURE_INTERLACE
			//AvdLog(DUMP_SLICE,DUMP_DCORE "\nGBLCvec:vec(%d + %d + %d),list=%d",vec1_y, bwMV->y,chroma_vector_adjustment,list_offset);

			GetBlockChromaNxN(img,info, vec2_x, vec2_y, img->listX[1+list_offset][bw_ref_idx],uv,
		
				tmpMPR, 16, clpHtCr,gbSizeX ,gbSizeY);


			if (!img->apply_weights) {
				SIChromaAvgBlock(img,info, xTmp, yTmp, &srcUV, tmpMPR,gbSizeX ,gbSizeY);
			}
			else
			{
				applyWeights2(img,info,fw_ref_idx,bw_ref_idx,
					srcUV,
					uv+1,list_offset,gbSizeX ,gbSizeY,weightShift);

			}
		}
	} //if (pred_dir != 2)
#endif //FEATURE_BFRAME
				} // for i;
			} // for j;
			k += (step_h>>2);
			if (step_v == 4&&k>0) // 16x16 or 8x16
				break;
		} // for k;
		//itrans Y
		//if (parser->cbp&15)
		if(mbCBPHasLuma(mbBits))
		{
			//if (!smb)
		 {
			 int jj,ii;
			 xTmp = 0;

			 srcY = dec_picture->plnY + posInfo->pix_y * yPitch + posInfo->pix_x;
#if FEATURE_T8x8
			 if (mbIsMbT8x8Flag(currMB))
				{
					//block0
					if ((iTransFlags & 0x0000000f))//||(iTransFlags & 2)||(iTransFlags & 4)||(iTransFlags & 8))
						itrans8x8(img,info, srcY, cof, yPitch);
#if (DUMP_VERSION & DUMP_SLICE)//this is only for comparision with ref code
					else
					{
						//srcY = dec_picture->plnY + j * yPitch + i;
						DUMP_T8x8_NO_T(img,info, srcY, cof, yPitch);
					}
#endif//#if (DUMP_VERSION & DUMP_SLICE)
					if ((iTransFlags & 0x000000f0))//||(iTransFlags & 32)||(iTransFlags & 64)||(iTransFlags & 128))
						itrans8x8(img,info, srcY+8, cof+64, yPitch);
#if (DUMP_VERSION & DUMP_SLICE)//this is only for comparision with ref code
					else
					{
						//srcY = dec_picture->plnY + j * yPitch + i;
						DUMP_T8x8_NO_T(img,info, srcY+8, cof+64, yPitch);
					}
#endif//#if (DUMP_VERSION & DUMP_SLICE)
					if ((iTransFlags & 0x00000f00))//||(iTransFlags & 32*16)||(iTransFlags & 64*16)||(iTransFlags & 128*16))
						itrans8x8(img,info, srcY+yPitch*8, cof+128, yPitch);
#if (DUMP_VERSION & DUMP_SLICE)//this is only for comparision with ref code
					else
					{
						//srcY = dec_picture->plnY + j * yPitch + i;
						DUMP_T8x8_NO_T(img,info, srcY+yPitch*8, cof+128, yPitch);
					}
#endif//#if (DUMP_VERSION & DUMP_SLICE)
					if ((iTransFlags & 0x0000f000))//16*256)||(iTransFlags & 32*256)||(iTransFlags & 64*265)||(iTransFlags & 128*256))
						itrans8x8(img,info, srcY+8+yPitch*8, cof+192, yPitch);
#if (DUMP_VERSION & DUMP_SLICE)//this is only for comparision with ref code
					else
					{
						//srcY = dec_picture->plnY + j * yPitch + i;
						DUMP_T8x8_NO_T(img,info, srcY+8+yPitch*8, cof+192, yPitch);
					}
#endif//#if (DUMP_VERSION & DUMP_SLICE)
				}
			 else
#endif//FEATURE_T8x8
				{
					for (jj = 0; jj < 4; jj++){
						for (ii = 0; ii < 4; ii++){
							if (iTransFlags & (1<<xTmp)){
								itrans(img,info, srcY + (ii<<2), 
									cof + (xTmp<<4), yPitch);
							}
							xTmp++;
						}
						srcY += (yPitch<<2);
					}
				}

			}
		} // if (parser->cbp&15);
	} 

}
static void CheckIntraNeighbor(ImageParameters *img,TMBsProcessor* info)
{
	int	constrained_intra_pred_flag2 = img->active_pps->constrained_intra_pred_flag &&img->type != I_SLICE ;
	MacroBlock* currMB = info->currMB;
	if (constrained_intra_pred_flag2 && IS_INTRA (currMB)){
#if FEATURE_INTERLACE
		if (GetMbAffFrameFlag(img)&& IsNeedToCalcualteNeighborA_MBAFF(info) && !IS_INTRA(currMB - 1))
			ResetNeighborA_MBAFF(info);
		else
#else//FEATURE_INTERLACE
	if (GetLeftMB(info) && !IS_INTRA(currMB - 1))
		GetLeftMB(info) = NULL;
#endif//FEATURE_INTERLACE
	if (GetUpMB(info) && !niIsIntraMb2(info, GetMBX(info)))//
		GetUpMB(info) = NULL;
	if (GetUpRightMB(info) && !niIsIntraMb2(info, GetMBX(info) + 1))
		GetUpRightMB(info) = NULL;
	if (GetUpLeftMB(info) && !niIsIntraMb2(info, GetMBX(info) - 1))
		GetUpLeftMB(info) = NULL;
	}
}
avdNativeInt decode_one_macroblock(ImageParameters *img,TMBsProcessor *info)
{
	Macroblock *currMB   = info->currMB;
	StorablePicture	*dec_picture = img->dec_picture;
	avdUInt8 *srcUV, *srcY;
	avdNativeUInt i, j, k, cacheMBIdx, xTmp, uv, uvPitch, yPitch, intra4x4,intra8x8,mbType;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	avdInt16 *cof = GetMBCofBuf(info);
	avdUInt32 iTransFlags = mbBits->iTransFlags;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	yPitch = sizeInfo->yPlnPitch;
	uvPitch = (yPitch>>1);
	mbType = mbGetMbType(currMB);
	intra4x4 = mbType==I4MB;
	if (img->vdLibPar->optFlag&OPT_DISABLE_DECODE)
		return;	
	
#if USE_SEINTERLACE
	if ((img->vdLibPar->optFlag&OPT_ENABLE_DEINTERLACE)&&!dec_picture->used_for_reference)
	{
		if (dec_picture->structure==BOTTOM_FIELD
			)
		{
			//AvdLog2(LL_INFO,"@Interlace drop one MB\n");
			return 1;//do nothing in this case
		}
#if USE_SEINTERLACE_MBAFF
		if(GetMbAffFrameFlag(img)&&(mbIsMbField(currMB)&&!CurrMBisTop(info)))
		{
			Copy2Bottom(img,info);
			return 1;
		}
#endif//USE_SEINTERLACE_MBAFF
		
	}
#endif//#if USE_SEINTERLACE	
	if(IS_INTER(currMB))
	{
		if (img->type==I_SLICE)
			AVD_ERROR_CHECK(img,"I_SLICE has intermb\n",ERROR_InvalidBitstream);
		
		decode_one_inter_macroblock(img,info);
	}
	else
	{
		

		CheckIntraNeighbor(img,info);
		if (intra4x4 || mbType==I16MB)
		{
			avdNativeInt pX, pY;

			if (mbType==I16MB)//!intra4x4)
				intrapred_luma_16x16(img, info,mbGetI16PredMode(mbBits));

			pX = posInfo->pix_x;
			pY = posInfo->pix_y;
			for (k = 0; k < 16; k++){
				i = pX | (decode_scan[k]<<2);
				j = pY | (decode_scan[16+k]<<2);
				xTmp = inv_decode_scan[k];
				if (intra4x4)				
					intrapred(img,info, i, j);  /* make 4x4 prediction block from given prediction img->mb_mode */

				if (iTransFlags & (1<<xTmp)){
					srcY = dec_picture->plnY + j * yPitch + i;
					itrans(img,info, srcY, cof + (xTmp<<4), yPitch);
				}
			}
		}
#if FEATURE_T8x8
		else if(mbType==I8MB)
		{
			avdNativeInt pX, pY;

			int pos;
			pX = posInfo->pix_x;
			pY = posInfo->pix_y;
			for (pos = 0; pos < 4; pos++){
				int flag1;
				k=T8x8KPos[pos];
				i = pX | ((pos&1)<<3);
				j = pY | ((pos>>1)<<3);
				xTmp = inv_decode_scan[k];
				intrapred8x8(img,info, i, j);  /* make 8x8 prediction block from given prediction img->mb_mode */
				flag1=(1<<xTmp)|(1<<(xTmp+1))|(1<<(xTmp+2))|(1<<(xTmp+3));
				if (iTransFlags & flag1){
					srcY = dec_picture->plnY + j * yPitch + i;
					itrans8x8(img,info,srcY, cof + (pos<<6), yPitch);
				}
			}

		}
#endif//FEATURE_T8x8
		intrapred_chroma_uv(img,info);

	}
	



	// itran and add uv;
	//if (parser->cbp > 15)
	if(mbCBPHasChroma(mbBits))
	{
		{

			k = posInfo->pix_c_y * uvPitch + posInfo->pix_c_x;
			for(uv=0;uv<2;uv++)
			{

				srcUV = uv == 0 ? dec_picture->plnU : dec_picture->plnV;
				srcUV += k;
				// notice: combine i & j loops caused a strange initial bad picture problem in Windows CE;
				for (j = 4; j < 6; j++)
				{
					xTmp = (uv<<1) | (j<<2);
					if (iTransFlags & (1<<xTmp))
						itrans(img,info, srcUV, cof + (xTmp<<4), uvPitch);
					xTmp++;

					if (iTransFlags & (1<<xTmp))
						itrans(img,info, srcUV + 4, cof + (xTmp<<4), uvPitch);
					srcUV += (uvPitch<<2);
				}
			}
		}
	} // if (parser->cbp > 15);
	return 0;
}





int decode_mbs(ImageParameters *img,TMBsProcessor* info,int decNum)
{
	TFuncInfo	*funcInfo = img->funcInfo;
	TMBsParser*	 parser = info->mbsParser;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo	*posInfo  = GetPosInfo(info);
	int coreNum = img->vdLibPar->multiCoreNum;
	int width  = sizeInfo->PicWidthInMbs;//*(1+GetMbAffFrameFlag(img));
	int mbAffPos = img->sizeInfo->PicWidthInMbs*(info->mb_y - (info->mb_y&1));
	int endRow = 0,ret;
	
	
	
	
	info->dec_mbaff_pos = mbAffPos;
#if 1//def X86_TEST
#define TRACE_DEC 0
#endif

#if TRACE_DEC
	if(info->mcoreThreadParam)
		AvdLog2(LL_INFO,"@!@decode_mbs(%d,%d)",GetMBX(info)+1,GetMBY(info),GetMBX(info));
#endif
	
	do
	{
		do 
		{
#if FEATURE_INTERLACE
			if(!GetMbAffFrameFlag(img)||!info->currMBisBottom)
#endif//#if FEATURE_INTERLACE
				GetMBX(info)++;
			while(!IsReadyDecode(img,info,info->anotherProcessor))
			{
				CHECK_ERR_RET_INT
					Wait2(img,info,0);
				if((info->mcoreThreadParam==NULL)//current is main thread
					&&(info->anotherProcessor->mcoreThreadParam==NULL))//if the work thread exited, the main thread should exit 
					break;
			}
#if FEATURE_INTERLACE
			if(GetMbAffFrameFlag(img))
			{
				Init_MB_Neighbor_InfoMBAff(img,info,0);
			}
			else
#endif//FEATURE_INTERLACE
				Init_MB_Neighbor_Info(img,info,0);

			/* Define vertical positions */
			posInfo->block_y = (GetMBY(info)<<2);	   /* luma block position */
			posInfo->pix_y	 = (GetMBY(info)<<4);	/* luma macroblock position */
			posInfo->pix_c_y = (GetMBY(info)<<3);	/* chroma macroblock position */

			/* Define horizontal positions */
			posInfo->block_x = (GetMBX(info)<<2);	   /* luma block position */
			posInfo->pix_x	 = (GetMBX(info)<<4);				/* luma pixel position */
			posInfo->pix_c_x = (GetMBX(info)<<3);	/* chroma pixel position */
			if(mbGetMbType(info->currMB) == IPCM) 
			{
				decode_ipcm_mb(img,info);
			}
			else
			{
				//if (img->type!=P_SLICE) //test
				ret = decode_one_macroblock(img,info);
			}
			trace_overflow(img);
			mbSaveMB(info->currMB);
			CHECK_ERR_RET_INT
			//if(ret==0)//ret==1 means the MB is skipped which is used in the dropping one MB in deinterlace
			StoreInPlaceDeblockTopLine(img,info);
			CHECK_ERR_RET_INT
#if TRACE_DEC
				if(info->mcoreThreadParam)
					TAvdLog(LL_INFO,"@!@:Dec another(%d,%d),s=%d,curr(%d,%d),s=%d\n",GetMBX(info->anotherProcessor),GetMBY(info->anotherProcessor),info->anotherProcessor->status,GetMBX(info),GetMBY(info),info->status);
#endif//#ifdef X86_TEST
#if (DUMP_VERSION & DUMP_SLICE)
				//if (GetMBX(info) == sizeInfo->PicWidthInMbs - 1)
				DumpMBBeforeDeblock(img,info,GetCurPos(info,sizeInfo),GetCurPos(info,sizeInfo)+1);
#endif
			if(img->deblockType==DEBLOCK_BY_MB)
			{
				deblock_mbs(img,info,GetMBX(info));
			}
			if(GetMbAffFrameFlag(img))
			{
				ExitOneMBAff(img,info);
			}
			CHECK_ERR_RET_INT
			if (GetMbAffFrameFlag(img))
			{
				info->dec_mbaff_pos = mbAffPos 	+ info->dec_mb_num + 1;	
				endRow = ((info->dec_mb_num+1)>>1)==width;
			}
			else
				endRow = info->dec_mb_num+1==width;

			if (endRow&&coreNum>1){//for thread safe, check the boundary conditon first
				
#ifdef        USE_JOBPOOL
                           info->flag |= 8;
#else
				info->read_mb_num = 0;
				info->dec_mb_num  = 0;
				info->flag |= 8;

				GetMBY(info)  = GetMBY(info) + (1 + GetMbAffFrameFlag(img))*coreNum;//jump to next 
				GetMBX(info)  = -1;
#endif				
				
				AvdLog2(LL_INFO,"newY=%d,anY=%d,coreNum=%d\n",  GetMBY(info),GetMBY(info->anotherProcessor),coreNum);
			}
			else
				info->dec_mb_num++;
		
			break;
		} while (1);

	} while (--decNum>0);

#if TRACE_DEC
	if(info->mcoreThreadParam)
		TAvdLog(LL_INFO," y=%d,x=%d,ay=%d,ax=%d,(%d,%d)\n",GetMBY(info),GetMBX(info),GetMBY(info->anotherProcessor),GetMBX(info->anotherProcessor),info->dec_mb_num,info->read_mb_num);
#endif//#ifdef X86_TEST
	return  GetCurPos(info,sizeInfo);
}










