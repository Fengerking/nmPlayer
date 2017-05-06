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
#if FEATURE_BFRAME
static const avdInt16 iRPArray[32]={
	0,16384,8192,5461,
	4096,3277,2731,2341,
	2048,1820,1638,1489,
	1365,1260,1170,1092,
	1024,964,910,862,
	819,780,745,712,
	683,655,630,607,
	585,565,546,529
};
void read_motion_info_of_BTempol(ImageParameters *img,TMBsProcessor *info)
{
	Macroblock *currMB  = info->currMB;
	StorablePicture	*dec_picture = img->dec_picture;
	struct storable_picture	* List1,*List0;
	struct storable_picture	** listMap;
	AVDStoredPicMotionInfo	*motionInfo = dec_picture->motionInfo;
	AVDStoredPicMotionInfo	*L1_motionInfo;
	avdUInt8 fw_rFrame,bw_rFrame;

	TMBBitStream	*mbBits = GetMBBits(currMB);
	avdUInt8 **ref_idx0, **ref_idx1, **L1_ref_idx0, **L1_ref_idx1;
	AVDMotionVector	**mv0, **mv1,**L1_mv0, **L1_mv1;

	//avdUInt8 	     *b8Pdir = mbBits->uMBS.b8Info.b8DirectDir;
	avdUInt8 	     *b8Step = mbBits->uMBS.b8Info.b8Step;
	avdUInt8 		*b8Mode = mbBits->uMBS.b8Info.b8Mode;
	avdInt32		picNum;
	SliceType ipbType ;
	avdNativeUInt motionInfoNotExist,block_y,block_x;//, j6;
	int mapped_idx, iref, refList;
	AVDMotionVector **mvRef;
	AVDMotionVector *tmpMV1;
	avdNativeInt  k,i,j,j4, i4, j8,i8,ii,jj, xTmp, yTmp,ref_idx,iTRb,iTRp,mv_scale;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	avdNativeInt  imgblock_y2,imgblock_y = posInfo->block_y;
	avdNativeInt imgblock_x = posInfo->block_x;
	avdNativeUInt is8x8Inference = img->active_sps->direct_8x8_inference_flag;
	avdNativeInt refMax; 
	avdNativeInt list_offset, curr_mb_field,botMb,j4_backup;
	int all4InOne = 0;
	TMVRef1		mvRef1={0};

	avdInt32*  currDec_ref_pic_num;
	int mbType = mbGetMbType(currMB);
	imgblock_y2 = imgblock_y;
#if FEATURE_INTERLACE
	botMb = !CurrMBisTop(info);
	curr_mb_field = (GetMbAffFrameFlag(img) && mbIsMbField(currMB));
	list_offset = !curr_mb_field ? 0 : botMb ? 4 : 2;
	imgblock_y = !curr_mb_field ? posInfo->block_y : botMb ? ((posInfo->block_y-4)>>1) :
		(posInfo->block_y>>1);
#endif//FEATURE_INTERLACE
	List1 = img->listX[LIST_1+list_offset][0];
	List0 = img->listX[LIST_0+list_offset][0];
	listMap = img->listX[LIST_0 + list_offset];
	refMax = min(img->num_ref_idx_l0_active, img->listXsize[LIST_0+list_offset]);
	currDec_ref_pic_num = dec_picture->ref_pic_num[LIST_0+ list_offset];

	if(List1==NULL)
		AVD_ERROR_CHECK2(img,"list[1][0] is NULL1\n",ERROR_InvalidBitstream);
	L1_motionInfo  = List1->motionInfo;
	ipbType = List1->typeIPB;
	ref_idx0 = motionInfo->ref_idx0;
	mv0 = motionInfo->mv0;
	ref_idx1 = motionInfo->ref_idx1;
	mv1 = motionInfo->mv1;
	iTRp = 0;
	if(L1_motionInfo)
	{
		L1_ref_idx0 = L1_motionInfo->ref_idx0;
		L1_mv0 = L1_motionInfo->mv0;
		L1_ref_idx1 = L1_motionInfo->ref_idx1;
		L1_mv1 = L1_motionInfo->mv1;
	}
	b8Step[0] = 
		b8Step[1] = 
		b8Step[2] = 
		b8Step[3] = 2;//default use 2, mbGetMbType(currMB)==0?4:2;
	for (i=0;i<4;i++)
	{
		int step2 = b8Step[i];
		int tmpiTRpHalf;
		int prescale ;
		if (b8Mode[i])
			continue;

		yTmp = ((i>>1)<<1);
		xTmp = ((i&1)<<1);

		block_y = imgblock_y + yTmp ;
		block_x = imgblock_x + xTmp ;
		j4_backup = j4=imgblock_y2 + yTmp;
		i4=block_x;
		j8 = block_y>>1;
		i8 = block_x>>1;
		refList = (ipbType == I_SLICE || 
			L1_ref_idx0[j8][i8]== AVD_INVALID_REF_IDX ? LIST_1 : LIST_0);
		motionInfoNotExist = (ipbType == I_SLICE || (ipbType == P_SLICE &&
			refList == LIST_1));
		ref_idx= motionInfoNotExist ? AVD_INVALID_REF_IDX :
			(refList == LIST_0) ? 
			L1_ref_idx0[j8][i8] :
		L1_ref_idx1[j8][i8];

		j8 = j4>>1;//now the j8 points to the cur motion
		if (ref_idx == AVD_INVALID_REF_IDX)
		{
			ref_idx0[j8][i8] = 0;
			ref_idx1[j8][i8] = 0; 
			// TBD: break here;
		}
		else
		{

			int list0_poc;
			mvRef = (refList == 0) ? L1_mv0 :L1_mv1;	
			picNum = List1->ref_pic_num[refList][ref_idx];
			mapped_idx=INVALIDINDEX;
			for (iref=0;iref < refMax;iref++)
			{

				if(dec_picture->structure==0 && curr_mb_field==0)//GetMbAffFrameFlag(img)&&
				{
					// If the current MB is a frame MB and the colocated is from a field picture,
					// then the colocated->ref_pic_id may have been generated from the wrong value of
					// frame_poc if it references it's complementary field, so test both POC values
					if(img->listX[0][iref]->top_poc*2 == picNum || img->listX[0][iref]->bottom_poc*2 == picNum)
					{
						mapped_idx=iref;
						break;
					}
				}

				if (currDec_ref_pic_num[iref] == picNum) {
					mapped_idx=iref;
					break;
				}
			}
			if (INVALIDINDEX == mapped_idx)
			{
				mapped_idx =0;
	//			AVD_ERROR_CHECK2(img,"temporal direct error\ncolocated block has ref that is unavailable",ERROR_InvalidRefIndex);
			}

			ref_idx0[j8][i8] = mapped_idx;
			ref_idx1[j8][i8] = 0; 
			list0_poc = listMap[mapped_idx]->poc;

			iTRp = (avdNativeInt)(List1->poc - list0_poc); 
			if(iTRp>31)
			{
				AVD_ERROR_CHECK2(img,"@iTRp>31",100)
					iTRp = 31;
			}
			tmpMV1 = refList == 0 ? &L1_mv0[block_y+1][i4] : &L1_mv1[block_y+1][i4];

			if(is8x8Inference||(*(avdInt32 *)mvRef== *(avdInt32 *)(mvRef + 1) &&
				*(avdInt32 *)mvRef == *(avdInt32 *)tmpMV1 &&
				*(avdInt32 *)mvRef == *(avdInt32 *)(tmpMV1 + 1)))
			{
				step2 = 2; 
			}
			else{

				step2 = 1; 
			}

			if (iTRp==0)
			{
				mv0[j4][i4].x = mvRef[block_y][i4].x;
				mv0[j4][i4].y = mvRef[block_y][i4].y;
				//dec_picture->mv1[i4][j4][ii]=0;
			}
			else
			{
#if FEATURE_INTERLACE
				if (!curr_mb_field)
					iTRb = (avdNativeInt)(dec_picture->poc - list0_poc);
				else if (!botMb)
					iTRb = (avdNativeInt)(dec_picture->top_poc - list0_poc);
				else
					iTRb = (avdNativeInt)(dec_picture->bottom_poc - list0_poc);
#else//FEATURE_INTERLACE
				iTRb = (avdNativeInt)(dec_picture->poc - list0_poc);
#endif//FEATURE_INTERLACE
				tmpiTRpHalf = iTRp / 2;
				prescale = iRPArray[iTRp];//(16384 + absm(tmpiTRpHalf)) / iTRp;

				if(iTRp<0)
					prescale = -prescale;
				mv_scale = Clip3(-1024, 1023, (iTRb * prescale + 32)>>6) ;
				mv0[j4][i4].x=(mv_scale * mvRef[block_y][i4].x + 128)>>8;
				mv0[j4][i4].y=(mv_scale * mvRef[block_y][i4].y + 128)>>8;
				mv1[j4][i4].x=mv0[j4][i4].x - mvRef[block_y][i4].x ;
				mv1[j4][i4].y=mv0[j4][i4].y - mvRef[block_y][i4].y ;
			}
			//tmpMV1 = refList == 0 ? &L1_mv0[block_y+1][i4] : &L1_mv1[block_y+1][i4];

			if(step2 == 1)
			{
				int block_y2 = block_y;
				if(iTRp)
				{
					i4++;
					mv0[j4][i4].x=(mv_scale * mvRef[block_y2][i4].x + 128)>>8;
					mv0[j4][i4].y=(mv_scale * mvRef[block_y2][i4].y + 128)>>8;
					mv1[j4][i4].x=mv0[j4][i4].x - mvRef[block_y2][i4].x ;
					mv1[j4][i4].y=mv0[j4][i4].y - mvRef[block_y2][i4].y ;
					j4++;i4--;block_y2++;
					mv0[j4][i4].x=(mv_scale * mvRef[block_y2][i4].x + 128)>>8;
					mv0[j4][i4].y=(mv_scale * mvRef[block_y2][i4].y + 128)>>8;
					mv1[j4][i4].x=mv0[j4][i4].x - mvRef[block_y2][i4].x ;
					mv1[j4][i4].y=mv0[j4][i4].y - mvRef[block_y2][i4].y ;
					i4++;
					mv0[j4][i4].x=(mv_scale * mvRef[block_y2][i4].x + 128)>>8;
					mv0[j4][i4].y=(mv_scale * mvRef[block_y2][i4].y + 128)>>8;
					mv1[j4][i4].x=mv0[j4][i4].x - mvRef[block_y2][i4].x ;
					mv1[j4][i4].y=mv0[j4][i4].y - mvRef[block_y2][i4].y ;
				}
				else
				{
					i4++;
					mv0[j4][i4].x = mvRef[block_y2][i4].x;
					mv0[j4][i4].y = mvRef[block_y2][i4].y;
					j4++;i4--;block_y2++;
					mv0[j4][i4].x = mvRef[block_y2][i4].x;
					mv0[j4][i4].y = mvRef[block_y2][i4].y;
					i4++;
					mv0[j4][i4].x = mvRef[block_y2][i4].x;
					mv0[j4][i4].y = mvRef[block_y2][i4].y;
				}
			}

		}
		if(mbType==BSKIP_DIRECT&&step2==2)
		{
			if(i==0)
			{
				mvRef1.mv0 = *(avdUInt32*)(&mv0[j4][i4]);
				mvRef1.mv1 =  *(avdUInt32*)(&mv1[j4][i4]);
				mvRef1.ref0 = ref_idx0[j8][i8];
				mvRef1.ref1 = ref_idx1[j8][i8];
			}
			else
			{
				if(mvRef1.mv0 == *(avdUInt32*)(&mv0[j4][i4])
					&&mvRef1.mv1 == *(avdUInt32*)(&mv1[j4][i4])
					&&mvRef1.ref0 == ref_idx0[j8][i8]
				&&mvRef1.ref1 == ref_idx1[j8][i8])
				{
					all4InOne++;
				}

			}

		}
		j4 = j4_backup;
		i4 = block_x;
		if(step2>1)	
			for (jj = 0; jj < step2; jj++){
				for (ii = (jj ? 0 : 1); ii < step2; ii++){
					mv0[j4+jj][i4+ii].x = mv0[j4][i4].x;
					mv0[j4+jj][i4+ii].y = mv0[j4][i4].y;
					if (iTRp){
						mv1[j4+jj][i4+ii].x = mv1[j4][i4].x;
						mv1[j4+jj][i4+ii].y = mv1[j4][i4].y;
					}
				}
			}
			b8Step[i]=step2;

			if(step2==4)
			{
				ref_idx0[j8+1][i8] =
					ref_idx0[j8][i8+1] =
					ref_idx0[j8+1][i8+1] =
					ref_idx0[j8][i8];

				ref_idx1[j8+1][i8] =
					ref_idx1[j8][i8+1] =
					ref_idx1[j8+1][i8+1] =
					ref_idx1[j8][i8];

				break;
			}
	}
	if(all4InOne==3)
	{
		b8Step[0]=4;
	}
}
#endif

#if FEATURE_WEIGHTEDPRED
void pred_weight_table(ImageParameters *img,Bitstream *currStream)
{
	Slice *currSlice = img->currentSlice;
	avdNativeInt luma_weight_flag_l0, luma_weight_flag_l1, chroma_weight_flag_l0, chroma_weight_flag_l1;
	avdNativeUInt i,j,t1,t2;
	avdInt32 defaultWeightLuma, defaultWeightChroma;
	TWeightInfo* weight = img->weightInfo;
	TWeightArray* wp_weight =  weight->wp_weight ;
	TWeightArray* wp_offset =  weight->wp_offset ;
	int chroma_format_idc	= img->active_sps->chroma_format_idc;
	weight->luma_log2_weight_denom = avd_ue_v (currStream);
	weight->wp_round_luma = weight->luma_log2_weight_denom ? 1<<(weight->luma_log2_weight_denom - 1): 0;

	weight->chroma_log2_weight_denom = avd_ue_v (currStream);
	weight->wp_round_chroma = weight->chroma_log2_weight_denom ? 1<<(weight->chroma_log2_weight_denom - 1): 0;
	defaultWeightLuma   = (1<<weight->luma_log2_weight_denom);
	defaultWeightChroma = (1<<weight->chroma_log2_weight_denom);
	for (i=0; i<img->num_ref_idx_l0_active; i++)
	{
		avdInt16* weight = (*wp_weight)[0][i];
		avdInt16* offset    = (*wp_offset)[0][i];
		luma_weight_flag_l0 = avd_u_1(currStream);

		if (luma_weight_flag_l0) {
			weight [0] = (avdInt16)avd_se_v (currStream);
			offset [0] = (avdInt16)avd_se_v (currStream);
		}
		else {
			weight [0] = defaultWeightLuma;
			offset   [0] = 0;
		}
#if DUMP_VERSION
		AvdLog(DUMP_SLICE,0,"ref=%d,luma:w=%d,o=%d\n",i,weight[0],weight[0]);
#endif//DUMP_VERSION
		if(chroma_format_idc)
		{
			chroma_weight_flag_l0 = avd_u_1 (currStream);
			//for (j=1; j<3; j++) {
			if (chroma_weight_flag_l0)
			{
				weight[1]  = (avdInt16)avd_se_v(currStream);
				offset[1] =	(avdInt16)avd_se_v(currStream);
				weight[2] = (avdInt16)avd_se_v(currStream);
				offset[2] =	(avdInt16)avd_se_v(currStream);
			}
			else
			{
				weight [1]  = weight [2]  = defaultWeightChroma;
				offset  [1]   = offset   [2] = 0;
			}

		}
#if DUMP_VERSION
		AvdLog(DUMP_SLICE,0,"ref=%d:w=(%d,%d,%d),o=(%d,%d,%d)\n",i,
			weight[0],weight[1],weight[2],
			offset[0],offset[1],offset[2]);
#endif//DUMP_VERSION

	}

	if (img->type == B_SLICE )
	{
		for (i=0; i<img->num_ref_idx_l1_active; i++)
		{

			avdInt16* weight = (*wp_weight)[1][i];
			avdInt16* offset    = (*wp_offset)[1][i];
			luma_weight_flag_l1 = avd_u_1(currStream);
			if (luma_weight_flag_l1)
			{
				weight[0] = (avdInt16)avd_se_v (currStream);
				offset[0] = (avdInt16)avd_se_v (currStream);
			}
			else
			{
				weight [0] = defaultWeightLuma;
				offset   [0] = 0;
			}
			if(chroma_format_idc)
			{
				chroma_weight_flag_l1 = avd_u_1 (currStream);

				if (chroma_weight_flag_l1)
				{
					weight [1] = (avdInt16)avd_se_v(currStream);
					offset [1] =	  (avdInt16)avd_se_v(currStream);
					weight [2] = (avdInt16)avd_se_v(currStream);
					offset [2] =	  (avdInt16)avd_se_v(currStream);
				}
				else
				{
					weight [1]  = weight [2]  = defaultWeightChroma;
					offset  [1]   = offset   [2] = 0;
				}
			}
#if DUMP_VERSION
			AvdLog(DUMP_SLICE,0,"ref1=%d:w=(%d,%d,%d),o=(%d,%d,%d)\n",i,
				weight[0],weight[1],weight[2],
				offset[0],offset[1],offset[2]);
#endif//DUMP_VERSION	
		}

	}    
}
#if DUMP_VERSION
#define DUMP_WP 1
#endif//DUMP_VERSION
void fill_wp_params(ImageParameters *img)
{
	avdInt32 pt, p0, x;
	avdNativeInt i, j, k, z, m, log_weight_denom;
	avdNativeInt comp;
	avdNativeInt bframe = (img->type==B_SLICE);
	avdNativeInt max_bwd_ref, max_fwd_ref;
	avdInt16 weight_denom;
	Slice *currSlice = img->currentSlice;
	TWeightInfo* weight = img->weightInfo;
	TWeightArray* wp_weight =  weight->wp_weight;
	TWeightArray* wp_offset =  weight->wp_offset;
	int luma_log2_weight_denom = weight->luma_log2_weight_denom ;
	int chroma_log2_weight_denom  = weight->chroma_log2_weight_denom;
	int currPoc = img->pocInfo->currPoc;
	int weighted_bipred_idc = img->active_pps->weighted_bipred_idc;
	//int times = img->active_sps->frame_mbs_only_flag==0?2:1;
	if (!bframe)
		return;

	// alloc only if it is necessary;
#if DUMP_WP
	AvdLog(DUMP_SLICE,0,"weightInfo(%d,%d,%d,%d)\n",weight->luma_log2_weight_denom,weight->chroma_log2_weight_denom,weight->wp_round_luma,weight->wp_round_chroma);
#endif//
	max_fwd_ref = img->num_ref_idx_l0_active;
	max_bwd_ref = img->num_ref_idx_l1_active;
	if (weighted_bipred_idc == 2) {//&&ioInfo->outNumber<MAX_REFERENCE_FRAMES

		weight->luma_log2_weight_denom = 5;
		weight->chroma_log2_weight_denom = 5;
		weight->wp_round_luma = 16;
		weight->wp_round_chroma = 16;
		//for (comp=0; comp<3; comp++) {

		for (i = 0; i < MAX_REFERENCE_PICTURES; i++) {
			(*wp_weight)[0][i][0] = (*wp_weight)[0][i][1] = (*wp_weight)[0][i][2] =	32;
			(*wp_offset)[0][i][0] = (*wp_offset)[0][i][1] = (*wp_offset)[0][i][2] =	0;
			(*wp_weight)[1][i][0] = (*wp_weight)[1][i][1] = (*wp_weight)[1][i][2] = 32;
			(*wp_offset)[1][i][0] = (*wp_offset)[1][i][1] = (*wp_offset)[1][i][2] = 0;

		}

	}
	//hxy2010:TBD
	max_fwd_ref = min(img->listXsize[0],max_fwd_ref);
	max_bwd_ref = min(img->listXsize[1],max_bwd_ref);
	for (i=0; i<max_fwd_ref; i++) {
		for (j=0; j<max_bwd_ref; j++) {
			k = i * MAX_REFERENCE_PICTURES + j;
			if (weighted_bipred_idc == 1) {
				weight->wbp_weight[0][0][k] =  (*wp_weight)[0][i][0];
				weight->wbp_weight[0][1][k] =  (*wp_weight)[0][i][1];
				weight->wbp_weight[0][2][k] =  (*wp_weight)[0][i][2];
				weight->wbp_weight[1][0][k] =  (*wp_weight)[1][j][0] ;
				weight->wbp_weight[1][1][k] =  (*wp_weight)[1][j][1] ;
				weight->wbp_weight[1][2][k] =  (*wp_weight)[1][j][2] ;//img->listX[1][j]->wp_weight[1][comp];
			}
			else// if (img->active_pps->weighted_bipred_idc == 2) 
			{
				int t1=32,t0=32;
				int poc0 = img->listX[LIST_0][i]->poc;
				pt = img->listX[LIST_1][j]->poc -poc0 ;
				if (!(pt == 0 || img->listX[LIST_1][j]->is_long_term || img->listX[LIST_0][i]->is_long_term))
				{
					p0 = currPoc - poc0;
					x = voH264IntDiv2(16384 + (pt>>1),pt);
					z = Clip3(-1024, 1023, (x*p0 + 32 )>>6);
					t1 = z >> 2;
					if (t1 > 128)
						t1 = 32;
					t0 = 64 - t1;
				}
				weight->wbp_weight[0][0][k] =  weight->wbp_weight[0][1][k] =weight->wbp_weight[0][2][k] = t0;//img->listX[0][i]->wp_weight[0][comp];
				weight->wbp_weight[1][0][k] =  weight->wbp_weight[1][1][k] =weight->wbp_weight[1][2][k] = t1;//img->listX[1][j]->wp_weight[1][comp];

			}
#if DUMP_WP
			AvdLog(DUMP_SLICE,0,"(%d,%d):(%d,%d,%d,%d,%d,%d)\n",i,j,weight->wbp_weight[0][0][k],
				weight->wbp_weight[0][1][k],
				weight->wbp_weight[0][2][k],
				weight->wbp_weight[1][0][k],
				weight->wbp_weight[1][1][k],
				weight->wbp_weight[1][2][k]);
	
#endif//
		}
	}
#if DUMP_VERSION
	for (i=0; i<img->num_ref_idx_l0_active; i++)
	{

		avdInt16* weight = (*wp_weight)[0][i];
		avdInt16* offset    = (*wp_offset)[0][i];
		AvdLog(DUMP_SLICE,0,"ref=%d:w=(%d,%d,%d),o=(%d,%d,%d)\n",i,
			weight[0],weight[1],weight[2],
			offset[0],offset[1],offset[2]);
	}
	for (i=0; i<img->num_ref_idx_l1_active; i++)
	{

		avdInt16* weight = (*wp_weight)[1][i];
		avdInt16* offset    = (*wp_offset)[1][i];
		AvdLog(DUMP_SLICE,0,"ref1=%d:w=(%d,%d,%d),o=(%d,%d,%d)\n",i,
			weight[0],weight[1],weight[2],
			offset[0],offset[1],offset[2]);
	}
#endif//DUMP_VERSION	
#if FEATURE_INTERLACE
	if (!GetMbAffFrameFlag(img))
		return;

	for (i=0; i<2*max_fwd_ref; i++)
	{
		for (j=0; j<2*max_bwd_ref; j++)
		{
			m = i * MAX_REFERENCE_PICTURES + j;
			for (comp = 0; comp<3; comp++)
			{
				for (k=2; k<6; k+=2)
				{
					//img->listX[k+0][i]->wp_weight[0][comp] = img->listX[0][i>>1]->wp_weight[0][comp];
					//img->listX[k+0][i]->wp_weight[1][comp] = img->listX[0][i>>1]->wp_weight[1][comp];
					//img->listX[k+1][i]->wp_weight[0][comp] = img->listX[0][i>>1]->wp_weight[0][comp];
					//img->listX[k+1][i]->wp_weight[1][comp] = img->listX[0][i>>1]->wp_weight[1][comp];
					(*wp_offset)[k+0][i][comp] = (*wp_offset)[0][i>>1][comp];
					(*wp_offset)[k+1][j][comp] = (*wp_offset)[1][j>>1][comp];
					log_weight_denom = (comp == 0) ? luma_log2_weight_denom : chroma_log2_weight_denom;
					if (weighted_bipred_idc == 1)
					{
						//img->wbp_weight[k+0][comp][m] =  img->listX[0][i>>1]->wp_weight[0][comp];
						//img->wbp_weight[k+1][comp][m] =  img->listX[1][j>>1]->wp_weight[1][comp];
						weight->wbp_weight[k+0][comp][m] =  (*wp_weight)[0][i>>1][comp];
						weight->wbp_weight[k+1][comp][m] =  (*wp_weight)[1][j>>1][comp];
					}
					else if (weighted_bipred_idc == 2)
					{
						pt = img->listX[k+LIST_1][j]->poc - img->listX[k+LIST_0][i]->poc;
						if (pt == 0 || img->listX[k+LIST_1][j]->is_long_term || img->listX[k+LIST_0][i]->is_long_term)
						{
							weight->wbp_weight[k+0][comp][m] =   32;
							weight->wbp_weight[k+1][comp][m] =   32;
						}
						else
						{
							TPOCInfo* pocInfo = img->pocInfo;
							p0 = ((k==2)?pocInfo->toppoc:pocInfo->bottompoc) - img->listX[k+LIST_0][i]->poc;
							x = voH264IntDiv2(16384 + (pt>>1),pt);
							z = Clip3(-1024, 1023, (x*p0 + 32 )>>6);
							weight->wbp_weight[k+1][comp][m] = z >> 2;
							weight->wbp_weight[k+0][comp][m] = 64 - weight->wbp_weight[k+1][comp][m];
							if (weight->wbp_weight[k+1][comp][m]<-64||weight->wbp_weight[k+1][comp][m] > 128)
							{
								weight->wbp_weight[k+1][comp][m] = 32;
								weight->wbp_weight[k+0][comp][m] = 32;
								//currSlice->wp_offset[k+0][i][comp] = 0;
								//currSlice->wp_offset[k+1][j][comp] = 0;
								(*wp_offset)[k+0][i][comp] = 
								(*wp_offset)[k+1][j][comp] = 0;
							}
						}
					}
				}
			}
		}
	}
	trace_overflow(img);
#endif//FEATURE_INTERLACE
}

#if defined(NEON) && defined(_LINUX_ANDROID)
#define USE_WP_NEON WP_ASM
#if USE_WP_NEON
#define  WP_NEON_8 1
#define  WP_NEON_16 1
#define  WP_NEON2_8 1
#define  WP_NEON2_16 1
#include <arm_neon.h>

#define DAMN_SR \
	switch (shift2)\
	{\
	case 0:\
	break;\
	case 1:\
	temp = vshrq_n_s16(temp,1);\
	break;\
	case 2:\
	temp = vshrq_n_s16(temp,2);\
	break;\
	case 3:\
	temp = vshrq_n_s16(temp,3);\
	break;\
	case 4:\
	temp = vshrq_n_s16(temp,4);\
	break;\
	case 5:\
	temp = vshrq_n_s16(temp,5);\
	break;\
	case 6:\
	temp = vshrq_n_s16(temp,6);\
	break;\
	case 7:\
	temp = vshrq_n_s16(temp,7);\
	break;\
	case 8:\
	temp = vshrq_n_s16(temp,8);\
	break;\
	default:\
	AvdLog2(LL_INFO,"error:the wp shift is great than 8 %d\n",shift2);\
	break;\
}
#define NEON_WP8_PREPARE(adjust,alpha_l0,wpoffset)\
	_alpha_l0  = vdupq_n_s16 (alpha_l0);\
	_adjust	   = vdupq_n_s16 (adjust);\
	_wpoffset  = vdupq_n_s16 (wpoffset);

#define NEON_WP8(src)\
	src1 =  vld1_u8 (src);\
	_src =  (int16x8_t)vmovl_u8(src1);\
	temp = vmulq_s16 (_src,_alpha_l0);\
	temp = vqaddq_s16(temp, _adjust);\
	DAMN_SR \
	temp = vqaddq_s16(temp, _wpoffset);\
	result = vqmovun_s16(temp);\
	vst1_u8 (src, result);

static void applyWeights_NEON(int  gbSizeY,int  gbSizeX,avdUInt8* src, int Pitch,int alpha_l0,int adjust,int  shift2,int wpoffset,avdUInt8* clip255)
{

	int sizeY = gbSizeY;
	int offset;// = Pitch - gbSizeX + 4;
	int16x8_t _alpha_l0;//  = vdupq_n_s16 (alpha_l0);
	int16x8_t _adjust;//	 = vdupq_n_s16 (adjust);
	int16x8_t _wpoffset;//  = vdupq_n_s16 (wpoffset);
	
	int16x8_t   temp;
	uint8x8_t   result;
	uint8x8_t	src1;//	 =  vld1_u8 (src);
	int16x8_t	_src;//	 =  (int16x8_t)vmovl_u8(src1);
	//const int   shift3 = (const int)shift2;

	if(gbSizeX==16)
	{
		offset = Pitch - 8;
		NEON_WP8_PREPARE(adjust,alpha_l0,wpoffset);
		do
		{  
#if WP_NEON_16			
			NEON_WP8(src);src+=8;
			NEON_WP8(src);src+=offset;
			NEON_WP8(src);src+=8;
			NEON_WP8(src);src+=offset;
			NEON_WP8(src);src+=8;
			NEON_WP8(src);src+=offset;
			NEON_WP8(src);src+=8;
			NEON_WP8(src);src+=offset;
			NEON_WP8(src);src+=8;
			NEON_WP8(src);src+=offset;
			NEON_WP8(src);src+=8;
			NEON_WP8(src);src+=offset;
			NEON_WP8(src);src+=8;
			NEON_WP8(src);src+=offset;
			NEON_WP8(src);src+=8;
			NEON_WP8(src);src+=offset;
			sizeY-=8;
#else//WP_NEON_16
			avdUInt32* src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			sizeY-=8;
#endif//WP_NEON_16
		}while(sizeY>0);
	}
	else if(gbSizeX==8)
	{
		offset = Pitch;
		NEON_WP8_PREPARE(adjust,alpha_l0,wpoffset);
		do
		{  
#if WP_NEON_8
			NEON_WP8(src);src+=offset;
			NEON_WP8(src);src+=offset;
			NEON_WP8(src);src+=offset;
			NEON_WP8(src);src+=offset;
			sizeY-=4;
#else//WP_NEON_8
			avdUInt32* src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			sizeY-=4;
#endif//WP_NEON_8
		}while(sizeY>0);
	}
	else if(gbSizeX==4)
	{
		offset = Pitch;
		do
		{  
			avdUInt32* src2 = (avdUInt32*)src;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			sizeY-=2;
		}while(sizeY>0);
	}
	else// if(gbSizeX==2)
	{
		offset = Pitch;
		do
		{  
			avdUInt16* src2 = (avdUInt32*)src;
			*src2 = COMBINE_2_BYTES_TO16(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset));
			src += offset;
			sizeY--;
		}while(sizeY>0);
	}

}
#define NEON_WP8_PREPARE2(adjust,alpha_fw,alpha_bw,wpoffset)\
	_alpha_fw  = vdupq_n_s16 (alpha_fw);\
	_alpha_bw  = vdupq_n_s16 (alpha_bw);\
	_adjust	   = vdupq_n_s16 (adjust);\
	_wpoffset  = vdupq_n_s16 (wpoffset);

#define NEON_WP8_2(src,mpr)\
	src1 =  vld1_u8 (src);\
	_src =  (int16x8_t)vmovl_u8(src1);\
	mpr1 =  vld1_u8 (mpr);\
	_mpr =  (int16x8_t)vmovl_u8(mpr1);\
	temp = vmulq_s16 (_src,_alpha_fw);\
	temp2 = vmulq_s16 (_mpr,_alpha_bw);\
	temp = vqaddq_s16(temp, temp2);\
	temp = vqaddq_s16(temp, _adjust);\
	DAMN_SR \
	temp = vqaddq_s16(temp, _wpoffset);\
	result = vqmovun_s16(temp);\
	vst1_u8 (src, result);
static void applyWeights2_NEON(int gbSizeY,int gbSizeX,avdUInt8* src,avdUInt8* mpr,int Pitch,int alpha_fw,int alpha_bw,int adjust, int shift2,int wpoffset,avdUInt8* clip255)
{
	int sizeY = gbSizeY;
	int offset1; 
	int offset2; 
	int16x8_t _alpha_fw;
	int16x8_t _alpha_bw;
	int16x8_t _adjust;
	int16x8_t _wpoffset;

	int16x8_t   temp,temp2;
	uint8x8_t   result;
	uint8x8_t	src1,mpr1;
	int16x8_t	_src,_mpr;
	if(gbSizeX==16)
	{
		offset1 = Pitch - 8;
		offset2 = 8;
		NEON_WP8_PREPARE2(adjust,alpha_fw,alpha_bw,wpoffset)
		do
		{  
#if WP_NEON2_16
			NEON_WP8_2(src,mpr);src+=8;mpr+=8;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			NEON_WP8_2(src,mpr);src+=8;mpr+=8;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			NEON_WP8_2(src,mpr);src+=8;mpr+=8;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			NEON_WP8_2(src,mpr);src+=8;mpr+=8;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			NEON_WP8_2(src,mpr);src+=8;mpr+=8;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			NEON_WP8_2(src,mpr);src+=8;mpr+=8;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			NEON_WP8_2(src,mpr);src+=8;mpr+=8;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			NEON_WP8_2(src,mpr);src+=8;mpr+=8;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			gbSizeY-=8;
#else//WP_NEON2_16
			avdUInt32* src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			gbSizeY-=8;
#endif//WP_NEON2_16
		}while(gbSizeY>0);
	}
	else if(gbSizeX==8)
	{
		offset1 = Pitch;
		offset2 = MB_BLOCK_SIZE;
		NEON_WP8_PREPARE2(adjust,alpha_fw,alpha_bw,wpoffset)
		do
		{ 
#if WP_NEON2_8
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			NEON_WP8_2(src,mpr);src+=offset1;mpr += offset2;
			gbSizeY-=4;
#else//WP_NEON2_8
			avdUInt32* src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;mpr += offset2;

			gbSizeY-=4;
#endif//WP_NEON2_8
		}while(gbSizeY>0);
	}
	else if(gbSizeX==4)
	{
		offset1 = Pitch;
		offset2 = MB_BLOCK_SIZE;
		do
		{ 
			avdUInt32* src2 = (avdUInt32*)src;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src  += offset1;mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src  += offset1;mpr += offset2;
			gbSizeY-=2;
		}while(gbSizeY>0);
	}
	else//if(gbSizeX==2)
	{
		offset1 = Pitch;
		offset2 = MB_BLOCK_SIZE;
		do
		{ 
			avdUInt16* src2 = (avdUInt32*)src;
			*src2 = COMBINE_2_BYTES_TO16(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset));
			src  += offset1;mpr += offset2;
		}while(--gbSizeY);
	}

}


#endif//NEON && LINUX_ANDROID
#endif//USE_WP_NEON
static void applyWeights_c(int  gbSizeY,int  gbSizeX,avdUInt8* src, int Pitch,int alpha_l0,int adjust, int shift2,int wpoffset,avdUInt8* clip255)
{

	int sizeY = gbSizeY;
	int offset;// = Pitch - gbSizeX + 4;
#if USE_WP_NEON
	return applyWeights_NEON(gbSizeY,gbSizeX,src,Pitch,alpha_l0,adjust, shift2,wpoffset,clip255);
#endif
	if(gbSizeX==16)
	{
		offset = Pitch - 12;
		do
		{  
			avdUInt32* src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			sizeY-=8;
		}while(sizeY>0);
	}
	else if(gbSizeX==8)
	{
		offset = Pitch - 4;
		do
		{  
			avdUInt32* src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src+=4;	
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			sizeY-=4;
		}while(sizeY>0);
	}
	else if(gbSizeX==4)
	{
		offset = Pitch;
		do
		{  
			avdUInt32* src2 = (avdUInt32*)src;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			src2 = (avdUInt32*)src;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
			src += offset;
			sizeY-=2;
		}while(sizeY>0);
	}
	else// if(gbSizeX==2)
	{
		offset = Pitch;
		do
		{  
			avdUInt16* src2 = (avdUInt32*)src;
			*src2 = COMBINE_2_BYTES_TO16(iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset));
			src += offset;
			sizeY--;
		}while(sizeY>0);
	}

}

static void applyWeights2_C(int gbSizeY,int gbSizeX,avdUInt8* src,avdUInt8* mpr,int Pitch,int alpha_fw,int alpha_bw,int adjust, int shift2,int wpoffset,avdUInt8* clip255)
{
	int sizeY = gbSizeY;
	int offset1; 
	int offset2; 
#if USE_WP_NEON
	return applyWeights2_NEON(gbSizeY,gbSizeX,src,mpr,Pitch,alpha_fw,alpha_bw,adjust, shift2,wpoffset,clip255);
#endif//USE_WP_NEON
	if(gbSizeX==16)
	{
		offset1 = Pitch - 12;
		offset2 = MB_BLOCK_SIZE-12;
		do
		{  

			avdUInt32* src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2   = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;
			mpr += offset2;
			gbSizeY-=8;
		}while(gbSizeY>0);
	}
	else if(gbSizeX==8)
	{
		offset1 = Pitch - 4;
		offset2 = MB_BLOCK_SIZE-4;
		do
		{ 
			avdUInt32* src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2++ = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src+=4;mpr+=4;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src += offset1;mpr += offset2;

			gbSizeY-=4;
		}while(gbSizeY>0);
	}
	else if(gbSizeX==4)
	{
		offset1 = Pitch;
		offset2 = MB_BLOCK_SIZE;
		do
		{ 
			avdUInt32* src2 = (avdUInt32*)src;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src  += offset1;mpr += offset2;
			src2 = (avdUInt32*)src;
			*src2 = COMBINE_4_BYTES_TO32(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));
			src  += offset1;mpr += offset2;
			gbSizeY-=2;
		}while(gbSizeY>0);
	}
	else//if(gbSizeX==2)
	{
		offset1 = Pitch;
		offset2 = MB_BLOCK_SIZE;
		do
		{ 
			avdUInt16* src2 = (avdUInt32*)src;
			*src2 = COMBINE_2_BYTES_TO16(iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset));
			src  += offset1;mpr += offset2;
		}while(--gbSizeY);
	}

}

void applyWeights(ImageParameters* img,TMBsProcessor* info,int pred_dir,int ref_idx,int fw_refframe,
				  int j4,
				  int i4,int gbSizeX,int gbSizeY)
{
	int alpha_l0 ;
	int wpoffset ;
	avdUInt8* srcUV;
	StorablePicture	*dec_picture = img->dec_picture;
	Slice *currSlice = img->currentSlice;
	TWeightInfo* weight = img->weightInfo;
	TWeightArray* wp_weight =  weight->wp_weight;
	TWeightArray* wp_offset =  weight->wp_offset;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	int Pitch = sizeInfo->yPlnPitch;
	avdUInt8* src = dec_picture->plnY + ((Pitch * j4 + i4)<<2);
	int jj,ii,yTmp,xTmp,uv;
	TCLIPInfo	*clipInfo = img->clipInfo;
	avdUInt8* clip255 = clipInfo->clip255;
	int  adjust = weight->wp_round_luma;
	int  shift2   = weight->luma_log2_weight_denom;
	TPosInfo*  posInfo  = GetPosInfo(info);
	int start;
	int  sizeX,sizeY,offset=Pitch-gbSizeX;
	alpha_l0  = (*wp_weight)[pred_dir][ref_idx][0];
	wpoffset = (*wp_offset)[pred_dir][fw_refframe][0];
	TIME_BEGIN(start)

	applyWeights_c(gbSizeY,gbSizeX,src,Pitch,alpha_l0,adjust, shift2,wpoffset,clip255);
	yTmp = (j4<<1);
	xTmp = (i4<<1);
	adjust = weight->wp_round_chroma;
	shift2   = weight->chroma_log2_weight_denom;
	gbSizeY>>=1;
	gbSizeX>>=1;

	Pitch>>=1;
	offset=Pitch-gbSizeX;
	for(uv=0;uv<2;uv++){
		src = uv == 0 ? dec_picture->plnU : dec_picture->plnV;
		src += yTmp * Pitch + xTmp;
		alpha_l0  = (*wp_weight)[pred_dir][ref_idx][uv+1];
		wpoffset = (*wp_offset)[pred_dir][fw_refframe][uv+1];
		applyWeights_c(gbSizeY,gbSizeX,src,Pitch,alpha_l0,adjust, shift2,wpoffset,clip255);
	}
	TIME_END(start,weightSum)
}


void applyWeights2(ImageParameters* img,TMBsProcessor* info,int fw_refframe,int bw_refframe,
				   avdUInt8* src,
				   int idx,
				   int list_offset,
				   int gbSizeX ,int gbSizeY,int weightShift)
{

	int wt_list_offset,ii,jj;
	StorablePicture	*dec_picture = img->dec_picture;
	Slice *currSlice = img->currentSlice;
	TWeightInfo* weight = img->weightInfo;
	//TWeightArray* wp_weight =  weight->wp_weight;
	TWeightArray* wp_offset =  weight->wp_offset;
	int vec1_x,vec1_y,vec2_y;
	int shift=idx>0;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	int start;
	int Pitch	= sizeInfo->yPlnPitch>>shift;
	int log2_weight_denom = idx==0? weight->luma_log2_weight_denom:weight->chroma_log2_weight_denom;

	gbSizeY = gbSizeY>>shift;
	gbSizeX = gbSizeX>>shift;
	TIME_BEGIN(start)
	if(weightShift)
	{
		fw_refframe>>=1;
		bw_refframe>>=1;
	}
	//if (1)
	{
		avdNativeInt alpha_fw, alpha_bw;
		int wpoffset;
		int adjust = 1<<log2_weight_denom;
		avdUInt8* clip255 = img->clipInfo->clip255;
		avdUInt8* mpr      = (avdUInt8*)info->mbsParser->mpr;
		int shift2 = log2_weight_denom+1;
		//int sizeY = gbSizeY;
		int sizeX;
		wt_list_offset = (img->active_pps->weighted_bipred_idc==2)? list_offset : 0;

		ii = fw_refframe * MAX_REFERENCE_PICTURES + bw_refframe;
		alpha_fw = weight->wbp_weight[0+wt_list_offset][idx][ii];
		alpha_bw = weight->wbp_weight[1+wt_list_offset][idx][ii];
		wpoffset = ((*wp_offset)[0+wt_list_offset][fw_refframe][idx]+(*wp_offset)[1+wt_list_offset][bw_refframe][idx] + 1)>>1;
		applyWeights2_C(gbSizeY,gbSizeX,src,mpr,Pitch,alpha_fw,alpha_bw,adjust, shift2,wpoffset,clip255);

	}
	TIME_END(start,weightSum)
}
#endif //FEATURE_WEIGHTEDPRED
void init_decoding_engine_IPCM(ImageParameters *img)
{
#if FEATURE_CABAC
	Slice *currSlice = img->currentSlice;
	Bitstream *currStream;
	avdNativeInt PartitionNumber;
	avdNativeInt i;

	
	PartitionNumber=1;
	
	for(i=0;i<PartitionNumber;i++)
	{
		currStream = currSlice->partArr[i].bitstream;
		StartAriDeco(currStream);
	}
#endif
}

/*!
************************************************************************
* \brief
*    Read IPCM pcm_alignment_zero_bit and pcm_byte[i] from stream to GetMBCofBuf(info)
*    (for IPCM CABAC and IPCM CAVLC  28/11/2003)
*
* \author
*    Dong Wang <Dong.Wang@bristol.ac.uk>
************************************************************************
*/

void readIPCMcoeffsFromNAL_CAVLC(ImageParameters *img,TMBsProcessor *info,Bitstream *currStream)
{
	SyntaxElement currSE;
	avdNativeInt i,j;
	TMBBitStream	*mbBits = GetMBBits(info->currMB);
	//For CABAC, we don't need to read bits to let stream avdUInt8 aligned
	i = (currStream->bBitToGo&7);//fixed_MP
	if (i)
		FlushBits(currStream, i);

	{ 

		//read bits to let stream avdUInt8 aligned


		//read luma and chroma IPCM coefficients
		currSE.len=8;

		for(i=0;i<16;i++)
			for(j=0;j<16;j++)
			{
				//readSyntaxElement_FLC(&currSE, dP->bitstream);
				GetMBCofBuf(info)[(inv_decode_scan[((i>>2)<<2) + (j>>2)]<<4)|((i&3)<<2)|(j&3)]=(avdNativeUInt)GetBits(currStream, 8);
			}

			for(i=0;i<8;i++)
				for(j=0;j<8;j++)
				{
					//readSyntaxElement_FLC(&currSE, dP->bitstream);
					GetMBCofBuf(info)[(16<<4)+((((i>>2)<<1) + (j>>2))<<4)|((i&3)<<2)|(j&3)]=(avdNativeUInt)GetBits(currStream, 8);
				}

				for(i=0;i<8;i++)
					for(j=0;j<8;j++)
					{
						//readSyntaxElement_FLC(&currSE, dP->bitstream);
						GetMBCofBuf(info)[(16<<4)+(8<<3)+((((i>>2)<<1) + (j>>2))<<4)|((i&3)<<2)|(j&3)]=(avdNativeUInt)GetBits(currStream, 8);
					}
	}
}



/*!
************************************************************************
* \brief
*    Copy IPCM coefficients to decoded picture buffer and set parameters for this MB
*    (for IPCM CABAC and IPCM CAVLC  28/11/2003)
*
* \author
*    Dong Wang <Dong.Wang@bristol.ac.uk>
************************************************************************
*/

void decode_ipcm_mb(ImageParameters *img,TMBsProcessor *info)
{

	TSizeInfo	*sizeInfo = img->sizeInfo;
	avdNativeInt yPitch = sizeInfo->yPlnPitch;
	avdNativeInt uvPitch = (sizeInfo->yPlnPitch>>1);
	TPosInfo*  posInfo  = GetPosInfo(info);


	StorablePicture	*dec_picture = img->dec_picture;
	avdUInt8 *plnY = &dec_picture->plnY[posInfo->pix_y * sizeInfo->yPlnPitch + posInfo->pix_x];
	avdUInt8 *plnU = &dec_picture->plnU[posInfo->pix_c_y * uvPitch + posInfo->pix_c_x];
	avdUInt8 *plnV = &dec_picture->plnV[posInfo->pix_c_y * uvPitch + posInfo->pix_c_x];

	avdNativeInt i,j;
	Macroblock *currMb = info->currMB;
	TMBBitStream	*mbBits = GetMBBits(info->currMB);
	//Copy coefficents to decoded picture buffer
	//IPCM coefficents are stored in GetMBCofBuf(info) which is set in function readIPCMcoeffsFromNAL()
	for(i=0;i<16;i++){
		for(j=0;j<16;j++)
			plnY[j] = (avdUInt8)GetMBCofBuf(info)[(inv_decode_scan[((i>>2)<<2) + (j>>2)]
		<<4)|((i&3)<<2)|(j&3)];
		plnY += yPitch;
	}		
	for(i=0;i<8;i++){
		for(j=0;j<8;j++)
			plnU[j]
		=(avdUInt8)GetMBCofBuf(info)[(16<<4)+((((i>>2)<<1) + (j>>2))<<4)
			|((i&3)<<2)|(j&3)];
		plnU += uvPitch;
	}	
	for(i=0;i<8;i++){
		for(j=0;j<8;j++)
			plnV[j]
		=(avdUInt8)GetMBCofBuf(info)[(16<<4)+(8<<3)+((((i>>2)<<1) + 
			(j>>2))<<4)|((i&3)<<2)|(j&3)];
		plnV += uvPitch;
	}	

	//For Deblocking Filter  16/08/2003
	if (mbGetMbType(currMb) == IPCM){
		mbSetQP(currMb, 0);
		// TBD: check this with Reference code???
		//For CABAC decoding of MB skip flag 
		if (!IsVLCCoding(img->active_pps))
			mbUnMarkSkipped(currMb);
		//for Loop filter CABAC
		//currMb->cbp_blk=(avdInt16)0xFFFF;
		mbMarkCBPBlk(currMb);
	}




#if FEATURE_CABAC
	//For CABAC decoding of Dquant
	img->cabacInfo->last_dquant=0;
#endif //VOI_H264D_NON_BASELINE
}


