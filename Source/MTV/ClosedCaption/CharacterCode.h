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
* @file CharacterCode.h
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#ifndef _CHARACTERCODE_H_
#define _CHARACTERCODE_H_

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

typedef enum
{
	Color_Type_Black = 0,
	Color_Type_White = 1,
	Color_Type_Green = 2,
	Color_Type_Blue = 3,
	Color_Type_Cyan = 4,
	Color_Type_Red = 5,
	Color_Type_Yellow = 6,
	Color_Type_Magenta = 7 ,
	Color_Type_UnKnow
}Color_Type;

//Special Character Symbol Code  for CEA_608

const VO_BYTE SPECIAL_CHARACTER_AND_PREAMBLE_ADDRESS_ROW_1_2_SYMBOL_DATA_CHANNEL_1 = 0x11;
const VO_BYTE SPECIAL_CHARACTER_AND_PREAMBLE_ADDRESS_ROW_1_2_SYMBOL_DATA_CHANNEL_2 = 0x19;

//Special Character Set Table for CEA_608
const VO_BYTE SPECIAL_CHARACTER_MID_ROW_CONTROL_EXTENDED_CHAR_SET_START = 0x20;
const VO_BYTE SPECIAL_CHARACTER_MID_ROW_CONTROL_END = 0x2F;

const VO_BYTE SPECIAL_CHAR_COUNT = 0x10;
const VO_BYTE SPECIAL_CHARACTER_SPECIAL_CODE_START = 0x30;
const VO_BYTE SPECIAL_CHARACTER_SPECIAL_CODE_EXTENDED_CHAR_SET_END = 0x3F;

const VO_BYTE PREAMBLE_ADDRESS_CODE_ROW_START_1 = 0x40;
const VO_BYTE PREAMBLE_ADDRESS_CODE_ROW_END_1 = 0x5F;

const VO_BYTE PREAMBLE_ADDRESS_CODE_ROW_START_2 = 0x60;
const VO_BYTE PREAMBLE_ADDRESS_CODE_ROW_2_END_2 = 0x7F;

//Preamble Character Symbol and Extended Character Code for CEA_608
const VO_BYTE EXTENDED_CHAR_COUNT = 0x20;
const VO_BYTE PREAMBLE_ADDRESS_ROW_3_4_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_1 = 0x12;
const VO_BYTE PREAMBLE_ADDRESS_ROW_3_4_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_2 = 0x1A;

const VO_BYTE PREAMBLE_ADDRESS_ROW_5_6_SYMBOL_DATA_CHANNEL_1 = 0x15;
const VO_BYTE PREAMBLE_ADDRESS_ROW_5_6_SYMBOL_DATA_CHANNEL_2 = 0x1D;

const VO_BYTE PREAMBLE_ADDRESS_ROW_7_8_SYMBOL_DATA_CHANNEL_1 = 0x16;
const VO_BYTE PREAMBLE_ADDRESS_ROW_7_8_SYMBOL_DATA_CHANNEL_2 = 0x1E;

const VO_BYTE PREAMBLE_ADDRESS_ROW_9_10_AND_CONTROL_2_SYMBOL_DATA_CHANNEL_1 = 0x17;
const VO_BYTE PREAMBLE_ADDRESS_ROW_9_10_AND_CONTROL_2_SYMBOL_DATA_CHANNEL_2 = 0x1F;

const VO_BYTE PREAMBLE_ADDRESS_ROW_11_SYMBOL_DATA_CHANNEL_1 = 0x10;
const VO_BYTE PREAMBLE_ADDRESS_ROW_11_SYMBOL_DATA_CHANNEL_2 = 0x18;

const VO_BYTE PREAMBLE_ADDRESS_ROW_12_13_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_1 = 0x13;
const VO_BYTE PREAMBLE_ADDRESS_ROW_12_13_SYMBOL_EXTENDED_CHAR_SET_DATA_CHANNEL_2 = 0x1B;

const VO_BYTE PREAMBLE_ADDRESS_ROW_14_15_AND_CONTROL_1_SYMBOL_DATA_CHANNEL_1 = 0x14;
const VO_BYTE PREAMBLE_ADDRESS_ROW_14_15_AND_CONTROL_1_SYMBOL_DATA_CHANNEL_2 = 0x1C;

//Control Character Set Table for CEA_608
const VO_BYTE CONTROL_CHARACTER_START = 0x20;
const VO_BYTE CONTROL_CHARACTER_END = 0x2F;



//Standard Character Set Table for CEA_608
const VO_BYTE STANDARD_CHARACTER_START = 0x20;
const VO_BYTE STANDARD_CHARACTER_END = 0x7F;

//add extra code for display
const VO_BYTE CARRIAGE_RETURN_CHAR = 0x0A;
const VO_BYTE ASCII_SPACE_CHAR = 0x20;
///<add ext code for unicode buffer
const VO_BYTE FONT_PARAM_SRART_CODE = 0x0;


///<===================================================================================
///<ARIB standard


///<EUC-JP charset
const VO_BYTE EUCJP_CHARACTER_SET_START = 0xA1;
const VO_BYTE EUCJP_CHARACTER_SET_END = 0xFE;

///<ARIB control char
const VO_BYTE ARIB_CONTROL_CHARACTER_START = 0x00;
const VO_BYTE ARIB_CONTROL_CHARACTER_END = 0xA0;
const VO_BYTE ARIB_CONTROL_CHARACTER_SPECIAL = 0xFF;

//<Shift-JIS codec char
const VO_BYTE SJIS_TWO_CHAR_CODEC_SET_1_FIRST_BYTE_START = 0x81;
const VO_BYTE SJIS_TWO_CHAR_CODEC_SET_1_FIRST_BYTE_END = 0x9F;
const VO_BYTE SJIS_TWO_CHAR_CODEC_SET_2_FIRST_BYTE_START = 0xE0;
const VO_BYTE SJIS_TWO_CHAR_CODEC_SET_2_FIRST_BYTE_END = 0xFC;

const VO_BYTE SJIS_TWO_CHAR_CODEC_SET_1_SECOND_BYTE_START = 0x40;
const VO_BYTE SJIS_TWO_CHAR_CODEC_SET_1_SECOND_BYTE_END = 0x7E;
const VO_BYTE SJIS_TWO_CHAR_CODEC_SET_2_SECOND_BYTE_START = 0x80;
const VO_BYTE SJIS_TWO_CHAR_CODEC_SET_2_SECOND_BYTE_END = 0xFC;

const VO_BYTE ARIB_MAX_CHAR_COUNT_ONE_ROW = 0x10;///<0x10


///<===================================================================================
///<CEA708
const VO_BYTE C0_SET_START_CODE_CEA_708 = 0x0;
const VO_BYTE C0_SET_END_CODE_CEA_708 = 0x1F;

const VO_BYTE C1_SET_START_CODE_CEA_708 = 0x80;
const VO_BYTE C1_SET_END_CODE_CEA_708 = 0x9F;

const VO_BYTE G0_G2_SET_START_CODE_CEA_708 = 0x20;
const VO_BYTE G0_G2_SET_END_CODE_CEA_708 = 0x7F;

const VO_BYTE G1_G3_SET_START_CODE_CEA_708 = 0xA0;
const VO_BYTE G1_G3_SET_END_CODE_CEA_708 = 0xFF;

const VO_BYTE EXT_CODE_CEA_708 = 0x10;

const VO_BYTE COUNT_G2G3_CHARACTER = 0x1B;

const VO_BYTE MAX_ROW_COUNT = 0xF;
const VO_BYTE MAX_COLUMN_COUNT = 0x20;
const VO_BYTE MAX_COLUMN_COUNT_16_9 = 0x2A;
const VO_BYTE CC708RGB_TIMESCALE = 85;///<255/3

#ifdef _VONAMESPACE
}
#endif

#endif ///<_CHARACTERCODE_H_