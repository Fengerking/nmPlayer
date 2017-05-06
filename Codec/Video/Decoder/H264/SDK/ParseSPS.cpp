#include "ParseSPS.h"
#include "memory.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#ifdef LINUX
#include <stdlib.h>
#endif

typedef struct  
{
	int bit_offset;
	unsigned char* pos;
}BitStream;
//6 discriptor,u,ue,se,te,me
int u(BitStream* bs,int len);
int ue(BitStream* bs);


int SetBitstream(BitStream* bs,unsigned char* pos);

int GetNextLevel();
BitStream* GetBitStream();


typedef struct 
{
	int len;
	int info;
}CodeWord;
//CodeWord  code;
//BitStream bs;
CodeWord GetVLCSymbol(BitStream* bs);

int SetBitstream(BitStream *bs,unsigned char* pos)
{
	bs->pos = pos;
	bs->bit_offset = 0;
	return 1;
}

void AdjustBitstream(BitStream *bs,int len)
{
	bs->pos+=(len+bs->bit_offset)/8;
	bs->bit_offset = (len+bs->bit_offset)%8;
}
int GetBits(BitStream* bs,int len)
{
	int forward = len;
	unsigned char* buf = bs->pos;
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

int u(BitStream* bs,int len)
{
	return GetBits(bs,len);	
}

int ue(BitStream* bs)
{
	CodeWord code = GetVLCSymbol(bs);
	//return (int)pow(2,code.len)+code.info-1; 
	return (1<<code.len)+code.info-1; 
}

int se(BitStream* bs)
{
	CodeWord code = GetVLCSymbol(bs);

	int code_num = (1<<code.len)+code.info-1;
	int value = (code_num+1)/2;
	if((code_num & 0x01)==0)         // lsb is signed bit
		value = -value;

	return value;
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
	unsigned char *buf = bs->pos;// = GetBitStreamPos();
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
int GetNextLevel(BitStream* bs)
{
	unsigned char *buf = bs->pos;// = GetBitStreamPos();
	int bit_offset = 7-bs->bit_offset;
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
	AdjustBitstream(bs,len+1);
	return len;
}

#define	Boolean int
typedef struct
{
	Boolean   Valid;                  // indicates the parameter set is valid

	unsigned int profile_idc;                                       // u(&bs,8)
	Boolean   constrained_set0_flag;                                // u(&bs,1)
	Boolean   constrained_set1_flag;                                // u(&bs,1)
	Boolean   constrained_set2_flag;                                // u(&bs,1)
	Boolean   constrained_set3_flag;                                // u(&bs,1)
	unsigned  int level_idc;                                        // u(&bs,8)
	unsigned  int seq_parameter_set_id;                             // u(&bsv)
	unsigned  int chroma_format_idc;                                // u(&bsv)

	Boolean   seq_scaling_matrix_present_flag;                   // u(&bs,1)
	int       seq_scaling_list_present_flag[12];                 // u(&bs,1)
	int       ScalingList4x4[6][16];                             // se(v)
	int       ScalingList8x8[6][64];                             // se(v)
	Boolean   UseDefaultScalingMatrix4x4Flag[6];
	Boolean   UseDefaultScalingMatrix8x8Flag[6];

	unsigned int bit_depth_luma_minus8;                            // u(&bsv)
	unsigned int bit_depth_chroma_minus8;                          // u(&bsv)
	unsigned int log2_max_frame_num_minus4;                        // u(&bsv)
	unsigned int pic_order_cnt_type;
	// if( pic_order_cnt_type == 0 )
	unsigned int log2_max_pic_order_cnt_lsb_minus4;                 // u(&bsv)
	// else if( pic_order_cnt_type == 1 )
	Boolean delta_pic_order_always_zero_flag;               // u(&bs,1)
	int     offset_for_non_ref_pic;                         // se(v)
	int     offset_for_top_to_bottom_field;                 // se(v)
	unsigned int num_ref_frames_in_pic_order_cnt_cycle;          // u(&bsv)
	// for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
	//int   offset_for_ref_frame[256];   // se(v)
	unsigned int num_ref_frames;                                   // u(&bsv)
	Boolean   gaps_in_frame_num_value_allowed_flag;             // u(&bs,1)
	unsigned int pic_width_in_mbs_minus1;                          // u(&bsv)
	unsigned int pic_height_in_map_units_minus1;                   // u(&bsv)
	Boolean   frame_mbs_only_flag;                              // u(&bs,1)
	// if( !frame_mbs_only_flag )
	Boolean   mb_adaptive_frame_field_flag;                   // u(&bs,1)
	Boolean   direct_8x8_inference_flag;                        // u(&bs,1)
	Boolean   frame_cropping_flag;                              // u(&bs,1)
	unsigned int frame_cropping_rect_left_offset;                // u(&bsv)
	unsigned int frame_cropping_rect_right_offset;               // u(&bsv)
	unsigned int frame_cropping_rect_top_offset;                 // u(&bsv)
	unsigned int frame_cropping_rect_bottom_offset;              // u(&bsv)
	Boolean   vui_parameters_present_flag;                      // u(&bs,1)
	//vui_seq_parameters_t vui_seq_parameters;                  // vui_seq_parameters_t
	//unsigned  separate_colour_plane_flag;                       // u(&bs,1)
} seq_parameter_set_rbsp_t;

#define XRAW_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define XRAW_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
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

seq_parameter_set_rbsp_t* ProcessSPS(unsigned char* buf2,int size)
{
	//DumpNALU(nalu);
	//assert(nalu);
	//assert(seq_params);
	unsigned char tmpBuf1[MAX_BUF_SIZE3];
	seq_parameter_set_rbsp_t seq_params={0};
	unsigned char* buf = tmpBuf1;
	BitStream bs;
	if(size>MAX_BUF_SIZE3)
		size = MAX_BUF_SIZE3;
	memcpy(buf,buf2,size);
	CleanTheBitStream(buf,size);
	SetBitstream(&bs,buf);
	seq_parameter_set_rbsp_t *sps = (seq_parameter_set_rbsp_t *)calloc(1,sizeof(seq_parameter_set_rbsp_t));
	//memset(sps,0,sizeof(seq_parameter_set_rbsp_t));

	sps->profile_idc                            = u(&bs,8);//, "SPS: profile_idc"                           , s);
	
	sps->constrained_set0_flag                  = u(&bs,1);//   "SPS: constrained_set0_flag"                 , s);
	sps->constrained_set1_flag                  = u(&bs,1);//   "SPS: constrained_set1_flag"                 , s);
	sps->constrained_set2_flag                  = u(&bs,1);//   "SPS: constrained_set2_flag"                 , s);
	int reserved_zero                           = u(&bs,5);//, "SPS: reserved_zero_5bits"                   , s);
	//	assert (reserved_zero==0);
	
	sps->level_idc                              = u(&bs,8);//, "SPS: level_idc"                             , s);
	sps->seq_parameter_set_id                   = ue(&bs);//"SPS: seq_parameter_set_id"                     , s);
	sps->chroma_format_idc = 1;
	if(sps->profile_idc==100)
	{
		sps->chroma_format_idc = ue(&bs);

		if(sps->chroma_format_idc==3)
		{
			int separate_colour_plane_flag =u(&bs,1);
			
		}
		{
			int bit_depth_luma_minus8			=ue(&bs);
			int bit_depth_chroma_minus8		=ue(&bs);
			int qpprime_y_zero_transform_bypass_flag=u(&bs,1);
			int seq_scaling_matrix_present_flag = u(&bs,1);

			if(seq_scaling_matrix_present_flag)
			{
				for(int i=0; i<8; i++)
				{
					int seq_scaling_list_present_flag   = u(&bs,1);
					
				}
			}
		}

	}
	
	sps->log2_max_frame_num_minus4              = ue(&bs);// ("SPS: log2_max_frame_num_minus4"                , s);
	sps->pic_order_cnt_type                     = ue(&bs);//"SPS: pic_order_cnt_type"                       , s);
	
	if (sps->pic_order_cnt_type == 0)
		sps->log2_max_pic_order_cnt_lsb_minus4  = ue(&bs);//"SPS: log2_max_pic_order_cnt_lsb_minus4"           , s)
	else if (sps->pic_order_cnt_type == 1)
	{
		int tmp,count;
		tmp						= u(&bs,1);
		tmp					    = se(&bs);
		tmp					    = se(&bs);
		count				    = ue(&bs);

		for(int i=0; i<count; i++)
			tmp = se(&bs);
	}											
	
	sps->num_ref_frames                        = ue(&bs);// ("SPS: num_ref_frames"                         , s);
	sps->gaps_in_frame_num_value_allowed_flag  = u(&bs,1);// ("SPS: gaps_in_frame_num_value_allowed_flag"   , s);
	sps->pic_width_in_mbs_minus1               = ue(&bs);//("SPS: pic_width_in_mbs_minus1"                , s);
	sps->pic_height_in_map_units_minus1        = ue(&bs);// ("SPS: pic_height_in_map_units_minus1"         , s);
	sps->frame_mbs_only_flag                   = u(&bs,1);//("SPS: frame_mbs_only_flag"                    , s);
	if(sps->frame_mbs_only_flag==0)
		sps->mb_adaptive_frame_field_flag        = u(&bs,1);//  ("SPS: mb_adaptive_frame_field_flag"           , s);
	sps->direct_8x8_inference_flag             = u(&bs,1); // (s);
	sps->frame_cropping_flag                   = u(&bs,1); // (s);
	if(sps->frame_cropping_flag)
	{
		sps->frame_cropping_rect_left_offset      = ue (&bs);
		sps->frame_cropping_rect_right_offset     = ue (&bs);
		sps->frame_cropping_rect_top_offset       = ue (&bs);//*(4-2*sps->frame_mbs_only_flag);//TODO:
		sps->frame_cropping_rect_bottom_offset    = ue (&bs);//*(4-2*sps->frame_mbs_only_flag);
	}
	return sps;	
}

unsigned char* FindNalu(unsigned char* buffer,int size,int targetType)
{

	int naluType = 0;
	while(naluType!=targetType)//find next NALU
	{
		//buffer = GetNextFrame(buffer,size)
		unsigned char* p = buffer;  
		unsigned char* endPos = buffer+size;
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
			return NULL; 

	}
	return buffer;
}
int GetSizeInfo(unsigned char* buf,int size,int* width,int* height)
{
	if((buf[0]&0x0f)!=7)
		return 0;//this is not sps
	static const int SubWidthC  [4]= { 1, 2, 2, 1};
	static const int SubHeightC [4]= { 1, 2, 1, 1};
	seq_parameter_set_rbsp_t *seq_params =  ProcessSPS(buf+1,size-1);
	*width = (seq_params->pic_width_in_mbs_minus1+1)*16;
	*height= (seq_params->pic_height_in_map_units_minus1+1)*16*(2-seq_params->frame_mbs_only_flag);
	if(seq_params->frame_cropping_flag)
	{
		int crop_left   = SubWidthC[seq_params->chroma_format_idc] * seq_params->frame_cropping_rect_left_offset;
		int crop_right  = SubWidthC[seq_params->chroma_format_idc] * seq_params->frame_cropping_rect_right_offset;
		int crop_top    = SubHeightC[seq_params->chroma_format_idc]*( 2 - seq_params->frame_mbs_only_flag ) * seq_params->frame_cropping_rect_top_offset;
		int crop_bottom = SubHeightC[seq_params->chroma_format_idc]*( 2 - seq_params->frame_mbs_only_flag ) * seq_params->frame_cropping_rect_bottom_offset;
		*width=*width-
			crop_left-
			crop_right;
		*height=*height-
			crop_top -
			crop_bottom;
	}
	free(seq_params);
	return 1;
}

int GetH264Profile(unsigned char* buf, int size)
{
	seq_parameter_set_rbsp_t* seq_params;
	if((buf[0]&0x0f)!=7)
		return 0;//this is not sps
	seq_params  = ProcessSPS(buf+1,size-1);
	int result = seq_params->profile_idc; 
	free(seq_params);
	return result;
}
int GetH264Levle(unsigned char* buf, int size)
{
	seq_parameter_set_rbsp_t* seq_params;
	if((buf[0]&0x0f)!=7)
		return 0;//this is not sps
	seq_params = ProcessSPS(buf+1,size-1);
	int result = seq_params->level_idc; 
	free(seq_params);
	return result;
	
}


enum{
	VO_VIDEO_FRAME_I,
	VO_VIDEO_FRAME_P,
	VO_VIDEO_FRAME_B,
	VO_VIDEO_FRAME_NULL
};


int IsRefFrame(char * buffer, int size)
{
	int naluType = buffer[0]&0x0f;
	int isRef	 = 1;
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
			return 0; 

	}
	//if(naluType==1)
	{
		isRef = (buffer[0]>>5) & 3;
	}
	return isRef;
}

int GetFrameType(char * buffer, int size)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len,value;
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
#if 0
unsigned char sps[]={0x27,0x64, 0x00, 0x28, 0xad, 0x00, 0xec,0x07,0x80,0x44, 0x7d,0xe0,0x22,0x00,0x00,0x03,0x00,0x02,0x00,0x00,0x03,0x00,0x65};
int main()
{
	
	int width,height;
	GetSizeInfo(sps,sizeof(sps),&width,&height);
	printf("%d,%d\n",width,height);
}
#endif//