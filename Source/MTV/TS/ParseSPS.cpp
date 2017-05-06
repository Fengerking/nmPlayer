#include "ParseSPS.h"
#include "memory.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#ifdef LINUX
#include <stdlib.h>
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif




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
int f(int len);
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
CodeWord GetVLCSymbolH264(BitStream* bs);
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

void AdjustBitstream_I(BitStream *bs,int len)
{
	bs->pos+=(len+bs->bit_offset)/8;
	bs->bit_offset = (len+bs->bit_offset)%8;
}
int ForwardBitstream(int len)
{
	AdjustBitstream_I(&bs,len);
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

int GetBits_I(BitStream* bs,int len)
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
	AdjustBitstream_I(bs,forward);
	return value;
}

int u(int len)
{
	return GetBits_I(&bs,len);	
}

int f(int len)
{
	return GetBits_I(&bs,len);
}

int ue()
{
	CodeWord code = GetVLCSymbolH264(&bs);
	//return (int)pow(2,code.len)+code.info-1; 
	return (1<<code.len)+code.info-1; 
}

int se()
{
	CodeWord code = GetVLCSymbolH264(&bs);

	int code_num = (1<<code.len)+code.info-1;
	int value = (code_num+1)/2;
	if((code_num & 0x01)==0)         // lsb is signed bit
		value = -value;

	return value;
}
int me(int IsInter)
{
	CodeWord code = GetVLCSymbolH264(&bs);
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
	int code_num = GetVLCSymbolH264(&bs);

	


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
CodeWord GetVLCSymbolH264(BitStream* bs)
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
	
	AdjustBitstream_I(bs,2*len+1);
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
	AdjustBitstream_I(&bs,len+1);
	return len;
}

typedef int Boolean;
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
	unsigned  chroma_format_idc;
	Boolean  mb_adaptive_frame_field_flag;
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
static __inline int CleanTheBitStream(unsigned char*	head, int size)
{
	
	unsigned char*   end  = head + size;
	
	do {

		if (head[0]) {// faster if most of bytes are not zero;	
			head++;
			continue;
		}
		// find 00xx
		if (head[1]){
			head += 2; // skip two bytes;
			continue;
		}
		// find 0000xx

		if(head[2]!=3)
		{
			head+=3;
		}
		else
		{
			memmove(head+2,head+3,end-(head+3));
			end--;
			head+=2;
		}


	}
	while(head+3<end);
	return 0;
}
int ProcessSPS(unsigned char* buf,int size)
{
	//DumpNALU(nalu);
	//assert(nalu);
	//assert(seq_params);
	CleanTheBitStream(buf,size);
	SetBitstream(buf);
	seq_parameter_set_rbsp_t *sps = &seq_params;
	memset(sps,0,sizeof(seq_parameter_set_rbsp_t));
	sps->profile_idc                            = u(8);//, "SPS: profile_idc"                           , s);

	/*
	profile_idc -> 

	:66		baseline profile
	:77		main profile
	:88		extended profile
	:100	high(FRExt)
	:110	high 10(FRExt)
	:122	high 4:2:2(FRExt)
	:144	high 4:4:4(FRExt)

	*/
	
	sps->constrained_set0_flag                  = u(1);//   "SPS: constrained_set0_flag"                 , s);
	sps->constrained_set1_flag                  = u(1);//   "SPS: constrained_set1_flag"                 , s);
	sps->constrained_set2_flag                  = u(1);//   "SPS: constrained_set2_flag"                 , s);
	int reserved_zero                           = u(5);//, "SPS: reserved_zero_5bits"                   , s);
	//	assert (reserved_zero==0);
	
	sps->level_idc                              = u(8);//, "SPS: level_idc"                             , s);
	sps->seq_parameter_set_id                   = ue();//"SPS: seq_parameter_set_id"                     , s);

	if(sps->profile_idc==100)
	{
		sps->chroma_format_idc = ue();

		if(sps->chroma_format_idc==3)
		{
			int separate_colour_plane_flag =u(1);
			
		}
		{
			int bit_depth_luma_minus8			=ue();
			int bit_depth_chroma_minus8		=ue();
			int qpprime_y_zero_transform_bypass_flag=u(1);
			int seq_scaling_matrix_present_flag = u(1);

			if(seq_scaling_matrix_present_flag)
			{
				for(int i=0; i<8; i++)
				{
					int seq_scaling_list_present_flag   = u(1);
					
				}
			}
		}

	}
	
	sps->log2_max_frame_num_minus4              = ue();// ("SPS: log2_max_frame_num_minus4"                , s);
	sps->pic_order_cnt_type                     = ue();//"SPS: pic_order_cnt_type"                       , s);
	
	if (sps->pic_order_cnt_type == 0)
		sps->log2_max_pic_order_cnt_lsb_minus4  = ue();//"SPS: log2_max_pic_order_cnt_lsb_minus4"           , s);
	else if (sps->pic_order_cnt_type == 1)
	{
		int tmp,count;
		tmp						= u(1);
		tmp					    = se();
		tmp					    = se();
		count				    = ue();

		for(int i=0; i<count; i++)
			tmp = se();
	}											
	
	sps->num_ref_frames                        = ue();// ("SPS: num_ref_frames"                         , s);
	sps->gaps_in_frame_num_value_allowed_flag  = u(1);// ("SPS: gaps_in_frame_num_value_allowed_flag"   , s);
	sps->pic_width_in_mbs_minus1               = ue();//("SPS: pic_width_in_mbs_minus1"                , s);
	sps->pic_height_in_map_units_minus1        = ue();// ("SPS: pic_height_in_map_units_minus1"         , s);
	sps->frame_mbs_only_flag                   = u(1);//("SPS: frame_mbs_only_flag"                    , s);
	if(sps->frame_mbs_only_flag==0)
		sps->mb_adaptive_frame_field_flag        = u(1);//  ("SPS: mb_adaptive_frame_field_flag"           , s);
	sps->direct_8x8_inference_flag             = u(1); // (s);
	sps->frame_cropping_flag                   = u(1); // (s);
	if(sps->frame_cropping_flag)
	{
		sps->frame_cropping_rect_left_offset      = ue ();
		sps->frame_cropping_rect_right_offset     = ue ();
		sps->frame_cropping_rect_top_offset       = ue ();//*(4-2*sps->frame_mbs_only_flag);//TODO:
		sps->frame_cropping_rect_bottom_offset    = ue ();//*(4-2*sps->frame_mbs_only_flag);
	}
	return 1;	
}

int GetSizeInfo(unsigned char* buf,int size,int* width,int* height)
{
	if((buf[0]&0x0f)!=7)
		return 0;//this is not sps
	static const int SubWidthC  [4]= { 1, 2, 2, 1};
	static const int SubHeightC [4]= { 1, 2, 1, 1};
	ProcessSPS(buf+1,size-1);
	*width = (seq_params.pic_width_in_mbs_minus1+1)*16;
	*height= (seq_params.pic_height_in_map_units_minus1+1)*16*(2-seq_params.frame_mbs_only_flag);
	if(seq_params.frame_cropping_flag)
	{
		int crop_left   = SubWidthC[seq_params.chroma_format_idc] * seq_params.frame_cropping_rect_left_offset;
		int crop_right  = SubWidthC[seq_params.chroma_format_idc] * seq_params.frame_cropping_rect_right_offset;
		int crop_top    = SubHeightC[seq_params.chroma_format_idc]*( 2 - seq_params.frame_mbs_only_flag ) * seq_params.frame_cropping_rect_top_offset;
		int crop_bottom = SubHeightC[seq_params.chroma_format_idc]*( 2 - seq_params.frame_mbs_only_flag ) * seq_params.frame_cropping_rect_bottom_offset;
		*width=*width-
			crop_left-
			crop_right;
		*height=*height-
			crop_top -
			crop_bottom;
	}
	return 1;
}

/**
* SEI message types
*/
typedef enum {
	SEI_BUFFERING_PERIOD             =  0, ///< buffering period (H.264, D.1.1)
	SEI_TYPE_PIC_TIMING              =  1, ///< picture timing
	SEI_TYPE_REGISTERED_ITU_T_T35    =  4, ///< register ITU-T35
	SEI_TYPE_USER_DATA_UNREGISTERED  =  5, ///< unregistered user data
	SEI_TYPE_RECOVERY_POINT          =  6  ///< recovery point (frame # to decoder sync)
} SEI_Type;



int rbsp_trailing_bits()
{
	return 0x80;
}
#define MAX_CC_DATA_DATA_COUNT 32
typedef struct 
{
	BYTE one_bit :1;
	BYTE reserved :4;
	BYTE cc_valid :1;
	BYTE cc_type :2;
	BYTE cc_data_1;
	BYTE cc_data_2;

}str_cc_data_data;
typedef struct  
{
	BYTE reserved_1 :1;
	BYTE process_cc_data_flag :1;
	BYTE one_bit :1;
	BYTE cc_count :5;
	BYTE reserved_2 ;
	str_cc_data_data data[MAX_CC_DATA_DATA_COUNT]; 
}str_cc_data;

str_cc_data cc_data;

int DecodeCCData()
{
	int tmp = GetBits_I(&bs,8);
	cc_data.process_cc_data_flag = (tmp>>6) & 0x1;
	cc_data.cc_count = tmp & 0x1f;
	cc_data.reserved_2 = GetBits_I(&bs,8);
	for (int i = 0;i< cc_data.cc_count;i++)
	{
		int tmp = GetBits_I(&bs,8);
		cc_data.data[i].cc_valid = (tmp >>2) & 0x01;
		cc_data.data[i].cc_type = tmp & 0x03;
		cc_data.data[i].cc_data_1 = GetBits_I(&bs,8);
		cc_data.data[i].cc_data_2 = GetBits_I(&bs,8);
		if (cc_data.data[i].cc_valid == 0x1 && cc_data.data[i].cc_type == 0x0)
		{
			return i;
		}
	}
	return -1;

}
int DecodeRegistered_ITU_T_T35_ForClosedCaption()
{
	int itu_t_t35_country_code = GetBits_I(&bs,8);
	int itu_t_t35_provider_code = GetBits_I(&bs,16);
	int user_identifier = GetBits_I(&bs,32);
	if (0xb5 != itu_t_t35_country_code || 0x0031 != itu_t_t35_provider_code || user_identifier != 0x47413934)
	{
		return -1;
	}
	int user_data_type_code = GetBits_I(&bs,8);	
	if (user_data_type_code != 0x3)
	{
		return -1;
	}
	return DecodeCCData();


}
bool ParserSEI(unsigned char* pInPutbuf,int InPutbufSize ,unsigned char* pOutPutbuf,int *OutPutbufSize)
{
	CleanTheBitStream(pInPutbuf,InPutbufSize);
	SetBitstream(pInPutbuf);
	int type=0,size = 0;
	do 
	{
		do 
		{
			type += ShowBits(8);
		} while (0xff == GetBits_I(&bs,8));

		do{
			size+= ShowBits(8);
		}while(0xff == GetBits_I(&bs,8));

		switch(type){
		case SEI_TYPE_PIC_TIMING: // Picture timing SEI
			break;
		case SEI_TYPE_REGISTERED_ITU_T_T35:
			{
				int count = DecodeRegistered_ITU_T_T35_ForClosedCaption();
				if (count >= 0 && count < 31)
				{
					memcpy((void *)pOutPutbuf,(void *)&cc_data.data[count].cc_data_1,1);
					pOutPutbuf+=1;
					memcpy((void *)pOutPutbuf,(void *)&cc_data.data[count].cc_data_2,1);
					*OutPutbufSize = 2;
				}
				else
					*OutPutbufSize = 0;
				return true;
			}
			
			break;
		case SEI_TYPE_USER_DATA_UNREGISTERED:
			break;
		case SEI_TYPE_RECOVERY_POINT:
			break;
		case SEI_BUFFERING_PERIOD:
			break;
		default:
			break;
		}


	} while (0x80 != rbsp_trailing_bits());
	return false;
}

#ifdef _VONAMESPACE
}
#endif