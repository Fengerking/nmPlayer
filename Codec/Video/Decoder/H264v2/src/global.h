#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define VLD  0
#define MEMPRINT  0
#if  VLD
#include "stdafx.h"
#include <vld.h>
#endif


#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <time.h>
//#include <sys/timeb.h>

#include "win32.h"
#include "defines.h"
#include "defineID.h" 

#include "parsetcommon.h"
#include "types.h"
//#include "io_image.h"
#include "frame.h"
//#include "distortion.h"
//#include "io_video.h"
#include  "voH264New.h"
#include "voRVThread.h"
//#include "voH264Readbits.h"
#include "sdkbase.h"
#include "h264base.h"
#if defined(ANDROID)&& defined(_VOLOG_ERROR)
//#include <pthread.h>
#include <android/log.h>
#define LOG_TAG "_VOH264"
#define LOGI(...) ((int)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#else//_LINUX_ANDROID
#define LOGI(...) 
#endif//_LINUX_ANDROID


#ifdef RVDS
#define inline __inline
#endif

#if defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
#define MAY_ALIAS __attribute__((may_alias))
#else
#define MAY_ALIAS
#endif
#if defined(_MSC_VER)
#define DECLARE_ALIGNED( var, n ) __declspec(align(n)) var
#define DECLARE_ALIGNED_EXT(n,t,v)		__declspec(align(n)) t v
#define RESTRICT

typedef struct xmm_reg { VO_U64 a, b; } xmm_reg;

#elif  defined(_MAC_OS)

typedef unsigned long       DWORD;

#define DECLARE_ALIGNED( var, n ) __declspec(align(n)) var
#define DECLARE_ALIGNED_EXT(n,t,v)		__declspec(align(n)) t v
#define RESTRICT

typedef struct xmm_reg { VO_U64 a, b; } xmm_reg;

#elif defined (VO_NDK_X86)

typedef unsigned long       DWORD;

#define DECLARE_ALIGNED( var, n )  __attribute__ ((aligned (n))) var
#define DECLARE_ALIGNED_EXT(n,t,v)	t __attribute__ ((aligned (n))) v
#define RESTRICT

typedef struct xmm_reg { VO_U64 a, b; } xmm_reg;

#else

#define DECLARE_ALIGNED( var, n ) var __attribute__((aligned(n)))
#define DECLARE_ALIGNED_EXT(n,t,v)      t v

#define RESTRICT //restrict	//must support C99

#endif

#define ALIGNED_16( var ) DECLARE_ALIGNED( var, 16 )
#define ALIGNED_8( var )  DECLARE_ALIGNED( var, 8 )
#define ALIGNED_4( var )  DECLARE_ALIGNED( var, 4 )


#if defined(WIN32)||defined(_WIN32)
#    pragma pack(push, 4)	
#    define VOCODECAPI __cdecl
#	define VOAPI __cdecl
#	define VOCBI __stdcall
#else// defined(LINUX) || defined(RVDS) || defined(_IOS) || defined(_MAC_OS)
#	define VOCODECAPI 
#	define VOAPI 
#	define VOCBI 
#endif//defined(WIN32)||defined(_WIN32)


//add by Really Yang 20110408
typedef union { VO_U16 i; VO_U8  c[2]; } MAY_ALIAS UNION16_TYPE;
typedef union { VO_U32 i; VO_U16 b[2]; VO_U8  c[4]; } MAY_ALIAS UNION32_TYPE;
typedef union { VO_U64 i; VO_U32 a[2]; VO_U16 b[4]; VO_U8 c[8]; } MAY_ALIAS UNION64_TYPE;
#define M16(src) (((UNION16_TYPE*)(src))->i)
#define M32(src) (((UNION32_TYPE*)(src))->i)
#define M64(src) (((UNION64_TYPE*)(src))->i)
// #define M16(src) (*((VO_U16*)(src)))
// #define M32(src) (*((VO_U32*)(src)))
// #define M64(src) (*((VO_U64*)(src)))

#define CP16(dst,src) M16(dst) = M16(src)
#define CP32(dst,src) M32(dst) = M32(src)
#define CP64(dst,src) M64(dst) = M64(src)
//end of add
static const VO_U8 cache_scan[16 + 2*4]={
 4+1*8, 5+1*8, 4+2*8, 5+2*8,
 6+1*8, 7+1*8, 6+2*8, 7+2*8,
 4+3*8, 5+3*8, 4+4*8, 5+4*8,
 6+3*8, 7+3*8, 6+4*8, 7+4*8,
 1+1*8, 2+1*8,
 1+2*8, 2+2*8,
 1+4*8, 2+4*8,
 1+5*8, 2+5*8,
};



#define STEP_BY_STEP 0
#if STEP_BY_STEP
static void WaitAnyKey(const char *msg)
{
	printf("%s\n", msg);
	getchar();
}
#else
#define WaitAnyKey(msg) 
#endif

//add by Really Yang 20110303
#define FDEC_STRIDE 16
//end of add

//////Added by Harry////////////////////////////////////////////////////////////////////
#define USE_MULTI_THREAD2	0
#define MAX_THREAD_NUM	2
#define MB_Y_MAX (200)
#define MB_X_MAX (128)
#define MB_X_MAX4 (MB_X_MAX*4)
#define USE_FRAME_THREAD	1
#if USE_FRAME_THREAD
#include "frameThd.h"
#endif

//////////////////////////////////////////////////////////////////////////

typedef struct bit_stream Bitstream;

typedef enum
{
  LumaComp = 0,
  CrComp = 1,
  CbComp = 2
} Color_Component;

typedef struct VO_CABACContext
{
    int low_bits;
    int range;
    int count;

    const VO_U8 *stream_start;
    const VO_U8 *stream;
    const VO_U8 *stream_end;

}VOCABACContext;


// Motion Vector structure
typedef struct
{
  short mv_x;
  short mv_y;
} MotionVector;

static const MotionVector zero_mv = {0, 0};

typedef struct
{
  short x;
  short y;
} BlockPos;



// structures that will be declared somewhere else
struct storable_picture;
struct datapartition;



#define FIFO_WRITE	0
#define FIFO_READ	1

#define FIFO_NUM	(32)
typedef struct {
  VO_U32 img_index[FIFO_NUM];     /* Circular buffer */
  VO_U32 w_idx;				/* Index of write */
  VO_U32 r_idx;				/* Index of read */
}FIFOTYPE;



typedef struct
{
	VO_U32				Size;				/*!< Buffer stride */
	VO_S32				Flag;
	void*				VBuffer;			/*!< user data pointer */
	void*				PBuffer;			/*!< user data pointer */
}VOMEM_INFO;

typedef struct  
{
	//new SDK from
	int  (VOAPI * Alloc4) (int uID, VOMEM_INFO * pMemInfo);
	int (VOAPI * Free4) (int uID, void* pBuff);
	int (VOAPI * Set) (int uID, void* pBuff, unsigned char uValue, unsigned int uSize);
	int (VOAPI * Copy) (int uID, void* pDest, void* pSource, unsigned int uSize);
	int (VOAPI * Check) (int uID, void* pBuffer, unsigned int uSize);
}VOH264MEMOP;



typedef VO_U32 (VOAPI  *_voCheckLibInit) (void** phCheck, VO_U32 nID, VO_U32 nFlag, void* hInst,void*);
typedef VO_U32 (VOAPI  *_voCheckLibCheckVideo) (void* hCheck, void* pOutBuffer, void* pOutInfo);
typedef VO_U32 (VOAPI  *_voCheckLibResetVideo) (void* hCheck, void* pOutBuffer);
typedef VO_U32 (VOAPI  *_voCheckLibUninit) (void* hCheck);

typedef struct{

	_voCheckLibInit						init;
	_voCheckLibCheckVideo		checkVideo;
	_voCheckLibResetVideo			resetVideo;
	_voCheckLibUninit					unInit;
	void*										hCheck;
	void*								instance;
	VO_U8*								lastVideo;
}TLicenseCheck2;



typedef struct DecRefPicMarking_s
{
  int memory_management_control_operation;
  int difference_of_pic_nums_minus1;
  int long_term_pic_num;
  int long_term_frame_idx;
  int max_long_term_frame_idx_plus1;
  struct DecRefPicMarking_s *Next;
} DecRefPicMarking_t;
typedef struct h264dec_g H264DEC_G;
typedef struct h264dec_l H264DEC_L;

struct h264dec_l
{
  struct slice       *p_Slice;                    //!< pointer to the current slice	
  int                 mbAddrX;                    //!< current MB address
  //int mbAddrA, mbAddrB, mbAddrC, mbAddrD;
  Boolean mbAvailA, mbAvailB, mbAvailC, mbAvailD;

  BlockPos mb;
  //int block_x;
  //int block_y;
  //int block_y_aff;
  VO_S32 pix_x;
  VO_S32 pix_y;
  VO_S32 pix_c_x;
  VO_S32 pix_c_y;
  //int block_xy;
  VO_S32 mb_xy;
  VO_S32 top_mb_xy;
  VO_S32 topleft_mb_xy;
  VO_S32 topright_mb_xy;
  VO_S32 left_mb_xy[2];
  const VO_U8 * left_block;
  VO_S32 top_type;
  VO_S32 left_type[2];
  VO_S32 topleft_pos;

  VO_S32 intra_ava_topleft;
  VO_S32 intra_ava_top;
  VO_S32 intra_ava_topright;
  VO_S32 intra_ava_left;
  

  int           qp_scaled[MAX_PLANE];  //!< QP scaled for all comps.
  Boolean       is_v_block;
  //short         list_offset;

  int           i16mode;

  signed char          b8mode[4];
  signed char          b8pdir[4];
  signed char          b8sub[4];
  ALIGNED_8(VO_U16 b8type[4]);
  //char          ipmode_DPCM;
  VO_S32          c_ipred_mode;       //!< chroma intra prediction mode

  Boolean       is_intra_block;
  int           mb_type0;
  int           left_mb_type;
  int           up_mb_type;
  int           cbp;
  int           cbp_up;
  int           cbp_left;
  int          skip_flag;
  int          pre_skip;
  int          next_skip;
  int          lu_transform8x8_flag;

  Boolean       mb_field;
  ALIGNED_8(VO_U8 nz_coeff[48]);
  ALIGNED_8(VO_S8 ipredmode[40]);
  ALIGNED_8(VO_S8 b8cache[40]);
  ALIGNED_16( VO_S32 mvd_cache[2][40] );
  ALIGNED_16( VO_S32 mv_cache[2][40] );
  ALIGNED_8( VO_S8 ref_cache[2][40] );
  ALIGNED_8( VO_U8 left_intra[64]);
  ALIGNED_8( VO_U8 direct_mode_row[MB_X_MAX4*2] );
//   VO_S16 cof_yuv_row[MB_BLOCK_SIZE*MB_BLOCK_SIZE*MB_X_MAX*3];
//   ALIGNED_16( VO_S16 cof_yuv_row[MB_BLOCK_SIZE*MB_BLOCK_SIZE*MB_X_MAX*3] );
  VO_S16 *cof_yuv;
  VO_U8 *direct_mode;
  struct storable_picture *pic_cache[2][40];
};


//! Bitstream
struct bit_stream
{
  // CABAC Decoding
  int           read_len;           //!< actual position in the codebuffer, CABAC only
  int           code_len;           //!< overall codebuffer length, CABAC only
  // CAVLC Decoding
  int           frame_bitoffset;    //!< actual position in the codebuffer, bit-oriented, CAVLC only
  int           bitstream_length;   //!< over codebuffer lnegth, byte oriented, CAVLC only
  // ErrorConcealment
  byte          *streamBuffer;      //!< actual codebuffer for read bytes
  int           ei_flag;            //!< error indication, 0: no error, else unspecified error
};

//! DataPartition
typedef struct datapartition
{

  Bitstream           *bitstream;
  VOCABACContext      c;
} DataPartition;

#if (MVC_EXTENSION_ENABLE)
typedef struct nalunitheadermvcext_tag
{
   unsigned int non_idr_flag;
   unsigned int priority_id;
   unsigned int view_id;
   unsigned int temporal_id;
   unsigned int anchor_pic_flag;
   unsigned int inter_view_flag;
   unsigned int reserved_one_bit;
   unsigned int iPrefixNALU;
} NALUnitHeaderMVCExt_t;
#endif
#define MAX_LIST_SIZE 33

typedef struct storable_picture
{
  PictureStructure structure;

  int         poc;
  int         top_poc;
  int         bottom_poc;
  int         frame_poc;
  unsigned int  frame_num;
  //unsigned int  recovery_frame;

  int         pic_num;
  int         long_term_pic_num;
  int         long_term_frame_idx;
  volatile VO_U32       removable;
  volatile VO_S32       errFlag;
  volatile VO_U32	bFinishOutput;
  volatile VO_S32	nProgress[2];
//   volatile VO_S32 * pProgress;

  VO_U32        is_long_term;
  int         used_for_reference;
  int         is_output;
  int         coded_frame;
  VO_U32         cur_buf_index;
  VO_U32 type;
  VO_S64		Time;
  VO_PTR        UserData;

  imgpel *     imgY;         //!< Y picture component
  imgpel *     imgUV[2];        //!< U and V picture components
  imgpel *     refY;         
  imgpel *     refUV[2];       
#if 1 //USE_FRAME_THREAD
	//VO_U32 need_prepare_param;
	//VO_S32 mbaff_flag;
#endif
  //struct pic_motion_params **mv_info;          //!< Motion info
  //struct pic_motion_params *mv_cache;
  VO_S8 *ref_idx[2];
  struct storable_picture **ref_pic[2];
  VO_S32* mv[2];
  VO_S32* mb_type_buffer;
  //VO_S16* mv[2];
  
  //short **     slice_id;      //!< reference picture   [mb_x][mb_y]

  struct storable_picture *top_field;     // for mb aff, if frame for referencing the top field
  struct storable_picture *bottom_field;  // for mb aff, if frame for referencing the bottom field
  struct storable_picture *frame;         // for mb aff, if field for referencing the combined frame
  struct storable_picture *frame_real;         // for mb aff, if field for referencing the combined frame

  int         adaptive_ref_pic_buffering_flag;
  DecRefPicMarking_t	*dec_ref_pic_marking_buffer;  

#if (MVC_EXTENSION_ENABLE)
  int         view_id;
  int         inter_view_flag;
  int         anchor_pic_flag;
#endif
  //imgpel **cur_imgY; // for more efficient get_block_luma
  int no_ref;
  int iCodingType;
  VO_S32 frame_set,top_set,bottom_set;
  //
  VO_S32 listXsize[32][2];
  struct storable_picture *listX[32][2][MAX_LIST_SIZE];

  //for display output
  int iYUVFormat;             //0: 4:0:0, 1: 4:2:0, 2: 4:2:2, 3: 4:4:4
  byte *pY;                   //if iPictureFormat is 1, [0]: top; [1] bottom;
  byte *pU;
  byte *pV;
  int iWidth;                 //frame width;              
  int iHeight;                //frame height;
  int iYBufStride;            //stride of pY[0/1] buffer in bytes;
  int iUVBufStride;           //stride of pU[0/1] and pV[0/1] buffer in bytes;
//   VO_U32 type;
//   VO_S64 Time;
} StorablePicture;

typedef StorablePicture *StorablePicturePtr;

typedef struct frame_store
{
  int       is_used;                //!< 0=empty; 1=top; 2=bottom; 3=both fields (or frame)
  int       is_reference;           //!< 0=not used for ref; 1=top used; 2=bottom used; 3=both fields (or frame) used
  int       is_long_term;           //!< 0=not used for ref; 1=top used; 2=bottom used; 3=both fields (or frame) used
  int       is_orig_reference;      //!< original marking by nal_ref_idc: 0=not used for ref; 1=top used; 2=bottom used; 3=both fields (or frame) used

  int       is_non_existent;

  unsigned  frame_num;
  unsigned  recovery_frame;

  int       frame_num_wrap;
  int       long_term_frame_idx;
  int       is_output;
  int       poc;
  // picture error concealment
  int concealment_reference;

//   StorablePicture *frame_real;
  StorablePicture *frame;
  StorablePicture *top_field;
  StorablePicture *bottom_field;

#if (MVC_EXTENSION_ENABLE)
  int       view_id;
  int       inter_view_flag[2];
  int       anchor_pic_flag[2];
#endif
  
} FrameStore;

//! Slice
typedef struct slice
{
  //struct video_par    *p_Vid;
  //struct inp_par      *p_Inp;
  pic_parameter_set_rbsp_t *active_pps;
  seq_parameter_set_rbsp_t *active_sps;
  int svc_extension_flag;

  // dpb pointer
  //struct decoded_picture_buffer *p_Dpb;

  //slice property;
  int idr_flag;
  int idr_pic_id;
  int nal_reference_idc;                       //!< nal_reference_idc from NAL unit
  int Transform8x8Mode;
  Boolean is_not_independent;

  int toppoc;      //poc for this top field // POC200301
  int bottompoc;   //poc of bottom field of frame
  int framepoc;    //poc of this frame // POC200301

  //the following is for slice header syntax elements of poc
  // for poc mode 0.
  unsigned int pic_order_cnt_lsb;
  int delta_pic_order_cnt_bottom;
  // for poc mode 1.
  int delta_pic_order_cnt[2];

  // ////////////////////////
  // for POC mode 0:
  signed   int PicOrderCntMsb;

  //signed   int PrevPicOrderCntMsb;
  //unsigned int PrevPicOrderCntLsb;

  // for POC mode 1:
  unsigned int AbsFrameNum;
  int ThisPOC;
  //signed int ExpectedPicOrderCnt, PicOrderCntCycleCnt, FrameNumInPicOrderCntCycle;
  //unsigned int PreviousFrameNum, FrameNumOffset;
  //int ExpectedDeltaPerPicOrderCntCycle;
  //int PreviousFrameNumOffset;
  // /////////////////////////

  //information need to move to slice;
  volatile VO_S32 current_mb_nr; // bitstream order
  volatile Boolean end_of_slice;
  volatile VO_S32 error_flag;
  //unsigned int num_dec_mb;
  VO_S32        current_slice_nr;
  FrameStore  *fs_ref;
  FrameStore  *fs_ltref;
  unsigned      ref_frames_in_buffer;
  unsigned      ltref_frames_in_buffer;
  struct storable_picture *cur_storable_pic;
  int cod_counter;                   //!< Current count of number of skipped macroblocks in a row
  int allrefzero;
  //end;

  int                 mb_aff_frame_flag;
  int                 direct_spatial_mv_pred_flag;       //!< Indicator for direct mode type (1 for Spatial, 0 for Temporal)
  int                 num_ref_idx_active[2];             //!< number of available list references
  //int                 num_ref_idx_l0_active;             //!< number of available list 0 references
  //int                 num_ref_idx_l1_active;             //!< number of available list 1 references

  int                 ei_flag;       //!< 0 if the partArr[0] contains valid information
  int                 qp;
  int                 slice_qp_delta;
  int                 qs;
  int                 slice_qs_delta;
  int                 slice_type;    //!< slice type
  int                 model_number;  //!< cabac model number
  unsigned int        frame_num;   //frame_num for this frame
  unsigned int        field_pic_flag;
  unsigned int        bottom_field_flag;
  PictureStructure    structure;     //!< Identify picture structure type
  int                 start_mb_nr;   //!< MUST be set by NAL even in case of ei_flag == 1
  int                 start_mb_nr_mbaff;
  int                 last_dquant;

  //slice header information;
  int colour_plane_id;               //!< colour_plane_id of the current coded slice
  int redundant_pic_cnt;
  int sp_switch;                              //!< 1 for switching sp, 0 for normal sp  
  int slice_group_change_cycle;
  int redundant_slice_ref_idx;     //!< reference index of redundant slice
  int no_output_of_prior_pics_flag;
  int long_term_reference_flag;
  int adaptive_ref_pic_buffering_flag;
  DecRefPicMarking_t* dec_ref_pic_marking_buffer;                    //!< stores the memory management control operations
  VO_S32 slice_number;
  char listXsize[32][6];
  char reserved[2];
  struct storable_picture **listX[32][6];

  DataPartition       *partArr;      //!< array of partitions
  VO_U8 cabac_ctx[460];
  int mvscale[6][MAX_REFERENCE_PICTURES];

  int                 ref_pic_list_reordering_flag[2];
  int                 *reordering_of_pic_nums_idc[2];
  int                 *abs_diff_pic_num_minus1[2];
  int                 *long_term_pic_idx[2];

#if (MVC_EXTENSION_ENABLE)
  int									*abs_diff_view_idx_minus1[2];

  int				          view_id;
  int                 inter_view_flag;
  int                 anchor_pic_flag;

  NALUnitHeaderMVCExt_t NaluHeaderMVCExt;
#endif



  int                 pic_parameter_set_id;  

  Boolean is_reset_coeff;



  //weighted prediction
  unsigned short weighted_pred_flag;
  unsigned short weighted_bipred_idc;
  //VO_U8 weighted_luma;
  //VO_U8 weighted_chroma;

  unsigned short luma_log2_weight_denom;
  unsigned short chroma_log2_weight_denom;
  VO_U8 ***weighted_flag;
  int ***wp_weight;  // weight in [list][index][component] order
  int ***wp_offset;  // offset in [list][index][component] order
  int ****wbp_weight; //weight in [list][fw_index][bw_index][component] order
  short wp_round_luma;
  short wp_round_chroma;

#if (MVC_EXTENSION_ENABLE)
  int listinterviewidx0;
  int listinterviewidx1;
  struct frame_store **fs_listinterview0;
  struct frame_store **fs_listinterview1;
#endif
} Slice;

#if 0
typedef struct decodedpic_t
{
  int bValid;                 //0: invalid, 1: valid, 3: valid for 3D output;
  int iViewId;                //-1: single view, >=0 multiview[VIEW1|VIEW0];
  int iPOC;
  int iYUVFormat;             //0: 4:0:0, 1: 4:2:0, 2: 4:2:2, 3: 4:4:4
  int iYUVStorageFormat;      //0: YUV seperate; 1: YUV interleaved; 2: 3D output;
  int iBitDepth;
  byte *pY;                   //if iPictureFormat is 1, [0]: top; [1] bottom;
  byte *pU;
  byte *pV;
  int iWidth;                 //frame width;              
  int iHeight;                //frame height;
  int iYBufStride;            //stride of pY[0/1] buffer in bytes;
  int iUVBufStride;           //stride of pU[0/1] and pV[0/1] buffer in bytes;
  int iSkipPicNum;
  VO_U32 type;
  VO_S64 Time;
  struct decodedpic_t *pNext;
} DecodedPicList;
#endif


struct h264dec_g
{
  seq_parameter_set_rbsp_t *SeqParSet; //[MAXSPS];
  pic_parameter_set_rbsp_t *PicParSet; //[MAXPPS];
  BlockPos *PicPos;
  //struct old_slice_par *old_slice;
  //struct snr_par       *snr;
  VO_S32 spsNum;
  int idr_flag;
  int long_term_reference_flag;
  //int adaptive_ref_pic_buffering_flag;
  int chroma_qp_offset[2];
  DecRefPicMarking_t *dec_ref_pic_marking_buffer; //[MAX_REFERENCE_FRAMES];
  unsigned int  recovery_frame;
  VO_S32 num_dec_mb;
  VO_S32 initDone;
  VO_S32 disableDeblock;
  VO_S32 extra_dpb;
  VO_S32 interlace;
  VO_S32 first_sps;
  VO_S32 mb_x;
  VO_S32 mb_y;
  VO_S32 field_offset;
  VO_S32 mbaff_dis;
  int number;
  unsigned int pre_frame_num;
  unsigned int previous_frame_num; //!< frame number of previous slice
  int MaxFrameNum;
//   int newframe;
  int non_conforming_stream;         //!< store the frame_num in the last decoded slice. For detecting gap in frame_num.
  int structure;                     //!< Identify picture structure type
  int type;  
  int width;
  int height;
  int width_cr;								//!< width chroma  
  int height_cr;								//!< height chroma
  VO_U32 direct_set;
  int iSliceNumOfCurrPic;
  Slice *pCurSlice;
  
//   VO_U32			frame_index[32];
  FIFOTYPE			vFrameIndexFIFO;
  VO_S32  pic_head;
  VO_S32  pic_end;
  VO_S8 *ipredmode_frame;

  VO_U8 *nz_coeff_frame;
  VO_U8 *slice_number_frame;


  // for POC mode 0:
  signed   int PrevPicOrderCntMsb;
  unsigned int PrevPicOrderCntLsb;
  // for POC mode 1:
  signed int ExpectedPicOrderCnt, PicOrderCntCycleCnt, FrameNumInPicOrderCntCycle;
  unsigned int PreviousFrameNum, FrameNumOffset;
  int ExpectedDeltaPerPicOrderCntCycle;
  int PreviousFrameNumOffset;

  VO_S32 PicWidthInMbs;
  VO_S32 PicHeightInMapUnits;
  VO_S32 FrameHeightInMbs;
  VO_S32 PicHeightInMbs;
  VO_S32 PicSizeInMbs;
  VO_S32 FrameSizeInMbs;
  //unsigned int oldFrameSizeInMbs;
  int no_output_of_prior_pics_flag;

  int last_has_mmco_5;
  int last_pic_bottom_field;

  int profile_idc;

  int recovery_point;
  int recovery_point_found;
  int recovery_frame_cnt;
  int recovery_frame_num;
  int recovery_poc;

  int cabac_init_done;
//   int LastAccessUnitExists;
//   int NALUCount;
//   VO_S32 oldSliceType;

  Boolean global_init_done;

  int *qp_per_matrix;
  int *qp_rem_matrix;

  // Scaling matrix info
  int  InvLevelScale4x4_Intra[3][6][4][4];
  int  InvLevelScale4x4_Inter[3][6][4][4];
  int  InvLevelScale8x8_Intra[3][6][8][8];
  int  InvLevelScale8x8_Inter[3][6][8][8];

  const int  *qmatrix[12];

  //struct frame_store *last_out_fs;
  //int pocs_in_dpb[100];
  struct storable_picture *dec_picture;
  //struct storable_picture *last_output;
  int         frame_cropping_flag;
  VO_U32         frame_cropping_rect_left_offset;
  VO_U32         frame_cropping_rect_right_offset;
  VO_U32         frame_cropping_rect_top_offset;
  VO_U32         frame_cropping_rect_bottom_offset;
  
  struct storable_picture **p_output;
  VO_U32 output_size;
  struct decoded_picture_buffer *p_Dpb;

  //slice funcs
  VO_S32 (*read_CBP_and_coeffs_from_NAL) (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 dct8x8);
  //int  (*decode_one_component     )    (H264DEC_G *pDecGlobal,Macroblock *currMB, ColorPlane curr_plane, imgpel **currImg, struct storable_picture *dec_picture);
  //int  (*readSlice                )    (struct video_par *, struct inp_par *);  
  int  (*nal_startcode_follows    )    (H264DEC_G *pDecGlobal,struct slice*, int );
  VO_S32 (*read_motion_info_from_NAL)    (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0);
  VO_S32 (*read_one_macroblock      )    (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
  VO_S32 (*interpret_mb_mode        )    (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mb_type0);
  void (*init_lists               )    (H264DEC_G *pDecGlobal,struct slice *currSlice);
  void (*intrapred_chroma         )    (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);  
  VO_S32 (*update_direct_mv_info    )    (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type);
  VO_S32 (*mb_pred_b                )    (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type);
  //void (*GetMVPredictor) (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int n, 
  //  short *pmv, short ref_frame, int list, int mb_x, int mb_y, int blockshape_x, int blockshape_y);
  //VO_S32 list_count;
#if 0
  DecodedPicList *pDecOuputPic;
#endif
  int iDeblockMode;  //0: deblock in picture, 1: deblock in slice;
  struct nalu_t *nalu;
  int iLumaPadX;
  int iLumaPadY;
  int iChromaPadX;
  int iChromaPadY;
  int iLumaStride;
  int iChromaStride;

  //add by Really Yang 20110323
  //VO_U8 BlockAvailable[30];
  //VO_S16 BlockAddr[30];
  //end of add
#if (MVC_EXTENSION_ENABLE)
	subset_seq_parameter_set_rbsp_t *active_subset_sps;
	//int svc_extension_flag;
	subset_seq_parameter_set_rbsp_t SubsetSeqParSet[MAXSPS];
	int last_pic_width_in_mbs_minus1;
	int last_pic_height_in_map_units_minus1;
	int last_max_dec_frame_buffering;
	int last_profile_idc;
#endif
#if !(defined(RVDS) || defined(_WIN32_WCE))
  //TIME_T start_time;
  //TIME_T end_time;
  //int64 tot_time;
#if(CAL_DEBLOCK)
  int64 tot_deblock;
#endif
#if(CAL_PARSER)
  int64 tot_parser;
#endif 
#if(CAL_SLICE)
  int64 tot_slice;
#endif  
#if(CAL_HEADER)
  int64 tot_header;
#endif  
#if(CAL_MC)
  int64 tot_mc;
#endif
#endif
  int old_pps_id;
//   VO_S32 old_pps_id2;

  /*bitstream */
  VO_U32 cache_a;
  VO_U32 cache_b;
  VO_S32 nBitPos;
  const VO_U8 *pBitPtr;
  const VO_U8 *pBitEnd;
  
  //deblock
  short               DFDisableIdc;     //!< Disable deblocking filter on slice
  short               DFAlphaC0Offset;  //!< Alpha and C0 offset for filtering slice
  short               DFBetaOffset;     //!< Beta offset for filtering slice
  short	 reserved;	
#if(FAST_FILTER)
  VO_S32				DFQPThresh;
#endif

  VO_U8 *qp_frame_buffer;
  VO_U8 *qpc_frame_buffer;
  VO_S32 *mb_type0_frame_buffer;
  VO_S32 *cbp_frame_buffer;
  //int64 *cbp_blk0_frame_buffer;
  VO_U8 *b8direct_frame_buffer;
  VO_S8 *c_ipred_frame_buffer;
  VO_S32 *mvd_frame_buffer;
  VO_U8 *intra_pix_buffer;
  VO_S16 *cof_yuv_row;
  VO_U8 *edge_buffer_alloc;
  VO_U8 *edge_buffer;
  VO_U8 *edge_uvbuffer_alloc;
  VO_U8 *edge_ubuffer;
  VO_U8 *edge_vbuffer;

  VO_U32 nThdNum;
  VO_U32 enbaleSEI;
  VO_U32 thdinit;
#if USE_FRAME_THREAD
	FrameThdPool *pFrameThdPool;
	FrameThdInfo *pThdInfo;
	VO_U32 preThdIdx;
#endif
// 	pthread_mutex_t decoderMutex;
// 	pthread_mutex_t finishMutex;
// 
// 	pthread_cond_t parserCond;
// 	pthread_cond_t decoderCond;
// 	pthread_cond_t finishCond;

// 	volatile VO_U32 nParser;
// 	VO_U32 nLastParserPos;
// 	VO_S32 nLineBufPos;
VOH264MEMOP*					customMem2;
TLicenseCheck2					*licenseCheck;
VO_U8                           bThumbnailMode;
VO_U8                           bDirectOutputMode;
#if defined(VOSSSE3)
VO_U8                           *tmp_block;
VO_S32                          is_sse3;
#endif
};


typedef struct decoder_params
{
  //VideoParameters   *p_Vid;          //!< Image Parameters
  int64              bufferSize;     //!< buffersize for tiff reads (not currently supported)
  //int                UsedBits;      // for internal statistics, is adjusted by se_v, ue_v, u_1
  FILE              *p_trace;        //!< Trace file
  int                bitcounter;
  H264DEC_G         *pDecGlobal;
  VO_U32               new_input;
  VO_S32 FileFormat;
#if COMBINE_FRAME
  VO_U8 *				inBuf;
  VO_U8 *				curPos;
  const VO_U8 *				lastPos;
  VO_U32				leftBytes;
  VO_U32               usedBytes;
  VO_U32				bFindFirstSlice;
  VO_U32				bHasNext;
  VO_U32				bNeedFlush;
  VO_U32				frameLen[MAX_FRAME_INPUT];
  VO_S64				Time[MAX_FRAME_INPUT];
  VO_PTR				UserData[MAX_FRAME_INPUT];
#endif
} DecoderParams;




//extern DecoderParams  *p_Dec;

// prototypes
extern void error(char *text, int code);

// dynamic mem allocation
extern int  init_global_buffers( H264DEC_G *pDecGlobal );
extern void free_global_buffers( H264DEC_G *pDecGlobal );

extern int RBSPtoSODB(byte *streamBuffer, int last_byte_pos);
extern int EBSPtoRBSP(byte *streamBuffer, int end_bytepos, int begin_bytepos);

extern void FreePartition (H264DEC_G *pDecGlobal, DataPartition *dp, int n);
extern DataPartition *AllocPartition(H264DEC_G *pDecGlobal, int n);

extern unsigned CeilLog2   ( unsigned uiVal);
extern unsigned CeilLog2_sf( unsigned uiVal);


extern Slice *malloc_slice(H264DEC_G *pDecGlobal);
//extern void CopySliceInfo ( H264DEC_G *pDecGlobal,Slice *currSlice, OldSliceParams *p_old_slice );


pic_parameter_set_rbsp_t *AllocPPS (H264DEC_G *pDecGlobal);
seq_parameter_set_rbsp_t *AllocSPS (H264DEC_G *pDecGlobal);

void FreePPS (H264DEC_G *pDecGlobal,  pic_parameter_set_rbsp_t *pps);
void FreeSPS (H264DEC_G *pDecGlobal,  seq_parameter_set_rbsp_t *sps);

#endif

