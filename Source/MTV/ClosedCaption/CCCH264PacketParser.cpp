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
* @file CCCH264PacketParser.cpp
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#include "CCCH264PacketParser.h"
#include "CCCharParser.h"
#include "CC708CharParser.h"
#include "voLog.h"
#include "fMacros.h"
#include "readutil.h"
#include "CDumper.h"

///<Caption data in h264 NALu

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CCCH264PacketParser::CCCH264PacketParser(VO_CAPTION_PARSER_INIT_INFO* pParam)
:CCCPacketParser(pParam)
,m_DataField(CEA_608_DATA_FIELD_1)
{
	m_DTVCCPacket.Init();
	if (pParam->nType == VO_CAPTION_TYPE_EIA608)
	{
		m_CharParser =  new CC608CharParser(&m_InitInfo);
	}
	else if (pParam->nType == VO_CAPTION_TYPE_EIA708)
	{
		m_CharParser =  new CC708CharParser(&m_InitInfo);
	}
	else if (pParam->nType == VO_CAPTION_TYPE_DEFAULT_608 || pParam->nType == VO_CAPTION_TYPE_DEFAULT_708)///<new char parser later
	{
	}
	else
		VOLOGE("ClosedCaption Error:Not support EIA Type Data");
	
}
CCCH264PacketParser::~CCCH264PacketParser()
{
}

VO_U32 CCCH264PacketParser::Open(VO_CAPTION_PARSER_INIT_INFO* pParam)
{
	if (!pParam)
	{
		VOLOGE("ClosedCaption Error:pParam is Null");
		return VO_ERR_PARSER_INVLAID_PARAM_ID;
	}
	if (pParam->stDataInfo.nType == 0)
	{
		if (pParam->stDataInfo.nSize == 0 || !pParam->stDataInfo.pHeadData)
		{
			VOLOGE("ClosedCaption Error:No H264 Sequence Head");
			return VO_ERR_PARSER_INVLAID_PARAM_ID;
		}
		if(VO_ERR_PARSER_OK != ParseH264Head(pParam->stDataInfo.pHeadData,pParam->stDataInfo.nSize))
		{
			VOLOGE("ClosedCaption Error:ParseH264Head Failed");
			return VO_ERR_PARSER_INVLAID_PARAM_ID;
		}
	}
	return VO_ERR_PARSER_OK;
}
VO_U32 CCCH264PacketParser::SetParam(VO_U32 uID, VO_PTR pParam)
{
	if (uID == VO_PARAMETER_ID_CHANNEL)
	{
		if (VO_CAPTION_TYPE_EIA608 == m_InitInfo.nType)
		{
			VO_U32 nFileNum = (*(VO_U32*)pParam)/2;
			if(m_CharParser && VO_ERR_PARSER_OK == SetCEA608DataField(nFileNum))
			{
				return m_CharParser->SetParam(pParam);
			}
		}
		else if (VO_CAPTION_TYPE_EIA708 == m_InitInfo.nType)
		{
		}
		
	}
	else if (m_CharParser && uID == VO_PARAMETER_ID_DATAINFO)
	{
		Reset();
	}
	else if (m_CharParser && uID == VO_PARAMETER_ID_RESTART)
	{
		m_CharParser->ReStart();
	}
	return VO_ERR_PARSER_OK;
}
VO_U32 CCCH264PacketParser::SetCEA608DataField(VO_U32 nFieldNum)
{
	if (nFieldNum>=VO_CAPTION_CHANNEL_1 && nFieldNum<=VO_CAPTION_CHANNEL_UNKOWN)
	{
		m_DataField = nFieldNum;
	}
	return VO_ERR_PARSER_OK;
}

VO_U32 CCCH264PacketParser::CheckDataType(str_cc_data_data * pstr,VO_U32 nStrCCData)
{
	if (!pstr || nStrCCData == 0)
	{
		return VO_ERR_PARSER_INPUT_BUFFER_SMALL;
	}
	VO_BOOL b608Valid = VO_FALSE,b708Valid = VO_FALSE;

	if(Is608DataValid(pstr,nStrCCData))
	{
		b608Valid = VO_TRUE;
	}
	if (Is708DataValid(pstr,nStrCCData))
	{
		b708Valid = VO_TRUE;
	}
	if (!b608Valid && !b708Valid)
	{
		return VO_ERR_PARSER_INPUT_BUFFER_SMALL;
	}
	if (m_InitInfo.nType == VO_CAPTION_TYPE_DEFAULT_608)
	{
		if (b608Valid)
		{
			m_InitInfo.nType = VO_CAPTION_TYPE_EIA608;
		}
		else
			m_InitInfo.nType = VO_CAPTION_TYPE_EIA708;
	}
	else if (m_InitInfo.nType == VO_CAPTION_TYPE_DEFAULT_708)
	{
		if (b708Valid)
		{
			m_InitInfo.nType = VO_CAPTION_TYPE_EIA708;
		}
		else
			m_InitInfo.nType = VO_CAPTION_TYPE_EIA608;
	}
	if (m_InitInfo.nType == VO_CAPTION_TYPE_EIA608)
	{
		m_CharParser =  new CC608CharParser(&m_InitInfo);
		if(!m_CharParser)
			return VO_ERR_PARSER_OUT_OF_MEMORY;
	}
	else
	{
		m_CharParser =  new CC708CharParser(&m_InitInfo);
		if(!m_CharParser)
			return VO_ERR_PARSER_OUT_OF_MEMORY;
	}
	
	return VO_ERR_PARSER_OK;
}
VO_BOOL CCCH264PacketParser::Is608DataValid(str_cc_data_data * pstr,VO_U32 nStrCCData)
{
	VO_S32 ret = 0;
	while((ret = GetPacketData(pstr,nStrCCData,VO_CAPTION_TYPE_EIA608)) > 0)
	{
		pstr += ret;
		nStrCCData -= ret;
		VO_U32 nCnt = 0;
		while(nCnt < m_nCntData)
		{
			m_pCCData[nCnt] &= 0x7f;
			if (m_pCCData[nCnt])
			{
				Reset();
				return VO_TRUE;
			}
			nCnt++;
		}
	}
	Reset();
	return VO_FALSE;
}

VO_BOOL CCCH264PacketParser::Is708DataValid(str_cc_data_data * pstr,VO_U32 nStrCCData)
{
	VO_S32 ret = 0;
	while((ret = GetPacketData(pstr,nStrCCData,VO_CAPTION_TYPE_EIA708)) > 0)
	{
		Reset();
		return VO_TRUE;
	}
	Reset();
	return VO_FALSE;
}
VO_S32 CCCH264PacketParser::GetPacketData(str_cc_data_data * pstr,VO_U32 nStrCCData,VO_U32 nType)
{
	if (!pstr || !nStrCCData)
	{
		return -1;
	}
	for (VO_U32 nCnt = 0;nCnt < nStrCCData;nCnt++)
	{
		if (VO_CAPTION_TYPE_EIA608 == nType)
		{
			if (pstr[nCnt].cc_valid == 0x1 && pstr[nCnt].cc_type == m_DataField)
			{
				m_pCCData[m_nCntData++] = pstr[nCnt].cc_data_1;
				m_pCCData[m_nCntData++] = pstr[nCnt].cc_data_2;
				return nCnt+1;
			}
		}
		else if (VO_CAPTION_TYPE_EIA708 == nType)
		{
			if (m_nCntData > 0 && ((pstr[nCnt].cc_type ==0x3 && pstr[nCnt].cc_valid == 0x1)||
				pstr[nCnt].cc_valid == 0x0))
			{
				VO_U32 nDataSize = m_DTVCCPacket.Load(m_pCCData,m_nCntData);
				if (nDataSize)
				{
					MemCopy(m_pCCData,m_DTVCCPacket.Service.pServData,m_DTVCCPacket.Service.nServDataSize);
					m_nCntData = m_DTVCCPacket.Service.nServDataSize;
					return nCnt;
				}
				Reset();
			}
			if (pstr[nCnt].cc_valid == 0x1 && (pstr[nCnt].cc_type == 2 || pstr[nCnt].cc_type == 3))
			{
				m_pCCData[m_nCntData++] = pstr[nCnt].cc_data_1;
				m_pCCData[m_nCntData++] = pstr[nCnt].cc_data_2;
			}
		}
		else
		{
			VOLOGE("ClosedCaption Error:Not support EIA Type Data");
			return -1;
		}
	}
	return 0;
}

VO_U32 CCCH264PacketParser::ParseH264Head(VO_PBYTE pHeadData, VO_U32 nSize)
{
	return VO_ERR_PARSER_OK;
}


VO_U32 CCCH264PacketParser::Process(VO_PBYTE pInputData, VO_U32 cbInputData,UserDataType eType)
{
	if (!pInputData || cbInputData == 0)
	{
		VOLOGE("ClosedCaption Error:invalid input data");
		return VO_ERR_PARSER_FAIL;
	}
//	VOLOGE("dump raw data:%d",cbInputData);
//	CDumper::DumpRawData((VO_PBYTE)&cbInputData,4);///<dump 4 byte's size
//	CDumper::DumpRawData(pInputData,cbInputData);///<dump the data
		
	///<Get the characters from SEI_NALU
	str_cc_data_data *pStrCCData = NULL;
	VO_U32	 nStrCCData = 0;
	VO_S32 ret = 0;
	if (eType == UserDataType_Sei)
	{
		ret = m_SeiDecoder.ParserSEI(pInputData,cbInputData,pStrCCData,nStrCCData);
	}
	else if(eType == UserDataType_Sei_Payload)
	{
		ret = m_SeiDecoder.ParserSEIPayload(pInputData,cbInputData,pStrCCData,nStrCCData);
	}
	else
	{
		VOLOGE("ClosedCaption Error:Unknown UserDataType:%d",eType);
	}
	if (ret == 0)
	{
		return VO_ERR_PARSER_OK;
	}
	if (!m_CharParser)
	{
		if(VO_ERR_PARSER_OK != CheckDataType(pStrCCData,nStrCCData))
			return VO_ERR_PARSER_OK;
	}
	while((ret = GetPacketData(pStrCCData,nStrCCData,m_InitInfo.nType)) > 0)
	{
		pStrCCData += ret;
		nStrCCData -= ret;
		ret = m_CharParser->Process(m_pCCData,m_nCntData);
		Reset();
	}	
	return ret;
}

VO_U32 CCCH264PacketParser::GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pBuffer)
{
	pBuffer->pSubtitleInfo = NULL;
	if (!m_CharParser)
	{
		return VO_ERR_PARSER_INPUT_BUFFER_SMALL;
	}
	return m_CharParser->GetData(pBuffer);
}

VO_U32 ServiceBlock::Load(VO_PBYTE pInputData, VO_U32 cbInputData,VO_U8 SerNoSetting)
{
	Reset();
	VO_PBYTE pTmp = pInputData;
	VO_S32 nTmpSize = cbInputData;
	while (nTmpSize > 0)
	{
		nServiceNo = R3B7(*pTmp);
		nBlockSize = R5B4(*pTmp++);
		nTmpSize--;
		if (nServiceNo == 0)
		{
			if (nBlockSize !=0)
			{
				VOLOGE("ClosedCaption Error:nServiceNo is 0,but nBlockSize is not 0");
			}		
			return nServDataSize;///<once NULL service block,there are no more service block need to be processed
		}
		if (nServiceNo == 7 && nBlockSize > 0)
		{
			nServiceNo = R6B5(*pTmp++);
			nTmpSize--;
		}
		if (nServiceNo == SerNoSetting)
		{
			if (nBlockSize > cbInputData -(pTmp - pInputData))
			{
				nBlockSize = cbInputData -(pTmp - pInputData);
				VOLOGE("ClosedCaption Error:ServiceBlock Packet size bigger than nBlockSize");
			}
			if (nServDataSize + nBlockSize > MAX_SERVICE_BLOCK_SIZE)
			{
				VOLOGE("ClosedCaption Error:ServiceBlock Packet Out of Memory");
				nBlockSize = 63 - nServDataSize;
			}
			memcpy(pServData+nServDataSize,pTmp,nBlockSize);
			nServDataSize += nBlockSize;
		}
		pTmp += nBlockSize;
		nTmpSize -= nBlockSize;
	}
	return nServDataSize;
	
}

VO_U32 DTVCCCaption::Load(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	VO_PBYTE pTmp = pInputData;
	nSeqNo = R2B7(*pTmp);
	if (nLastSeqNo != -1 && nSeqNo != (nLastSeqNo + 1)%4)
	{
		VOLOGE("ClosedCaption Error:DTVCCCaption Packet lost");///<we should add packet lost handle
	}
	nLastSeqNo = nSeqNo;
	nPacketSize = R6B5(*pTmp++);
	if (!nPacketSize)
	{
		nPacketSize = 128;
	}
	else
	{
		nPacketSize = nPacketSize*2 - 1;
	}
	if (nPacketSize > cbInputData - 1)
	{
		nPacketSize = cbInputData - 1;
		VOLOGE("ClosedCaption Error:DTVCCCaption Packet size bigger than nPacketSize");
	}
	return Service.Load(pTmp,nPacketSize,nSerNoSetting);
}
VO_VOID DTVCCCaption::Init()
{
	nLastSeqNo = -1;
	nSerNoSetting = VO_CAPTION_CHANNEL_2;///<service #1,primary caption service
	Service.Reset();
}

VO_U32 CCCH264PacketParser::GetLanguage(VO_CAPTION_LANGUAGE_INFO** pLangInfo)
{
	//VOLOGE("CCCH264PacketParser::GetLanguage");
	if (!m_CharParser)
	{
		return VO_ERR_PARSER_INVLAID_HANDLE;
	}
	VOLOGE("CCCH264PacketParser::GetLanguage 1:%d:%d",m_Langinfo.nLangCnt,m_InitInfo.nType);
	if (m_Langinfo.nLangCnt == 0)
	{
		
		*pLangInfo = NULL;

		if (m_InitInfo.nType == VO_CAPTION_TYPE_EIA608)
		{
			m_Langinfo.nLangCnt = 4;
			m_Langinfo.pLanguage = new VO_CAPTION_LANGUAGE *[4];
			for (VO_U32 nCnt = 0; nCnt < 4;nCnt++)
			{
				m_Langinfo.pLanguage[nCnt] = new VO_CAPTION_LANGUAGE;
				m_Langinfo.pLanguage[nCnt]->chLangName[0] = 'c';
				m_Langinfo.pLanguage[nCnt]->chLangName[1] = 'c';
				m_Langinfo.pLanguage[nCnt]->chLangName[2] = nCnt + 0x31;
				m_Langinfo.pLanguage[nCnt]->chLangName[3] = 0;
			}
		}
		else if (m_InitInfo.nType == VO_CAPTION_TYPE_EIA708)
		{
			m_Langinfo.nLangCnt = 1;
			m_Langinfo.pLanguage = new VO_CAPTION_LANGUAGE *[1];
			m_Langinfo.pLanguage[0] = new VO_CAPTION_LANGUAGE;
			m_Langinfo.pLanguage[0]->chLangName[0] = 'c';
			m_Langinfo.pLanguage[0]->chLangName[1] = 'c';
			m_Langinfo.pLanguage[0]->chLangName[2] = 0;
			m_Langinfo.pLanguage[0]->chLangName[3] = 0;
			//return VO_ERR_PARSER_NOT_IMPLEMENT;
		}
	}
	*pLangInfo = &m_Langinfo;
	return VO_ERR_PARSER_OK;
}

VO_U32 CCCH264PacketParser::SelectLanguage(VO_U32 nLangNum)
{
	VOLOGE("CCCH264PacketParser SelectLanguage:%d",nLangNum);
	if (!m_CharParser)
	{
		return VO_ERR_PARSER_INVLAID_HANDLE;
	}
	VOLOGE("CCCH264PacketParser SelectLanguage 2:%d",nLangNum);
	if (m_InitInfo.nType == VO_CAPTION_TYPE_EIA608)
	{
		VOLOGE("CCCH264PacketParser SelectLanguage 3:%d",nLangNum);
		VO_U32 nFileNum = nLangNum / 2;
		if(m_CharParser && VO_ERR_PARSER_OK == SetCEA608DataField(nFileNum))
		{
			VOLOGE("CCCH264PacketParser SelectLanguage 4:%d",nLangNum);
			return m_CharParser->SetParam((VO_PTR)&nLangNum);
		}
	}
	else if (m_InitInfo.nType == VO_CAPTION_TYPE_EIA708)
	{
		return VO_ERR_PARSER_NOT_IMPLEMENT;
	}
	return VO_ERR_PARSER_OK;
}