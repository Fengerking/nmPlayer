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
* @file ParseSEI.h
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#ifndef _PARSESEI_H_
#define _PARSESEI_H_
#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define MAX_CC_DATA_DATA_COUNT 32
typedef struct 
{
	VO_BYTE one_bit :1;
	VO_BYTE reserved :4;
	VO_BYTE cc_valid :1;
	VO_BYTE cc_type :2;
	VO_BYTE cc_data_1;
	VO_BYTE cc_data_2;

}str_cc_data_data;
typedef struct  
{
	VO_BYTE reserved_1 :1;
	VO_BYTE process_cc_data_flag :1;
	VO_BYTE one_bit :1;
	VO_BYTE cc_count :5;
	VO_BYTE reserved_2 ;
	str_cc_data_data data[MAX_CC_DATA_DATA_COUNT]; 
}str_cc_data;

typedef struct  
{
	VO_S32 bit_offset;
	VO_PBYTE pos;
}BitStreamH264;

typedef struct 
{
	int len;
	int info;
}CodeWord;

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

class CH264SEI
{
public:	
	CH264SEI();
	~CH264SEI();
public:
	VO_BOOL ParserSEI(VO_PBYTE pInPutbuf,VO_U32 InPutbufSize ,str_cc_data_data* &pStrCC,VO_U32 &nCntStrCC);
	VO_BOOL ParserSEIPayload(VO_PBYTE pInPutbuf,VO_U32 InPutbufSize ,str_cc_data_data* &pStrCC,VO_U32 &nCntStrCC);
	VO_VOID Reset();
protected:
private:
	CodeWord			code;
	BitStreamH264		bs;
	str_cc_data			cc_data;
private:
	VO_S32				u(VO_S32 len);
	VO_S32				ue();
	VO_S32				f(VO_S32 len);

	VO_S32				SetBitstream(VO_PBYTE pos);
	VO_VOID				SetBitstream(BitStreamH264 &bitstream);
	BitStreamH264*		GetBitStream();
	VO_VOID				AdjustBitstream(BitStreamH264 *bs,VO_S32 len);
	VO_S32				ForwardBitstream(VO_S32 len);
	VO_S32				ShowBits(VO_S32 len);
	VO_S32				GetBits(BitStreamH264* bs,VO_S32 len);
	VO_S32				GetNextLevel();
	CodeWord			GetVLCSymbolH264(BitStreamH264* bs);
	VO_S32				CleanTheBitStream(VO_PBYTE head, VO_S32 size);

	VO_S32				rbsp_trailing_bits();
	VO_S32				DecodeCCData(VO_S32 nCCDataSize);
	VO_S32				DecodeRegistered_ITU_T_T35_ForClosedCaption(VO_S32 nPayloadSize);
};

#ifdef _VONAMESPACE
}
#endif

#endif//_PARSESEI_H_