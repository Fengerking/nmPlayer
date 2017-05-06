#include "ParseSPS.h"
#include <string.h>
#define BYTE unsigned char
typedef struct  
{
	int bit_offset;
	BYTE* pos;
}BitStream;
//6 discriptor,u,ue,se,te,me
int u(int len);
int ue();
int se();
int te(int bOnly2Ref);
int me(int IsInter);

int SetBitstream(BYTE* pos);
int ShowBits(int len);
int ForwardBitstream(int len);
int GetNextLevel();
BitStream* GetBitStream();

//! gives CBP value from codeword number, both for intra and inter
const BYTE NCBP[48][2]=
{
	{47, 0},{31,16},{15, 1},{ 0, 2},{23, 4},{27, 8},{29,32},{30, 3},{ 7, 5},{11,10},{13,12},{14,15},
	{39,47},{43, 7},{45,11},{46,13},{16,14},{ 3, 6},{ 5, 9},{10,31},{12,35},{19,37},{21,42},{26,44},
	{28,33},{35,34},{37,36},{42,40},{44,39},{ 1,43},{ 2,45},{ 4,46},{ 8,17},{17,18},{18,20},{20,24},
	{24,19},{ 6,21},{ 9,26},{22,28},{25,23},{32,27},{33,29},{34,30},{36,22},{40,25},{38,38},{41,41},
};
typedef struct 
{
	int len;
	int info;
}CodeWord;
CodeWord  code;
BitStream bs;
CodeWord GetVLCSymbol(BitStream* bs);
BitStream* GetBitStream()
{
	return &bs;
}
int SetBitstream(BYTE* pos)
{
	bs.pos = pos;
	bs.bit_offset = 0;
	return 1;
}

void AdjustBitstream(BitStream *bs,int len)
{
	bs->pos+=(len+bs->bit_offset)/8;
	bs->bit_offset = (len+bs->bit_offset)%8;
}
int ForwardBitstream(int len)
{
	AdjustBitstream(&bs,len);
	return len;
}
int ShowBits(int len)
{
	int forward = len;
	BYTE* buf = bs.pos;
	int bit_offset = 7-bs.bit_offset;
	int value = 0;
	int byte_offset = 0;
	while(len>0)
	{
		
		value<<=1;
		if(bit_offset < 0)
		{
			bit_offset+=8;
			byte_offset++;
		}
		if(buf[byte_offset] & (0x01<<bit_offset))
			value |=1;
		len--;
		bit_offset--;
		
	}
	return value;
}

int GetBits(BitStream* bs,int len)
{
	int forward = len;
	BYTE* buf = bs->pos;
	int bit_offset = 7-bs->bit_offset;
	int value = 0;
	int byte_offset = 0;
	while(len>0)
	{
		
		value<<=1;
		if(bit_offset < 0)
		{
			bit_offset+=8;
			byte_offset++;
		}
		if(buf[byte_offset] & (0x01<<bit_offset))
			value |=1;

		len--;
		bit_offset--;
		
	}
	AdjustBitstream(bs,forward);
	return value;
}

int u(int len)
{
	return GetBits(&bs,len);	
}

int ue()
{
	CodeWord code = GetVLCSymbol(&bs);
	//return (int)pow(2,code.len)+code.info-1; 
	return (1<<code.len)+code.info-1; 
}

int se()
{
	CodeWord code = GetVLCSymbol(&bs);

	int code_num = (1<<code.len)+code.info-1;
	int value = (code_num+1)/2;
	if((code_num & 0x01)==0)         // lsb is signed bit
		value = -value;

	return value;
}
int me(int IsInter)
{
	CodeWord code = GetVLCSymbol(&bs);
	int code_num = (1<<code.len)+code.info-1;
	return NCBP[code_num][IsInter];
}
int te(int bOnly2Ref)
{
	//int retVal;
	if(bOnly2Ref)
		return 1-u(1);
	else
		return ue();
	

}
/*!
************************************************************************
* \brief
*    read next UVLC codeword from UVLC-partition and
*    map it to the corresponding syntax element
************************************************************************

int readNextCodeword_VLC()
{
	int code_num = GetVLCSymbol(&bs);

	


}*/
/*!
************************************************************************
* \brief
*  read one exp-golomb VLC symbol
*

* \return
*    bits read
************************************************************************
*/
CodeWord GetVLCSymbol(BitStream* bs)
{
	BYTE *buf = bs->pos;// = GetBitStreamPos();
	int bit_offset = 7-bs->bit_offset;
	int ctr_bit = (buf[0] &(0x01<<bit_offset));
	int len = 0;
	//int bit_counter = 0;
	int byte_offset = 0;
	int inf,info_bit;
	while (0 == ctr_bit) 
	{
		len++;
		bit_offset--;           
		//bit_counter++;
		if(bit_offset<0)
		{						// finish with current byte ?
			bit_offset+=8;
			byte_offset++;
		}
		ctr_bit=buf[byte_offset] & (0x01<<(bit_offset));
	
	}
	//len--;
	// make infoword
	inf=0;                          // shortest possible code is 1, then info is always 0
	for(info_bit=0;info_bit<len; info_bit++)
	{
		
		//bit_counter++;
		bit_offset--;
		if (bit_offset<0)
		{                 // finished with current byte ?
			bit_offset+=8;
			byte_offset++;
		}
		
		
		inf <<=1;
		if(buf[byte_offset] & (0x01<<(bit_offset)))
			inf |=1;

		
	}
	
	CodeWord code;
	code.len = len;
	code.info = inf;
	
	//adjust the bitstream position
	
	AdjustBitstream(bs,2*len+1);
	return code;
}
int GetNextLevel()
{
	BYTE *buf = bs.pos;// = GetBitStreamPos();
	int bit_offset = 7-bs.bit_offset;
	int ctr_bit = (buf[0] &(0x01<<bit_offset));
	int len = 0;
	//int bit_counter = 0;
	int byte_offset = 0;
	while (0 == ctr_bit) 
	{
		len++;
		bit_offset--;           
		//bit_counter++;
		if(bit_offset<0)
		{						// finish with current byte ?
			bit_offset+=8;
			byte_offset++;
		}
		ctr_bit=buf[byte_offset] & (0x01<<(bit_offset));
		
	}
	AdjustBitstream(&bs,len+1);
	return len;
}

#define	Boolean int
typedef struct
{
	
	unsigned  profile_idc;                                      // u(8)
	Boolean   constrained_set0_flag;                            // u(1)
	Boolean   constrained_set1_flag;                            // u(1)
	Boolean   constrained_set2_flag;                            // u(1)
	unsigned  level_idc;                                        // u(8)
	unsigned  seq_parameter_set_id;                             // ue(v)
	unsigned  log2_max_frame_num_minus4;                        // ue(v)
	unsigned pic_order_cnt_type;
	// if( pic_order_cnt_type == 0 ) 
	unsigned log2_max_pic_order_cnt_lsb_minus4;                 // ue(v)
	// else if( pic_order_cnt_type == 1 )
	//  Boolean delta_pic_order_always_zero_flag;               // u(1)
	//  int     offset_for_non_ref_pic;                         // se(v)
	// int     offset_for_top_to_bottom_field;                 // se(v)
	// unsigned  num_ref_frames_in_pic_order_cnt_cycle;          // ue(v)
    // for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
    //  int   offset_for_ref_frame[MAXnum_ref_frames_in_pic_order_cnt_cycle];   // se(v)
	unsigned  num_ref_frames;                                   // ue(v)
	Boolean   gaps_in_frame_num_value_allowed_flag;             // u(1)
	unsigned  pic_width_in_mbs_minus1;                          // ue(v)
	unsigned  pic_height_in_map_units_minus1;                   // ue(v)
	Boolean   frame_mbs_only_flag;                              // u(1)
	// if( !frame_mbs_only_flag ) 
    //Boolean   mb_adaptive_frame_field_flag;                   // u(1)
	Boolean   direct_8x8_inference_flag;                        // u(1)
	Boolean   frame_cropping_flag;                              // u(1)
    unsigned  frame_cropping_rect_left_offset;                // ue(v)
	unsigned  frame_cropping_rect_right_offset;               // ue(v)
	unsigned  frame_cropping_rect_top_offset;                 // ue(v)
	unsigned  frame_cropping_rect_bottom_offset;              // ue(v)
	Boolean   vui_parameters_present_flag;                      // u(1)
	// vui_seq_parameters_t vui_seq_parameters;                  // vui_seq_parameters_t
} seq_parameter_set_rbsp_t;
seq_parameter_set_rbsp_t seq_params={0};
int ProcessSPS(unsigned char* buf)
{
	//DumpNALU(nalu);
	//assert(nalu);
	//assert(seq_params);
	SetBitstream(buf);
	seq_parameter_set_rbsp_t *sps = &seq_params;
	memset(sps,0,sizeof(seq_parameter_set_rbsp_t));

	sps->profile_idc                            = u(8);//, "SPS: profile_idc"                           , s);
	
	sps->constrained_set0_flag                  = u(1);//   "SPS: constrained_set0_flag"                 , s);
	sps->constrained_set1_flag                  = u(1);//   "SPS: constrained_set1_flag"                 , s);
	sps->constrained_set2_flag                  = u(1);//   "SPS: constrained_set2_flag"                 , s);
	int reserved_zero                           = u(5);//, "SPS: reserved_zero_5bits"                   , s);
	//	assert (reserved_zero==0);
	
	sps->level_idc                              = u(8);//, "SPS: level_idc"                             , s);
	
	
	sps->seq_parameter_set_id                   = ue();//"SPS: seq_parameter_set_id"                     , s);
	sps->log2_max_frame_num_minus4              = ue();// ("SPS: log2_max_frame_num_minus4"                , s);
	sps->pic_order_cnt_type                     = ue();//"SPS: pic_order_cnt_type"                       , s);
	
	if (sps->pic_order_cnt_type == 0)
		sps->log2_max_pic_order_cnt_lsb_minus4  = ue();//"SPS: log2_max_pic_order_cnt_lsb_minus4"           , s);
													
	
	sps->num_ref_frames                        = ue();// ("SPS: num_ref_frames"                         , s);
	sps->gaps_in_frame_num_value_allowed_flag  = u(1);// ("SPS: gaps_in_frame_num_value_allowed_flag"   , s);
	sps->pic_width_in_mbs_minus1               = ue();//("SPS: pic_width_in_mbs_minus1"                , s);
	sps->pic_height_in_map_units_minus1        = ue();// ("SPS: pic_height_in_map_units_minus1"         , s);
	sps->frame_mbs_only_flag                   = u(1);//("SPS: frame_mbs_only_flag"                    , s);
													 
	sps->direct_8x8_inference_flag             = u(1); // (s);
	sps->frame_cropping_flag                   = u(1); // (s);
	if(sps->frame_cropping_flag)
	{
		sps->frame_cropping_rect_left_offset      = ue ()*2;
		sps->frame_cropping_rect_right_offset     = ue ()*2;
		sps->frame_cropping_rect_top_offset       = ue ()*(4-2*sps->frame_mbs_only_flag);//TODO:
		sps->frame_cropping_rect_bottom_offset    = ue ()*(4-2*sps->frame_mbs_only_flag);
	}
	return 1;	
}

int GetSizeInfo(unsigned char* buf,int* width,int* height)
{
	if((buf[0]&0x0f)!=7)
		return 0;//this is not sps
	ProcessSPS(buf+1);
	*width = (seq_params.pic_width_in_mbs_minus1+1)*16;
	*height= (seq_params.pic_height_in_map_units_minus1+1)*16;
	if(seq_params.frame_cropping_flag)
	{
		*width=*width-
			seq_params.frame_cropping_rect_left_offset-
			seq_params.frame_cropping_rect_right_offset;
		*height=*height-
			seq_params.frame_cropping_rect_top_offset-
			seq_params.frame_cropping_rect_bottom_offset;
	}
	return 1;
}

int IsH264ProfileSupported(unsigned char* buf)
{
#define H264_BASELINE_PROFILE 66
	if(seq_params.profile_idc==0)//the seq has not been parsed
	{
		if((buf[0]&0x0f)!=7)
			return 0;//this is not sps
		ProcessSPS(buf+1);
	}
	if(seq_params.profile_idc!=H264_BASELINE_PROFILE&&seq_params.constrained_set0_flag!=1)
	{
		return 0;
	}
	return 1;
}
