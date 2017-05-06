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
* @file CCCH264PacketParser.h
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#ifndef _CCCH264PACKETPARSER_H_
#define _CCCH264PACKETPARSER_H_

#include "CCCPacketParser.h"
#include "ParseSEI.h"
#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

///<declair
class CCCPacketParser;
const VO_BYTE MAX_SERVICE_BLOCK_SIZE = 0x3F;
typedef enum
{
	CEA_608_DATA_FIELD_1 = 0,
	CEA_608_DATA_FIELD_2
}CEA_608_DATA_FIELD;

typedef struct
{
	VO_U8		nServiceNo	  :3;
	VO_U8		nBlockSize	  :5;
	VO_U8		pServData[MAX_SERVICE_BLOCK_SIZE];///<max DTVCCCaption packet size :6 bit
	VO_U8		nServDataSize;
	VO_VOID		Reset(){nServDataSize = 0;}
	VO_U32		Load(VO_PBYTE pInputData, VO_U32 cbInputData,VO_U8 SerNoSetting);
}ServiceBlock;
typedef struct
{
	VO_U8			nSeqNo;
	VO_U8			nPacketSize;
	ServiceBlock	Service;
	VO_S8			nLastSeqNo;
	VO_U8			nSerNoSetting;
	VO_VOID			Init();
	VO_U32			Load(VO_PBYTE pInputData, VO_U32 cbInputData);
}DTVCCCaption;

////<class define

class CCCH264PacketParser:public CCCPacketParser
{
public:
	CCCH264PacketParser(VO_CAPTION_PARSER_INIT_INFO* pParam);
	virtual ~CCCH264PacketParser();
	VO_VOID			Reset(){CCCPacketParser::Reset();}
public:
	VO_U32			Open(VO_CAPTION_PARSER_INIT_INFO* pParam);
	VO_U32			Process(VO_PBYTE pInputData, VO_U32 cbInputData,UserDataType eType);
	VO_U32			GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer);
	VO_U32			SetParam(VO_U32 uID, VO_PTR pParam);
	VO_S32			GetPacketData(str_cc_data_data * pstr,VO_U32 nStrCCData,VO_U32 nType);
	VO_U32			GetLanguage(VO_CAPTION_LANGUAGE_INFO** pLangInfo);
	VO_U32			SelectLanguage(VO_U32 nLangNum);
protected:
	VO_U32			ParseH264Head(VO_PBYTE pHeadData, VO_U32 nSize);
	VO_U32			CheckDataType(str_cc_data_data * pstr,VO_U32 nStrCCData);
	VO_BOOL			Is608DataValid(str_cc_data_data * pstr,VO_U32 nStrCCData);
	VO_BOOL			Is708DataValid(str_cc_data_data * pstr,VO_U32 nStrCCData);
private:
	VO_U32							m_DataField;
	CH264SEI						m_SeiDecoder;
	DTVCCCaption					m_DTVCCPacket;

private:
	VO_U32		SetCEA608DataField(VO_U32 nFieldNum);
};

#ifdef _VONAMESPACE
}
#endif

#endif //_CCCH264PACKETPARSER_H_