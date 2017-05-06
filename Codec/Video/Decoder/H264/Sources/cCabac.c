
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		x86Cabac.c
*
* \brief
*		defines short MMX inline function for local usage;
*
************************************************************************
*/

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

#if FEATURE_CABAC
extern const avdUInt8 SNGL_SCAN_TO_INDEX[16];
extern const avdUInt8 chromaScan[16];
extern const  	int T8x8KPos[4];
extern const avdUInt8 SNGL_SCAN8x8[64][2];
extern const avdUInt8 FIELD_SCAN8x8[64][2];
extern const avdUInt8 FIELD_SCAN_TO_INDEX[16];
void readIPCMcoeffsFromNAL_CABAC(ImageParameters *img,TMBsProcessor *info,Bitstream *bs)
{
	SyntaxElement currSE;
	avdNativeInt i,j;
	Bitstream   *currStream = bs;
	TMBBitStream	*mbBits = GetMBBits(info->currMB);
	//For CABAC, we don't need to read bits to let stream avdUInt8 aligned
	i = (currStream->bBitToGo&7);//fixed_MP
	if (i)
		FlushBits(currStream, i);

	{
		//read luma and chroma IPCM coefficients
		currSE.len=8;

		for(i=0;i<16;i++)
			for(j=0;j<16;j++)
			{
				//readIPCMBytes_CABAC(img,info,bs,&currSE, currStream);
				GetMBCofBuf(info)[(inv_decode_scan[((i>>2)<<2) + (j>>2)]<<4)|((i&3)<<2)|(j&3)]= GetBits(currStream, 8);
			}

			for(i=0;i<8;i++)
				for(j=0;j<8;j++)
				{
					//readIPCMBytes_CABAC(img,info,bs,&currSE, currStream);
					GetMBCofBuf(info)[(16<<4)+((((i>>2)<<1) + (j>>2))<<4)|((i&3)<<2)|(j&3)]= GetBits(currStream, 8);
				}

				for(i=0;i<8;i++)
					for(j=0;j<8;j++)
					{
						//readIPCMBytes_CABAC(img,info,info,&currSE, currStream);
						GetMBCofBuf(info)[(16<<4)+(8<<3)+((((i>>2)<<1) + (j>>2))<<4)|((i&3)<<2)|(j&3)]=GetBits(currStream, 8);
					}

					//If the decoded MB is IPCM MB, decoding engine is initialized

					// here the decoding engine is directly initialized without checking End of Slice
					// The reason is that, whether current MB is the last MB in slice or not, there is
					// at least one 'end of slice' syntax after this MB. So when fetching bytes in this 
					// initialisation process, we can guarantee there is bits available in bitstream. 
					init_decoding_engine_IPCM(img);

	}

}

void read_CBP_and_coeffs_from_NAL_CABAC(ImageParameters *img,TMBsProcessor *info,Bitstream *bs)

{
	Macroblock *currMB = info->currMB;
	Slice *currSlice;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	avdUInt8 *numRuns = &info->mbsParser->numRuns[0]; 
	avdInt16 *cofPt;
	avdInt32 *tmp32Pt;
	avdInt64 *tmp64Pt;
	const avdUInt16 *dequanPt;
	avdNativeInt numcoeff;
	avdNativeInt i,j,k,ll;
	avdNativeInt cbp;
	avdNativeInt coef_ctr, i0, j0;
	avdNativeInt yTmp, qp_per, qp_rem;
	int start;
	
	avdNativeInt *tmpLevarr, *levarr = (avdNativeInt *)&info->mbsParser->m7[0][0];
	avdNativeInt *tmpRunarr, *runarr = levarr + 16;

	avdNativeInt newIntra = IS_NEWINTRA (currMB);
	avdNativeInt Is4x4Intra, cbTypeIdx;
	//Bitstream *bs;
	SyntaxElement currSE;
	avdUInt8 *coefLocation;
	int		 div_qp_per;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TCABACInfo	*cabacInfo = img->cabacInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	TMBsParser			 *parser = GetMBsParser(info);
#if FEATURE_MATRIXQUANT

	TDequantArray*		dequantcoef		= img->dequant_coef_shift;
	TDequant8Array*		dequantcoef8	= img->dequant_coef_shift8;
	TDequantArray_coef* InvLevelScale4x4_Intra	= dequantcoef;
	TDequantArray_coef* InvLevelScale4x4_Inter	= (avdUInt8*)dequantcoef + sizeof(TDequantArray_coef);
	TDequant8Array_coef* InvLevelScale8x8_Intra = dequantcoef8;
	TDequant8Array_coef* InvLevelScale8x8_Inter = (avdUInt8*)dequantcoef8 + sizeof(TDequant8Array_coef);
	avdUInt16 (*InvLevelScale4x4)[4] = NULL;
	avdUInt16 (*InvLevelScale8x8)[8] = NULL;
	int intra = IS_INTRA (currMB);
	int		IsFrame = img->dec_picture->structure==FRAME;
#endif



	//DataPartition *dP;
	//const avdUInt8 *partMap; //= avd_assignSE2partition[currSlice->dp_mode];

#if FEATURE_INTERLACE
	avdUInt8 *coefScan = ((img->specialInfo->structure == FRAME) && !mbIsMbField(currMB)) ? 
SNGL_SCAN_TO_INDEX : FIELD_SCAN_TO_INDEX;
#else//FEATURE_INTERLACE
	avdUInt8 *coefScan =SNGL_SCAN_TO_INDEX;
#endif//FEATURE_INTERLACE


	//avdNativeInt smb       = ((img->type==SP_SLICE) && IS_INTER (currMB)) || 
	//	(img->type == SI_SLICE && mbGetMbType(currMB) == SI4MB);

	currSlice = img->currentSlice;



	//StartRunTimeClock(DT_READCOEFFMEMSET0);
	// TBD: AVD_TI_C55x, long long is 40 bits not 64bits;
	// TBD: Intel compiler does not generate ldrd; 
	// initialize it to zeros;
	//assert(((avdInt32)numRuns & 7) == 0);
	tmp64Pt = (avdInt64 *)numRuns; // size is NUMBER_RUN_SIZE * sizeof(avdUInt8);
	tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = 0;
	*(avdInt32 *)(numRuns + 24) = 0;
	EndRunTimeClock(DT_READCOEFFMEMSET0);

	// read CBP if not new intra mode
	if (!newIntra)
	{
		avdNativeInt value;
		TIME_BEGIN(start)
		
		Is4x4Intra = (IS_4x4_INTRA (currMB) || mbGetMbType(currMB) == SI4MB||IS_8x8_INTRA(currMB));

		TRACE_STRING("coded_block_pattern");

		readCBP_CABAC(img,info,&currSE,bs);
		value = currSE.value1;

		parser->cbp = cbp = value;
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\ncbp(%d)",  cbp);
#endif
		// Delta quant only if nonzero coeffs
		if (cbp)
		{

#if FEATURE_T8x8
			int mbType=mbGetMbType(currMB);
			int direct_8x8_inference_flag=img->active_sps->direct_8x8_inference_flag;
			int isDirect=IS_DIRECT(currMB);
			int need_transform_size_flag;
			int NoMbPartLessThan8x8Flag = ( isDirect&& !(direct_8x8_inference_flag))? 0: 1;
			if (mbType==P8x8)
			{
				int i;
				for (i = 0; i < 4; ++i)
				{
					int b8Mode= mbBits->uMBS.b8Info.b8Mode[i]; //mbGetB8Mode(currMB,i);
					NoMbPartLessThan8x8Flag &= (b8Mode==0 && direct_8x8_inference_flag) ||
						(b8Mode==4);
				}
			}
			need_transform_size_flag = (((mbType >= 1 && mbType <= 3)||
				(isDirect && direct_8x8_inference_flag) ||
				NoMbPartLessThan8x8Flag)
				&&(cbp&15)
				&& mbType != I8MB && mbType != I4MB
				&& img->Transform8x8Mode);
			if (need_transform_size_flag)
			{
				AvdLog(DUMP_SLICE,DCORE_DUMP"need_transform_size_flag:mbType=%d,isDirect=%d,direct_8x8_inference_flag=%d,NoMbPartLessThan8x8Flag=%d,8x8Mode=%d\n",mbType,isDirect,direct_8x8_inference_flag,NoMbPartLessThan8x8Flag,img->Transform8x8Mode);
				currSE.type   =  SE_HEADER;
				//dP = &(currSlice->partArr[partMap[SE_HEADER]]);
				TRACE_STRING("transform_size_8x8_flag");

				readMB_transform_size_flag_CABAC(img,info,currMB, &currSE, bs);


				if (currSE.value1)
				{
					mbMarkMbT8x8Flag(currMB);
				}
			}
#endif//FEATURE_T8x8

			if (IS_INTER (currMB))  
				currSE.type = SE_DELTA_QUANT_INTER;
			else                    
				currSE.type = SE_DELTA_QUANT_INTRA;


			TRACE_STRING("mb_qp_delta");

#if 0
			{
				 int i =0;
				 
				 for(i = 0; i < 16; i++)
				   AvdLog2(LL_ERROR, "%02x ", bs->currReadBuf[i]);
			}

			AvdLog2(LL_ERROR, "===> data\n");
#endif
			readDquant_CABAC(img,info,&currSE,bs); //gabi
			value = currSE.value1;
			
   //                  AvdLog2(LL_ERROR, "mb_qp_delta %d \n", value );
			

			if(value>25||value<-26)
			{
				currSE.value1 = 0;
				AVD_ERROR_CHECK2(img,"mb_qp_delta out of range",ERROR_InvalidMbQPDelta);
			}

			//if(1) img->qp = img->clipInfo->clipQP51[img->qp-MIN_QP+value];else 
			{
				int tmp = img->qp+value+52;
				while(tmp>=52)
					tmp -= 52;
				if(tmp<0)
					tmp = 0;
				img->qp = tmp;
			}
			//(MAX_QP-MIN_QP+1))%(MAX_QP-MIN_QP+1)+MIN_QP;
#if (DUMP_VERSION & DUMP_SLICE)
			AvdLog(DUMP_SLICE,DUMP_DCORE "\ndeltaQP(%d), QP(%d)", value, img->qp);
#endif
		}
		TIME_END(start,readCBPSum)
	}
	else {
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\n16x16 LumaDC");
#endif
		TIME_BEGIN(start)
		cbp = parser->cbp;
		currSE.type = SE_DELTA_QUANT_INTRA;
		readDquant_CABAC(img,info,&currSE,bs);
		//if(1) img->qp = img->clipInfo->clipQP51[img->qp-MIN_QP+currSE.value1];else 
		//img->qp= img->clipInfo->clipQP51[img->qp-MIN_QP+currSE.value1];
		{
			int tmp = img->qp+currSE.value1+52;
			while(tmp>=52)
				tmp -= 52;
			if(tmp<0)
				tmp = 0;
			img->qp = tmp;
		}
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\ndeltaQP(%d), QP(%d)", currSE.value1, img->qp);
#endif
		cbTypeIdx = COEFF_LUMA_INTRA16x16DC_IDX;




		currSE.type = SE_LUM_DC_INTRA;

		currSE.context      = LUMA_16DC;
		readRunLevelArray_CABAC(img,info,&currSE, bs, levarr, runarr, &numcoeff);



		if (numcoeff || (cbp&15)){
			// GetMBCofBuf(info) is always 8 bytes aligned;
			// TBD: Intel compiler does not generate ldrd;
			tmp64Pt = (avdInt64 *)GetMBCofBuf(info); // size is 384 * sizeof(avdInt16) * 2/3 for luma;
			i = 8; // 384 * 2 / 8 * 2/3 = 64
			do {
				tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 
					tmp64Pt[4] = tmp64Pt[5] = tmp64Pt[6] = tmp64Pt[7] = 0;
				tmp64Pt += 8;
			} while (--i);

			if (numcoeff){
				cofPt = GetMBCofBuf(info);
				tmpLevarr = levarr;
				tmpRunarr = runarr;
				coefLocation = (coefScan - 1);
				k = numcoeff;
				do {
					avdNativeInt lev, run;
#if (DUMP_VERSION & DUMP_SLICE)
					AvdLogLevel(img,info,DUMP_SLICE, "\nlevel(%d), run(%d)", *tmpLevarr, *tmpRunarr);
#endif
					run = *tmpRunarr++;
					lev = *tmpLevarr++;
					// use cofPt[6][4][4][4] offset;
					coefLocation += (run+1);
					// get j<<6 + i<<4

					if((coefLocation - coefScan) & 0xffff0000) // <0 or > 15;
						AVD_ERROR_CHECK2(img,"0: coef_ctr<0||coef_ctr>15",ERROR_InvalidBitstream);

					cofPt[((*coefLocation<<6)|(*coefLocation<<2))&0xf0]= lev;// add new intra DC coeff
				} while (--k);

				numRuns[cbTypeIdx] = (avdUInt8)numcoeff;
				itrans_2(img,info);// transform new intra DC
#if FEATURE_CABAC//TODOBframe
				parser->cbpBits |= (((avdUInt32)1)<<cbTypeIdx);
#endif//FEATURE_CABAC
			}
		}

		TIME_END(start,readLumaDC16x16Sum)
	}
	mbSetQP(currMB, img->qp);

	if (!cbp)
		return;

	TIME_BEGIN(start)
	if (cbp&15){
		// img->qp may be changed;
		yTmp	  = img->qp-MIN_QP;
		div_qp_per = DIVIDE6[yTmp];
		qp_per    = div_qp_per * 6;
		qp_rem    = yTmp - qp_per;
		// QPI
#if FEATURE_MATRIXQUANT
		if(img->initMatrixDequantDone)
		{
			InvLevelScale4x4 = intra? (*InvLevelScale4x4_Intra)[0][qp_rem] : (*InvLevelScale4x4_Inter)[0][qp_rem];
		}
		else
#endif//FEATURE
			dequanPt  = &img->dequant_coef_shift[DEQUANT_IDX(qp_rem,0,0)] + (qp_per<<4);

		// luma coefficients

		{
			currSE.context = newIntra ? LUMA_16AC : LUMA_4x4;
			currSE.type = (IS_INTRA(currMB) ?
				(newIntra ? SE_LUM_AC_INTRA : SE_LUM_DC_INTRA) :
				(newIntra ? SE_LUM_AC_INTER : SE_LUM_DC_INTER));              
			//dP = &(currSlice->partArr[partMap[currSE.type]]);
#if FEATURE_T8x8
			if (mbIsMbT8x8Flag(currMB))
			{	
				avdUInt8* dequanPt8  = &dequant_coef8[DEQUANT_IDX8(qp_rem,0,0)];// + (qp_per<<6);
				int kPos;
				runarr = levarr+64;//it is important 
#if FEATURE_MATRIXQUANT
				if(img->initMatrixDequantDone)
				{
					InvLevelScale8x8 = intra? (*InvLevelScale8x8_Intra)[0][qp_rem] : (*InvLevelScale8x8_Inter)[0][qp_rem];

				}
				else
#endif//FEATURE
					dequanPt  = &img->dequant_coef_shift8[DEQUANT_IDX8(qp_rem,0,0)] + (qp_per<<6);

				currSE.context = LUMA_8x8;
				for (kPos = 0; kPos < 4; kPos++){
					if (!(cbp & (1<<(kPos)))){  /* are there any coeff in current block at all */
						//ll += 3;
						continue;
					} //if (!(ll&3))
#if (DUMP_VERSION & DUMP_SLICE)
					//AvdLog(DUMP_SLICE,DUMP_DCORE "\nCoef: type(Luma/%d)", ll);
#endif //(DUMP_VERSION & DUMP_SLICE)
					j0 = T8x8KPos[kPos];
					ll=inv_decode_scan[j0];
					//posInfo->subblock_x = i = decode_scan[ll]; // position for coeff_count ctx
					//posInfo->subblock_y = j = decode_scan[16+ll]; // position for coeff_count ctx
					//j0 = (j<<2) | i;
					i = (kPos&0x01) << 3;
					j = (kPos >> 1) << 3;
					coefLocation = IsFrame&&!mbIsMbField(info->currMB)?SNGL_SCAN8x8[0]:FIELD_SCAN8x8[0];//SNGL_SCAN8x8[0];//coefScan;
					//if (!newIntra)
					//	coefLocation--;
					cbTypeIdx = COEFF_LUMA_START_IDX + ll; // h8fTODO:in SubBlockScan order;
					/*
					* make distinction between INTRA and INTER coded
					* luminance coefficients
					*/
					readRunLevelArray_CABAC(img,info,&currSE, bs, levarr, runarr, &numcoeff);
					if (numcoeff){
						mbBits->iTransFlags |= (1<<ll)|(1<<(ll+1))|(1<<(ll+2))|(1<<(ll+3));
						//avdInt16* cofPt1,cofPt2,cofPt3;
						cofPt = &GetMBCofBuf(info)[kPos<<6];
						//cofPt1 = cofPt+16;
						//cofPt2 = cofPt+32;
						//cofPt3 = cofPt+64;
						if (1)//!newIntra)
						{
							// GetMBCofBuf(info) is always 8 bytes aligned;
							tmp64Pt = (avdInt64 *)cofPt; // size is 384 * sizeof(avdInt16) * 2/3;
							tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
							tmp64Pt[4] = tmp64Pt[5] = tmp64Pt[6] = tmp64Pt[7] = 0;
							tmp64Pt[8] = tmp64Pt[9] = tmp64Pt[10] = tmp64Pt[11] = 0;
							tmp64Pt[12] = tmp64Pt[13] = tmp64Pt[14] = tmp64Pt[15] = 0;
						}
						// TBD: use tmpLevarr, tmpRunarr, and replace GetMBCofBuf(info)Type[j0] with local var;
						for (k = 0; k < numcoeff; k++)
						{
							int i1;//=coefLocation[1]*8+coefLocation[0];
							coefLocation += 2*runarr[k];
							i1=coefLocation[1]*8+coefLocation[0];
							i0 = i1;//+i;
#if FEATURE_MATRIXQUANT
							if(img->initMatrixDequantDone)
							{
								cofPt[i0] = rshift_rnd_sf((levarr[k] * InvLevelScale8x8[coefLocation[1]][coefLocation[0]])<<div_qp_per, 6);
							}
							else
#endif//FEATURE_MATRIXQUANT
								cofPt[i0] = (levarr[k] * dequanPt[i1]+3)>>2;//h8ftodo20100130:double check spec and ref code again


#if (DUMP_VERSION & DUMP_SLICE)
							AvdLogLevel(img,info,DUMP_SLICE, "\nlevel(%d), run(%d),(%d,%d,%d)val=%d", levarr[k], runarr[k],i,coefLocation[0],coefLocation[1],cofPt[i0]);
#endif //(DUMP_VERSION & DUMP_SLICE)
							coefLocation+=2;
						}// for k;
						numRuns[cbTypeIdx] = (avdUInt8)numcoeff;
						mbSetCBPBlk(currMB,(1<<j0)|(1<<(j0+1))|(1<<(j0+4))|(1<<(j0+5)));
						parser->cbpBits |= (((avdUInt32)0x0f)<<cbTypeIdx);

					}
				} // for ll
			}
			else

#endif//FEATURE
			{
				for (ll = 0; ll < 16; ll++){
					if (!(cbp & (1<<(ll>>2)))){  /* are there any coeff in current block at all */
						ll += 3;
						continue;
					} //if (!(ll&3))
#if (DUMP_VERSION & DUMP_SLICE)
					//AvdLog(DUMP_SLICE, "\nCoef: type(Luma/%d)", ll);
#endif //(DUMP_VERSION & DUMP_SLICE)
					posInfo->subblock_x = i = decode_scan[ll]; // position for coeff_count ctx
					posInfo->subblock_y = j = decode_scan[16+ll]; // position for coeff_count ctx
					j0 = (j<<2) | i;
					coefLocation = coefScan;
					if (!newIntra)
						coefLocation--;
					cbTypeIdx = COEFF_LUMA_START_IDX + ll; // in SubBlockScan order;
					/*
					* make distinction between INTRA and INTER coded
					* luminance coefficients
					*/
					readRunLevelArray_CABAC(img,info,&currSE, bs, levarr, runarr, &numcoeff);
					if (numcoeff){
						mbBits->iTransFlags |= (1<<j0);
						cofPt = &GetMBCofBuf(info)[j0<<4];
						if (!newIntra){
							// GetMBCofBuf(info) is always 8 bytes aligned;
							tmp64Pt = (avdInt64 *)cofPt; // size is 384 * sizeof(avdInt16) * 2/3;
							tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
						}
						// TBD: use tmpLevarr, tmpRunarr, and replace GetMBCofBuf(info)Type[j0] with local var;
						for (k = 0; k < numcoeff; k++)
						{
							coefLocation += (runarr[k]+1);

							if((coefLocation - coefScan) & 0xffff0000)
							{
								AVD_ERROR_CHECK2(img,"3:coef_ctr<0||coef_ctr>15",ERROR_InvalidBitstream);
							}
							i0 = *coefLocation;
#if FEATURE_MATRIXQUANT
							if(img->initMatrixDequantDone)
							{

								cofPt[i0] = rshift_rnd_sf((levarr[k] * InvLevelScale4x4[i0&3][i0/4])<<div_qp_per, 4);

							}
							else
#endif//FEATURE_MATRIXQUANT
								cofPt[i0] = levarr[k] * dequanPt[i0];
#if (DUMP_VERSION & DUMP_SLICE)
							AvdLogLevel(img,info,DUMP_SLICE, "\nlevel(%d), run(%d)", levarr[k], runarr[k]);
							if(img->initMatrixDequantDone)
							{

								AvdLogDeq(DUMP_SLICE,info, "\n(%d,%d),deq=%d(%d,%d)", i0&3,i0/4,cofPt[i0],levarr[k] , InvLevelScale4x4[i0&3][i0/4]);	
							}
#endif
						}// for k;
						numRuns[cbTypeIdx] = (avdUInt8)numcoeff;
						mbSetCBPBlk(currMB,1 << j0) ;
						parser->cbpBits |= (((avdUInt32)1)<<cbTypeIdx);

					}
				} // for ll
			}//if T8x8 else

		}


	}
	TIME_END(start,readACSum)

	

	if(cbp>15)
	{
		// chroma 2x2 DC coeff
		TCLIPInfo	*clipInfo = img->clipInfo;
		avdNativeInt qp_uv = img->qp + img->active_pps->chroma_qp_index_offset;
		avdNativeInt *uPt;
		avdNativeInt   coFu[4];
		TIME_BEGIN(start)

		qp_uv = clipInfo->clipQP51[qp_uv];
		div_qp_per = DIVIDE6[QP_SCALE_CR[qp_uv-MIN_QP]];
		qp_per =  div_qp_per* 6;
		qp_rem = QP_SCALE_CR[qp_uv-MIN_QP] - qp_per;

		dequanPt  = &img->dequant_coef_shift[DEQUANT_IDX(qp_rem,0,0)] + (qp_per<<4);

		//if (!IsVLCCoding(img->active_pps))
		{
			currSE.context = CHROMA_DC;
			currSE.type    = (IS_INTRA(currMB) ? SE_CHR_DC_INTRA : SE_CHR_DC_INTER);
			//dP = &(currSlice->partArr[partMap[currSE.type]]);
		}
		yTmp = dequanPt[0];

		uPt = coFu;
		for (ll=0;ll<3;ll+=2)
		{
			int uv = ll>>1; 
#if (DUMP_VERSION & DUMP_SLICE)
			//AvdLog(DUMP_SLICE,DUMP_DCORE "\nCoef: type(ChromaDC/%s)", ll==0 ? "U" : "V");
#endif
			cbTypeIdx = (ll == 0) ? COEFF_CHROMA_U_DC_IDX : COEFF_CHROMA_V_DC_IDX;
#if FEATURE_MATRIXQUANT
			if(img->initMatrixDequantDone)
			{
				InvLevelScale4x4 = intra ? (*InvLevelScale4x4_Intra)[uv + 1][qp_rem] :  (*InvLevelScale4x4_Inter)[uv + 1][qp_rem];

			}
#endif//FEATURE

			cabacInfo->is_v_block = (ll>0);
			readRunLevelArray_CABAC(img,info,&currSE, bs, levarr, runarr, &numcoeff);

			cofPt = (avdInt16*)&GetMBCofBuf(info)[(16+ll)*16];
			if (numcoeff){
				avdUInt32 flag;
				avdNativeInt a,b,c,d, e;
				tmpLevarr = levarr;
				tmpRunarr = runarr;
				coef_ctr=-1;
				uPt[0] = uPt[1] = uPt[2] = uPt[3] = 0;
				k = numcoeff;
				do {
					avdNativeInt lev, run;
#if (DUMP_VERSION & DUMP_SLICE)
					AvdLogLevel(img,info,DUMP_SLICE, "\nlevel(%d), run(%d)", *tmpLevarr, *tmpRunarr);
#endif //(DUMP_VERSION & DUMP_SLICE)
					run = *tmpRunarr++;
					lev = *tmpLevarr++;
					coef_ctr += 1 + run;
#if FEATURE_MATRIXQUANT
					if(img->initMatrixDequantDone)
					{
						uPt[coef_ctr] = (((lev  * InvLevelScale4x4[0][0])<<div_qp_per)>>4);
					}
					else
#endif//FEATURE
						uPt[coef_ctr] = lev * yTmp;
				} while (--k);


				numRuns[cbTypeIdx] = (avdUInt8)numcoeff;

				parser->cbpBits |= (((avdUInt32)1)<<cbTypeIdx);

				//if (!smb) // check to see if MB type is SPred or SIntra4x4 
				{
					b = uPt[0]+uPt[1];
					d = uPt[2]+uPt[3];
					a = ((b + d)>>1);
					c = ((b - d)>>1);
					e = uPt[0]-uPt[1];
					d = uPt[2]-uPt[3];
					b = ((e + d)>>1);
					d = ((e - d)>>1);
				}

#if FEATURE_MATRIXQUANT
				if(img->initMatrixDequantDone)
				{
#if (DUMP_VERSION & DUMP_SLICE)
					AvdLogDeq(DUMP_SLICE,info, "\nihadamard2x2(%d,%d,%d,%d),%d)",a,b,c,d,InvLevelScale4x4[0][0]);		
#endif
				}
#endif//FEATURE
				if (a){
					tmp64Pt = (avdInt64 *)cofPt;
					tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
					mbBits->iTransFlags |= (1<<(16+ll));
				}
				// alias problem in GCC compiler if put below into if(a){};
				cofPt[0] = a;

				if (b){
					tmp64Pt = (avdInt64 *)(cofPt + 16);
					tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
					mbBits->iTransFlags |= (1<<(17+ll));
				}
				cofPt[16] = b;

				if (c){
					tmp64Pt = (avdInt64 *)(cofPt + 64);
					tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
					mbBits->iTransFlags |= (1<<(20+ll));
				}
				cofPt[64] = c;

				if (d){
					tmp64Pt = (avdInt64 *)(cofPt + 80);
					tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
					mbBits->iTransFlags |= (1<<(21+ll));
				}
				cofPt[80] = d;
			}
			else
				cofPt[0] = cofPt[16] = cofPt[64] = cofPt[80] = 0;


		}


		// chroma AC coeff, all zero from start_scan
		if (cbp>31){

			currSE.context = CHROMA_AC;
			currSE.type    = (IS_INTRA(currMB) ? SE_CHR_AC_INTRA : SE_CHR_AC_INTER);

			for (ll = 0; ll < 8; ll++){
#if (DUMP_VERSION & DUMP_SLICE)
				//AvdLog(DUMP_SLICE,DUMP_DCORE "\nCoef: type(ChromaAC/%s%d)", (ll<4?"U":"V"), (ll&3));
#endif //(DUMP_VERSION & DUMP_SLICE)
#if FEATURE_MATRIXQUANT
				if(img->initMatrixDequantDone)
				{
					int uv=ll>3;
					InvLevelScale4x4 = intra ? (*InvLevelScale4x4_Intra)[uv + 1][qp_rem] :  (*InvLevelScale4x4_Inter)[uv + 1][qp_rem];

				}
#endif//FEATURE
				i = chromaScan[ll];
				j = chromaScan[8+ll];	
				j0 = (j<<2)|i;
				//j1 = (1 << (((j - 4)<<1) + 16 + (i&1) + ((i>>1)<<2)));
				cbTypeIdx = COEFF_CHROMA_U_AC_START_IDX + ll;


				{
					cabacInfo->is_v_block = (ll>=4);
					posInfo->subblock_y = (j==4) ? 0 : 1;
					posInfo->subblock_x = (i&1);
					readRunLevelArray_CABAC(img,info,&currSE, bs, levarr, runarr, &numcoeff);
				} // if IsVLCCoding(img->active_pps);


				if (numcoeff){
					avdNativeInt index = j0<<4;

					//	if(i0+index>=24*16)//hbfTODO20061013:over the bound
					const avdNativeInt errIdx = 24*16 - index;

					mbBits->iTransFlags |= (1<<j0);
					cofPt = &GetMBCofBuf(info)[index];
					if (!cofPt[0]){
						tmp64Pt = (avdInt64 *)cofPt;
						tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
					}
					tmpLevarr = levarr;
					tmpRunarr = runarr;
					coefLocation = coefScan;
					k = numcoeff;
					do {
						avdNativeInt lev, run;
#if (DUMP_VERSION & DUMP_SLICE)
						AvdLogLevel(img,info,DUMP_SLICE, "\nlevel(%d), run(%d)", *tmpLevarr, *tmpRunarr);
#endif
						run = *tmpRunarr++;
						lev = *tmpLevarr++;
						coefLocation += (run+1);

						if((coefLocation - coefScan) & 0xffff0000)
						{
							AVD_ERROR_CHECK2(img,"4:coef_ctr<0||coef_ctr>15",ERROR_InvalidBitstream);
						}
						i0 = *coefLocation; // get columnIdx*4+rowIdx;


						if(i0 >= errIdx)//hbfTODO20061013:over the bound
						{
							AVD_ERROR_CHECK2(img,"bitstream error,memmory overbound",ERROR_NULLPOINT);
						}
#if FEATURE_MATRIXQUANT
						if(img->initMatrixDequantDone)
						{
							cofPt[i0] = rshift_rnd_sf((lev * InvLevelScale4x4[i0&3][i0/4])<<div_qp_per, 4);
#if (DUMP_VERSION & DUMP_SLICE)
							AvdLogDeq(DUMP_SLICE,info, "\n(%d,%d),deq=%d(%d,%d)", i0&3,i0/4,cofPt[i0],lev , InvLevelScale4x4[i0&3][i0/4]);		
#endif
						}
						else
#endif//FEATURE

							cofPt[i0] = lev * dequanPt[i0];
					} while (--k);					
					numRuns[cbTypeIdx] = (avdUInt8)numcoeff;
					parser->cbpBits |= (((avdUInt32)1)<<cbTypeIdx);

				}
			} // for ll;
		} //if (cbp<=31)
		TIME_END(start,readCDCSum)
	}

}




void read_motion_info_of_BDirect(ImageParameters *img,TMBsProcessor *info)
{
	Macroblock *currMB  = info->currMB;
	StorablePicture	*dec_picture = img->dec_picture;
	AVDStoredPicMotionInfo	*motionInfo = dec_picture->motionInfo;
	avdUInt8 fw_rFrame,bw_rFrame;
	struct storable_picture	* List1;

	avdUInt8 **ref_idx0, **ref_idx1;
	AVDMotionVector	**mv0, **mv1;
	avdNativeInt refIdx[2][3];
	TMBBitStream	*mbBits = GetMBBits(currMB);
	avdUInt8 	     *b8Pdir = mbBits->uMBS.b8Info.b8DirectDir;
	avdUInt8 	     *b8Step = mbBits->uMBS.b8Info.b8Step;
	avdUInt8 		*b8Mode = mbBits->uMBS.b8Info.b8Mode;
	////TMBsProcessor *info  = img->mbsProcessor;
	MacroBlock	*mbAInfo, *mbBInfo;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	avdNativeInt  imgblock_y  = posInfo->block_y;
	avdNativeInt  imgblock_x  = posInfo->block_x;
	avdNativeInt i8 = (imgblock_x>>1);
	avdNativeInt j8; //= (imgblock_y>>1);

	avdNativeInt  mbType = mbGetMbType(currMB);
	avdNativeInt  step = mbType==BSKIP_DIRECT?4:1;
	avdNativeInt  k,i,j,j4, i4, ii,jj, xTmp, yTmp,isMovingList1,step2,curr_mb_field,list_offset;
	int all4InOne = 0;
	TMVRef1		mvRef1={0};
	int					leftMBYForIntra = -1;
	avdNativeInt botMb = 0;
	int imgblock_y2 = imgblock_y;
#if FEATURE_INTERLACE
	botMb = !CurrMBisTop(info);
	curr_mb_field = (GetMbAffFrameFlag(img) && mbIsMbField(currMB));
	list_offset = !curr_mb_field ? 0 : botMb ? 4 : 2;
	imgblock_y = !curr_mb_field ? posInfo->block_y :
		botMb ? ((posInfo->block_y-4)>>1) : (posInfo->block_y>>1);
#endif//FEATURE_INTERLACE
	List1 = img->listX[LIST_1+list_offset][0];
	if(List1==NULL)
		AVD_ERROR_CHECK2(img,"list0[1][0] is NULL2\n",ERROR_InvalidBitstream);


	j8 = (imgblock_y2>>1);
	ref_idx0 = motionInfo->ref_idx0;
	mv0 = motionInfo->mv0;
	ref_idx1 = motionInfo->ref_idx1;
	mv1 = motionInfo->mv1;
	b8Step[0] = 
		b8Step[1] = 
		b8Step[2] = 
		b8Step[3] = mbGetMbType(currMB)==0?4:2;
	if(!GetMbAffFrameFlag(img))
	{
		mbAInfo = GetLeftMB(info);
		mbBInfo = GetUpMB(info);

		if (i8 > 0
			&&INSAMESLICE(currMB,mbAInfo)

			){
				refIdx[LIST_0][NEIGHBOR_A] = ref_idx0[j8][i8 - 1];
				refIdx[LIST_1][NEIGHBOR_A] = ref_idx1[j8][i8 - 1];
		}
		else 
			refIdx[LIST_0][NEIGHBOR_A] = refIdx[LIST_1][NEIGHBOR_A] 
		= AVD_INVALID_REF_IDX;

		refIdx[LIST_0][NEIGHBOR_C] = refIdx[LIST_1][NEIGHBOR_C] 
		= AVD_INVALID_REF_IDX;
		if (posInfo->block_y > 0
			&&INSAMESLICE(currMB,mbBInfo)

			){
				avdUInt8 * ref_idx01 = &ref_idx0[j8 - 1][i8];
				avdUInt8 * ref_idx11 = &ref_idx1[j8 - 1][i8];
				refIdx[LIST_0][NEIGHBOR_B] = *ref_idx01;
				refIdx[LIST_1][NEIGHBOR_B] = *ref_idx11;
				if ((i8>>1) < sizeInfo->PicWidthInMbs - 1
					&&INSAMESLICE(currMB,GetUpRightMB(info))
					){
						refIdx[LIST_0][NEIGHBOR_C] = *(ref_idx01+2);
						refIdx[LIST_1][NEIGHBOR_C] = *(ref_idx11+2);
				}
				else if(INSAMESLICE(currMB,GetUpLeftMB(info)))
				{
					// use Neighbor D;
					refIdx[LIST_0][NEIGHBOR_C] = *(ref_idx01-1);
					refIdx[LIST_1][NEIGHBOR_C] = *(ref_idx11-1);
				}
		}
		else {
			refIdx[LIST_0][NEIGHBOR_B] = refIdx[LIST_1][NEIGHBOR_B] 
			= AVD_INVALID_REF_IDX;
		}		
	}
	else
	{
		Macroblock* nMBs[3];
		avdNativeInt x4x4AB[3], y4x4AB[3];
		Get_Neighbor_ABCnRefIdx(img,info, 0, 0, BOTH_LISTS, 4, nMBs, 
			x4x4AB, y4x4AB, refIdx);	
	}
	
	fw_rFrame = min(refIdx[LIST_0][NEIGHBOR_A],refIdx[LIST_0][NEIGHBOR_B]);
	fw_rFrame = min(fw_rFrame,refIdx[LIST_0][NEIGHBOR_C]);
	bw_rFrame = min(refIdx[LIST_1][NEIGHBOR_A],refIdx[LIST_1][NEIGHBOR_B]);
	bw_rFrame = min(bw_rFrame,refIdx[LIST_1][NEIGHBOR_C]);

	for (i=0;i<4;i++) {

		int block_y,block_x,temp,blockIdx,block_y4;

		AVDMotionVector pmvfw = {0};
		AVDMotionVector pmvbw = {0};
		avdNativeInt pmvFwInit = 1, pmvBwInit = 1;
		step2 = b8Step[i];
		//b8Pdir[i] = mbBits->uMBS.b8Info.b8Pdir[i];
		if (b8Mode[i])
			continue;

		b8Pdir[i]  = 2;//default
		yTmp = ((i>>1)<<1)+2;
		xTmp = ((i&1)<<1)+2;
		block_y  = imgblock_y2 + yTmp - 2;
		block_y4 = imgblock_y + yTmp - 2;
		block_x  = imgblock_x + xTmp - 2;
		//block_y4 = imgblock_y2 + yTmp - 2;////common to save_allMV and not
		if (!fw_rFrame || !bw_rFrame){
			b8Step[i] = step2 = (!i && mbType!=P8x8 &&
				Are16Moving4x4FlagsSameInMB(List1, block_y4, block_x, temp)) ?

				4 : 

			Are4Moving4x4FlagsSameIn8x8Block(List1, block_y4, block_x, temp) ?

				2 : 1; 							

		}
		
		for(j = yTmp - 2; j < yTmp;j+=step2){
			int j4 = imgblock_y2+j;
			block_y4 = imgblock_y + j;//the value is for colPic
			j8 = (j4>>1);
			for(k = xTmp - 2; k < xTmp; k+=step2){
				i4 = imgblock_x+k;
				i8 = (i4>>1);
				if (fw_rFrame == AVD_INVALID_REF_IDX && 
					bw_rFrame == AVD_INVALID_REF_IDX)
				{
					ref_idx0[j8][i8] =
						ref_idx1[j8][i8] = 0;
					//j++;
					break; // finish 8x8 block; mv = 0 in default;
				}

				isMovingList1 = IsMoving4x4(List1,block_y4, i4);
				blockIdx = (4<<4)|(4<<12);//(step2<<4)|(step2<<12)|(j<<8)|k;

				if (fw_rFrame != AVD_INVALID_REF_IDX)
				{
					if  (fw_rFrame  ||isMovingList1)
					{
						if (pmvFwInit){
							pmvFwInit = 0;
							SetMotionVectorPredictor (img,info,blockIdx, LIST_0, 
								fw_rFrame, &pmvfw);
						}	
						AVD_ASSIGN_MV(&mv0[j4][i4], &pmvfw);

					}
					ref_idx0[j8][i8] = fw_rFrame;
				}
				if (bw_rFrame != AVD_INVALID_REF_IDX)
				{
					if  (bw_rFrame  ||isMovingList1)
					{
						if (pmvBwInit){
							pmvBwInit = 0;
							SetMotionVectorPredictor (img,info,blockIdx, LIST_1, 
								bw_rFrame, &pmvbw);
						}
						AVD_ASSIGN_MV(&mv1[j4][i4], &pmvbw);

					}
					ref_idx1[j8][i8] = bw_rFrame;
				}
				if      (ref_idx1[j8][i8]==AVD_INVALID_REF_IDX) 
					b8Pdir[i]  = 0;
				else if (ref_idx0[j8][i8]==AVD_INVALID_REF_IDX) 
					b8Pdir[i]  = 1;
//#if (!DUMP_VERSION)//for dump version, disable the function in order to compare with ref code

				if(mbType==BSKIP_DIRECT&&step2==2)
				{
					if(i==0)
					{
						mvRef1.mv0 = *(avdUInt32*)(&pmvfw);
						mvRef1.mv1 = *(avdUInt32*)(&pmvbw);
						mvRef1.ref0 = ref_idx0[j8][i8];
						mvRef1.ref1 = ref_idx1[j8][i8];
					}
					else
					{
						if(mvRef1.mv0 == *(avdUInt32*)(&pmvfw)
							&&mvRef1.mv1 == *(avdUInt32*)(&pmvbw)
							&&mvRef1.ref0 == ref_idx0[j8][i8]
						&&mvRef1.ref1 == ref_idx1[j8][i8])
						{
							all4InOne++;
						}

					}

				}
			}
		}

		if(*(avdUInt32*)(&pmvfw)||*(avdUInt32*)(&pmvbw))
			for (jj = 0; jj < step2; jj++){
				for (ii = (jj ? 0 : 1); ii < step2; ii++){
					//mv0[block_y+jj][block_x+ii].x = pmvfw.x;
					//mv0[block_y+jj][block_x+ii].y = pmvfw.y;
					AVD_ASSIGN_MV(&mv0[block_y+jj][block_x+ii], &pmvfw);
					//mv1[block_y+jj][block_x+ii].x = pmvbw.x;
					//mv1[block_y+jj][block_x+ii].y = pmvbw.y;
					AVD_ASSIGN_MV(&mv1[block_y+jj][block_x+ii], &pmvbw);
				}
			}

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
typedef AVDMotionVector					MVDArray[BLOCK_MULTIPLE][BLOCK_MULTIPLE]; 
void read_motion_info_from_NAL_pb_slice_CABAC (ImageParameters *img,TMBsProcessor *info,Bitstream* bs)
{
	Macroblock *currMB  = info->currMB;
	SyntaxElement currSE;
	Slice *currSlice    = img->currentSlice;
	StorablePicture	*dec_picture = img->dec_picture;
	AVDStoredPicMotionInfo	*motionInfo = dec_picture->motionInfo;
	AVDMotionVector pmv;
	avdNativeInt partmode        = (IS_P8x8(currMB) ? 4 : mbGetMbType(currMB));
	avdNativeInt step_h0         = BLOCK_STEP[partmode];
	avdNativeInt step_v0         = BLOCK_STEP[8+partmode];
	avdNativeInt i,j,k,step_h,step_v,curr_mvd, curr_mvd_y, mvRefStartY4x4;
	avdNativeInt mv_mode, i0, j0;
	avdNativeInt j8, i8, j4, i4, ii,jj, xTmp, yTmp;
	avdNativeInt ref_idx,vec,iTRb,iTRp,isMovingList1;
	avdNativeInt mv_scale = 0;
	avdNativeInt flag_mode,imgblock_y, imgblock_x;
	SliceType ipbType;
	avdUInt8 refframe;
	avdNativeUInt motionInfoNotExist;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	avdUInt8 *b8Pdir = mbBits->uMBS.b8Info.b8Pdir;	
	avdUInt8 *b8Mode = mbBits->uMBS.b8Info.b8Mode; 
	avdUInt8 **ref_idx0, **ref_idx1;
	AVDMotionVector	**mv0, **mv1;
	int mbType=mbGetMbType(currMB);
	TCABACInfo	*cabacInfo = img->cabacInfo;
	MVDArray* mvdArray;
	avdNativeInt bframe          = (img->type==B_SLICE);
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	imgblock_y = posInfo->block_y; 
	//mbSetMbFlag(info->currMB,currMB);
	mbSetMbB8FromImgB8(info->currMB,mbBits->uMBS.b8Info.b8Mode);
	//mbSetMbB8FromImgB8(img->mbsProcessor->leftMB,mbBits->uMBS.b8Info.b8Mode);

	if(img->notSaveAllMV)
		mvRefStartY4x4 = 0;
	else //SAVE_ALL_MV
		mvRefStartY4x4 = posInfo->block_y;


	imgblock_x = posInfo->block_x;
	ref_idx0 = motionInfo->ref_idx0;
	mv0 = motionInfo->mv0;
#if FEATURE_BFRAME
	ref_idx1 = motionInfo->ref_idx1;
	mv1 = motionInfo->mv1;
	//mbBits->hasDirect8x8 = IS_B8x8(currMB) && !(b8Mode[0] && b8Mode[1]&& b8Mode[2] && b8Mode[3]);
	// TBD: combine with P8x8 mode reading if there is no 4x4 IBlcok;
	if (IS_B8x8(currMB) && !(b8Mode[0] && b8Mode[1]&& b8Mode[2] && b8Mode[3]))
	{
		if (currSlice->direct_type!=B_DIRECT_TEMPORAL)
		{
			read_motion_info_of_BDirect(img,info);					
		}
		else
		{
			read_motion_info_of_BTempol(img,info);
		}
	} 
#endif //VOI_H264D_NON_BASELINE
	CHECK_ERR_RET_VOID
		//  If multiple ref. frames, read reference frame for the MB *********************************
		if(img->num_ref_idx_l0_active < 2) 
		{
			const avdNativeUInt i8 = (imgblock_x>>1);
#if FEATURE_BFRAME//hxy2010
			k = 0;
			do {
				if ((b8Mode[k] && HasB8FwPred(b8Pdir[k])))
					ref_idx0[(mvRefStartY4x4 + k)>>1][i8 + (k&1)] = 0;
			} while (++k < 4);
#else //FEATURE_BFRAME

			{
				int j8 = mvRefStartY4x4>>1;
				ref_idx0[j8][i8] = ref_idx0[j8][i8 + 1] = 
					ref_idx0[j8+1][i8] = ref_idx0[j8+1][i8 + 1] = 0; 
			}
#endif //FEATURE_BFRAME
		}
		else
		{
			avdNativeInt getRef = !IS_P8x8 (currMB) || bframe || !info->mbsParser->allrefzero;
			currSE.type = SE_REFFRAME;
			flag_mode = (img->num_ref_idx_l0_active == 2 ? 1 : 0);
			k = 0;
			do {
				avdNativeUInt tmpX, tmpY;
#if FEATURE_BFRAME
				if (b8Mode[k]==0 || HasNoB8FwPred(b8Pdir[k]))
					continue;
#endif //VOI_H264D_NON_BASELINE

				TRACE_STRING("ref_idx_l0");
				tmpX = ((k&1)<<1);
				tmpY = ((k>>1)<<1);          
				if (getRef)
				{
					currSE.context = (b8Mode[k] < 4) ? 0 : 1;
					posInfo->subblock_x = tmpX;
					posInfo->subblock_y = tmpY;          
					currSE.value2 = LIST_0;
					readRefFrame_CABAC(img,info,&currSE,bs);
					refframe = currSE.value1;

				}
				else
					refframe = 0;

				if(refframe<0||refframe>=img->num_ref_idx_l0_active)//valid [0,img->num_ref_idx_l0_active)
				{
					AVD_ERROR_CHECK2(img,"refframe<0||refframe>img->num_ref_idx_l0_active",ERROR_InvalidRefIndex);
				}


#if 0//(DUMP_VERSION & DUMP_SLICE)
				AvdLog(DUMP_SLICE,DUMP_DCORE "\nl0ref(%d), j,i=(%d,%d), num_ref %s 2", refframe, posInfo->subblock_y, posInfo->subblock_x,
					(flag_mode ? "is" : "not"));
#endif

				i0 = ((imgblock_x + tmpX)>>1);
				if(img->notSaveAllMV)
					j0 = (tmpY>>1);
				else //SAVE_ALL_MV
					j0 = ((posInfo->block_y + tmpY)>>1);

				ref_idx0[j0][i0] = refframe;
				if (step_h0 == 4){
					ref_idx0[j0][i0 + 1] = refframe;
					if (step_v0 == 4){ //16x16
						ref_idx0[j0 + 1][i0] = 
							ref_idx0[j0 + 1][i0 + 1] = refframe;
						break;
					}
					k++; //16x8;
				}
				else if (step_v0 == 4){ //8x16;
					ref_idx0[j0 + 1][i0] = refframe;
					if ((k == 0 && EqualBwPred(b8Pdir[k+1])) || k == 1)
						break;
				}
			} while (++k < 4);
		}

		//  If backward multiple ref. frames, read backward reference frame for the MB *********************************

#if FEATURE_BFRAME
		if(img->num_ref_idx_l1_active < 2)
		{
			for (k = 0; k < 4; k++){
				if (b8Mode[k]!=0 && HasB8BwPred(b8Pdir[k]))
					ref_idx1[(mvRefStartY4x4 + k)>>1][(imgblock_x>>1) + (k&1)] = 0;
			}
		}
		else
		{
			currSE.type = SE_REFFRAME;
			flag_mode = (img->num_ref_idx_l1_active == 2 ? 1 : 0);
			for (k = 0; k < 4; k++) {
				if (b8Mode[k]==0 || HasNoB8BwPred(b8Pdir[k]))
					continue;

				TRACE_STRING("ref_idx_l1");
				posInfo->subblock_x = ((k&1)<<1);
				posInfo->subblock_y = ((k>>1)<<1);          
				currSE.context = (b8Mode[k] < 4) ? 0 : 1;
				currSE.value2 = LIST_1;
				readRefFrame_CABAC(img,info,&currSE, bs);
				refframe = currSE.value1;
#if 0//DUMP_SLICE
				AvdLog(DUMP_SLICE,DUMP_DCORE "\nl1ref(%d), j,i=(%d,%d), num_ref %s 2", refframe, posInfo->subblock_y, posInfo->subblock_x,
					(flag_mode ? "is" : "not"));
				AvdLog(DUMP_BITS, "\nl1ref(%d), j,i=(%d,%d), num_ref %s 2", refframe, posInfo->subblock_y, posInfo->subblock_x,
					(flag_mode ? "is" : "not"));
#endif//DUMP_SLICE
				i0 = ((imgblock_x + posInfo->subblock_x)>>1);
				j0 = ((mvRefStartY4x4 + posInfo->subblock_y)>>1);
				ref_idx1[j0][i0] = refframe;
				if (step_h0 == 4){
					ref_idx1[j0][i0 + 1] = refframe;
					if (step_v0 == 4){ //16x16
						ref_idx1[j0 + 1][i0] = 
							ref_idx1[j0 + 1][i0 + 1] = refframe;
						break;
					}
					k++; //16x8;
				}
				else if (step_v0 == 4){ //8x16;
					ref_idx1[j0 + 1][i0] = refframe;
					if ((k == 0 && EqualFwPred(b8Pdir[k+1])) || k == 1)
						break;
				}
			}
		}
#endif //VOI_H264D_NON_BASELINE

		currSE.type = SE_MVD;

		//=====  READ FORWARD MOTION VECTORS =====
		j0 = 0;
		mvdArray = cabacInfo->mvd[0];
		do {

			i0 = 0;
			do {

				k=((j0>>1)<<1)|(i0>>1);

#if FEATURE_BFRAME
				if ((b8Mode[k] && HasB8FwPred(b8Pdir[k])))
#endif //VOI_H264D_NON_BASELINE
				{
					avdNativeUInt blkIdx;
					mv_mode  = b8Mode[k];
					step_h   = BLOCK_STEP[mv_mode];
					step_v   = BLOCK_STEP[8+mv_mode]; 
					// see definition in SetMotionVectorPredictor();
					blkIdx = (step_h<<4)|(step_v<<12);

					refframe = ref_idx0[(mvRefStartY4x4+j0)>>1][(imgblock_x+i0)>>1];        
					// below use | to replace "+", because if j = 1, jj = 0 Only;
					// j = 2, jj = 0, 1 only; j = 3, jj = 0, only;
					j = j0;
					do {
						j4 = mvRefStartY4x4 | j;
#if FEATURE_CABAC
						posInfo->subblock_y = j; // position used for context determination
#endif //VOI_H264D_NON_BASELINE
						i = i0;
						do {
							i4 = imgblock_x | i;            
							// first make mv-prediction
							SetMotionVectorPredictor (img,info,(blkIdx|(j<<8)|i), LIST_0, refframe, &pmv);
							posInfo->subblock_x = i; // position used for context determination
							currSE.value2 = 0; // identifies the component; only used for context determination
							readMVD_CABAC(img,info,&currSE,bs);
							curr_mvd = currSE.value1;               
							currSE.value2 = 2; 

							readMVD_CABAC(img,info,&currSE,bs);
							curr_mvd_y = currSE.value1;               
							pmv.x += curr_mvd;
							pmv.y += curr_mvd_y;
							jj = 0;
							do {
								avdUInt32 *tmpMV = (avdUInt32 *)&mv0[j4|jj][i4];
								AVDMotionVector* mvd2 = (*mvdArray)[j|jj];
								ii = 0;
								do {
									AVD_ASSIGN_MV2(&mvd2[i|ii],curr_mvd,curr_mvd_y);
									*tmpMV++ = *(avdUInt32 *)&pmv;
#if 0//(DUMP_VERSION & DUMP_SLICE)
									AvdLog(DUMP_SLICE,DUMP_DCORE "\n(h8f_tmp) j4=%d,i4=%d,MV=(%d,%d),jj=%d,ii=%d\n", 
										j4,i4,
										pmv.x,pmv.y,
										jj,ii);
#endif
								} while(++ii < step_h); 
							} while (++jj < step_v);
#if (DUMP_VERSION & DUMP_SLICE)
							AvdLog(DUMP_SLICE,DUMP_DCORE "\n(enc)l0mvd(%d,%d),mv(%d,%d) j,i=(%d,%d),refidx=%d", 
								curr_mvd, curr_mvd_y, 
								mv0[j4][i4].x,mv0[j4][i4].y, j, i,refframe);
#endif

						} while ((i+=step_h) < i0 + step_h0);
					} while ((j+=step_v) < j0 + step_v0); 
				}
			} while ((i0+=step_h0) < 4);
		} while ((j0+=step_v0) < 4);

		//=====  READ BACKWARD MOTION VECTORS =====
#if FEATURE_BFRAME
		currSE.type = SE_MVD;
		mvdArray = cabacInfo->mvd[1];
		for (j0=0; j0<4; j0+=step_v0)
			for (i0=0; i0<4; i0+=step_h0)
			{
				avdNativeUInt blkIdx;
				k=((j0>>1)<<1)|(i0>>1);
				if (b8Mode[k]==0 || HasNoB8BwPred(b8Pdir[k]))
					continue;

				mv_mode  = b8Mode[k];
				step_h   = BLOCK_STEP[mv_mode];
				step_v   = BLOCK_STEP[8+mv_mode];
				blkIdx = (step_h<<4)|(step_v<<12);
				refframe = ref_idx1[(mvRefStartY4x4+j0)>>1][(imgblock_x+i0)>>1];
				for (j=j0; j<j0+step_v0; j+=step_v)
				{
					j4 = mvRefStartY4x4 | j;
					posInfo->subblock_y = j; // position used for context determination
					for (i=i0; i<i0+step_h0; i+=step_h)
					{
						i4 = imgblock_x | i;            
						posInfo->subblock_x = i; // position used for context determination
						// first make mv-prediction
						SetMotionVectorPredictor (img,info,(blkIdx|(j<<8)|i), LIST_1, refframe, &pmv);
						currSE.value2 = 1; 
						readMVD_CABAC(img,info,&currSE,bs);

						curr_mvd = currSE.value1;               
						currSE.value2 = 3; 
						readMVD_CABAC(img,info,&currSE,bs);
						curr_mvd_y = currSE.value1;               
						pmv.x += curr_mvd;
						pmv.y += curr_mvd_y;
						for(jj=0;jj<step_v;jj++){
							avdUInt32 *tmpMV = (avdUInt32 *)&mv1[j4|jj][i4];
							AVDMotionVector* mvd2 = (*mvdArray)[j|jj];
							for(ii=0;ii<step_h;ii++)
							{
								AVD_ASSIGN_MV2(&mvd2[i|ii],curr_mvd,curr_mvd_y);
								*tmpMV++ = *(avdUInt32 *)&pmv;
							}
						}
						AvdLog(DUMP_SLICE,DUMP_DCORE "\n(enc)l1mvd(%d,%d),mv(%d,%d) j,i=(%d,%d),refidx=%d", 
							curr_mvd, curr_mvd_y, 
							mv1[j4][i4].x,mv1[j4][i4].y,j,i,refframe);

					}
				}
			}
#endif //VOI_H264D_NON_BASELINE
}

void read_ipred_modes_CABAC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs)
{
	SyntaxElement currSE;
	const avdUInt8 *partMap;
	Slice *currSlice;
	//DataPartition *dP;
	Macroblock *currMB = info->currMB;
	pic_parameter_set_rbsp_t *active_pps = img->active_pps;
	//avdNativeInt isEI,isVLCOREI,isVLC=1;
	avdNativeInt is8x8=0,is4x4=0; 
	TMBBitStream	*mbBits = GetMBBits(currMB);
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	int c_ipred_mode;
#if (DUMP_VERSION & DUMP_SLICE)
	int dumpI4Mode[4][4];
	int dumpFlag = 0;
	int i, j;
	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++)
			dumpI4Mode[j][i] = 9;
#endif //(DUMP_VERSION & DUMP_SLICE)
	currSlice = img->currentSlice;

#if FEATURE_T8x8
	is8x8 = IS_8x8_INTRA(currMB);
#endif
	is4x4 = IS_4x4_INTRA(currMB);
	if (is4x4||is8x8){ 

		MacroBlock *mbA, *mbB;
		avdNativeInt k, bx, by, addr;
		avdNativeInt mostProbableIntraPredMode;
		avdNativeInt upIntraPredMode;
		avdNativeInt leftIntraPredMode, bitShift;
		//TMBsProcessor *info  = img->mbsProcessor;
		avdUInt32			mode4x4;
		avdNativeInt		step = is4x4?1:4;
		int					leftMBYForIntra = -1;
		mbA = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,GetPosInfo(info)->subblock_y*4,16,&leftMBYForIntra):GetLeftMB(info);
		mbB = GetUpMB(info);
		if (active_pps->constrained_intra_pred_flag){
			//change definition of neighbor A, B;
			if (mbA && !IS_INTRA(currMB - 1))
				mbA = NULL;

			if (mbB && !niIsIntraMb(info, GetMBX(info)))
				mbB = NULL;
		}

		for (k = 0; k < 16; k+=step){ 
			avdNativeInt value;
			//avdNativeInt k2=Coef8x8[k];
			//get from stream

			currSE.context= k;
			readIntraPredMode_CABAC(img,info,&currSE,bs);
			value = currSE.value1;


			bx = decode_scan[k]; 
			by = decode_scan[16+k]; 

			if (by){
				upIntraPredMode = mbBits->uMBS.mbIntra4x4Mode[((by - 1)<<2) | bx];
			}
			else{

				if (mbB){
					upIntraPredMode = (!active_pps->constrained_intra_pred_flag || 
						IS_INTRA_MB_ADDRESS2(mbB)) ?
						(IS_4x4_INTRA_MB_ADDRESS3(mbB) ? 
						niGetIntra4x4Mode(mbB, bx) : DC_PRED) : AVD_INVALID_INTRAMODE;
				}
				else 
					upIntraPredMode = AVD_INVALID_INTRAMODE;

			}

			if (bx){
				leftIntraPredMode = mbBits->uMBS.mbIntra4x4Mode[(by<<2) | (bx - 1)];
			}
			else{

				if (mbA){
					int idx = by;
					MacroBlock* leftMB = mbA;
					if(GetMbAffFrameFlag(img))
					{
						leftMB = GetLeftMBAffMB2(img,info,by*4,16,&leftMBYForIntra);
						idx = leftMBYForIntra>>2;	
					}
					leftIntraPredMode =  
						(!active_pps->constrained_intra_pred_flag || 
						IS_INTRA_MB_ADDRESS2(leftMB)) ?
						(IS_4x4_INTRA_MB_ADDRESS3(leftMB) ? 
						niGetIntra4x4ModeLeft(leftMB, idx) : DC_PRED) : AVD_INVALID_INTRAMODE;
				}
				else
					leftIntraPredMode = AVD_INVALID_INTRAMODE;

			}
			mostProbableIntraPredMode  = (upIntraPredMode == AVD_INVALID_INTRAMODE
				|| leftIntraPredMode == AVD_INVALID_INTRAMODE) ? 
DC_PRED : upIntraPredMode < leftIntraPredMode ? upIntraPredMode : 
		  leftIntraPredMode;
			{
				avdNativeInt index=(by<<2) | bx;
				avdNativeInt value2=(value == -1) ? mostProbableIntraPredMode : value + (value >= mostProbableIntraPredMode);
				mbBits->uMBS.mbIntra4x4Mode[index]=value2;
#if FEATURE_T8x8
				if(is8x8)
				{
					mbBits->uMBS.mbIntra4x4Mode[index+1]=value2; index+=4;
					mbBits->uMBS.mbIntra4x4Mode[index]=value2; 
					mbBits->uMBS.mbIntra4x4Mode[index+1]=value2; 
				}
#endif//#if FEATURE_T8x8

			}
			if (mostProbableIntraPredMode<0||mostProbableIntraPredMode>8)
			{
				mostProbableIntraPredMode = 0;
				AVD_ERROR_CHECK2(img,"mostProbableIntraPredMode<0||mostProbableIntraPredMode>8!", ERROR_InvalidIntraPredMode);
			}
#if (DUMP_VERSION & DUMP_SLICE)
			dumpFlag = 1;
#if FEATURE_T8x8
			if(is8x8)
			{
				dumpI4Mode[by][bx] =
					dumpI4Mode[by][bx+1] =
					dumpI4Mode[by+1][bx] =
					dumpI4Mode[by+1][bx+1] = mbBits->uMBS.mbIntra4x4Mode[(by<<2)|bx];
				_dumpIP4[mbBits->uMBS.mbIntra4x4Mode[(by<<2)|bx]]+=4;
			}
			else
#endif
			{
				dumpI4Mode[by][bx] = mbBits->uMBS.mbIntra4x4Mode[(by<<2)|bx];
				_dumpIP4[mbBits->uMBS.mbIntra4x4Mode[(by<<2)|bx]]++;

			}

#endif //(DUMP_VERSION & DUMP_SLICE)

		}


		// for 4x4 Intra;
		niClearIntra4x4Mode(info->currMB);
		niSetAllIntra4x4Mode(info->currMB, mbBits->uMBS.mbIntra4x4Mode[12], 
			mbBits->uMBS.mbIntra4x4Mode[13], mbBits->uMBS.mbIntra4x4Mode[14], mbBits->uMBS.mbIntra4x4Mode[15]);
		//niClearIntra4x4Mode(info->leftMB);
		niSetAllIntra4x4ModeLeft(info->currMB, mbBits->uMBS.mbIntra4x4Mode[3], 
			mbBits->uMBS.mbIntra4x4Mode[7], mbBits->uMBS.mbIntra4x4Mode[11], mbBits->uMBS.mbIntra4x4Mode[15]);
	}

#if (DUMP_VERSION & DUMP_SLICE)
	if (dumpFlag)
		for (i = 0; i < 4; i++)
			AvdLog(DUMP_SLICE,DUMP_DCORE "\n%5s(%d) %5s(%d) %5s(%d) %5s(%d)", 
			IPredName4x4[dumpI4Mode[i][0]], dumpI4Mode[i][0],
			IPredName4x4[dumpI4Mode[i][1]],dumpI4Mode[i][1],IPredName4x4[dumpI4Mode[i][2]],
			dumpI4Mode[i][2],IPredName4x4[dumpI4Mode[i][3]],dumpI4Mode[i][3]);
#endif //(DUMP_VERSION & DUMP_SLICE)



	readCIPredMode_CABAC(img,info,&currSE, bs);

	c_ipred_mode = (avdNativeUInt)currSE.value1;

	mbSetCPredMode(mbBits,c_ipred_mode);
	if (c_ipred_mode > PLANE_8 || c_ipred_mode<DC_PRED_8)
	{
		c_ipred_mode = DC_PRED_8;
		AVD_ERROR_CHECK2(img,"illegal chroma intra pred mode!\n", ERROR_InvalidIntraPredCMode);
	}

#if (DUMP_VERSION & DUMP_SLICE)
	AvdLog(DUMP_SLICE,DUMP_DCORE "\nchrm8: %s(%d)", IPredName8x8[c_ipred_mode], c_ipred_mode);
	_dumpIP8[c_ipred_mode]++;
#endif //(DUMP_VERSION & DUMP_SLICE)

}

avdNativeInt read_one_macroblock_i_slice_CABAC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs)
{
	// TBD: allocate one SE at img to reuse in everywhere;
	SyntaxElement currSE;
	Macroblock *currMB = info->currMB;
	Slice *currSlice = img->currentSlice;
	avdNativeInt i, j;
	avdNativeInt listIdx; // for list 0 & 1;
	avdInt32 *tmp32Pt;
	MacroBlock   *currMBInfo, *leftMBInfo;
	Bitstream *currStream = bs;//dP->bitstream;
	TFuncInfo	*funcInfo = img->funcInfo;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	TCABACInfo	*cabacInfo = img->cabacInfo;
#if (DUMP_VERSION & DUMP_SLICE)
	TPosInfo	*posInfo = GetPosInfo(info);
	TSizeInfo	*sizeInfo = img->sizeInfo;
	AvdLog(DUMP_SLICE,DUMP_DCORE "\n\n*** MB=%d", GetCurPos(info,sizeInfo));
#endif //(DUMP_VERSION & DUMP_SLICE)


	mbUnMarkSkipped(currMB);
#if FEATURE_INTERLACE
	if (GetMbAffFrameFlag(img))
	{
		if(CurrMBisTop(info))
		{
			
			Macroblock* nMBs[2]={0,0};
			NeighborAB_ABS_CABAC(img,info);
			
			readFieldModeInfo_CABAC(img,info,&currSE,currStream);
			mbSetMbFieldValue(currMB, currSE.value1);
#if (DUMP_VERSION & DUMP_SLICE)
			AvdLog(DUMP_SLICE,DUMP_DCORE "\nread mbFieldFlag(%d)", currSE.value1);
#endif //(DUMP_VERSION & DUMP_SLICE)
		}
		
		Init_MB_Neighbor_InfoMBAff(img,info, 1);
	}
#endif //VOI_H264D_NON_BASELINE

	readMB_typeInfo_CABAC(img,info,&currSE, currStream);

	mbUnMarkEi(currMB);
	mbSetMbType(currMB, currSE.value1);
	funcInfo->interpret_mb_mode(img,info);

	CHECK_ERR_RET_INT
	currMBInfo = info->currMB;
	//leftMBInfo = info->leftMB;

#if FEATURE_T8x8
	mbUnMarkMbT8x8Flag(currMB);
	if (IS_4x4_INTRA(currMB)&& img->Transform8x8Mode)//VOTO8x8
	{

		readMB_transform_size_flag_CABAC(img,info,currMB, &currSE, bs);
		if (currSE.value1)
		{
			mbMarkMbT8x8Flag(currMB);
			mbSetMbType(currMB,I8MB);
			MEMSET_B8MODE(I8MB);
			MEMSET_B8PDIR(B8NOPRED);
			AvdLog(DUMP_SLICE,DUMP_DCORE "\nI4MB->I8MB(%d)\n ", currSE.value1);

		}

	}
#endif//FEATURE_T8x8




	//niSetCabacSIBlock(currMBInfo, cabacInfo->siBlock);


	//--- init macroblock data ---

	mbSetMbB8FromImgB8(currMB, mbBits->uMBS.b8Info.b8Mode);
	mbResetItransFlag(mbBits); // only 24 bits used now;


	if(mbGetMbType(currMB) != IPCM)		
	{
		// intra prediction modes for a macroblock **********************************************
		if (IS_INTRA(currMB)){
			read_ipred_modes_CABAC(img,info,bs);CHECK_ERR_RET_INT
		}
		// read CBP and Coeffs  ***************************************************************

		funcInfo->read_CBP_and_coeffs_from_NAL (img,info,bs);CHECK_ERR_RET_INT

	}
	else
	{

		readIPCMcoeffsFromNAL_CABAC(img,info,bs);CHECK_ERR_RET_INT
	}
	return DECODE_MB;
}


avdNativeInt read_one_macroblock_pb_slice_CABAC(ImageParameters *img,TMBsProcessor *info,Bitstream* bs)

{
	// TBD: allocate one SE at img to reuse in everywhere;
	SyntaxElement currSE;
	Macroblock *currMB = info->currMB;
	Slice *currSlice = img->currentSlice;
	Macroblock *topMB = NULL;
	TMBsParser *parser = GetMBsParser(info);
	//TMBsProcessor *info  = img->mbsProcessor;
	avdNativeInt i, j;
	avdNativeInt listIdx; // for list 0 & 1;
	avdNativeInt prevMbSkipped=0;
	avdNativeInt check_bottom, read_bottom, read_top;
	pic_parameter_set_rbsp_t *active_pps = img->active_pps;
	avdInt32 *tmp32Pt;
	MacroBlock   *currMBInfo, *leftMBInfo;
	//avdNativeInt isVLC=1,isEI=1,isVLCOREI=1; 
	TFuncInfo	*funcInfo = img->funcInfo;
	//DataPartition *dP = &(currSlice->partArr[0]);
	Bitstream *currStream = bs;//dP->bitstream;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	TCABACInfo	*cabacInfo = img->cabacInfo;
	TPosInfo	*posInfo = GetPosInfo(info);
	int reset = 1;
	int inferenceField = 0;
	int start;
#if (DUMP_VERSION & DUMP_SLICE)

	TSizeInfo	*sizeInfo = img->sizeInfo;
	AvdLog(DUMP_SLICE,DUMP_DCORE "\n\n*** MB=%d", GetCurPos(info,sizeInfo));
#endif //(DUMP_VERSION & DUMP_SLICE)
#if FEATURE_INTERLACE
	if (GetMbAffFrameFlag(img) && !CurrMBisTop(info))
	{
		topMB = GetTopMB(info,info->mb_x);
		prevMbSkipped = mbIsSkipped(topMB);
	}
	//(CurrMBisTop(info)|| !mbIsMbField(topMB)) ? 
	//	mbUnMarkMbField(currMB) : mbMarkMbField(currMB);
#endif
	mbUnMarkSkipped(currMB);
	//mbSetQP(currMB, img->qp);



	// non I/SI-slice CABAC
	//if (!isVLC)
	{
		// read MB skipflag

		//Init_MB_Neighbor_Info(img);
		int mbType = 0;
		int curIsSkip;
		int curMbTypeIsZero;
		MacroBlock *left=NULL,*up=NULL;
		if (GetMbAffFrameFlag(img))
		{
			MacroBlock* nMBs[2]={NULL,NULL};
			NeighborAB_ABS_CABAC(img,info);
			left = GetLeftMB(info);
			up   = GetUpMB(info);
			if((CurrMBisTop(info)||prevMbSkipped))
			{
				int inferenceField = left? mbIsMbField(left):up?mbIsMbField(up) :0;
				mbSetMbFieldValue(currMB,inferenceField);

			}
			NeighborAB_Inference_CABAC2(img,info,
				currMB,
				info->mb_x,
				info->mb_y,
				nMBs);
		}
			//field_flag_inference(currMB);
		TRACE_STRING("mb_skip_flag");
		readMB_skip_flagInfo_CABAC(img,info,&currSE, currStream);
		
		//mbSetMbType(currMB, currSE.value1);
		mbType = currSE.value1;
		curIsSkip = !(currSE.value1);//mbIsSkipped(currMB);
		curMbTypeIsZero = mbType==0;
		if (img->type==B_SLICE)
		{
			parser->cbp = currSE.value2;
			if(!currSE.value1 && !currSE.value2)
				img->cod_counter=0;
		}
		//if(!isEI)
		mbUnMarkEi(currMB);
#if FEATURE_INTERLACE
		if (GetMbAffFrameFlag(img)) 
		{
			AvdLog(DUMP_SLICE,DCORE_DUMP"\nisSkipped=%d",curIsSkip);
			check_bottom=read_bottom=read_top=0;
			if (CurrMBisTop(info))
			{
				check_bottom = curIsSkip;
				read_top = !check_bottom;
			}
			else
			{
				int topIsSkip =  mbIsSkipped(topMB);
				//int topTypeIsZero = mbIsMbTypeZero(topMB) ;
				read_bottom = (topIsSkip && !curIsSkip);//(img->type!=B_SLICE) ? (topTypeIsZero && curMbTypeIsZero): (topIsSkip && !curIsSkip);
				if(!read_bottom)
					mbSetMbFieldValue(currMB, mbIsMbField(topMB));
					
			}
			if (read_bottom || read_top)
			{
				
				readFieldModeInfo_CABAC2(img,info,&currSE, currStream,left,up);
				mbSetMbFieldValue(currMB, currSE.value1);
				AvdLog(DUMP_SLICE, DUMP_DCORE"\nread mbFieldFlag(%d)", currSE.value1);
			}
			// need real mb_field to reconstruct skip mb;
			// can not use 7.4.4 inferred mb_field;
			// what if the whole pair skipped? Should we use inferred mb_field?
			if (check_bottom)
				check_next_mb_and_get_field_mode_CABAC(img,info,&currSE,currStream);
			
			CHECK_ERR_RET_INT
			
			Init_MB_Neighbor_InfoMBAff(img,info,1);//need to re-calcualte the neighbor
			
		}
		!curIsSkip ? mbUnMarkSkipped(currMB) : mbMarkSkipped(currMB);
		mbSetMbType(currMB, mbType);
		
#endif//FEATURE_INTERLACE



		listIdx = (img->type == B_SLICE) ? 2 : 1; // for list 0 & 1;
		// read MB type
		if (mbGetMbType(currMB))
		{
			readMB_typeInfo_CABAC(img,info,&currSE, currStream);
			mbSetMbType(currMB, currSE.value1);
			mbUnMarkEi(currMB);
		}
	}

	funcInfo->interpret_mb_mode(img,info);
	//mbBits->isDirect16x16 = (img->type == B_SLICE && !currSE.value1);
	CHECK_ERR_RET_INT


#if FEATURE_BFRAME//||FEATURE_BFRAME//TODOBframe
		currMBInfo = info->currMB;
	//leftMBInfo = info->leftMB;
#endif//FEATURE_BFRAME
#if FEATURE_T8x8
	mbUnMarkMbT8x8Flag(currMB);
	if (IS_4x4_INTRA(currMB)&& img->Transform8x8Mode)//VOTO8x8
	{

		TRACE_STRING("transform_size_8x8_flag");

		readMB_transform_size_flag_CABAC(img,info,currMB, &currSE, currStream);


		if (currSE.value1)
		{
			mbMarkMbT8x8Flag(currMB);
			mbSetMbType(currMB,I8MB);
			//memset(&currMB->b8mode, I8MB, 4 * sizeof(char));
			//memset(&currMB->b8pdir, -1, 4 * sizeof(char));
			MEMSET_B8MODE(I8MB);
			MEMSET_B8PDIR(B8NOPRED);
			AvdLog(DUMP_SLICE,DUMP_DCORE "\nI4MB->I8MB(%d)\n ", currSE.value1);

		}
	}
#endif//FEATURE_T8x8
	//====== READ 8x8 SUB-PARTITION MODES (modes of 8x8 blocks) and Intra VBST block modes ======
	if (IS_P8x8 (currMB))
	{
		for (i = 0; i < 4; i++)
		{
			TRACE_STRING("sub_mb_type");

			readB8_typeInfo_CABAC(img,info,&currSE, currStream);

			SetB8Mode (img,info,currMB, currSE.value1, i);CHECK_ERR_RET_INT
#if (DUMP_VERSION & DUMP_SLICE)
				AvdLog(DUMP_SLICE,DUMP_DCORE "subMbType(%d/%d), ", i, currSE.value1);
#endif
		}
	}

	//! End TO

	{
		int tmp0,tmp1,tmp2,tmp3,tmp4;
#define IsDirectBiDir(info,i) ((info)->uMBS.b8Info.b8Mode[(i)]==0&&(info)->uMBS.b8Info.b8Pdir[(i)]==2)
		niResetCabacDirectBiDir(currMB);
		tmp0 = IsDirectBiDir(mbBits,0);
		niSetCabacDirectBiDir(currMB,0,tmp0);
		tmp0 = IsDirectBiDir(mbBits,1);
		niSetCabacDirectBiDir(currMB,1,tmp0);
		tmp0 = IsDirectBiDir(mbBits,2);
		niSetCabacDirectBiDir(currMB,2,tmp0);
		tmp0 = IsDirectBiDir(mbBits,3);
		niSetCabacDirectBiDir(currMB,3,tmp0);
	}

	if (IS_DIRECT (currMB))
	{

		if (currSlice->direct_type!=B_DIRECT_TEMPORAL)
			read_motion_info_of_BDirect(img,info);
		else
			read_motion_info_of_BTempol(img,info);

		if(img->cod_counter >= 0)
		{
			parser->cbp = 0;
			{
				avdNativeInt size = 4;
				avdInt32 *tmp32Pt2 = (avdInt32 *)&niGetCabacNeighborStruct(currMBInfo)->mvd[0][0];
				avdInt32 *tmp32Pt = (avdInt32 *)&niGetCabacNeighborStruct(currMBInfo)->mvdLeft[0][0];
				do {
					*tmp32Pt++ = *tmp32Pt2++ = 0;
				} while (--size);
				img->cod_counter=-1;
			}

			return DECODE_MB;
		}			

	}


	if (IS_COPY (currMB)){ //keep last macroblock
		parser->cbp = 0;
		return GetPSliceSkippedMV(img,info);
	}

	//--- init macroblock data ---

	mbSetMbB8FromImgB8(currMB, mbBits->uMBS.b8Info.b8Mode);

	mbResetItransFlag(mbBits); // only 24 bits used now;


	if(mbGetMbType(currMB) != IPCM)
	{
		// intra prediction modes for a macroblock **********************************************
		if (IS_INTRA(currMB)){

TIME_BEGIN(start)
			read_ipred_modes_CABAC(img,info,bs);
TIME_END(start,readIpModeSum)
			CHECK_ERR_RET_INT
				if(img->notSaveAllMV&&img->type!=I_SLICE)
				{
					AVDStoredPicMotionInfo *motionInfo = img->dec_picture->motionInfo;
					//AvdLog(DUMP_SEQUENCE,"RIM2_intra prediction\n");
					//CHECK_MOTIONINFO(img,motionInfo);
					i = (posInfo->block_x>>1);
					motionInfo->ref_idx0[0][i] = 
						motionInfo->ref_idx0[0][i+1] = 
						motionInfo->ref_idx0[1][i] = 
						motionInfo->ref_idx0[1][i+1] = AVD_INVALID_REF_IDX;
					// only update MB boundary;
					tmp32Pt = (avdInt32 *)&motionInfo->mv0[0][posInfo->block_x].x;
					tmp32Pt[0] = tmp32Pt[1] = tmp32Pt[2] = tmp32Pt[3] = 0;
					tmp32Pt = (avdInt32 *)&motionInfo->mv0[1][posInfo->block_x].x;
					tmp32Pt[0] = tmp32Pt[3] = 0;
					tmp32Pt = (avdInt32 *)&motionInfo->mv0[2][posInfo->block_x].x;
					tmp32Pt[0] = tmp32Pt[3] = 0;
					tmp32Pt = (avdInt32 *)&motionInfo->mv0[3][posInfo->block_x].x;
					tmp32Pt[0] = tmp32Pt[1] = tmp32Pt[2] = tmp32Pt[3] = 0;
#if FEATURE_BFRAME
					if (img->type == B_SLICE){
						motionInfo->ref_idx1[0][i] = 
							motionInfo->ref_idx1[0][i+1] = 
							motionInfo->ref_idx1[1][i] = 
							motionInfo->ref_idx1[1][i+1] = AVD_INVALID_REF_IDX;
						// only update MB boundary;
						tmp32Pt = (avdInt32 *)&motionInfo->mv1[0][posInfo->block_x].x;
						tmp32Pt[0] = tmp32Pt[1] = tmp32Pt[2] = tmp32Pt[3] = 0;
						tmp32Pt = (avdInt32 *)&motionInfo->mv1[1][posInfo->block_x].x;
						tmp32Pt[0] = tmp32Pt[3] = 0;
						tmp32Pt = (avdInt32 *)&motionInfo->mv1[2][posInfo->block_x].x;
						tmp32Pt[0] = tmp32Pt[3] = 0;
						tmp32Pt = (avdInt32 *)&motionInfo->mv1[3][posInfo->block_x].x;
						tmp32Pt[0] = tmp32Pt[1] = tmp32Pt[2] = tmp32Pt[3] = 0;
					}
				}
#endif //VOI_H264D_BASELINE_PLUS_BFRAME
		}

		// read inter frame vector data *********************************************************
		if (IS_INTERMV (currMB)){

			AVD_ALIGNED32_MEMSET(&cabacInfo->mvd[0][0][0], 0, 16 * sizeof(avdInt16));
TIME_BEGIN(start)
			funcInfo->read_motion_info_from_NAL (img,info,bs);CHECK_ERR_RET_INT
TIME_END(start,readMotionSum)
			reset = 0;


				for (i = 0; i < listIdx; i++){
					for (j = 0; j < 4; j++){
						// mvd of neighborA is used, now it is safe to update mvd in neighbor;
						// for bottom row of current mb;
						int tmp = cabacInfo->mvd[i][3][j].x;
						niGetCabacNeighborStruct(currMBInfo)->mvd[i][j][0] = iClip3( -128, 127,tmp);
						tmp = cabacInfo->mvd[i][3][j].y;
						niGetCabacNeighborStruct(currMBInfo)->mvd[i][j][1] = iClip3( -128, 127,tmp);
						tmp = cabacInfo->mvd[i][j][3].x;
						niGetCabacNeighborStruct(currMBInfo)->mvdLeft[i][j][0] = iClip3( -128, 127,tmp);
						tmp = cabacInfo->mvd[i][j][3].y;
						niGetCabacNeighborStruct(currMBInfo)->mvdLeft[i][j][1] = iClip3( -128, 127,tmp);
						// save right boundary to be used in left boundary later;


					}
				}

		}
		

		// read CBP and Coeffs  ***************************************************************
		TIME_BEGIN(start)
		funcInfo->read_CBP_and_coeffs_from_NAL (img,info,bs);CHECK_ERR_RET_INT
		TIME_END(start,readCoefSum)

	}
	else
	{

		readIPCMcoeffsFromNAL_CABAC(img,info,bs);
		CHECK_ERR_RET_INT
	}

	if(reset)
	{
		AVD_ALIGNED32_MEMSET(&niGetCabacNeighborStruct(currMBInfo)->mvd[0][0], 0, 4);
		//if(leftMBInfo)
		AVD_ALIGNED32_MEMSET(&niGetCabacNeighborStruct(currMBInfo)->mvdLeft[0][0], 0, 4);
	}
	return DECODE_MB;
}
#if FEATURE_BFRAME
extern const avdUInt8 ICBPTAB[6];
const avdUInt8 offset2pdir16x16[12]   = {0, 0, 1, 2, 0,0,0,0,0,0,0,0};
const avdUInt8 offset2pdir16x8[22][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,1},{0,0},{0,1},{0,0},{1,0},
{0,0},{0,2},{0,0},{1,2},{0,0},{2,0},{0,0},{2,1},{0,0},{2,2},{0,0}};
const avdUInt8 offset2pdir8x16[22][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{1,1},{0,0},{0,1},{0,0},
{1,0},{0,0},{0,2},{0,0},{1,2},{0,0},{2,0},{0,0},{2,1},{0,0},{2,2}};

void interpret_mb_mode_B(ImageParameters *img,TMBsProcessor *info)
{

	Macroblock *currMB = info->currMB;
	avdNativeInt i;
	avdUInt8 newType, mbtype;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	TMBsParser *parser = GetMBsParser(info);
	int i16mode = 0;
	newType = mbtype = mbGetMbType(currMB);
	//--- set mbtype, b8type, and b8pdir ---
	if (mbtype==0)       // direct
	{
		MEMSET_B8MODE(0);
		MEMSET_B8PDIR(B8BIPRED);
		newType = 0;
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\n%s(%d):Bi", MBModeName[0], mbtype);
		_dumpMbMode[0]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
	}
	else if (mbtype==23) // intra4x4
	{
		MEMSET_B8MODE(IBLOCK);
		MEMSET_B8PDIR(B8NOPRED);
		newType = I4MB;
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\nI4MB, i4mode(%d):", mbtype);
		_dumpMbMode[10]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
	}
	else if (mbtype>23 && (mbtype<48)) // intra16x16
	{
		MEMSET_B8MODE(0);
		MEMSET_B8PDIR(B8NOPRED);
		parser->cbp = ICBPTAB[(mbtype-24)>>2];
		i16mode = (mbtype-24) & 0x03;
		newType = I16MB;
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\nI16MB: cbp = %d, i16mode = %s(%d)", 
			parser->cbp,IPredName16x16[i16mode], mbtype);
		_dumpIP16[i16mode]++;
		_dumpMbMode[9]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
	}
	else if (mbtype==22) // 8x8(+split)
	{
		newType = P8x8; // b8mode and pdir is transmitted in additional codewords
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\nP8x8(%d):", mbtype);
#endif //(DUMP_VERSION & DUMP_SLICE)

	}
	else if (mbtype<4)   // 16x16
	{
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\n%s(%d):", MBModeName[1], mbtype);
		_dumpMbMode[1]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
		MEMSET_B8MODE(1);
		for(i=0;i<4;i++) {
			mbBits->uMBS.b8Info.b8Pdir[i] = (avdUInt8)offset2pdir16x16[mbtype];
#if (DUMP_VERSION & DUMP_SLICE)
			AvdLog(DUMP_SLICE,DUMP_DCORE " %s", MCDirName[offset2pdir16x16[mbtype]]);
#endif //(DUMP_VERSION & DUMP_SLICE)
		}
		newType = 1;

	}
	else if(mbtype==48)
	{
		newType = IPCM;
		MEMSET_B8MODE(0);
		MEMSET_B8PDIR(B8NOPRED);
		parser->cbp = ALLCBPNONZERO;
		i16mode = 0;
	}
	else if (!(mbtype&1)) // 16x8
	{
		if(mbtype>=22)
		{
			AVD_ERROR_CHECK2(img,"16x8:mbtype>=22",ERROR_InvalidBitstream);
		}
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\n%s(%d):", MBModeName[2], mbtype);
		_dumpMbMode[2]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
		MEMSET_B8MODE(2);
		for(i=0;i<4;i++) {
			mbBits->uMBS.b8Info.b8Pdir[i] = (avdUInt8)offset2pdir16x8 [mbtype][i>>1];
#if (DUMP_VERSION & DUMP_SLICE)
			AvdLog(DUMP_SLICE,DUMP_DCORE " %s", MCDirName[offset2pdir16x8 [mbtype][i/2]]);
#endif //(DUMP_VERSION & DUMP_SLICE)
		}
		newType = 2;
	}
	else // 8x16
	{
		if(mbtype>=22)
		{
			AVD_ERROR_CHECK2(img,"8x16:mbtype>=22",ERROR_InvalidBitstream);
		}
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\n%s(%d):", MBModeName[3], mbtype);
		_dumpMbMode[3]++;
#endif //(DUMP_VERSION & DUMP_SLICE)
		MEMSET_B8MODE(3);
		for(i=0;i<4;i++) {
			mbBits->uMBS.b8Info.b8Pdir[i] = (avdUInt8)offset2pdir8x16 [mbtype][i&1];
#if (DUMP_VERSION & DUMP_SLICE)
			AvdLog(DUMP_SLICE,DUMP_DCORE " %s", MCDirName[offset2pdir8x16 [mbtype][i%2]]);
#endif //(DUMP_VERSION & DUMP_SLICE)
		}
		newType = 3;
	}
	mbSetMbType(currMB, newType);
	mbSetI16PredMode(mbBits,i16mode);
}
#endif//FEATURE_BFRAME

#endif//FEATURE_CABAC