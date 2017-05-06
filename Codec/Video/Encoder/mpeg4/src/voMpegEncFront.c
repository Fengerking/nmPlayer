	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2005				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		voMpegEncFront.c
*
* \brief
*		Implementation of VisualOn's MPEG-4 encoder interface functions
*
************************************************************************
*/
#include <string.h>
#include "voMpegEnc.h"
#include "midlevel/voMpegMemmory.h"
#include "midlevel/voMpegEncFrameProc.h"
#include "midlevel/voMpegEncFrame.h"

#include "midlevel/ratecontrol/voMpegEncRC.h"
#include "midlevel/voMpegEncBits.h"
#include "lowlevel/voMpegEncTable.h"
#include "lowlevel/voMpegEncQuant.h"

//#define VOLOG_TEST

//For android debug.
//Also need to add following macro in MAKEFILE:-D_LINUX_ANDROID -D_VOLOG_ERROR -D_VOLOG_WARNING -D_VOLOG_INFO
#ifdef VOLOG_TEST
#include "voLog.h"
#define LOG_TAG "VOMPEG4ENC"
int g_frameCount = 0;
#endif

#ifdef LICENSE_CHECK
#include "voCheck.h"
VO_HANDLE voMPEG4EnchInst = NULL;
#endif	

#ifdef MAX_RAM_USED_CHECK
FILE* fpMemLog;
int g_RamUsed = 0;
#endif

/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/

#ifdef RECON_FILE
FILE *rec_file, *file_rc, *yuvdata, *rc_file;
#endif
#ifdef _DEBUG
extern int ff_debug ; 
//extern FILE* fLog ;
#endif// 
/*TBD to clean up for API*/
#define FRAMERATE_INCR		1001
#define SMALL_EPS			(1e-10)

#define SWAP(_T_,A,B)    { _T_ tmp = A; A = B; B = tmp; }

#ifdef CAL_PSNR
#include <math.h>
VO_S32 sse_y;  
VO_S32 sse_u;   
VO_S32 sse_v;
#define SSEPSNR(sse, pel_count) ((!(sse))?0.0f : 48.131f - 10*(float)log10((float)(sse)/((float)((pel_count)))))
float total_psnr_y, total_psnr_u,total_psnr_v;
FILE* fp_PSNR;
int frame_count = 0;
#endif

extern  VO_S32 pred_mv_block_pos[4][3];

static __inline VO_VOID UpdateTC(ENCHND * enc_hnd)
{
	const VO_U32 time_base = enc_hnd->framerate_base;
	enc_hnd->enc_local.ticks = (VO_S32)enc_hnd->enc_local.cur_stamp % time_base;
	enc_hnd->enc_local.seconds =  ((VO_S32)enc_hnd->enc_local.cur_stamp / time_base) - 
        ((VO_S32)enc_hnd->enc_local.ref_stamp / time_base) ;
	enc_hnd->enc_local.ref_stamp = enc_hnd->enc_local.cur_stamp;
	enc_hnd->enc_local.cur_stamp += enc_hnd->framerate_incr;
#ifdef _DEBUG
	enc_hnd->sStat.d_framenum++;	/* debug ticker */
#endif
}

RETURN_CODE voMpegSetConfig(ENCHND *enc_hnd, CUSTOM_CONFIG* custom_config)
{
	VO_S32 framerate_incr, framerate_base;
	RC_CREATE rc_create;

#ifdef RECON_FILE
	rec_file = fopen("d:\\test\\recon1.yuv", "w+b");
	file_rc  = fopen("d:\\test\\quality.xls", "w+b");
//	yuvdata  = fopen("d:\\test\\yuvdata.yuv", "w+b");
//	rc_file  = fopen("d:\\test\\rc_file.xls", "w+b");
	fprintf(file_rc, "quality \t qp \t pred qp \t qp_min \t qp_max \t pred text \t real text \t\
		buffer level\t pred intra bits \t real intra bits \t pred inter bits \t real inter bits \t\
		intra error \t inter error \t pred error \t real mv \t pred mv \t header \t real all bits \t theta \t intra mb\n");
#endif

	if (custom_config->image_width&0xf || custom_config->image_height&0xf ||\
		custom_config->image_width<=0 || custom_config->image_height<=0)
		return INVALID_INPUT_SIZE;

	memset(&rc_create, 0, sizeof(RC_CREATE));
	enc_hnd->codec_id = custom_config->codec_id;

	/* setting size related parameters  */
	enc_hnd->img_width	= custom_config->image_width;
	enc_hnd->img_height = custom_config->image_height;
	enc_hnd->mb_width	= (enc_hnd->img_width + 15) >> 4;
	enc_hnd->mb_height	= (enc_hnd->img_height + 15) >> 4;
	enc_hnd->mb_num_len = log2bin(enc_hnd->mb_width*enc_hnd->mb_height);
	enc_hnd->img_mbsize = enc_hnd->mb_width * enc_hnd->mb_height;
	enc_hnd->edged_width  = (enc_hnd->mb_width << 4) + (EDGE_SIZE << 1);
	enc_hnd->edged_height = (enc_hnd->mb_height << 4) + (EDGE_SIZE << 1);
	enc_hnd->vp_byte_size = custom_config->vp_size;

	/* VO_VIDEO_CodingH263, setting size related parameters  */
	if(custom_config->codec_id == VO_INDEX_ENC_H263){
		if((enc_hnd->img_width == 128)&&(enc_hnd->img_height == 96)){
			enc_hnd->h263_pic_format = 1; /*sub_qcif*/
		}else if((enc_hnd->img_width == 176)&&(enc_hnd->img_height == 144)){
			enc_hnd->h263_pic_format = 2; /*qcif*/
		}else if((enc_hnd->img_width == 352)&&(enc_hnd->img_height == 288)){
			enc_hnd->h263_pic_format = 3; /*cif*/
		}else if((enc_hnd->img_width == 704)&&(enc_hnd->img_height == 576)){
			enc_hnd->h263_pic_format = 4; /*4cif*/
		}else if((enc_hnd->img_width == 1408)&&(enc_hnd->img_height == 1152)){
			enc_hnd->h263_pic_format = 5; /*16cif*/
		}else{
			enc_hnd->h263_pic_format = 6;
		}
		enc_hnd->EncInterMB =	 EncodeH263InterMB;
		enc_hnd->EncIntraMB =	 EncodeH263IntraMB;
		enc_hnd->WritePicHeader = H263WritePicHeader;		
	}else{
		enc_hnd->EncInterMB =	 EncodeMpeg4InterMB;
		enc_hnd->EncIntraMB =	 EncodeMpeg4IntraMB;
		enc_hnd->WritePicHeader = Mpeg4WriteVopHeader;
	}

	enc_hnd->enc_local.algorithm_flag |= VOI_RESTRICT_MV;/*TBD*/
	enc_hnd->enc_local.algorithm_flag |= VOI_ADAPTIVE_IFRAME;

	if(custom_config->enc_quality == VO_ENC_MID_QUALITY || custom_config->enc_quality == VO_ENC_HIGH_QUALITY){
#if	(defined(VOWMMX) || defined(VT) || defined(WIN32) || defined(VOARMV6) || defined(VOARMV7))
		enc_hnd->enc_local.algorithm_flag |= VOI_HALFPELREFINE16;
#endif
	}

	if(custom_config->enc_quality == VO_ENC_HIGH_QUALITY){
		enc_hnd->enc_local.algorithm_flag |= VOI_CHROMA_ME;
		enc_hnd->enc_local.algorithm_flag |= VOI_TRELLISQUANT;
	}


	if(custom_config->codec_id == VO_INDEX_ENC_MPEG4){
		enc_hnd->enc_local.algorithm_flag |= VOI_ACDCPRED;
#if	(defined(VOWMMX) || defined(VT) || defined(WIN32) || defined(VOARMV6) || defined(VOARMV7))
		enc_hnd->enc_local.algorithm_flag |=  VOI_INTER4V;
#endif
	}
	//enc_hnd->enc_local.algorithm_flag |= VOI_HQACPRED;

	// max keyframe interval 
    if (custom_config->max_key_frame_interval  > 0) {
        enc_hnd->max_Iframe = custom_config->max_key_frame_interval;
    }else {
	    enc_hnd->max_Iframe = (VO_S32) custom_config->framerate *10;
    }

	//Frame rate - Do some quick float fps = framerate_incr/framerate_base
	if ((custom_config->framerate - (VO_S32) custom_config->framerate) < SMALL_EPS) {
		framerate_incr = 1;
		framerate_base = (VO_S32) custom_config->framerate;
	} else {
		framerate_incr = FRAMERATE_INCR;
		framerate_base = (VO_S32) (FRAMERATE_INCR * custom_config->framerate);
	}

	enc_hnd->framerate_incr = MAX(framerate_incr, 0);
	enc_hnd->framerate_base = framerate_incr <= 0 ? 25 : framerate_base;

	rc_create.bitrate = custom_config->bitrate;
	rc_create.max_Iframe = enc_hnd->max_Iframe;
	rc_create.img_size = enc_hnd->img_width * enc_hnd->img_height;

#ifdef P_DOMAIN_RC
	MpegRCCreate(enc_hnd, &rc_create, enc_hnd->framerate_incr, enc_hnd->framerate_base, enc_hnd->img_mbsize, (RC_SINGLE**)&enc_hnd->rc_control);
#else
	MpegRCCreate(enc_hnd, &rc_create, enc_hnd->framerate_incr, enc_hnd->framerate_base, enc_hnd->img_mbsize, (RC_SINGLE**)&enc_hnd->rc_control);
#endif

	/* allocate working frame-frame memory */
	enc_hnd->img_cur = voMpegMalloc(enc_hnd, sizeof(Mpeg4Frame), CACHE_LINE);
	enc_hnd->img_ref = voMpegMalloc(enc_hnd, sizeof(Mpeg4Frame), CACHE_LINE);

	if (enc_hnd->img_cur == NULL || enc_hnd->img_ref == NULL)
		goto free_memory;

	/* allocate macroblock memory */
	enc_hnd->mbs = 	voMpegMalloc(enc_hnd, sizeof(MACROBLOCK) * enc_hnd->mb_width *
					enc_hnd->mb_height, CACHE_LINE);

	if (enc_hnd->mbs == NULL)
		goto free_memory;

	enc_hnd->pred_acdc_row = (VO_S16*) voMpegMalloc(enc_hnd, sizeof(VO_S16) * (enc_hnd->img_width*2 + 32), CACHE_LINE);
	enc_hnd->pred_acdc_col = (VO_S16*) voMpegMalloc(enc_hnd, sizeof(VO_S16) * 32 , CACHE_LINE);

	if(!enc_hnd->pred_acdc_row || !enc_hnd->pred_acdc_col)
		goto free_memory;

	FrameClean(enc_hnd->img_cur);
	FrameClean(enc_hnd->img_ref);

	if (FrameCreate(enc_hnd, enc_hnd->img_cur, enc_hnd->edged_width,enc_hnd->edged_height))
		goto free_memory;
	if (FrameCreate(enc_hnd, enc_hnd->img_ref, enc_hnd->edged_width,enc_hnd->edged_height))
		goto free_memory;

#ifdef CAL_PSNR
	if (FrameCreate(enc_hnd, &enc_hnd->img_ori, enc_hnd->edged_width, enc_hnd->edged_height))
		goto free_memory;
#endif

#ifdef P_DOMAIN_RC
	/* qcoeff + dctcoeff*/
	if(!(enc_hnd->work_space = (VO_U8*) voMpegMalloc(enc_hnd, sizeof(VO_U16) * (enc_hnd->img_mbsize + 1) * MB_SIZE , CACHE_LINE)))
		goto free_memory;
#else
	if(!(enc_hnd->work_space = (VO_U8*) voMpegMalloc(enc_hnd, sizeof(VO_U8) * MB_SIZE * 4, CACHE_LINE)))
		goto free_memory;
#endif
	/* timestamp stuff */
	enc_hnd->enc_local.ref_stamp = 0;
	enc_hnd->enc_local.cur_stamp = 0;
	enc_hnd->enc_local.prev_mvsigma = -1;

#ifdef _DEBUG
	enc_hnd->sStat.d_framenum = 0;
#endif
	enc_hnd->enc_local.frame_mum = 0;
    
    //init pred_mv_block table
    {//0 
        pred_mv_block_pos[0][0] = -1; //l
        pred_mv_block_pos[0][1] = 0 -1*enc_hnd->mb_width; //t
        pred_mv_block_pos[0][2] = 1 -1*enc_hnd->mb_width; //r

        //1
        pred_mv_block_pos[1][0] = 0; 
        pred_mv_block_pos[1][1] = 0 -1*enc_hnd->mb_width;   
        pred_mv_block_pos[1][2] = 1 -1*enc_hnd->mb_width;    

        //2 
        pred_mv_block_pos[2][0] = -1; 
        pred_mv_block_pos[2][1] = 0; 
        pred_mv_block_pos[2][2] = 0; 
        //
        pred_mv_block_pos[3][0] = 0; 
        pred_mv_block_pos[3][1] = 0; 
        pred_mv_block_pos[3][2] = 0; 
    }


	return VO_ERR_NONE; 

free_memory:
	voMpegFree(enc_hnd, enc_hnd->work_space);
	FrameDestroy(enc_hnd, enc_hnd->img_cur, enc_hnd->edged_width,
		enc_hnd->edged_height);
	
	FrameDestroy(enc_hnd, enc_hnd->img_ref, enc_hnd->edged_width,
		enc_hnd->edged_height);

	voMpegFree(enc_hnd, enc_hnd->mbs);
	voMpegFree(enc_hnd, enc_hnd->pred_acdc_row);
	voMpegFree(enc_hnd, enc_hnd->pred_acdc_col);
	voMpegFree(enc_hnd, enc_hnd->img_cur);
	voMpegFree(enc_hnd, enc_hnd->img_ref);

	MpegRCDestroy(enc_hnd, (RC_SINGLE*)enc_hnd->rc_control);

	voMpegFree(enc_hnd, enc_hnd);
	return INVALID_INTERNAL_MEM_MALLOC;

} //!< voMpegSetConfig()


RETURN_CODE voMpeg4EncFrame(FRAME_INFO_CONFIG* frame_info_config, VO_VOID *p_hnd, VO_U8 *input, VO_U8 *bitstream)
{
	VO_S32 type, length;
	BitStream bs;
	RETURN_CODE ret;
	ENCHND *enc_hnd = (ENCHND*)(p_hnd);

    //int a=-65536,b=0,c;
	
	if (!input) {
		return INVALID_INPUT_RAW_DATA_ADDR;
	}

    //b = a>>31;
    //c = a-(2*a&b);

	/* Initialize the bitstream*/
	InitBits(&bs, bitstream);//, 0);
	/* Set VP size, defalt is 0
	enc_hnd->vp_byte_size = 130;//TBD
	*/

#ifdef P_DOMAIN_RC
	if(SkipFrame((RC_SINGLE*)enc_hnd->rc_control)){
		/*EncodeFrameNULL(enc_hnd, &bs);*/		
		goto end_frame;
	}
#endif
	UpdateTC(enc_hnd);
	/*Input the raw data to current frame buffer or do preprocess*/
	if ((ret = FramePreProcess(enc_hnd->img_cur, frame_info_config->input_type, enc_hnd->custom_config.rt_type, enc_hnd->img_width, enc_hnd->img_height,
		enc_hnd->edged_width, input, enc_hnd->img_width))){
		return ret;
	}

#ifdef CAL_PSNR /* for PSRN*/
	FrameCopy(&enc_hnd->img_ori, enc_hnd->img_cur, enc_hnd->edged_width, enc_hnd->img_height);
#endif	

	if (frame_info_config->force_intra_frame == 1 || enc_hnd->enc_local.frame_mum == 0 ||\
		(enc_hnd->max_Iframe > 0 && enc_hnd->enc_local.frame_mum >= enc_hnd->max_Iframe)){
		enc_hnd->enc_local.frame_mum = 0;
		type = I_VOP;
	}else{
		type = P_VOP;
	}

	/* RC conctrol before a frame	TBD
	enc_hnd->enc_local.quant = MpegRCBefore((RC_SINGLE*)enc_hnd->rc_control, type);
	*/
	if (type == I_VOP || enc_hnd->force_iframe_num>0) {/* Encode I-vop */
		enc_hnd->force_iframe_num--;
		EncodeIVOP(enc_hnd, &bs, QPI);
		frame_info_config->frame_type = VO_VIDEO_FRAME_I;
	} else {/* Encode P-vop */
		 EncodePVOP(enc_hnd, &bs);
		 if(enc_hnd->enc_local.coding_type == I_VOP) {/* adaptive update the I frame*/
			enc_hnd->force_iframe_num--;
			InitBits(&bs, bitstream);//, 0);
			EncodeIVOP(enc_hnd, &bs, QPI);
			frame_info_config->frame_type = VO_VIDEO_FRAME_I;
		}else{
			frame_info_config->frame_type = VO_VIDEO_FRAME_P;
		}
	}

#ifdef CAL_PSNR
 	sse_y = PlaneSSE(enc_hnd->img_ori.y, enc_hnd->img_cur->y,
				enc_hnd->edged_width, enc_hnd->img_width,
				enc_hnd->img_height);
			
	sse_u = PlaneSSE(enc_hnd->img_ori.uv[0], enc_hnd->img_cur->uv[0],
				enc_hnd->edged_width/2, enc_hnd->img_width/2,
				enc_hnd->img_height/2);
			
	sse_v = PlaneSSE(enc_hnd->img_ori.uv[1], enc_hnd->img_cur->uv[1],
				enc_hnd->edged_width/2, enc_hnd->img_width/2,
				enc_hnd->img_height/2);

	{
		float curr_psnr_y = SSEPSNR(sse_y, enc_hnd->img_width * enc_hnd->img_height);
		float curr_psnr_u = SSEPSNR(sse_u, enc_hnd->img_width * enc_hnd->img_height / 4);
		float curr_psnr_v = SSEPSNR(sse_v, enc_hnd->img_width * enc_hnd->img_height / 4);
		total_psnr_y += curr_psnr_y;
		total_psnr_u += curr_psnr_u;
		total_psnr_v += curr_psnr_v;

		fprintf(fp_PSNR,"\nFrame %d: PSNR_Y:%4f, PSNR_U:%4f, PSNR_V:%4f", frame_count, curr_psnr_y, curr_psnr_u, curr_psnr_v);
	}
#endif

	/* the current FRAME becomes the reference */
	SWAP(Mpeg4Frame*, enc_hnd->img_cur, enc_hnd->img_ref);

	enc_hnd->enc_local.frame_mum++;
#ifdef P_DOMAIN_RC
end_frame:
#endif//P_DOMAIN_RC
	length = FlushBits(&bs);

#ifdef RECON_FILE
	if(length){
		VO_U32 i;
#ifdef P_DOMAIN_RC
		VO_S32 pred_text = ((RC_SINGLE*)enc_hnd->rc_control)->R;
		VO_S32 real_text = enc_hnd->sStat.text_bits;

		FrameCopy(&enc_hnd->img_ori, enc_hnd->img_cur, enc_hnd->edged_width, enc_hnd->img_height);
	
		if(length)
		fprintf(file_rc, " %d \t %ld \t %ld \t %ld \t %ld \t %ld \t %ld \t %ld \t %ld \t %ld \t %ld \t %ld \t %ld \t %d \t %d \t %d \t %ld \t %ld \t %ld \t %ld \t %f4 \t \n", \
			frame_info_config->frame_type,\
			((RC_SINGLE*)enc_hnd->rc_control)->quality,\
			((RC_SINGLE*)enc_hnd->rc_control)->qpall/((RC_SINGLE*)enc_hnd->rc_control)->img_mbsize,\
			enc_hnd->enc_local.quant,\
			((RC_SINGLE*)enc_hnd->rc_control)->qp_min,\
			((RC_SINGLE*)enc_hnd->rc_control)->qp_max,\
			pred_text,\
			real_text,\
			((RC_SINGLE*)enc_hnd->rc_control)->B,\
			((RC_SINGLE*)enc_hnd->rc_control)->R,\
			enc_hnd->sStat.text_bits,\
			((RC_SINGLE*)enc_hnd->rc_control)->R,\
			enc_hnd->sStat.text_bits,\
			VOGETABS(((RC_SINGLE*)enc_hnd->rc_control)->R - enc_hnd->sStat.text_bits),\
			VOGETABS(((RC_SINGLE*)enc_hnd->rc_control)->R - enc_hnd->sStat.text_bits),\
			VOGETABS(pred_text-real_text),\
			enc_hnd->sStat.mv_bits,\
			enc_hnd->sStat.mv_all,\
			enc_hnd->sStat.mb_hr_bits + enc_hnd->sStat.pic_hr_bits,\
			length*8,\
			((RC_SINGLE*)enc_hnd->rc_control)->theta);
#endif//P_DOMAIN_RC
//		if(enc_hnd->enc_local.frame_mum==30){
//			int a;
//			a = 1;
//		}
			for(i=0; i< enc_hnd->img_height; i++){
				fwrite(enc_hnd->img_ref->y+i*enc_hnd->edged_width, 
					sizeof(VO_U8)*enc_hnd->img_width,1,rec_file);
			}
			for(i=0; i< (enc_hnd->img_height/2); i++){
				fwrite(enc_hnd->img_ref->uv[0]+i*(enc_hnd->edged_width/2), 
					sizeof(VO_U8)*(enc_hnd->img_width/2),1,rec_file);
			}
			for(i=0; i< (enc_hnd->img_height/2); i++){
				fwrite(enc_hnd->img_ref->uv[1]+i*(enc_hnd->edged_width/2), 
					sizeof(VO_U8)*(enc_hnd->img_width/2),1,rec_file);
			}
	}
#endif//RECON_FILE

	// RC conctrol after a frame
	MpegRCAfter((RC_SINGLE*)enc_hnd->rc_control, 
                    enc_hnd->framerate_incr, 
                    enc_hnd->framerate_base, 
		            length, 
                    enc_hnd->enc_local.coding_type);

    //printf(" length = %d  ",length);

	return length;
}

RETURN_CODE voMpeg4EncFinish(VO_VOID *p_hnd)
{
	ENCHND *enc_hnd = (ENCHND*)(p_hnd);
	/* All images, reference, current etc ... */
//	if(!enc_hnd)
//		return VORC_ERR;

	FrameDestroy(enc_hnd, enc_hnd->img_cur, enc_hnd->edged_width, enc_hnd->edged_height);
	FrameDestroy(enc_hnd, enc_hnd->img_ref, enc_hnd->edged_width, enc_hnd->edged_height);
#ifdef CAL_PSNR
	FrameDestroy(enc_hnd, &enc_hnd->img_ori, enc_hnd->edged_width,
				  enc_hnd->edged_height);
#endif

	/* Encoder structure */
	if(enc_hnd->work_space){
		voMpegFree(enc_hnd, enc_hnd->work_space);
		enc_hnd->work_space = NULL;
	}
	if(enc_hnd->mbs){
		voMpegFree(enc_hnd, enc_hnd->mbs);
		enc_hnd->mbs = NULL;
	}

	voMpegFree(enc_hnd, enc_hnd->img_cur);
	enc_hnd->img_cur = NULL;
	voMpegFree(enc_hnd, enc_hnd->img_ref);
	enc_hnd->img_ref = NULL;
	MpegRCDestroy(enc_hnd, (RC_SINGLE*)enc_hnd->rc_control);

	if(enc_hnd->pred_acdc_row){
		voMpegFree(enc_hnd, enc_hnd->pred_acdc_row);
		enc_hnd->pred_acdc_row = NULL;
	}
	if(enc_hnd->pred_acdc_col){
		voMpegFree(enc_hnd, enc_hnd->pred_acdc_col);
		enc_hnd->pred_acdc_col = NULL;
	}
	
	voMpegFree(enc_hnd,enc_hnd);
	enc_hnd = NULL;

#ifdef RECON_FILE
	if(rec_file){
		fclose(rec_file);
		rec_file = NULL;
	}
	if(file_rc){
		fclose(file_rc);
		file_rc = NULL;
	}
	//if(yuvdata){
	//	fclose(yuvdata);
	//	yuvdata = NULL;
	//}
	//if(rc_file){
	//	fclose(rc_file);
	//	rc_file = NULL;
	//}
#endif
#ifdef _DEBUG
//	if(fLog){
//		fclose(fLog);
//		fLog = NULL;
//	}
#endif

	return (0);
} //!< voMpeg4EncFinish()

#ifdef NEW_SDK

VO_U32 VO_API voMPEG4ENCInit(VO_HANDLE * phEnc,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	VO_U32 codec_idx;
	VO_LIB_OPERATOR *pLibOP = NULL;
#ifdef LICENSE_CHECK
	VO_PTR phLicenseCheck = NULL;
	VO_U32 err;
#endif
	VO_U32 default_malloc;

#ifdef VOLOG_TEST	
	VOLOGE("\nvoMPEG4ENCInit start");
#endif

	switch(vType){
	case VO_VIDEO_CodingMPEG4:
		codec_idx = VO_INDEX_ENC_MPEG4;
		break;
	case VO_VIDEO_CodingH263:
		codec_idx = VO_INDEX_ENC_H263;
		break;
	default:
		return VO_ERR_NOT_IMPLEMENT;
	}

#ifdef LICENSE_CHECK
	if(pUserData){
		if(pUserData->memflag & 0XF0){//valid libOperator flag
			pLibOP = pUserData->libOperator;
		}
	}
	err = voCheckLibInit (&phLicenseCheck, codec_idx, 0, voMPEG4EnchInst, pLibOP);
	if(err != VO_ERR_NONE)
	{
		*phEnc = NULL;
		if(phLicenseCheck)
		{
			voCheckLibUninit(phLicenseCheck);
			phLicenseCheck = NULL;
		}
		return err;
	}
#endif

	default_malloc = 1;
	if(pUserData){
		if((pUserData->memflag & 0x0F) == VO_IMF_USERMEMOPERATOR) {
			VO_U8 *tmp, *mem_ptr;
			VO_MEM_OPERATOR* pMemOp = (VO_MEM_OPERATOR*)pUserData->memData;
			voMemAlloc(tmp, pMemOp, codec_idx, sizeof(ENCHND)+CACHE_LINE);
			if(tmp == NULL) {
				return(VO_ERR_OUTOF_MEMORY);
			}
			mem_ptr = (VO_U8 *)( (VO_U32) (tmp + CACHE_LINE - 1) & ( ~(VO_U32) (CACHE_LINE - 1) ) );
			if (mem_ptr == tmp)
				mem_ptr += CACHE_LINE;
			*(mem_ptr - 1) = (VO_U8) (mem_ptr - tmp);
			*phEnc = (VO_VOID *)mem_ptr;
			memset(mem_ptr, 0, sizeof(ENCHND));
			((ENCHND *)(*phEnc))->memOperater = pMemOp;
			default_malloc = 0;
		}
	}

	if(default_malloc){
		if(!(*phEnc = (ENCHND *) voMpegMalloc (NULL,sizeof(ENCHND),CACHE_LINE)))
			return VO_ERR_OUTOF_MEMORY;
		memset((*phEnc), 0, sizeof(ENCHND));
		((ENCHND *)(*phEnc))->memOperater = NULL;
	}

#ifdef LICENSE_CHECK	
	((ENCHND *)(*phEnc))->phLicenseCheck = phLicenseCheck; 
#endif

	((ENCHND *)(*phEnc))->custom_config.codec_id = codec_idx;
	((ENCHND *)(*phEnc))->custom_config.rt_type = VO_RT_DISABLE;

#ifdef MAX_RAM_USED_CHECK
	fpMemLog = fopen("d:\\Test\\mpeg4enc_mem_log.txt", "a+");
	g_RamUsed += sizeof(ENCHND);
#endif
#ifdef CAL_PSNR
	frame_count = 0;
	fp_PSNR = fopen("E:/MyResource/Video/clips/YUV/mpeg4enc_PSNR_log.txt", "a+");
#endif

#ifdef VOLOG_TEST	
	VOLOGE("\nvoMPEG4ENCInit end");
#endif

	 //init_fdct_enc();
	return VO_ERR_NONE;
}


/**
 * Encode one frame data.
 * \param hCodec [in] MPEG4 Encoder instance handle, returned by voMPEG4ENCInit().
 * \param pInData [in] Input buffer pointer. 
 * \param pOutData [in/out] Output buffer pointer and length. 
 *  pOutData->buffer should be alloced by Application, buffer size can get from voMPEG4ENCGetParameter(), ID is VO_PID_ENC_MPEG4_OUTBUF_SIZE.
 *  When the function return, pOutData->length indicates the encoded stream size.

 * \param FrameType [out] Output encoded frame type. 
 * \retval VO_ERR_NONE Succeeded.
 * \retval VO_ERR_WRONG_STATUS The Encoder is not ready to encode. Usually this indicates more parameters needed. See voMPEG4SetParameter().
 */
VO_U32 VO_API voMPEG4ENCProcess(VO_HANDLE hDec, VO_VIDEO_BUFFER * pInput, VO_CODECBUFFER * pOutput, VO_VIDEO_FRAMETYPE * pType)
{
	ENCHND *enc_hnd = (ENCHND *)hDec;
#ifdef LICENSE_CHECK
	VO_VIDEO_FORMAT	video_format;
#endif

#ifdef VOLOG_TEST	
	VOLOGE("\nvoMPEG4ENCProcess,frame count:%d", g_frameCount);
	VOLOGE("\nvoMPEG4ENCProcess start");
#endif

	if(enc_hnd->config != CONFIG_OK)
		return VO_ERR_WRONG_STATUS;
	if(pInput->ColorType == VO_COLOR_YUV_PLANAR420){
		enc_hnd->frame_info.input_type = VO_COLOR_YUV_PLANAR420;
	}else if(pInput->ColorType == VO_COLOR_UYVY422_PACKED){
		enc_hnd->frame_info.input_type = VO_COLOR_UYVY422_PACKED;
	}else if(pInput->ColorType == VO_COLOR_RGB565_PACKED){
		enc_hnd->frame_info.input_type = VO_COLOR_RGB565_PACKED;
	}else if(pInput->ColorType == VO_COLOR_YVU_PLANAR420){
		enc_hnd->frame_info.input_type = VO_COLOR_YVU_PLANAR420;
	}else if(pInput->ColorType == VO_COLOR_VYUY422_PACKED_2){
		enc_hnd->frame_info.input_type = VO_COLOR_VYUY422_PACKED_2;
	}else if(pInput->ColorType == VO_COLOR_UYVY422_PACKED_2){
		enc_hnd->frame_info.input_type = VO_COLOR_UYVY422_PACKED_2;
	}else if(pInput->ColorType == VO_COLOR_YUYV422_PACKED){
		enc_hnd->frame_info.input_type = VO_COLOR_YUYV422_PACKED;
	}else if(pInput->ColorType == VO_COLOR_YUV_PLANAR422_12){
		enc_hnd->frame_info.input_type = VO_COLOR_YUV_PLANAR422_12;
	}else if(pInput->ColorType == VO_COLOR_YUV_420_PACK_2){
		enc_hnd->frame_info.input_type = VO_COLOR_YUV_420_PACK_2;
	}else if(pInput->ColorType == VO_COLOR_ARGB32_PACKED){
		enc_hnd->frame_info.input_type = VO_COLOR_ARGB32_PACKED;
	}else{
		return VO_ERR_ENC_MPEG4_VIDEOTYPE;
	}
#ifdef LICENSE_CHECK
	video_format.Height = enc_hnd->img_height;
	video_format.Width  = enc_hnd->img_width;
	video_format.Type = VO_VIDEO_FRAME_I;
	voCheckLibCheckVideo (enc_hnd->phLicenseCheck, pInput, &video_format);
#endif
	pOutput->Length = voMpeg4EncFrame(&enc_hnd->frame_info, enc_hnd, pInput->Buffer[0], pOutput->Buffer);

	*pType = enc_hnd->frame_info.frame_type;
	memset(&enc_hnd->frame_info, 0, sizeof(FRAME_INFO_CONFIG));

#ifdef CAL_PSNR
	frame_count ++;
#endif

#ifdef VOLOG_TEST	
	VOLOGE("\nvoMPEG4ENCProcess end");
	g_frameCount ++;
#endif

	if(pOutput->Length&VO_ERR_BASE){
		return VO_ERR_ENC_MPEG4_FRAME;
	}else{
		return VO_ERR_NONE;
	}
}


/**
 * Close the MPEG4 Encoder instance, release any resource it used.
 * \param hCodec [in] MPEG4 Encoder instance handle, returned by voMPEG4ENCInit().
 * \return The function should always return VO_ERR_NONE.
 */
VO_U32 VO_API voMPEG4ENCUninit(VO_HANDLE hEnc)
{
#ifdef VOLOG_TEST	
	VOLOGE("\nvoMPEG4ENCUninit start");
#endif

#ifdef LICENSE_CHECK
	voCheckLibUninit (((ENCHND *)hEnc)->phLicenseCheck);
#endif
	voMpeg4EncFinish((ENCHND *)hEnc);

#ifdef MAX_RAM_USED_CHECK
	if(fpMemLog){
		fprintf( fpMemLog,"\nTotal RAM allocated: %2.2f K.\n", (float)(g_RamUsed/1024));
		fclose(fpMemLog);
	}
#endif
#ifdef CAL_PSNR
	if(fp_PSNR){
		if (frame_count > 0) {
			total_psnr_y /= frame_count;
			total_psnr_u /= frame_count;
			total_psnr_v /= frame_count;
		}
		fprintf(fp_PSNR,"\n******************* Average PSNR *******************");
		fprintf(fp_PSNR,"\nTotal frame:%d, avgPSNR_Y:%4f, avgPSNR_U:%4f, avgPSNR_V:%4f\n", frame_count, total_psnr_y, total_psnr_u, total_psnr_v);
		fclose(fp_PSNR);
	}
#endif

#ifdef VOLOG_TEST	
	VOLOGE("\nvoMPEG4ENCUninit end");
#endif
	return VO_ERR_NONE;
}


/**
 * Set parameter of the Encoder instance.
 * \param hCodec [in] MPEG4 Encoder instance handle, returned by voMPEG4ENCInit().
 * \param nID [in] Parameter ID, Supports: VOMPEG4PARAMETERID
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VO_ERR_NONE Succeeded.
 * \retval VO_ERR_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_U32 VO_API voMPEG4ENCSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	ENCHND *enc_hnd = (ENCHND *)hCodec;

#ifdef VOLOG_TEST	
	VOLOGE("\nvoMPEG4ENCSetParameter start");
#endif

	switch(uParamID){
	case VO_PID_ENC_MPEG4_BITRATE: 
		enc_hnd->custom_config.bitrate = *((VO_S32*)pData);
		enc_hnd->config |= 1;
		break;
	case VO_PID_ENC_MPEG4_FRAMERATE:
		enc_hnd->custom_config.framerate = *((float*)pData);
		enc_hnd->config |= (1<<1);
		break;
	case VO_PID_ENC_MPEG4_WIDTH:
		enc_hnd->custom_config.image_width = *((VO_S32*)pData);
		enc_hnd->config |= (1<<2);
		break;
	case VO_PID_ENC_MPEG4_HEIGHT:
		enc_hnd->custom_config.image_height = *((VO_S32*)pData);
		enc_hnd->config |= (1<<3);
		break;
	case VO_PID_ENC_MPEG4_KEY_FRAME_INTERVAL:
		enc_hnd->custom_config.max_key_frame_interval = *((VO_S32*)pData);
		enc_hnd->config |= (1<<4);
		break;
	case VO_PID_ENC_MPEG4_VIDEO_QUALITY:
		enc_hnd->custom_config.enc_quality = *((VO_S32*)pData);
		enc_hnd->config |= (1<<5);
		break;
	case VO_PID_ENC_MPEG4_FORCE_KEY_FRAME:
		enc_hnd->frame_info.force_intra_frame = *((VO_S32*)pData);
		break;
	case VO_PID_ENC_MPEG4_INPUT_ROTATION:
		enc_hnd->custom_config.rt_type = *((VO_S32*)pData);
		break;
	case VO_PID_ENC_MPEG4_VP_SIZE:
		enc_hnd->vp_byte_size = *((VO_S32*)pData);
		break;
	default :
			return VO_ERR_WRONG_PARAM_ID;
	}
	/* parameters setting completed*/
	if(enc_hnd->config == 0x3f){
		if(voMpegSetConfig(enc_hnd, &enc_hnd->custom_config)){
				return VO_ERR_ENC_MPEG4_CONFIG;
		}
		/* config ok */
		enc_hnd->config = CONFIG_OK; 
	}

#ifdef VOLOG_TEST	
	VOLOGE("\nvoMPEG4ENCSetParameter end");
#endif

	return VO_ERR_NONE;
}

/**
 * Get parameter of the Encoder .
 * \param hCodec [in] MPEG4 Encoder instance handle, returned by voMPEG4ENCInit().
 * \param nID [in] Parameter ID, Supports: VOMPEG4PARAMETERID
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VO_ERR_NONE Succeeded.
 * \retval VO_ERR_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_U32 VO_API voMPEG4ENCGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	ENCHND *enc_hnd = (ENCHND *)hCodec;

#ifdef VOLOG_TEST	
	VOLOGE("\nvoMPEG4ENCGetParameter start");
#endif

	if(enc_hnd->config != CONFIG_OK)
		return VO_ERR_WRONG_STATUS;

	switch(uParamID){
	case VO_PID_ENC_MPEG4_OUTBUF_SIZE: 
		*((VO_S32 *)pData) = enc_hnd->img_width*enc_hnd->img_height*3/2;
		break;
	case VO_PID_ENC_MPEG4_VOL_HEADER:
		((VO_CODECBUFFER *)pData)->Length = voMpeg4WriteVIH((ENCHND *)hCodec, ((VO_CODECBUFFER *)pData)->Buffer);
		break;
	default :
		return VO_ERR_WRONG_PARAM_ID;
	}

#ifdef VOLOG_TEST	
	VOLOGE("\nvoMPEG4ENCGetParameter end");
#endif
	return VO_ERR_NONE;
}
#endif/*NEW_SDK*/


EXPORT VO_S32 VO_API voGetMPEG4EncAPI (VO_VIDEO_ENCAPI * pDecHandle)
{
	VO_VIDEO_ENCAPI *pMpeg4Enc = pDecHandle;

	if(!pMpeg4Enc)
		return VO_ERR_INVALID_ARG;

	pMpeg4Enc->Init   = voMPEG4ENCInit;
	pMpeg4Enc->Uninit = voMPEG4ENCUninit;
	pMpeg4Enc->SetParam = voMPEG4ENCSetParameter;
	pMpeg4Enc->GetParam = voMPEG4ENCGetParameter;
	pMpeg4Enc->Process = voMPEG4ENCProcess;


	return VO_ERR_NONE;

}
