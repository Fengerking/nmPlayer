/* ********************************************** BEGIN LICENSE BLOCK *************************************************** 
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
 * *********************************************************************************** END LICENSE BLOCK ***********************************************/ 

/****************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information.
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
****************************************************************************************
Decoder interface. 

****************************************************************************************
*    INTEL Corporation Proprietary Information
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement.
*    Copyright (c) 1995 - 2000 Intel Corporation.
*    All Rights Reserved. 
******************************************************************************************
************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/decoder.h,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp $ */

#ifndef DECODER_H__
#define DECODER_H__

/**************************************************************************************
* The following undefs are required because windows.h is included 
* with pmonctrs.h, and RV_Boolean will later be defined to use 
* TRUE and FALSE in an enum. 
***************************************************************************************/
#undef TRUE
#undef FALSE

#include <stdio.h>

#include "rvstatus.h"
#include "rvstruct.h"
#include "rvdebug.h"
#include "realvdo.h"
#include "decdefs.h"
#include "frame.h"
#include "spinterp.h"
#include "dec4x4md.h"
#include "voRealVideo.h"
#include "voRVThread.h"

//#define THREAD_TEST


#if defined(_MSC_VER)
#pragma pack(push, 8)
#endif


#ifdef __cplusplus
extern "C" 
{
#endif
/* ***************************************************************************************************************
* CPU Scalability Threshold definitions.
* These constants define the thresholds for certain functionality based
* on the CPU Scalability parameter in  m_options.CPU_Scalability_Setting. 
* For thresholds less than 50, the action is performed when the scalability 
* setting is less than or equal to the threshold value. For thresholds greater 
* than 50, the actions are performed when the scalability setting is greater 
* than or equal to the threshold value. 
******************************************************************************************************************/
#define MAX_NUM_RPR_SIZES  8

#define TR_WRAP  256
#define TR_WRAP_RV  8192
#define TR_SHIFT	14
#define TR_RND		(1 << (TR_SHIFT - 1))
/**********************************************************************************
* RealVideo bitstreams as of "G2 with SVT" use a 13-bit TR, 
* giving millisecond granularity.  RealVideo "G2 Gold", and 
* RV89Combo use an 8-bit TR.
***********************************************************************************/
#define LUMA_SIZE 256
#define CHROMA_SIZE 64

#define MAX_THREAD_COUNT 16

#if defined(VODEBUG)
extern unsigned char aa[400];
extern FILE *out_file;
static void NU_SIO_Puts(unsigned char aa[])
{
	fprintf(out_file,"%s",aa); 
}
#endif

extern VO_HANDLE g_hRVDecInst;

typedef struct
{
	U8 *cur[3];
	U8 *ref[3];
	U8 *dst[3];
	U32 uDCOnly; 
	RV_Boolean isIntra; 
	U32        m_is16;
	_4x4_DSC_DECODE_TABLE *pDC4x4DscLuma;
	_2x2_DSC_DECODE_TABLE *pDC2x2DscLuma;
	_4x4_DSC_DECODE_TABLE *p4x4DscLuma;
	_2x2_DSC_DECODE_TABLE *p2x2DscLuma;
	_4x4_DSC_DECODE_TABLE *p4x4DscChroma;
	_2x2_DSC_DECODE_TABLE *p2x2DscChroma;
	LEVEL_DSC_DECODE_TABLE *pLevelDsc;
	LEVEL_DSC_DECODE_TABLE *pLevelDscChroma;

}RV89_DEC_LOCAL;

#define MAX_INTRA_QP_REGIONS 5
#define MAX_INTER_QP_REGIONS 7

#define FIFO_WRITE	0
#define FIFO_READ	1

#define MAXFRAMES   4
#define FIFO_NUM	(MAXFRAMES+1)

//huwei 20110307 yuv420pack
#define MAXYUV420PACKFRAMES 5

#define FBM_INTERNAL_CACHE_SUPPORT
typedef struct {
#ifdef FBM_INTERNAL_CACHE_SUPPORT
	IMAGE **img_seq;   
  U32 buf_num;
#else
  IMAGE *img_seq[FIFO_NUM];/* Circular buffer */
#endif
	U32 w_idx;				  /* Index of write */
	U32 r_idx;				  /* Index of read */
}FIFOTYPE;

typedef enum
{
	RV_INTRAPIC,		/* 0 (00) */
	RV_FORCED_INTRAPIC,	/* 1 (01) */
	RV_INTERPIC,		/* 2 (10) */
	RV_TRUEBPIC,		/* 3 (11) */
	RV_FRAME_TYPE_MAX = VO_MAX_ENUM_VALUE
} EnumRVPicCodType;

#define ISINTRAPIC(p) ((EnumRVPicCodType)p == RV_INTRAPIC || (EnumRVPicCodType)p == RV_FORCED_INTRAPIC)
#define ISINTERPIC(p) ((EnumRVPicCodType)p == RV_INTERPIC)
#define ISTRUEBPIC(p) ((EnumRVPicCodType)p == RV_TRUEBPIC)

typedef struct
{
	Bool32      is_valid;
	U32         offset;
} Bitstream_Segment_Info;

typedef struct{
	U32       message_id;

	I32       value1;
	I32       value2;
	U32       *pValue;
} RV_MSG_Simple;

struct Global_Decoder_Options
{
		/* ====================== */
		/* legacy RPR settings */
        U32             Num_RPR_Sizes;
            /* Number of different RPR sizes used in RealVideo */

        U32             RPR_Sizes[2 * MAX_NUM_RPR_SIZES];
            /* TBD, why not make this an array of RV_Dimensions? */
            /* Array containing the different RPR sizes */
            /* for (i = 0; i < Num_RPR_Sizes; i++) { */
            /*     U32 horizontal_size[i] */
            /*     U32 vertical_size[i] */
            /* } */
		/* ====================== */


		U32			   DeblockType;
		U32            debockingFlage;


};		  

/* This constructor simply gives each member a default value. */
void  Global_Decoder_Options_Init(struct Global_Decoder_Options *t);

typedef struct
{
	I32 ithread;
	void *ptr1;
} DECODETHREAD_DATA;

typedef struct
{
	I32                 m_bits; 
	I32                 m_bitpos;
	U8*					m_pBitptr;
	U8*					m_pBitstart;
	U8*					m_pBitend;
	U32                 m_maxBsSize;    //YU TBD

	I32					m_mba;
	U8					m_pquant;
	U8					m_OSVQUANT;
	U8					m_entropyqp;
	I32					m_resync_mb_x;
	I32					m_resync_mb_y;
	U32					uEdgeAvail;/*whether left, top, top rights and top left MBs are available*/
	U32					m_xmb_pos;
	U32					m_ymb_pos;
	I32					m_last_mb_row_decoded;
	I32                 m_first_flage;
	I32*				m_pQuantBuf;
	U8*					m_pPredBuf;
	DecoderIntraType*   m_pAboveSubBlockIntraTypes;
	DecoderIntraType*   m_pMBIntraTypes;

	struct DecoderPackedMotionVector  *m_pReferenceMotionVectors;
	struct DecoderPackedMotionVector  *m_pBMotionVectors;
	struct DecoderPackedMotionVector  *m_pBidirMotionVectors;
	struct DecoderPackedMotionVector  *m_pTopBidirMotionVectors;
	struct DecoderPackedMotionVector  *m_pCurBidirMotionVectors;

	struct DecoderPackedMotionVector  *m_pTopMotionVectors;
	struct DecoderPackedMotionVector  *m_pCurMotionVectors;
	struct DecoderMBInfo              *m_pReferenceMBInfo;
	struct DecoderMBInfo              *m_pBFrameMBInfo;
	struct DecoderMBInfo              *m_pMBInfo;
} SLICE_DEC;

struct Decoder
{
	RV_FID		m_fid;
	/* This fid represents the image format that this decoder */
	/* was instantiated to decode.  If a decoder supports multiple */
	/* input formats, then m_fid will not necessarily match the */
	/* input format that is eventually agreed to. */

	RV_Boolean	m_bIsRV8; /* is this RV8 or RV9 */
	struct Global_Decoder_Options      m_options;
	/* This is the value at which the TR Wraps around for this  */
	I32			TRB;
	I32			TRD;
	U32			m_uTRWrap;
	U32			m_tr;
	/* particular sequence. */

	U32         m_enable_deblocking_flage;
	U32         m_output_mode;//huwei 20100514 add output mode int * 0, display seq. 1, output immediately

	volatile I32        m_multithreaded_rd;
	I32					m_current_mb_col_main;
	I32					m_decoding_thread_count;

	pthread_t*  m_decoding_thread;
#ifdef THREAD_TEST
    pthread_mutex_t child_mutex;
	pthread_cond_t child_cond;
	pthread_mutex_t main_mutex;
	pthread_cond_t main_cond;
#else

	VO_SEM*      m_event_slice_decoding;
	VO_SEM       m_event_main;
#endif


	SLICE_DEC           *m_slice_di;
	DECODETHREAD_DATA   *m_thread_data;

	volatile I32        m_main_threaded_finish;

	U8*                 m_pb;
	U32                 m_pb_size;
	U32                 m_slice_minor;
	U32                 m_slice_main;

	EnumRVPicCodType	m_ptype;
	EnumRVPicCodType	m_display_ptype;
	RV_Boolean			m_deblocking_filter_passthrough;

	EnumRVPicCodType	m_pre_ptype;
	U32                 m_first_Iframe_flage;
	U32                 m_flush_err;
	U32                 m_raw_data_flage;

	/* slice information for packetization w/o rtp extension */
	U32                 m_nSlices;
	U32	                m_mbaSize;		/* size of MBA field */
	U8					m_pctsz; /* RV8 */

	Bitstream_Segment_Info  *m_sliceInfo;

	/* size of PCTSZ (RealVideo RPR size index) */
	U32			m_pctszSize;

	/* number of RPR sizes */
	U32			m_numSizes;

	/* location of RealVideo RPR size array */
	U32			*m_pSizes;

	IMAGE		*m_pBFrame;
	/* pointer to decoded B frame */

	IMAGE       *m_pCurrentFrame;

	IMAGE		*m_pRefFrame;
	/* This points to either m_BFrame, or one of the frames in our */
	/* reference frame list.  It is the frame that we are currently */
	/* parsing in Decode().  It's previous() and future() methods */
	/* can be used to access the frames it is predicted from. */
	/* After a successful call to Decode(), this remains a valid */
	/* pointer, so that the application can use custom messages to */
	/* extract bitstream information for the just-decoded picture. */

	IMAGE       *m_pDisplayFramePre;

	IMAGE       *m_pDisplayFrame;
	/* This points to either m_BFrame, or one of the frames in our */
	/* reference frame list.  It is the frame that we are going to */
	/* return from our Decode() invocation (or that we have returned */
	/* from our most recent Decode() invocation, if we're outside */
	/* the context of Decode()).  Due to B-frame latency, this may */
	/* not be the frame we are actually decoding.  Due to post */
	/* processing, the YUV data that we eventually display might */
	/* actually reside in one of our post processing buffers. */
	U32            m_img_height;
	U32            m_img_width;
	I32            m_img_stride;
	I32            m_img_UVstride;

	I32            m_rpr_no_YUVMem;
	I32            m_uv_interlace_flage;

	VO_MEM_VIDEO_OPERATOR m_mem_share;
	VO_MEM_OPERATOR       *m_pMemOP;           //huwei 20101228 new memory malloc

	U32            m_out_img_width;
	U32            m_out_img_height;

	U32            m_pre_out_img_width;
	U32            m_pre_out_img_height;

	U32			   m_img_mb;
	U32			   m_mbX;
	U32			   m_mbY;

	I32            m_curTR;
	I32            m_refTR;

	U32            m_max_img_size;
	U32            m_max_img_width;
	U32            m_max_img_height;

  FIFOTYPE	     m_priv;/*buffer manage*/

#ifdef FBM_INTERNAL_CACHE_SUPPORT
  IMAGE**        m_pDecReferenceFrames;
#else
  IMAGE*         m_decReferenceFrames[MAXFRAMES];
#endif

	U32            m_frame_number;
	U32            m_yuv420_pack_flag;
	YUV420PACK*    m_YUV420Frame[MAXYUV420PACKFRAMES];//huwei 20110110 yuv420_pack
	

	const I8*      m_pQPSTab;
	VO_PTR         m_phCheck;
	PU8            m_pYsh;
	PU8            m_pYsv;
	PU8            m_pUsh;
	PU8            m_pUsv;
	PU8            m_pVsh;
	PU8            m_pVsv;
	U8             m_deblock_pquant;

	RV_Status		(*GetPictureHeader)(struct Decoder *t, SLICE_DEC *slice_di);
	RV_Status		(*GetSliceHeader)(struct Decoder *t, I32* iMBA, SLICE_DEC *slice_di);
	RV_Status		(*GetIntraMBType)(SLICE_DEC *slice_di, const struct DecoderMBInfo*, DecoderIntraType * pAboveIntraTypes);
	RV_Status		(*GetInterMBType)(SLICE_DEC *slice_di, struct DecoderMBInfo *pMBInfo, U8 quant_prev,U32 * ulSkipModesLeft, I32 uWidthInMBs);
	RV_Status       (*InLoopFilter)(struct Decoder *t, U32 row);
	const T_InterpFnxTableNewPtr *gc_C_InterpolateNew;
	const T_InterpFnxTableNewPtr *gc_C_InterpolateChromaNew;
	const T_InterpFnxTableNewPtr *gc_C_AddInterpolateNew;
	const T_InterpFnxTableNewPtr *gc_C_AddInterpolateChromaNew;



	U8                     *m_pParsedData;
	/* This points to a huge, monolithic buffer that contains data */
	/* derived from parsing the current frame.  It contains motion */
 	/* vectors for a single frame, and MB info for one reference */
	/* frame and one B frame, among other things. */
	/* Logically this information belongs in the DecodedFrame class. */
 	/* However, algorithmically, we only need to keep around this */
 	/* information for the most recent reference frame and B frame */
 	/* that we decoded.  Thus, as a space saving optimization, we */
	/* allocate this information in the Decoder class rather than */
 	/* in the DecodedFrame class. */

	U32                     m_parsedDataLength;

#define NUM_QUANT_BUFFER_ELEMENTS 64*6
	/* Number of quant buffer elements in one MB for which space is allocated. */
	/* Add four additional entries, in case channel bit errors cause */
	/* too many coefficients to be present. */

#define NUM_INTRA_TYPE_ELEMENTS 16
	/* Number of intra type elements in one MB for which space is allocated. */

	VO_S64   m_timeStamp;
	VO_PTR		UserData;   /*!< The user data for later use.*/
	//VO_S64   m_timeStampPre;

	tDecoderTables	*m_pTables;
};

static INLINE void loadbits(SLICE_DEC* slice_info)
{
	I32 bitpos = slice_info->m_bitpos;							
	if (bitpos >= 8) {							
		I32 bits = slice_info->m_bits;					
		U8* bitptr = slice_info->m_pBitptr;	
		do {									
			bits = (bits << 8) | *bitptr++;		
			bitpos -= 8;						
		} while (bitpos>=8);					
		slice_info->m_bits = bits;						
		slice_info->m_pBitptr = bitptr;	
		slice_info->m_bitpos = bitpos;
	}											
}

#define showbits(slice_info,n) ((U32)(slice_info->m_bits << slice_info->m_bitpos) >> (32-(n)))
#define showbitsNeg(slice_info,n) ((U32)((~slice_info->m_bits) << slice_info->m_bitpos) >> (32-(n)))

#define flushbits(slice_info,n) slice_info->m_bitpos += n

// use this only in boolean expression. to get a 1 bit value use getbits(dec,1)
#define getbits1(slice_info) ((slice_info->m_bits << slice_info->m_bitpos++) < 0)

static INLINE I32 getbits(SLICE_DEC* slice_info,I32 n)
{
	I32 i = showbits(slice_info,n);
	flushbits(slice_info,n);
	return i;
}

#define loadbits_check(slice_info, bits, bitpos, minBitpos)	\
	if (bitpos >= minBitpos) {					\
	U8* bitptr = slice_info->m_pBitptr;	\
	do {									\
	bits = (bits << 8) | *bitptr++;		\
	bitpos -= 8;						\
	} while (bitpos>=8);					\
	slice_info->m_pBitptr = bitptr;					\
	}

#define showbits_pos(bits,bitpos,n) ((U32)((bits) << (bitpos)) >> (32-(n)))
#define getbits1_pos(bits,bitpos) (((bits) << (bitpos)++) < 0)
#define showbitsNeg_pos(bits, bitpos,n) ((U32)((~(bits)) << (bitpos)) >> (32-(n)))

#define flushbits_pos(bitpos, n) (bitpos) += n

/*****************************************************************************************
* Decodes a cbp descriptor.
* Use:
*  unsigned int decode_cbp (int intra, int qp,
*      unsigned char **p_pbs, unsigned int *p_bitOffset);
* Input:
*  intra - 0, if inter coded macroblock
*          1, if normal intra macroblock
*          2, if DC-removed intra macroblock
*  qp    - quantization parameter (1..31)
*  p_pbs - pointer to pointer to the current position in the bitstream
*  p_bitOffset - pointer to the bit position in the byte pointed by *p_pbs
* Returns:
*  decoded cbp descriptor
*****************************************************************************************/
I32 decode_4x4_block(SLICE_DEC *slice_di, I32 *block, U32 uQPC0, U32 uQPC1_2, U32 uQP, _4x4_DSC_DECODE_TABLE *p_4x4_dsc,
					 _2x2_DSC_DECODE_TABLE *p_2x2_dsc,LEVEL_DSC_DECODE_TABLE *p_level_dsc);

U32 decode_cbp (struct Decoder *t, U32 intra, U32 qp, I32 slice_index);

void ResampleYUVFrame(struct Decoder *t,U8 *pSrcFrame[],U8 *pDstFrame[],T_RPR_EdgeParams *pEdgeParams);

U32 Decoder_allocateParsedData(struct Decoder *t,U32 uWidth,U32 uHeight,RV_Boolean exactSizeRequested);

    /* Reallocate m_pParsedData, if necessary, and initialize all the */
    /* various pointers that point into it.  This gets called */
    /* during Start_Sequence, and at each Decode invocation, based */
    /* on the incoming image dimensions. */
    /* If exactSizeRequested is false, then any existing */
    /* allocated buffer, even if way too big, will be reused without */
    /* being deallocated.  Othwerwise, any existing allocated buffer */
    /* will be first deallocated if is not exactly the size needed. */

void  Decoder_deallocateParsedData(struct Decoder *t);



RV_Status Decoder_prepareDecodeBuffers(struct Decoder *t);
    /* prepareDecodeBuffers is called at the beginning of Decode(), */
    /* to set up m_pCurrentFrame, m_pDisplayFrame and m_pMBInfo for */
    /* the decoding of the incoming frame.  The 'notes' parameter */
    /* is also initialized here, based on the frame that is going */
    /* to be displayed.  If upon return the 'notes' indicate */
    /* RV_DDN_DONT_DRAW, then m_pCurrentFrame may still point to */
    /* a frame that should be decoded, though nothing should */
    /* be displayed.  If we have to display a frame prior to */
    /* decoding, the display is handled here and m_pDisplayframe */
    /* will be left NULL. */


				
			
		    /* Decode the luma and chroma coefficients for a macroblock, */
		    /* placing them in m_pQuantBuf and performing the inverse transform */
		    /* on them. */
RV_Status Decoder_reconstructLumaIntraMacroblock_16x16(
                struct Decoder *t,
                const struct DecoderMBInfo*,
                const RV89_DEC_LOCAL *rv89_local,
				const DecoderIntraType *pMBIntraTypes,
				I32 *pQuantBuf,
				U8 *pPredBuf);


RV_Status DecodeDirectMV
			    (
				struct Decoder *t,
				const U32               mbTypeRef,
			    struct DecoderPackedMotionVector *pMVReference,
				struct DecoderPackedMotionVector *pMVf,
				struct DecoderPackedMotionVector *pMVp,
				const U32               mbXOffset,
				const U32               mbYOffset,
				const I32				iRatio0,
				const I32				iRatio1
				);



RV_Status DecodePMBType_RV8(
							SLICE_DEC *slice_di, 
							struct DecoderMBInfo *pMBInfo, 
							U8 quant_prev,
							U32 * ulSkipModesLeft,
							I32 uWidthInMBs
							);

RV_Status DecodePMBType_RV9(
							SLICE_DEC *slice_di, 
							struct DecoderMBInfo *pMBInfo, 
							U8 quant_prev,
							U32 * ulSkipModesLeft,
							I32 uWidthInMBs
							);

RV_Status DecodeBMBType_RV8(
							SLICE_DEC *slice_di, 
							struct DecoderMBInfo *pMBInfo, 
							U8 quant_prev,
							U32 * ulSkipModesLeft,
							I32 uWidthInMBs
							);

RV_Status DecodeBMBType_RV9(
							SLICE_DEC *slice_di,
							struct DecoderMBInfo *pMBInfo,
							U8 quant_prev,
							U32 * ulSkipModesLeft,
							I32 uWidthInMBs
							);

RV_Status GetPredictorsMV(
						SLICE_DEC *slice_di,
						const struct DecoderMBInfo*,
						struct DecoderPackedMotionVector*,
						I32 block,    /* block or subblock number */
						I32 *pMVx,
						I32 *pMVy,     /* resulting MV predictor */
						const U32 uMB_X,
						I32 isRV8
						);

RV_Status GetPredictorsMVBFrame(
						SLICE_DEC *slice_di,
						const struct DecoderMBInfo *pMBInfo,
						const struct DecoderPackedMotionVector *pMV,
						I32 *pMVx,    /* resulting MV predictors */
						I32 *pMVy,
						const U32 uMB_X,
						const I32 mbX
						);

void Decoder_Init(struct Decoder *t, RV_Boolean	bIsRV8, RV_FID fid);
void Decoder_setDeblockCoefs(struct Decoder *t,struct DecoderMBInfo  *pMB,struct DecoderPackedMotionVector *pMV);

IMAGE*  buf_seq_ctl(FIFOTYPE *priv, IMAGE* img , const U32 flag);

RV_Status Decoder_IPicture(struct Decoder *t);
RV_Status Decoder_PPicture(struct Decoder *t);
RV_Status Decoder_BPicture(struct Decoder *t);
RV_Status Decoder_Slice(struct Decoder *t, I32 slice_number, I32 slice_index);
U32 Decoder_Custom_Message(struct Decoder *decoder,U32 *p_msgid);

/********************************************************************
* Initializes 4x4 decoding engine.
* Use:
*  void decode_4x4_init_vlc ();
* Returns:
*  none.
***********************************************************************/
void decode_4x4_init_vlc (struct Decoder *t, tDecoderTables  *m_pTables);
void decode_4x4_delete(struct Decoder* t, tDecoderTables  *m_pTables);

VO_U32 image_create(struct Decoder* t, IMAGE *image,const U32 edged_width,const U32 edged_height, const I32 uv_interlace_flage);	
void image_destroy (struct Decoder* t, IMAGE *image, const U32 uStride);		
void image_setedges(struct Decoder* t, IMAGE *image,const U32 ExWidth,const U32 ExHeight,U32 width, U32 height, const I32 uv_interlace_flage);		
void image_swap(IMAGE * image1,IMAGE * image2);
void YUV420T0YUV420PACK(VO_U8* srcy, VO_U8* srcu, VO_U8* srcv,  VO_U8* dstY, VO_U8 *dstUV, VO_U32 srcStrideY, VO_U32 srcStrideUV, VO_U32 width, VO_U32 height);
VO_U32 decoder_create_threads(void *decoder);
void decoder_remove_threads(struct Decoder *t);

#if defined(VOARMV7)
void YUV420T0YUV420PACKMB_Ctx(VO_U8* srcu, VO_U8* srcv, VO_U8* dst, VO_U32 srcStride, VO_U32 dstStride);
#define YUV420T0YUV420PACKMB YUV420T0YUV420PACKMB_Ctx
#else
void YUV420T0YUV420PACKMB_C(VO_U8* srcu, VO_U8* srcv, VO_U8* dst, VO_U32 srcStride, VO_U32 dstStride);
#define YUV420T0YUV420PACKMB YUV420T0YUV420PACKMB_C
#endif


#ifdef __cplusplus
}
#endif

#if defined(_MSC_VER)
#pragma pack(pop)
#endif



#endif /* DECODER_H__ */
