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
* @file CCCPacketParser.h
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#ifndef _CCCPACKETPARSER_H_
#define _CCCPACKETPARSER_H_
#include "voCaptionParser.h"
#include "voType.h"
#include "CvoBaseMemOpr.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum UserDataType
{
	UserDataType_Sei = 0,
	UserDataType_Sei_Payload
};

///<declair
class CCCharParser;

////<class define
class CCCPacketParser:public CvoBaseMemOpr
{
public:
	CCCPacketParser(VO_CAPTION_PARSER_INIT_INFO* pParam);
	virtual ~CCCPacketParser();
	VO_VOID	 Reset();
public:
	virtual	VO_U32 Open(VO_CAPTION_PARSER_INIT_INFO* pParam){ return VO_ERR_PARSER_OK;}
	virtual VO_U32 Process(VO_PBYTE pInputData, VO_U32 cbInputData,UserDataType eType);
	virtual	VO_U32 GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pOutputData);
	virtual VO_U32 SetParam(VO_U32 uID, VO_PTR pParam){return VO_ERR_PARSER_OK;}
	virtual VO_U32 GetParam(VO_U32 uID, VO_PTR pParam){return VO_ERR_PARSER_OK;}
	virtual VO_U32 GetDataInfoType(){return m_InitInfo.stDataInfo.nType;}
	virtual VO_U32 GetLanguage(VO_CAPTION_LANGUAGE_INFO** pLangInfo){return VO_ERR_PARSER_OK;}
	virtual VO_U32 SelectLanguage(VO_U32 nLangNum){return VO_ERR_PARSER_OK;}
protected:
	VO_PBYTE					m_pCCData;
	VO_U32						m_nCntData;
	CCCharParser *				m_CharParser;
	VO_CAPTION_PARSER_INIT_INFO m_InitInfo;
	VO_CAPTION_LANGUAGE_INFO	m_Langinfo;
private:
	
};

///<===========================================================================================================
#define ARIB_SUPPORT_MAX_LANGUAGE_COUNT 8
///<CEIAHeaderPacketParser
typedef struct
{
	VO_U8 Hour_FirstDig : 4;///<first digit of value BCD coded 
	VO_U8 Hour_SecndDig : 4;

	VO_U8 Minute_FirstDig : 4;
	VO_U8 Minute_SecndD : 4;

	VO_U8 Second_FirstDig : 4;
	VO_U8 Second_SecndDig : 4;

	VO_U32 MilliSecond_FirstDig : 4;
	VO_U32 MilliSecond_SecndDig : 4;
	VO_U32 MilliSecond_ThirdDig : 4;
	VO_U32 Reserved : 4;
	VO_VOID ReadTime(VO_PBYTE& pInputData, VO_U32 cbInputData);
	VO_U8 GetHour(){return 0;};
	VO_U8 GetMinute(){return 0;};
	VO_U8 GetSecond(){return 0;};
	VO_U16 GetMilliSecond(){return 0;};
}BCD_OTM;

typedef struct
{
	VO_U8	language_tag : 3;
	VO_U8	Reserved : 1;
	VO_U8	DMF : 4;
	VO_U8	DC;///<8 bit
	VO_U32	iso_language_code;///<24 bit
	VO_U8	format : 4;
	VO_U8	TCS : 2;
	VO_U8	rollup_mode : 2;
}Language_info;

typedef struct 
{
	VO_U8		unit_separator;
	VO_U8		data_unit_parameter;
	VO_U32		data_unit_size;
	VO_PBYTE	Load(VO_PBYTE pInputData, VO_U32 cbInputData);
	VO_VOID		Reset();
}Arid_DataUnit;

typedef struct 
{
	VO_U8			TMD :2;
	VO_U8			Reserved :6;
	BCD_OTM			OTM;///<40 bit
	VO_U8			number_language;///< 8 bit
	Language_info	language_info[ARIB_SUPPORT_MAX_LANGUAGE_COUNT];///<6*8 bit
	VO_U32			data_unit_loop_length;///<24bit
//	Arid_DataUnit data_unit;
	VO_PBYTE		Load(VO_PBYTE pInputData, VO_U32 cbInputData);
}Arid_CaptionManagementData;

typedef struct 
{
	VO_U8		TMD :2;
	VO_U8		Reserved :6;
	BCD_OTM		SMT;///<40 bit
	VO_U32		data_unit_loop_length;
//	Arid_DataUnit data_unit;
	VO_PBYTE	Load(VO_PBYTE pInputData, VO_U32 cbInputData);
}Arid_CaptionStatementData;

typedef struct 
{
	VO_U8			data_group_id :6;
	VO_U8			data_group_version :2;
	VO_U8			data_group_link_number;///<8 bit
	VO_U8			last_data_group_link_number;///<8 bit
	VO_U16			data_group_size;///<16 bit
	union
	{
		Arid_CaptionManagementData management_data;
		Arid_CaptionStatementData statement_data;
	};
	Arid_DataUnit	data_unit;
	VO_U16			CRC_16;///<16 bit
	VO_PBYTE		Load(VO_PBYTE pInputData, VO_U32 cbInputData);
}Arid_CaptionDataGroup;

class CEIAHeaderPacketParser:public CCCPacketParser ,public Arid_CaptionDataGroup
{
public:
	CEIAHeaderPacketParser(VO_CAPTION_PARSER_INIT_INFO* pParam);
	virtual ~CEIAHeaderPacketParser();
public:
	VO_U32 Process(VO_PBYTE pInputData, VO_U32 cbInputData,VO_PBYTE& pOutputBuf,VO_U32& nOutputSiz);
	VO_U32 ParserPESPacket(VO_PBYTE pInputData, VO_U32 cbInputData);
protected:
private:
};

#ifdef _VONAMESPACE
}
#endif

#endif //_CCCPACKETPARSER_H_