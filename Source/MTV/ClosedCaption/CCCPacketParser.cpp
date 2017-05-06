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
* @file CCCPacketParser.cpp
* 
*
*
* @author  Dolby Du
* @author  Ferry Zhang
* Change History
* 2012-11-28  Create File
************************************************************************/
#include "CCCPacketParser.h"
#include "voLog.h"
#include "fMacros.h"
#include "CCCharParser.h"
#include "readutil.h"
#include "CvoBaseMemOpr.h"
#include "CDumper.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CCCPacketParser::CCCPacketParser(VO_CAPTION_PARSER_INIT_INFO* pParam)
:CvoBaseMemOpr(0)
,m_CharParser(NULL)
{
	if (NULL != pParam)
	{
		memcpy(&m_InitInfo,pParam,sizeof(VO_CAPTION_PARSER_INIT_INFO));
#if 0
		if (pParam->stDataInfo.nType == 0 && pParam->stDataInfo.pHeadData)
		{
			m_InitInfo.stDataInfo.nSize =  pParam->stDataInfo.nSize;
			m_InitInfo.stDataInfo.pHeadData = new VO_BYTE[pParam->stDataInfo.nSize];
			memcpy(m_InitInfo.stDataInfo.pHeadData,pParam->stDataInfo.pHeadData,pParam->stDataInfo.nSize);
		}
#else
		m_InitInfo.stDataInfo.nSize = 0;
		m_InitInfo.stDataInfo.pHeadData = NULL;
#endif
	}
	m_pCCData = new VO_BYTE[1024*4];
	m_nCntData = 0;
	memset(&m_Langinfo,0,sizeof(VO_CAPTION_LANGUAGE_INFO));
}

CCCPacketParser::~CCCPacketParser()
{
//	SAFE_DELETE(m_pCCData);
	if (VO_NULL != m_pCCData)
	{
		delete []m_pCCData;
		m_pCCData = VO_NULL;
	}
	SAFE_DELETE(m_CharParser);
	SAFE_DELETE(m_InitInfo.stDataInfo.pHeadData);
	Reset();
}


VO_U32 CCCPacketParser::Process(VO_PBYTE pInputData, VO_U32 cbInputData,UserDataType eType)
{
	return VO_ERR_PARSER_OK;
}

VO_U32 CCCPacketParser::GetData(VO_CAPTION_PARSER_OUTPUT_BUFFER* pOutputData)
{
	pOutputData->pSubtitleInfo = NULL;
	return VO_ERR_PARSER_OK;
}

VO_VOID CCCPacketParser::Reset()
{
	m_nCntData = 0;

	VO_U32 nCnt = 0;
	while(nCnt < m_Langinfo.nLangCnt)
	{
		SAFE_DELETE(m_Langinfo.pLanguage[nCnt]);
		nCnt++;
	}
	SAFE_DELETE(m_Langinfo.pLanguage);
	m_Langinfo.nLangCnt = 0;
}
///<Caption data in PES payload
CEIAHeaderPacketParser::CEIAHeaderPacketParser(VO_CAPTION_PARSER_INIT_INFO* pParam)
:CCCPacketParser(pParam)
{
	//m_CharParser =  new CERIBCharParser(&m_InitInfo);
}

CEIAHeaderPacketParser::~CEIAHeaderPacketParser()
{

}
VO_U32 CEIAHeaderPacketParser::Process(VO_PBYTE pInputData, VO_U32 cbInputData,VO_PBYTE& pOutputBuf,VO_U32& nOutputSiz)
{
	pOutputBuf = NULL;
	nOutputSiz = 0;

	if (pInputData == NULL || cbInputData == 0)
	{
		VOLOGE("ClosedCaption Error:invalid input data");
		return VO_ERR_PARSER_FAIL;
	}
//	CDumper::DumpRawData((VO_PBYTE)&cbInputData,4);///<dump 4 byte's size
//	CDumper::DumpRawData(pInputData,cbInputData);///<dump the data

	VO_U32 ret = ParserPESPacket(pInputData,cbInputData);
	if (ret != VO_ERR_PARSER_OK)
	{
		VOLOGE("ClosedCaption Error:ParserPESPacket Error");
		return VO_ERR_PARSER_FAIL;
	}
	///<parser the characters
	if (m_CharParser && m_nCntData > 0)
	{
		ret = m_CharParser->Process(m_pCCData,m_nCntData);
	}
	return ret;
}

VO_U32 CEIAHeaderPacketParser::ParserPESPacket(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (pInputData == NULL || pInputData[0] != 0x80 || pInputData[1] != 0xFF ||pInputData[2] != 0xF0)
	{
		return VO_ERR_PARSER_FAIL;
	}
	VO_PBYTE pCCData = VO_NULL;
	if (VO_NULL != (pCCData = Load(pInputData+3,cbInputData-3)))
	{
		m_nCntData = data_unit.data_unit_size;
		MemCopy(m_pCCData,pCCData,m_nCntData);
		return VO_ERR_PARSER_OK;
	}
	return VO_ERR_PARSER_FAIL;
	
}
VO_VOID BCD_OTM::ReadTime(VO_PBYTE& pInputData, VO_U32 cbInputData)
{
	VO_PBYTE& pTmp = pInputData;
	Hour_FirstDig = R4B7(*pTmp);
	Hour_FirstDig = R4B3(*pTmp++);

	Minute_FirstDig = R4B7(*pTmp);
	Minute_SecndD = R4B3(*pTmp++);

	Second_FirstDig = R4B7(*pTmp);
	Second_SecndDig = R4B3(*pTmp++);
	
	MilliSecond_FirstDig = R4B7(*pTmp);
	MilliSecond_SecndDig = R4B3(*pTmp++);
	MilliSecond_ThirdDig = R4B7(*pTmp);
}
VO_PBYTE Arid_CaptionStatementData::Load(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	///<reset the data unit
//	data_unit.Reset();

	VO_PBYTE pTmp = pInputData;
	TMD = R2B7(*pTmp++);
	if (TMD != 0x00)
	{
		VOLOGE("ClosedCaption Error:Not supported Time control mode in CaptionStatement");
	}
	if (TMD == 0x01 || TMD == 0x02)
	{
		BCD_OTM otm;
		otm.ReadTime(pTmp,5);
	//	pTmp+=5;
	}
	R24B(pTmp,data_unit_loop_length);
	return pTmp;//data_unit.Load(pTmp,data_unit_loop_length);
}
VO_VOID Arid_DataUnit::Reset()
{
	unit_separator = 0;
	data_unit_parameter = 0;
	data_unit_size = 0;
}	
VO_PBYTE Arid_DataUnit::Load(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	VO_PBYTE pTmp = pInputData;
	unit_separator = *pTmp++;
	if (unit_separator != 0x1F)
	{
		VOLOGE("ClosedCaption Error:error unit_separator");
	}
	data_unit_parameter = *pTmp++;
	if (data_unit_parameter != 0x20)
	{
		VOLOGE("ClosedCaption Error:not supported Data Unit parameter:%d",data_unit_parameter);
	}
	R24B(pTmp,data_unit_size);
	return pTmp;
}

VO_PBYTE Arid_CaptionManagementData::Load(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	VO_PBYTE pTmp = pInputData;
	///<reset the data unit
//	data_unit.Reset();

	TMD = R2B7(*pTmp++);
	if (TMD != 0x00)
	{
		VOLOGE("ClosedCaption Error:Not supported Time control mode in CaptionManagement");
	}
	if (TMD == 0x02)
	{
		BCD_OTM otm;
		otm.ReadTime(pTmp,5);
	//	pTmp+=5;
	}
	number_language = *pTmp++;

	for (int i = 0;i<number_language;i++)
	{
		language_info[i].language_tag = R3B7(*pTmp);
		language_info[i].DMF = R4B3(*pTmp++);
		if (language_info[i].DMF == 0x0C || language_info[i].DMF == 0x0D || language_info[i].DMF == 0x0E)
		{
			language_info[i].DC = *pTmp++;
		}
		R24B(pTmp,language_info[i].iso_language_code);
		language_info[i].format = R4B7(*pTmp);
		language_info[i].TCS = R2B3(*pTmp);
		language_info[i].rollup_mode = R2B1(*pTmp++);
		if (language_info[i].rollup_mode != 0x00)
		{
			VOLOGE("ClosedCaption Error:not supported rollup_mode");
		}
	}
	R24B(pTmp,data_unit_loop_length);
	return pTmp;//data_unit.Load(pTmp,data_unit_loop_length);
}

VO_PBYTE Arid_CaptionDataGroup::Load(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	
	VO_PBYTE pTmp = pInputData;
	///<reset the cc buffer
	data_unit.Reset();

	data_group_id = R6B7(*pTmp);
	data_group_version = R2B1(*pTmp++);

	data_group_link_number = *pTmp++;
	last_data_group_link_number = *pTmp++;
	R16B(pTmp,data_group_size);
	if ((VO_U32)(pTmp - pInputData) >= cbInputData || data_group_size == 0)
	{
		return VO_NULL;
	}
	VO_PBYTE pDataUnit = VO_NULL;
	VO_U32 nDataUnitSize = 0;

	if ((data_group_id & 0x0F) == 0x00)
	{
		pDataUnit = management_data.Load(pTmp,data_group_size);
		nDataUnitSize = management_data.data_unit_loop_length;
	}
	else if((data_group_id & 0x0F) < 0x09)
	{
		pDataUnit = statement_data.Load(pTmp,data_group_size);
		nDataUnitSize = statement_data.data_unit_loop_length;
	}
	else
	{
		pDataUnit = VO_NULL;
	}
	if (pDataUnit == VO_NULL || (VO_U32)(pDataUnit - pInputData) >= cbInputData || nDataUnitSize == 0)
	{
		return VO_NULL;
	}

	return data_unit.Load(pDataUnit,nDataUnitSize);
#if 0
	VO_PBYTE pCRC = pTmp+data_group_size;
	R16B(pCRC,CRC_16);
#endif
}