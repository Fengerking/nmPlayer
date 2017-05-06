#include "ParseSPS.h"
#include "memory.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifdef LINUX
#include <stdlib.h>
#endif

#ifdef _IOS
#include <stdlib.h>
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
	//int forward = len;
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
	Boolean   Valid;                  // indicates the parameter set is valid

	unsigned int profile_idc;                                       // u(8)
	Boolean   constrained_set0_flag;                                // u(1)
	Boolean   constrained_set1_flag;                                // u(1)
	Boolean   constrained_set2_flag;                                // u(1)
	Boolean   constrained_set3_flag;                                // u(1)
	unsigned  int level_idc;                                        // u(8)
	unsigned  int seq_parameter_set_id;                             // ue(v)
	unsigned  int chroma_format_idc;                                // ue(v)

	Boolean   seq_scaling_matrix_present_flag;                   // u(1)
	int       seq_scaling_list_present_flag[12];                 // u(1)
	int       ScalingList4x4[6][16];                             // se(v)
	int       ScalingList8x8[6][64];                             // se(v)
	Boolean   UseDefaultScalingMatrix4x4Flag[6];
	Boolean   UseDefaultScalingMatrix8x8Flag[6];

	unsigned int bit_depth_luma_minus8;                            // ue(v)
	unsigned int bit_depth_chroma_minus8;                          // ue(v)
	unsigned int log2_max_frame_num_minus4;                        // ue(v)
	unsigned int pic_order_cnt_type;
	// if( pic_order_cnt_type == 0 )
	unsigned int log2_max_pic_order_cnt_lsb_minus4;                 // ue(v)
	// else if( pic_order_cnt_type == 1 )
	Boolean delta_pic_order_always_zero_flag;               // u(1)
	int     offset_for_non_ref_pic;                         // se(v)
	int     offset_for_top_to_bottom_field;                 // se(v)
	unsigned int num_ref_frames_in_pic_order_cnt_cycle;          // ue(v)
	// for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
	//int   offset_for_ref_frame[256];   // se(v)
	unsigned int num_ref_frames;                                   // ue(v)
	Boolean   gaps_in_frame_num_value_allowed_flag;             // u(1)
	unsigned int pic_width_in_mbs_minus1;                          // ue(v)
	unsigned int pic_height_in_map_units_minus1;                   // ue(v)
	Boolean   frame_mbs_only_flag;                              // u(1)
	// if( !frame_mbs_only_flag )
	Boolean   mb_adaptive_frame_field_flag;                   // u(1)
	Boolean   direct_8x8_inference_flag;                        // u(1)
	Boolean   frame_cropping_flag;                              // u(1)
	unsigned int frame_cropping_rect_left_offset;                // ue(v)
	unsigned int frame_cropping_rect_right_offset;               // ue(v)
	unsigned int frame_cropping_rect_top_offset;                 // ue(v)
	unsigned int frame_cropping_rect_bottom_offset;              // ue(v)
	Boolean   vui_parameters_present_flag;                      // u(1)
	//vui_seq_parameters_t vui_seq_parameters;                  // vui_seq_parameters_t
	//unsigned  separate_colour_plane_flag;                       // u(1)
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
#define MAX_BUF_SIZE3 512
static unsigned char tmpBuf1[MAX_BUF_SIZE3];
int ProcessSPS(unsigned char* buf2,int size)
{
	//DumpNALU(nalu);
	//assert(nalu);
	//assert(seq_params);
	unsigned char* buf = tmpBuf1;
	if(size>MAX_BUF_SIZE3)
		size = MAX_BUF_SIZE3;
	memcpy(buf,buf2,size);
	CleanTheBitStream(buf,size);
	SetBitstream(buf);
	seq_parameter_set_rbsp_t *sps = &seq_params;
	memset(sps,0,sizeof(seq_parameter_set_rbsp_t));

	sps->profile_idc                            = u(8);//, "SPS: profile_idc"                           , s);
	
	sps->constrained_set0_flag                  = u(1);//   "SPS: constrained_set0_flag"                 , s);
	sps->constrained_set1_flag                  = u(1);//   "SPS: constrained_set1_flag"                 , s);
	sps->constrained_set2_flag                  = u(1);//   "SPS: constrained_set2_flag"                 , s);
	//int reserved_zero                           = u(5);//, "SPS: reserved_zero_5bits"                   , s);
	u(5);
	//	assert (reserved_zero==0);
	
	sps->level_idc                              = u(8);//, "SPS: level_idc"                             , s);
	sps->seq_parameter_set_id                   = ue();//"SPS: seq_parameter_set_id"                     , s);
	sps->chroma_format_idc = 1;
	if(sps->profile_idc==100)
	{
		sps->chroma_format_idc = ue();

		if(sps->chroma_format_idc==3)
		{
			u(1);
			//int separate_colour_plane_flag =
			
		}
		{
			//int bit_depth_luma_minus8			=ue();
			//int bit_depth_chroma_minus8		=ue();
			ue();
			ue();
			//int qpprime_y_zero_transform_bypass_flag=u(1);
			u(1);
			int seq_scaling_matrix_present_flag = u(1);

			if(seq_scaling_matrix_present_flag)
			{
				for(int i=0; i<8; i++)
				{
					//int seq_scaling_list_present_flag   = u(1);
					u(1);
				}
			}
		}

	}
	
	sps->log2_max_frame_num_minus4              = ue();// ("SPS: log2_max_frame_num_minus4"                , s);
	sps->pic_order_cnt_type                     = ue();//"SPS: pic_order_cnt_type"                       , s);
	
	if (sps->pic_order_cnt_type == 0)
		sps->log2_max_pic_order_cnt_lsb_minus4  = ue();//"SPS: log2_max_pic_order_cnt_lsb_minus4"           , s)
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

int GetH264Profile(unsigned char* buf, int size)
{

	if((buf[0]&0x0f)!=7)
		return 0;//this is not sps
	ProcessSPS(buf+1,size-1);
	
	return seq_params.profile_idc;
}
int GetH264Levle(unsigned char* buf, int size)
{

	if((buf[0]&0x0f)!=7)
		return 0;//this is not sps
	ProcessSPS(buf+1,size-1);
	
	return seq_params.level_idc;
}


enum{
	VO_VIDEO_FRAME_I,
	VO_VIDEO_FRAME_P,
	VO_VIDEO_FRAME_B,
	VO_VIDEO_FRAME_NULL
};
#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
int GetFrameType(char * buffer, int size)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len;
	int info_bit;
	int totbitoffset = 0;
	int naluType = buffer[0]&0x0f;
	
	while(naluType!=1&&naluType!=5)//find next NALU
	{
		//buffer = GetNextFrame(buffer,size)
		char* p = buffer;  
		char* endPos = buffer+size;
		for (; p < endPos; p++)
		{
			if (XRAW_IS_ANNEXB(p))
			{
				size  -= p-buffer;
				buffer = p+3;
				naluType = buffer[0]&0x0f;
				break;
			}
			if (XRAW_IS_ANNEXB2(p))
			{
				size  -= p-buffer;
				buffer = p+4;
				naluType = buffer[0]&0x0f;
				break;
			}
		}
		if(p>=endPos)
			return -1; 

	}
	if(naluType==5)
		return VO_VIDEO_FRAME_I;//I_FRAME
	buffer++;
	for(i=0;i<2;i++)
	{
		byteoffset= totbitoffset/8;
		bitoffset= 7-(totbitoffset%8);
		ctr_bit = (buffer[byteoffset] & (0x01<<bitoffset));   // set up control bit

		len=1;
		while (ctr_bit==0)
		{                 // find leading 1 bit
			len++;
			bitoffset-=1;           
			bitcounter++;
			if (bitoffset<0)
			{                 // finish with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}
			ctr_bit=buffer[byteoffset] & (0x01<<(bitoffset));
		}
		// make infoword
		inf=0;                          // shortest possible code is 1, then info is always 0
		for(info_bit=0;(info_bit<(len-1)); info_bit++)
		{
			bitcounter++;
			bitoffset-=1;
			if (bitoffset<0)
			{                 // finished with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}

			inf=(inf<<1);
			if(buffer[byteoffset] & (0x01<<(bitoffset)))
				inf |=1;
		}
		totbitoffset+=len*2-1;
		if(totbitoffset>48)
			return -1;
	}
	//(int)pow(2,(bitsUsed/2))+info-1;//pow(2,x)==1<<x
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if (inf>=5)
	{
		inf-=5;
	}
	if(inf<0||inf>2)
		return VO_VIDEO_FRAME_NULL;
	else if (inf == 0)
		return VO_VIDEO_FRAME_P;
	else if (inf == 1)
		return VO_VIDEO_FRAME_B;
	else
		return VO_VIDEO_FRAME_I;

}

#ifdef _VONAMESPACE
}
#endif