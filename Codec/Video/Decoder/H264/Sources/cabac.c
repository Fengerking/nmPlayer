
#if !BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#include <string.h>
#endif
#include "global.h"
#if FEATURE_CABAC

//#include "H264_C_Type.h"
#include "avd_neighbor.h"
#include "cabac.h"
#include "memalloc.h"

#include "elements.h"
#include "image.h"
#include "biaridecod.h"
//#include "mb_access.h"
#include "vlc.h"

#if TRACE
avdNativeUInt symbolCount = 0;
#endif //TRACE

#include "biaridecod.h"
#include "vlc.h"
#if DUMP_VERSION
static int biCount=0;
#endif
#include "macroblock.h"
#if defined(ARM_ASM)&&defined(LINUX)&&!defined(RVDS)
#ifndef ARMv4
#define ENABLE_GCC_INLINE_OPT 1
#endif//ARMv4
#endif//ARM_ASM
void NeighborAB_Inference_CABAC2(ImageParameters *img,TMBsProcessor* info,
								 MacroBlock* currMB,
								 avdNativeInt mb_x,
								 avdNativeInt mb_y,
								 MacroBlock* nMBs[2]);

static __inline avdNativeUInt biari_decode_symbol(Bitstream *dep, TMBsProcessor* info,BiContextTypePtr bi_ct)
{

	avdNativeUInt bit = bi_ct->MPS;
	avdNativeUInt Drange = dep->Drange;
	avdNativeUInt Dvalue = dep->Dvalue;
	avdNativeUInt state  = bi_ct->state;
	avdNativeUInt rLPS = avd_rLPS_table_64x4[state][(Drange>>6) & 0x03];
#if DUMP_VERSION
	AvdLog(DUMP_SLICE,DUMP_DCORE"\nBDS%d:state=%d,Drange=%d\n",biCount++,state,Drange);
#endif

	Drange -= rLPS;
	if (Dvalue < Drange) /* MPS */ 
		state = avd_AC_next_state_MPS_64[state]; // next state
	else{						  /* LPS */
		Dvalue -= Drange;
		Drange = rLPS;
		bit ^= 1;
		if (!state)			 // switch meaning of MPS if necessary	
			bi_ct->MPS ^= 1;              
		state = avd_AC_next_state_LPS_64[state]; // next state 
	}
	if (Drange < QUARTER){
		//for version above ARMv5E, use CLZ to optimize it
		avdUInt32 shift;
#if ENABLE_GCC_INLINE_OPT
		avdUInt32 tmp3;
		tmp3 = (Drange<<24)&0xff000000;//only clr the first byte
		asm volatile( 
			"CLZ  %[shift], %[tmp3] \n" 
			:[shift] "=r" (shift): [tmp3] "r" (tmp3)
			); 
		shift+=1;
#else//ENABLE_GCC_INLINE_OPT
		shift = avdNumLeadingZerosPlus1[Drange];
#endif//ENABLE_GCC_INLINE_OPT
		Drange <<= shift;
		Dvalue = (Dvalue<<shift) | (avdUInt16)GetBits(dep, shift);
	}

	dep->Drange = Drange;
	dep->Dvalue = Dvalue;
	bi_ct->state = state;
	return(bit);

}
/***********************************************************************
 * L O C A L L Y   D E F I N E D   F U N C T I O N   P R O T O T Y P E S
 ***********************************************************************
 */
avdNativeUInt unary_bin_decode(Bitstream *dep_dp,TMBsProcessor* info,
                              BiContextTypePtr ctx,
                              avdNativeInt ctx_offset);


avdNativeUInt unary_bin_max_decode(Bitstream *dep_dp,TMBsProcessor* info,
                                  BiContextTypePtr ctx,
                                  avdNativeInt ctx_offset,
                                  avdNativeUInt max_symbol);

avdNativeUInt unary_exp_golomb_level_decode( Bitstream *dep_dp,TMBsProcessor* info,
                                            BiContextTypePtr ctx);

avdNativeUInt unary_exp_golomb_mv_decode(Bitstream *dep_dp,TMBsProcessor* info,
                                        BiContextTypePtr ctx,
                                        avdNativeUInt max_bin);
//#define arideco_bits_read(dep) ((*dep->Dcodestrm_len-1)<<3) + (31-dep->Dbits_to_go) - 16

void cabac_new_slice(ImageParameters *img)
{
	img->cabacInfo->last_dquant=0;
}

/*!
 ************************************************************************
 * \brief
 *    Allocation of contexts models for the motion info
 *    used for arithmetic decoding
 *
 ************************************************************************
 */
//TBD: may move this to img if multiple slices in a frame is not supported; 
MotionInfoContexts* create_contexts_MotionInfo(ImageParameters *img)
{
	MotionInfoContexts *deco_ctx;
	deco_ctx = (MotionInfoContexts*) voH264AlignedMalloc(img,70, sizeof(MotionInfoContexts));
	if( deco_ctx == NULL )
		no_mem_exit("create_contexts_MotionInfo: deco_ctx");

	return deco_ctx;
}

/*!
 ************************************************************************
 * \brief
 *    Allocates of contexts models for the texture info
 *    used for arithmetic decoding
 ************************************************************************
 */
TextureInfoContexts* create_contexts_TextureInfo(ImageParameters *img)
{
	TextureInfoContexts *deco_ctx;
	deco_ctx = (TextureInfoContexts*) voH264AlignedMalloc(img,71, sizeof(TextureInfoContexts) );
	if( deco_ctx == NULL )
		no_mem_exit("create_contexts_TextureInfo: deco_ctx");

	return deco_ctx;
}

/*!
 ************************************************************************
 * \brief
 *    Frees the memory of the contexts models
 *    used for arithmetic decoding of the motion info.
 ************************************************************************
 */
void delete_contexts_MotionInfo(ImageParameters *img,MotionInfoContexts *deco_ctx)
{
	if( deco_ctx == NULL )
		return;
	SafevoH264AlignedFree(img, deco_ctx );
	return;
}

/*!
 ************************************************************************
 * \brief
 *    Frees the memory of the contexts models
 *    used for arithmetic decoding of the texture info.
 ************************************************************************
 */
void delete_contexts_TextureInfo(ImageParameters *img,TextureInfoContexts *deco_ctx)
{
	if( deco_ctx == NULL )
		return;
	SafevoH264AlignedFree(img, deco_ctx );
	return;
}

/*!
 ************************************************************************
 * \brief
 *    This function is used to arithmetically decode the motion
 *    vector data of a B-frame MB.
 ************************************************************************
 */
void readMVD_CABAC( ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,
                    Bitstream *dep_dp)
{
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	avdNativeInt i = posInfo->subblock_x;
	avdNativeInt j = posInfo->subblock_y;
	avdNativeInt a, b, k5, act_sym, act_ctx;
	avdNativeInt mv_local_err, isFram;
	avdNativeInt mv_sign;
	avdNativeInt list_idx = se->value2 & 0x01;
	avdNativeInt k = (se->value2>>1); // MVD component
	MotionInfoContexts *ctx = img->currentSlice->mot_ctx;
	Macroblock *currMB = info->currMB;
	MacroBlock	*mbInfo;
	//AVCNeighbor4x4Boundary *mbBdry;
	//TMBsProcessor *info  = img->mbsProcessor;
	TCABACInfo	*cabacInfo = img->cabacInfo;
	int					leftMBYForIntra = -1;
	k5 = mbIsMbField(currMB);

	if (j){
		AVDMotionVector* mvd= &cabacInfo->mvd[list_idx][j-1][i];
		b = (k == 0) ? mvd->x : mvd->y;
		b = absm(b);
	}
	else {

		mbInfo = GetUpMB(info);

		if (mbInfo){
			avdInt8* mvd= (avdInt8*)&niGetCabacNeighborStruct(mbInfo)->mvd[list_idx][i];
			b = (k == 0) ?mvd[0] : mvd[1];
			b = absm(b);
#if FEATURE_INTERLACE
			if (GetMbAffFrameFlag(img) && b && k==1){
				isFram = !mbIsMbField(mbInfo);//!IsUpField2(info,currMB,mbInfo);//(IsNeighborBIsSelf(info))? !mbIsMbField(currMB):!IsUpField(mbInfo);
				if (!k5 && !isFram)
					b <<= 1;
				else if (k5 && isFram)
					b >>= 1;
			}
#endif//FEATURE_INTERLACE
		}
		else
			b = 0;
	}

	if (i){
		AVDMotionVector* mvd= & cabacInfo->mvd[list_idx][j][i-1];
		a = (k == 0) ? mvd->x : mvd->y;
		a = absm(a);
	}
	else {
		leftMBYForIntra = j*4;
		mbInfo = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,j*4,16,&leftMBYForIntra):GetLeftMB(info);
		if (mbInfo){
			avdInt8* mvd; 	
			int yOffset = leftMBYForIntra;
			act_ctx = (yOffset&0xf)>>2;
			mvd = (avdInt8*)(&niGetCabacNeighborStruct(mbInfo)->mvdLeft[list_idx][act_ctx]);
			a = (k == 0) ? mvd[0] : mvd[1];
			a = absm(a);
#if FEATURE_INTERLACE
			if (GetMbAffFrameFlag(img) && a && k==1){
				isFram = !mbIsMbField(mbInfo);
				if (!k5 && !isFram)
					a <<= 1;
				else if (k5 && isFram)
					a >>= 1;
			}
#endif//FEATURE_INTERLACE
		}
		else
			a = 0;
	}

	k5 = k + (k<<2);
	if ((mv_local_err=a+b)<3)
		act_ctx = k5;
	else
	{
		if (mv_local_err>32)
			act_ctx = k5 + 3;
		else
			act_ctx = k5 + 2;
	}
	se->context = act_ctx;
	act_sym = biari_decode_symbol(dep_dp,info,&ctx->mv_res_contexts[0][act_ctx]);
	if (act_sym)
	{
		act_sym = unary_exp_golomb_mv_decode(dep_dp,info,ctx->mv_res_contexts[1]+k5,3);
		act_sym++;
		mv_sign = biari_decode_symbol_eq_prob(dep_dp);
		if(mv_sign)
			act_sym = -act_sym;
	}
	se->value1 = act_sym;

}


/*!
 ************************************************************************
 * \brief
 *    This function is used to arithmetically decode the 8x8 block type.
 ************************************************************************
 */
void readB8_typeInfo_CABAC (ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,
                            Bitstream *dep_dp)
{
	MotionInfoContexts *ctx = (img->currentSlice)->mot_ctx;
	avdNativeInt act_sym;
	avdNativeInt bframe  = (img->type==B_SLICE);
	
	
	if (!bframe)
	{
		BiContextType* c0 = ctx->b8_type_contexts[0];
		if (biari_decode_symbol(dep_dp,info, c0+1))
			act_sym = 0;
		else
		{
			if (biari_decode_symbol(dep_dp,info, c0+3))
			{
				if (biari_decode_symbol(dep_dp,info, c0+4)) 
					act_sym = 2;
				else                                                            
					act_sym = 3;
			}
			else
				act_sym = 1;
		}
	}
	else
	{

		BiContextType* c1 = ctx->b8_type_contexts[1];
		BiContextType* c13 = &ctx->b8_type_contexts[1][3];
		if (biari_decode_symbol(dep_dp,info, c1))
		{
			if (biari_decode_symbol(dep_dp,info, c1+1))
			{
				if (biari_decode_symbol(dep_dp,info, c1+2))
				{
					if (biari_decode_symbol(dep_dp,info, c13))
					{
						act_sym = 10;
						if (biari_decode_symbol(dep_dp,info, c13)) 
							act_sym++;
					}
					else
					{
						act_sym = 6;
						if (biari_decode_symbol(dep_dp,info, c13)) 
							act_sym+=2;
						if (biari_decode_symbol(dep_dp,info, c13)) 
							act_sym++;
					}
				}
				else
				{
					act_sym=2;
					if (biari_decode_symbol(dep_dp,info, c13)) 
						act_sym+=2;
					if (biari_decode_symbol(dep_dp,info, c13)) 
						act_sym+=1;
				}
			}
			else
			{
				act_sym = biari_decode_symbol(dep_dp,info, c13)
					? 1 : 0;
			}
			act_sym++;
		}
		else
			act_sym= 0;
	}
	se->value1 = act_sym;

}

/*!
 ************************************************************************
 * \brief
 *    This function is used to arithmetically decode the macroblock
 *    type info of a given MB.
 ************************************************************************
 */
void readMB_skip_flagInfo_CABAC( ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,
                                 Bitstream *dep_dp)
{
	MotionInfoContexts *ctx = (img->currentSlice)->mot_ctx;
	Macroblock *currMB = info->currMB;
	avdNativeInt a, b;
	avdNativeInt bframe=(img->type==B_SLICE);
	//TMBsProcessor *info  = img->mbsProcessor;
	//avdInt16			*mbAddr = info->inferredABAddress;
	int					leftMBYForIntra = 0;
	MacroBlock	*mbAInfo, *mbBInfo;
	//MacroBlock **nMBs = &info->neighborABCD[0];
	mbAInfo = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,0,16,&leftMBYForIntra):GetLeftMB(info);
	mbBInfo = GetUpMB(info);

	a = INSAMESLICE(currMB,mbAInfo) ? !(mbIsSkipped(mbAInfo)): 0 ;
	b = INSAMESLICE(currMB,mbBInfo) ? !(mbIsSkipped(mbBInfo)): 0 ;

	if (bframe)
	{
		if (biari_decode_symbol(dep_dp,info, &ctx->mb_type_contexts[2][7 + a + b]) == 1)
			se->value1 = se->value2 = 0;
		else
			se->value1 = se->value2 = 1;
	}
	else
	{
		se->value1 = (biari_decode_symbol(dep_dp,info, &ctx->mb_type_contexts[1][a + b]) == 1)
			? 0 : 1;
	}
	// be careful, is_skipped = 1 for skip; in reference code skip_flag = 0 for skip;
	se->value1 ? mbUnMarkSkipped(currMB) : mbMarkSkipped(currMB);


	if (!se->value1)
		img->cabacInfo->last_dquant=0;
	return;
}
/*!
 ************************************************************************
 * \brief
 *    This function is used to arithmetically decode the macroblock
 *    type info of a given MB.
 ************************************************************************
 */
void readMB_typeInfo_CABAC( ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,
                            Bitstream *dep_dp)
{
	MotionInfoContexts *ctx = (img->currentSlice)->mot_ctx;
	avdNativeInt a, b;
	avdNativeInt act_ctx;
	avdNativeInt act_sym;
	avdNativeInt bframe=(img->type==B_SLICE);
	avdNativeInt mode_sym;
	avdNativeInt curr_mb_type;
	TMBBitStream	*mbBits = GetMBBits(info->currMB);
	MacroBlock	*mbAInfo, *mbBInfo;
	avdNativeInt					aType, bType;

	int					leftMBYForIntra = -1;
	mbAInfo = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,0,16,&leftMBYForIntra):GetLeftMB(info);
	mbBInfo = GetUpMB(info);
	if (mbAInfo){
		aType = mbGetMbType(mbAInfo);
	}
	if (mbBInfo){
		bType = mbGetMbType(mbBInfo);
	}

	
	

	if(img->type == I_SLICE)  // INTRA-frame
	{
		BiContextTypePtr c0 = ctx->mb_type_contexts[0];

		b = (!mbBInfo || bType == I4MB||bType == I8MB) ? 0 : 1;
		a = (!mbAInfo || aType == I4MB||aType == I8MB) ? 0 : 1;

		act_ctx = a + b;
		act_sym = biari_decode_symbol(dep_dp,info, c0 + act_ctx);
		se->context = act_ctx; // store context
		if (act_sym==0) // 4x4 Intra
			curr_mb_type = act_sym;
		else // 16x16 Intra
		{
			mode_sym = biari_decode_final(dep_dp);
			if(mode_sym == 1)
				curr_mb_type = 25;
			else
			{
				act_sym = 1;
				mode_sym =  biari_decode_symbol(dep_dp,info, c0
					+ 4 ); // decoding of AC/no AC
				act_sym += (mode_sym<<4) - (mode_sym<<2); // * 12;
				// decoding of cbp: 0,1,2
				mode_sym =  biari_decode_symbol(dep_dp,info, c0
					+ 5 );
				if (mode_sym!=0)
				{
					mode_sym = biari_decode_symbol(dep_dp,info, c0
						+ 6 );
					act_sym += mode_sym ? 8 : 4;
				}
				// decoding of I pred-mode: 0,1,2,3
				mode_sym =  biari_decode_symbol(dep_dp,info, c0
					+ 7 );
				act_sym += (mode_sym<<1);
				mode_sym =  biari_decode_symbol(dep_dp,info, c0
					+ 8 );
				act_sym += mode_sym;
				curr_mb_type = act_sym;
			}
		}
	
	
	}
	else
	{
		if (bframe)
		{
			BiContextTypePtr c2 = ctx->mb_type_contexts[2];
			BiContextTypePtr c26 = c2+6;
			b = (!mbBInfo || !bType) ? 0 : 1;
			a = (!mbAInfo || !aType) ? 0 : 1;
			if (biari_decode_symbol(dep_dp,info, c2+a + b))
			{
				if (biari_decode_symbol(dep_dp,info, c2+4))
				{
					if (biari_decode_symbol(dep_dp,info, c2+5))
					{
						act_sym=12;
						if (biari_decode_symbol(dep_dp,info, c26)) 
							act_sym+=8;
						if (biari_decode_symbol(dep_dp,info, c26)) 
							act_sym+=4;
						if (biari_decode_symbol(dep_dp,info, c26)) 
							act_sym+=2;

						if (act_sym==24)  
							act_sym=11;
						else if (act_sym==26)  
							act_sym=22;
						else
						{
							if (act_sym==22)     
								act_sym=23;
							if (biari_decode_symbol(dep_dp,info, c26)) 
								act_sym+=1; 
						}
					}
					else
					{
						act_sym=3;
						if (biari_decode_symbol(dep_dp,info, c26)) 
							act_sym+=4;
						if (biari_decode_symbol(dep_dp,info, c26)) 
							act_sym+=2;
						if (biari_decode_symbol(dep_dp,info, c26)) 
							act_sym+=1;
					}
				}
				else
				{
					act_sym = biari_decode_symbol(dep_dp,info,c26)
						? 2 : 1;
				}
			}
			else
				act_sym = 0;
		}
		else // P-frame
		{
			BiContextTypePtr c1 = ctx->mb_type_contexts[1];
			if (biari_decode_symbol(dep_dp,info, c1+4 )) 
			{
				act_sym = biari_decode_symbol(dep_dp,info, c1+7)
					? 7 : 6;
			}
			else
			{
				if (biari_decode_symbol(dep_dp,info, c1+5 ))
				{
					act_sym = biari_decode_symbol(dep_dp,info,c1+7)
						? 2 : 3;
				}
				else
				{
					act_sym = biari_decode_symbol(dep_dp,info, c1+6)
						? 4 : 1;
				}
			}
		}
		
		if (act_sym<=6 || (((img->type == B_SLICE)?1:0) && act_sym<=23))
			curr_mb_type = act_sym;
		else  // additional info for 16x16 Intra-mode
		{
			mode_sym = biari_decode_final(dep_dp);
			if( mode_sym==1 ){
				if(bframe)  // B frame
					curr_mb_type = 48;
				else      // P frame
					curr_mb_type = 31;
			}
			else
			{
				BiContextTypePtr c1 = ctx->mb_type_contexts[1];
				mode_sym =  biari_decode_symbol(dep_dp,info, c1+
					+ 8 ); // decoding of AC/no AC
				act_sym += (mode_sym<<4) - (mode_sym<<2); //*12;
				// decoding of cbp: 0,1,2
				if (biari_decode_symbol(dep_dp,info, c1+9))
				{
					act_sym += biari_decode_symbol(dep_dp,info, c1
						+ 9) ? 8 : 4;
				}
				
				// decoding of I pred-mode: 0,1,2,3
				mode_sym = biari_decode_symbol(dep_dp,info, c1 + 10);
				act_sym += (mode_sym<<1);
				curr_mb_type = act_sym + biari_decode_symbol(dep_dp,info, 
					c1 + 10);
			}
		}
	}
	se->value1 = curr_mb_type;


}
#if FEATURE_T8x8
/*!
***************************************************************************
* \brief
*    This function is used to arithmetically decode the macroblock
*    intra_pred_size flag info of a given MB.
***************************************************************************
*/
void readMB_transform_size_flag_CABAC(ImageParameters *img,TMBsProcessor *info, Macroblock *currMB, 
									  SyntaxElement *se,
									  Bitstream *dep_dp)
{
	//Macroblock* currMB=info->currMB;
	Slice *currSlice = img->currentSlice;
	TextureInfoContexts*ctx = currSlice->tex_ctx;
	//TMBsProcessor *info  = img->mbsProcessor;
	int act_ctx,act_sym;

	MacroBlock	*mbAInfo, *mbBInfo;
	//MacroBlock **nMBs = &info->neighborABCD[0];
	avdNativeInt a,b;
	int					leftMBYForIntra = -1;
	mbAInfo = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,GetPosInfo(info)->subblock_y*4,16,&leftMBYForIntra):GetLeftMB(info);
	mbBInfo = GetUpMB(info);
	a = (!mbAInfo) || 
		mbIsNotMbT8x8Flag(mbAInfo) ? 0 : 1;
	b = (!mbBInfo) || 
		mbIsNotMbT8x8Flag(mbBInfo) ? 0 : 1;

	//int b = (currMB->mb_up   == NULL) ? 0 : currMB->mb_up->luma_transform_size_8x8_flag;
	//int a = (currMB->mb_left == NULL) ? 0 : currMB->mb_left->luma_transform_size_8x8_flag;

	act_ctx = a + b;
	act_sym = biari_decode_symbol(dep_dp,info, ctx->transform_size_contexts + act_ctx );

	se->value1 = act_sym;



}
#endif//FEATURE_T8x8
/*!
 ************************************************************************
 * \brief
 *    This function is used to arithmetically decode a pair of
 *    intra prediction modes of a given MB.
 ************************************************************************
 */
void readIntraPredMode_CABAC( ImageParameters *img,TMBsProcessor* info,SyntaxElement *se,
                              Bitstream *dep_dp)
{
	TextureInfoContexts *ctx     = img->currentSlice->tex_ctx;
	// use_most_probable_mode
	// remaining_mode_selector
	if (biari_decode_symbol(dep_dp,info, ctx->ipr_contexts) == 1)
		se->value1 = -1;
	else
	{
		se->value1  = (biari_decode_symbol(dep_dp,info, ctx->ipr_contexts+1)     );
		se->value1 |= (biari_decode_symbol(dep_dp,info, ctx->ipr_contexts+1) << 1);
		se->value1 |= (biari_decode_symbol(dep_dp,info, ctx->ipr_contexts+1) << 2);
	}


}
/*!
 ************************************************************************
 * \brief
 *    This function is used to arithmetically decode the reference
 *    parameter of a given MB.
 ************************************************************************
 */
void readRefFrame_CABAC(ImageParameters *img,TMBsProcessor *info, SyntaxElement *se,
                         Bitstream *dep_dp)
{
	MotionInfoContexts *ctx = img->currentSlice->mot_ctx;
	Macroblock *currMB = info->currMB;
	StorablePicture	*dec_picture = img->dec_picture;

	avdUInt8  **refframe_array = se->value2 == 0 ? 
		dec_picture->motionInfo->ref_idx0 : 
	    dec_picture->motionInfo->ref_idx1;

	avdNativeInt   addctx  = 0;
	avdNativeInt   a=0, b=0;
	avdNativeInt   act_ctx;
	avdNativeInt   act_sym;
	avdNativeInt   b8a, b8b;
	avdNativeInt	y4x4AB[2], X8x8;
	avdNativeInt   currMBAffField = mbIsMbField(currMB);

	//TMBsProcessor *info  = img->mbsProcessor;
	
	MacroBlock *nMBs[2];
	MacroBlock *nMb;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	MacroBlock   *currMBInfo = info->currMB;
	TPosInfo*  posInfo  = GetPosInfo(info);
	int x4x4Offset = posInfo->subblock_x;
	int y4x4Offset = posInfo->subblock_y;
	int block_y;
	MacroBlock	*mbAInfo, *mbBInfo;
	if (GetMbAffFrameFlag(img))
	{
		GetNeighborAB(img,info,x4x4Offset, y4x4Offset,nMBs, 
			y4x4AB);
	}
	else
	{
		mbAInfo = GetLeftMB(info);
		mbBInfo = GetUpMB(info);

		block_y = posInfo->block_y;

		if (x4x4Offset > 0){
			nMBs[NEIGHBOR_A] = currMBInfo;
		}
		else {
			nMBs[NEIGHBOR_A] = mbAInfo&&INSAMESLICE(currMB,mbAInfo)?mbAInfo:NULL ;
		}

		y4x4AB[NEIGHBOR_A] = block_y+y4x4Offset;

		// Get neighborB
		if (y4x4Offset > 0){
			nMBs[NEIGHBOR_B] = currMBInfo;
		}
		else {
			nMBs[NEIGHBOR_B] =  mbBInfo&&INSAMESLICE(currMB,mbBInfo)?mbBInfo:NULL ;
		}
		y4x4AB[NEIGHBOR_B] = block_y+y4x4Offset-1;
	}
	

	if (nMBs[NEIGHBOR_B]){
		b8b = (posInfo->subblock_x>>1) + (y4x4AB[NEIGHBOR_B]&2);
		X8x8 = (GetMBX(info)<<1) + (posInfo->subblock_x>>1);

		nMb = nMBs[NEIGHBOR_B];

		if (!((mbGetMbType(nMb) == IPCM) || IS_DIRECT(nMb) || 
			refframe_array[y4x4AB[NEIGHBOR_B]>>1][X8x8] == AVD_INVALID_REF_IDX ||
			//(mbGetB8Mode(nMb, b8b) == 0 && niGetCabacB8PredDir(nMBs[NEIGHBOR_B], b8b) == 2)
			niGetCabacDirectBiDir(nMb,b8b)
			))
		{	

			if (GetMbAffFrameFlag(img) && !currMBAffField && 
				GetFieldByPos(img,info,y4x4AB[NEIGHBOR_B]>>2,X8x8>>1))
				b = (refframe_array[y4x4AB[NEIGHBOR_B]>>1][X8x8] > 1 ? 1 : 0);
			else
				b = (refframe_array[y4x4AB[NEIGHBOR_B]>>1][X8x8] > 0 ? 1 : 0);
			//AvdLog(DUMP_SLICE,DUMP_DCORE"\nGetRefCabac:(%d,%d),field=%d",y4x4AB[NEIGHBOR_B]>>2,X8x8>>1,GetMbAffFrameFlag(img)?GetFieldByPos(img,info,y4x4AB[NEIGHBOR_B]>>2,X8x8>>1):mbIsMbField(nMb));
		}
	}
	

	if (nMBs[NEIGHBOR_A]){
		b8a = ((posInfo->subblock_x&1)==((posInfo->subblock_x&2)>>1) ? 1 : 0) 
			+ (y4x4AB[NEIGHBOR_A]&2);
		X8x8 = (GetMBX(info)<<1) + (!posInfo->subblock_x ? -1 : (posInfo->subblock_x != 3 ?
			0 : 1));

		nMb = nMBs[NEIGHBOR_A];

		if (!((mbGetMbType(nMb) == IPCM) || IS_DIRECT(nMb) || 
			refframe_array[y4x4AB[NEIGHBOR_A]>>1][X8x8] == AVD_INVALID_REF_IDX ||
			//(mbGetB8Mode(nMb, b8a) ==0 && niGetCabacB8PredDir(nMBs[NEIGHBOR_A], b8a) == 2)
			niGetCabacDirectBiDir(nMb,b8a)
			))
		{
			if (GetMbAffFrameFlag(img) && !currMBAffField && 
				mbIsMbField(nMb))
				a = (refframe_array[y4x4AB[NEIGHBOR_A]>>1][X8x8] > 1 ? 1 : 0);
			else
				a = (refframe_array[y4x4AB[NEIGHBOR_A]>>1][X8x8] > 0 ? 1 : 0);
		}
	}


	act_ctx = a + (b<<1);
	se->context = act_ctx; // store context
	act_sym = biari_decode_symbol(dep_dp,info,ctx->ref_no_contexts[addctx] + act_ctx );
	if (act_sym != 0)
	{
		act_sym = unary_bin_decode(dep_dp,info,ctx->ref_no_contexts[addctx]+4,1) + 1;
	}
	se->value1 = act_sym;

}


/*!
 ************************************************************************
 * \brief
 *    This function is used to arithmetically decode the delta qp
 *     of a given MB.
 ************************************************************************
 */
void readDquant_CABAC(ImageParameters *img, TMBsProcessor* info,SyntaxElement *se,
                       Bitstream *dep_dp)
{
	MotionInfoContexts *ctx = img->currentSlice->mot_ctx;
	avdNativeInt act_sym;
	avdNativeInt dquant;
	TCABACInfo	*cabacInfo = img->cabacInfo;
	act_sym = biari_decode_symbol(dep_dp,info,ctx->delta_qp_contexts + 
		((cabacInfo->last_dquant != 0) ? 1 : 0));
	if (act_sym != 0)
	{
		act_sym = unary_bin_decode(dep_dp,info,ctx->delta_qp_contexts+2,1) + 1;
	}
	dquant = ((act_sym+1)>>1);
	if((act_sym & 0x01)==0)                           // lsb is signed bit
		dquant = -dquant;
	se->value1 = dquant;
	cabacInfo->last_dquant = dquant;
#if TRACE
	fprintf(p_trace, "@%d %s\t\t\t%d\n",symbolCount++, se->tracestring, se->value1);
	fflush(p_trace);
#endif
}
/*!
 ************************************************************************
 * \brief
 *    This function is used to arithmetically decode the coded
 *    block pattern of a given MB.
 ************************************************************************
 */
void readCBP_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,
                   Bitstream *dep_dp)
{
	TextureInfoContexts *ctx = img->currentSlice->tex_ctx;
	avdNativeInt mb_x, mb_y;
	avdNativeInt a, b;
	//int curr_cbp_idx;
	avdNativeInt cbp = 0;
	avdNativeInt cbp_bit;
	avdNativeInt mask;
	//TMBsProcessor *info  = img->mbsProcessor;
	MacroBlock	*mbAInfo, *mbBInfo;
	
	avdNativeInt		cbpMbA[2], cbpMbB;
	avdNativeInt		mbB8[2];
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	//MacroBlock **nMBs = &info->neighborABCD[0];
	MacroBlock			*mbA[2], *mbB;
	int					leftMBYForIntra = -1;
	mbAInfo = GetLeftMB(info);//GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,GetPosInfo(info)->subblock_y*4,16,&leftMBYForIntra):
	mbBInfo = GetUpMB(info);

	if (mbBInfo) {

		mbB = mbBInfo;
		cbpMbB = niGetCabacCBP(mbBInfo);
	}
	else
		mbB = NULL;

	for (a = 0; a < 2; a++){

		if (mbAInfo){
			mbA[a] = mbAInfo;
			mbB8[a] = ((((posInfo->pix_y+a*8)&0xf)>>3)<<1)+1;
			cbpMbA[a] = niGetCabacCBP(mbAInfo);
		}
		else if(GetMbAffFrameFlag(img)&&IsNeedToCalcualteNeighborA_MBAFF(info))
		{
			mbA[a] = GetLeftMBAffMB2(img,info,a*8,16,&leftMBYForIntra);
			mbB8[a] = ((leftMBYForIntra>>3)<<1)+1;
			cbpMbA[a] = niGetCabacCBP(mbA[a]);
		}
		else
			mbA[a] = NULL;
	}

	//  coding of luma part (bit by bit)
	for (mb_y=0; mb_y < 4; mb_y += 2)
	{
		for (mb_x=0; mb_x < 4; mb_x += 2)
		{
			if (mb_y == 0) {
				if (!mbB || mbGetMbType(mbB) == IPCM)
					b = 0;
				else
					b = (((cbpMbB & (1<<(2+(mb_x>>1)))) == 0) ? 1 : 0);
			}
			else
				b = (((cbp & (1<<(mb_x>>1))) == 0) ? 1: 0);

			if (mb_x == 0) {
				a = (mb_y>>1); // tmp storage;
				if (mbA[a] && mbGetMbType(mbA[a]) != IPCM)
					a = (((cbpMbA[a] & (1<<mbB8[a])) == 0) ? 1 : 0);
				else
					a = 0;
			}
			else
				a = ( ((cbp & (1<<mb_y)) == 0) ? 1: 0);
			cbp_bit = biari_decode_symbol(dep_dp,info, ctx->cbp_contexts[0] + a + (b<<1) );
			if (cbp_bit) {
				mask = (1<<(mb_y+(mb_x>>1)));
				cbp += mask;
			}
		}
	}
	//curr_cbp_idx = (se->type == SE_CBP_INTRA) ? 0 : 1;
	// coding of chroma part
	b = (!mbB || (cbpMbB <= 15 && mbGetMbType(mbB) != IPCM)) ? 0 : 1;
	a = (!mbA[0] || (cbpMbA[0] <= 15 && mbGetMbType(mbA[0]) != IPCM)) ? 0 : 1;
	cbp_bit = biari_decode_symbol(dep_dp,info, ctx->cbp_contexts[1] + a + (b<<1));
	if (cbp_bit) // set the chroma bits
	{
		b = (!mbB || (mbGetMbType(mbB) != IPCM && 
			(cbpMbB <= 15 || (cbpMbB>>4) != 2))) ? 0 : 1;
		a = (!mbA[0] || (mbGetMbType(mbA[0]) != IPCM && 
			(cbpMbA[0] <= 15 || (cbpMbA[0]>>4) != 2))) ? 0 : 1;
		cbp_bit = biari_decode_symbol(dep_dp,info, ctx->cbp_contexts[2] + a + (b<<1));
		cbp += (cbp_bit == 1) ? 32 : 16;
	}
	se->value1 = cbp;
	if (!cbp)
		img->cabacInfo->last_dquant=0;

}

/*!
 ************************************************************************
 * \brief
 *    This function is used to arithmetically decode the chroma
 *    intra prediction mode of a given MB.
 ************************************************************************
 */  //GB
void readCIPredMode_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *se,
                          Bitstream *dep_dp)
{
	TextureInfoContexts *ctx = img->currentSlice->tex_ctx;
	avdNativeInt        a,b;
	//TMBsProcessor *info  = img->mbsProcessor;
	MacroBlock	*mbAInfo, *mbBInfo;
	int					leftMBYForIntra = -1;
	mbAInfo = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,0,8,&leftMBYForIntra):GetLeftMB(info);
	mbBInfo = GetUpMB(info);

	b = (!mbBInfo || !niGetCabacNonZeroIntraChromaMode(mbBInfo)) ? 0 : 1;
	a = (!mbAInfo || !niGetCabacNonZeroIntraChromaMode(mbAInfo)) ? 0 : 1;
	if ((se->value1 = biari_decode_symbol(dep_dp,info, ctx->cipr_contexts + a + b))) 
		se->value1 = unary_bin_max_decode(dep_dp,info,ctx->cipr_contexts+3,0,2)+1;
#if TRACE
	fprintf(p_trace, "@%d %s\t\t\t%d\n",symbolCount++, se->tracestring, se->value1);
	fflush(p_trace);
#endif

}

static const avdUInt8 maxpos       [] = {16, 15, 64, 32, 32, 16,  4, 15};
static const avdUInt8 c1isdc       [] = { 1,  0,  1,  1,  1,  1,  1,  0};
static const avdUInt8 type2ctx_bcbp[] = { 0,  1,  2,  2,  3,  4,  5,  6}; // 7
static const avdUInt8 type2ctx_map [] = { 0,  1,  2,  3,  4,  5,  6,  7}; // 8
static const avdUInt8 type2ctx_last[] = { 0,  1,  2,  3,  4,  5,  6,  7}; // 8
static const avdUInt8 type2ctx_one [] = { 0,  1,  2,  3,  3,  4,  5,  6}; // 7
static const avdUInt8 type2ctx_abs [] = { 0,  1,  2,  3,  3,  4,  5,  6}; // 7

//===== position -> ctx for MAP =====
//--- zig-zag scan ----
static const avdUInt8  pos2ctx_map8x8 [] = { 0,  1,  2,  3,  4,  5,  5,  4,  4,  3,  3,  4,  4,  4,  5,  5,
                                        4,  4,  4,  4,  3,  3,  6,  7,  7,  7,  8,  9, 10,  9,  8,  7,
                                        7,  6, 11, 12, 13, 11,  6,  7,  8,  9, 14, 10,  9,  8,  6, 11,
                                       12, 13, 11,  6,  9, 14, 10,  9, 11, 12, 13, 11 ,14, 10, 12, 14}; // 15 CTX
static const avdUInt8  pos2ctx_map8x4 [] = { 0,  1,  2,  3,  4,  5,  7,  8,  9, 10, 11,  9,  8,  6,  7,  8,
                                        9, 10, 11,  9,  8,  6, 12,  8,  9, 10, 11,  9, 13, 13, 14, 14}; // 15 CTX
static const avdUInt8  pos2ctx_map4x4 [] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 14}; // 15 CTX
static const avdUInt8* pos2ctx_map    [] = {pos2ctx_map4x4, pos2ctx_map4x4, pos2ctx_map8x8, pos2ctx_map8x4,
                                       pos2ctx_map8x4, pos2ctx_map4x4, pos2ctx_map4x4, pos2ctx_map4x4};
static const avdUInt8  pos2ctx_map8x8i[] = { 0,  1,  1,  2,  2,  3,  3,  4,  5,  6,  7,  7,  7,  8,  4,  5,
6,  9, 10, 10,  8, 11, 12, 11,  9,  9, 10, 10,  8, 11, 12, 11,
9,  9, 10, 10,  8, 11, 12, 11,  9,  9, 10, 10,  8, 13, 13,  9,
9, 10, 10,  8, 13, 13,  9,  9, 10, 10, 14, 14, 14, 14, 14, 14}; // 15 CTX
static const avdUInt8  pos2ctx_map8x4i[] = { 0,  1,  2,  3,  4,  5,  6,  3,  4,  5,  6,  3,  4,  7,  6,  8,
9,  7,  6,  8,  9, 10, 11, 12, 12, 10, 11, 13, 13, 14, 14, 14}; // 15 CTX
static const avdUInt8  pos2ctx_map4x8i[] = { 0,  1,  1,  1,  2,  3,  3,  4,  4,  4,  5,  6,  2,  7,  7,  8,
8,  8,  5,  6,  9, 10, 10, 11, 11, 11, 12, 13, 13, 14, 14, 14}; // 15 CTX

static const avdUInt8* pos2ctx_map_int[] = {pos2ctx_map4x4, pos2ctx_map4x4, pos2ctx_map8x8i,pos2ctx_map8x4i,
												pos2ctx_map4x8i,pos2ctx_map4x4, pos2ctx_map4x4, pos2ctx_map4x4};
//===== position -> ctx for LAST =====
static const avdUInt8  pos2ctx_last8x8 [] = { 0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
                                         2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
                                         3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,  4,
                                         5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8}; //  9 CTX
static const avdUInt8  pos2ctx_last8x4 [] = { 0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,
                                         3,  3,  3,  3,  4,  4,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8}; //  9 CTX

static const avdUInt8  pos2ctx_last4x4 [] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15}; // 15 CTX
static const avdUInt8* pos2ctx_last    [] = {pos2ctx_last4x4, pos2ctx_last4x4, pos2ctx_last8x8, pos2ctx_last8x4,
                                        pos2ctx_last8x4, pos2ctx_last4x4, pos2ctx_last4x4, pos2ctx_last4x4};

void readRunLevelArray_CABAC (ImageParameters *img,TMBsProcessor *info,SyntaxElement  *se, Bitstream* bs,
					avdNativeInt *level, avdNativeInt *runarr, avdNativeInt *numCoeff)
{
	// TBD: a lot of operations could be done only once for LUMA_16AC, LUMA_4x4 and CHROMA_AC;
	// need to bring the function one level up;
	const avdUInt8 *pos2CtxMap;
	//Bitstream *bs;
	BiContextTypePtr	ctType;
	Macroblock			*currMB = info->currMB;
	BiContextTypePtr	map_ctx, last_ctx;
	//TMBsProcessor *info  = img->mbsProcessor;
	MacroBlock	*mbInfo;
	//AVCNeighbor4x4Boundary	*mbBdry;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	avdNativeInt i, j, upper_bit, left_bit, startBit, ystepShift;
	avdNativeInt c1, c2, prevIdx, idx;
	TCABACInfo	*cabacInfo = img->cabacInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	avdNativeInt fld = (img->specialInfo->structure!=FRAME || mbIsMbField(currMB));
#ifdef VOI_H264D_BASELINE_SINGLE_SLICE
	//MacroBlock **nMBs = &info->neighborABCD[0];
#endif
	MacroBlock	*mbAInfo, *mbBInfo;
	TMBsParser			 *parser = GetMBsParser(info);
	int					leftMBYForIntra = -1;
	int					upMBYOffset = posInfo->pix_y-1;
	int					start;

	//MotionInfoContexts			*mot_ctx = img->currentSlice->mot_ctx;      //!< pointer to struct of context models for use in CABAC
	TextureInfoContexts			*tex_ctx = img->currentSlice->tex_ctx;      //!< pointer to struct of context models for use in CABAC
	TIME_BEGIN(start);
#if FEATURE_INTERLACE
	map_ctx  = fld ? tex_ctx->fld_map_contexts[type2ctx_map [se->context]]
	: tex_ctx->map_contexts[type2ctx_map [se->context]];
	last_ctx = fld ? tex_ctx->fld_last_contexts[type2ctx_last[se->context]]
	: tex_ctx->last_contexts[type2ctx_last[se->context]];
#else//FEATURE_INTERLACE
	map_ctx  = 
	 tex_ctx->map_contexts[type2ctx_map [se->context]];
	last_ctx = 
	 tex_ctx->last_contexts[type2ctx_last[se->context]];
#endif//FEATURE_INTERLACE
	//Read CBP4-BIT
	switch (se->context){
		case LUMA_16AC:
		case LUMA_4x4:
		case LUMA_8x8:
			ystepShift = 2;
			j = posInfo->subblock_y;
			i = posInfo->subblock_x;
			startBit = COEFF_LUMA_START_IDX;
			break;
		case CHROMA_AC:
			ystepShift = 3;
			j = posInfo->subblock_y;
			i = posInfo->subblock_x;
			startBit = !cabacInfo->is_v_block ? COEFF_CHROMA_U_AC_START_IDX : COEFF_CHROMA_V_AC_START_IDX;
			break;
		case CHROMA_DC:
			ystepShift = i = j = 0;
			startBit = !cabacInfo->is_v_block ? COEFF_CHROMA_U_DC_IDX : COEFF_CHROMA_V_DC_IDX;
			break;
		case LUMA_16DC:
			ystepShift = i = j = 0;
			startBit = COEFF_LUMA_INTRA16x16DC_IDX;
			break;
		default:
			printf( "Wrong readRunLevelArray_CABAC type = %d!\n", se->context);
			exit(0);
			break;
	}
	*numCoeff = 0;
	//bs=dp->bitstream;
	mbAInfo = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,(j<<(ystepShift-2))*4,16,&leftMBYForIntra):GetLeftMB(info);
	mbBInfo = GetMbAffFrameFlag(img)? (Macroblock*)GetMBAffNeighbor2(img,info,NEIGHBOR_B,-1,&upMBYOffset,16):GetUpMB(info);
#if FEATURE_T8x8
	if(se->context==LUMA_8x8)
	{
		upper_bit = left_bit = 1;
	}
	else
#endif//FEATURE_T8x8
	{
		TMBBitStream	*mbBits = GetMBBits(currMB);
		upper_bit = left_bit = IS_INTRA(currMB) ? 1 : 0; //default;

		//--- get bits from neighbouring blocks ---
		if (j)
			upper_bit = (avdNativeInt)((parser->cbpBits>>(startBit + inv_decode_scan[((j-1)<<2)+i]))&1);
		else {

			int yOffset = upMBYOffset;
			mbInfo = mbBInfo;

			if (mbInfo) // use (ystepShift>>1) to zero out DC;
			{

				int isIPCM = mbGetMbType(mbInfo)==IPCM;

				if(isIPCM)//fixed_MP
					upper_bit = 1;
				else
				upper_bit = ((niGetCabacNeighborStruct(mbInfo)->cbpBits>>(startBit + (ystepShift>>1) * 

				inv_decode_scan[(((yOffset&0xf)>>ystepShift)<<2)+i]))&1);

			}
		}
		if (i)
			left_bit = (avdNativeInt)((parser->cbpBits>>(startBit + inv_decode_scan[(j<<2)+i-1]))&1);
		else {

			int yOffset;
			mbInfo = mbAInfo;
			
			

			if (mbInfo) // use (ystepShift>>1) to zero out DC; ((3-ystepShift)<<1 + 1): 3 Luma, 1 Chroma;
			{

				int isIPCM = mbGetMbType(mbInfo)==IPCM;
				yOffset = GetMbAffFrameFlag(img)? leftMBYForIntra//GetAbsYPos(img,info,NEIGHBOR_A,(j<<(ystepShift-2))*4,MB_BLOCK_SIZE)
												: posInfo->pix_y+(j<<(ystepShift-2))*4;
				
				if(isIPCM)//fixed_MP
					left_bit = 1;
				else
					left_bit = ((niGetCabacNeighborStruct(mbInfo)->cbpBits>>(startBit + (ystepShift>>1) *

				inv_decode_scan[(((yOffset&0xf)>>ystepShift)<<2)+((3-ystepShift)<<1) + 1]))&1);

			}
		}

		
		//===== encode symbol =====

		if (!biari_decode_symbol (bs,info, 
			tex_ctx->bcbp_contexts[type2ctx_bcbp[se->context]]
		+ ((upper_bit<<1) | left_bit)))
			return; // 0 CBP4-BIT
	}


	//--- set bits for current block ---
	ctType = map_ctx; 
	//===== decode significance map =====
	if (c1isdc[se->context]){
		c1        = 0;
		c2        = maxpos[se->context]-1;
	}
	else{
		c1        = 1;
		c2        = maxpos[se->context];
	}

	pos2CtxMap =   (!fld)  ? pos2ctx_map[se->context] : pos2ctx_map_int[se->context];
	for (i=c1, prevIdx=-1, idx=0; i<c2; i++, idx++){ // if last coeff is reached, it has to be significant
		//--- read significance symbol ---
		if (!biari_decode_symbol(bs,info, map_ctx + pos2CtxMap[i]))
			continue;

		runarr[*numCoeff] = idx - prevIdx - 1;
		prevIdx = idx;
		++*numCoeff;
		//--- read last coefficient symbol ---
		if (biari_decode_symbol (bs, info,last_ctx +
			pos2ctx_last[se->context][i]))
			break;
	}
	//--- last coefficient must be significant if no last symbol was received ---
	if (i==c2){
		runarr[*numCoeff] = idx - prevIdx - 1;
		++*numCoeff;
	}

	//===== decode significant coefficients =====
	c1 = 1;
	c2 = 0;
	ctType = tex_ctx->one_contexts[type2ctx_one[se->context]];
	for (i = *numCoeff-1; i >= 0; i--)
	{
		// first bit of coeff_absolute_value_minus_one;
		level[i] = biari_decode_symbol (bs,info,ctType + ((c1 <= 4) ? c1 : 4)) + 1;
		if (level[i]==2){
			// concatenated unary_exp_golomb;
			level[i] += unary_exp_golomb_level_decode (bs, info,((c2 <= 4) ? c2 : 4) +
				tex_ctx->abs_contexts[type2ctx_abs[se->context]]);
			c1=0;
			c2++;
		}
		else if (c1)
			c1++;

		if (biari_decode_symbol_eq_prob(bs)) // sign bit;
			level[i] = -level[i];


	}
	TIME_END(start,readrunLevelSum)
}										
										
/*!
 ************************************************************************
 * \brief
 *    decoding of unary binarization using one or 2 distinct
 *    models for the first and all remaining bins; no terminating
 *    "0" for max_symbol
 ***********************************************************************
 */
avdNativeUInt unary_bin_max_decode(Bitstream *dep_dp,
								   TMBsProcessor* info,
                                  BiContextTypePtr ctx,
                                  avdNativeInt ctx_offset,
                                  avdNativeUInt max_symbol)
{
	BiContextTypePtr ictx;
	avdNativeUInt l;
	avdNativeUInt symbol;
	symbol =  biari_decode_symbol(dep_dp,info, ctx );
	if (!symbol)
		return 0;

	if (max_symbol == 1)
		return symbol;

	ictx=ctx+ctx_offset;
	for (symbol = 1; (l = biari_decode_symbol(dep_dp,info, ictx)) && 
		(symbol < max_symbol - 1); symbol++);
	if ((l!=0) && (symbol==max_symbol-1))
		symbol++;
	return symbol;
}


/*!
 ************************************************************************
 * \brief
 *    decoding of unary binarization using one or 2 distinct
 *    models for the first and all remaining bins
 ***********************************************************************
 */
avdNativeUInt unary_bin_decode(Bitstream *dep_dp,
							   TMBsProcessor* info,
                              BiContextTypePtr ctx,
                              avdNativeInt ctx_offset)
{
	BiContextTypePtr ictx;
	avdNativeUInt symbol;
	if (!biari_decode_symbol(dep_dp,info, ctx))
		return 0;

	ictx=ctx+ctx_offset;
	for (symbol = 1; biari_decode_symbol(dep_dp,info, ictx); symbol++);
	return symbol;
}


/*!
 ************************************************************************
 * \brief
 *    finding end of a slice in case this is not the end of a frame
 *
 * Unsure whether the "correction" below actually solves an off-by-one
 * problem or whether it introduces one in some cases :-(  Anyway,
 * with this change the bit stream format works with CABAC again.
 * StW, 8.7.02
 ************************************************************************
 */
avdNativeInt cabac_startcode_follows(ImageParameters *img,TMBsProcessor* info,avdNativeInt eos_bit)
{

	Bitstream *dep_dp;
	avdNativeUInt  bit;
	dep_dp = img->currentSlice->partArr[0].bitstream;
	if( eos_bit )
	{
		bit = biari_decode_final (dep_dp); //GB

	}
	else
		bit = 0;
	return bit;
}

/*!
 ************************************************************************
 * \brief
 *    Exp Golomb binarization and decoding of a symbol
 *    with prob. of 0.5
 ************************************************************************
 */
avdNativeUInt exp_golomb_decode_eq_prob( Bitstream *dep_dp,
                                        avdNativeInt k)
{
	avdNativeUInt symbol = 0;
	avdNativeUInt binary_symbol = 0;
	for (; biari_decode_symbol_eq_prob(dep_dp); symbol |= (1<<k), k++);
	while (k--)                             //next binary part
		if (biari_decode_symbol_eq_prob(dep_dp)==1) 
			binary_symbol |= (1<<k);
	return (symbol+binary_symbol);
}


/*!
 ************************************************************************
 * \brief
 *    Exp-Golomb decoding for LEVELS
 ***********************************************************************
 */
avdNativeUInt unary_exp_golomb_level_decode( Bitstream *dep_dp,
											TMBsProcessor* info,
                                            BiContextTypePtr ctx)
{
	avdNativeUInt l,k;
	avdNativeUInt symbol;
	if (!biari_decode_symbol(dep_dp,info, ctx))
		return 0;

	for (symbol = 1, k = 2; (l=biari_decode_symbol(dep_dp,info, ctx)) 
		&& k != 13; symbol++, k++); //exp_start = 13
	if (l!=0)
		symbol += exp_golomb_decode_eq_prob(dep_dp,0)+1;
	return symbol;
}


/*!
 ************************************************************************
 * \brief
 *    Exp-Golomb decoding for Motion Vectors
 ***********************************************************************
 */
avdNativeUInt unary_exp_golomb_mv_decode(Bitstream *dep_dp,
										 TMBsProcessor* info,
                                        BiContextTypePtr ctx,
                                        avdNativeUInt max_bin)
{
	BiContextTypePtr ictx = ctx;
	avdNativeUInt l,k;
	avdNativeUInt bin=1;
	avdNativeUInt symbol;
	if (!biari_decode_symbol(dep_dp,info, ictx))
		return 0;

	ictx++;
	for (symbol = 1, k = 2; (l=biari_decode_symbol(dep_dp,info, ictx)) 
		&& k != 8; symbol++, k++){ //exp_start = 8
		if ((++bin)==2) 
			ictx++;
		if (bin==max_bin) 
			ictx++;
	}
	if (l!=0)
		symbol += exp_golomb_decode_eq_prob(dep_dp,3)+1;
	return symbol;
}

/*!
 ************************************************************************
 * \brief
 *    Read one avdUInt8 from CABAC-partition. 
 *    Bitstream->read_len will be modified
 *    (for IPCM CABAC  28/11/2003)
 *
 * \author
 *    Dong Wang <Dong.Wang@bristol.ac.uk>  
 ************************************************************************
*/
void readIPCMBytes_CABAC(ImageParameters *img,TMBsProcessor *info,SyntaxElement *sym, Bitstream *currStream)
{
  //sym->value1= GetBits(currStream, 8);


  
}

avdNativeInt check_next_mb_and_get_field_mode_CABAC(ImageParameters *img, TMBsProcessor* info,SyntaxElement *se,
													Bitstream* bs)
{
#if FEATURE_INTERLACE
	BiContextTypePtr          mb_type_ctx_copy[4];
	BiContextTypePtr          mb_aff_ctx_copy;
	Bitstream *dep_dp_copy;
	Bitstream *dep_dp = bs;
	Macroblock *currMB, *nextMB,*prevMB,*up,*left;
	avdNativeInt length;
	avdNativeInt bframe = (img->type==B_SLICE);
	avdNativeInt skip   = 0;
	avdNativeInt i;
	avdUInt8* tmpBuf = (avdUInt8*)info->mbsParser->m7;
	MacroBlock* nMBs[2]={0,0};
	// TBD: Rewrite the programe;
	//get next MB (bottom MB)
	currMB = info->currMB;
	//prevMB = GetBottomMB(info,info->mb_x-1);
	nextMB = GetBottomMB(info,info->mb_x);
	mbSetMbFieldValue(nextMB,mbIsMbField(currMB));
	
	NeighborAB_Inference_CABAC2(img,info,
		nextMB,
		info->mb_x,
		info->mb_y+1,
		nMBs);
	left = nMBs[NEIGHBOR_A];
	up   = nMBs[NEIGHBOR_B];
	//mbSetMbFieldValue(currMB, mbIsMbField(prevMB));
	//create
	dep_dp_copy = (Bitstream *)tmpBuf;//(Bitstream *) voH264AlignedMalloc(img,72, sizeof(Bitstream) );
	tmpBuf+=sizeof(Bitstream);
	for (i=0;i<4;i++,tmpBuf+=NUM_MB_TYPE_CTX * sizeof(BiContextType))
		mb_type_ctx_copy[i] = (BiContextTypePtr) tmpBuf;
	tmpBuf-=NUM_MB_TYPE_CTX * sizeof(BiContextType);
		//voH264AlignedMalloc(img,73, NUM_MB_TYPE_CTX * sizeof(BiContextType) );
	mb_aff_ctx_copy = (BiContextTypePtr)tmpBuf;//(BiContextTypePtr) voH264AlignedMalloc(img,74, NUM_MB_AFF_CTX * sizeof(BiContextType) );
	
	//copy
	memcpy(dep_dp_copy,dep_dp,sizeof(Bitstream));
	length = dep_dp->currReadBuf - dep_dp->streamBuffer;
	dep_dp_copy->currReadBuf = dep_dp_copy->streamBuffer + length;
	for (i=0;i<4;i++)
		memcpy(mb_type_ctx_copy[i], img->currentSlice->mot_ctx->mb_type_contexts[i],
		NUM_MB_TYPE_CTX*sizeof(BiContextType) );
	memcpy(mb_aff_ctx_copy, img->currentSlice->mot_ctx->mb_aff_contexts,
		NUM_MB_AFF_CTX*sizeof(BiContextType) );


	//check_next_mb
#if TRACE
	strncpy(se->tracestring, "mb_skip_flag (of following bottom MB)", TRACESTRING_SIZE);
#endif
	img->cabacInfo->last_dquant = 0;
	//NeighborAB_Inference_CABAC_Bottom(img,info,nextMB,nMBs);

	readMB_skip_flagInfo_CABAC(img,info,se,dep_dp_copy);
	skip = (bframe)? (se->value1==0 && se->value2==0) : (se->value1==0);
	if (!skip){
#if TRACE
		strncpy(se->tracestring, "mb_field_decoding_flag (of following bottom MB)", 
			TRACESTRING_SIZE);
#endif
		readFieldModeInfo_CABAC2(img,info, se,dep_dp_copy,left,up);
		mbSetMbFieldValue(currMB, se->value1);
		AvdLog(DUMP_SLICE, DCORE_DUMP"\nmbFieldFlag(%d) from bottom MB", se->value1);
	}

	//reset
	//nextMB->slice_nr = -1;
	//img->current_mb_nr--;
	//memcpy(dep_dp,dep_dp_copy,sizeof(Bitstream));
	//dep_dp->currReadBuf = dep_dp->streamBuffer + length;
	for (i=0;i<4;i++)
		memcpy(img->currentSlice->mot_ctx->mb_type_contexts[i],mb_type_ctx_copy[i], 
		NUM_MB_TYPE_CTX*sizeof(BiContextType));
	memcpy( img->currentSlice->mot_ctx->mb_aff_contexts,mb_aff_ctx_copy,
		NUM_MB_AFF_CTX*sizeof(BiContextType) );
	
	return skip;
#else//TBD
	AVD_ERROR_CHECK(img,"not implemented check_next_mb_and_get_field_mode_CABAC\n",ERROR_InvalidBitstream);
	return 0;
#endif
}


#if FEATURE_INTERLACE
void NeighborAB_ABS_CABAC(ImageParameters *img,TMBsProcessor* info)
{
	avdNativeInt		mb_x = GetMBX(info);
	avdNativeInt		mb_y = GetMBY(info);
	int isTop	= CurrMBisTop(info);
	TPosInfo*  posInfo  = GetPosInfo(info);
	TSizeInfo	*sizeInfo = img->sizeInfo;
	int yPos = isTop ? mb_y:mb_y-1;
	int current_mb_nr = yPos*sizeInfo->PicWidthInMbs + mb_x;
	Macroblock* currMB = info->currMB = GetCurrMB(info,mb_x);
	int diff = current_mb_nr - info->start_mb_nr;
	TMBsProcessor* info2 = info->anotherProcessor;
	int widthMBAff = sizeInfo->PicWidthInMbs*2;
#if DEBUG_POS
	currMB->mb_x = mb_x;
	currMB->mb_y = mb_y;
#endif
	if(mb_x && diff)
	{
		GetLeftMB(info)		=  currMB - 1;//GetTopMB(info,mb_x-1);
	}
	else
	{
		GetLeftMB(info)		=  NULL; 
	}
	current_mb_nr	= GetCurPos(info,sizeInfo);
	diff			= current_mb_nr - info->start_mb_nr;

	if((mb_y && diff >= widthMBAff))	
	{
		GetUpMB(info)		= isTop ? GetTopMB(info2,mb_x) : GetBottomMB(info2,mb_x);
	}
	else
		GetUpMB(info) =  NULL; 
}


void readFieldModeInfo_CABAC2( ImageParameters *img,TMBsProcessor* info,SyntaxElement *se,
							 Bitstream *dep_dp,MacroBlock* left,MacroBlock* up)
{
	MotionInfoContexts *ctx         = (img->currentSlice)->mot_ctx;
	avdNativeInt a,b,act_ctx;
	int					leftMBYForIntra = -1;
	Macroblock* A,*B;
	A = left;
	B = up;
	a = (A) && mbIsMbField(A) ? 1 : 0;
	b = (B) && mbIsMbField(B) ? 1 : 0;

	act_ctx = a + b;
	se->value1 = biari_decode_symbol (dep_dp, info,&ctx->mb_aff_contexts[act_ctx]);

}
void readFieldModeInfo_CABAC( ImageParameters *img,TMBsProcessor* info,SyntaxElement *se,
							  Bitstream *dep_dp)
{
	MotionInfoContexts *ctx         = (img->currentSlice)->mot_ctx;
	avdNativeInt a,b,act_ctx;
	int					leftMBYForIntra = -1;
	Macroblock* A,*B;
	A = GetLeftMB(info);
	B = GetUpMB(info);
	a = (A) && mbIsMbField(A) ? 1 : 0;
	b = (B) && mbIsMbField(B)? 1 : 0;

	act_ctx = a + b;
	se->value1 = biari_decode_symbol (dep_dp, info,&ctx->mb_aff_contexts[act_ctx]);

}
#endif//FEATURE_INTERLACE



#endif//CABAC 