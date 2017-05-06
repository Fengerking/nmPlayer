

#ifndef _VOH264ENCODER_H_
#define _VOH264ENCODER_H_


#include "voVCodecCommon.h"

#include "voH264.h"


/* H264ENC:
 *      opaque handler for encoder */
typedef struct H264ENC H264ENC;
typedef struct H264ENC_L H264ENC_L;

/* Slice type */
#define I_PIC_TYPE              2
#define P_PIC_TYPE              0
#define IDR_PIC_TYPE			1

typedef struct 
{
  VO_S32 width;
  VO_S32 height;
  VO_S32 fps_num;
  VO_S32 max_iframe_interval;      
  VO_S32 i_keyint_min;      
  VO_S32 b_deblocking_filter;
  VO_S32 i_bitrate;

}OUT_PARAM;

typedef struct
{

    VO_S32         i_deblocking_filter_alphac0;    
    VO_S32         i_deblocking_filter_beta;       

	VO_S32          i_me_range; 
	VO_S32          i_mv_range; 
	VO_S32          b_dct_decimate; 
	VO_S32         i_qp_min;       
	VO_S32         i_qp_max;         
    VO_S32 b_annexb;               
                                 
    VO_S32 i_slice_max_size;   
    VO_S32 i_slice_max_mbs;    
    VO_S32 i_slice_count;       

	//add by Really Yang 1208
	VO_U32 i_analyse_flags;     
	//end of add 

} INTER_PARAM;

typedef struct {
    VO_S32 level_idc;
    VO_S32 mbps;        
    VO_S32 frame_size;  
    VO_S32 dpb;         
    VO_S32 bitrate;     
  VO_S32 mv_range;    
} H264LEVEL;

extern const H264LEVEL AVCLevel[];


void    IniInternParam( INTER_PARAM *);


#define AVC_PARAM_BAD_NAME  (-1)
#define AVC_PARAM_BAD_VALUE (-2)





enum vo_unit_type
{
    VO_NAL_UNKNOWN = 0,
    VO_NAL_SLICE   = 1,
    VO_NAL_SLICE_DPA   = 2,
    VO_NAL_SLICE_DPB   = 3,
    VO_NAL_SLICE_DPC   = 4,
    VO_NAL_SLICE_IDR   = 5,   
    VO_NAL_SEI         = 6,   
    VO_NAL_SPS         = 7,
    VO_NAL_PPS         = 8,
    VO_NAL_AUD         = 9,
};
enum vo_nal_priority
{
    VO_NAL_PRIORITY_DISPOSABLE = 0,
    VO_NAL_PRIORITY_LOW        = 1,
    VO_NAL_PRIORITY_HIGH       = 2,
    VO_NAL_PRIORITY_HIGHEST    = 3,
};


typedef struct
{
    VO_S32 i_ref_idc; 
    VO_S32 nal_type; 

    VO_S32 i_payload;
    VO_U8 *p_payload;
} VO_NAL;


#endif //_VOH264ENCODER_H_
