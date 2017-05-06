#include "CCComandParser.h"
#include "CvoBaseMemOpr.h"


CCComandParser::CCComandParser()
:CvoBaseMemOpr(0)
,m_nLineNumber(1)
,m_eCharType(command_none)
{
	for (VO_U8 count = 0;count < MAX_LINE_COUNT;count++)
	{
		MemSet(m_LineInfo[count].LineBuffer,0x00,MAX_CHARACTER_COUNT_IN_ONE_LINE);
		m_LineInfo[count].CharCnt = 0;
	}

}

CCComandParser::~CCComandParser()
{

}

CC608ComandParser::CC608ComandParser()
{
	
}
CC608ComandParser::~CC608ComandParser()
{

}

VO_BOOL CC608ComandParser::ParserSpecialChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	return VO_TRUE;
}

VO_BOOL CC608ComandParser::ParserControlChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	switch (pInputData[1])
	{
	case 0x25:
		{
			if (m_eCharType == command_rollUp)
			{
				return VO_FALSE;
			}
			m_eCharType = command_rollUp;
			if (m_nLineNumber > 3)
			{
				return VO_FALSE;
			}
			VO_U8 TmpLineNum = m_nLineNumber;
			while (TmpLineNum-- > 0)
			{
			//	MemSet(m_LineInfo[m_nLineNumber+1].LineBuffer,0x0,MAX_CHARACTER_COUNT_IN_ONE_LINE);
				m_LineInfo[m_nLineNumber].CharCnt = m_LineInfo[m_nLineNumber-1].CharCnt;
				MemCopy(m_LineInfo[m_nLineNumber].LineBuffer,m_LineInfo[m_nLineNumber-1].LineBuffer,m_LineInfo[m_nLineNumber-1].CharCnt);
			}
			m_nLineNumber++;
			m_LineInfo[0].CharCnt = 0;
		}
		break;
	case 0x2D:
		if (m_eCharType == command_Carriage_return)
		{
			return VO_FALSE;
		}
		m_eCharType = command_Carriage_return;
		break;
	}
	return VO_TRUE;
}

VO_BOOL CC608ComandParser::ParserMIDROWChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	return VO_TRUE;
}

VO_BOOL CC608ComandParser::ParserPreambleChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	switch (pInputData[1])
	{
	case 0x70:
		if (m_eCharType == command_indent)
		{
			return VO_FALSE;
		}
		m_eCharType = command_indent;
		break;
	}
	return VO_TRUE;
}

VO_BOOL CC608ComandParser::ParserStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (m_LineInfo[0].CharCnt + cbInputData >= MAX_CHARACTER_COUNT_IN_ONE_LINE)
	{
		return VO_FALSE;
	}
	m_LineInfo[0].LineBuffer[m_LineInfo[0].CharCnt++] = pInputData[0];
	m_LineInfo[0].LineBuffer[m_LineInfo[0].CharCnt++] = pInputData[1];
	return VO_TRUE;
}
VO_BOOL CC608ComandParser::ParserNonStandardChar(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (NULL == pInputData)
	{
		return VO_FALSE;
	}
	VO_BOOL ret = VO_FALSE;
	switch (pInputData[0])
	{
	case PREAMBLE_ADDRESS_ROW_11_SYMBOL:
		{
			if ((pInputData[1] >= SPECIAL_CHARACTER_MID_ROW_CONTROL_START ) && (pInputData[1] <= SPECIAL_CHARACTER_MID_ROW_CONTROL_END))
			{
				ret = ParserMIDROWChar(pInputData,cbInputData);
			}
			else if ((pInputData[1] >= SPECIAL_CHARACTER_SPECIAL_CODE_START ) && (pInputData[1] <= SPECIAL_CHARACTER_SPECIAL_CODE_END))
			{
				ret = ParserSpecialChar(pInputData,cbInputData);
			}
			else
				ret =ParserPreambleChar(pInputData,cbInputData);
		}
		break;
	case PREAMBLE_ADDRESS_ROW_14_15_AND_CONTROL_1_SYMBOL:
		if ((pInputData[1] >= SPECIAL_CHARACTER_MID_ROW_CONTROL_START ) && (pInputData[1] <= SPECIAL_CHARACTER_MID_ROW_CONTROL_END))
		{
			ret = ParserControlChar(pInputData,cbInputData);
		}
		else
			ret = ParserPreambleChar(pInputData,cbInputData);
		break;
	case PREAMBLE_ADDRESS_ROW_9_10_AND_CONTROL_2_SYMBOL:
		if ((pInputData[1] >= SPECIAL_CHARACTER_MID_ROW_CONTROL_START+1 ) && (pInputData[1] <= SPECIAL_CHARACTER_MID_ROW_CONTROL_START+3))
		{
			ret = ParserControlChar(pInputData,cbInputData);
		}
		else
			ret = ParserPreambleChar(pInputData,cbInputData);
		break;
	default:
		ret = ParserPreambleChar(pInputData,cbInputData);
		break;
	}
	return ret;
}
VO_BOOL CC608ComandParser::ParserCommandCode(VO_PBYTE pInputData, VO_U32 cbInputData)
{
	if (cbInputData != 2 || pInputData == NULL)
	{
		return VO_FALSE;
	}
	///<remove the paity bit
	pInputData[0] &= 0x7f;
	pInputData[1] &= 0x7f;
	if ((pInputData[0] >= PREAMBLE_ADDRESS_ROW_11_SYMBOL ) && (pInputData[0] <= PREAMBLE_ADDRESS_ROW_9_10_AND_CONTROL_2_SYMBOL) )
	{
		return ParserNonStandardChar(pInputData,cbInputData);
	}
	else
	{
		if (pInputData[0] >= STANDARD_CHARACTER_START && pInputData[0] <= STANDARD_CHARACTER_END)
		{
			return ParserStandardChar(pInputData,cbInputData);
		}
		else
		{
			///<non-handle code
			return VO_FALSE;
		}
	}
	return VO_TRUE;
}
