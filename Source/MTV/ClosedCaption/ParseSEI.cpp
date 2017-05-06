/************************************************************************
VisualOn Proprietary
Copyright (c) 2003, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
/************************************************************************
* @file ParseSEI.cpp
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#include "ParseSEI.h"
#include "memory.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CH264SEI::CH264SEI()
{

}
CH264SEI::~CH264SEI()
{

}

VO_VOID CH264SEI::Reset()
{
	memset(&cc_data,0x0,sizeof(cc_data));
}
BitStreamH264* CH264SEI::GetBitStream()
{
	return &bs;
}
VO_S32 CH264SEI::SetBitstream(VO_PBYTE pos)
{
	bs.pos = pos;
	bs.bit_offset = 0;
	return 1;
}

VO_VOID CH264SEI::SetBitstream(BitStreamH264 &bitstream)
{
	bs.pos = bitstream.pos;
	bs.bit_offset = bitstream.bit_offset;
}

VO_VOID CH264SEI::AdjustBitstream(BitStreamH264 *bs,VO_S32 len)
{
	bs->pos+=(len+bs->bit_offset)/8;
	bs->bit_offset = (len+bs->bit_offset)%8;
}
VO_S32 CH264SEI::ForwardBitstream(VO_S32 len)
{
	AdjustBitstream(&bs,len);
	return len;
}
VO_S32 CH264SEI::ShowBits(VO_S32 len)
{
	VO_PBYTE buf = bs.pos;
	VO_S32 bit_offset = 7-bs.bit_offset;
	VO_S32 value = 0;
	VO_S32 byte_offset = 0;
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

VO_S32 CH264SEI::GetBits(BitStreamH264* bs,VO_S32 len)
{
	VO_S32 forward = len;
	VO_PBYTE buf = bs->pos;
	VO_S32 bit_offset = 7-bs->bit_offset;
	VO_S32 value = 0;
	VO_S32 byte_offset = 0;
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

VO_S32 CH264SEI::u(VO_S32 len)
{
	return GetBits(&bs,len);	
}

VO_S32 CH264SEI::f(VO_S32 len)
{
	return GetBits(&bs,len);
}

VO_S32 CH264SEI::ue()
{
	CodeWord code = GetVLCSymbolH264(&bs);
	return (1<<code.len)+code.info-1; 
}


CodeWord CH264SEI::GetVLCSymbolH264(BitStreamH264* bs)
{
	VO_PBYTE buf = bs->pos;// = GetBitStreamPos();
	VO_S32 bit_offset = 7-bs->bit_offset;
	VO_S32 ctr_bit = (buf[0] &(0x01<<bit_offset));
	VO_S32 len = 0;
	VO_S32 byte_offset = 0;
	VO_S32 inf,info_bit;
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
VO_S32 CH264SEI::GetNextLevel()
{
	VO_PBYTE buf = bs.pos;// = GetBitStreamPos();
	VO_S32 bit_offset = 7-bs.bit_offset;
	VO_S32 ctr_bit = (buf[0] &(0x01<<bit_offset));
	VO_S32 len = 0;
	//VO_S32 bit_counter = 0;
	VO_S32 byte_offset = 0;
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

//typedef VO_S32 Boolean;

VO_S32 CH264SEI::CleanTheBitStream(VO_PBYTE head, VO_S32 size)
{
	
	VO_PBYTE end  = head + size;
	
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


VO_S32 CH264SEI::rbsp_trailing_bits()
{
	return ShowBits(8);
}

VO_S32 CH264SEI::DecodeCCData(VO_S32 nCCDataSize)
{
	if (nCCDataSize < 2)
	{
		return 0;
	}
//	VO_S32 nCnt_cc_data_data = 0;
	VO_S32 tmp = GetBits(&bs,8);
	cc_data.process_cc_data_flag = (tmp>>6) & 0x1;
	cc_data.cc_count = tmp & 0x1f;
	cc_data.reserved_2 = GetBits(&bs,8);
	if (cc_data.cc_count * 3 > nCCDataSize - 2 )
	{
		cc_data.cc_count = (nCCDataSize - 2)/3;
	}
	for (VO_S32 i = 0;i< cc_data.cc_count;i++)
	{
		VO_S32 tmp = GetBits(&bs,8);
		cc_data.data[i].cc_valid = (tmp >>2) & 0x01;
		cc_data.data[i].cc_type = tmp & 0x03;
		cc_data.data[i].cc_data_1 = GetBits(&bs,8);
		cc_data.data[i].cc_data_2 = GetBits(&bs,8);
	}
	return cc_data.cc_count;

}
VO_S32 CH264SEI::DecodeRegistered_ITU_T_T35_ForClosedCaption(VO_S32 nPayloadSize)
{
	if (nPayloadSize < 10)
	{
		return 0;
	}
	VO_S32 itu_t_t35_country_code = GetBits(&bs,8);
	VO_S32 itu_t_t35_provider_code = GetBits(&bs,16);
	VO_S32 user_identifier = GetBits(&bs,32);
	if (0xb5 != itu_t_t35_country_code || 0x0031 != itu_t_t35_provider_code || user_identifier != 0x47413934)
	{
		return 0;
	}
	VO_S32 user_data_type_code = GetBits(&bs,8);	
	if (user_data_type_code != 0x3)
	{
		return 0;
	}
	VO_S32 ret = DecodeCCData(nPayloadSize - 8);
	if (u(8) != 0xff)
	{
		return 0;
	}
	return ret;


}
VO_BOOL CH264SEI::ParserSEI(VO_PBYTE pInPutbuf,VO_U32 InPutbufSize ,str_cc_data_data*& pStrCC,VO_U32 &nCntStrCC)
{
	Reset();
	pStrCC = cc_data.data;
	nCntStrCC = 0;
	CleanTheBitStream(pInPutbuf,InPutbufSize);
	SetBitstream(pInPutbuf);
	VO_S32 type=0,size = 0;

	//////add code
	do 
	{
		type=0;
		size = 0;
		do 
		{
			type += ShowBits(8);
		} while (0xff == GetBits(&bs,8));

		do{
			size+= ShowBits(8);
		}while(0xff == GetBits(&bs,8));

		if ((VO_U32)((bs.pos - pInPutbuf) + size) > InPutbufSize)
		{
			return VO_FALSE;
		}
		BitStreamH264 bs_bak = bs;
		
		switch(type){
		case SEI_TYPE_REGISTERED_ITU_T_T35:
			{
				VO_S32 count = DecodeRegistered_ITU_T_T35_ForClosedCaption(size);
				if (count > 0 && count < 32)
				{
					nCntStrCC += count;
				}
			}
			
			break;
		case SEI_TYPE_PIC_TIMING: // Picture timing SEI
		case SEI_TYPE_USER_DATA_UNREGISTERED:
		case SEI_TYPE_RECOVERY_POINT:
		case SEI_BUFFERING_PERIOD:
		default:
			break;
		}
		///<skip the current payload
		SetBitstream(bs_bak);
		ForwardBitstream(size*8);

	} while ((VO_U32)(bs.pos - pInPutbuf) < InPutbufSize - 2);

	return (VO_BOOL)(nCntStrCC > 0);
}

VO_BOOL CH264SEI::ParserSEIPayload(VO_PBYTE pInPutbuf,VO_U32 InPutbufSize ,str_cc_data_data*& pStrCC,VO_U32 &nCntStrCC)
{
	Reset();
	pStrCC = cc_data.data;
	nCntStrCC = 0;
	CleanTheBitStream(pInPutbuf,InPutbufSize);
	SetBitstream(pInPutbuf);

	VO_S32 count = DecodeRegistered_ITU_T_T35_ForClosedCaption(InPutbufSize);
	if (count > 0 && count < 32)
	{
		nCntStrCC += count;
	}
	
	return (VO_BOOL)(nCntStrCC > 0);
}