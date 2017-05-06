
/*!
 ***********************************************************************
 *  \file
 *      mbuffer.h
 *
 *  \brief
 *      Frame buffer functions
 *
 *  \author
 *      Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Karsten Sühring          <suehring@hhi.de>
 ***********************************************************************
 */
#ifndef _MBUFFER_H_
#define _MBUFFER_H_

#include "global.h"
//#include "H264_C_Type.h"
#include "defines.h"

#if VOI_H264D_NON_BASELINE
#define MAX_REUSED_BUFFER	32
#else // VOI_H264D_NON_BASELINE
#define MAX_REUSED_BUFFER	32
#endif // VOI_H264D_NON_BASELINE

#ifdef VOI_H264D_BLOCK_LONG_TERM_REFERENCE
#define IS_LONG_TERM(s)				0
#else 
#define IS_LONG_TERM(s)				s->is_long_term
#endif //VOI_H264D_BLOCK_LONG_TERM_REFERENCE

#define MAX_LIST_SIZE		33


#define AVD_INVALID_REF_IDX		0xff
 
#define AVD_INVALID_REF_IDX_32	0xffffffff


// Notice that: moving4x4 = 1, for I frame (from the old definition of moving_block;
#define IsMoving4x4(pic, j4, i4) ((pic->typeIPB != I_SLICE) ? \
				((pic->motionInfo->moving4x4[j4][(i4)>>3]>>((i4)&7))&1) : 1)
#define SetMoving4x4(flag, motionInfo, j4, i4) (flag ? motionInfo->moving4x4[j4][(i4)>>3] |= (1<<((i4)&7)) : 0)
#define Set2Moving4x4s(motionInfo, j4, i4) (motionInfo->moving4x4[j4][(i4)>>3] |= (0x3<<((i4)&7)))
#define Are4Moving4x4FlagsSameIn8x8Block(pic, j4, i4, tmp) ((pic->typeIPB != I_SLICE) ? \
				(((tmp = ((pic->motionInfo->moving4x4[j4][(i4)>>3]>>((i4)&7))&0x3)) == 0 || \
				 tmp == 0x3) && tmp == ((pic->motionInfo->moving4x4[j4+1][(i4)>>3]>>((i4)&7))&0x3)) : 1)
#define Are16Moving4x4FlagsSameInMB(pic, j4, i4, tmp) ((pic->typeIPB != I_SLICE) ? \
				(((tmp = ((pic->motionInfo->moving4x4[j4][(i4)>>3]>>((i4)&7))&0xf)) == 0 || \
				 tmp == 0xf) && \
				tmp == ((pic->motionInfo->moving4x4[j4+1][(i4)>>3]>>((i4)&7))&0xf) && \
				tmp == ((pic->motionInfo->moving4x4[j4+2][(i4)>>3]>>((i4)&7))&0xf) && \
				tmp == ((pic->motionInfo->moving4x4[j4+3][(i4)>>3]>>((i4)&7))&0xf)) : 1)

typedef struct avdStoredPicMotionInfo
{
	avdNativeUInt			alignedWidth8x8;
	avdUInt8				**ref_idx0;       // [-1, 63], 7 bits used, reference picture [subblock_y/2][subblock_x/2]
	AVDMotionVector			**mv0;            //<! motion vector [subblock_y][subblock_x]
#if 1//def VOI_INPLACE_DEBLOCKING
	avdUInt8				*allocMVnRef;
#endif //VOI_INPLACE_DEBLOCKING

#if 1//FEATURE_BFRAME
	avdUInt8				**ref_idx1;       // [-1, 63], 7 bits used, reference picture [subblock_y/2][subblock_x/2]
	AVDMotionVector			**mv1;            //<! motion vector [subblock_y][subblock_x]
	avdUInt8				**moving4x4;		// one bit per 4x4 block; [subblock_y][subblock_x/8];
#endif //FEATURE_BFRAME
	avdNativeInt			height4x4Mb;		// used to remember memory size of mv and ref_idx;
} AVDStoredPicMotionInfo;

//! definition a picture (field or frame)
#define BIT_PROTECT_CHECK 0x12345678
typedef struct storable_picture
{
#if 1//FEATURE_BFRAME
	// did not find it used in Baseline code;
	avdInt32			ref_pic_num[AVD_MOTION_LIST_SIZE][MAX_LIST_SIZE];
#endif // VOI_H264D_NON_BASELINE
	// TBD: from JM8.3, check if we need this;
	//int64				frm_ref_pic_num[6][MAX_LIST_SIZE];
	//int64				top_ref_pic_num[6][MAX_LIST_SIZE];
	//int64				bottom_ref_pic_num[6][MAX_LIST_SIZE];
	avdInt32			poc;
	avdInt32			top_poc;
	avdInt32			bottom_poc;
	avdInt32			frame_poc;
	avdInt32			order_num;
	avdInt32			pic_num;
	avdInt32			long_term_pic_num;
	avdInt32			long_term_frame_idx;

	avdNativeUInt	    frame_num;
	avdNativeUInt       is_long_term;
	avdNativeUInt       used_for_reference;
	avdNativeUInt       is_output;
	avdNativeUInt       non_existing;
	avdNativeUInt		isCombinedOrSplit;

	avdNativeUInt       size_x, size_y, size_x_cr, size_y_cr;
	avdNativeInt        chroma_vector_adjustment;
	avdNativeUInt       coded_frame;
	avdNativeUInt       mb_adaptive_frame_field_flag;
	avdNativeUInt       MbaffFrameFlag;
	avdNativeUInt		PicSizeInMbs;
	SliceType			typeIPB;
	PictureStructure	structure;
	AVDStoredPicMotionInfo	*motionInfo;
	avdUInt8			*plnBuffer;
	avdUInt8			*plnY;
	avdUInt8			*plnU;
	avdUInt8			*plnV;

	struct storable_picture *top_field;     // for mb aff, if frame for referencing the top field
	struct storable_picture *bottom_field;  // for mb aff, if frame for referencing the bottom field
	struct storable_picture *frame;         // for mb aff, if field for referencing the combined frame
 
	avdNativeUInt       idr_flag;
	avdNativeUInt       no_output_of_prior_pics_flag;
	avdNativeUInt       long_term_reference_flag;
	avdNativeUInt       adaptive_ref_pic_buffering_flag;
	DecRefPicMarking_t	*dec_ref_pic_marking_buffer;                    //!< stores the memory management control operations
	// TBD: need to rebuild core lib first;
	VO_CODECBUFFER		seiPicInfo;
	
	long				index;//for shared memory
	long				error;
	long				valid;
	TTimeStamp			timestamp;
#if TEST_TIME
	TFrameProfile		*fprofile;
#endif//TEST_TIME
} StorablePicture;

//! Frame Stores for Decoded Picture Buffer
typedef struct frame_store
{
	StorablePicture *frame;
	StorablePicture *top_field;
	StorablePicture *bottom_field;
	avdUInt32			frame_num;
	avdInt32			frame_num_wrap;
	avdInt32			long_term_frame_idx;
	avdInt32			poc;
	avdNativeInt        is_used;                //<! 0=empty; 1=top; 2=bottom; 3=both fields (or frame)
	avdNativeInt        is_reference;           //<! 0=not used for ref; 1=top used; 2=bottom used; 3=both fields (or frame) used
	avdNativeInt        is_long_term;           //<! 0=not used for ref; 1=top used; 2=bottom used; 3=both fields (or frame) used
	avdNativeInt		is_orig_reference;      //!< original marking by nal_ref_idc: 0=not used for ref; 1=top used; 2=bottom used; 3=both fields (or frame) used
	avdNativeInt        is_non_existent;
	avdNativeInt        is_output;
} FrameStore;


//! Decoded Picture Buffer
typedef struct avdDPB
{
	struct storable_picture		*reusedFrBuf[MAX_REUSED_BUFFER];
	struct storable_picture		*reusedFldBuf[MAX_REUSED_BUFFER];
	AVDStoredPicMotionInfo		*reusedFrMotionInfo[AVD_MOTION_INFO_SIZE][MAX_REUSED_BUFFER];
	AVDStoredPicMotionInfo		*reusedFldMotionInfo[AVD_MOTION_INFO_SIZE][MAX_REUSED_BUFFER];
	FrameStore			**fs;
	FrameStore			**fs_ref;
	FrameStore			**fs_ltref;
	FrameStore			*last_picture;

	avdInt32			last_output_poc;
	avdInt32			max_long_term_pic_idx;
	avdNativeUInt		size;
	avdNativeUInt		used_size;
	avdNativeUInt		ref_frames_in_buffer;
	avdNativeUInt		ltref_frames_in_buffer;
	avdNativeInt		numReusedFrBuf; 
	avdNativeInt        init_done;
	avdNativeInt		numReusedFldBuf;
	avdNativeInt		numReusedFrMotionInfo[AVD_MOTION_INFO_SIZE]; // for P & B; 
	avdNativeInt		numReusedFldMotionInfo[AVD_MOTION_INFO_SIZE]; // for P & B;
	//the folowing two flag are safe for the decoder to allocate/free memory //NEW_BUF_MANAGEMENT
	avdNativeInt		actualAllocatedPic;
	avdNativeInt		actualAllocatedMotion;
	AVDStoredPicMotionInfo		*actualAllocatedMotionBuf[MAX_REUSED_BUFFER*AVD_MOTION_INFO_SIZE];
	struct storable_picture		*actualAllocatedPicBuf[MAX_REUSED_BUFFER];
} DecodedPictureBuffer;

void             init_dpb(ImageParameters *img);
void             free_dpb(ImageParameters *img);
FrameStore*      alloc_frame_store(ImageParameters *img);
void             free_frame_store(ImageParameters *img,FrameStore* f);
StorablePicture* alloc_storable_picture(ImageParameters *img,PictureStructure type, SliceType typeIPB, avdNativeUInt size_x, 
					avdNativeUInt size_y, avdNativeUInt size_x_cr, avdNativeUInt size_y_cr, avdNativeUInt createMotionInfo);
void             free_storable_picture(ImageParameters *img,StorablePicture* p);
void             store_picture_in_dpb(ImageParameters *img,StorablePicture* p);
void             flush_dpb(ImageParameters *img);

void             dpb_split_field(ImageParameters *img,FrameStore *fs);
void             dpb_combine_field(ImageParameters *img,FrameStore *fs);

void             init_lists(ImageParameters *img);
void             reorder_ref_pic_list(ImageParameters *img,StorablePicture **list, avdNativeUInt *list_size, 
                                      avdNativeInt num_ref_idx_lX_active_minus1, avdNativeInt *remapping_of_pic_nums_idc, 
                                      avdNativeInt *abs_diff_pic_num_minus1, avdNativeInt *long_term_pic_idx);
void             init_mbaff_lists(ImageParameters *img);
void             alloc_ref_pic_list0_reordering_buffer(ImageParameters *img,Slice *currSlice);
void             alloc_ref_pic_list1_reordering_buffer(ImageParameters *img,Slice *currSlice);
void             free_ref_pic_list0_reordering_buffer(ImageParameters *img,Slice *currSlice);
void             free_ref_pic_list1_reordering_buffer(ImageParameters *img,Slice *currSlice);

void			InitStorablePic(ImageParameters *img,StorablePicture *s, PictureStructure structure, SliceType typeIPB, avdNativeUInt size_x, 
					avdNativeUInt size_y, avdNativeUInt size_x_cr, avdNativeUInt size_y_cr, avdNativeUInt createMotionInfo);
StorablePicture *GetStorablePic(ImageParameters *img,PictureStructure structure, SliceType typeIPB, avdNativeUInt size_x, avdNativeUInt size_y, 
								avdNativeUInt size_x_cr, avdNativeUInt size_y_cr, avdNativeUInt createMotionInfo);
void			PutStorablePic(ImageParameters *img,StorablePicture *s);
void			DpbFrPostProc(ImageParameters *img,FrameStore *fs);
void			DpbFldPostProc(ImageParameters *img,StorablePicture *p);
void			DpbCombineYUVFields(ImageParameters *img,FrameStore *fs);
void			FreeMotionInfo(ImageParameters *img,AVDStoredPicMotionInfo *motionInfo, SliceType typeIPB);
void PutMotionInfo(ImageParameters *img,AVDStoredPicMotionInfo *mi,SliceType typeIPB,PictureStructure structure);
void			write_picture(ImageParameters *img,StorablePicture *p);
void write_unpaired_field(ImageParameters *img,FrameStore* fs);
#endif

